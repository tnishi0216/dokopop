//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OCRTextFrm.h"
#include "Util.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TntStdCtrls"
#pragma link "TntStdCtrls"
#pragma resource "*.dfm"
TOCRTextForm *OCRTextForm;
const char *sOCRText = "OCRText";
//---------------------------------------------------------------------------
__fastcall TOCRTextForm::TOCRTextForm(TComponent* Owner)
	: TForm(Owner)
{
	VCL_SIZEABLE();
}
//---------------------------------------------------------------------------
void __fastcall TOCRTextForm::FormShow(TObject *Sender)
{
	LoadForm(sOCRText, this, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TOCRTextForm::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
	SaveForm(sOCRText, this, NULL);
}
//---------------------------------------------------------------------------
void TOCRTextForm::SetText(const wchar_t *text)
{
	int wlen = wcslen(text);
	char *buf = new char[wlen*2+1];
	if (!buf) return;
	BOOL defchar = FALSE;
	WideCharToMultiByte( CP_ACP, 0,
		text, wlen,
		buf, wlen*2+1, 0, &defchar );
	memo->Text = buf;
	delete[] buf;
}


