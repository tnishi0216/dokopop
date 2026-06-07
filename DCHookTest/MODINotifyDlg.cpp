//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MODINotifyDlg.h"
#include "Util.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

// 参考：https://msdn.microsoft.com/ja-jp/library/5a4x27ek(v=vs.110).aspx
#define	URL_DNF_INSTALL		"https://www.microsoft.com/ja-JP/download/details.aspx?id=40773"
#define	URL_MODI_INSTALL	"http://support.microsoft.com/kb/982760"

TMODINotifyDialog *MODINotifyDialog;
//---------------------------------------------------------------------------
__fastcall TMODINotifyDialog::TMODINotifyDialog(TComponent* Owner)
	: TForm(Owner)
{
	LinkClicked = false;
	modiInstalled = false;
	amodiRunable = false;
}
//---------------------------------------------------------------------------
void __fastcall TMODINotifyDialog::FormCreate(TObject *Sender)
{
	MoveCenter(Handle, NULL);
	reMsgId = GetDlgCtrlID(reMsg->Handle);
	//SetWindowLong(Handle, GWL_USERDATA, (LONG)reMsg->Handle);
	SendMessage(reMsg->Handle, EM_AUTOURLDETECT, (WPARAM)TRUE, 0 );	// RichEdit 2.0 only
	DWORD emask = SendMessage(reMsg->Handle, EM_GETEVENTMASK, 0, 0);
	SendMessage(reMsg->Handle, EM_SETEVENTMASK, 0, emask | ENM_LINK);

	modiInstalled = MODIInstalled();
	amodiRunable = ATSOCRRunable();

	SetMessage();	
}
//---------------------------------------------------------------------------
void TMODINotifyDialog::SetMessage()
{
	if (modiInstalled && !amodiRunable){
		reMsg->Lines->Add(
"【重要】 文字認識を使用するには .NET Framework 4.5.1 以上が必要です\n"
"\n"
URL_DNF_INSTALL"\n"
"\n"
"リンク先からインストールを行ってください\n"
"\n"
"※リンク先が見つからない場合は\".NET Framework インストール\"で検索してください。\n"
"※文字認識を使用するとポップアップ検索ができるアプリが増えます。\n"
		);
	} else {
		reMsg->Lines->Add(
"【重要】 MODIがインストールされていません！！\n"
"\n"
"MODIがインストールされていると、ポップアップ検索ができるアプリが増えます。\n"
"\n"
URL_MODI_INSTALL"\n"
"\n"
"リンク先にあるいずれかの方法でインストールを行ってください。\n"
"※通常は「方法２」\n"
"※MODIは\"Microsoft Office Document Imaging\"の略です\n"
"※上記リンクが開けない場合は \"MODI\" \"インストール\" などで検索してみてください\n"
		);
	}
}
void __fastcall TMODINotifyDialog::tmInstallCheckTimer(TObject *Sender)
{
	bool _modiInstalled = MODIInstalled();
	bool _amodiRunable = ATSOCRRunable();
	if (_modiInstalled && _amodiRunable){
		// ready to go
		Close();
	} else {
		if (modiInstalled!=_modiInstalled || amodiRunable!=_amodiRunable){
			modiInstalled = _modiInstalled;
			amodiRunable = _amodiRunable;
			SetMessage();
		}
	}
}
//---------------------------------------------------------------------------
void TMODINotifyDialog::EvNotify( TMessage &msg )
{
	if (msg.WParam==reMsgId){
		NMHDR *nmhdr = (NMHDR*)msg.LParam;
		if (nmhdr->code==EN_LINK){
			ENLINK *enlink = (ENLINK*)nmhdr;
			if (enlink->msg == WM_LBUTTONDOWN){
				bool dnf = modiInstalled && !amodiRunable;
				HINSTANCE hInst = ShellExecute(Handle, NULL, dnf ? URL_DNF_INSTALL : URL_MODI_INSTALL, NULL, NULL, SW_SHOWNORMAL);
				if ((int)hInst>32){
					LinkClicked = true;
				}
			}
		}
	}
}


