// ********************************
// VMLDRAWDEVICE.H
//
// Has definition for the VMLDrawDevice, which object implements the functionality of the
// abstract BaseDrawDevice class, which is used for drawing DWGs (and other things).  See basedrawdevice.h.
//
//
#ifndef _VMLDRAWDEVICE_H
#define _VMLDRAWDEVICE_H

// forward declarations
//
class ofstream;

#ifndef _RASTERCONSTANTS_H
	#include "rasterconstants.h" /*DNT*/
#endif

#include "drawdevice.h" /*DNT*/

// ****************************
// class VMLDrawDevice
//
//
class VMLDrawDevice : public BaseDrawDevice
	{

	// ctors/dtors
	//
	public:
		VMLDrawDevice( LPCTSTR lpFileName );
		virtual ~VMLDrawDevice( void );



	// Public interface (virtual overrides)
	//
	public:

		virtual HDC  getHdc( void ); //Added by Vitaly Spirin (to use in drawDisplayObjects)
		virtual bool SetDrawMode( RasterConstants::DrawMode mode );
		virtual RasterConstants::DrawMode  GetDrawMode( void );
		virtual bool PolyBezier(const POINT* lppt, DWORD cPoints);	/*D.G.*/
		virtual bool Polyline( const POINT *lppt, int iPoints  );
		virtual bool Polygon( const POINT *lppt, int iPoints, bool onlyFill  );
		virtual bool TextOut( const int xCoord, const int yCoord, POINT BltArea[2], const CFont *font, int ACADcolor, const char *text );
		virtual bool SetPixel( int x, int y, int color );
		virtual bool SetLineColorACAD( int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID );
		virtual bool SetLineColorRGB( int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID );
		virtual bool SetLineColorXOR( int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID );
		virtual bool SetFillColorACAD( int color );
		virtual bool SetFillColorRGB( int color );
		virtual bool SetFillColorXOR( int color );

	// Virtual internal operations
	protected:

		// Each sub-class should verify that it's internals are in a valid state
		//
		virtual bool isValid( void );

		int		getLineColor( void );
		int		getLineWidth( void );
		int		getLineStyle( void );
		int		getFillColor( void );
		bool	setLineColor( int color, int width, RasterConstants::LineStyle style );
		bool	setFillColor( int color );

		bool	writeHeader();
		bool	writeEnd();

	// Internal data
	//
	private:
		ofstream	*m_pfile;
		int		m_iLineColor;
		int		m_iLineWidth;
		int		m_iLineStyle;

		int		m_iFillColor;

		RasterConstants::DrawMode		m_iDrawMode;




	};




#endif // _VMLDRAWDEVICE_H
