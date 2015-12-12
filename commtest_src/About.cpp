//---------------------------------------------------------------------------
// Copyright (C) 2004 by Dmitry Vassiliev
// slydiman@mail.ru
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"

#include <shellApi.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm2 *Form2;

//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner) : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TForm2::Label4Click(TObject *Sender)
{
  ShellExecute( Handle, NULL, "mailto:slydiman@mail.ru", NULL, NULL, SW_SHOWDEFAULT );
}

//---------------------------------------------------------------------------
void __fastcall TForm2::Label9Click(TObject *Sender)
{
  ShellExecute( Handle, NULL, "http://slydiman.narod.ru/", NULL, NULL, SW_SHOWDEFAULT );
}

//---------------------------------------------------------------------------

