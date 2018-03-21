//---------------------------------------------------------------------------
//
// 問題点：
// Ver0.50
//
// 最適化の課題：
// standard libraryを使わないようにする
// 共有領域を増やす
//
// Tips:
// 突然Detachされる現象
// --> Hook Functionで例外が発生している可能性が高い
//     もしくは、アプリケーションで例外発生している
// 共有メモリについて
// 物理メモリが同一だけであって、論理アドレスは同一とは限らない！！
// 従って、共有メモリ上にpointerを配置することは基本的に危険！！
#define STRICT
#include <windows.h>
#include <tchar.h>
#pragma hdrstop
#ifndef GWL_WNDPROC
#define GWL_WNDPROC         (-4)	// この定義がないとerrorになった 2014.11.19
#endif
#include <tlhelp32.h>
#include <imagehlp.h>
#pragma comment(lib, "ImageHlp")
#include "DCHook.h"
#ifndef UNICODE
#include "..\VxD\hk95d.h"
#endif
#include "MonitorScale.h"

// Compiler //
#pragma	warning( disable : 4710 )	// どうしてこういうwarningが出てくるのか？

// Configuration //
#define	USE_SCALING		1	// Uses DPI scaling

#define	USE_DBW			0	// DBWを使用する
#define	USE_SHARE0		1	// shareしても問題ない変数

#define	METAEXTTEXTOUT	1	// MetaMethodでExtTextOutxをhookする
#define	HOOK_GETDC		0	// GetDC(),ReleaseDC()をhookする
#define	HOOK_TEXT		0	// DrawText(Ex),TextOut,TabbedTextOutのhooking
#define	USE_REDRAW		1
#define	HOOK_BITBLT		0	// 2016.1.15 外してみた
#define	HOOK_PAINT		0
#define	GUARD			0	// 共有メモリをVirtualProtectでguardする
							// 有効にするとApplication終了時に異常終了が発生する
                            // Application close ->
                            // 原因：Dettachする前に、OS or Applicationが
                            // Protected code領域に書き込みを行っているようだ

#define	USE_INVALIDATE		0	// 描画前にInvalidateを送る(IE5@Win98は必要)
#define	INVALIDATE_TRUE		0	// InvalidateRectでrepaintを行う
#define	ORG_OFFSET			1	// ViewPortのorg offset処理を行う

#define	USE_OPTIMALINVALID	1	// 再描画領域をなるべく小さくする
#define	USE_VXD				0	// no longer support (Win9x)
#define	EXC_WOW64			0	// do not attach on WOW64

#define	MAX_KEYS			8	// キーフラグの最大定義数

#define	DEBUG_HITTEXT		0	// for debug hit text

// ExtTextOut hookingでMetafileも使用する
// 単独で行う場合は、RETRYMETA
#if HOOK_PAINT || HOOK_GETDC || !USE_REDRAW
#define	USE_META	1
#else
#define	USE_META	0
#endif

#define	RETRYMETA	1

#if USE_DBW
#include <stdio.h>
#else	// !USE_DBW
#undef DEBUG_HITTEXT
#define	DEBUG_HITTEXT	0
#endif	// !USE_DBW

#define	MOVESEND_POST	1

// Type Definitions //

// Macros //
#if GUARD
#define	PROTECT_SHARE()		ShareProtect( true )
#define	UNPROTECT_SHARE()	ShareProtect( false )
#else
#define	PROTECT_SHARE()
#define	UNPROTECT_SHARE()
#endif

#if USE_DBW
#define	DBW		dbw
#else
#define	DBW		(void)
#endif
void dbw(const char *format, ...);

#define	WM_AMODI				(WM_APP+0x400)	// app communication message with AMODI
#define	WM_MOVESEND				(WM_APP+0x208)	// DCH_MOVESENDの代わりにPostMessageで送る
#define	AMODI_CMD_QUERY			0
#define	AMODI_CMD_PAGE_CAPTURE	1

#define	tsizeof(type)	sizeof(type)
#define	int_bool(v)		((v)!=0)

#pragma warning (disable : 4996)

//==============================================//
// TString template class						//
//==============================================//
template <class T>
class TString {
protected:
	T *Buffer;
	int Size;	// Buffer size in character
	int Length;	// String length in character
public:
	TString();
	~TString();
	void Clear()
		{ Buffer[0] = '\0'; Length = 0; }
	void Set( const T *str, int len );
	void Set( TString *obj )
		{ Set( obj->c_str(), obj->Length ); }
	void Cat( const T *str, int len );
	operator const T *() const
		{ return Buffer; }
	T operator [] ( int index )
		{ return Buffer[index]; }
	const T *c_str()
		{ return Buffer; }
	int GetLength() const { return Length; }
	int GetByte() const { return Length * sizeof(T); }
//	int GetSize() const { return Size; }
};
template <class T>
TString<T>::TString()
{
	Size = 256;		// initial size
	Buffer = new T[ Size + 1 ];
	if (!Buffer)
		Size = 0;
	Length = 0;
}
template <class T>
TString<T>::~TString()
{
	if (Buffer)
		delete[] Buffer;
}
template <class T>
void TString<T>::Set( const T *str, int len )
{
	if (Buffer){
		if ( Size < len ){
			// not enough space to store
			delete[] Buffer;
		} else goto j1;
	}
	Buffer = new T[ len + 1 ];
	Size = len;
	if (Buffer){
j1:;
		CopyMemory( Buffer, str, len*sizeof(T) );
		Buffer[len] = '\0';
		Length = len;
	} else {
		Size = 0;
		Length = 0;
	}
}
template <class T>
void TString<T>::Cat( const T *str, int len )
{
	if (!Buffer){
		Set( str, len );
		return;
	}
	int newlen = Length + len;
	if (Size < newlen){
		T *p = new T[ newlen + 1 ];
		if (p)
			CopyMemory( p, Buffer, newlen*sizeof(T) );
		delete[] Buffer;
		Buffer = p;
		Size = newlen;
	}
	if (Buffer){
		CopyMemory( Buffer+Length, str, len * sizeof(T) );
		Buffer[newlen] = '\0';
		Length = newlen;
	} else {
		Size = 0;
		Length = 0;
		Set( str, len );
	}
}

int GetTextFromPoint( HDC hdc, const char *text, int len, int pos, int pos_y, const int *dx=NULL );
int GetTextFromPoint( HDC hdc, const wchar_t *text, int len, int pos, int pos_y, const int *dx=NULL );

bool CaptureImage(HWND hwnd, bool movesend, bool non_block);
bool WaitAndGetResult(const TCHAR *text_path, unsigned waittime);
HANDLE WaitForResult(const TCHAR *text_path, unsigned waittime);
HWND FindAMODI();
int SendAMODI(int cmd, const char *data, int len);
void CheckAMODIAlive();
void SendMoveMessage();
void SendCancelMove();
DWORD WINAPI SendMoveThread(LPVOID vdParam);
void CheckWOW64();

extern "C" {
//==============================================//
// Prototypes									//
//==============================================//
typedef HDC (WINAPI *FNBeginPaint)( HWND, LPPAINTSTRUCT );
typedef BOOL (WINAPI *FNEndPaint)( HWND, CONST PAINTSTRUCT * );
typedef HDC (WINAPI *FNGetDC)( HWND );
typedef int (WINAPI *FNReleaseDC)( HWND, HDC );
typedef BOOL (WINAPI *FNExtTextOutW)(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCWSTR str, UINT count, CONST INT *dx );
typedef BOOL (WINAPI *FNExtTextOutA)(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCSTR str, UINT count, CONST INT *dx );
typedef BOOL (WINAPI *FNBitBlt)(HDC hdcdest, int xdest, int ydest, int width, int height, HDC hdcsrc, int xsrc, int ysrc, DWORD rop );

typedef LONG (WINAPI *FNTabbedTextOutA)( HDC hdc, int x, int y, LPCSTR str, int count, int ntabs, LPINT tabs, int origin );
typedef LONG (WINAPI *FNTabbedTextOutW)( HDC hdc, int x, int y, LPCWSTR str, int count, int ntabs, LPINT tabs, int origin );
typedef BOOL (WINAPI *FNTextOutA)( HDC hdc, int x, int y, LPCSTR str, int count );
typedef BOOL (WINAPI *FNTextOutW)( HDC hdc, int x, int y, LPCWSTR str, int count );
typedef int (WINAPI *FNDrawTextA)( HDC hdc, LPCSTR str, int count, LPRECT rc, UINT format );
typedef int (WINAPI *FNDrawTextW)( HDC hdc, LPCWSTR str, int count, LPRECT rc, UINT format );
typedef int (WINAPI *FNDrawTextExA)( HDC hdc, LPCSTR str, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params );
typedef int (WINAPI *FNDrawTextExW)( HDC hdc, LPCWSTR str, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params );

typedef BOOL (WINAPI *FNMoveToEx)(HDC hdc, int x, int y, LPPOINT pt );


HDC WINAPI _BeginPaint( HWND hwnd, LPPAINTSTRUCT ps );
BOOL WINAPI _EndPaint( HWND hwnd, CONST PAINTSTRUCT *ps );
HDC WINAPI _GetDC( HWND hwnd );
int WINAPI _ReleaseDC( HWND hwnd, HDC hdc );
BOOL  WINAPI _ExtTextOutA(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCSTR str, UINT count, CONST INT *dx );
BOOL  WINAPI _ExtTextOutW(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCWSTR str, UINT count, CONST INT *dx );
BOOL WINAPI _BitBlt(HDC hdcdest, int xdest, int ydest, int width, int height, HDC hdcsrc, int xsrc, int ysrc, DWORD rop );

LONG WINAPI _TabbedTextOutA( HDC hdc, int x, int y, LPCSTR str, int count, int ntabs, LPINT tabs, int origin );
LONG WINAPI _TabbedTextOutW( HDC hdc, int x, int y, LPCWSTR str, int count, int ntabs, LPINT tabs, int origin );
BOOL WINAPI _TextOutA( HDC hdc, int x, int y, LPCSTR str, int count );
BOOL WINAPI _TextOutW( HDC hdc, int x, int y, LPCWSTR str, int count );
int WINAPI _DrawTextA( HDC hdc, LPCSTR str, int count, LPRECT rc, UINT format );
int WINAPI _DrawTextW( HDC hdc, LPCWSTR str, int count, LPRECT rc, UINT format );
int WINAPI _DrawTextExA( HDC hdc, LPCSTR str, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params );
int WINAPI _DrawTextExW( HDC hdc, LPCWSTR str, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params );

BOOL WINAPI _MoveToEx( HDC hdc, int x, int y, LPPOINT pt );

void ShareProtect( bool f );
DWORD SetWriteProtect( LPVOID addr, bool f );
bool ChangeMemory( void *dst, const void *src, unsigned size );
bool LoadVxD();
void UnloadVxD();

static HMODULE ModuleFromAddress(PVOID pv);

// Prototypes for PSAPI.DLL //
typedef BOOL (WINAPI *FNEnumProcessModules)( HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
typedef DWORD (WINAPI *FNGetModuleFileNameExA)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
typedef HANDLE (WINAPI *FNCreateToolhelp32Snapshot)( DWORD dwFlags, DWORD th32ProcessID );
typedef BOOL (WINAPI *FNModule32First)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
typedef BOOL (WINAPI *FNModule32Next)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );


LRESULT CALLBACK MouseProc( int code, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam );
bool CaptureText( HWND hwnd, bool movesend );
bool DoCapture(HWND hwnd, POINT pt, bool movesend, bool image_only=false, bool runOnLaunchedProc=false, bool nonBlock=false);
void DokoPopMenu( HWND hwnd );
void ToggleClick( HWND hwnd );
int CALLBACK EnumMetaFileProc( HDC hdc, HANDLETABLE *, CONST ENHMETARECORD *mfr, int nobj, LPARAM user );

//__declspec(dllexport)
BOOL  WINAPI _ExtTextOutA(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCSTR str, UINT count, CONST INT *dx );

#if METAEXTTEXTOUT
void ExtTextOutHit();
#endif

void DoScale(HDC hdc, POINT *pts, int num);

// Prototypes for High DPI //
typedef enum _PROCESS_DPI_AWARENESS { 
  Process_DPI_Unaware            = 0,
  Process_System_DPI_Aware       = 1,
  Process_Per_Monitor_DPI_Aware  = 2
} PROCESS_DPI_AWARENESS;

typedef HRESULT (WINAPI *FNGetProcessDPIAwareness)(HANDLE hprocess, PROCESS_DPI_AWARENESS *value);
typedef HRESULT (WINAPI *FNSetProcessDPIAwareness)(PROCESS_DPI_AWARENESS value);

//==============================================//
// Shared Section								//
//==============================================//
#pragma data_seg(".sdata")
#pragma bss_seg( ".sbss" )
HWND hwndCallback = NULL;
#if USE_VXD
char VxDpathName[256];	// HK95.vxd path
#endif
DWORD siPageSize = 0;
HWND hwndOrg = NULL;
HWND hwndAMODI = NULL;
bool RequireHwndAMODI = false;
bool OnlyAMODI = false;
bool tryAMODI = false;
bool MoveSend = false;
#if USE_DBW
int attach = 0;
#endif
#if USE_VXD
bool WindowsNT;
#else
const bool WindowsNT = true;
#endif
bool Initialized = false;	// Whether this module initialized or not
DWORD idDokopopProcess;		// DKPP.EXE process ID
HHOOK hMouseHook;	// Windows Hook Handle for mouse
bool ClickOnlyEnabled = false;

struct TKeyConfig {
	unsigned char Action;
	int Flag;
};

TKeyConfig KeyConfig[MAX_KEYS] = {
	{ KA_POPUP, KF_CONTROL },			// Ctrl+click
	{ KA_POPUP_NC, KF_CLICKONLY },			// clickだけでpopup(DO NOT CHANGE)
//	{ KA_MENU, KF_CONTROL | KF_MENU },	// dokopop menu
//	{ KA_MENU, KF_CONTROL | KF_MENU | KF_CLICKONLY },	// dokopop menu
	{ KA_TOGGLE, KF_CONTROL | KF_MENU },	// dokopop menu
	{ 0, 0 }
};

bool fWow64 = false;

POINT CursorPoint;	// Client area coordinate //
POINT ScreenPoint;	// Screen coordinate //
POINT PrevCursorPoint;
HWND hwndLast = NULL;
DWORD PrevMoveTime = 0;
bool MoveSent = true;
bool OnlyImage = false;

bool MouseMoving;

RECT rcClient;
RECT rcInvalid;
RECT rcOrgInvalid;	// 再描画前にinvalidだった領域
bool HitFound;
bool RButtonUpCancel = false;	// 次のRButtonUpは破棄する
bool LButtonUpCancel = false;	// 次のLButtonUpは破棄する
bool MButtonUpCancel = false;	// 次のMButtonUpは破棄する
bool LButtonDown = false;
bool MButtonDown = false;

HINSTANCE hPSAPI;		// handle of PSAPI.DLL(NT only)
FNEnumProcessModules fnEnumProcessModules;
FNGetModuleFileNameExA fnGetModuleFileNameExA;
FNCreateToolhelp32Snapshot fnCreateToolhelp32Snapshot;
FNModule32First fnModule32First;
FNModule32Next fnModule32Next;

#if USE_META || RETRYMETA
HDC OrgHDC;
HDC hdcMeta;
#endif
HDC hdcExtTextOut;
#if HOOK_GETDC
HDC OrgGetDC;
#endif

int BitBltCount;		// 現在のBitBlt回数
int ExtTextOutCount;	// 現在のBand内のExtTextOut回数(A/Wとも)
int TargetBitBltCount;	// 検査対象のBitBlt(初回描画後に判定) 
POINT TargetOffset;		// 検査対象のBitBltの描画オフセット(初回描画後に判定)
bool RetryPhase;

#if METAEXTTEXTOUT
// hit text information
TString<char> *FoundTextA;
TString<wchar_t> *FoundTextW;
int FoundLocA;
int FoundLocW;

// 最後に描画したテキストの情報
// for text merge
int LastX;	// 最後に描画したテキストの右上座標
int LastY;
TString<char> *LastTextA;
TString<wchar_t> *LastTextW;
#endif	// METAEXTTEXTOUT

bool MoveSendMode;

TCHAR AMODIPath[256];
bool ExtAMODI = false;
int ScaleX = 0;	// 96を1とした画面の拡大率
int ScaleY = 0;
int generation = 0;
int NumPrevWords = 1;	// 何個まで前の単語を拾うか？

TCHAR ImageTextPath[256+40];	// OCR用image filename or text filename
DWORD SaveImageTime;
bool CaptureImageBlocking = false;

const char STR_USER32[] = "user32.dll";
const char STR_GDI32[] = "gdi32.dll";
const char STR_KERNEL32[] = "kernel32.dll";
const TCHAR STR_PSAPI[] = _T("psapi.dll");

// KERNEL32.DLL //
const char STR_CreateToolhelp32Snapshot[] = "CreateToolhelp32Snapshot";
const char STR_Module32First[] = "Module32First";
const char STR_Module32Next[] = "Module32Next";

// PSAPI.DLL //
const char STR_EnumProcessModules[] = "EnumProcessModules";
const char STR_GetModuleFileNameExA[] = "GetModuleFileNameExA";

const char STR_BeginPaint[] = "BeginPaint";
const char STR_EndPaint[] = "EndPaint";
const char STR_BitBlt[] = "BitBlt";
const char STR_GetDC[] = "GetDC";
const char STR_ReleaseDC[] = "ReleaseDC";

const char STR_ExtTextOutA[] = "ExtTextOutA";
const char STR_ExtTextOutW[] = "ExtTextOutW";
const char STR_TabbedTextOutA[] = "TabbedTextOutA";
const char STR_TabbedTextOutW[] = "TabbedTextOutW";
const char STR_TextOutA[] = "TextOutA";
const char STR_TextOutW[] = "TextOutW";
const char STR_DrawTextA[] = "DrawTextA";
const char STR_DrawTextW[] = "DrawTextW";
const char STR_DrawTextExA[] = "DrawTextExA";
const char STR_DrawTextExW[] = "DrawTextExW";

#if USE_SHARE0
#pragma data_seg()
#pragma bss_seg()
#endif

//==============================================//
// Module depend values							//
//==============================================//
HINSTANCE hInstance;	// attached instance
DWORD idProcess;		// attached process
HANDLE hProcess;	// Hook時GetCurrentProcess()は使えない
					// Hook時のcurrent processが書き換えたいthunkのprocessと一致するとは限らないため
					// そのため、DLLのattach時にWriteProcessMemory()用にprocess handleを取得する
DWORD curProcess;	// hooking process
#if USE_VXD
HANDLE vxd_Handle = INVALID_HANDLE_VALUE;	// Not shared
#endif

#if !MOVESEND_POST
HANDLE SendMoveEvent = NULL;
DWORD SendMoveThreadId;
#endif

HINSTANCE hInstSHCore = NULL;
FNGetProcessDPIAwareness GetProcessDPIAwareness = NULL;

//==============================================//
// APIHOOK										//
//==============================================//
class APIHOOK {
public:
	const char *ModName;
	const char *FuncName;
protected:
	bool ExcludeMe;
	bool Hooked;
public:
	PROC OrgFunc;	// not shared@NT
protected:
	PROC HookFunc;	// not shared
	// Process dedicated values //
public:
	static void *MaxAppAddr;	// Maximum private memory address
protected:
	static APIHOOK *LinkTop;	// top of APIHOOK link
	APIHOOK *LinkNext;			// next pointer of API HOOK link
public:
	APIHOOK( const char *modname, const char *name, void *hookfunc, bool excme = true );
	~APIHOOK();
	bool GetProcOrgFunc( HINSTANCE hInst );
	bool GetProcOrgFunc();
	bool HookAgain();
	void Finish();
protected:
	bool StoreCode( );
	void RestoreCode( );
protected:
	static void ReplaceIATEntryInAllMods( APIHOOK *obj, PROC pfnCurrent, PROC pfnNew );
	static bool ReplaceIATEntryInOneMod( APIHOOK *obj, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller);
};

const BYTE cPushOpCode = 0x68;   // The PUSH opcode on x86 platforms
APIHOOK* APIHOOK::LinkTop = NULL;
PVOID APIHOOK::MaxAppAddr = NULL;

APIHOOK::APIHOOK( const char *modname, const char *funcname, void *hookfunc, bool excme )
{
//	DBW("APIHOOK constructor:%08X %s %s",this, modname, funcname);
	Hooked = false;
	ModName = modname;
	FuncName = funcname;
	HookFunc = (PROC)hookfunc;
	ExcludeMe = excme;
	LinkNext  = LinkTop;
	LinkTop = this;
	if (!MaxAppAddr){
		// Functions with address above lpMaximumApplicationAddress require
		// special processing (Windows 98 only)
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		MaxAppAddr = si.lpMaximumApplicationAddress;
	}
}

APIHOOK::~APIHOOK()
{
	Finish();
	// Remove this object from the linked list
	APIHOOK* p = LinkTop;
	if (p == this) {     // Removing the head node
		LinkTop = p->LinkNext;
	} else {
		// Walk list from head and fix pointers
		for (; p->LinkNext; p = p->LinkNext){
			if (p->LinkNext == this) {
				// Make the node that points to us point to the our next node
				p->LinkNext = p->LinkNext->LinkNext;
				break;
			}
		}
	}
}
bool APIHOOK::GetProcOrgFunc()
{
	return GetProcOrgFunc( GetModuleHandleA( ModName ) );
}
bool APIHOOK::GetProcOrgFunc( HINSTANCE hInst )
{
	if (Hooked) return true;

	// Save information about this hooked function
	OrgFunc = ::GetProcAddress( hInst, FuncName);
	if (!OrgFunc){
		DBW("Function doesn't exist =%s:%s", ModName, FuncName);
		return false;
	}

	if (OrgFunc > MaxAppAddr) {
//		DBW("The address is in a shared DLL; the address needs fixing up");
		PBYTE pb = (PBYTE)OrgFunc;
		if (pb[0] == cPushOpCode){
			// Skip over the PUSH op code and grab the real address
			PVOID pv = * (PVOID*) &pb[1];
			OrgFunc = (PROC) pv;
		}
	}

	// Set assembler code for hooking at the org entry
	if ( !StoreCode() ){
		DBW("Cannot write process memory");
		return false;
	}

	Hooked = true;
	return true;
}
bool APIHOOK::StoreCode( )
{
	DBW("StoreCode:%s %s", ModName, FuncName);
	// Hook this function in all currently loaded modules
	ReplaceIATEntryInAllMods( this, OrgFunc, HookFunc );
	return true;
}
void APIHOOK::RestoreCode()
{
	DBW("RestoreCode:%s %s", ModName, FuncName);
	ReplaceIATEntryInAllMods( this, HookFunc, OrgFunc );
}
void APIHOOK::ReplaceIATEntryInAllMods( APIHOOK *obj, PROC pfnCurrent, PROC pfnNew )
{
	HMODULE hmodThisMod = obj->ExcludeMe
		? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;

	DWORD cbNeeded;
	HMODULE hMods[1024];
	unsigned i;
	if (fnCreateToolhelp32Snapshot){
		// Toolhelpが使用できる場合は使用する
		// Get the list of modules in this process
		HANDLE hSnapshot = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, idProcess);

		MODULEENTRY32 me = { sizeof(me) };
		for (BOOL fOk = fnModule32First(hSnapshot,&me); fOk; fOk = fnModule32Next(hSnapshot,&me)){
			// NOTE: We don't hook functions in our own module
			if (me.hModule != hmodThisMod){
	//			DBW("Module : %s %s", me.szModule,FuncName);
				// Hook this function in this module
				ReplaceIATEntryInOneMod( obj, pfnCurrent, pfnNew, me.hModule);
			}
		}
		CloseHandle( hSnapshot );
	} else {
		if( fnEnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
			{
				if (hMods[i] != hmodThisMod){
					char szModName[MAX_PATH];
					// Get the full path to the module's file.
					if ( fnGetModuleFileNameExA( hProcess, hMods[i], szModName,
										sizeof(szModName))){
	//					DBW("Module : %s %s", szModName, FuncName );
						ReplaceIATEntryInOneMod( obj, pfnCurrent, pfnNew, hMods[i]);
					}
				}
			}
		}
#if USE_DBW
		else DBW("EnumProcessModules failed");
#endif
	}
}

// This functions is almost taken from Jeffrey Richter's sample, thank you.
bool APIHOOK::ReplaceIATEntryInOneMod( APIHOOK *obj, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller)
{
	if (!hProcess) return false;

	// Get the address of the module's import section
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
		ImageDirectoryEntryToData(hmodCaller, TRUE,
			IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	if (!pImportDesc){
//		DBW("No import section : %s", obj->ModName);
		return false;
	}

	// Find the import descriptor containing references to callee's functions
	for (; pImportDesc->Name; pImportDesc++){
		PSTR pszModName = (PSTR) ((PBYTE) hmodCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, obj->ModName) == 0) 
			break;   // Found
	}

	if (pImportDesc->Name == 0){
//		DBW("This module doesn't import any functions from this callee");
		return false;  // This module doesn't import any functions from this callee
	}

   // Get caller's import address table (IAT) for the callee's functions
	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) 
		((PBYTE) hmodCaller + pImportDesc->FirstThunk);

	// Replace current function address with new function address
	for (; pThunk->u1.Function; pThunk++){
		// Get the address of the function address
		PROC* ppfn = (PROC*) &pThunk->u1.Function;
		// Is this the function we're looking for?
		BOOL fFound = (*ppfn == pfnCurrent);
		if (!fFound && (*ppfn > MaxAppAddr)){
			// If this is not the function and the address is in a shared DLL, 
			// then maybe we're running under a debugger on Windows 98. In this 
			// case, this address points to an instruction that may have the 
			// correct address.
			PBYTE pbInFunc = (PBYTE) *ppfn;
			if (pbInFunc[0] == cPushOpCode){
				// We see the PUSH instruction, the real function address follows
				ppfn = (PROC*) &pbInFunc[1];

				// Is this the function we're looking for?
				fFound = (*ppfn == pfnCurrent);
			}
		}

		if (fFound){
			// The addresses match, change the import section address
			if (!WriteProcessMemory(hProcess, ppfn, &pfnNew, sizeof(pfnNew), NULL)){
				if (SetWriteProtect( (LPVOID)ppfn, false )==~0){	// write protection release
					DBW("SetWriteProtect failed: %08X %s %s", ppfn, obj->ModName, obj->FuncName);
					return false;
				}
#if USE_VXD
				if (!WindowsNT && IsBadWritePtr(ppfn,sizeof(pfnNew))){
					// Windows9x shared DLL section
					// change memory attribte using VxD
					if (!ChangeMemory( ppfn, &pfnNew, sizeof(pfnNew) )){
						DBW("ChangeMemory failed");
					}
				} else
#endif
				{
					if (!WriteProcessMemory(hProcess, ppfn, &pfnNew, sizeof(pfnNew), NULL)){
						//SetWriteProtect( ppfn, true );	// restore protection
						DBW("%08X %08X %08X %08X %s %s", hProcess, ppfn, &pfnNew, sizeof(pfnNew), obj->ModName, obj->FuncName );
						DBW("WriteProcessMemory failed : %d",GetLastError());
						return false;
					}
				}
			}
			return true;  // We did it, get out
		}
	}

	//DBW("Not found");

	// If we get to here, the function is not in the caller's import section
	return false;
}
bool APIHOOK::HookAgain()
{
	if ( Hooked ) return true;

	// Set assembler code for hooking at the org entry
	if ( !StoreCode() ){
		DBW("Cannot write process memory@rehook");
		return false;
	}

	Hooked = true;
	return true;
}
void APIHOOK::Finish()
{
	if ( !Hooked ) return;
	RestoreCode();
	Hooked = false;
}
// Returns the HMODULE that contains the specified memory address
static HMODULE ModuleFromAddress(PVOID pv)
{
	MEMORY_BASIC_INFORMATION mbi;
	return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) 
		? (HMODULE) mbi.AllocationBase : NULL);
}

#if HOOK_PAINT || RETRYMETA
APIHOOK BeginPaintHook( STR_USER32, STR_BeginPaint, _BeginPaint );
APIHOOK EndPaintHook( STR_USER32, STR_EndPaint, _EndPaint );
#endif

#if HOOK_BITBLT
APIHOOK BitBltHook( STR_GDI32, STR_BitBlt, _BitBlt );
#endif

#if HOOK_GETDC
APIHOOK GetDCHook( STR_USER32, STR_GetDC, _GetDC );
APIHOOK ReleaseDCHook( STR_USER32, STR_ReleaseDC, _ReleaseDC );
#endif

#if METAEXTTEXTOUT
APIHOOK ExtTextOutAHook( STR_GDI32, STR_ExtTextOutA, _ExtTextOutA );
APIHOOK ExtTextOutWHook( STR_GDI32, STR_ExtTextOutW, _ExtTextOutW );
#if HOOK_TEXT
APIHOOK TabbedTextOutAHook( STR_USER32, STR_TabbedTextOutA, _TabbedTextOutA );
APIHOOK TabbedTextOutWHook( STR_USER32, STR_TabbedTextOutW, _TabbedTextOutW );
APIHOOK TextOutAHook( STR_GDI32, STR_TextOutA, _TextOutA );
APIHOOK TextOutWHook( STR_GDI32, STR_TextOutW, _TextOutW );
APIHOOK DrawTextAHook( STR_USER32, STR_DrawTextA, _DrawTextA );
APIHOOK DrawTextWHook( STR_USER32, STR_DrawTextW, _DrawTextW );
APIHOOK DrawTextExAHook( STR_USER32, STR_DrawTextExA, _DrawTextExA );
APIHOOK DrawTextExWHook( STR_USER32, STR_DrawTextExW, _DrawTextExW );
#endif
#endif	// METAEXTTEXTOUT

__declspec(dllexport)
bool WINAPI Init( HWND _hwnd, const char * /*module_name*/, bool windowsnt, const char *vxd_path )
{
	DBW("Init:%08X %d", _hwnd, windowsnt);
	hwndCallback = _hwnd;
	if ( Initialized ){
		return true;
	}
#if USE_VXD
	WindowsNT = windowsnt;
	if (vxd_path){
//		DBW("vxd_path=%s",vxd_path);
		lstrcpy( VxDpathName, "\\\\.\\" );
		lstrcat( VxDpathName, vxd_path );
		if (!LoadVxD()){
			DBW("Cannot open VxD:%s:%d", VxDpathName,GetLastError() );
			return false;
		}
	}
#endif

//	HINSTANCE hDll = GetModuleHandle( module_name );
	hMouseHook = SetWindowsHookEx( WH_MOUSE, (HOOKPROC)MouseProc, hInstance, NULL );
	if (!hMouseHook){
		DBW("SetWindowsHookEx error: %d", GetLastError());
		return false;
	}

//	hKeyHook = SetWindowsHookEx( WH_KEYBOARD, (HOOKPROC)KeyboardProc, hDll, NULL );

	Initialized = true;

#if GUARD
	PROTECT_SHARE();
	DBW("%08X(%08X) Protected", (((INT_PTR)&ExtTextOutAHook) / siPageSize) * siPageSize,&ExtTextOutAHook);
#endif

	idDokopopProcess = GetCurrentProcessId();

	// get AMODI infomation.
	if (!tryAMODI){
		tryAMODI = true;
		hwndAMODI = FindAMODI();
#if 0
		if (hwndAMODI){
			DWORD procId;
			if (GetWindowThreadProcessId(hwnd, &procId)){
				HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, procId);
				if (hProc){
					memset(AMODIPath, 0, sizeof(AMODIPath));
					if (GetModuleFileNameEx(hProc, NULL, AMODIPath, sizeof(AMODIPath))>0){
						// OK
					}
					CloseHandle(hProc);
				}
			}
		}
#endif
	}

	return true;
}

__declspec(dllexport)
void WINAPI Uninit()
{
	UNPROTECT_SHARE();
	if ( !Initialized ) return;

	DBW("Uninit");
	UnhookWindowsHookEx( hMouseHook );

//	UnhookWindowsHookEx( hKeyHook );

#if USE_VXD
	UnloadVxD();
#endif

	Initialized = false;
}
__declspec(dllexport)
int WINAPI Config( int clickonly, int keyaction, int keyflag )
{
	DBW("Config:%d %d %d", clickonly, keyaction, keyflag);
	if (clickonly!=-1)
		ClickOnlyEnabled = int_bool(clickonly);
	if ( keyflag != 0 ){
		for ( int i=0;KeyConfig[i].Action;i++ ){
			if (KeyConfig[i].Action == keyaction){
				KeyConfig[i].Flag = keyflag;
				break;
			}
		}
	}
	return 0;
}
__declspec(dllexport)
int WINAPI Config2( const struct TDCHConfig *cfg )
{
	DBW("Config2: %d %d %d %d %d", (int)cfg->ScaleX, (int)cfg->ScaleY, (int)cfg->UseAMODI, (int)ExtAMODI, cfg->UseNumPrev ? cfg->NumPrevWords : 1);

	MoveSend = cfg->MoveSend ? true : false;
	MoveSent = false;
	OnlyImage = cfg->OnlyImage;
	ScaleX = cfg->ScaleX;
	ScaleY = cfg->ScaleY;
	NumPrevWords = cfg->UseNumPrev ? cfg->NumPrevWords : 1;

	RequireHwndAMODI = false;

	if (cfg->UseAMODI){
		if (!ExtAMODI){
			RequireHwndAMODI = true;
			if (!hwndAMODI)
				hwndAMODI = FindAMODI();
		}
	} else {
		hwndAMODI = NULL;
		ExtAMODI = false;
		return 0;
	}

	OnlyAMODI = int_bool(cfg->OnlyAMODI);
	AMODIPath[0] = '\0';
	ExtAMODI = false;
	if (cfg->AMODIPath[0]){
		size_t len = strlen(cfg->AMODIPath);
		if (len<sizeof(AMODIPath)-2){
			memcpy(AMODIPath, cfg->AMODIPath, len);
			if (AMODIPath[len-1]!='\\'){
				AMODIPath[len] = '\\';
				len++;
			}
			AMODIPath[len] = '\0';
			ExtAMODI = true;
		}
	}
	dbw("Config2[%d]: %d %d %s", ++generation, cfg->ScaleX, cfg->ScaleY, AMODIPath);
	return 0;
}
//Note:
//	non_block = trueの場合
//		・wait状態にあるかどうかは、WaitForCaptureResult( false, 0 ) < 0 でわかる。
//		・必ず最後にはWaitForCaptureResult(1,...)を呼び出すこと。
__declspec(dllexport)
int WINAPI Capture( HWND hwnd, POINT *ppt, bool movesend, bool non_block )
{
	DBW("Capture:%08X %d,%d %d %d", hwnd, ppt ? ppt->x : 0, ppt ? ppt->y : 0, movesend, non_block);
	if (!hwnd)
		hwnd = hwndLast;
	POINT pt;
	if (ppt)
		pt = *ppt;
	else
		pt = PrevCursorPoint;
	const bool image_only = true;
	const bool runOnLaunchedProc = false;
	return int_bool(DoCapture(hwnd, pt, movesend, image_only, runOnLaunchedProc, non_block));
}
//Note:
// waittimeはCapture()開始してからの経過時間
// pollingする場合はwaittimeを少しずつ増やす必要あり(その場合はsend_text=0)
// CaptureImageを行った場合は、「必ず」WaitForCaptureResult( 1, ... )を呼び出す必要がある。
// そうしないと、CaptureImageBlockingが true になったまま。
//
// return:
//	0 : wait中のものはなし
//	1 : OK(text ready)
//	-1: timeout
__declspec(dllexport)
int WINAPI WaitForCaptureResult( bool send_text, unsigned waittime )
{
	DBW("WaitForCaptureResult: %d %d %d", send_text, waittime, CaptureImageBlocking);
	if (!CaptureImageBlocking) return 0;

	if (send_text){
		if (WaitAndGetResult(ImageTextPath, waittime)){
			return 1;
		}
	} else {
		HANDLE hf = WaitForResult(ImageTextPath, waittime);
		if (hf){
			CloseHandle(hf);
			return 1;
		}
	}
	return -1;
}
#if GUARD
void ShareProtect( bool f )
{
	LPVOID BaseAddress = (LPVOID)((((INT_PTR)&ExtTextOutAHook) / siPageSize) * siPageSize);

	DWORD OldProtect;
	if ( !VirtualProtect( BaseAddress, siPageSize, f ? PAGE_READONLY : PAGE_READWRITE, &OldProtect ) ){
		DBW("Cannot change protect:%08X@rehook",BaseAddress);
	}
}
#endif
DWORD SetWriteProtect( LPVOID addr, bool f )
{
	LPVOID BaseAddress = (LPVOID)((((INT_PTR)addr) / siPageSize) * siPageSize);

	DWORD OldProtect;
	if ( !VirtualProtect( BaseAddress, siPageSize, f ? PAGE_READONLY : PAGE_READWRITE, &OldProtect ) ){
		DBW("Cannot change protect:%08X-%08X@rehook %d", (INT_PTR)BaseAddress>>32, BaseAddress, GetLastError());
		return ~0;
	}
	return OldProtect;
}
#if USE_VXD
// change write protected memory for Windows9x
bool ChangeMemory( void *dst, const void *src, unsigned size )
{
	DWORD DIOC_count;
	if ( vxd_Handle == INVALID_HANDLE_VALUE ){
		if (!LoadVxD())
			return false;
	}

	bool r = int_bool(DeviceIoControl( vxd_Handle, DIOC_CHANGE_MEMORY, (void*)src, size, dst, 0, &DIOC_count, NULL));
#if USE_DBW
	if ( !r ){
		DBW("DeviceIoControl failed");
	}
#endif
	return r;
}
bool LoadVxD()
{
	if (vxd_Handle!=INVALID_HANDLE_VALUE){
		DBW("Already loaded VxD");
		return true;
	}
	vxd_Handle=CreateFile(&VxDpathName[0],0,0,NULL,0,FILE_FLAG_DELETE_ON_CLOSE,NULL);
	return vxd_Handle != INVALID_HANDLE_VALUE;
}
void UnloadVxD()
{
	if (vxd_Handle == INVALID_HANDLE_VALUE)
		return;
	CloseHandle(vxd_Handle);
	vxd_Handle = INVALID_HANDLE_VALUE;
}
#endif
int GetKeyFlag( )
{
	int r = 0;
	if ( GetKeyState( VK_SHIFT ) & 0x8000 ) r |= KF_SHIFT;
	if ( GetKeyState( VK_CONTROL ) & 0x8000 ) r |= KF_CONTROL;
	if ( GetKeyState( VK_MENU ) & 0x8000 ) r |= KF_MENU;
	if ( GetKeyState( VK_LWIN ) & 0x8000 ) r |= KF_LWIN;
	if ( GetKeyState( VK_RWIN ) & 0x8000 ) r |= KF_RWIN;
	if ( ClickOnlyEnabled && r == 0 ) r = KF_CLICKONLY;
//	if ( GetKeyState( VK_APPS ) & 0x8000 ) r |= KF_APPS;
	return r;
}
LRESULT CALLBACK MouseProc( int code, WPARAM wParam, LPARAM lParam )
{
	if ( code < 0 || code != HC_ACTION )
		return CallNextHookEx( hMouseHook, code, wParam, lParam );

	MOUSEHOOKSTRUCT *mhs;
	HWND hwnd;
	int kf;
	int i;

#if 0
	if (wParam!=0x200 && wParam!=0xA0)
		dbw("WParam:%08X", wParam);
#endif

	curProcess = GetCurrentProcessId();
	bool runOnLaunchedProc = false;

	switch ( wParam ){
		case WM_LBUTTONDOWN:
		// case WM_NCLBUTTONDOWN:
			LButtonDown = true;
			LButtonUpCancel = false;
//			if (LButtonUpCancel)
//				goto j_end;
			kf = KF_LEFTCLICK;
			goto j_rbuttondown;
		case WM_MBUTTONDOWN:
		// case WM_NCMBUTTONDOWN:
			MButtonDown = true;
			MButtonUpCancel = false;
//			if (MButtonUpCancel)
//				goto j_end;
			kf = KF_MIDCLICK;
			goto j_rbuttondown;
		case WM_RBUTTONDOWN:
		//  case WM_NCRBUTTONDOWN:
			DBW("RButtonDown");
			RButtonUpCancel = false;
//			if (RButtonUpCancel)
//				goto j_end;
			kf = 0;
			if (LButtonDown){
				kf = KF_LRCLICK;
			}
	j_rbuttondown:;
			mhs = (MOUSEHOOKSTRUCT*)lParam;
			hwnd = WindowFromPoint( mhs->pt );
			//hwnd = mhs->hwnd;
#if 0
			HWND h = GetParent( hwnd );
			if ( h )
				hwnd = h;
#endif
			kf |= GetKeyFlag();

#if 0
			dbw("kf=%02X", kf);
#endif

			if (curProcess == idDokopopProcess){
				// runs on launched process -> ignored.
				DBW("Runs on launched proc(%d)", curProcess);
				runOnLaunchedProc = true;
			}

			for ( i=0;KeyConfig[i].Action;i++ ){
				//DBW("%d:%d %x", i, KeyConfig[i].Action, KeyConfig[i].Flag);
				if ( kf == KeyConfig[i].Flag ){
					switch ( KeyConfig[i].Action ){
						case KA_POPUP:
						case KA_POPUP_NC:
							{
							const bool move_send = false;
							if (DoCapture(hwnd, mhs->pt, move_send, OnlyImage, runOnLaunchedProc)){
j_discard:
								if (wParam == WM_LBUTTONDOWN)
									LButtonUpCancel = true;
								if (wParam == WM_MBUTTONDOWN)
									MButtonUpCancel = true;
								else
									RButtonUpCancel = true;
								return 1;	// discard this message
							}
							}
							break;
						case KA_MENU:
							DokoPopMenu( hwnd );
							goto j_discard;
						case KA_TOGGLE:
							DBW("KA_TOGGLE");
							ToggleClick( hwnd );
							goto j_discard;
					}
				}
			}
			break;
		case WM_RBUTTONUP:
			DBW("WM_RBUTTONUP:%d",RButtonUpCancel);
			if ( RButtonUpCancel ){
				RButtonUpCancel = false;
				return 1;	// no longer process the message
			}
			break;
		case WM_LBUTTONUP:
			LButtonDown = false;
			if ( LButtonUpCancel ){
				LButtonUpCancel = false;
				return 1;	// no longer process the message
			}
			break;
		case WM_MBUTTONUP:
			MButtonDown = false;
			if ( MButtonUpCancel ){
				MButtonUpCancel = false;
				return 1;	// no longer process the message
			}
			break;
		case WM_MOUSEMOVE:
			if (MoveSend){
				mhs = (MOUSEHOOKSTRUCT*)lParam;
				hwnd = WindowFromPoint( mhs->pt );
				if (PrevCursorPoint.x!=mhs->pt.x || PrevCursorPoint.y!=mhs->pt.y){
					PrevMoveTime = GetTickCount();
					MoveSent = false;
					PrevCursorPoint = mhs->pt;
					hwndLast = hwnd;
					//DBW("Moving:%d %d", mhs->pt.x, mhs->pt.y);
					SendMoveMessage();
#if 0
					//TODO: 5pixel未満であればDoCaptureする
#endif
				} else {
					if (!MoveSent){
#if 0	// 2014.11.18 inc.srchで落ちるのでとりあえずcomment out
						const int MOVE_SEND_INTERVAL = 50;	// msec
						if (GetTickCount()-PrevMoveTime > MOVE_SEND_INTERVAL){
							MoveSent = true;
							if (DoCapture(hwndLast, PrevCursorPoint, true)){
								// stop the capture from the main.
								SendCancelMove();
							}
						}
#endif
					}
				}
			}
			break;
	}

//j_end:;
	return CallNextHookEx( hMouseHook, code, wParam, lParam );
}

bool DoCapture(HWND hwnd, POINT pt, bool movesend, bool image_only, bool runOnLaunchedProc, bool non_block)
{
	DBW("DoCapture: %d %d %d %d %d", image_only, runOnLaunchedProc, hwndAMODI, ExtAMODI, OnlyAMODI);
	CursorPoint = ScreenPoint = pt;
	ScreenToClient( hwnd, &CursorPoint );

#if 1	// acrobat reader上でclickするとhwndAMODIがnullになってしまう場合があるため（原因不明）
		// ・reader上ではmouse eventがピタッと止まる
		// ・reader上でclickしただけではnullにならない
		// ・reader上でctrl+clickすると、reader上からmouseを外すとnullになっている
		// ・reader上でshift+click, alt+click, ctrl+clickではnullにならない
		// ・設定をAlt+clickに変更すると、Alt+clickでnullになり、Ctrl+clickではnullにならない
		// まるでDokoPop!対策をしているかのようだ。いずれにせよ、mouse eventがまったく来ない、
		// おそらくreaderのほうでmouse hookを呼ばないようにしているのだろう、
		// なので、null原因が仮にわかったとしても、reader上では検索ができないはず
	if (RequireHwndAMODI){
		if (!hwndAMODI)
			hwndAMODI = FindAMODI();
	}
#endif

	if (!image_only && !runOnLaunchedProc){
		//DBW("hwnd:%08X %d %d", (int)hwnd, CursorPoint.x, CursorPoint.y);
		if ((!hwndAMODI && !ExtAMODI) || !OnlyAMODI){
			// Redraw Metafile //
			UNPROTECT_SHARE();
			DBW("Target : %08X %d", hwnd, curProcess);
			bool r = CaptureText( hwnd, movesend );
			PROTECT_SHARE();
			if (r){
				return true;	// discard this message
			}
		}
	}
	if (image_only || hwndAMODI || ExtAMODI){
		if (CaptureImage(hwnd, movesend, non_block))
			return true;
	}
	return false;
}

void CallbackMain(int msg, const void *data, int len, HWND wparam=0)
{
	COPYDATASTRUCT cds;
	cds.dwData = msg;
	cds.cbData = len;
	cds.lpData = (LPSTR)data;
	SendMessage( hwndCallback, WM_COPYDATA, (WPARAM)wparam, (LPARAM)&cds );
}

void DokoPopMenu( HWND hwnd )
{
	CallbackMain(DCH_MENU, NULL, 0, hwnd);
}
void ToggleClick( HWND hwnd )
{
	CallbackMain(DCH_TOGGLE, NULL, 0, hwnd);
}
#if 0
LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam )
{
	if ( code < 0 || code != HC_ACTION )
		return CallNextHookEx( hKeyHook, code, wParam, lParam );

	if ( wParam == VK_CONTROL ){
		if ( lParam & (KF_UP<<16) ){
			// control key up
			// -> hold on time
			goto j1;
		} else
		if ( !(lParam & (KF_REPEAT<<16)) ){
			// control key down and not repeat
//			DBW("wparam=%08X lparam=%08X",wParam,lParam);
			long nowt = GetTickCount();
			if ( nowt - LastTypeTime < GetDoubleClickTime() ){
//				DBW("On/off!");
				ClickOnlyEnabled = !ClickOnlyEnabled;
				MessageBeep( MB_OK );
				// -> cancel time
			} else {
				// -> hold on time
				LastTypeTime = nowt;
				goto j1;
			}
		} else {
			// control key down and repeat
			// -> cancel time
		}
	}
	LastTypeTime -= 10000;
j1:;
	return CallNextHookEx( hMouseHook, code, wParam, lParam );
}
#endif

//__declspec(dllexport)
#if HOOK_PAINT || RETRYMETA
HDC WINAPI _BeginPaint( HWND hwnd, LPPAINTSTRUCT ps )
{
//	DBW("BeginPaint");
	HDC hdc = ((FNBeginPaint)BeginPaintHook.OrgFunc)( hwnd, ps );
	OrgHDC = hdc;
	ps->hdc = hdcMeta;
#if 1
	ps->rcPaint.left = 0;
	ps->rcPaint.top = 0;
	ps->rcPaint.right = rcInvalid.right;
	ps->rcPaint.bottom = rcInvalid.bottom;
#endif
	return hdcMeta;
}
//__declspec(dllexport)
BOOL WINAPI _EndPaint( HWND hwnd, CONST PAINTSTRUCT *ps )
{
	((PAINTSTRUCT*)ps)->hdc = OrgHDC;
	return ((FNEndPaint)EndPaintHook.OrgFunc)( hwnd, ps );
}
#endif
#if HOOK_GETDC
HDC WINAPI _GetDC( HWND hwnd )
{
	DBW("GetDC");
	HDC hdc = ((FNGetDC)GetDCHook.OrgFunc)( hwnd );
	OrgHDC = hdc;
	if ( hwnd == hwndOrg ){
		return hdcMeta;
	} else {
		return OrgHDC;
	}
}
int WINAPI _ReleaseDC( HWND hwnd, HDC hdc )
{
	DBW("ReleaseDC");
	if ( hdc == hdcMeta ){
		hdc = OrgHDC;
	}
	int r = ((FNReleaseDC)ReleaseDCHook.OrgFunc)( hwnd, hdc );
	return r;
}
#endif

#if METAEXTTEXTOUT
//__declspec(dllexport)
BOOL  WINAPI _ExtTextOutA(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCSTR str, UINT count, CONST INT *dx )
{
	DBW("_ExtTextOutA");
	POINT pt;
	GetCurrentPositionEx(hdc,&pt);

	BOOL r = ((FNExtTextOutA)ExtTextOutAHook.OrgFunc)( hdc, x, y, option, rc, str, count, dx );

	hdcExtTextOut = hdc;

	if ( r
#if USE_META
		&& hdcMeta != hdc
#endif
		&& count > 0 ){
		RECT _rc;
		if ( rc ){
			_rc.right = rc->right;
			_rc.bottom = rc->bottom;
		} else {
			_rc.right = rcClient.right;
			_rc.bottom = rcClient.bottom;
		}
		if ( GetTextAlign( hdc ) & TA_UPDATECP ){
			_rc.left = pt.x;
			_rc.top = pt.y;
		} else {
			_rc.left = x;
			_rc.top = y;
		}
		if ( RetryPhase ){
			if ( TargetBitBltCount == BitBltCount ){
				_rc.right += CursorPoint.x;	//*+++ bug こうしないと幅がせまくてhitしない
				OffsetRect( &_rc, TargetOffset.x, TargetOffset.y );
			} else {
				return r;
			}
		} else {
			ExtTextOutCount++;
		}
#if ORG_OFFSET	/* fixed 2001.1.14 for IE@win2k */
		POINT vpt;
		POINT wpt;
		GetViewportOrgEx( hdc, &vpt );
		GetWindowOrgEx( hdc, &wpt );
		OffsetRect( &_rc, vpt.x-wpt.x, vpt.y-wpt.y);
#else
#if USE_DBW
		POINT vpt;
		POINT wpt;
		GetViewportOrgEx( hdc, &vpt );
		GetWindowOrgEx( hdc, &wpt );
		if (vpt.x != wpt.x || vpt.y != wpt.y)
			DBW("!!!Org DIFF=%d %d",vpt.x-wpt.x, vpt.y-wpt.y);
#endif
#endif

		// 連続したテキストの場合は
		// 連結する
		if (LastY == _rc.top && LastX == _rc.left ){
			// 連続したテキスト描画→連結
			LastTextA->Cat( str, count );
		} else {
			LastTextA->Set( str, count );
		}
		LastX = _rc.left;
		LastY = _rc.top;

		if ( PtInRect( &_rc, CursorPoint ) )
		{
#if DEBUG_HITTEXT
			char *buf = new char[ count + 1 ];
			CopyMemory( buf, str, count );
			buf[count] = '\0';
			DBW("CheckA -- %d %d %d %d (%d,%d)", _rc.left, _rc.top, _rc.right, _rc.bottom, CursorPoint.x, CursorPoint.y, count);
			DBW("%s", buf);
			delete[] buf;
#endif
			int loc = GetTextFromPoint( hdc, str, count, CursorPoint.x - _rc.left, CursorPoint.y - _rc.top, dx );
			if ( loc != -1 ){
				FoundLocA = loc + LastTextA->GetLength() - count;
				FoundTextA->Set( LastTextA );
#if DEBUG_HITTEXT
				DBW("HitA loc=%d Len=%d Text=%ws", FoundLocA, FoundTextA->GetLength(), FoundTextA->c_str());
#endif
			}
		}
	}

	return r;
}
//__declspec(dllexport)
BOOL  WINAPI _ExtTextOutW(HDC hdc, int x, int y, UINT option, CONST RECT *rc, LPCWSTR str, UINT count, CONST INT *dx )
{
//	DBW("_ExtTextOutW");
	POINT pt;
	GetCurrentPositionEx(hdc,&pt);

	BOOL r = ((FNExtTextOutW)ExtTextOutWHook.OrgFunc)( hdc, x, y, option, rc, str, count, dx );

	hdcExtTextOut = hdc;

	if ( r
#if USE_META
		&& hdcMeta != hdc
#endif
		&& count > 0 ){
		RECT _rc;
		if ( rc ){
			_rc.right = rc->right;
			_rc.bottom = rc->bottom;
		} else {
			_rc.right = rcClient.right;
			_rc.bottom = rcClient.bottom;
		}
		if ( GetTextAlign( hdc ) & TA_UPDATECP ){
			_rc.left = pt.x;
			_rc.top = pt.y;
		} else {
			_rc.left = x;
			_rc.top = y;
		}
		if ( RetryPhase ){
			if ( TargetBitBltCount == BitBltCount ){
				_rc.right += CursorPoint.x;	//*+++ bug こうしないと幅がせまくてhitしない
				OffsetRect( &_rc, TargetOffset.x, TargetOffset.y );
			} else {
				return r;
			}
		} else {
			ExtTextOutCount++;
		}
#if ORG_OFFSET	/* fixed 2001.1.14 for IE@win2k */
		POINT vpt;
		POINT wpt;
		GetViewportOrgEx( hdc, &vpt );
		GetWindowOrgEx( hdc, &wpt );
		OffsetRect( &_rc, vpt.x-wpt.x, vpt.y-wpt.y);
#else	// !ORG_OFFSET
#if USE_DBW
		POINT vpt;
		POINT wpt;
		GetViewportOrgEx( hdc, &vpt );
		GetWindowOrgEx( hdc, &wpt );
		if (vpt.x != wpt.x || vpt.y != wpt.y)
			DBW("!!!Org DIFF=%d %d",vpt.x-wpt.x, vpt.y-wpt.y);
#endif	// USE_DBW
#endif	// !ORG_OFFSET

		// 連続したテキストの場合は連結する
		if (LastY == _rc.top && LastX == _rc.left ){
			// 連続したテキスト描画→連結
			LastTextW->Cat( str, count );
		} else {
			LastTextW->Set( str, count );
		}
		LastX = _rc.left;
		LastY = _rc.top;

		if ( PtInRect( &_rc, CursorPoint ) )
		{
			DBW("%04X %04X %04X %04X",
				str[0],
				str[1],
				str[2],
				str[3]);
#if DEBUG_HITTEXT
			wchar_t *buf = new wchar_t[ count + 1 ];
			CopyMemory( buf, str, count*sizeof(wchar_t) );
			buf[count] = '\0';
			DBW("CheckW -- %d %d %d %d (%d,%d)", _rc.left, _rc.top, _rc.right, _rc.bottom, CursorPoint.x, CursorPoint.y, count);
			DBW("%ws", buf);
			delete[] buf;
#endif
			int loc = GetTextFromPoint( hdc, str, count, CursorPoint.x - _rc.left, CursorPoint.y - _rc.top, dx );
			if ( loc != -1 ){
				FoundLocW = loc + LastTextW->GetLength() - count;
				FoundTextW->Set( LastTextW );
#if DEBUG_HITTEXT
				DBW("HitW loc=%d Len=%d Text=%ws", FoundLocW, FoundTextW->GetLength(), FoundTextW->c_str());
#endif
			}
		}
	}

	return r;
}
#endif	// !METAEXTTEXTOUT

#if HOOK_BITBLT
BOOL WINAPI _BitBlt(HDC hdcdest, int xdest, int ydest, int width, int height, HDC hdcsrc, int xsrc, int ysrc, DWORD rop )
{
	DBW("BitBlt:%08X %d %d %d %d <- %08X %d %d", hdcdest, xdest, ydest, width, height, hdcsrc, xsrc, ysrc );
	BOOL r = ((FNBitBlt)BitBltHook.OrgFunc)(hdcdest, xdest, ydest, width, height, hdcsrc, xsrc, ysrc, rop );
//	DBW("BitBlt:%08X %d %d %d %d <- %08X %d %d", hdcdest, xdest, ydest, width, height, hdcsrc, xsrc, ysrc );
	if ( r && hdcExtTextOut == hdcsrc ){
		if ( !RetryPhase ){
			// 今回のBitBlt時にExtTextOutがあったか？
			if ( ExtTextOutCount > 0 ){
				// CursorPointが含まれるか？
				DBW("ExtTextOutCount=%d",ExtTextOutCount);
				if ( (xdest <= CursorPoint.x)
					&& (xdest+width > CursorPoint.x)
					&& (ydest <= CursorPoint.y)
					&& (ydest+height > CursorPoint.y)
				){
					TargetBitBltCount = BitBltCount;
					TargetOffset.x = xdest - xsrc;
					TargetOffset.y = ydest - ysrc;
					DBW("TargetOffset %d %d (%d,%d %dx%d)<-(%d,%d)",
						TargetOffset.x, TargetOffset.y,xdest,ydest,width,height,xsrc,ysrc);
#if USE_DBW
					{
					POINT vpt;
					POINT wpt;
					GetViewportOrgEx( hdcdest, &vpt );
					GetWindowOrgEx( hdcdest, &wpt );
					DBW("BitBlt OrgDIFF=%d %d",vpt.x-wpt.x, vpt.y-wpt.y);
					SIZE sz1;
					SIZE sz2;
					GetViewportExtEx( hdcdest, &sz1 );
					GetWindowExtEx( hdcdest, &sz2 );
					DBW("BitBlt Ext %d (%d,%d) (%d,%d)", GetMapMode(hdcdest), sz1.cx, sz1.cy, sz2.cx, sz2.cy );
#if 1
					XFORM xform;
					GetWorldTransform( hdcdest, &xform );
					{
					char buf[100];
					sprintf(buf,"== [%d] %g %g %g %g %g %g", GetGraphicsMode(hdcdest), xform.eM11, xform.eM12, xform.eM21, xform.eM22, xform.eDx, xform.eDy);
					DBW(buf);
					}
#endif
					}
#endif
				}
#if METAEXTTEXTOUT
				if ( TargetBitBltCount != -1 ){
					// すでにほかのbandでhitしている場合、
					// Cancel ExtTextOut
					FoundLocA = -1;
					FoundLocW = -1;
				}
#endif
			}
		}
		BitBltCount++;
		DBW("BitBltCount=%d",BitBltCount);
		ExtTextOutCount = 0;
		return r;
	} else {
//		DBW("Unknown destination BitBlt %08X(%d,%d %dx%d)<-%08X(%d,%d)",hdcdest,xdest,ydest,width,height,hdcsrc,xsrc,ysrc);
	}
	return r;
}
#endif

#if HOOK_TEXT
#define	HK1( name, ret_type, args ) \
	ret_type r = ((FN##name)name##Hook.OrgFunc) args; \
	return r
LONG WINAPI _TabbedTextOutA( HDC hdc, int x, int y, LPCSTR str, int count, int ntabs, LPINT tabs, int origin )
{
	DBW("_TabbedTextOutA");
	HK1( TabbedTextOutA, LONG, ( hdc, x, y, str, count, ntabs, tabs, origin ) );
}
LONG WINAPI _TabbedTextOutW( HDC hdc, int x, int y, LPCWSTR str, int count, int ntabs, LPINT tabs, int origin )
{
	DBW("TabbedTextOutW");
	HK1( TabbedTextOutW, LONG, ( hdc, x, y, str, count, ntabs, tabs, origin ) );
}
BOOL WINAPI _TextOutA( HDC hdc, int x, int y, LPCSTR str, int count )
{
	DBW("_TextOutA");
	HK1( TextOutA, BOOL, ( hdc, x, y, str, count ) );
}
BOOL WINAPI _TextOutW( HDC hdc, int x, int y, LPCWSTR str, int count )
{
	DBW("_TextOutW");
	HK1( TextOutW, BOOL, ( hdc, x, y, str, count ) );
}
int WINAPI _DrawTextA( HDC hdc, LPCSTR str, int count, LPRECT rc, UINT format )
{
	DBW("s5");
	HK1( DrawTextA, int, ( hdc, str, count, rc, format ) );
}
int WINAPI _DrawTextW( HDC hdc, LPCWSTR str, int count, LPRECT rc, UINT format )
{
	DBW("s6");
	HK1( DrawTextW, int, ( hdc, str, count, rc, format ) );
}
int WINAPI _DrawTextExA( HDC hdc, LPCSTR str, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params )
{
	DBW("s7");
	HK1( DrawTextExA, int, ( hdc, str, count, rc, format, params ) );
}
int WINAPI _DrawTextExW( HDC hdc, LPCWSTR str, int count, LPRECT rc, UINT format, LPDRAWTEXTPARAMS params )
{
	DBW("s8");
	HK1( DrawTextExW, int, ( hdc, str, count, rc, format, params ) );
}
#endif	// HOOK_TEXT

bool CaptureText( HWND hwnd, bool movesend )
{
	if ( !hwnd ){
		return false;
	}

	MoveSendMode = movesend;

	//DoScale(NULL, &CursorPoint, 1);

#if METAEXTTEXTOUT
	FoundLocA = -1;
	FoundLocW = -1;
	FoundTextA = new TString<char>;
	FoundTextW = new TString<wchar_t>;
	LastTextA = new TString<char>;
	LastTextW = new TString<wchar_t>;
#endif

#if !USE_REDRAW
	HRGN hRgn;
#endif

	GetClientRect( hwnd, &rcClient );
	GetUpdateRect( hwnd, &rcOrgInvalid, FALSE );

#if USE_OPTIMALINVALID
	SetRect( &rcInvalid, 0, max(CursorPoint.y - 64,0), rcClient.right, max(CursorPoint.y + 64,0) );
#else
	rcInvalid = rcClient;
#endif

#if !USE_REDRAW
	hRgn = CreateRectRgn( rcInvalid.left, rcInvalid.top, rcInvalid.right, rcInvalid.bottom );

#if USE_INVALIDATE
	InvalidateRect( hwnd, &rcInvalid, INVALIDATE_TRUE );
#endif

	SendMessage( hwnd, WM_PAINT, 0, 0 );
	SendMessage( hwnd, WM_NCPAINT, (WPARAM)hRgn, 0 );
#endif

	hwndOrg = hwnd;

#if USE_META
	HDC hdc;
#endif

	HINSTANCE hPSAPI = NULL;
	HMODULE hInst;
	HMODULE hInstGdi;

	hInst = GetModuleHandleA( STR_KERNEL32 );
	fnCreateToolhelp32Snapshot = (FNCreateToolhelp32Snapshot)GetProcAddress( hInst, STR_CreateToolhelp32Snapshot );
	if (fnCreateToolhelp32Snapshot){
		// Toolhelpによるmodule enumrationが使用可能
		fnModule32First = (FNModule32First)GetProcAddress( hInst, STR_Module32First );
		fnModule32Next = (FNModule32Next)GetProcAddress( hInst, STR_Module32Next );
	} else {
		// Toolhelpが存在しない場合
		hPSAPI = LoadLibrary( STR_PSAPI );
		DBW("LoadLibrary : %08X",hPSAPI);
		if (!hPSAPI){
			DBW("Cannot load PSAPI.DLL!!");
			return false;
		}
		fnEnumProcessModules = (FNEnumProcessModules)GetProcAddress( hPSAPI, STR_EnumProcessModules );
		fnGetModuleFileNameExA = (FNGetModuleFileNameExA)GetProcAddress( hPSAPI, STR_GetModuleFileNameExA );
	}

	hInst = GetModuleHandleA( STR_USER32 );
#if USE_DBW
	if ( !hInst ){
		DBW("Cannot load USER32.DLL");
		return false;
	}
#endif
	hInstGdi = GetModuleHandleA( STR_GDI32 );
#if USE_DBW
	if ( !hInstGdi ){
		DBW("Cannot load GDI32.DLL");
		goto err9;
	}
#endif

#if METAEXTTEXTOUT
	if ( !ExtTextOutAHook.GetProcOrgFunc( hInstGdi ) ){
		DBW("Cannot find ExtTextOutA");
		goto err8;
	}
	if ( !ExtTextOutWHook.GetProcOrgFunc( hInstGdi ) ){
		DBW("Cannot find ExtTextOutW");
		goto err7;
	}
#endif

#if HOOK_BITBLT
	if ( !BitBltHook.GetProcOrgFunc( hInstGdi ) ){
		DBW("Cannot find BitBlt");
		goto err71;
	}
#endif
	RetryPhase = false;
	TargetBitBltCount = -1;
	BitBltCount = 0;
	ExtTextOutCount = 0;

#if HOOK_PAINT
	if ( !BeginPaintHook.GetProcOrgFunc( hInst ) ){
		DBW("Cannot hook BeginPaint");
		goto err73;
	}
	if ( !EndPaintHook.GetProcOrgFunc( hInst ) ){
		DBW("Cannot hook EndPaint");
		goto err6;
	}
#endif

#if HOOK_TEXT
	TabbedTextOutAHook.GetProcOrgFunc( hInst );
	TabbedTextOutWHook.GetProcOrgFunc( hInst );
	TextOutAHook.GetProcOrgFunc( hInstGdi );
	TextOutWHook.GetProcOrgFunc( hInstGdi );
	DrawTextAHook.GetProcOrgFunc( hInst );
	DrawTextWHook.GetProcOrgFunc( hInst );
	DrawTextExAHook.GetProcOrgFunc( hInst );
	DrawTextExWHook.GetProcOrgFunc( hInst );
#endif

#if USE_META
	hdc = GetDC( hwnd );
	if ( !hdc ){
		DBW("Cannot get DC");
		goto err5;
	}
#endif

#if HOOK_GETDC
	if ( !GetDCHook.GetProcOrgFunc( hInst ) ){
		DBW("Cannot hook GetDC");
		goto err4;
	}
	if ( !ReleaseDCHook.GetProcOrgFunc( hInst ) ){
		DBW("Cannot hook ReleaseDC");
		goto err3;
	}
#endif

	{
    	DBW("Hooked successfully");
//		rect.right = 9999;
//		rect.bottom = 9999;
#if 0
		int iWidthMM = GetDeviceCaps(hdc, HORZSIZE);
		int iHeightMM = GetDeviceCaps(hdc, VERTSIZE);
		int iWidthPels = GetDeviceCaps(hdc, HORZRES);
		int iHeightPels = GetDeviceCaps(hdc, VERTRES);

		// Use iWidthMM, iWidthPels, iHeightMM, and iHeightPels to determine the
		// number of .01-millimeter units per pixel in the x and y directions.

		int iMMPerPelX = (iWidthMM * 100)/iWidthPels;
		int iMMPerPelY = (iHeightMM * 100)/iHeightPels;
		DBW("-%d %d-",iMMPerPelX, iMMPerPelY);

		// Convert client coordinates to .01-mm units.

		rcClient.left = rcClient.left * iMMPerPelX;
		rcClient.top = rcClient.top * iMMPerPelY;
		rcClient.right = rcClient.right * iMMPerPelX;
		rcClient.bottom = rcClient.bottom * iMMPerPelY;
#endif

#if USE_META
		hdcMeta = CreateEnhMetaFile(hdc,
				  NULL,
				  &rcClient, "DCHook\0EnhMetaFile\0");
		if ( !hdcMeta ){
			DBW("Cannot create Meta file");
			goto err3;
		}

		DBW("hdcMeta=%08X", hdcMeta);
#endif

#if USE_REDRAW
		RedrawWindow( hwnd, &rcInvalid, NULL, RDW_UPDATENOW | RDW_NOERASE | RDW_NOFRAME | RDW_INVALIDATE );
#else
		WNDPROC PaintWndProc = (WNDPROC)GetWindowLong( hwnd, GWL_WNDPROC );
		SendMessage( hwnd, WM_PAINT, (WPARAM)hdcMeta, 0 );
#endif
#if HOOK_GETDC
		ReleaseDCHook.Finish();
		GetDCHook.Finish();
#endif

#if HOOK_TEXT
		TabbedTextOutAHook.Finish( );
		TabbedTextOutWHook.Finish( );
		TextOutAHook.Finish( );
		TextOutWHook.Finish( );
		DrawTextAHook.Finish( );
		DrawTextWHook.Finish( );
		DrawTextExAHook.Finish( );
		DrawTextExWHook.Finish( );
#endif

#if HOOK_PAINT
		EndPaintHook.Finish();
		BeginPaintHook.Finish();
#endif

#if HOOK_BITBLT
		BitBltHook.Finish();
#endif

#if METAEXTTEXTOUT
		ExtTextOutAHook.Finish();
		ExtTextOutWHook.Finish();
#endif

		// Notify start enumrate meta-file
		CallbackMain(DCH_START, &CursorPoint, sizeof(CursorPoint), hwnd);

		HitFound = false;

#if USE_META
		HENHMETAFILE emf = CloseEnhMetaFile( hdcMeta );

		if ( emf )
#endif
		{
#if METAEXTTEXTOUT
			if ( FoundLocA != -1 || FoundLocW != -1 ){
//				DBW("Hit ExtText:%d %d",FoundLocA,FoundLocW);
				// ExtTextOutx hookのほうを優先する
				ExtTextOutHit( );
				HitFound = true;
			} else
#endif
			{
#if USE_META
				// Create compatible device-context for enumration
				HDC _hdc = CreateCompatibleDC( hdc );
				HBITMAP hbitmap = CreateCompatibleBitmap( hdc, rcClient.right, rcClient.bottom );
				HGDIOBJ gdiobj = SelectObject( _hdc, hbitmap );

				DBW("Start meta");
				// Enumerate meta-file
				EnumEnhMetaFile( _hdc, emf, EnumMetaFileProc, NULL, &rcClient );
				DBW("End meta");

				// Delete compatible device-context
				SelectObject( _hdc, gdiobj );
				DeleteObject( hbitmap );
				DeleteDC( _hdc );
#endif
			}
#if USE_META
			DeleteEnhMetaFile( emf );
#endif
		}
#if USE_META
		else {
			DBW("emf is NULL");
		}
#endif

#if METAEXTTEXTOUT
		if ( !HitFound && TargetBitBltCount != -1 ){
			DBW("Search from band:%d",TargetBitBltCount);
#if USE_META
			// Bandから検索
			hdcMeta = CreateCompatibleDC( hdc );
			if ( !hdcMeta ){
				DBW("Cannot create HDC in retry");
			} else
#endif
			{
#if USE_META
				HBITMAP hbitmap = CreateCompatibleBitmap( hdc, rcClient.right, rcClient.bottom );
				HGDIOBJ gdiobj = SelectObject( hdcMeta, hbitmap );
				DBW("hdcMeata=%08X", hdcMeta);
#endif

				BitBltCount = 0;
				RetryPhase = true;
#if METAEXTTEXOUT
				FoundLocA = -1;
				FoundLocW = -1;
#endif

#if METAEXTTEXTOUT
				ExtTextOutAHook.HookAgain();
				ExtTextOutWHook.HookAgain();
#endif
#if HOOK_BITBLT
				BitBltHook.HookAgain();
#endif
#if HOOK_PAINT
				BeginPaintHook.HookAgain();
				EndPaintHook.HookAgain();
#endif

//				DBW("t1:%d %d %d %d",rcClient.left, rcClient.top,rcClient.right,rcClient.bottom);

#if USE_INVALIDATE
				InvalidateRect( hwnd, &rcInvalid, INVALIDATE_TRUE );
#endif

#if USE_REDRAW
				RedrawWindow( hwnd, &rcInvalid, NULL, RDW_UPDATENOW | RDW_NOERASE | RDW_NOFRAME | RDW_INVALIDATE );
#else
				CallWindowProc( (WNDPROC)PaintWndProc, hwnd, WM_PAINT, (WPARAM)hdcMeta, 0 );
#endif

#if HOOK_PAINT
				EndPaintHook.Finish();
				BeginPaintHook.Finish();
#endif
#if HOOK_BITBLT
				BitBltHook.Finish();
#endif

#if METAEXTTEXTOUT
				ExtTextOutAHook.Finish();
				ExtTextOutWHook.Finish();
#endif
#if USE_META
				SelectObject( hdcMeta, gdiobj );
				DeleteObject( hbitmap );
				DeleteDC( hdcMeta );
#endif
				if ( FoundLocA != -1 || FoundLocW != -1 ){
					DBW("Found in BitBlt ExtText:%d %d",FoundLocA,FoundLocW);
					ExtTextOutHit( );
					HitFound = true;
				}
			}
		}
#endif	// METAEXTTEXTOUT

#if RETRYMETA
		if ( !HitFound ){
			HENHMETAFILE emf;
#if !USE_META
			HDC hdc = GetDC( hwnd );
			if ( !hdc ){
				DBW("Cannot get DC");
				goto err01;
			}
#endif
#if HOOK_PAINT
			if ( !BeginPaintHook.HookAgain( ) ){
				DBW("Cannot hook BeginPaint");
				goto err02;
			}
			if ( !EndPaintHook.HookAgain( ) ){
				DBW("Cannot hook EndPaint");
				goto err03;
			}
#else
			if ( !BeginPaintHook.GetProcOrgFunc( hInst ) ){
				DBW("Cannot hook BeginPaint");
				goto err02;
			}
			if ( !EndPaintHook.GetProcOrgFunc( hInst ) ){
				DBW("Cannot hook EndPaint");
				goto err03;
			}
#endif

			hdcMeta = CreateEnhMetaFile(hdc,
					  NULL,
					  &rcClient, _T("DCHook\0EnhMetaFile\0"));
			if ( !hdcMeta ){
				DBW("Cannot create Meta file");
				goto err04;
			}
			DBW("hdcMeta=%08X", hdcMeta);
//			InvalidateRect( hwnd, &rcInvalid, INVALIDATE_TRUE );
#if 1		// 2000.7.15 NTでもなぜこっちにしていなかったのかわからないが・・・
			SendMessage( hwnd, WM_PAINT, (WPARAM)hdcMeta, 0 );
#else
			// Notepadでは異常終了するため却下
			// 何もないところでCtrl+右クリック
			WNDPROC PaintWndProc = (WNDPROC)GetWindowLong( hwnd, GWL_WNDPROC );
			if ( PaintWndProc ){
				PaintWndProc( hwnd, WM_PAINT, (WPARAM)hdcMeta, 0 );
			}
#endif
			EndPaintHook.Finish();
			BeginPaintHook.Finish();

			emf = CloseEnhMetaFile( hdcMeta );

			if ( emf ){
				HDC _hdc = CreateCompatibleDC( hdc );
				HBITMAP hbitmap = CreateCompatibleBitmap( hdc, rcClient.right, rcClient.bottom );
				HGDIOBJ gdiobj = SelectObject( _hdc, hbitmap );

				DBW("Start meta-retry");
				// Enumerate meta-file
				EnumEnhMetaFile( _hdc, emf, EnumMetaFileProc, NULL, &rcClient );
				DBW("End meta-retry");

				// Delete compatible device-context
				SelectObject( _hdc, gdiobj );
				DeleteObject( hbitmap );
				DeleteDC( _hdc );
				DeleteEnhMetaFile( emf );
			}
		err04:
			EndPaintHook.Finish();
		err03:
			BeginPaintHook.Finish();
		err02:
#if !USE_META
			ReleaseDC( hwnd, hdc );
#endif
		err01:;
		}
#endif
		
		// Notify end enumrate meta-file
		CallbackMain(DCH_END | (MoveSendMode ? DCH_MOVESEND : 0), NULL, 0, hwnd);
	}

#if USE_META
	ReleaseDC( hwnd, hdc );
#endif

	if ( rcOrgInvalid.right != 0 ){
		DBW("Update OrgInvalid--------");
		InvalidateRect( hwnd, NULL, INVALIDATE_TRUE );
	}

	goto jreturn;

#if HOOK_GETDC || USE_META
err3:;
#endif
#if HOOK_GETDC
	GetDCHook.Finish();
err4:;
#if USE_META
	ReleaseDC( hwnd, hdc );
#endif
#endif
#if USE_META
err5:;
#endif
#if HOOK_PAINT
	EndPaintHook.Finish();
err6:;
	BeginPaintHook.Finish();
err73:;
#endif
#if HOOK_BITBLT
	BitBltHook.Finish();
#endif
#if HOOK_BITBLT
err71:;
#endif
#if METAEXTTEXTOUT
	ExtTextOutWHook.Finish();
err7:;
	ExtTextOutAHook.Finish();
err8:
#endif
	if (hPSAPI)
		FreeLibrary( hPSAPI );
#if USE_DBW
err9:
#endif
	HitFound = false;
jreturn:;
#if USE_VXD
	if (vxd_Handle!=INVALID_HANDLE_VALUE)
		UnloadVxD();
#endif
#if METAEXTTEXTOUT
	delete FoundTextA;
	delete FoundTextW;
	delete LastTextA;
	delete LastTextW;
#endif

	return HitFound;
}
#if METAEXTTEXTOUT
void ExtTextOutHit()
{
	if ( FoundLocA != -1 ){
		// Hit on ANSI
		DBW("Hit on ANSI@ExtTextOut");
		CallbackMain(DCH_HITTEXT1, FoundTextA->c_str(), FoundTextA->GetByte(), (HWND)FoundLocA);
	} else {
		// Hit on UNICODE
		DBW("Hit on UNICODE@ExtTextOut");
		CallbackMain(DCH_HITTEXT2, FoundTextW->c_str(), FoundTextW->GetByte(), (HWND)FoundLocW);
	}
}
#endif	// METAEXTTEXTOUT

void DoScale(HDC hdc, POINT *pts, int num)
{
	//TODO: hInst, procはglobalにしたほうがいいのでは？

	int scale = 0;
	if (ScaleX && ScaleY){
		scale = ScaleX;	// 指定したscaling parameter
	} else {
		scale = GetMonitorScale();
		if (scale == 96) return;
	}

#if 1
#if 1
	PROCESS_DPI_AWARENESS value = Process_DPI_Unaware;
	if (hInstSHCore){
		if (!GetProcessDPIAwareness){
			GetProcessDPIAwareness = (FNGetProcessDPIAwareness)GetProcAddress(hInstSHCore, "GetProcessDpiAwareness");
		}
		if (GetProcessDPIAwareness){
			GetProcessDPIAwareness(hProcess, &value);
			DBW("DPIAware: value=%d", value);
		}
	}
	if (value != Process_System_DPI_Aware){
		int dpi_x = scale;
		int dpi_y = scale;
		DBW("dpi: %d,%d", dpi_x, dpi_y);
		for (int i=0;i<num;i++){
			pts[i].x = MulDiv(pts[i].x, dpi_x, 96);
			pts[i].y = MulDiv(pts[i].y, dpi_y, 96);
		}
	}
#else
	HINSTANCE hInst = NULL;
	FNGetProcessDPIAwareness GetProcessDPIAwareness = NULL;
	//FNSetProcessDPIAwareness SetProcessDPIAwareness = NULL;
	//if (!GetProcessDPIAwareness)
	{
		hInst = LoadLibrary("shcore.dll");
		if (hInst){
			GetProcessDPIAwareness = (FNGetProcessDPIAwareness)GetProcAddress(hInst, "GetProcessDpiAwareness");
			//SetProcessDPIAwareness = (FNSetProcessDPIAwareness)GetProcAddress(hInst, "SetProcessDpiAwareness");
			//DBW("DPI API:%08X %08X", GetProcessDPIAwareness, SetProcessDPIAwareness);
			DBW("DPI API:%08X", GetProcessDPIAwareness);
		}
	}
	if (GetProcessDPIAwareness || (ScaleX && ScaleY)){
		int dpi_x, dpi_y;
		PROCESS_DPI_AWARENESS value;
		if (GetProcessDPIAwareness && hdc){
			HRESULT hRes = GetProcessDPIAwareness(hProcess, &value);
			DBW("DPIAware: value=%d", value);
			//SetProcessDPIAwareness(Process_System_DPI_Aware);
			dpi_x = GetDeviceCaps(hdc, LOGPIXELSX);
			dpi_y = GetDeviceCaps(hdc, LOGPIXELSY);
			DBW("dpi: %d, %d", dpi_x, dpi_y);
		} else {
			dpi_x = ScaleX;
			dpi_y = ScaleY;
		}
		for (int i=0;i<num;i++){
			pts[i].x = MulDiv(pts[i].x, dpi_x, 96);
			pts[i].y = MulDiv(pts[i].y, dpi_y, 96);
		}
#if 0
		if (SetProcessDPIAwareness){
			SetProcessDPIAwareness(value);
		}
#endif
	}
	if (hInst){
		FreeLibrary(hInst);
	}
#endif
#else	// 旧コード
	HINSTANCE hInst = NULL;
	if (!GetProcessDPIAwareness){
		hInst = LoadLibrary("shcore.dll");
		DBW("shcore=%08X", hInst);
		if (hInst){
			GetProcessDPIAwareness = (FNGetProcessDPIAwareness)GetProcAddress(hInst, "GetProcessDPIAwareness");
			SetProcessDPIAwareness = (FNSetProcessDPIAwareness)GetProcAddress(hInst, "SetProcessDPIAwareness");
			DBW("DPI API:%08X %08X", GetProcessDPIAwareness, SetProcessDPIAwareness);
		}
	}
	if (GetProcessDPIAwareness || (ScaleX && ScaleY)){
		int dpi_x, dpi_y;
		PROCESS_DPI_AWARENESS value;
		if (GetProcessDPIAwareness && hdc){
			HRESULT hRes = GetProcessDPIAwareness(hProcess, &value);
			SetProcessDPIAwareness(Process_System_DPI_Aware);
			dpi_x = GetDeviceCaps(hdc, LOGPIXELSX);
			dpi_y = GetDeviceCaps(hdc, LOGPIXELSY);
		} else {
			dpi_x = ScaleX;
			dpi_y = ScaleY;
		}
		for (int i=0;i<num;i++){
			pts[i].x = MulDiv(pts[i].x, dpi_x, 96);
			pts[i].y = MulDiv(pts[i].y, dpi_y, 96);
		}
		if (SetProcessDPIAwareness){
			SetProcessDPIAwareness(value);
		}
		if (hInst){
			FreeLibrary(hInst);
		}
	}
#endif
}

int CALLBACK EnumMetaFileProc( HDC hdc, HANDLETABLE *ht, CONST ENHMETARECORD *mfr, int nobj, LPARAM /* user */ )
{
	PlayEnhMetaFileRecord( hdc, ht, mfr, nobj );

	//DBW("iType=%d",mfr->iType);

	switch ( mfr->iType ){
		case EMR_EXTTEXTOUTA:
		case EMR_EXTTEXTOUTW:
		{
			EMREXTTEXTOUTW *emr = (EMREXTTEXTOUTW*)mfr;

			//DBW("(%d,%d) <%d=%d=%d=%d>", CursorPoint.x, CursorPoint.y, emr->rclBounds.left, emr->rclBounds.top,emr->rclBounds.right, emr->rclBounds.bottom);
#if 1
			if ( PtInRect( (RECT*)&emr->rclBounds, CursorPoint ) ){
				DBW("(%d,%d) <%d=%d>", CursorPoint.x, CursorPoint.y, emr->rclBounds.left, emr->emrtext.ptlReference.x);
#if 0
				{
				SIZE sz1, sz2;
				GetWindowExtEx( hdc, &sz1 );
				GetViewportExtEx( hdc, &sz2 );
				DBW("%d-%d %d-%d", sz1.cx, sz1.cy, sz2.cx, sz2.cy);
				}
#endif
				int loc;
				if ( mfr->iType == EMR_EXTTEXTOUTW ){
					//DBW("offString=%d nChars=%d left=%d top=%d offDx=%d", emr->emrtext.offString, emr->emrtext.nChars, emr->rclBounds.left, emr->rclBounds.top, emr->emrtext.offDx);
					//DBW("text=%ws", (wchar_t*) ( ((char*)emr) + emr->emrtext.offString ));
					loc = GetTextFromPoint( hdc, (wchar_t*) ( ((char*)emr) + emr->emrtext.offString ),
						emr->emrtext.nChars,
						CursorPoint.x - emr->rclBounds.left /* + emr->emrtext.ptlReference.x */,
						CursorPoint.y - emr->rclBounds.top  /* + emr->emrtext.ptlReference.y */,
						((int*)(((char*)emr) + emr->emrtext.offDx)) );
				} else {
					loc = GetTextFromPoint( hdc, ((char*)emr) + emr->emrtext.offString,
						emr->emrtext.nChars,
						CursorPoint.x - emr->rclBounds.left /* + emr->emrtext.ptlReference.x */,
						CursorPoint.y - emr->rclBounds.top  /* + emr->emrtext.ptlReference.y */,
						((int*)(((char*)emr) + emr->emrtext.offDx)) );
				}
				if ( loc != -1 ){
//					char *buf = NULL;
					const char *text = ((char*)emr) + emr->emrtext.offString;
					int len = emr->emrtext.nChars;
					if ( mfr->iType == EMR_EXTTEXTOUTW ){
						// WIDE
						//DBW("len=%d loc=%d text=%ws", len, loc, text);
						CallbackMain(DCH_HITTEXT2, text, len * sizeof(wchar_t), (HWND)loc);
#if 0
						// UNICODE->ANSI
						// ANSI上でのlocを求めるため、前半と後半を分けて変換
						wchar_t *wp = (wchar_t*) ( ((char*)emr) + emr->emrtext.offString );
						buf = new char[ emr->emrtext.nChars * 2 ];
						memset( buf, 0, emr->emrtext.nChars*2 );
						// 前半部分
						len = 0;
						if ( loc > 0 ){
							len = WideCharToMultiByte( CP_ACP, 0, wp, loc,
								buf, emr->emrtext.nChars * 2, NULL, NULL );
						}
						int newloc = len;
						// 後半部分
						if ( emr->emrtext.nChars > (unsigned int)loc ){
							len += WideCharToMultiByte( CP_ACP, 0, wp+loc, emr->emrtext.nChars - loc,
								buf+len, emr->emrtext.nChars * 2 - len, NULL, NULL );
						}
						loc = newloc;
						text = buf;
#endif

					} else {
						// ANSI
						CallbackMain(DCH_HITTEXT1, text, len, (HWND)loc);
					}

					DBW("Found@Enum");
#if 0
					CallbackMain(DCH_HITTEXT1, text, len, (HWND)loc);
					if ( buf )
						delete buf;
#endif
#if 0
					{
					char b[100];
					sprintf(b,"exScale=%f eyScale=%f ptlReference=(%d,%d)",emr->exScale, emr->eyScale, emr->emrtext.ptlReference.x, emr->emrtext.ptlReference.y );
					DBW(b);
					DBW("iGraphicsMode=%d", emr->iGraphicsMode);
					}
#endif
#if 0
					{
						for ( int i=0;i<(int)emr->emrtext.nChars;i++ ){
							DBW(">%d", ((int*)(((char*)emr) + emr->emrtext.offDx))[i] );
						}
					}
#endif
//					DBW("[%d %d][%d %d]text=(%s)", emr->rclBounds.left, emr->rclBounds.top, CursorPoint.x, CursorPoint.y, text);
//					result = false;	// stop enumration	// 高速化のため途中で終わりにしたいが・・・どうしよう？
#if METAEXTTEXTOUT
					// prevent to get from ExtTextOut hook
					FoundLocA = -1;
					FoundLocW = -1;
#endif
					HitFound = true;
				}
			}
#else	// for debug
			if ( mfr->iType == EMR_EXTTEXTOUTW ){
				CallbackMain(DCH_EXTTEXTOUTW, emr, sizeof(EMREXTTEXTOUTW) + emr->emrtext.nChars * sizeof(wchar_t), (HWND)hdc);
			} else {
				CallbackMain(DCH_EXTTEXTOUTA, emr, sizeof(EMREXTTEXTOUTA) + emr->emrtext.nChars, (HWND)hdc);
			}
#endif
		}
			break;
	}
	return true;
}

}	// extern "C"

// textの先頭を(0,0)とした座標で、posの位置にあるtextの文字位置を返す
int GetTextFromPoint( HDC hdc, const char *text, int len, int pos_x, int pos_y, const int *dx )
{
	if ( pos_x < 0 || pos_y < 0 ) return -1;

	int count = 0;
	SIZE sz;
	int *rdx = NULL;
	if ( dx ){
		rdx = new int[len];
		memset( rdx, 0, len*sizeof(int) );
	}
	if ( !GetTextExtentExPointA( hdc, text, len, pos_x, &count, rdx, &sz ) ){
		DBW("GetTextExtentExPointA Failure!!");
		if ( rdx )
			delete rdx;
		return -1;
	}
#if METAEXTTEXTOUT
	LastX += sz.cx;	// update
#endif
	// size over check
	if ( pos_y > sz.cy
		|| pos_x > sz.cx
		){
		if ( rdx ) delete rdx;
		return -1;
	}
	DBW("A:len=%d pos_x=%d sz.cx=%d", len, pos_x, sz.cx);
	if ( dx && count > 0 ){
		// ex.
		// Win98,Notepadの日本語上で
		//  dx = 0, 8, 0, 8, 0, 8, ...
		// rdx = 9,10, 9,10, 9,10, ...
#if 0
		{
			int i;
			for ( i=0;i<len;i++ ){
				DBW(" dx[%2d]=%d",i,dx[i]);
			}
			for ( i=0;i<count;i++ ){
				DBW("rdx[%2d]=%d",i,rdx[i+1]-rdx[i]);
			}
		}
#endif
		// spacingの誤差を計算
		// Times New Romanなどで調べるとかなりの誤差がある。なぜ？

		int i = 0;
		int j = 0;
		int sumdx = 0;
//		DBW("rd(x)[0]=%d,%d",dx[0],rdx[0]);
		if ( rdx[0] ){
			if ( dx[0] ){
				sumdx = dx[0] - rdx[0];
			} else {
				sumdx = dx[1] - rdx[0];
			}
			i++;
			j++;
		}
		for ( ;i<count;i++,j++ ){
#if 0
			if ( rdx[i] == rdx[i-1] ){ j--; continue;}	// for multi-bytes font
														//*++ あとで ExtTextOut API manualを参照
			sumdx += dx[j] - (rdx[i]-rdx[i-1]);
//			DBW("%3d <> %3d", dx[j], rdx[i]-rdx[i-1]);
#else	// Win98では、multi-byteでも分散させている場合があるため、dxで判断
			sumdx += (dx[j]?dx[j]:dx[j+1]) - (rdx[i]-rdx[i-1]);
#endif
		}
//		DBW("sumdx=%d",sumdx);
		if ( !GetTextExtentExPointA( hdc, text, len, pos_x - sumdx, &count, rdx, &sz ) ){
			if ( rdx )
				delete rdx;
			return -1;
		}
	}
	if ( rdx )
		delete rdx;
	return count;
}
int GetTextFromPoint( HDC hdc, const wchar_t *text, int len, int pos_x, int pos_y, const int *dx )
{
#if USE_VXD
	if (WindowsNT)
#endif
	{
		if ( pos_x < 0 || pos_y < 0 ) return -1;

		int count = 0;
		SIZE sz;
		int *rdx = NULL;
		if ( dx ){
			rdx = new int[ len ];
			memset( rdx, 0, len );
		}
		if ( !GetTextExtentExPointW( hdc, text, len, pos_x, &count, rdx, &sz ) )
		{
			DBW("GetTextExtentExPointW Failure!!");
			if ( rdx )
				delete[] rdx;
			return -1;
		}
#if METAEXTTEXTOUT
		LastX += sz.cx;
#endif
		// size over check
		if ( pos_y > sz.cy
			|| pos_x > sz.cx
			){
			if ( rdx ) delete[] rdx;
			return -1;
		}
		DBW("W:len=%d pos_x=%d sz.cx=%d", len, pos_x, sz.cx);
		if ( dx && count > 0 ){
			// spacingの誤差を計算
			// Times New Romanなどで調べるとかなりの誤差がある。なぜ？
			int i = 0;
			int j = 0;
			int sumdx = 0;
	//		DBW("rd(x)[0]=%d,%d",dx[0],rdx[0]);
			if ( rdx[0] ){
				sumdx = dx[0] - rdx[0];
				i++;
				j++;
			}
			for ( ;i<count-1;i++,j++ ){
#if 0
				if ( rdx[i] == rdx[i-1] ){ j--; continue;}	// for multi-bytes font
														//*++ あとで ExtTextOut API manualを参照
				sumdx += dx[j] - (rdx[i]-rdx[i-1]);
	//			DBW("%3d <> %3d", dx[j], rdx[i]-rdx[i-1]);
#else
				sumdx += (dx[j]?dx[j]:dx[j+1]) - (rdx[i]-rdx[i-1]);
#endif
			}
	//		DBW("sumdx=%d",sumdx);
			if ( !GetTextExtentExPointW( hdc, text, len, pos_x - sumdx, &count, rdx, &sz ) ){
				if ( rdx )
					delete rdx;
				return -1;
			}
		}
		if ( rdx )
			delete rdx;
		return (count == len && sz.cx < pos_x) ? -1 : count;
	}
#if USE_VXD
	else {
		char *buf = new char[ len * sizeof(wchar_t) ];
		memset( buf, 0, len * sizeof(wchar_t) );
		int ansilen = WideCharToMultiByte( CP_ACP, 0, text, len, buf, len*sizeof(wchar_t), NULL, NULL );
		int r = GetTextFromPoint( hdc, buf, ansilen, pos_x, pos_y, dx );
		delete buf;
		return r;
	}
#endif
}

#define	WMCD_EXISTCHECK		0x4000
#define	WMCD_SETPOINT		0x4001

class TDC {
	HDC hdc;
	HWND hwnd;
public:
	TDC()
	:hwnd(NULL)
	{
		hdc = GetDC(NULL);
	}
	TDC(HWND _hwnd)
	:hwnd(_hwnd)
	{
		hdc = GetWindowDC(hwnd);
	}
	~TDC()
	{
		reset();
	}
	void reset()
	{
		if (hdc){
			ReleaseDC(hwnd, hdc);
			hdc = NULL;
		}
	}
	operator HDC() { return hdc; }
	bool operator !() { return hdc==NULL; }
};

bool CaptureImage(HWND hwnd, bool movesend, bool non_block)
{
	MoveSendMode = movesend;
	CaptureImageBlocking = false;

	bool capture_page = false;
	if (hwndAMODI){
		capture_page = SendMessage(hwndAMODI, WM_AMODI, AMODI_CMD_PAGE_CAPTURE, 0) ? true : false;
	}

	// Get the rect of the target window.
	RECT rcTarget;
	if (!GetWindowRect(hwnd, &rcTarget)){
		// window died?
		return false;
	}
	if (!ExtAMODI){
		if (!IsWindowEnabled(hwndAMODI)){
			// AMODI died?
			hwndAMODI = FindAMODI();
			if (!hwndAMODI)
				return false;
		}
	}
	int w = rcTarget.right - rcTarget.left;
	int h = rcTarget.bottom - rcTarget.top;
	if (w==0 || h==0){
		return false;	// no area in the target.
	}

	POINT ptCursor;	// 画像上のカーソル位置
	int x, y;
	if (capture_page){
		x = rcTarget.left;
		y = rcTarget.top;
		ptCursor.x = ScreenPoint.x - rcTarget.left;
		ptCursor.y = ScreenPoint.y - rcTarget.top;
	} else {
		// x方向 : target rectの左端から
		// y方向 : cursor pointの上下100 pixel
		x = rcTarget.left;
		int offs = 100;
		if (ScreenPoint.y - rcTarget.top < offs){
			if (ScreenPoint.y < rcTarget.top) return false;	// click on out of rect?
			y = rcTarget.top;
			h = ScreenPoint.y - rcTarget.top + offs;
		} else {
			y = ScreenPoint.y - offs;
			h = offs * 2;
		}
		if (y+h>rcTarget.bottom){
			h = rcTarget.bottom - y;
		}
		ptCursor.x = ScreenPoint.x - rcTarget.left;
		ptCursor.y = ScreenPoint.y - y;
	}

	if (0){
		HWND hwnd = GetDesktopWindow();
		RECT rc;
		GetWindowRect(hwnd, &rc);
		x = 0;
		y = 0;
		w = rc.right - rc.left;
		h = rc.bottom - rc.top;
	}

	if (!capture_page){
		// Notify start enumrate
		CallbackMain(DCH_START, &CursorPoint, sizeof(CursorPoint), hwnd);
	}

	bool ok = false;

	TDC hdc;

	if (!hdc){
		goto jend;
	}

#if USE_SCALING
	POINT pts[3];
	pts[0].x = x;
	pts[0].y = y;
	pts[1].x = w;
	pts[1].y = h;
	pts[2].x = ptCursor.x;
	pts[2].y = ptCursor.y;
	DoScale(hdc, pts, 3);
	//DBW("%d,%d %d,%d,%d,%d -> %d,%d %d,%d,%d,%d", ptCursor.x, ptCursor.y, x, y, w, h, pts[2].x, pts[2].y, pts[0].x, pts[0].y, pts[1].x, pts[1].y);
	x = pts[0].x;
	y = pts[0].y;
	w = pts[1].x;
	h = pts[1].y;
	ptCursor.x = pts[2].x;
	ptCursor.y = pts[2].y;
#endif

	// capture image

	int wlen = (w*3+3)&~3;

	/* 書き込み用バッファのサイズ計算 */
	DWORD dwFSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) + wlen * h;

	/* バッファ確保とポインタ設定 */
	LPBYTE lpBuf = (LPBYTE)new char[dwFSize];
	if (!lpBuf){
		goto jend;
	}

	LPBITMAPFILEHEADER lpHead = (LPBITMAPFILEHEADER)lpBuf;
	LPBITMAPINFOHEADER lpInfo = (LPBITMAPINFOHEADER)(lpBuf+sizeof(BITMAPFILEHEADER));
	LPBYTE lpPixel = lpBuf+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	/* 24ビットBMPファイルのヘッダ作成 */
	lpHead->bfType = ('M'<<8) +'B';
	lpHead->bfSize = dwFSize;
	lpHead->bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	lpInfo->biSize = sizeof(BITMAPINFOHEADER);
	lpInfo->biWidth = w;
	lpInfo->biHeight = h;
	lpInfo->biPlanes = 1;
	lpInfo->biBitCount = 24;

	HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);
	if (hBmp){
		HDC hdcMem = CreateCompatibleDC(hdc);
		if (hdcMem){
			HDC hOld = (HDC)SelectObject(hdcMem, hBmp);
			if (hOld!=(HDC)GDI_ERROR){
				BOOL r = BitBlt(hdcMem, 0, 0, w, h, hdc, x, y, SRCCOPY);
				SelectObject(hdcMem, hOld);
				if (r){
//					int lines = GetDIBits(hdc, hBmp, 0, h, NULL, (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);
					int lines = GetDIBits(hdc, hBmp, 0, h, lpPixel, (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);
					if (lines!=0){
						ok = true;
					} else {
						dbw("GetDIBits error??? %d h=%d w=%d bfSize=%d", GetLastError(), h, w, dwFSize);
					}
				} else {
//						dbw("BitBlt error??? %d", GetLastError());
				}
			}
			DeleteObject(hdcMem);
		}
		DeleteObject(hBmp);
	}

	hdc.reset();

	if (ok){
		ok = false;

		//TCHAR path[sizeof(AMODIPath)+40];
		TCHAR *path = ImageTextPath;
		size_t path_size = sizeof(ImageTextPath);
		size_t len;
		if (ExtAMODI){
			len = _tcslen(AMODIPath);
			_tcscpy(path, AMODIPath);
		} else {
			memset(path, 0, path_size);
			len = GetTempPath((DWORD)path_size, path);
		}
		if (len>0){
			// send image to AMODI
			SYSTEMTIME t;
			GetLocalTime(&t);
			if (ExtAMODI){
				wsprintf(path+len, /*path_size-len,*/ _T("\\%04d-%02d-%02d-%02d%02d%02d-(%d,%d)-n%d.bmp"),
					t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, ptCursor.x, ptCursor.y, NumPrevWords );
			} else {
				wsprintf(path+len, /*path_size-len,*/ _T("\\amodi\\%04d-%02d-%02d-%02d%02d%02d-n%d.bmp"),
					t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, NumPrevWords );
				SendAMODI(WMCD_SETPOINT, (char*)&ptCursor, sizeof(ptCursor));
			}
			HANDLE fh = CreateFile(path,  GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (fh!=INVALID_HANDLE_VALUE){
				DWORD dwSize;
				WriteFile(fh, lpBuf, dwFSize, &dwSize, NULL);
				CloseHandle(fh);
				SaveImageTime = GetTickCount();

				if (capture_page){
					ok = true;
				} else {
					len = (int)_tcslen(path);
					_tcsncat(path+len, _T(".txt"), path_size-len);
					if (non_block){
						CaptureImageBlocking = true;
					} else {
						if (WaitAndGetResult(path, 3000))
							ok = true;
					}
				}
			}
		}
	}
	delete[] lpBuf;

jend:
	if (!CaptureImageBlocking){
		if (!capture_page){
			CallbackMain(DCH_END | (MoveSendMode ? DCH_MOVESEND : 0), NULL, 0, hwnd );
		}
	}

	return ok;
}

bool WaitAndGetResult(const TCHAR *path, unsigned waittime)
{
	bool ok = false;

	// wait and get text from AMODI

	HANDLE fh = WaitForResult(path, waittime);

	CaptureImageBlocking = false;

	if (fh==INVALID_HANDLE_VALUE){
		DBW("file open timeout");
		CheckAMODIAlive();
	} else {
		DWORD size = GetFileSize(fh, NULL);
		if (size>0){
			unsigned bufsize = size+sizeof(wchar_t);
			char *text = new char[bufsize];
			if (text){
				DWORD rbyte;
				if (ReadFile(fh, text, size, &rbyte, NULL)){
					if (rbyte==size){
						*(wchar_t*)&text[size] = '\0';
						// textの１行目は解析情報
						// format
						// (マウスカーソルのあるテキスト位置[文字目 zero-index])
						int loc = 0;
						int col = 0;
						wchar_t *p = (wchar_t*)text;
						while (*p){
							wchar_t c = *p++;
							if (c=='\n'){
								break;
							}
							if (col==0){
								loc = _wtoi(p);
								col++;
								while (iswdigit(*p)) p++;
							}
						}

						DBW("%d:%ws", loc, p);
						CallbackMain(DCH_HITTEXT2, p, bufsize - (int)((char*)p-text), (HWND)loc);

						ok = true;
					}
				}
				delete[] text;
			}
		}
		CloseHandle(fh);
		DeleteFile(path);
	}
	CallbackMain(DCH_END | (MoveSendMode ? DCH_MOVESEND : 0), NULL, 0, NULL );
	return ok;
}

HANDLE WaitForResult(const TCHAR *path, unsigned waittime)
{
	HANDLE fh = INVALID_HANDLE_VALUE;

	while (1){
		fh = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fh!=INVALID_HANDLE_VALUE)
			break;

		DWORD now = GetTickCount();
		if (now-SaveImageTime > waittime)
			break;

		Sleep(10);
	}

	return fh;
}

#define	APPNAME	_T("Auto MODI")

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

HWND FindAMODI()
{
	hwndFind = NULL;
	EnumWindows(EnumWindowsProc, 0);
	return hwndFind;
}

int SendAMODI(int cmd, const char *data, int len)
{
	COPYDATASTRUCT cd;
	cd.dwData = cmd;
	cd.lpData = (void*)data;
	cd.cbData = len;
	return (int)SendMessage(hwndAMODI, WM_COPYDATA, 0, (LPARAM)&cd);
}

void CheckAMODIAlive()
{
	CallbackMain(DCH_LAUNCH_AMODI, NULL, 0, 0);
}

void SendMoveMessage()
{
#if MOVESEND_POST
	PostMessage(hwndCallback, WM_MOVESEND, MouseMoving ? 0 : -1, 0);
#else
//	CallbackMain(DCH_MOVESEND, pt, sizeof(*pt), 0);
	MouseMoving = true;
	if (SendMoveEvent) SetEvent(SendMoveEvent);
#endif
}
void SendCancelMove()
{
#if MOVESEND_POST
	PostMessage(hwndCallback, WM_MOVESEND, MouseMoving ? 0 : -1, 0);
#else
//	CallbackMain(DCH_MOVESEND, NULL, 0, (HWND)-1);
	MouseMoving = false;
	if (SendMoveEvent) SetEvent(SendMoveEvent);
#endif
}

#if !MOVESEND_POST
// main programのWindows message処理が重いと、CalblackMainも重くなるため、
// mouse moveは別threadで処理
DWORD WINAPI SendMoveThread(LPVOID vdParam)
{
	while(1){
		if (WaitForSingleObject(SendMoveEvent, INFINITE) != WAIT_OBJECT_0){
			break;
		}
		CallbackMain(DCH_MOVESEND, NULL, 0, (HWND)(MouseMoving ? 0 : -1));
	}
	return 0;
}
#endif

#if EXC_WOW64 && defined(_M_X64)
void CheckWOW64()
{
	typedef BOOL (WINAPI *FNIsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);

	FNIsWow64Process _IsWow64Process = (FNIsWow64Process)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");
	if (_IsWow64Process){
		BOOL flag = FALSE;
		if (_IsWow64Process(hProcess, &flag)){
			fWow64 = int_bool(flag);
		}
	}
}
#endif

bool EnablePrivilege(LPTSTR lpszPrivilege, bool bEnable)
{
	BOOL             bResult;
	LUID             luid;
	HANDLE           hToken;
	TOKEN_PRIVILEGES tokenPrivileges;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return false;
	
	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
		CloseHandle(hToken);
		return false;
	}

	tokenPrivileges.PrivilegeCount           = 1;
	tokenPrivileges.Privileges[0].Luid       = luid;
	tokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	
	bResult = AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	
	CloseHandle(hToken);

	return bResult && GetLastError() == ERROR_SUCCESS;
}

/*======================================================================//
//	DllMain
//----------------------------------------------------------------------//
// Description:
//
// Arguments:
//
// Return Value:
//
//======================================================================*/
#ifdef _MSC_VER
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID )
#else
int WINAPI DllEntryPoint(HINSTANCE hInst, DWORD reason, LPVOID)
#endif
{
	switch ( reason ){
		case DLL_PROCESS_ATTACH:
			hInstance = hInst;
			idProcess = GetCurrentProcessId();
			for (int i=0;i<2;i++){
				hProcess = OpenProcess( PROCESS_ALL_ACCESS|PROCESS_VM_WRITE|PROCESS_VM_OPERATION, TRUE, idProcess );
				if ( hProcess ){
#if USE_DBW
					DBW("Attach %d(%08X)-%d : %08X", idProcess, hProcess, attach++,_ExtTextOutA);
#endif
					break;
				} else {
					DBW("OpenProcess Error: %08X %d", idProcess, GetLastError());
					if (i==0){
						if (!EnablePrivilege(SE_DEBUG_NAME, true)) {
							break;	// failed
						}
					}
				}
			}
#if EXC_WOW64 && defined(_M_X64)
			CheckWOW64();
#ifdef _WIN64
			if (fWow64){
				DBW("WOW64 true");
				return FALSE;
			}
#else
			if (!fWow64){
				DBW("WOW64 false");
				return FALSE;
			}
#endif
			DBW("fWow64=%d", fWow64);
#endif
			if (siPageSize==0){
				SYSTEM_INFO si;
				GetSystemInfo( &si );
				siPageSize = si.dwPageSize;
			}
			hInstSHCore = LoadLibrary(_T("shcore.dll"));
#if !MOVESEND_POST
			SendMoveEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			CreateThread(NULL , 0 , SendMoveThread , NULL , 0 , &SendMoveThreadId);
#endif
			break;
		case DLL_PROCESS_DETACH:
//			UNPROTECT_SHARE();
#if !MOVESEND_POST
			if (SendMoveEvent){
				CloseHandle(SendMoveEvent);
				SendMoveEvent = NULL;
			}
#endif
			if (hInstSHCore){
				FreeLibrary(hInstSHCore);
				hInstSHCore = NULL;
			}
#if USE_DBW
			DBW("Detach %d-%d", GetCurrentProcessId(),--attach);
#endif
			if ( hProcess ){
				CloseHandle( hProcess );
			}
			break;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
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
#ifdef _WIN64
	strcpy(buf, "x64:");
	wvsprintfA( buf+4, format, ap );
#else
	wvsprintfA( buf, format, ap );
#endif
	COPYDATASTRUCT cds;
	cds.dwData = 1;	// Indicate String
	cds.cbData = (DWORD)strlen(buf);
	cds.lpData = buf;
	SendMessage( hWin, WM_COPYDATA, NULL, (LPARAM)&cds );
	va_end( ap );
}
