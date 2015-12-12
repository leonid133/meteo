//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "vector.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
   TButton *Button1;
   TMemo *Memo1;
   TButton *Button2;
   TEdit *Edit1;
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
vector <unsigned char>data(32);
float LirAngle, V, A;
//---------------------------------------------------------------------------
class MyThread : public TThread
{
private:
protected:
   void __fastcall pb();
   void __fastcall Execute();
public:
   __fastcall MyThread(bool CreateSuspended);
};

//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
