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

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
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


