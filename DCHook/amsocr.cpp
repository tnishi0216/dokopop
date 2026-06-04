#define STRICT
#include <windows.h>
#include <tchar.h>
#pragma hdrstop
#include "amsocr.h"

#define	tsizeof(type)	sizeof(type)

#define	APPNAME	_T("Auto MSOCR")

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

HWND AMSOCR::Find(bool force)
{
	if (!force){
		if (hwndAMSOCR) return hwndAMSOCR;
	}
	hwndFind = NULL;
	EnumWindows(EnumWindowsProc, 0);
	return hwndAMSOCR = hwndFind;
}

int AMSOCR::Send(int cmd, const char *data, int len)
{
	COPYDATASTRUCT cd;
	cd.dwData = cmd;
	cd.lpData = (void*)data;
	cd.cbData = len;
	return (int)SendMessage(hwndAMSOCR, WM_COPYDATA, 0, (LPARAM)&cd);
}
