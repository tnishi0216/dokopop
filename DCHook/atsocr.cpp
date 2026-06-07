#define STRICT
#include <windows.h>
#include <tchar.h>
#pragma hdrstop
#include "atsocr.h"

#define	tsizeof(type)	sizeof(type)

#define	APPNAME	_T("Auto TSOCR")

static HWND hwndFind;
static BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	TCHAR wndname[80];
	if (GetWindowText(hwnd, wndname, tsizeof(wndname))<0){
		return TRUE;
	}
	if (_tcscmp(wndname, APPNAME)){ return TRUE; }

	COPYDATASTRUCT cd;
	cd.dwData = WMCD_EXISTCHECK;
	cd.lpData = (void*)APPNAME;
	cd.cbData = (DWORD)(_tcslen(APPNAME)+1)*sizeof(TCHAR);
	if ( SendMessage( hwnd, WM_COPYDATA, 0, (LPARAM)&cd ) )
	{
		// found
		hwndFind = hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND ATSOCR::Find(bool force)
{
	if (!force){
		if (hwndATSOCR) return hwndATSOCR;
	}
	hwndFind = NULL;
	EnumWindows(EnumWindowsProc, 0);
	return hwndATSOCR = hwndFind;
}

int ATSOCR::Send(int cmd, const char *data, int len)
{
	COPYDATASTRUCT cd;
	cd.dwData = cmd;
	cd.lpData = (void*)data;
	cd.cbData = len;
	return (int)SendMessage(hwndATSOCR, WM_COPYDATA, 0, (LPARAM)&cd);
}
