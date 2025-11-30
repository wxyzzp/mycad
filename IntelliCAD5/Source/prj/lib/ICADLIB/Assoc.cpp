
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

struct resbuf *ic_rbassoc;   /* return buffer for ic_assoc function */

short ic_assoc(struct resbuf *elist, short tp) {

    struct resbuf *tb;
    if(elist==NULL) return(-1);
    for(tb=elist; tb !=NULL; tb=tb->rbnext) {
        if(tb->restype==tp) {
            ic_rbassoc=tb; return(0);
        }
    }
    return(-1);
}

// A version not dependent on setting a global
//
struct resbuf *
ic_ret_assoc( struct resbuf *prbList, int type )
	{
    struct resbuf *prbRetval = NULL;
	struct resbuf *prbTemp;
    for( prbTemp = prbList; prbTemp !=NULL; prbTemp = prbTemp->rbnext) 
		{
        if( prbTemp->restype == type ) 
			{
			prbRetval = prbTemp;
			break;
	        }
		}

	return prbRetval;

	}

// This function should be used to release a sds_resbuf chain that
// contains DWGdirect data.
void ic_relOdDbResbuf (struct resbuf *prbList)
{
	if (prbList == NULL)
		return;

	struct resbuf* tmpRb = prbList;
	while(tmpRb != NULL)
	{
		// Because when we get an OdDbObjectId from DWGdirect, we
		// convert it to a handle, and store it in the rstring field
		// of sds_resbuf, we need to clean it up here. sds_relrb, would
		// simply skip this resbuf type.
		switch(ic_resval(tmpRb->restype))
		{
		case RTENAME:
			IC_FREE(tmpRb->resval.rstring);
			break;
		}

		tmpRb = tmpRb->rbnext;
	}
	IC_RELRB(prbList);

	return;
}
