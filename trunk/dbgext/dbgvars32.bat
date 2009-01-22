@echo off

if NOT "%DDKBUILDENV%"=="" goto :ContinueWithSetEnv

CALL "D:\WinDDK\6001.17051\bin\setenv.bat" D:\WinDDK\6001.17051 fre wxp

:ContinueWithSetEnv
rem Replace WDBG_PATH with path to your Windows Debugging Tools installation folder!
SET WDBG_PATH=d:\Programs\windbg
SET DBGSDK_INC_PATH=%WDBG_PATH%\sdk\inc
SET DBGSDK_LIB_PATH=%WDBG_PATH%\sdk\lib
SET DBGLIB_LIB_PATH=%WDBG_PATH%\sdk\lib
