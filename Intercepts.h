/*
	intercepts.h - allocation/cleanup interception

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
	Generated on Donnerstag, 14. Oktober 2010 11:08:05. Do not EDIT!!!
*/
#pragma once


namespace detail
{
	typedef enum
	{
		Function_Undefined=-1,
		Function_LoadBitmapA,
		Function_LoadBitmapW,
		Function_CreateBitmap,
		Function_CreateBitmapIndirect,
		Function_CreateCompatibleBitmap,
		Function_CreateDiscardableBitmap,
		Function_CreateDIBitmap,
		Function_CreateDIBSection,
		Function_CreateBrushIndirect,
		Function_CreateSolidBrush,
		Function_CreatePatternBrush,
		Function_CreateDIBPatternBrush,
		Function_CreateDIBPatternBrushPt,
		Function_CreateHatchBrush,
		Function_CreateFontA,
		Function_CreateFontW,
		Function_CreateFontIndirectA,
		Function_CreateFontIndirectW,
		Function_CreatePen,
		Function_CreatePenIndirect,
		Function_ExtCreatePen,
		Function_PathToRegion,
		Function_CreateEllipticRgn,
		Function_CreateEllipticRgnIndirect,
		Function_CreatePolygonRgn,
		Function_CreatePolyPolygonRgn,
		Function_CreateRectRgn,
		Function_CreateRectRgnIndirect,
		Function_CreateRoundRectRgn,
		Function_ExtCreateRegion,
		Function_CreateHalftonePalette,
		Function_CreatePalette,
		Function_GetWindowDC,
		Function_CreateCompatibleDC,
		Function_CreateDCA,
		Function_CreateDCW,
		Function_CreateICA,
		Function_CreateICW,
		Function_CreateMetaFileA,
		Function_CreateMetaFileW,
		Function_CreateEnhMetaFileA,
		Function_CreateEnhMetaFileW,
		Function_GetEnhMetaFileA,
		Function_GetEnhMetaFileW,
		Function_CloseEnhMetaFile,
		Function_CloseMetaFile,
		Function_CreateIconFromResourceEx,
		Function_CreateIconIndirect,
		Function_CopyIcon,
		Function_ExtractIconA,
		Function_ExtractIconW,
		Function_CopyImage,
		Function_LoadImageA,
		Function_LoadImageW,
		Function_GetDCEx,
		Function_GetDC,
		Function_DrawStateA,
		Function_DrawStateW,
		Function_LoadCursorA,
		Function_LoadCursorW,
		Function_LoadCursorFromFileA,
		Function_LoadCursorFromFileW,
		Function_LoadIconA,
		Function_LoadIconW,
		Function_CreateAcceleratorTableA,
		Function_CreateAcceleratorTableW,
		Function_CreateCursor,
		Function_DdeConnect,
		Function_DdeConnectList,
		Function_SetWindowsHookEx,
		Function_CreateMenu,
		Function_CreatePopupMenu,
		Function_LoadMenuA,
		Function_LoadMenuW,
		Function_LoadMenuIndirect,
		Function_BeginDeferWindowPos,
		Function_CreateWindowExA,
		Function_CreateWindowExW,
		Function_CreateDialogParamA,
		Function_CreateDialogParamW,
		Function_CreateDialogIndirectParamA,
		Function_CreateDialogIndirectParamW,
		Function_CreateMDIWindowA,
		Function_CreateMDIWindowW,
		Function_CreateDesktopA,
		Function_CreateDesktopW,
		Function_OpenDesktopA,
		Function_OpenDesktopW,
		Function_OpenInputDesktop,
		Function_CreateWindowStationA,
		Function_CreateWindowStationW,
		Function_OpenWindowStationA,
		Function_OpenWindowStationW,
		Function_DeleteObject,
		Function_DeleteDC,
		Function_DeleteMetaFile,
		Function_DeleteEnhMetaFile,
		Function_ReleaseDC,
		Function_DestroyIcon,
		Function_DestroyCursor,
		Function_DdeDisconnect,
		Function_DdeDisconnectList,
		Function_DestroyWindow,
		Function_EndDeferWindowPos,
		Function_DestroyAcceleratorTable,
		Function_UnhookWindowsHookEx,
		Function_DestroyMenu,
		Function_CloseDesktop,
		Function_CloseWindowStation,
	} function_tag;
	typedef enum
	{
		Handle_Undefined=-1,
		Handle_PEN=OBJ_PEN,
		Handle_BRUSH=OBJ_BRUSH,
		Handle_DC=OBJ_DC,
		Handle_METADC=OBJ_METADC,
		Handle_PAL=OBJ_PAL,
		Handle_FONT=OBJ_FONT,
		Handle_BITMAP=OBJ_BITMAP,
		Handle_REGION=OBJ_REGION,
		Handle_METAFILE=OBJ_METAFILE,
		Handle_MEMDC=OBJ_MEMDC,
		Handle_EXTPEN=OBJ_EXTPEN,
		Handle_ENHMETADC=OBJ_ENHMETADC,
		Handle_ENHMETAFILE=OBJ_ENHMETAFILE,
		Handle_COLORSPACE=OBJ_COLORSPACE,
		Handle_ACCEL=1000,
		Handle_CARET=1001,
		Handle_CURSOR=1002,
		Handle_CONV=1003,
		Handle_CONVLIST=1004,
		Handle_WNDHOOK=1005,
		Handle_ICON=1006,
		Handle_MENU=1007,
		Handle_WINDOW=1008,
		Handle_DWP=1009,
	} handle_tag;
	bool DetourAll();
	void UndetourAll();
	char const *From(function_tag);
	char const *From(handle_tag);
	handle_tag GetHandleTag(function_tag ftag, void* handle);
	bool IsGdiHandle(handle_tag);
} // namespace detail

// Intercepts.h

