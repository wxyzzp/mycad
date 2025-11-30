// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/ 
#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern                        SDS_AtNodeDrag;

// =========================================================================================================================================
// =========================================================== Global Variables ============================================================
// =========================================================================================================================================

extern BOOL  tmp_depoints;
extern db_drawing*  tmp_flp;

// =========================================================================================================================================
// =========================================================== Global structure's ==========================================================
// =========================================================================================================================================

extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;

// =======================================================================================================================================
// ============================================================= Function's ==============================================================
// =======================================================================================================================================
//Bugzilla No. 77887 ; 03-06-2002
static sds_real getdimfontheight(db_drawing *flp);
static sds_real calculateDIMTFAC(sds_real dimfontheigth);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (BLOCK SAVE)
//
// This is a path that points are put through when a block is saved in the database.
//
//  NATIVE (to) UCS (to) BASEPOINT TRANSFORMATION (to) UCS (to) NEW NATIVE FORMAT.
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DIMENSION Point cordinate system. For all dimension types.
//
// 10   : WCS
// 11   : ECS
// 12   : ECS
// 13   : WCS
// 14   : WCS
// 15   : WCS
// 16   : ECS
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MODE = 0,1
// These are the manditory group codes that will be saved every time a Linear dimesion is created. using
// the commands (DIMLINEAR,DIMALIGNED,DIM HOR,DIM VER,DIM ALIGNED,DIM ROTATED). Example follows.
// --------------------------------------------------------------------------------------------------------
// (8 . "0")                           = Layer name.
// (10 213.357 164.982 0.0)            = Definition point. Note: This seems to be at the same level as the extension line but where the extension
//                                       line and dimension line intersect.
// (11 177.718 166.857 0.0)            = Middle point of dimension text.
// (12 0.0 0.0 0.0)                    = Insertion point for clones of a dimension. for now 0,0,0
// (70 . 32)                           = Dimension type. and other flags.
// (1 . "")                            = Dimension text explicitly entered by the user.
// (52 . 0.0)                          = Angle of dimension (Extension lines) for an obliqued dim.
// (53 . 0.0)                          = Rotation angle of dimension text.
// (54 . 0.0)                          = Always 0.0
// (51 . 0.0)                          = Always 0.0
// (210 0.0 0.0 1.0)                   = Always 0,0,1
// (3 . "STANDARD")                    = Dimension style name.
// (13 142.08 123.938 0.0)             = First pick point for extension line.
// (14 213.357124.178 0.0)             = Seconed pick point for extension line.
// (15 0.0 0.0 0.0)                    = Always 0.0
// (16 0.0 0.0 0.0)                    = Always 0.0
// (40 . 0.0)                          = Always 0.0
// (50 . 0.0)                          = Angle of dimension.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The following table contains the Meanings to the elements in the (cmd_dimeparlink) structure and how they relate
// to the group codes.
//
//  Structure    Group
//  definition   Codes                      Meaning
// -------------------------------------------------------------------------------------------
//   bnidx      |  2  | Index to the name of pseudo-Block that contains the dimension picture.
//   dsidx      |  3  | Index to the dimension style name.
//   defpt      | 10  | Definition point.
//   textmidpt  | 11  | Middle point of dimension text.
//   blkipt     | 12  | Insertion point for clones of a dimension (for Baseline and Continue).
//   gcodeflg   | 70  | Dimension type.
//   text       |  1  | Dimension text entered by the user.
//   pt13       | 13  | Definition point for linear and angular dimensions.
//   pt14       | 14  | Definition point for linear and angular dimensions.
//   pt15       | 15  | Definition point for diameter,radius,and angular dimensions.
//   pt16       | 16  | Point defining dimension arc for angular dimensions.
//   leadlen    | 40  | Leader length for radius and diameter dimensions.
//   dlnang     | 50  | Angle of rotated,horizontal,or vertical linear dimensions.
//   hdir       | 51  | Horizontal direction.
//   elnang     | 52  | Extension line angle for oblique linear dimensions.
//   textrot    | 53  | Rotation angle of dimension text.
// -------------------------------------------------------------------------------------------
//
// dragmode  1: The point is being dragged when the dimension is being first created.
// dragmode -1: The point is being called through a lisp or script.
// dragmode  0: The dimension is being entmaked.
// Since SDS_dragobject returns a WCS point I need to convert it to a UCS point, so when
// the point is being dragged (dragmode) is set to 1.
//
// This is (ANGULAR and ANGULAR_3P) comments:
// In this function (plink->DefValidflg) is being used by (ANGULAR or ANGULAR_3P). Angular dimensions have two
// drag poits, If angular 3 point first it drags the (10) point then the (11) point. If angular it drags the (16)
// point then the (11) point. so when the (11) point is being dragged (plink->DefValidflg) is set to TRUE.
//
int cmd_makedimension(db_handitem *elp,db_drawing *flp,short dragmode)
{
    resbuf			*rb=NULL,
                    rbecs,
                    rbwcs,
                    rbucs;
    short           type = 0;
    gr_view*		view = SDS_CURGRVW;
    sds_real        fr1 = 0.0;
	sds_point		pt1 = {0.0, 0.0, 0.0}, pt2 = {0.0, 0.0, 0.0};
	int				ret=RTNORM;

 	static int		recurs = 0;

    // SystemMetrix(Hiro)-[FIX:radial/diametric for UCS
    BOOL bGet = FALSE;
    // ]-SystemMetrix(Hiro) FIX:radial/diametric for UCS

    if (recurs) return(RTNORM);
	recurs=1;

	cmd_initialize();
    // Need to get the 210 group out of the elp because (cmd_initialrbcs) uses (plink->pt210).
    elp->get_210(plink->pt210);
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp,plink->pt210);

	// hold these values, restore at the end
	struct resbuf holducsx,holducsy;
	SDS_getvar(NULL,DB_QUCSXDIR,&holducsx,flp,NULL,NULL);
    SDS_getvar(NULL,DB_QUCSYDIR,&holducsy,flp,NULL,NULL);

// we retrieve the dimension rotation, calculate new UCS Xdir and Ydir
// based on that rotation, stuff them in, create the dimension, then
// restore the original UCS at the end.  MR 8/6/98.

	elp->get_51(&plink->hdir);  // get the rotation
	sds_point newxdir,newydir;
	struct resbuf rbpt;
	rbpt.restype=RT3DPOINT;

	ic_rotaxis(rbecs.resval.rpoint,-plink->hdir,newxdir,newydir);
	ic_ptcpy(rbpt.resval.rpoint,newxdir);
	SDS_setvar(NULL,DB_QUCSXDIR,&rbpt,flp,&SDS_CURCFG,&SDS_CURSES,0);
	ic_ptcpy(rbpt.resval.rpoint,newydir);
    SDS_setvar(NULL,DB_QUCSYDIR,&rbpt,flp,&SDS_CURCFG,&SDS_CURSES,0);

    if (!dragmode) {
        fr1=0.0;
    }

	// Note <= IC_ACAD14 have dimension parts placed on layer zero.
	if (flp->ret_FileVersion() > IC_ACAD14)
		elp->get_8(&plink->pLayerName);	

	plink->bname[0]=0;
	// Get the dimension type
    elp->get_70(&plink->flags);
    type=plink->flags;
    type &= ~DIM_TEXT_MOVED;
    type &= ~DIM_BIT_6;

    // (PARLINK->lweight). 2003/3/31
    if(!dragmode) elp->get_370(&(plink->lweight));
	// (PARLINK->TEXT).
	elp->get_1(plink->text,sizeof(plink->text)-1);
	// (PARLINK->DEFPT).
	elp->get_10(plink->defpt);
        // If the (10) point is being dragged. Then (SDS_DragObject) is putting in a (WCS) (10) point.
        // Make sure that the (10) point is being transformed to a (UCS) point.
        // This is only done on a drag on a create and a node drag and if the (10) point is the node being dragged and type is of right type.
	if (SDS_AtNodeDrag==3 || (!SDS_AtNodeDrag && dragmode==1 && !plink->DefValidflg) && (type==DIM_ALIGNED || type==DIM_ROT_HOR_VER || type==DIM_ANGULAR_3P)) {
        sds_trans(plink->defpt,&rbwcs,&rbucs,0,plink->defpt);
    }
    // We should only do a transformation once the dragging has started on node selection and the node being dragged is not (10).
    if ((plink->BeginDrag && SDS_AtNodeDrag!=3 && SDS_AtNodeDrag) || (!dragmode && !SDS_AtNodeDrag)) {
        sds_trans(plink->defpt,&rbwcs,&rbucs,0,plink->defpt);
    }
    // (PARLINK->TEXTMIDPT).
    elp->get_11(plink->textmidpt);
    // If the (11) point is being dragged. Then (SDS_DragObject) is putting in a (WCS) (10) point.
    // Make sure that the (11) point is being transformed to a (UCS) point.
    // EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Add case of creating DimDiameter or DimRadius in DIMTMOVE==2
    if (SDS_AtNodeDrag==2 || 
    	(plink->DefValidflg && dragmode==1) && 
    	( type==DIM_ANGULAR || type==DIM_ANGULAR_3P || type==DIM_DIAMETER || type == DIM_RADIUS ))
    //if (SDS_AtNodeDrag==2 || (plink->DefValidflg && dragmode==1) && (type==DIM_ANGULAR || type==DIM_ANGULAR_3P))
    // EBATECH(CNBR) ]-
    {
        sds_trans(plink->textmidpt,&rbwcs,&rbucs,0,plink->textmidpt);
    }
    // We should only do a transformation once the draging has started on node selection and the node being dragged is not (11).
    if ((plink->BeginDrag && SDS_AtNodeDrag!=2 && SDS_AtNodeDrag) || (!dragmode && !SDS_AtNodeDrag)) {
        sds_trans(plink->textmidpt,&rbecs,&rbucs,0,plink->textmidpt);
    }
    // (PARLINK->BLKIPT).
        // Don't need to do any transformation on this point becouse it is calcualted in the entmake of the dimension and is not used on a drag.
    elp->get_12(plink->blkipt);
    // (PARLINK->PT13).
    // EBATECH(watanabe)-[radius dimension
    //elp->get_13(plink->pt13);
    if ((dragmode == 0) && (type == DIM_RADIUS)) {
        // do nothing
    } else if ((dragmode == 0) && (type == DIM_DIAMETER)) {
        // do nothing
    } else {
        elp->get_13(plink->pt13);

        // SystemMetrix(Hiro)-[FIX:radial/diametric for UCS
        bGet = TRUE;
        // ]-SystemMetrix(Hiro) FIX:radial/diametric for UCS
    }
    // ]-EBATECH(watanabe)

    //  If the (13) point is being dragged. Then (SDS_DragObject) is putting in a (WCS) (13) point.
    // Make sure that the (13) point is being transformed to a (UCS) point.
    if (SDS_AtNodeDrag==4) {
        sds_trans(plink->pt13,&rbwcs,&rbucs,0,plink->pt13);
    }
    // We should only do a transformation once the draging has started on node selection and the node being dragged is not (13).
    if ((plink->BeginDrag && SDS_AtNodeDrag!=4 && SDS_AtNodeDrag) || (!dragmode && !SDS_AtNodeDrag)) {
        // SystemMetrix(Hiro)-[FIX:radial/diametric for UCS
        //sds_trans(plink->pt13,&rbwcs,&rbucs,0,plink->pt13);
        if (bGet)
          {
            sds_trans(plink->pt13,&rbwcs,&rbucs,0,plink->pt13);
          }
        // ]-SystemMetrix(Hiro) FIX:radial/diametric for UCS
    }
    // (PARLINK->PT14).
    // EBATECH(watanabe)-[radius dimension
    //elp->get_14(plink->pt14);
    if ((dragmode == 0) && (type == DIM_RADIUS)) {
        // do nothing
    } else if ((dragmode == 0) && (type == DIM_DIAMETER)) {
        // do nothing
    } else {
        elp->get_14(plink->pt14);
    }
    // ]-EBATECH(watanabe)

    // If the (14) point is being dragged. Then (SDS_DragObject) is putting in a (WCS) (10) point.
    // Make sure that the (14) point is being transformed to a (UCS) point.
    // EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Remove case of creating DimDiameter or DimRadius in DIMTMOVE==2.
    if(SDS_AtNodeDrag==5 ||
    	((type==DIM_x_ORDINATE || type==DIM_Y_ORDINATE ) && (!SDS_AtNodeDrag && dragmode==1)) ||
    	((type==DIM_DIAMETER || type==DIM_RADIUS) && (!plink->DefValidflg && !SDS_AtNodeDrag && dragmode==1)))
    //if (SDS_AtNodeDrag==5 || ((!SDS_AtNodeDrag && dragmode==1) && (type==DIM_x_ORDINATE || type==DIM_Y_ORDINATE || type==DIM_DIAMETER || type==DIM_RADIUS)))
    // EBATECH(CNBR) ]-
    {
        sds_trans(plink->pt14,&rbwcs,&rbucs,0,plink->pt14);
    }
    // If the (14) point is being dragged then don't transform the point.
    if ((plink->BeginDrag && SDS_AtNodeDrag!=5 && SDS_AtNodeDrag) || (!dragmode && !SDS_AtNodeDrag)) {
        // SystemMetrix(Hiro)-[FIX:radial/diametric for UCS
        //sds_trans(plink->pt14,&rbwcs,&rbucs,0,plink->pt14);
        if (bGet)
          {
            sds_trans(plink->pt14,&rbwcs,&rbucs,0,plink->pt14);
          }
        // ]-SystemMetrix(Hiro) FIX:radial/diametric for UCS
    }
    // (PARLINK->PT15).
    elp->get_15(plink->pt15);
    if (SDS_AtNodeDrag==6) {
        sds_trans(plink->pt15,&rbwcs,&rbucs,0,plink->pt15);
    }
    if ((plink->BeginDrag && SDS_AtNodeDrag!=6 && SDS_AtNodeDrag) || (!dragmode && !SDS_AtNodeDrag)) {
        sds_trans(plink->pt15,&rbwcs,&rbucs,0,plink->pt15);
    }
    // (PARLINK->PT16).
    elp->get_16(plink->pt16);
    // If the (16) point is being dragged when the dimension is first being created (SDS_DragObject) is putting in a (WCS) (16) point.
    // Make sure that the (16) point is being transformed to a (UCS) point.
    if (!SDS_AtNodeDrag && !plink->DefValidflg && dragmode==1 && type==DIM_ANGULAR) {
        sds_trans(plink->pt16,&rbwcs,&rbucs,0,plink->pt16);
    }
    if ((SDS_AtNodeDrag) || (!dragmode && !SDS_AtNodeDrag)) {
        sds_trans(plink->pt16,&rbecs,&rbucs,0,plink->pt16);
    }

	// (PARLINK->dimtxsty).
	elp->get_3(&plink->dimstyle);

    // ALL OTHERS.
    elp->get_40(&plink->leadlen);
    elp->get_50(&plink->dlnang);

	if(plink->hdir && type == DIM_ROT_HOR_VER)
		plink->dlnang = plink->dlnang + plink->hdir - 2 * IC_PI;

//    elp->get_51(&plink->hdir);
    elp->get_52(&plink->elnang);
    elp->get_53(&plink->textrot);
	plink->txtangflag = type != DIM_ROT_HOR_VER && fabs(plink->textrot) > IC_ZRO;	/*D.G.*/// Set the flag.

    cmd_setparlink(flp);
    cmd_geteedvalues(elp,flp);

    if ((flp!=tmp_flp || tmp_depoints)) {
        // search through the layer table for layer DEFPOINTS.
        rb=SDS_tblgetter("LAYER"/*DNT*/,DIM_DEFPOINTS,TRUE,flp);
        // If not rb then the layer DEPOINTS does not exit. so create one.
        if (!rb) {
			// DEFPOINTS layer set 290(Plotterble) is 0(avoid to plot) and 370(lineweight) is -3(Default).
            if ((rb=sds_buildlist(RTDXF0,"LAYER"/*DNT*/,2,DIM_DEFPOINTS,62,ic_colornum("white"),6,"CONTINUOUS"/*DNT*/,0))==NULL)
                goto exit;
            if (SDS_tblmesser(rb,IC_TBLMESSER_MAKE,flp)!=RTNORM) {
				sds_relrb(rb); rb=NULL;
				goto exit;
			}
        }
		sds_relrb(rb); rb=NULL;
        tmp_depoints=FALSE;
        tmp_flp=flp;
    }

    //Bugzilla No. 77887 ; 03-06-2002
	if (plinkvar[DIMTOL].dval.ival || plinkvar[DIMLIM].dval.ival || 
		plinkvar[DIMALT].dval.ival)
	{
		plink->dimfontheight = getdimfontheight(flp);
		plink->dimtfac = calculateDIMTFAC(plink->dimfontheight);
	}

    switch(type) {
        case DIM_DIAMETER:
        case DIM_RADIUS:
            if (!cmd_Diam_Radius(plink,flp,dragmode)) goto exit;
            goto ok;
            break;
        case DIM_ANGULAR_3P:
        case DIM_ANGULAR:
            if (!cmd_Angular_A3P(plink,flp,dragmode)) goto exit;
            goto ok;
            break;
        case DIM_x_ORDINATE:
        case DIM_Y_ORDINATE:
            if (!cmd_Ordinate(plink,flp,dragmode)) goto exit;
            goto ok;
            break;
        case DIM_ALIGNED:
        case DIM_ROT_HOR_VER:

            if (!cmd_defaultdim(plink,flp,dragmode)) goto exit;
            goto ok;
            break;
    }

    ok:
		if (plink->BeginDrag && dragmode)
		{
			 plink->BeginDrag=FALSE;
		}
		else if (!dragmode)
		{
			plink->BeginDrag=TRUE;
		}
		//BugZilla No. 78181; 10-06-2002
		if(dragmode==0 && type!=DIM_DIAMETER && type!=DIM_RADIUS)
		cmd_savelast(plink, view);	/*D.G.*/// We should update lastdimpts
									// (during calling from SDS_EntNodeDrag, for example)
									// TO DO: lastdimpts should be translated if we change a CS.
        elp->set_1(plink->text);
		elp->set_3(plink->dimstyle, flp);
        if (!dragmode) {
            sds_trans(plink->defpt,&rbucs,&rbwcs,0,plink->defpt);
        }
        elp->set_10(plink->defpt);
        if (!dragmode) {
            sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
        }
        elp->set_11(plink->textmidpt);
        if (!dragmode) {
            sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
        }
        elp->set_13(plink->pt13);
        if (!dragmode) {
            sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
        }
        elp->set_14(plink->pt14);
        if (!dragmode) {
            sds_trans(plink->pt15,&rbucs,&rbwcs,0,plink->pt15);
        }
        elp->set_15(plink->pt15);
        if (!dragmode) {
            sds_trans(plink->pt16,&rbucs,&rbecs,0,plink->pt16);
        }
        // Don't transform the (12) insertion point becouse it has already been calculated in the entmake part of the dimension.
        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
        elp->set_12(pt1/*plink->blkipt*/);

		if(plink->hdir && type == DIM_ROT_HOR_VER)
			plink->dlnang = plink->dlnang - plink->hdir + 2 * IC_PI;

        elp->set_16(plink->pt16);
        elp->set_40(plink->leadlen);
		//Bugzilla No. 78310 ; 07-10-2002
		elp->set_42(plink->dimvalue);
        elp->set_50(plink->dlnang);
        elp->set_51(plink->hdir);
        elp->set_52(plink->elnang);
        elp->set_53(plink->textrot);
        elp->set_70(plink->flags);
        elp->set_2(plink->bname,flp);

		goto cleanup;

    exit:
        if (plink->BeginDrag && dragmode) plink->BeginDrag=FALSE;
		else if (!dragmode) plink->BeginDrag=TRUE;
		ret=RTERROR;

	cleanup:
		recurs=0;
		IC_RELRB(rb);
		SDS_setvar(NULL,DB_QUCSXDIR,&holducsx,flp,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_setvar(NULL,DB_QUCSYDIR,&holducsy,flp,&SDS_CURCFG,&SDS_CURSES,0);
        return(ret);
}

//Bugzilla No. 77887 ; 03-06-2002
static sds_real getdimfontheight(db_drawing *flp)
{
	sds_real dimfontheigth=0.0;
	db_handitem   *handItem = NULL;
	handItem = flp->findtabrec(DB_DIMSTYLETAB,plink->dimstyle,1);
	if ( handItem != NULL )
	{
		db_handitem*  stylehip=NULL;
		handItem->get_340(&stylehip);
		if ( stylehip )
			stylehip->get_40(&dimfontheigth);
	}	
	return dimfontheigth;
}

//Bugzilla No. 77887 ; 03-06-2002
static sds_real calculateDIMTFAC(sds_real dimfontheigth)
{
	sds_real dimtfac = 1.0;	
	if (icadRealEqual(dimfontheigth,0.0))
		dimtfac = plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval * plinkvar[DIMTFAC].dval.rval;
	else
		dimtfac = dimfontheigth * plinkvar[DIMSCALE].dval.rval * plinkvar[DIMTFAC].dval.rval;
	return dimtfac ;
}
