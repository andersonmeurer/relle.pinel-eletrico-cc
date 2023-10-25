// Atualizado 13/03/2015

unit u_uart;

interface

uses
	SysUtils, Windows, Classes, Registry, DateUtils;

//DCB struct
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa363214(v=vs.85).aspx

const
  dcbFMBinary         = $00000001;
  dcbFMParityCheck    = $00000002;
  dcbFMOutxCtsFlow    = $00000004;
  dcbFMOutxDsrFlow    = $00000008;
  dcbFMDtrMask        = $00000030;
  dcbFMDtrDisable     = $00000000;
  dcbFMDtrEnable      = $00000010;
  dcbFMDtrHshake      = $00000020;
  dcbFMDsrSensivity   = $00000040;
  dcbFMTXContOnXoff   = $00000080;
  dcbFMOutXonXoff     = $00000100;
  dcbFMInXonXoff      = $00000200;
  dcbFMErrorChar      = $00000400;
  dcbFMNullStrip      = $00000800;
  dcbFMRtsCtrlMask    = $00003000;
  dcbFMRtsCtrlDisable = $00000000;
  dcbFMRtsCtrlEnable  = $00001000;
  dcbFMRtsCtrlHshake  = $00002000;
  dcbFMRtsCtrlToggle  = $00003000;
  dcbFMAbortOnError   = $00004000;
  dcbFMReserved       = $FFFF8000;

type
  tUART = class
  private
    //buffer é iniciado durante a primeira abertura de porta
    rxbuf,txbuf: array of byte;
    rxcnt,rxpos,txpos: integer;
    const kBUFSIZE = 8*1024;

  public
    comh: THandle;
    coms: DCB; //ver DCB struct
    comt: TCommTimeouts;
    port: string;
    baud: integer;
    Timeout: integer; // timer oout para leitura da porta UART

    constructor Create;
    destructor Destroy; override;

    procedure SetBaud(newbaud: integer);
    function Open(aPort: string = ''): boolean;
    procedure Close;
    function Active: boolean;
    procedure Purge;

    function Write(const buf: array of byte; n: integer): integer;
    function PutC(const c: byte): boolean;
    function PutS(const s: string): integer;
    function Push(c: byte): boolean; overload;
    function Push(const a: array of byte): boolean; overload;
    function Flush: boolean;

    function GetC(var c: byte): boolean;
    function GetS(): string;
    function Read(var buf: array of byte; n: integer): integer;
    function RXda: integer;

    function Exec(cmd: string): string;
  end;

  function uart_GetCommList(list: TStrings): integer; overload;
  function uart_GetCommList(list: TStrings; filter: string; clear:boolean=TRUE): integer; overload;

implementation

{ tUART }

// #############################################################################
// SYSTEM
// #############################################################################
constructor tUART.Create;
begin
    port := 'COM1';
	baud := 9600;
	timeout := 50; // tem 50ms para ler a porta uart para recepção de dados
	rxcnt := 0;
	rxpos := 0;
	txpos := 0;
    comh := 0;
end;

destructor tUART.Destroy;
begin
    Close;
end;

// #############################################################################
// OPEN E CLOSE PORT
// #############################################################################

// -----------------------------------------------------------------------------
// RETORNA COM UMA LISTA PREENCHIDA DE PORTAS SERIAIS DISPONIVEIS
// Parametro 1: Tstring list que será preencida com as portas disponíveis
// retorna com a quatidade de portas disponíveis
// Ex: uart_GetCommList(TComboBox.Items);
// -----------------------------------------------------------------------------
function uart_GetCommList(list: TStrings): integer;
begin
    result := uart_GetCommList(list, '', TRUE);
end;

// -----------------------------------------------------------------------------
// RETORNA COM UMA LISTA PREENCHIDA DE PORTAS SERIAIS DISPONIVEIS
// Parametro 1: Tstring list que será preencida com as portas disponíveis
// Parametro 2: Filtro para adicionar os drivers que contém o nome do filtro
// Parametro 3: Se TRUE limpa a lista TComboBox.Items antes de
// retorna com a quatidade de portas disponíveis
// Ex:
//      uart_GetCommList(TComboBox.Items, 'BthModem');
//          Isto vai adicionar as portas blutetooth que iniciam com
//              BthModem >> BthModem1, BthModem2 ... BthModemX
//      uart_GetCommList(TComboBox.Items, 'VCP', TRUE);
//          Isto vai adicionar as portas FTDI UBB<>SERIAL que iniciam com
//              VCP >> VCP0, VCP1 ... VCPX
// -----------------------------------------------------------------------------
function uart_GetCommList(list: TStrings; filter: string; clear:boolean=TRUE): integer;
var	reg: TRegistry;
	st: Tstrings;
	i: Integer;
begin
    if clear then list.Clear;

    reg := TRegistry.Create(KEY_READ);
	reg.RootKey := HKEY_LOCAL_MACHINE;
    if not reg.OpenKey('hardware\devicemap\serialcomm', FALSE) then begin
        result:= -1;
    end else result := 0;

    if (result = 0) then begin
        st := TstringList.Create;
	    try
        reg.GetValueNames(st);
        if (st.count>0) then begin
    	    for i := 0 to st.Count - 1 do begin
    		    if length(filter) > 0 then
                    if pos(filter, st.strings[i]) <= 0 then continue;
    			    list.Add(reg.Readstring(st.strings[i]));
	    		    inc(result);
		        end;
    	    end;
	    finally
        st.Free;
        end;
    end;

    reg.CloseKey;
    reg.Free;
end;

// -----------------------------------------------------------------------------
// AJUSTAR A VELOCIDADE DE COMUNICAÇÃO
// Parametro 1: valor em baundrate
// Ex:
//      if tUART.SetBaud(9600);
// -----------------------------------------------------------------------------
procedure tUART.SetBaud(newbaud: integer);
begin
	baud := newbaud;
    coms.BaudRate := newbaud;
	SetCommState(comh,coms);
end;

// -----------------------------------------------------------------------------
// ABRE UMA PORTA COM PARA COMUNICAÇÃO
// Parametro 1: nome da porta a ser aberta, se vazio abre a porta padrão
// retorna TRUE se a porta foi abera com sucesso, senão retorna FALSE
// Ex:
//      if tUART.Open('COM22') then
// -----------------------------------------------------------------------------
function tUART.Open(aPort: string = ''): boolean;
begin
	result := FALSE;

    // SE ESPECIFICOU ALGUMA PORTA VAMOS ABRIR NELA
    if (aPort <> '') then port := aPort;

    // SE A PORTA ESTIVER ABERTA VAMOS FECHAR
    if Active then Close;

    // INICIA OS BUFFERS DE TX/RX
    SetLength(rxbuf, kBUFSIZE);
    SetLength(txbuf, kBUFSIZE);

	//com port
	comh := CreateFile(
		@format('\\.\%s',[port])[1],
		GENERIC_READ or GENERIC_WRITE,
		0,
		NIL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
    );

	if comh = MAXDWORD then	begin
        //printf('Failed to open '+port);
        comh := 0;
        SetLength(rxbuf, 0);
        SetLength(txbuf, 0);
		exit;
	end;

	// COM STATUS
	GetCommState(comh, coms);
	coms.BaudRate := baud;
    coms.Flags := dcbFMBinary + dcbFMRtsCtrlDisable; // Pino RTS desligado
	coms.ByteSize := 8;
	SetCommState(comh, coms);

	//com timeouts
	GetCommTimeouts(comh, comt);
	comt.ReadIntervalTimeout := MAXDWORD;
	comt.ReadTotalTimeoutMultiplier := 0;
	comt.ReadTotalTimeoutConstant := 0;
	comt.WriteTotalTimeoutMultiplier := 0;
	comt.WriteTotalTimeoutConstant := 0;
	SetCommTimeouts(comh, comt);

	// TUDO OK
	Purge;

    //	printf('Opened %s @ %d bps',[port,baud]);
	result := TRUE;
end;

// -----------------------------------------------------------------------------
// FECHA A PORTA E LIBERA MEMÓRIA ALOCADA PARA OS BUFFERS
// -----------------------------------------------------------------------------
procedure tUART.Close;
begin
    SetLength(rxbuf, 0);
    SetLength(txbuf, 0);
	FileClose(comh);
    comh := 0;
end;

// -----------------------------------------------------------------------------
// Retorna TRUE se a porta está aberta, senão retorna FALSE
// -----------------------------------------------------------------------------
function tUART.Active: boolean;
begin
    result := (comh <> 0);
end;

// -----------------------------------------------------------------------------
// Descarrega todos os caracteres dos buffers de entrada e saida do recurso de comunicação
// Isto também termina as operações de leitura e escrita pendentes
// -----------------------------------------------------------------------------
procedure tUART.Purge;
begin
	PurgeComm(comh, $F);
	rxcnt := 0;
end;

// #############################################################################
// TRANSMISSÃO
// #############################################################################

// -----------------------------------------------------------------------------
// TRANSMITE UM  BUFFER DE BYTES NA PORTA UART
// Parametro 1: Ponteiro do buffer
// Parametro 2: Tamanho do buffer
// retorna com a quantidade de bytes transmitidos
// -----------------------------------------------------------------------------
function tUART.Write(const buf: array of byte; n: integer): integer;
begin
	Result := FileWrite(comh,buf,n);
end;

// -----------------------------------------------------------------------------
// TRANSMITE UM BYTE NA PORTA UART
// Parametro 1: Byte a ser transmitido
// retorna TRUE se o caractre foi transmitido, senão retorna FALSE
// -----------------------------------------------------------------------------
function tUART.PutC(const c: byte): boolean;
begin
	result := Write(c,1)=1;
end;

// -----------------------------------------------------------------------------
// TRANSMITE UMA STRING NA PORTA UART
// Parametro 1: String a ser transmitida
// retorna com a quantidade de bytes transmitidos
// -----------------------------------------------------------------------------
function tUART.PutS(const s: string): integer;
var buf: array[0..255] of byte;
	i: integer;
begin
	for i:=0 to length(s)-1 do
        buf[i] := BYTE(s[i+1]);

	result := Write(buf, length(s));
end;

// -----------------------------------------------------------------------------
// INSERE STRING NO FIM DA BUFFER DE TRANSMISSÃO
// Parametro 1: String a ser transmitida
// Retorna TRUE se toda a string foi adicionada
// -----------------------------------------------------------------------------
function tUART.Push(const a: array of byte): boolean;
var i: integer;
begin
	result := FALSE;
	for i:=0 to length(a)-1 do
        if not push(a[i]) then exit;
	result := TRUE;
end;

// -----------------------------------------------------------------------------
// INSERE UM CARACTERE NO FIM DA BUFFER DE TRANSMISSÃO
// Parametro 1: String a ser transmitida
// Retorna TRUE se o caractere foi adicionada
// -----------------------------------------------------------------------------
function tUART.Push(c: byte): boolean;
begin
	result := FALSE;
	if (txpos >= high(txbuf)) then exit;
	txbuf[txpos] := c;
	inc(txpos);
	result := TRUE;
end;

// -----------------------------------------------------------------------------
// Força a transmissão dos caracteres restantes no buffer TX
//  retorna TRUE se todos chars foram transmitidos
// -----------------------------------------------------------------------------
function tUART.Flush: boolean;
begin
	result := write(txbuf, txpos) = txpos;
	txpos := 0;
end;

// #############################################################################
// RECEPÇÃO
// #############################################################################

// -----------------------------------------------------------------------------
// LÊ UM CARACTERE DO BUFFER DE RECEPÇÃO
// Parametro: byte com o retorno do caractere
// Retorna TRUE se o caractere foi recuperado, se retornar FALSE o buffer
//              está vazio ou  houve algum erro de leitura
// -----------------------------------------------------------------------------
function tUART.GetC(var c: byte): boolean;
var start: TDateTime;
begin
    result := FALSE;
    if not Active then exit;    // Retorna false se não há conexão com a porta

    c := 0;

    // SE O BUFFER RX ESTIVER VAZIO VAMOS CARREGÁ-LO DA FIFO DE RECEPÇÃO DA UART
    //  Teremos um tempo  (timeout) para que o buffer RX seja carregado
	if (rxcnt <= 0) then begin // Checa se o buffer RX está vazio
		rxpos := 0;
		start := now;
		repeat
            rxcnt := FileRead(comh,rxbuf[0],length(rxbuf));
			if rxcnt < 0 then exit; // erro de leitura? cancela...
            // timeout? cancela...
			if (MilliSecondsBetween(Now, start)>timeout) and (rxcnt=0) then exit;
            // ainda nao tem nada? aguarda um pouco...
            if rxcnt=0 then sleep(1);
            //tem alguma coisa? sai do loop...
		until rxcnt > 0;
	end;

	// recupera o dado do buffer RX
	c := rxbuf[rxpos];
	inc(rxpos);
	dec(rxcnt);
	result := TRUE;
end;

// -----------------------------------------------------------------------------
//  LE UM BUFFER DE BYTES
// parametro 1: Ponteiro do array de bytes
// parametro 2: Tamanho do array
// Retorna a quantidade de bytes carregados para o buffer
// -----------------------------------------------------------------------------
//result: quantidade de bytes lidos
function tUART.Read(var buf: array of byte; n: integer): integer;
var t0: TDateTime;
begin
	result := 0;
    t0 := now;
	while (result < n) do begin
		if not getc(buf[result]) then exit;
		inc(result);
        if MilliSecondsBetween(t0,now)>timeout then exit;
	end;
end;

// -----------------------------------------------------------------------------
// LÊ UMA STRING DA PORTA UART ATÉ QUE RECEBA O CARACTERE \R
// -----------------------------------------------------------------------------
function tUART.GetS(): string;
var c: byte;
begin
	result := '';
	while (getc(c)) do begin
		result := result + char(c);
		if (c=13) then exit;
	end;
end;

// -----------------------------------------------------------------------------
// RETORNA QUANTOS BYTES ESTAO DISPONIVEIS PARA LEITURA
// -----------------------------------------------------------------------------
function tUART.RXda: integer;
var lpErrors: DWORD;
	lpStat: TComStat;
begin
	result := 0;
	if not ClearCommError(comh,lpErrors,@lpStat) then exit;
	result := lpStat.cbInQue;
end;

// #############################################################################
// AUX
// #############################################################################

// -----------------------------------------------------------------------------
// TRANSMITE UMA STRING NA PORTA UART FINALIZANDO COM \r
//      executa uma especie de comando no prompt com finalizador '\r'
//      esta é uma função simplificada e de uso restrito! Para funções mais
//      elaboradas escreva seu proprio driver
// Parametro 1: String a ser transmitida
// Retorna com buffer de recepção
// -----------------------------------------------------------------------------
function tUART.Exec(cmd: string): string;
begin
    if cmd <> '' then puts(cmd+#13);
    result := gets;
end;

end.
