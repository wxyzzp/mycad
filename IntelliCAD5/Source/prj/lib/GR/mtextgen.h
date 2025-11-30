#ifndef __mtextgenH__
#define __mtextgenH__

#include "sds.h"
#include "textgen.h"

/*-------------------------------------------------------------------------
 CMtextFragment Structure
 --------------------------------------------------------------------------*/

//const int MTEXTPARSE_BUFSIZE = 8192;
const int MTEXTPARSE_BUFSIZE = 16384;
class CMtextFragment
{
public:
	enum EFragmentType
	{
		eDEFAULT,
		eSTACKED,
		eWITH_STACKED
	};
	int m_firstChar;
	int m_length;
	int m_nLine;
	sds_real m_size[2];
	EFragmentType m_fragmentType;
	CTextFragmentStyle m_style;
	static char s_buffer[MTEXTPARSE_BUFSIZE];
	static int s_bufferLength;
};
typedef std::vector<CMtextFragment>		MTEXTFRAGMENTS_VECTOR;
typedef std::stack<CTextFragmentStyle>	TEXTFRAGMENTSTYLE_STACK;

/*-------------------------------------------------------------------------
 Service Function in mtextgen.cpp
 Most functions share with explodemtext.cpp
 --------------------------------------------------------------------------*/
int rotatePoint(
 sds_point destination,
 sds_point source,
 sds_real angleSin,
 sds_real angleCos,
 char axis='Z');

int getFontName(
 char* pString,
 int length,
 CTextFragmentStyle& textStyle,
 db_drawing* pDrawing,
 HDC hDC);

int getValue(
 char* pString,
 int length,
 sds_real& value);

int gr_mtextparse(
 MTEXTFRAGMENTS_VECTOR& fragments,
 CDbMText* pMtext,
 db_drawing* pDrawing,
 CDC* pDC);

int getMtextFragmentBB(
 db_text& text,
 CTextFragmentStyle& textStyle,
 sds_point& bmin,
 sds_point& bmax,
 gr_view* pView,
 db_drawing* pDrawing,
 CDC* pDC);

int generateMtextFragment(
 gr_displayobject*&		pDOListBegin,
 gr_displayobject*&		pDOListEnd,
 sds_point&				offset,
 db_text&				text,
 CTextFragmentStyle&	textStyle,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
);

int generateMtextLine(
 gr_displayobject*&		pDOListBegin,
 gr_displayobject*&		pDOListEnd,
 sds_point&				insertionPoint,
 sds_point&				lineOffset,
 sds_real*				lineSize,
 db_text&				text,
 MTEXTFRAGMENTS_VECTOR&	fragments,
 int					startIndex,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
);

int getLineRect(
 sds_real*				size,
 MTEXTFRAGMENTS_VECTOR&	fragments,
 sds_real&				spacing,
 sds_real&				lineSpacing,
 int					lineDirection,
 int					nLine,
 int					index = 0
);

bool mtextReorder(
 MTEXTFRAGMENTS_VECTOR&	mtextFrags,		// <=>
 db_drawing&			drw,			// =>
 CDC&					dc				// =>
);

bool BindMtextFragments(
 MTEXTFRAGMENTS_VECTOR&	mtextFrags		// <=>
);

typedef std::vector<long>	LONG_VECTOR;

int
explodeMtextFragment
(
 sds_point&				offset,
 db_text&				text,
 CTextFragmentStyle&	textStyle,
 LONG_VECTOR&			ssetnew,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
);

int
explodeMtextLine
(
 sds_point&				insertionPoint,
 sds_point&				lineOffset,
 sds_real*				lineSize,
 db_text&				text,
 MTEXTFRAGMENTS_VECTOR&	fragments,
 int					startIndex,
 LONG_VECTOR&			ssetnew,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
);

/*-------------------------------------------------------------------------*//**
Forms display objects of MTEXT entity.

@author Denis Petrov
@param pDOListBegin <= begin of display objects list for MTEXT
@param pDOListBegin <= begin of display objects list for MTEXT
@param pView =>
@param pDrawing =>
@param pDC =>
@return 1 for success, 0 for error
*//*--------------------------------------------------------------------------*/

int gr_mtextgen(
 gr_displayobject*& pDOListBegin,
 gr_displayobject*& pDOListEnd,
 gr_view* pView,
 db_drawing* pDrawing,
 CDC* pDC);

#endif
