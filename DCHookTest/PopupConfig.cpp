//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "PopupConfig.h"
#include "Util.h"
#include "prgprof.h"
#include "../DCHook/DCHook.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPopupConfigDlg *PopupConfigDlg;
//---------------------------------------------------------------------------
__fastcall TPopupConfigDlg::TPopupConfigDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::cbGroupOpenClick(TObject *Sender)
{
	bool f = cbGroupOpen->Checked;
	edGroupName->Enabled = f;
	lbGroupName->Enabled = f;
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::FormShow(TObject *Sender)
{
#if USE_DDE
	cbGroupOpenClick( this );

	// Add groups to combo-box //
	TAppIni ini;
	if ( !ini.hroot ){
		return;
	}

	ini.ReadValues( PFS_GROUP, edGroupName->Items );

	if (!cbGroupOpen->Checked ){
		int sel = ini.ReadInteger( PFS_COMMON, PFS_GRPSEL, -1 );
		edGroupName->ItemIndex = sel;
	}
#else	// !USE_DDE
	// WM_COPYDATAの場合は、辞書グループ指定はできない
	cbGroupOpen->Enabled = false;
	lbGroupName->Enabled = false;
	edGroupName->Enabled = false;
	lb1->Enabled = false;
	lb2->Enabled = false;
#endif
	if (fWow64)
		cbUse64->Visible = true;

	cbAdvancedClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::FormCloseQuery(TObject *Sender,
	  bool &CanClose)
{
	if (ModalResult!=mrOk) return;

	if (cbToggleKey->ItemIndex != 0){
		if (cbToggleKey->ItemIndex-1 == cbPopupKey->ItemIndex){
			// 同じキーは割り当てられない
			Application->MessageBox("ポップアップ検索と右クリック検索ON/OFFは同じキーを割り当てられません",Caption.c_str(),MB_OK);
			CanClose = false;
			return;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::cbAdvancedClick(TObject *Sender)
{
	bool enabled = cbAdvanced->Checked;
	grpAdvanced->Enabled = enabled;
	cbCaptureMode->Enabled = enabled;
	cbUse64->Enabled = enabled;
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::cbCaptureModeChange(TObject *Sender)
{
#if 0
	bool f = cbCaptureMode->ItemIndex != 2;
	lbScaling->Enabled = f;
	lbScaleNormal->Enabled = f;
	tbScaling->Enabled = f;
	lbScaleMax->Enabled = f;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::btnHelpClick(TObject *Sender)
{
	ShowManual(Handle);
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::actHelpExecute(TObject *Sender)
{
	ShowManual(Handle);
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::lbDebugDblClick(TObject *Sender)
{
	miShowATSOCR->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::miShowATSOCRClick(TObject *Sender)
{
	ShowATSOCR();
}
//---------------------------------------------------------------------------
void __fastcall TPopupConfigDlg::cbDPISettingClick(TObject *Sender)
{
	bool setting = cbDPISetting->Checked;
	lbScaling->Enabled = setting;
	lbScaleNormal->Enabled = setting;
	tbScaling->Enabled = setting;
}
//---------------------------------------------------------------------------
/*
Ctrl+右クリック
Shift+右クリック
Alt+右クリック
Ctrl+Shift+右クリック
Ctrl+Alt+右クリック
Shift+Alt+右クリック
Ctrl+左クリック
Shift+左クリック
Alt+左クリック
Ctrl+Shift+左クリック
Ctrl+Alt+左クリック
Shift+Alt+左クリック
左クリック+右クリック→却下
*/
int KeyCombTable[] = {
	KF_CONTROL,
	KF_SHIFT,
	KF_MENU,
	KF_CONTROL + KF_SHIFT,
	KF_CONTROL + KF_MENU,
	KF_SHIFT   + KF_MENU,
	KF_CONTROL + KF_LEFTCLICK,
	KF_SHIFT   + KF_LEFTCLICK,
	KF_MENU    + KF_LEFTCLICK,
	KF_CONTROL + KF_SHIFT + KF_LEFTCLICK,
	KF_CONTROL + KF_MENU + KF_LEFTCLICK,
	KF_SHIFT   + KF_MENU + KF_LEFTCLICK,
	KF_MIDCLICK,
	KF_CONTROL + KF_MIDCLICK,
	KF_SHIFT   + KF_MIDCLICK,
	KF_MENU    + KF_MIDCLICK,
	KF_CONTROL + KF_SHIFT + KF_MIDCLICK,
	KF_CONTROL + KF_MENU + KF_MIDCLICK,
	KF_SHIFT   + KF_MENU + KF_MIDCLICK,
	//KF_LRCLICK,
	0
};
// key combination -> index
int KeyCombToIndex( int keycomb )
{
	int i;
	for (i=0;KeyCombTable[i];i++){
		if (keycomb == KeyCombTable[i])
			return i;
	}
	return -1;
}
int IndexToKeyComb( int index )
{
	if (index < 0)
		return 0;
	return KeyCombTable[index];
}
void TPopupConfigDlg::SetPopupKey( int keycomb )
{
	cbPopupKey->ItemIndex = KeyCombToIndex( keycomb );
}
int TPopupConfigDlg::GetPopupKey( )
{
	return IndexToKeyComb( cbPopupKey->ItemIndex );
}
void TPopupConfigDlg::SetToggleKey( int keycomb )
{
	cbToggleKey->ItemIndex = KeyCombToIndex( keycomb )+1;
}
int TPopupConfigDlg::GetToggleKey( )
{
	return IndexToKeyComb( cbToggleKey->ItemIndex-1 );
}

void TPopupConfigDlg::SetDPIDetect(bool dpi_detect)
{
	cbDPISetting->Checked = !dpi_detect;
}
bool TPopupConfigDlg::GetDPIDetect()
{
	return !cbDPISetting->Checked;
}
void TPopupConfigDlg::SetScaler(int scale)
{
	if (scale<120){
		tbScaling->Position = 0;
	} else
	if (scale<144){
		tbScaling->Position = 1;
	} else {
		tbScaling->Position = 2;
	}
}
int TPopupConfigDlg::GetScaler()
{
	switch (tbScaling->Position){
		default: return 0;
		case 1: return 120;
		case 2: return 144;
	}
}

void TPopupConfigDlg::SetCaptureMode(int mode)
{
	switch (mode){
		case CM_TEXT | CM_IMAGE: cbCaptureMode->ItemIndex = 0; break;
		case CM_IMAGE:			 cbCaptureMode->ItemIndex = 1; break;
		case CM_TEXT:			 cbCaptureMode->ItemIndex = 2; break;
	}
}
int TPopupConfigDlg::GetCaptureMode()
{
	switch (cbCaptureMode->ItemIndex){
		default: case 0:	return CM_TEXT | CM_IMAGE;
		case 1: return CM_IMAGE;
		case 2: return CM_TEXT;
	}
}

