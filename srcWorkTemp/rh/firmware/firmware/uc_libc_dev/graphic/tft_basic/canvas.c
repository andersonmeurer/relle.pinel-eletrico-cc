#include "canvas.h"
#include "tft.h"
#include "string.h"

//	static u8 _aVRAM[0x100000]  __attribute__ ((section(".VRAM"))) = { 0 };
//	#define VRAM_ADDR_PHY	(u32)&_aVRAM[0]
//	#define VRAM_SIZE 	0x100000

#if CANVAS_USE_DEBUG == pdON
#include "stdio_uc.h"
#endif

#if CANVAS_USE_TESTBENCH == pdON
extern tTime now (void);
#endif


// Determina em que sentido será exibido o gráfico
#if TFT_USE_ROTATE == TFT_ROTATE_NONE
	#define GETPIXEL(x,y)			canvas.frame[(y)*TFT_H_SIZE + x] 		// Não rotacionar TFT
#else
	#define GETPIXEL(x,y)			canvas.frame[(TFT_V_SIZE - 1 - (y))*TFT_H_SIZE + TFT_H_SIZE - 1 - (x)] 	// Rotacionar TFT 180º deixando a tela de cabeça para baixo;
#endif
	//#define GETPIXEL(x,y)			canvas.frame[(x)*800 + 799 - (y)] 			// Rotacionar TFT no sentido horário (90º);
	#define PUTPIXEL(x,y,c) 		GETPIXEL(x,y) = c
	#define PUTPIXELB(x,y) 			GETPIXEL(x,y) = canvas.brush.color
	#define PUTPIXELP(x,y)	 		GETPIXEL(x,y) = canvas.pen.color

tCanvas canvas; //canvas global

static pchar getcharmap(char c);


// ######################################################################################
// FUNÇÕES DE CONTROLE
// ######################################################################################
tRect asRect(int x, int y, int w, int h) {
	tRect t;
	t.x = x;
	t.y = y;
	t.w = w;
	t.h = h;
	return t;
}

tPoint asPoint(int x, int y) {
	tPoint t;
	t.x = x;
	t.y = y;
	return t;
}

// Cria o canvas para trabalho de acordo com a resolução do TFT
void canvas_Create(void) {
	canvas.w = TFT_H_SIZE;
	canvas.h = TFT_V_SIZE;
	canvas.bkcolor = clWhite;
	canvas.pen.width = 1;
	canvas.pen.color = clBlack;
	canvas.brush.color = clWhite;
	canvas.brush.style = bsClear;
	canvas.font.color = clBlack;
	canvas.font.align = taLeftJustify;
	canvas_SetFrame(0); // Ajusatr o buffer de video para endereço 0 de memória de video na SDRAM
	canvas_SetFont(0, 10);
	canvas_Clear();
}

// ajusta a cor de fundo do display
void canvas_SetBackColor(tColor bkColor) {
	canvas.bkcolor = bkColor;
	canvas_Clear();
}

tColor canvas_GetBackColor(void) {
	return canvas.bkcolor;
}

// Há 4 frames de video na SDRAM que é apontado pelo n

// captura o endereço memória de video do frame apontado
u32 canvas_GetFrame(int f) {
	return (VRAM_ADDR + VRAM_SIZE*f);
}


// Determina quais dos frames na memória de video será será usada pelo canvas
void canvas_SetFrame(int f) {
	canvas.frame = (u16 *)(VRAM_ADDR + VRAM_SIZE*f);
	canvas.frameid = f;
	tft_Frame((u32)(canvas.frame));
}

// Salva o canvas na SDRAM posição do frame 2
void canvas_SaveFrame(void) {
	u32 p = canvas_GetFrame(2);
	memcpy((void*)(p), canvas.frame, canvas.w*canvas.h*2);
}

// Restaura o canvas da SDRAM posição do frame 2
void canvas_LoadFrame(void) {
	u32 p = canvas_GetFrame(2);
	memcpy(canvas.frame,(void*)(p),canvas.w*canvas.h*2);
}


// ######################################################################################
// FUNÇÕES DE PINTAR
// ######################################################################################
// preenche toda tela com uma cor de fundo
void canvas_Clear(void) {
	canvas.brush.color = canvas.bkcolor;
	canvas_FillRect(0, 0, canvas.w-1, canvas.h-1);
}

// rotaciona a tela n linhas
void canvas_Scrool(int n) {
	int x,y;
	for (x=0;x<=canvas.w;x++)
	for (y=0;y<=canvas.h-n;y++)
		GETPIXEL(x,y) = GETPIXEL(x,y+n);
}

// Preenche um retângulo com cor de fundo
// A cor de fundo definido pelo canvas.brush.color
void canvas_FillRect(int xi, int yi, int xf, int yf) {
	int x,y;
	if (canvas.brush.color == clNone) return;
	for (y=yi;y<=yf;y++)
	for (x=xi;x<=xf;x++)
		PUTPIXELB(x,y);
}

// Preenche um retângulo com cor de fundo canvas.brush.color
// As coordenadas são as posições centrais do retângulo
// A cor de fundo definido pelo canvas.brush.color
void canvas_FillRectX(tRectC r) {
	canvas_FillRect(RECTX_XI(r), RECTX_YI(r), RECTX_XF(r) - 1, RECTX_YF(r) - 1);
}

// Preenche uma região com cor de fundo
// A cor de fundo definido pelo canvas.brush.color
void canvas_FillRectColor(int xi, int yi, int xf, int yf, tColor c) {
	int x,y;
	for (y=yi;y<=yf;y++)
	for (x=xi;x<=xf;x++)
		PUTPIXEL(x,y,c);
}


// Pinta um pixel na posição X,Y da tela;
// O tamanho do pixel é definido pelo canvas.pen.width
// A cor do pixel é definido pelo canvas.pen.color
void canvas_PutPixel(int x, int y) {
	#if TFT_USE_CURSOR > 0
	// dentro da tela
	if (!canvas_Over(x,y)) return;
	#endif

	// tamanho da caneta maior que 1?
	if (canvas.pen.width > 1) {
		int w = canvas.pen.width;
		canvas_FillRectColor(x-w/2,y-w/2,x+w/2,y+w/2, canvas.pen.color);
		return;
	}
	//nao foi especificado um tamanho valido
	PUTPIXELP(x,y);
}


// Traça uma linha na tela;
// A cor do pixel é definido pelo canvas.pen.color
void canvas_Line(int x0, int y0, int x1, int y1) {
	int dx,dy,a,ix,iy;
	long s;

	if (x0>x1) { ix = -1; dx = x0-x1; } else { ix = 1; dx = x1-x0; }
	if (y0>y1) { iy = -1; dy = y0-y1; } else { iy = 1; dy = y1-y0; }

	//diagonal
	if (dx==dy)	{
		while (dx--) { PUTPIXELP(x0,y0); x0 += ix; y0 += iy; }
		return;
	}

	//horizontal
	if (dy==0) {
		while (dx--) { PUTPIXELP(x0,y0); x0 += ix; }
		return;
	}

	//vertical
	if (dx==0) {
		while (dy--) { PUTPIXELP(x0,y0); y0 += iy; }
		return;
	}

	//outro
  	s = 32768;
  	if (dx>dy)  {
	  	dx++;
    	a = (long)(dy*65536L) / dx;
    	while (dx--) {
      		PUTPIXELP(x0,y0);
      		x0 += ix;
      		s += a;
      		if (s>=65536) { y0 += iy; s -= 65536; }
    	}
  	} else {
	  	dy++;
    	a = (long)(dx*65536L) / dy;
    	while (dy--) {
      		PUTPIXELP(x0,y0);
      		y0 += iy;
      		s += a;
      		if (s>=65536) { x0 += ix; s -= 65536; }
    	}
	}
}

// Traça uma linha na tela. Ideal para criar linhas subsequentes
// Iniciar a posição da caneta (canvas.pen.pos.x e canvas.pen.pos.y) com MoveTo
// A cor do pixel é definido pelo canvas.pen.color
void canvas_LineTo(int x, int y) {
	//faz a linha
	canvas_Line(canvas.pen.pos.x, canvas.pen.pos.y, x, y);
	// posiciona a caneta
	canvas_MoveTo(x,y);
}

// Coloca a caneta na posição desejada
void canvas_MoveTo(int x, int y) {
	canvas.pen.pos.x = x;
	canvas.pen.pos.y = y;
}


// Criar uma cruz na tela com posição central X,Y
// A cor do pixel é definido pelo canvas.pen.color
void canvas_Cross(int x, int y, int size) {
	canvas_Line(x, y-size/2, x, y+size/2);
	canvas_Line(x-size/2, y, x+size/2, y);
}

// Cria um retangulo
// A cor do pixel é definido pelo canvas.pen.color
void canvas_Rect(int xi, int yi, int xf, int yf) {
	canvas_MoveTo(xi,yi);
	canvas_LineTo(xf,yi);
	canvas_LineTo(xf,yf);
	canvas_LineTo(xi,yf);
	canvas_LineTo(xi,yi);
}

// Cria um retangulo utilizando o tipo tRectC
// A cor do pixel é definido pelo canvas.pen.color
void canvas_Rect2(tRectC r) {
	canvas_Rect(r.cx, r.cy, RECT_XF(r) - 1, RECT_YF(r) - 1);
}


// Cria um retangulo pelas dimensões r. Porém o retangulo será impresso pelo centro do mesmo
// A cor do pixel é definido pelo canvas.pen.color
void canvas_RectX(tRectC r) {
	canvas_Rect(RECTX_XI(r), RECTX_YI(r), RECTX_XF(r) - 1, RECTX_YF(r) - 1);
}

// ###############################################################################
// FUNÇÕES DE CARACTERES
// ###############################################################################
// TODO - Definir o local do aplicativo abaixo
//geração das fontes: "H:\Projetos\PR212 - CPU Grafica\pr212v1 - pic\editor\exe"
#include "fontmap1.h"
#include "fontmap2.h"

#define FONT_LIST_SIZE	2 // define a quantiade de fontes a ser usadas no sistema

pFontBin fontlist[FONT_LIST_SIZE] = {
	&fontbin1,
	&fontbin2,
};

static pchar getcharmap(char c) {
	pFontBin f = fontlist[canvas.font.size];
	return &f->pfont[(u8)(c-f->first) * f->bpc];
}

void canvas_SetFont(int size, int fake) {
	//limites
	if (size<0) size=0;
	if (size>=FONT_LIST_SIZE) size = FONT_LIST_SIZE-1;
	//atributos
	canvas.font.fake = fake;
	canvas.font.size = size;
	canvas.font.w = fontlist[size]->width;
	canvas.font.h = fontlist[size]->height+canvas.font.fake*2;
}


// Imprime um caractere C na posição X,Y da tela
// A cor do pixel é definido pelo canvas.pen.color
void canvas_PutC(int x, int y, int c) {
	int i,j,k,map;
	tColor cl;

	//dentro da tela?
	#if TFT_USE_CURSOR > 0
	if (!canvas_Over(x,y)) return;
	#endif
	//espaço?
	if (c==32) {
		canvas_FillRect(x,y,x+canvas.font.w-1,y+canvas.font.h-1);
		return;
	}

	//dentro do range?
	if (((u8)(c)<fontlist[canvas.font.size]->first)||(c>fontlist[canvas.font.size]->last)) {
		c = '?';
	}

	//bitmap da letra
	pchar s = getcharmap(c);

	//imprime a letra
	//esta levando aproximadamente 75us para gerar o caractere
	if (canvas.font.fake) {
		canvas_FillRect(x,y,x+canvas.font.w-1,y+canvas.font.fake-1);
		y = y + canvas.font.fake;
	}
	for (i=0;i<canvas.font.h-canvas.font.fake*2;i++) {
		map = *s++;
		k = 0;
		for (j=0;j<canvas.font.w;j++) {
			if (k++>7) { k=0; map = *s++; }
			cl = (map&0x80)?canvas.pen.color:canvas.brush.color;
			if (cl!=clNone) PUTPIXEL(x+j, y+i, cl);
			map = map<<1;
		}
	}
	if (canvas.font.fake) {
		y = y + canvas.font.h - canvas.font.fake*2;
		canvas_FillRect(x,y,x+canvas.font.w-1,y+canvas.font.fake-1);
	}
}

// Imprime uma string TEXT na posição X,Y da tela
// A cor do pixel é definido pelo canvas.pen.color
// canvas.font.align==taCenter
void canvas_TextOut(int x, int y, cpchar text) {
	int k = strlen(text);

	//centralizar?
	if (canvas.font.align==taCenter) {
		x = x - (canvas.font.w*k)/2;
		y = y - canvas.font.h/2;
	}

	//imprime o texto
	while (k--) {
		canvas_PutC(x,y,*text++);
		x += canvas.font.w;
	}
}

char canvas_Over(int x, int y) {
	if (x<0) return 0;
	if (y<0) return 0;
	if (x>=canvas.w) return 0;
	if (y>=canvas.h) return 0;
	return 1;
}

// result: 1=dentro, 0=fora
char canvas_RectXClick(tRectC r, int x, int y) {
	if (x<(r.cx-r.w/2)) return 0;
	if (x>(r.cx+r.w/2)) return 0;
	if (y<(r.cy-r.h/2)) return 0;
	if (y>(r.cy+r.h/2)) return 0;
	return 1;
}

//char rectxMOUSEOVER(tRectX r, int x, int y) {
//	if (x<RECTX_XI(r)) return 0;
//	if (x>RECTX_XF(r)) return 0;
//	if (y<RECTX_YI(r)) return 0;
//	if (y>RECTX_YF(r)) return 0;
//	return 1;
//}

// ###############################################################################
// FUNÇÕES TESTBENCH
// ###############################################################################
#if CANVAS_USE_TESTBENCH == pdON
void canvas_Fractal_TestBench(void) {
	int xx, yy, i;
	float x0; //scaled x co-ordinate of pixel (must be scaled to lie somewhere in the interval (-2.5 to 1)
	float y0; //scaled y co-ordinate of pixel (must be scaled to lie somewhere in the interval (-1, 1)
	float x, y, xtemp;
	tColor c;
	#if CANVAS_USE_DEBUG == pdON
	plog("TFT: canvas.w = %d "CMD_TERMINATOR, canvas.w);
	plog("TFT: canvas.h = %d "CMD_TERMINATOR, canvas.h);
	#endif
	for (xx = 0; xx < canvas.w; xx++) {
		#if CANVAS_USE_DEBUG == pdON
		plog("TFT: xx = %d"CMD_TERMINATOR, xx);
		#endif

		for (yy = 0; yy < canvas.h; yy++) {
			x0 = -2.5 + (float) xx / 182.8;
			y0 = -2.0 + (float) yy / 120.0;
			x = 0;
			y = 0;
			for (i = 0; i < 15; i++) {
				if ((x * x + y * y) > 4) break;
				xtemp = x * x - y * y + x0;
				y = 2 * x * y + y0;
				x = xtemp;
			}
			if (x * x + y * y < 4) c = clBlack;
			else {
				int r = 0; //yy/1.88;
				int g = i * 25;
				int b = xx / 2.5;
				c = RGB16(r,g,b);
			}
			PUTPIXEL(xx,yy,c);
		}
	}
}

void canvas_PutChars_TestBench(tColor color) {
    int i,k;
    canvas.pen.color = color;

    k = canvas.w/canvas.font.w;
    canvas_Clear();
	#if CANVAS_USE_DEBUG == pdON
    tTime start,end;
    plog("canvas: PutCTB"CMD_TERMINATOR);
    start = now();
    #endif

    for (i=0;i<256;i++) {
        int x = canvas.font.w*(i%k);
        int y = canvas.font.h*(i/k);
        canvas_PutC(x, y, i);
    }

	#if CANVAS_USE_DEBUG == pdON
    end = now();
    plog("canvas: %d ms"CMD_TERMINATOR,end-start);
    plog("canvas: %d char/s"CMD_TERMINATOR,256000/(end-start));
	#endif
}

const tColor clTable[CL_TABLECOUNT] = {
	clBlack,	clMaroon,	clGreen,	clOlive,
	clNavy,		clPurple,	clTeal,		clGray,
	clSilver,	clRed,		clLime,		clYellow,
	clBlue,		clFuchsia,	clAqua,		clLtGray,
	clDarkGray,	clWhite
};

tColor canvas_Degrade(tColor ci, tColor cf, float x);
void canvas_FillRectDeg(int xi, int yi, int xf, int yf, tColor ci, tColor cf);
tColor canvas_Degrade(tColor ci, tColor cf, float x) {
	tRGB565 pi,pf,pr;
	pi.c = ci;
	pf.c = cf;
	pr.r = (int)(pi.r + (pf.r-pi.r)*x);
	pr.g = (int)(pi.g + (pf.g-pi.g)*x);
	pr.b = (int)(pi.b + (pf.b-pi.b)*x);
	return pr.c;
}

// cria uma caixa degrade
void canvas_FillRectDeg(int xi, int yi, int xf, int yf, tColor ci, tColor cf) {
	int i,j = xf-xi;
	for (i=0;i<j;i++) {
		//parte degrade
		canvas.brush.color = canvas_Degrade(ci, cf, (float)i/(float)j);
		canvas_FillRect(i+xi,(yi+yf)/2,i+xi,yf);
		//normal
		canvas.brush.color = (i<j/2)?ci:cf;
		canvas_FillRect(i+xi,yi,i+xi,(yi+yf)/2);
	}
}


void canvas_Pallet_TestBench(void) {
	int i;
	for (i=0;i<CL_TABLECOUNT;i++) {
		canvas_FillRectDeg(
			((i+0)*canvas.w)/CL_TABLECOUNT, 	0,
			((i+1)*canvas.w)/CL_TABLECOUNT,	canvas.h,
			clTable[i],clTable[(i+1)%CL_TABLECOUNT]
			);
	}
}

void canvas_PutPixel_TestBench(void) {
	int x,y;

	#if CANVAS_USE_DEBUG == pdON
	tTime start,end;
	plog("canvas: PutPixelTB"CMD_TERMINATOR);
	start = now();
	#endif

	for (x=0;x<canvas.w;x++)
	for (y=0;y<canvas.h;y++) PUTPIXEL(x,y,x|y);

	#if CANVAS_USE_DEBUG == pdON
	end = now();
	plog("canvas: %d ms"CMD_TERMINATOR,end-start);
	plog("canvas: %d px/ms",(canvas.w*canvas.h)/(end-start));
	#endif
}
#endif

//tRectC asRECTx(int xi, int yi, int xf, int yf) {
//	tRectC r;
//	r.cx = (xi+xf)/2;
//	r.cy = (yi+yf)/2;
//	r.w = xf-xi;
//	r.h = yf-yi;
//	return r;
//}
