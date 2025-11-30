#include "cmds.h"
#include "IcadView.h"
#include "DoBase.h"
#include "DbMText.h"
// *****************************************
// This is an SDS External API
//
int
sdsengine_textbox(const struct sds_resbuf *prbTextEnt, sds_point ptCorner, sds_point ptOpposite)
{

	ptCorner[0] = ptCorner[1] = ptCorner[2] = 0.0;
	ptOpposite[0] = ptOpposite[1] = ptOpposite[2] = 0.0;
	int iRetval = RTERROR;

	ASSERT( OnEngineThread() );

    struct resbuf *tmp, *entBuf;
    // Looking to see if it is an actual entity or just a resbuf chain. If there is no
    // -1 group, it's just a resbuf chain. Otherwise it is in the database, and gr_textbox can
    // handle it. Even an existing mtext entity.
    entBuf = const_cast<struct sds_resbuf *>(prbTextEnt);
	tmp = ic_ret_assoc( entBuf, 0 );

    if (tmp != NULL &&
		(strisame(tmp->resval.rstring, "TEXT"/*DNT*/) ||
		strisame(tmp->resval.rstring, "ATTDEF"/*DNT*/) ||
		strisame(tmp->resval.rstring, "ATTRIB"/*DNT*/)))
	{
        // If it is not mtext, then let gr_textbox handle it.
		CDC *dc = NULL;
		CIcadView *icadView = SDS_CMainWindow->GetIcadView();
		if (icadView)
		{
			ASSERT_KINDOF(CIcadView, icadView);
			dc = icadView->GetTargetDeviceCDC();
		}
		else
			return RTERROR;

        return(gr_textbox((struct resbuf *)prbTextEnt,ptCorner,ptOpposite,SDS_CURDWG,SDS_CURGRVW,dc));

	}
	// Modified Cybage VSB 06/08/2001 [
	// Reason: Bug No 77820 from BugZilla.TextBox command through lisp program crashes for BLOCKS, dimensions TEXT etc.
	// (textbox (cdr (entget (entlast))))
	else  if (tmp != NULL && strisame(tmp->resval.rstring, "MTEXT"/*DNT*/))
	// Modified Cybage VSB 06/08/2001 ]
	{
        // Here we are probably dragging a dimension. But anyway we are trying to get
        // the text box for an mtext item that may or may not be in the data base. If
        // not, we will create a dummy, and then call SDS_UpdateEntDisp() which will
        // inturn call set extents for this mtext item. Then we can get extents
        // to get the actual textbox.
        ASSERT(entBuf != NULL);

        bool haveDbaseEnt = FALSE;
        sds_name ename;
        CDbMText *elp = new CDbMText(SDS_CURDWG);

        for (; entBuf != NULL; entBuf = entBuf->rbnext)
		{
            switch (entBuf->restype)
			{
			case -1:
				delete elp;
				elp = (CDbMText*)entBuf->resval.rlname[0];
				ename[0]=(long)entBuf->resval.rlname[0];
				ename[1]=(long)entBuf->resval.rlname[1];
				haveDbaseEnt = TRUE;
                break;
			case 1:
				elp->set_1(entBuf->resval.rstring);
                break;
			case 7:
				elp->set_7(entBuf->resval.rstring, SDS_CURDWG);
#if 0
				struct sds_resbuf *estyle;
				estyle=sds_tblsearch("STYLE"/*DNT*/,entBuf->resval.rstring,0);
				ic_assoc(estyle,40);
				ASSERT(ic_rbassoc != NULL);
				if (ic_rbassoc->resval.rreal == 0)
				{
					elp->set_40(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval);
				}
				else
					elp->set_40(ic_rbassoc->resval.rreal);
				sds_relrb(estyle);
#endif
                break;
                case 10:
                    elp->set_10(entBuf->resval.rpoint);
					break;
                case 40:
                    elp->set_40(entBuf->resval.rreal);
					break;
                case 41:
                    elp->set_41(entBuf->resval.rreal);
					break;
				// EBATECH(CNBR) -[ 2002/6/10 Sometimes these value is wrong.
				// EBATECH(CNBR) -[ When eval (textbox), the following value is cleared.
                //case 42:
                //    elp->set_42(entBuf->resval.rreal);
				//	break;
                //case 43:
                //    elp->set_43(entBuf->resval.rreal);
				//	break;
				// EBATECH(CNBR) ]-
				// EBATECH(CNBR) ]-
                case 50:
                    elp->set_50(entBuf->resval.rreal, SDS_CURDWG);
					break;
                case 71:
                    elp->set_71(entBuf->resval.rint);
					break;
			}
		}
        if (!haveDbaseEnt)
		{
			ename[0]=(long)elp;
			ename[1]=(long)SDS_CURDWG;
		}
		// DP: old code didn't set extents of entity
		//SDS_UpdateEntDisp(ename,0);
		SDS_UpdateEntDisp(ename,2);

#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in sdsTextbox.cpp")
		CD3 minPoint, maxPoint;
		CDoBase::getPlanarBoundingBox(SDS_CURGRVW, elp, minPoint, maxPoint);
		memcpy(ptCorner, (sds_real*)minPoint, 3 * sizeof(sds_real));
		memcpy(ptOpposite, (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
        elp->get_extent(ptCorner, ptOpposite);
#endif
        // This is to mimic acad. It always has the ptCorner set to {0.0,0.0,0.0}.
        // EBATECH(CNBR) -[ 2002/6/10 elsewhere should be positive.
        //ptOpposite[SDS_X] = ptOpposite[SDS_X] - ptCorner[SDS_X];
        //ptOpposite[SDS_Y] = ptOpposite[SDS_Y] - ptCorner[SDS_Y];
        ptOpposite[SDS_X] = fabs(ptOpposite[SDS_X] - ptCorner[SDS_X]);
        ptOpposite[SDS_Y] = fabs(ptOpposite[SDS_Y] - ptCorner[SDS_Y]);
        // EBATECH(CNBR) ]-
        ptCorner[0] = ptCorner[1] = 0.0;
        // Get rid of it now.
        if (!haveDbaseEnt)
		{
			delete elp;
		}

		iRetval = RTNORM;
	}
	return iRetval;
}

//////////////////////////////////////////////////////////////////////////////
// cmd_getboxintersec Function
//
// This function calcualtes the intersection points of a line passing through a box.
//              _____________________________intpt[0] or intpt[1].
//             |
//             |    ____ Line Crossing box.
//             |  /|____ pt1 or pt2
//             | /
//       ______|/________
//      |                |
//      |________________|
//          /|_____________________intpt[0] or intpt[1].
//         /_______________________ pt2 or pt1
//
// txtbox[0] : Bottom left corner of the box.
// txtbox[1] : Top right corner of the box.
// anglec    : The angle of the box.
// mpt       : The point in the ucs the box will be located.
// pt1       : First vertex.
// pt2       : Seconed vertex.
// intpt[0]  : First intersection point.
// intpt[1]  : Second intersection point.
// nbox      : Array of four points that make up the new coords of the box after it has
//             been moved and rotated.
//
//  Return values for ic_linexline:
//
//      -1 = At least 1 line undefined (coincident points), or both parallel.
//       0 = Intersection is not on either segment.
//       1 = Intersection is on the 1st segment (p0-p1) but not the 2nd.
//       2 = Intersection is on the 2nd segment (p2-p3) but not the 1st.
//       3 = Intersection is on both line segments.
//
BOOL cmd_getboxintersec(sds_point *txtbox,sds_real anglec,sds_point mpt,sds_point pt1,sds_point pt2,sds_point *intpt,sds_point *nbox,short *FirstLVal,short *SecondLVal)
{
    sds_point ptp  [4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};   // this is the point picked.
    short     fi1 = 0;
    sds_real  cosdeg = 0.0,
              sindeg = 0.0;

    cosdeg=cos(anglec); sindeg=sin(anglec);
    memset(nbox,0,(sizeof(sds_point)*4));
    // Copy the bottom left corner to xpt[0] and xpt[2], and top right corner to xpt[1] and [2].
    ic_ptcpy(ptp[0],txtbox[0]); ic_ptcpy(ptp[1],txtbox[1]); ic_ptcpy(ptp[2],txtbox[0]);
    ic_ptcpy(ptp[3],txtbox[1]); ptp[0][1]=txtbox[1][1]; ptp[3][1]=txtbox[0][1];
    // ========================================================================================================================================================
    // This is here for the new reconstruction. ===============================================================================================================
    // ========================================================================================================================================================
    // This moves the center of the box to the center of the coord. system.
    //
    //          |
    //          |
    //      _________
    //     |         |
    // ----|----.----|----
    //     |_________|
    //          |
    //          |
    //          |
    //
    for (fi1=0;fi1<4;++fi1) ptp[fi1][0]-=(txtbox[1][0]/2);
    for (fi1=0;fi1<4;++fi1) ptp[fi1][1]-=(txtbox[1][1]/2);
    // rotate the box (anglec) degrees.
    nbox[0][0]=(ptp[0][0]*cosdeg-ptp[0][1]*sindeg); nbox[0][1]=(ptp[0][1]*cosdeg+ptp[0][0]*sindeg);
    nbox[1][0]=(ptp[1][0]*cosdeg-ptp[1][1]*sindeg); nbox[1][1]=(ptp[1][1]*cosdeg+ptp[1][0]*sindeg);
    nbox[2][0]=(ptp[2][0]*cosdeg-ptp[2][1]*sindeg); nbox[2][1]=(ptp[2][1]*cosdeg+ptp[2][0]*sindeg);
    nbox[3][0]=(ptp[3][0]*cosdeg-ptp[3][1]*sindeg); nbox[3][1]=(ptp[3][1]*cosdeg+ptp[3][0]*sindeg);
    // Move the box to the correct location.
    for (fi1=0;fi1<4;++fi1) { nbox[fi1][0]+=(mpt[0]); nbox[fi1][1]+=(mpt[1]); }
    // This tests if the dimension line crosses the top of the box.
    (*FirstLVal)=ic_linexline(nbox[0],nbox[1],pt1,pt2,intpt[0]);
    // find where the line crosses the bottom of the box.
    if ((*FirstLVal)==3 || (*FirstLVal)==1 || (*FirstLVal)==2) {
        // find where the lines intersect.
        (*SecondLVal)=ic_linexline(nbox[2],nbox[3],pt1,pt2,intpt[1]);
    } else {
        // Tests where the line crosses the left side of the box.
        (*FirstLVal)=ic_linexline(nbox[0],nbox[2],pt1,pt2,intpt[0]);
        // Tests where the line crosses the right side of the box.
        if ((*FirstLVal)==3 || (*FirstLVal)==1 || (*FirstLVal)==2) (*SecondLVal)=ic_linexline(nbox[1],nbox[3],pt1,pt2,intpt[1]);
    }
    if (((*FirstLVal)==3 || (*FirstLVal)==1) && ((*SecondLVal)==3 || (*SecondLVal)==1)) return(FALSE); else return(TRUE);
}

// EBATECH(watanbe)-[
//////////////////////////////////////////////////////////////////////////////
// extended cmd_getboxintersec Function
//
BOOL
cmd_getboxintersecEx(
  sds_point     txtbox[],
  const double  angle,
  sds_point     mpt,
  sds_point     pt1,
  sds_point     pt2,
  sds_point     intpt[]
)
{
  // this is the point picked.
  sds_point ptp[4];
  sds_point nbox[4];
  int i;

  // Copy the bottom left corner to [0] and [2],
  // and top right corner to [1] and [2].
  ic_ptcpy(ptp[0], txtbox[0]);
  ic_ptcpy(ptp[1], txtbox[1]);
  ic_ptcpy(ptp[2], txtbox[0]);
  ic_ptcpy(ptp[3], txtbox[1]);
  ptp[0][1] = txtbox[1][1];
  ptp[3][1] = txtbox[0][1];

  // move to center
  for (i = 0; i < 4; ++i)
    {
      ptp[i][0] -= txtbox[1][0] / 2;
      ptp[i][1] -= txtbox[1][1] / 2;
      ptp[i][2] = 0;
    }

  double dCos = cos(angle);
  double dSin = sin(angle);
  for (i = 0; i < 4; ++i)
    {
      // rotate the box (angle) degrees.
      nbox[i][0] = ptp[i][0] * dCos - ptp[i][1] * dSin;
      nbox[i][1] = ptp[i][1] * dCos + ptp[i][0] * dSin;
      nbox[i][2] = 0;

      // Move the box to the correct location.
      ic_add(nbox[i], mpt, nbox[i]);
    }

  // This tests if the dimension line crosses the top of the box.
  short s1 = 0;
  short s2 = 0;

  // find where the line crosses the bottom of the box.
  s1 = ic_linexline(nbox[0], nbox[1], pt1, pt2, intpt[0]);
  if (s1 == 3)
    {
      // find where the lines intersect.
      s2 = ic_linexline(nbox[0], nbox[2], pt1, pt2, intpt[1]);
      if (s2 == 3)
        {
          return TRUE;
        }

      s2 = ic_linexline(nbox[1], nbox[3], pt1, pt2, intpt[1]);
      if (s2 == 3)
        {
          return TRUE;
        }

      s2 = ic_linexline(nbox[2], nbox[3], pt1, pt2, intpt[1]);
      if (s2 == 3)
        {
          return TRUE;
        }

      return FALSE;
    }

  // Tests where the line crosses the left side of the box.
  s1 = ic_linexline(nbox[0], nbox[2], pt1, pt2, intpt[0]);
  if (s1 == 3)
    {
      // find where the lines intersect.
      s2 = ic_linexline(nbox[1], nbox[3], pt1, pt2, intpt[1]);
      if (s2 == 3)
        {
          return TRUE;
        }

      s2 = ic_linexline(nbox[2], nbox[3], pt1, pt2, intpt[1]);
      if (s2 == 3)
        {
          return TRUE;
        }

      return FALSE;
    }

  // Tests where the line crosses the left side of the box.
  s1 = ic_linexline(nbox[1], nbox[3], pt1, pt2, intpt[0]);
  if (s1 == 3)
    {
      s2 = ic_linexline(nbox[2], nbox[3], pt1, pt2, intpt[1]);
      if (s2 == 3)
        {
          return TRUE;
        }

      return FALSE;
    }

  return FALSE;
}
// ]-EBATECH(watanbe)

//////////////////////////////////////////////////////////////////////////////
// cmd_getboxinsersecArc Function
//
// This function calculates the intersection points of an arc passing through
// a box. This is an example of the most complicated box cutting the arc in
// five seperate smaller arcs.
//
//               __________________ intpt[3]
//  nbox[0]__   |      ____________ intpt[4]
//           \  |____ |
//            \_/____\|_________________________ nbox[1]
// intpt[2] __|        |___________ intpt[5]
//           /|        |\
//          | |    .___|_|_________ Center of arc.
//          | |        | |
//           \|        |/__________ intpt[6]
// intpt[1] __|________|___________ intpt[7]
//           / /\/   \/|________________________ nbox[3]
//          / / /     \
// nbox[2]_/  |/       \
//            |
//            |
//            |____________________ intpt[0]
//
//
// txtbox[0] : Bottom left corner of the box.
// txtbox[1] : Top right corner of the box.
// anglec    : The angle of the box.
// mpt       : The point in the ucs the box will be located.
// center    : The center point of the arc.
// radius    : Distance from the center of the arc to the arc.
// sAngle    : The start angle of the arc.
// eAngle    : The end angle of the arc.
// intpt     : An array of points, these points are all the intersection points of the arc crossing the box.
// nbox      : An array of points, The corners of the box.
//
DWORD cmd_getboxinsersecArc(sds_point *txtbox,sds_real anglec,sds_point mpt,sds_point center,sds_real radius,sds_real sAngle,sds_real eAngle,sds_point *intpt,sds_point *nbox)
{
    sds_point ptp  [4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};   // this is the point picked.
    short     fi1 =0;
    BYTE     *bPtr = NULL;
    DWORD     retvalue = 0;

    bPtr=(BYTE *)&retvalue;
    memset(nbox,0,(sizeof(sds_point)*4));
    // Copy the bottom left corner to ptp[0] and ptp[2], and top right corner to ptp[1] and [2], Then Create the points of the box.
    ic_ptcpy(ptp[0],txtbox[0]); ic_ptcpy(ptp[1],txtbox[1]); ic_ptcpy(ptp[2],txtbox[0]); ic_ptcpy(ptp[3],txtbox[1]);
    ptp[0][1]=txtbox[1][1]; ptp[3][1]=txtbox[0][1];
    // rotate the box (anglec) degrees.
    nbox[0][0]=(ptp[0][0]*cos(anglec)-ptp[0][1]*sin(anglec)); nbox[0][1]=(ptp[0][1]*cos(anglec)+ptp[0][0]*sin(anglec));
    nbox[1][0]=(ptp[1][0]*cos(anglec)-ptp[1][1]*sin(anglec)); nbox[1][1]=(ptp[1][1]*cos(anglec)+ptp[1][0]*sin(anglec));
    nbox[2][0]=(ptp[2][0]*cos(anglec)-ptp[2][1]*sin(anglec)); nbox[2][1]=(ptp[2][1]*cos(anglec)+ptp[2][0]*sin(anglec));
    nbox[3][0]=(ptp[3][0]*cos(anglec)-ptp[3][1]*sin(anglec)); nbox[3][1]=(ptp[3][1]*cos(anglec)+ptp[3][0]*sin(anglec));
    // Move the box to the correct location.
    for (fi1=0;fi1<4;++fi1) { nbox[fi1][0]+=(mpt[0]); nbox[fi1][1]+=(mpt[1]); }
    //  -1 : No intersections.  (Line farther than rr from circle --
    //       or one or both entities are undefined (line seg has len
    //       0, or circle has 0 radius).)
    // >-1 : Bit-coded (bits counted from right):
    //       Bit 0 : i0 is on the line
    //       Bit 1 : i0 is on the arc
    //       Bit 2 : i1 is on the line
    //       Bit 3 : i1 is on the arc
    *bPtr=(BYTE)ic_linexarc(nbox[0],nbox[1],center,radius,sAngle,eAngle,intpt[0],intpt[1]); ++bPtr;
    *bPtr=(BYTE)ic_linexarc(nbox[1],nbox[3],center,radius,sAngle,eAngle,intpt[2],intpt[3]); ++bPtr;
    *bPtr=(BYTE)ic_linexarc(nbox[3],nbox[2],center,radius,sAngle,eAngle,intpt[4],intpt[5]); ++bPtr;
    *bPtr=(BYTE)ic_linexarc(nbox[2],nbox[0],center,radius,sAngle,eAngle,intpt[6],intpt[7]);

    return(retvalue);
}
