//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("DCHookTest.res");
USEFORM("DCHookMain.cpp", DCHookMainForm);
USEUNIT("StringLib.cpp");
USEFORM("PopupConfig.cpp", PopupConfigDlg);
USEUNIT("Util.cpp");
USEFORM("Notify.cpp", NotifyForm);
USERC("icon.rc");
USEUNIT("mustr.cpp");
USEUNIT("HookDLl.cpp");
USEUNIT("HookLoader.cpp");
USEUNIT("DCHookLoader.cpp");
USE("..\Diary.txt", File);
USEUNIT("prgprof.cpp");
USEFORM("MODINotifyDlg.cpp", MODINotifyDialog);
USEUNIT("prgconfig.cpp");
USEFORM("OCRTextFrm.cpp", OCRTextForm);
USEUNIT("MonitorScale.cpp");
//---------------------------------------------------------------------------
#include "prgconfig.h"
#include "DCHookMain.h"
//---------------------------------------------------------------------------

void SetDpiAware();

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	SetDpiAware();
#if __PROTO
	SYSTEMTIME st;
	GetSystemTime(&st);
	if ( (st.wYear>=2017 && st.wMonth>=2) || st.wYear>=2018){
		int ret = MessageBox(NULL, "試作版の使用期限が切れました。新しいDokoPop!を入手してください", APPNAME, MB_YESNO);
		if (ret != IDNO){
			ShowLatestVersion();
			return 0;
		}
	}
#endif

	HWND hwnd = FindPrev();
	if ( hwnd ){
		if (CheckVersion(hwnd)<0){
			// The older version already lauched.
			MessageBox(NULL, "DokoPop!がすでに起動しています。\r古いバージョンのDokoPop!をアンインストールしてください", APPNAME, MB_OK);
			return 0;
		}
		MoveToTop( hwnd );
		return 0;
	}
	try
	{
		Application->Initialize();
		Application->Title = "DokoPop";
		Application->CreateForm(__classid(TDCHookMainForm), &DCHookMainForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	if (Reboot){
		_WinExec(Application->ExeName.c_str(), SW_SHOW, 0);
	}
	return 0;
}
//---------------------------------------------------------------------------

typedef WINUSERAPI BOOL (WINAPI *FNSetProcessDpiAwarenessContext)(UINT_PTR vaule);	// for Windows10 or later
#define	DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2	((UINT_PTR)-4)
typedef enum {
  PROCESS_DPI_UNAWARE = 0,
  PROCESS_SYSTEM_DPI_AWARE = 1,
  PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef WINUSERAPI HRESULT (WINAPI *FNSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS vaule);
void SetDpiAware()
{
	HINSTANCE hDll = LoadLibrary( TEXT("user32") );
	if (!hDll)
		return;
	bool ok = false;
	// Windows10 or lter
	FNSetProcessDpiAwarenessContext fnSetProcessDpiAwarenessContext = (FNSetProcessDpiAwarenessContext)GetProcAddress(hDll, "SetProcessDpiAwarenessContext");
	if (fnSetProcessDpiAwarenessContext){
		if (fnSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)){
			ok = true;
		} else {
			DBW("SetDpiAwareContext failed: %d", GetLastError());
		}
	} else {
		DBW("No SetProcessDpiAwarenessContext");
	}
	FreeLibrary(hDll);
	if (ok) return;

	// Windows8.1
	hDll = LoadLibrary(TEXT("shcore"));
	if (hDll){
		FNSetProcessDpiAwareness fnSetProcessDpiAwareness = (FNSetProcessDpiAwareness)GetProcAddress(hDll, "SetProcessDpiAwareness");
		if (fnSetProcessDpiAwareness){
			HRESULT hResult = fnSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			if (hResult == S_OK){
			} else {
				DBW("SetDpiAware failed: %d", GetLastError());
			}
		} else {
			DBW("No SetProcessDpiAwareness");
		}
		FreeLibrary(hDll);
	}
}


