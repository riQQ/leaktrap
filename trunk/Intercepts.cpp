/*
	intercepts.cpp - allocation/cleanup interception

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
/*
	Generated on Mittwoch, 12. November 2008 16:51:19. Do not EDIT!!!
*/
#define _WIN32_WINNT 0x0501	// we need WinXP anyways
#include <windows.h>
#include <wingdi.h>
#include <detours.h>
#include "leaktrap.h"	//import/export macros
#include "Intercepts.h"
#include "impl.h"


namespace detail
{
	namespace
	{
		// stack capture helper
		USHORT (WINAPI *pfnCaptureStackBackTrace)(ULONG,ULONG,PVOID*,PULONG)=0;
	} // internals
	
	//
	// function pointers
	//
	HBITMAP (WINAPI *OldLoadBitmapA)(HINSTANCE,char const *) = LoadBitmapA;
	HBITMAP (WINAPI *OldLoadBitmapW)(HINSTANCE,wchar_t const *) = LoadBitmapW;
	HBITMAP (WINAPI *OldCreateBitmap)(INT,INT,UINT,UINT,VOID CONST *) = CreateBitmap;
	HBITMAP (WINAPI *OldCreateBitmapIndirect)(BITMAP CONST *) = CreateBitmapIndirect;
	HBITMAP (WINAPI *OldCreateCompatibleBitmap)(HDC,INT,INT) = CreateCompatibleBitmap;
	HBITMAP (WINAPI *OldCreateDiscardableBitmap)(HDC,INT,INT) = CreateDiscardableBitmap;
	HBITMAP (WINAPI *OldCreateDIBitmap)(HDC,BITMAPINFOHEADER CONST *,DWORD,VOID CONST *,BITMAPINFO CONST *,UINT) = CreateDIBitmap;
	HBITMAP (WINAPI *OldCreateDIBSection)(HDC,BITMAPINFO CONST *,UINT,VOID **,HANDLE,DWORD) = CreateDIBSection;
	HBRUSH (WINAPI *OldCreateBrushIndirect)(LOGBRUSH CONST *) = CreateBrushIndirect;
	HBRUSH (WINAPI *OldCreateSolidBrush)(COLORREF) = CreateSolidBrush;
	HBRUSH (WINAPI *OldCreatePatternBrush)(HBITMAP) = CreatePatternBrush;
	HBRUSH (WINAPI *OldCreateDIBPatternBrush)(HGLOBAL,UINT) = CreateDIBPatternBrush;
	HBRUSH (WINAPI *OldCreateDIBPatternBrushPt)(VOID CONST *,UINT) = CreateDIBPatternBrushPt;
	HBRUSH (WINAPI *OldCreateHatchBrush)(INT,COLORREF) = CreateHatchBrush;
	HFONT (WINAPI *OldCreateFontA)(INT,INT,INT,INT,INT,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,char const *) = CreateFontA;
	HFONT (WINAPI *OldCreateFontW)(INT,INT,INT,INT,INT,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,wchar_t const *) = CreateFontW;
	HFONT (WINAPI *OldCreateFontIndirectA)(LOGFONTA const *) = CreateFontIndirectA;
	HFONT (WINAPI *OldCreateFontIndirectW)(LOGFONTW const *) = CreateFontIndirectW;
	HPEN (WINAPI *OldCreatePen)(INT,INT,COLORREF) = CreatePen;
	HPEN (WINAPI *OldCreatePenIndirect)(LOGPEN CONST *) = CreatePenIndirect;
	HPEN (WINAPI *OldExtCreatePen)(DWORD,DWORD,LOGBRUSH CONST *,DWORD,DWORD CONST *) = ExtCreatePen;
	HRGN (WINAPI *OldPathToRegion)(HDC) = PathToRegion;
	HRGN (WINAPI *OldCreateEllipticRgn)(INT,INT,INT,INT) = CreateEllipticRgn;
	HRGN (WINAPI *OldCreateEllipticRgnIndirect)(RECT CONST *) = CreateEllipticRgnIndirect;
	HRGN (WINAPI *OldCreatePolygonRgn)(POINT CONST *,INT,INT) = CreatePolygonRgn;
	HRGN (WINAPI *OldCreatePolyPolygonRgn)(POINT CONST *,INT CONST *,INT,INT) = CreatePolyPolygonRgn;
	HRGN (WINAPI *OldCreateRectRgn)(INT,INT,INT,INT) = CreateRectRgn;
	HRGN (WINAPI *OldCreateRectRgnIndirect)(RECT CONST *) = CreateRectRgnIndirect;
	HRGN (WINAPI *OldCreateRoundRectRgn)(INT,INT,INT,INT,INT,INT) = CreateRoundRectRgn;
	HRGN (WINAPI *OldExtCreateRegion)(XFORM CONST *,DWORD,RGNDATA CONST *) = ExtCreateRegion;
	HPALETTE (WINAPI *OldCreateHalftonePalette)(HDC) = CreateHalftonePalette;
	HPALETTE (WINAPI *OldCreatePalette)(LOGPALETTE CONST *) = CreatePalette;
	HDC (WINAPI *OldGetWindowDC)(HWND) = GetWindowDC;
	HDC (WINAPI *OldCreateCompatibleDC)(HDC) = CreateCompatibleDC;
	HDC (WINAPI *OldCreateDCA)(char const *,char const *,char const *,DEVMODEA const *) = CreateDCA;
	HDC (WINAPI *OldCreateDCW)(wchar_t const *,wchar_t const *,wchar_t const *,DEVMODEW const *) = CreateDCW;
	HDC (WINAPI *OldCreateICA)(char const *,char const *,char const *,DEVMODEA const *) = CreateICA;
	HDC (WINAPI *OldCreateICW)(wchar_t const *,wchar_t const *,wchar_t const *,DEVMODEW const *) = CreateICW;
	HDC (WINAPI *OldCreateMetaFileA)(char const *) = CreateMetaFileA;
	HDC (WINAPI *OldCreateMetaFileW)(wchar_t const *) = CreateMetaFileW;
	HDC (WINAPI *OldCreateEnhMetaFileA)(HDC,char const *,RECT CONST *,char const *) = CreateEnhMetaFileA;
	HDC (WINAPI *OldCreateEnhMetaFileW)(HDC,wchar_t const *,RECT CONST *,wchar_t const *) = CreateEnhMetaFileW;
	HENHMETAFILE (WINAPI *OldGetEnhMetaFileA)(char const *) = GetEnhMetaFileA;
	HENHMETAFILE (WINAPI *OldGetEnhMetaFileW)(wchar_t const *) = GetEnhMetaFileW;
	HENHMETAFILE (WINAPI *OldCloseEnhMetaFile)(HDC) = CloseEnhMetaFile;
	HMETAFILE (WINAPI *OldCloseMetaFile)(HDC) = CloseMetaFile;
	HICON (WINAPI *OldCreateIconFromResourceEx)(PBYTE,DWORD,BOOL,DWORD,INT,INT,UINT) = CreateIconFromResourceEx;
	HICON (WINAPI *OldCreateIconIndirect)(PICONINFO) = CreateIconIndirect;
	HICON (WINAPI *OldCopyIcon)(HICON) = CopyIcon;
	HICON (WINAPI *OldExtractIconA)(HINSTANCE,char const *,UINT) = ExtractIconA;
	HICON (WINAPI *OldExtractIconW)(HINSTANCE,wchar_t const *,UINT) = ExtractIconW;
	HANDLE (WINAPI *OldCopyImage)(HANDLE,UINT,INT,INT,UINT) = CopyImage;
	HANDLE (WINAPI *OldLoadImageA)(HINSTANCE,char const *,UINT,INT,INT,UINT) = LoadImageA;
	HANDLE (WINAPI *OldLoadImageW)(HINSTANCE,wchar_t const *,UINT,INT,INT,UINT) = LoadImageW;
	HDC (WINAPI *OldGetDCEx)(HWND,HRGN,DWORD) = GetDCEx;
	HDC (WINAPI *OldGetDC)(HWND) = GetDC;
	BOOL (WINAPI *OldDrawStateA)(HDC,HBRUSH,DRAWSTATEPROC,LPARAM,WPARAM,INT,INT,INT,INT,UINT) = DrawStateA;
	BOOL (WINAPI *OldDrawStateW)(HDC,HBRUSH,DRAWSTATEPROC,LPARAM,WPARAM,INT,INT,INT,INT,UINT) = DrawStateW;
	HCURSOR (WINAPI *OldLoadCursorA)(HINSTANCE,char const *) = LoadCursorA;
	HCURSOR (WINAPI *OldLoadCursorW)(HINSTANCE,wchar_t const *) = LoadCursorW;
	HCURSOR (WINAPI *OldLoadCursorFromFileA)(char const *) = LoadCursorFromFileA;
	HCURSOR (WINAPI *OldLoadCursorFromFileW)(wchar_t const *) = LoadCursorFromFileW;
	HICON (WINAPI *OldLoadIconA)(HINSTANCE,char const *) = LoadIconA;
	HICON (WINAPI *OldLoadIconW)(HINSTANCE,wchar_t const *) = LoadIconW;
	HACCEL (WINAPI *OldCreateAcceleratorTableA)(LPACCEL,INT) = CreateAcceleratorTableA;
	HACCEL (WINAPI *OldCreateAcceleratorTableW)(LPACCEL,INT) = CreateAcceleratorTableW;
	HCURSOR (WINAPI *OldCreateCursor)(HINSTANCE,INT,INT,INT,INT,CONST VOID *,CONST VOID *) = CreateCursor;
	HCONV (WINAPI *OldDdeConnect)(DWORD,HSZ,HSZ,PCONVCONTEXT) = DdeConnect;
	HCONVLIST (WINAPI *OldDdeConnectList)(DWORD,HSZ,HSZ,HCONVLIST,PCONVCONTEXT) = DdeConnectList;
	HHOOK (WINAPI *OldSetWindowsHookEx)(INT,HOOKPROC,HINSTANCE,DWORD) = SetWindowsHookEx;
	HMENU (WINAPI *OldCreateMenu)() = CreateMenu;
	HMENU (WINAPI *OldCreatePopupMenu)() = CreatePopupMenu;
	HMENU (WINAPI *OldLoadMenuA)(HINSTANCE,char const *) = LoadMenuA;
	HMENU (WINAPI *OldLoadMenuW)(HINSTANCE,wchar_t const *) = LoadMenuW;
	HMENU (WINAPI *OldLoadMenuIndirect)(MENUTEMPLATE CONST *) = LoadMenuIndirect;
	HDWP (WINAPI *OldBeginDeferWindowPos)(INT) = BeginDeferWindowPos;
	HWND (WINAPI *OldCreateWindowExA)(DWORD,char const *,char const *,DWORD,INT,INT,INT,INT,HWND,HMENU,HINSTANCE,LPVOID) = CreateWindowExA;
	HWND (WINAPI *OldCreateWindowExW)(DWORD,wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND,HMENU,HINSTANCE,LPVOID) = CreateWindowExW;
	HWND (WINAPI *OldCreateDialogParamA)(HINSTANCE,char const *,HWND,DLGPROC,LPARAM) = CreateDialogParamA;
	HWND (WINAPI *OldCreateDialogParamW)(HINSTANCE,wchar_t const *,HWND,DLGPROC,LPARAM) = CreateDialogParamW;
	HWND (WINAPI *OldCreateDialogIndirectParamA)(HINSTANCE,LPCDLGTEMPLATEA,HWND,DLGPROC,LPARAM) = CreateDialogIndirectParamA;
	HWND (WINAPI *OldCreateDialogIndirectParamW)(HINSTANCE,LPCDLGTEMPLATEW,HWND,DLGPROC,LPARAM) = CreateDialogIndirectParamW;
	HWND (WINAPI *OldCreateMDIWindowA)(char const *,char const *,DWORD,INT,INT,INT,INT,HWND,HINSTANCE,LPARAM) = CreateMDIWindowA;
	HWND (WINAPI *OldCreateMDIWindowW)(wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND,HINSTANCE,LPARAM) = CreateMDIWindowW;
	BOOL (WINAPI *OldDeleteObject)(HGDIOBJ) = DeleteObject;
	BOOL (WINAPI *OldDeleteDC)(HDC) = DeleteDC;
	BOOL (WINAPI *OldDeleteMetaFile)(HMETAFILE) = DeleteMetaFile;
	BOOL (WINAPI *OldDeleteEnhMetaFile)(HENHMETAFILE) = DeleteEnhMetaFile;
	INT (WINAPI *OldReleaseDC)(HWND,HDC) = ReleaseDC;
	BOOL (WINAPI *OldDestroyIcon)(HICON) = DestroyIcon;
	BOOL (WINAPI *OldDestroyCursor)(HCURSOR) = DestroyCursor;
	BOOL (WINAPI *OldDdeDisconnect)(HCONV) = DdeDisconnect;
	BOOL (WINAPI *OldDdeDisconnectList)(HCONVLIST) = DdeDisconnectList;
	BOOL (WINAPI *OldDestroyWindow)(HWND) = DestroyWindow;
	BOOL (WINAPI *OldEndDeferWindowPos)(HDWP) = EndDeferWindowPos;
	BOOL (WINAPI *OldDestroyAcceleratorTable)(HACCEL) = DestroyAcceleratorTable;
	BOOL (WINAPI *OldUnhookWindowsHookEx)(HHOOK) = UnhookWindowsHookEx;
	BOOL (WINAPI *OldDestroyMenu)(HMENU) = DestroyMenu;
	//
	// cleanup callbacks
	//
	extern cleanup_callback_ret CopyImageCleanupCallback(HANDLE,UINT,INT,INT,UINT);
	extern cleanup_callback_ret LoadImageCleanupCallbackA(HINSTANCE,char const *,UINT,INT,INT,UINT);
	extern cleanup_callback_ret LoadImageCleanupCallbackW(HINSTANCE,wchar_t const *,UINT,INT,INT,UINT);
	extern cleanup_callback_ret GetDCExCleanupCallback(HWND,HRGN,DWORD);
	extern cleanup_callback_ret GetDCCleanupCallback(HWND);
	extern cleanup_callback_ret SetWindowsHookCallback(INT,HOOKPROC,HINSTANCE,DWORD);
	extern cleanup_callback_ret CreateWindowCallbackA(DWORD,char const *,char const *,DWORD,INT,INT,INT,INT,HWND,HMENU,HINSTANCE,LPVOID);
	extern cleanup_callback_ret CreateWindowCallbackW(DWORD,wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND,HMENU,HINSTANCE,LPVOID);
	extern cleanup_callback_ret CreateWindowCallbackA(HINSTANCE,char const *,HWND,DLGPROC,LPARAM);
	extern cleanup_callback_ret CreateWindowCallbackW(HINSTANCE,wchar_t const *,HWND,DLGPROC,LPARAM);
	extern cleanup_callback_ret CreateWindowCallbackA(HINSTANCE,LPCDLGTEMPLATEA,HWND,DLGPROC,LPARAM);
	extern cleanup_callback_ret CreateWindowCallbackW(HINSTANCE,LPCDLGTEMPLATEW,HWND,DLGPROC,LPARAM);
	extern cleanup_callback_ret CreateWindowCallbackA(char const *,char const *,DWORD,INT,INT,INT,INT,HWND,HINSTANCE,LPARAM);
	extern cleanup_callback_ret CreateWindowCallbackW(wchar_t const *,wchar_t const *,DWORD,INT,INT,INT,INT,HWND,HINSTANCE,LPARAM);
	//
	// api detours
	//
	HBITMAP WINAPI MyLoadBitmapA(HINSTANCE a0,char const * a1)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldLoadBitmapA(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_LoadBitmapA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyLoadBitmapW(HINSTANCE a0,wchar_t const * a1)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldLoadBitmapW(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_LoadBitmapW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyCreateBitmap(INT a0,INT a1,UINT a2,UINT a3,VOID CONST * a4)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldCreateBitmap(a0,a1,a2,a3,a4);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateBitmap,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyCreateBitmapIndirect(BITMAP CONST * a0)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldCreateBitmapIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateBitmapIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyCreateCompatibleBitmap(HDC a0,INT a1,INT a2)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldCreateCompatibleBitmap(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateCompatibleBitmap,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyCreateDiscardableBitmap(HDC a0,INT a1,INT a2)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldCreateDiscardableBitmap(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDiscardableBitmap,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyCreateDIBitmap(HDC a0,BITMAPINFOHEADER CONST * a1,DWORD a2,VOID CONST * a3,BITMAPINFO CONST * a4,UINT a5)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldCreateDIBitmap(a0,a1,a2,a3,a4,a5);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDIBitmap,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBITMAP WINAPI MyCreateDIBSection(HDC a0,BITMAPINFO CONST * a1,UINT a2,VOID ** a3,HANDLE a4,DWORD a5)
	{
		bool flat_call=enter();
		HBITMAP ret_handle=OldCreateDIBSection(a0,a1,a2,a3,a4,a5);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDIBSection,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBRUSH WINAPI MyCreateBrushIndirect(LOGBRUSH CONST * a0)
	{
		bool flat_call=enter();
		HBRUSH ret_handle=OldCreateBrushIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateBrushIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBRUSH WINAPI MyCreateSolidBrush(COLORREF a0)
	{
		bool flat_call=enter();
		HBRUSH ret_handle=OldCreateSolidBrush(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateSolidBrush,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBRUSH WINAPI MyCreatePatternBrush(HBITMAP a0)
	{
		bool flat_call=enter();
		HBRUSH ret_handle=OldCreatePatternBrush(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreatePatternBrush,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBRUSH WINAPI MyCreateDIBPatternBrush(HGLOBAL a0,UINT a1)
	{
		bool flat_call=enter();
		HBRUSH ret_handle=OldCreateDIBPatternBrush(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDIBPatternBrush,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBRUSH WINAPI MyCreateDIBPatternBrushPt(VOID CONST * a0,UINT a1)
	{
		bool flat_call=enter();
		HBRUSH ret_handle=OldCreateDIBPatternBrushPt(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDIBPatternBrushPt,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HBRUSH WINAPI MyCreateHatchBrush(INT a0,COLORREF a1)
	{
		bool flat_call=enter();
		HBRUSH ret_handle=OldCreateHatchBrush(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateHatchBrush,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HFONT WINAPI MyCreateFontA(INT a0,INT a1,INT a2,INT a3,INT a4,DWORD a5,DWORD a6,DWORD a7,DWORD a8,DWORD a9,DWORD a10,DWORD a11,DWORD a12,char const * a13)
	{
		bool flat_call=enter();
		HFONT ret_handle=OldCreateFontA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateFontA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HFONT WINAPI MyCreateFontW(INT a0,INT a1,INT a2,INT a3,INT a4,DWORD a5,DWORD a6,DWORD a7,DWORD a8,DWORD a9,DWORD a10,DWORD a11,DWORD a12,wchar_t const * a13)
	{
		bool flat_call=enter();
		HFONT ret_handle=OldCreateFontW(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateFontW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HFONT WINAPI MyCreateFontIndirectA(LOGFONTA const * a0)
	{
		bool flat_call=enter();
		HFONT ret_handle=OldCreateFontIndirectA(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateFontIndirectA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HFONT WINAPI MyCreateFontIndirectW(LOGFONTW const * a0)
	{
		bool flat_call=enter();
		HFONT ret_handle=OldCreateFontIndirectW(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateFontIndirectW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HPEN WINAPI MyCreatePen(INT a0,INT a1,COLORREF a2)
	{
		bool flat_call=enter();
		HPEN ret_handle=OldCreatePen(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreatePen,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HPEN WINAPI MyCreatePenIndirect(LOGPEN CONST * a0)
	{
		bool flat_call=enter();
		HPEN ret_handle=OldCreatePenIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreatePenIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HPEN WINAPI MyExtCreatePen(DWORD a0,DWORD a1,LOGBRUSH CONST * a2,DWORD a3,DWORD CONST * a4)
	{
		bool flat_call=enter();
		HPEN ret_handle=OldExtCreatePen(a0,a1,a2,a3,a4);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_ExtCreatePen,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyPathToRegion(HDC a0)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldPathToRegion(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_PathToRegion,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreateEllipticRgn(INT a0,INT a1,INT a2,INT a3)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreateEllipticRgn(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateEllipticRgn,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreateEllipticRgnIndirect(RECT CONST * a0)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreateEllipticRgnIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateEllipticRgnIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreatePolygonRgn(POINT CONST * a0,INT a1,INT a2)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreatePolygonRgn(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreatePolygonRgn,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreatePolyPolygonRgn(POINT CONST * a0,INT CONST * a1,INT a2,INT a3)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreatePolyPolygonRgn(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreatePolyPolygonRgn,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreateRectRgn(INT a0,INT a1,INT a2,INT a3)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreateRectRgn(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateRectRgn,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreateRectRgnIndirect(RECT CONST * a0)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreateRectRgnIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateRectRgnIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyCreateRoundRectRgn(INT a0,INT a1,INT a2,INT a3,INT a4,INT a5)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldCreateRoundRectRgn(a0,a1,a2,a3,a4,a5);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateRoundRectRgn,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HRGN WINAPI MyExtCreateRegion(XFORM CONST * a0,DWORD a1,RGNDATA CONST * a2)
	{
		bool flat_call=enter();
		HRGN ret_handle=OldExtCreateRegion(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_ExtCreateRegion,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HPALETTE WINAPI MyCreateHalftonePalette(HDC a0)
	{
		bool flat_call=enter();
		HPALETTE ret_handle=OldCreateHalftonePalette(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateHalftonePalette,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HPALETTE WINAPI MyCreatePalette(LOGPALETTE CONST * a0)
	{
		bool flat_call=enter();
		HPALETTE ret_handle=OldCreatePalette(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteObject;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreatePalette,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyGetWindowDC(HWND a0)
	{
		bool flat_call=enter();
		HDC ret_handle=OldGetWindowDC(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_ReleaseDC;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_GetWindowDC,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateCompatibleDC(HDC a0)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateCompatibleDC(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteDC;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateCompatibleDC,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateDCA(char const * a0,char const * a1,char const * a2,DEVMODEA const * a3)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateDCA(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteDC;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDCA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateDCW(wchar_t const * a0,wchar_t const * a1,wchar_t const * a2,DEVMODEW const * a3)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateDCW(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteDC;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateDCW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateICA(char const * a0,char const * a1,char const * a2,DEVMODEA const * a3)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateICA(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteDC;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateICA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateICW(wchar_t const * a0,wchar_t const * a1,wchar_t const * a2,DEVMODEW const * a3)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateICW(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteDC;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateICW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateMetaFileA(char const * a0)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateMetaFileA(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_CloseMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateMetaFileA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateMetaFileW(wchar_t const * a0)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateMetaFileW(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_CloseMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateMetaFileW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateEnhMetaFileA(HDC a0,char const * a1,RECT CONST * a2,char const * a3)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateEnhMetaFileA(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_CloseEnhMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateEnhMetaFileA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyCreateEnhMetaFileW(HDC a0,wchar_t const * a1,RECT CONST * a2,wchar_t const * a3)
	{
		bool flat_call=enter();
		HDC ret_handle=OldCreateEnhMetaFileW(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_CloseEnhMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CreateEnhMetaFileW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HENHMETAFILE WINAPI MyGetEnhMetaFileA(char const * a0)
	{
		bool flat_call=enter();
		HENHMETAFILE ret_handle=OldGetEnhMetaFileA(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteEnhMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_GetEnhMetaFileA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HENHMETAFILE WINAPI MyGetEnhMetaFileW(wchar_t const * a0)
	{
		bool flat_call=enter();
		HENHMETAFILE ret_handle=OldGetEnhMetaFileW(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteEnhMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_GetEnhMetaFileW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HENHMETAFILE WINAPI MyCloseEnhMetaFile(HDC a0)
	{
		bool flat_call=enter();
		HENHMETAFILE ret_handle=OldCloseEnhMetaFile(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteEnhMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CloseEnhMetaFile,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		// perform cleanup tasks
		{
			HDC handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_CloseEnhMetaFile != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: CloseEnhMetaFile(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HMETAFILE WINAPI MyCloseMetaFile(HDC a0)
	{
		bool flat_call=enter();
		HMETAFILE ret_handle=OldCloseMetaFile(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DeleteMetaFile;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					DWORD obj_type=::GetObjectType(ret_handle);
					if(0==obj_type)	// unable to determine object type
					{
						qqDebug("Unable to determine GDI object type for handle=%lx, error=%lx",ret_handle,::GetLastError());
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					handle_tag htag=static_cast<handle_tag>(obj_type);
					stackdb[traceHash]=stack_trace(Function_CloseMetaFile,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		// perform cleanup tasks
		{
			HDC handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_CloseMetaFile != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: CloseMetaFile(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyCreateIconFromResourceEx(PBYTE a0,DWORD a1,BOOL a2,DWORD a3,INT a4,INT a5,UINT a6)
	{
		bool flat_call=enter();
		HICON ret_handle=OldCreateIconFromResourceEx(a0,a1,a2,a3,a4,a5,a6);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyIcon;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateIconFromResourceEx,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateIconFromResourceEx,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyCreateIconIndirect(PICONINFO a0)
	{
		bool flat_call=enter();
		HICON ret_handle=OldCreateIconIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyIcon;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateIconIndirect,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateIconIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyCopyIcon(HICON a0)
	{
		bool flat_call=enter();
		HICON ret_handle=OldCopyIcon(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyIcon;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CopyIcon,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CopyIcon,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyExtractIconA(HINSTANCE a0,char const * a1,UINT a2)
	{
		bool flat_call=enter();
		HICON ret_handle=OldExtractIconA(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyIcon;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_ExtractIconA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_ExtractIconA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyExtractIconW(HINSTANCE a0,wchar_t const * a1,UINT a2)
	{
		bool flat_call=enter();
		HICON ret_handle=OldExtractIconW(a0,a1,a2);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyIcon;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_ExtractIconW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_ExtractIconW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HANDLE WINAPI MyCopyImage(HANDLE a0,UINT a1,INT a2,INT a3,UINT a4)
	{
		bool flat_call=enter();
		HANDLE ret_handle=OldCopyImage(a0,a1,a2,a3,a4);
		cleanup_callback_ret s=CopyImageCleanupCallback(a0,a1,a2,a3,a4);
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CopyImage,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CopyImage,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HANDLE WINAPI MyLoadImageA(HINSTANCE a0,char const * a1,UINT a2,INT a3,INT a4,UINT a5)
	{
		bool flat_call=enter();
		HANDLE ret_handle=OldLoadImageA(a0,a1,a2,a3,a4,a5);
		cleanup_callback_ret s=LoadImageCleanupCallbackA(a0,a1,a2,a3,a4,a5);
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_LoadImageA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_LoadImageA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HANDLE WINAPI MyLoadImageW(HINSTANCE a0,wchar_t const * a1,UINT a2,INT a3,INT a4,UINT a5)
	{
		bool flat_call=enter();
		HANDLE ret_handle=OldLoadImageW(a0,a1,a2,a3,a4,a5);
		cleanup_callback_ret s=LoadImageCleanupCallbackW(a0,a1,a2,a3,a4,a5);
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_LoadImageW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_LoadImageW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyGetDCEx(HWND a0,HRGN a1,DWORD a2)
	{
		bool flat_call=enter();
		HDC ret_handle=OldGetDCEx(a0,a1,a2);
		cleanup_callback_ret s=GetDCExCleanupCallback(a0,a1,a2);
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_GetDCEx,ret_handle);
					stackdb[traceHash]=stack_trace(Function_GetDCEx,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDC WINAPI MyGetDC(HWND a0)
	{
		bool flat_call=enter();
		HDC ret_handle=OldGetDC(a0);
		cleanup_callback_ret s=GetDCCleanupCallback(a0);
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_GetDC,ret_handle);
					stackdb[traceHash]=stack_trace(Function_GetDC,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	BOOL WINAPI MyDrawStateA(HDC a0,HBRUSH a1,DRAWSTATEPROC a2,LPARAM a3,WPARAM a4,INT a5,INT a6,INT a7,INT a8,UINT a9)
	{
		bool flat_call=enter();
		BOOL ret_handle=OldDrawStateA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	BOOL WINAPI MyDrawStateW(HDC a0,HBRUSH a1,DRAWSTATEPROC a2,LPARAM a3,WPARAM a4,INT a5,INT a6,INT a7,INT a8,UINT a9)
	{
		bool flat_call=enter();
		BOOL ret_handle=OldDrawStateW(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCURSOR WINAPI MyLoadCursorA(HINSTANCE a0,char const * a1)
	{
		bool flat_call=enter();
		HCURSOR ret_handle=OldLoadCursorA(a0,a1);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCURSOR WINAPI MyLoadCursorW(HINSTANCE a0,wchar_t const * a1)
	{
		bool flat_call=enter();
		HCURSOR ret_handle=OldLoadCursorW(a0,a1);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCURSOR WINAPI MyLoadCursorFromFileA(char const * a0)
	{
		bool flat_call=enter();
		HCURSOR ret_handle=OldLoadCursorFromFileA(a0);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCURSOR WINAPI MyLoadCursorFromFileW(wchar_t const * a0)
	{
		bool flat_call=enter();
		HCURSOR ret_handle=OldLoadCursorFromFileW(a0);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyLoadIconA(HINSTANCE a0,char const * a1)
	{
		bool flat_call=enter();
		HICON ret_handle=OldLoadIconA(a0,a1);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HICON WINAPI MyLoadIconW(HINSTANCE a0,wchar_t const * a1)
	{
		bool flat_call=enter();
		HICON ret_handle=OldLoadIconW(a0,a1);
		if(!flat_call)
		{
			goto Epilog;
		}
		leave();
Epilog:
		return ret_handle;
	}
	HACCEL WINAPI MyCreateAcceleratorTableA(LPACCEL a0,INT a1)
	{
		bool flat_call=enter();
		HACCEL ret_handle=OldCreateAcceleratorTableA(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyAcceleratorTable;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateAcceleratorTableA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateAcceleratorTableA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HACCEL WINAPI MyCreateAcceleratorTableW(LPACCEL a0,INT a1)
	{
		bool flat_call=enter();
		HACCEL ret_handle=OldCreateAcceleratorTableW(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyAcceleratorTable;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateAcceleratorTableW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateAcceleratorTableW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCURSOR WINAPI MyCreateCursor(HINSTANCE a0,INT a1,INT a2,INT a3,INT a4,CONST VOID * a5,CONST VOID * a6)
	{
		bool flat_call=enter();
		HCURSOR ret_handle=OldCreateCursor(a0,a1,a2,a3,a4,a5,a6);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyCursor;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateCursor,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateCursor,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCONV WINAPI MyDdeConnect(DWORD a0,HSZ a1,HSZ a2,PCONVCONTEXT a3)
	{
		bool flat_call=enter();
		HCONV ret_handle=OldDdeConnect(a0,a1,a2,a3);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DdeDisconnect;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_DdeConnect,ret_handle);
					stackdb[traceHash]=stack_trace(Function_DdeConnect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HCONVLIST WINAPI MyDdeConnectList(DWORD a0,HSZ a1,HSZ a2,HCONVLIST a3,PCONVCONTEXT a4)
	{
		bool flat_call=enter();
		HCONVLIST ret_handle=OldDdeConnectList(a0,a1,a2,a3,a4);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DdeDisconnectList;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_DdeConnectList,ret_handle);
					stackdb[traceHash]=stack_trace(Function_DdeConnectList,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HHOOK WINAPI MySetWindowsHookEx(INT a0,HOOKPROC a1,HINSTANCE a2,DWORD a3)
	{
		bool flat_call=enter();
		HHOOK ret_handle=OldSetWindowsHookEx(a0,a1,a2,a3);
		cleanup_callback_ret s=SetWindowsHookCallback(a0,a1,a2,a3);
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_SetWindowsHookEx,ret_handle);
					stackdb[traceHash]=stack_trace(Function_SetWindowsHookEx,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HMENU WINAPI MyCreateMenu()
	{
		bool flat_call=enter();
		HMENU ret_handle=OldCreateMenu();
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyMenu;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateMenu,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateMenu,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HMENU WINAPI MyCreatePopupMenu()
	{
		bool flat_call=enter();
		HMENU ret_handle=OldCreatePopupMenu();
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyMenu;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreatePopupMenu,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreatePopupMenu,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HMENU WINAPI MyLoadMenuA(HINSTANCE a0,char const * a1)
	{
		bool flat_call=enter();
		HMENU ret_handle=OldLoadMenuA(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyMenu;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_LoadMenuA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_LoadMenuA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HMENU WINAPI MyLoadMenuW(HINSTANCE a0,wchar_t const * a1)
	{
		bool flat_call=enter();
		HMENU ret_handle=OldLoadMenuW(a0,a1);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyMenu;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_LoadMenuW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_LoadMenuW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HMENU WINAPI MyLoadMenuIndirect(MENUTEMPLATE CONST * a0)
	{
		bool flat_call=enter();
		HMENU ret_handle=OldLoadMenuIndirect(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_DestroyMenu;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_LoadMenuIndirect,ret_handle);
					stackdb[traceHash]=stack_trace(Function_LoadMenuIndirect,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HDWP WINAPI MyBeginDeferWindowPos(INT a0)
	{
		bool flat_call=enter();
		HDWP ret_handle=OldBeginDeferWindowPos(a0);
		cleanup_callback_ret s;
		s.FunctionTag=TRUE;
		s.ctag=Function_EndDeferWindowPos;
		if(!flat_call)
		{
			goto Epilog;
		}
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_BeginDeferWindowPos,ret_handle);
					stackdb[traceHash]=stack_trace(Function_BeginDeferWindowPos,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		leave();
Epilog:
		return ret_handle;
	}
	HWND WINAPI MyCreateWindowExA(DWORD a0,char const * a1,char const * a2,DWORD a3,INT a4,INT a5,INT a6,INT a7,HWND a8,HMENU a9,HINSTANCE a10,LPVOID a11)
	{
		HWND ret_handle=OldCreateWindowExA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);
		cleanup_callback_ret s=CreateWindowCallbackA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateWindowExA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateWindowExA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateWindowExW(DWORD a0,wchar_t const * a1,wchar_t const * a2,DWORD a3,INT a4,INT a5,INT a6,INT a7,HWND a8,HMENU a9,HINSTANCE a10,LPVOID a11)
	{
		HWND ret_handle=OldCreateWindowExW(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);
		cleanup_callback_ret s=CreateWindowCallbackW(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateWindowExW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateWindowExW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateDialogParamA(HINSTANCE a0,char const * a1,HWND a2,DLGPROC a3,LPARAM a4)
	{
		HWND ret_handle=OldCreateDialogParamA(a0,a1,a2,a3,a4);
		cleanup_callback_ret s=CreateWindowCallbackA(a0,a1,a2,a3,a4);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateDialogParamA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateDialogParamA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateDialogParamW(HINSTANCE a0,wchar_t const * a1,HWND a2,DLGPROC a3,LPARAM a4)
	{
		HWND ret_handle=OldCreateDialogParamW(a0,a1,a2,a3,a4);
		cleanup_callback_ret s=CreateWindowCallbackW(a0,a1,a2,a3,a4);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateDialogParamW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateDialogParamW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateDialogIndirectParamA(HINSTANCE a0,LPCDLGTEMPLATEA a1,HWND a2,DLGPROC a3,LPARAM a4)
	{
		HWND ret_handle=OldCreateDialogIndirectParamA(a0,a1,a2,a3,a4);
		cleanup_callback_ret s=CreateWindowCallbackA(a0,a1,a2,a3,a4);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateDialogIndirectParamA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateDialogIndirectParamA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateDialogIndirectParamW(HINSTANCE a0,LPCDLGTEMPLATEW a1,HWND a2,DLGPROC a3,LPARAM a4)
	{
		HWND ret_handle=OldCreateDialogIndirectParamW(a0,a1,a2,a3,a4);
		cleanup_callback_ret s=CreateWindowCallbackW(a0,a1,a2,a3,a4);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateDialogIndirectParamW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateDialogIndirectParamW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateMDIWindowA(char const * a0,char const * a1,DWORD a2,INT a3,INT a4,INT a5,INT a6,HWND a7,HINSTANCE a8,LPARAM a9)
	{
		HWND ret_handle=OldCreateMDIWindowA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
		cleanup_callback_ret s=CreateWindowCallbackA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateMDIWindowA,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateMDIWindowA,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	HWND WINAPI MyCreateMDIWindowW(wchar_t const * a0,wchar_t const * a1,DWORD a2,INT a3,INT a4,INT a5,INT a6,HWND a7,HINSTANCE a8,LPARAM a9)
	{
		HWND ret_handle=OldCreateMDIWindowW(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
		cleanup_callback_ret s=CreateWindowCallbackW(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
		// perform allocation tasks
		if(0!=ret_handle && s.FunctionTag)
		{
			DWORD stack[32];
			ULONG traceHash=0;
			SHORT frames=pfnCaptureStackBackTrace(1, 32, reinterpret_cast<PVOID*>(&stack[0]), &traceHash);
			FILETIME ts={0,0};
			::GetSystemTimeAsFileTime(&ts);
			allocations[ret_handle]=allocation(traceHash,ts);
			if(!traceHash)	// has not been properly calculated; must be a shorty stack!
			{
				qqDebug("**********************");
				qqDebug("Empty trace hash retrieved - call stack contains FPO-optimized functions! Stack trace record will not recorded!!");
				qqDebug("**********************");
			}
			else
			{
				stackdb_type::iterator it=stackdb.find(traceHash);
				if(it==stackdb.end())	// no matching stack record exists
				{
					handle_tag htag=GetHandleTag(Function_CreateMDIWindowW,ret_handle);
					stackdb[traceHash]=stack_trace(Function_CreateMDIWindowW,s.ctag,htag,stack,frames);
				}
				else
				{
					it->second.allocationCount_ += 1;	// update allocation counter
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDeleteObject(HGDIOBJ a0)
	{
		BOOL ret_handle=OldDeleteObject(a0);
		// perform cleanup tasks
		{
			HGDIOBJ handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DeleteObject != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DeleteObject(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDeleteDC(HDC a0)
	{
		BOOL ret_handle=OldDeleteDC(a0);
		// perform cleanup tasks
		{
			HDC handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DeleteDC != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DeleteDC(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDeleteMetaFile(HMETAFILE a0)
	{
		BOOL ret_handle=OldDeleteMetaFile(a0);
		// perform cleanup tasks
		{
			HMETAFILE handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DeleteMetaFile != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DeleteMetaFile(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDeleteEnhMetaFile(HENHMETAFILE a0)
	{
		BOOL ret_handle=OldDeleteEnhMetaFile(a0);
		// perform cleanup tasks
		{
			HENHMETAFILE handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DeleteEnhMetaFile != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DeleteEnhMetaFile(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	INT WINAPI MyReleaseDC(HWND a0,HDC a1)
	{
		INT ret_handle=OldReleaseDC(a0,a1);
		// perform cleanup tasks
		{
			HDC handle=a1;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_ReleaseDC != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: ReleaseDC(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDestroyIcon(HICON a0)
	{
		BOOL ret_handle=OldDestroyIcon(a0);
		// perform cleanup tasks
		{
			HICON handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DestroyIcon != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DestroyIcon(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDestroyCursor(HCURSOR a0)
	{
		BOOL ret_handle=OldDestroyCursor(a0);
		// perform cleanup tasks
		{
			HCURSOR handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DestroyCursor != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DestroyCursor(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDdeDisconnect(HCONV a0)
	{
		BOOL ret_handle=OldDdeDisconnect(a0);
		// perform cleanup tasks
		{
			HCONV handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DdeDisconnect != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DdeDisconnect(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDdeDisconnectList(HCONVLIST a0)
	{
		BOOL ret_handle=OldDdeDisconnectList(a0);
		// perform cleanup tasks
		{
			HCONVLIST handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DdeDisconnectList != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DdeDisconnectList(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDestroyWindow(HWND a0)
	{
		BOOL ret_handle=OldDestroyWindow(a0);
		// perform cleanup tasks
		{
			HWND handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DestroyWindow != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DestroyWindow(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyEndDeferWindowPos(HDWP a0)
	{
		BOOL ret_handle=OldEndDeferWindowPos(a0);
		// perform cleanup tasks
		{
			HDWP handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_EndDeferWindowPos != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: EndDeferWindowPos(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDestroyAcceleratorTable(HACCEL a0)
	{
		BOOL ret_handle=OldDestroyAcceleratorTable(a0);
		// perform cleanup tasks
		{
			HACCEL handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DestroyAcceleratorTable != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DestroyAcceleratorTable(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyUnhookWindowsHookEx(HHOOK a0)
	{
		BOOL ret_handle=OldUnhookWindowsHookEx(a0);
		// perform cleanup tasks
		{
			HHOOK handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_UnhookWindowsHookEx != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: UnhookWindowsHookEx(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	BOOL WINAPI MyDestroyMenu(HMENU a0)
	{
		BOOL ret_handle=OldDestroyMenu(a0);
		// perform cleanup tasks
		{
			HMENU handle=a0;
			allocations_type::iterator it=allocations.find(handle);
			if(it != allocations.end())	// matched an allocation
			{
				stackdb_type::iterator sit=stackdb.find(it->second.hash_);
				if(sit != stackdb.end())
				{
					// look up corresponding stack trace record and remove it
					sit->second.allocationCount_ -= 1;	// update allocation counter
					// check allocation/cleanup consistency
					if(Function_DestroyMenu != sit->second.ctag_)
					{
						qqDebug("**********************");
						qqDebug("* Inconsistent cleanup: DestroyMenu(0x%lx) invoked for handle created with '%s' (expected %s()), trace=0x%lx", handle, From(sit->second.ftag_), From(sit->second.ctag_), it->second.hash_);
						qqDebug("**********************");
						if(::IsDebuggerPresent()) ::DebugBreak();
					}
					allocations.erase(it); // dump this allocation
				}
				else
				{
					// no allocations recorded for this cleanup
					qqDebug("**********************");
					qqDebug("* Inconsistent cleanup: no allocation record for handle 0x%lx!",handle);
					qqDebug("**********************");
				}
			}
		}
		return ret_handle;
	}
	bool DetourAll()
	{
		// initialize stack capture routine
		pfnCaptureStackBackTrace = 
			(USHORT (WINAPI *)(ULONG, ULONG, PVOID*, PULONG))::GetProcAddress(
				::GetModuleHandleW(L"ntdll.dll"), 
				"RtlCaptureStackBackTrace"
			);
		if(!pfnCaptureStackBackTrace)
		{
			qqDebug("RtlCaptureStackBackTrace not available!");
			return false;
		}
		DetourTransactionBegin();
		DetourUpdateThread(::GetCurrentThread());
		BOOL bOk=TRUE;
		LONG Status=0L;
		Status=DetourAttach(&(PVOID&)OldLoadBitmapA,MyLoadBitmapA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadBitmap'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadBitmapW,MyLoadBitmapW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadBitmap'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateBitmap,MyCreateBitmap);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateBitmap'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateBitmapIndirect,MyCreateBitmapIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateBitmapIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateCompatibleBitmap,MyCreateCompatibleBitmap);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateCompatibleBitmap'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDiscardableBitmap,MyCreateDiscardableBitmap);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDiscardableBitmap'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDIBitmap,MyCreateDIBitmap);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDIBitmap'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDIBSection,MyCreateDIBSection);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDIBSection'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateBrushIndirect,MyCreateBrushIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateBrushIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateSolidBrush,MyCreateSolidBrush);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateSolidBrush'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePatternBrush,MyCreatePatternBrush);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePatternBrush'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDIBPatternBrush,MyCreateDIBPatternBrush);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDIBPatternBrush'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDIBPatternBrushPt,MyCreateDIBPatternBrushPt);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDIBPatternBrushPt'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateHatchBrush,MyCreateHatchBrush);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateHatchBrush'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateFontA,MyCreateFontA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateFont'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateFontW,MyCreateFontW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateFont'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateFontIndirectA,MyCreateFontIndirectA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateFontIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateFontIndirectW,MyCreateFontIndirectW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateFontIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePen,MyCreatePen);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePen'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePenIndirect,MyCreatePenIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePenIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldExtCreatePen,MyExtCreatePen);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'ExtCreatePen'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldPathToRegion,MyPathToRegion);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'PathToRegion'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateEllipticRgn,MyCreateEllipticRgn);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateEllipticRgn'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateEllipticRgnIndirect,MyCreateEllipticRgnIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateEllipticRgnIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePolygonRgn,MyCreatePolygonRgn);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePolygonRgn'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePolyPolygonRgn,MyCreatePolyPolygonRgn);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePolyPolygonRgn'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateRectRgn,MyCreateRectRgn);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateRectRgn'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateRectRgnIndirect,MyCreateRectRgnIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateRectRgnIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateRoundRectRgn,MyCreateRoundRectRgn);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateRoundRectRgn'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldExtCreateRegion,MyExtCreateRegion);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'ExtCreateRegion'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateHalftonePalette,MyCreateHalftonePalette);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateHalftonePalette'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePalette,MyCreatePalette);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePalette'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldGetWindowDC,MyGetWindowDC);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'GetWindowDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateCompatibleDC,MyCreateCompatibleDC);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateCompatibleDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDCA,MyCreateDCA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDCW,MyCreateDCW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateICA,MyCreateICA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateIC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateICW,MyCreateICW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateIC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateMetaFileA,MyCreateMetaFileA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateMetaFileW,MyCreateMetaFileW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateEnhMetaFileA,MyCreateEnhMetaFileA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateEnhMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateEnhMetaFileW,MyCreateEnhMetaFileW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateEnhMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldGetEnhMetaFileA,MyGetEnhMetaFileA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'GetEnhMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldGetEnhMetaFileW,MyGetEnhMetaFileW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'GetEnhMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCloseEnhMetaFile,MyCloseEnhMetaFile);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CloseEnhMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCloseMetaFile,MyCloseMetaFile);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CloseMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateIconFromResourceEx,MyCreateIconFromResourceEx);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateIconFromResourceEx'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateIconIndirect,MyCreateIconIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateIconIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCopyIcon,MyCopyIcon);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CopyIcon'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldExtractIconA,MyExtractIconA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'ExtractIcon'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldExtractIconW,MyExtractIconW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'ExtractIcon'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCopyImage,MyCopyImage);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CopyImage'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadImageA,MyLoadImageA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadImage'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadImageW,MyLoadImageW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadImage'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldGetDCEx,MyGetDCEx);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'GetDCEx'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldGetDC,MyGetDC);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'GetDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDrawStateA,MyDrawStateA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DrawState'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDrawStateW,MyDrawStateW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DrawState'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadCursorA,MyLoadCursorA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadCursor'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadCursorW,MyLoadCursorW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadCursor'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadCursorFromFileA,MyLoadCursorFromFileA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadCursorFromFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadCursorFromFileW,MyLoadCursorFromFileW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadCursorFromFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadIconA,MyLoadIconA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadIcon'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadIconW,MyLoadIconW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadIcon'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateAcceleratorTableA,MyCreateAcceleratorTableA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateAcceleratorTable'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateAcceleratorTableW,MyCreateAcceleratorTableW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateAcceleratorTable'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateCursor,MyCreateCursor);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateCursor'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDdeConnect,MyDdeConnect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DdeConnect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDdeConnectList,MyDdeConnectList);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DdeConnectList'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldSetWindowsHookEx,MySetWindowsHookEx);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'SetWindowsHookEx'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateMenu,MyCreateMenu);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateMenu'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreatePopupMenu,MyCreatePopupMenu);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreatePopupMenu'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadMenuA,MyLoadMenuA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadMenu'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadMenuW,MyLoadMenuW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadMenu'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldLoadMenuIndirect,MyLoadMenuIndirect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'LoadMenuIndirect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldBeginDeferWindowPos,MyBeginDeferWindowPos);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'BeginDeferWindowPos'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateWindowExA,MyCreateWindowExA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateWindowEx'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateWindowExW,MyCreateWindowExW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateWindowEx'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDialogParamA,MyCreateDialogParamA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDialogParam'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDialogParamW,MyCreateDialogParamW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDialogParam'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDialogIndirectParamA,MyCreateDialogIndirectParamA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDialogIndirectParam'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateDialogIndirectParamW,MyCreateDialogIndirectParamW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateDialogIndirectParam'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateMDIWindowA,MyCreateMDIWindowA);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateMDIWindow'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldCreateMDIWindowW,MyCreateMDIWindowW);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'CreateMDIWindow'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDeleteObject,MyDeleteObject);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DeleteObject'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDeleteDC,MyDeleteDC);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DeleteDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDeleteMetaFile,MyDeleteMetaFile);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DeleteMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDeleteEnhMetaFile,MyDeleteEnhMetaFile);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DeleteEnhMetaFile'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldReleaseDC,MyReleaseDC);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'ReleaseDC'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDestroyIcon,MyDestroyIcon);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DestroyIcon'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDestroyCursor,MyDestroyCursor);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DestroyCursor'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDdeDisconnect,MyDdeDisconnect);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DdeDisconnect'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDdeDisconnectList,MyDdeDisconnectList);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DdeDisconnectList'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDestroyWindow,MyDestroyWindow);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DestroyWindow'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldEndDeferWindowPos,MyEndDeferWindowPos);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'EndDeferWindowPos'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDestroyAcceleratorTable,MyDestroyAcceleratorTable);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DestroyAcceleratorTable'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldUnhookWindowsHookEx,MyUnhookWindowsHookEx);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'UnhookWindowsHookEx'");
			bOk=FALSE;
			goto Epilog;
		}
		Status=DetourAttach(&(PVOID&)OldDestroyMenu,MyDestroyMenu);
		if(NO_ERROR!=Status)
		{
			qqDebug("failed to detour 'DestroyMenu'");
			bOk=FALSE;
			goto Epilog;
		}
Epilog:
		if(bOk)
		{
			DetourTransactionCommit();
			return true;
		}
		else
		{
			qqDebug("DetourAll() failed to detour one (or more) functions");
			DetourTransactionAbort();
			return false;
		}
	}
	void UndetourAll()
	{
		DetourTransactionBegin();
		DetourUpdateThread(::GetCurrentThread());
		DetourDetach(&(PVOID&)OldLoadBitmapA,MyLoadBitmapA);
		DetourDetach(&(PVOID&)OldLoadBitmapW,MyLoadBitmapW);
		DetourDetach(&(PVOID&)OldCreateBitmap,MyCreateBitmap);
		DetourDetach(&(PVOID&)OldCreateBitmapIndirect,MyCreateBitmapIndirect);
		DetourDetach(&(PVOID&)OldCreateCompatibleBitmap,MyCreateCompatibleBitmap);
		DetourDetach(&(PVOID&)OldCreateDiscardableBitmap,MyCreateDiscardableBitmap);
		DetourDetach(&(PVOID&)OldCreateDIBitmap,MyCreateDIBitmap);
		DetourDetach(&(PVOID&)OldCreateDIBSection,MyCreateDIBSection);
		DetourDetach(&(PVOID&)OldCreateBrushIndirect,MyCreateBrushIndirect);
		DetourDetach(&(PVOID&)OldCreateSolidBrush,MyCreateSolidBrush);
		DetourDetach(&(PVOID&)OldCreatePatternBrush,MyCreatePatternBrush);
		DetourDetach(&(PVOID&)OldCreateDIBPatternBrush,MyCreateDIBPatternBrush);
		DetourDetach(&(PVOID&)OldCreateDIBPatternBrushPt,MyCreateDIBPatternBrushPt);
		DetourDetach(&(PVOID&)OldCreateHatchBrush,MyCreateHatchBrush);
		DetourDetach(&(PVOID&)OldCreateFontA,MyCreateFontA);
		DetourDetach(&(PVOID&)OldCreateFontW,MyCreateFontW);
		DetourDetach(&(PVOID&)OldCreateFontIndirectA,MyCreateFontIndirectA);
		DetourDetach(&(PVOID&)OldCreateFontIndirectW,MyCreateFontIndirectW);
		DetourDetach(&(PVOID&)OldCreatePen,MyCreatePen);
		DetourDetach(&(PVOID&)OldCreatePenIndirect,MyCreatePenIndirect);
		DetourDetach(&(PVOID&)OldExtCreatePen,MyExtCreatePen);
		DetourDetach(&(PVOID&)OldPathToRegion,MyPathToRegion);
		DetourDetach(&(PVOID&)OldCreateEllipticRgn,MyCreateEllipticRgn);
		DetourDetach(&(PVOID&)OldCreateEllipticRgnIndirect,MyCreateEllipticRgnIndirect);
		DetourDetach(&(PVOID&)OldCreatePolygonRgn,MyCreatePolygonRgn);
		DetourDetach(&(PVOID&)OldCreatePolyPolygonRgn,MyCreatePolyPolygonRgn);
		DetourDetach(&(PVOID&)OldCreateRectRgn,MyCreateRectRgn);
		DetourDetach(&(PVOID&)OldCreateRectRgnIndirect,MyCreateRectRgnIndirect);
		DetourDetach(&(PVOID&)OldCreateRoundRectRgn,MyCreateRoundRectRgn);
		DetourDetach(&(PVOID&)OldExtCreateRegion,MyExtCreateRegion);
		DetourDetach(&(PVOID&)OldCreateHalftonePalette,MyCreateHalftonePalette);
		DetourDetach(&(PVOID&)OldCreatePalette,MyCreatePalette);
		DetourDetach(&(PVOID&)OldGetWindowDC,MyGetWindowDC);
		DetourDetach(&(PVOID&)OldCreateCompatibleDC,MyCreateCompatibleDC);
		DetourDetach(&(PVOID&)OldCreateDCA,MyCreateDCA);
		DetourDetach(&(PVOID&)OldCreateDCW,MyCreateDCW);
		DetourDetach(&(PVOID&)OldCreateICA,MyCreateICA);
		DetourDetach(&(PVOID&)OldCreateICW,MyCreateICW);
		DetourDetach(&(PVOID&)OldCreateMetaFileA,MyCreateMetaFileA);
		DetourDetach(&(PVOID&)OldCreateMetaFileW,MyCreateMetaFileW);
		DetourDetach(&(PVOID&)OldCreateEnhMetaFileA,MyCreateEnhMetaFileA);
		DetourDetach(&(PVOID&)OldCreateEnhMetaFileW,MyCreateEnhMetaFileW);
		DetourDetach(&(PVOID&)OldGetEnhMetaFileA,MyGetEnhMetaFileA);
		DetourDetach(&(PVOID&)OldGetEnhMetaFileW,MyGetEnhMetaFileW);
		DetourDetach(&(PVOID&)OldCloseEnhMetaFile,MyCloseEnhMetaFile);
		DetourDetach(&(PVOID&)OldCloseMetaFile,MyCloseMetaFile);
		DetourDetach(&(PVOID&)OldCreateIconFromResourceEx,MyCreateIconFromResourceEx);
		DetourDetach(&(PVOID&)OldCreateIconIndirect,MyCreateIconIndirect);
		DetourDetach(&(PVOID&)OldCopyIcon,MyCopyIcon);
		DetourDetach(&(PVOID&)OldExtractIconA,MyExtractIconA);
		DetourDetach(&(PVOID&)OldExtractIconW,MyExtractIconW);
		DetourDetach(&(PVOID&)OldCopyImage,MyCopyImage);
		DetourDetach(&(PVOID&)OldLoadImageA,MyLoadImageA);
		DetourDetach(&(PVOID&)OldLoadImageW,MyLoadImageW);
		DetourDetach(&(PVOID&)OldGetDCEx,MyGetDCEx);
		DetourDetach(&(PVOID&)OldGetDC,MyGetDC);
		DetourDetach(&(PVOID&)OldDrawStateA,MyDrawStateA);
		DetourDetach(&(PVOID&)OldDrawStateW,MyDrawStateW);
		DetourDetach(&(PVOID&)OldLoadCursorA,MyLoadCursorA);
		DetourDetach(&(PVOID&)OldLoadCursorW,MyLoadCursorW);
		DetourDetach(&(PVOID&)OldLoadCursorFromFileA,MyLoadCursorFromFileA);
		DetourDetach(&(PVOID&)OldLoadCursorFromFileW,MyLoadCursorFromFileW);
		DetourDetach(&(PVOID&)OldLoadIconA,MyLoadIconA);
		DetourDetach(&(PVOID&)OldLoadIconW,MyLoadIconW);
		DetourDetach(&(PVOID&)OldCreateAcceleratorTableA,MyCreateAcceleratorTableA);
		DetourDetach(&(PVOID&)OldCreateAcceleratorTableW,MyCreateAcceleratorTableW);
		DetourDetach(&(PVOID&)OldCreateCursor,MyCreateCursor);
		DetourDetach(&(PVOID&)OldDdeConnect,MyDdeConnect);
		DetourDetach(&(PVOID&)OldDdeConnectList,MyDdeConnectList);
		DetourDetach(&(PVOID&)OldSetWindowsHookEx,MySetWindowsHookEx);
		DetourDetach(&(PVOID&)OldCreateMenu,MyCreateMenu);
		DetourDetach(&(PVOID&)OldCreatePopupMenu,MyCreatePopupMenu);
		DetourDetach(&(PVOID&)OldLoadMenuA,MyLoadMenuA);
		DetourDetach(&(PVOID&)OldLoadMenuW,MyLoadMenuW);
		DetourDetach(&(PVOID&)OldLoadMenuIndirect,MyLoadMenuIndirect);
		DetourDetach(&(PVOID&)OldBeginDeferWindowPos,MyBeginDeferWindowPos);
		DetourDetach(&(PVOID&)OldCreateWindowExA,MyCreateWindowExA);
		DetourDetach(&(PVOID&)OldCreateWindowExW,MyCreateWindowExW);
		DetourDetach(&(PVOID&)OldCreateDialogParamA,MyCreateDialogParamA);
		DetourDetach(&(PVOID&)OldCreateDialogParamW,MyCreateDialogParamW);
		DetourDetach(&(PVOID&)OldCreateDialogIndirectParamA,MyCreateDialogIndirectParamA);
		DetourDetach(&(PVOID&)OldCreateDialogIndirectParamW,MyCreateDialogIndirectParamW);
		DetourDetach(&(PVOID&)OldCreateMDIWindowA,MyCreateMDIWindowA);
		DetourDetach(&(PVOID&)OldCreateMDIWindowW,MyCreateMDIWindowW);
		DetourDetach(&(PVOID&)OldDeleteObject,MyDeleteObject);
		DetourDetach(&(PVOID&)OldDeleteDC,MyDeleteDC);
		DetourDetach(&(PVOID&)OldDeleteMetaFile,MyDeleteMetaFile);
		DetourDetach(&(PVOID&)OldDeleteEnhMetaFile,MyDeleteEnhMetaFile);
		DetourDetach(&(PVOID&)OldReleaseDC,MyReleaseDC);
		DetourDetach(&(PVOID&)OldDestroyIcon,MyDestroyIcon);
		DetourDetach(&(PVOID&)OldDestroyCursor,MyDestroyCursor);
		DetourDetach(&(PVOID&)OldDdeDisconnect,MyDdeDisconnect);
		DetourDetach(&(PVOID&)OldDdeDisconnectList,MyDdeDisconnectList);
		DetourDetach(&(PVOID&)OldDestroyWindow,MyDestroyWindow);
		DetourDetach(&(PVOID&)OldEndDeferWindowPos,MyEndDeferWindowPos);
		DetourDetach(&(PVOID&)OldDestroyAcceleratorTable,MyDestroyAcceleratorTable);
		DetourDetach(&(PVOID&)OldUnhookWindowsHookEx,MyUnhookWindowsHookEx);
		DetourDetach(&(PVOID&)OldDestroyMenu,MyDestroyMenu);
		DetourTransactionCommit();
	}
} // namespace detail

// Intercepts.cpp

