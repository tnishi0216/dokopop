object PopupConfigDlg: TPopupConfigDlg
  Left = 414
  Top = 142
  BorderStyle = bsDialog
  Caption = 'DokoPop!の設定'
  ClientHeight = 419
  ClientWidth = 306
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = pmDebugMenu
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object lb1: TLabel
    Left = 40
    Top = 54
    Width = 240
    Height = 12
    Caption = '指定しない場合は、PDIC本体が現在開いている'
  end
  object lb2: TLabel
    Left = 40
    Top = 70
    Width = 136
    Height = 12
    Caption = '辞書グループを使用します'
  end
  object lbGroupName: TLabel
    Left = 40
    Top = 91
    Width = 87
    Height = 12
    Caption = '辞書ｸﾞﾙｰﾌﾟ名(&N)'
  end
  object Label1: TLabel
    Left = 24
    Top = 120
    Width = 107
    Height = 12
    Caption = 'ポップアップ検索(&P)：'
    FocusControl = cbPopupKey
  end
  object Label2: TLabel
    Left = 24
    Top = 144
    Width = 122
    Height = 12
    Caption = '右ｸﾘｯｸ検索ON/OFF(&R):'
    FocusControl = cbToggleKey
  end
  object lbScaling: TLabel
    Left = 40
    Top = 386
    Width = 53
    Height = 12
    Caption = '拡大率(&M)'
    Enabled = False
    FocusControl = tbScaling
    Visible = False
  end
  object lbScaleNormal: TLabel
    Left = 117
    Top = 387
    Width = 24
    Height = 12
    Caption = '標準'
    Enabled = False
    Visible = False
  end
  object lbScaleMax: TLabel
    Left = 319
    Top = 366
    Width = 24
    Height = 12
    Caption = '最大'
    Visible = False
  end
  object lbDebug: TLabel
    Left = 264
    Top = 8
    Width = 20
    Height = 12
    Caption = '     '
    OnDblClick = lbDebugDblClick
  end
  object grpAdvanced: TGroupBox
    Left = 16
    Top = 278
    Width = 257
    Height = 81
    TabOrder = 9
    object cbCaptureMode: TComboBox
      Left = 24
      Top = 24
      Width = 193
      Height = 20
      Style = csDropDownList
      ItemHeight = 12
      TabOrder = 0
      OnChange = cbCaptureModeChange
      Items.Strings = (
        '文字認識＋文字抽出'
        '文字認識のみ'
        '文字抽出のみ')
    end
    object cbUse64: TCheckBox
      Left = 24
      Top = 51
      Width = 193
      Height = 17
      Caption = '&64ビットフックを使用する'
      TabOrder = 1
      Visible = False
    end
    object cbDPISetting: TCheckBox
      Left = 24
      Top = 70
      Width = 193
      Height = 17
      Caption = '解像度設定をする(&D)'
      TabOrder = 2
      Visible = False
      OnClick = cbDPISettingClick
    end
  end
  object cbGroupOpen: TCheckBox
    Left = 24
    Top = 32
    Width = 249
    Height = 17
    Caption = '辞書グループを指定する(&G)'
    TabOrder = 1
    OnClick = cbGroupOpenClick
  end
  object cbCtrlClose: TCheckBox
    Left = 24
    Top = 218
    Width = 233
    Height = 17
    Caption = 'Ctrlキーを離したらﾎﾟｯﾌﾟｱｯﾌﾟを閉じる(&C)'
    TabOrder = 6
    Visible = False
  end
  object btnOK: TButton
    Left = 12
    Top = 379
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 11
  end
  object btnCancel: TButton
    Left = 112
    Top = 379
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 12
  end
  object edGroupName: TComboBox
    Left = 136
    Top = 88
    Width = 145
    Height = 20
    ItemHeight = 12
    TabOrder = 2
  end
  object cbBanner: TCheckBox
    Left = 24
    Top = 8
    Width = 225
    Height = 17
    Caption = '起動時にスプラッシュを表示する(&B)'
    Checked = True
    State = cbChecked
    TabOrder = 0
  end
  object Memo1: TMemo
    Left = 32
    Top = 168
    Width = 257
    Height = 49
    TabStop = False
    Alignment = taCenter
    BorderStyle = bsNone
    Color = clBtnFace
    Lines.Strings = (
      '※"右ｸﾘｯｸ検索"とは、マウスの右クリックだけで'
      'ポップアップ検索を行う機能です'
      '※組み合わせによってはアプリケーションと'
      '競合してしまう場合があります。')
    ReadOnly = True
    TabOrder = 5
  end
  object cbPopupKey: TComboBox
    Left = 152
    Top = 115
    Width = 130
    Height = 20
    Style = csDropDownList
    ItemHeight = 12
    TabOrder = 3
    Items.Strings = (
      'Ctrl+右クリック'
      'Shift+右クリック'
      'Alt+右クリック'
      'Ctrl+Shift+右クリック'
      'Ctrl+Alt+右クリック'
      'Shift+Alt+右クリック'
      'Ctrl+左クリック'
      'Shift+左クリック'
      'Alt+左クリック'
      'Ctrl+Shift+左クリック'
      'Ctrl+Alt+左クリック'
      'Shift+Alt+左クリック'
      '中クリック'
      'Ctrl+中クリック'
      'Shift+中クリック'
      'Alt+中クリック'
      'Ctrl+Shift+中クリック'
      'Ctrl+Alt+中クリック'
      'Shift+Alt+中クリック')
  end
  object cbToggleKey: TComboBox
    Left = 152
    Top = 140
    Width = 129
    Height = 20
    Style = csDropDownList
    ItemHeight = 12
    TabOrder = 4
    Items.Strings = (
      'なし'
      'Ctrl+右クリック'
      'Shift+右クリック'
      'Alt+右クリック'
      'Ctrl+Shift+右クリック'
      'Ctrl+Alt+右クリック'
      'Shift+Alt+右クリック'
      'Ctrl+左クリック'
      'Shift+左クリック'
      'Alt+左クリック'
      'Ctrl+Shift+左クリック'
      'Ctrl+Alt+左クリック'
      'Shift+Alt+左クリック')
  end
  object cbIgnoreJ: TCheckBox
    Left = 24
    Top = 244
    Width = 225
    Height = 17
    Caption = '英数字のみ検索する(&J)'
    TabOrder = 7
  end
  object tbScaling: TTrackBar
    Left = 146
    Top = 383
    Width = 121
    Height = 20
    Enabled = False
    Max = 2
    Orientation = trHorizontal
    PageSize = 1
    Frequency = 1
    Position = 0
    SelEnd = 0
    SelStart = 0
    TabOrder = 10
    ThumbLength = 8
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    Visible = False
  end
  object btnHelp: TButton
    Left = 212
    Top = 379
    Width = 75
    Height = 25
    Caption = '&Help'
    TabOrder = 13
    OnClick = btnHelpClick
  end
  object cbAdvanced: TCheckBox
    Left = 24
    Top = 275
    Width = 97
    Height = 17
    Caption = '詳細設定(&A)'
    TabOrder = 8
    OnClick = cbAdvancedClick
  end
  object ActionList1: TActionList
    Left = 272
    Top = 272
    object actHelp: TAction
      Caption = 'Help'
      ShortCut = 112
      OnExecute = actHelpExecute
    end
  end
  object pmDebugMenu: TPopupMenu
    Left = 264
    Top = 216
    object miShowATSOCR: TMenuItem
      Caption = 'Show &ATSOCR'
      Visible = False
      OnClick = miShowATSOCRClick
    end
  end
end
