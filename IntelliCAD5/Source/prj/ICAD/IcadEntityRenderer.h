#ifndef __IcadEntityRendererH__
#define __IcadEntityRendererH__

// ****************************************************************
// class CIcadEntityRenderer
//
// A class used to keep entity drawing functionality
// together
//
#include "EntityRenderer.h"

// support functions placed here for now
void DrawGDITrueTypeTextBox(db_handitem *text, CDC *dc, gr_view *view);
bool GetGDITrueTypeTextBox(db_handitem *text, sds_point pt1, sds_point pt2,
	sds_point pt3, sds_point pt4, sds_point pt5);	// IntelliCAD uses 5 pts for a closed text box - pts 1 and 5 are coincident

class CUCSIcon;
class CGrid;
class CDbSortentsTable;

class CIcadEntityRenderer : public CEntityRenderer
{
	// member data
private:
	CIcadView* m_pView; // this will eventually migrate to subclasses
	CIcadDoc* m_pDoc;
	db_drawing* m_pDrawing;
	CDrawDevice* m_pDD;
	int m_fillmode;
	int m_sortents;
	int m_tilemode;
	int m_cvport;
	CDbSortentsTable* m_pSortingTable;
	CUCSIcon* m_pUCS;
	CGrid* m_pGrid;

	// Author: unknown
	// The default constructor, do not use any more
	CIcadEntityRenderer();
public:

	// Author: Denis Petrov
	// To perform output to all views of document (if second is NULL) or only one
	CIcadEntityRenderer(CIcadDoc* pDoc, CIcadView* pView = NULL);

	// Author: Denis Petrov
	// To perform output to specified view
	CIcadEntityRenderer(db_drawing* pDrawing, CIcadView* pView);

	// Author: unknown
	// The destructor
	~CIcadEntityRenderer();

	// Author: unknown
	// Returns view to which output is performed
    CIcadView* getIcadView();

	// Author: Denis Petrov
	// Temporary function for use in SDS_DrawEntity() only which will be removed
	void setDrawDevice(CDrawDevice* pDD);

	// Author: Denis Petrov
	// External API for output of UCS icon
	int drawGrid();

	// Author: Denis Petrov
	// External API for output of UCS icon
	int drawUCSIcon(sds_point vdir,
		const sds_real* sclfact, 
		int unpaint);

	// Author: Denis Petrov
	// External API for output of polyline (sds_grvecs())
	int drawVecs(const struct resbuf* pList, 
		sds_matrix transform);

	// Author: Denis Petrov
	// External API for output of line (sds_grdraw())
	int drawLine(const sds_point from, 
		const sds_point to, 
		int color, 
		int highlight, 
		int xor,
		bool bToAllBuffers);

	// Author: Denis Petrov
	// External API for output of entity (sds_redraw(name,))
    void drawEntity(db_handitem* pEntity,
		int mode,
		bool bRedraw,
		int width);

	// Author: unknown
	// Internal function for output of entity (sds_redraw(name,))
	// DP: TODO: hide into private part
    void DrawEntity(db_handitem* pEntity,
		struct gr_view* gView,
		POINT BltArea[2],
		int* FstPt,
		int xor,
		int hl,
		int undraw,
		int undrawcol,
		int norecurs,
		db_viewport* pViewportForVpLayerSettings,
		int width);

	// Author: unknown
	// External API function for output of whole drawing (sds_redraw(NULL,))
	int DrawDrawing(int iDoingVP,
		db_viewport *pViewport,
		struct gr_view *gViewVP);

	bool entityIsOnVpLayerFrozenLayer(db_handitem *pEntity, 
		db_viewport *pViewport, 
		db_drawing *pDrawing);

protected:
	virtual int drawEntitySpecial(struct gr_displayobject*& pSpecialDO,
		CDrawDevice* pDrawDevice,
		db_drawing* pDrawing, 
		struct gr_view* gView, 
		int hl,
		int xor, 
		int undraw);

	// Author: Denis Petrov
	// Returns true if draworder is on
	bool isSortingOn() const { if(m_sortents & (~0x03)) return true; else return false; }

private:
	int calcDrawmode(int highlight, 
		int xor, 
		int undraw);

	void drawImageObject(db_handitem *elp, 
		struct gr_view *gView, 
		db_drawing *flp, 
		int hl, int xor, 
		int undraw);

	void drawOLE2Frame(db_handitem *elp, 
		CDrawDevice *pDrawDevice,
		struct gr_view *gView);

	void drawViewPort(db_viewport *pViewport,
		struct gr_view *gView,
		POINT BltArea[2], 
		int *FstPt, 
		int xor, 
		int hl, 
		int undraw);

	void drawPrintViewport(int cvport);

	void SetDcFillColorMem(int color, 
		int hl, 
		int rop, 
		CDrawDevice *pDevice = NULL);
	
	void SetDcColorMem(int color, 
		int width, 
		int hl, 
		int rop, 
		CDrawDevice *pDevice = NULL);

	bool IsPrinting();
	
	void drawDisplayObjects(struct gr_displayobject *beg,
		int width,
		CDrawDevice *pDrawDevice,
		db_drawing *flp, 
		struct gr_view *gView,
		POINT BltArea[2],
		int *FstPt,
		int xor,
		int hl,
		int undraw,
		int undrawcol,
		int fillmode);

};

#endif
