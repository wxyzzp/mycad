/* File  : <DevDir>\source\prj\lib\gr\explodemtext.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Mtext specific functions
 */

#include "gr.h"/*DNT*/
#include "gr_view.h"/*DNT*/
#include "textgen.h"/*DNT*/
#include "truetypeutils.h"/*DNT*/
#include "styletabrec.h"/*DNT*/
#include "TTFInfoGetter.h"/*DNT*/
#include <stack>
#include <vector>
//#include "DBCSfontname.h"/*DNT*/
#include "dwgcodepage.h"/*DNT*/
#include "gr_parsetext.h"/*DNT*/
#include "DbMText.h"
#include "mtextgen.h"/*DNT*/
#include "gr_wordwrap.h"/*DNT*/

/* -------------------------------------------------------------------- */
/* Explode MTEXT        EBATECH(CNBR) 04/02/2002                        */
/* -------------------------------------------------------------------- */
//#define TRIAL_EXPLODE_MTEXT
#ifndef TRIAL_EXPLODE_MTEXT
GR_API int ExplodeMtext(
  sds_name          ename,
  LONG_VECTOR&      ssetnew,
  gr_view*			pView,
  db_drawing*		pDrawing,
  CDC*				pDC
){
	ssetnew.resize(0);
	return 0;
}
#else
GR_API int ExplodeMtext(
  sds_name          ename,
  LONG_VECTOR&      ssetnew,
  gr_view*			pView,
  db_drawing*		pDrawing,
  CDC*				pDC
){
	int rc = 0;
	pView->ment = (db_handitem*)(ename[0]);
	db_mtext*	pMtext = (db_mtext*)pView->ment;
	ssetnew.resize(0);
	
	// form transformation matrix (from ECS).
	sds_point	entaxis[3];
	pMtext->get_210(entaxis[2]);
	sds_real	vectorLength =	entaxis[2][0] * entaxis[2][0] +
								entaxis[2][1] * entaxis[2][1] +
								entaxis[2][2] * entaxis[2][2];

	if(isEqualZeroWithin(vectorLength - 1.0))
	{
		vectorLength = sqrt(vectorLength);
		entaxis[2][0] /= vectorLength;
		entaxis[2][1] /= vectorLength;
		entaxis[2][2] /= vectorLength;
	}

	pMtext->get_11(entaxis[0]);
	vectorLength =	entaxis[0][0] * entaxis[0][0] +
					entaxis[0][1] * entaxis[0][1] +
					entaxis[0][2] * entaxis[0][2];

	if(isEqualZeroWithin(vectorLength - 1.0))
	{
		vectorLength = sqrt(vectorLength);
		entaxis[0][0] /= vectorLength;
		entaxis[0][1] /= vectorLength;
		entaxis[0][2] /= vectorLength;
	}

	ic_crossproduct(entaxis[2], entaxis[0], entaxis[1]);

	CMatrix4 entityTransformation;
	entityTransformation(0,0) = entaxis[0][0];
	entityTransformation(1,0) = entaxis[0][1];
	entityTransformation(2,0) = entaxis[0][2];
	entityTransformation(0,1) = entaxis[1][0];
	entityTransformation(1,1) = entaxis[1][1];
	entityTransformation(2,1) = entaxis[1][2];
	entityTransformation(0,2) = entaxis[2][0];
	entityTransformation(1,2) = entaxis[2][1];
	entityTransformation(2,2) = entaxis[2][2];
	entityTransformation(3,3) = 1.0;
	pView->m_transformation = pView->m_transformation * entityTransformation;
	// get back transformation matrix (to ECS).
	//CMatrix4 invertTransformation;
	//invertTransformation = entityTransformation.invert();

	// Chinese/Japanese word wrap pre-process from dispobjs.cpp
	char*	str_in_base = NULL;					//VS
	char*	str_bak = NULL;						//VS
	char*	str_with_slashes = NULL;			//VS

	pMtext->get_1(&str_in_base);
	if(str_in_base == NULL )
		return 0;

	str_bak = new char[strlen(str_in_base) + 1];
	if(str_bak == NULL )
		return 0;
	strcpy(str_bak, str_in_base);
	str_with_slashes = cjk_wordwrap((unsigned char*)str_bak, pDrawing);
	pMtext->set_1(str_with_slashes);
	// Parse MTEXT String into fragments
	MTEXTFRAGMENTS_VECTOR	mtextFragments;
	int						nLines = gr_mtextparse(mtextFragments, pMtext, pDrawing, pDC);
	// Restore MTEXT String for error termination
	pMtext->set_1(str_bak);
	delete str_with_slashes;
	delete str_bak;
	// Chinese/Japanese word wrap pre-process from dispobjs.cpp

	if(!nLines)
		return 0;

	// temporary DB object to pass to gr_textgen()
	db_text			mtextFragment;
	// pointer to MTEXT style object
	db_styletabrec*	pTextStyle = NULL;
	pMtext->get_7((db_handitem **)(&pTextStyle));

	// initialization of data
	int	attachmentPoint,
		textHAlign, textVAlign;
	// MTEXT DXF=71 TL=1 TC=2 TR=3 ML=4 MC=5 MR=6 BL=7 BC=8 BR=9
	pMtext->get_71(&attachmentPoint);
	assert(attachmentPoint > 0 &&  attachmentPoint < 10);

	/*
	**	gr_textgen() needs the llpt (10) of a text item.
	**	We are going to get each line's ATTACHMENT pt (11)
	**	by offsetting insertPoint.	Therefore, we need to call
	**	gr_fixtextjust() to set the llpt for us.
	**	This requires a db_text object.
	**	Let's fill it in with the necessary values --
	**	except for pMtextString, alignmentPoint, and llpt, which will vary as we go,
	**	line by line:
	*/
	// textHAlign same as TEXT DXF=72 Left=0 Center=1 Right=2 (Aligned=3 Middle=4 Fit=5)
	textHAlign = (attachmentPoint - 1) % 3;
	mtextFragment.set_72(textHAlign);
	// textVAlign same as TEXT DXF=73 Top=3 Middle=2 Baseline=0 (Bottom=1)
	textVAlign = 3 - (attachmentPoint - 1) / 3;
	if(textVAlign == 1)
		textVAlign = 0;
	mtextFragment.set_73(textVAlign);
	// new textVAlign set to Top=0 Middle=1 Bottom=2
	textVAlign = (attachmentPoint - 1) / 3;
	// EBATECH(CNBR) -[ 2002-4-2
	// pTextStyle always is not NULL, m_style.gen doesn't set at gr_mtextparse().
	// rotation always is zero.
	// mtextFragment.set_50(mtextFragments.at(0).m_style.m_rotation);
	//if(pTextStyle)
	//{
	//	mtextFragment.set_7(pTextStyle);
	//	mtextFragment.set_71(mtextFragments.at(0).m_style.gen);
	//}
	mtextFragment.set_7((db_handitem *)pTextStyle);
	// EBATECH(CNBR) ]-

	int			lineDirection, linesDirection;
	sds_real	mtextSize[2] = {0.0, 0.0},
				initialTextHeight = 0.0;
	// Exploding MTEXT take effect only STYLE vertical setting.
	// Displaying MTEXT take effect both MTEXT flag and STYLE setting.
	pTextStyle->get_70(&lineDirection);
	lineDirection = (lineDirection & IC_SHAPEFILE_VERTICAL) ? 1 : 0;
	linesDirection = 1 - lineDirection;

	pMtext->get_40(&initialTextHeight);
	pMtext->get_41(&mtextSize[lineDirection]);
	if(mtextSize[lineDirection] <= 0.0 )
		mtextSize[lineDirection] = 0.0;

	sds_point	insertPoint, insertPointWCS;
	pMtext->get_10(insertPointWCS);

	// transform insertion point by inverse MTEXT matrix
	insertPoint[0] = entaxis[0][0] * insertPointWCS[0] + entaxis[0][1] * insertPointWCS[1] + entaxis[0][2] * insertPointWCS[2];
	insertPoint[1] = entaxis[1][0] * insertPointWCS[0] + entaxis[1][1] * insertPointWCS[1] + entaxis[1][2] * insertPointWCS[2];
	insertPoint[2] = entaxis[2][0] * insertPointWCS[0] + entaxis[2][1] * insertPointWCS[1] + entaxis[2][2] * insertPointWCS[2];

	sds_real	spacingFactor = 0.0, lineSpacing = 0.0, lineSpacingFactor = 0.0;

	pMtext->get_44(&lineSpacingFactor);
	if(lineSpacingFactor <= 0.0)
		lineSpacingFactor = 1.0;

	/* DP: According to commentaries lffact member of db_fontlink must be used for
	calculating of line spacing but really lffact is not correct member. In previous
	realization of MTEXT generation was used next formula: below / above

	if(lineSpacingFactor)
		lineSpacingFactor *= mtextFragments.at(0).m_style.m_pFont->lffact;
	else
		lineSpacingFactor = mtextFragments.at(0).m_style.m_pFont->lffact;
	*/

	/*D.G.*/// Re-order mtext if necessary.
	// mtextReorder() don't touch the following member variables: mtextFragments.at(i). m_size[2], m_style.m_insertionPoint
	if(mtextFragments.size() > 1 && !mtextReorder(mtextFragments, *pDrawing, *pDC))
		assert(false);

	// compute bounding boxes of mtext fragments
	int			i;
	sds_point	bbmin, bbmax;

	for(i = mtextFragments.size(); i--; )
	{
		mtextFragments.at(i).m_style.val = CMtextFragment::s_buffer + mtextFragments.at(i).m_firstChar;
		// Force to set vert, m_pBigfont, m_pFont same as TEXT STYLE, m_spaceFactor set zero.
		mtextFragments.at(i).m_style.vert = lineDirection;
		mtextFragments.at(i).m_style.m_pBigfont = pTextStyle->ret_bigfont();
		mtextFragments.at(i).m_style.m_pFont    = pTextStyle->ret_font();
		mtextFragments.at(i).m_style.m_spaceFactor = 0.0; // Zero means no change font's factor.
		//VS: I changes algorithm of bounding box calculation (gr_textgen()) and remove trick
		// getMtextFragmentBB() don't modify mtextFragment and m_style, change bbmin and bbmax.
		if(getMtextFragmentBB(mtextFragment, mtextFragments.at(i).m_style, bbmin, bbmax, pView, pDrawing, pDC))
		{
			mtextFragments.at(i).m_size[0] = bbmax[0] - bbmin[0];
			mtextFragments.at(i).m_size[1] = bbmax[1] - bbmin[1];
		}
	}

	// wrap lines if need
	int			nLine = 1;
	sds_real	lineSize[2] = {0.0, 0.0};
	int			j = 0, k = 0;

	// EBATECH(CNBR) -[ 2001/12/9 Vertical Word Wrap
	// Now m_nLine is temp value, m_size[2] has true width and height, m_style.m_insertionPoint has still zero(s).
	if(!icadRealEqual(mtextSize[lineDirection],0.0))
	{
		for(i = 0 ; i < mtextFragments.size(); )
		{
			if(nLine == mtextFragments.at(i).m_nLine)
			{
				lineSize[lineDirection] += mtextFragments.at(i).m_size[lineDirection];
				if( lineSize[lineDirection] > mtextSize[lineDirection] &&
					((i > 0 && mtextFragments.at(i).m_fragmentType != CMtextFragment::EFragmentType::eWITH_STACKED &&
					mtextFragments.at(i - 1).m_nLine == mtextFragments.at(i).m_nLine) ||
					(i > 1 && mtextFragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED &&
					mtextFragments.at(i - 2).m_nLine == mtextFragments.at(i).m_nLine)) )
				{
					// When Word Wrap is required, rest mtextFragments records increase m_nLine (move next line).
					j = mtextFragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED ? i - 1 : i;

					for( ; j < mtextFragments.size() /*&& mtextFragments.at(j).m_nLine == nLine*/ ; ++j)
					{
						++(mtextFragments.at(j).m_nLine);
					}

					lineSize[lineDirection] = 0.0;
					i = k;
					continue;
				}
			}
			else
			{
				++nLine;
				k = i;
				lineSize[lineDirection] = mtextFragments.at(i).m_size[lineDirection];
			}
			++i;
		}
	}
	// EBATECH(CNBR) ]-
	// Now m_nLine is true value, don't change m_size[2],  m_style.m_insertionPoint has still zero(s).

	// EBATECH(CNBR) -[ 2002/4/2 Bind fragments for speed
	if( mtextFragments.size() > 1 )
	{
		if( BindMtextFragments( mtextFragments ) == true )
		{
			for(i = mtextFragments.size(); i--; )
			{
				//VS: I changes algorithm of bounding box calculation (gr_textgen()) and remove trick
				if(getMtextFragmentBB(mtextFragment, mtextFragments.at(i).m_style, bbmin, bbmax, pView, pDrawing, pDC))
				{
					mtextFragments.at(i).m_size[0] = bbmax[0] - bbmin[0];
					mtextFragments.at(i).m_size[1] = bbmax[1] - bbmin[1];
				}
			}
		}
	}
	// EBATECH(CNBR) ]-

	// accumulate height of MTEXT
	i = 0;
	nLine = 1;

	// EBATECH(CNBR) -[ 2001/12/9 Support Fixed Line Spacing
	int fixedLineSpacing; // 2:Fixed 1:Variable(Default)
	sds_real initialLineSpacing;
	pMtext->get_73(&fixedLineSpacing);
	initialLineSpacing = initialTextHeight * mtextFragments.at(0).m_style.m_pFont->lffact * spacingFactor;

	while(i < mtextFragments.size())
	{
		// EBATECH(CNBR) -[ 2001/12/10 Line Rectangle was half size when VAlign is Mid or Bottom.
		//i += getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);
		// (o) lineSize : width & height of nLine th fragments. don't change m_size[2], m_insertionPoint is still zero(s).
		i = getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);
		// EBATECH(CNBR) ]-
		++nLine;
		if( fixedLineSpacing != 2 )	// Variable Line Pitch (Default setting)
		{
			mtextSize[linesDirection] += lineSize[linesDirection];
		}
		else // Fixed Line Pitch (since R14 alternative setting)
		{
			mtextSize[linesDirection] += initialLineSpacing;
		}
	}
	// Now mtextSize[linesDirection] is MTEXT extents size. mtextSize[lineDirection] is still MTEXT limitation size.
	// EBATECH(CNBR) ]-

	//EBATECH(CNBR) -[ 2002/4/2
	// it's seems what we don't need to calculate rotation because we obtain ECS already rotated
	sds_point alignOffset = {0.0, 0.0, 0.0};
	//sds_point alignOffset = {0.0, 0.0, 0.0}, realAlignOffset = {0.0, 0.0, 0.0};
	// EBATECH(CNBR) ]-

	if( lineDirection ) // Vertical
	{
		//                             0:Bottom is don't change offset value.
		if(textVAlign == 1)         // 1:Middle sets the half size of limitation.
			alignOffset[lineDirection] = -0.5 * mtextSize[lineDirection];
		else if(textVAlign == 2)    // 2:Top sets the full size of limitation
				alignOffset[lineDirection] = - mtextSize[lineDirection];
		//                             0:Left is don't change offset value.
		//if(textHAlign == 1)         // 1:Center sets the half size of extents
		//	alignOffset[linesDirection] = 0.5 * mtextSize[linesDirection];
		//else if(textHAlign == 2)    // 2:Right set size of extents
		//		alignOffset[linesDirection] = mtextSize[linesDirection];
	}
	else	// Horizontal
	{
		//                             0:Bottom is don't change offset value.
		if(textVAlign == 1)         // 1:Middle sets the half size of limitation.
			alignOffset[linesDirection] = 0.5 * mtextSize[linesDirection];
		else if(textVAlign == 2)    // 2:Top sets the full size of limitation
				alignOffset[linesDirection] = mtextSize[linesDirection];
		//                             0:Left is don't change offset value.
		if(textHAlign == 1)         // 1:Center sets the half size of extents
			alignOffset[lineDirection] = - 0.5 * mtextSize[lineDirection];
		else if(textHAlign == 2)    // 2:Right set size of extents
				alignOffset[lineDirection] = - mtextSize[lineDirection];
	}
	//EBATECH(CNBR) -[ 2002-4-2
	// it's seems what we don't need to calculate rotation because we obtain ECS already rotated
	//rotatePoint(realAlignOffset, alignOffset, sin(mtextFragments.at(0).m_style.m_rotation), cos(mtextFragments.at(0).m_style.m_rotation));
	insertPoint[0] += alignOffset[0];
	insertPoint[1] += alignOffset[1];
	//insertPoint[0] += realAlignOffset[0];
	//insertPoint[1] += realAlignOffset[1];
	//EBATECH(CNBR) ]-

	//sds_point	lineOffset = {0.0, -initialTextHeight, 0.0};
	sds_point	lineOffset = {0.0,0.0,0.0};
	if( lineDirection ) // Vertical
	{
		lineOffset[linesDirection] = initialTextHeight;
	}
	else	// Horizontal
	{
		lineOffset[linesDirection] = -initialTextHeight;
	}
	j = i = 0;
	nLine = 1;
	// EBATECH(CNBR) -[
	//lineSize[1] = initialTextHeight;
	//lineSize[0] = 0.0;
	lineSize[linesDirection] = initialTextHeight;
	lineSize[lineDirection] = 0.0;
	// ]-
	while(i < mtextFragments.size())
	{
		if(i == j)
		{
			if( lineDirection ) // Vertical
			{
				// at(i) - begin of new line
				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection) * (mtextFragments.at(j).m_nLine - nLine - 1);

				nLine = mtextFragments.at(j).m_nLine;
				j = getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);

				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection);

				lineOffset[lineDirection] = 0.0;
				if(textVAlign == 1)
					lineOffset[lineDirection] -= 0.5 * (mtextSize[lineDirection] - lineSize[lineDirection]);
				else
					if(textVAlign == 2)
						lineOffset[lineDirection] -= (mtextSize[lineDirection] - lineSize[lineDirection]);
				lineSize[linesDirection] = mtextSize[linesDirection];
			}
			else // Horizontal
			{
				// at(i) - begin of new line
				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection) * (mtextFragments.at(j).m_nLine - nLine - 1);

				nLine = mtextFragments.at(j).m_nLine;
				j = getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);

				lineOffset[lineDirection] = 0.0;
				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection);

				if(textHAlign == 1)
					lineOffset[lineDirection] += 0.5 * (mtextSize[lineDirection] - lineSize[lineDirection]);
				else
					if(textHAlign == 2)
						lineOffset[lineDirection] += (mtextSize[lineDirection] - lineSize[lineDirection]);
				lineSize[lineDirection] = mtextSize[lineDirection];
			}
		}

		// try to explode line
		k = explodeMtextLine(insertPoint, lineOffset, lineSize,
							  mtextFragment, mtextFragments, i, ssetnew, pView, pDrawing, pDC);
		if(k)
		{
			if(k < j)
				assert(false);
			else
				i = k;
		}
		else
		{
			assert(false);
			return 0;
		}

	}
	return 1;
}

/*-------------------------------------------------------------------------*//**
Forms TEXT object of given MTEXT fragment using gr_textgen().

@author Chikahiro Masami
@param offset => offset of given fragment from the insertion point of MTEXT
@param invertTransformation => Transformation Matrix to ECS points
@param text => temporary DB object to pass to gr_textgen()
@param textStyle => style of fragment
@param ssetnew <= store a pointer of TEXT result buffer
@param pView =>
@param pDrawing =>
@param pDC =>
@return 1 for success, 0 for error or allowed width or height less then real width or height

*//*--------------------------------------------------------------------------*/
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
)
{
	int			result = 0;
	db_mtext*	pMtext = (db_mtext*)pView->ment;

	if(textStyle.m_modifiers & CTextFragmentStyle::EModifiers::eUNDERLINED)
	{
		*(--textStyle.val) = 'U';
		*(--textStyle.val) = '%';
		*(--textStyle.val) = '%';
	}

	if(textStyle.m_modifiers & CTextFragmentStyle::EModifiers::eOVERLINED)
	{
		*(--textStyle.val) = 'O';
		*(--textStyle.val) = '%';
		*(--textStyle.val) = '%';
	}

	// generate text fragment
	sds_resbuf *rp = NULL;
	char* stylep=NULL;
	char* layerp=NULL;
	sds_real rotation;
	sds_point	p11, vtaxis;

	pMtext->get_7(&stylep);
	pMtext->get_8(&layerp);
	pMtext->get_50(&rotation, pDrawing);
	//textStyle.m_rotation = rotation;
	//text.set_50(rotation);
	pMtext->get_210(vtaxis);
	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	int lineweight;
	pMtext->get_370(&lineweight);
	//EBATECH(CNBR)]-

	// it's seems what we don't need to calculate rotation because we obtain ECS already rotated
	sds_point realOffset;
	rotatePoint(realOffset, offset, sin(rotation), cos(rotation));
	textStyle.m_insertionPoint[0] += realOffset[0];
	textStyle.m_insertionPoint[1] += realOffset[1];
	textStyle.m_insertionPoint[2] += realOffset[2];
	//textStyle.m_insertionPoint[0] += offset[0];
	//textStyle.m_insertionPoint[1] += offset[1];
	text.set_11(textStyle.m_insertionPoint);
	text.set_10(textStyle.m_insertionPoint);
	p11[0] = p11[1] = p11[2] = 0.0;

	if(!gr_fixtextjust(&text, pDrawing, pView, pDC, textStyle.vert))
	{
		text.get_10(textStyle.m_insertionPoint);

		if( rp = sds_buildlist(
			RTDXF0, "TEXT",
			67, pMtext->ret_ent_pspace(),
			8, layerp,
			62, textStyle.m_color,
			370, lineweight,
			10, textStyle.m_insertionPoint,
			40, textStyle.m_height,
			1, textStyle.val,
			50, rotation,
			41, textStyle.m_widthFactor,
			51, textStyle.m_obliqueAngle,
			7, stylep,
			71, textStyle.gen,
			72, 0,
			11, p11,
			210, vtaxis,
			73, 0,
			0 ))
		{
			ssetnew.push_back((long)rp);
			result = 1;
		}
	}
	return result;
}

/*-------------------------------------------------------------------------*//**
Create array of TEXT result buffer pointer for each line using explodeMtextFragment().

@author Denis Petrov
@param insertionPoint => insertion point of MTEXT
@param lineOffset => offset of given line from the insertion point of MTEXT
@param invTransform => Transformation Matrix to ECS points
@param lineSize => sizes of line (width and height) for correct alignment
@param text => temporary DB object to pass to gr_textgen()
@param fragments => vector of all MTEXT fragments
@param startIndex => start index of line in fragments vector
@param pView =>
@param pDrawing =>
@param pDC =>
@return index of fragment following last generated for success, 0 for error or allowed width or height less then real width or height
*//*--------------------------------------------------------------------------*/
int
explodeMtextLine
(
 sds_point&				insertionPoint, // alignOffset
 sds_point&				lineOffset,
 sds_real*				lineSize,
 db_text&				text,
 MTEXTFRAGMENTS_VECTOR&	fragments,
 int					startIndex,
 LONG_VECTOR&			ssetnew,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
)
{
	// this will be used to store display objects of MTEXT fragments
	sds_point			currentFragmentOffset;
	int					i = startIndex;

	fragments.at(i).m_style.m_insertionPoint[0] = insertionPoint[0];
	fragments.at(i).m_style.m_insertionPoint[1] = insertionPoint[1];
	fragments.at(i).m_style.m_insertionPoint[2] = insertionPoint[2];
	fragments.at(i).m_style.val = CMtextFragment::s_buffer + fragments.at(i).m_firstChar;
	currentFragmentOffset[0] = lineOffset[0];
	currentFragmentOffset[1] = lineOffset[1];
	currentFragmentOffset[2] = lineOffset[2];

	if(fragments.at(i).m_style.m_alignment == 1)
	{
		if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
			currentFragmentOffset[1] += 0.5 * (lineSize[1] - fragments.at(i).m_style.m_height);
		else
			currentFragmentOffset[1] += 0.5 * lineSize[1] - fragments.at(i).m_style.m_height;
	}
	else
	{
		if(fragments.at(i).m_style.m_alignment == 2)
		{
			if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
				currentFragmentOffset[1] += lineSize[1] - fragments.at(i).m_style.m_height;
			else
				currentFragmentOffset[1] += lineSize[1] - 2.0 * fragments.at(i).m_style.m_height;
		}
	}

	if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED)
		currentFragmentOffset[1] += fragments.at(i).m_style.m_height * (1.0 + 
		static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
		static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));

	while(fragments.at(startIndex).m_nLine == fragments.at(i).m_nLine)
	{
		if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED &&
			(fragments.at(i).m_style.m_modifiers & CTextFragmentStyle::EModifiers::eUNDERLINED) &&
			i + 1 < fragments.size() &&
			fragments.at(i + 1).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED &&
			(fragments.at(i + 1).m_style.m_modifiers & CTextFragmentStyle::EModifiers::eOVERLINED))
		{
			if(fragments.at(i + 1).m_size[0] > fragments.at(i).m_size[0])
				fragments.at(i).m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eUNDERLINED;
			else
				fragments.at(i + 1).m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eOVERLINED;
		}

		explodeMtextFragment(currentFragmentOffset, text, fragments.at(i).m_style, 
			ssetnew, pView, pDrawing, pDC);

		++i;

		if(i >= fragments.size())
			break;

		// prepare to next fragment
		fragments.at(i).m_style.m_insertionPoint[0] = insertionPoint[0];
		fragments.at(i).m_style.m_insertionPoint[1] = insertionPoint[1];
		fragments.at(i).m_style.m_insertionPoint[2] = insertionPoint[2];
		fragments.at(i).m_style.val = CMtextFragment::s_buffer + fragments.at(i).m_firstChar;
		currentFragmentOffset[2] = lineOffset[2];

		switch(fragments.at(i).m_fragmentType)
		{
		case CMtextFragment::EFragmentType::eDEFAULT :
			if(fragments.at(i).m_style.vert)
			{
				currentFragmentOffset[1] -= fragments.at(i - 1).m_size[1];
				currentFragmentOffset[0] = lineOffset[0];
			}
			else
			{
				currentFragmentOffset[1] = lineOffset[1];
				currentFragmentOffset[0] +=	fragments.at(i - 1).m_size[0];
				if(i > 1 && fragments.at(i - 2).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED &&
					fragments.at(i - 1).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED)
				{
					currentFragmentOffset[0] -=	0.5 * (fragments.at(i - 2).m_size[0] - fragments.at(i - 1).m_size[0]);
					if(fragments.at(i - 2).m_size[0] > fragments.at(i - 1).m_size[0])
						currentFragmentOffset[0] += fragments.at(i - 2).m_size[0] - fragments.at(i - 1).m_size[0];
				}
			}
			break;

		case CMtextFragment::EFragmentType::eSTACKED :
			if(i > 1 && fragments.at(i - 1).m_style.vert)
			{
				currentFragmentOffset[1] -= fragments.at(i - 1).m_size[1];
				currentFragmentOffset[0] = lineOffset[0];
				currentFragmentOffset[1] -= fragments.at(i).m_style.m_height * (1.0 + static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
					static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));
			}
			else
			{
				currentFragmentOffset[1] = lineOffset[1];
				currentFragmentOffset[1] += fragments.at(i).m_style.m_height * (1.0 + static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
					static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));
				currentFragmentOffset[0] +=	fragments.at(i - 1).m_size[0];
			}
			break;

		case CMtextFragment::EFragmentType::eWITH_STACKED:
			if(i > 1 && fragments.at(i - 2).m_style.vert)
			{
				currentFragmentOffset[1] -= fragments.at(i - 1).m_size[1];
				currentFragmentOffset[0] = lineOffset[0];
				currentFragmentOffset[1] -= fragments.at(i).m_style.m_height * (1.0 + static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
					static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));
			}
			else
			{
				currentFragmentOffset[1] = lineOffset[1];
				currentFragmentOffset[0] +=	0.5 * (fragments.at(i - 1).m_size[0] - fragments.at(i).m_size[0]);
			}
			break;

		default :
			assert(false);
		}	// switch

		if(fragments.at(i).m_style.m_alignment == 1)
		{
			if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
				currentFragmentOffset[1] += 0.5 * (lineSize[1] - fragments.at(i).m_style.m_height);
			else
				currentFragmentOffset[1] += 0.5 * lineSize[1] - fragments.at(i).m_style.m_height;
		}
		else
		{
			if(fragments.at(i).m_style.m_alignment == 2)
			{
				if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
					currentFragmentOffset[1] += lineSize[1] - fragments.at(i).m_style.m_height;
				else
					currentFragmentOffset[1] += lineSize[1] - 2.0 * fragments.at(i).m_style.m_height;
			}
		}
	}
	return i;
}
#endif // TRIAL_EXPLODE_MTEXT
