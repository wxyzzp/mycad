/* G:\ICADDEV\PRJ\ICAD\ENTITYRENDERER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
#undef	ASSERT
#undef	VERIFY

#include <afxwin.h>         // MFC core and standard components
#include "icad.h"/*DNT*/
#include "EntityRenderer.h"/*DNT*/
#include "db.h"/*DNT*/
#include "gr.h"/*DNT*/
#include "viewport.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "cmds.h"

#if defined(USE_SMARTHEAP)
	#include "smrtheap.h"/*DNT*/
#endif

// globals that formerly were in icaddraw.cpp
bool           SDS_DontDrawVPGuts;
bool           SDS_DrawVPGutsOnly;

POINT CEntityRenderer::s_PointsBuffer[ePointsBufferSize];

#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in EntityRenderer.cpp")
/*------------------------------------------------------------------------*//**
@author Denis Petrov
Rewritten for use with 3D display objects.
*//*-------------------------------------------------------------------------*/
void CEntityRenderer::DrawEntity(db_handitem* elp,
								 CDrawDevice* pDrawDevice, 
								 db_drawing* flp, 
								 struct gr_view* gView, 
								 POINT BltArea[2], 
								 int* FstPt, 
								 int xor, 
								 int hl, 
								 int undraw, 
								 int undrawcol, 
								 int fillmode, 
								 int norecurs, 
								 db_viewport* pViewportForVpLayerSettings, 
								 bool bRedraw, 
								 int width)
{
	db_handitem *telp;
	
	int iProxyShow = 1;
	// If this is a proxy, and proxyshow == 0, don't display it.
	if (elp->ret_type() == DB_ACAD_PROXY_ENTITY)
	{
		struct resbuf rbProxyShow;
		SDS_getvar(NULL, DB_QPROXYSHOW, &rbProxyShow, flp, &SDS_CURCFG, &SDS_CURSES);
		if (rbProxyShow.resval.rint == 0)
			return;
		else
			iProxyShow = rbProxyShow.resval.rint;
	}
	// get the target DC in case we have any TrueType text
	CDC*		pDC = pDrawDevice->getCDC();
	CIcadView*	pIcadView = SDS_CMainWindow->GetIcadView();

	gr_displayobject* pOriginal = (struct gr_displayobject *)elp->ret_disp();
	gr_displayobject* pEnd = NULL;
	
	if (elp->ret_type() == DB_ACAD_PROXY_ENTITY && iProxyShow == 2) // Show bounding box.
	{
		pOriginal = (struct gr_displayobject *)((db_acad_proxy_entity*)elp)->ret_boundingBox(flp);
	}

 	if(IsPrinting() || cmd_InsideRegen)
	{
		struct resbuf rbPsltscale, rbTilemode;
		SDS_getvar(NULL,DB_QPSLTSCALE,&rbPsltscale,flp,&SDS_CURCFG,&SDS_CURSES);
		SDS_getvar(NULL,DB_QTILEMODE,&rbTilemode,flp,&SDS_CURCFG,&SDS_CURSES);
		int entityType = elp->ret_type();
		if(	pOriginal && rbTilemode.resval.rint == 0 && rbPsltscale.resval.rint && elp->ret_ltypehip()
			&& entityType != DB_VERTEX && entityType != DB_POINT && entityType != DB_VIEWPORT
			&& entityType != DB_SOLID && entityType != DB_3DFACE && entityType != DB_TRACE 
			&& entityType != DB_TEXT && entityType != DB_MTEXT && entityType != DB_ARCALIGNEDTEXT && entityType != DB_RTEXT  
			&& entityType != DB_3DSOLID && entityType != DB_BODY && entityType != DB_REGION) 
		{				 
			db_handitem* lthip = elp->ret_ltypehip();
			if(lthip->ret_bylayer())
			{
				db_layertabrec* layerhip = (db_layertabrec*)elp->ret_layerhip();
				if(layerhip && layerhip->ret_ltype())
					lthip = layerhip->ret_ltype();
			}
			else if(lthip->ret_byblock())
			{
				db_handitem* parent = elp->m_parentBlock;
				db_handitem* lthipParent = NULL;
				while(parent && (lthipParent = parent->ret_ltypehip()) && lthipParent->ret_byblock())
				{
					parent = parent->m_parentBlock;
				}
				if(parent && lthipParent && lthipParent->ret_bylayer())
				{
					db_layertabrec* layerhip = (db_layertabrec*)parent->ret_layerhip();
					if(layerhip && layerhip->ret_ltype())
						lthip = layerhip->ret_ltype();
				}
			}
			if(lthip && !lthip->ret_continuous())
			{
				// Every viewport has individual ltscale coefficient if PSLTSCALE is 1
				// To display dashed lines correctly in all viewports we should regenerate one every time before drawing
				gr_freedisplayobjectll(pOriginal);
				pOriginal = NULL;
				elp->set_disp(pOriginal);
			}
		}
	}
	if(pOriginal == NULL)
	{
		telp = elp;
		// As an optimization, only use the VpLayerSettings if they really are going to matter
		//
		db_viewport *pViewport = NULL;
		if ( ( pViewportForVpLayerSettings != NULL ) &&
			( pViewportForVpLayerSettings->GetVpLayerFrozenLayerCount( ) > 0 ) )
		{
			pViewport = pViewportForVpLayerSettings;
		}
		
		// get the display objects
		int result = gr_getdispobjs(flp,
			NULL,
			&telp,
			&pOriginal,
			&pEnd,
			NULL,
			flp,
			gView,
			0,
			pDC,
			pViewport);

		if(pOriginal == NULL)
			return;

		elp->set_disp(pOriginal);
	}

	int i, j;
	gr_displayobject* pCurDO;
    int nType;
	int rop;
	bool bUsingGDIPath = false;
	bool bNeedToDelete;
	sds_point projectedPoint;
	
	POINT* pPolyPts; 
	int	iFillColor;
    //<alm>
    int hl_store = hl;
    //</alm>

	for(pCurDO = pOriginal; pCurDO; pCurDO = pCurDO->next) 
	{
		if(pCurDO->type & DISP_OBJ_SPECIAL)
		{
			// DP: currently only the following entities can have special display objects
            nType = pCurDO->GetSourceEntity()->ret_type();
			ASSERT( nType == DB_IMAGE || nType == DB_SOLID || nType == DB_TRACE ||
				nType == DB_LWPOLYLINE || nType == DB_POLYLINE );
			
			// DP: don't draw during real time operations (probably for performance reason)
			// Test for IcadView being NULL. I ran into this on the first pass at reading a .dxf file. 
			bool bRealTime = false;
			if (pIcadView != NULL)
			{
				bRealTime = pIcadView->GetRealTime() != NULL;
			}
			if(nType == DB_IMAGE && bRealTime)
				continue;
			
			if(!drawEntitySpecial(pCurDO, pDrawDevice, flp, gView, hl, xor, undraw))
				ASSERT(FALSE); 
			continue;
		}

		if(pCurDO->npts < 2 || !pCurDO->chain)
			continue;

		ASSERT(pCurDO->type & DISP_OBJ_PTS_3D);

		if((pCurDO->color == -1 || xor) && !undraw) 
		{
			rop = R2_XORPEN;
			pDrawDevice->SetDrawMode(RasterConstants::MODE_XOR);
		}
		else
		{
			rop = R2_COPYPEN;
			pDrawDevice->SetDrawMode(RasterConstants::MODE_COPY);
		}

		if(pDrawDevice->is3D())
		{
	        if((pCurDO->type & DISP_OBJ_PTS_GDI_FILLPATH ||
				pCurDO->type & DISP_OBJ_PTS_GDI_TRUETYPE) && !bUsingGDIPath)
			{
				SetDcFillColorMem(undraw ? undrawcol : pCurDO->color, hl, rop, pDrawDevice);
				SetDcColorMem(undraw ? undrawcol : pCurDO->color, width, hl, rop, pDrawDevice);
				pDrawDevice->Polygon(NULL, 0, false);
				bUsingGDIPath = true;
			}

			if(bUsingGDIPath)
			{
				pDrawDevice->Polygon((POINT*)pCurDO->chain, -pCurDO->npts, false);
			}
			else
			{
				SetDcFillColorMem(undraw ? undrawcol : pCurDO->color, hl, rop, pDrawDevice);
				SetDcColorMem(undraw ? undrawcol : pCurDO->color, width, hl, rop, pDrawDevice);
				pDrawDevice->SetLineWeight(pCurDO->lweight);
				if(pCurDO->type & DISP_OBJ_PTS_CLOSED)
					pDrawDevice->Polygon((POINT*)pCurDO->chain, pCurDO->npts, true);
				else
					pDrawDevice->Polyline((POINT*)pCurDO->chain, pCurDO->npts);
			}

			if(bUsingGDIPath && (!pCurDO->next || pCurDO->GetSourceEntity() != pCurDO->next->GetSourceEntity() ||
				(!(pCurDO->next->type & DISP_OBJ_PTS_GDI_FILLPATH) &&
				!(pCurDO->next->type & DISP_OBJ_PTS_GDI_TRUETYPE))))
			{
				pDrawDevice->Polygon(NULL, 0, false);
				bUsingGDIPath = false;
			}
			continue;
		}
		
        //<alm>
        if (pCurDO->type & DISP_OBJ_HIGHLIGHTED)
            hl = 1;
        else
            hl = hl_store;
        //</alm>
	
        if((pCurDO->type & DISP_OBJ_PTS_GDI_FILLPATH ||
			pCurDO->type & DISP_OBJ_PTS_GDI_TRUETYPE) && !bUsingGDIPath)
		{
			// Prepare a GDI path for filling.
			pDC->SetPolyFillMode(ALTERNATE);
			pDC->BeginPath();
			iFillColor = pCurDO->color;
			bUsingGDIPath = true;
		}
		
		if(pCurDO->npts + 2 > ePointsBufferSize)
		{
			pPolyPts = new POINT[pCurDO->npts + 2];
			bNeedToDelete = true;
		}
		else
		{
			pPolyPts = s_PointsBuffer;
			bNeedToDelete = false;
		}
		SetDcFillColorMem(undraw ? undrawcol : pCurDO->color, hl, rop, pDrawDevice);
		SetDcColorMem(undraw ? undrawcol : pCurDO->color, width, hl, rop, pDrawDevice);
		pDrawDevice->SetLineWeight(pCurDO->lweight);

		// Now draw it!
		if(((pCurDO->type & DISP_OBJ_PTS_FILLABLE) || (pCurDO->type & DISP_OBJ_PTS_GDI_FILLPATH)) && fillmode)
        {
			for(i = 0, j = 0; i < pCurDO->npts; ++i, j += 3)
			{
				gr_ucs2rp(&(pCurDO->chain[j]), projectedPoint, gView);
				gr_rp2pix(gView, projectedPoint[0], projectedPoint[1], (int*)&pPolyPts[i].x, (int*)&pPolyPts[i].y);
				if(BltArea)
				{
					if(*FstPt) 
					{
						BltArea[0].x = pPolyPts[i].x;  
						BltArea[0].y = pPolyPts[i].y;
						BltArea[1].x = pPolyPts[i].x;  
						BltArea[1].y = pPolyPts[i].y;
						*FstPt=0;
					} 
					else 
					{
						if(BltArea[0].x > pPolyPts[i].x)
							BltArea[0].x = pPolyPts[i].x;
						else
							if(BltArea[1].x < pPolyPts[i].x)
								BltArea[1].x = pPolyPts[i].x;
						if(BltArea[0].y > pPolyPts[i].y)
							BltArea[0].y = pPolyPts[i].y;
						else
							if(BltArea[1].y < pPolyPts[i].y)
								BltArea[1].y = pPolyPts[i].y;
					}
				}
			}
			if(pCurDO->type & DISP_OBJ_PTS_CLOSED)
			{
				gr_ucs2rp(&(pCurDO->chain[0]), projectedPoint, gView);
				gr_rp2pix(gView, projectedPoint[0], projectedPoint[1], (int*)&pPolyPts[pCurDO->npts].x, (int*)&pPolyPts[pCurDO->npts].y);
				if(BltArea)
				{
					if(BltArea[0].x > pPolyPts[i].x)
						BltArea[0].x = pPolyPts[i].x;
					else
						if(BltArea[1].x < pPolyPts[i].x)
							BltArea[1].x = pPolyPts[i].x;
					if(BltArea[0].y > pPolyPts[i].y)
						BltArea[0].y = pPolyPts[i].y;
					else
						if(BltArea[1].y < pPolyPts[i].y)
							BltArea[1].y = pPolyPts[i].y;
				}
			}
			
			// Solid Fill or a path for a Solid hatch or for a TTF text contour
			if(pCurDO->type & DISP_OBJ_PTS_CLOSED)
			{
				//Bugzilla::78166; 18-08-03;
				if ( IsPrinting() && ( pCurDO->GetSourceEntity()->ret_type() == DB_LWPOLYLINE || pCurDO->GetSourceEntity()->ret_type() == DB_POLYLINE ) )
					pDrawDevice->Polygon(pPolyPts, pCurDO->npts + 1, false );					
				else
				pDrawDevice->Polygon(pPolyPts, pCurDO->npts + 1, !(pCurDO->type & DISP_OBJ_PTS_GDI_FILLPATH));
			}
			else
				pDrawDevice->Polygon(pPolyPts, pCurDO->npts, !(pCurDO->type & DISP_OBJ_PTS_GDI_FILLPATH));
        }
		else
		{
			if(pCurDO->type & DISP_OBJ_PTS_GDI_TRUETYPE)
			{
				for(i = 0, j = 0; i < pCurDO->npts; ++i, j += 3)
				{
					gr_ucs2rp(&(pCurDO->chain[j]), projectedPoint, gView);
					gr_rp2pix(gView, projectedPoint[0], projectedPoint[1], (int*)&pPolyPts[i].x, (int*)&pPolyPts[i].y);
					if(BltArea)
					{
						if(*FstPt) 
						{
							BltArea[0].x = pPolyPts[i].x;  
							BltArea[0].y = pPolyPts[i].y;
							BltArea[1].x = pPolyPts[i].x;  
							BltArea[1].y = pPolyPts[i].y;
							*FstPt=0;
						} 
						else 
						{
							if(BltArea[0].x > pPolyPts[i].x)
								BltArea[0].x = pPolyPts[i].x;
							else
								if(BltArea[1].x < pPolyPts[i].x)
									BltArea[1].x = pPolyPts[i].x;
							if(BltArea[0].y > pPolyPts[i].y)
								BltArea[0].y = pPolyPts[i].y;
							else
								if(BltArea[1].y < pPolyPts[i].y)
									BltArea[1].y = pPolyPts[i].y;
						}
					}
				}
				if(pCurDO->type & DISP_OBJ_PTS_CLOSED)
				{
					gr_ucs2rp(&(pCurDO->chain[0]), projectedPoint, gView);
					gr_rp2pix(gView, projectedPoint[0], projectedPoint[1], (int*)&pPolyPts[pCurDO->npts].x, (int*)&pPolyPts[pCurDO->npts].y);
					if(BltArea)
					{
						if(BltArea[0].x > pPolyPts[i].x)
							BltArea[0].x = pPolyPts[i].x;
						else
							if(BltArea[1].x < pPolyPts[i].x)
								BltArea[1].x = pPolyPts[i].x;
						if(BltArea[0].y > pPolyPts[i].y)
							BltArea[0].y = pPolyPts[i].y;
						else
							if(BltArea[1].y < pPolyPts[i].y)
								BltArea[1].y = pPolyPts[i].y;
					}
				}
				
				if(pCurDO->type & DISP_OBJ_PTS_CLOSED)
					pDrawDevice->PolyBezier(pPolyPts, pCurDO->npts + 1);
				else
					pDrawDevice->PolyBezier(pPolyPts, pCurDO->npts);
			}
			else
			{
				int k = 0;
				int clipResult;
				sds_point projectedPoint2;
				// Convert the points into the poly list.
				gr_ucs2rp(&(pCurDO->chain[0]), projectedPoint, gView);
				for (i = 1, j = 3; i < pCurDO->npts; ++i, j += 3) 
				{
					gr_ucs2rp(&(pCurDO->chain[j]), projectedPoint2, gView);
					clipResult = gr_vect_rp2pix(projectedPoint, projectedPoint2,
						(int*)&pPolyPts[k].x,  /* Filling 4 ints. */
						gView);
					memcpy(projectedPoint, projectedPoint2, sizeof(sds_point));
					
					if(BltArea && (clipResult==0 || clipResult==2))
					{
						if(k == 0)
						{
							if(*FstPt) 
							{
								BltArea[0].x = pPolyPts[0].x;  
								BltArea[0].y = pPolyPts[0].y;
								BltArea[1].x = pPolyPts[0].x;  
								BltArea[1].y = pPolyPts[0].y;
								*FstPt = 0;
							} 
							else 
							{
								if(BltArea[0].x > pPolyPts[0].x)
									BltArea[0].x = pPolyPts[0].x;
								else
									if(BltArea[1].x < pPolyPts[0].x)
										BltArea[1].x = pPolyPts[0].x;
								if(BltArea[0].y > pPolyPts[0].y)
									BltArea[0].y = pPolyPts[0].y;
								else
									if(BltArea[1].y < pPolyPts[0].y)
										BltArea[1].y = pPolyPts[0].y;
							}
						}
						if(BltArea[0].x > pPolyPts[k + 1].x)
							BltArea[0].x = pPolyPts[k + 1].x;
						else
							if(BltArea[1].x < pPolyPts[k + 1].x)
								BltArea[1].x = pPolyPts[k + 1].x;
						if(BltArea[0].y > pPolyPts[k + 1].y)
							BltArea[0].y = pPolyPts[k + 1].y;
						else
							if(BltArea[1].y < pPolyPts[k + 1].y)
								BltArea[1].y = pPolyPts[k + 1].y;
					} // end of if(BltArea && ...
					
					++k;	
					// each of the cases clipresult==1 and clipresult==2 sets chainidx to 0, which keeps the next code block
					// from executing, so commenting out the continues is valid
					//
					if(clipResult == 1) 
					{
						if(k > 1) // Off the screen.
							pDrawDevice->Polyline(pPolyPts, k);
						// Reset the beg poly point counter.
						k = 0;
					}
					else 
						if(clipResult==2) 
						{  // Chopped it
							pDrawDevice->Polyline(pPolyPts, k + 1);
							// Reset the beg poly point counter.
							k = 0;
						}
						
				} // end of loop over points in poly list
				
				if(pCurDO->type & DISP_OBJ_PTS_CLOSED)
				{
					gr_ucs2rp(&(pCurDO->chain[0]), projectedPoint2, gView);
					clipResult = gr_vect_rp2pix(projectedPoint, projectedPoint2,
						(int*)&pPolyPts[k].x,  /* Filling 4 ints. */
						gView);
					
					if(BltArea && (clipResult==0 || clipResult==2))
					{
						if(BltArea[0].x > pPolyPts[k + 1].x)
							BltArea[0].x = pPolyPts[k + 1].x;
						else
							if(BltArea[1].x < pPolyPts[k + 1].x)
								BltArea[1].x = pPolyPts[k + 1].x;
						if(BltArea[0].y > pPolyPts[k + 1].y)
							BltArea[0].y = pPolyPts[k + 1].y;
						else
							if(BltArea[1].y < pPolyPts[k + 1].y)
								BltArea[1].y = pPolyPts[k + 1].y;
					} // end of if(BltArea && ...
					
					++k;
					// each of the cases clipresult==1 and clipresult==2 sets chainidx to 0, which keeps the next code block
					// from executing, so commenting out the continues is valid
					//
					if(clipResult == 1) 
					{
						if(k > 1) // Off the screen.
							pDrawDevice->Polyline(pPolyPts, k);
						// Reset the beg poly point counter.
						k = 0;
					}
					else 
						if(clipResult==2) 
						{  // Chopped it
							pDrawDevice->Polyline(pPolyPts, k + 1);
							// Reset the beg poly point counter.
							k = 0;
						}
						
				} // end of loop over points in poly list
				
				if(k) 
					pDrawDevice->Polyline(pPolyPts, k + 1);
				
			}
		}

		/*DG - 14.4.2003*/// DISP_OBJ_ENDPATH:
		// GDI fuctions for stroking/filling paths are very slow when the whole text is given to them,
		// so let them work in per-symbol mode.
		if( bUsingGDIPath &&
			((pCurDO->type & DISP_OBJ_ENDPATH) ||
			 !pCurDO->next ||
			 pCurDO->GetSourceEntity() != pCurDO->next->GetSourceEntity() ||
			 (!(pCurDO->next->type & DISP_OBJ_PTS_GDI_FILLPATH) && !(pCurDO->next->type & DISP_OBJ_PTS_GDI_TRUETYPE))) )
		{
			// Now let's fill the GDI path.
			pDC->EndPath();
			
			if(IsPrinting())
				SetDcFillColorMem(undraw ? undrawcol : iFillColor, hl, rop, pDrawDevice);
			else
			{
				HBRUSH hBrush;
				if(hl)
				{
					resbuf	rb;
					sds_getvar("BKGCOLOR", &rb);
					int		BGColor = SDS_RGBFromACADColor(rb.resval.rint);
					hBrush = ::CreateHatchBrush(HS_DIAGCROSS, BGColor);
				}
				else
					hBrush = ::CreateSolidBrush(::SDS_BrushColorFromACADColor(undraw ? undrawcol : iFillColor));
				
				::DeleteObject(::SelectObject(pDC->m_hDC, hBrush));
			}
			
			if(hl)
			{
				int	oldBackMode = pDC->SetBkMode(TRANSPARENT);
				pDC->StrokeAndFillPath();
				pDC->SetBkMode(oldBackMode);
			}
			else
				pDC->StrokeAndFillPath();
			
			bUsingGDIPath = false;
		}

		if(bNeedToDelete)
			delete[] pPolyPts;
	} // end of loop over items (DispObjs) to draw
}
#endif

void CEntityRenderer::outputPolygon(int ndims, 
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
									bool onlyFill)
{
	int fi1,fi2,fi4;
	
	fi2=(tdo1->npts+face)*ndims;
	
	const int BufferSize = 100;
	bool NeedToDelete;
	
	POINT * pPolyPts; 
	POINT PointBuffer[BufferSize];
	
	if (fi2/ndims > BufferSize)
	{
		pPolyPts = new POINT[fi2/ndims];
		NeedToDelete = true;
	}
	else
	{
		pPolyPts = PointBuffer;
		NeedToDelete = false;
	}
	
	for (fi1=0; fi1<fi2; fi1+=ndims) 
	{
		if (face && fi1>=fi2-ndims) 
		{
			gr_rp2pix(gView,tdo1->chain[0],tdo1->chain[1],(int *)&pPolyPts[fi1/ndims].x,(int *)&pPolyPts[fi1/ndims].y);
		} 
		else 
		{
			gr_rp2pix(gView,tdo1->chain[fi1],tdo1->chain[fi1+1],(int *)&pPolyPts[fi1/ndims].x,(int *)&pPolyPts[fi1/ndims].y);
		}
	} // end of for(fi1=0...
	
	// Expand the Bitblt area if needed.
	if(BltArea) 
	{		
		for (fi1=0; fi1<fi2; fi1+=ndims) 
		{
			fi4=fi1/ndims;
			if(*FstPt) 
			{
				BltArea[0].x=pPolyPts[fi4].x;  
				BltArea[0].y=pPolyPts[fi4].y;
				BltArea[1].x=pPolyPts[fi4].x;  
				BltArea[1].y=pPolyPts[fi4].y;
				*FstPt=0;
			} 
			else 
			{
				if(BltArea[0].x>pPolyPts[fi4].x)
					BltArea[0].x=pPolyPts[fi4].x;
				if(BltArea[0].y>pPolyPts[fi4].y)
					BltArea[0].y=pPolyPts[fi4].y;
				if(BltArea[1].x<pPolyPts[fi4].x)
					BltArea[1].x=pPolyPts[fi4].x;
				if(BltArea[1].y<pPolyPts[fi4].y)
					BltArea[1].y=pPolyPts[fi4].y;
			}
		}
	} // end of if (BltArea)
	SetDcFillColorMem(undraw ? undrawcol : tdo1->color,hl,rop,pDrawDevice);
	SetDcColorMem(undraw ? undrawcol : tdo1->color,width,hl,rop,pDrawDevice);
	if( !onlyFill ) pDrawDevice->SetLineWeight(tdo1->lweight);
	pDrawDevice->Polygon(pPolyPts,tdo1->npts+face, onlyFill);
	
	if (NeedToDelete)
	{
		delete [] pPolyPts;
	}
} 

void CEntityRenderer::outputPolyBezier(int ndims, 
									   int face, 
									   gr_displayobject* pDispObj,
									   CDrawDevice* pDrawDevice,
									   gr_view* pView,
									   POINT BltArea[2],
									   int*	pFirstPt,
									   int undraw,
									   int undrawcol,
									   int rop,
									   int hl,
									   int width)
{
	int fi1, fi2, fi4;
	fi2 = (pDispObj->npts + face) * ndims;
	
	const int	BufferSize = 100;
	bool		NeedToDelete;
	
	POINT*		pPolyPts; 
	POINT		PointBuffer[BufferSize];
	
	if(fi2 / ndims > BufferSize)
	{
		pPolyPts = new POINT[fi2 / ndims];
		NeedToDelete = true;
	}
	else
	{
		pPolyPts = PointBuffer;
		NeedToDelete = false;
	}
	
	for(fi1 = 0; fi1 < fi2; fi1 += ndims) 
	{
		if(face && fi1 >= fi2 - ndims) 
			gr_rp2pix(pView, pDispObj->chain[0], pDispObj->chain[1], (int*)&pPolyPts[fi1 / ndims].x, (int*)&pPolyPts[fi1 / ndims].y);
		else 
			gr_rp2pix(pView, pDispObj->chain[fi1], pDispObj->chain[fi1+1], (int*)&pPolyPts[fi1 / ndims].x, (int*)&pPolyPts[fi1 / ndims].y);
	} // end of for(fi1=0...
	
	// Expand the Bitblt area if needed.
	if(BltArea) 
		for(fi1 = 0; fi1 < fi2; fi1 += ndims) 
		{
			fi4 = fi1 / ndims;
			if(*pFirstPt)
			{
				BltArea[0].x = pPolyPts[fi4].x;  
				BltArea[0].y = pPolyPts[fi4].y;
				BltArea[1].x = pPolyPts[fi4].x;  
				BltArea[1].y = pPolyPts[fi4].y;
				*pFirstPt = 0;
			} 
			else 
			{
				if(BltArea[0].x > pPolyPts[fi4].x)
					BltArea[0].x = pPolyPts[fi4].x;
				if(BltArea[0].y > pPolyPts[fi4].y)
					BltArea[0].y = pPolyPts[fi4].y;
				if(BltArea[1].x < pPolyPts[fi4].x)
					BltArea[1].x = pPolyPts[fi4].x;
				if(BltArea[1].y < pPolyPts[fi4].y)
					BltArea[1].y = pPolyPts[fi4].y;
			}
		}
		
	SetDcFillColorMem(undraw ? undrawcol : pDispObj->color, hl, rop, pDrawDevice);
	SetDcColorMem(undraw ? undrawcol : pDispObj->color, width, hl, rop, pDrawDevice);
	pDrawDevice->SetLineWeight(pDispObj->lweight);
	pDrawDevice->PolyBezier(pPolyPts, pDispObj->npts + face);
		
	if(NeedToDelete)
		delete[] pPolyPts;
} 

void CEntityRenderer::outputPolyline(int ndims, 
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
							   int width)
{
	int fi1,fi2,fi3,fi4;
	int chainidx;
	int clipresult;
	
	int drawpl=0;    // if 1, draw the polyline
	int numplpts=0;  // number of points in the polyline
	
	SetDcColorMem(undraw ? undrawcol : tdo1->color,width,hl,rop,pDrawDevice);
	pDrawDevice->SetLineWeight(tdo1->lweight);

	fi2=(tdo1->npts-2+face)*ndims;
	
	const int BufferSize = 100;
	bool NeedToDelete;
	
	POINT * pPolyPts;
	POINT PointBuffer[BufferSize];
	
	// Note space needed is fi2/ndims + 2 because the loop below has fi1 <= fi2
	// and the function gr_vect_rp2pix stores two points in the pPolyPts array,
	// not just one.
	
	int space_needed = fi2/ndims + 2; 
	
	if (space_needed > BufferSize)
	{
		pPolyPts = new POINT[space_needed];
		NeedToDelete = true;
	}
	else
	{
		pPolyPts = PointBuffer;
		NeedToDelete = false;
	}
	
	// Convert the points into the poly list.
	for (chainidx=0,fi1=0; fi1<=fi2; fi1+=ndims) 
	{
		if (face && fi1==fi2) 
		{
			clipresult=gr_vect_rp2pix(
				&tdo1->chain[fi1],
				&tdo1->chain[0],
				(int *)&pPolyPts[chainidx].x,  /* Filling 4 ints. */
				gView);
		} 
		else 
		{
			clipresult=gr_vect_rp2pix(
				&tdo1->chain[fi1],
				&tdo1->chain[fi1+ndims],
				(int *)&pPolyPts[chainidx].x,  /* Filling 4 ints. */
				gView);
		}  
		
		if(BltArea && (clipresult==0 || clipresult==2)) 
		{
			for(fi3=((chainidx==0) ? 0 :1); fi3<2; fi3++) 
			{
				fi4=chainidx+fi3;
				assert(fi4 < space_needed);
				if(*FstPt) 
				{
					BltArea[0].x=pPolyPts[fi4].x;  
					BltArea[0].y=pPolyPts[fi4].y;
					BltArea[1].x=pPolyPts[fi4].x;  
					BltArea[1].y=pPolyPts[fi4].y;
					*FstPt=0;
				} 
				else 
				{
					if(BltArea[0].x>pPolyPts[fi4].x)
						BltArea[0].x=pPolyPts[fi4].x;
					if(BltArea[0].y>pPolyPts[fi4].y)
						BltArea[0].y=pPolyPts[fi4].y;
					if(BltArea[1].x<pPolyPts[fi4].x)
						BltArea[1].x=pPolyPts[fi4].x;
					if(BltArea[1].y<pPolyPts[fi4].y)
						BltArea[1].y=pPolyPts[fi4].y;
				}
			} // end of for(fi3=...
		} // end of if(BltArea && ...
		
		chainidx++;	
		
		// each of the cases clipresult==1 and clipresult==2 sets chainidx to 0, which keeps the next code block
		// from executing, so commenting out the continues is valid
		//
		
		if(clipresult==0) 
		{  
			// Normal, acceptable vector, so see if there are more
			continue;
		} 
		else if(clipresult==1) 
		{
			if(chainidx>1) 
			{  // Off the screen.
				drawpl=1;           // set flag to indicate need to draw
				numplpts=chainidx;  // set number of points
			}
			// Reset the beg poly point counter.
			chainidx=0;
		} 
		else if(clipresult==2) 
		{  // Chopped it
			drawpl=1;               // set flag to indicate need to draw
			numplpts=chainidx+1;    // set number of points
			
			// Reset the beg poly point counter.
			chainidx=0;
		}
		
		if (drawpl) 
		{
			pDrawDevice->Polyline(pPolyPts,numplpts);
		}	// end of draw the polyline block
		
		drawpl = 0;
		
	} // end of loop over points in poly list
	
	// at this point we've gone through this list item and we draw anything remaining
	
    if(chainidx) 
	{
		
		numplpts=chainidx+1;   
		
		pDrawDevice->Polyline(pPolyPts,numplpts);
		
	}
	
	if (NeedToDelete)
	{
		delete [] pPolyPts;
	}
}

void CEntityRenderer::drawDisplayObjects(gr_displayobject* beg, 
										 int width,
										 CDrawDevice* pDrawDevice,
										 db_drawing* flp, 
										 gr_view*gView,
										 POINT BltArea[2],
										 int* FstPt,
										 int xor,
										 int hl,
										 int undraw,
										 int undrawcol,
										 int fillmode)
{
	gr_displayobject*	pTmpDispObj;
	int					rop;
	bool				bUsingGDIPath = false;
	
	CDC*		pDC = pDrawDevice->getCDC();
	CIcadView*	pIcadView = SDS_CMainWindow->GetIcadView();
	
    //<alm>
    int hl_store = hl;
    //</alm>

	for(pTmpDispObj = beg; pTmpDispObj; pTmpDispObj = pTmpDispObj->next) 
	{
		int		face, ndims,
			iFillColor;
		
		if(pTmpDispObj->npts < 2 || !pTmpDispObj->chain)
			continue;
		
		ndims = ((pTmpDispObj->type & DISP_OBJ_PTS_3D) ? 1 : 0) + 2;
		face = ((pTmpDispObj->type & DISP_OBJ_PTS_CLOSED) != 0);
		
		if((pTmpDispObj->color == -1 || xor) && !undraw) 
		{
			rop = R2_XORPEN;
			pDrawDevice->SetDrawMode(RasterConstants::MODE_XOR);
		}
		else
		{
			rop = R2_COPYPEN;
			pDrawDevice->SetDrawMode(RasterConstants::MODE_COPY);
		}

        //<alm>
        if (pTmpDispObj->type & DISP_OBJ_HIGHLIGHTED)
        {
            hl = 1;
        }
        else
        {
            hl = hl_store;
        }
        //</alm>

        if((pTmpDispObj->type & DISP_OBJ_PTS_GDI_FILLPATH ||
			pTmpDispObj->type & DISP_OBJ_PTS_GDI_TRUETYPE) && !bUsingGDIPath)
		{
			// Prepare a GDI path for filling.
			pDC->SetPolyFillMode(ALTERNATE);
			pDC->BeginPath();
			iFillColor = pTmpDispObj->color;
			bUsingGDIPath = true;
		}
		
		// Now draw it!
		if(((pTmpDispObj->type & DISP_OBJ_PTS_FILLABLE) || (pTmpDispObj->type & DISP_OBJ_PTS_GDI_FILLPATH)) && fillmode)
        {
			// Solid Fill or a path for a Solid hatch or for a TTF text contour
			CEntityRenderer::outputPolygon(ndims,
				face,
				pTmpDispObj,
				pDrawDevice,
				gView,
				BltArea,
				FstPt,
				undraw,
				undrawcol,
				rop,
				hl,
				width,
				!(pTmpDispObj->type & DISP_OBJ_PTS_GDI_FILLPATH));	//Fill the interior only, do not draw outline.
			/*D.G.*/// ... if it's not a path of a solid hatch or TTF text contour!
        }
		else
			if(pTmpDispObj->type & DISP_OBJ_PTS_GDI_TRUETYPE)
				CEntityRenderer::outputPolyBezier(ndims,
				face,
				pTmpDispObj,
				pDrawDevice,
				gView,
				BltArea,
				FstPt,
				undraw,
				undrawcol,
				rop,
				hl,
				width);
			else
				CEntityRenderer::outputPolyline(ndims,
				face,
				pTmpDispObj,
				pDrawDevice,
				gView,
				BltArea,
				FstPt,
				undraw,
				undrawcol,
				rop,
				hl,
				width);
			
			if(bUsingGDIPath && (!pTmpDispObj->next || pTmpDispObj->GetSourceEntity() != pTmpDispObj->next->GetSourceEntity() ||
				(!(pTmpDispObj->next->type & DISP_OBJ_PTS_GDI_FILLPATH) &&
				!(pTmpDispObj->next->type & DISP_OBJ_PTS_GDI_TRUETYPE))))
			{
				// Now let's fill the GDI path.
				pDC->EndPath();

				if(IsPrinting())
					SetDcFillColorMem(undraw ? undrawcol : iFillColor, hl, rop, pDrawDevice);
				else
				{
					HBRUSH hBrush;
					if(hl)
					{
						resbuf	rb;
						sds_getvar("BKGCOLOR", &rb);
						int		BGColor = SDS_RGBFromACADColor(rb.resval.rint);
						hBrush = ::CreateHatchBrush(HS_DIAGCROSS, BGColor);
					}
					else
						hBrush = ::CreateSolidBrush(::SDS_BrushColorFromACADColor(undraw ? undrawcol : iFillColor));
					
					::DeleteObject(::SelectObject(pDC->m_hDC, hBrush));
				}
				
				if(hl)
				{
					int		oldBackMode = pDC->SetBkMode(TRANSPARENT);
					pDC->StrokeAndFillPath();
					pDC->SetBkMode(oldBackMode);
				}
				else
					pDC->StrokeAndFillPath();
				
				bUsingGDIPath = false;
			}
	} // end of loop over items (DispObjs) to draw
}

#ifndef _USE_DISP_OBJ_PTS_3D_
void CEntityRenderer::DrawEntity(db_handitem *elp,
								CDrawDevice *pDrawDevice,
								db_drawing *flp, 
								struct gr_view *gView,
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
								int width)
{
	struct gr_displayobject *beg=NULL,*end=NULL;
	db_handitem *telp;
	int freebeg=0;
	
	// get the target DC in case we have any TrueType text
	CDC*		pDC = pDrawDevice->getCDC();
	CIcadView*	pIcadView = SDS_CMainWindow->GetIcadView();
	
#if 0
	// DP: first version of code for displaying of 3D display objects
	// DP: now this functionality in rewritten DrawEntity()
	gr_displayobject* pOriginal = (struct gr_displayobject *)elp->ret_disp();
	gr_displayobject* pEnd = NULL;
	if(pOriginal == NULL)
	{
		telp=elp;
		// As an optimization, only use the VpLayerSettings if they really are going to matter
		//
		db_viewport *pViewport = NULL;
		if ( ( pViewportForVpLayerSettings != NULL ) &&
			( pViewportForVpLayerSettings->GetVpLayerFrozenLayerCount( ) > 0 ) )
		{
			pViewport = pViewportForVpLayerSettings;
		}
		
		// get the display objects
		int result = gr_getdispobjs(flp,
			NULL,
			&telp,
			&pOriginal,
			&pEnd,
			NULL,
			flp,
			gView,
			0,
			pDC,
			pViewport);

		if(pOriginal == NULL)
			return;

		elp->set_disp(pOriginal);
		freebeg = 1;
	}
	else
		freebeg = 1;

	int i, j, k;
	int inc; 
	
	beg = new gr_displayobject;
	beg->SetSourceEntity(pOriginal->GetSourceEntity());
	
	beg->next = NULL;
	beg->type = pOriginal->type & ~0x01;
	beg->color = pOriginal->color;
	beg->npts = pOriginal->npts;
	inc = 2 + (pOriginal->type & 0x01);
	
	beg->chain = new sds_real[(pOriginal->npts * 2 + 1)];
	for(i = 0, j = 0, k = 0; i < pOriginal->npts; ++i, j += inc, k += 2)
		gr_ucs2rp(&(pOriginal->chain[j]), &(beg->chain[k]), gView);
	
	end = beg;
	pOriginal = pOriginal->next;
	while(pOriginal != NULL)
	{
		end->next = new gr_displayobject;
		end->next->SetSourceEntity(pOriginal->GetSourceEntity());

		end->next->next = NULL;
		end->next->type = pOriginal->type & ~0x01;
		end->next->color = pOriginal->color;
		end->next->npts = pOriginal->npts;
		inc = 2 + (pOriginal->type & 0x01);

		end->next->chain = new sds_real[(pOriginal->npts * 2 + 1)];
		for(i = 0, j = 0, k = 0; i < pOriginal->npts; ++i, j += inc, k += 2)
			gr_ucs2rp(&(pOriginal->chain[j]), &(end->next->chain[k]), gView);

		end = end->next;
		pOriginal = pOriginal->next;
	}
#else
	if (bRedraw)
	{  // begin Redraw case
		beg=(struct gr_displayobject *)elp->ret_disp();
	}  //   end Redraw case
	else
	{  // begin Regen case
		telp=elp;
		// As an optimization, only use the VpLayerSettings if they really are going to matter
		//
		db_viewport *pViewport = NULL;
		if ( ( pViewportForVpLayerSettings != NULL ) &&
			( pViewportForVpLayerSettings->GetVpLayerFrozenLayerCount( ) > 0 ) )
		{
			pViewport = pViewportForVpLayerSettings;
		}
		
		
		// get the display objects
		int result = gr_getdispobjs(flp,
			NULL,
			&telp,
			&beg,
			&end,
			NULL,
			flp,
			gView,
			1,
			pDC,
			pViewport);
		
		if(result) 
			return;
        freebeg=1;
	}  //   end Regen case
#endif // 0
	
	if(beg != NULL)
		CEntityRenderer::drawDisplayObjects( beg, 
		width, 
		pDrawDevice, 
		flp, 
		gView, 
		BltArea, 
		FstPt, 
		xor, 
		hl, 
		undraw, 
		undrawcol, 
		fillmode);
	
		/*
		The last parameter, onlyFill is not used anymore.
		If the entity has polygon display object(s) and Polygon() member function is called,
		it will always fill the interior without drawing outline.
		
		  However, if a polyline diplay object has same start and end points, and
		  Polygon() member function is called from Polyline() to remove nubbin problems, 
		  it will not	fill in the interior, but it will draw outline.
		  
			The previous code did not work for blocks containing solid, dimension, etc...
			because entities inside a block does not go thru drawDisplayObjects, and
			there is no way to catch that.
	*/
	
	if (freebeg)
		gr_freedisplayobjectll(beg);
}
#endif

void CEntityRenderer::SetDcFillColorMem(int color, 
									   int hl, 
									   int rop, 
									   CDrawDevice *pDevice)
{
	// this is here just to placate the compiler
}

void CEntityRenderer::SetDcColorMem(int color, 
								   int width, 
								   int hl, 
								   int rop, 
								   CDrawDevice *pDevice)
{
	// this is here just to placate the compiler
}

