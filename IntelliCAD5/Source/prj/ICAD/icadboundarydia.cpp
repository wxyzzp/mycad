/*
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Funcationality to initialize the dialog for Boundary/BPoly
 *
 */


#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Region.h"/*DNT*/
#include "Boolean.h"/*DNT*/
#include "BhatchDialog.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "BPolyBuilder.h"/*DNT*/
#include "BHatchBuilder.h"
#include "GeoConvert.h"/*DNT*/


void SelectObjects(CBHatchBuilder &builder , BHatchDialog &hatchDia);


//helper functions
void SetIslandDetection(CBoundaryBuilder *builder, BHatchBoundaryTab::island islandOption)
{
	switch (islandOption)
	{
	case BHatchBoundaryTab::IGNORE_ISLAND:
		builder->SetIslandDetection(Ignore_Islands);
		break;

	case BHatchBoundaryTab::OUTER_ISLAND:
		builder->SetIslandDetection(Outer_Only);
		break;

	case BHatchBoundaryTab::NESTED_ISLAND:
		builder->SetIslandDetection(Nested_Islands);
		break;
	}
}

int RetainBoundaryWithHatch(db_hatch *hatch, BHatchDialog &hatchDia, CBPolyBuilder *boundaryBuilder,
							CBHatchBuilder *hatchBuilder, C3Point &normal)
{
	// build pline
	boundaryBuilder->AcceptHanditemColl(SDS_CURDWG, hatchDia.m_boundaryTab.m_boundarySet, normal);
	SetIslandDetection(boundaryBuilder, (BHatchBoundaryTab::island) hatchDia.m_boundaryTab.m_islandRadio);

	int numSeeds;
	hatch->get_98(&numSeeds);

	sds_point point;

	for (int i = 0; i < numSeeds; i++)
	{
		hatch->get_seed(point, i);
		boundaryBuilder->ClickPoint(point[0], point[1]);
	}

	bool selected;
	RC rc = boundaryBuilder->PersistMostRecentlySelected(selected);
	HanditemCollection *selectedEnts = NULL;

	if (rc == FAILURE)
		return rc;

	if (!selected)
		boundaryBuilder->Persist();

	selectedEnts = boundaryBuilder->getHanditemCollection();

	if (!selectedEnts || (cmd_boundaryAccept(*boundaryBuilder) != RTNORM))
		return FAILURE;

	//build hatch

	// Set candidate list not to toggle since hatch has already been selected
	hatchBuilder->SetToggle(false);

	hatchBuilder->set_hatch(hatch, true);
	hatchBuilder->AcceptHanditemColl(SDS_CURDWG, *selectedEnts, normal);
	SetIslandDetection(hatchBuilder, (BHatchBoundaryTab::island) hatchDia.m_boundaryTab.m_islandRadio);

	for (i = 0; i < numSeeds; i++)
	{
		hatch->get_seed(point, i);
		hatchBuilder->ClickPoint(point[0], point[1]);
	}

	if (cmd_boundaryAccept(*hatchBuilder) != RTNORM)
		return FAILURE;

	return SUCCESS;
}


int CMainWindow::PickPoints(BHatchDialog &hatchDia, CBoundaryBuilder &builder, bool &firstPickPoints,
							C3Point &ptNormal)
{
	sds_point pt;
	int pointResult;
	
	// Only add boundary set the first time pick points is selected
	if (firstPickPoints)
	{
		//Use the normal vector calculated from the beginning.
		builder.AcceptHanditemColl(SDS_CURDWG, hatchDia.m_boundaryTab.m_boundarySet, ptNormal);
		firstPickPoints = false;
	}

	// Set island detection
	SetIslandDetection(&builder, (BHatchBoundaryTab::island) hatchDia.m_boundaryTab.m_islandRadio);

	pointResult = internalGetpoint(NULL,ResourceString(IDC_ICADBHATCH_PICKPOINT, "\nSelect a point to define a boundary or hatch area: "),pt);

	while (pointResult == RTNORM)
	{
		if (!builder.m_pCandidates->HasSelectedCandidate(C3Point(pt[0], pt[1], pt[2])))
			cmd_boundaryPointSelection(pt, builder, hatchDia.m_boundaryTab.m_boundarySet);
		else
			cmd_ErrorPrompt(CMD_ERR_BOUNDARY_DUPLICATE,0);

		pointResult = internalGetpoint(NULL,ResourceString(IDC_ICADBHATCH_PICKPOINT, "\nSelect a point to define a boundary or hatch area: "),pt);
	}
	
	return pointResult;
}

long CMainWindow::SetSelect(BHatchDialog &hatchDia)
{
	hatchDia.m_boundaryTab.m_boundarySetRadio = BHatchBoundaryTab::CURRENT_SET;

	 return cmd_getEntitySet(hatchDia.m_boundaryTab.m_boundarySet);
}

void CMainWindow::IcadBoundaryPolyDia()
{
	int ret;
	bool firstPickPoints = true;
	long result;

	C3Point ptNormal;
	GetUcsZ(ptNormal);
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBPolyBuilder polyBuilder( ptNormal, result);	
#else
	CBPolyBuilder polyBuilder(0.00001, ptNormal, result);
#endif

	ASSERT (SUCCEEDED(result));

	BHatchDialog hatchDia(BHatchDialog::POLYLINE);
	
	do {
		ret = hatchDia.DoModal();

		switch (ret) {
			case BOUNDARY_PICK_POINTS:
				PickPoints(hatchDia, polyBuilder, firstPickPoints, ptNormal);
				break;

			case BOUNDARY_SET_SELECT:
				SetSelect(hatchDia);
				break;

			case IDOK:
				cmd_boundaryAccept(polyBuilder);
				break;

			case IDCANCEL:
				cmd_boundaryCancel(polyBuilder);
				break;
		}



	} while ((ret != IDOK) && (ret != IDCANCEL));
}


void CMainWindow::IcadBoundaryHatchDia(sds_name ss)
{
	int ret;
	bool firstPickPoints = true;
	long result;

	C3Point ptNormal;
	GetUcsZ(ptNormal);

	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBHatchBuilder hatchBuilder(ptNormal, result);
#else
	CBHatchBuilder hatchBuilder(HATCH_TOLERANCE, ptNormal, result);
#endif
	

	ASSERT (SUCCEEDED(result));

	// We do not want to destroy this hatch here.  The hatch is passed to the CBHatchBuilder. The CBHatchBuilder
	// will add it to a Handitem collection which will destroy it if appropriate (the user hits cancel). If the
	// user hits OK then we do not want to destroy it because it will be added to the drawing.
	db_hatch *hatch = new db_hatch(SDS_CURDWG);

	hatchBuilder.set_hatch(hatch, true);

	//both the CBHatchBuilder and the BHatchDialog need to point to the same hatch so the pattern is updated
	//appropriately as the user makes changes.
	BHatchDialog hatchDia(BHatchDialog::HATCH, hatch);
	int pointResult;
	hatchDia.m_boundaryTab.m_callFromBhatch = TRUE; 

	do {
		ret = hatchDia.DoModal();

		switch (ret) {
			case BOUNDARY_PICK_POINTS:
				{
					hatchBuilder.m_updateSeeds = true;
					hatchBuilder.m_storeSeeds = false;
					//Bugzilla No 72003 06-05-2002 [
					struct resbuf rb;
					sds_getvar("OSMODE"/*DNT*/,&rb);
					int prevmode=rb.resval.rint;
					rb.resval.rint=0;
					sds_setvar("OSMODE",&rb);
					pointResult = PickPoints(hatchDia, hatchBuilder, firstPickPoints, ptNormal);
					
					if(pointResult == RTCAN)
					{
						hatchDia.m_boundaryTab.m_pickPointsCalled = false; 
						hatchBuilder.m_updateSeeds = false;
						hatchBuilder.m_storeSeeds = false;
						cmd_boundaryCancel(hatchBuilder);
					}
					
					rb.resval.rint=prevmode;
					sds_setvar("OSMODE",&rb);
					//Bugzilla No 72003 06-05-2002	]


				}
				break;

			case BOUNDARY_SET_SELECT:
				{
					hatchBuilder.m_updateSeeds = false;
					hatchBuilder.m_storeSeeds = false;
					SetSelect(hatchDia);
					
				}
				break;
			
			case BOUNDARY_SELECT_OBJECTS:
				SelectObjects(hatchBuilder,hatchDia);
				break;

			case IDOK:
				{
					
					hatchBuilder.m_updateSeeds = false;
					hatchBuilder.m_storeSeeds = true;
					if (hatchDia.m_boundaryTab.m_retainBoundCheck)
					{
						// Hack.  Basically with retain boundaries we need to create the boundary
						//polyline. Then we need to recreate the hatch and make it associated with
						//the new polyline.  To do this we use a new CBHatchBuilder. Both the hatch
						//builder and the dialog point to the same hatch.  So, when we introduce
						//a new hatch builder, we need to make sure the first hatch builder doesn't
						//try to destroy the hatch.
						hatchBuilder.Persist();
						HanditemCollection *selectedEnts = hatchBuilder.getHanditemCollection();

						if (selectedEnts)
						{
							db_handitem *entity = selectedEnts->GetNext(NULL);

							while (entity)
							{
								// ensure entity is not destroyed when list is destroyed since it is being
								// added to the drawing.
								selectedEnts->SetDestroyHI(entity, false);
								entity = selectedEnts->GetNext(entity);
							}
						}
						//Bugzilla No. 78414 ; 01-30-2003 
				#ifdef VARYING_HATCH_TOLERANCE
						CBPolyBuilder boundaryBuilder( ptNormal, result);
						CBHatchBuilder newHatchBuilder(ptNormal, result);
				#else
						CBPolyBuilder boundaryBuilder(HATCH_TOLERANCE, ptNormal, result);
						CBHatchBuilder newHatchBuilder(HATCH_TOLERANCE, ptNormal, result);
				#endif

						/*D.G.*/// Break old associations (if any) before retaining a new boundary.
						int		IsAssociative;
						hatch->get_71(&IsAssociative);
						if(IsAssociative)
						{
							hatch->BreakAssociation(true);
							hatch->set_71(IsAssociative);
						}

						if (SUCCESS != RetainBoundaryWithHatch(hatch, hatchDia, &boundaryBuilder, &newHatchBuilder, ptNormal))
							// At least give them the hatch
							cmd_boundaryAccept(hatchBuilder);
					}
					else
						cmd_boundaryAccept(hatchBuilder);
					
					// EBATECH(CNBR) Regen Hatch display list.
					{
						sds_name ename;
						ename[0] = (long)( hatch );
						ename[1] =	(long)( SDS_CURDWG );
						sds_entupd( ename );
					}
					//
				}
				break;

			case IDCANCEL:
				hatchBuilder.m_updateSeeds = false;
				hatchBuilder.m_storeSeeds = false;
				cmd_boundaryCancel(hatchBuilder);
				break;

			case PATTERN_COPY_HATCH:
				hatchDia.CopyHatchPropertiesAndPattern();
				break;

			default:
				ASSERT(FALSE);
		}



	} while ((ret != IDOK) && (ret != IDCANCEL));
}

//4M Item:17->
// BHatch edit
void IcadHatchPropDia(db_hatch *hatch ,sds_name * hatches)
{
	int ret;
	bool firstPickPoints = true;
	long result;

	C3Point ptNormal;
	GetUcsZ(ptNormal);
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBHatchBuilder hatchBuilder( ptNormal, result);
#else
	CBHatchBuilder hatchBuilder(HATCH_TOLERANCE, ptNormal, result);
#endif	

	ASSERT (SUCCEEDED(result));

	struct resbuf rb;
	char *mp, m2[100];
	sds_point m10;
	sds_real m41,m47,m52;
	int m70,m71,m75,m76,m77,m78,m91,m98;

	hatch->get_2(&mp);
	strcpy(m2, mp);
	hatch->get_10(m10);
	hatch->get_41(&m41);
	hatch->get_47(&m47);
	hatch->get_52(&m52);
	hatch->get_70(&m70);
	hatch->get_71(&m71);
	hatch->get_75(&m75);
	hatch->get_76(&m76);
	hatch->get_77(&m77);
	hatch->get_78(&m78);
	hatch->get_91(&m91);
	hatch->get_98(&m98);

	// set variables
	rb.restype = RTSHORT;	rb.resval.rint = m75;	sds_setvar("HPSTYLE", &rb);
	rb.restype = RTSTR;		rb.resval.rstring = m2;	sds_setvar("HPNAME"/*DNT*/,&rb);
	rb.restype = RTREAL;	rb.resval.rreal = m41;	sds_setvar("HPSCALE"/*DNT*/,&rb);
	rb.restype = RTREAL;	rb.resval.rreal = m41;	sds_setvar("HPSPACE"/*DNT*/,&rb);
	rb.restype = RTSHORT;	rb.resval.rint = m77;	sds_setvar("HPDOUBLE"/*DNT*/,&rb);
	rb.restype = RTREAL;	rb.resval.rreal = m52;	sds_setvar("HPANG"/*DNT*/,&rb);

	// We do not want to destroy this hatch, at no means.
	hatchBuilder.set_hatch(hatch, false);
	//both the CBHatchBuilder and the BHatchDialog need to point to the same hatch so the pattern is updated
	//appropriately as the user makes changes.
	BHatchDialog hatchDia(BHatchDialog::HATCH, hatch);
	//Trick the dialog that the bounds have already been selected, so as to use the existing ones
	hatchDia.m_boundaryTab.m_pickPointsCalled=TRUE;

	do {
		ret = hatchDia.DoModal();

		switch (ret) {
			case BOUNDARY_PICK_POINTS:
				break;

			case BOUNDARY_SET_SELECT:
				break;

			case IDOK:
				hatchDia.RecalcPattern(hatches);
				hatchDia.RedrawHatch(hatches);
				break;

			case IDCANCEL:
				hatch->set_2(m2);
				hatch->set_41(m41);
				hatch->set_52(m52);
				hatch->set_75(m75);
				hatch->set_77(m77);

				// set variables
				rb.restype = RTSHORT;	rb.resval.rint = m75;	sds_setvar("HPSTYLE", &rb);
				rb.restype = RTSTR;		rb.resval.rstring = m2;	sds_setvar("HPNAME"/*DNT*/,&rb);
				rb.restype = RTREAL;	rb.resval.rreal = m41;	sds_setvar("HPSCALE"/*DNT*/,&rb);
				rb.restype = RTREAL;	rb.resval.rreal = m41;	sds_setvar("HPSPACE"/*DNT*/,&rb);
				rb.restype = RTSHORT;	rb.resval.rint = m77;	sds_setvar("HPDOUBLE"/*DNT*/,&rb);
				rb.restype = RTREAL;	rb.resval.rreal = m52;	sds_setvar("HPANG"/*DNT*/,&rb);

				{
				hatchDia.UndrawHatch();
				if (hatchDia.m_patternTab.m_patternDef)
					cmd_hatch_freehpdefll(&hatchDia.m_patternTab.m_patternDef);

			char * patName=new char[hatchDia.m_patternTab.m_patternName.GetLength()+1];
			strcpy(patName,hatchDia.m_patternTab.m_patternName);
				hatchDia.m_hatch->set_2(patName);
			delete patName;
				if(strsame("SOLID"/*DNT*/, hatchDia.m_patternTab.m_patternName))	/*D.Gavrilov*/// for solid fill hatches
					hatchDia.m_hatch->set_70(1);				// support from user interface
				CString file = hatchDia.m_patternTab.filepat;
				int numlines = cmd_hatch_loadpat((LPCTSTR)file, hatchDia.m_patternTab.m_patternName, &hatchDia.m_patternTab.m_patternDef);
				if (numlines == RTERROR)
					break;
				hatchDia.m_patternTab.m_numPatLines = numlines;
				hatchDia.RecalcPattern();
				hatchDia.RedrawHatch();
				}
				break;

			case PATTERN_COPY_HATCH:
				hatchDia.CopyHatchPropertiesAndPattern();
				break;

			default:
				ASSERT(FALSE);
		}
	} while ((ret != IDOK) && (ret != IDCANCEL));
}
//<-4M Item:17


/********************************************************************************
 * Author:	Mohan Nayak
 *
 * Purpose:	To select and hatch the objects
 *
 * Returns:	void
 ********************************************************************************/

void SelectObjects(CBHatchBuilder &builder,BHatchDialog &hatchDia)
{
	HanditemCollection	SelectedSet;
	long lResult = 0;
	C3Point ptNormal;
	GetUcsZ(ptNormal);

	//***Get the selected entities
	//lResult = cmd_getEntitySet(SelectedSet);
	lResult = cmd_getEntitySet(SelectedSet);
	if(lResult == RTCAN)
	{
		hatchDia.m_boundaryTab.m_selectObjectsCalled = false; 
		builder.m_updateSeeds = false;
		builder.m_storeSeeds = false;
		cmd_boundaryCancel(builder);
	}
	//hatchDia.m_boundaryTab.m_boundarySet
	if (!SelectedSet.IsEmpty())
	{
		long lSize = 0;
		bool bSelected = false;
		CandidateList *pCandidates = NULL;
		builder.m_updateSeeds = true;
		builder.m_storeSeeds = false;
		hatchDia.m_boundaryTab.m_boundarySetRadio = BHatchBoundaryTab::ALL_OBJECTS;
		builder.AcceptHanditemColl( SDS_CURDWG, SelectedSet, ptNormal); 
		SetIslandDetection(&builder,(BHatchBoundaryTab::island)hatchDia.m_boundaryTab.m_islandRadio);   
		builder.m_pFragment->Operate ();
		builder.m_pFragment->WrapUp();
		pCandidates = builder.m_pCandidates ;
		pCandidates->GetIslands();
		lSize = pCandidates->Size ();
		if (lSize)
		{
			long lIndex = 0;
			// from CandidateList::ClickPoint for these items
			for( ; lIndex < lSize; lIndex ++ )
			{
				if( pCandidates->Item( lIndex )->GetContainer( pCandidates, false ) == -1 &&
					pCandidates->Item( lIndex )->IsOpen( pCandidates ) &&
					!pCandidates->Item( lIndex )->IsFinalist() &&
					!pCandidates->Item( lIndex )->IsHatchFinalist())
				{
					pCandidates->Item( lIndex )->SetFinalist( true, pCandidates->m_RecursionDepth );
					pCandidates->Item( lIndex )->SetHatchFinalist( true, MAX_ISLAND_DEPTH );
				}
			}

			// Add some other candidates to the finalists set - holes (not islands!) in non-finalists.
			for( lIndex = 0; lIndex < lSize; lIndex ++ )
			{
				Candidate*	AnItem = pCandidates->Item( lIndex );
				int	ItemContainerNum = AnItem->GetContainer( pCandidates, false );
				if( !AnItem->IsFinalist() && !AnItem->GetParent() && ItemContainerNum != -1 &&
					!pCandidates->Item( ItemContainerNum )->IsFinalist())
				{
					AnItem->SetFinalist( true, pCandidates->m_RecursionDepth );
					AnItem->SetHatchFinalist( true, MAX_ISLAND_DEPTH );
				}
			}
		} // if(Size)

		builder.PersistMostRecentlySelected(bSelected); 
		
		db_handitem *pEntity = NULL;
		db_handitem *pPosition = NULL;
		HanditemCollection *pSelectedEnts = NULL;

		if( !bSelected )
		{
			if( !SelectedSet.IsEmpty())
			{
				pEntity = SelectedSet.GetNext( NULL );
				while (pEntity)
				{
					pPosition = pEntity;
					cmd_redrawEntity( pEntity, IC_REDRAW_UNHILITE );
					pEntity = SelectedSet.GetNext( pPosition );
				}
			}
			builder.Persist();
		}
		pSelectedEnts = builder.getHanditemCollection();
		if( pSelectedEnts )
		{
			cmd_drawHICollection( pSelectedEnts, IC_REDRAW_HILITE );
		}
		
	}
	else
		sds_printf( ResourceString(IDC_BHATCH_NO_SELECTED, "\nThere are no selected objects" ));

	
	
}



















