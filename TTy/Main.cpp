//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include <tty.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
TTY Com1;

//---------------------------------------------------------------------------

__fastcall MyThread::MyThread(bool CreateSuspended)
   : TThread(CreateSuspended)
{
}
//----------------------------------------------------------------------------
void __fastcall MyThread::pb()
{
    Form1->Memo1->Text="Com1 is OK";
    Form1->Memo1->Lines->Add("***");
    Form1->Memo1->Lines->Add("ЛИР");
    Form1->Memo1->Lines->Add(LirAngle);
    Form1->Memo1->Lines->Add("Ветер м/с");
    Form1->Memo1->Lines->Add(V);
    Form1->Memo1->Lines->Add("Угол");
    Form1->Memo1->Lines->Add(A);
    Application->ProcessMessages();
}
//---------------------------------------------------------------------------
void __fastcall MyThread::Execute()
{
    Com1.Connect("Com1",115200);
    typedef union INT
    {
       unsigned short int i;
       unsigned char b[2];
    } INT;
    typedef union FLOAT
    {
       float f;
       unsigned char b[4];
    } FLOAT;

   for(int rep=1; rep<256; rep++)
   {
      if(Terminated) break;
      Com1.Read(data);

      INT LirData;
      LirData.i=0;
      FLOAT Vspeed;
      Vspeed.f=0.0;
      FLOAT Angle;
      Angle.f=0.0;
      for(int i=0; i<32; i++)
      {
        if(data[i]==0x41)
        {
           if(data[i+11]==0x40)
           {
              LirData.b[1] = data[i+1];
              LirData.b[0] = data[i+2];

              Vspeed.b[3] = data[i+3];
              Vspeed.b[2] = data[i+4];
              Vspeed.b[1] = data[i+5];
              Vspeed.b[0] = data[i+6];

              Angle.b[3] = data[i+7];
              Angle.b[2] = data[i+8];
              Angle.b[1] = data[i+9];
              Angle.b[0] = data[i+10];

              LirAngle = LirAngle + (360.*(float)LirData.i/4095. - LirAngle)*0.3;
              V = V + (Vspeed.f - V)*0.3;
              A = A + (Angle.f - A)*0.3;
           }
        }
      }
      Synchronize(pb);
   }
   Com1.Disconnect();
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{
   LirAngle=0.0;
   V=0.0;
   A=0.0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
   MyThread *thread1 = new MyThread(false);
   thread1->Priority=tpIdle;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
    Com1.Connect("Com1",115200);
    typedef union INT
    {
       unsigned short int i;
       unsigned char b[2];
    } INT;
    typedef union FLOAT
    {
       float f;
       unsigned char b[4];
    } FLOAT;
   vector <unsigned char>dataw(4);
   INT LirData;
   if(Edit1->Text.ToDouble()<360. && Edit1->Text.ToDouble()>=0.)
   {
      float ftmp = 4095.*Edit1->Text.ToDouble()/360.;
      LirData.i = (int)ftmp;
      dataw[0] = 0x40;
      dataw[1] = LirData.b[1];
      dataw[2] = LirData.b[0];
      dataw[3] = 0x41;
      Com1.Write(dataw);
   }
   Com1.Disconnect();
}
//---------------------------------------------------------------------------

