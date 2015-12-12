//---------------------------------------------------------------------------
// Copyright (C) 2000 by Dmitry Vassiliev
// slydiman@mailru.com
//---------------------------------------------------------------------------
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <Dialogs.hpp>

#include "CommPort.h"
#include "Placemnt.hpp"
#include "RXSpin.hpp"
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TCommPort *CommPort1;
        TMemo *Memo1;
        TPanel *PanelTop;
        TGroupBox *GroupBox3;
        TRadioButton *P_Even;
        TRadioButton *P_Mark;
        TRadioButton *P_None;
        TRadioButton *P_Odd;
        TRadioButton *P_Space;
        TPanel *PanelFlags;
        TGroupBox *GroupBox6;
        TCheckBox *CTS;
        TCheckBox *DSR;
        TCheckBox *Ring;
        TCheckBox *DCD;
        TGroupBox *GroupBox7;
        TCheckBox *DTR;
        TCheckBox *RTS;
        TPanel *PanelBottom;
        TTimer *Timer1;
        TStatusBar *StatusBar1;
        TMainMenu *MainMenu1;
        TMenuItem *File1;
        TMenuItem *IrmanInit1;
        TMenuItem *N1;
        TMenuItem *Exit1;
        TMenuItem *Edit1;
        TMenuItem *Help1;
        TMenuItem *About1;
        TMenuItem *Display1;
        TMenuItem *Digits;
        TMenuItem *HEX;
        TMenuItem *DEC;
        TMenuItem *ASCII;
        TMenuItem *Copy1;
        TMenuItem *N3;
        TMenuItem *Clear1;
        TMenuItem *SendBreak1;
        TMenuItem *N4;
        TMenuItem *ReverseBits;
        TPanel *PanelComSpeed;
        TPanel *PanelBits;
        TGroupBox *GroupBox2;
        TRadioButton *Bits5;
        TRadioButton *Bits6;
        TRadioButton *Bits7;
        TRadioButton *Bits8;
        TGroupBox *GroupBox4;
        TRadioButton *Sop1;
        TRadioButton *Stop15;
        TRadioButton *Stop2;
        TGroupBox *GroupBox8;
        TComboBox *ControlMode;
        TLabel *Label3;
        TComboBox *Speed;
        TBevel *Bevel1;
        TButton *ComOpen;
        TGroupBox *GroupBox5;
        TLabel *PortIn;
        TLabel *PortOut;
        TGroupBox *GroupBox12;
        TButton *GetButton;
        TCheckBox *AutoShow;
        TGroupBox *GroupBox9;
        TLabel *Label2;
        TButton *PutButton;
        TCheckBox *AutoRepeat;
        TGroupBox *GroupBox1;
        TLabel *MainIn;
        TLabel *MainOut;
        TFormStorage *FormStorage1;
        TMenuItem *N2;
        TMenuItem *SelectAll1;
        TPanel *Panel1;
        TButton *ButtonSend;
        TMenuItem *Font1;
        TFontDialog *FontDialog1;
        TComboBox *EditSend;
        TMenuItem *N5;
        TMenuItem *ConvOem;
        TLabel *Label1;
        TEdit *OutputEdit;
        TButton *ClearButton;
        TButton *FlushButton;
        TEdit *ComEdit;
        TUpDown *ComNumber;
        TUpDown *OutCount;
        TEdit *OutCountEdit;
        TTrackBar *TimeoutBar;
        TMenuItem *Flushbuffers1;
        TLabel *Label4;
        TLabel *Label5;
        void __fastcall ComOpenClick(TObject *Sender);
        void __fastcall CommPort1DataReceived(TObject *Sender,
          DWORD Count);
        void __fastcall Bits5Click(TObject *Sender);
        void __fastcall Bits6Click(TObject *Sender);
        void __fastcall Bits7Click(TObject *Sender);
        void __fastcall Bits8Click(TObject *Sender);
        void __fastcall P_EvenClick(TObject *Sender);
        void __fastcall P_MarkClick(TObject *Sender);
        void __fastcall P_NoneClick(TObject *Sender);
        void __fastcall P_OddClick(TObject *Sender);
        void __fastcall P_SpaceClick(TObject *Sender);
        void __fastcall Sop1Click(TObject *Sender);
        void __fastcall Stop15Click(TObject *Sender);
        void __fastcall Stop2Click(TObject *Sender);
        void __fastcall SpeedChange(TObject *Sender);
        void __fastcall CommPort1CTSSignal(TObject *Sender);
        void __fastcall CommPort1DSRSignal(TObject *Sender);
        void __fastcall CommPort1RingSignal(TObject *Sender);
        void __fastcall CommPort1DCDSignal(TObject *Sender);
        void __fastcall DTRClick(TObject *Sender);
        void __fastcall RTSClick(TObject *Sender);
        void __fastcall PutButtonClick(TObject *Sender);
        void __fastcall CommPort1TxDSignal(TObject *Sender);
        void __fastcall ControlModeChange(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall GetButtonClick(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall ClearButtonClick(TObject *Sender);
        void __fastcall Exit1Click(TObject *Sender);
        void __fastcall SendBreak1Click(TObject *Sender);
        void __fastcall IrmanInit1Click(TObject *Sender);
        void __fastcall AutoShowClick(TObject *Sender);
        void __fastcall CommPort1Open(TObject *Sender, int Error);
        void __fastcall CommPort1Close(TObject *Sender);
        void __fastcall DECClick(TObject *Sender);
        void __fastcall HEXClick(TObject *Sender);
        void __fastcall DigitsClick(TObject *Sender);
        void __fastcall About1Click(TObject *Sender);
        void __fastcall Copy1Click(TObject *Sender);
        void __fastcall SelectAll1Click(TObject *Sender);
        void __fastcall ButtonSendClick(TObject *Sender);
        void __fastcall Font1Click(TObject *Sender);
        void __fastcall EditSendKeyPress(TObject *Sender, char &Key);
        void __fastcall FlushButtonClick(TObject *Sender);
        void __fastcall TimeoutBarChange(TObject *Sender);
        void __fastcall ComEditChange(TObject *Sender);
private:	// User declarations
        bool Ready;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
