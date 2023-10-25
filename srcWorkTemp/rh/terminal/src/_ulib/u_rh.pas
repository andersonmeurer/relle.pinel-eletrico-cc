unit u_rh;

{
    Esta lib foi criada em 03/04/2015 e gerencia somente uma conexão por vez nos WorkBench
    Para multiplas coenxões podemos explorar os recuros do tList
}

interface

{$IFDEF VER280} // XE7
uses Classes, SysUtils, DateUtils, u_modbus, u_uart, Vcl.Forms;
{$ELSE}
uses Classes, SysUtils, DateUtils, u_modbus, u_uart;
{$ENDIF}

const
    // status dos sensores
    stsSENSOR_READING	= 0;
    stsSENSOR_OK	    = 1;
    stsSENSOR_ERRMIN  = 2;
    stsSENSOR_ERRMAX  = 3;

type
    // comandos para serem enviados ao Multimetro
    tCommand = (
        cmdNONE,
        cmdGET_INFO,        // Comando para ler os registradores: modelo, versão firmware e modo de operações dos reles
        cmdGET_MULTIMETERS, // Comando para ler os sensores
        cmdGET_DOUT,        // Comando para ler os estados das saídas digitais do RH
        cmdSET_DOUT,        // Comando para gravar os estados das saídas digitais do RH
        cmdGET_RELAY,       // Comando para ler os estados dos reles do RH
        cmdSET_RELAY        // Comando para gravar os estados dos reles do RH
    );


    tMultimeter = record
	    stsCom:integer;	// Status de comunicação:
        				//		0: Sem comunicação com o dispositivo. O mesmo não está conectado, ou está desligado, ou não há dispositivo neste endereço.
		        		//		1: O multímetro recebeu uma função que não foi implementada;
				        //		2: Foi acessado a um endereço de registrador inexistente;
        				//		3: Foi tentado gravar um valor inválido no registrador do multímetro;
		        		//		4: Um irrecuperável erro ocorreu enquanto o multímetro estava tentando executar a ação solicitada;
				        //		5: Comunicação estabelecida com sucesso
        func:integer;           // Funcção assumida pelo multimetro
                                //      0: Voltimetro
                                //      1: Amperimetro
        sts:integer;            // Status do sensor
                                //      0: Sinaliza que o multimetro está lendo pela primeira vez.
                                //          Isto somente acontece no momento que o multimetro é ligado.
                                //      1: O Multimetro já contém valor da amp/volts;
                                //      2: Sinaliza um erro, indica que o valor está abaixo da escala
                                //      3: Sinaliza um erro, indica que o valor está acima da escala
        value:integer;      // valores em miliampers ou milivolts
    end;


    tRH = class
    private
        flog:textfile;
        FRegs: array [0..120] of word;      // registradores para comunicação via modbus
        FBaudRate: integer;
        FConnected: boolean;
        FSaveLog:boolean;                   // sinaliza para salvar ou não arquivo de LOG
        FTimeout: integer;
        FModBus:tModBus;
        Fid: integer;                       // Endereço do Multimetro a ser acessado
        FUart:tUart;

        procedure SetSaveLog(sl:boolean);
        procedure SetTimeout(tout:integer);
        procedure SetBaudRate(br:integer);
    public
        nMultimetersGeren: integer;	                // Determina a quantidade de multimetros o RH deve gerenciar, inicio pelo ID=1
        ErrorCom:integer;                           // Salva o erro mais recente de comunicação via modbus
        Busy: boolean;                              // Sinalia quando estamos eviando ou esperando resposta do Multimetro
        Fimware:string;                             // Versão do Multimetro
        Model:string;                               // Modelo do Multimetro
        Dout: integer;                              // Estado das saídas digitais do RH
        Relay: integer;                             // Estado dos reles no RH
        Multimeter: array[0..15] of tMultimeter;    // Dados dos sensors do Multimetro

        property Timeout:integer read FTimeout write SetTimeout;
        property BaudRate:integer read FBaudRate write SetBaudRate;
        property Connected: boolean read FConnected write FConnected;
        property SaveLog:boolean read FSaveLog write SetSaveLog;

        constructor Create;
        destructor Destroy; override;
        function GetPorts(PortsList:TStrings):integer;
        function OpenConnection(PortCOM: string; id:integer):integer;
        procedure CloseConnection;
        function SendCommand(cmd: tCommand):integer;
    end;

implementation

// #############################################################################
// SYSTEM
// #############################################################################
constructor tRH.Create;
var flogName:string;
    x:integer;
begin
    flogName:= 'commands.log';
    assignfile(flog, flogName);
    Rewrite(flog);
    writeLn(flog, ' >>>>>>>>> INICIAÇÃO DO LOG DE COMANDOS - Data: ' + DateToStr(date) + ' Hora:' + TimeToStr(time) + ' <<<<<<<< ');

    FUart:= tUART.Create;
    FModBus:= tModBus.Create(FUart.Write, FUart.GetC, FUart.Purge);

    Busy := FALSE;
    BaudRate:= 115200;
    ErrorCom := 0;
    nMultimetersGeren:= 1;

    Fimware:= '';
    Model:= '';

    // Sinalizar com informações inválidas
    for x:= 0 to 15 do begin
        Multimeter[x].stsCom := 0;
    end;
    Dout:= 0;
    Relay:=0;
end;

destructor tRH.Destroy;
begin
    FModBus.Free;

    if (Connected) then FUart.Close;
    FUart.Free;

    closefile(fLog);

    // CHAMA A FUNÇÃO DESTROY PAI
    inherited; // Destroi esta lista (tList)
end;

procedure tRH.SetTimeout(tout:integer);
begin
    if (tout < 1) or (tout>10) then tout:= 3;
    FTimeout := tout;
    FModBus.Timeout := tout;
end;

procedure tRH.SetSaveLog(sl:boolean);
begin
    FSaveLog := sl;
    FModBus.SaveLog := sl;
end;

procedure tRH.SetBaudRate(br:integer);
begin
    FBaudRate:= br;
    FUart.SetBaud(br);
end;

// #############################################################################
// CONEXÃO
// #############################################################################
// retorna com a lista de string com as portas COMs encontradas
// EXEMPLO de uso:
//        procedure TfrmMain.btnUpDatePortsClick(Sender: TObject);
//        var qtdPort:integer;
//            i:integer;
//        begin
//            qtdPort := Multimetro.GetPorts(cbxPorts.Items);
//            if ( qtdPort <= 0) then begin
//                report(clERROR, 'Nenhuma porta COM encontrada',[qtdPort]);
//            end else begin
//                report(clOK, 'Encontrado %d portas COMs',[qtdPort]);
//                for i:= 0 to cbxPorts.Items.Count -1 do
//                     report(clOK, 'Encontrado porta %s',[(cbxPorts.Items[i])]);
//            end;
//        end;
function tRH.GetPorts(PortsList:TStrings):integer;
begin
     result:= uart_GetCommList(PortsList);
end;

// Abre uma conexão com o Multimetro
// se a conexão foi aberta a função retorna 1 e com as vars Model e Version preenchidas
// senão retorna o código de erro modbus, também salvo em ErrorCom, caso não abriu a conexão
function tRH.OpenConnection(PortCOM: string; id:integer):integer;
begin
    Fid := id;
    if (Connected) then FUart.Close;

    if FSaveLog then writeLn(flog, format('Abrindo porta %s',[PortCOM]));

    // abrir porta UART
    if (not FUart.Open(PortCOM)) then begin
        ErrorCom := errMODBUS_OPEN_UART;
        exit(errMODBUS_OPEN_UART);      // Não prossegue se a porta não existe ou se está sendo usada
                                        //   por outro software
    end;

    // enviar um comando de solicitação de reconhecimento, neste caso vamos solicitar o modelo do Multimetro
    result := SendCommand(cmdGET_INFO); // envia o comando ao Multimetro

    if (result > 0 ) then begin
        result:= 1;
        Connected := TRUE;
    end else begin
        Connected := FALSE;
    end;
end;

procedure tRH.CloseConnection;
var x:integer;
begin
    if (not Connected) then exit;

    ErrorCom := 0;
    Busy := FALSE;
    FUart.Close;
    Connected:= FALSE;
    Fimware:= '';
    Model:= '';

    // Sinalizar com informações inválidas
    for x:= 0 to 15 do begin
        Multimeter[x].stsCom := 0;
    end;
    Dout:= 0;
    Relay:=0;
end;

// Envia um comando de leitura ou escrita nos registradores do Multimetro.
// Para comando de leitura as respectivas variáveis no Multimetro serão atualizadas
// Para comando de escrita as respectivas variáveis no Multimetro devem estar preenchidas
// Retorna 1 se tudo ocorreu bem
// senão retorna o código de erro modbus, também salvo em ErrorCom
// EXEMPLO:
//        function TfrmMain.SendCommand(cmd: tCommand):integer;
//        var ret: integer;
//        begin
//            repeat
//                ret := Multimetro.SendCommand(cmd);
//            until (ret <> errMODBUS_BUSY);
//
//            result := ret;
//        end;
//
//        procedure TfrmMain.btnRTCReadClick(Sender: TObject);
//        var ret:integer;
//        begin
//            report(0, 'Envio de comando para ler os valores de calibração do RTC');
//            ret := SendCommand(cmdGET_RTC_CAL);
//
//            if (ret < 0) then begin
//                ModbusError(ret);
//                exit;
//            end;
//
//            edtCALVAL.Value := Multimetro.CalVal;
//            edtCALDIR.Value := Multimetro.CalDir;
//
//            report(0, 'Valores de calibração do RTC lidos com sucesso');
//        end;


type tcmd = (readREGS, writeREG, writeREGS);
function tRH.SendCommand(cmd: tCommand):integer;
var addrInit, nRegs, x:integer;
    typeCMD: tcmd;
    value: word;
begin
    if (Busy) then exit(errMODBUS_BUSY);
    Busy := TRUE;

    // APONTA QUAIS REGISTRADORES A ACESSAR NO MULTIMETRO
    // -----------------------------------------------------------------------------------------------------------------

    // Comando para ler os registradores: modelo, versão firmware e modo de operações dos reles
    typeCMD := readREGS;
    addrInit := 0;
    nRegs:= 3;
    value := 0;
    case cmd of

    // comando para ler os estados das saidas digitais
    cmdGET_DOUT: begin
        addrInit := $200;
        nRegs:= 1;
    end;

    // comando para gravar os estados das saidas digitais
    cmdSET_DOUT: begin
        typeCMD := writeREG;
        addrInit := $200;
        nRegs:= 1;
        value := Dout;
    end;

    // comando para ler os estados dos reles
    cmdGET_RELAY: begin
        addrInit := $300;
        nRegs:= 1;
    end;

    // comando para gravar os estados dos reles
    cmdSET_RELAY: begin
        typeCMD := writeREG;
        addrInit := $300;
        nRegs:= 1;
        value := Relay;
    end;

    // Comando para ler os valores dos sensores
    cmdGET_MULTIMETERS: begin;
        addrInit := $400;
        nRegs:= nMultimetersGeren*4;
    end;
    end;

    // ENVIA O COMANDO AO MULTIMETRO
    // -----------------------------------------------------------------------------------------------------------------
    if typeCMD = writeREG then begin
        if FSaveLog then writeLn(flog, format('Enviando comando de escrita REG addr 0x%x = valor',[addrInit, value]));
        result:= FModbus.Write_Register(Fid, addrInit, value);
    end else if typeCMD = writeREGS then begin
        if FSaveLog then writeLn(flog, format('Enviando comando de escrita REGS addr 0x%x len %d',[addrInit, nRegs]));
        result:= FModbus.Write_Registers(Fid, addrInit, nRegs, @FRegs);
    end else begin
        if FSaveLog then writeLn(flog, format('Enviando comando de leitura REGS addr 0x%x len %d',[addrInit, nRegs]));
        result:= FModbus.Read_Registers(Fid, addrInit, nRegs, @FRegs);
    end;

    if (result < 0) then begin
        if FSaveLog then writeLn(flog, format('    Erro %d',[result]));
        ErrorCom := result;
        Busy := FALSE;
        exit;
    end;
    ErrorCom := 0;

    if FSaveLog then begin
        writeLn(flog, format('    RX %d regs',[nRegs]));
        for x := 0 to nRegs-1 do begin
            writeLn(flog, format('        0x%x',[FRegs[x]]));
        end;
    end;


    // ATUALIZA VAR COM A LEITURA
    // -----------------------------------------------------------------------------------------------------------------

    case cmd of

    // Comando para ler os registradores: modelo, versão firmware e modo de operações dos reles
    cmdGET_INFO: begin;
        Model := chr(FRegs[0] and $ff) + chr((FRegs[0] shr 8)) + chr(FRegs[1] and $ff) + chr((FRegs[1] shr 8));
        Fimware := chr(FRegs[2] and $ff) + '.' + chr((FRegs[2] shr 8));
    end;

    // comando para ler os estados das saidas digitais
    cmdGET_DOUT: begin
        Dout := FRegs[0];
    end;

    // comando para ler os estados dos reles
    cmdGET_RELAY: begin
        Relay := FRegs[0];
    end;

    // Comando para ler os sensores
    cmdGET_MULTIMETERS: begin
       	for x:=0 to nMultimetersGeren-1 do begin
			Multimeter[x].stsCom := FRegs[4*x] and $ff;
			Multimeter[x].func := (FRegs[(4*x)+1] and $10) shr 4;
			Multimeter[x].sts := FRegs[(4*x)+1] and $f;
			Multimeter[x].value := smallint((FRegs[(4*x)+2])) or smallint((FRegs[(4*x)+3] shl 16));
        end;
    end;
    end;

    Busy := FALSE;
end;

end.
