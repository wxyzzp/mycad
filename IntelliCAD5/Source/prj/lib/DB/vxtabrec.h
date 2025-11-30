/* C:\ICAD\PRJ\LIB\DB\VXTABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _INC_VXTABREC
#define _INC_VXTABREC

#include "viewport.h"

class DB_CLASS db_vxtabrec : public db_tablerecord {  /*   Need to research.  Refers to VIEWPORT entities. */
/* Ebatech(cnbr) research about VXTABREC. 2002/6/19

	db_vxtabrec is a kind of iterator which helps to find floating viewport quickly.
	When user iterates all floating viewports, use tbl_next() as same behavior as layers.
*/

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_vxtabrec(void);
    db_vxtabrec(char *pname, db_drawing *dp);
    db_vxtabrec(const db_vxtabrec &sour);  /* Copy constructor */

    db_viewport *ret_vpehip(void)
		{
		return vpehip;
		}
    int          ret_flag(void)   { return flag;   }

    void         set_vpehip(db_viewport *p)
		{
		ASSERT( (p == NULL) || (p->ret_type() == DB_VIEWPORT) );
		vpehip=p;
		}
    void         set_flag(int p) { flag=p; }

  private:

    db_viewport *vpehip;  /* The associated VIEWPORT entity */
    int          flag;    /* Unknown short in R12 */
};

#endif

