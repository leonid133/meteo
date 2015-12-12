program SendFile;

uses
  Forms,
  MainSend in 'MainSend.pas' {SendForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'SendFile';
  Application.CreateForm(TSendForm, SendForm);
  Application.Run;
end.
