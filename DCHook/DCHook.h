#ifndef __DCHOOK_H
#define	__DCHOOK_H

struct TDCHConfig {
	int UseAMODI : 1;
	int OnlyAMODI : 1;
	int MoveSend : 1;
	int OnlyImage : 1;
	int ScaleX;
	int ScaleY;
	char AMODIPath[_MAX_PATH];
};

extern "C" {

__declspec(dllexport) bool WINAPI Init( HWND, const char *module_name, bool windowsnt, const char *vxd_path );
__declspec(dllexport) void WINAPI Uninit();
__declspec(dllexport) unsigned int WINAPI Debug();

typedef bool (WINAPI *FNDCHInit)( HWND, const char *module_name, bool windowsnt, const char *vxd_path );
typedef void (WINAPI *FNDCHUninit)();
typedef int (WINAPI *FNDCHConfig)( int clickonly, int keyaction, int keyflag );
typedef int (WINAPI *FNDCHConfig2)( struct TDCHConfig *cfg );
typedef int (WINAPI *FNDCHCapture)( HWND hwnd, POINT *pt, bool movesend, bool non_block );
typedef int (WINAPI *FNDCHWaitForCaptureResult)( bool send_text, unsigned waittime );
typedef unsigned int (WINAPI *FNDebug)();

}

/* WM_COPYSTRUCT message */
#define	DCH_START		0x1000
#define	DCH_END			0x1001
#define	DCH_EXTTEXTOUTA	0x1002
#define	DCH_EXTTEXTOUTW	0x1003
#define	DCH_HITTEXT1	0x1010
#define	DCH_HITTEXT2	0x1011	// UNICODEのみ(DCH_HITTEXT2のあと、DCH_HITTEXT3がくる)
#define	DCH_HITTEXT3	0x1012	// ANSIのみ
// UNICODEで扱いたい場合は、DCH_HITTEXT2とDCH_HITTEXT3を使用する
// UNICODEで扱いたくない場合(UNICODE->ANSI変換を必要とする場合)は、DCH_HITTEXT1のみを使用する
#define	DCH_MOVESEND	0x0200	// DCH_ENDとの組み合わせ or 単体

#define	DCH_MENU		0x1100	// DokoPop Menu
#define	DCH_TOGGLE		0x1101	// Toggle Click Only
#define	DCH_LAUNCH_AMODI 0x1102	// Launch AMODI if not alive

/* Do not use 0x12xx!! */

/* Key Definitions */
#define	KA_POPUP		1
#define	KA_MENU			2
#define	KA_TOGGLE		3
#define	KA_POPUP_NC		0x10

#define	KF_SHIFT		0x01
#define	KF_CONTROL		0x02
#define	KF_MENU			0x04
#define	KF_LWIN			0x08
#define	KF_RWIN			0x10
#define	KF_APPS			0x20
#define	KF_LEFTCLICK	0x40
#define	KF_CLICKONLY	0x80
#define	KF_LRCLICK		0x100
#define	KF_MIDCLICK		0x200


#endif	// __DCHOOK_H

