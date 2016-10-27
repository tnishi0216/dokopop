#ifndef __HookDll_h
#define	__HookDll_h

class THookDllI {
public:
	virtual ~THookDllI(){}
	virtual bool Load(HWND hwnd) = 0;
	virtual void Unload() = 0;
	virtual bool IsLoaded() = 0;

	virtual bool Init(HWND hwnd) = 0;
	virtual void Uninit() = 0;
	virtual int Config( int clickonly, int keyaction, int keyflag ) = 0;
	virtual int Config2( struct TDCHConfig *cfg ) = 0;
	virtual int Capture( ) = 0;
	virtual int CaptureAsync() = 0;
	virtual int CaptureAsyncWait() = 0;
	virtual int Debug() = 0;
};


#endif
