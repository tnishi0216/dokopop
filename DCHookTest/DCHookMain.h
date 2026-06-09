//---------------------------------------------------------------------------
#ifndef DCHookMainH
#define DCHookMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include "../DCHook/DCHook.h"
#include <ExtCtrls.hpp>
#include <DdeMan.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
#include "Util.h"
#include "StringLib.h"

#define	WM_POPUP	(WM_APP+0x200)
#define	WM_NOTIFYICON (WM_APP+0x201)
#define	WM_TOGGLE	(WM_APP+0x202)
#define	UM_SHORTCUT	(WM_APP+33)
#define	WM_MOVESEND	(WM_APP+0x208)	// DCH_MOVESENDの代わりにPostMessageで送る

class TDCHookMainForm : public TForm
{
typedef TForm super;
__published:	// IDE 管理のコンポーネント
	TMainMenu *MainMenu;
	TMenuItem *File1;
	TMenuItem *miExit;
	TMenuItem *miHook;
	TMenuItem *miUnhook;
	TMenuItem *N1;
	TMenuItem *miTest;
	TMemo *Memo;
	TSplitter *Splitter1;
	TPaintBox *PaintBox;
	TPanel *Panel1;
	TMenuItem *miDdeTest;
	TEdit *edFound;
	TEdit *edWord;
	TEdit *edPrev;
	TPopupMenu *pmTrayIcon;
	TMenuItem *miExit2;
	TMenuItem *miAbout;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *miOption;
	TMenuItem *miGroup;
	TMenuItem *N4;
	TMenuItem *miEnablePopup;
	TTimer *BootTimer;
	TImage *Image;
	TTimer *TestTimer;
	TButton *btnOK;
	TLabel *lbVersion;
	TMenuItem *miClickOnly;
	TMenuItem *miReload;
	TMenuItem *miClose;
	TTimer *tmMenuClose;
	TTimer *tmNotify;
	TTimer *tmPopup;
	TMenuItem *miReadMe;
	TMenuItem *miIncSearch;
	TTimer *tmMouseMove;
	TMenuItem *N2;
	TMenuItem *miConfig;
	TTimer *tmDNFInstallCheck;
	TTimer *tmDNFNotify;
	TMenuItem *miCheckLatest;
	TMenuItem *miOCRText;
	TTimer *tmReInit;
	void __fastcall miHookClick(TObject *Sender);
	void __fastcall miUnhookClick(TObject *Sender);
	void __fastcall miExitClick(TObject *Sender);
	void __fastcall miTestClick(TObject *Sender);
	void __fastcall miDdeTestClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall miAboutClick(TObject *Sender);
	void __fastcall miOptionClick(TObject *Sender);
	void __fastcall pmTrayIconPopup(TObject *Sender);
	void __fastcall miEnablePopupClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall BootTimerTimer(TObject *Sender);
	void __fastcall ImageMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall TestTimerTimer(TObject *Sender);
	void __fastcall btnOKClick(TObject *Sender);
	void __fastcall ImageMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall ImageMouseMove(TObject *Sender, TShiftState Shift, int X,
		  int Y);
	void __fastcall miClickOnlyClick(TObject *Sender);
	void __fastcall miReloadClick(TObject *Sender);
	void __fastcall miCloseClick(TObject *Sender);
	void __fastcall tmMenuCloseTimer(TObject *Sender);
	void __fastcall tmNotifyTimer(TObject *Sender);
	void __fastcall tmPopupTimer(TObject *Sender);
	void __fastcall miReadMeClick(TObject *Sender);
	void __fastcall miIncSearchClick(TObject *Sender);
	void __fastcall tmMouseMoveTimer(TObject *Sender);
	void __fastcall miConfigClick(TObject *Sender);
	void __fastcall tmDNFInstallCheckTimer(TObject *Sender);
	void __fastcall tmDNFNotifyTimer(TObject *Sender);
	void __fastcall miCheckLatestClick(TObject *Sender);
	void __fastcall miOCRTextClick(TObject *Sender);
	void __fastcall tmReInitTimer(TObject *Sender);
private:	// ユーザー宣言
	//void __fastcall IdleHandler(TObject *sender, bool &done);

	bool BootUp;
	bool Capturing;
	POINT CapPoint;
	class TDCHookLoader *hDll;

	int SavedWidth;

	// Popup Configuration //
	bool GroupOpen;
	AnsiString GroupName;
	bool CtrlClose;
	bool IgnoreJ;
	bool Banner;
	bool EnableAdvanced;
	int CaptureMode;	// CM_IMAGE or CM_TEXT
	bool ATSOCRAvail;	// ATSOCR available or not
	bool MouseIncSrch;
	int GetActualCaptureMode() const;
	bool IsActualUse64() const;
public:		// ユーザー宣言
	__fastcall TDCHookMainForm(TComponent* Owner);
	void EvCopyData(TMessage&);
	void InitATSOCR();
	void Start();
	void MenuClosed();
	void ShowNotify();
	void EvToggle( TMessage & );
	void EvShortcut( TMessage & );
	void EvGetVersion( TMessage & );

	// Hooking //
	bool Hook();
	void Unhook();

	// Popup Control //
	POINT CursorPoint;
	void SetupConfig();
	void SetupConfig2();
	void SetupATSOCR();
	bool EnablePopup;
	bool DoPopup( const tchar *text, int click_pos, const tchar *prevtext, bool movesend );
	tchar *PopupText;
	int PopupLoc;
	bool PopupMoveSend;
	void EvPopup(TMessage &);
	int DoPopupRetry;
	void EvMoveSend(TMessage &);
	void EvMoveSend(bool enable);

	// Tray Icon Control //
	void RegisterTrayIcon( bool flag );
	void EvNotifyIcon( TMessage & );
	void SaveConfig();
	void EnableClickOnly( bool enable );
	void DokoPopMenu( bool all );

	// PDIC Control //
	void __fastcall miDicGroup(TObject *Sender);
	void OpenDicGroup( const char *name );
	TDdeClientConv *OpenPdic( const char *topic );
	void ClosePdic( TDdeClientConv *dde );
	int GetPdicVersion();

	// WM_COPYDATA control //
	HWND FindPdic( );
	LRESULT SendCopyData( HWND hwnd, int message, const char *str );

	void NotifyDNF();
	void Reboot();
protected:
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_COPYDATA, TMessage, EvCopyData)
		VCL_MESSAGE_HANDLER(WM_POPUP, TMessage, EvPopup)
		VCL_MESSAGE_HANDLER(WM_MOVESEND, TMessage, EvMoveSend)
		VCL_MESSAGE_HANDLER(WM_NOTIFYICON, TMessage, EvNotifyIcon)
		VCL_MESSAGE_HANDLER(WM_TOGGLE, TMessage, EvToggle)
		VCL_MESSAGE_HANDLER(UM_SHORTCUT, TMessage, EvShortcut)
		VCL_MESSAGE_HANDLER(WM_GET_VERSION, TMessage, EvGetVersion)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TDCHookMainForm *DCHookMainForm;
//---------------------------------------------------------------------------

extern bool Reboot;

#endif

