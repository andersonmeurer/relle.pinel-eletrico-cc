program terminalRH;

uses
  System.StartUpCopy,
  FMX.Forms,
  main in 'src\main.pas' {frmMain},
  u_uart in 'src\_ulib\u_uart.pas',
  u_rh in 'src\_ulib\u_rh.pas',
  u_modbus in 'src\_ulib\u_modbus.pas',
  u_crc in 'src\_ulib\u_crc.pas';

{$R *.res}

begin
  //ReportMemoryLeaksOnShutdown := TRUE; // ao finalizar o programa o delphi relata se deixamos algo de destuir
  Application.Initialize;
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.
