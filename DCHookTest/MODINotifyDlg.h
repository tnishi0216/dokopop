//---------------------------------------------------------------------------

#ifndef MODINotifyDlgH
#define MODINotifyDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMODINotifyDialog : public TForm
{
__published:	// IDE 管理のコンポーネント
	TRichEdit *reMsg;
	TButton *btnClose;
	TTimer *tmInstallCheck;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall tmInstallCheckTimer(TObject *Sender);
private:	// ユーザー宣言
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_NOTIFY, TMessage, EvNotify);
	END_MESSAGE_MAP( TForm )
	void EvNotify( TMessage &msg );
	int reMsgId;
	bool modiInstalled;
	bool amodiRunable;
public:		// ユーザー宣言
	__fastcall TMODINotifyDialog(TComponent* Owner);
	void SetMessage();
	bool LinkClicked;
};
//---------------------------------------------------------------------------
extern PACKAGE TMODINotifyDialog *MODINotifyDialog;
//---------------------------------------------------------------------------
#endif
