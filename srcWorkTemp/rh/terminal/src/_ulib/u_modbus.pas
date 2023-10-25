unit u_modbus;

// criado em 22/03/2015
// atualizado 22/04/2015

interface

{$IFDEF VER280} // XE7
uses Classes, SysUtils, DateUtils, Vcl.Forms, u_crc;
{$ELSE}
uses Classes, SysUtils, DateUtils, ExtCtrls, Forms, u_crc;
{$ENDIF}

const
    errMODBUS_ILLEGAL_FUNCTION		= -1; // O servidor recebeu uma função que não foi implementada ou não foi habilitada.
    errMODBUS_ILLEGAL_DATA_ADDRESS	= -2; // O servidor precisou acessar um endereço inexistente.
    errMODBUS_ILLEGAL_DATA_VALUE	= -3; // O valor contido no campo de dado não é permitido pelo servidor. Isto indica uma falta de informações na estrutura do campo de dados.
    errMODBUS_SLAVE_DEVICE_FAILURE	= -4; // Um irrecuperável erro ocorreu enquanto o servidor estava tentando executar a ação solicitada.
    errMODBUS_TX = -5;
    errMODBUS_TIMEOUT = -6;
    errMODBUS_BUFFER_OVERFLOW = -7;
    errMODBUS_CRC = -8;
    errMODBUS_NORESPONSE = -9;
    errMODBUS_ADDR = -10;
    errMODBUS_FUNC = - 11;
    errMODBUS_LEN = -12;
    errMODBUS_VALUE = -13;
    errMODBUS_OPEN_UART = -14;
    errMODBUS_BUSY = -15;

type
    tPortOut    = function(const buf: array of byte; n: integer): integer of object;
    tPortIn     = function(var c: byte): boolean of object;
    tPurge      = procedure of object;

    tModBus = class
    private
        flog:textfile;
        FSaveLog: boolean;

        FPortOut: tPortOut;
        FPortIn: tPortIn;
        FPurge: tPurge;
        FQuerieOut: array [0..255] of byte;
        FQuerieIn: array [0..255] of byte;

        FTimeOut:integer; // timeout em segundos na espera da resposta do esctavo
        function GetPacket:integer;
    public
        constructor Create(PortOut: tPortOut; PortIn:tPortIn; Purge:tPurge);
        destructor Destroy; override;

        property Timeout:integer read FTimeOut write FTimeOut;
        property SaveLog: boolean read FSaveLog write FSaveLog;

        function ValidatePacket: integer;
        function Read_Registers(AddrSlave:integer; AddrInit: integer; len:integer; regs:pword):integer;
        function Write_Register(AddrSlave:integer; Addr: integer; Value:word):integer;
        function Write_Registers(AddrSlave:integer; AddrInit: integer; len:integer; regs:pword):integer;
    end;

implementation

// #####################################################################################################################
// SYSTEM
// #####################################################################################################################
constructor tModBus.Create(PortOut: tPortOut; PortIn:tPortIn; Purge:tPurge);
var flogName:string;
begin
    flogName:= 'modbus.log';
    assignfile(flog, flogName);
    Rewrite(flog);

    FPortOut:= PortOut;
    FPortIn:= PortIn;
    FPurge:= Purge;
    FTimeOut := 1;
    FSaveLog := FALSE;
end;

destructor tModBus.Destroy;
begin
    closefile(fLog);
    // CHAMA A FUNÇÃO DESTROY PAI
    inherited; // Destroi esta lista (tList)
end;


// #####################################################################################################################
// AUX
// #####################################################################################################################

// ---------------------------------------------------------------------------------------------------------------------
// Preenche o buffer FQuerieIn com os byte s enviados pelo escravo
// A função fica esperando um tempo (timeout) pela chegada do primeiro byte,
//      caso não vier esse byte, a função retorna com o mensagem de erro de timeout.
//  Uma vez chegado o primeiro byte a função fica até não ter mais byte no buffer serial,
//  sinalizando fim doo pacote, ou quando estourar o buffer FQuerieIn.
//  A função retorna com o buffer preenchido e com a quantidade de bytes lidos da serial.
// ---------------------------------------------------------------------------------------------------------------------
function tModbus.GetPacket:integer;
var len: integer;
    dat: byte;
    firstByte:boolean;
    TOut: double;
    timeDataIn:double; // conta quanto tempo o dado é recebido do escravo. se o bus ficar em silencio mais que 10ms é porque o escravo terminou a sua transmissão
    crc, crc_calc: word;
begin
    len:=0;
    firstByte:= TRUE;
    TOut:= time;
    timeDataIn:= time;
    if FSaveLog then writeLn(flog, 'Wait data of the slave');

    while(TRUE) do begin
        if (FPortIn(dat) = TRUE) then begin         // Checa se recebeu dados
            firstByte := FALSE;                     // Sinaliza que não é o mais o primeiro byte

            if (len >= 256) then begin
                if FSaveLog then writeLn(flog, '    BUF OVERFLOW');
                exit(errMODBUS_BUFFER_OVERFLOW);
            end;

            if FSaveLog then writeLn(flog, format('    [%d] rx 0x%x',[len, dat]));
            FQuerieIn[len] := dat;                  // Adiciona o dado no buffer
            inc(len);                               // Aponta para o próximo indice do buffer
            timeDataIn := time;			// zera o tempo de espera de recebiemntos de dados do escravo

     	// se não há mais bytes no buffer serial em um determinado tempo é porque é fim de transmissão
 	    	// valor 10 ms funciona bem entre 2400 a 115200 bps. Valor 5ms funcionou bem com 57600 e 115200
            // para boudrate menores pode ser que devemos aumetar esse valor.
            // Recomendo fazer uma macro associado ao boudrate da serial
        end else if ( (time - timeDataIn)*100000000 > 10) then begin
            if (not firstByte) then begin
                if (len < 3) then begin
                    if FSaveLog then writeLn(flog, '    NO RESPONSE');
                    exit(errMODBUS_NORESPONSE);
                end;

                // Vamos pegar o pacote deste buffer e calcular e verificar a legitimidade
                // Calcular CRC do pacote e comparar
                crc_calc := crc16_MODBUS(@FQuerieIn[0], len-2);
                crc := (FQuerieIn[len-1] shl 8 ) or FQuerieIn[len-2];
                if FSaveLog then writeLn(flog, format('    [crc msg 0x%x] [crc calc 0x%x]',[crc, crc_calc]));
                if (crc <> crc_calc) then begin
                     if FSaveLog then writeLn(flog, '    ERROR CRC');
                     exit(errMODBUS_CRC)
                end else exit(len);

            // se ainda não recebemos o primeiro byte após um tempo vamos cancelar
            end else if ( (time > TOut + FTimeOut/100000)) then begin
                if FSaveLog then writeLn(flog, '    TIMEOUT');
                exit(errMODBUS_TIMEOUT);
            end;
        end;

        // Checa a lista de mensagens de outros eventos enviados pelo windowns para a aplicação.
        // CUIDADO com a reetrancia, ou seja, desabilite o evento que chamou esse recurso para não ser
        // chamado denovo
        application.ProcessMessages;
    end;
end;


// ---------------------------------------------------------------------------------------------------------------------
// Checa se o pacote recebido do escravo é a solicitação atual do mestre.
// Caso seja verificar se é ou não uma mensagem de exceção
// ---------------------------------------------------------------------------------------------------------------------
function tModBus.ValidatePacket: integer;
var exception: integer;
begin
    // checar se o ID do escravo é o mesmo enviado
    if (FQuerieIn[0] <> FQuerieOut[0]) then begin
        if FSaveLog then writeLn(flog, '    err ADDR');
        exit(errMODBUS_ADDR);
    end;

    // checar se a função é a mesmo enviado
    if ((FQuerieIn[1] and $7f) <> FQuerieOut[1]) then begin
        if FSaveLog then writeLn(flog, '    err FUNC');
        exit(errMODBUS_FUNC);
    end;

    // checa se o escravo mandou algum erro de exceção
    if (FQuerieIn[1] and $80 > 0) then begin
        exception := FQuerieIn[2];
        exception := not exception + 1;

        if FSaveLog then writeLn(flog, format('    exception %d',[exception]));
        exit(exception);
    end;

    result:= 0;
end;

// #####################################################################################################################
// FUNCTIONS
// #####################################################################################################################

// ---------------------------------------------------------------------------------------------------------------------
// Função: Read_Registers;
// Descrição: Faz uma leitura nos N registradores de 16 bits a partir de um endereço inicial no escravo
// Parametro AddrSlave: Endereço do dispositivo escravo alvo da leitura
// Parametro AddrInit: Endereço do primeiro registrador a ser lido
// Parametro len: Quantidade de registradores de 16 bits a serem lidos
// Parametro regs: Ponteiro do buffer de 16 bits onde vai ser gravado os valores lidos (jÁ FAZ DESLOCAMENTO ADDR)
// Retorna: Quantidade de registradores lidos caso a comunicação foi feito com sucesso
//          Senão retorna o valor do erro, consultar const acima
// ---------------------------------------------------------------------------------------------------------------------
//
// Exemplo :
//        procedure TfrmMain.btnReadClick(Sender: TObject);
//        var sts:integer;
//            id, addrInit, nRegs:integer;
//        begin
//            btnRead.Enabled := FALSE;
//            id:= StrToIntDef(edtID.Text, 1);
//            addrInit:= StrToIntDef(edtAddrInit.Text, 0);
//            nRegs:= StrToIntDef(edtQtd.Text, 0);
//
//            sts:= modbus.Read_Registers(id, addrInit, nRegs, @regs);
//            btnRead.Enabled := TRUE;
//            if (sts < 0) then begin
//                modbusError(sts);
//                exit;
//            end;
//
//            report(clBlue, 'Registers read = %d',[sts]);
//            plotRegisters;
//        end;
// ---------------------------------------------------------------------------------------------------------------------
function tModBus.Read_Registers(AddrSlave:integer; AddrInit: integer; len:integer; regs:pword):integer;
var crc: word;
    ret:integer;
    x:integer;
    countBytes:integer;

begin
    if FSaveLog then writeLn(flog, format('Read Regs [Device=ID %d] [addr init=0x%x] [len=%d]',[AddrSlave, AddrInit, len]));
    FPurge; // limpa os byffers TX e RX da serial

    // preparar a query
    FQuerieOut[0] := byte(AddrSlave);
    FQuerieOut[1] := 3;
    FQuerieOut[2] := byte((AddrInit shr 8) and $ff);
    FQuerieOut[3] := byte(AddrInit and $ff);
    FQuerieOut[4] := byte((len shr 8) and $ff);
    FQuerieOut[5] := byte(len and $ff);
    crc := crc16_MODBUS(@FQuerieOut[0], 6);
    FQuerieOut[6] := crc and $ff;
    FQuerieOut[7] := (crc shr 8) and $ff;

    // enviar a query para o escravo
    if (FPortOut(FQuerieOut, 8) <> 8) then begin
        if FSaveLog then writeLn(flog, '    err TX');
        exit(errMODBUS_TX);
    end;

    // esperar pela resposta do escravo
    ret := GetPacket;
    if (ret < 0 ) then exit(ret);

    // Checa se este pacote é mesmo do escravo solicitado
    ret := ValidatePacket;
    if (ret < 0 ) then exit(ret);

    // captura a quantidade de bytes recebidos
    countBytes := FQuerieIn[2];
    if (2*len <> countBytes) then begin
        if FSaveLog then writeLn(flog, format('    err len bytes rx %d <> %d',[countBytes, FQuerieIn[2]]));
        exit(errMODBUS_LEN);
    end;

    // Tirar os valores dos registradores do bufferin para o buffer da aplicação
    if FSaveLog then writeLn(flog, format('    read %d bytes',[countBytes]));

    for x:= 0 to len-1 do begin
        if FSaveLog then writeLn(flog, format('    addr 0x%x = 0x%x',[integer(regs), (FQuerieIn[2*x+3] shl 8) or (FQuerieIn[2*x+4])]));
        regs^:=(FQuerieIn[2*x+3] shl 8) or (FQuerieIn[2*x+4]);
        inc(regs);
    end;

    result:= len;
end;

// ---------------------------------------------------------------------------------------------------------------------
// Função: Write_Register;
// Descrição: Grava um registrador de 16 bits no escravo
// Parametro AddrSlave: Endereço do dispositivo escravo alvo da escrita
// Parametro Addr: Endereço do registrador a ser gravado
// Parametro Value: Valor a ser gravado no registradores de 16 bits
// Retorna: Retorna valor 1 caso a escrita foi feito com sucesso
//          Senão retorna o valor do erro, consultar const acima
// ---------------------------------------------------------------------------------------------------------------------
//
// Exemplo:
//        procedure TfrmMain.btnWriteClick(Sender: TObject);
//        var sts:integer;
//            id, addr:integer;
//            value:word;
//        begin
//            btnWrite.Enabled := FALSE;
//            id:= StrToIntDef(edtID.Text, 1);
//            addr:= StrToIntDef(edtAddr.Text, 0);
//            value:= StrToIntDef(edtValue.Text, 0);
//
//            sts:= modbus.Write_Register(id, addr, value);
//            btnWrite.Enabled := TRUE;
//            if (sts < 0) then begin
//                modbusError(sts);
//                exit;
//            end;
//
//            report(clBlue, 'Write Register OK');
//        end;
// ---------------------------------------------------------------------------------------------------------------------
function tModBus.Write_Register(AddrSlave:integer; Addr: integer; Value:word):integer;
var crc: word;
    ret:integer;
    AddrComp: integer;
    ValueComp:word;

begin
    if FSaveLog then writeLn(flog, format('Write Reg [Device=ID %d] [addr init=0x%x] [value=%d]',[AddrSlave, Addr, value]));
    FPurge; // limpa os byffers TX e RX da serial

    // preparar a query
    FQuerieOut[0] := byte(AddrSlave);
    FQuerieOut[1] := 6;
    FQuerieOut[2] := byte((Addr shr 8) and $ff);
    FQuerieOut[3] := byte(Addr and $ff);
    FQuerieOut[4] := byte((value shr 8) and $ff);
    FQuerieOut[5] := byte(value and $ff);
    crc := crc16_MODBUS(@FQuerieOut[0], 6);
    FQuerieOut[6] := crc and $ff;
    FQuerieOut[7] := (crc shr 8) and $ff;

    // enviar a query para o escravo
    if (FPortOut(FQuerieOut, 8) <> 8) then begin
        if FSaveLog then writeLn(flog, '    err TX');
        exit(errMODBUS_TX);
    end;

    // esperar pela resposta do escravo
    ret := GetPacket;
    if (ret < 0 ) then exit(ret);

    // Checa se este pacote é mesmo do escravo solicitado
    ret := ValidatePacket;
    if (ret < 0 ) then exit(ret);

    // compara endereço do registrador
    AddrComp := (FQuerieIn[2] shl 8) or (FQuerieIn[3]);
    if (Addr <> AddrComp) then begin
        if FSaveLog then writeLn(flog, format('    err addr %d <> %d',[Addr, AddrComp]));
        exit(errMODBUS_ADDR);
    end;

    // compara valor do registrador
    ValueComp := (FQuerieIn[4] shl 8) or (FQuerieIn[5]);
    if (Value <> ValueComp) then begin
        if FSaveLog then writeLn(flog, format('    err value %d <> %d',[Value, ValueComp]));
        exit(errMODBUS_VALUE);
    end;

    result:= 1;
end;


// ---------------------------------------------------------------------------------------------------------------------
// Função: Write_Registers;
// Descrição: Faz escritas nos registradores de 16 bits a partir de um endereço inicial no escravo
// Parametro AddrSlave: Endereço do dispositivo escravo alvo de escrita
// Parametro AddrInit: Endereço do primeiro registrador a ser gravado
// Parametro len: Quantidade de registradores de 16 bits a serem gravados
// Parametro regs: Ponteiro do buffer de 16 bits contendo valores a serem gravados no escravos (NÃO FAZ DESLOCAMENTO ADDR)
// Retorna: Retorna valor 1 caso a escrita foi feito com sucesso
//          Senão retorna o valor do erro, consultar const acima
// ---------------------------------------------------------------------------------------------------------------------

// Exemplo 1:
//      sts:= modbus.Write_Registers(id, AddrInit, len, @r[offset]);
// Exemplo 2:
//      procedure TfrmMain.btnWriteRegsClick(Sender: TObject);
//      var sts:integer;
//          x:integer;
//          id:integer;
//          r: array [0..3] of word;
//
//      begin
//          btnWriteRegs.Enabled := FALSE;
//
//          for x:= 0 to 3 do begin
//              r[x] := StrToIntDef(stgRegs.Cells[1,1+x],0);
//              stgRegs.Cells[1,1+x] := format('0x%x',[r[x]]);;
//          end;
//
//          id:= StrToIntDef(edtID.Text, 1);
//          sts:= modbus.Write_Registers(id, 0, 4, @r);
//          btnWriteRegs.Enabled := TRUE;
//
//          if (sts < 0) then begin
//              modbusError(sts);
//              exit;
//          end;
//
//          report(clBlue, 'Write Registers read');
//      end;

// ---------------------------------------------------------------------------------------------------------------------
function tModBus.Write_Registers(AddrSlave:integer; AddrInit: integer; len:integer; regs:pword):integer;
var crc: word;
    ret:integer;
    x:integer;
    cmp:integer;

begin
    if FSaveLog then writeLn(flog, format('Write Regs [Device=ID %d] [addr init=0x%x] [len=%d]',[AddrSlave, AddrInit, len]));
    FPurge; // limpa os byffers TX e RX da serial

    // preparar a query
    FQuerieOut[0] := byte(AddrSlave);
    FQuerieOut[1] := 16;
    FQuerieOut[2] := byte((AddrInit shr 8) and $ff);
    FQuerieOut[3] := byte(AddrInit and $ff);
    FQuerieOut[4] := byte((len shr 8) and $ff);
    FQuerieOut[5] := byte(len and $ff);
    FQuerieOut[6] := 2*len;

    for x:=0 to len-1 do begin
        FQuerieOut[7+2*x] := byte(regs^ shr 8);
        FQuerieOut[8+2*x] := byte(regs^ and $ff);
        inc(regs);
    end;

    crc := crc16_MODBUS(@FQuerieOut[0], 7+(2*len));
    FQuerieOut[7+2*len] := crc and $ff;
    FQuerieOut[8+2*len] := (crc shr 8) and $ff;

    // enviar a query para o escravo
    if (FPortOut(FQuerieOut, 9+2*len) <> 9+2*len) then begin
        if FSaveLog then writeLn(flog, '    err TX');
        exit(errMODBUS_TX);
    end;

    // esperar pela resposta do escravo
    ret := GetPacket;
    if (ret < 0 ) then exit(ret);

    // Checa se este pacote é mesmo do escravo solicitado
    ret := ValidatePacket;
    if (ret < 0 ) then exit(ret);

    // compara endereço do registrador
    cmp := (FQuerieIn[2] shl 8) or (FQuerieIn[3]);
    if (AddrInit <> cmp) then begin
        if FSaveLog then writeLn(flog, format('    err addr %d <> %d',[AddrInit, cmp]));
        exit(errMODBUS_ADDR);
    end;

    // compara quantidade
    cmp := (FQuerieIn[4] shl 8) or (FQuerieIn[5]);
    if (len <> cmp) then begin
        if FSaveLog then writeLn(flog, format('    err qtd %d <> %d',[len, cmp]));
        exit(errMODBUS_VALUE);
    end;

    result:= 1;
end;

end.
