/*
	impl.cpp - 
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

#define _WIN32_WINNT 0x0501	// we need WinXP anyways
#include <windows.h>
#include <string>			// std::basic_string<>
#include "Reminder.h"		// Reminder for #pragma message
#include "leaktrap.h"
#include "intercepts.h"
#include "impl.h"

namespace detail
{
	int const debug_buffer_size=256;
	void qqDebug( char const *format, ... )
	{
		char buf[debug_buffer_size];
		va_list ap;
		va_start(ap, format);
		vsprintf(buf, format, ap);
		//vsprintf_s(buf, debug_buffer_size, format, ap);
		va_end(ap);
		std::string fstr(buf);
		fstr += "\n";
		::OutputDebugStringA(fstr.c_str());
	}
	namespace
	{
		// tls slot used to store thread-sensitive information
		DWORD tls_index=TLS_OUT_OF_INDEXES;
		//int const dc_uncommon_mask=CS_OWNDC|CS_CLASSDC|CS_PARENTDC;

		// \return	true if style indeed identifies a common DC
		bool IsCommonDC(int style)
		{
			//return !(style&dc_uncommon_mask);
			return !(style&CS_OWNDC || style&CS_CLASSDC || style&CS_PARENTDC);
		}

		// \brief	retrieves class information for the specified HWND
		bool QueryClassInfo(HWND a0, WNDCLASSEXA &ci)
		{
			static char szClassName[128];
			ci.cbSize=sizeof(ci);
			if(!::GetClassNameA(a0,szClassName,128))
			{
				return false;
			}
			HINSTANCE hinst=reinterpret_cast<HINSTANCE>(::GetWindowLongA(a0,GWL_HINSTANCE));
			if(!::GetClassInfoExA(hinst,szClassName,&ci))
			{
				return false;
			}
			return true;
		}
		cleanup_callback_ret CreateWindowCallbackHelper(HWND a0)
		{
			cleanup_callback_ret s={0};
			/*
			s.FunctionTag=TRUE;
			s.ctag=Function_DestroyWindow;
			if(NULL!=a0)	// parent is specified
			{
				HWND hWndRoot=::GetAncestor(a0, GA_ROOT);
				if(allocations.find(hWndRoot)!=allocations.end())	// we have a record for this root hWnd
				{
					qqDebug("%s: found a root parent hwnd=%lx for hwnd=%lx", __FUNCTION__, hWndRoot, a0);
					if(::IsDebuggerPresent()) ::DebugBreak();
					// we want to avoid recording this allocation
					s.FunctionTag=FALSE;	// mark it as shared resource ;o)
				}
			}
			*/
			s.FunctionTag=(NULL==a0);	// make it very simple
			s.ctag=Function_DestroyWindow;

			return s;
		}
	}

	// valued maps
	allocations_type allocations;
	stackdb_type stackdb;

	bool LeaksAvail()
	{
		return allocations.begin() != allocations.end();
	}

	bool init_tls()
	{
		// allocate tls slot
		tls_index=::TlsAlloc();
		if(TLS_OUT_OF_INDEXES==tls_index)
			return false;
		return true;
	}
	
	void init_tls_slot()
	{
		::TlsSetValue(tls_index,0);
	}

	void release_tls()
	{
		// release tls slot
		::TlsFree(tls_index);
	}

	bool enter()
	{
/*
		LPVOID lpvData=::TlsGetValue(tls_index);
		if ((lpvData == 0) && (::GetLastError() != ERROR_SUCCESS))
		{
			qqDebug("failed to read nested-call indicator (result=%lx)\n",::GetLastError());
			return true;	// assume entered()
		}
		BOOL nested_indicator=reinterpret_cast<BOOL>(lpvData);
		if(!nested_indicator)
		{	// set nested_indicator
			if(!::TlsSetValue(tls_index, reinterpret_cast<LPVOID>(TRUE)))
			{
				qqDebug("failed to set nested-call indicator (result=%lx)\n",::GetLastError());
				return true;	// assume entered()
			}
		}
		return !nested_indicator;
*/
		BOOL nested_indicator=reinterpret_cast<BOOL>(::TlsGetValue(tls_index));
		if(!nested_indicator)
		{	// set nested_indicator
			::TlsSetValue(tls_index, reinterpret_cast<LPVOID>(TRUE));
		}
		return !nested_indicator;
	}
	void leave()
	{
/*
		LPVOID lpvData=::TlsGetValue(tls_index);
		if ((lpvData == 0) && (::GetLastError() != ERROR_SUCCESS))
		{
			qqDebug("failed to read nested-call indicator (result=%lx)\n",::GetLastError());
			return;
		}
		BOOL nested_indicator=reinterpret_cast<BOOL>(lpvData);
		if(nested_indicator)
		{	// clear nested_indicator
			if(!::TlsSetValue(tls_index, reinterpret_cast<LPVOID>(FALSE)))
			{
				qqDebug("failed to set nested-call indicator (result=%lx)\n",::GetLastError());
				return;
			}
		}
*/
		BOOL nested_indicator=reinterpret_cast<BOOL>(::TlsGetValue(tls_index));
		if(nested_indicator)
		{	// clear nested_indicator
			::TlsSetValue(tls_index, reinterpret_cast<LPVOID>(FALSE));
		}
	}

	cleanup_callback_ret CopyImageCleanupCallback(HANDLE a0,UINT a1,INT a2,INT a3,UINT a4)
	{
		a0,a2,a3,a4;
		cleanup_callback_ret s={0};
		s.FunctionTag=TRUE;
		switch(a1)
		{
			case IMAGE_BITMAP:
				s.ctag=Function_DeleteObject;
				break;
			case IMAGE_CURSOR:
				s.ctag=Function_DestroyCursor;
				break;
			case IMAGE_ICON:
				s.ctag=Function_DestroyIcon;
				break;
			default:
				s.ctag=Function_Undefined;
				break;
		}
		return s;
	}
	cleanup_callback_ret LoadImageCleanupCallbackA(HINSTANCE a0,char const * a1,UINT a2,INT a3,INT a4,UINT a5)
	{
		a0,a1,a3,a4,a5;
		cleanup_callback_ret s={0};
		s.FunctionTag=!(a5&LR_SHARED);
		if(s.FunctionTag)
		{
			switch(a2)
			{
			case IMAGE_BITMAP:
				s.ctag=Function_DeleteObject;
				break;
			case IMAGE_CURSOR:
				s.ctag=Function_DestroyCursor;
				break;
			case IMAGE_ICON:
				s.ctag=Function_DestroyIcon;
				break;
			default:
				s.ctag=Function_Undefined;
				break;
			}
		}
		return s;
	}
	cleanup_callback_ret LoadImageCleanupCallbackW(HINSTANCE a0,wchar_t const * a1,UINT a2,INT a3,INT a4,UINT a5)
	{
		a0,a1,a3,a4,a5;
		cleanup_callback_ret s={0};
		s.FunctionTag=!(a5&LR_SHARED);
		if(s.FunctionTag)
		{
			switch(a2)
			{
			case IMAGE_BITMAP:
				s.ctag=Function_DeleteObject;
				break;
			case IMAGE_CURSOR:
				s.ctag=Function_DestroyCursor;
				break;
			case IMAGE_ICON:
				s.ctag=Function_DestroyIcon;
				break;
			default:
				s.ctag=Function_Undefined;
				break;
			}
		}
		return s;
	}
	cleanup_callback_ret LoadImageCleanupCallback(HINSTANCE a0,wchar_t const * a1,UINT a2,INT a3,INT a4,UINT a5)
	{
		return LoadImageCleanupCallbackW(a0,a1,a2,a3,a4,a5);
	}
	cleanup_callback_ret GetDCExCleanupCallback(HWND a0,HRGN a1,DWORD a2)
	{
		a1,a2;
		cleanup_callback_ret s={0};
		WNDCLASSEXA ci;
		if(QueryClassInfo(a0,ci) && IsCommonDC(ci.style))
		{
			s.FunctionTag=TRUE;
			s.ctag=Function_ReleaseDC;
		}
		else
		{
			// do not require ReleaseDC() for `uncommon' DCs
		}
		return s;	
	}
	cleanup_callback_ret GetDCCleanupCallback(HWND a0)
	{
		return GetDCExCleanupCallback(a0,0,0);
	}
	handle_tag GetHandleTag(function_tag ftag, void* handle)
	{
		handle;
		switch(ftag)
		{
			case Function_LoadCursorA:
			case Function_LoadCursorW:
			case Function_LoadCursorFromFileA:
			case Function_LoadCursorFromFileW:
			case Function_CreateCursor:
				return Handle_CURSOR;
			case Function_LoadIconA:
			case Function_LoadIconW:
			case Function_CreateIconIndirect:
			case Function_CopyIcon:
			case Function_CreateIconFromResourceEx:
				return Handle_ICON;
			case Function_CreateAcceleratorTableA:
			case Function_CreateAcceleratorTableW:
				return Handle_ACCEL;
			case Function_DdeConnect:
				return Handle_CONV;
			case Function_DdeConnectList:
				return Handle_CONVLIST;
			case Function_SetWindowsHookEx:
				return Handle_WNDHOOK;
			case Function_CreateMenu:
			case Function_CreatePopupMenu:
			case Function_LoadMenuA:
			case Function_LoadMenuW:
			case Function_LoadMenuIndirect:
				return Handle_MENU;
			case Function_CreateWindowExA:
			case Function_CreateWindowExW:
			case Function_CreateDialogParamA:
			case Function_CreateDialogParamW:
			case Function_CreateDialogIndirectParamA:
			case Function_CreateDialogIndirectParamW:
			case Function_CreateMDIWindowA:
			case Function_CreateMDIWindowW:
				return Handle_WINDOW;
			case Function_BeginDeferWindowPos:
				return Handle_DWP;
			case Function_GetDC:
			case Function_GetDCEx:
				return Handle_DC;
			case Function_LoadImageA:
			case Function_LoadImageW:
#pragma message(Reminder "LoadImage creates heterogeneous handles!")
				return Handle_CURSOR;	// Actually, can be one of (Handle_BITMAP, Handle_CURSOR, Handle_ICON)
										// still need to decide how to handle this
			default:
				qqDebug("Unhandled function tag specified: %ld for handle=%lx", ftag, handle);
				return Handle_Undefined;
		}
	}
	cleanup_callback_ret CreateWindowCallbackA(DWORD,char const *,char const *,DWORD,INT,INT,INT,INT,HWND hwndParent,HMENU,HINSTANCE,LPVOID)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackW(DWORD,wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND hwndParent,HMENU,HINSTANCE,LPVOID)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackA(HINSTANCE,char const *,HWND hwndParent,DLGPROC,LPARAM)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackW(HINSTANCE,wchar_t const *,HWND hwndParent,DLGPROC,LPARAM)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackA(HINSTANCE,LPCDLGTEMPLATEA,HWND hwndParent,DLGPROC,LPARAM)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackW(HINSTANCE,LPCDLGTEMPLATEW,HWND hwndParent,DLGPROC,LPARAM)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackA(char const *,char const *,DWORD,INT,INT,INT,INT,HWND hwndParent,HINSTANCE,LPARAM)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret CreateWindowCallbackW(wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND hwndParent,HINSTANCE,LPARAM)
	{
		return CreateWindowCallbackHelper(hwndParent);
	}
	cleanup_callback_ret SetWindowsHookCallback(INT idHook,HOOKPROC,HINSTANCE,DWORD dwThreadId)
	{
		cleanup_callback_ret s={0};
		s.FunctionTag=TRUE;
		s.ctag=Function_UnhookWindowsHookEx;
		switch(idHook)
		{
			// explicit globals:
		case WH_JOURNALPLAYBACK:
		case WH_JOURNALRECORD:
		case WH_KEYBOARD_LL:
		case WH_MOUSE_LL:
		case WH_SYSMSGFILTER:
			s.FunctionTag=FALSE;	// avoid this allocation
			break;
		default:	// all other
			if(!dwThreadId)			// hook for all threads
			{
				s.FunctionTag=FALSE;
			}
			break;
		}
		return s;
	}
}
