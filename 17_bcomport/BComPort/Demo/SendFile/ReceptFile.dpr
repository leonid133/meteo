program ReceptFile;

uses
  Forms,
  MainRecept in 'MainRecept.pas' {ReceptForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'ReceptFile';
  Application.CreateForm(TReceptForm, ReceptForm);
  Application.Run;
end.
