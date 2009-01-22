/*
	remoteaccess.h - remote allocation and stackdb node classes

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
	class ExtRemoteStacktraceNode
	{
	public: // lifetime
		ExtRemoteStacktraceNode(ExtRemoteTyped node)
			: node_(node)
		{

		}
		~ExtRemoteStacktraceNode() {}
	public: // interface
		USHORT frames()
		{
			return node_.Field("frames_").GetShort();
		}
		ULONG allocation_count()
		{
			return node_.Field("allocationCount_").GetUlong();
		}
		LONG ftag()
		{
			return node_.Field("ftag_").GetLong();
		}
		LONG ctag()
		{
			return node_.Field("ctag_").GetLong();
		}
		LONG htag()
		{
			return node_.Field("htag_").GetLong();
		}
		DWORD *stack()
		{
			ExtRemoteTyped stack=node_.Field("stack_");
			ULONG no_frames=frames();

			memset(stack_, 0, 32*sizeof(stack_[0]));
			for(ULONG i=0;i < no_frames;i++)
			{
				stack_[i]=stack.ArrayElement(i).GetUlong();
			}

			return stack_;
		}
	private: // attrs
		ExtRemoteTyped node_;
		DWORD stack_[32];
	};

	class ExtRemoteAllocationNode
	{
	public: // lifetime
		ExtRemoteAllocationNode(ExtRemoteTyped node)
			: node_(node)
		{

		}
		~ExtRemoteAllocationNode() {}
	public: // interface
		ULONG trace()
		{
			return node_.Field("hash_").GetUlong();
		}
		FILETIME timestamp()
		{
			FILETIME ts;
			ts.dwLowDateTime=node_.Field("ts_").Field("dwLowDateTime").GetUlong();
			ts.dwHighDateTime=node_.Field("ts_").Field("dwHighDateTime").GetUlong();
			return ts;
		}
	private: // attrs
		ExtRemoteTyped node_;
	};
}
