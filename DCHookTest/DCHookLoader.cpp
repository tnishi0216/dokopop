#include <windows.h>
#pragma	hdrstop
#include <tchar.h>
#include <System.hpp>
#include "StringLib.h"
#include "DCHookLoader.h"
#include "HookLoader.h"
#include "Util.h"

// LoadHook()はapplication立ち上がり時、またはConfiguration時に１回だけ呼ばれる
// UnloadHook()はLoadHook()と必ず対で使用する

#define	USE_HOOK64		1

#if USE_HOOK64
#define	IsWow64()	(fWow64)
#else
#define	IsWow64()	(false)
#endif

/// THookArray ///
THookArray::~THookArray()
{
	clear();
}

bool THookArray::Load(HWND hwnd)
{
	bool ret = true;
	foreach(Hooks, it, hook_vec){
		if (!(*it)->Load(hwnd)){
			ret = false;
			break;
		}
	}
	return ret;
}
void THookArray::Unload()
{
	foreach(Hooks, it, hook_vec){
		(*it)->Unload();
	}
}
void THookArray::Uninit()
{
	foreach(Hooks, it, hook_vec){
		(*it)->Uninit();
	}
}
bool THookArray::Init(HWND hwnd)
{
	DBW("HookArray::Init");
	bool ret = true;
#if 0	// debug code
	DBW("Init1"); Hooks[1]->Init(hwnd);	// 64bit
	DBW("Init0"); Hooks[0]->Init(hwnd);	// 32bit
#else
	foreach(Hooks, it, hook_vec){
		if (!(*it)->Init(hwnd)){
			ret = false;
		}
	}
#endif
	return ret;
}
void THookArray::Init32(HWND hwnd)
{
	Hooks[0]->Init(hwnd);
}
int THookArray::Config( int clickonly, int keyaction, int keyflag )
{
	foreach(Hooks, it, hook_vec){
		(*it)->Config(clickonly, keyaction, keyflag);
	}
	return 0;
}
int THookArray::Config2( struct TDCHConfig *cfg )
{
	foreach(Hooks, it, hook_vec){
		(*it)->Config2(cfg);
	}
	return 0;
}
int THookArray::Capture()
{
	int ret = 0;
	foreach(Hooks, it, hook_vec){
		if ((*it)->Capture())
			ret = 1;
	}
	return ret;
}
int THookArray::CaptureAsync()
{
	int ret = 0;
	foreach(Hooks, it, hook_vec){
		if ((*it)->CaptureAsync())
			ret = 1;
	}
	return ret;
}
int THookArray::CaptureAsyncWait()
{
	int ret = 0;
	foreach(Hooks, it, hook_vec){
		int r = (*it)->CaptureAsyncWait();
		if (r!=0){
			ret = r;
			break;
		}
	}
	return ret;
}
int THookArray::Debug()
{
	int ret = 0;
	foreach(Hooks, it, hook_vec){
		int r = (*it)->Debug();
		if (r)
			ret = r;
	}
	return ret;
}

void THookArray::clear()
{
	Uninit();
	foreach(Hooks, it, hook_vec){
		delete *it;
	}
	Hooks.clear();
}

bool THookArray::IsLoaded()
{
	if (Hooks.size()==0)
		return false;
	return Hooks[0]->IsLoaded();
}

/// TDCHookLoader ///
TDCHookLoader::TDCHookLoader(bool use64)
	:Use64(use64)
{
	EnableSendAllKeys = false;
	Hook64 = NULL;
	Hook32 = NULL;
	LoadPending = false;
}
#if !defined(DSKSVR) && !defined(DCHOOK)
void TDCHookLoader::SaveProfile(const tchar *param_string)
{
	prof.WriteString(PFS_SHORTCUT, StrParams, param_string);
}

tnstr TDCHookLoader::LoadProfile()
{
	return prof.ReadString(PFS_SHORTCUT, StrParams, _T(""));
}
#endif

bool TDCHookLoader::LoadHook( HWND hwndParent )
{
	if (Hooks.IsLoaded() || LoadPending) return true;

	if (Hooks.size()==0){
		Hooks.add(Hook32 = new THookLoader(NAME_DKPPHKDLL) );
#ifdef _DEBUG
		if (!DebugHook)
#endif
		{
			if (Use64 && IsWow64()){
				Hooks.add(Hook64 = new THookLoader64(hwndParent));
			}
		}
	}

	if (!Hooks.Load(hwndParent)){
		DBW("Hooks.Load failed");
		return false;
	}

	if (Hook64 && Hook64->IsPending()){
		LoadPending = true;
		hwndSavedParent = hwndParent;
		return true;
	}

	LoadHookPost(hwndParent);

	return true;
}

void TDCHookLoader::LoadHookPost(HWND hwndParent)
{
	Hooks.Init(hwndParent);
#if 0
	int num = LoadKeyProfiles();
	if ( num > 0 || EnableSendAllKeys ){
		if ( !Hooks.InitHook( hwndParent, UM_SHORTCUT ) ){
			num = 0;
		}
	}

#if !defined(DSKSVR) && !defined(DCHOOK)
	//Note: SendAllKeysを有効にすると32bitApp上で二重にイベントが送られてくる。（原因不明）
	// そのためこの設定だけは32bit側だけ有効にする。
	// （64bitApp上では二重にならない）
	// なお、64bitHookだけをloadした場合はイベントがまったくこなくなる（これも原因不明）
	if (Hook32)
		Hook32->Add( NULL, EnableSendAllKeys, FUNC_SENDALLKEYS );
#endif

	if ( num == 0 && !EnableSendAllKeys ){
		Hooks.Unload();
	}
#endif
}

void TDCHookLoader::UnloadHook()
{
	LoadPending = false;
	if (!Hooks.IsLoaded()){
		return;
	}
	//Hooks.FinishHook();
	Hooks.Uninit();
	Hooks.Unload();
	//if ( fWindowsNT )
	{
#if 1	// しばらくはこっちの方法(C++Builderが対応していない - というか、MSのbugでうまくlinkできない)
		typedef long (WINAPI *FNBroadcastSystemMessage)
			(DWORD dwFlags,LPDWORD lpdwRecipients,UINT uiMessage,WPARAM wParam,LPARAM lParam);
		HINSTANCE hDll = LoadLibrary( _T("user32") );
		if ( hDll ){
			FNBroadcastSystemMessage _BroadcastSystemMessage = (FNBroadcastSystemMessage)GetProcAddress( hDll, "BroadcastSystemMessage");
			if ( _BroadcastSystemMessage ){
				UINT msg = RegisterWindowMessage( _T("wmBroadcastForUnhook") );
				if ( msg ){
					DWORD receipt = BSM_APPLICATIONS;
					_BroadcastSystemMessage( BSF_POSTMESSAGE, &receipt, msg, 0, 0 );	// Win95ではsupportされていない関数
				}
			}
			FreeLibrary( hDll );
		}
#else
		UINT msg = RegisterWindowMessage( "wmBroadcastForUnhook" );
		DWORD receipt = BSM_APPLICATIONS;
		BroadcastSystemMessage( BSF_POSTMESSAGE, &receipt, msg, 0, 0 );	// Win95ではsupportされていない関数
#endif
	}
}

void TDCHookLoader::Config( int clickonly, int keyaction, int keyflag )
{
	Hooks.Config(clickonly, keyaction, keyflag);
}
void TDCHookLoader::Config2( struct TDCHConfig *cfg )
{
	Hooks.Config2(cfg);
}

void TDCHookLoader::Capture()
{
	Hooks.Capture();
}
void TDCHookLoader::CaptureAsync()
{
	Hooks.CaptureAsync();
}
void TDCHookLoader::CaptureAsyncWait()
{
	Hooks.CaptureAsyncWait();
}
void TDCHookLoader::Init32(HWND hwnd)
{
	Hooks.Init32(hwnd);
}

void TDCHookLoader::Notify64(LPARAM lParam)
{
	DBW("Notify64 : lParam=%08X Hook64=%08X", lParam, Hook64);
	if (!Hook64 || !lParam)
		return;
	Hook64->Notify64();
	Hook64->SetTarget((HWND)lParam);
	if (LoadPending){
		LoadHookPost(hwndSavedParent);
		LoadPending = false;
	}
}

