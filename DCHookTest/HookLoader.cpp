#include <windows.h>
#pragma hdrstop
#include "stdafx.h"
#include "HookLoader.h"
#include "../DCHook64/DCHook64.h"
#include "Util.h"

#define	HOOK64_SUBDIR	0		// x64関連ファイルはsub directory x64へ

HWND hwnd64;

THookLoader::THookLoader(const char *dllname)
	:DllName(dllname)
{
	hDll = NULL;
	DCHInit = NULL;
	DCHConfig = NULL;
	DCHConfig2 = NULL;
	DCHCapture = NULL;
	DCHWaitForCaptureResult = NULL;
	DCHUninit = NULL;
#ifdef _DEBUG
	DCDebug = NULL;
#endif

	hThread = NULL;
	dwThreadId = 0;
}
THookLoader::~THookLoader()
{
	Uninit();
}
bool THookLoader::Load(HWND hwnd)
{
	if (hDll)
		return true;

	hDll = LoadLibrary( DllName );
	if ( !hDll )
	{
		GetLastError();
#ifndef _DEBUG
		DBW("Cannot load DLL:%s",DllName);
#endif
		// DKPPHK.DLL がなければ DCHOOK.DLLをload
		DllName = "DCHOOK.DLL";

		hDll = LoadLibrary( DllName );
		if ( !hDll ){
			DBW("Cannot load DLL:%s",DllName);
			return false;
		}
	}

	DCHInit = (FNDCHInit)GetProcAddress( hDll, "Init" );
	DCHUninit = (FNDCHUninit)GetProcAddress( hDll, "Uninit" );
	DCHConfig = (FNDCHConfig)GetProcAddress( hDll, "Config" );
	DCHConfig2 = (FNDCHConfig2)GetProcAddress( hDll, "Config2" );
	DCHCapture = (FNDCHCapture)GetProcAddress( hDll, "Capture" );
	DCHWaitForCaptureResult = (FNDCHWaitForCaptureResult)GetProcAddress( hDll, "WaitForCaptureResult" );
#ifdef _DEBUG
	DCDebug = (FNDebug)GetProcAddress( hDll, "Debug" );
#endif

	if (!DCHInit || !DCHUninit || !DCHConfig || !DCHConfig2 || !DCHCapture || !DCHWaitForCaptureResult){
		FreeLibrary(hDll);
		hDll = NULL;
		return false;
	}

#if 0
#ifdef USE_UNICODE
	const char *pVxdPath = NULL;
#else
	AnsiString VxdPath;
	if (!WindowsNT){
		// for Windows9x
		VxdPath = ExtractFilePath( Application->ExeName );
		char path[256];
		GetShortPathName( VxdPath.c_str(), path, sizeof(path) );
		VxdPath = path;
		VxdPath += "HK95D.VXD";
	}
	const char *pVxdPath = VxdPath.data();
#endif

	if ( !DCHInit( hwnd, DllName, WindowsNT, pVxdPath ) ){
		FreeLibrary( hDll );
		hDll = NULL;
		DBW("Cannot initialized DLL");
		return false;
	}
#endif

	return true;
}

void THookLoader::Unload()
{
	if (!hDll)
		return;
	DCHUninit();
	FreeLibrary(hDll);
	hDll = NULL;
	DCHUninit = NULL;
}

bool THookLoader::Init(HWND hwnd)
{
	DBW("Init");
	return DCHInit(hwnd, NULL, NULL, NULL);
}
void THookLoader::Uninit()
{
	if (DCHUninit) DCHUninit();
}

int THookLoader::Capture()
{
	if (!hDll)
		return 0;
	return DCHCapture(NULL, NULL, true, false);
}
int THookLoader::CaptureAsync()
{
	if (!hDll)
		return 0;
	int ret = DCHCapture(NULL, NULL, true, true);
	int result = DCHWaitForCaptureResult(false, 0);
	DBW("result:%d", result);
	if (result != 0){
		// wait dataあり
		OpenThread();
	}
	return ret;
}
int THookLoader::CaptureAsyncWait()
{
	return 0;
}

void THookLoader::OpenThread()
{
	DBW("OpenThread: %08X", hThread);
	if (hThread) return;

	ThreadEnd = false;
	hThread = CreateThread(NULL, 0, cbWaitCaptureThread, this, CREATE_SUSPENDED, &dwThreadId);
	ResumeThread(hThread);
}

DWORD WINAPI THookLoader::cbWaitCaptureThread(LPVOID This)
{
	((THookLoader*)This)->WaitCaptureThread();
	return 0;
}

void THookLoader::WaitCaptureThread()
{
	DCHWaitForCaptureResult(true, 3000);
	hThread = NULL;
}

static const char *MyWinTitle = "<Processing...in dchook>";
inline HANDLE __WinExec( const char *cmd, int show, const char *dir=NULL )
{
	return WinExecEx(cmd, show, dir, MyWinTitle);
}

//#ifdef _DEBUG
#if 0
const char *StrDCHK64EXE = "..\\dchk64\\x64\\Debug\\dchk64.exe";	// for debug
#else
#if HOOK64_SUBDIR
const char *StrDCHK64EXE = "x64\\DKPUHK64.exe";
#else
const char *StrDCHK64EXE = "DKPUHK64.exe";
#endif
#endif

THookLoader64::THookLoader64(HWND hwnd_recv)
	:hwndRecv(hwnd_recv)
{
	hwndTarget = NULL;
	hProc = NULL;
	LoadPending = false;
}
THookLoader64::~THookLoader64()
{
	if (hProc)
		CloseHandle(hProc);
	Unload();
}
bool THookLoader64::Load(HWND hwnd)
{
#if 1	// Find already launched hook executer.
	HWND hwndFound = FindWindow("dchk64", "DCHK64");
//	HWND hwndFound = FindApp("dchk64", "DCHK64", "");
	if (hwndFound){
		DBW("x64:Found %08X already launched exe", hwndFound);
		hwndTarget = hwndFound;
		hwnd64 = hwndTarget;
		return true;
	}
#endif
#if 1
	char param[280];
	wsprintf(param, "%s %08X", StrDCHK64EXE, hwndRecv);
	hProc = __WinExec(param, SW_HIDE, 0);	// Launch dchk64.exe
	if (!hProc){
		DBW("Launch failed: %s", param);
		return false;
	}
	LoadPending = true;
	//WaitForInputIdle(hProc, 3000);
#endif
	return true;
}
void THookLoader64::Unload()
{
	if (!hProc)
		return;
	Uninit();
	TerminateProcess(hProc, 0);
	CloseHandle(hProc);
	hProc = NULL;
	LoadPending = false;
}
bool THookLoader64::Init(HWND hwnd)
{
	DBW("x64:Init hwndTarget=%08X", hwndTarget);
	if (!hwndTarget){
		DBW("Init failed");
		return false;
	}
	DBW("x64:Init");
	HDCInitParam p = {hwnd};
	return Send(HDC_Init, &p, sizeof(p)) ? true : false;
}
void THookLoader64::Uninit( )
{
	if (!hwndTarget)
		return ;
	Send(HDC_Uninit, 0);
	LoadPending = false;
}
int THookLoader64::Config( int clickonly, int keyaction, int keyflag )
{
	if (!hwndTarget)
		return false;
	HDCConfigParam p = {clickonly, keyaction, keyflag };
	return Send(HDC_Config, &p, sizeof(p));
}
int THookLoader64::Config2( struct TDCHConfig *cfg )
{
	if (!hwndTarget)
		return false;
	return Send(HDC_Config2, cfg, sizeof(*cfg));
}
int THookLoader64::Capture()
{
	if (!hwndTarget)
		return false;
	return Send(HDC_Capture, 0);
}
int THookLoader64::CaptureAsync()
{
	if (!hwndTarget)
		return false;
	return Send(HDC_CaptureAsync, 0);
}
int THookLoader64::CaptureAsyncWait()
{
	if (!hwndTarget)
		return false;
	return Send(HDC_CaptureAsyncWait, 0);
}
int THookLoader64::Debug()
{
	if (!hwndTarget)
		return false;
	return Send(HDC_Debug, 0);
}

int THookLoader64::Send(int msg, void *data, int len)
{
	COPYDATASTRUCT cds;
	memset(&cds, 0, sizeof(cds));
	cds.dwData = msg;
	cds.lpData = data;
	cds.cbData = len;
	return (int)SendMessage(hwndTarget, WM_COPYDATA, 0, (LPARAM)&cds);
}

