//---------------------------------------------------------------------------
// Copyright (C) 2003 by Dmitry Vassiliev
// slydiman@mail.ru
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>

#include "Main.h"
#include "About.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CommPort"
#pragma link "Placemnt"
#pragma resource "*.dfm"
TForm1 *Form1;

/*
	BYTE b;
	DWORD start;
	bool Run=FALSE;
	char buffer[256];
	char left[128];
	char right[128];
    int bufc=0;
	int cmp;
	int devide;
	int modulus;

        int RepeatThreshhold=9;
	RepeatSensitivity=60;
	CompareSensitivity=90;

			if (!Run)
			{
				start=GetTickCount();
				Run=TRUE;
			}


			if (bufc>250)
				bufc=0;

			devide=b >> 4;
			modulus=b & 15;

			buffer[bufc]=  char( devide>9 ? devide + 55 : devide+ 48);
			bufc++;
			buffer[bufc]=  char( modulus>9 ? modulus + 55 : modulus+ 48);
			bufc++;
			buffer[bufc]=  char(0);


		    if (bufc > ( RepeatThreshhold << 1 )&& (bufc & 3)==0)
			{

				strncpy(left, buffer,128);
				left[( strlen(buffer)>> 1) ]=char(0);
				strncpy(right, buffer+ ( strlen(buffer) >> 1) , 128);
				cmp=match(left,right);
				// fprintf(f, "%s %s %d\n", left, right, cmp);
				if (cmp>RepeatSensitivity)
				{

					SendString(left);
					strncpy(buffer, right, 128);
					bufc=strlen(buffer);
				}
			}
*/

//--------------------------------------------------
int match(char *left, char *right)
{
  unsigned int i;
  unsigned int len;
  int per = 0;

  len = strlen(left) < strlen(right) ? strlen(left) : strlen(right);
  if( (len >> 1) == 0 ) return 0;

  for( i=0; i < len; i += 2 ){
    if( strncmp( left+i, right+i, 2) == 0 )  per++;
  }

  return (per * 200) / (len);
}

//--------------------------------------------------
__int64 HexToInt64( AnsiString hexdata )
{
  if( strnicmp( hexdata.c_str(), "0x", 2) )  hexdata = AnsiString("0x") + hexdata;
/*
  LONGLONG Ret = 0;
  StrToInt64Ex( hexdata.c_str(), STIF_SUPPORT_HEX, &Ret );
  return Ret;
*/
  return StrToInt64Def( hexdata, 0 );
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
  if( Speed->ItemIndex < 0 )  Speed->ItemIndex = 6;
  Ready = true;
  FormStorage1->IniFileName = ChangeFileExt(Application->ExeName,".ini");

//  EditSend->Items->CaseSensitive = false;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
  DTR->Checked = CommPort1->DTR;
  RTS->Checked = CommPort1->RTS;

  HEX->Enabled = Digits->Checked;
  DEC->Enabled = Digits->Checked;
  ReverseBits->Enabled = Digits->Checked;

  FontDialog1->Font->Assign( Memo1->Font );

  Timer1->Enabled = true;

  CommPort1Close(this);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CommPort1->Open = false;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ComEditChange(TObject *Sender)
{
  CommPort1->ComNumber = ComNumber->Position;
  CommPort1Close(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ComOpenClick(TObject *Sender)
{
  if( !Ready )  return;
  Ready = false;
  if( !CommPort1->Open ){
    CommPort1->ComNumber = ComNumber->Position;
    ControlModeChange(NULL);
    SpeedChange(NULL);
    DTRClick(NULL);
    RTSClick(NULL);
    if( P_Even->Checked  ) CommPort1->Parity = paEven;
    if( P_Mark->Checked  ) CommPort1->Parity = paMark;
    if( P_None->Checked  ) CommPort1->Parity = paNone;
    if( P_Odd->Checked   ) CommPort1->Parity = paOdd;
    if( P_Space->Checked ) CommPort1->Parity = paSpace;
    if( Bits5->Checked )  CommPort1->DataBits = 5;
    if( Bits6->Checked )  CommPort1->DataBits = 6;
    if( Bits7->Checked )  CommPort1->DataBits = 7;
    if( Bits8->Checked )  CommPort1->DataBits = 8;
    if( Sop1->Checked   ) CommPort1->StopBits = sb1_0;
    if( Stop15->Checked ) CommPort1->StopBits = sb1_5;
    if( Stop2->Checked  ) CommPort1->StopBits = sb2_0;
  }

  CommPort1->Open = !CommPort1->Open;

  if( CommPort1->Open ){
    TimeoutBarChange(NULL);
  }

  Ready = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Bits5Click(TObject *Sender)
{
  CommPort1->DataBits = 5;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Bits6Click(TObject *Sender)
{
  CommPort1->DataBits = 6;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Bits7Click(TObject *Sender)
{
  CommPort1->DataBits = 7;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Bits8Click(TObject *Sender)
{
  CommPort1->DataBits = 8;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::P_EvenClick(TObject *Sender)
{
  CommPort1->Parity = paEven;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::P_MarkClick(TObject *Sender)
{
  CommPort1->Parity = paMark;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::P_NoneClick(TObject *Sender)
{
  CommPort1->Parity = paNone;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::P_OddClick(TObject *Sender)
{
  CommPort1->Parity = paOdd;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::P_SpaceClick(TObject *Sender)
{
  CommPort1->Parity = paSpace;
}



//---------------------------------------------------------------------------
void __fastcall TForm1::Sop1Click(TObject *Sender)
{
  CommPort1->StopBits = sb1_0;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Stop15Click(TObject *Sender)
{
  CommPort1->StopBits = sb1_5;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Stop2Click(TObject *Sender)
{
  CommPort1->StopBits = sb2_0;
}



//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedChange(TObject *Sender)
{
  CommPort1->Baud = StrToInt(Speed->Items->Strings[Speed->ItemIndex]);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ControlModeChange(TObject *Sender)
{
  CommPort1->Control = (TComControl)(ControlMode->ItemIndex);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::DTRClick(TObject *Sender)
{
  if( !Ready )  return;
  Ready = false;
  CommPort1->DTR = DTR->Checked;
  DTR->Checked = CommPort1->DTR;
  Ready = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::RTSClick(TObject *Sender)
{
  if( !Ready )  return;
  Ready = false;
  CommPort1->RTS = RTS->Checked;
  RTS->Checked = CommPort1->RTS;
  Ready = true;
}



//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1CTSSignal(TObject *Sender)
{
  CTS->State = CommPort1->CTS ? cbGrayed : cbUnchecked;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1DSRSignal(TObject *Sender)
{
  DSR->State = CommPort1->DSR ? cbGrayed : cbUnchecked;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1DCDSignal(TObject *Sender)
{
  DCD->State = CommPort1->DCD ? cbGrayed : cbUnchecked;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1RingSignal(TObject *Sender)
{
  Ring->State = CommPort1->Ring ? cbGrayed : cbUnchecked;
}




//---------------------------------------------------------------------------
void __fastcall TForm1::PutButtonClick(TObject *Sender)
{
  char b[8192];

  int Count = 0;
  unsigned __int64 v = HexToInt64( OutputEdit->Text );
  for( int i=0; i<8; i++ ){
    unsigned char c = (v & 0xFF);
    v >>= 8;
    b[i] = c;
    if( c != 0 )  Count = i;
  }
  ++Count;

  for( int i=0; i < OutCount->Position-1; i++ ){
    memcpy( b + (Count*(i+1)), b, Count );
  }

  int s = OutCount->Position * Count;
  int out_b = CommPort1->PutBlock( b, s );

  char str1[128];
  DWORD _b = (unsigned char)b[0];
  sprintf( str1, "%.2Xh", _b );
  for( int i=1; i < Count; i++ ){
    char str2[16];
    _b = (unsigned char)b[i];
    sprintf( str2, ", %.2Xh", _b );
    strcat( str1, str2 );
  }

  char str[128];
  int _c = OutCount->Position;
  sprintf( str, "Output %s (%d times), total %d of %d bytes%s", str1, _c, out_b, s,
    (out_b == s) ? "" : " (operation is not complete)" );
  StatusBar1->SimpleText = AnsiString(str);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1TxDSignal(TObject *Sender)
{
  if( AutoRepeat->Checked )  PutButtonClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1DataReceived(TObject *Sender, DWORD Count)
{
  if( AutoShow->Checked )  GetButtonClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ClearButtonClick(TObject *Sender)
{
  Memo1->Lines->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::GetButtonClick(TObject *Sender)
{
  if( CommPort1->InBuffUsed == 0 )  return;

  int in_b = 0;

  if( Digits->Checked ){
    AnsiString s;
    while( CommPort1->InBuffUsed ){
      ++in_b;
      int n = (unsigned char)CommPort1->GetChar();

//--------------------------------------- reverse
      if( ReverseBits->Checked ){
        int m = 0;
        for( int i=0; i<8; i++ ){
          int f1 = 1 << i;
          int f2 = 0x80 >> i;
          if( n & f1 )  m |= f2;
        }
        n = m;
      }

      if( Digits->Checked ){
        if( HEX->Checked ){
          s = s + IntToHex( n, 2 ) + "h";
        }else{
          s = s + IntToStr( n );
        }
      }

      if( ASCII->Checked ){
        char c;
        if( (n >= 32) && (n <= 127) ){
          c = n;
        }else{
          c = '.';
        }
        s = s + " (" + AnsiString(c) + ")";
      }

      if( CommPort1->InBuffUsed )  s = s + AnsiString(", ");
    }

    if( ConvOem->Checked ){
      AnsiString a;
      a.SetLength( s.Length() );
      OemToCharBuff( s.c_str(), a.c_str(), s.Length() );
      Memo1->Lines->Add( a );
    }else{
      Memo1->Lines->Add( s );
    }

  }else{

    in_b = CommPort1->InBuffUsed;
    char *s = new char[in_b + 1];
    in_b = CommPort1->GetBlock( s, in_b );
    s[in_b] = 0;
    if( ConvOem->Checked ){
      AnsiString a;
      a.SetLength( strlen(s) );
      OemToCharBuff( s, a.c_str(), strlen(s) );
      Memo1->Lines->Text = Memo1->Lines->Text + a;
    }else{
      Memo1->Lines->Text = Memo1->Lines->Text + AnsiString( s );
    }
    delete s;
  }

  char str[128];
  sprintf( str, "Received %d bytes", in_b );
  StatusBar1->SimpleText = AnsiString(str);
}



//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  GetButton->Enabled = (CommPort1->InBuffUsed > 0);

  AnsiString s = AnsiString("Input: ") + IntToStr(CommPort1->InBuffUsed);
  if( CommPort1->InBuffUsed == CommPort1->InSize )
    s = s + AnsiString(" (full)");
  MainIn->Caption = s;

  s = AnsiString("Output: ") + IntToStr(CommPort1->OutBuffUsed);
  if( CommPort1->OutBuffUsed == CommPort1->OutSize )
    s = s + AnsiString(" (full)");
  MainOut->Caption = s;

//--------------------------
  int i = 0, o = 0;
  if( CommPort1->Open ){
    COMSTAT lpStat;
    DWORD lpErrors;
    ClearCommError( CommPort1->ComHandle, &lpErrors, &lpStat );
    i = lpStat.cbInQue;
    o = lpStat.cbOutQue;
  }

  PortIn->Caption = AnsiString("Input: ") + IntToStr(i);
  PortOut->Caption = AnsiString("Output: ") + IntToStr(o);
}



//---------------------------------------------------------------------------
void __fastcall TForm1::Exit1Click(TObject *Sender)
{
  Close();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::SendBreak1Click(TObject *Sender)
{
  CommPort1->SendBreak( 10 );  // 10 Ticks * 55 = 0.55 sec
}

//---------------------------------------------------------------------------
void __fastcall TForm1::IrmanInit1Click(TObject *Sender)
{
  CommPort1->PowerOff();
  Sleep(250);
  CommPort1->PowerOn();

  DTR->Checked = CommPort1->DTR;
  RTS->Checked = CommPort1->RTS;

  Sleep(250);
  PurgeComm(CommPort1->ComHandle, PURGE_TXABORT |
                                  PURGE_RXABORT |
                                  PURGE_TXCLEAR |
                                  PURGE_RXCLEAR );
  Sleep(2);
  CommPort1->PutChar('I');
  Sleep(2);
  CommPort1->PutChar('R');

  Sleep(100); // let's wait for the microcontrol's response
}

//---------------------------------------------------------------------------
void __fastcall TForm1::AutoShowClick(TObject *Sender)
{
  GetButton->Enabled = (AutoShow->Checked && CommPort1->Open);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1Open(TObject *Sender, int Error)
{
  CommPort1Close(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::CommPort1Close(TObject *Sender)
{
  char str[256];
  int n = ComNumber->Position;
  sprintf( str, "CommTest by Dmitry Vassiliev (COM%d %s)",
            n, CommPort1->Open ? "Opened" : "Closed" );
  Caption = str;

  if( CommPort1->Open ){
    ComOpen->Caption = "&Close";
  }else{
    ComOpen->Caption = "&Open";
  }

  PutButton->Enabled  = CommPort1->Open;
  IrmanInit1->Enabled = CommPort1->Open;
  SendBreak1->Enabled = CommPort1->Open;
  ButtonSend->Enabled = CommPort1->Open;

  AutoShowClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::DECClick(TObject *Sender)
{
  DEC->Checked = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::HEXClick(TObject *Sender)
{
  HEX->Checked = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::DigitsClick(TObject *Sender)
{
  HEX->Enabled = Digits->Checked;
  DEC->Enabled = Digits->Checked;
  ReverseBits->Enabled = Digits->Checked;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::About1Click(TObject *Sender)
{
  Form2->ShowModal();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Copy1Click(TObject *Sender)
{
  Memo1->CopyToClipboard();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::SelectAll1Click(TObject *Sender)
{
  Memo1->SelectAll();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Font1Click(TObject *Sender)
{
  if( FontDialog1->Execute() ){
    Memo1->Font->Assign( FontDialog1->Font );
  }
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonSendClick(TObject *Sender)
{
  char str[512];
  char cr[2] = { 0x0D, 0 };
  if( ConvOem->Checked ){
    CharToOemBuff( EditSend->Text.c_str(), str, EditSend->Text.Length() );
  }else{
    strcpy( str, EditSend->Text.c_str() );
  }
  strcat( str, cr );

  StatusBar1->SimpleText = AnsiString("Send string '") + AnsiString(str) + AnsiString("'");

  CommPort1->PutString( str );

  if( EditSend->Items->IndexOf(EditSend->Text) < 0 )
      EditSend->Items->Add(EditSend->Text);

  EditSend->Text = "";
}

//---------------------------------------------------------------------------
void __fastcall TForm1::EditSendKeyPress(TObject *Sender, char &Key)
{
  if( Key == 13 )  ButtonSendClick(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::FlushButtonClick(TObject *Sender)
{
  if( !Ready )  return;
  if( !CommPort1->Open )  return;
  Ready = false;
  CommPort1->FlushOutBuffer();
  PurgeComm( CommPort1->ComHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
  CommPort1->FlushInBuffer();
  Ready = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::TimeoutBarChange(TObject *Sender)
{
  TimeoutBar->Hint = AnsiString("Timeout ") + TimeoutBar->Position + " ms";
  Label4->Caption = TimeoutBar->Position;

  if( !Ready )  return;
  if( !CommPort1->Open )  return;
  Ready = false;
  COMMTIMEOUTS to;
  to.ReadIntervalTimeout = 2;
  to.ReadTotalTimeoutConstant = TimeoutBar->Position;
  to.ReadTotalTimeoutMultiplier = 10;
  to.WriteTotalTimeoutConstant = 2;
  to.WriteTotalTimeoutMultiplier = 2;
  SetCommTimeouts(CommPort1->ComHandle, &to);
  Ready = true;
}

//---------------------------------------------------------------------------

