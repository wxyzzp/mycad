/* C:\ICAD\PRJ\LIB\DRW\DRWFNS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * "smaller" member fns for drw_readwrite, and helper fns for drw generally
 *
 */

/*DG - 28.5.2002*/// Use ODT versions defines in odtver.h instead of one(s) in configure.h.
#include "odtver.h"
/*
//COSMO (ShokuY) ->
// OpenDWG ODT 2.008 adaption switch(BUILD_WITH_ODT2008GLE) added 2002-04-01
#include "..\..\ICAD\Configure.h"
//<-COSMO
*/

#include "Db.h"
#include "Drw.h"
#include "drwprotos.h"
#include "dwgcnvtdef.h"
#include "BinaryResource.h"
#include "dimstyletabrec.h"

#include "OdaCommon.h"
#include "OdToolKit.h"
#include "OdString.h"
#include "ExSystemServices.h"
#include "ExHostAppServices.h"
#include "ModelerGeometryImpl.h"
#include "CollectionClasses.h"

#include "DWGdirectReader.h"
#include "ThumbnailImage.h"

drw_readwrite::drw_readwrite(bool bCheckOnLoad /* = true */):
m_bCheckOnLoad(bCheckOnLoad)
{
    m_CurConfig = NULL;
}

drw_readwrite::~drw_readwrite()
{
}

bool drw_readwrite::drw_getdwgbitmap(const char *pFielName, void* &pBuffer, unsigned long &bytes, bool &isBmp)
{
	ASSERT(pFielName != NULL);

	bool retVal = false;

    pBuffer = NULL;
    bytes = 0L;

	OdString fileReadName;
	char qualifiedName[DB_PATHNAMESZ];
	try {
		// Mostly for xref's without fully qualified file names.
		if (ic_findinpath(NULL, pFielName, qualifiedName, DB_PATHNAMESZ) == RTNORM) {
			OdString fileReadName(qualifiedName);
			fileReadName.makeUpper();

	        OdRxObjectImpl<DWGdirectReaderServices> exServices;

	        OdThumbnailImage image;
	        odDbGetPreviewBitmap(exServices.createFile(fileReadName), &image);

            OdBinaryData binaryData;
            if (image.hasBmp()) {
	            binaryData = image.bmp;
                isBmp = true;
            }
            else if (image.hasWmf()) {
                binaryData = image.wmf;
                isBmp = false;
            }
            else
                return false;

	        bytes = binaryData.length() * sizeof(OdUInt8);

            pBuffer = new char* [bytes];
            memcpy(pBuffer, (char*)binaryData.asArrayPtr(), bytes);
            retVal = true;
        }
	}
	catch (OdError& err) {
		OdError err1 = err;
#ifdef _DEBUG_MHB
		int error = err.code();
		if (error == 30) // 'Can't open file'	// Probably a missing xref.
		{
			char sBuf[100];
			sprintf(sBuf, "Can't open file: %s\n", fileName);
			OutputDebugString(sBuf);
		}
		else if (error == 128)	// User hit cancel on password dialog.
		{
			;// Do nothing.
		}
		else
		{
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader file read error: ");
		}
#endif
	}
    catch (...) {
#ifdef _DEBUG_MHB
		ASSERT(!"DWGdirectReader file read error: unknown");
#endif
	}

	return retVal;
}

void drw_progress(short perc) {
    // *0.95 so we have some progress left fo the convert.
    db_progresspercent((short)((double)perc*0.95));
    return;
}

static
sds_resbuf*
GetPreviousRb(
  sds_resbuf*   rb,
  sds_resbuf*   rbDest
)
{
  sds_resbuf* rbPrevious = NULL;
  while (rb != NULL)
    {
      if (rb->rbnext == rbDest)
        {
          rbPrevious = rb;
          break;
        }
      rb = rb->rbnext;
    }
  return rbPrevious;
}

static
BOOL
RemoveEed(
  sds_resbuf*   rb,
  sds_resbuf*   rbDest
)
{
  sds_resbuf* rbPrevious = GetPreviousRb(rb, rbDest);
  if (rbPrevious != NULL)
    {
      rbPrevious->rbnext = rbDest->rbnext->rbnext;
      rbDest->rbnext->rbnext = NULL;
      sds_relrb(rbDest);
    }

  return TRUE;
}

static
BOOL
AddNewEed(
  sds_resbuf*   rbDest,
  const int     iCode1,
  const int     iValue1,
  const int     iCode2,
  const int     iValue2
)
{
  if (rbDest == NULL)
    {
      return FALSE;
    }

  sds_resbuf* rb = rbDest->rbnext;
  if (rb == NULL)
    {
      return FALSE;
    }

  sds_resbuf* rbNew1 = sds_newrb(1070);
  sds_resbuf* rbNew2 = sds_newrb(1070);

  rbNew1->rbnext      = rbNew2;
  rbNew1->resval.rint = iCode1;
  rbNew2->rbnext      = rb->rbnext;
  rbNew2->resval.rint = iValue1;

  // Change DIMFIT to DIMTMOVE, or DIMUNIT to DIMLUNIT
  rbDest->resval.rint = iCode2;
  rb->rbnext          = rbNew1;
  rb->resval.rint     = iValue2;

  return TRUE;
}

static
BOOL
ChangeEed(
  sds_resbuf*   rbDest,
  const int     iCode,
  const int     iValue
)
{
  if (rbDest == NULL)
    {
      return FALSE;
    }

  sds_resbuf* rb = rbDest->rbnext;
  if (rb == NULL)
    {
      return FALSE;
    }

  // Change value
  rbDest->resval.rint         = iCode;
  rbDest->rbnext->resval.rint = iValue;

  return TRUE;
}

struct TDimoverrideBuf
{
  sds_resbuf* fit;
  sds_resbuf* atfit;
  sds_resbuf* tmove;
  sds_resbuf* unit;
  sds_resbuf* lunit;
  sds_resbuf* frac;
  // EBATECH(CNBR) -[ Bugzilla#78443 dimazin, dimadec
  sds_resbuf* azin;
  sds_resbuf* adec;
  // EBATECH(CNBR) ]-
};

static
BOOL
GetDimoverrideBuf(
  sds_resbuf*       rbb,
  TDimoverrideBuf&  dov
)
{
  sds_resbuf* rb;
  BOOL bAcad   = FALSE;
  BOOL bDstyle = FALSE;
  dov.fit   = NULL;
  dov.atfit = NULL;
  dov.tmove = NULL;
  dov.unit  = NULL;
  dov.lunit = NULL;
  dov.frac  = NULL;
  for(rb = rbb; rb != NULL; rb = rb->rbnext)
    {
      short iType = rb->restype;
      if (rb->restype == 1000)
        {
          if (bAcad)
            {
              if (stricmp(rb->resval.rstring, "DSTYLE"/*DNT*/) == 0)
                {
                  bDstyle = TRUE;
                }
            }
        }
      else if (rb->restype == 1001)
        {
          if (stricmp(rb->resval.rstring, "ACAD"/*DNT*/) == 0)
            {
              bAcad = TRUE;
            }
        }
      else if (rb->restype == 1070)
        {
          if (bDstyle)
            {
              switch (rb->resval.rint)
                {
                  case 287:
                      dov.fit   = rb;
                      break;
                  case 289:
                      dov.atfit = rb;
                      break;
                  case 279:
                      dov.tmove = rb;
                      break;
                  case 270:
                      dov.unit  = rb;
                      break;
                  case 277:
                      dov.lunit = rb;
                      break;
                  case 276:
                      dov.frac  = rb;
                      break;
                  // EBATECH(CNBR) -[ Bugzilla#78443 dimazin, dimadec
                  case 79:
                      dov.azin  = rb;
                      break;
                  case 179:
                      dov.adec  = rb;
                      break;
                  // EBATECH(CNBR) ]-
                }
              rb = rb->rbnext;

              // SystemMetrix(Hiro)-[FIX: For dimension which has invalid EED is set in EED of dimstyle.
              if (rb == NULL)
                {
                  break;
                }
              // ]-SystemMetrix(Hiro) FIX: For dimension which has invalid EED is set in EED of dimstyle.
            }
        }
    }

  return TRUE;
}

// If the type of drawing is R14, eed must change to 2000.
//static	MHB removed static so I could use it in dwgload.cpp for DWGdirect conversion.
BOOL
ChangeEedAfterLoading(
  db_drawing*   dp,
  db_handitem*  hip,
  sds_resbuf*   rbb
)
{
  struct TDimoverrideBuf dov;
  GetDimoverrideBuf(rbb, dov);

  // If DIMFIT exist in eed.
  if (dov.fit)
    {
      if ((dov.atfit) || (dov.tmove))
        {
          // This case happens by AutoCAD 2000 saving by of R14 format.

          // remove DIMFIT
          RemoveEed(rbb, dov.fit);
        }
      else
        {
          // This case happens by AutoCAD R14 saving.

          // get DIMATFIT and DIMTMOVE
          int fit = dov.fit->rbnext->resval.rint;
          int atfit = fit;
          int tmove = 0;
          if (fit > 3)
            {
              atfit = 3;
              tmove = fit - 3;
            }

          // Add DIMATFIT
          AddNewEed(dov.fit, 289, atfit, 279, tmove);
        }
    }

  // If DIMUNIT exist in eed.
  if (dov.unit)
    {
      if ((dov.lunit) || (dov.frac))
        {
          // This case happens by AutoCAD 2000 saving by of R14 format.

          // remove DIMUNIT
          RemoveEed(rbb, dov.unit);
        }
      else
        {
          // This case happens by AutoCAD R14 saving.

          // get DIMLUNIT and DIMFRAC
          int unit = dov.unit->rbnext->resval.rint;
          int lunit = unit;
          int frac = 0;

          if (unit >= 6)
            {
              lunit -= 2;
            }

          switch (unit)
            {
              case 4:
              case 5:
              case 6:
              case 7:
                  if ((unit == 6) || (unit == 7))
                    {
                      frac = 2;
                    }
                  break;
            }

          // Add DIMLUNIT
          AddNewEed(dov.unit, 277, lunit, 276, frac);
        }
    }

  return TRUE;
}

db_dimstyletabrec*
GetValFromDimstyle(
  db_drawing*   dp,
  db_handitem*  hip
)
{
  db_dimstyletabrec* dship = NULL;
  if (hip != NULL)
    {
      if (hip->ret_type() == DB_DIMENSION)
        {
          char* name;
          hip->get_3(&name);
          dship = (db_dimstyletabrec*)dp->findtabrec(DB_DIMSTYLETAB, name, 1);

//          sds_resbuf* rbDstyle = sds_tblsearch("DIMSTYLE"/*DNT*/, name, 0);
/*
          while (rbDstyle)
            {
              if (rbDstyle->restype == iCode)
                {
                  return rbDstyle->resval.rint;
                }
              rbDstyle = rbDstyle->rbnext;
            }
*/
        }
    }
  return dship;
}

// If the type of drawing is R14, eed must change to R14.
static
BOOL
ChangeEedBeforeSaving(
  db_drawing*   dp,
  db_handitem*  hip,
  sds_resbuf*   rbb
)
{
  struct TDimoverrideBuf dov;
  GetDimoverrideBuf(rbb, dov);

  db_dimstyletabrec* dship = GetValFromDimstyle(dp, hip);
  // SystemMetrix(Hiro)-[FIX: In case of NULL pointer
  if (dship == NULL)
    {
      return TRUE;
    }
  // ]-SystemMetrix(Hiro) FIX: In case of NULL pointer

  // EBATECH(CNBR) -[ Bugzilla#78443 dimazin, dimadec
  if(dov.azin)
  {
    RemoveEed(rbb, dov.azin);
  }
  if(dov.adec)
  {
    RemoveEed(rbb, dov.adec);
  }
  // EBATECH(CNBR) ]-

  BOOL bChange = FALSE;
  int atfit = 0;
  int tmove = 0;
  sds_resbuf* rb;
  if (dov.atfit)
    {
      // If DIMATFIT exist in eed.
      rb = dov.atfit;
      atfit = rb->rbnext->resval.rint;

      if (dov.tmove)
        {
          // get DIMTMOVE from dimoverride
          tmove = dov.tmove->rbnext->resval.rint;
          RemoveEed(rbb, dov.tmove);
        }
      else
        {
          // get DIMTMOVE from dimstyle
          dship->get_279(&tmove);
        }

      // change DIMATFIT fo DIMFIT
      bChange = TRUE;
    }
  else if (dov.tmove)
    {
      // If DIMTMOVE exist in eed.
      rb = dov.tmove;
      tmove = rb->rbnext->resval.rint;

      // get DIMATFIT from dimstyle
      dship->get_289(&atfit);

      // change DIMTMOVE fo DIMFIT
      bChange = TRUE;
    }

  if (bChange)
    {
      int fit = atfit;
      if (tmove == 1)
        {
          fit = 4;
        }
      else if (tmove == 2)
        {
          fit = 5;
        }
      ChangeEed(rb, 287, fit);
    }

  bChange = FALSE;
  int lunit = 0;
  int frac  = 0;
  if (dov.lunit)
    {
      // If DIMLUNIT exist in eed.
      rb = dov.lunit;
      lunit = rb->rbnext->resval.rint;

      if (dov.frac)
        {
          // get DIMFRAC from dimoverride
          frac = dov.frac->rbnext->resval.rint;
          RemoveEed(rbb, dov.frac);
        }
      else
        {
          // get DIMFRAC from dimstyle
          dship->get_276(&frac);
        }

      // change DIMLUNIT fo DIMUNIT
      bChange = TRUE;
    }
  else if (dov.frac)
    {
      // If DIMFRAC exist in eed.
      rb = dov.frac;
      frac = rb->rbnext->resval.rint;

      // get DIMLUNIT from dimstyle
      dship->get_277(&lunit);

      // change DIMFRAC fo DIMUNIT
      bChange = TRUE;
    }

  if (bChange)
    {
      int unit = lunit;
      switch (unit)
        {
          case 4:
          case 5:
              if (frac == 2)
                {
                  unit += 2;
                }
              break;
          case 6:
              unit += 2;
              break;
        }
      ChangeEed(rb, 270, unit);
    }

  return TRUE;
}

void drw_findfile(char *shapefile,char *newshapefile,void *userdata)
{
	if( sds_findfile( shapefile, newshapefile ) != RTNORM){
		char	szFileName[MAX_PATH] = {0}, szFileExt[MAX_PATH] = {0};
		_splitpath(shapefile, NULL, NULL, szFileName, szFileExt);
		strcat(szFileName, szFileExt);
		if(sds_findfile(szFileName, newshapefile) != RTNORM){
			strcpy( newshapefile, shapefile );
		}
	}
	userdata = userdata;
}
