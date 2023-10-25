/*
	SOMENTE SUPORTA IMAGEM BMP 24BITS PADRÃO WINDOWS, SEM PALETAS, FRAMES E COMPACTAÇÃO RLE
*/ 

#include "cdjpeg.h"		// Common decls for cjpeg/djpeg applications 

#ifdef BMP_SUPPORTED

// Macros to deal with unsigned chars as efficiently as compiler allows 

#ifdef HAVE_UNSIGNED_CHAR
typedef unsigned char U_CHAR;
#define UCH(x)	((int) (x))
#else // !HAVE_UNSIGNED_CHAR 
#ifdef CHAR_IS_UNSIGNED
typedef char U_CHAR;
#define UCH(x)	((int) (x))
#else
typedef char U_CHAR;
#define UCH(x)	((int) (x) & 0xFF)
#endif
#endif // HAVE_UNSIGNED_CHAR 

#ifndef NOT_USE_FILES 
#define	ReadOK(file,buffer,len)	(JFREAD(file,buffer,len) == ((size_t) (len)))
#endif

// Private version of data source object 

typedef struct _bmp_source_struct *bmp_source_ptr;

typedef struct _bmp_source_struct {
  	struct cjpeg_source_struct pub; 	// public fields 
  	j_compress_ptr cinfo;				// back link saves passing separate parm 
  	JSAMPARRAY colormap;				// BMP colormap (converted to my format) 
  	jvirt_sarray_ptr whole_image;		// Needed to reverse row order 
  	JDIMENSION source_row;				// Current source row number 
  	JDIMENSION row_width;				// Physical width of scanlines in file 
  	int bits_per_pixel;					// remembers 8- or 24-bit format 
  	INT32 biWidth;
  	INT32 biHeight;  	
} bmp_source_struct;

//
// Read one row of pixels.
// The image has been read into the whole_image array, but is otherwise
// unprocessed.  We must read it out in top-to-bottom row order, and if
// it is an 8-bit image, we must expand colormapped pixels to 24bit format.

METHODDEF(JDIMENSION)
get_24bit_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
// This version is for reading 24-bit pixels 
{
  	bmp_source_ptr source = (bmp_source_ptr) sinfo;
  	JSAMPARRAY image_ptr;
  	register JSAMPROW inptr, outptr;
  	register JDIMENSION col;

  	// Fetch next row from virtual array 
  	source->source_row--;
  	image_ptr = (*cinfo->mem->access_virt_sarray)
    			((j_common_ptr) cinfo, source->whole_image,
     			source->source_row, (JDIMENSION) 1, FALSE);

  	// Transfer data.  Note source values are in BGR order
  	// (even though Microsoft's own documents say the opposite).
   
  	inptr = image_ptr[0];
  	outptr = source->pub.buffer[0];
  	
  	for (col = cinfo->image_width; col > 0; col--) {
    	outptr[2] = *inptr++;	// can omit GETJSAMPLE() safely 
    	outptr[1] = *inptr++;
    	outptr[0] = *inptr++;
    	outptr += 3;
  	}

  	return 1;
}


//
// This method loads the image into whole_image during the first call on
// get_pixel_rows.  The get_pixel_rows pointer is then adjusted to call
// get_8bit_row or get_24bit_row on subsequent calls.
 

METHODDEF(JDIMENSION) preload_image (j_compress_ptr cinfo, cjpeg_source_ptr sinfo) {
  	bmp_source_ptr source = (bmp_source_ptr) sinfo;
  	#ifdef NOT_USE_FILES 
  		register int c = 0;
  	#else
  		register FILE *infile = source->pub.input_file;
  		register int c;
  	#endif
  	register JSAMPROW out_ptr;
  	JSAMPARRAY image_ptr;
  	JDIMENSION row, col;
  	
  	cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;

  	// Read the data into a virtual array in input-file row order. 
  	for (row = 0; row < cinfo->image_height; row++) {
    	if (progress != NULL) {
      		progress->pub.pass_counter = (long) row;
      		progress->pub.pass_limit = (long) cinfo->image_height;
      		(*progress->pub.progress_monitor) ((j_common_ptr) cinfo);
    	}
    
    	image_ptr = (*cinfo->mem->access_virt_sarray)
      				((j_common_ptr) cinfo, source->whole_image,
       				row, (JDIMENSION) 1, TRUE);
    	out_ptr = image_ptr[0];
    	
    	for (col = source->row_width; col > 0; col--) {
  			// inline copy of read_byte() for speed 
  			#ifdef NOT_USE_FILES 
  			 			
	   			if (source->pub.buffer_in_index >= source->pub.buffer_in_size) {
					ERREXIT(cinfo, JERR_INPUT_EOF);
				} else {
					c = (int)source->pub.buffer_in[source->pub.buffer_in_index++];  		
				}
      		
      			*out_ptr++ = (JSAMPLE) c;
      		#else
      		
      			if ((c = getc(infile)) == EOF)
					ERREXIT(cinfo, JERR_INPUT_EOF);
      			*out_ptr++ = (JSAMPLE) c;
      		#endif
    	}
  	}
  
  	if (progress != NULL)
    	progress->completed_extra_passes++;

	// Set up to read from the virtual array in top-to-bottom order 
	source->pub.get_pixel_rows = get_24bit_row;  	
  	source->source_row = cinfo->image_height;

  	// And read the first row 
  	return (*source->pub.get_pixel_rows) (cinfo, sinfo);
}


//
// Read the file header; return image size and component count.
METHODDEF(void) start_input_bmp (j_compress_ptr cinfo, cjpeg_source_ptr sinfo) {
  	bmp_source_ptr source = (bmp_source_ptr) sinfo;
  	
	JDIMENSION row_width; 
	
	//source->biWidth  = biWidth;
  	//source->biHeight = biHeight;

	source->bits_per_pixel = 24;
	source->pub.buffer_in_index = 54;
  	source->pub.get_pixel_rows = preload_image;
  	cinfo->in_color_space = JCS_RGB;
  	cinfo->input_components = 3;
  	cinfo->data_precision = 8;
  	cinfo->image_width =  (JDIMENSION)source->biWidth;
  	cinfo->image_height = (JDIMENSION)source->biHeight;
  	
	TRACEMS2(cinfo, 1, JTRC_BMP, (int) source->biWidth, (int)source->biHeight);
  	row_width = (JDIMENSION) (source->biWidth * 3);
  	
  	while ((row_width & 3) != 0) row_width++;
  	source->row_width = row_width;
  	 	
  	// Allocate space for inversion array, prepare for preload pass 
  	source->whole_image = 	(*cinfo->mem->request_virt_sarray)
    						((j_common_ptr) cinfo, JPOOL_IMAGE, FALSE,
     						row_width, (JDIMENSION) source->biHeight, (JDIMENSION) 1);
  	  	
  	if (cinfo->progress != NULL) {
    	cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;
    	progress->total_extra_passes++; // count file input as separate pass 
  	}

  	// Allocate one-row buffer for returned data 
  	source->pub.buffer = 	(*cinfo->mem->alloc_sarray)
    						((j_common_ptr) cinfo, JPOOL_IMAGE,
     						(JDIMENSION) (source->biWidth * 3), (JDIMENSION) 1);
}


//
// Finish up at the end of the file.
 

METHODDEF(void) finish_input_bmp (j_compress_ptr cinfo, cjpeg_source_ptr sinfo) {
  // no work 
}


//
// The module selection routine for BMP format input.
 

GLOBAL(cjpeg_source_ptr) jinit_read_bmp (j_compress_ptr cinfo) {
  	bmp_source_ptr source;

  	// ALOCA RAM PARA A INTERFACE
  	source = 	(bmp_source_ptr)
      			(*cinfo->mem->alloc_small) 
      			((j_common_ptr) cinfo, JPOOL_IMAGE, SIZEOF(bmp_source_struct));
  	source->cinfo = cinfo;							// Guarda o link de retorno para a subrotina
  	
  	// PREENCHE OS PONTEIROS DAS FUNÇÃO PAA CHAMADA DO INICIO E FIM DA COMPATAÇÃO
  	source->pub.start_input = start_input_bmp;		// Ponteiro da função que inicializa o tratamento de leitura bmp
  	source->pub.finish_input = finish_input_bmp;	// Ponteiro da função que finaliza o tratamento de leitura bmp

  	return (cjpeg_source_ptr) source;				// Retorna endereço da fonte de imagem
}

GLOBAL(void) jinit_set_image (j_compress_ptr cinfo, cjpeg_source_ptr sinfo, INT32 biWidth, INT32 biHeight) {
	bmp_source_ptr source = (bmp_source_ptr) sinfo;
	
	source->biWidth  = biWidth;
  	source->biHeight = biHeight;
}

#endif // BMP_SUPPORTED 
