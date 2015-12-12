program Terminal;

uses
  Forms,
  Main in 'Main.pas' {MainForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'Terminal';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
