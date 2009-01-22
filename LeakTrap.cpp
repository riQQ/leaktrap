/*
	leaktrap.cpp - main entry point

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
#include "LeakTrap.h"
#include "Intercepts.h"		// detail::DetourAll(), detail::UndetourAll()
#include "impl.h"

namespace
{
	void atexit_cleanup()
	{
		using namespace detail;
		if(LeaksAvail())
		{
			if(::IsDebuggerPresent()) ::DebugBreak();
		}
		UndetourAll();
	}
}

BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                       DWORD  ul_reason_for_call, 
                       LPVOID /*lpReserved*/
					 )
{
	using namespace detail;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			BOOL bRet=static_cast<BOOL>(DetourAll());
			if(bRet)
			{
				atexit(atexit_cleanup);
				bRet=init_tls();
			}

			return bRet;
		}
		//break;
	case DLL_THREAD_ATTACH:
		{
			// initialize tls slot for this thread
			init_tls_slot();
		}
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		release_tls();
		break;
	}
    return TRUE;
}

//Dummy export to fulfill ordinal #1 requirement
void __declspec(dllexport) DummyExport() {}
