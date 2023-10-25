unit main;

interface

uses
  System.SysUtils, System.Types, System.UITypes, System.Classes, System.Variants,
  FMX.Types, FMX.Controls, FMX.Forms, FMX.Graphics, FMX.Dialogs, FMX.Layouts,
  FMX.StdCtrls, FMX.ListBox, FMX.Objects, FMX.Controls3D, System.Math.Vectors,
  FMX.Objects3D, System.Rtti, FMX.Grid, FMX.SpinBox,
  FMX.Controls.Presentation, FMX.Edit, IniFiles, FMX.TabControl, FMX.Colors,
  FMX.Memo, FMX.DateTimeCtrls, FMX.ExtCtrls, FMX.EditBox, FMX.NumberBox,

  // minhas libs
  u_rh, u_modbus;

const
    clERROR = TAlphaColorRec.Red;
    clOK = TAlphaColorRec.Black;
    styleWINDOWS = 0;
    styleTRANSP = 1;
    styleJET = 2;
    MODEL_UNKNOW = 'Modelo';

type
  TfrmMain = class(TForm)
    timerProcess: TTimer;
    StatusBar1: TStatusBar;
    StyleBookJet: TStyleBook;
    StyleBookTransp: TStyleBook;
    lblStatus: TLabel;
    Label3: TLabel;
    Grid3D1: TGrid3D;
    StyleBookWindows: TStyleBook;
    pnlModel: TPanel;
    lblModel: TLabel;
    Panel10: TPanel;
    btnConnection: TButton;
    btnPE: TButton;
    btnSair: TButton;
    TabControl: TTabControl;
    TabItem5: TTabItem;
    TabItem7: TTabItem;
    pnlConnection: TPanel;
    Label6: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    edtID: TNumberBox;
    spiTimeout: TSpinBox;
    btnCnt: TButton;
    cbxPort: TComboBox;
    Label12: TLabel;
    btnUpDatePorts: TButton;
    cbxStyle: TComboBox;
    LayoutIndConnect: TLayout;
    indCnt: TAniIndicator;
    lblIndCon: TLabel;
    pnlMain: TPanel;
    pnlTR: TPanel;
    cbxUpdate: TCheckBox;
    btnStatus: TButton;
    memoLog: TMemo;
    btnClear: TButton;
    edtBaud: TNumberBox;
    Label2: TLabel;
    Label5: TLabel;
    Label8: TLabel;
    btnApplyPE: TButton;
    btnReadPE: TButton;
    cbxRelay1: TCheckBox;
    cbxRelay2: TCheckBox;
    cbxRelay3: TCheckBox;
    cbxRelay4: TCheckBox;
    Panel3: TPanel;
    lblS1: TLabel;
    lblM1: TLabel;
    Panel1: TPanel;
    lblS2: TLabel;
    lblM2: TLabel;
    Panel2: TPanel;
    lblS3: TLabel;
    lblM3: TLabel;
    Panel4: TPanel;
    lblS4: TLabel;
    lblM4: TLabel;
    Panel5: TPanel;
    lblS5: TLabel;
    lblM5: TLabel;
    Panel6: TPanel;
    lblS6: TLabel;
    lblM6: TLabel;
    Panel7: TPanel;
    lblS7: TLabel;
    lblM7: TLabel;
    Panel8: TPanel;
    lblS8: TLabel;
    lblM8: TLabel;
    Panel9: TPanel;
    lblS9: TLabel;
    lblM9: TLabel;
    cbxRelay5: TCheckBox;
    cbxRelay6: TCheckBox;
    cbxRelay7: TCheckBox;
    cbxRelay8: TCheckBox;
    Panel11: TPanel;
    lblS10: TLabel;
    lblM10: TLabel;
    Label1: TLabel;
    Label4: TLabel;
    Label7: TLabel;
    Label11: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    Label18: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure btnCntClick(Sender: TObject);
    procedure timerProcessTimer(Sender: TObject);
    procedure btnSairClick(Sender: TObject);
    procedure cbxStyleChange(Sender: TObject);
    procedure spiTimeoutChange(Sender: TObject);
    procedure btnUpDatePortsClick(Sender: TObject);
    procedure btnClearClick(Sender: TObject);
    procedure btnStatusClick(Sender: TObject);
    procedure btnConnectionClick(Sender: TObject);
    procedure btnPEClick(Sender: TObject);
    procedure cbxUpdateChange(Sender: TObject);
    procedure btnReadPEClick(Sender: TObject);
    procedure btnApplyPEClick(Sender: TObject);
    procedure edtBaudChange(Sender: TObject);
    procedure Panel11Click(Sender: TObject);
  private
    { Private declarations }
    StyleForm:integer;
    Flog:textfile;
    SendCmdTR: boolean;
    function Report(const color: TAlphaColor; const Format: string; const Args: array of const): string; overload;
    function Report(const color: TAlphaColor; const Format: string): string; overload;
    procedure InitPrintSensor(lblM, lblS:tlabel);
    procedure InitPrint;
    procedure CompsState;
    function SendCommand(cmd: tCommand):integer;
    procedure ModbusError(codError:integer);
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;
  fileInit:TIniFile;
  rh:tRH;

implementation

{$R *.fmx}

// #############################################################################
// SYSTEM
// #############################################################################
procedure TfrmMain.FormCreate(Sender: TObject);
begin
    TabControl.Visible := FALSE;
    btnConnectionClick(NIL);

    assignfile(Flog, 'terminalRH.log');
    Rewrite(Flog);
    writeLn(Flog, ' >>>>>>>>> INICIAÇÃO DO SISTEMA - Data: ' + DateToStr(date) + ' Hora:' + TimeToStr(time) + ' <<<<<<<< ');

    fileInit := TIniFile.Create('terminalRH.ini');
    cbxStyle.ItemIndex := fileInit.ReadInteger('STYLE','SYSTEM', styleTRANSP);

    rh:= tRH.Create;
    rh.nMultimetersGeren := 10;
    rh.SaveLog := TRUE;
    btnUpDatePortsClick(NIL);
    cbxPort.ItemIndex := fileInit.ReadInteger('MODBUS','PORT', 0);
    rh.BaudRate:= fileInit.ReadInteger('MODBUS','BAUDRATE', 115200);
    rh.Timeout:= fileInit.ReadInteger('MODBUS','TIMEOUT', 3);
    edtBaud.Value := rh.BaudRate;
    edtID.Value := fileInit.ReadInteger('MODBUS','ADDRESS', 1);
    spiTimeout.Value:= rh.Timeout;

    lblModel.Text := MODEL_UNKNOW;
    InitPrint;
    LayoutIndConnect.Visible := FALSE;
    indCnt.Enabled := TRUE;
    sendCmdTR := TRUE;

    CompsState;
end;

procedure TfrmMain.FormDestroy(Sender: TObject);
begin
    fileInit.WriteInteger('STYLE','SYSTEM', StyleForm);
    fileInit.WriteInteger('MODBUS','PORT', cbxPort.ItemIndex);
    fileInit.WriteInteger('MODBUS','BAUDRATE',rh.BaudRate);
    fileInit.WriteInteger('MODBUS','ADDRESS', round(edtID.Value));
    fileInit.WriteInteger('MODBUS','TIMEOUT', rh.Timeout);

    fileInit.Free;
    rh.Free;

    CloseFile(FLog);
end;

procedure TfrmMain.btnSairClick(Sender: TObject);
begin
    Application.Terminate;
end;

procedure ApllyStyle(Style:integer);
begin
    case Style of
    styleWINDOWS: frmMain.StyleBook := frmMain.StyleBookWindows;
    styleTRANSP: frmMain.StyleBook := frmMain.StyleBookTransp;
    styleJET: frmMain.StyleBook := frmMain.StyleBookJet;
    end;
    frmMain.StyleForm := Style;
end;

// #############################################################################
// REPORT
// #############################################################################
function TfrmMain.Report(const color: TAlphaColor; const Format: string; const Args: array of const): string;
begin
    fmtStr(result, format, args);
    lblStatus.TextSettings.FontColor := color;
    lblStatus.Text := Result;
    memoLog.lines.add(Result);
    memoLog.GoToTextEnd;
end;

function TfrmMain.Report(const color: TAlphaColor; const Format: string): string;
begin
    Report(color, Format,[]);
end;

procedure TfrmMain.btnClearClick(Sender: TObject);
begin
    memoLog.Lines.Clear;
end;

// #############################################################################
// CONEXÃO
// #############################################################################
procedure TfrmMain.edtBaudChange(Sender: TObject);
begin
    rh.BaudRate := round(edtBaud.Value);
end;

procedure TfrmMain.spiTimeoutChange(Sender: TObject);
begin
    rh.Timeout := round(spiTimeout.Value);
end;

procedure TfrmMain.cbxStyleChange(Sender: TObject);
begin
    ApllyStyle(cbxStyle.ItemIndex);
end;

procedure TfrmMain.CompsState;
begin
    btnPE.Enabled := rh.Connected;

    cbxPort.Enabled := not rh.Connected;
    edtID.Enabled := not rh.Connected;
    edtBaud.Enabled := not rh.Connected;
    spiTimeout.Enabled := not rh.Connected;
    btnUpDatePorts.Enabled := not rh.Connected;

    timerProcess.Enabled := rh.Connected;

    //    for i:=0 to pnlCtrl.ControlCount -1 do begin
    //        if  (pnlCtrl.Controls[i] is TButton) or
    //            (pnlCtrl.Controls[i] is TCheckBox)
    //        then
    //            pnlCtrl.Controls[i].Enabled := portOpen;
    //    end;
end;

procedure TfrmMain.btnCntClick(Sender: TObject);
var id:integer;
    port:string;
begin
    lblModel.Text := MODEL_UNKNOW;

    if (rh.Connected) then begin
        rh.CloseConnection;
        LayoutIndConnect.Visible := FALSE;  // Esconde o painel visor de conexão

        writeLn(Flog, 'FECHANDO CONEXÃO');
        btnCnt.Text := 'Conectar';
        Report(clOK, 'Desconectado do dispositivo');
        InitPrint;
        CompsState;
        exit;
    end;

    port:= cbxPort.Items[cbxPort.ItemIndex];
    id := round(edtID.Value);

    btnCnt.Enabled := FALSE;
    LayoutIndConnect.Visible := TRUE;
    writeLn(Flog, 'ABRINDO CONEXÃO');
    writeLn(Flog, format('  Conectando ao Recurso de Hardware id %d porta %s',[id, port]));
    Report(clOK, 'Conectando ao Recurso de Hardware id %d porta %s',[id, port]);

    if (rh.OpenConnection(port, id) < 0) then begin
        MessageDlg(
            'Não foi possível fazer conexão com o Recurso de Hardware, por favor cheque os seguintes itens:'+#10#13+
            '1 - Se o Recurso de Hardware está ligado' +#10#13+
            '2 - Se há porta de comunicação COM' +#10#13+
            '3 - Se o cabo está conectado entre o computador ao Recurso de Hardware'+#10#13+
            '4 - Se a porta de comunicação esteja sendo usado por outro software',
            TMsgDlgType.mtWarning,
            [TMsgDlgBtn.mbOk],
            0
        );

        btnCnt.Text := 'Conectar';
        Report(clERROR, 'Erro [%d] de conexão ao Recurso de Hardware',[rh.ErrorCom]);
        writeLn(Flog, '  Não estabelecido a comunicação');
        writeLn(Flog, format('  Erro [%d] de conexão ao Recurso de Hardware',[rh.ErrorCom]));
    end else begin
        Report(clERROR, 'Conectado ao dispositivo %s firmware versão %s',[rh.Model, rh.Fimware]);
        writeLn(frmMain.Flog, format('Conectado ao dispositivo %s firmware versão %s',[rh.Model, rh.Fimware]));
        lblModel.Text := format('Recurso de Hardware %s - Firmware %s',[rh.Model, rh.Fimware]);

        btnCnt.Text := 'Desconectar';
    end;

    CompsState;
    LayoutIndConnect.Visible := FALSE;
    btnCnt.Enabled := TRUE;
end;


procedure TfrmMain.btnUpDatePortsClick(Sender: TObject);
var qtdPort:integer;
    i:integer;
begin
    qtdPort := rh.GetPorts(cbxPort.Items);
    if ( qtdPort <= 0) then begin
        cbxPort.ItemIndex := -1;
        report(clERROR, 'Nenhuma porta COM foi encontrada',[qtdPort]);
    end else begin
        cbxPort.ItemIndex := 0;
        report(clOK, 'Portas COMs encontradas:');
        for i:= 0 to cbxPort.Items.Count -1 do
             report(clOK, '    %s',[(cbxPort.Items[i])]);
        report(clOK, 'Encontrado %d portas COMs',[qtdPort]);
    end;
end;


// #############################################################################
// MENU
// #############################################################################
procedure TfrmMain.btnConnectionClick(Sender: TObject);
begin
    pnlConnection.Parent := pnlMain;
    pnlTR.Parent := NIL;
end;

procedure TfrmMain.btnPEClick(Sender: TObject);
begin
    pnlConnection.Parent := NIL;
    pnlTR.Parent := pnlMain;
end;


// #############################################################################
// CMD
// #############################################################################
function TfrmMain.SendCommand(cmd: tCommand):integer;
var ret: integer;
begin
    repeat
        ret := rh.SendCommand(cmd);
    until (ret <> errMODBUS_BUSY);

    result := ret;
end;

procedure TfrmMain.ModbusError(codError:integer);
var msg:string;
begin
    case codError of
    errMODBUS_ILLEGAL_FUNCTION: begin
        msg := 'ILLEGAL FUNCTION';
    end;
    errMODBUS_ILLEGAL_DATA_ADDRESS: begin
        msg := 'ILLEGAL DATA ADDRESS';
    end;
    errMODBUS_ILLEGAL_DATA_VALUE: begin
        msg := 'ILLEGAL DATA VALUE';
    end;
    errMODBUS_SLAVE_DEVICE_FAILURE: begin
        msg := 'SLAVE DEVICE FAILURE';
    end;
    errMODBUS_TIMEOUT: begin
        msg := 'TIMEOUT';
    end else begin
        msg := format('Unknow %d',[codError]);
    end;
    end;

    //    {Cria a janela de mensagem}
    //    Mensagem:= CreateMessageDialog(
    //        msg,
    //        mtError,  // mtWarning, mtError, mtInformation, mtConfirmation, mtCustom
    //        [mbOK]      // mbYes, mbNo, mbOK, mbCancel, mbAbort, mbRetry, mbIgnore, mbAll, mbNoToAll, mbYesToAll, mbHelp, mbClose
    //    );
    //
    //    {Trazur o titulo da mensagem}
    //    Mensagem.Caption:= 'MOD BUS ERROR';
    //
    //    {Exibr a caixa de mensagem}
    //    //if sound then  PlaySystemSound('SYSTEMHAND');
    //    Mensagem.ShowModal;
    //    Mensagem.Free;

    report(0, 'Modbus error %s',[msg]);
end;


procedure TfrmMain.Panel11Click(Sender: TObject);
begin

end;

// #############################################################################
// SENSORES
// #############################################################################
procedure TfrmMain.InitPrintSensor(lblM, lblS:tlabel);
begin
    lblM.Text := 'Multímetro';
    lblS.Tag := -1000000;
    lblS.Text := '---';
end;

procedure TfrmMain.InitPrint;
begin
    InitPrintSensor(lblM1, lblS1);
    InitPrintSensor(lblM2, lblS2);
    InitPrintSensor(lblM3, lblS3);
    InitPrintSensor(lblM4, lblS4);
    InitPrintSensor(lblM5, lblS5);
    InitPrintSensor(lblM6, lblS6);
    InitPrintSensor(lblM7, lblS7);
    InitPrintSensor(lblM8, lblS8);
    InitPrintSensor(lblM9, lblS9);
end;

function formatValue(value:integer):string;
begin
    if (value>=1000) then begin
        result := format('%f',[value/1000]);
    end else result := format('%d',[value]);
end;

procedure PrintSensor (lblM, lblS:tlabel; mult:tMultimeter);
begin
    if (mult.stsCom >= 0) and (mult.stsCom < 5) then begin
        lblS.Text:= 'Desconectado';
        exit;
    end;

    lblM.Tag := 1;
    if ( (lblS.tag <> mult.value) or (lblS.tag <> mult.sts) ) then begin
        if (mult.sts = stsSENSOR_READING) then begin
            lblS.tag := stsSENSOR_READING;
            lblS.Text:= '0';
        end else if (mult.sts = stsSENSOR_ERRMIN) then begin
            lblS.tag := stsSENSOR_ERRMIN;
            lblS.Text:= '0.00';
        end else if (mult.sts = stsSENSOR_ERRMAX) then begin
            lblS.tag := stsSENSOR_ERRMAX;
            lblS.Text:= 'ALTO';
        end else if (mult.sts = stsSENSOR_OK) then begin
            lblS.tag := mult.value;
            lblS.Text:= formatValue(mult.value) + '  '; // IntToStr(sensor.value) + '  ';
        end else begin
            lblM.Tag := 0;
            lblS.tag := mult.sts;
            lblS.Text:= '---';
        end;
    end;

    if (lblM.Tag = 0) then
        lblM.Text := 'Multímetro'
    else if (mult.func = 1) then
         lblM.Text := 'Amperímetro'
    else lblM.Text := 'Voltímetro';
end;

procedure TfrmMain.cbxUpdateChange(Sender: TObject);
begin
    if cbxUpdate.IsChecked then
            report(0, 'Leitura dos sensores automática')
    else report(0, '´Leitura automática cancelada');
    btnStatus.Enabled := not cbxUpdate.IsChecked;
end;

procedure TfrmMain.btnStatusClick(Sender: TObject);
var ret :integer;
begin
    if (timerProcess.Tag = 0) then report(0, 'Envio de comando para ler multimetros');

    ret := SendCommand(cmdGET_MULTIMETERS);
    if ret > 0 then begin
        PrintSensor(lblM1, lblS1, rh.Multimeter[0]);
        PrintSensor(lblM2, lblS2, rh.Multimeter[1]);
        PrintSensor(lblM3, lblS3, rh.Multimeter[2]);
        PrintSensor(lblM4, lblS4, rh.Multimeter[3]);
        PrintSensor(lblM5, lblS5, rh.Multimeter[4]);
        PrintSensor(lblM6, lblS6, rh.Multimeter[5]);
        PrintSensor(lblM7, lblS7, rh.Multimeter[6]);
        PrintSensor(lblM8, lblS8, rh.Multimeter[7]);
        PrintSensor(lblM9, lblS9, rh.Multimeter[8]);
        PrintSensor(lblM10, lblS10, rh.Multimeter[9]);
    end;

    if (ret < 0) then begin
        ModbusError(ret);
        exit;
    end;

    if (timerProcess.Tag = 0) then report(0, 'Status lido com sucesso');
end;

procedure TfrmMain.timerProcessTimer(Sender: TObject);
begin
    if (not cbxUpdate.IsChecked) then exit;

    // vamos desligar o timer para não haver overload das chamadas das funções abaixo
    // pois algumas funções são bem grandes
    timerProcess.Enabled:= FALSE;
    timerProcess.Tag := 1;
    btnStatusClick(NIL);
    timerProcess.Enabled:= TRUE;
    timerProcess.Tag := 0;
end;

procedure TfrmMain.btnReadPEClick(Sender: TObject);
var ret :integer;
begin
    report(0, 'Envio de comando para ler os estados das chaves e dos reles');

    ret := SendCommand(cmdGET_RELAY);
    if ret > 0 then begin
        cbxRelay1.IsChecked := ((rh.Relay and $1) > 0);
        cbxRelay2.IsChecked := ((rh.Relay and $2) > 0);
        cbxRelay3.IsChecked := ((rh.Relay and $4) > 0);
        cbxRelay4.IsChecked := ((rh.Relay and $8) > 0);
        cbxRelay5.IsChecked := ((rh.Relay and $10) > 0);
        cbxRelay6.IsChecked := ((rh.Relay and $20) > 0);
        cbxRelay7.IsChecked := ((rh.Relay and $40) > 0);
        cbxRelay8.IsChecked := ((rh.Relay and $80) > 0);
    end;

    if (ret < 0) then begin
        ModbusError(ret);
        exit;
    end;

    report(0, 'Leituras feitas com sucesso');
end;

procedure TfrmMain.btnApplyPEClick(Sender: TObject);
var ret:integer;
begin
    report(0, 'Gravando estados dos reles e chaves');

    rh.Relay := 0;
    if (cbxRelay1.IsChecked) then inc(rh.Relay, $1);
    if (cbxRelay2.IsChecked) then inc(rh.Relay, $2);
    if (cbxRelay3.IsChecked) then inc(rh.Relay, $4);
    if (cbxRelay4.IsChecked) then inc(rh.Relay, $8);
    if (cbxRelay5.IsChecked) then inc(rh.Relay, $10);
    if (cbxRelay6.IsChecked) then inc(rh.Relay, $20);
    if (cbxRelay7.IsChecked) then inc(rh.Relay, $40);
    if (cbxRelay8.IsChecked) then inc(rh.Relay, $80);

    ret := SendCommand(cmdSET_RELAY);
    if (ret < 0) then begin
        ModbusError(ret);
        exit;
    end;

    report(0, 'Gravações feitas com sucesso');
end;

end.
