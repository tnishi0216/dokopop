//---------------------------------------------------------------------------
#ifndef PopupConfigH
#define PopupConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

enum eCaptureMode {
	CM_TEXT = 1,	// 文字抽出
	CM_IMAGE = 2,	// OCR
};

class TPopupConfigDlg : public TForm
{
__published:	// IDE 管理のコンポーネント
	TCheckBox *cbGroupOpen;
	TLabel *lb1;
	TLabel *lbGroupName;
	TCheckBox *cbCtrlClose;
	TButton *btnOK;
	TButton *btnCancel;
	TLabel *lb2;
	TComboBox *edGroupName;
	TCheckBox *cbBanner;
	TMemo *Memo1;
	TLabel *Label1;
	TComboBox *cbPopupKey;
	TLabel *Label2;
	TComboBox *cbToggleKey;
	TCheckBox *cbIgnoreJ;
	TLabel *lbAMODIPath;
	TEdit *edAMODIPath;
	TButton *btnBrowse;
	TLabel *lbScaling;
	TTrackBar *tbScaling;
	TLabel *lbScaleNormal;
	TLabel *lbScaleMax;
	TButton *btnHelp;
	TActionList *ActionList1;
	TAction *actHelp;
	TLabel *lbDebug;
	TPopupMenu *pmDebugMenu;
	TMenuItem *miShowAMODI;
	TComboBox *cbCaptureMode;
	TCheckBox *cbUse64;
	TCheckBox *cbDPISetting;
	void __fastcall cbGroupOpenClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall btnHelpClick(TObject *Sender);
	void __fastcall actHelpExecute(TObject *Sender);
	void __fastcall lbDebugDblClick(TObject *Sender);
	void __fastcall miShowAMODIClick(TObject *Sender);
	void __fastcall cbCaptureModeChange(TObject *Sender);
	void __fastcall cbDPISettingClick(TObject *Sender);
private:	// ユーザー宣言
	bool MODIAvail;
public:		// ユーザー宣言
	__fastcall TPopupConfigDlg(TComponent* Owner);
	void SetPopupKey( int keycomb );
	int GetPopupKey( );
	void SetToggleKey( int keycomb );
	int GetToggleKey( );
	void SetDPIDetect(bool dpi_detect);
	bool GetDPIDetect();
	void SetScaler(int scale);
	int GetScaler();
	void SetMODIAvail(bool avail) { MODIAvail = avail; }
	void SetCaptureMode(int mode);
	int GetCaptureMode();
};
//---------------------------------------------------------------------------
extern PACKAGE TPopupConfigDlg *PopupConfigDlg;
//---------------------------------------------------------------------------
#endif

