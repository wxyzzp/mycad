#ifndef __linetypegenH__
#define __linetypegenH__

class db_entity;
class db_drawing;
struct gr_displayobject;
struct gr_view;
class CDC;

short gr_doltype(db_entity *pSourceEntity,
				 gr_displayobject **begpp, gr_displayobject **endpp, gr_view *viewp, 
				 db_drawing* pDrawing, CDC* pDC);

#endif
