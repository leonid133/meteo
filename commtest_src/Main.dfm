object Form1: TForm1
  Left = 483
  Top = 350
  Width = 512
  Height = 388
  Caption = 'CommTest by Dmitry Vassiliev - Freeware'
  Color = clBtnFace
  Constraints.MinHeight = 320
  Constraints.MinWidth = 512
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 233
    Width = 504
    Height = 90
    Align = alClient
    HideSelection = False
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object PanelTop: TPanel
    Left = 0
    Top = 0
    Width = 504
    Height = 145
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object GroupBox3: TGroupBox
      Left = 89
      Top = 0
      Width = 76
      Height = 145
      Align = alLeft
      Caption = 'Parity'
      TabOrder = 1
      object P_Even: TRadioButton
        Left = 8
        Top = 16
        Width = 64
        Height = 17
        Caption = 'Even'
        TabOrder = 0
        OnClick = P_EvenClick
      end
      object P_Mark: TRadioButton
        Left = 8
        Top = 36
        Width = 64
        Height = 17
        Caption = 'Mark'
        TabOrder = 1
        OnClick = P_MarkClick
      end
      object P_None: TRadioButton
        Left = 8
        Top = 56
        Width = 64
        Height = 17
        Caption = 'None'
        Checked = True
        TabOrder = 2
        TabStop = True
        OnClick = P_NoneClick
      end
      object P_Odd: TRadioButton
        Left = 8
        Top = 76
        Width = 64
        Height = 17
        Caption = 'Odd'
        TabOrder = 3
        OnClick = P_OddClick
      end
      object P_Space: TRadioButton
        Left = 8
        Top = 96
        Width = 64
        Height = 17
        Caption = 'Space'
        TabOrder = 4
        OnClick = P_SpaceClick
      end
    end
    object PanelFlags: TPanel
      Left = 321
      Top = 0
      Width = 183
      Height = 145
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 3
      object GroupBox6: TGroupBox
        Left = 0
        Top = 53
        Width = 183
        Height = 92
        Align = alClient
        Caption = 'Modem flags (read only)'
        TabOrder = 1
        object CTS: TCheckBox
          Left = 8
          Top = 16
          Width = 150
          Height = 17
          AllowGrayed = True
          Caption = 'CTS (ClearToSend)'
          TabOrder = 0
        end
        object DSR: TCheckBox
          Left = 8
          Top = 32
          Width = 150
          Height = 17
          AllowGrayed = True
          Caption = 'DSR (DataSetReady)'
          TabOrder = 1
        end
        object Ring: TCheckBox
          Left = 8
          Top = 48
          Width = 150
          Height = 17
          AllowGrayed = True
          Caption = 'Ring'
          TabOrder = 2
        end
        object DCD: TCheckBox
          Left = 8
          Top = 64
          Width = 150
          Height = 17
          AllowGrayed = True
          Caption = 'DCD (DataCarrierDetect)'
          TabOrder = 3
        end
      end
      object GroupBox7: TGroupBox
        Left = 0
        Top = 0
        Width = 183
        Height = 53
        Align = alTop
        Caption = 'Control flags'
        TabOrder = 0
        object DTR: TCheckBox
          Left = 8
          Top = 16
          Width = 150
          Height = 17
          Caption = 'DTR (DataTerminalReady)'
          Checked = True
          State = cbChecked
          TabOrder = 0
          OnClick = DTRClick
        end
        object RTS: TCheckBox
          Left = 8
          Top = 32
          Width = 150
          Height = 17
          Caption = 'RTS (RequestToSend)'
          Checked = True
          State = cbChecked
          TabOrder = 1
          OnClick = RTSClick
        end
      end
    end
    object PanelComSpeed: TPanel
      Left = 0
      Top = 0
      Width = 89
      Height = 145
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 0
      object Label3: TLabel
        Left = 8
        Top = 12
        Width = 24
        Height = 13
        Caption = 'COM'
      end
      object Bevel1: TBevel
        Left = 0
        Top = 88
        Width = 89
        Height = 57
        Align = alBottom
        Shape = bsTopLine
      end
      object Label4: TLabel
        Left = 64
        Top = 56
        Width = 18
        Height = 13
        Caption = '100'
      end
      object Label5: TLabel
        Left = 68
        Top = 68
        Width = 13
        Height = 13
        Caption = 'ms'
      end
      object Speed: TComboBox
        Left = 8
        Top = 31
        Width = 69
        Height = 21
        ItemHeight = 13
        TabOrder = 2
        OnChange = SpeedChange
        Items.Strings = (
          '110'
          '300'
          '600'
          '1200'
          '2400'
          '4800'
          '9600'
          '14400'
          '19200'
          '38400'
          '56000'
          '57600'
          '115200'
          '128000'
          '256000')
      end
      object ComOpen: TButton
        Left = 5
        Top = 96
        Width = 75
        Height = 21
        Caption = '&Open'
        TabOrder = 4
        OnClick = ComOpenClick
      end
      object FlushButton: TButton
        Left = 4
        Top = 120
        Width = 75
        Height = 21
        Caption = 'Flush'
        TabOrder = 5
        OnClick = FlushButtonClick
      end
      object ComEdit: TEdit
        Left = 36
        Top = 8
        Width = 24
        Height = 21
        TabOrder = 0
        Text = '1'
        OnChange = ComEditChange
      end
      object ComNumber: TUpDown
        Left = 60
        Top = 8
        Width = 15
        Height = 21
        Associate = ComEdit
        Min = 1
        Max = 32
        Position = 1
        TabOrder = 1
        Wrap = False
      end
      object TimeoutBar: TTrackBar
        Left = 4
        Top = 60
        Width = 61
        Height = 21
        Hint = 'Timeout 100 ms'
        LineSize = 25
        Max = 500
        Min = 50
        Orientation = trHorizontal
        ParentShowHint = False
        PageSize = 50
        Frequency = 50
        Position = 100
        SelEnd = 0
        SelStart = 0
        ShowHint = True
        TabOrder = 3
        ThumbLength = 8
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = TimeoutBarChange
      end
    end
    object PanelBits: TPanel
      Left = 165
      Top = 0
      Width = 156
      Height = 145
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 2
      object GroupBox2: TGroupBox
        Left = 0
        Top = 0
        Width = 81
        Height = 97
        Align = alLeft
        Caption = 'Data width'
        TabOrder = 0
        object Bits5: TRadioButton
          Left = 8
          Top = 16
          Width = 64
          Height = 17
          Caption = '5 bits'
          TabOrder = 0
          OnClick = Bits5Click
        end
        object Bits6: TRadioButton
          Left = 8
          Top = 36
          Width = 64
          Height = 17
          Caption = '6 bits'
          TabOrder = 1
          OnClick = Bits6Click
        end
        object Bits7: TRadioButton
          Left = 8
          Top = 56
          Width = 64
          Height = 17
          Caption = '7 bits'
          TabOrder = 2
          OnClick = Bits7Click
        end
        object Bits8: TRadioButton
          Left = 8
          Top = 76
          Width = 64
          Height = 17
          Caption = '8 bits'
          Checked = True
          TabOrder = 3
          TabStop = True
          OnClick = Bits8Click
        end
      end
      object GroupBox4: TGroupBox
        Left = 81
        Top = 0
        Width = 75
        Height = 97
        Align = alClient
        Caption = 'Stop bits'
        TabOrder = 1
        object Sop1: TRadioButton
          Left = 8
          Top = 16
          Width = 49
          Height = 17
          Caption = '1'
          Checked = True
          TabOrder = 0
          TabStop = True
          OnClick = Sop1Click
        end
        object Stop15: TRadioButton
          Left = 8
          Top = 36
          Width = 53
          Height = 17
          Caption = '1.5'
          TabOrder = 1
          OnClick = Stop15Click
        end
        object Stop2: TRadioButton
          Left = 8
          Top = 56
          Width = 45
          Height = 17
          Caption = '2'
          TabOrder = 2
          OnClick = Stop2Click
        end
      end
      object GroupBox8: TGroupBox
        Left = 0
        Top = 97
        Width = 156
        Height = 48
        Align = alBottom
        Caption = 'Control'
        TabOrder = 2
        object ControlMode: TComboBox
          Left = 8
          Top = 16
          Width = 141
          Height = 21
          ItemHeight = 13
          TabOrder = 0
          Text = 'None'
          OnChange = ControlModeChange
          Items.Strings = (
            'None'
            'Soft (XON/XOFF)'
            'Hard (RTS/CTS)')
        end
      end
    end
  end
  object PanelBottom: TPanel
    Left = 0
    Top = 145
    Width = 504
    Height = 64
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object GroupBox5: TGroupBox
      Left = 425
      Top = 0
      Width = 79
      Height = 64
      Align = alClient
      Caption = 'Port Buffer'
      TabOrder = 3
      object PortIn: TLabel
        Left = 8
        Top = 20
        Width = 36
        Height = 13
        Caption = 'Input: 0'
      end
      object PortOut: TLabel
        Left = 8
        Top = 40
        Width = 44
        Height = 13
        Caption = 'Output: 0'
      end
    end
    object GroupBox12: TGroupBox
      Left = 0
      Top = 0
      Width = 89
      Height = 64
      Align = alLeft
      Caption = 'Input'
      TabOrder = 0
      object GetButton: TButton
        Left = 5
        Top = 16
        Width = 75
        Height = 21
        Caption = '&Get'
        Enabled = False
        TabOrder = 0
        OnClick = GetButtonClick
      end
      object AutoShow: TCheckBox
        Left = 8
        Top = 42
        Width = 77
        Height = 17
        Caption = '&Auto'
        Checked = True
        State = cbChecked
        TabOrder = 1
        OnClick = AutoShowClick
      end
    end
    object GroupBox9: TGroupBox
      Left = 89
      Top = 0
      Width = 232
      Height = 64
      Align = alLeft
      Caption = 'Output'
      TabOrder = 1
      object Label2: TLabel
        Left = 132
        Top = 44
        Width = 35
        Height = 13
        Caption = 'Repeat'
      end
      object Label1: TLabel
        Left = 84
        Top = 20
        Width = 22
        Height = 13
        Caption = 'Hex:'
      end
      object PutButton: TButton
        Left = 6
        Top = 16
        Width = 75
        Height = 21
        Caption = '&Put'
        Enabled = False
        TabOrder = 0
        OnClick = PutButtonClick
      end
      object AutoRepeat: TCheckBox
        Left = 8
        Top = 42
        Width = 85
        Height = 17
        Caption = 'Continuously'
        TabOrder = 1
      end
      object OutputEdit: TEdit
        Left = 110
        Top = 16
        Width = 113
        Height = 21
        CharCase = ecUpperCase
        TabOrder = 2
        Text = '0'
      end
      object OutCount: TUpDown
        Left = 209
        Top = 40
        Width = 15
        Height = 21
        Associate = OutCountEdit
        Min = 1
        Max = 1024
        Position = 1
        TabOrder = 3
        Wrap = False
      end
      object OutCountEdit: TEdit
        Left = 172
        Top = 40
        Width = 37
        Height = 21
        TabOrder = 4
        Text = '1'
      end
    end
    object GroupBox1: TGroupBox
      Left = 321
      Top = 0
      Width = 104
      Height = 64
      Align = alLeft
      Caption = 'Main Buffer'
      TabOrder = 2
      object MainIn: TLabel
        Left = 8
        Top = 20
        Width = 36
        Height = 13
        Caption = 'Input: 0'
      end
      object MainOut: TLabel
        Left = 8
        Top = 40
        Width = 44
        Height = 13
        Caption = 'Output: 0'
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 323
    Width = 504
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object Panel1: TPanel
    Left = 0
    Top = 209
    Width = 504
    Height = 24
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 4
    object ButtonSend: TButton
      Left = 324
      Top = 2
      Width = 101
      Height = 21
      Caption = 'Send'
      TabOrder = 0
      OnClick = ButtonSendClick
    end
    object EditSend: TComboBox
      Left = 2
      Top = 2
      Width = 319
      Height = 21
      DropDownCount = 32
      ItemHeight = 13
      TabOrder = 1
      OnKeyPress = EditSendKeyPress
      Items.Strings = (
        'ATZ'
        'AT&F1L0M1'
        'AT&T1'
        'ATS0=1'
        'ATDT'
        'ATDP'
        'ATH0'
        'ATH1')
    end
    object ClearButton: TButton
      Left = 425
      Top = 2
      Width = 80
      Height = 21
      Caption = 'C&lear'
      TabOrder = 2
      OnClick = ClearButtonClick
    end
  end
  object CommPort1: TCommPort
    ComNumber = 1
    DTR = True
    RTS = True
    InSize = 4096
    OutSize = 8192
    Baud = 9600
    BaudRate = cbr9600
    DataBits = 8
    MonitorEvents = [evRxChar, evTxEmpty, evCTS, evDSR, evRlsd, evRing]
    XoffLim = 8
    XonChar = #17
    XoffChar = #19
    OnCTSSignal = CommPort1CTSSignal
    OnDCDSignal = CommPort1DCDSignal
    OnDSRSignal = CommPort1DSRSignal
    OnTxDSignal = CommPort1TxDSignal
    OnRingSignal = CommPort1RingSignal
    OnDataReceived = CommPort1DataReceived
    OnOpen = CommPort1Open
    OnClose = CommPort1Close
    Left = 32
    Top = 268
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 100
    Top = 268
  end
  object MainMenu1: TMainMenu
    Left = 172
    Top = 268
    object File1: TMenuItem
      Caption = '&File'
      object IrmanInit1: TMenuItem
        Caption = '&UIR/Irman IR/OK Init'
        Enabled = False
        OnClick = IrmanInit1Click
      end
      object SendBreak1: TMenuItem
        Caption = 'Send &Break'
        Enabled = False
        OnClick = SendBreak1Click
      end
      object Flushbuffers1: TMenuItem
        Caption = 'Flush buffers'
        OnClick = FlushButtonClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Exit1: TMenuItem
        Caption = 'E&xit'
        OnClick = Exit1Click
      end
    end
    object Edit1: TMenuItem
      Caption = '&Edit'
      object Copy1: TMenuItem
        Caption = '&Copy'
        OnClick = Copy1Click
      end
      object SelectAll1: TMenuItem
        Caption = 'Se&lect All'
        OnClick = SelectAll1Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object Clear1: TMenuItem
        Caption = 'Clea&r'
        OnClick = ClearButtonClick
      end
    end
    object Display1: TMenuItem
      Caption = '&Display'
      object ASCII: TMenuItem
        AutoCheck = True
        Caption = 'A&SCII'
        Checked = True
      end
      object Digits: TMenuItem
        AutoCheck = True
        Caption = 'Di&gits'
        Checked = True
        OnClick = DigitsClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object HEX: TMenuItem
        Caption = 'HE&X'
        Checked = True
        RadioItem = True
        OnClick = HEXClick
      end
      object DEC: TMenuItem
        Caption = 'DE&C'
        RadioItem = True
        OnClick = DECClick
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object ConvOem: TMenuItem
        AutoCheck = True
        Caption = 'OEM Convert'
        Checked = True
      end
      object ReverseBits: TMenuItem
        AutoCheck = True
        Caption = '&Reverse bits'
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object Font1: TMenuItem
        Caption = '&Font...'
        OnClick = Font1Click
      end
    end
    object Help1: TMenuItem
      Caption = '&Help'
      object About1: TMenuItem
        Caption = '&About...'
        OnClick = About1Click
      end
    end
  end
  object FormStorage1: TFormStorage
    IniFileName = 'CommTest.ini'
    IniSection = '.\'
    MinMaxInfo.MinTrackHeight = 320
    MinMaxInfo.MinTrackWidth = 512
    StoredProps.Strings = (
      'ASCII.Checked'
      'Digits.Checked'
      'HEX.Checked'
      'DEC.Checked'
      'AutoRepeat.Checked'
      'AutoShow.Checked'
      'Bits5.Checked'
      'Bits6.Checked'
      'Bits7.Checked'
      'Bits8.Checked'
      'ControlMode.Text'
      'DTR.Checked'
      'RTS.Checked'
      'P_Even.Checked'
      'P_Mark.Checked'
      'P_None.Checked'
      'P_Odd.Checked'
      'P_Space.Checked'
      'ReverseBits.Checked'
      'Sop1.Checked'
      'Stop15.Checked'
      'Stop2.Checked'
      'Memo1.Font'
      'ConvOem.Checked'
      'ComNumber.Position'
      'Speed.ItemIndex')
    StoredValues = <>
    Left = 252
    Top = 268
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    MinFontSize = 0
    MaxFontSize = 0
    Left = 328
    Top = 268
  end
end
