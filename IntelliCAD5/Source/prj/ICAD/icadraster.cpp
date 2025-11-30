/*	SOURCE\PRJ\ICAD\ICADRASTER.CPP - Image related routines
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *	This file contains image manipulation code. Most of this is specificially
 *  here for the hitachi developers.
 *
 */
//** Includes
#include "Icad.h"/*DNT*/
#include "IRasDef.h"/*DNT*/
#include "CVisioRaster.h"/*DNT*/   // See this header for documentation of all CVisioRaster methods
#include "objects.h"
#include "IcadView.h"
#include "configure.h"
#include "IcadApp.h"
#include "LicensedComponents.h"

//** Prototype for update function.
BOOL Ras_ProgressUpdate(short nPos);

//** Global Vars.
bool             Ras_TriedToLoad;
HINSTANCE        Ras_hDLL;
char         * (*Ras_pfnGetLastError) ();
CVisioRaster * (*Ras_newRaster) ();
void (*Ras_deleteRaster)(CVisioRaster *);

int Ras_InitRaster(void) {

#ifndef BUILD_WITH_RASTER_SUPPORT
	return 0;
#endif //BUILD_WITH_RASTER_SUPPORT

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::RASTER))
		return 0;

	int rc=0;
	char fs1[IC_ACADBUF];

	if(Ras_hDLL!=NULL) return(TRUE);
	if(Ras_TriedToLoad) return(FALSE);

	Ras_hDLL= LoadLibrary("HiisRas.dll"/*DNT*/);
	if (Ras_hDLL == NULL) { rc=__LINE__; goto out; }

	// Get a pointer to the functions hiisRas_newRaster and hiisGetLastError
	Ras_newRaster = (CVisioRaster * (*) ())GetProcAddress(Ras_hDLL, "hiisNewRaster"/*DNT*/);
	if (Ras_newRaster == NULL) { rc=__LINE__; goto out; }

	Ras_pfnGetLastError = (char * (*) ())GetProcAddress(Ras_hDLL, "hiisGetLastError"/*DNT*/);
	if (Ras_pfnGetLastError == NULL) { rc=__LINE__; goto out; }

	Ras_deleteRaster = (void(*)(CVisioRaster *))GetProcAddress(Ras_hDLL, "hiisDeleteRaster"/*DNT*/);
	if (Ras_deleteRaster == NULL) { rc=__LINE__; goto out; }

	out: ;

	Ras_TriedToLoad=TRUE;

	if(rc) {
		if(Ras_hDLL) FreeLibrary(Ras_hDLL);
		Ras_hDLL=NULL;
		sprintf(fs1,ResourceString(IDC_ICADRASTER_PROBLEM_LOADI_3, "Problem loading imaging code. (%i)" ),rc);
		sds_alert(fs1);
		return(FALSE);
	}
	return(TRUE);
}



// Bug fix #6407
void Ras_Unloadpixelmapbuffer(db_handitem *elp)
{
    if (((db_imagedef *)elp)->ret_imagedisp() != NULL)
        ((CVisioRaster *)((db_imagedef *)elp)->ret_imagedisp())->unloadImage();
}

void Ras_Reloadpixelmapbuffer(db_handitem *elp, const char* imageName, const char *szDLLPath)
{
    if (((db_imagedef *)elp)->ret_imagedisp() != NULL &&
        !((CVisioRaster *)((db_imagedef *)elp)->ret_imagedisp())->isLoaded())  {
        SDS_ProgressStart();
        ((CVisioRaster *)((db_imagedef *)elp)->ret_imagedisp())->loadImage(imageName, Ras_ProgressUpdate, szDLLPath);
        SDS_ProgressStop();
        sds_grtext(-2,"",0);
    }
}


CVisioRaster *Ras_GetCVisioRaster(sds_name ename) {
	if(ename==NULL || ename[0]==0L || ename[1]==0L) return(NULL);
	db_handitem *hip=(db_handitem *)ename[0];
	db_handitem *defref;
	hip->get_340(&defref);
	if(defref==NULL) return(NULL);
	return((CVisioRaster *)((db_imagedef *)defref)->ret_imagedisp());
}

CVisioRaster *Ras_GetCVRfromDef(sds_name imagedefname) {
	if(imagedefname==NULL || imagedefname[0]==0L || imagedefname[1]==0L) return(NULL);
	return((CVisioRaster *)((db_imagedef *)imagedefname[0])->ret_imagedisp());
}

HINSTANCE Ras_GetInstance(void) {
	if(Ras_hDLL==NULL) return(NULL);
    return(Ras_hDLL);
}

void Ras_rp2pix(
    struct gr_view *p_viewp,
    sds_real         p_rx,
    sds_real         p_ry,
    double          *p_ixp,
    double          *p_iyp) {
/*
**  Converts a point in real projection plane coordinates to the
**  equivalent point in pixels coordinates, based on the parameters set
**  in *p_viewp.
**
**  The Windows drawing functions don't seem to like pixel values
**  outside of the range of a short (+or- 16K), so there's some code
**  in here to limit the results to that range.  The idea is that
**  any endpoint of a line that crosses the screen that has a pixel
**  coordinate beyond the 16K range can be moved toward the center of the
**  screen until it is just inside that limit wihout shifting
**  the resulting image too badly.  The code in here is written
**  for speed, not beauty, so it may look a little awkward.
**
**  MAKE SURE YOU CALL gr_initview() BEFORE CALLING THIS
**  FUNCTION.  (The call has been omitted here for speed.)
*/
    sds_real dx,dy,adx,ady;

    if (p_viewp==NULL || icadRealEqual(p_viewp->viewsize,0.0)) return;

    /* Get pixel offset (and its abs val) from center of screen: */
//4M Item:99
//Prints upside down
/*
    adx=fabs(( dx=(p_rx-p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth() ));
    ady=fabs(( dy=(p_ry-p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight() ));
*/
    if (p_viewp->isprinting==2){
       adx=fabs(( dx=-(p_rx-p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth() ));
       ady=fabs(( dy=-(p_ry-p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight() ));
    }
    else{
       adx=fabs(( dx=(p_rx-p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth() ));
       ady=fabs(( dy=(p_ry-p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight() ));
    }
//<-4M


    *p_ixp=(dx+p_viewp->midwin[0]);
    *p_iyp=(dy+p_viewp->midwin[1]);
}

struct resbuf *
Ras_GetRasterVariables()
	{
	// Get the ename of the Named Object Dictionary:
    sds_name nod_ename;

	int nRes;
    nRes = sds_namedobjdict(nod_ename);
    if (nRes != RTNORM)
		{
        return NULL;
		}

    struct resbuf *rb;

    // Does it already have an image dictionary?
    rb = sds_dictsearch(nod_ename, "ACAD_IMAGE_VARS", 0);

	return rb;
	}

// Bug fix # 6405
int Ras_GetRasterQuality()
{
    // If there is no named object dictionary or no
    // ACAD_IMAGEVARS in the dictionary, this function will
    // return 1 for Ras::High quality. If it gets all the way
    // to the 70 group code of the RASTERVARIABLES, it will return
    // it's current value.
    int nVal = 1;

	struct resbuf *rb = Ras_GetRasterVariables();
	if ( rb != NULL )
		{
    	// 0 indicates success in ic_assoc
		// 71 is the Raster Quality
		//
		if ( ic_assoc(rb,71) == 0)
			{
			nVal = ic_rbassoc->resval.rint;
			}


	    sds_relrb(rb);
		}

    return(nVal);
}

int Ras_GetDisplayImageFramesFlag()
{
    // If there is no named object dictionary or no
    // ACAD_IMAGEVARS in the dictionary, we will return 1,
	// indicating we SHOULD display the image frame
	//
    int nVal = 1;


	struct resbuf *rb = Ras_GetRasterVariables();
	if ( rb != NULL )
		{
		// 0 indicates success in ic_assoc
		// 70 is the DisplayImageFramesFlags
		//
		if ( ic_assoc(rb,70) == 0)
			{
			nVal = ic_rbassoc->resval.rint;
			}


	    sds_relrb(rb);
		}

//    sds_relrb(rb);

    return(nVal);
	}


int Ras_PaintRaster(CIcadView *pView,db_handitem *elp,struct gr_view *gView,int drawmode,int color) {

	if(elp==NULL || pView==NULL || pView->m_bIsTempView) return(FALSE);
	if(!Ras_InitRaster()) return(FALSE);

    int rasterQuality;

	char fs1[IC_ACADBUF];
	int rc=0;
	static int haderror;
	CVisioRaster *pRaster = NULL;
	IRas::ErrorCode err;

	char *filename;
	int dispprops;
    int boundarytype;
	int clipon;
	int brightness;
	int contrast;
	int fade;
	int nclipverts;
    int wasloaded = 0;
	sds_point *ppt=NULL;
	sds_point ipt,imagesz,uvect,vvect;
	db_handitem *defref;
	db_handitem *defreactref;
	struct resbuf *elist;
	BOOL save_bMapToRGBColor;

	if(gView==NULL) gView=pView->m_pCurDwgView;

	elp->get_10(ipt);
	elp->get_70(&dispprops);
	elp->get_71(&boundarytype);
	elp->get_91(&nclipverts);
	elp->get_280(&clipon);
	elp->get_281(&brightness);
	elp->get_282(&contrast);
	elp->get_283(&fade);
	elp->get_340(&defref);
	elp->get_360(&defreactref);
	elp->get_13(imagesz);
	elp->get_11(uvect);
	elp->get_12(vvect);

	if(defref==NULL || !(dispprops&1)) goto out;

	ppt=new sds_point [2];
	elp->get_14(ppt,2);

	defref->get_1(&filename);

	pRaster=(CVisioRaster *)((db_imagedef *)defref)->ret_imagedisp();

    // Check to see if the image was loaded the last time the drawing was saved,
    // or regened. Allows for setting the 280 programmatically on or off too.
    ((db_imagedef *)defref)->get_280(&wasloaded);

	// It the image is not loaded, load it.
	if(pRaster==NULL && wasloaded) {

		if(((db_imagedef *)defref)->ret_looked4image()) goto out;

		char ffname[IC_ACADBUF];
		if(RTNORM!=sds_findfile(filename,ffname)) {
			if(NULL==strchr(filename,'\\') || RTNORM!=sds_findfile(strchr(filename,'\\')+1,ffname)) {
			    sds_printf(ResourceString(IDC_ICADRASTER__N___UNABLE_T_5, "\n-- Unable to locate image file %s. --\n" ),filename);
                // Hitachi fix to let them know when an image was not found.
                ((db_imagedef *)defref)->set_280(0);
				goto out;
			}
        }
        // Only set this if the image was found.
		((db_imagedef *)defref)->set_looked4image(1);

		// Create a new raster image
		pRaster = Ras_newRaster();
		if (pRaster == NULL) { rc=__LINE__; goto out; }

		// Set the units
		pRaster->setUnits(IRas::Inch);

		char dllpath[IC_ACADBUF];
		if(RTNORM!=sds_findfile(SDS_PROGRAMFILE,dllpath)) { rc=__LINE__; goto out; }
		if(strrchr(dllpath,'\\')) *strrchr(dllpath,'\\')=0;

		SDS_ProgressStart();
		sds_grtext(-2,ResourceString(IDC_ICADRASTER_LOADING_IMAGE_6, "Loading image..." ),0);
		if (IRas::eSuccess!=pRaster->loadImage(ffname,Ras_ProgressUpdate, dllpath)) {
			rc=__LINE__;  goto out;
			SDS_ProgressStop();
		}
		sds_grtext(-2,ResourceString(IDC_ICADRASTER__7, "" ),0);
		SDS_ProgressStop();
	}

	((db_imagedef *)defref)->set_imagedisp((void *)pRaster);

	if(pRaster==NULL) { rc=__LINE__; goto out; }

    // Bug# 6408 Hitachi bug fix.
    // MHB Assuming the frame has already been drawn.
    // Hitachi guys sitting right beside me when I made this change.
    if(!pRaster->isLoaded()) { rc=0; goto out; }

    sds_point ptRasterLL, ptRasterUL, ptRasterUR, ptRasterLR, ptViewLL, ptViewUR;
    struct resbuf rbucs,rbwcs;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;

	ic_ptcpy(ptRasterLL,ipt);

	ptRasterLR[0]=ptRasterLL[0]+(uvect[0]*imagesz[0]);
	ptRasterLR[1]=ptRasterLL[1]+(uvect[1]*imagesz[0]);
	ptRasterLR[2]=ptRasterLL[2]+(uvect[2]*imagesz[0]);

	ptRasterUL[0]=ptRasterLL[0]+(vvect[0]*imagesz[1]);
	ptRasterUL[1]=ptRasterLL[1]+(vvect[1]*imagesz[1]);
	ptRasterUL[2]=ptRasterLL[2]+(vvect[2]*imagesz[1]);

	ptRasterUR[0]=ptRasterLR[0]+(vvect[0]*imagesz[1]);
	ptRasterUR[1]=ptRasterLR[1]+(vvect[1]*imagesz[1]);
	ptRasterUR[2]=ptRasterLR[2]+(vvect[2]*imagesz[1]);

	//When setting the extent, calculate the minx, miny and maxx and maxy.  The extent is a rectangle
	//with axes parallel to the x & y axes.  It is not the LL and UR corner of the raster rectangle.

	//NOTE : we will set the extent only if it has not already been set.  The ent extent is maintained by
	//the display system (SDS_UpdateEntDisp) and is updated everytime the object is displayed, rotated, or
	//view changes are made.  IMO, we do not need to be setting the extent here at all, but will continue
	//to set it if it has never been set.  AM 12/16/98
    sds_point entextent[2];
	elp->get_extent (entextent[0],entextent[1]);
    // If it appears to be unset, set the local extents now
    if (icadRealEqual(entextent[0][0],0.0) && icadRealEqual(entextent[0][1],0.0) && icadRealEqual(entextent[0][2],0.0) &&
        icadRealEqual(entextent[1][0],0.0) && icadRealEqual(entextent[1][1],0.0) && icadRealEqual(entextent[1][2],0.0))
		{
		sds_point ptMin, ptMax;
		int i;
		for (i=0;i<3;i++)
		{
			ptMin[i] = min (ptRasterLL[i], min(ptRasterLR[i], min(ptRasterUR[i], ptRasterUL[i])));
			ptMax[i] = max (ptRasterLL[i], max(ptRasterLR[i], max(ptRasterUR[i], ptRasterUL[i])));
		}
		elp->set_extent(ptMin,ptMax);
		}

	sds_trans(ptRasterLL,&rbwcs,&rbucs,0,ptRasterLL);
	gr_ucs2rp(ptRasterLL,ptRasterLL,gView);
	Ras_rp2pix(gView,ptRasterLL[0],ptRasterLL[1],&ptRasterLL[0],&ptRasterLL[1]);

	sds_trans(ptRasterLR,&rbwcs,&rbucs,0,ptRasterLR);
	gr_ucs2rp(ptRasterLR,ptRasterLR,gView);
	Ras_rp2pix(gView,ptRasterLR[0],ptRasterLR[1],&ptRasterLR[0],&ptRasterLR[1]);

	sds_trans(ptRasterUL,&rbwcs,&rbucs,0,ptRasterUL);
	gr_ucs2rp(ptRasterUL,ptRasterUL,gView);
	Ras_rp2pix(gView,ptRasterUL[0],ptRasterUL[1],&ptRasterUL[0],&ptRasterUL[1]);

	sds_trans(ptRasterUR,&rbwcs,&rbucs,0,ptRasterUR);
	gr_ucs2rp(ptRasterUR,ptRasterUR,gView);
	Ras_rp2pix(gView,ptRasterUR[0],ptRasterUR[1],&ptRasterUR[0],&ptRasterUR[1]);

	ptViewLL[0] = 0;				ptViewLL[1] = pView->m_iWinY;
    ptViewUR[0] = pView->m_iWinX;	ptViewUR[1] = 0;

	elist=elp->entgetx(NULL,SDS_CURDWG);

    // Bug fix #6405
    // Get the current raster quality value from the 70 group code of the AcDbRasterVariables
    // class (acad class)
    if (pView->IsPrinting())
        rasterQuality = 1;
    else
        rasterQuality = Ras_GetRasterQuality();

	//Set the MapToRGBColor on, so that Hitachi always gets an RGB COLORREF.  Bug #8239.
	save_bMapToRGBColor = SDS_CMainWindow->m_bMapToRGBColor;
	SDS_CMainWindow->m_bMapToRGBColor = TRUE;

    err = pRaster->drawRaster(*pView->GetTargetDeviceCDC(),
                              ptRasterLL,
                              ptRasterUL,
                              ptRasterUR,
                              ptRasterLR,
                              ptViewLL,
                              ptViewUR,
							  SDS_PenColorFromACADColor(256),
							  SDS_PenColorFromACADColor(color),
							  drawmode,
							  rasterQuality == 1 ? IRas::High : IRas::Draft,
							  elist);

	sds_relrb(elist);  elist=NULL;

	SDS_CMainWindow->m_bMapToRGBColor = save_bMapToRGBColor;

	if(!pView->IsPrinting()) SDS_BitBlt2Scr(0);

    if (IRas::eSuccess != err) { rc=__LINE__; goto out; }

	out: ;

	delete [] ppt;

	if(rc==0) return(TRUE);

	((db_imagedef *)defref)->set_imagedisp(NULL);
	delete pRaster;  pRaster=NULL;
#if !defined(DEBUG)
	if(haderror) return(FALSE);
#endif
	char * szError;
	szError = Ras_pfnGetLastError();
	if(rc==139) {
		sprintf(fs1,ResourceString(IDC_ICADRASTER_CAN_T_LOAD_IM_8, "Can't load image %s.\nThis file type is currently not supported." ),filename);
		haderror=0;
	} else if(rc==141) {
		sprintf(fs1,ResourceString(IDC_ICADRASTER_CAN_T_LOAD_IM_9, "Can't load image %s.\nUnsupported 32 bit alpha channel image." ),filename);
		haderror=0;
	} else {
		    sprintf(fs1,ResourceString(IDC_ICADRASTER_PROBLEM_LOAD_10, "Problem loading image %s.\n(%i) %s" ),filename,rc,szError);
	}
    if (wasloaded)  // If it used to be loaded and we got here, do the alert!
	    sds_alert(fs1);
	haderror=1;
	return(FALSE);
}

BOOL Ras_ProgressUpdate(short nPos)
{
	SDS_ProgressPercent(nPos);
    // return 0 if the load procedure should stop, otherwise the load will continue.
	return(1);
}

void *Ras_GetHiis(void)
{
	// Get a pointer to the functions hiisRas_newRaster and hiisGetLastError
	void *(*tmpFunc)() = (void * (*) ())GetProcAddress(Ras_hDLL, "hiisGetHiisInstance"/*DNT*/);
	if (tmpFunc == NULL) return(NULL);
	return tmpFunc();
}

void *Ras_OpenRaster(sds_name imagedef_name)
{
	if(imagedef_name==NULL || imagedef_name[0]==0L) return(NULL);
	CVisioRaster *pRaster;
	pRaster=(CVisioRaster *)((db_imagedef *)imagedef_name[0])->ret_imagedisp();

	long cookie;
	void *result = pRaster->openRaster(&cookie);
	SDS_SetUndo(IC_UNDO_IMAGE, (void *)imagedef_name[0], (void *) cookie, NULL, SDS_CURDWG);

	return result;
}

void Ras_CloseRaster(sds_name imagedef_name)
{
	if(imagedef_name==NULL || imagedef_name[0]==0L) return;
	CVisioRaster *pRaster;
	pRaster=(CVisioRaster *)((db_imagedef *)imagedef_name[0])->ret_imagedisp();
	pRaster->closeRaster();
	return;
}

