//---------------------------------------------------------------------------
// Copyright (C) 2000 by Dmitry Vassiliev
// slydiman@mailru.com
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("CommTest.res");
USEFORM("Main.cpp", Form1);
USEFORM("About.cpp", Form2);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "COM watcher";
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(TForm2), &Form2);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
