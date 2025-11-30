#ifndef __DoDefH__
#define __DoDefH__

#include <ostream.h>

class CMatrix4;
// constants used as values for gr_displayobject.type
const int DISP_OBJ_PTS_3D				= 1;	// not set -- points are 2D
const int DISP_OBJ_PTS_CLOSED			= 2;	// not set -- lines are open
const int DISP_OBJ_PTS_FILLABLE			= 4;	// not set -- entity is not fillable
const int DISP_OBJ_PTS_GDI_TRUETYPE		= 8;	// not set -- points are entity other than TrueType text/mtext/etc. output by GDI
const int DISP_OBJ_PTS_GDI_FILLPATH		= 16;	// this is a path of a SOLID hatch boundary or a TTF text contour (for filling by GDI)
const int DISP_OBJ_HIGHLIGHTED          = 32;   // display object should be highlighted
const int DISP_OBJ_SPECIAL				= 64;   // indicates special display object, data interprets dependening on type of source entity
const int DISP_OBJ_ENDPATH				= 128;  // this bit is used for indication that GDI path should be end'ed

// ****************************************
// CLASS DEFINITION -- gr_displayobject
struct gr_displayobject 
{
	// CONSTRUCTION AND DESTRUCTION
	//
public:
	// Added static factory methods rather than CTOR/DTOR combinations in order to
	// incrementally make the transformation over from using malloc/free.
	//
	static gr_displayobject* newDisplayObject( db_entity *pSourceEntity );
	static void freeDisplayObject( gr_displayobject *pObject );
public:
	char         type;    /* Bit-coded (76543210):                                 */
	/*   Bit 0 (1) :                                         */
	/*     0 : 2D (real projection plane system)             */
	/*     1 : 3D (in UCS)                                   */
	/*   Bit 1 (2) :                                         */
	/*     0 : Open chain of lines                           */
	/*     1 : Closed (last pt closes to first)              */
	/*   Bit 2 (4) : Fillable object (from a SOLID           */
	/*                 TRACE, or 2D POLYLINE with            */
	/*                 width).                               */
	/*   Bit 3 (8) : dispobj for TrueType text output by GDI */
	/*                 WARNING!!!!!!!  DON'T display these;  */
	/*                 they are generated for extents calcs  */
	/*													   */
	/* Bit 4 (0x10): SOLID Hatch path object or			   */
	/*			   TTF text contour						   */
	/*													   */
				/*D.G.*/// Please see my comments about TTF text output in the truetypeutils.cpp file.
	
	short        color;   /* Color for lines and fill (1-255:ACI/0:No pts.) */
	short        lweight; /* Lineweight for lines (0-211:const/-3:Default) 24-03-2003 Bugzilla#78471 Support Lineweight */
	
	int npts;    /* # of POINTS in chain (2D or 3D) */
	sds_real    *chain;   /* Array of XYXY... or XYZXYZ... */
	
	struct gr_displayobject *next;

public:
	
	db_entity* GetSourceEntity() { return m_pSourceEntity; }
	void SetSourceEntity(db_entity *pEntity) { m_pSourceEntity = pEntity; }

	friend ostream & operator<<(ostream &os, const gr_displayobject &disObj);
		
private:
	// This is the object that generated this gr_displayobject
	//
	db_entity	*m_pSourceEntity;
};

GR_API gr_displayobject* copyDisplayObject(struct gr_displayobject* pSourceObject, const CMatrix4* pTransformation = NULL);
GR_API void gr_freedisplayobjectll(void* pList);

#ifdef _GR_DEBUG
GR_CLASS void printDispObjList(ostream &os, gr_displayobject* pEntityListBegin, gr_displayobject* pEntityListEntity);
GR_CLASS void printDispObjList(ostream &os, sds_dobjll* pDispList);
#endif //_GR_DEBUG

inline void gr_setdochainpt(gr_displayobject* dop, int ptidx, sds_point sour, int ndims) 
{
/*
**  An extension of gr_gensolid().
**
**  This function was created to make a single location where
**  display object chain coordinates are assigned.  This will
**  help with viewp->noucs2rp trick for generating 3D UCS display
**  objects via the 2D "efficient" (no duplicate edges) logic,
**  where we're taking 3D points instead of the normal 2D.
**
**    dop : Pointer to the disp obj whose chain is being built.
**  ptidx : Which POINT in the chain (NOT which element in the array).
**   sour : The source point.
**  ndims : 2 for 2D (RP), or 3 for 3D (UCS)
*/
// new version 10/26/98
// ternary operator used to avoid a multiply
	memcpy(&dop->chain[ptidx*ndims],sour,(ndims==2) ? 2*sizeof(double) : 3*sizeof(double));
//    int cidx;

//    cidx=ptidx*ndims;
//                 dop->chain[cidx++]=sour[0];
//                 dop->chain[cidx++]=sour[1];
//    if (ndims>2) dop->chain[cidx  ]=sour[2];
}



#endif
