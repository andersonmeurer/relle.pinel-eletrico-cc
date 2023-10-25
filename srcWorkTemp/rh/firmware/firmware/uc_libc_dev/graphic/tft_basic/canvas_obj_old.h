#ifndef __CANVAS_OBJ_H
#define __CANVAS_OBJ_H

#include "_config_cpu_.h"
#include "canvas.h"

// tLabel
//---------------------------------------------------------------------
typedef struct {
	string caption; //caption
	string pcap; 	//printed caption
	tColor fgcolor; //foreground color (text color)
	tColor bkcolor; //background color
	tColor bdcolor; //border color
	tColor tgcolor; //if<>clNONE pinta uma marca com a cor
	tPoint pos;
	char size; 		// size in chars
	unsigned repaint : 1; //full repaint?
}tLabel, *pLabel;

void label_Init(pLabel label);
void label_Caption(pLabel label, pchar caption);
void label_Pos(pLabel label, int x, int y);
void label_Color(pLabel label, tColor fgcolor, tColor bkcolor, tColor bdcolor);
void label_Repaint(pLabel label);
int label_MouseOver(pLabel label, int x, int y);

tRect asRect(int x, int y, int w, int h);

/*
// tButton
//---------------------------------------------------------------------
typedef struct {
	tLabel label;
	tPoint pos; //top.left position
	int width, height;
	//char icon; //-1 = off
}tButton, *pButton;

void button_Init(pButton button);
void button_Repaint(pButton button);
*/
#endif

