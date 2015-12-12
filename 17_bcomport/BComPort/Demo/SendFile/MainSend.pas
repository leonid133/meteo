///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//          Пример пересылки файла через СОМ-порт с использованием           //
//  компонента TBComPort. Файл передается частями (пакетами), т.к. он может  //
//  не поместиться целиком в выходной буфер порта. Формат передачи:          //
//  1. Имя файла;                                                            //
//  2. Размер файла;                                                         //
//  3. Строка '[BEGIN]';                                                     //
//  4. Пакеты с частями файла;                                               //
//  5. Строка '[END]'.                                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

unit MainSend;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  BCPort, ComCtrls, StdCtrls;

type
  TSendForm = class(TForm)
    BComPort1: TBComPort;
    ComboBox1: TComboBox;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    OpenDialog1: TOpenDialog;
    ProgressBar1: TProgressBar;
    procedure FormCreate(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure BComPort1TxEmpty(Sender: TObject);
  private
    { Private declarations }
    FBreak: Boolean;
    FEmpty: Boolean;
  public
    { Public declarations }
  end;

var
  SendForm: TSendForm;

implementation

{$R *.DFM}

procedure TSendForm.FormCreate(Sender: TObject);
begin
  ComboBox1.ItemIndex := 6;
end;

procedure TSendForm.Button2Click(Sender: TObject);
begin
  FBreak := True;
end;

procedure TSendForm.Button1Click(Sender: TObject);
var
  F: TFileStream;
  Buf: String;
  N, PackSize, PackCount: Integer;
begin
  FBreak := False;
  BComPort1.BaudRate := TBaudRate(ComboBox1.ItemIndex);
  if not BComPort1.Open then Exit;  // Открываем порт
  Button2.Enabled := True;
  F := TFileStream.Create(OpenDialog1.FileName, fmOpenRead); // Открываем файл
  try
    // Определим размер и количество пакетов.
    if F.Size <= BComPort1.OutBufSize then
    begin
      PackSize := F.Size; PackCount := 1;
    end else
    begin
      PackSize := BComPort1.OutBufSize;
      PackCount := F.Size div PackSize;
      if (F.Size mod PackSize) > 0 then Inc(PackCount);
    end;
    ProgressBar1.Max := PackCount;
    ProgressBar1.Position := 0;
    N := 0; FEmpty := False;
    // Передаем имя файла и его размер
    BComPort1.WriteStr(ExtractFileName(OpenDialog1.FileName) + ':' +
      IntToStr(F.Size) + '[BEGIN]');
    repeat
      repeat
        // Ждем, пока выходной буфер не освободится или пользователь не прервет передачу
        Application.ProcessMessages; Sleep(10);
      until FEmpty or FBreak;
      if FBreak then
      // Пользователь прервал передачу. Ожидание конца передачи текущего пакета
      begin
        repeat
          Application.ProcessMessages; Sleep(10);
        until FEmpty;
        Break;
      end;
      F.Seek(N * PackSize, soFromBeginning);
      if (F.Size - F.Position) < PackSize then  // Последний пакет
        PackSize := F.Size - F.Position;
      SetLength(Buf, PackSize);
      F.Read(Buf[1], PackSize);
      FEmpty := False;
      BComPort1.WriteStr(Buf);
      Inc(N);
      ProgressBar1.Position := ProgressBar1.Position + 1;
    until N = PackCount;
    BComPort1.WriteStr('[END]'); // Строка, идентифицирующая конец передачи
  finally
    F.Free;
    if BComPort1.Close then Button2.Enabled := False;
  end;
end;

procedure TSendForm.Button3Click(Sender: TObject);
begin
  if OpenDialog1.Execute then Button1.Enabled := True;
end;

procedure TSendForm.BComPort1TxEmpty(Sender: TObject);
begin
  FEmpty := True; // Все данные ушли из буфера порта, можно передавать еще пакет
end;

end.
