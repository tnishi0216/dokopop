object MODINotifyDialog: TMODINotifyDialog
  Left = 199
  Top = 120
  BorderStyle = bsDialog
  Caption = 'DokoPop!'
  ClientHeight = 263
  ClientWidth = 486
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = '‚l‚r ‚oƒSƒVƒbƒN'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object reMsg: TRichEdit
    Left = 14
    Top = 12
    Width = 458
    Height = 213
    TabStop = False
    Anchors = [akLeft, akTop, akRight, akBottom]
    ReadOnly = True
    TabOrder = 0
  end
  object btnClose: TButton
    Left = 206
    Top = 233
    Width = 75
    Height = 25
    Anchors = [akBottom]
    Cancel = True
    Caption = 'Close'
    ModalResult = 2
    TabOrder = 1
  end
  object tmInstallCheck: TTimer
    OnTimer = tmInstallCheckTimer
    Left = 328
    Top = 232
  end
end
