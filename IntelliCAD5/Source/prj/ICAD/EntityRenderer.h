#ifndef __EntityRendererH__
#define __EntityRendererH__

// ****************************************************************
// class CEntityRenderer
//
// A generic class used to keep entity drawing functionality
// together
//
#ifdef SDS_ICADMFC
#include <afxwin.h>

// Copied here from IcadDef.h
#define CHECKDATA( p, l )		((p) && !IsBadReadPtr( (LPVOID)(p), l ) )
#define CHECKPTR( p )			CHECKDATA( p, 1 )
#else
#include <windows.h>
#endif

// enable/disable old sds_grdraw() handler
#undef _SDS_GRDRAW_
// enable/disable old sds_grvecs() handler
#undef _SDS_GRVECS_
// enable/disable old UCS icon rendering
#undef _SDS_UCSICON_
// enable/disable old grid rendering
#undef _SDS_GRID_
// enable/disable old cursor rendering
#undef _SDS_CURSOR_
// enable/disable OpenGL draw (_USE_DISP_OBJ_PTS_3D_ should be also defined)
#undef _USE_OPENGL_

// these necessary for proper OpenGL rendering
#ifdef _USE_OPENGL_
#undef _SDS_GRDRAW_
#undef _SDS_GRVECS_
#undef _SDS_UCSICON_
#undef _SDS_GRID_
#undef _SDS_CURSOR_
#endif

#include "DrawDevice.h"
class db_handitem;
class db_viewport;
class db_drawing;

class CEntityRenderer
{
protected:
	void outputPolygon(int ndims, 
		int face, 
		struct gr_displayobject *tdo1, 
		CDrawDevice *pDrawDevice,	
		struct gr_view *gView, 
		POINT BltArea[2],
		int *FstPt,
		int undraw,
		int undrawcol,
		int rop,
		int hl,
		int width,
		bool onlyFill);
	
	/********************************************************************************
	* Author:	Dmitry Gavrilov
	*
	* Purpose:	Draw one or more Bezier curves defined by points in pDispObj linked list.
	*
	* Returns:	None.
	********************************************************************************/
	void outputPolyBezier(int ndims,
		int	face,
		struct gr_displayobject* pDispObj,
		CDrawDevice*	pDrawDevice,
		struct gr_view* pView,
		POINT BltArea[2],
		int* pFirstPt,
		int	undraw,
		int	undrawcol,
		int	rop,
		int	hl,
		int	width);
				
	void outputPolyline(int ndims, 
		int face, 
		struct gr_displayobject *tdo1, 
		CDrawDevice *pDrawDevice,	
		struct gr_view *gView, 
		POINT BltArea[2],
		int *FstPt,
		int undraw,
		int undrawcol,
		int rop,
		int hl,
		int width );
				
	void drawDisplayObjects(struct gr_displayobject *beg,
		int width,
		CDrawDevice *pDrawDevice,	
		db_drawing *pDrawing, 
		struct gr_view *gView,
		POINT BltArea[2],
		int *FstPt,
		int xor,
		int hl,
		int undraw,
		int undrawcol,
		int fillmode);
				
	virtual void DrawEntity(db_handitem *elp,
		CDrawDevice* pDrawDevice, 
		db_drawing* pDrawing, 
		struct gr_view* gView,
		POINT BltArea[2],
		int *FstPt,
		int xor,
		int hl,
		int undraw,
		int undrawcol,
		int fillmode,
		int norecurs,
		db_viewport *pViewportForVpLayerSettings, 
		bool bRedraw,
		int width=1);

/*-------------------------------------------------------------------------*//**
Draws special display objects depending on source entity type

@author Denis Petrov
@param pSpecialDO <=> start of the special display objects list, end of the special list
@param pDrawDevice => draw device class to use 
@param pDrawing => drawing to use
@param gView => view data to use
@param hl => highlighted
@param xor => xor-ed
@param undraw => undraw
@return 1 for success, 0 for error
*//*------------------------------------------------------------------------*/
	virtual int drawEntitySpecial(struct gr_displayobject*& pSpecialDO,
		CDrawDevice* pDrawDevice,
		db_drawing* pDrawing, 
		struct gr_view* gView, 
		int hl,
		int xor, 
		int undraw) { return 0; }

	virtual bool IsPrinting() {return false;} // may be overridden by subclass method
				
private:
	enum EPointsBufferSize
	{
		ePointsBufferSize = 100,
	};
	static POINT s_PointsBuffer[ePointsBufferSize];

	void drawImageObject(db_handitem *elp, 
		struct gr_view *gView, 
		db_drawing *pDrawing, 
		int hl, 
		int xor, 
		int undraw );
	
	void drawOLE2Frame(db_handitem *elp, 
		CDrawDevice *pDrawDevice,	
		struct gr_view *gView);
	
	void drawViewPort(db_viewport *pViewport,
		CDrawDevice *pDrawDevice,	
		struct gr_view *gView,
		db_drawing *pDrawing, 
		POINT BltArea[2], 
		int *FstPt, 
		int xor, 
		int hl, 
		int undraw, 
		int fillmode);
	
	virtual void SetDcFillColorMem(int color, 
		int hl, 
		int rop, 
		CDrawDevice *pDevice = NULL);
	
	virtual void SetDcColorMem(int color, 
		int width, 
		int hl, 
		int rop, 
		CDrawDevice *pDevice = NULL);		
};

#endif
