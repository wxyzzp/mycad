#ifndef __tolerancegenH__
#define __tolerancegenH__

#include "sds.h"
#include "textgen.h"

class db_tolerance;
class db_drawing;
struct gr_view;
struct gr_displayobject;
class CDC;

/*
**  Data for one text fragment (one piece of text where the parameters
**  stay constant).  A buffer of formatted text can be broken up
**  into a llist of these by gr_getftextfrags().
*/
struct gr_ftextfraglink {
    gr_textgenparslink  tgpar;    /* MOST of the parameters for this fragment. */

    int                 color;    /* This one's strange.  It's not part of */
                                  /*   db_textgenparslink because gr_textgen() */
                                  /*   gets color from the gr_view struct. */
                                  /*   We need it here on a per-fragment basis. */
                                  /*   This is the resolved color and will be */
                                  /*   used to force viewp->color when we call */
                                  /*   gr_textgen() to actually generate the */
                                  /*   display objects for this fragment. */

    char                eopara;   /* End of parargraph (MTEXT) or end of */
                                  /*   a line of text within a box */
                                  /*   (TOLERANCE). */
    char                eotolrow; /* End of TOLERANCE row (implies end-of-box, too. */
    char                eotolbox; /* End of TOLERANCE box. */

    sds_point           textboxpt[2];

    gr_ftextfraglink   *next;
};

int gr_tol2doll(
	db_tolerance			 *pToleranceEntity,
    gr_displayobject **begdopp,
    gr_displayobject **enddopp,
    gr_view           *viewp,
	CDC						 *dc,
    db_drawing               *dp);

#endif
