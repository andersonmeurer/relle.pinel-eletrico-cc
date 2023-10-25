#ifndef __COLORS_H
#define __COLORS_H

#include "uc_libdefs.h"

// consultar documento:  D:\meus_conhecimentos\videos & imagens\RGB color table.docx

#define clBlack 		0x00000000
#define clMaroon 		0x00000080
#define clGreen 		0x00008000
#define clOlive 		0x00008080
#define clNavy 			0x00800000
#define clPurple 		0x00800080
#define clTeal 			0x00808000
#define clGray 			0x00808080
#define clSilver 		0x00C0C0C0
#define clRed 			0x000000FF
#define clLime 			0x0000FF00
#define clYellow 		0x0000FFFF
#define clBlue 			0x00FF0000
#define clAqua 			0x00FFFF00
#define clCyan 			clAqua
#define clMagenta		0x00FF00FF
#define clFuchsia		clMagenta
#define clLtGray 		0x00C0C0C0
#define clDarkGray 		0x00808080
#define clNone			0x1FFFFFFF
#define clDarkBlue 		0x00400000
#define clDarkYellow 	0x0000C0C0
#define clWhite 		0x00FFFFFF
/*
					RRGGBB
#define clMaroon	#800000
#define clDark red	#8B0000
#define clBrown	#A52A2A
#define clFireBrick	#B22222
*/
#define clCrimson		0x003C14DC
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
#define clOrangeRed		0x000045FF
#define clDarkOrange	0x00008CFF
#define clOrange		0x0000A5FF
#define clGold			0x0000D7FF
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
#define clSpringGreen		0x007FFF00
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
#define clTurquoise			0x00D0E040
#define clMediumTurquoise	0x00CCD148
#define clPaleTurquoise		0x00EEEEAF
#define clAquaMarine		0x00D4FF7F
#define clPowderBlue		0x00E6E0B0
#define clCadetBlue			0x00A09E5F
#define clSteelBlue			0x00B482B4
/*
#define clcorn flower blue	#6495ED
*/

#define clDeepSkyBlue		0x00FFBF00
#define clDodgerBlue		0x00FF901E

/*
#define cllight blue	#ADD8E6
#define clsky blue	#87CEEB
#define cllight sky blue	#87CEFA
*/
#define clMidNightBlue		0x00701919
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
#define clorchid	#DA70D6
#define clmedium viole tred	#C71585
#define clpale violet red	#DB7093
*/
#define clDeepPink	0x009314FF
#define clHotPink	0x00B469FF
/*
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
*/
#define clLightYellow	0x00e0ffff
/*
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
#define cllight_gray 	RGB24(#D3D3D3
#define clgainsboro		RGB24(#DCDCDC
#define clWhiteSmoke	RGB24(#F5F5F5

*/


#endif // __COLORS_H
