#ifndef __DCHook64_h
#define __DCHook64_h

// App -> dchk64.exe

enum eHookDllCmd {
	HDC_Load = WM_APP+0x100,
	HDC_Unload,
	HDC_Init,
	HDC_Uninit,
	HDC_Config,
	HDC_Config2,
	HDC_Capture,
	HDC_CaptureAsync,
	HDC_CaptureAsyncWait,
	HDC_Debug,
};

struct HDCInitParam {
	HWND hwnd;
	//char module_name[];
	//bool windowsnt;
	//const char *vxd_path;
};

struct HDCConfigParam {
	int clickonly;
	int keyaction;
	int keyflag;
};

// dchk64.exe -> App
#define	UM_SHORTCUT			(WM_APP+33)

// WPARAM value
#define	SCINX_NOTIFY64	(0x1000)	// notify from 64bit HookDll

#endif
