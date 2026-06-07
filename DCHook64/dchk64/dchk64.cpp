// dchk64.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "dchk64.h"
#include "../../DCHookTest/HookLoader.h"
#include "../DCHook64.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

const char *StrDCHOOKDLL = "DKPUHK64.dll";
THookLoader HookDll(StrDCHOOKDLL);

HWND hWnd = NULL;
HWND hwndParent = NULL;
HANDLE hMutex = NULL;
bool WindowsNT = true;

#ifdef _DEBUG
#define	DBW	dbw
#else
#define	DBW
#endif
void dbw( const char *format, ... );

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, LPTSTR lpCmdLine, int);
void Cleanup();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int atox( const TCHAR *str, TCHAR **next );
void SetDpiAware();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	SetDpiAware();

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DCHK64, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, lpCmdLine, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DCHK64));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Cleanup();

	return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;	//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PDHK64));
#ifdef _DEBUG
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
#else
	wcex.hCursor		= NULL;	//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;	//(HBRUSH)(COLOR_WINDOW+1);
#endif
	wcex.lpszMenuName	= NULL;	//MAKEINTRESOURCE(IDC_PDHK64);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;	//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, LPTSTR, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, LPTSTR lpCmdLine, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	TCHAR ModuleName[_MAX_PATH];
	GetModuleFileName(NULL, ModuleName, sizeof(ModuleName)/sizeof(TCHAR));
	hMutex = CreateMutex(NULL, FALSE, ModuleName);
	if (GetLastError()==ERROR_ALREADY_EXISTS){
		return FALSE;	// dup launched
	}

#ifdef _DEBUG
	const int width = 800;
	const int height = 400;
#else
	const int width = 0;
	const int height = 0;
#endif

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

	hwndParent = (HWND)(LONG_PTR)atox(lpCmdLine, NULL);
	//DBW("hwndParent=%d", (int)(LONG_PTR)hwndParent);
	if (hwndParent){
		PostMessage(hwndParent, UM_SHORTCUT, SCINX_NOTIFY64, (LPARAM)hWnd);
		//DBW("Posted");
	}

#ifdef _DEBUG
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
#endif

   return TRUE;
}

void Cleanup()
{
	if (hMutex){
		CloseHandle(hMutex);
		hMutex = NULL;
	}
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		HookDll.Load(hWnd);
#ifdef _DEBUG
		{
		if (!HookDll.Init(hWnd)){
			DBW("Hook.Init error");
		}
#if 0
		HookDll.Config(-1, KA_POPUP, KF_CONTROL);
		TDCHConfig cfg;
		memset(&cfg, 0, sizeof(cfg));
		cfg.UseATSOCR = 0;
		cfg.MoveSend = 0;
		cfg.ScaleX = 0;
		cfg.ScaleY = 0;
		HookDll.Config2(&cfg);
#endif
		}
#endif
		break;
#if 0
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
#endif
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COPYDATA:
		{
		COPYDATASTRUCT *cds = (COPYDATASTRUCT*)lParam;
		if ( !cds ) break;
		switch (cds->dwData){
			case HDC_Init:
				{
				HDCInitParam &p = *(HDCInitParam*)cds->lpData;
				return HookDll.Init(p.hwnd);
				}
			case HDC_Uninit:
				HookDll.Uninit();
				break;
			case HDC_Config:
				{
				HDCConfigParam &p = *(HDCConfigParam*)cds->lpData;
				return HookDll.Config(p.clickonly, p.keyaction, p.keyflag);
				}
			case HDC_Config2:
				return HookDll.Config2((TDCHConfig*)cds->lpData);
			case HDC_Capture:
				return HookDll.Capture();
			case HDC_CaptureAsync:
				return HookDll.CaptureAsync();
			case HDC_CaptureAsyncWait:
				return HookDll.CaptureAsyncWait();
			case HDC_Debug:
				return HookDll.Debug();
		}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int atox( const TCHAR *str, TCHAR **next )
{
	unsigned val = 0;
	while ( 1 )
	{
		TCHAR c = *str;
		if ( c >= '0' && c <= '9' ){
			val = (val<<4) + (c - '0');
		} else if ( c >= 'A' && c <= 'F' ){
			val = (val<<4) + ( c - 'A' + 10 );
		} else if ( c >= 'a' && c <= 'f' ){
			val = (val<<4) + ( c - 'a' + 10 );
		} else {
			break;
		}
		str++;
	}
	if ( next ) *next = (TCHAR*)str;
	return val;
}

static HWND hWin = NULL;
static const TCHAR *clsnameW = _T("TDbgMsgForm");
static const TCHAR *winnameW = _T("Debug Messenger");

void dbw( const char *format, ... )
{
	if ( !hWin ){
		hWin = FindWindow( clsnameW, winnameW );
		if ( !hWin ) return;
	}
	va_list ap;
	va_start( ap, format );
	char buf[ 2048 ];
	wvsprintfA( buf, format, ap );
	COPYDATASTRUCT cds;
	cds.dwData = 1;	// Indicate String
	cds.cbData = (DWORD)strlen(buf)+1;
	cds.lpData = buf;
	SendMessageA( hWin, WM_COPYDATA, NULL, (LPARAM)&cds );
	va_end( ap );
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

typedef WINUSERAPI BOOL (WINAPI *FNSetProcessDpiAwarenessContext)(UINT_PTR vaule);
#define	DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2	((UINT_PTR)-4)
void SetDpiAware()
{
	HINSTANCE hDll = LoadLibrary( _T("user32") );
	if (!hDll)
		return;
	FNSetProcessDpiAwarenessContext fnSetProcessDpiAwarenessContext = (FNSetProcessDpiAwarenessContext)GetProcAddress(hDll, "SetProcessDpiAwarenessContext");
	if (fnSetProcessDpiAwarenessContext){
		if (fnSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)){
		} else {
			DBW("x64-SetDpiAware failed: %d", GetLastError());
		}
	}
	FreeLibrary(hDll);
}

