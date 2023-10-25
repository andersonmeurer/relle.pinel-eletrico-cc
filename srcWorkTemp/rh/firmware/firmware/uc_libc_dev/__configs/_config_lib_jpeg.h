#ifndef __CONFIG_LIB_JCONFIG_H
#define __CONFIG_LIB_JCONFIG_H

// ###################################################################################################################
// Sinaliza as libs do codec que tipo de saida e entrada de imagem será usado
#define NOT_USE_FILES 	// Define isso para sinalizar as libs do codec que não vai ser usado FILEs para 

// ###################################################################################################################
// ESPECIFICA O TIPO DE SAIDA DE ERRO
//#define PRINTF_ERR(fmt, args...) rprintf(PRINTF_UART0 ,fmt, ##args) // uC
#define PRINTF_ERR(fmt, args...) fprintf(stderr,fmt, ##args) // PC

// ###################################################################################################################
// Does your compiler support function prototypes?  (If not, you also need to use ansi2knr, see install.doc)
//		Exemplo função com protótipo: char *getenv(const char * name);
//		Exemplo função sem protótipo: char *getenv();
#define HAVE_PROTOTYPES

// ###################################################################################################################
// Does your compiler support the declaration "unsigned char" ?  How about "unsigned short" ? 
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT // UINT16 must hold at least the values 0..65535


// ###################################################################################################################
// Define "void" as "char" if your compiler doesn't know about type void.
//  NOTE: be sure to define void such that "void *" represents the most general
// pointer type, e.g., that returned by malloc().
// #define void char 


// ###################################################################################################################
// Define "const" as empty if your compiler doesn't know the "const" keyword.
// #define const

// ###################################################################################################################
// Define this if an ordinary "char" type is unsigned.
// If you're not sure, leaving it undefined will work at some cost in speed.
// If you defined HAVE_UNSIGNED_CHAR then the speed difference is minimal.
#undef CHAR_IS_UNSIGNED


// ###################################################################################################################
// Define this if your system has an ANSI-conforming <stddef.h> file.
#undef HAVE_STDDEF_H


// ###################################################################################################################
// Define this if your system has an ANSI-conforming <stdlib.h> file.
#define HAVE_STDLIB_H // Para uso do malloc, free do compilador e outros recursos


// ###################################################################################################################
// Define this if your system does not have an ANSI/SysV <string.h>,
// but does have a BSD-style <strings.h>.
#undef NEED_BSD_STRINGS


// ###################################################################################################################
// Define this if your system does not provide typedef size_t in any of the
// ANSI-standard places (stddef.h, stdlib.h, or stdio.h), but places it in
// <sys/types.h> instead.
#undef NEED_SYS_TYPES_H


// ###################################################################################################################
// For 80x86 machines, you need to define NEED_FAR_POINTERS,
// unless you are using a large-data memory model or 80386 flat-memory mode.
//  On less brain-damaged CPUs this symbol must not be defined.
//  (Defining this symbol causes large data structures to be referenced through
//  "far" pointers and to be allocated with a special version of malloc.)
#undef NEED_FAR_POINTERS	// DJGPP uses flat 32-bit addressing 


// ###################################################################################################################
// Define this if your linker needs global names to be unique in less
// than the first 15 characters.
#undef NEED_SHORT_EXTERNAL_NAMES


// ###################################################################################################################
// Although a real ANSI C compiler can deal perfectly well with pointers to
//  unspecified structures (see "incomplete types" in the spec), a few pre-ANSI
//  and pseudo-ANSI compilers get confused.  To keep one of these bozos happy,
//  define INCOMPLETE_TYPES_BROKEN.  This is not recommended unless you
//  actually get "missing structure definition" warnings or errors while
//  compiling the JPEG code.
#undef INCOMPLETE_TYPES_BROKEN


// ###################################################################################################################
// SE NÃO EXISTE A ROTINA getenv() NO COMPILADOR DEFINE NO_GETENV
#undef NO_GETENV


// ###################################################################################################################
#undef USE_FDOPEN
	

// ###################################################################################################################
#undef USE_SETMODE		// Needed to make one-file style work in DJGPP 


// ###################################################################################################################
//  The following options affect code selection within the JPEG library,
//  but they don't need to be visible to applications using the library.
//  To minimize application namespace pollution, the symbols won't be
//  defined unless JPEG_INTERNALS has been defined.
#ifdef JPEG_INTERNALS // Uso das libs jpeg
	// Define this if your compiler implements ">>" on signed values as a logical
	// (unsigned) shift; leave it undefined if ">>" is a signed (arithmetic) shift,
	// which is the normal and rational definition.
	#undef RIGHT_SHIFT_IS_UNSIGNED
	
 	// These defines indicate whether to include various optional functions.
 	// Undefining some of these symbols will produce a smaller but less capable
 	// library.  Note that you can leave certain source files out of the
 	// compilation/linking process if you've #undef'd the corresponding symbols.
 	// (You may HAVE to do that if your compiler doesn't like null source files.)
	
	//  Arithmetic coding is unsupported for legal reasons.  Complaints to IBM.

	//  Capability options common to encoder and decoder:  relacionado a função cinfo.dct_method
	//#define DCT_ISLOW_SUPPORTED	// slow but accurate integer algorithm
	#define DCT_IFAST_SUPPORTED		// faster, less accurate integer method
	//#define DCT_FLOAT_SUPPORTED	// floating-point: accurate, fast on fast HW 

	//  Encoder capability options:
	#undef C_ARITH_CODING_SUPPORTED    	//  Arithmetic coding back end? 
	#undef C_MULTISCAN_FILES_SUPPORTED 	//  Multiple-scan JPEG files?
	#undef C_PROGRESSIVE_SUPPORTED	    //  Progressive JPEG? (Requires MULTISCAN)
	#define ENTROPY_OPT_SUPPORTED	    //  Optimization of entropy coding parms?

	// Note: if you selected 12-bit data precision, it is dangerous to turn off
 	// ENTROPY_OPT_SUPPORTED.  The standard Huffman tables are only good for 8-bit
 	// precision, so jchuff.c normally uses entropy optimization to compute
 	// usable tables for higher precision.  If you don't want to do optimization,
 	// you'll have to supply different default Huffman tables.
 	// The exact same statements apply for progressive JPEG: the default tables
 	// don't work for progressive mode.  (This may get fixed, however.)

	#undef INPUT_SMOOTHING_SUPPORTED   //  Input image smoothing option?

	//  Decoder capability options:
	#undef  D_ARITH_CODING_SUPPORTED    // Arithmetic coding back end? 
	#define D_MULTISCAN_FILES_SUPPORTED // Multiple-scan JPEG files? 
	#define D_PROGRESSIVE_SUPPORTED	    // Progressive JPEG? (Requires MULTISCAN)
	#define SAVE_MARKERS_SUPPORTED	    // jpeg_save_markers() needed? 
	#define BLOCK_SMOOTHING_SUPPORTED   // Block smoothing? (Progressive only) 
	#define IDCT_SCALING_SUPPORTED	    // Output rescaling via IDCT? 
	#undef  UPSAMPLE_SCALING_SUPPORTED  // Output rescaling at upsample stage? 
	#define UPSAMPLE_MERGING_SUPPORTED  // Fast path for sloppy upsampling? 
	#define QUANT_1PASS_SUPPORTED	    // 1-pass color quantization? 
	#define QUANT_2PASS_SUPPORTED	    // 2-pass color quantization? 	
#endif


// ###################################################################################################################
//  The remaining options do not affect the JPEG library proper,
//  but only the sample applications cjpeg/djpeg (see cjpeg.c, djpeg.c).
//  Other applications can ignore these.
#ifdef JPEG_CJPEG_DJPEG // uso da lib jpeg
	// ###################################################################################################################
	// These defines indicate which image (non-JPEG) file formats are allowed.
	#define BMP_SUPPORTED		// BMP image file format
	#undef GIF_SUPPORTED		// GIF image file format
	#undef PPM_SUPPORTED		// PBMPLUS PPM/PGM image file format
	#undef RLE_SUPPORTED		// Utah RLE image file format
	#undef TARGA_SUPPORTED		// Targa image file format


	// ###################################################################################################################
	// Define this if you want to name both input and output files on the command
	//  line, rather than using stdout and optionally stdin.  You MUST do this if
	//  your system can't cope with binary I/O to stdin/stdout.  See comments at
	//  head of cjpeg.c or djpeg.c.
	#undef TWO_FILE_COMMANDLINE	// optional

	// ###################################################################################################################
	// Define this if your system needs explicit cleanup of temporary files.
	// This is crucial under MS-DOS, where the temporary "files" may be areas
	// of extended memory; on most other systems it's not as important.
	#undef NEED_SIGNAL_CATCHER	// Define this if you use jmemname.c


	// ###################################################################################################################
	// By default, we open image files with fopen(...,"rb") or fopen(...,"wb").
	//  This is necessary on systems that distinguish text files from binary files,
	//  and is harmless on most systems that don't.  If you have one of the rare
	//  systems that complains about the "b" spec, define this symbol.
	#undef DONT_USE_B_MODE


	// ###################################################################################################################
	// Define this if you want percent-done progress reports from cjpeg/djpeg.
	#undef PROGRESS_REPORT		// optional
#endif


// ###################################################################################################################
// OS ALGORITMOS FAZEM MUITOS MULTIPLICAÇÕES DE 32BITS. 
// NÃO DECLARANDO NENHUMA DEFINIÇÃO ABAIXO AS LIBS ASSUMEM PROCEDIMENTO PADRÃO
// #define SHORTxLCONST_32 
// #define SHORTxSHORT_32 

#endif
