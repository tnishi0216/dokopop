//---------------------------------------------------------------------------

#ifndef NotifyH
#define NotifyH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TNotifyForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TImage *EnableBmp;
	TImage *DisableBmp;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall FormDestroy(TObject *Sender);
private:	// ユーザー宣言
	HRGN hRgn;
public:		// ユーザー宣言
	bool enable;
	__fastcall TNotifyForm(TComponent* Owner);
	void Setup();
};
//---------------------------------------------------------------------------
extern PACKAGE TNotifyForm *NotifyForm;
//---------------------------------------------------------------------------
#endif

