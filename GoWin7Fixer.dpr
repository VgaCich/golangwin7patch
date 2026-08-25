program GoWin7Fixer;

uses
  Forms,
  Main in 'Main.pas' {frmMain},
  Patcher in 'Patcher.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.
