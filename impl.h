/*
	impl.h - 
		tracker's internal data structures
		internal functionality and helper code

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

#include <hash_map>

namespace detail
{
	/*!
		\brief	describes an allocation
		Work-in-progress allocation descriptor
	*/
	struct allocation
	{
		ULONG hash_;		// stack trace hash
		FILETIME ts_;		// timestamp
		allocation()
			: hash_(0)
		{
			ts_.dwLowDateTime=0;
			ts_.dwHighDateTime=0;
		}
		allocation(ULONG hash, FILETIME const &ts)
			: hash_(hash)
			, ts_(ts)
		{

		}
	};
	/*!
		\brief	describes a stack trace
	*/
	struct stack_trace
	{
		function_tag ftag_;	// resource allocation function id
		function_tag ctag_;	// resource cleanup function id
		handle_tag htag_;	// resource handle id
		ULONG allocationCount_;	// number of handles that were created this way
		DWORD stack_[32];	// allocation stack
		SHORT frames_;		// actual number of stack frames (<=32)
		stack_trace()
			: ftag_(Function_Undefined)
			, ctag_(Function_Undefined)
			, htag_(Handle_Undefined)
			, frames_(0)
			, allocationCount_(0)
		{
			// required by QMap<>
			memset(stack_, 0, sizeof(stack_));
		}
		stack_trace(function_tag ftag, function_tag ctag, handle_tag htag,DWORD *stack, SHORT frames)
			: ftag_(ftag)
			, ctag_(ctag)
			, htag_(htag)
			, frames_(frames)
			, allocationCount_(1)
		{
			memcpy(stack_, stack, frames*sizeof(stack[0]));
		}
		stack_trace(stack_trace const &other)
			: ftag_(other.ftag_)
			, ctag_(other.ctag_)
			, htag_(other.htag_)
			, frames_(other.frames_)
			, allocationCount_(other.allocationCount_)
		{
			memcpy(stack_, other.stack_, frames_*sizeof(stack_[0]));
		}
		stack_trace &operator =(stack_trace const &other)
		{
			ftag_=other.ftag_;
			ctag_=other.ctag_;
			htag_=other.htag_;
			frames_=other.frames_;
			allocationCount_=other.allocationCount_;
			memcpy(stack_, other.stack_, frames_*sizeof(stack_[0]));
			return *this;
		}
	};


	typedef stdext::hash_map<HANDLE, allocation> allocations_type;
	typedef stdext::hash_map<ULONG, stack_trace> stackdb_type;

	// actual value maps
	extern allocations_type allocations;				// all resource allocations
	extern stackdb_type stackdb;						// all stack trace records

	// outputs to debugger
	void qqDebug( char const *format, ... );
	// export to check for live allocations
	bool LeaksAvail();

	// thread-specific functionality
	bool enter();
	void leave();
	bool init_tls();
	void init_tls_slot();
	void release_tls();
	
	/*!
		\brief	describes a cleanup callback 
		Cleanup callback is used by certain allocators when it is not possible to infer
		(statistical) information in the code generator
	*/
	typedef struct
	{
		BOOL FunctionTag;	// if TRUE, ctag corresponds to the cleanup function
							// otherwise, the return is a shared resource
		union 
		{
			function_tag ctag;
			LPVOID dummy_shared_resource;
		};
	} cleanup_callback_ret;

	cleanup_callback_ret CopyImageCleanupCallback(HANDLE a0,UINT,INT a2,INT a3,UINT a4);
	cleanup_callback_ret LoadImageCleanupCallbackA(HINSTANCE a0,char const * a1,UINT a2,INT a3,INT a4,UINT a5);
	cleanup_callback_ret LoadImageCleanupCallbackW(HINSTANCE a0,wchar_t const * a1,UINT a2,INT a3,INT a4,UINT a5);
	// for LoadIcoCur
	cleanup_callback_ret LoadImageCleanupCallback(HINSTANCE a0,wchar_t const * a1,UINT a2,INT a3,INT a4,UINT a5);
	cleanup_callback_ret LoadImageCleanupCallback(HINSTANCE a0,wchar_t const * a1,UINT a2,INT a3,INT a4,UINT a5);
	cleanup_callback_ret GetDCExCleanupCallback(HWND a0,HRGN a1,DWORD a2);
	cleanup_callback_ret GetDCCleanupCallback(HWND a0);
	/*!
		\brief	Decide if we need to record this allocation bases on inheritance information.
		Child windows allocation need not be recorded as they're automatically destroyed by a single
		DestroyWindow(hWndRoot) invocation.
	*/
	cleanup_callback_ret CreateWindowCallbackA(DWORD,char const *,char const *,DWORD,INT,INT,INT,INT,HWND,HMENU,HINSTANCE,LPVOID);
	cleanup_callback_ret CreateWindowCallbackW(DWORD,wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND,HMENU,HINSTANCE,LPVOID);
	cleanup_callback_ret CreateWindowCallbackA(HINSTANCE,char const *,HWND,DLGPROC,LPARAM);
	cleanup_callback_ret CreateWindowCallbackW(HINSTANCE,wchar_t const *,HWND,DLGPROC,LPARAM);
	cleanup_callback_ret CreateWindowCallbackA(HINSTANCE,LPCDLGTEMPLATEA,HWND,DLGPROC,LPARAM);
	cleanup_callback_ret CreateWindowCallbackW(HINSTANCE,LPCDLGTEMPLATEW,HWND,DLGPROC,LPARAM);
	cleanup_callback_ret CreateWindowCallbackA(char const *,char const *,DWORD,INT,INT,INT,INT,HWND,HINSTANCE,LPARAM);
	cleanup_callback_ret CreateWindowCallbackW(wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND,HINSTANCE,LPARAM);
	cleanup_callback_ret SetWindowsHookCallback(INT idHook,HOOKPROC,HINSTANCE,DWORD dwThreadId);
}
