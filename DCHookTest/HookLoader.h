#ifndef __HookLoader_h
#define	__HookLoader_h

#include "../DCHook/DCHook.h"
#include "HookDll.h"

class THookLoader : public THookDllI {
public:
	THookLoader(const char *dllname);
	~THookLoader();
	virtual bool Load(HWND hwnd);
	virtual void Unload();
	virtual bool IsLoaded()
		{ return hDll!=NULL; }

	virtual bool Init(HWND hwnd);
	virtual void Uninit();
	virtual int Config( int clickonly, int keyaction, int keyflag )
		{ return DCHConfig(clickonly, keyaction, keyflag); }
	virtual int Config2( struct TDCHConfig *cfg )
		{ return DCHConfig2(cfg); }
	virtual int Capture();
	virtual int CaptureAsync();
	virtual int CaptureAsyncWait();
	virtual int Debug()
#ifdef _DEBUG
		{ return DCDebug(); }
#else
		{ return 0; }
#endif
protected:
	HANDLE hThread;
	DWORD dwThreadId;
	bool ThreadEnd;
	void OpenThread();
	static DWORD WINAPI cbWaitCaptureThread(LPVOID This);
	void WaitCaptureThread();

protected:
	const char *DllName;
	HINSTANCE hDll;
	FNDCHInit DCHInit;
	FNDCHUninit DCHUninit;
	FNDCHConfig DCHConfig;
	FNDCHConfig2 DCHConfig2;
	FNDCHCapture DCHCapture;
	FNDCHWaitForCaptureResult DCHWaitForCaptureResult;
#ifdef _DEBUG
	FNDebug DCDebug;
	#define COMPSIZE	16
	char share[ COMPSIZE ];
#endif
};

class THookLoader64 : public THookDllI {
public:
	THookLoader64(HWND hwnd_recv);
	virtual ~THookLoader64();
	virtual bool Load(HWND hwnd);
	virtual void Unload();
	virtual bool IsLoaded()
		{ return hProc!=NULL; }

	virtual bool Init(HWND hwnd);
	virtual void Uninit() ;
	virtual int Config( int clickonly, int keyaction, int keyflag );
	virtual int Config2( struct TDCHConfig *cfg );
	virtual int Capture();
	virtual int CaptureAsync();
	virtual int CaptureAsyncWait();
	virtual int Debug();
	void SetTarget(HWND hwnd)
		{ hwndTarget = hwnd; }
	bool WaitFor() const
	{
		return WaitForInputIdle(hProc, 500)==0;
	}
	bool IsPending() const { return LoadPending; }
	void Notify64() { LoadPending = false; }

protected:
	int Send(int msg, void *data=NULL, int len=0);

	HWND hwndRecv;	// receive window handle (=main window handle)
	HWND hwndTarget;	// main window handle of pdhk64.exe
	HANDLE hProc;
	bool LoadPending;
};

#endif

