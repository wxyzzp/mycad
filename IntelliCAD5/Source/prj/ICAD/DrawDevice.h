// ************************
// BASEDRAWDEVICE.H
//
//
// Contains exported class definitions for a Draw Device Object, which is a polymorphic abstraction
// of a Windows DC, a printer, a file format or other device we might draw to.
//
//
#ifndef __DrawDeviceH__
#define __DrawDeviceH__

#ifdef SDS_ICADMFC
#else
#include <windows.h>
#endif

#ifndef _RASTERCONSTANTS_H
	#include "rasterconstants.h" /*DNT*/
#endif

#ifdef SDS_ICADMFC
#else
// *****************************************************
// ASSERT and VERIFY macros
//
//  For the moment I've chosen to NOT override MFC's versions if found
//
#if !defined( assert )
	#include <assert.h>
#endif


#if !defined( ASSERT )
	#if defined( NDEBUG )
		#define ASSERT( expr )		((void)0)
	#else
		#define ASSERT( expr )		assert( expr )
	#endif // defined( NDEBUG )
#endif // !defined( ASSERT )

#if !defined( VERIFY )
	#if defined( NDEBUG )
		#define VERIFY( expr ) 		((void)(expr))
	#else
		#define VERIFY( expr ) 		assert( expr )
	#endif // defined( NDEBUG )
#endif // !defined( VERIFY )

// *******************************************************
// POINTER CHECKING MACROS
// These macros depend on windows.h to be included.  Rather than set up the includes here,
// perhaps pulling windows into places we don't want it, the macro user had better have it where they use it.
//
#define CHECKDATA( p, l )		((p) && !IsBadReadPtr( (LPVOID)(p), l ) )
#define CHECKPTR( p )			CHECKDATA( p, 1 )
#define CHECKFUNCPTR( p ) 		((p) && !IsBadCodePtr( (FARPROC) (p) ) )
#define CHECKSTR( s )			((s) && !IsBadStringPtr( (LPCSTR)(s), 30000 ) )
#define CHECKSTRUCTPTR( p )		CHECKDATA( p, sizeof( *p ) ) 
#endif // SDS_ICADMFC

class CDC;
struct gr_view;

// Redesigned by Denis Petrov
// Description: This is an abstract base class for drawing devices
class CDrawDevice
{
	// Base functions (not overridden)
	//
public:
	enum EFlags
	{
		eSINGLE_BUFFER = 0x0001,
		eDOUBLE_BUFFER = 0x0002,
	};
	enum EDrawBuffer
	{
		eFRONT = 0x0001,
		eBACK = 0x0002
	};
	// IsValid first checks that the this pointer itself is okay, then
	// calls the virtual isValid() method on the object	so it can check
	// itself out.
	bool IsValid()
	{
		bool bRetval = false;
		if(CHECKPTR(this))
		{
			bRetval = this->isValid();
		}
		
		return bRetval;
	}
	
	// Public interface
	//
public:
	CDrawDevice(): m_hdc(NULL), m_pDC(NULL), m_lwdisplay(0), m_lwdefault(0) {}
	virtual	~CDrawDevice() {}

	// Author: Denis Petrov
	// Description: creates draw device
	virtual bool create(CDC* pDC, int flags = eDOUBLE_BUFFER, int width = 0, int height = 0, HPALETTE hPalette = NULL) { return false; }

	// Author: Denis Petrov
	// Description: resizes draw device
	virtual bool resize(CDC* pDC, int width, int height) { return false; }

	// Author: Denis Petrov
	// Description: destroys draw device
	virtual void destroy() {}

	// Author: Denis Petrov
	// Description: to be invoked before output to draw device
	virtual void beginDraw(CDC* pDC, gr_view* pView);

	// Author: Denis Petrov
	// Description: to be invoked after output to draw device
	virtual void endDraw() {}

	// Author: Denis Petrov
	// Description: returns whether draw device is 3D or 2D
	virtual bool is3D() const { return false; }

	// Author: Denis Petrov
	// Description: clears buffers associated with draw device
	virtual void clear() {}

	// Author: Denis Petrov
	// Description: fills rect in buffer associated with draw device 
	virtual void fillRect(const RECT& rect, int color) {}

	// Author: Denis Petrov
	// Description: copy to front buffer (i.e. screen) given rect
	virtual bool copyToFront(int x, int y, int width, int height) { return false; }

	// Author: Denis Petrov
	// Description: selects palette into draw device
	virtual HPALETTE selectPalette(HPALETTE hPalette, BOOL bForceBackground) { return NULL; }

	// Author: Denis Petrov
	// Description: realize palette on draw device
	virtual UINT realizePalette() { return 0; }
	
	// Author: Denis Petrov
	// Description: returns device context class for draw device
	virtual CDC* getCDC();

	// Author: Denis Petrov
	// Description: sets active buffer for double buffer draw devices
	virtual int setDrawBuffer(EDrawBuffer mode) { return 0; }

	 //Added by Vitaly Spirin (to use in drawDisplayObjects)
	virtual HDC  getHdc();
	virtual bool SetDrawMode(RasterConstants::DrawMode mode) = 0;
	virtual RasterConstants::DrawMode GetDrawMode(void) = 0;
	virtual bool PolyBezier(const POINT *lppt, DWORD cPoints) = 0;
	virtual bool Polyline(const POINT *lppt, int iPoints) = 0;
	virtual bool Polygon(const POINT *lppt, int iPoints, bool onlyFill) = 0;
	virtual bool SetPixel(int x, int y, int color) = 0;
	virtual bool SetLineColorACAD(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID) = 0;
	virtual bool SetLineColorRGB(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID) = 0;
	virtual bool SetLineColorXOR(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID) = 0;
	virtual bool SetFillColorACAD(int color) = 0;
	virtual bool SetFillColorRGB(int color) = 0;
	virtual bool SetFillColorXOR(int color) = 0;
	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	virtual bool SetLineWeight(int weight ) { return false; }
	
protected:
	HDC m_hdc;
	CDC* m_pDC;
	
	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	short m_lwdisplay;
	short m_lwdefault;

	// Each sub-class should verify that it's internals are in a valid state
	virtual bool isValid() = 0;
};

typedef CDrawDevice BaseDrawDevice;

#endif


