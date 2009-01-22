/*
	gdi.h - GDI-specific debugger extension helpers.

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

#pragma once

namespace dbgext
{
//	
// GDI-related data structures and functions
// Information obtained from various sources 
//	 * Alex Ionescu (http://www.alex-ionescu.com)
//	 * Christophe Nasarre (http://msdn.microsoft.com/en-us/magazine/cc188782.aspx)
//
	typedef enum _GDI_OBJ_TYPE
	{
		DC=0x1,
		RGN=0x4,
		IDB=0x5,
		PAL=0x8,
		LFONT=0xa,
		BRUSH=0x10,
		META=0x21,
		PEN=0x30,
		EPEN=0x50
	} GDI_OBJ_TYPE, *PGDI_OBJ_TYPE;
	ULONG ToUserGdiType(GDI_OBJ_TYPE got)
	{
		switch(got)
		{
		case DC: return OBJ_DC;
		case RGN: return OBJ_REGION;
		case IDB: return OBJ_BITMAP;
		case PAL: return OBJ_PAL;
		case LFONT: return OBJ_FONT;
		case BRUSH: return OBJ_BRUSH;
		case META: return OBJ_ENHMETAFILE;
		case PEN: return OBJ_PEN;
		case EPEN: return OBJ_EXTPEN;
		}
		return static_cast<ULONG>(-1);
	}
	ULONG const MAX_GDI_HANDLES=0x4000;
	typedef struct _GDI_HANDLE
	{
		HGDIOBJ Handle;
		DWORD Type;	// user-mode mapped GDI object type
	} GDI_HANDLE, *PGDI_HANDLE;

	class ExtRemoteGdiTable;
	class ExtRemoteGdiHandle
	{
		friend class ExtRemoteGdiTable;
	public: // lifetime
		~ExtRemoteGdiHandle() {}
	public: // interface
		operator HGDIOBJ();
		operator bool();
		DWORD Type();
		WORD ProcessId();
	private: // implementation
		ExtRemoteGdiHandle(ExtRemoteGdiTable *table, USHORT index, ULONG pid);
		bool IsDeleted();
		HGDIOBJ BuildHandle();
	private: // attrs
		ExtRemoteGdiTable *table_;
		USHORT index_;
		ULONG pid_;
	};
	/*!
		\brief	Debugger-friendly remote data access proxy (remote GDI handle table)
	*/
	class ExtRemoteGdiTable
	{
	public: // lifetime
		explicit ExtRemoteGdiTable(ExtRemoteTyped peb, ULONG pid)
			: peb_(peb)
			, pid_(pid)
		{

		}
		~ExtRemoteGdiTable() {}
	public: // interface
		PVOID KernelInfo(USHORT index) {return FieldPV(Offset(index,fKernelInfo));}
		WORD ProcessId(USHORT index) {return FieldW(Offset(index,fProcessId));}
		WORD Count(USHORT index) {return FieldW(Offset(index,fCount));}
		WORD MaxCount(USHORT index) {return FieldW(Offset(index,fMaxCount));}
		WORD Type(USHORT index) {return FieldW(Offset(index,fType));}
		PVOID UserInfo(USHORT index) {return FieldPV(Offset(index,fUserInfo));}
		ExtRemoteGdiHandle operator[](USHORT index)
		{
			return ExtRemoteGdiHandle(this, index, pid_);
		}
	private: // attrs
		typedef struct _GDI_TABLE_ENTRY
		{
			PVOID	KernelInfo;
			WORD	ProcessId;
			WORD	Count;
			WORD	MaxCount;
			WORD	Type;
			PVOID	UserInfo;
		} GDI_TABLE_ENTRY, *PGDI_TABLE_ENTRY;
		enum GdiTableEntryField {fKernelInfo,fProcessId,fCount,fMaxCount,fType,fUserInfo};
		WORD Offset(WORD index, GdiTableEntryField field) const
		{
			BYTE offset=0;
			switch(field)
			{
			case fKernelInfo: break;
			case fProcessId: offset=sizeof(PVOID); break;
			case fCount: offset=sizeof(PVOID)+sizeof(WORD); break;
			case fMaxCount: offset=sizeof(PVOID)+2*sizeof(WORD); break;
			case fType: offset=sizeof(PVOID)+3*sizeof(WORD); break;
			case fUserInfo: offset=sizeof(PVOID)+4*sizeof(WORD); break;
			default: /*_assert(!"Unknown handle entry field");*/ break;
			}
			return index*sizeof(GDI_TABLE_ENTRY)+offset;
		}
		PVOID FieldPV(WORD FieldOffset)
		{
			return reinterpret_cast<PVOID>(ExtRemoteData(peb_.Field("GdiSharedHandleTable").GetPtr()+FieldOffset, sizeof(PVOID)).GetPtr());
		}
		WORD FieldW(WORD FieldOffset)
		{
			return static_cast<WORD>(ExtRemoteData(peb_.Field("GdiSharedHandleTable").GetPtr()+FieldOffset, sizeof(WORD)).GetUshort());
		}
		ExtRemoteTyped peb_;
		ULONG pid_;
	};
	ExtRemoteGdiHandle::ExtRemoteGdiHandle(ExtRemoteGdiTable *table, USHORT index, ULONG pid)
		: table_(table)
		, index_(index)
		, pid_(pid)
	{

	}
	ExtRemoteGdiHandle::operator HGDIOBJ() {return BuildHandle();}
	ExtRemoteGdiHandle::operator bool() {return !IsDeleted() && (static_cast<ULONG>(table_->ProcessId(index_))==pid_);}
	DWORD ExtRemoteGdiHandle::Type()
	{
		return LOBYTE(table_->MaxCount(index_))&0x7f;
	}
	WORD ExtRemoteGdiHandle::ProcessId()
	{
		return table_->ProcessId(index_);
	}
	bool ExtRemoteGdiHandle::IsDeleted()
	{
		return (reinterpret_cast<ULONG>(table_->KernelInfo(index_)) < 0x80000000);
	}
	HGDIOBJ ExtRemoteGdiHandle::BuildHandle()
	{
		return reinterpret_cast<HGDIOBJ>(MAKELONG(index_, table_->MaxCount(index_)));
	}

	typedef std::map<HGDIOBJ, GDI_HANDLE> gdi_table;

	/*!
		\brief	Compiles a GDI handle table for a specific pid 
				provided the shared GDI table in peb
	*/
	gdi_table build_table(ExtRemoteTyped peb, ULONG pid, bool verbose)
	{
		/*
		//@ds: i wonder why this won't work:
		ULONG64 PebOffset=0l;
		ULONG Pid=0l;
		HRESULT Status=S_OK;
		if((Status=m_System4->GetCurrentProcessDataOffset(&PebOffset))!=S_OK)
		{
			g_Ext->ThrowRemote(Status,"Failed to obtain PEB offset");
		}
		//if((Status=m_System4->GetCurrentProcessSystemId(&Pid))!=S_OK)
		if((Status=m_System4->GetProcessIdByPeb(PebOffset,&Pid))!=S_OK)
		{
			g_Ext->ThrowRemote(Status,"Failed to obtain process's PID");
		}
		ExtRemoteGdiTable remote_table(ExtNtOsInformation::GetOsPeb(PebOffset), Pid);
		*/

		ExtRemoteGdiTable remote_table(peb, pid);
		gdi_table table;
		for(USHORT i=0;i<MAX_GDI_HANDLES;i++)
		{
			ExtRemoteGdiHandle handle=remote_table[i];
			if(handle)
			{
				HGDIOBJ value=(HGDIOBJ)handle;
				if(!value) continue;
				if(verbose) ExtOut("Handle 0x%lx [type=%lx for pid=%lx]\n",value,handle.Type(),handle.ProcessId());
				GDI_HANDLE h={value,handle.Type()};
				table.insert(std::make_pair(h.Handle, h));
			}
			else
			{
				//if(verbose) Out("GdiTable[%ld] is marked 'deleted'\n",i);
			}
		}
		if(verbose) ExtOut("Ready.\n");
		return table;
	}
}
