/* C:\ICAD\PRJ\LIB\DB\FONTLINK.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/*
**  Font definition stuff:
*/

#ifndef _INC_FONTLINK
#define _INC_FONTLINK

struct db_fontchrlink {

    db_fontchrlink(void) { code=defsz=0; def=symbolName=NULL; }
   ~db_fontchrlink(void) { delete [] def; delete[] symbolName; }

    short  code;  /* Character code (should match array index for */
                  /*   0-255). */
    short  defsz; /* Size of the def array (can't use terminating 0 */
                  /*   since vectors can have 0 components). */
    char  *def;   /* The definition bytes. */

	// added by Denis Petrov for support of complex linetypes
	char* symbolName; /* Name of this symbol. */
};

struct db_fontlink {
	enum FontType {SHAPE_1_0, SHAPE_1_1, UNIFONT, BIGFONT, TRUETYPE};

    db_fontlink(void) {
        pn=desc=NULL; above=6; below=2; modes=2; width=6;
        encoding=type=0; format=SHAPE_1_1; lffact=5.0/3.0; spfact=1.0;
        nesc=0; esc=NULL; nchrs=0; chr=NULL; next=NULL;
    }
   ~db_fontlink(void) {
        delete [] pn; delete [] desc; delete [] esc; delete [] chr;
    }

    /* Data describing the font (see font documentation): */
    char                   *pn;     /* Pathname from which it was loaded */
    char                   *desc;

    unsigned char           above,  /* Or height for extended bigfont */
                            below;
    char                    modes;
    unsigned char           width;  /* Extended bigfont */
    char                    encoding;
    char                    type;   /* See ACAD UNICODE documentation. */

    FontType                format;   /* Format: */
                                    /*    SHAPE 1.0 or 1.1 */
                                    /*    UNICODE */
                                    /*    BIGFONT */
    /*
    **  These two are POSITIVE spacing factors, determined by reading
    **  the line-feed (10) and space (32) chr defs.  Multiply them by
    **  text height to get line-spacing and space-spacing for a given
    **  situation.  spfact should also be multiplied by the width
    **  factor that's being used.
    */
    sds_real                lffact; /* Line-feed   vert spacing in text hts */
    sds_real                spfact; /* Space char horiz spacing in text hts */


    /* The following 2 are for big fonts only; they are 0/NULL otherwise: */

    char                    nesc;   /* # of escape code RANGES. */
    char                   *esc;    /* Alloc'd array of escape code */
                                    /*   range specifiers (alternating */
                                    /*   beginning,ending,...). */
                                    /*   (There are 2*nesc chars.) */
                                    /*   NOT A STRING. */

    /* The array of font characters: */
    short                   nchrs;  /* # of characters defined (256 + */
                                    /*   1 element for each code>255). */
    struct db_fontchrlink  *chr;    /* An array with nchrs elements */

    struct db_fontlink     *next;
};


#endif
