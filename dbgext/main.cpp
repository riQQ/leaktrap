/*
	main.cpp - windows debugger extension to explore handle allocations

	user mode GDI and USER handle leak tracker project
	Copyright (c) 2008 Dmitri Shelenin (deemok@gmail.com) 

	This program is free software; you can redistribute it and/or modify     
	it under the terms of the GNU General Public License as published by     
	the Free Software Foundation; either version 2 of the License, or        
	(at your option) any later version.                                      

	This program is distributed in the hope that it will be useful,          
	but WITHOUT ANY WARRANTY; without even the implied warranty of           
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            
	GNU General Public License for more details.                             

	You should have received a copy of the GNU General Public License        
	along with this program; if not, write to the Free Software              
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#define EXT_CLASS VPWinDbgExts
// forward declaration
class EXT_CLASS;

#include <engextcpp.hpp>
#include "../Intercepts.h"				// function_tag
#include <set>
#include <shlwapi.h>					// StrStrIA()
#include <vector>						// std::vector<>
#include <sstream>						// std::stringstream<>
#include <map>							// std::map<>
#include <algorithm>					// std::sort<>
#include <fstream>						// std::ifstream<>/std::ofstream<>
#include "RemoteHashMap.h"				// dbgext::ExtRemoteHashMap<>
#include "RemoteHashMapStackdb.h"		// Stack DB access based on remote std::hash_map<> implementation
#include "RemoteAccess.h"				// Remote target's type mappings
#include "gdi.h"						// GDI support classes / code

#if defined(IN_RANGE)
#undef IN_RANGE
#endif // IN_RANGE
#define IN_RANGE(a,b,c)\
	((a)>=(b) && (a)<=(c))

using namespace dbgext;
namespace
{
	// typedefs
	typedef std::vector<std::pair<HANDLE,ExtRemoteAllocationNode> > allocation_list;
	typedef std::set<HANDLE> snapshot_type;
	typedef std::vector<std::string> exclusion_rules;

	// \pre	rule is not empty
	bool is_top_level_exclusion(std::string const &rule)
	{
		return ('^'==rule[0]);
	}

	bool is_empty(snapshot_type const &snapshot)
	{
		return (0 == snapshot.size());
	}

	allocation_list elements(ExtRemoteAllocations &allocations, snapshot_type const &snapshot=snapshot_type())
	{
		allocation_list list;
		for(ExtRemoteAllocations::iterator it=allocations.begin();it != allocations.end();++it)
		{
			HANDLE handle=reinterpret_cast<HANDLE>(it.key());
			if(is_empty(snapshot) || snapshot.find(handle)==snapshot.end())
				list.push_back(std::make_pair(handle,*it));
		}
		return list;
	}
}
namespace std
{
	/*!
		\brief various std:: helpers
	*/

	typedef union
	{
		unsigned long val;
		char dummy[4];
	} reshape;
	ostream &binout(ostream &os, unsigned long val)
	{
		reshape buf;
		buf.val=val;
		os.write(buf.dummy, 4);
		return os;
	}
	istream &binin(istream &is, unsigned long &val)
	{
		reshape buf;
		is.read(buf.dummy, 4);
		val=buf.val;
		return is;
	}
	ostream &operator<<(ostream &os, snapshot_type const &snapshot)
	{
		binout(os, static_cast<unsigned long>(snapshot.size()));	// write the vector's size
		for(snapshot_type::const_iterator it=snapshot.begin();it!=snapshot.end();++it)
		{
			binout(os, reinterpret_cast<unsigned long>(*it));
		}
		return os;
	}
	istream &operator>>(istream &is, snapshot_type &snapshot)
	{
		unsigned long size;
		binin(is, size);
		for(snapshot_type::size_type i=0;i<size;++i)
		{
			unsigned long el;
			binin(is, el);
			snapshot.insert(reinterpret_cast<HANDLE>(el));
		}
		return is;
	}
	string &trimmed(string &str)
	{
		string::size_type p=0;
		while(string::npos != (p=str.find_first_of(' ')))
		{
			str.erase(p, 1);
		}
		return str;
	}
}

//----------------------------------------------------------------------------
//
// Base extension class.
// Extensions derive from the provided ExtExtension class.
//
//----------------------------------------------------------------------------
class EXT_CLASS : public ExtExtension
{
public: // lifetime
	EXT_CLASS() {}
public:
	EXT_COMMAND_METHOD(handle);
	EXT_COMMAND_METHOD(sd);
public: // implementation
	void dump_stack(ExtRemoteStacktraceNode node, ULONG64 trace);	// by value on purpose
	bool filter(ExtRemoteStacktraceNode node);
private: // attrs
	snapshot_type snapshot_;
	//exclusion_rules exclusion_rules_;
	static CHAR name_buffer[256];	// shared symbol lookup buffer
	// exclusion rule descriptor
	typedef struct 
	{
		//DEBUG_MODULE_AND_ID ids[1];	// symbol information (module base + symbol id)
		ULONG64 ModuleBase;
		ULONG64 OffsetStart;
		std::string	Symbol;			// symbol name
	} exclusion_rule;
	// exclusion rules
	typedef std::map<ULONG64,std::vector<exclusion_rule>> exclusion_map;
	exclusion_map top_rules_;	// top level rules
	exclusion_map rules_;		// all other rules
private: // implementation
	exclusion_rule make_rule(std::string const &symbol, ULONG64 offset);
	void add_rule(exclusion_map &rules, exclusion_rule const &rule);
	bool match_addr_in_exclusions(ULONG64 addr, exclusion_map const &rules);
	gdi_table build_table();
};
CHAR EXT_CLASS::name_buffer[256]={'\0'};

// additional helpers
namespace
{
	/*!
		\brief	Query to determine if a specified node should be displayed (or otherwise accounted for)
		Note, only nodes that have not been either exluded through exclusion rules or, being GDI, have been
		actually matched in the shared gdi table (found in PEB) are 'displayable'
	*/
	bool node_to_display(EXT_CLASS &ext, gdi_table &table, ExtRemoteStacktraceNode node, HANDLE handle)
	{
		bool ignore=ext.HasArg("ignore");
		return (ignore || 
				(!detail::IsGdiHandle(static_cast<detail::handle_tag>(node.htag())) && ext.filter(node)) || 
				(table.find(handle)!=table.end() && ext.filter(node))
				);
	}
}

// EXT_DECLARE_GLOBALS must be used to instantiate
// the framework's assumed globals.
EXT_DECLARE_GLOBALS();

void EXT_CLASS::dump_stack(ExtRemoteStacktraceNode node, ULONG64 trace)
{
	LONG ftag=node.ftag(), ctag=node.ctag();
	Out("Function tag: 0x%lx (%s)\n", ftag, detail::From(static_cast<detail::function_tag>(ftag)));
	Out("Cleanup tag: 0x%lx (%s)\n", ctag, detail::From(static_cast<detail::function_tag>(ctag)));
	Out("Allocated: %ld time(s)\n", node.allocation_count());
	Out("Trace: 0x%lx\n", trace);

	USHORT frames=node.frames();
	DWORD *stack=node.stack();
	for(USHORT i=0;i < frames;i++)
	{
		ULONG64 displacement=0;
		ULONG eip=stack[i];
		// retrieve symbol at the specified address
		GetSymbol(eip,name_buffer,&displacement);
		Out("0x%lx\t%s+0x%lx\n", eip, name_buffer, displacement);
	}
	// <br>
	Out("\n");
}

//----------------------------------------------------------------------------
//
// sd extension command.
//
// Used to lookup and dump c++ objects with vtable on a address range 
//
//----------------------------------------------------------------------------
EXT_COMMAND(sd,
			"Lookup c++ vtable-based object(s) acc. to search pattern",
			"{pat;s;search_pattern;Define search pattern}"
			"{range1;e;low_range;Define search interval low value}"
			"{range2;e;high_range;Define search interval high value}"
			)
{
	ULONG64 addr=0L;
	SearchMemory(
		0,	// Address
		0,	// Length
		0,	// PatternLength
		0,	// Pattern (PVOID)
		&addr	// Found address
		);
}
//----------------------------------------------------------------------------
//
// dumpstack extension command.
//
// Used to lookup and dump specific allocations/allocation traces
//
//----------------------------------------------------------------------------
EXT_COMMAND(handle,
			"Dump specific allocation/trace",
			"{t;e;trace;Dump stack for specific trace}"
			"{h;e;handle;Dump stack for specific allocation}"
			"{list;b;;List all active handles}"
			"{ht;e;handle_type;(Used with -list) Limit stack traces to specific handle type}"
			"{v;b;;Verbose output (for use individually with each command)}"
			"{stats;b;;Display allocation statistics}"
			"{snapshot;b;;Take a snapshot of currently active allocations}"
			"{diff;b;;(Used with -list and -stats) Compare currently active allocations with last taken snapshot}"
			"{read;x;file;(Used with -snapshot and -filter) Read data from a file}"
			"{write;x;file;(Used with -snapshot) Store data to a file}"
			"{filter;b;;Manage exclusion configuration}"
			"{ignore;b;;Ignore suppressions (display everything)}"
			)
{
	bool verbose=HasArg("v");
	//bool ignore=HasArg("ignore");	// ignore suppressions
	bool diff=HasArg("diff") && !is_empty(snapshot_);

	ExtRemoteStackdb stackdb;
	
	if(HasArg("filter") && HasArg("read"))
	{	// read exlcusion config
		rules_.clear();
		std::ifstream f(GetArgStr("read"));
		if(!f.is_open())
		{
			std::stringstream buf;
			buf << "Failed to open '" << GetArgStr("read") << "'";
			g_Ext->ThrowRemote(E_FAIL,buf.str().c_str());
		}
		std::string line;
		ULONG exclusions=0L;
		while(!f.eof())
		{
			std::getline(f,line);
			line=std::trimmed(line);
			if(!line.empty())
			{
				std::string symbol=line.substr(1);
				ULONG64 offset;
				if(SUCCEEDED(m_Symbols3->GetOffsetByName(symbol.c_str(), &offset)))
				{
					exclusion_rule rule=make_rule(symbol, offset);
					if(is_top_level_exclusion(line))
						add_rule(top_rules_, rule);
					else
						add_rule(rules_, rule);
					++exclusions;
				}
				else
				{
					Out("Failed to obtain offset for '%s', skipping symbol.\n", symbol.c_str());
				}
			}
			g_Ext->ThrowInterrupt();
		}
		Out("Read %ld exclusion rules.\n", exclusions);
	}
	else
	if(HasArg("snapshot"))
	{
		if(HasArg("write"))	// write a snapshot to a file
		{
			std::ofstream f(GetArgStr("write"), std::ios::out|std::ios::binary|std::ios::trunc);
			f << snapshot_;
			if(verbose) Out("%ld handles written.\n", snapshot_.size());
		}
		else
		if(HasArg("read"))	// read a snapshot
		{
			snapshot_.clear();
			std::ifstream f(GetArgStr("read"), std::ios::in|std::ios::binary);
			f >> snapshot_;
			if(verbose) Out("%ld handles read.\n", snapshot_.size());
		}
		else
		{
			snapshot_.clear();
			ExtRemoteAllocations allocations;
			for(ExtRemoteAllocations::iterator it=allocations.begin();it != allocations.end();++it)
			{
				snapshot_.insert(reinterpret_cast<HANDLE>(it.key()));
			}
			Out("Taken.\n");
		}
	}
	else
	if(HasArg("t"))
	{
		ULONG64 trace=GetArgU64("t",false);
		dump_stack(ExtRemoteStacktraceNode(*stackdb.find(static_cast<ULONG>(trace))), trace);
	}
	else
	if(HasArg("h"))
	{
		ExtRemoteAllocations allocations;
		HANDLE handle=reinterpret_cast<HANDLE>(GetArgU64("h",false));
		ExtRemoteAllocations::iterator node=allocations.find(handle);
		if(allocations.end()!=node)
		{
			ULONG trace=ExtRemoteAllocationNode(*node).trace();
			if(0 != trace)
			{
				dump_stack(ExtRemoteStacktraceNode(*stackdb.find(trace)), trace);
			}
			else
				Out("** Warning: empty trace hash (=0) for 0x%lx handle, no stack recorded!\n",handle);
		}
		else
		{
			Out("** Warning: no stack trace matched for 0x%lx handle!\n",handle);
		}
	}
	else
	if(HasArg("list"))
	{
		ExtRemoteAllocations allocations;
		int alloc_count=allocations.count();
		gdi_table table=build_table();
		Out("Total allocations: %ld, (Total GDI handles: %ld (acc. to PEB))\n", alloc_count, table.size());
		if(diff) Out("*** Note: displaying positive diff compared to previous snapshot\n");

		allocation_list list=elements(allocations, diff?snapshot_:snapshot_type());
		for(allocation_list::iterator it=list.begin();it != list.end();++it)
		{
			ExtRemoteStacktraceNode node(*stackdb.find(it->second.trace()));
			//if((ignore || filter(node)) && (!HasArg("ht") || static_cast<LONG>(GetArgU64("ht"))==node.htag()))
			if(node_to_display(*this,table,node,it->first) && (!HasArg("ht") || static_cast<LONG>(GetArgU64("ht"))==node.htag()))
			{
				std::stringstream command;
				command << ".printf /D \"Handle: <link cmd=\\\"!leakdbg.handle -h " << std::hex << it->first << "\\\">" << std::hex << it->first << "</link>\\n\"";
				m_Control->Execute(DEBUG_OUTCTL_THIS_CLIENT |
									DEBUG_OUTCTL_OVERRIDE_MASK |
									DEBUG_OUTCTL_NOT_LOGGED,
									command.str().c_str(),
									DEBUG_EXECUTE_NOT_LOGGED |
									DEBUG_EXECUTE_NO_REPEAT);
				if(verbose)
				{
					Out("\tFound in trace: %lx\n", it->second.trace());
					Out("\tAllocated: %ld (times)\n", node.allocation_count());
					Out("\tHandle: %s\n", node.ftag(), detail::From(static_cast<detail::handle_tag>(node.htag())));
					Out("\tFunction tag: %ld (%s)\n", node.ftag(), detail::From(static_cast<detail::function_tag>(node.ftag())));
					Out("\tCleanup tag: %ld (%s)\n", node.ctag(), detail::From(static_cast<detail::function_tag>(node.ctag())));
				}
			}
			// keep responsive
			g_Ext->ThrowInterrupt();
		}
	}
	else
	if(HasArg("stats"))
	{
		ExtRemoteAllocations allocations;
		int alloc_count=allocations.count();
		gdi_table table=build_table();
		Out("Total allocations: %ld, (Total GDI handles: %ld (acc. to PEB))\n", alloc_count, table.size());
		if(diff) Out("*** Note: displaying positive diff compared to previous snapshot\n");

		typedef std::map<detail::handle_tag,std::pair<ULONG,ULONG>> handle_list;
		// @ds: <actual hits,suppressed hits>
		handle_list handles;

		allocation_list list=elements(allocations, diff?snapshot_:snapshot_type());
		for(allocation_list::iterator it=list.begin();it != list.end();++it)
		{
			ExtRemoteStacktraceNode node(*stackdb.find(it->second.trace()));
			if(node_to_display(*this,table,node,it->first))	// if not filtered out and (if GdiTypes.belongsTo() && GdiHandleTable.hasIt())
			{
				std::pair<handle_list::iterator,bool> ret=handles.insert(std::make_pair(static_cast<detail::handle_tag>(node.htag()),std::make_pair(1,0)));
				if(!ret.second)	// already available
				{
					++ret.first->second.first;	// increase hit counter
				}
			}
			else
			{
				std::pair<handle_list::iterator,bool> ret=handles.insert(std::make_pair(static_cast<detail::handle_tag>(node.htag()),std::make_pair(0,1)));
				if(!ret.second)	// already available
				{
					++ret.first->second.second;	// increase hit counter
				}
			}

			// keep responsive
			g_Ext->ThrowInterrupt();
		}

		// dump usage stats
		for(handle_list::const_iterator it=handles.begin();it != handles.end();++it)
		{
			std::stringstream command;
			command << ".printf /D \"<link cmd=\\\"!leakdbg.handle -list " << (diff?"-diff":"") << " -ht " 
					<< std::hex << it->first << "\\\">" << detail::From(it->first) << "</link>: " 
					<< std::dec << it->second.first;
			if(it->second.second)
			{
				command << " (<link cmd=\\\"!leakdbg.handle -list " 
						<< (diff?"-diff":"") << " -ignore -ht " << std::hex << it->first << "\\\">" 
						<< std::dec << it->second.second << "</link>)\\n\"";
			}
			else
			{
				command << "\\n\"";
			}
			m_Control->Execute(DEBUG_OUTCTL_THIS_CLIENT |
								DEBUG_OUTCTL_OVERRIDE_MASK |
								DEBUG_OUTCTL_NOT_LOGGED,
								command.str().c_str(),
								DEBUG_EXECUTE_NOT_LOGGED |
								DEBUG_EXECUTE_NO_REPEAT);
			// keep responsive
			g_Ext->ThrowInterrupt();
		}
	}
}
EXT_CLASS::exclusion_rule EXT_CLASS::make_rule(std::string const &symbol, ULONG64 offset)
{
	bool verbose=HasArg("v");
	HRESULT hres=S_OK;
	exclusion_rule rule;
	rule.Symbol=symbol;
	if(verbose) Out("Reading rule for '%s' (at offset=%I64lx)\n", symbol.c_str(), offset);
	ULONG64 disps[1]={0};
	ULONG entries=0L;
	DEBUG_MODULE_AND_ID ids[1]={0};
	if(SUCCEEDED(hres=m_Symbols3->GetSymbolEntriesByOffset(
		offset, 0L, ids, disps, 1, &entries 
		)))
	{
		rule.ModuleBase=ids[0].ModuleBase;
		rule.OffsetStart=offset-rule.ModuleBase;
		if(verbose)
		{
			//Out("Symbol in module '%s', base=%lx\n", symbol.c_str(), rule.ids[0].ModuleBase);
			//Out("Matched symbol entry with %ld displacements, and %ld entries\n", disps[0], entries);
			Out("DEBUG_MODULE_AND_ID: module base=%I64lx, id=%ld, offset start=%I64lx\n",rule.ModuleBase,ids[0].Id,rule.OffsetStart);
		}
		return rule;
	}
	else
	{
		g_Ext->ThrowRemote(hres,"Failed to obtain symbol information");
	}
}
bool EXT_CLASS::filter(ExtRemoteStacktraceNode node)
{
	// @ds: optimization tip
	// somehow take notice that particular stack trace 
	// has already been evaluated and suppressed
	// something like visited_stack of trace hashes

	DWORD *stack=node.stack();
	USHORT frames=node.frames();
	if(frames>0)
	{
		// check top rules
		if(match_addr_in_exclusions(stack[0], top_rules_))
			return false;
		for(USHORT i=0;i<frames;++i)
		{
			if(match_addr_in_exclusions(stack[i], rules_))
				return false;
			// keep responsive
			g_Ext->ThrowInterrupt();
		}
	}

	return true;
}
void EXT_CLASS::add_rule(EXT_CLASS::exclusion_map &rules, EXT_CLASS::exclusion_rule const &rule)
{
	exclusion_map::iterator it=rules.find(rule.ModuleBase);
	if(rules.end()!=it)
		it->second.push_back(rule);
	else
	{
		exclusion_map::mapped_type arr;
		arr.push_back(rule);
		rules.insert(std::make_pair(rule.ModuleBase,arr));
	}
}
bool EXT_CLASS::match_addr_in_exclusions(ULONG64 addr, EXT_CLASS::exclusion_map const &rules)
{
	//bool verbose=HasArg("v");
	HRESULT hres=S_OK;
	FPO_DATA fpo;
	if(SUCCEEDED(hres=m_Symbols3->GetFunctionEntryByOffset(
		addr,	// Offset
		0,		//DEBUG_GETFNENT_RAW_ENTRY_ONLY,	// Flags
		&fpo,			// Buffer
		sizeof(fpo),	// BufferSize
		0		// BufferNeeded
		)))
	{
		// match a module
		ULONG64 base;
		m_Symbols3->GetModuleByOffset(
			addr,		// Offset
			0,			// StartIndex
			0,			// Index
			&base		// Base
			);
		exclusion_map::const_iterator it=rules.find(base);
		if(it==rules.end())	
		{
			return false;
		}
		// look it up in exclusion rules
		for(exclusion_map::mapped_type::const_iterator it2=it->second.begin();it2!=it->second.end();++it2)
		{
			if(it2->OffsetStart==fpo.ulOffStart)
			{
				return true;
			}
		}
	}
	else
	{
		//Out("Warning: GetFunctionEntryByOffset(%I64lX) failed, result=%lx\n",addr,hres);
	}
	return false;	// both, failure to GetFunctionByOffset and unavailability of corr. exclusion rule
}
gdi_table EXT_CLASS::build_table()
{
	bool verbose=HasArg("v");
	ULONG64 PebOffset=ExtNtOsInformation::GetOsPebPtr(),
		TebOffset=ExtNtOsInformation::GetOsTebPtr();	// Current thread's TEB
	ULONG Pid=ExtNtOsInformation::GetOsTeb(TebOffset).Field("ClientId").Field("UniqueProcess").GetUlong();

	if(verbose) Out("Building GDI handle table (PEB=0x%I64lx, PID=0x%lx)...\n",PebOffset,Pid);
	return dbgext::build_table(ExtNtOsInformation::GetOsPeb(PebOffset), Pid, verbose);
}
