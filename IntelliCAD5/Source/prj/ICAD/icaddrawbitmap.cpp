
/* C:\DEV\PRJ\ICAD\ICADDRAWBITMAP.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Moved out of ICADAPI.CPP functions having to do with drawing to bitmaps
 * Also see ICADDRAW.CPP which contains entity drawing functions
 * 
 */ 

// ** Includes
#include "Icad.h"
#include "IcadApi.h"
#include "icadview.h"

#include "IcadPrintDia.h"
#include "lisp.h"
#include "cmds.h"
#include "db.h"
#include <eh.h>
#include <process.h>

#include <Wingdi.h>

#if defined(USE_SMARTHEAP)
	#include "smrtheap.h"
#endif

#ifdef DRAW_PSPACE_VPORTS
 #include "DDPlaneBuffer.h"
 #include "IcadEntityRenderer.h"
 #include "icaddoc.h"
#endif

#ifdef BMP_USE_HIDESHADE_STATUS
 #include "hideandshade.h"
#endif

extern int SDS_DontBitBlt;

//////// Preview for open dialog functions.
DWORD   SDS_PreviewThreadID;
HANDLE	SDS_PreviewThread;
char   *SDS_PreviewFilename;
HWND    SDS_PreviewHWND;
bool    SDS_PreviewStop;


typedef struct tagOLDRECT
{
  short left;
  short top;
  short right;
  short bottom;
} OLDRECT;

typedef struct {
  DWORD   key;
  WORD    hmf;
  OLDRECT bbox;
  WORD    inch;
  DWORD   reserved;
  WORD    checksum;
} ALDUSMFHEADER;


#define METAFILE_VERSION    1
#define ALDUSKEY            0x9AC6CDD7
#define ALDUSMFHEADERSIZE   22  // Avoid sizeof is struct alignment >1

int SDS_PaintPreviewThread(void *not_used);



CRect Calc(int bmpWid, int bmpDep, int wndWid, int wndDep)
{
    int d,w,x,y;
    if (bmpDep > bmpWid) {
        d = __min(bmpDep, wndDep);
        w = (bmpWid * d) / bmpDep;
        if (w > wndWid) {
            d = (d * wndWid) / w;
            w = wndWid;
        }
    }
    else {
        w = __min(bmpWid, wndWid);
        d = (bmpDep * w) / bmpWid;
        if (d > wndDep) {
            w = (w * wndDep) / d;
            d = wndDep;
        }
    }
    x = (wndWid - w) / 2;
    y = (wndDep - d) / 2;
    return CRect(x, y, x + w, y + d);
}

int SDS_PaintPreview(char *dwgfile,HWND hWnd) 
{
    int ret = RTERROR;

#ifdef REGEN_PREVIEW
	if(SDS_PreviewThreadID) {
		SDS_PreviewStop=TRUE;

		while(SDS_PreviewThreadID) {
			Sleep(5);
		}
		delete [] SDS_PreviewFilename;
		SDS_PreviewFilename=NULL;
	}

	SDS_PreviewStop=FALSE;
#endif // REGEN_PREVIEW
	if (dwgfile==NULL || hWnd==NULL) 
		return(RTNORM);
		
	CBrush cb(GetSysColor(COLOR_BTNFACE));

    CWnd* pWnd = new CWnd();
    pWnd->Attach(hWnd);
	CDC *cdc = pWnd->GetDC();
        
    CRect rect;
	pWnd->GetClientRect(&rect);
	cdc->FillRect(rect,&cb);

	cdc->SetBkMode(TRANSPARENT);
    cdc->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);

	void *pBuffer=NULL;
	unsigned long bytes;
    bool isBmp;
    if (SDS_CMainWindow->m_pFileIO.drw_getdwgbitmap(dwgfile, pBuffer, bytes, isBmp) && bytes) {

        CRect cr;
        int xOffset = 0, yOffset = 0;
        if (isBmp) {
            BITMAPINFOHEADER *pHeader = (BITMAPINFOHEADER *)pBuffer;

            cr = Calc(pHeader->biWidth, pHeader->biHeight, rect.Width(), rect.Height());

            unsigned char *p = (unsigned char*)pHeader;
            p += pHeader->biSize;
            switch(pHeader->biBitCount)
            {
            case 1:
                p += sizeof(RGBQUAD) * 2;
                break;
            case 4:
                p += sizeof(RGBQUAD) * 16;
                break;
            case 8:
                p += sizeof(RGBQUAD) * 256;
                break;
            }
            StretchDIBits(cdc->GetSafeHdc(), cr.left + xOffset, cr.top + yOffset, cr.Width(), 
                          cr.Height(), 0, 0, pHeader->biWidth, pHeader->biHeight, (const void *)p,
                          (CONST BITMAPINFO * )pHeader, DIB_RGB_COLORS, SRCCOPY); 
        }
        else {
            CDC     newDC;
            DWORD   dwIsAldus;
            METAHEADER*    mfHeader = NULL;
            ALDUSMFHEADER* aldusMFHeader= NULL;

            DWORD dwSize;
            DWORD seekpos;

            newDC.Attach(cdc->GetSafeHdc());
            dwIsAldus = *((DWORD*)pBuffer);
            if (dwIsAldus != ALDUSKEY)
                seekpos = 0;
            else {
                aldusMFHeader = (ALDUSMFHEADER*)pBuffer;
                seekpos = ALDUSMFHEADERSIZE;
            }
            BYTE *p = (BYTE *)pBuffer;
            mfHeader = (METAHEADER*)( p + seekpos);

            if ((mfHeader->mtType != 1) && (mfHeader->mtType != 2))
                return ret;

            dwSize = mfHeader->mtSize * 2;
            // Create the enhanced metafile
            HENHMETAFILE m_emf;
            m_emf = ::SetWinMetaFileBits(dwSize, (const BYTE*)mfHeader, NULL, NULL);

            CSize size(0,0);
            if (aldusMFHeader) {
                size.cx = 254 * (aldusMFHeader->bbox.right - aldusMFHeader->bbox.left)
                              / aldusMFHeader->inch;
                size.cy = 254 * (aldusMFHeader->bbox.bottom - aldusMFHeader->bbox.top)
                              / aldusMFHeader->inch;
            }

            cr = Calc(size.cx, size.cy, rect.Width(), rect.Height());
            cr.OffsetRect(xOffset, yOffset);
            newDC.PlayMetaFile(m_emf, &cr );
        }
		IC_FREE(pBuffer);
        ret = RTNORM;
	}
#ifdef REGEN_PREVIEW
    else {
		db_astrncpy(&SDS_PreviewFilename,dwgfile,strlen(dwgfile)+1);
		SDS_PreviewHWND=hWnd;
	   
		SDS_PreviewThread=SDS_CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SDS_PaintPreviewThread,NULL,0,&SDS_PreviewThreadID);    
        ret = RTNORM;
	}
#endif // REGEN_PREVIEW
	pWnd->ReleaseDC(cdc);
	pWnd->Detach();
	delete pWnd;
	return ret;
}


int SDS_PaintPreviewThread(void *not_used) 
	{

	int ret=RTERROR;
	CWnd* pWnd; 
	CDC *cdc;
	CRect rect;
	CBitmap* Bitmap = NULL;

	if( SDS_PreviewHWND && IsWindow(SDS_PreviewHWND) && (pWnd = new CWnd()) ) 
		{ 
								// make sure these go out of scope before thread
								// exits or resources leak
		CDC CmpDC;
		CBrush cb(GetSysColor(COLOR_BTNFACE));

		pWnd->Attach(SDS_PreviewHWND);
		cdc = pWnd->GetDC();
		pWnd->GetClientRect(&rect);
		cdc->FillRect(rect,&cb);

		cdc->SetBkMode(TRANSPARENT);
		cdc->TextOut(10,(rect.Height()/2)-10,"Loading preview...");

		Bitmap=SDS_DrawBitmap(NULL,SDS_PreviewFilename,NULL,NULL,rect.Width(),rect.Height(),TRUE);

		delete [] SDS_PreviewFilename;
		SDS_PreviewFilename = NULL;

		if( !Bitmap ) 
			{
			cdc->FillRect(rect,&cb);
			}
		else
			{
			if( CmpDC.CreateCompatibleDC(cdc) )
				{
				CBitmap *OldBitmap=CmpDC.SelectObject(Bitmap);
				cdc->BitBlt(0,0,rect.Width(),rect.Height(),&CmpDC,0,0,SRCCOPY);
				delete CmpDC.SelectObject(OldBitmap);
				}


			ret=RTNORM;
			}

		pWnd->ReleaseDC(cdc);
		pWnd->Detach();
		delete pWnd;
		}


	// Always supposed to CloseHandle on the thread handle 
	//
	CloseHandle( SDS_PreviewThread );
	SDS_PreviewThread = NULL;

	SDS_PreviewThreadID=0;

	SDS_ExitThread(RTNORM);

	return(RTNORM);
}


// *************************************
// PUBLIC FUNCTION
//
// SDS_DrawBitmap
//
// Used for creating preview images and for
// explorer's block view among other things
//	6/3/98 Expanded function to also handle drawing of a single entity.
//	Ename - handitem of Entity to Draw
//	bFill - BOOl indicating whether to fill Bitmap background with black.
//
CBitmap *SDS_DrawBitmap(db_drawing* flp, char* Fname, const char* Bname, db_handitem* Ename, int cx, int cy, BOOL bFill)
{
	CBitmap*	RetBmp = NULL;

	int			scrX = 0, scrY = 0, FstPt = 1, freedwg = 0;
	sds_name	ename;
	db_handitem	*elp, *blkelp;
	resbuf		*elist = NULL, rb, extmin, extmax, SaveExtmin, SaveExtmax, SaveVsmin, SaveVsmax,
				Saveucsxdir, Saveucsydir, Saveucsorg, Saveviewdir, SaveViewctr, SaveViewsize, SaveScreensize;
	//Modified Cybage SBD 24/12/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77950 from Bugzilla 
	resbuf		SaveViewtwist;
	CIcadView*	pView;
	gr_view*	Saveview;
	int			freeCview = 0;
	bool		dopal = TRUE;
    CBitmap		*pbmMem = NULL,
				*pbmOld = NULL;
	RECT		rect;
	CBrush		brush;
	HGDIOBJ		hOldPen = NULL,
				hOldBrush = NULL;
	CDC*		pDC = NULL;
	resbuf		plrb;
	bool		convertpl;

	SDS_getvar(NULL, DB_QPLINETYPE, &plrb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	convertpl = plrb.resval.rint == 2;

	if(cx < 1 || cy < 1)
		return NULL;

	if (SDS_CMainWindow->m_bIsRecovering & 0x01)
		return NULL;

	if(!flp) 
	{
		if(Fname) 
		{
			db_setfontsubstfn(NULL);
			flp = SDS_CMainWindow->m_pFileIO.drw_openfile(NULL, Fname, NULL, convertpl);
			db_setfontsubstfn(SDS_FontSubsMsg);
			freedwg = 1;
		} 
		else 
			flp = SDS_CURDWG;
	}

	SDS_CMainWindow->m_bUsingTempView = true;

#ifdef DRAW_PSPACE_VPORTS
	SDS_getvar(NULL, DB_QTILEMODE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
	extern bool SDS_InExplorer;
	bool	bTilemodeOff = !rb.resval.rint,
			bDrawWholeDrawing = flp && !Bname && !Ename && !SDS_InExplorer;
	int		saveWinX, saveWinY;

	if(bDrawWholeDrawing)
	{
		if(!(pView = SDS_CMainWindow->m_pCurView))
		{
			pView = new CIcadView;
			freeCview = 1;
		}
		saveWinX = pView->m_iWinX;	// SDS_ViewToVars2Use uses CIcadView's m_iWinX and m_iWinY instead
		saveWinY = pView->m_iWinY;	// of SCREENSIZE sysvar, so we should save these values and
		pView->m_iWinX = cx;		// change them temporarily.
		pView->m_iWinY = cy;		// CBitmapHLRenderer also uses them.
	}
	else
#endif // DRAW_PSPACE_VPORTS
	{
		pView = new CIcadView;
		freeCview = 1;
	}

	/* Don't return on not found file, this messes up the display in the IntelliCAD Explorer.
       We want some bitmap for a not found file, right now it will be a black rectangle.
	   Maybe a red cross or something like that would be better to indicate that something's wrong.
	if(!flp) 
		goto out;
	*/

	if(flp)
	{
		Saveview = pView->m_pCurDwgView;
		pView->m_pCurDwgView = NULL;

		gr_initview(flp, &SDS_CURCFG, &pView->m_pCurDwgView, NULL);

		if(Bname) 
		{
			if(!(elist = SDS_tblgetter("BLOCK"/*DNT*/, Bname, 0, flp)) || ic_assoc(elist, -2))
				goto out;
			blkelp = (db_handitem*)ic_rbassoc->resval.rlname[0];
			sds_relrb(elist);
		}
		else 
		{
			if(Ename)
				blkelp = Ename;
			else
				blkelp = flp->entnext_noxref(NULL);
		}

		//Modified Cybage SBD 24/12/2001 DD/MM/YYYY ]
		//Reason : Fix for Bug No. 77950 from Bugzilla 
		//Order of the variables is important
		if(Bname || Ename)
		{
			SDS_getvar(NULL, DB_QVIEWTWIST, &SaveViewtwist, flp, &SDS_CURCFG, &SDS_CURSES);
			rb.restype = RTREAL;
			rb.resval.rreal = 0.0;
			SDS_setvar(NULL, DB_QVIEWTWIST, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		}
		//Modified Cybage SBD 24/12/2001 DD/MM/YYYY ]
		
		SDS_getvar(NULL, DB_QEXTMAX,     &SaveExtmax,     flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QEXTMIN,     &SaveExtmin,     flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QUCSXDIR,    &Saveucsxdir,    flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QUCSYDIR,    &Saveucsydir,    flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QUCSORG,     &Saveucsorg,     flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QVIEWDIR,    &Saveviewdir,    flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QVIEWCTR,    &SaveViewctr,    flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QVIEWSIZE,   &SaveViewsize,   flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QSCREENSIZE, &SaveScreensize, flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QVSMAX,      &SaveVsmax,      flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QVSMIN,      &SaveVsmin,      flp, &SDS_CURCFG, &SDS_CURSES);
		
#ifdef DRAW_PSPACE_VPORTS
		if(!bDrawWholeDrawing)
#endif
		{	/*DG - 31.3.2003*/// Setup UCS first, then VIEWDIR (since VIEWDIR is considered as in UCS and therefore transformed in db_setvar from UCS t WCS).
			rb.restype = RT3DPOINT;
			rb.resval.rpoint[0] = 1.0;
			rb.resval.rpoint[1] = 0.0;
			rb.resval.rpoint[2] = 0.0;
			SDS_setvar(NULL, DB_QUCSXDIR, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
			rb.resval.rpoint[0] = 0.0;
			rb.resval.rpoint[1] = 1.0;
			SDS_setvar(NULL, DB_QUCSYDIR, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
			rb.resval.rpoint[1] = 0.0;
			SDS_setvar(NULL, DB_QUCSORG, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
			rb.resval.rpoint[2] = 1.0;
			SDS_setvar(NULL, DB_QVIEWDIR, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);

			gr_initview(flp, &SDS_CURCFG, &pView->m_pCurDwgView, NULL);
		}

		// Update the extmin and the extmax variables.
		if(Bname || Ename) 
		{
			ename[1] = (long)flp;
			for(elp = blkelp; elp && elp->ret_type() != DB_ENDBLK; elp = flp->entnext_noxref(elp))
			{
				ename[0] = (long)elp;
				//BugZilla No. 78202 ; 17-06-2002
				//SDS_UpdateEntDisp(ename, FstPt ? 7 : 3, pView->m_pCurDwgView);
				SDS_UpdateEntDisp(ename, FstPt ? 5 : 1, pView->m_pCurDwgView);
				if(elp->ret_disp())
					FstPt = 0;
			}
		}

		// Full the view with the new extents.
		SDS_getvar(NULL, DB_QEXTMAX, &extmax, flp, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QEXTMIN, &extmin, flp, &SDS_CURCFG, &SDS_CURSES);
#ifdef DRAW_PSPACE_VPORTS
		if(!bDrawWholeDrawing || !bTilemodeOff)
#endif
		{
			rb.resval.rpoint[0] = (extmax.resval.rpoint[0] + extmin.resval.rpoint[0]) * 0.5;
			rb.resval.rpoint[1] = (extmax.resval.rpoint[1] + extmin.resval.rpoint[1]) * 0.5;
			rb.resval.rpoint[2] = (extmax.resval.rpoint[2] + extmin.resval.rpoint[2]) * 0.5;
			rb.restype = RT3DPOINT;
			SDS_setvar(NULL, DB_QVIEWCTR, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		}

#ifdef DRAW_PSPACE_VPORTS
		if(!bDrawWholeDrawing || (bTilemodeOff && !SDS_PreviewFilename))
#endif
		{
			//calculate and set size of view taking aspect ratio into account
			double	x = extmax.resval.rpoint[0] - extmin.resval.rpoint[0],
					y = extmax.resval.rpoint[1] - extmin.resval.rpoint[1];
			if((x / y) > (cx / cy))
				rb.resval.rreal = x * cy / cx;
			else
				rb.resval.rreal = y;
			rb.restype = RTREAL;
			SDS_setvar(NULL, DB_QVIEWSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		}

		//calculate and set the new screen size
		rb.restype = RTPOINT;
		rb.resval.rpoint[0] = cx;
		rb.resval.rpoint[1] = cy;
		SDS_setvar(NULL, DB_QSCREENSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		
		gr_initview(flp, &SDS_CURCFG, &pView->m_pCurDwgView, NULL);
	} // if(flp)

	{
		HDC	syshdc;
		int	cols = GetDeviceCaps(syshdc = GetDC(NULL), NUMCOLORS);
		if(Bname && cols <= 20 && cols != -1)
			dopal = 0;
		ReleaseDC(NULL, syshdc);
	}

	// Setup the CDC for drawing.	
    pDC = new CDC();
	if(freeCview)
	{
		CDC*	tDC = SDS_CMainWindow->GetDC();

		if(dopal)
			tDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);

		pDC->CreateCompatibleDC(tDC);

		if(dopal)
			pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);

		pbmMem = new CBitmap();
		pbmMem->CreateCompatibleBitmap(tDC, cx, cy);
		pbmOld=pDC->SelectObject(pbmMem);

		hOldPen = SelectObject(pDC->m_hDC, CreatePen(PS_SOLID, 1, SDS_PenColorFromACADColor(7)));
		hOldBrush = SelectObject(pDC->m_hDC, CreateSolidBrush(SDS_BrushColorFromACADColor(7)));

		SDS_CMainWindow->ReleaseDC(tDC);
	}
	else
	{
		pDC->CreateCompatibleDC(pView->GetFrameBufferCDC());
		pbmMem = new CBitmap();
		pbmMem->CreateCompatibleBitmap(pView->GetFrameBufferCDC(), cx, cy);
		pbmOld = pDC->SelectObject(pbmMem);
	}

	if(dopal)
		pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);

	rect.left = 0;
	rect.top = 0;
	rect.right = cx;
	rect.bottom = cy;
	if(bFill)
	{
		brush.CreateSolidBrush(SDS_BrushColorFromACADColor(256));
		pDC->FillRect(&rect, &brush);
		brush.DeleteObject();
	}
	else
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(&rect, &brush);
		brush.DeleteObject();
	}

	if(flp)
	{
		// Now draw the entities into the CDC.
		pView->m_LastCol = -1;
		pView->m_fUseRegenList = FALSE;
		db_setfontsubstfn(NULL);

#ifdef DRAW_PSPACE_VPORTS
		if(bDrawWholeDrawing)
		{
#ifdef BMP_USE_HIDESHADE_STATUS
			if(pView->m_iViewMode != CIcadView::ViewMode::kNormal)
			{
				CBitmapHLRenderer	hlRenderer(pView, pDC);
				hlRenderer.render();
			}
			else
			{
#endif
				db_handitem*	pSavedVportTabHip = pView->m_pVportTabHip;
				db_drawing*		pSavedDrw = SDS_CMainWindow->m_pCurDoc ?
											SDS_CMainWindow->m_pCurDoc->m_dbDrawing : NULL;
				if(SDS_CMainWindow->m_pCurDoc)
					SDS_CMainWindow->m_pCurDoc->m_dbDrawing = flp;

				CDrawDevice* pDrawDevice = new CDDPlaneBuffer;
				pDrawDevice->create(pDC, CDrawDevice::eSINGLE_BUFFER);
				if(pDrawDevice)
				{
					extern bool	SDS_RedrawDrawing_Doing;
					SDS_RedrawDrawing_Doing = true;
					pDrawDevice->beginDraw(pDC, pView->m_pCurDwgView);
					int drawBuffer = pDrawDevice->setDrawBuffer(CDrawDevice::eFRONT);

					CIcadEntityRenderer	renderer(flp, pView);
					renderer.setDrawDevice(pDrawDevice);
					renderer.DrawDrawing(0, NULL, NULL);

					pDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
					pDrawDevice->endDraw();
					SDS_RedrawDrawing_Doing = false;
					delete pDrawDevice;
				}

				if(SDS_CMainWindow->m_pCurDoc)
					SDS_CMainWindow->m_pCurDoc->m_dbDrawing = pSavedDrw;
				pView->m_pVportTabHip = pSavedVportTabHip;
#ifdef BMP_USE_HIDESHADE_STATUS
			}
#endif
		}
		else
#endif // DRAW_PSPACE_VPORTS
		{
			for(elp = blkelp; elp && elp->ret_type() != DB_ENDBLK; elp = flp->entnext_noxref(elp))
			{
				if(SDS_PreviewStop) 
					goto out;

				SDS_DrawEntity(elp, pDC, flp, pView);
		
				// If it was a block free the display objects.
				if(Bname && elp->ret_disp())
				{
					gr_freedisplayobjectll(elp->ret_disp());
					elp->set_disp(NULL);
				}
			}
		}

		pView->m_LastCol = -1;

		// Restore all of the variables and reset the view.
		SDS_setvar(NULL, DB_QEXTMAX,     &SaveExtmax,     flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QEXTMIN,     &SaveExtmin,     flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QUCSXDIR,    &Saveucsxdir,    flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QUCSYDIR,    &Saveucsydir,    flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QUCSORG,     &Saveucsorg,     flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QVIEWDIR,    &Saveviewdir,    flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QVIEWCTR,    &SaveViewctr,    flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QVIEWSIZE,   &SaveViewsize,   flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QSCREENSIZE, &SaveScreensize, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QVSMAX,      &SaveVsmax,      flp, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QVSMIN,      &SaveVsmin,      flp, &SDS_CURCFG, &SDS_CURSES, 0);

		//Modified Cybage SBD 24/12/2001 DD/MM/YYYY [
		//Reason : Fix for Bug No. 77950 from Bugzilla 
		if(Bname || Ename)
			SDS_setvar(NULL, DB_QVIEWTWIST, &SaveViewtwist, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		//Modified Cybage SBD 24/12/2001 DD/MM/YYYY ]
	
		// Reset the view back to the current drawing.
		gr_freeviewll(pView->m_pCurDwgView);	
		pView->m_pCurDwgView = Saveview;

#ifdef DRAW_PSPACE_VPORTS
		pView->m_iWinX = saveWinX;
		pView->m_iWinY = saveWinY;
#endif

	} // if(flp)

	// Return the bitmap.
	RetBmp = pDC->SelectObject(pbmOld);

	// In this case, RetBmp is a copy of pbmMem, we don't want to delete pbmMem later
	pbmMem = NULL;

	if(hOldPen)
		DeleteObject(SelectObject(pDC->m_hDC, hOldPen));

	if(hOldBrush)
		DeleteObject(SelectObject(pDC->m_hDC, hOldBrush));

out: ;

	delete pDC;
	delete pbmMem;

	db_setfontsubstfn(SDS_FontSubsMsg);

	if(freedwg)
		delete flp;

	if(freeCview)
		delete pView;

	SDS_CMainWindow->m_bUsingTempView = false;

	if(SDS_PreviewStop) 
	{
		delete RetBmp;  
		return NULL;
	}

	return RetBmp;
}
