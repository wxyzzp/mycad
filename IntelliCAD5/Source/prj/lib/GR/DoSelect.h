#ifndef __DoSelectH__
#define __DoSelectH__

class db_handitem;
struct gr_view;

/*------------------------------------------------------------------------*//**
@author Denis Petrov
For reliability mostly copied behavior of first version but also oriented for
3D display objects. Using selectors classes.
TODO: build some hierarchy for selection using different selectors and modes, 
remove first version.
*//*-------------------------------------------------------------------------*/
GR_API int gr_selected2
(
db_handitem* pEntity, 
struct gr_selectorlink* pSelector, 
gr_view* pView
);

#endif
