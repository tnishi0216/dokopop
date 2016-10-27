#ifndef __DCHookLoader_h
#define	__DCHookLoader_h

#include <vector>
using namespace std;

enum eShortCutIndexes {
	SCINX_AUTOSRCH_TOGGLE,
	SCINX_AUTOSRCH_ON,
	SCINX_AUTOSRCH_OFF,
	SCINX_TOPMOST,
	SCINX_TO_MAIN,
	SCINX_QUICK_POPUP,
	SCINX_POPUP_WINDOW,
	SCINX_GROUP_CHANGE_1,
	SCINX_GROUP_CHANGE_2,
	SCINX_GROUP_CHANGE_3,
	SCINX_GROUP_CHANGE_4,
	SCINX_CBDSRCH,
	SCINX_CLIPHISTORY,
	SCINX_TAB_CHANGE_1,
	SCINX_TAB_CHANGE_2,
	SCINX_TAB_CHANGE_3,
	SCINX_TAB_CHANGE_4,
	SCINX_LAST,
};

#define	SCINX_NOTIFY64	(0x1000)	// notify from 64bit HookDll

#include "HookDll.h"

class THookArray : public THookDllI {
protected:
	typedef vector<THookDllI*> hook_vec;
	vector<THookDllI*> Hooks;
public:
	virtual ~THookArray();
	void add(THookDllI*dll)
	{
		Hooks.push_back(dll);
	}
	int size() const
		{ return Hooks.size(); }
	virtual bool Load(HWND hwnd);
	virtual void Unload();

	virtual bool Init(HWND hwnd);
	void Init32(HWND hwnd);
	virtual void Uninit();
	virtual int Config( int clickonly, int keyaction, int keyflag );
	virtual int Config2( struct TDCHConfig *cfg );
	virtual int Capture();
	virtual int CaptureAsync();
	virtual int CaptureAsyncWait();
	virtual int Debug();
	virtual bool IsLoaded();
	THookDllI &operator [](int i)
		{ return *Hooks[i]; }
protected:
	void clear();
};

class TDCHookLoader {
protected:
	bool Use64;
	bool EnableSendAllKeys;
	THookArray Hooks;
	class THookLoader64 *Hook64;	// reference pointer
	class THookLoader *Hook32;		// reference pointer
	bool LoadPending;
	HWND hwndSavedParent;
public:
	TDCHookLoader(bool use64);
	bool LoadHook( HWND hwndParent );
	void UnloadHook();
	void Config( int clickonly, int keyaction, int keyflag );
	void Config2( struct TDCHConfig *cfg );
	void Capture();
	void CaptureAsync();
	void CaptureAsyncWait();

	// for 64bit OS
	void Notify64(LPARAM lParam);
	bool IsReady() const
		{ return !LoadPending; }

	void Init32(HWND hwnd);	// for debug
protected:
	void LoadHookPost(HWND hwndParent);
	int LoadKeyProfiles( );
};

extern ::TDCHookLoader shortcut;

#endif	// __DCHookLoader_h

