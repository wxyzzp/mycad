#include "cmds.h"/*DNT*/
#include "objects.h"/*DNT*/
#include "icadapi.h"/*DNT*/
#include "Gr.h"/*DNT*/
#include "commandqueue.h"/*DNT*/
#include "undo\UndoSaver.h"/*DNT*/

extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;



//
//	**************************  XCLIP command processing functions  **************************
//

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Check if given polyline set by NumberOfPoints points in pPointsArray
 *			has self-intersection(s).
 * Returns:	'true' if polyline has self-intersection(s).
 ********************************************************************************/
bool
xclipHasSelfIntersection
(
 sds_point*		pPointsArray,	// =>
 int			NumberOfPoints	// =>
)
{
	bool		bRetsult = false;
	sds_point	IntersectionPt;

	ASSERT(pPointsArray);
	if(!pPointsArray)
		return false;

	ASSERT(NumberOfPoints > 1);

	if(NumberOfPoints < 3)	// Note, it's right for this function,
		return false;		// but we look at this case from opposite view point in xclipCreateNewFilter.

	for(int Vert1 = 0; Vert1 < NumberOfPoints - 2; ++Vert1)
	{
		for(int Vert2 = Vert1 + 2; Vert2 < NumberOfPoints - 1; ++Vert2)
			if(ic_linexline(pPointsArray[Vert1], pPointsArray[Vert1+1], pPointsArray[Vert2], pPointsArray[Vert2+1], IntersectionPt) == 3)
			{
				bRetsult = true;
				break;
			}
		if(Vert1 && ic_linexline(pPointsArray[Vert1], pPointsArray[Vert1+1], pPointsArray[NumberOfPoints-1], pPointsArray[0], IntersectionPt) == 3)
		{
			bRetsult = true;
			break;
		}
	}

	return	bRetsult;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Create insertion transformation matrix.
 * Returns:	None.
 ********************************************************************************/
void
xclipGetInsertTransform
(
 db_insert*		pInsert,					// =>
 CMatrix4&		InsertionTransformation		// <=
)
{
	sds_point		InsPoint, BlockBasePoint,
					Scales,
					Axes[3];
	sds_real		Angle, Cos, Sin;
	CMatrix4		TmpTransformation;
	db_blocktabrec*	pBlockTabRec = NULL;

	InsertionTransformation.makeIdentity();

	ASSERT(pInsert);
	if(!pInsert)
		return;

	Axes[0][1] = Axes[0][2] = Axes[1][0] = Axes[1][2] = Axes[2][0] = Axes[2][1] = 0.0;
	Axes[0][0] = Axes[1][1] = 1.0;

	pInsert->get_10(InsPoint);

	pInsert->get_41(&Scales[0]);
	pInsert->get_42(&Scales[1]);
	pInsert->get_43(&Scales[2]);

	if(icadRealEqual(Scales[0],0.0))
		Scales[0] = 1.0;
	if(icadRealEqual(Scales[1],0.0))
		Scales[1] = 1.0;
	if(icadRealEqual(Scales[2],0.0))
		Scales[2] = 1.0;

	pInsert->get_50(&Angle);
	Cos = cos(Angle);
	Sin = sin(Angle);

	pInsert->get_210(Axes[2]);
	if(ic_arbaxis(Axes[2], Axes[0], Axes[1], Axes[2]) == -1)
		Axes[2][2] = 1.0;

	pInsert->get_2((db_handitem**)(&pBlockTabRec));
	pBlockTabRec->get_10(BlockBasePoint);

	TmpTransformation.makeIdentity();
	TmpTransformation(0,0) = Cos * Scales[0];
	TmpTransformation(1,0) = Sin * Scales[0];
	TmpTransformation(2,0) = 0.0;
	TmpTransformation(0,1) = - Sin * Scales[1];
	TmpTransformation(1,1) = Cos * Scales[1];
	TmpTransformation(2,1) = 0.0;
	TmpTransformation(0,2) = 0.0;
	TmpTransformation(1,2) = 0.0;
	TmpTransformation(2,2) = Scales[2];
	TmpTransformation(0,3) = InsPoint[0] -
							TmpTransformation(0,0) * BlockBasePoint[0] -
							TmpTransformation(0,1) * BlockBasePoint[1];
	TmpTransformation(1,3) = InsPoint[1] -
							TmpTransformation(1,0) * BlockBasePoint[0] -
							TmpTransformation(1,1) * BlockBasePoint[1];
	TmpTransformation(2,3) = InsPoint[2] - Scales[2] * BlockBasePoint[2];

	InsertionTransformation(0,0) = Axes[0][0];
	InsertionTransformation(1,0) = Axes[0][1];
	InsertionTransformation(2,0) = Axes[0][2];
	InsertionTransformation(0,1) = Axes[1][0];
	InsertionTransformation(1,1) = Axes[1][1];
	InsertionTransformation(2,1) = Axes[1][2];
	InsertionTransformation(0,2) = Axes[2][0];
	InsertionTransformation(1,2) = Axes[2][1];
	InsertionTransformation(2,2) = Axes[2][2];

	InsertionTransformation = InsertionTransformation * TmpTransformation;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Process xclip>new command option.
 * Returns:	'true' in success and 'false' in failure (also if a user cansels).
 ********************************************************************************/
bool
xclipCreateNewFilter
(
 sds_name&	AllInsertionsSS,		// =>
 sds_name&	ClippedInsertionsSS		// =>
)
{
	bool			bRetCode = true;
	long			SSLength;
	sds_point*		pPointsArray = NULL;
	int				NumberOfPoints;
	sds_point		ExtrusionVector = {0.0, 0.0, 1.0};
	int				iRetCode;
	db_drawing*		pDrawing = SDS_CURDWG;
    char			KeyWord[IC_ACADBUF];

	CMatrix4		InsertTransformation;

	sds_sslength(ClippedInsertionsSS, &SSLength);

	if(SSLength)
	{
		sds_initget(0, ResourceString(IDC_CMDS3_XCLIP__9, "Yes No ~_Yes ~_No"));
		iRetCode = sds_getkword(ResourceString(IDC_CMDS3_XCLIP__10, "\nDelete old boundary(s)? [Yes/No] <Yes>: "), KeyWord);

		if(iRetCode == RTERROR || iRetCode == RTCAN)
			return false;

		if(strisame(KeyWord, "NO"))
			return false;
	}

	// choose a way of creating clipping boundary
	sds_initget(0, ResourceString(IDC_CMDS3_XCLIP__11, "Select_polyline|Select Rectangular ~_Select ~_Rectangular"));
	iRetCode = sds_getkword(ResourceString(IDC_CMDS3_XCLIP__12, "\n[Select polyline/Rectangular] <Rectangular>: "), KeyWord);

	if(iRetCode == RTERROR || iRetCode == RTCAN)
		return false;

	if(strisame(KeyWord, "SELECT"/*DNT*/))
	{	// select polyline
		// TO DO: implement selecting and using polylines (as well as lwpolylines)

		sds_name		EntityName;
		sds_point		SomePoint;
		db_handitem*	pEntity = NULL;
		db_lwpolyline*	pLWPline = NULL;
		db_polyline*	pPline = NULL;

		for( ; ; )
		{
			iRetCode = sds_entsel(ResourceString(IDC_CMDS3_XCLIP__13, "\nSelect polyline: "), EntityName, SomePoint);
			
			if(iRetCode != RTNORM)
				return false;

			pEntity = (db_handitem*)EntityName[0];
			if(pEntity && pEntity->ret_type() == DB_LWPOLYLINE)
			{
				pLWPline = (db_lwpolyline*)pEntity;
				break;
			}
		}

		// get polyline's data
		pLWPline->get_90(&NumberOfPoints);
		if(!(pPointsArray = new sds_point[NumberOfPoints]))
			return false;
		pLWPline->get_10(pPointsArray, NumberOfPoints);
		pLWPline->get_210(ExtrusionVector);

		// check if selected polyline doesn't have self-intersections
		if(NumberOfPoints < 3 || xclipHasSelfIntersection(pPointsArray, NumberOfPoints))
		{
			sds_printf(ResourceString(IDC_CMDS3_XCLIP__14, "\nError: Self-intersecting clip boundaries are not acceptable."));
			bRetCode = false;
			goto bail;
		}

	}	// select
	else
		if(strisame(KeyWord, "POLYGONAL"/*DNT*/))
		{	// polygonal
						// TO DO.
						// Also change IDC_CMDS3_XCLIP__11 & IDC_CMDS3_XCLIP__12 resources strings
						// after implementing this option.
		}	// polygonal
		else
		{	// rectangular
			sds_point	FirstPoint, SecondPoint;
			resbuf		rb;
			sds_real	SaveElev;

	        if(sds_getpoint(NULL, ResourceString(IDC_CMDS3_XCLIP__15, "Specify first corner: ") , FirstPoint) != RTNORM)
				return false;

			if(SDS_getvar(NULL, DB_QELEVATION, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
				return false;

			SaveElev = rb.resval.rreal;

			if(FirstPoint[2] != SaveElev)
			{
				SDS_CorrectElevation = SaveElev;
				SDS_ElevationChanged = TRUE;
				rb.resval.rreal = FirstPoint[2];
				rb.restype = RTREAL;
				SDS_setvar(NULL, DB_QELEVATION, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
			}

			SDS_getvar(NULL, DB_QDRAGMODE, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES);
			if(SDS_dragobject(21, rb.resval.rint, FirstPoint, NULL, 0.0,
							  ResourceString(IDC_CMDS3_XCLIP__16, "\nSpecify opposite corner: "),
							  NULL, SecondPoint, NULL) != RTNORM)
				bRetCode = false;

			if(SDS_ElevationChanged)
			{
				rb.restype = RTREAL;
				rb.resval.rreal = SaveElev;
				SDS_setvar(NULL, DB_QELEVATION, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
				SDS_ElevationChanged = FALSE;
			}

			if(!bRetCode)
				return false;

			if(!(pPointsArray = new sds_point[NumberOfPoints = 2]))
				return false;

			GR_PTCPY(pPointsArray[0], FirstPoint);
			GR_PTCPY(pPointsArray[1], SecondPoint);

		}	// rectangular

	// create appropriate objects (spatial_filter, etc)
	int					InsertNum, i, j;
	sds_name			InsertEname;
	db_spatial_filter*	pSpatialFilter;
	sds_point			MatrixToSet[4];
	sds_real			Swap;

	for(InsertNum = 0; sds_ssname(AllInsertionsSS, InsertNum, InsertEname) == RTNORM; ++InsertNum)
	{
		if(!(pSpatialFilter = (db_spatial_filter*)((db_insert*)InsertEname[0])->CreateNewFilter(pDrawing)))
			continue;

		pSpatialFilter->set_extru(ExtrusionVector);
		pSpatialFilter->set_71(1);
		pSpatialFilter->set_ptarray(pPointsArray, NumberOfPoints);

		MatrixToSet[0][1] = MatrixToSet[0][2] = MatrixToSet[1][0] = MatrixToSet[1][2] = MatrixToSet[2][0] =
		MatrixToSet[2][1] = MatrixToSet[3][0] = MatrixToSet[3][1] = MatrixToSet[3][2] = 0.0;
		MatrixToSet[0][0] = MatrixToSet[1][1] = MatrixToSet[2][2] = 1.0;
		ic_arbaxis(ExtrusionVector, MatrixToSet[0], MatrixToSet[1], MatrixToSet[2]);
		for(i = 0; i < 3; ++i)
			for(j = 0; j < i; ++j)
			{
				Swap = MatrixToSet[i][j];
				MatrixToSet[i][j] = MatrixToSet[j][i];
				MatrixToSet[j][i] = Swap;
			}

		pSpatialFilter->set_clipboundmat(MatrixToSet);

		xclipGetInsertTransform((db_insert*)InsertEname[0], InsertTransformation);

		InsertTransformation = InsertTransformation.invert();

		for(i = 3; i--; )
			for(j = 3; j--; )
				MatrixToSet[i][j] = InsertTransformation(j, i);

		for(j = 3; j--; )
			MatrixToSet[3][j] = InsertTransformation(j, 3);


		pSpatialFilter->set_inverseblockmat(MatrixToSet);
	}

bail:
	delete [] pPointsArray;

	return	bRetCode;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Process "xclip>generate polyline" command option.
 * Returns:	'true' in success and 'false' in failure.
 ********************************************************************************/
bool
xclipGeneratePolyline
(
 sds_name&	ClippedInsertionsSS		// =>
)
{
	db_drawing*			pDrawing = SDS_CURDWG;
	bool				bRetCode = true;
	resbuf				ResBuf;
	db_insert*			pInsert = NULL;
	db_spatial_filter*	pSpatialFilter = NULL;
	int					OriginalNumberOfPoints, NumberOfPoints, j;
	sds_point*			pPointsArray = NULL;
	db_lwpolyline*		pLWPline = NULL;
	db_polyline*		pPline = NULL;
	sds_name			InsertEname;

	sds_point			ExtrusionVector;
	sds_point			pTmpMatrix[4];
	sds_matrix			pMatrix;
	CMatrix4			FullTransformation, OrigInsertTransformInv, ActualInsertTransform, BoundaryTransform;

	pMatrix[3][0] = pMatrix[3][1] = pMatrix[3][2] = 0.0;
	pMatrix[3][3] = 1.0;

	for(int i = 0; sds_ssname(ClippedInsertionsSS, i, InsertEname) == RTNORM; ++i)
	{
		pInsert = (db_insert*)InsertEname[0];

		// I. get data from filter
		pSpatialFilter = (db_spatial_filter*)pInsert->GetFilter(pDrawing);

		ASSERT(pSpatialFilter);
		if(!pSpatialFilter)
			continue;

		// 1. extrusion vector
		pSpatialFilter->get_extru(ExtrusionVector);

		// 2. boundary points
		NumberOfPoints = OriginalNumberOfPoints = pSpatialFilter->ret_npts();

		if(OriginalNumberOfPoints == 2)
			NumberOfPoints += 2;				// we will create rectangle adding two points

		if(!(pPointsArray = new sds_point[NumberOfPoints]))
		{
			bRetCode = false;
			goto bail;
		}

		pSpatialFilter->get_ptarray(pPointsArray, OriginalNumberOfPoints);

		if(OriginalNumberOfPoints == 2)
		{
			GR_PTCPY(pPointsArray[2], pPointsArray[1]);
			pPointsArray[1][0] = pPointsArray[0][0];
			GR_PTCPY(pPointsArray[3], pPointsArray[0]);
			pPointsArray[3][0] = pPointsArray[2][0];
		}

		// transform points - TO DO: test for all cases
		pSpatialFilter->get_inverseblockmat(pTmpMatrix);
		for(j = 4; j--; )	// transpose the matrix
		{
			pMatrix[0][j] = pTmpMatrix[j][0];
			pMatrix[1][j] = pTmpMatrix[j][1];
			pMatrix[2][j] = pTmpMatrix[j][2];
		}

		OrigInsertTransformInv = (double*)pMatrix;

		xclipGetInsertTransform(pInsert, ActualInsertTransform);

		pSpatialFilter->get_clipboundmat(pTmpMatrix);
		for(j = 4; j--; )	// transpose the matrix
		{
			pMatrix[0][j] = pTmpMatrix[j][0];
			pMatrix[1][j] = pTmpMatrix[j][1];
			pMatrix[2][j] = pTmpMatrix[j][2];
		}

		BoundaryTransform = (double*)pMatrix;

		FullTransformation = BoundaryTransform *
							 ActualInsertTransform * OrigInsertTransformInv *
							 BoundaryTransform.invert();

		for(int j = NumberOfPoints; j--; )
		{
			sds_point	point;
			FullTransformation.multiplyRight(point, pPointsArray[j]);
			GR_PTCPY(pPointsArray[j], point);
		}

		// II. create (lw)polyline
		db_handitem	*pFirstItem, *pLastItem;
		pDrawing->get_entllends(&pFirstItem, &pLastItem);
		SDS_getvar(NULL, DB_QPLINETYPE, &ResBuf, pDrawing, &SDS_CURCFG, &SDS_CURSES);
		if(1 /*ResBuf.resval.rint*/)	// TO DO: create db_polyline if PLINETYPE == 0.
		{	// create db_lwpolyline

			if(!(pLWPline = new db_lwpolyline(pDrawing)))
			{
				bRetCode = false;
				goto bail;
			}
			
			pLWPline->set_10(pPointsArray, NumberOfPoints);
			pLWPline->set_38(pPointsArray[0][2]);
			pLWPline->set_70(1);
			pLWPline->set_210(ExtrusionVector);

			pDrawing->addhanditem(pLWPline);
			SDS_SetUndo(IC_UNDO_NEW_ENT_TAB_CLS, (void*)pLastItem, (void*)pLWPline, (void*)(-1), pDrawing);

			pLWPline = NULL;

		}	// db_lwpolyline
		else
		{	// create db_polyline
		}	// db_polyline

	}	// for( thru insertions

bail:

	if(!bRetCode)
	{
		delete pLWPline;
		delete pPline;
	}

	delete [] pPointsArray;

	return bRetCode;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Process "xclip>clipdepth" command option.
 * Returns:	'true' in success and 'false' in failure (also if a user cansels).
 ********************************************************************************/
bool
xclipClipDepth
(
 sds_name&	ClippedInsertionsSS		// =>
)
{
	int			iRetCode, i;
	sds_real	FrontDistance,
				BackDistance;
	bool		bSetFront = false,
				bSetBack = false;
	sds_point	SdsPoint;
	db_drawing*	pDrawing = SDS_CURDWG;
    char		KeyWord[IC_ACADBUF];
	sds_name	InsertEname;

	// front plane(s)
	sds_initget(0, ResourceString(IDC_CMDS3_XCLIP__4, "Distance Remove ~_Distance ~_Remove"));
	iRetCode = sds_getpoint(NULL, ResourceString(IDC_CMDS3_XCLIP__5, "\nSpecify front clip point or [Distance/Remove]: "), SdsPoint);

	switch(iRetCode)
	{
	case RTERROR :
	case RTCAN	 :
					return false;
	case RTKWORD :
					sds_getinput(KeyWord);
					if(strisame(KeyWord, "DISTANCE"/*DNT*/))
					{
						int		iRetCode;
						iRetCode = sds_getdist(NULL, ResourceString(IDC_CMDS3_XCLIP__6, " Specify distance from boundary: "), &FrontDistance);

						if(iRetCode == RTERROR)
							return false;

						if(iRetCode == RTNORM)
							bSetFront = true;
					}
					else
					{	// Remove front plane(s)
						CUndoSaver*	pUndoSaver = 0;
						SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, pDrawing);
						for(i = 0; sds_ssname(ClippedInsertionsSS, i, InsertEname) == RTNORM; ++i)
						{
							pUndoSaver = new CUndoSaver(((db_insert*)InsertEname[0])->GetFilter(pDrawing), pDrawing);

							((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_frontclipon(0);

							pUndoSaver->saveToUndo();
							delete pUndoSaver;
						}
					}

					break;
	case RTNORM  :
					FrontDistance = SdsPoint[2];	// maybe TO DO: ACAD computes front distance accordingly to clipping boundary UCS
					bSetFront = true;
	}

	// back plane(s)
	sds_initget(0, ResourceString(IDC_CMDS3_XCLIP__4, "Distance Remove ~_Distance ~_Remove"));
	iRetCode = sds_getpoint(NULL, ResourceString(IDC_CMDS3_XCLIP__7, "\nSpecify back clip point or [Distance/Remove]: "), SdsPoint);

	switch(iRetCode)
	{
	case RTERROR :
	case RTCAN	 :
					return false;
	case RTKWORD :
					sds_getinput(KeyWord);
					if(strisame(KeyWord, "DISTANCE"/*DNT*/))
					{
						int		iRetCode;
						iRetCode = sds_getdist(NULL, ResourceString(IDC_CMDS3_XCLIP__6, " Specify distance from boundary: "), &BackDistance);

						if(iRetCode == RTERROR || iRetCode == RTCAN)
							return false;

						if(iRetCode == RTNORM)
							bSetBack = true;
					}
					else
					{	// Remove back plane(s)
						CUndoSaver*	pUndoSaver = 0;
						SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, pDrawing);
						for(i = 0; sds_ssname(ClippedInsertionsSS, i, InsertEname) == RTNORM; ++i)
						{
							pUndoSaver = new CUndoSaver(((db_insert*)InsertEname[0])->GetFilter(pDrawing), pDrawing);

							((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_backclipon(0);

							pUndoSaver->saveToUndo();
							delete pUndoSaver;
						}
					}

					break;
	case RTNORM  :
					BackDistance = SdsPoint[2];	// maybe TO DO: ACAD computes front distance accordingly to clipping boundary UCS
					bSetBack = true;
	}

	// set planes if necessary
	if(bSetFront || bSetBack)
	{
		bool	bError = false,
				bSetOneFront, bSetOneBack;

		CUndoSaver*	pUndoSaver = 0;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, pDrawing);
		for(i = 0; sds_ssname(ClippedInsertionsSS, i, InsertEname) == RTNORM; ++i)
		{
			bSetOneFront = bSetOneBack = false;

			// analyse clipping planes distances (back plane should be behind front plane)
			if(bSetFront)
			{
				if(bSetBack)
				{
					if(FrontDistance - BackDistance > IC_ZRO)
						bSetOneFront = bSetOneBack = true;
					else
						bError = true;
				}
				else
				{
					if(((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->ret_backclipon())
					{
						if(FrontDistance - ((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->ret_backclip() > IC_ZRO)
							bSetOneFront = true;
						else
							bError = true;
					}
					else
						bSetOneFront = true;
				}
			}
			else
			{
				if(((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->ret_frontclipon())
				{
					if(((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->ret_frontclip() - BackDistance > IC_ZRO)
						bSetOneBack = true;
					else
						bError = true;
				}
				else
					bSetOneBack = true;
			}

			// set distances if necessary
			pUndoSaver = new CUndoSaver(((db_insert*)InsertEname[0])->GetFilter(pDrawing), pDrawing);
			if(bSetOneFront)
			{
				((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_frontclipon(1);
				((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_frontclip(FrontDistance);
			}

			if(bSetOneBack)
			{
				((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_backclipon(1);
				((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_backclip(BackDistance);
			}
			pUndoSaver->saveToUndo();
			delete pUndoSaver;

		}	// for( thru inserts

		if(bError)
			sds_printf(ResourceString(IDC_CMDS3_XCLIP__8, "\nError: Front clipping plane behind clipping back plane.\n"));

	}	// bSetFront || bSetBack

	return true;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Remove non-insert entities from given selection set and
 *			get inserts with spatial_filters from selected inserts.
 * Returns:	None.
 ********************************************************************************/
void
xclipFilterSS
(
 sds_name&	InsertionsSSIn,		// <=>
 sds_name&	InsertionsSSOut		// <=
)
{
	db_drawing*	pDrawing = SDS_CURDWG;
	sds_name	InsertEname;

	for(int i = 0; sds_ssname(InsertionsSSIn, i, InsertEname) == RTNORM; ++i)
	{
		if(((db_handitem*)InsertEname[0])->ret_type() != DB_INSERT)
		{
			sds_ssdel(InsertEname, InsertionsSSIn);
			i--;
			continue;
		}
		if(((db_insert*)InsertEname[0])->GetFilter(pDrawing))
			sds_ssadd(InsertEname, InsertionsSSOut, InsertionsSSOut);
	}
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Process XCLIP command.
 * Returns:	SDS RTxxx return codes.
 ********************************************************************************/
short
cmd_xclip()
{
	long		clippedInsertsNum;
	int			RetCode, i, OnOffFlag;
	resbuf*		SelectionFilter;
	sds_name	InsertionsSS, ClippedInsertionsSS,
				InsertEname;
    char		KeyWord[IC_ACADBUF];
	db_drawing*	pDrawing = SDS_CURDWG;

    if(cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	// select inserts
	if(!(SelectionFilter = sds_buildlist(RTDXF0, "INSERT"/*DNT*/, 0)))
	{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
		RetCode = - __LINE__;
		goto bail;
	}

	for( ; ; )
	{
		if((RetCode = sds_pmtssget(ResourceString(IDC_CMDS3_XCLIP__1, "\nSelect objects: "),
								   (GetActiveCommandQueue()->IsEmpty() && !lsp_cmdhasmore) ? NULL : (LPCTSTR)""/*DNT*/,
								   NULL, NULL, SelectionFilter, InsertionsSS, 1)) == RTERROR)
		{
			cmd_ErrorPrompt(CMD_ERR_NOENT, 0);
			RetCode = -1;
			goto bail;
		}
		else
			if(RetCode == RTCAN)
			{
				RetCode = -1;
				goto bail;
			}

		xclipFilterSS(InsertionsSS, ClippedInsertionsSS);
		long	ssLength;
		sds_sslength(InsertionsSS, &ssLength);

		if(ssLength)
			break;

		sds_printf("\nOnly block inserts are eligible for clipping.");
		sds_ssfree(InsertionsSS);
	}

	sds_sslength(ClippedInsertionsSS, &clippedInsertsNum);

	// get the first command option
	for( ; ; )
	{
		sds_initget(0, ResourceString(IDC_CMDS3_XCLIP__2, "ON OFF|OFf Clipdepth Delete generate_Polyline|Polyline New_boundary|New ~_ON ~_OFf ~_Clipdepth ~_Delete ~_Polyline ~_New"));
		RetCode = sds_getkword(ResourceString(IDC_CMDS3_XCLIP__3, "\nON/OFF/Clipdepth/Delete/generate Polyline/New boundary/<New>: "), KeyWord);

		if(RetCode == RTERROR || RetCode == RTCAN)
		{
			RetCode = -1;
			goto bail;
		}

		if(!clippedInsertsNum)
		{
			if( strisame(KeyWord, "ON"/*DNT*/) || strisame(KeyWord, "OFF"/*DNT*/) ||
				strisame(KeyWord, "DELETE"/*DNT*/) || strisame(KeyWord, "POLYLINE"/*DNT*/) )
			{
				sds_printf("\nNo clipping boundary found.");
				continue;
			}

			if(strisame(KeyWord, "CLIPDEPTH"/*DNT*/))
			{
				sds_printf("\nInsert must contain a clip boundary before specifying a clipdepth.");
				continue;
			}
		}

		break;
	}

	// process command option with selected inserts
	if((OnOffFlag = strisame(KeyWord, "ON"/*DNT*/)) || strisame(KeyWord, "OFF"/*DNT*/))
	{	// enable/disable spatial filters
		CUndoSaver*	pUndoSaver = 0;
		SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, pDrawing);
		for(i = 0; sds_ssname(ClippedInsertionsSS, i, InsertEname) == RTNORM; ++i)
		{
			pUndoSaver = new CUndoSaver(((db_insert*)InsertEname[0])->GetFilter(pDrawing), pDrawing);

			((db_spatial_filter*)((db_insert*)InsertEname[0])->GetFilter(pDrawing))->set_71(OnOffFlag);

			pUndoSaver->saveToUndo();
			delete pUndoSaver;
		}
	}
	else
		if(strisame(KeyWord, "CLIPDEPTH"/*DNT*/))
		{
			if(!xclipClipDepth(ClippedInsertionsSS))
			{
				RetCode = -1;
				goto bail;
			}
		}
		else
			if(strisame(KeyWord, "DELETE"/*DNT*/))
			{	// delete spatial filter(s)
				for(i = 0; sds_ssname(ClippedInsertionsSS, i, InsertEname) == RTNORM; ++i)
					((db_insert*)InsertEname[0])->DeleteFilter(pDrawing);
			}
			else
				if(strisame(KeyWord, "POLYLINE"/*DNT*/))
				{
					if(!xclipGeneratePolyline(ClippedInsertionsSS))
					{
						RetCode = -1;
						goto bail;
					}
				}
				else
				{
					if(!xclipCreateNewFilter(InsertionsSS, ClippedInsertionsSS))
					{
						RetCode = -1;
						goto bail;
					}
				}

bail:

	cmd_regenall();

	IC_RELRB(SelectionFilter);
	sds_ssfree(InsertionsSS);
	sds_ssfree(ClippedInsertionsSS);

	switch(RetCode)
	{
	case  0 :
		return RTNORM;
	case -1 :
		return RTCAN;
	default :
		return RTERROR;
	}
}
