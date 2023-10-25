unit u_crc;

{
Atualizado em 21/03/2015

Autor Diego Fontana

leia-me

	o calculo do CRC eh feito em cima dos polinomios, como por exemplo o
MODBUS, ele utiliza o polinomio "X^16+X^15+X^2+X^0" que em hexa é 0x8005,
as rotinas de calculos de CRC precisam de um valor inicial par ao CRC
é do polinomio, o CRC é calculado em cima de cada BYTE do pacote.

	Existem duas forma de calcular o CRC, uma rotacionando para diretira
e outra para esquerda, aqui temos duas duas rotinas genéricas:
	crc16_Right: calcula o CRC de modo Rigth Shifted
	crc16_Left: calcula o CRC de modo Left Shifted

	Por algum motivo que eu ainda nao sei, cada sistema utiliza sua proprio
combinação de crc16_Right ou crc16_Left, polinomio e valor inicial.

	O detalhe na rotina crc16_Right (Right Shifted) é que o polinomio utilizado
deve estar espelhado, por exemplo o MODBUS utiliza o valor 0x8005, portando
o polinomio real a ser calculado na rotina é 0xA001. Por isso a função crc16_Poly
faz o espelhamento do polinomio, ajustando para o uso correto.

	O calculo do CRC para DNP3 é um pouco mais estranho, se for avaliado
o CRC de byte a byte ele fecha com a tabela encontrada neste site
http://www.experts-exchange.com/Programming/Languages/Visual_Basic/Q_11722859.html
mas o valor final utilizado deve ser aplicado complemento "~" e os BYTES da word
estão com os valores ALTO e BAIXO trocados.

	[REFERENCIAS]

	http://en.wikipedia.org/wiki/Cyclic_redundancy_check
	http://www.lammertbies.nl/comm/info/crc-calculation.html
	http://ghsi.de/CRC/
}
interface
//uses
//    Windows, SysUtils, IniFiles, Graphics, TlHelp32, ShlObj, forms, controls,
//    Dialogs, StdCtrls, MMSystem, DateUtils;

//type
//    tStatus = (cStatusIdle, cStatusWakeUp, cStatusWorking);
//    tMsn = (cMsnOk, cMsnFail);

function crc16_Poly(poly: word):word;
function crc16_Right(data:pbyte; length:integer; poly:word; init:word): word;
function crc16_Left(data:pbyte; length:integer; poly:word; init:word): word;
function crc16_NBR14522(data:pbyte; length:integer):word;
function crc16_MODBUS(data:pbyte; length: integer): word;
function crc16_XMODEM(data:pbyte; length: integer):word;
function crc16_DNP3(data:pbyte; length:integer):word;
function crc7(data:pbyte; length:integer):byte;
function crc8_HEX(data:pbyte; length:integer):byte;

implementation

// cria espelho do polinomio
function crc16_Poly(poly: word):word;
var i,r:word;
begin
    r := 0;
    for i:= 0 to 15 do begin
        r := r shl 1;
        r := r or (poly and 1);
        poly := poly shr 1;
    end;
    result := r;
end;

// 16bits CRC right shifted
// data = buffer de bytes a serem calculados
// length = quantidade de bytes
// poly = polinomio original
// init = valor inicial do crc
function crc16_Right(data:pbyte; length:integer; poly:word; init:word): word;
var i, crc: word;
    p:pbyte;
    x:integer;
begin
	crc := init;
	p := data;
	poly := crc16_Poly(poly);
    for x:= 0 to length-1 do begin
        crc := crc xor p[x];
        for i:= 0 to 7 do begin
            if (crc and $0001 > 0) then
                crc := (crc shr 1) xor poly
            else crc := (crc shr 1);
        end;
    end;

	result:= crc;
end;

// 16bits CRC left shifted
// data = buffer de bytes a serem calculados
// length = quantidade de bytes
// poly = polinomio original
// init = valor inicial do crc
function crc16_Left(data:pbyte; length:integer; poly:word; init:word): word;
var i, crc: word;
    p:pbyte;
    x:integer;
begin
    crc := init;
    p := data;

    for x:= 0 to length-1 do begin
        crc := crc xor (p[x] shl 8);
        for i:= 0 to 7 do begin
            if (crc and $8000 > 0) then
                crc := (crc shl 1) xor poly
            else crc := (crc shl 1);
        end;
    end;

	result:= crc;
end;

//---------------------------------------------------------------------
// CRC: aplicações práticas
//---------------------------------------------------------------------

// TESTAR
//poly = x16 + x15 + x2 + x0 = 0x8005
function crc16_NBR14522(data:pbyte; length:integer):word;
begin
	result:= crc16_Right( data, length, $8005, $0000);
end;


//poly = x16 + x15 + x2 + x0 = 0x8005
function crc16_MODBUS(data:pbyte; length: integer): word;
begin
	result:= crc16_Right(data, length, $8005, $FFFF);
end;

// TESTAR
//poly = x16 + x12 + x5 + x0 = 0x1021
function crc16_XMODEM(data:pbyte; length: integer):word;
begin
	result:= crc16_Left( data, length, $1021, $0000 );
end;


// TESTAR
//poly = x16 + x13 + x12 + x11 + x10 + x8 + x6 + x5 + x2 + x0  = 0x3D65
function crc16_DNP3(data:pbyte; length:integer):word;
var r:word;
begin
	//calcula e depois aplica o complemento
    r:= not crc16_Right(data, length, $3D65, $0000 );
    //troca os bytes
    result:= (r shr 8) or (r shl 8);
end;

// TESTAR
//texe(33F): 350us/100bytes [data 18.01.12]
//texe(32MX): 137us/100bytes [data 24.01.12]
//X^7+X^3+X^0 = 0x09
function crc7(data:pbyte; length:integer):byte;
var i,c, crc :byte;
    p:pbyte;
    x:integer;
begin
	p:= data;
	crc := $00;
    for x:= 0 to length-1 do begin
		c := p[x];
        for i:= 0 to 7 do begin
			crc := crc shl 1;
            if ( (c xor crc) and $80 > 0) then
                crc := crc xor $09;
			c:= c shl 1;
        end;

		crc:= crc and $7F;
    end;

	result:= crc * 2 + 1;
end;


// TESTAR
//padrão de checksum de arquivos tipo "intel hex"
function crc8_HEX(data:pbyte; length:integer):byte;
var r: byte;
    p: pbyte;
    x:integer;
begin
	r:= 0;
	p:= data;
    for x:= 0 to length-1 do begin
        r:= r + p[x];
    end;
	result:=  -r;
end;

end.
