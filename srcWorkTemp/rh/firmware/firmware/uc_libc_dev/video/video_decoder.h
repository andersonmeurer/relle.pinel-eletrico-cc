#ifndef _LINUX_VIDEO_DECODER_H
#define _LINUX_VIDEO_DECODER_H

#include "uc_libdefs.h"

#define HAVE_VIDEO_DECODER 1

struct video_decoder_capability { /* this name is too long */
	u32	flags;
#define	VIDEO_DECODER_PAL	1	/* can decode PAL signal */
#define	VIDEO_DECODER_NTSC	2	/* can decode NTSC */
#define	VIDEO_DECODER_SECAM	4	/* can decode SECAM */
#define	VIDEO_DECODER_AUTO	8	/* can autosense norm */
#define	VIDEO_DECODER_CCIR	16	/* CCIR-601 pixel rate (720 pixels per line) instead of square pixel rate */
	int	inputs;			/* number of inputs */
	int	outputs;		/* number of outputs */
};

/*
DECODER_GET_STATUS returns the following flags.  The only one you need is
DECODER_STATUS_GOOD, the others are just nice things to know.
*/
#define	DECODER_STATUS_GOOD	1	/* receiving acceptable input */
#define	DECODER_STATUS_COLOR	2	/* receiving color information */
#define	DECODER_STATUS_PAL	4	/* auto detected */
#define	DECODER_STATUS_NTSC	8	/* auto detected */
#define	DECODER_STATUS_SECAM	16	/* auto detected */

struct video_decoder_init {
	unsigned char len;
	const unsigned char *data;
};

#define	DECODER_GET_CAPABILITIES 1
#define	DECODER_GET_STATUS    	 2
#define	DECODER_SET_NORM		 3
#define	DECODER_SET_INPUT		4	/* 0 <= input < #inputs */
#define	DECODER_SET_OUTPUT		5	/* 0 <= output < #outputs */
#define	DECODER_ENABLE_OUTPUT	6	/* boolean output enable control */
#define	DECODER_SET_PICTURE   	7
#define	DECODER_SET_GPIO		8	/* switch general purpose pin */
#define	DECODER_INIT			9	/* init internal registers at once */
#define	DECODER_SET_VBI_BYPASS	10	/* switch vbi bypass */

#define	DECODER_DUMP			192		/* debug hook */


#endif
