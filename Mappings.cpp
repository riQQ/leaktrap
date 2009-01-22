/*
	mappings.cpp - various mapping helpers

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
#include "Intercepts.h"


namespace detail
{
	char const *From(function_tag tag)
	{
		switch(tag)
		{
			case Function_LoadBitmapA:
			{
				return "LoadBitmapA";
			}
			case Function_LoadBitmapW:
			{
				return "LoadBitmapW";
			}
			case Function_CreateBitmap:
			{
				return "CreateBitmap";
			}
			case Function_CreateBitmapIndirect:
			{
				return "CreateBitmapIndirect";
			}
			case Function_CreateCompatibleBitmap:
			{
				return "CreateCompatibleBitmap";
			}
			case Function_CreateDiscardableBitmap:
			{
				return "CreateDiscardableBitmap";
			}
			case Function_CreateDIBitmap:
			{
				return "CreateDIBitmap";
			}
			case Function_CreateDIBSection:
			{
				return "CreateDIBSection";
			}
			case Function_CreateBrushIndirect:
			{
				return "CreateBrushIndirect";
			}
			case Function_CreateSolidBrush:
			{
				return "CreateSolidBrush";
			}
			case Function_CreatePatternBrush:
			{
				return "CreatePatternBrush";
			}
			case Function_CreateDIBPatternBrush:
			{
				return "CreateDIBPatternBrush";
			}
			case Function_CreateDIBPatternBrushPt:
			{
				return "CreateDIBPatternBrushPt";
			}
			case Function_CreateHatchBrush:
			{
				return "CreateHatchBrush";
			}
			case Function_CreateFontA:
			{
				return "CreateFontA";
			}
			case Function_CreateFontW:
			{
				return "CreateFontW";
			}
			case Function_CreateFontIndirectA:
			{
				return "CreateFontIndirectA";
			}
			case Function_CreateFontIndirectW:
			{
				return "CreateFontIndirectW";
			}
			case Function_CreatePen:
			{
				return "CreatePen";
			}
			case Function_CreatePenIndirect:
			{
				return "CreatePenIndirect";
			}
			case Function_ExtCreatePen:
			{
				return "ExtCreatePen";
			}
			case Function_PathToRegion:
			{
				return "PathToRegion";
			}
			case Function_CreateEllipticRgn:
			{
				return "CreateEllipticRgn";
			}
			case Function_CreateEllipticRgnIndirect:
			{
				return "CreateEllipticRgnIndirect";
			}
			case Function_CreatePolygonRgn:
			{
				return "CreatePolygonRgn";
			}
			case Function_CreatePolyPolygonRgn:
			{
				return "CreatePolyPolygonRgn";
			}
			case Function_CreateRectRgn:
			{
				return "CreateRectRgn";
			}
			case Function_CreateRectRgnIndirect:
			{
				return "CreateRectRgnIndirect";
			}
			case Function_CreateRoundRectRgn:
			{
				return "CreateRoundRectRgn";
			}
			case Function_ExtCreateRegion:
			{
				return "ExtCreateRegion";
			}
			case Function_CreateHalftonePalette:
			{
				return "CreateHalftonePalette";
			}
			case Function_CreatePalette:
			{
				return "CreatePalette";
			}
			case Function_GetWindowDC:
			{
				return "GetWindowDC";
			}
			case Function_CreateCompatibleDC:
			{
				return "CreateCompatibleDC";
			}
			case Function_CreateDCA:
			{
				return "CreateDCA";
			}
			case Function_CreateDCW:
			{
				return "CreateDCW";
			}
			case Function_CreateICA:
			{
				return "CreateICA";
			}
			case Function_CreateICW:
			{
				return "CreateICW";
			}
			case Function_CreateMetaFileA:
			{
				return "CreateMetaFileA";
			}
			case Function_CreateMetaFileW:
			{
				return "CreateMetaFileW";
			}
			case Function_CreateEnhMetaFileA:
			{
				return "CreateEnhMetaFileA";
			}
			case Function_CreateEnhMetaFileW:
			{
				return "CreateEnhMetaFileW";
			}
			case Function_GetEnhMetaFileA:
			{
				return "GetEnhMetaFileA";
			}
			case Function_GetEnhMetaFileW:
			{
				return "GetEnhMetaFileW";
			}
			case Function_CloseEnhMetaFile:
			{
				return "CloseEnhMetaFile";
			}
			case Function_CloseMetaFile:
			{
				return "CloseMetaFile";
			}
			case Function_CreateIconFromResourceEx:
			{
				return "CreateIconFromResourceEx";
			}
			case Function_CreateIconIndirect:
			{
				return "CreateIconIndirect";
			}
			case Function_CopyIcon:
			{
				return "CopyIcon";
			}
			case Function_ExtractIconA:
			{
				return "ExtractIconA";
			}
			case Function_ExtractIconW:
			{
				return "ExtractIconW";
			}
			case Function_CopyImage:
			{
				return "CopyImage";
			}
			case Function_LoadImageA:
			{
				return "LoadImageA";
			}
			case Function_LoadImageW:
			{
				return "LoadImageW";
			}
			case Function_GetDCEx:
			{
				return "GetDCEx";
			}
			case Function_GetDC:
			{
				return "GetDC";
			}
			case Function_DrawStateA:
			{
				return "DrawStateA";
			}
			case Function_DrawStateW:
			{
				return "DrawStateW";
			}
			case Function_LoadCursorA:
			{
				return "LoadCursorA";
			}
			case Function_LoadCursorW:
			{
				return "LoadCursorW";
			}
			case Function_LoadCursorFromFileA:
			{
				return "LoadCursorFromFileA";
			}
			case Function_LoadCursorFromFileW:
			{
				return "LoadCursorFromFileW";
			}
			case Function_LoadIconA:
			{
				return "LoadIconA";
			}
			case Function_LoadIconW:
			{
				return "LoadIconW";
			}
			case Function_CreateAcceleratorTableA:
			{
				return "CreateAcceleratorTableA";
			}
			case Function_CreateAcceleratorTableW:
			{
				return "CreateAcceleratorTableW";
			}
			case Function_CreateCursor:
			{
				return "CreateCursor";
			}
			case Function_DdeConnect:
			{
				return "DdeConnect";
			}
			case Function_DdeConnectList:
			{
				return "DdeConnectList";
			}
			case Function_SetWindowsHookEx:
			{
				return "SetWindowsHookEx";
			}
			case Function_CreateMenu:
			{
				return "CreateMenu";
			}
			case Function_CreatePopupMenu:
			{
				return "CreatePopupMenu";
			}
			case Function_LoadMenuA:
			{
				return "LoadMenuA";
			}
			case Function_LoadMenuW:
			{
				return "LoadMenuW";
			}
			case Function_LoadMenuIndirect:
			{
				return "LoadMenuIndirect";
			}
			case Function_BeginDeferWindowPos:
			{
				return "BeginDeferWindowPos";
			}
			case Function_CreateWindowExA:
			{
				return "CreateWindowExA";
			}
			case Function_CreateWindowExW:
			{
				return "CreateWindowExW";
			}
			case Function_CreateDialogParamA:
			{
				return "CreateDialogParamA";
			}
			case Function_CreateDialogParamW:
			{
				return "CreateDialogParamW";
			}
			case Function_CreateDialogIndirectParamA:
			{
				return "CreateDialogIndirectParamA";
			}
			case Function_CreateDialogIndirectParamW:
			{
				return "CreateDialogIndirectParamW";
			}
			case Function_CreateMDIWindowA:
			{
				return "CreateMDIWindowA";
			}
			case Function_CreateMDIWindowW:
			{
				return "CreateMDIWindowW";
			}
			case Function_DeleteObject:
			{
				return "DeleteObject";
			}
			case Function_DeleteDC:
			{
				return "DeleteDC";
			}
			case Function_DeleteMetaFile:
			{
				return "DeleteMetaFile";
			}
			case Function_DeleteEnhMetaFile:
			{
				return "DeleteEnhMetaFile";
			}
			case Function_ReleaseDC:
			{
				return "ReleaseDC";
			}
			case Function_DestroyIcon:
			{
				return "DestroyIcon";
			}
			case Function_DestroyCursor:
			{
				return "DestroyCursor";
			}
			case Function_DdeDisconnect:
			{
				return "DdeDisconnect";
			}
			case Function_DdeDisconnectList:
			{
				return "DdeDisconnectList";
			}
			case Function_DestroyWindow:
			{
				return "DestroyWindow";
			}
			case Function_EndDeferWindowPos:
			{
				return "EndDeferWindowPos";
			}
			case Function_DestroyAcceleratorTable:
			{
				return "DestroyAcceleratorTable";
			}
			case Function_UnhookWindowsHookEx:
			{
				return "UnhookWindowsHookEx";
			}
			case Function_DestroyMenu:
			{
				return "DestroyMenu";
			}
			case Function_Undefined:
			default:
			{
				return "";
			}
		}
	}
	char const *From(handle_tag tag)
	{
		switch(tag)
		{
			case Handle_PEN:
			{
				return "PEN";
			}
			case Handle_BRUSH:
			{
				return "BRUSH";
			}
			case Handle_DC:
			{
				return "DC";
			}
			case Handle_METADC:
			{
				return "METADC";
			}
			case Handle_PAL:
			{
				return "PAL";
			}
			case Handle_FONT:
			{
				return "FONT";
			}
			case Handle_BITMAP:
			{
				return "BITMAP";
			}
			case Handle_REGION:
			{
				return "REGION";
			}
			case Handle_METAFILE:
			{
				return "METAFILE";
			}
			case Handle_MEMDC:
			{
				return "MEMDC";
			}
			case Handle_EXTPEN:
			{
				return "EXTPEN";
			}
			case Handle_ENHMETADC:
			{
				return "ENHMETADC";
			}
			case Handle_ENHMETAFILE:
			{
				return "ENHMETAFILE";
			}
			case Handle_COLORSPACE:
			{
				return "COLORSPACE";
			}
			case Handle_ACCEL:
			{
				return "ACCEL";
			}
			case Handle_CARET:
			{
				return "CARET";
			}
			case Handle_CURSOR:
			{
				return "CURSOR";
			}
			case Handle_CONV:
			{
				return "CONV";
			}
			case Handle_CONVLIST:
			{
				return "CONVLIST";
			}
			case Handle_WNDHOOK:
			{
				return "WNDHOOK";
			}
			case Handle_ICON:
			{
				return "ICON";
			}
			case Handle_MENU:
			{
				return "MENU";
			}
			case Handle_WINDOW:
			{
				return "WINDOW";
			}
			case Handle_DWP:
			{
				return "DWP";
			}
			case Handle_Undefined:
			default:
			{
				return "";
			}
		}
	}
	bool IsGdiHandle(handle_tag tag)
	{
		switch(tag)
		{
				// fall throughs
			case Handle_PEN:
			case Handle_BRUSH:
			case Handle_DC:
			case Handle_METADC:
			case Handle_PAL:
			case Handle_FONT:
			case Handle_BITMAP:
			case Handle_REGION:
			case Handle_METAFILE:
			case Handle_MEMDC:
			case Handle_EXTPEN:
			case Handle_ENHMETADC:
			case Handle_ENHMETAFILE:
			case Handle_COLORSPACE:
			{
				return true;
			}
				// fall throughs
			case Handle_ACCEL:
			case Handle_CARET:
			case Handle_CURSOR:
			case Handle_CONV:
			case Handle_CONVLIST:
			case Handle_WNDHOOK:
			case Handle_ICON:
			case Handle_MENU:
			case Handle_WINDOW:
			case Handle_DWP:
			case Handle_Undefined:
			default:
			{
				return false;
			}
		}
	}
} // namespace detail

// Mappings.cpp

