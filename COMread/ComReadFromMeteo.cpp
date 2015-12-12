//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ComReadFromMeteo.h"
#include <windows.h>
#include <vector.h>
#include <assert.h>
#include <string.h>
//---------------------------------------------------------------------------
#define assert(ignore)((void)0)

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
bool perekl=false;
struct hComException
{
};
#define COM "Com5"
#define BodRate CBR_115200
#define TimeOutMS 3000
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
   static int TIMEOUT = TimeOutMS;
   HANDLE hCom;
   //Memo1->Text="";
   hCom = CreateFile(COM,GENERIC_READ | GENERIC_WRITE,0,NULL, OPEN_EXISTING, 0, NULL);
   if( hCom == INVALID_HANDLE_VALUE )
   {
      ShowMessage("Com port error");
      CloseHandle(hCom);
      Stat->SimpleText="Com port error";
   }
   else
   {
      SetCommMask(hCom, EV_RXCHAR);
      SetupComm(hCom, 1500, 1500);

      COMMTIMEOUTS CommTimeOuts;
      CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
      CommTimeOuts.ReadTotalTimeoutMultiplier = TIMEOUT;
      CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
      CommTimeOuts.WriteTotalTimeoutMultiplier = TIMEOUT;
      CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;

      if(!SetCommTimeouts(hCom, &CommTimeOuts))
      {
         hCom = 0;
         throw hComException();
      }

      DCB dcb;

      memset(&dcb, 0, sizeof(dcb));
      dcb.DCBlength = sizeof(DCB);
      GetCommState(hCom, &dcb);
      dcb.BaudRate = BodRate;
      dcb.fParity = 0;
      dcb.ByteSize = 8;
      dcb.Parity = NOPARITY;
      dcb.StopBits = ONESTOPBIT;
      dcb.fAbortOnError = FALSE;
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
      dcb.fBinary = TRUE;
      dcb.fParity = FALSE;
      dcb.fInX = FALSE;
      dcb.fOutX = FALSE;
      dcb.XonChar = 0;
      dcb.XoffChar = (unsigned char)0xFF;
      dcb.fErrorChar = FALSE;
      dcb.fNull = FALSE;
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.XonLim = 128;
      dcb.XoffLim = 128;
      
      SetCommState(hCom,&dcb);
      PurgeComm(hCom, PURGE_RXCLEAR);
      DWORD begin = GetTickCount();
      DWORD feedback = 0;
      BYTE data1[20];
      vector<unsigned char> data(data1, data1+20);
      String tmptxt = COM;
      tmptxt += " br";
      tmptxt += dcb.BaudRate;
      tmptxt += " p";
      tmptxt += dcb.Parity;
      tmptxt += " By";
      tmptxt += dcb.ByteSize;
      tmptxt += " sb";
      tmptxt += dcb.StopBits;
      tmptxt += " �����";
      Stat->SimpleText= tmptxt;
      unsigned char* buf = &data[0];
      DWORD len = (DWORD)data.size();
      int attempts = 3;
      while(len && (attempts || (GetTickCount()-begin) < (DWORD)TIMEOUT/3))
      {
         if(attempts) attempts--;
         if(!ReadFile(hCom, buf, len, &feedback, NULL))
         {
            CloseHandle(hCom);
            hCom = 0;
            throw hComException();
         }
         assert(feedback <= len);
         len -= feedback;
         buf += feedback;
      }
      if(len)
      {
         CloseHandle(hCom);
         hCom = 0;
         throw hComException();
      }
      int i;
      i = 0;
      while(i<(DWORD)data.size())
      {
         //Memo1->Lines->Add(i);
         //Memo1->Lines->Add(data[i]);
         i++;
      }   

    unsigned char tmp = 0;
    unsigned char startbit40 = 0;
	 while(startbit40<((DWORD)data.size()-14))
	 {
      for (int i = startbit40; i < (15+startbit40); i++ )
  	     	tmp = tmp ^ data[i];
      tmp = tmp | 0x80;
      if(tmp!=0x80 && data[startbit40]==(0x40 | 40) && tmp==data[15+startbit40])
      {
         Memo1->Lines->Add("<< ������ ����� 40");

         long Lat;
         Lat = data[1+startbit40]&~0x80;
         Lat |= ((long)(data[2+startbit40]&~0x80)<<7);
         Lat |= ((long)(data[3+startbit40]&~0x80)<<14);
         Lat |= ((long)(data[4+startbit40]&~0x80)<<21);
         Dannye->Cells[0][0]="������";
         Dannye->Cells[1][0]=Lat;

         long Lon;
         Lon = data[5+startbit40]&~0x80;
         Lon |= ((long)(data[6+startbit40]&~0x80)<<7);
         Lon |= ((long)(data[7+startbit40]&~0x80)<<14);
         Lon |= ((long)(data[8+startbit40]&~0x80)<<21);
         Dannye->Cells[0][1]="�������";
         Dannye->Cells[1][1]=Lon;

         int Angle;
         Angle = data[9+startbit40]&~0x80;
         Angle |= ((int)(data[10+startbit40]&~0x80)<<7);
         Dannye->Cells[0][2]="���� �����";
         Dannye->Cells[1][2]=Angle;

         int Vspeed;
         Vspeed = data[11+startbit40]&~0x80;
         Vspeed |= ((int)(data[12+startbit40]&~0x80)<<7);
         Dannye->Cells[0][3]="�������� �����";
         Dannye->Cells[1][3]=Vspeed;

         int Pressure;
         Pressure = data[13+startbit40]&~0x80;
         Pressure |= ((int)(data[14+startbit40]&~0x80)<<7);
         Dannye->Cells[0][4]="��������";
         Dannye->Cells[1][4]=Pressure;
      }
      startbit40++;
     }
    unsigned char startbit41 = 0;
	 while(startbit41<((DWORD)data.size()-2))
    {
      tmp = 0;
      for (int i = startbit41; i < (3+startbit41); i++ )
	     	tmp = tmp ^ data[i];
      tmp = tmp | 0x80;
      if(tmp !=0x80 && data[startbit41]==(0x40 | 41) && tmp == data[3+startbit41])
      {
         Memo1->Lines->Add("<< ������ ����� 41");

         int Antena;
         Antena = data[1+startbit41]&~0x80;
         Antena |= ((int)(data[2+startbit41]&~0x80)<<7);
         Dannye->Cells[0][5]="���� ������";
         Dannye->Cells[1][5]=Antena;
      }
      startbit41++;
     }
   }
   CloseHandle(hCom);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
   int A;
   Edit1->Text = Track->Position;
   A = Edit1->Text.ToInt();

   if(A>360)
   {
      A=360;
   }
   else if(A<0)
   {
      A=0;
   }
   Edit1->Text = A;
   int K;
   K = Edit2->Text.ToInt();
   static int TIMEOUT = TimeOutMS;
   HANDLE hCom;

   hCom = CreateFile(COM,GENERIC_READ | GENERIC_WRITE,0,NULL, OPEN_EXISTING, 0, NULL);
   if( hCom == INVALID_HANDLE_VALUE )
   {
      ShowMessage("Com port error");
      CloseHandle(hCom);
   }
   else
   {
      SetCommMask(hCom, EV_RXCHAR);
      SetupComm(hCom, 1500, 1500);

      COMMTIMEOUTS CommTimeOuts;
      CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
      CommTimeOuts.ReadTotalTimeoutMultiplier = TIMEOUT;
      CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
      CommTimeOuts.WriteTotalTimeoutMultiplier = TIMEOUT;
      CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;

      if(!SetCommTimeouts(hCom, &CommTimeOuts))
      {
         hCom = 0;
         throw hComException();
      }

      DCB dcb;

      memset(&dcb, 0, sizeof(dcb));
      dcb.DCBlength = sizeof(DCB);
      GetCommState(hCom, &dcb);
      dcb.BaudRate = BodRate;
      dcb.fParity = 0;
      dcb.ByteSize = 8;
      dcb.Parity = NOPARITY;
      dcb.StopBits = ONESTOPBIT;
      dcb.fAbortOnError = FALSE;
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
      dcb.fBinary = TRUE;
      dcb.fParity = FALSE;
      dcb.fInX = FALSE;
      dcb.fOutX = FALSE;
      dcb.XonChar = 0;
      dcb.XoffChar = (unsigned char)0xFF;
      dcb.fErrorChar = FALSE;
      dcb.fNull = FALSE;
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.XonLim = 128;
      dcb.XoffLim = 128;

      String tmptxt = COM;
      tmptxt += " br";
      tmptxt += dcb.BaudRate;
      tmptxt += " p";
      tmptxt += dcb.Parity;
      tmptxt += " By";
      tmptxt += dcb.ByteSize;
      tmptxt += " sb";
      tmptxt += dcb.StopBits;
      tmptxt += " ��������";
      Stat->SimpleText= tmptxt;

      SetCommState(hCom,&dcb);
      PurgeComm(hCom, PURGE_RXCLEAR);
      DWORD begin = GetTickCount();
      DWORD feedback = 0;
      BYTE data1[6];
      vector<unsigned char> data(data1, data1+6);

      unsigned char* buf = &data[0];
      DWORD len = (DWORD)data.size();
      data[0] = 46 | 0x40; //������� ����
      data[1] = (A & 0x007f)| 0x80;
      data[2] = ((A & 0x3f80) >> 7)| 0x80;
      data[3] = 0;
      for (int i = 0; i < 3; i++ )
		  	data[3] = data[3] ^ data[i];
      data[3] = data[3] | 0x80;
      int attempts = 3;
      while(len && (attempts || (GetTickCount()-begin) < (DWORD)TIMEOUT/3))
      {
         if(attempts) attempts--;
         if(!WriteFile(hCom, buf, len, &feedback, NULL))
         {
            CloseHandle(hCom);
            hCom = 0;
            throw hComException();
         }
         assert(feedback <= len);
         len -= feedback;
         buf += feedback;
      }
      if(len)
      {
         CloseHandle(hCom);
         hCom = 0;
         throw hComException();
      }
      Memo1->Lines->Add(">> ��������� ����� 46");
   }
   CloseHandle(hCom);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1KeyPress(TObject *Sender, char &Key)
{
  String txt;
   Set <char, '0', '9'> Dig;
	Dig << '0' << '1' << '2' << '3' << '4' << '5' << '6' <<'7'<<'8'<< '9';

   if(Key == 8)
   {

   }
   else if(Key == 0x2e)
	{
      Key = 0;
   }
	else if(Key == 0x2c)
	{
   	Key = 0x2e;
      Key = 0;
   }
   else if(Key == 13)
   {
   }
   else if(Key == 45)
   {
   }
   else if(Key == 16)
   {
   }
	else if ( !Dig.Contains(Key) )
	{
		Key = 0;
		Beep();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    if(perekl==false)
    {
       perekl=true;
       if(CheckBox1->Checked)
       {
          Button2Click(Sender);
       }
    }
    else
    {
       perekl=false;
       if(CheckBox2->Checked)
       {
          Button1Click(Sender);
       }
    }
}
//---------------------------------------------------------------------------





void __fastcall TForm1::TrackChange(TObject *Sender)
{
   Edit1->Text = Track->Position;   
}
//---------------------------------------------------------------------------


void __fastcall TForm1::TrackKeyPress(TObject *Sender, char &Key)
{
   if(Key==13)
   {
      Button2Click(Sender);
   }
}
//---------------------------------------------------------------------------

