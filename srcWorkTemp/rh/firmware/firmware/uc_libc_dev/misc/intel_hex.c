#include "intel_hex.h"
#include "utils.h"

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Interpretador da linha de comandos padrão Intel HEX
// Parametros:	flineHEX: Pointeiro para a linha de comando a ser interpretada, tem que ser
//						finalizada com char nukoi
//						Ex: u8 lineHEX[256] = {":1000100014F09FE50000A0E120F11FE50CF09FE542\0"};
//				flen: Ponteiro para retorno da quantidade de dados contido no buffer caso o comando for do tipo 0
// 				faddrOffSet: ponteiro para retorno do endereço offset da memória para gravação dos dados do buffer
//					caso o comando for do tipo 0
// 				fbuffer:  Ponteiro para o buffer preenchido com os dados a ser gravados na memória
// Retorna: 	Retorna com o tipo de comando da linha que foi interpretada (0 a 5)
//				Ou retorn com o código de erro:
//					errINTEL_HEX_DELIMITER: Não foi encontrado o delimitador de comandos.
//							Arquivo deve estar corrompido ou inválido
//					errINTEL_HEX_CHECKSUM: Erro de checsum da linha de comando Intel HEX
//					errINTEL_HEX_NUMBER_INVALID: O caractere presente na linha de comando não é um digito
//						hexadecimal válido
//------------------------------------------------------------------------------------------------------------------
// O arquivo Intel HEX é um arquivo ASCII composto de linhas de comandos
// A linha de comandos é finalizada pelo caractere feed 0x10 (\n)
// Cada linha contém digitos hexadecimais sempre em letras maiúsculas
// O sentido da leitura da linha é da esquerda para direita represetando os bytes de MSB para LSB
//	O formato da linha de comando é composto dos seguintes campos:
//	:LLAAAATT[DD...]CC
//	:		Delimitador da linha de comando sinaliza uma nova linha de comando.
//	LL 		Sinaliza a quantidade de bytes de dados ou de informação contidas no campo de dados (DD).
//	AAAA	Endereço de 16 bits da memória onde os dados serão gravados.
//	TT		Sinaliza que tipo de comando é a linha:
//			Comando do tipo 0: Gravação de dados;
//			Comando do tipo 1: Fim de arquivo;
//			Comando do tipo 2: Extensão de segmentação endereço (HEX86);
//			Comando do tipo 3:
//			Comando tipo 4: Extensão de endereço linear (HEX386);
//			Comando tipo 5: Endereço linear inicial de gravação.
//	DD		Dados a ser gravado na memória a partir do endereço apontado pelo campo de endereço (AAAA).
//	CC		Checksum da linha de comandos.

// Para mais detalhes consulte o documento D:\meus_conhecimentos\arquivo intel hex
int IntelHexInterpret (
    u8 *flineHEX,     	// Linha de comando
    u8 *flen,           // Retorno da quantidade de dados contido no buffer
    u16 *faddrOffSet,    // Retorno do endereço offset da memória para gravação dos dados do buffer
    u8 *fbuffer 			// Retorno do buffer preenchido com os dados a ser gravados na memória
) {
	u8 xx;
	u8 checksum;
	s16 value;

	// CHECANDO SE O PRIMEIRO CARACTERE É O DELIMINITADOR DE COMANDO
    //  NA LINHA DE COMANDO
    if (flineHEX[0] != ':')  return errINTEL_HEX_DELIMITER;

    // CALCULANDO CHECKSUM
    checksum = 0;
    xx = 1;

    while(flineHEX[xx]) {
       //value := StrToIntDef('$'+copy(lineHEX, xx, 2),-1);
    	value = _hex2byte(&flineHEX[xx]);
    	if (value < 0) return errINTEL_HEX_NUMBER_INVALID;
    	checksum+=(u8)(value);
    	xx+=2;
    }

    if (checksum > 0) return errINTEL_HEX_CHECKSUM;

    // CAPTURA A QUANTIDADE DE DADOS A SER COPIADOS PARA A MEMÓRIA
    // CAPTURA O ENDEREÇO OFFSET DA MEMÓRIA ONDE SERÁ COPIADO OS DADOS

    //len^        := StrToIntDef('$'+copy(lineHEX,2,2),-1); // Copia a quantidade de dados
    //addrOffSet^ := StrToIntDef('$'+copy(lineHEX,4,4),-1); // Copia o endereço offset
    if (  (_hex2byte(&flineHEX[1]) == -1) || (_hex2byte(&flineHEX[3]) == -1) || (_hex2byte(&flineHEX[5]) == -1))  return errINTEL_HEX_NUMBER_INVALID;

    *flen = _hex2byte(&flineHEX[1]);  // Copia a quantidade de dados
    *faddrOffSet = (_hex2byte(&flineHEX[3]) << 8) | (_hex2byte(&flineHEX[5])) ; // Copia o endereço offset

    // CAPTRURA OS DADOS A SER GARVADO NA MEMÓRIA E SALVA EM UM BUFFER
    if (*flen > 0) {
        xx = 0;
        while (xx < *flen) {
        	value = _hex2byte(&flineHEX[9+(xx*2)]); // Copia o dado
            if (value == -1) return errINTEL_HEX_NUMBER_INVALID;
            *fbuffer++ = value;
            xx+= 1;
        }
    }

    // CAPTURA O TIPO DE COMANDO
    if ( _hex2byte(&flineHEX[7]) == -1 )  return errINTEL_HEX_NUMBER_INVALID;
    return _hex2byte(&flineHEX[7]); // Copia o tipo de comando
}

//------------------------------------------------------------------------------------------------------------------
// Descrição: 	Retorna com o valor inteiro de dois caracteres em hexadecimal
// Parametros: 	s: ponteiro com os dois digitos hexadecimail
// Retorna: 	Retorna com o valor decimal do caracteres hexadecimais, caso a converão foi feita com sucesso
//				Retorna com o valor -1 caso os caracteres hexadecimais são inválidos
//------------------------------------------------------------------------------------------------------------------
s16 _hex2byte(u8 *s) {
	s16 a,b;
	if ((a = hex2byte(s[1])) == -1) return -1;
	if ((b = hex2byte(s[0])) == -1) return -1;

	return ( ((u8)b << 4) | (u8)a );
}
