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
struct hComException
{
};
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
   static int TIMEOUT = 1000;
   HANDLE hCom;

   hCom = CreateFile("Com1",GENERIC_READ | GENERIC_WRITE,0,NULL, OPEN_EXISTING, 0, NULL);
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
      CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
      CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
      CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
      CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
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
      dcb.BaudRate = CBR_115200;
      dcb.fParity = false;
      dcb.ByteSize = 8;
      dcb.Parity = EVENPARITY;
      dcb.StopBits = ONESTOPBIT;
      dcb.fAbortOnError = TRUE;
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

      DWORD begin = GetTickCount();
      DWORD feedback = 0;
      BYTE data1[10];
      vector<unsigned char> data(data1, data1+13);

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
         Memo1->Lines->Add(data[i]);
         i++;
      }
      /*
        if(!SetCommState(m_Handle, &ComDCM)) {
                CloseHandle(m_Handle);
                m_Handle = 0;
                throw TTYException();
        }
      */


      /*
      unsigned char buf[13];
      ReadFile(hCom, buf, sizeof(buf), &feedback, NULL);
      int LirData, tmp;
      Memo1->Lines->Add("---------------");
      Memo1->Lines->Add(buf[0]);
      typedef union INT {
         unsigned int i;
         unsigned char b[2];
      } INT;
      INT tmpInt;
      tmpInt.b[0] = buf[1];
      tmpInt.b[1] = buf[2];
      LirData =  tmpInt.i;
      Memo1->Lines->Add(LirData);
      typedef union FLOAT {
         float f;
         unsigned char b[4];
      } FLOAT;
      FLOAT tmpFloat;
      tmpFloat.b[0] = buf[3];
      tmpFloat.b[1] = buf[4];
      tmpFloat.b[2] = buf[5];
      tmpFloat.b[3] = buf[6];
      Memo1->Lines->Add(tmpFloat.f);
      tmpFloat.b[0] = buf[7];
      tmpFloat.b[1] = buf[8];
      tmpFloat.b[2] = buf[9];
      tmpFloat.b[3] = buf[10];
      Memo1->Lines->Add(tmpFloat.f);
      for(int i =0; i<14; i++)
      {
         tmp = buf[i];
         Memo1->Lines->Add(tmp);
      }
      */
        /*
          void TTY::Read(vector<unsigned char>& data) {
 
        if(m_Handle == 0) {
                throw TTYException();
        }
 
        DWORD begin = GetTickCount();
        DWORD feedback = 0;
 
        unsigned char* buf = &data[0];
        DWORD len = (DWORD)data.size();
 
        int attempts = 3;
        while(len && (attempts || (GetTickCount()-begin) < (DWORD)TIMEOUT/3)) {
 
                if(attempts) attempts--;
 
                if(!ReadFile(m_Handle, buf, len, &feedback, NULL)) {
                        CloseHandle(m_Handle);
                        m_Handle = 0;
                        throw TTYException();
                }
 
                assert(feedback <= len);
                len -= feedback;
                buf += feedback;
 
        }
 
        if(len) {
                CloseHandle(m_Handle);
                m_Handle = 0;
                throw TTYException();
        }
 
 }
        */
   }
   CloseHandle(hCom);

}
//---------------------------------------------------------------------------

