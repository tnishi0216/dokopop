//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "prgconfig.h"
#include "Util.h"
#include "prgprof.h"
#include "pdver.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#ifdef USE_UNICODE
const char APPNAME[] = {"DokoPop/Unicode"};
#else
const char APPNAME[] = {"DokoPop"};
#endif

const char *ATSOCR_EXE_PATH = "atsocr.exe";
//const char *ATSOCR_EXE_PATH = "\\src\\atsocr\\atsocr\\bin\\Debug\\atsocr.exe";

const char *EXMODIST_EXE_PATH = "ExMODIst.exe";

bool WindowsNT = true;
bool fWow64 = false;

HWND hwndMain = NULL;

HKEY OpenKey( HKEY hkey, const char *keyname )
{
	HKEY newkey;
	if ( RegOpenKeyEx( hkey, keyname, 0, KEY_ALL_ACCESS, &newkey ) == ERROR_SUCCESS ){
		return newkey;
	}
	return NULL;
}

HKEY CreateKey( HKEY hkey, const char *keyname )
{
	HKEY newkey;
	DWORD result;
	if ( RegCreateKeyEx( hkey, keyname, 0, (LPTSTR)keyname, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &newkey, &result ) == ERROR_SUCCESS ){
		return newkey;
	}
	return NULL;
}
DWORD GetValueLength( HKEY hkey, const char *name )
{
	DWORD len = 0;
	if ( RegQueryValueEx( hkey, (LPTSTR)name, 0, NULL, NULL, &len ) == ERROR_SUCCESS ){
		return len;
	}
	return 0L;
}
#if 0
DWORD ReadInteger( HKEY hkey, const char *name, DWORD val )
{
	DWORD len = sizeof(DWORD);
	DWORD v;
	if ( RegQueryValueEx( hkey, (LPTSTR)name, 0, NULL, (LPBYTE)&v, &len ) == ERROR_SUCCESS ){
		return v;
	}
	return val;
}
AnsiString ReadString( HKEY hkey, const char *name, const char *def )
{
	DWORD len = GetValueLength( hkey, name );
	if ( len ){
		char *buf = new char[ len ];
		if ( RegQueryValueEx( hkey, (LPTSTR)name, 0, NULL, (LPBYTE)buf, &len ) == ERROR_SUCCESS ){
			AnsiString s = buf;
			delete buf;
			return s;
		}
	}
	return def;
}
#endif
TMyIni::TMyIni( HKEY root, const char *soft, bool can_create )
{
	hroot = can_create ? CreateKey( root, soft ) : OpenKey( root, soft );
}
TMyIni::~TMyIni()
{
	if ( hroot ){
		RegCloseKey( hroot );
	}
}
void TMyIni::WriteInteger( const char *section, const char *key, int val )
{
	HKEY hkey = CreateKey( hroot, section );
	if ( hkey ){
		RegSetValueEx( hkey, (LPCTSTR)key, 0, REG_DWORD, (LPBYTE)&val, sizeof(DWORD) );
		RegCloseKey( hkey );
	}
}
int TMyIni::ReadInteger( const char *section, const char *key, int val )
{
	HKEY hkey = OpenKey( hroot, section );
	if ( hkey ){
		DWORD len = sizeof(DWORD);
		DWORD v;
		if ( RegQueryValueEx( hkey, (LPTSTR)key, 0, NULL, (LPBYTE)&v, &len ) == ERROR_SUCCESS ){
			return v;
		}
	}
	return val;
}
void TMyIni::WriteString( const char *section, const char *key, const char *str )
{
	HKEY hkey = NULL;
	if ( section )
		hkey = CreateKey( hroot, section );
	if ( !section || hkey ){
		RegSetValueEx( hkey, (LPCTSTR)key, 0, REG_SZ, (LPBYTE)str, lstrlen(str)+1 );
		RegCloseKey( hkey );
	}
}
AnsiString TMyIni::ReadString( const char *section, const char *key, const char *str )
{
	HKEY hkey = NULL;
	if ( section )
		hkey = OpenKey( hroot, section );
	if ( !section || hkey ){
		DWORD len = GetValueLength( hkey, key );
		if ( len ){
			char *buf = new char[ len ];
			if ( RegQueryValueEx( hkey, (LPTSTR)key, 0, NULL, (LPBYTE)buf, &len ) == ERROR_SUCCESS ){
				AnsiString s = buf;
				delete[] buf;
				return s;
			}
			delete[] buf;
		}
	}
	return str;
}

#define	NAMEBUFFSIZE	512
BOOL QueryInfoKey( HKEY hkey, DWORD *maxvalue, DWORD *maxdata );
BOOL EnumValue( HKEY hkey, DWORD index, AnsiString &name, void *pbuffer=NULL, DWORD *maxlen=NULL, DWORD *type=NULL );

void TMyIni::ReadValues( const char *section, TStrings *strs )
{
	strs->Clear();

	HKEY hkey = OpenKey( hroot, section );
	if ( hkey ){
		DWORD maxvaluename;
		DWORD maxvaluedata;
		if ( !QueryInfoKey( hkey, &maxvaluename, &maxvaluedata ) ){
			// 動かない！！！
			maxvaluename = 128;
			maxvaluedata = 4096;	// 適当！！
		}

		AnsiString _entry;
		char *buf = new char[ maxvaluedata + 1 ];
		for ( int i=0;;i++ ){
			DWORD maxlen = maxvaluedata + 1;
			if ( !EnumValue( hkey, i, _entry, buf, &maxlen ) )
				break;
			strs->Add( _entry );
		}
		delete[] buf;
	}
}

//
// TAppIni class
//
TAppIni::TAppIni()
	:super(HKEY_CURRENT_USER, REG_PDICEXE, false)
{
#ifdef USE_UNICODE
	if (!hroot){
		super::super(HKEY_CURRENT_USER, REG_PDICEXE, false);
	}
#endif
}

const char *sLeft = "Left";
const char *sTop = "Top";
const char *sWidth  = "Width";
const char *sHeight = "Height";

void LoadForm( const char *section, TForm *form, HWND hwnd )
{
	RECT rc;
	if ( hwnd ){
		GetWindowRect( hwnd, &rc );
	} else {
		rc.left = rc.top = 0;
	}
	form->Left = Ini->ReadInteger(section, sLeft, form->Left) + rc.left;
	form->Top = Ini->ReadInteger(section, sTop, form->Top ) + rc.top;
	RECT rcScr;
	GetScreenSize(form->Handle?form->Handle:hwnd, &rcScr);
	int sx = rcScr.right - rcScr.left;
	int sy = rcScr.bottom - rcScr.top;

	switch ( form->BorderStyle ){
	case bsSizeable:
	case bsToolWindow:
	case bsSizeToolWin:
		form->Width = Ini->ReadInteger( section, sWidth, form->Width );
		form->Height = Ini->ReadInteger( section, sHeight, form->Height );
		break;
	}
	// 画面の外かどうか？
	if (form->Left+form->Width >= sx){
		// 右端
		form->Left = sx - form->Width;
	}
	if (form->Left < rcScr.left){
		// 左端
		form->Left = rcScr.left;
	}
	if (form->Top+form->Height >= sy){
		// 下端
		form->Top = sy - form->Height;
	}
	if (form->Top < rcScr.top){
		// 上端
		form->Top = rcScr.top;
	}
}
void SaveForm( const char *section, TForm *form, HWND hwnd )
{
	RECT rc;
	if ( hwnd ){
		GetWindowRect( hwnd, &rc );
	} else {
		rc.left = rc.top = 0;
	}
	Ini->WriteInteger( section, sLeft, form->Left - rc.left );
	Ini->WriteInteger( section, sTop, form->Top - rc.top );
	switch ( form->BorderStyle ){
	case bsSizeable:
	case bsToolWindow:
	case bsSizeToolWin:
		Ini->WriteInteger( section, sWidth, form->Width );
		Ini->WriteInteger( section, sHeight, form->Height );
		break;
	}
}

BOOL QueryInfoKey( HKEY hkey, DWORD *maxvalue, DWORD *maxdata )
{
	char *classname = new char[ 512 ];	// こんなもんでいいかなぁ？
	DWORD classnamesize = 512;
	DWORD SubKeys;
	DWORD MaxSubKey;
	DWORD MaxClass;
	DWORD Values;
	DWORD MaxValueName;
	DWORD MaxValueData;
	DWORD SecurityDescriptor;
	FILETIME LastWriteTime;
	if ( ::RegQueryInfoKey( hkey, classname, &classnamesize,
		NULL,
		&SubKeys,
		&MaxSubKey,
		&MaxClass,
		&Values,
		&MaxValueName,
		&MaxValueData,
		&SecurityDescriptor,
		&LastWriteTime
		) != ERROR_SUCCESS ){
		delete[] classname;
		return FALSE;
	}
	if ( maxvalue )
		*maxvalue = MaxValueName;
	if ( maxdata )
		*maxdata = MaxValueData;
	delete[] classname;
	return TRUE;
}
BOOL EnumValue( HKEY hkey, DWORD index, AnsiString &name, void *pbuffer, DWORD *maxlen, DWORD *type )
{
	char *buffer = new char[ NAMEBUFFSIZE ];
	DWORD buflen = NAMEBUFFSIZE;
	if ( RegEnumValue( hkey, index, buffer, &buflen, 0, type, (LPBYTE)pbuffer, maxlen ) == ERROR_SUCCESS ){
		name = buffer;
		delete[] buffer;
		return TRUE;
	}
	delete[] buffer;
	return FALSE;
}
bool CheckPassword( const char *str )
{
	AnsiString s;
	if ( str ){
		s = str;
	} else {
		s = Ini->ReadString( "Main", "PW", "" );
	}
	return s == "123";
}
void CheckWOW64()
{
	typedef BOOL WINAPI (*FNIsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);

	FNIsWow64Process _IsWow64Process = (FNIsWow64Process)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");
	if (_IsWow64Process){
		BOOL flag = FALSE;
		if (_IsWow64Process(GetCurrentProcess(), &flag)){
			fWow64 = flag;
		}
	}
}
void MoveToTop( HWND hwnd )
{
	if ( IsIconic( hwnd ) )
		ShowWindow( hwnd, SW_RESTORE );

	// Undocumented way

	HMODULE hUser32 = GetModuleHandle("user32");
	if ( hUser32 ){
		typedef void (WINAPI *PROCSWITCHTOTHISWINDOW) (HWND, BOOL);
		PROCSWITCHTOTHISWINDOW SwitchToThisWindow = (PROCSWITCHTOTHISWINDOW)GetProcAddress(hUser32, TEXT("SwitchToThisWindow"));
		if ( SwitchToThisWindow ){
			SwitchToThisWindow( hwnd, true );
			FreeLibrary( hUser32 );
			return;
		}
	}

	FreeLibrary( hUser32 );

	DWORD locktimeout;
	/*BOOL changed = */ SystemParametersInfo( SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &locktimeout, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE );
	SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, NULL, SPIF_UPDATEINIFILE);
	SetForegroundWindow(hwnd);
	SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, locktimeout, NULL, SPIF_UPDATEINIFILE);
}
// window(hwnd)をhwndParentのcenterへ移動
// screenの外へ出る場合は調整する
void MoveCenter( HWND hwnd, HWND hwndParent )
{
	RECT rc;
	RECT rd;
	if (!hwndParent){
		hwndParent = hwndMain;	// parentがなければmain window
	}
	if (hwndParent){
#ifndef WINCE
		if ( IsIconic(hwndParent) ){
			GetScreenSize(hwndParent, &rc);
		} else
#endif
		{
			::GetWindowRect( hwndParent, &rc );
		}
	} else {
		GetScreenSize(hwnd, &rc);
	}
	RECT rcScr;
	GetScreenSize(hwnd, &rcScr);
	::GetWindowRect( hwnd, &rd );
	int width = rd.right - rd.left;
	int height = rd.bottom - rd.top;
	int left = rc.left + ( ( rc.right - rc.left ) - width )/2;
	int top = rc.top + ( ( rc.bottom - rc.top ) - height )/2;
	if ( left < rcScr.left ){
		left = rcScr.left;
	}
	if ( top < rcScr.top ){
		top = rcScr.top;
	}
	if ( left + width > rcScr.right ){
		left = rcScr.right - (rd.right - rd.left);
	}
	if ( top + height > rcScr.bottom ){
		top = rcScr.bottom - (rd.bottom - rd.top);
	}
	::SetWindowPos( hwnd, (HWND)NULL, left & ~7, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
}
void GetScreenSize(HWND hwndBase, RECT *rcWork, RECT *rcScreen)
{
#if 0
	if (!PdicMain){
		__assert(false);
		MLFXPC_CMN_ASSERT(false);
		if (rcScreen){
			SetRect(rcScreen, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
		if (rcWork){
			SystemParametersInfo(SPI_GETWORKAREA, 0, rcWork, 0);
		}
		return;
	}
#endif
	//__assert(hwndMain!=NULL);
	HMONITOR hMonitor = MonitorFromWindow(hwndBase ? hwndBase : (hwndMain ? hwndMain : GetActiveWindow()), MONITOR_DEFAULTTONEAREST);
	//__assert(hMonitor!=NULL);
	MONITORINFO mi;
	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);
	if (rcScreen){
		*rcScreen = mi.rcMonitor;
	}
	if (rcWork){
		*rcWork = mi.rcWork;
	}
}
bool _WinExec( const char *cmd, int show, int waittime, const char *dir)
{
	STARTUPINFO sui;
	memset( &sui, 0, sizeof(STARTUPINFO) );
	sui.cb = sizeof(STARTUPINFO);
	sui.dwFlags = STARTF_USESHOWWINDOW;
	sui.wShowWindow = (WORD)show;
	sui.lpTitle = (LPTSTR)"";
	PROCESS_INFORMATION pi;
	if ( !CreateProcess( NULL, (LPTSTR)cmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, dir, &sui, &pi ) )
		return false;
	bool ret = true;
	if ( waittime ){
//		DWORD startTime = GetTickCount();
		int r = WaitForInputIdle( pi.hProcess, waittime );
		ret = r==0;
		if (!ret){
			dbw("CreateProcess: %d, 0x%X", r, GetLastError());
		} else {
//			dbw("CreateProcess: %d", GetTickCount() - startTime);
		}
	}
	CloseHandle( pi.hProcess );
	return ret;
}
static const char *MyWinTitle = "<Processing...>";
HANDLE WinExecEx( const char *cmd, int show, const char *dir, const char *title )
{
	STARTUPINFO sui;
	memset( &sui, 0, sizeof(STARTUPINFO) );
	sui.cb = sizeof(STARTUPINFO);
	sui.dwFlags = STARTF_USESHOWWINDOW;
	sui.wShowWindow = (WORD)show;
	sui.lpTitle = (LPTSTR)(title ? title : MyWinTitle);
	PROCESS_INFORMATION pi;
	if ( !CreateProcess( NULL, (LPTSTR)cmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, dir, &sui, &pi ) )
		return NULL;
	return pi.hProcess;
}
bool LaunchPdic()
{
	TAppIni *ini = new TAppIni;
	if ( !ini->hroot ){
		delete ini;
		return false;
	}

	AnsiString s = ini->ReadString( PFS_COMMON, PFS_PATH, "" );
	delete ini;
	if ( s.Length() == 0 )
		return false;
	s += "\\";
	s += NAME_PDICEXE;

	return _WinExec( s.c_str(), SW_SHOWNOACTIVATE|SW_MINIMIZE, 500 );
}
bool MODIInstalled()
{
	HANDLE h = WinExecEx(EXMODIST_EXE_PATH, SW_HIDE);
	if (!h){
		dbw("ExMODist exec error: %d", GetLastError());
		return false;
	}
	WaitForSingleObject(h, INFINITE);
	DWORD exitCode = 0;
	BOOL r = GetExitCodeProcess(h, &exitCode);
	CloseHandle(h);
	if (r){
		return exitCode ? true : false;
	}
	return false;
}
bool ATSOCRRunable()
{
	return GetDNFVersion()>=472;
}
bool LaunchATSOCR()
{
	if (!ATSOCRRunable()) return false;

	int show = 
#ifdef _DEBUG
		SW_SHOWNOACTIVATE|SW_MINIMIZE;
#else
		SW_HIDE;
#endif
	if (!_WinExec( ATSOCR_EXE_PATH, show, 300))
		return false;

	HWND hwnd = NULL;
	for (int i=0;i<10;i++){
		hwnd = FindATSOCR();
		if (hwnd)
			break;
		Sleep(10);
	}
	return hwnd!=NULL;
}
void TerminateATSOCR()
{
	HWND hwnd = FindATSOCR();
	if (hwnd){
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}
}
void ShowATSOCR()
{
	HWND hwnd = FindATSOCR();
	if (hwnd){
		ShowWindow(hwnd, SW_SHOW);
		ShowWindow(hwnd, SW_RESTORE);
	}
}
static HWND hwndFind;
static const char *findClass = NULL;
static const char *findWindow = NULL;
static const char *findAppName = NULL;
static BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	char buf[80];
	
	if (findClass){
		if ( !GetClassName( hwnd, buf, sizeof(buf)-1 ) ) return TRUE;
		//DBW("class: %s", buf);
		if (strcmp( buf, findClass )) return TRUE;
	}
	if (findWindow){
		if (GetWindowText(hwnd, buf, sizeof(buf))<0){
			return TRUE;
		}
		//DBW("wnd: %s", buf);
		if (strcmp(buf, findWindow)){ return TRUE; }
	}

	if (findAppName){
		COPYDATASTRUCT cd;
		cd.dwData = WMCD_EXISTCHECK;
		cd.lpData = (void*)findAppName;
		cd.cbData = strlen(findAppName)+1;
		if ( SendMessage( hwnd, WM_COPYDATA, 0, (LPARAM)&cd ) ){
			goto jfound;
		}
	} else {
jfound:
		hwndFind = hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND FindApp(const char *clsname, const char *wndname, const char *appname)
{
	hwndFind = NULL;
	findClass = clsname;
	findWindow = wndname;
	findAppName = appname;
	EnumWindows( (FARPROC)EnumWindowsProc, 0 );
	return hwndFind;
}
HWND FindPrev()
{
	return FindApp("TDCHookMainForm", NULL, APPNAME);
}
const char *GetStrVersion()
{
	ProjectRCVersion rcver(Application->ExeName.c_str());
	static AnsiString str_version;
	if (str_version.data())
		return str_version.c_str();
	rcver.GetProductVersion(str_version);
	return str_version.c_str();
}
unsigned GetVersionValue()
{
	ProjectRCVersion rcver(Application->ExeName.c_str());
	return rcver.GetProductVersionValue();
}
int CheckVersion(HWND hwnd)
{
	int ver = SendMessage(hwnd, WM_GET_VERSION, 0, 0);
	if (ver==0){ return -1; }	// older than ver.2.0
	int version_value = GetVersionValue();
	return ver - version_value;
}
HWND FindATSOCR()
{
	static const char APPNAME_ATSOCR[] =  "Auto TSOCR";
	return FindApp(NULL, APPNAME_ATSOCR, APPNAME_ATSOCR);
}
static HWND hwndFound;
static BOOL CALLBACK EnumWindowsProcPS( HWND hwnd, LPARAM lParam )
{
	char clsname[80];
	if ( !GetClassName( hwnd, clsname, sizeof(clsname)-1 ) ) return TRUE;
	if ( strcmp( clsname, "PSPOPUPWIN" )
		&& strcmp( clsname, "PSMENU" ) ) return TRUE;

	hwndFound = hwnd;
	
	return FALSE;
}
// PDICのpopup windowを探す
HWND FindPopupWindow( )
{
	hwndFound = NULL;
	EnumWindows( (WNDENUMPROC)EnumWindowsProcPS, 0 );
	return hwndFound;
}

void ShowManual(HWND hwnd)
{
	AnsiString dir = ExtractFileDir( Application->ExeName );
	ShellExecute( hwnd, "open", NAME_DKPPTXT, NULL, dir.c_str(), SW_SHOW );	
}

void ShowLatestVersion()
{
#if __PROTO
	const char *url = "https://github.com/tnishi0216/dokopop";
#else
	const char *url = "https://pdic.sakura.ne.jp/dokopop/";
#endif
	ShellExecute( NULL, _T("open"), url, NULL, NULL, SW_SHOW );
}

// 参考
// https://qiita.com/yamoridon/items/0524f0a6930f6b808094
#define	LOAD_LIBRARY_SEARCH_SYSTEM32	0x00000800	// Windows7 or later
//typedef NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW lpVersionInformation);
typedef int NTSTATUS;
typedef NTSTATUS (WINAPI *FNRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
void GetWindowsVersion(DWORD* majorVersion, DWORD* minorVersion, DWORD* buildNumber)
{
	DWORD major = 0;
	DWORD minor = 0;
	DWORD build = 0;
	HMODULE ntdll = LoadLibraryExW(L"ntdll.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (ntdll){
		FNRtlGetVersion rtlGetVersion = (FNRtlGetVersion)GetProcAddress(ntdll, "RtlGetVersion");
		if (rtlGetVersion) {
			RTL_OSVERSIONINFOW versionInfo = {};
			versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
			rtlGetVersion(&versionInfo);
			major = versionInfo.dwMajorVersion;
			minor = versionInfo.dwMinorVersion;
			build = versionInfo.dwBuildNumber;
		}
		FreeLibrary(ntdll);
	}
	if (majorVersion) *majorVersion = major;
	if (minorVersion) *minorVersion = minor;
	if (buildNumber) *buildNumber = build;
}

// 参考
// https://msdn.microsoft.com/ja-jp/library/hh925568(v=vs.110).aspx#net_d
// http://www.atmarkit.co.jp/ait/articles/1210/26/news086.html
unsigned GetDNFVersion()
{
	HKEY dnfKey;
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP", 0, KEY_READ, &dnfKey ) == ERROR_SUCCESS ){
		int version = 0;
		HKEY verKey;
		if ( RegOpenKeyEx( dnfKey, "v4.0", 0, KEY_READ, &verKey ) == ERROR_SUCCESS ){
			version = 400;
			RegCloseKey(verKey);
			if ( RegOpenKeyEx( dnfKey, "v4", 0, KEY_READ, &verKey ) == ERROR_SUCCESS ){
				HKEY fullKey;
				if ( RegOpenKeyEx( verKey, "Full", 0, KEY_READ, &fullKey ) == ERROR_SUCCESS ){
					DWORD dwType = REG_DWORD;
					DWORD dwByte = 4;
					DWORD dwValue = 0;
					if ( RegQueryValueEx( fullKey, "Release", NULL, &dwType, (BYTE*)&dwValue, &dwByte) == ERROR_SUCCESS){
						if (dwValue >= 533509){
							version = 481;
						} else
						if (dwValue >= 528040){
							version = 480;
						} else
						if (dwValue >= 461808){
							version = 472;
						} else
						if (dwValue >= 461308){
							version = 471;
						} else
						if (dwValue >= 460798){
							version = 470;
						} else
						if (dwValue >= 394802){
							version = 462;
						} else
						if (dwValue >= 394254){
							version = 461;
						} else
						if (dwValue >= 393295){
							version = 460;
						} else
						if (dwValue >= 379893){
							version = 452;
						} else
						if (dwValue >= 378675){
							version = 451;
						} else
						if (dwValue >= 378389){
							version = 450;
						}
					}
					RegCloseKey( fullKey );
				}
				RegCloseKey( verKey );
			}
		} else
		if ( RegOpenKeyEx( dnfKey, "v3.5", 0, KEY_READ, &verKey ) == ERROR_SUCCESS ){
			version = 350;
			RegCloseKey(verKey);
		}
		RegCloseKey(dnfKey);
		return version;
	}
	return 0;
}

static HWND hWin = NULL;
static const char *clsname = "TDbgMsgForm";
static const char *winname = "Debug Messenger";
void dbw( const char *format, ... )
{
	if ( !hWin ){
		hWin = FindWindowA( clsname, winname );
		if ( !hWin ) return;
	}
	va_list ap;
	va_start( ap, format );
	char buf[ 2048 ];
	wvsprintf( buf, format, ap );
	COPYDATASTRUCT cds;
	cds.dwData = 1;	// Indicate String
	cds.cbData = strlen(buf);
	cds.lpData = buf;
	SendMessage( hWin, WM_COPYDATA, NULL, (LPARAM)&cds );
	va_end( ap );
}

