//---------------------------------------------------------------------------

#ifndef OCRTextFrmH
#define OCRTextFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TOCRTextForm : public TForm
{
__published:	// IDE 管理のコンポーネント
	TMemo *memo;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// ユーザー宣言
public:		// ユーザー宣言
	__fastcall TOCRTextForm(TComponent* Owner);
	void SetText(const wchar_t *text);
};
//---------------------------------------------------------------------------
extern PACKAGE TOCRTextForm *OCRTextForm;
//---------------------------------------------------------------------------
#endif

