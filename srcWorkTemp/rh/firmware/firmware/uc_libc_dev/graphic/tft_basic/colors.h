#ifndef __COLORS_H
#define __COLORS_H

// consultar documento:  D:\meus_conhecimentos\videos & imagens\RGB color table.docx

#include "uc_libdefs.h"

//						  __BBGGRR
#define clBlack 		RGB16(0x00,0x00,0x00)
#define clMaroon 		RGB16(0x00,0x00,0x80)
#define clGreen 		RGB16(0x00,0x80,0x00)
#define clOlive 		RGB16(0x00,0x80,0x80)
#define clNavy 			RGB16(0x80,0x00,0x00)
#define clPurple 		RGB16(0x80,0x00,0x80)
#define clTeal 			RGB16(0x80,0x80,0x00)
#define clGray 			RGB16(0x80,0x80,0x80)
#define clSilver 		RGB16(0xC0,0xC0,0xC0)
#define clRed 			RGB16(0x00,0x00,0xFF)
#define clLime 			RGB16(0x00,0xFF,0x00)
#define clYellow 		RGB16(0x00,0xFF,0xFF)
#define clBlue 			RGB16(0xFF,0x00,0x00)
#define clFuchsia 		RGB16(0xFF,0x00,0xFF)
#define clAqua 			RGB16(0xFF,0xFF,0x00)
#define clCyan 			RGB16(0xFF,0xFF,0x00)
#define clLtGray 		RGB16(0xC0,0xC0,0xC0)
#define clDarkGray 		RGB16(0x80,0x80,0x80)
#define clNone			0x1FFFFFFF
#define clDarkBlue 		RGB16(0x40,0x00,0x00)
#define clDarkYellow 	RGB16(0x00,0xC0,0xC0)
#define clWhite 		RGB16(0xFF,0xFF,0xFF)
/*
					RRGGBB
#define clMaroon	#800000
#define clDark red	#8B0000
#define clBrown	#A52A2A
#define clFireBrick	#B22222
*/
#define clCrimson		RGB16(0x3C,0x14,0xDC)
/*
#define clRed	#FF0000
#define clTomato	#FF6347
#define clCoral	#FF7F50
#define clIndianRed	#CD5C5C
#define clLightCoral	#F08080
#define clDarkSalmon	#E9967A
#define clSalmon	#FA8072
#define clLightSalmon	#FFA07A
*/
#define clOrangeRed		RGB16(0x00,0x45,0xFF)
#define clDarkOrange	RGB16(0x00,0x8C,0xFF)
#define clOrange		RGB16(0x00,0xA5,0xFF)
#define clGold			RGB16(0x00,0xD7,0xFF)
/*
#define clDark golden rod	#B8860B
#define clGolden rod	#DAA520
#define clPale golden rod	#EEE8AA
#define clDark khaki	#BDB76B
#define clKhaki	#F0E68C
#define clOlive	#808000
#define clYellow	#FFFF00
#define clYellowGreen	#9ACD32
#define clDarkOliveGreen	#556B2F
#define clOlive drab	#6B8E23
#define clLawn green	#7CFC00
#define clChart reuse	#7FFF00
#define clGreen yellow	#ADFF2F
#define clDarkGreen	#006400
#define clGreen	#008000
#define clForest green	#228B22
#define clLime	#00FF00
#define clLime green	#32CD32
#define clLight green	#90EE90
#define clPale green	#98FB98
#define clSarkSeaGreen	#8FBC8F
#define clMediumSpringGreen	#00FA9A
*/
#define clSpringGreen		RGB16(0x7F, 0xFF, 0x00)
/*
#define clSea green	#2E8B57
#define clMedium aqua marine	#66CDAA
#define clMedium sea green	#3CB371
#define cllight sea green	#20B2AA
#define cldark slate gray	#2F4F4F
#define clteal	#008080
#define cldark cyan	#008B8B
#define claqua	#00FFFF
#define clcyan	#00FFFF
#define cllight cyan	#E0FFFF
#define cldark turquoise	#00CED1
*/
#define clTurquoise			RGB16(0xD0, 0xE0, 0x40)
#define clMediumTurquoise	RGB16(0xCC, 0xD1, 0x48)
#define clPaleTurquoise		RGB16(0xEE, 0xEE, 0xAF)
#define clAquaMarine		RGB16(0xD4, 0xFF, 0x7F)
#define clPowderBlue		RGB16(0xE6, 0xE0, 0xB0)
#define clCadetBlue			RGB16(0xA0, 0x9E, 0x5F)
#define clSteelBlue			RGB16(0xB4, 0x82, 0xB4)
/*
#define clcorn flower blue	#6495ED
#define cldeep sky blue	#00BFFF
*/
#define clDodgerBlue		RGB16(0xFF, 0x90, 0x1E)
/*
#define cllight blue	#ADD8E6
#define clsky blue	#87CEEB
#define cllight sky blue	#87CEFA
*/
#define clMidNightBlue		RGB16(0x70, 0x19, 0x19)
/*
#define clnavy	#000080
#define cldark blue	#00008B
#define clmedium blue	#0000CD
#define clblue	#0000FF
#define clroyal blue	#4169E1
#define clblue violet	#8A2BE2
#define clindigo	#4B0082
#define cldark slate blue	#483D8B
#define clslate blue	#6A5ACD
#define clmedium slate blue	#7B68EE
#define clmedium purple	#9370DB
#define cldark magenta	#8B008B
#define cldark violet	#9400D3
#define cldark orchid	#9932CC
#define clmedium orchid	#BA55D3
#define clpurple	#800080
#define clthistle	#D8BFD8
#define clplum	#DDA0DD
#define clviolet	#EE82EE
#define clmagenta / fuchsia	#FF00FF
#define clorchid	#DA70D6
#define clmedium viole tred	#C71585
#define clpale violet red	#DB7093
#define cldeep pink	#FF1493
#define clhot pink	#FF69B4
#define cllight pink	#FFB6C1
#define clpink	#FFC0CB
#define clantique white	#FAEBD7
#define clbeige	#F5F5DC
#define clbisque	#FFE4C4
#define clblanched almond	#FFEBCD
#define clwheat	#F5DEB3
#define clcorn silk	#FFF8DC
#define cllemon chiffon	#FFFACD
#define cllight golden rod yellow	#FAFAD2
#define cllight yellow	#FFFFE0
#define clsaddle brown	#8B4513
#define clsienna	#A0522D
#define clchocolate	#D2691E
#define clperu	#CD853F
#define clsandy brown	#F4A460
#define clburly wood	#DEB887
#define cltan	#D2B48C
#define clrosy brown	#BC8F8F
#define clmoccasin	#FFE4B5
#define clnavajo white	#FFDEAD
#define clpeach puff	#FFDAB9
#define clmisty rose	#FFE4E1
#define cllavender blush	#FFF0F5
#define cllinen	#FAF0E6
#define clold lace	#FDF5E6
#define clpapaya whip	#FFEFD5
#define clsea shell	#FFF5EE
#define clmint cream	#F5FFFA
#define clslate gray	#708090
#define cllight slate gray	#778899
#define cllight steel blue	#B0C4DE
#define cllavender	#E6E6FA
#define clfloral white	#FFFAF0
#define clalice blue	#F0F8FF
#define clghost white	#F8F8FF
#define clhoneydew	#F0FFF0
#define clivory	#FFFFF0
#define clazure	#F0FFFF
#define clsnow	#FFFAFA
#define clblack	#000000
#define cldim gray / dim grey	#696969
#define clgray / grey	#808080
#define cldark gray / dark grey	#A9A9A9
#define clsilver	#C0C0C0
#define cllight_gray 	RGB16(#D3D3D3
#define clgainsboro		RGB16(#DCDCDC
#define clWhiteSmoke	RGB16(#F5F5F5

*/


#endif // __COLORS_H
