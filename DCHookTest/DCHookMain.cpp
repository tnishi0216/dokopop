//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "DCHookMain.h"
#include <stdio.h>
#include <shellapi.h>
#include "StringLib.h"
#include "PopupConfig.h"
#include "Notify.h"
#include "DCHookLoader.h"
#include "prgprof.h"
#include "prgconfig.h"
#include "OCRTextFrm.h"
#include "MonitorScale.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

/*------------------------------------------*/
/*		Configuration						*/
/*------------------------------------------*/
// PopupのたびにDLLをUnload/Loadする
// 反応しなくなったときのための対症療法
// Note: 2013.3.12
//	inc.srch ONのとき、MoveSentが常にresetされてしまうため、
//	mouseがstill状態であっても何度もinc.srchのリクエストがやってくる。
//	ほか、popup/inc.srchをやるたびにDLL内の変数がresetされるためこれはよろしくない。
#define	DETACHRELEASE		0

#define	MIGROUP_INDEX		0		// 辞書グループ変更メニューのindex

// PDIC起動後のOpenLink()失敗時のretryおよびwaittime
#define	OPEN_RETRY			5
#define	OPEN_WAITTIME		500		// [mSec]

#define	DPI_DETECT			0		// DPI Detect in main (monitorごとの設定ができないためdebug用)

#define	DEF_IGNOREJ			(true)
#define	DEF_CAPTURE_MODE	CM_IMAGE
#define	DEF_USE64			(fWow64)

/*------------------------------------------*/
/*		Definitions							*/
/*------------------------------------------*/
#define	DEF_POPUPKEY		(KF_CONTROL)
#define	DEF_TOGGLEKEY		(KF_CONTROL+KF_MENU)

#define	WM_ATSOCR			(WM_APP+0x400)	// app communication message with ATSOCR

#define	FLG_MOVESEND		0x40000000	// mouse move send

#pragma resource "*.dfm"
TDCHookMainForm *DCHookMainForm;

#define	COMPANYNAME 	"Software\\DokoPopProject\\"

bool Reboot = false;

/*------------------------------------------*/
/*		Prototypes							*/
/*------------------------------------------*/
bool DdePoke( TDdeClientConv *dde, AnsiString Item, AnsiString Data );
#ifdef USE_UNICODE
// Unicodeで渡す場合
bool DdePoke( TDdeClientConv *dde, AnsiString Item, const wchar_t *Data );
bool DdeRequest( TDdeClientConv *dde, AnsiString Item, char *Data, int &inoutDataLen );
#endif;
void GetGroupList(TMyIni &ini, TStringList &list);

//---------------------------------------------------------------------------
__fastcall TDCHookMainForm::TDCHookMainForm(TComponent* Owner)
	: TForm(Owner)
{
	hDll = NULL;
	EnablePopup = true;

#if 0
	WindowsNT = false;

	OSVERSIONINFO VersionInfo;
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);
	switch (VersionInfo.dwPlatformId){
		case VER_PLATFORM_WIN32_NT:
			WindowsNT = true;
			break;
	}
#endif
	CheckWOW64();
	BootUp = true;
	Capturing = false;
	PopupText = NULL;
	DoPopupRetry = 0;
	CaptureMode = DEF_CAPTURE_MODE;
	ATSOCRAvail = false;
	MouseIncSrch = false;
}
//---------------------------------------------------------------------------
int TDCHookMainForm::GetActualCaptureMode() const
	{ return EnableAdvanced ? CaptureMode : DEF_CAPTURE_MODE; }
bool TDCHookMainForm::IsActualUse64() const
	{ return EnableAdvanced ? Ini->ReadInteger(PFS_CONFIG, PFS_USE64, DEF_USE64) : DEF_USE64; }
void __fastcall TDCHookMainForm::FormCreate(TObject *Sender)
{
	Ini = new TMyIni( HKEY_CURRENT_USER, COMPANYNAME APPREGNAME, true );
//	DBW("Handle=%08X",Handle);

#ifdef _DEBUG
	Left = Ini->ReadInteger( PFS_MAIN, "Left", Left );
	Top = Ini->ReadInteger( PFS_MAIN, "Top", Top );
	Width = Ini->ReadInteger( PFS_MAIN, "Width", Width );
	Height = Ini->ReadInteger( PFS_MAIN, "Height", Height );
#endif

	GroupOpen = Ini->ReadInteger( PFS_CONFIG, PFS_GROUPOPEN, false );
	GroupName = Ini->ReadString( PFS_CONFIG, PFS_GROUPNAME, "" );
	CtrlClose = Ini->ReadInteger( PFS_CONFIG, PFS_CTRLCLOSE, false );
	IgnoreJ   = Ini->ReadInteger( PFS_CONFIG, PFS_IGNOREJ, DEF_IGNOREJ );
	Banner    = Ini->ReadInteger( PFS_CONFIG, PFS_BANNER, true );
	EnableAdvanced = Ini->ReadInteger( PFS_CONFIG, PFS_ADVANCED, false );

#ifndef _DEBUG
	BorderStyle = bsNone;

	Memo->Visible = false;
	Panel1->Visible = false;
	Splitter1->Visible = false;
	Menu->Items->Delete( 0 );
	Image->Visible = true;
//	Title->Caption = Title->Caption + "   " + StrVersion;
//	Title->Align = alBottom;
#if 0
	Title->Visible = true;
#endif

	AnsiString bmp = ExtractFilePath( Application->ExeName );
	bmp += NAME_BMPFILE;
	if ( FileExists( bmp ) ){
		Image->Picture->Bitmap->LoadFromFile( bmp );
	}

	Width = Image->Picture->Width;
	Height = Image->Picture->Height;

	// To center
	Left += ( ( ( GetSystemMetrics( SM_CXSCREEN ) - ( Width ) ) / 2 + 4 ) & ~7 ) - Left;
	Top += ( ( GetSystemMetrics( SM_CYSCREEN ) - ( Height ) ) / 2 ) - Top;
#endif

	SavedWidth = Width;

#ifdef _DEBUG
	BootTimer->Interval = 100;
	Width = SavedWidth;
	WindowState = wsNormal;
#else
	if ( !Banner ){
		BootTimer->Interval = 10;
		// なんとVisible = false;にすると、
		// アイコン化したとき・・・アイコンが表示される
		// アイコン化しないとき・・アイコンは出てこない
		// 変な仕様？
		// というわけで幅をゼロにしてWindowを非表示化とうい邪道な方法
		// 最初からずっとVisible=falseという方法もあるが、
		// VCLがMainWindowを必ずvisibleにする処理を行っているため不可能
		Width = 0;
	} else {
		Visible = true;	// 最小化で起動すると、アイコン表示が消えないため。
						// この方法が適当であるかどうか深く考えていない・・
						// (たまたまうまく動いた:-)
		Width = SavedWidth;
		WindowState = wsNormal;
	}
#endif

	//TODO: default=ONにすると不安定になるのでしばらくdef=false
	//MouseIncSrch = Ini->ReadInteger( PFS_CONFIG, PFS_INCSRCH, MouseIncSrch );
	CaptureMode = Ini->ReadInteger( PFS_CONFIG, PFS_CAPTURE_MODE, CaptureMode );
	InitATSOCR();
	btnOK->Left = (SavedWidth - btnOK->Width)>>1;
	btnOK->Top = Height - btnOK->Height - 8;
	lbVersion->Caption = GetStrVersion();
	lbVersion->Left = btnOK->Left + btnOK->Width + 4;
	lbVersion->Top = btnOK->Top;
	
	BootTimer->Enabled = true;

	if (!ATSOCRAvail && (GetActualCaptureMode() & CM_IMAGE)){
		tmDNFNotify->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	if (ATSOCRAvail){
		TerminateATSOCR();
	}
	if (OCRTextForm){
		OCRTextForm->Visible = false;
		delete OCRTextForm;
		OCRTextForm = NULL;
	}
	RegisterTrayIcon( false );
#ifdef _DEBUG
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement( Handle, &wp );
	RECT &rc = wp.rcNormalPosition;
	Ini->WriteInteger( PFS_MAIN, "Left", rc.left );
	Ini->WriteInteger( PFS_MAIN, "Top", rc.top );
	Ini->WriteInteger( PFS_MAIN, "Width", rc.right - rc.left );
	Ini->WriteInteger( PFS_MAIN, "Height", rc.bottom - rc.top );
#endif

	miUnhookClick( this );

	delete Ini;
	if (PopupText){
		delete[] PopupText;
		PopupText = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::FormKeyPress(TObject *Sender, char &Key)
{
#ifndef _DEBUG
	if ( Key == 0x1b ){
		Key = 0;
		Start();
	}
#endif
}
//---------------------------------------------------------------------------
// Menu event
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miDicGroup(TObject *Sender)
{
	// 辞書グループ
	AnsiString name = ((TMenuItem*)Sender)->Caption;
	if ( name.Length() ){
		OpenDicGroup( name.c_str() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miClickOnlyClick(TObject *Sender)
{
	miClickOnly->Checked = !miClickOnly->Checked;
	EnableClickOnly( miClickOnly->Checked );
	MenuClosed();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miIncSearchClick(TObject *Sender)
{
	miIncSearch->Checked = !miIncSearch->Checked;
	MouseIncSrch = miIncSearch->Checked;
	Ini->WriteInteger( PFS_CONFIG, PFS_INCSRCH, MouseIncSrch );
	SetupConfig2();
	if (MouseIncSrch){
		HWND hwnd = FindPdic();
		if (hwnd){
			MoveToTop(hwnd);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miEnablePopupClick(TObject *Sender)
{
	if ( miEnablePopup->Checked ){
		miUnhookClick( this );
		EnablePopup = false;
	} else {
		miHookClick( this );
		EnablePopup = true;
	}
	MenuClosed();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miOCRTextClick(TObject *Sender)
{
	if (!OCRTextForm) OCRTextForm = new TOCRTextForm(this);
	OCRTextForm->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miOptionClick(TObject *Sender)
{
	if (PopupConfigDlg){
		MoveToTop(PopupConfigDlg->Handle);
		return;
	}

	// Configuration //
	PopupConfigDlg = new TPopupConfigDlg( this );
	PopupConfigDlg->cbGroupOpen->Checked = GroupOpen;
	PopupConfigDlg->edGroupName->Text = GroupName;
	PopupConfigDlg->SetPopupKey( Ini->ReadInteger( PFS_CONFIG, PFS_POPUPKEY, DEF_POPUPKEY ) );
	PopupConfigDlg->SetToggleKey( Ini->ReadInteger( PFS_CONFIG, PFS_TOGGLEKEY, DEF_TOGGLEKEY ) );
	PopupConfigDlg->cbCtrlClose->Checked = CtrlClose;
	PopupConfigDlg->cbIgnoreJ->Checked = IgnoreJ;
	PopupConfigDlg->cbBanner->Checked = Banner;
	PopupConfigDlg->cbAdvanced->Checked = EnableAdvanced;
	PopupConfigDlg->SetCaptureMode( CaptureMode );
	PopupConfigDlg->SetScaler( Ini->ReadInteger(PFS_CONFIG, PFS_SCALE, 0) );
	bool act_use64 = IsActualUse64();
	bool use64 = Ini->ReadInteger(PFS_CONFIG, PFS_USE64, DEF_USE64);
	PopupConfigDlg->cbUse64->Checked = use64;
	SetForegroundWindow( PopupConfigDlg->Handle );
	if ( PopupConfigDlg->ShowModal() == IDOK ){
		GroupOpen = PopupConfigDlg->cbGroupOpen->Checked;
		GroupName = PopupConfigDlg->edGroupName->Text;
		CtrlClose = PopupConfigDlg->cbCtrlClose->Checked;
		IgnoreJ = PopupConfigDlg->cbIgnoreJ->Checked;
		Banner = PopupConfigDlg->cbBanner->Checked;
		Ini->WriteInteger( PFS_CONFIG, PFS_POPUPKEY,
			PopupConfigDlg->GetPopupKey() );
		Ini->WriteInteger( PFS_CONFIG, PFS_TOGGLEKEY,
			PopupConfigDlg->GetToggleKey() );
		EnableAdvanced = PopupConfigDlg->cbAdvanced->Checked;
		CaptureMode = PopupConfigDlg->GetCaptureMode();
		Ini->WriteInteger( PFS_CONFIG, PFS_ADVANCED, EnableAdvanced );
		Ini->WriteInteger( PFS_CONFIG, PFS_CAPTURE_MODE, CaptureMode );
		Ini->WriteInteger( PFS_CONFIG, PFS_SCALE, PopupConfigDlg->GetScaler() );
		Ini->WriteInteger(PFS_CONFIG, PFS_USE64, PopupConfigDlg->cbUse64->Checked);
		SaveConfig();
		SetupConfig();
		tmReInit->Enabled = false;
		if (act_use64 != IsActualUse64()){
			// 64bit hook 変更時
			Unhook();
			Hook();
		}
	}
	delete PopupConfigDlg;
	PopupConfigDlg = NULL;
}
void __fastcall TDCHookMainForm::miReadMeClick(TObject *Sender)
{
	ShowManual(Handle);
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miAboutClick(TObject *Sender)
{
	// show version dialog
#if 1
	Width = SavedWidth;
	btnOK->Visible = true;
	btnOK->SetFocus();
	Visible = true;
#else
	VersionDlg = new TVersionDlg( this );
	VersionDlg->ShowModal();
	delete VersionDlg;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miCheckLatestClick(TObject *Sender)
{
	ShowLatestVersion();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miCloseClick(TObject *Sender)
{
	MenuClosed();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miReloadClick(TObject *Sender)
{
	// 再起動
	Unhook();
	Hook();
	MenuClosed();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miHookClick(TObject *Sender)
{
	if ( !Hook() ){
		Application->MessageBox( "DLLを初期化することができません\r終了します", APPNAME, MB_OK );
		Close();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miUnhookClick(TObject *Sender)
{
	Unhook();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miConfigClick(TObject *Sender)
{
	miOptionClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miDdeTestClick(TObject *Sender)
{
#ifdef _DEBUG
	DoPopup( _t("test"), 0, NULL, false );
#endif
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miTestClick(TObject *Sender)
{
#ifdef _DEBUG
#if 1
	hDll->Init32(Handle);
#endif
#if 0
	miIncSearchClick(Sender);
#endif
#if 0
	if (hDll){
		hDll->Capture();
	}
#endif
#endif	// _DEBUG
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::miExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
// Popup on tray icon //
void __fastcall TDCHookMainForm::pmTrayIconPopup(TObject *Sender)
{
	// Add group names to menu
	TAppIni ini;
	if ( !ini.hroot ){
		return;
	}

	int sel = ini.ReadInteger( PFS_COMMON, PFS_GRPSEL, -1 );

	TStringList *strs = new TStringList;
	TMenuItem *sub = pmTrayIcon->Items->Items[MIGROUP_INDEX];
	for ( ;sub->Count;){
		sub->Delete(0);
	}
	GetGroupList(ini, *strs);
	int index = 0;
	for ( int i=0;i<strs->Count;i++ ){
		AnsiString name = strs->Strings[i];
		if (name.IsEmpty() || name.AnsiPos("::")==1){
			continue;	// Ignore the group.
		}
		TMenuItem *item = new TMenuItem( this );
		item->Caption = name;
		item->OnClick = miDicGroup;
		sub->Add( item );
		// Check current group name //
		if ( sel == index ){
			item->Checked = true;
		}
		index++;
	}
	delete strs;
	miGroup->Visible = sub->Count != 0;
	// Check enable popup item //
	miIncSearch->Checked = MouseIncSrch;
	miEnablePopup->Checked = EnablePopup;

	miOCRText->Visible = GetActualCaptureMode() & CM_IMAGE ? true : false;
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::btnOKClick(TObject *Sender)
{
#ifndef _DEBUG
	Width = 0;
#endif
	MenuClosed();
}
//---------------------------------------------------------------------------
// Timers
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::TestTimerTimer(TObject *Sender)
{
#ifdef _DEBUG
#if 0
	if ( DCDebug ){
		char *p = (char*)DCDebug( );
		if ( memcmp( share, p, COMPSIZE ) ){
			DBW("Different");
			for ( int i=0;i<COMPSIZE;i++ ){
				DBW("[%02X:%02X]", (unsigned char)share[i],(unsigned char)p[i]);
			}
			memcpy( share, p, COMPSIZE );
		}
	}
#endif
#endif
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::BootTimerTimer(TObject *Sender)
{
	Start();
}
//---------------------------------------------------------------------------
// これをやらないと、”メニューを閉じる”を実行後、
// アイコンがタスクバーに残ってしまう。なぜ？？
void __fastcall TDCHookMainForm::tmMenuCloseTimer(TObject *Sender)
{
	Visible = true;
	Visible = false;
	ShowWindow( Application->Handle, SW_HIDE );
	tmMenuClose->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::tmNotifyTimer(TObject *Sender)
{
	if ( NotifyForm ){
		tmNotify->Enabled = false;
		delete NotifyForm;
		NotifyForm = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::tmPopupTimer(TObject *Sender)
{
	PostMessage( Handle, WM_POPUP, PopupLoc | (PopupMoveSend ? FLG_MOVESEND :  0), (LPARAM)PopupText );
	PopupText = NULL;	// release ownership
	tmPopup->Enabled = false;	// stop timer
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::tmMouseMoveTimer(TObject *Sender)
{
	tmMouseMove->Enabled = false;

	if (!hDll)
		return;
	//hDll->Capture();
	hDll->CaptureAsync();
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::tmDNFInstallCheckTimer(TObject *Sender)
{
	if (ATSOCRRunable()){
		// 最初のpopupで落ちてしまう？
		if (!ATSOCRAvail && (GetActualCaptureMode() & CM_IMAGE)){
			InitATSOCR();
		}
		if (ATSOCRAvail){
			SetupConfig2();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::tmDNFNotifyTimer(TObject *Sender)
{
	tmDNFNotify->Enabled = false;
	if (!ATSOCRRunable()){
		NotifyDNF();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::tmReInitTimer(TObject *Sender)
{
	// amodi.exeのlaunchに非常に時間がかかる場合がある
	// ex.古いPCでstartupに登録している場合
	//    →input idleになってもmain windowの生成に時間がかかっているのかもしれない
	// ATSOCRAvailがtrueになるまで初期化を続ける
	tmReInit->Enabled = false;
	SetupATSOCR();
	if (ATSOCRAvail){
		SetupConfig2();
	}
}
//---------------------------------------------------------------------------
// Mouse Events
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::ImageMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if ( BootUp ) return;
	Capturing = true;
	SetCapture( Handle );
	CapPoint.x = X;
	CapPoint.y = Y;
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::ImageMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
	if ( Capturing ){
		POINT pt;
		GetCursorPos( &pt );
		Left = pt.x - CapPoint.x;
		Top = pt.y - CapPoint.y;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDCHookMainForm::ImageMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if ( BootUp )
		Start();
	Capturing = false;
}
//---------------------------------------------------------------------------
// User Functions
//---------------------------------------------------------------------------
void TDCHookMainForm::InitATSOCR()
{
	SetupATSOCR();
}
void TDCHookMainForm::Start()
{
	BootTimer->Enabled = false;
	if ( BootUp ){
		RegisterTrayIcon( true );
		miHookClick( this );
		BootUp = false;
	}
#ifndef _DEBUG
	ShowWindow( Application->Handle, SW_HIDE );
	Visible = false;	// これをfalseにしないとAlt+Tabでmain windowが見えてしまう
	Width = 0;
#endif
}
//---------------------------------------------------------------------------
bool TDCHookMainForm::Hook()
{
	if ( hDll ){
		DBW("Already hooked");
		return true;	// already loaded
	}

	bool use64 = Ini->ReadInteger(PFS_CONFIG, PFS_USE64, DEF_USE64);
	hDll = new TDCHookLoader(use64);
	if (!hDll->LoadHook(Handle)){
		DBW("Load failed");
		delete hDll;
		hDll = NULL;
		return false;
	}

	miHook->Enabled = false;
	miUnhook->Enabled = true;

	if (hDll->IsReady()){
		SetupConfig();
		extern HWND hwnd64;
		PostMessage(Handle, UM_SHORTCUT, SCINX_NOTIFY64, (LPARAM)hwnd64);
	}

	DBW("Hook OK");
	
	return true;
}
void TDCHookMainForm::Unhook()
{
	if ( !hDll ) return;

	hDll->UnloadHook();
	delete hDll;
	hDll = NULL;

	miHook->Enabled = true;
	miUnhook->Enabled = false;
}
//---------------------------------------------------------------------------
#ifdef _DEBUG
// textの先頭を(0,0)とした座標で、posの位置にあるtextの文字位置を返す
int GetTextFromPoint( HDC hdc, const char *text, int pos )
{
	int len = lstrlen(text);
	int left = 0;
	int right = len;
	SIZE sz;
	int loc;
	for(;;){
		loc = (right + left)/2;
		if ( right - left <= 1 ){
			break;
		}
		GetTextExtentPoint32A( hdc, text, loc, &sz );
//		DBW("%08X %d %d %d %d", hdc, left, right, loc, sz.cx);
		if ( pos < sz.cx ){
			right = loc;
		} else {
			left = loc;
		}
	}
	return (right+left)/2;
}
int GetTextFromPoint( HDC hdc, const wchar_t *text, int pos )
{
	int len = lstrlenW(text);
	int left = 0;
	int right = len;
	SIZE sz;
	int loc;
	for(;;){
		loc = (right + left)/2;
		if ( right - left <= 1 ){
			break;
		}
		GetTextExtentPoint32W( hdc, text, loc, &sz );
//		DBW("%08X %d %d %d %d", hdc, left, right, loc, sz.cx);
		if ( pos < sz.cx ){
			right = loc;
		} else {
			left = loc;
		}
	}
	return (right+left)/2;
}
#endif
void TDCHookMainForm::EvCopyData(TMessage& msg )
{
	COPYDATASTRUCT *cds = (COPYDATASTRUCT*)msg.LParam;
	if ( !cds ){ return; }
	switch ( cds->dwData ){
		case DCH_MOVESEND:
			//DBW("DCH_MOVESEND");
			// mouse move message
			EvMoveSend(msg.WParam==0);
			break;
		case DCH_START:
			CursorPoint = *(POINT*)cds->lpData;
#ifdef _DEBUG
			PaintBox->Canvas->FillRect( PaintBox->Canvas->ClipRect );
#endif
			if (PopupText){
				delete[] PopupText;
				PopupText = NULL;
			}
			break;
		case DCH_END:
		case DCH_END | DCH_MOVESEND:
			if ( PopupText ){
				PopupMoveSend = cds->dwData & DCH_MOVESEND ? true : false;
				PostMessage( Handle, WM_POPUP, PopupLoc | (PopupMoveSend ? FLG_MOVESEND : 0), (LPARAM)PopupText );
				if (OCRTextForm) OCRTextForm->SetText(PopupText);
				PopupText = NULL;	// release ownership
			}
			break;
		case DCH_HITTEXT1:	// single char
		{
			DBW("DCH_HITTEXT1");
			delete[] PopupText;
#ifdef USE_UNICODE
			// ANSI -> Unicode
			//*+++ 動作未確認 2002.12.4
			PopupText = new tchar[ cds->cbData*2 + 1 ];
			// 前半部分
			PopupLoc = MultiByteToWideChar( CP_ACP, 0,
				(LPCSTR)cds->lpData, msg.WParam, PopupText, cds->cbData*2 );
			// 後半部分
			int r = MultiByteToWideChar( CP_ACP, 0,
				&((char*)cds->lpData)[msg.WParam], cds->cbData-msg.WParam, &PopupText[PopupLoc], cds->cbData-PopupLoc );
			PopupText[PopupLoc+r] = '\0';
#else	// !USE_UNICODE
			PopupText = new tchar[ cds->cbData + 1 ];
			memcpy( PopupText, cds->lpData, cds->cbData );
			PopupText[cds->cbData] = '\0';
			PopupLoc = msg.WParam;
#ifdef _DEBUG
			Memo->Lines->Add( PopupText );
			edFound->Text = PopupText + msg.WParam;
			TestTimer->Enabled = true;
			if ( DCDebug ){
				char *p = (char*)DCDebug( );
				memcpy( share, p, COMPSIZE );
			}
#endif
#endif
		}
			break;
		case DCH_HITTEXT2:	// wide char
		{
			DBW("DCH_HITTEXT2");
			delete[] PopupText;
#ifdef USE_UNICODE
			PopupText = new tchar[ cds->cbData + 1 ];
			memcpy( PopupText, cds->lpData, cds->cbData );
			PopupText[cds->cbData/sizeof(tchar)] = '\0';
			PopupLoc = msg.WParam;
#else	// !USE_UNICODE
			// Unicode -> ANSI
			//*+++ 動作未確認 2002.12.4
			PopupText = new tchar[ cds->cbData + 1 ];
			// 前半部分
			BOOL defchar;
			PopupLoc = WideCharToMultiByte( CP_ACP, 0,
				(wchar_t*)cds->lpData, msg.WParam,
				PopupText, cds->cbData, 0, &defchar );
			// 後半部分
			int r = WideCharToMultiByte( CP_ACP, 0,
				&((wchar_t*)cds->lpData)[msg.WParam], (cds->cbData>>1)-msg.WParam,
				&PopupText[PopupLoc], cds->cbData-PopupLoc, 0, &defchar );
			PopupText[PopupLoc+r] = '\0';
#endif
		}
			break;
#ifdef _DEBUG
		case DCH_EXTTEXTOUTA:
		case DCH_EXTTEXTOUTW:
		{
			EMREXTTEXTOUTW *emr = (EMREXTTEXTOUTW*)cds->lpData;
			wchar_t *p = (wchar_t*) ( ((char*)emr) + emr->emrtext.offString );
			char *buf = NULL;
#if 0
			char b[200];
			sprintf(b,"exScale=%f eyScale=%f ptlReference=(%d,%d)",emr->exScale, emr->eyScale, emr->emrtext.ptlReference.x, emr->emrtext.ptlReference.y );
			DBW(b);
#endif
			if ( cds->dwData == DCH_EXTTEXTOUTW ){
				buf = new char[ emr->emrtext.nChars * 2 ];
				memset( buf, 0, emr->emrtext.nChars*2 );
				WideCharToMultiByte( CP_ACP, 0, p, emr->emrtext.nChars, buf, emr->emrtext.nChars * 2, NULL, NULL );
			}
			const char *text = buf ? buf : (char*)p;
			Memo->Lines->Add( text );
			PaintBox->Canvas->TextOut( emr->rclBounds.left, emr->rclBounds.top, text );

			if ( PtInRect( (RECT*)&emr->rclBounds, CursorPoint ) ){
				int loc = GetTextFromPoint( (HDC)msg.WParam, text, CursorPoint.x - emr->rclBounds.left );
				if ( loc != -1 ){
					edFound->Text = text + loc;
				}
			}
			delete[] buf;
		}
			break;
#endif
		case DCH_MENU:
			DokoPopMenu( false );
			break;
		case DCH_TOGGLE:
			PostMessage( Handle, WM_TOGGLE, 0, 0 );
			break;
		case WMCD_EXISTCHECK:	// exist check
			if ( cds->cbData != strlen(APPNAME)+1 ){
				msg.Result = false;
				return;
			}
			msg.Result = (memcmp( cds->lpData, APPNAME, strlen(APPNAME)+1 ) == 0);
			return;
		case DCH_LAUNCH_ATSOCR:
			SetupATSOCR();
			//Reboot();		// ATSOCR.exe再起動では解決できないため
			break;
	}
	return;
}
//---------------------------------------------------------------------------
bool WaitTransaction( TDdeClientConv *dde )
{
#if 1
	for ( int i=0;i<100000;i++ ){
		Application->ProcessMessages();
		if ( !dde->WaitStat ){
//			DBW("DCHook:Wait OK");
			return true;
		}
	}
	DBW("DCHook:Wait failure");
	return false;
#else
	return true;
#endif
}
// ANSIで渡す場合
bool DdePoke( TDdeClientConv *dde, AnsiString Item, AnsiString Data )
{
#ifdef USE_UNICODE
	int ansilen = Data.Length();
	wchar_t *buf = new wchar_t[ansilen*2+1];
	int len = MultiByteToWideChar( CP_ACP, 0,
		(LPCSTR)Data.c_str(), ansilen, buf, ansilen*2 );
	buf[len] = '\0';
	bool r = DdePoke( dde, Item, (wchar_t*)buf );
	delete[] buf;
	return r;
#else	// !USE_UNICODE
	HSZ hszItem = DdeCreateStringHandle(ddeMgr->DdeInstId, Item.c_str(), CP_WINANSI );
	if ( !hszItem ) return false;
	HDDEDATA hszDat = DdeCreateDataHandle( ddeMgr->DdeInstId, Data.c_str(), Data.Length() + 1,
		0, hszItem, dde->DdeFmt, 0);
	bool r = false;
	if ( hszDat ){
		HDDEDATA hdata = DdeClientTransaction( (LPBYTE)hszDat, (DWORD)-1, (HCONV)dde->Conv, hszItem,
			dde->DdeFmt, XTYP_POKE, 10000, NULL);
		if (hdata || DdeGetLastError(ddeMgr->DdeInstId) != DMLERR_NO_ERROR){
			r = true;
			if (hdata)
				DdeFreeDataHandle( hdata );
		}
	}
	DdeFreeStringHandle(ddeMgr->DdeInstId, hszItem);
	return r;
#endif
}
#ifdef USE_UNICODE
// Unicodeで渡す場合
bool DdePoke( TDdeClientConv *dde, AnsiString Item, const wchar_t *Data )
{
	// Item名はANSI、データはUNICODE //
	int ddeFmt = /*dde->DdeFmt*/CF_UNICODETEXT;
	HSZ hszItem = DdeCreateStringHandle(ddeMgr->DdeInstId, Item.c_str(), CP_WINANSI );
	if ( !hszItem ) return false;
	HDDEDATA hszDat = DdeCreateDataHandle( ddeMgr->DdeInstId, (LPBYTE)Data, (_tcslen(Data)+1)*sizeof(wchar_t),
		0, hszItem, ddeFmt, 0);
	bool r = false;
	if ( hszDat ){
		HDDEDATA hdata = DdeClientTransaction( (LPBYTE)hszDat, (DWORD)-1, (HCONV)dde->Conv, hszItem,
			ddeFmt, XTYP_POKE, 10000, NULL);
		if (hdata || DdeGetLastError(ddeMgr->DdeInstId) != DMLERR_NO_ERROR){
			r = true;
			if (hdata)
				DdeFreeDataHandle( hdata );
		}
	}
	DdeFreeStringHandle(ddeMgr->DdeInstId, hszItem);
	return r;
}
bool DdeRequest( TDdeClientConv *dde, AnsiString Item, char *Data, int &ioDataLen )
{
	const int ddeFmt = /*dde->DdeFmt*/CF_UNICODETEXT;
	HSZ hszItem = DdeCreateStringHandle(ddeMgr->DdeInstId, Item.c_str(), CP_WINANSI );
	if ( !hszItem ) return false;
	bool r = false;
	HDDEDATA hdata = DdeClientTransaction( NULL, 0, (HCONV)dde->Conv, hszItem, ddeFmt, XTYP_REQUEST, 10000, NULL);
	if (hdata || DdeGetLastError(ddeMgr->DdeInstId) != DMLERR_NO_ERROR){
		r = true;
		if (hdata){
			ioDataLen = DdeGetData(hdata, (LPBYTE)Data, ioDataLen, 0);
			DdeFreeDataHandle( hdata );
		} else {
			Data[0] = '0';
			ioDataLen = 0;
		}
	}
	DdeFreeStringHandle(ddeMgr->DdeInstId, hszItem);
	return r;
}
#endif
TDdeClientConv *TDCHookMainForm::OpenPdic( const char *topic )
{
	TDdeClientConv *PdicDde = new TDdeClientConv( this );

	PdicDde->ConnectMode = ddeManual;
	static int entry = 0;
	PdicDde->Name = AnsiString("PdicDde") + (++entry);
	PdicDde->ServiceApplication = DDE_SERVICE_NAME;
	if ( !PdicDde->SetLink( DDE_SERVICE_NAME, topic ) ){
		DBW("DCHook:Cannot SetLink");
		delete PdicDde;
		return NULL;
	}
	if ( !PdicDde->OpenLink( ) ){	// ここが遅い！！
		// PDIC auto launch
		if ( LaunchPdic() ){
			for(int i=0;i<OPEN_RETRY;i++){
				if ( PdicDde->OpenLink() ){
					return PdicDde;
				}
				Sleep(OPEN_WAITTIME);
			}
		}
		DBW("PDICが見つかりません(OpenPdic)");
		Application->MessageBox( "PDICが見つかりません(OpenPdic)", APPNAME, MB_OK );
		delete PdicDde;
		return NULL;
	}
	return PdicDde;
}
#ifdef USE_UNICODE
static const TCHAR *clsname = TEXT("TPdicMain.UnicodeClass");
#else
static const TCHAR *clsname = TEXT(DDE_SERVICE_NAME);
#endif
static const TCHAR *winname = NULL;
HWND TDCHookMainForm::FindPdic( )
{
	HWND hWin = FindWindow( clsname, winname );
	if ( !hWin ){
		// PDIC auto launch
		if ( LaunchPdic() ){
			hWin = FindWindow( clsname, winname );
			if ( !hWin ){
				DBW("PDICが見つかりません(FindPdic)");
				Application->MessageBox( "PDICが見つかりません(FindPdic)", APPNAME, MB_OK );
				return NULL;
			}
		}
	}
	return hWin;
}
#if !USE_DDE
LRESULT TDCHookMainForm::SendCopyData( HWND hwnd, int message, const char *str )
{
	COPYDATASTRUCT cds;
	cds.dwData = message;
	cds.lpData = (LPSTR)str;
	cds.cbData = lstrlen(str)+1;
	SendMessage( hwnd, WM_COPYDATA, (WPARAM)Handle, (LPARAM)&cds );
}
#endif
void TDCHookMainForm::ClosePdic( TDdeClientConv *dde )
{
	dde->CloseLink();
	delete dde;
}
// XX.XX.XXXX (hex) 1+1+2 byte構成
int TDCHookMainForm::GetPdicVersion()
{
	TDdeClientConv *PdicDde = OpenPdic("PDIC");
	if (!PdicDde) return 0;
	int version = 0;
#ifdef USE_UNICODE
	char buf[40];
	int size = sizeof(buf);
	if (DdeRequest(PdicDde, "GetVersion", buf, size)){
		wchar_t *str = (wchar_t*)buf;
		version = _wtoi( str ) << 24;
		const wchar_t *p = wcschr( str, '.' );
		if (p){
			version |= _wtoi( p+1 ) << 16;
			p = wcschr( p+1, '.' );
			if (p){
				version |= _wtoi( p+1 );
			}
		}
		//DBW("version: %X", version);
	}
#endif
	ClosePdic( PdicDde );
	return version;
}
// click_pos: mouse clickされたtext上の文字位置(text先頭からのoffset)
bool TDCHookMainForm::DoPopup( const tchar *text, int click_pos, const tchar *prevtext, bool movesend )
{
	static int ct = 0;
	if ( ct >= 1 ){
		if (PopupText){
			// already pending
			DBW("Reentrant!!!!!");
			return true;
		}
		HWND hwnd = FindPopupWindow( );
		if (hwnd){
			HWND hwndParent = GetParent( hwnd );
			if (hwndParent){
				DBW("Parent exist!!!!");
				hwnd = hwndParent;
			}
			DBW("Retry again:%d:%08X",ct,hwnd);
			// 必ずPostMessage()でcloseすること。
			// CloseWindow()ではPopup Winodowの初期化中に
			// close処理を実行してしまい、
			// popup windowがおかしな状態にはいってしまう場合がある
			PostMessage( hwnd, WM_CLOSE, 0, 0 );
			return false;
		} else {
			if (ct >= 1 ){
				DBW("Not found popup window");
				if (DoPopupRetry++ > 10){
					DoPopupRetry = 0;
					return true;	// abort
				} else {
					return false;	// retry
				}
			}
		}
	}
	DoPopupRetry = 0;
	ct++;

#if USE_DDE
	// Windows9xでは、DDE transactionが遅いときがある
	// why? - because of 24bit OS:-)
	// ->と思って、WM_COPYDATA versionを作っていたら速くなった？？？
	// ->WM_COPYDATA化 projectは中断
	TDdeClientConv *PdicDde;
	bool r;

	if (movesend){
		// incremental search
		PdicDde = OpenPdic("Simulate");
		if (!PdicDde){
			ct--;
			return true;
		}
		r = DdePoke(PdicDde, "InputWord3", text);
		if (!r){
			DBW("PDIC DDE : InputWord3 failure");
			delete PdicDde;
			ct--;
			return true;
		}
	} else {
		// popup
		if (GroupOpen){
			PdicDde = OpenPdic( "Simulate" );
			if (PdicDde){
				DdePoke( PdicDde, "Open", GroupName );
				PdicDde->CloseLink();
				delete PdicDde;
			}
		}
		PdicDde = OpenPdic( "Dictionary" );
		if ( !PdicDde ){
			ct--;
			return true;
		}

		r = DdePoke( PdicDde, "Open", "" );
		if ( !r ){
			DBW("PDIC DDE : Open failure");
			PdicDde->CloseLink();
			delete PdicDde;
			ct--;
			if ( GroupOpen ){
				static int ct = 0;
				ct++;
				if ( ct == 1 ){
					Application->MessageBox(
						"辞書グループが開けません\r"
						"DokoPop!で「辞書グループ指定」を有効にしている場合は、すでにPDIC本体でオープンされている辞書と共有違反を起こしている可能性があります\r"
						"DokoPop!とPDIC本体で使用している辞書に同一のものがないか調べてください\r"
						"もしあった場合は登録禁止など共有できるような状態にしてください\r"
						"このメッセージの意味がわからない場合は、「辞書グループ指定」機能を使わないほうが安全です"
						,
						APPNAME, MB_OK );
				} else {
					SetForegroundWindow( Handle );
				}
				ct--;
			}
			return true;
		}
		if ( CtrlClose )
			DdePoke( PdicDde, "PopupSearchConfig", "c1" );	// Ctrl close
		//	Sleep(1000);
		DdePoke( PdicDde, "PopupSearchConfig", "o1w1" );	// overlap window and no wait transaction
		//DBW("prevtext="FMTS,prevtext);
		int len = _tcslen(prevtext);
		tchar *buf = new tchar[len+10];
		_itow( STR_DIFF(text, prevtext) + click_pos, buf, 10 );	// クリック位置
		tchar *dp = buf + _tcslen(buf);
		*dp++ = ',';
		wcscpy( dp, prevtext );
		DdePoke( PdicDde, "PopupSearch3", buf );
		delete[] buf;
		WaitTransaction( PdicDde );
		DdePoke( PdicDde, "Close", "" );
	}
	ClosePdic( PdicDde );
#else	// !USE_DDE
		// WM_COPYDATAによる方法(まだdocument化されていない方法(Ver.4.30)
		// PDIC/W32 Ver.4.22～に対応
		// ただし、Ver.4.29までは、WMCD_POPUPSEARCHとWMCD_POPUPSEARCH2は同じ

	// WM_COPYDATA code //
	// 以下の数値は将来変わる可能性がある(undocumented) //
	#define	WMCD_GETVERSION		0x0001
	#define	WMCD_CHECKOPEN		0x0010
	#define	WMCD_AUTOSEARCH		0x0020
	#define	WMCD_AUTOSEARCHMODE	0x0021

	#define	WMCD_ISOPENED			0x0080
	#define	WMCD_CLOSE				0x0081

	#define	WMCD_POPUPSEARCH		0x0100
	#define	WMCD_POPUPSEARCH2		0x0101
	#define	WMCD_POPUPSEARCHPOINT	0x0102
	#define	WMCD_POPUPSEARCHCONFIG	0x0103
	#define	WMCD_POPUPSEARCHDIALOG	0x0104
	#define	WMCD_POPUPSEARCHWINDOW	0x0105

	#define	WMCD_SPELLCHECK			0x0110

	HWND hwnd = FindPdic();
	if ( !hwnd ){
		ct--;
		return;
	}

	bool r;
	if ( CtrlClose )
		SendCopyData( hwnd, WMCD_POPUPSEARCHCONFIG, "c1" );
	POINT pt;
	GetCursorPos( &pt );
	char buf[20];
	wsprintf( buf, "%d,%d", pt.x, pt.y );
	SendCopyData( hwnd, WMCD_POPUPSEARCHPOINT, buf );
	if ( prevtext && (prevtext != text) ){
		DBW("prevtext=%s",prevtext);
		SendCopyData( hwnd, WMCD_POPUPSEARCH2, prevtext );
	} else {
		DBW("text=%s",text);
		SendCopyData( hwnd, WMCD_POPUPSEARCH, text );
	}
	SendCopyData( hwnd, WMCD_CLOSE, "" );
#endif	// !USE_DDE
	ct--;
	DBW("DCHook:Exit Popup:%d",ct);

#if DETACHRELEASE
	if ( EnablePopup ){
		miUnhookClick( this );
		miHookClick( this );
	}
#endif
	return true;
}
//---------------------------------------------------------------------------
// EvPopup
// WParam : text location & FLG_MOVESEND
// LParam : text pointer (must be freed when done)
void TDCHookMainForm::EvPopup(TMessage &msg)
{
	//DBW("EvPopup");
	if (!msg.LParam)
		return;

	int pdicVer = GetPdicVersion();
	const int numPrev = pdicVer >= 0x050A0018 ? 2 : 1;	// ver.5.10.24以上
		
	tchar *text = (tchar*)msg.LParam;
	int loc = msg.WParam & ~FLG_MOVESEND;
	bool movesend = msg.WParam & FLG_MOVESEND ? true : false;
	
	int start, end;
	int prevstart;
	if ( !GetWord( text, loc, start, end, prevstart, true, 10, true, IgnoreJ, numPrev ) ){
		// no words
		delete[] text;
		return;
	}
#ifdef _DEBUG
	edWord->Text = text + start;
	edPrev->Text = loc + prevstart;
#endif

	if (DoPopup( text + start, loc - start, text + prevstart, movesend )){
		// done
		delete[] text;
	} else {
		// retry again
		delete[] PopupText;	// あとに来たほうを優先する
		PopupText = text;
		PopupLoc = loc;
		PopupMoveSend = movesend;
		tmPopup->Enabled = true;	// start timer for delayed popup
	}
	//DBW("EvPopup - end");
}
void TDCHookMainForm::EvMoveSend(TMessage &msg)
{
	EvMoveSend(msg.WParam==0);
}
void TDCHookMainForm::EvMoveSend(bool enable)
{
	tmMouseMove->Enabled = false;
	if (enable){
		tmMouseMove->Enabled = true;
	}
}
void TDCHookMainForm::RegisterTrayIcon( bool flag )
{
  NOTIFYICONDATA m_tnid = {sizeof(NOTIFYICONDATA)};

  m_tnid.hWnd = Handle;
  m_tnid.uID = 1;
  m_tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  m_tnid.uCallbackMessage = WM_NOTIFYICON;
  // Get small icon from me
  HICON hIconSm;
  HICON hIcon;
  ExtractIconEx( Application->ExeName.c_str(), -2, &hIcon, &hIconSm, 1 );
  m_tnid.hIcon = hIconSm;
  lstrcpy(m_tnid.szTip, APPNAME );
  Shell_NotifyIcon( flag ? NIM_ADD : NIM_DELETE, &m_tnid);
}
void TDCHookMainForm::EvNotifyIcon(TMessage &msg)
{
	//UINT uID = (UINT) msg.WParam;
	UINT uMouseMsg = (UINT) msg.LParam;
	if (uMouseMsg == WM_LBUTTONDBLCLK)
	{
		miOptionClick( this );
	}
	else if (uMouseMsg == WM_RBUTTONDOWN)
	{
		DokoPopMenu( true );
	}
}
//---------------------------------------------------------------------------
void TDCHookMainForm::SaveConfig()
{
	Ini->WriteInteger( PFS_CONFIG, PFS_GROUPOPEN, GroupOpen );
	Ini->WriteString( PFS_CONFIG, PFS_GROUPNAME, GroupName.c_str() );
	Ini->WriteInteger( PFS_CONFIG, PFS_CTRLCLOSE, CtrlClose );
	Ini->WriteInteger( PFS_CONFIG, PFS_IGNOREJ, IgnoreJ );
	Ini->WriteInteger( PFS_CONFIG, PFS_BANNER, Banner );
	Ini->WriteInteger( PFS_CONFIG, PFS_ADVANCED, EnableAdvanced );
}
void TDCHookMainForm::OpenDicGroup( const char *name )
{
	TDdeClientConv *dde = OpenPdic( PFS_PDIC );
	if ( !dde )
		return;
	// 辞書グループオープン処理 //
	if ( name[0] == '&' )
		name++;
	DdePoke(dde, "OpenGroup", (tchar*)mustr(name));
	ClosePdic( dde );
}
//---------------------------------------------------------------------------
// DokoPopの設定
void TDCHookMainForm::SetupConfig()
{
	EnableClickOnly( miClickOnly->Checked );
	if (hDll){
		hDll->Config( -1, KA_POPUP,
			Ini->ReadInteger( PFS_CONFIG, PFS_POPUPKEY, DEF_POPUPKEY ) );
		hDll->Config( -1, KA_TOGGLE,
			Ini->ReadInteger( PFS_CONFIG, PFS_TOGGLEKEY, DEF_TOGGLEKEY ) );

		SetupConfig2();
	}
	SetupATSOCR();
}
void TDCHookMainForm::SetupConfig2()
{
	if (!hDll)
		return;

	TDCHConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.UseATSOCR = ATSOCRAvail && (GetActualCaptureMode() & CM_IMAGE);
	cfg.OnlyATSOCR = ATSOCRAvail && !(GetActualCaptureMode() & CM_TEXT);
	cfg.MoveSend = MouseIncSrch;
	//cfg.OnlyImage = 1;	//TODO:

	int dpiDetect = Ini->ReadInteger(PFS_CONFIG, PFS_DPI_DETECT, 1);
	if (dpiDetect){
#if DPI_DETECT
		cfg.ScaleX = GetMonitorScale();
#endif
	} else {
		cfg.ScaleX = Ini->ReadInteger(PFS_CONFIG, PFS_SCALE, 0);
	}
	cfg.ScaleY = cfg.ScaleX;
	cfg.UseNumPrev = 1;
	cfg.OnlyAlnum = IgnoreJ;
	cfg.NumPrevWords = 2;
	hDll->Config2(&cfg);
}
void TDCHookMainForm::SetupATSOCR()
{
	HWND hwnd = FindATSOCR();
	if (hwnd){
		ATSOCRAvail = true;
	} else {
		ATSOCRAvail = LaunchATSOCR();
		if (!ATSOCRAvail){
			HWND hwnd = FindATSOCR();
			if (hwnd)
				ATSOCRAvail = true;
			else
				tmReInit->Enabled = true;	// retry later
		}
	}
	if (ATSOCRAvail){
		tmDNFInstallCheck->Enabled = false;
	}
}
void TDCHookMainForm::EnableClickOnly( bool enable )
{
	if (hDll)
		hDll->Config( enable, 0, 0 );
}
void TDCHookMainForm::DokoPopMenu( bool all )
{
//	Width = 0;
	SetForegroundWindow( Handle );
	POINT pt;
	GetCursorPos( &pt );
	miEnablePopup->Visible = all;
	miReload->Visible = !all;
	miClose->Visible = !all;
	miExit2->Visible = all;
	pmTrayIcon->TrackButton = tbRightButton;
	pmTrayIcon->Popup( pt.x, pt.y );
	MoveToTop( Handle );
//	MoveToTop( pmTrayIcon->WindowHandle );
}
void TDCHookMainForm::MenuClosed()
{
	Visible = false;
	ShowWindow( Application->Handle, SW_HIDE );
	tmMenuClose->Enabled = true;
}
void TDCHookMainForm::EvToggle( TMessage &msg )
{
	miClickOnlyClick( this );
	ShowNotify();
}
void TDCHookMainForm::EvShortcut( TMessage &msg )
{
	switch (msg.WParam){
		case SCINX_NOTIFY64:
			DBW("SCINX_NOTIFY64:%08X %08X", msg.LParam, hDll);
			if (hDll){
				hDll->Notify64(msg.LParam);
				SetupConfig();
			}
			break;
	}
}
void TDCHookMainForm::EvGetVersion( TMessage &msg )
{
	msg.Result = GetVersionValue();
}
//---------------------------------------------------------------------------
// 右クリック検索ON/OFF表示
void TDCHookMainForm::ShowNotify()
{
	POINT pt;
	GetCursorPos( &pt );
	if ( !NotifyForm ){
		NotifyForm = new TNotifyForm( this );
	} else {
		NotifyForm->Visible = false;
	}
	NotifyForm->enable = miClickOnly->Checked;
	NotifyForm->Left = pt.x + 8;
	NotifyForm->Top = pt.y + 8;
	NotifyForm->Setup();
	NotifyForm->Show();
	SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
	SetWindowPos( NotifyForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
	tmNotify->Enabled = false;
	tmNotify->Enabled = true;
}
void TDCHookMainForm::NotifyDNF()
{
	MessageBox(Handle, ".NET Framework 4.7.2以上が必要です", "DokoPop!エラー", MB_OK|MB_ICONEXCLAMATION);
	if (ATSOCRRunable()){
		tmDNFInstallCheck->Enabled = true;
	}
}
void TDCHookMainForm::Reboot()
{
	::Reboot = true;
	Close();
}
void GetGroupList(TMyIni &ini, TStringList &list)
{
	AnsiString s = ini.ReadString("Group", "::GroupNames::", "");
	if (s.IsEmpty()){
		// for old version (not work correctly)
		ini.ReadValues( "Group", &list);
	} else {
		const TCHAR *p = _tcstok(s.c_str(), _T("\t"));
		while (p){
			list.Add( p );
			p = _tcstok(NULL, _T("\t"));
		}
	}
}

