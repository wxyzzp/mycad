/*
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Text generation functions
 *
 */

#include "gr.h"
#include "gr_view.h"
#include "textgen.h"
#include "DoBase.h"
#include "DoText.h"
#include "truetypeutils.h"
#include "styletabrec.h"
#include "pc2dispobj.h"
#include "dwgcodepage.h" // EBATECH(CNBR) 2001.12.5 \U in Bigfont String

extern gr_view gr_defview;

long gr_cstextcode(char **cpp);

GR_API short gr_fixtextjust(
    db_handitem     *enthip,
    db_drawing      *dp,
    gr_view *viewp,
    CDC             *dc,
    const int        mtextVert) {       // if mtext, need to know if shape specifies vertical--72
/*
**  Given a TEXT, ATTDEF, or ATTRIB entlink (enthip), and a dp
**  (for access to the TEXT STYLE table), this function corrects
**  various members of enthip for the justification indicated
**  by hjust and vjust.
**
**  Default justification:
**
**      Just sets the 11 pt to (0,0,elevation), where the elevation
**      comes from the 10 pt.
**
**  Aligned:
**
**      Makes sure the rotation agrees with the 10 and 11 pts.
**      Adjusts the text height so that the text fits between the
**      10 and 11 pts (without adjusting the width factor).
**
**  Fitted:
**
**      Makes sure the rotation agrees with the 10 and 11 pts.
**      Adjusts the text width factor so that the text fits between the
**      10 and 11 pts (without adjusting the text height).
**
**  All others:
**
**      Sets the 10 pt based on the 11 pt, the justification type,
**      and the textbox.
**
**  ° NOTE:  For now, vertical text only works with "normal"
**    generation (not backward or upside down) and 0.0 obliquing angle.
**
**  TCS refers to the Text Coordinate System.  Text always starts from
**  (0,0) (which aligns with the 10 pt) and is at rotation 0
**  and generates in the positive X-direction.
**  (Textbox coordinates are in the TCS.)
**
**  Our justifications are somewhat different than ACAD's.  We align
**  via textbox; ACAD is either using an oblique textbox or making
**  some other kinds of corrections.
**
**  Returns:
**       0 : OK
**      -1 : dp==NULL, enthip==NULL, or enthip doesn't belong
**             to a TEXT-like entity.
**      -2 : No memory (in gr_textgen() when getting the textbox).
*/
    short aligned,fitted,bward,udown,vert,nspaces[2],fi1,rc;
    int enttype,fint1;
    sds_real cs,sn,descend,afdx,afdy,afsep,boxwd,boxht,spacedist,ar1;
    sds_point box[2],tcs11,ap1;

    struct textbaglink {  /* Text data, regardless of which type of text-like entity. */
        char        *val;
        db_handitem *tship;
        sds_point    ipt;
        sds_point    apt;
        sds_real     ht;
        sds_real     xscl;
        sds_real     rot;
        sds_real     obl;
        char         gen;
        char         hjust;
        char         vjust;
    } textbag;


    rc=aligned=fitted=bward=udown=0;

    if (dp==NULL || enthip==NULL) { rc=-1; goto out; }
    enttype=enthip->ret_type();
    if (enttype!=DB_TEXT && enttype!=DB_ATTDEF && enttype!=DB_ATTRIB)
        { rc=-1; goto out; }


  /* FILL textbag WITH THE DATA FROM WHICHEVER TYPE OF TEXT-LIKE ENTITY: */

    memset(&textbag,0,sizeof(textbag));
    enthip->get_10( textbag.ipt);
    enthip->get_11( textbag.apt);
    enthip->get_40(&textbag.ht);
    enthip->get_41(&textbag.xscl);
    enthip->get_50(&textbag.rot);
    enthip->get_51(&textbag.obl);
    enthip->get_7( &textbag.tship);
    enthip->get_71(&fint1); textbag.gen=(char)fint1;
    enthip->get_72(&fint1); textbag.hjust=(char)fint1;
    if (enttype==DB_ATTDEF) enthip->get_2(&textbag.val);
    else                    enthip->get_1(&textbag.val);
    if (enttype==DB_TEXT) enthip->get_73(&fint1);
    else                  enthip->get_74(&fint1);
    textbag.vjust=(char)fint1;

  /* DO SOME PRELIMINARY CLEAN-UP AND CHECKS, AND SET SOME FLAGS: */

    if (textbag.val==NULL || !*textbag.val) goto out;  /* "" */

    if (icadRealEqual(textbag.ht,0.0)) textbag.ht=1.0;  /* Safety */

    if (!textbag.vjust) {
        if      (textbag.hjust==3) aligned=1;
        else if (textbag.hjust==5) fitted =1;
    }

    if (textbag.gen&2) bward=1;
    if (textbag.gen&4) udown=1;

    /*
    **  Get what we need from the TEXT STYLE record and font:
    **
    **    Determine if this text style is vertical.
    **
    **    Determine the "height" of a descender (the gap between
    **    "bottom" and "baseline").  (ACAD uses it for setting
    **    "bottom" justification even if the text has no descenders.)
    **
    **    Get the line-spacing and space-spacing factors.
    */
    vert = mtextVert;
    descend=textbag.ht/3.0;  /* TXT default -- in case font not found */
                            /* (which should never happen). */
    spacedist=0.0;
    if (textbag.tship!=NULL) {
        struct db_fontlink *tfp;

        textbag.tship->get_70(&fint1);
        if (fint1 & IC_SHAPEFILE_VERTICAL)
            vert = 1;

        // EBATECH(CNBR) -[ Bugzilla78574: AutoCAD uses bigfont above/below geometory.
        if ((tfp=textbag.tship->ret_bigfont())!=NULL) {
            if (tfp->above) descend=textbag.ht*tfp->below/tfp->above;
            spacedist = tfp->spfact;
        } else
        // ]- 
        if ((tfp=textbag.tship->ret_font())!=NULL) {
            if (tfp->above) descend=textbag.ht*tfp->below/tfp->above;
            spacedist = tfp->spfact;
        }
    }
    if (spacedist<0.01 || spacedist>100.0) spacedist=1.0;  /* TXT default */
    spacedist*=textbag.ht*textbag.xscl;


  /* FIX THE APPROPRIATE ITEMS IN textbag: */

    if (!textbag.hjust && !textbag.vjust) {  /* Default justification */

        textbag.apt[0]=textbag.apt[1]=0.0;
        textbag.apt[2]=textbag.ipt[2];  /* Take 11 elev from 10. */

    } else {  /* Not default justification */

        /*
        **  Get the textbox pts (and note that textbag.llp and textbag.rot
        **  aren't really used when just getting the textbox). For
        **  aligned and fitted, use 0.0 for the obliquing angle so that
        **  the "footprint" is fitted between the pts:
        */

        gr_textgenparslink par;

        // EBATECH(CNBR) -[ 2002/4/2 gr_textgenparslink has constructor.
        //par.ht=0.2; par.wfact=1.0;
        //memset(&par,0,sizeof(par));
        //par.ht=0.2; par.wfact=par.absxsclp=1.0;
        // EBATECH(CNBR) ]-

        GR_PTCPY(par.llpt,textbag.ipt);
        par.val     =textbag.val;
        par.ht      =textbag.ht;
        par.wfact   =textbag.xscl;
        par.rot     =textbag.rot;
        par.gen     =textbag.gen;
        par.obl     =0.0; // 2004/4/1 Center and Right require non oblique textbox.
        //par.obl     =(aligned || fitted) ? 0.0 : textbag.obl;
        par.absxsclp=1.0;
        par.vert = vert;
        if (textbag.tship!=NULL) {
//            textbag.tship->get_70(&par.vert); par.vert=((par.vert&0x04)!=0);
            par.font   =textbag.tship->ret_font();
            par.bigfont=textbag.tship->ret_bigfont();
        }

        if (gr_textgen((db_entity *)enthip, &par, 1, dp, viewp, dc, NULL, NULL, box[0], box[1]))
            { rc=-2; goto out; }

        /* If the text item has leading and/or trailing spaces, */
        /* push the box[][] X-walls: */
        nspaces[0]=nspaces[1]=0;
        fi1=0; while (isspace((unsigned char)textbag.val[fi1])) { nspaces[0]++; fi1++; }
        if (textbag.val[fi1]) {  /* Not all spaces */
            while (textbag.val[fi1]) fi1++;
            fi1--;
            while (fi1>-1 && isspace((unsigned char)textbag.val[fi1])) { nspaces[1]++; fi1--; }
        }
        box[0][0]-=nspaces[0]*spacedist;
        box[1][0]+=nspaces[1]*spacedist;

        boxwd=fabs(box[1][0]-box[0][0]);
        boxht=fabs(box[1][1]-box[0][1]);

        if (aligned || fitted) {
            /* Get the deltas and sep for the 2 pts: */
            afdx=textbag.apt[0]-textbag.ipt[0];
            afdy=textbag.apt[1]-textbag.ipt[1];
            if (icadRealEqual((afsep=sqrt(afdx*afdx+afdy*afdy)),0.0)) goto out;

            /* Make sure the rotation agrees with the 10 and 11 pts. */
            /* NOTE THAT FOR ALIGNED OR FITTED TEXT THAT IS BACKWARD, */
            /* ROTATION IS DETERMINED BY GOING FROM THE 11 TO THE 10 PT: */
            if (bward) textbag.rot=atan2(-afdy,-afdx);
            else       textbag.rot=atan2( afdy, afdx);

            if (vert) textbag.rot+=0.5*IC_PI;  /* Add 90 deg if vertical. */

            ic_normang(&textbag.rot,NULL);

            if (!icadRealEqual((ar1=(vert) ? boxht : boxwd),0.0) &&
                (ar1=afsep/ar1)>IC_ZRO && ar1<1.0/IC_ZRO) {

                if (aligned) textbag.ht   *=ar1;
                else         textbag.xscl*=ar1;
            }

            textbag.apt[2]=textbag.ipt[2];  /* Make 11 elev same as 10. */
        } else {  /* Cases where we set the 10 pt based on the 11 pt. */
            // Ebatech(cnbr) -[ 2003/6/13 Bugzilla78560 Let box[0] minus value in case of TrueType font
            /* Set the TCS 11 pt: */
            if (vert) {
                // Ebatech(cnbr) -[ 2003/7/5 Bugzilla78575 Vertical TEXT is not same as ACAD.
                if (textbag.hjust== IC_TEXT_JUST_RIGHT)
                    tcs11[1]=-boxht;     // RIGHT
                else if ( textbag.hjust== IC_TEXT_JUST_CENTER ||
                 ( textbag.hjust==IC_TEXT_JUST_MIDDLE && textbag.vjust == IC_TEXT_VALIGN_BASELINE ))
                    tcs11[1]=-0.5*boxht;    // CENTER and MIDDLE
                else
                    tcs11[1]=0.0; // LEFT, ALIGNED and FIT
                tcs11[0]= ( !bward ? 0.0 : boxwd );
                if (udown) tcs11[1]=-tcs11[1];  /* Upside down */
                // Ebatech(cnbr) ]-
            } else {
                if (textbag.hjust==IC_TEXT_JUST_MIDDLE && (textbag.vjust == IC_TEXT_VALIGN_BASELINE)) {  /* Middle */
                    tcs11[0]=0.5*boxwd;
                    tcs11[1]=0.5*boxht;
                } else {
                    if (textbag.hjust==IC_TEXT_JUST_LEFT)
                        tcs11[0]= ( !bward ? 0.0 : boxwd );
                    else if (textbag.hjust==IC_TEXT_JUST_CENTER)
                        tcs11[0]=0.5*boxwd;
                    else
                        tcs11[0]= ( !bward ? boxwd : 0.0 );

                    if (textbag.vjust==IC_TEXT_VALIGN_BASELINE)
                        tcs11[1]=0.0;
                    else if (textbag.vjust==IC_TEXT_VALIGN_BOTTOM)
                        tcs11[1]=-descend;
                    else if (textbag.vjust==IC_TEXT_VALIGN_MIDDLE)
                        tcs11[1]=0.5*textbag.ht;
                    else
                        tcs11[1]=textbag.ht;

                    if (udown) tcs11[1]=-tcs11[1];  /* Upside down */
                }
            }
            // Ebatech(cnbr) ]-

            /* In TCS coordinates, the 10 pt is at (0,0), */
            /* so get the TCS vector from the 11 pt to the 10 pt: */
            ap1[0]=-tcs11[0];
            ap1[1]=-tcs11[1];

            /* Apply the rotation: */
            cs=cos(textbag.rot); sn=sin(textbag.rot);
            ar1=ap1[0];
            ap1[0]=ap1[0]*cs-ap1[1]*sn;
            ap1[1]=ap1[1]*cs+ar1*sn;

            /* Add the vector to the ECS 11 pt to get the ECS 10 pt: */
            textbag.ipt[0]=textbag.apt[0]+ap1[0];
            textbag.ipt[1]=textbag.apt[1]+ap1[1];

            textbag.ipt[2]=textbag.apt[2];  /* Make 10 elev same as 11. */
        }

    }  /* End else not default justification. */


  /* PUT THE DATA BACK IN THE APPROPRIATE STRUCT (ONLY THE ITEMS */
  /* THAT MAY HAVE CHANGED): */

    enthip->set_10(textbag.ipt);
    enthip->set_11(textbag.apt);
    enthip->set_40(textbag.ht);
    enthip->set_41(textbag.xscl);
    enthip->set_50(textbag.rot);


out:
    return rc;
}

GR_API int gr_textbox(
    struct resbuf       *ent,
    sds_point            p1,
    sds_point            p2,
    db_drawing          *dp,
    gr_view      *viewp,
    CDC                 *dc) {
/*
**  Our version (with dp).
**
**  Returns RTNORM or RTERROR, but rc can be checked in "out" to
**  determine what went wrong.  Here are the rc codes:
**
**      0 : OK.
**     -1 : Bad pass parameters.
**     -2 : -1 or 0 group specifies entity type other than
**            TEXT, ATTDEF, or ATTRIB.
**     -3 : A db_getvar() failed.
**     -4 : No text value specified (TEXT or ATTRIB 1, or ATTDEF 2)
**     -5 : No STYLETABLE RECORD
**     -6 : gr_textgen() failed.
*/
    short gotht,gotwfact,gotobl,rc;
    int enttype;
    sds_real ar1;
    struct resbuf *trbp1,gvrb;
    db_handitem *enthip=NULL,*tship=NULL;
    gr_textgenparslink par;

    rc=0; gvrb.rbnext=NULL;
    //EBATEC(CNBR) -[ 2002/4/2 gr_textgenparslink now has constructor.
    par.ht=0.2; par.wfact=1.0;
    //memset(&par,0,sizeof(par)); par.ht=0.2; par.wfact=par.absxsclp=1.0;
    //EBATEC(CNBR) ]-

    if (ent==NULL || p1==NULL || p2==NULL || dp==NULL) { rc=-1; goto out; }

    gotht=gotwfact=gotobl=0; tship=NULL;
    enttype=DB_TEXT;
    p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=p2[2]=0.0;

    for (trbp1=ent; trbp1!=NULL; trbp1=trbp1->rbnext) {

        switch (trbp1->restype) {
            case -1:
                enthip=(db_handitem *)trbp1->resval.rlname[0];
                if (enthip==NULL) break;
                enttype=enthip->ret_type();
                if (enttype!=DB_TEXT && enttype!=DB_ATTDEF && enttype!=DB_ATTRIB && enttype!=DB_MTEXT)
                    { rc=-2; goto out; }

                if (enttype==DB_ATTDEF) enthip->get_2(&par.val);
                else                    enthip->get_1(&par.val);
                enthip->get_40(&par.ht);
                enthip->get_41(&par.wfact);
                enthip->get_51(&par.obl);
                enthip->get_7(&tship);
                enthip->get_71(&par.gen);
                gotht=gotwfact=gotobl=1;
                break;
            case  0:
                if  (strisame(trbp1->resval.rstring,"TEXT"))
                    enttype=DB_TEXT;
                else if ( strisame(trbp1->resval.rstring,"MTEXT"))
                    enttype=DB_MTEXT;
                else if (strisame(trbp1->resval.rstring,"ATTDEF"))
                    enttype=DB_ATTDEF;
                else if (strisame(trbp1->resval.rstring,"ATTRIB"))
                    enttype=DB_ATTRIB;
                else { rc=-2; goto out; }
                break;
            case  1:
                if (enttype!=DB_ATTDEF) par.val=trbp1->resval.rstring;
                break;
            case  2:
                if (enttype==DB_ATTDEF) par.val=trbp1->resval.rstring;
                break;
            case  7:
                if (trbp1->resval.rstring==NULL) break;
                tship=dp->findtabrec(DB_STYLETAB,trbp1->resval.rstring,1);
                // EBATECH(CNBR) -[
                //par.font = (db_fontlink*)tship;
                break;
            case 10:
                par.llpt[0]=trbp1->resval.rpoint[0];
                par.llpt[1]=trbp1->resval.rpoint[1];
                par.llpt[2]=trbp1->resval.rpoint[2];
                break;
            case 40:
                par.ht=trbp1->resval.rreal; gotht=1;
                break;
            case 41:
                par.wfact=trbp1->resval.rreal; gotwfact=1;
                break;
            case 50:
                //EBATECH(FUTA)fix -[
                //par.obl=trbp1->resval.rreal; gotobl=1;
                par.rot=trbp1->resval.rreal; // ]-
                break;
            case 51:
                //EBATECH(FUTA)fix -[
                //par.rot=trbp1->resval.rreal;
                par.obl=trbp1->resval.rreal; gotobl=1; // ]-
                break;
            case 71:
                par.gen=(unsigned char)trbp1->resval.rint;
                break;
        }

    }  /* End for each resbuf link */

    // DP: this function is not called for MTEXT
    assert(enttype != DB_MTEXT);
#if 0
    if(enttype == DB_MTEXT) {
        if (enthip != NULL) {
            enthip->get_extent(p1, p2);
            p2[SDS_X] = p2[SDS_X] - p1[SDS_X];
            p2[SDS_Y] = p2[SDS_Y] - p1[SDS_Y];
            p1[0] = p1[1] = 0.0;
            goto out;
        } else
            gotht=gotwfact=gotobl=0;
    }
#endif

    if (par.val==NULL) { rc=-4; goto out; }
    if (!*par.val) goto out;  /* p1 and p2 were zeroed earlier. */

    if (tship==NULL) {
        gvrb.resval.rstring=NULL;
        if (db_getvar(NULL,DB_QTEXTSTYLE,&gvrb,dp,NULL,NULL)!=RTNORM)
            { rc=-3; goto out; }
        if (gvrb.resval.rstring!=NULL) {
            tship=dp->findtabrec(DB_STYLETAB,gvrb.resval.rstring,1);
            IC_FREE(gvrb.resval.rstring); gvrb.resval.rstring=NULL;
        }
    }

    if (!gotht) {
        if (db_getvar(NULL,DB_QTEXTSIZE,&gvrb,dp,NULL,NULL)!=RTNORM)
            { rc=-3; goto out; }
        if (tship!=NULL) {
            tship->get_40(&ar1); par.ht=(!icadRealEqual(ar1,0.0)) ? ar1 : gvrb.resval.rreal;
        } else {
            par.ht=gvrb.resval.rreal;
        }
    }
    if (icadRealEqual(par.ht,0.0)) par.ht=0.2; else if (par.ht<0.0) par.ht=-par.ht;

    if (!gotwfact && tship!=NULL) tship->get_41(&par.wfact);
    if (icadRealEqual(par.wfact,0.0)) par.wfact=1.0;

    if (!gotobl && tship!=NULL) tship->get_51(&par.obl);

    if (tship!=NULL) {
        tship->get_70(&par.vert); par.vert=((par.vert&0x04)!=0);
        par.font   =tship->ret_font();
        par.bigfont=tship->ret_bigfont();
    }
    // EBATECH(CNBR) -[
    else{ rc=-5; goto out; }
    // EBATECH(CNBR) ]-

    if (gr_textgen((db_entity *)enthip, &par, 1, dp, viewp, dc, NULL, NULL, p1, p2)) { rc=-6; goto out; }

out:
    return (rc) ? RTERROR : RTNORM;
}


/*F*/
long gr_cstextcode(char **cpp, db_fontlink::FontType type) {
/*
**  This is an extension of gr_textgen().  It determines the chr
**  code (from %%, \U+, or \M+n control sequences) for the string *cpp
**  and steps *cpp TO the last char of the control sequence -- not 1 past --
**  (so that the caller can step past it as if it were a single char).
**
**  Non-control sequences are ok, too, but the caller can save time
**  by checking for a '%' or '\\' before calling this function.
**
**  Here are the current rules (based on testing ACAD); they aren't
**  perfect yet:
**
**      %% sequences:
**          Recognized following chars (case insensitive):
**              c : Diameter symbol             (0x2205)
**              d : Degree symbol               (0xB0)
**              o : Overscore toggle            (-2L)
**              p : Plus-or-minus symbol        (0xB1)
**              u : Underscore toggle           (-3L)
**            nnn : 1-3 decimal digits          (nnn)
**              ? : Anything else (including %) (That char)
**
**      \U+xxxx sequences (the 'U' is case insensitive):
**          All 4 chars of xxxx must be given (leading 0's, if necessary)
**          and must be valid hex digits.
**
**      \M+nxxxx (the 'M' is case insensitive):
**          Same as \U+xxxx (° n must be 1-4, but we are ignoring
**          its meaning for now).
**
**  Returns the UNICODE chr code or one of the following:
**       0L : Something specified chr 0 (%%0, \U+0000, \M+n0000),
**              or, somehow, this function got called with
**              *cpp pointing to the string-terminator.
**      -1L : Error (cpp==NULL)
**      -2L : Overscore  toggle
**      -3L : Underscore toggle
**
**      (The return value is a long so that negative codes can be used
**      for special purposes without conflicting with valid
**      unsign short UNICODE values.)
*/
    unsigned char *cp,*hexp,fuc1;
    short fi1;
    long rc;


    rc=0L;

    if (cpp==NULL) { rc=-1L; goto out; }

    cp=(unsigned char *)(*cpp);  /* For convenience (no neg vals) */

    rc=*cp;  /* Default */

    if (*cp=='%') {
        if (*(cp+1)=='%' && *(cp+2)) {
            cp+=2;
            switch (toupper(*cp)) {
                case 'C':
                    if (type == db_fontlink::SHAPE_1_0)
                        rc = 129;
                    else if (type == db_fontlink::SHAPE_1_1)
                        rc = 258;
                    else
                        rc=0x2205;
                    break;

                case 'D':
                    if (type == db_fontlink::SHAPE_1_0)
                        rc = 127;
                    else if (type == db_fontlink::SHAPE_1_1)
                        rc = 256;
                    else
                        rc=0x00B0;
                    break;

                case 'O': rc=-2L;    break;

                case 'P':
                    if (type == db_fontlink::SHAPE_1_0)
                        rc = 128;
                    else if (type == db_fontlink::SHAPE_1_1)
                        rc = 257;
                    else
                        rc=0x00B1;
                    break;

                case 'U': rc=-3L;    break;

                default :  /* Covers %%%, too. */
                    /* Look for up to 3 decimal digits (nnn): */
                    for (fi1=0; fi1<3 && cp[fi1] && isdigit((unsigned char) cp[fi1]); fi1++);
                    if (fi1) { rc=atoi((char *)cp); cp+=fi1-1; } else rc=*cp;
                    break;
            }
        }
    } else if (*cp=='\\') {
        hexp=NULL;
        fi1=(toupper(*(cp+1)));
        fi1=(fi1=='U')+2*(fi1=='M');
        if (fi1 && *(cp+2)=='+') {
            if (fi1==1) {
                //Special case for handling ACAD dimensions.  Dimensions in acad are stored as unicode
                //since we don't ship with a unicode font we need to special case these situations.
                char *begNum = (char*) (cp+3);

                //Special symbols in SHAPE_1_0 files are stored at pos 127 - 129; in SHAPE_1_1 files they are
                //stored at 256 - 258 therefore I add 129 to the SHAPE_1_0 position to get the SHAPE_1_1 position.
                int addpos = 0;
                if (type == db_fontlink::SHAPE_1_1)
                    addpos += 129;

                if ((type == db_fontlink::SHAPE_1_0) || (type == db_fontlink::SHAPE_1_1)) {
                    if (strnisame(begNum, "2205", 4))
                        rc = 129 + addpos;

                    else if (strnisame(begNum, "00B0", 4))
                        rc = 127 + addpos;

                    else if (strnisame(begNum, "00B1", 4))
                        rc = 128 + addpos;

                    else
                        hexp=cp+3;
                }

                else
                    hexp=cp+3;

                if (!hexp)
                    cp += 6;
            }
            //EBATECH(CNBR) -[ 2001/12/5 National Code is 0 to 5
            //else if (*(cp+3)>'0' && *(cp+3)<'5') hexp=cp+4;
            else if ('0' <= *(cp+3) && *(cp+3)<= '5' ) hexp=cp+4;
            //EBATECH(CNBR) ]-
        }
        if (hexp!=NULL) {
            for (fi1=0; fi1<4 && isxdigit((unsigned char) hexp[fi1]); fi1++);
            if (fi1>=4) {
                fuc1=hexp[4]; hexp[4]=0;
                rc=strtol((char *)hexp,NULL,16);
                hexp[4]=fuc1;
                cp=hexp+3;
            }
        }
    }

out:
    if (cpp!=NULL) *cpp=(char *)cp;
    return rc;
}
// EBATECH(CNBR) -[ 2001/12/5 \U in MBCS
/*F*/
long gr_cstextmbcode( short unicode, int dwgcodepage )
{
    unsigned char mbcsstr[2];
    long ret;
    // Unicode to MultiByte code
    if( WideCharToMultiByte(dwgcodepage, WC_COMPOSITECHECK|WC_DISCARDNS,
        (const unsigned short*)&unicode, 1, (char*)mbcsstr, 2, NULL, NULL  ) == 0 ){
        return -1L;
    }
    ret = (long)( 0x0000ffffL & (((unsigned long)mbcsstr[0] << 8) | (unsigned long)mbcsstr[1] ));
    return ret;
}
/*F*/
long gr_cstextunicode( short mbcode, int nation, int dwgcodepage )
{
    char mbcsstr[2];
    int code_page;
    wchar_t widechar;

    switch(nation){
    case '0':   code_page = dwgcodepage; break; // Current
    case '1':   code_page = 932;    break;      // Japanese
    case '2':   code_page = 950;    break;      // Traditional Chinese
    case '3':   code_page = 949;    break;      // Wansung (Korean)
    case '4':   code_page = 1361;   break;      // Johab(Korean)
    case '5':   code_page = 936;    break;      // Simplified Chinese
    default:
        return -1;
    }
    // Unicode to MultiByte code
    mbcsstr[0] = (char)(mbcode >> 8);
    mbcsstr[1] = (char)mbcode;
    if( MultiByteToWideChar(code_page, MB_PRECOMPOSED,
        mbcsstr, 2, &widechar, 1 ) == 0 ){
        return -1L;
    }
    return (long)(widechar & 0x0000ffffL);
}
// EBATECH(CNBR) ]-
/*F*/
short gr_getfontchr(
    db_drawing*           pDrw,
    char                  **valpp,
    struct db_fontlink     *font,
    struct db_fontlink     *bigfont,
    struct db_fontchrlink **fontchrpp,
    struct db_fontlink    **whichfontpp) {
/*
**  This is an extension of gr_textgen().  It locates the correct chr
**  definition to use for *valpp based on the bigfont/font priority
**  rules and the escape/control sequence priority rules.  (See
**  learned.doc.)
**
**  In greater detail, it points *fontchrpp to the correct element of
**  font->chr[] or bigfont->chr[] for the char or char sequence *valpp
**  points to -- or NULL, if neither has the appropriate chr.  If the chr
**  is found, *whichfontpp is set to the font in which it was found
**  (font or bigfont) (and set to NULL, if not found).  *valpp is then
**  pointed AT the last char of the sequence -- not one past -- (so that
**  the caller can step past it as if it were a single char). (That is,
**  if *valpp points to a two-char bigfont escape sequence or a control
**  sequence like "%%d" or "\\U+XXXX", *valpp is left pointing at the
**  last character of that sequence.)
**
**  Returns:
**       0 : Ok
**      -2 :  Overscore toggle (similar to gr_cstextcode())
**      -3 : Underscore toggle (similar to gr_cstextcode())
*/
    char *valp,*fcp1;
    short bfescseq,rc,fi1,fi2;
    short code;
    long fl1;
    struct db_fontlink *whichfontp;
    struct db_fontchrlink *fontchrp;
    int dwgcodepage;
    short code2=0;  //  EBATECH(CNBR) 2002/02/01 Duplicate Unicode and MBCS code

    rc=code=bfescseq=0; fontchrp=NULL; whichfontp=NULL; valp=NULL;


    /* Use font and bigfont as "ok-to-use" flags: */
    if (   font!=NULL &&    font->chr==NULL)    font=NULL;
    if (bigfont!=NULL && bigfont->chr==NULL) bigfont=NULL;

    if (fontchrpp==NULL || valpp==NULL || *valpp==NULL ||
        (font==NULL && bigfont==NULL)) goto out;

    valp=*valpp;  /* Covenience. */

    /* Unicode control sequences have priority (even over bigfont */
    /* escape sequences): */
    fi1=toupper(valp[1]);
    if (*valp=='\\' && (fi1=='U' || fi1=='M') && valp[2]=='+') {
        /* Possible Unicode; try it: */
        fcp1=valp;
        if ((fl1=gr_cstextcode(&valp, font->format))<1L) goto out;
        if (fcp1!=valp) {  /* Ptr advanced; valid Unicode found. */
            code=(short)fl1;
            // EBATECH(CNBR) -[ 2001/12/5 \U+XXXX in MBCS String
            //if (fi1!='M')
            //    bigfont=NULL;  /* Don't search bigfont for Unicode. */
            dwgcodepage = get_dwgcodepage( pDrw );
            if (fi1 =='U' && bigfont != NULL )
            {
                if(( fl1 = gr_cstextmbcode( code, dwgcodepage )) < 0L )
                    bigfont=NULL; // use Unicode
                else{
                    code2=code; // save unicode if the code cannot find in Bigfont EBATECH(CNBR) 2002/02/01
                    code=(short)fl1; // use Bigfont first
                }
            }
            if (fi1 =='M' && bigfont == NULL )
            {
                if(( fl1 = gr_cstextunicode( code, (int)fcp1[3], dwgcodepage )) < 0L )
                {
                    valp = fcp1;    // use encoded \M+NXXXX
                    code = 0;
                }
                else
                    code=(short)fl1;    // use Unicode
            }
            // EBATECH(CNBR) ]-
        }
    }

    if (!code && bigfont!=NULL) {  /* Try for bigfont escape sequence. */
        /* See if *valp is an esc char: */
        fi1=bigfont->nesc;
        if (bigfont->esc!=NULL)
            for (fi1=fi2=0; fi1<bigfont->nesc &&
                ((unsigned char)(*valp)<
                    (unsigned char)(bigfont->esc[fi2]) ||
                 (unsigned char)(*valp)>
                 (unsigned char)(bigfont->esc[fi2+1])); fi1++,fi2+=2);
        if (fi1<bigfont->nesc && valp[1]) {
            /* It is an esc char and there IS a char after it. */
            /* Get val of esc seq and search bigfont for it: */
            code=(((short)((unsigned char)(*valp)))<<8)|
                 ((short)((unsigned char)valp[1]));
            valp++;  /* Step to the char after the esc char */
            bfescseq=1;
        }
    }

    if (!code && *valp=='%' && valp[1]=='%' && valp[2]) {  /* %% control seq */
        /* Get the %%-control seq and search the normal font. */
        fl1=gr_cstextcode(&valp, font->format);
        if (fl1<1L) { rc=(short)fl1; goto out; }  /* Over/underscore */
        code= (short)fl1;
        bigfont=NULL;  /* Don't search bigfont for %% control seqs. */
    }

    if (!code) code=(unsigned char)(*valp);  /* Now we know it's not 0. */

    /* Check bigfont first: */
    if (bigfont!=NULL) {
        if (code>-1 && code<256) fi1=code;
        else for (fi1=256; fi1<bigfont->nchrs &&
            bigfont->chr[fi1].code!=code; fi1++);
        if (fi1>-1 && fi1<bigfont->nchrs && bigfont->chr[fi1].def!=NULL) {
            fontchrp=bigfont->chr+fi1;
            whichfontp=bigfont;
        } else if (bfescseq) code=(unsigned char)(*valp);
    }


    //added by Vitaly Spirin[
    /*DG - 11.6.2002*/// ...then commented out by Denis Petrov (the reason - "problems with diameter symbol"),
    // but since this commenting causes much more problems (when dwgcodepag != syscodepage, bug 78145)
    // and I haven't found problems with diameter I uncommented it...
    if (font->format == db_fontlink::UNIFONT && valp == *valpp )
    {
        char    ch;
        UINT    dwg_code_page;

        ch = code;
        dwg_code_page = get_dwgcodepage(pDrw);
        MultiByteToWideChar(dwg_code_page, MB_PRECOMPOSED,  &ch, 1, (wchar_t*)&code,
            sizeof(code));
    }
    //]

    /* Check normal font: */
    if (fontchrp==NULL && font!=NULL) {
        if(code2!=0) code=code2; // EBATECH(CNBR) 2002/02/01
        if (code>-1 && code<256) fi1=code;
        else for (fi1=256; fi1<font->nchrs &&
            font->chr[fi1].code!=code; fi1++);
        if (fi1>-1 && fi1<font->nchrs && font->chr[fi1].def!=NULL) {
            fontchrp=font->chr+fi1;
            whichfontp=font;
        }
    }


out:
    if (valpp!=NULL && valp!=NULL) *valpp=valp;
    if (fontchrpp!=NULL) *fontchrpp=fontchrp;
    if (whichfontpp!=NULL) *whichfontpp=whichfontp;
    return rc;
}

bool
TrueTypeTextForGDI(
        db_handitem *text,
        gr_view *view,
        bool *haveTrueTypeFont)     // WARNING!!!!!!!  May not be set by this function if function returns false!
    {
    ASSERT(text && view);

    if ((NULL == text) || (NULL == view))
        return false;

// what about thickness and width as determining factors?

    bool    haveTTF = false,
            canDrawText = false;

    int entType = text->ret_type();
    switch (entType)
        {
        case DB_TEXT:
        case DB_ATTDEF:
        case DB_ATTRIB:
        case DB_MTEXT:
            // most of the information we need for determing whether GDI can
            // draw TrueType text is contained in the text style
            db_styletabrec *textStyle = NULL;
            text->get_7((db_handitem **) &textStyle);   // acc. to AutoCAD docs, this can be NULL; but IntelliCAD
            ASSERT(textStyle);                          // seems to assume that it's always there

            haveTTF = textStyle->hasTrueTypeFont();
            if (haveTrueTypeFont)
                *haveTrueTypeFont = haveTTF;

            if (NULL == textStyle)                      // this did occur for Tolerances
                break;

#ifndef NEW_TTF_OUTPUT_METHOD

            if (view->isCoplanarToScreen((db_entity_with_extrusion *) text) &&  // must be coplanar to screen
                haveTTF &&                              // must use TrueType font
              !textStyle->isObliqued() &&               // can't be obliqued
              !textStyle->isVertical() &&               // can't be vertical
              !textStyle->isBackwards() &&              // can't be backwards
              !textStyle->isUpsideDown())               // can't be upside down
                canDrawText = true;

            // with regular Text and Attdefs and Attribs, whether the characters are drawn
            // vertically is indicated in the text style; with Mtext, vertical is indicated
            // both in the text style and by the text itself
            if (DB_MTEXT == entType)
                {
                int drawingDir;
                text->get_72(&drawingDir);
                if (IC_MTEXT_DRAWDIR_TTOB == drawingDir)
                    canDrawText = false;
                }
            break;

#endif

        }  // end switch (entType)

#ifndef NEW_TTF_OUTPUT_METHOD
    return canDrawText;
#else
    return haveTTF;
#endif

    }


/*F*/
short
gr_textgen
(
 db_entity*             pTextEntity,    // =>
 gr_textgenparslink*    pTextInfo,      // =>
 short                  boxonly,        // =>
 db_drawing*            pDrw,           // =>
 gr_view*               pGrView,        // =>
 CDC*                   pDC,            // =>
 gr_displayobject**     ppBegDispObj,   // <=
 gr_displayobject**     ppEndDispObj,   // <=
 sds_point              MinPoint,       // <=
 sds_point              MaxPoint        // <=
)
{
/*
**  An extension of gr_getdispobjs() and gr_textbox().
**
**  Uses pGrView's "gr_conv" items, but only ->pc and related members and
**  ->pGrView. (We're careful to use pGrView only when boxonly==0 -- that is,
**  when gr_getdispobjs() has been called to actually generate text.
**  The members set by gr_getdispobjs() should be avoided during calls
**  where boxonly!=0 because gr_getdispobjs() has not been called.)
**
**  Allocates a display objects llist for the TEXT currently being created
**  via gr_gensolid(). *ppBegDispObj is pointed to the first link;
**  *ppEndDispObj is pointed to the last link.
**
**  TCS refers to the Text Coordinate System.  Text always starts from
**  (0,0) and is at rotation 0 and generates in the positive X-direction.
**  Scaling and obliquing are applied in the TCS.  (Textbox coordinates
**  are in the TCS.)
**
**  The text parameters are passed (instead of coming from pGrView->ment)
**  to make it easier to use this function for ATTDEF, ATTDEF, and
**  gr_textbox().
**
**  pTextInfo->absxsclp is the absolute value of the cumulative X-scale factor
**  (for nested INSERTs).  gr_getdispobjs() passes pGrView->absxsclp,
**  but gr_textbox() (and any other functions that can't count on
**  gr_getdispobjs() setting it) pass 1.0.
**
**  If boxonly!=0, MinPoint and MaxPoint are set -- but the display objects
**  are not generated.  HOWEVER -- the pc chain IS generated.
**  The box is determined by examining the finished pc chains,
**  just before the calls to gr_gensolid() -- not from curpt as we go.
**  The curpt method didn't work: a pendown followed by a penup
**  with no drawing generated no line work, but tricked the textbox
**  adjuster into taking the pendown point.
**
**  MinPoint and MaxPoint are for the "textbox" corners; they are set to the
**  lower left and upper right corners of the enclosing text box
**  in the TCS (if they are non-NULL).  AutoLISP textbox returns 3D points
**  (and ADS uses sds_points), but it seems the Z's are not used
**  and are always set to 0.0 -- so, we're following suit.  (The alternative
**  would be to use the thickness (scaled properly).)
**
**  Once we have a pt in the TCS it can be transformed to the ECS
**  via the TCS axes defined in the ECS.  The TCS axes are defined
**  in ECS coordinates such that they have the proper rotation and
**  account for the "upside down" and "backward" flags.
**
**  For arcs, we trick gr_arc2pc() into adding arc segments
**  (in font vector units, scaled only by the vector factor vfact) to
**  the end of the pc chain.  Then we walk them and scale them (using
**  fact[0] and fact[1]) which applies the width factor, and use the
**  results to adjust the textbox. Then we convert them to the NCS.
**
**  SUBSHAPES:
**
**    I can think of 2 ways to do this: recursion or tricking the def
**    byte loop into working on another chr's def bytes for a while.  I
**    don't like either, but I think the latter may be simpler.  For
**    recursion, I'd have to break out the "each chr" part of this
**    function into its own function, pass a lot of this function's
**    locals/params to it, figure out which variables need to survive
**    each call and which don't, chain together the display objects
**    generated by each recursion, etc.  Perhaps that wouldn't have been
**    as big a deal if I had written this function with all that in mind
**    to begin with.
**
**    As it is, I think it's probably easier to go with a "stack" method
**    that makes it look like we're splicing in some extra def bytes
**    when they're needed.  First, start with the original chr's
**    font/chr data in the only link of a llist stack.  When a subshape
**    command comes along, create a new top link for that subshape and
**    work on it until it's done -- then chuck that link and continue
**    working on the one beneath it.  The key is to preserve all of the
**    font/chr-specific data on the stack; we can see those variables in
**    the code between the start of the each-chr loop and the start of
**    the each-def-byte loop (because that's where we're setting up for
**    a specific chr's def bytes).
**
**    This seems to work in simple cases, but we still have a problem
**    with NESTED subshapes.  Try this:
**
**      In a copy of isocp.shp, comment out the 'B' and replace
**      it with the following (a 'B' which subshapes 'C'):
**
**        *042,28,B
**        2,8,(6,2),1,0A0,10,(10,100),0A8,2,0A0,1,10,(8,100),7,043,0A8,
**        8,(0,-36),2,8,(26,-2),0
**
**      In a test bigfont (with 7E in the escape chr range), add this
**      chr (which is isocp's 'A' with a subshape-B call in it):
**
**        *07eff,30,A
**        2,8,(6,2),1,8,(12,36),7,042,8,(12,-36),3,2,2,8,(-7,20),1,
**        8,(-34,0),2,8,(53,-24),4,2,0
**
**        (The header was
**          *BIGFONT 46,1,126,126
**          *0,4,SPECIALS
**          21,7,2,0
**        )
**
**      Now make a TEXT with value |7E|FF| ("\\U+007E\\U+00FF").
**      It's an 'A' that subshapes a 'B' that subhapes 'C', but some of
**      our line work doesn't match ACAD's.
**
**  Minimal error-checking is done here; that's up to gr_getdispobjs().
**
**  Interesting ACAD bug:  While testing our 9-command processing, I
**  found a case where OURS works and ACAD's doesn't. I doctored '&'
**  and '0' in complex.shp by adding a 14 (do next command only if
**  vertical) in front of a 9 (multiple vectors) in each chr.  ACAD's
**  generator skips all the 9-vectors -- PLUS THE COMMAND THAT FOLLOWS
**  IT.  ACAD has the same problem with the other repeater: the 13-command.
**
**  I also noticed that complex.shp's '0' is missing a vector
**  in its definition.
**
**  Returns: gr_getdispobjs() codes.
*/
// ----------------------------------------------------------
//  EbaTech(Japan). modifies to display extended subshapes.
//  EXTENDED SUBSHAPES:
// ----------------------------------------------------------
    char*           valp;
    unsigned char   vlen, vdir;
    short           skip, vertonly, gotdxdy, arcmode, savltsclwblk, rc;
    short           pendown, repmode, done, psidx, pssz, cw, fi1;
    short           circ, genpc, cmdcode, scoretog;
    long            pcidx, fl1;
    // EBATECH(CNBR) -[ Extended Subshapes:
    //sds_real      ps[4][2], tcsaxis[2][2], vfact, dx, dy, TanObl;
    sds_real        ps[4][2], tcsaxis[2][2], vfactx, vfacty, savex, savey, dx, dy, TanObl;
    struct db_fontlink *wf;      /* whichfont */
    struct db_fontchrlink *tfc;  /* thisfontchr */
    // ]- EBATECH(CNBR)
    sds_real        rad, sa, ea, iang, bulge, defabove, oldabove, wabsxsclp, ar1;
    sds_point       box[2];
    sds_point       curpt, endpt, ap1;
    gr_displayobject *begdo, *enddo, *tdop1, *tdop2;
    db_fontchrlink  deffontchr;
    gr_pclink*      tpcp1;
    bool            bGenerateBox, bGotBox, bGotCenter = false;
    int             lastspace;  // EBATECH(CNBR) for MTEXT additional space
	sds_real		/* xBoxCenter, yBoxCenter, xShift = 0.0, yShift = 0.0, boxHeight, scale = 1.0, */ tolSymbolHeight = 0.0;
    
    // boxonly should always be 1 if MinPoint and MaxPoint aren't NULL
    // or always 0 if *ppBegDispObj and *ppEndDispObj aren't NULL; and
    // so far, we never get both display objects and points
    // at the same time
    ASSERT((MinPoint && MaxPoint && boxonly) || (ppBegDispObj && ppEndDispObj && !boxonly));

    ASSERT((MinPoint && MaxPoint && !ppBegDispObj && !ppEndDispObj) ||
           (!MinPoint && !MaxPoint && ppBegDispObj && ppEndDispObj));


    //
    // **************************** DATA FOR NON-TTF TEXT *******************************
    //

    /* Default chr definition (for deffontchr): */
    char    defdef[] = {2, 14, 3, 2, 14, 8, -3, -12, 14, 4, 2, 0x54, 1, 0x12, 0x10, 0x1E, 0x1C, 0x1A, 0x1C,
                        2, 0x1C, 1, 0x1C, 2, 0x30, 14, 3, 2, 14, 8, -7, -6, 14, 4, 2, 0};

    /* Subshape data: */
    struct subshapelink
    {
        char    extended; /* Flag: Extended subshape spec (extended bigfont). */
        short   code;     /* Code of chr to draw as a subshape. */
        char    basex;    /* Basepoint X (extended only). */
        char    basey;    /* Basepoint Y (extended only). */
        char    wd;       /* Width       (extended only). */
        char    ht;       /* Height      (extended only). */
    } subsh;

    /*
    **  The Current Chr Stack (for subshape processing).
    **
    **   forcependown : Used to force a pendown command under certain
    **                    circumstances (beginning of a chr or subshape,
    **                    after a pop command).
    **      whichfont : The font we're using (pTextInfo->font or pTextInfo->bigfont).
    **    thisfontchr : The chr we're working on (whichfont->chr or
    **                    &deffontchr).
    **           didx : Index into thisfontchr->def[].
    **          above : Height of a chr in font vector units
    **                    (whichfont->above or defabove).
    **        fact[2] : The factors to convert dx and dy from font vector
    **                    units to TCS units.
    */
    struct currchrlink
    {
        char            forcependown;
        db_fontlink*    whichfont;
        db_fontchrlink* thisfontchr;
        short           didx;
        sds_real        above;
        sds_real        fact[2];

        currchrlink*    next;
    } *ccs, *tccp1;

    /*
    **  Llist of underscore and overscore toggling for this TEXT entity:
    */
    struct scorelink
    {
        char        over;   /* Score being toggled: 0=Under; 1=Over.            */
        sds_real    tcsx;   /* TCS X-coord (on baseline (without obliquing))    */
                            /*   when toggled.  (Avoid Font Vector System,      */
                            /*   which can change with each chr.)               */

        scorelink*  next;
    } *scorell, *pscore, *tsp1;


    /***** Things to do before the first goto out: *****/

    ccs = NULL;
    scorell = pscore = NULL;

    rc = 0;
    bGotBox = false;
    begdo = enddo = NULL;

    if(MinPoint)
        MinPoint[0] = MinPoint[1] = MinPoint[2] = 0.0;
    if(MaxPoint)
        MaxPoint[0] = MaxPoint[1] = MaxPoint[2] = 0.0;

    if(!pGrView || icadRealEqual(pGrView->viewsize,0.0))
        pGrView = &gr_defview;

    /* Trick gr_gensolid() into doing the gr_ncs2ucs() calls    */
    /* so that we can work with a font vector system pc chain   */
    /* (and later a NCS pc chain) in this function:             */
    savltsclwblk = pGrView->ltsclwblk;
    pGrView->ltsclwblk = 1;

    /***** End things to do before the first goto out: *****/
    db_handitem* pTextStyle = NULL;

    if(!pTextInfo->val || !*pTextInfo->val)
        goto out;

    if(pTextInfo->val[0] == '%' && pTextInfo->val[1] == '%' && !pTextInfo->val[2])
        goto out;

    // Modified Cybage VSB 30/07/2001 DD/MM/YY [
    // Reason: Crash on lisp function 'textbox'
    // (textbox (cdr (entget (entlast))))
    // Bug No : 77765 from BugZilla
    if ( pTextEntity != NULL )
    {
        pTextEntity->get_7(&pTextStyle);
        if(pTextStyle == NULL &&
            (pTextEntity->ret_type() == DB_TEXT ||
            pTextEntity->ret_type() == DB_ATTRIB ||
            pTextEntity->ret_type() == DB_ATTDEF ||
            pTextEntity->ret_type() == DB_MTEXT))
            goto out;

		if (pTextEntity->ret_type() == DB_TOLERANCE) {
			resbuf rb;
            if (db_getvar(NULL,DB_QDIMTFAC,&rb, pDrw, NULL,NULL) == RTNORM) 
			    tolSymbolHeight = rb.resval.rreal;

            if (db_getvar(NULL,DB_QDIMTXT,&rb, pDrw, NULL,NULL) == RTNORM) 
				tolSymbolHeight *= rb.resval.rreal;
		}
    }
    // Modified Cybage VSB 30/07/2001 DD/MM/YY ]

    //
    // ***************** INITIALIZATION BOTH FOR TTF & NON-TTF CASES ********************
    //

    bGenerateBox = !boxonly && pGrView->qtextmode;

    /* Keep obliquing angle away from 90 and 270 deg (where tangent */
    /* is undefined): */
    TanObl = pTextInfo->obl;
    ic_normang(&TanObl, NULL);
    if(fabs(TanObl - IC_PI * 0.5) >= IC_ZRO && fabs(TanObl - IC_PI * 1.5) >= IC_ZRO)
        TanObl = tan(TanObl);
    else
        TanObl = tan(TanObl - IC_ZRO);

    // Define the TCS axes according to the ECS:
    tcsaxis[0][0] = cos(pTextInfo->rot);
    tcsaxis[0][1] = sin(pTextInfo->rot);
    tcsaxis[1][0] = - tcsaxis[0][1];
    tcsaxis[1][1] = tcsaxis[0][0];

    if(pTextInfo->gen & IC_TEXT_GEN_BACKWARD)       // Backward; flip the X-axis.
    {
        tcsaxis[0][0] = - tcsaxis[0][0];
        tcsaxis[0][1] = - tcsaxis[0][1];
    }
    if(pTextInfo->gen & IC_TEXT_GEN_UPSIDEDOWN)     // Upside down; flip the Y-axis.
    {
        tcsaxis[1][0] = - tcsaxis[1][0];
        tcsaxis[1][1] = - tcsaxis[1][1];
    }
    lastspace = pTextInfo->m_lastspace; // EBATECH(CNBR) for MTEXT additional space

    /*D.G.*/// The Old TTF text output method:
    // If GDI can output TrueType text, we don't vectorize the text. Instead, we create a text box
    // for the normal display objects and also keep the actual text fragment for outputting.
    // The New method: see my comments in the truetypeutils.cpp file.
    bool    haveGDITrueTypeText, haveTrueTypeFont;
    haveTrueTypeFont = false;
    haveGDITrueTypeText = TrueTypeTextForGDI(pTextEntity, pGrView, &haveTrueTypeFont);

    // this test was included as an interim solution since we decided not to handle the %% and other codes
    // in text and mtext; it can be removed once IntelliCAD supports the various codes
    if(pTextInfo->dontDrawGDITrueType)
        haveGDITrueTypeText = false;

    // Check if the text is in a block insert and insertion rotation is not zero.
    //if(pGrView->recurlevel > 1 && pGrView->absrot)    // TO DO:
    //  haveGDITrueTypeText = false;                // Remove it implementing correct transformation.

    if(haveGDITrueTypeText)
    {
        if(boxonly)
        {
            if(getBoundingBox(pTextEntity, pTextInfo, pDrw, pGrView, pDC, box) == 0)
            {
                rc = -3;
                goto out;
            }
            bGotBox = true;
            if(bGenerateBox)
                goto genbox;
            else
                goto out;
        }

        if(getDisplayObjects(pTextEntity, pTextInfo, pDrw,  pGrView, pDC, begdo, enddo) == 0)
            rc = -3;

        // we're done; don't do anything below this
        goto out;
    }

    // If we got this far, we're not dealing with TrueType text
    // that can be output by GDI.  This means we have to vectorize
    // the text.

    // we may have a TrueType font that couldn't be displayed with GDI;
    // if so, substitute txt.shx for it
    if(haveTrueTypeFont)
    {
        char szFontName[IC_ACADBUF];
        strcpy(szFontName, "txt.shx"/*DNT*/);
        db_fontlink*    tmpFontLink = NULL; // DO NOT FREE THIS, "NEVER".
        if(db_setfontusingmap(szFontName, NULL, &tmpFontLink, IC_ALLOW_ICAD_FNT, pDrw) >= 0)
            pTextInfo->font = tmpFontLink;
    }


    //
    // ************************ INITIALIZATION FOR NON-TTF CASE ****************************
    //

    // Start the pc list with 100 links. Should be plenty for any one pen-down chain (but we'll check later):
    if(pGrView->npc < 100 && pGrView->allocatePointsChain(100))
    {
        rc = -3;
        goto out;
    }

    wabsxsclp = (pTextInfo->absxsclp > 0.0) ? pTextInfo->absxsclp : 1.0;

    defabove = 6.0;  /* A default for the # of font vectors per chr height. */

    pssz = sizeof(ps) / sizeof(ps[0]);  /* # of elements in the position stack */
    psidx = -1;     /* Idx of most recently filled member of the position stack */
                    /* (-1 means none). */

    /* Set up default for unknown chrs ('?' from icad.fnt): */
    deffontchr.code = 0;
    deffontchr.defsz = sizeof(defdef) / sizeof(defdef[0]);
    deffontchr.def = defdef;

    /*
    **  vfact is the multiplier that is adjustable via codes 3 and 4; it
    **  is set to 1.0 at the beginning of each TEXT item:
    */
    // EBATECH(CNBR) -[ for Extended Shape
    //vfact = 1.0;
    vfactx = vfacty = savex = savey = 1.0;
    // ]- EBATECH(CNBR)

    /* Init the current point (font vector system) and the textbox points */
    /* (TCS): */
    curpt[0] = curpt[1] = curpt[2] = 0.0;
    box[1][0] = box[1][1] = - (box[0][0] = box[0][1] = 1.0 / IC_ZRO);
    oldabove = 1.0;     /* Safe default; see below. */


    //
    // ******************* MAIN LOOP FOR NON-TTF CASE ****************************
    //

    for (valp=pTextInfo->val; *valp; valp++) {  /* Each chr */
        /* Start the Current Chr Stack: */
        while (ccs!=NULL) { tccp1=ccs->next; delete ccs; ccs=tccp1; }
        if ((ccs= new struct currchrlink )==NULL) { rc=-3; goto out; }
		memset(ccs,0,sizeof(struct currchrlink));

        ccs->forcependown=1;

        /* Point ccs->thisfontchr to the db_fontchrlink to use */
        /* (the default, if necessary), and set ccs->whichfont: */
        scoretog=gr_getfontchr(pDrw, &valp,pTextInfo->font,pTextInfo->bigfont,&ccs->thisfontchr,
            &ccs->whichfont);

        if (ccs->thisfontchr==NULL) ccs->thisfontchr=&deffontchr;

        ccs->above=(ccs->whichfont!=NULL && ccs->whichfont->above) ?
            fabs((sds_real)ccs->whichfont->above) : defabove;

        /*
        **  CORRECTING curpt:
        **
        **  Here's a solution to a very subtle problem with curpt.
        **  curpt[] is maintained in the font vector system (and must
        **  stay that way, since maintaining it in the TCS would apply
        **  obliquing, which would screw up our arcs/circles generation).
        **  If a single text item has characters from two fonts (normal
        **  and bigfont) with different vector scales ("above"), then
        **  the relationship between the font vector system and the TCS
        **  changes.  curpt[] is the only value that has to make the
        **  transition and remain valid.  When we switch from one font
        **  to the other, curpt[] is incorrect for the new system.  The
        **  code below applies a simple correction factor so that curpt
        **  looks correct in the new font vector system: we divide by
        **  the old "above" and multiply by the new one.
        **
        **  For the very first character, oldabove is safe (non-0)
        **  but bogus (1.0).  That's okay -- because curpt[] is (0,0,0)
        **  at that time.
        */
        ar1=ccs->above/oldabove;
        if (ar1!=1.0) {
            curpt[0]*=ar1;
            curpt[1]*=ar1;
            curpt[2]*=ar1;  /* In case */
            oldabove=ccs->above;
        }

        /*
        **  ccs->fact[] will be used as the dx and dy multipliers
        **  to convert from font vector units to TCS units:
        */
        ccs->fact[0]=(ccs->fact[1]=pTextInfo->ht/ccs->above)*pTextInfo->wfact;

        /* Capture over/underscore toggle (now that we have ccs->fact[0] */
        /* so that we can convert curpt[0] from font vector system */
        /* to the TCS): */
        if (scoretog==-2 || scoretog==-3) {
            if ((tsp1= new struct scorelink )==NULL)
                    { rc=-3; goto out; }
			memset(tsp1,0,sizeof(struct scorelink));
            tsp1->over=(scoretog==-2);
            tsp1->tcsx=curpt[0]*ccs->fact[0];
            if (scorell==NULL) scorell=tsp1; else pscore->next=tsp1;
            pscore=tsp1;

            continue;
        }

        /*
        **  Walk the chr def.  For each pen-down chain, build pGrView->pc
        **  and send it to gr_gensolid().  (We build the pc chain in
        **  font vector units with the aid of curpt.  Then, when it's
        **  time to process it, we convert it to the TCS, adjust the
        **  textbox, convert it to the NCS, and pass it on to
        **  gr_gensolid().)
        */
        pendown=1; skip=done=cmdcode=0;
        dx=dy=0.0;  /* Used for vectors AND arcs; indicates the endpt */
                    /* in both cases (font vector units -- BEFORE APPLYING */
                    /* vfact). */
        repmode=0;  /* 0, 9, or 13 (the repeating commands) */
        pGrView->upc=0; pGrView->lupc=NULL;  /* Init the pc chain. */
        for (ccs->didx=0; !done; ccs->didx++) {  /* Each def byte */

            /*
            **  Flag a "done" condition -- but don't bail.  Let code
            **  at the bottom of this loop generate the last pc chain.
            **
            **  Also, note that this may mean we're done with a
            **  subshape -- not with the primary chr.  In that case, the
            **  code at the bottom of the loop will fix ccs and set
            **  "done" to zero again to continue after the subshape.
            */
            if (ccs->didx>=ccs->thisfontchr->defsz) done=1;

            vertonly=gotdxdy=circ=genpc=0; arcmode=-1;

            if (!done) {

                /* If "skip" is set, skip the ACTION, but do the STEPPING. */

                if (repmode) {

                    if (++ccs->didx<ccs->thisfontchr->defsz) {
                        if (!ccs->thisfontchr->def[ccs->didx-1] &&
                            !ccs->thisfontchr->def[ccs->didx]) repmode=0;

                        /* The logic from cases 8 and 12 is repeated here: */

                        if (repmode==9) {
                            if (!skip) {
                                dx=(sds_real)ccs->thisfontchr->def[ccs->didx-1];
                                dy=(sds_real)ccs->thisfontchr->def[ccs->didx];
                                gotdxdy=1;
                            }
                        } else if (repmode==13) {
                            if (++ccs->didx<ccs->thisfontchr->defsz && !skip) {
                                arcmode=1;  /* Set up for bulge format (so CW arcs */
                                            /* and circles can be done by */
                                            /* gr_arc2pc() for connectivity with */
                                            /* a chain that's already been started). */

                                /* GET THE PARAMETERS IN FONT VECTOR UNITS: */

                                /* dx and dy to endpt: */
                                dx=(sds_real)ccs->thisfontchr->def[ccs->didx-2];
                                dy=(sds_real)ccs->thisfontchr->def[ccs->didx-1];

                                /* Bulge */
                                if ((fi1=ccs->thisfontchr->def[ccs->didx])>127) fi1=127;
                                else if (fi1<-127) fi1=-127;
                                bulge=((sds_real)fi1)/127.0;
                            }
                        }
                    }

                } else {

                    cmdcode=(ccs->forcependown) ?
                        1 : ccs->thisfontchr->def[ccs->didx];

                    switch (cmdcode) {

                        case  0:  /* End */
                            if (skip) break;
                            ccs->didx=ccs->thisfontchr->defsz;  /* Trigger a "done" */
                            break;
                        case  1:  /* Pen down */
                            /* If we're doing a forced pendown, decrement */
                            /* ccs->didx so that we don't eat a def byte: */
                            if (ccs->forcependown) {
                                ccs->didx--;
                                ccs->forcependown=0;
                            }

                            if (skip) break;

                            pendown=1; dx=dy=0.0; gotdxdy=1;

                            break;
                        case  2:  /* Pen up */
                            if (skip || !pendown) break;
                            pendown=0;
                            genpc=1;
                            break;
                        case  3:  /* Divide vector lengths by next byte */
                            if (++ccs->didx>=ccs->thisfontchr->defsz) break;
                            if (skip) break;
                            if (!ccs->thisfontchr->def[ccs->didx]) break;
                            // EBATECH(CNBR) ]- for extended subshapes
                            //vfact/=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
                            vfactx/=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
                            vfacty/=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
                            // ]- EBATECH(CNBR)
                            break;
                        case  4:  /* Multiply vector lengths by next byte */
                            if (++ccs->didx>=ccs->thisfontchr->defsz) break;
                            if (skip) break;
                            if (!ccs->thisfontchr->def[ccs->didx]) break;
                            // EBATECH(CNBR) ]- for extended subshapes
                            //vfact*=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
                            vfactx*=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
                            vfacty*=((unsigned char)ccs->thisfontchr->def[ccs->didx]);
                            // ]- EBATECH(CNBR)
                            break;
                        case  5:  /* Push position */
                            if (skip || psidx>pssz-2) break;
                            psidx++;
                            ps[psidx][0]=curpt[0];
                            ps[psidx][1]=curpt[1];
                            break;
                        case  6:  /* Pop position */
                            /*
                            **  Okay.  The code's getting a little bizarre
                            **  as I keep patching problems.
                            **
                            **  Pop needs to lift the pen, move, and
                            **  then restore the pen to its original
                            **  status.  We can do all of this by
                            **  popping the position, setting
                            **  ccs->forcependown if the pen is
                            **  currently down, and doing the guts of
                            **  the penup command (case 2),.  (See the
                            **  processing code below the end of this
                            **  "else" and the forcependown code above.)
                            */

                            if (skip || psidx<0) break;

                            /* Pop curpt: */
                            curpt[0]=ps[psidx][0];
                            curpt[1]=ps[psidx][1];
                            dx=dy=0.0; psidx--; gotdxdy=1;

                            /* Set css->forcependown if it's currently down: */
                            ccs->forcependown=(pendown!=0);

                            /* Do a penup command: */
                            pendown=0; genpc=1;

                            break;
                        case  7:  /* Subshape */

                            subsh.extended=0;
                            if (ccs->whichfont!=NULL &&
                                ccs->whichfont->format == db_fontlink::UNIFONT) { /* UNIFONT */

                                if ((ccs->didx+=2)>=ccs->thisfontchr->defsz) break;
                                subsh.code=(unsigned char)
                                    ccs->thisfontchr->def[ccs->didx-1];
                                subsh.code=(subsh.code<<8)|
                                    ((short)((unsigned char)
                                    ccs->thisfontchr->def[ccs->didx]));
                            } else {  /* SHAPE or BIGFONT */
                                if (++ccs->didx>=ccs->thisfontchr->defsz) break;
                                subsh.code=(unsigned char)
                                    ccs->thisfontchr->def[ccs->didx];
                                if (!subsh.code) {
                                    subsh.extended=1;
                                    if ((ccs->didx+=6)>=ccs->thisfontchr->defsz) break;
                                    subsh.code=(unsigned char)
                                        ccs->thisfontchr->def[ccs->didx-5];
                                    // EBATECH(CNBR) ]- Miss it
                                    //subsh.code=(subsh.code<<8)&
                                    //  ((short)((unsigned char)
                                    //  ccs->thisfontchr->def[ccs->didx-4]));
                                    subsh.code=(subsh.code<<8)|
                                        ((short)((unsigned char)
                                        ccs->thisfontchr->def[ccs->didx-4]));
                                    // EBATECH(CNBR) ]-
                                    subsh.basex=ccs->thisfontchr->def[ccs->didx-3];
                                    subsh.basey=ccs->thisfontchr->def[ccs->didx-2];
                                    subsh.wd   =ccs->thisfontchr->def[ccs->didx-1];
                                    subsh.ht   =ccs->thisfontchr->def[ccs->didx];
                                }
                            }

                            /* Now that the stepping has been done: */
                            if (skip || !subsh.code) break;

                            if (subsh.extended) {
                              // EBATECH(CNBR) -[ resolve exteded bigfonts
                              wf=NULL; tfc=NULL;
                              for (fi1=256; fi1<pTextInfo->bigfont->nchrs &&
                                pTextInfo->bigfont->chr[fi1].code!=
                                subsh.code; fi1++);
                              if (fi1>-1 && fi1<pTextInfo->bigfont->nchrs &&
                                pTextInfo->bigfont->chr[fi1].def!=NULL) {
                                tfc=pTextInfo->bigfont->chr+fi1;
                                wf=pTextInfo->bigfont;
                              }
                              /* If we found it */
                              if (tfc!=NULL && wf!=NULL) {
                                /* Push Current Point */
                                if (psidx>pssz-2) break;
                                psidx++;
                                ps[psidx][0]=curpt[0];
                                ps[psidx][1]=curpt[1];
                                /* Set dx/dy and flag */
                                dx=(sds_real)subsh.basex;
                                dy=(sds_real)subsh.basey;
                                gotdxdy=1;
                              }
                              // EBATECH(CNBR) ]-

                              break;

                            } else {
                              // EBATECH(CNBR) Move wf and tfc to local
                              //struct db_fontlink *wf;      /* whichfont */
                              //struct db_fontchrlink *tfc;  /* thisfontchr */

                              /*
                              **  Look for subsh.code and set wf and tfc.
                              **  (A bigfont can subshape a bigfont or
                              **  normal font chr; a normal font can only
                              **  subshape a normal font chr.)
                              */
                              wf=NULL; tfc=NULL;
                              if (ccs->whichfont!=NULL &&
                                  ccs->whichfont->format == db_fontlink::BIGFONT &&
                                  pTextInfo->bigfont!=NULL) {

                                if (subsh.code>-1 && subsh.code<256)
                                  fi1=subsh.code;
                                else for (fi1=256; fi1<pTextInfo->bigfont->nchrs &&
                                  pTextInfo->bigfont->chr[fi1].code!=
                                  subsh.code; fi1++);
                                if (fi1>-1 && fi1<pTextInfo->bigfont->nchrs &&
                                  pTextInfo->bigfont->chr[fi1].def!=NULL) {

                                  tfc=pTextInfo->bigfont->chr+fi1;
                                  wf=pTextInfo->bigfont;
                                }
                              }
                              if (tfc==NULL && pTextInfo->font!=NULL) {
                                if (subsh.code>-1 && subsh.code<256)
                                  fi1=subsh.code;
                                else for (fi1=256; fi1<pTextInfo->font->nchrs &&
                                  pTextInfo->font->chr[fi1].code!=subsh.code; fi1++);
                                if (fi1>-1 && fi1<pTextInfo->font->nchrs &&
                                  pTextInfo->font->chr[fi1].def!=NULL) {

                                  tfc=pTextInfo->font->chr+fi1;
                                  wf=pTextInfo->font;
                                }
                              }

                              /* If we found it, create the new link on top */
                              /* of the current chr stack: */
                              if (tfc!=NULL && wf!=NULL) {

                                if ((tccp1= new struct currchrlink )==NULL)
                                    { rc=-3; goto out; }
								memset(tccp1,0,sizeof(struct currchrlink));

                                tccp1->forcependown=1;

                                /* Set vals for the subshape: */
                                tccp1->whichfont=wf;
                                tccp1->thisfontchr=tfc;
                                tccp1->didx=-1;  /* Loop will inc to 0 */
                                tccp1->above=(tccp1->whichfont!=NULL &&
                                  tccp1->whichfont->above) ?
                                    fabs((sds_real)tccp1->whichfont->above) :
                                    defabove;
                                tccp1->fact[0]=(tccp1->fact[1]=pTextInfo->ht/tccp1->above)*
                                  pTextInfo->wfact;

                                /* Push it onto the top: */
                                tccp1->next=ccs; ccs=tccp1;

                                /* Correct curpt for the new "above" value. */
                                /* (See "CORRECTING curpt" note earlier.): */
                                ar1=ccs->above/ccs->next->above;
                                if (ar1!=1.0) {
                                  curpt[0]*=ar1;
                                  curpt[1]*=ar1;
                                  curpt[2]*=ar1;  /* In case */
                                }

                                /* New ccs link in place; let 'er rip. */

                              }
                            }  /* End else not an extended subshape. */

                            break;
                        case  8:  /* dx,dy in next 2 bytes */
                            if ((ccs->didx+=2)>=ccs->thisfontchr->defsz) break;
                            if (skip) break;
                            dx=(sds_real)ccs->thisfontchr->def[ccs->didx-1];
                            dy=(sds_real)ccs->thisfontchr->def[ccs->didx];
                            gotdxdy=1;
                            break;
                        case  9: case 13:  /* Repeat until (0,0) */
                            repmode=ccs->thisfontchr->def[ccs->didx];
                            break;
                        case 10:  /* Octant arc (next 2 bytes) */
                            if ((ccs->didx+=2)>=ccs->thisfontchr->defsz) break;
                            if (skip) break;

                            arcmode=1;  /* Set up for bulge format (so CW arcs */
                                        /* and circles can be done by */
                                        /* gr_arc2pc() for connectivity with */
                                        /* a chain that's already been started). */

                            cw=((ccs->thisfontchr->def[ccs->didx]&'\x80')!=0);

                            /* GET THE PARAMETERS IN FONT VECTOR UNITS: */


                            /* Get the radius: */
                            rad=(sds_real)
                                ((unsigned char)ccs->thisfontchr->def[ccs->didx-1]);
                            if (rad<1.0) rad=1.0;

                            /* Start angle: */
                            sa=0.25*IC_PI*(((unsigned char)
                                (ccs->thisfontchr->def[ccs->didx]&'\x70'))>>4);

                            /* Included angle: */
                            if ((fi1=ccs->thisfontchr->def[ccs->didx]&'\x07')==0) {
                                /* Circle; set up an arc that almost closes */
                                /* (so we can continue to use bulge format */
                                /* as discussed above). */

                                /* Get the unit tangent direction vector: */
                                if (cw) { ap1[0]=-sin(sa); ap1[1]= cos(sa); }
                                else    { ap1[0]= sin(sa); ap1[1]=-cos(sa); }

                                /* Put the 2nd pt 0.01 font vector units away */
                                /* from the 1st pt in that direction: */
                                ar1=0.01;
                                dx=ar1*ap1[0]; dy=ar1*ap1[1];
                                /*
                                **  The approx angle subtended is ar1/rad.
                                **  The tangent of one fourth of this is the
                                **  fabs(bulge) of the short arc.  The
                                **  inverse of that is the fabs(bulge) of
                                **  the long arc:
                                */
                                bulge=1.0/tan(0.25*ar1/rad);

                                circ=1;
                            } else {
                                iang=0.25*IC_PI*fi1;

                                if (cw) ea=sa-iang; else ea=sa+iang;

                                /* dx and dy to endpt: */
                                dx=rad*(cos(ea)-cos(sa));
                                dy=rad*(sin(ea)-sin(sa));

                                /* abs(bulge): */
                                bulge=tan(0.25*iang);
                            }
                            if (cw) bulge=-bulge;  /* Neg bulge for CW arcs. */

                            break;
                        case 11:  /* Fractional arc (next 5 bytes) */

                            /* See documentation in learned.doc for this one; */
                            /* ACAD's documentation is incorrect and */
                            /* insufficient. */

                          {
                            sds_real soct,eoctrel,soff,eoff;

                            if ((ccs->didx+=5)>=ccs->thisfontchr->defsz) break;
                            if (skip) break;

                            arcmode=1;  /* Set up for bulge format (so CW arcs */
                                        /* and circles can be done by */
                                        /* gr_arc2pc() for connectivity with */
                                        /* a chain that's already been started). */

                            cw=((ccs->thisfontchr->def[ccs->didx]&'\x80')!=0);

                            /* GET THE PARAMETERS IN FONT VECTOR UNITS: */
                            /* (Note we work in degrees for rounding.) */

                            /* Radius: */
                            rad=256.0*((sds_real)((unsigned char)
                                ccs->thisfontchr->def[ccs->didx-2]))+
                                (sds_real)((unsigned char)
                                ccs->thisfontchr->def[ccs->didx-1]);
                            if (rad<1.0) rad=1.0;

                            /* Starting octant (deg): */
                            soct=45.0*(((unsigned char)
                                (ccs->thisfontchr->def[ccs->didx]&'\x70'))>>4);

                            /* Ending octant rel to starting octant (deg): */
                            if ((fi1=ccs->thisfontchr->def[ccs->didx]&'\x07')<1) fi1=8;
                            eoctrel=45.0*(fi1-1);

                            /* Starting offset (deg): */
                            ar1=((sds_real)((unsigned char)
                                ccs->thisfontchr->def[ccs->didx-4]))*45.0/256.0;
                            if (modf(ar1,&soff)>=0.5) soff+=1.0;

                            /* Ending offset (deg): */
                            ar1=((sds_real)((unsigned char)
                                ccs->thisfontchr->def[ccs->didx-3]))*45.0/256.0;
                            if (modf(ar1,&eoff)>=0.5) eoff+=1.0;
                            if (eoff<0.9) eoff=45.0;

                            /* Starting angle: */
                            sa=soct+((cw) ? -soff : soff);

                            /* Ending angle: */
                            ar1=eoctrel+eoff;
                            ea=soct+((cw) ? -ar1 : ar1);

                            /* Convert to radians finally: */
                            ar1=IC_PI/180.0;
                            sa*=ar1; ea*=ar1;

                            /* Get the included angle: */
                            ic_normang(&sa,&ea);
                            iang=ea-sa; if (cw) iang=IC_TWOPI-iang;

                            ar1=fabs(iang);
                            if (ar1<IC_ZRO || fabs(ar1-IC_TWOPI)<IC_ZRO) {
                                /* Circle; set up an arc that almost closes */
                                /* (so we can continue to use bulge format */
                                /* as discussed above). */

                                /* Get the unit tangent direction vector: */
                                if (cw) { ap1[0]=-sin(sa); ap1[1]= cos(sa); }
                                else    { ap1[0]= sin(sa); ap1[1]=-cos(sa); }

                                /* Put the 2nd pt 0.01 font vector units away */
                                /* from the 1st pt in that direction: */
                                ar1=0.01;
                                dx=ar1*ap1[0]; dy=ar1*ap1[1];
                                /*
                                **  The approx angle subtended is ar1/rad.
                                **  The tangent of one fourth of this is the
                                **  fabs(bulge) of the short arc.  The
                                **  inverse of that is the fabs(bulge) of
                                **  the long arc:
                                */
                                bulge=1.0/tan(0.25*ar1/rad);

                                circ=1;
                            } else {
                                /* dx and dy to endpt: */
                                dx=rad*(cos(ea)-cos(sa));
                                dy=rad*(sin(ea)-sin(sa));

                                /* abs(bulge): */
                                bulge=tan(0.25*iang);
                            }
                            if (cw) bulge=-bulge;  /* Neg bulge for CW arcs. */

                            break;

                          }

                        case 12:  /* Arc by bulge (next 3 bytes) */
                            if ((ccs->didx+=3)>=ccs->thisfontchr->defsz) break;
                            if (skip) break;

                            arcmode=1;  /* Set up for bulge format (so CW arcs */
                                        /* and circles can be done by */
                                        /* gr_arc2pc() for connectivity with */
                                        /* a chain that's already been started). */

                            /* GET THE PARAMETERS IN FONT VECTOR UNITS: */

                            /* dx and dy to endpt: */
                            dx=(sds_real)ccs->thisfontchr->def[ccs->didx-2];
                            dy=(sds_real)ccs->thisfontchr->def[ccs->didx-1];

                            /* Bulge */
                            if ((fi1=ccs->thisfontchr->def[ccs->didx])>127) fi1=127;
                            else if (fi1<-127) fi1=-127;
                            bulge=((sds_real)fi1)/127.0;

                            break;
                        case 14:  /* Process next command only for vertical text */
                            vertonly=1;
                            break;
                        case 15:  /* Not used */
                            break;
                        default:  /* Vector/direction */
                            if (skip) break;
                            vlen=(unsigned char)ccs->thisfontchr->def[ccs->didx];
                            vdir=vlen&'\x0F'; if (!(vlen>>=4)) break;
                            switch (vdir) {
                                case '\x00': dx= 1.0; dy= 0.0; break;
                                case '\x01': dx= 1.0; dy= 0.5; break;
                                case '\x02': dx= 1.0; dy= 1.0; break;
                                case '\x03': dx= 0.5; dy= 1.0; break;
                                case '\x04': dx= 0.0; dy= 1.0; break;
                                case '\x05': dx=-0.5; dy= 1.0; break;
                                case '\x06': dx=-1.0; dy= 1.0; break;
                                case '\x07': dx=-1.0; dy= 0.5; break;
                                case '\x08': dx=-1.0; dy= 0.0; break;
                                case '\x09': dx=-1.0; dy=-0.5; break;
                                case '\x0A': dx=-1.0; dy=-1.0; break;
                                case '\x0B': dx=-0.5; dy=-1.0; break;
                                case '\x0C': dx= 0.0; dy=-1.0; break;
                                case '\x0D': dx= 0.5; dy=-1.0; break;
                                case '\x0E': dx= 1.0; dy=-1.0; break;
                                case '\x0F': dx= 1.0; dy=-0.5; break;
                            }
                            dx*=vlen; dy*=vlen; gotdxdy=1;

                            break;
                    }

                }  /* End else !repmode */


                if (gotdxdy || arcmode>-1) {  /* Process vector or arc cmd */
                    // EBATECH(CNBR) ]- for extended subshapes
                    //endpt[0]=curpt[0]+dx*vfact;
                    //endpt[1]=curpt[1]+dy*vfact;
                    endpt[0]=curpt[0]+dx*vfactx;
                    endpt[1]=curpt[1]+dy*vfacty;
                    // ]- EBATECH(CNBR)

//VS: to consider interspace before and after character{
                    // Apply height, width factor, and obliquing
                    // to get to the TSC:
                    ap1[1]=endpt[1]*ccs->fact[1];
                    ap1[0]=endpt[0]*ccs->fact[0]+ap1[1]*TanObl;
                     // EBATECH(CNBR) only when pen down
                     // EBATECH(CNBR) -[ MTEXT needs last spaces.
                    if ( pendown || lastspace ) 
                    //if ( pendown) 
                    // EBATECH(CNBR) ]-
                    {
                        // Adjust the textbox:
                        if (box[0][0]>ap1[0]) box[0][0]=ap1[0];
                        if (box[1][0]<ap1[0]) box[1][0]=ap1[0];
                        if (box[0][1]>ap1[1]) box[0][1]=ap1[1];
                        if (box[1][1]<ap1[1]) box[1][1]=ap1[1];
                        bGotBox = true;
                    }
					if (bGotBox && pTextEntity->ret_type() == DB_TOLERANCE && tolSymbolHeight > 0.0) {
						double yCenter = (box[1][1] + box[0][1]) / 2.0;
						double yDiff = tolSymbolHeight / 2.0;
						box[0][1] = yCenter - yDiff;
						box[1][1] = yCenter + yDiff;
					}
                    // EBATECH(CNBR) only when pen down
//VS}
                    if (pendown) {

                        if (gotdxdy) {  /* Vector */

                            /*
                            **  In font isocp, several European chrs are
                            **  defined by subshaping English chrs and then
                            **  adding marks above them.  When the subshape
                            **  command is the first command, TWO pendowns
                            **  occur (the automatics) and a dot was being
                            **  produced.  The "if" below guards against
                            **  successive pendown commands where no
                            **  intervening drawing is occurring:
                            */
                            if (cmdcode!=1 || pGrView->lupc==NULL ||
                                !icadRealEqual(endpt[0],pGrView->lupc->pt[0]) ||
                                !icadRealEqual(endpt[1],pGrView->lupc->pt[1])) {

                                if (pGrView->upc>=pGrView->npc) {  /* Add 100 links: */
                                    fl1=pGrView->upc;  /* Save; allocatePointsChain() will set it. */
                                    if (pGrView->allocatePointsChain(pGrView->npc+100))
                                    {
                                        rc = -3;
                                        goto out;
                                    }
                                    pGrView->upc=fl1;
                                }

                                pGrView->upc++;
                                pGrView->lupc=(pGrView->lupc==NULL) ?
                                    pGrView->pc : pGrView->lupc->next;
                                pGrView->lupc->d2go=-1.0;
                                pGrView->lupc->swid=pGrView->lupc->ewid=0.0;
                                pGrView->lupc->startang=pGrView->lupc->endang=IC_ANGLE_DONTCARE;

                                /* Keep the pc chain in the font vector system */
                                /* for now: */
                                pGrView->lupc->pt[0]=endpt[0];
                                pGrView->lupc->pt[1]=endpt[1];
                                pGrView->lupc->pt[2]=0.0;
                            }

                        } else {  /* arcmode>-1 */

                            /*
                            **  We're going to call gr_arc2pc(), so
                            **  make sure that pGrView->lupc is
                            **  non-NULL.  (The pt is irrelevant here
                            **  because gr_arc2pc() will replace it.)
                            */
                            if (pGrView->lupc==NULL) {
                                pGrView->lupc=pGrView->pc; pGrView->upc=1;
                                pGrView->lupc->pt[0]=pGrView->lupc->pt[1]=
                                    pGrView->lupc->pt[2]=0.0;
                                pGrView->lupc->d2go=-1.0;
                                pGrView->lupc->swid=pGrView->lupc->ewid=0.0;
                                pGrView->lupc->startang=pGrView->lupc->endang=IC_ANGLE_DONTCARE;
                            }

                            /* Call gr_arc2pc(); it should write over */
                            /* pGrView->lupc: */
                            if ((rc=gr_arc2pc(arcmode,curpt[0],curpt[1],
                                endpt[0],endpt[1],bulge,pTextInfo->llpt[2],1,0.0,0.0,
                                1,  /* ltsclwblk */
                                /* pixsz: How big is a pixel in font vectors?: */
                                (boxonly) ?
                                    ccs->above/100.0 :
                                    fabs(pGrView->GetPixelHeight())/ccs->fact[1]/wabsxsclp,
                                /* Force a good resolution for boxonly: */
                                (boxonly) ? 100 : pGrView->curvdispqual,
                                pGrView))!=0)
                                        goto out;

                            /* If it's a circle, add 1 more seg to close */
                            /* the circle (since our bulge-specified */
                            /* "circles" don't quite close): */
                            if (circ) {
                                if (pGrView->upc>=pGrView->npc) {  /* Add 100 links: */
                                    fl1=pGrView->upc;  /* Save; allocatePointsChain() will set it. */
                                    if (pGrView->allocatePointsChain(pGrView->npc+100))
                                    {
                                        rc = -3;
                                        goto out;
                                    }
                                    pGrView->upc=fl1;
                                }
                                pGrView->upc++;
                                pGrView->lupc=(pGrView->lupc==NULL) ?
                                    pGrView->pc : pGrView->lupc->next;
                                pGrView->lupc->d2go=-1.0;
                                pGrView->lupc->swid=pGrView->lupc->ewid=0.0;
                                pGrView->lupc->startang=pGrView->lupc->endang=IC_ANGLE_DONTCARE;

                                GR_PTCPY(pGrView->lupc->pt,curpt);
                            }

                        }  /* End else arcmode>-1 */

                    }  /* End if pendown */

                    curpt[0]=endpt[0]; curpt[1]=endpt[1];

                }  /* End if (gotdxdy || arcmode>-1) */

            }  /* End if (!done) */

            /* pc chain finished if genpc is set, or we're done with */
            /* the primary chr (not just done with a subshape): */
            if (genpc || (done && ccs->next==NULL)) {
                if (pGrView->upc>1) {
                    /*
                    ** This pc chain is finished and has more
                    ** than 1 pt.  Walk it and do the following:
                    ** transform from font vector system to TCS;
                    ** adjust the textbox; transform from TCS to
                    ** NCS; call gr_gensolid():
                    */
					for (pcidx=0L,tpcp1=pGrView->pc; pcidx<
						 pGrView->upc && tpcp1!=NULL; pcidx++,
						 tpcp1=tpcp1->next) {

						// Apply height, width factor, and obliquing to get to the TSC:
						ap1[1]=tpcp1->pt[1]*ccs->fact[1];
						ap1[0]=tpcp1->pt[0]*ccs->fact[0]+ap1[1]*TanObl;

						// Adjust the textbox:
						if (box[0][0]>ap1[0]) box[0][0]=ap1[0];
						if (box[1][0]<ap1[0]) box[1][0]=ap1[0];
						if (box[0][1]>ap1[1]) box[0][1]=ap1[1];
						if (box[1][1]<ap1[1]) box[1][1]=ap1[1];
						bGotBox = true;

						// Transform to NCS and put the pt back into the pc chain:
						tpcp1->pt[0] = ap1[0] * tcsaxis[0][0] + ap1[1] * tcsaxis[1][0] + pTextInfo->llpt[0];
						tpcp1->pt[1] = ap1[0] * tcsaxis[0][1] + ap1[1] * tcsaxis[1][1] + pTextInfo->llpt[1];
						tpcp1->pt[2] = pTextInfo->llpt[2];
					}
	
					if (bGotBox && pTextEntity->ret_type() == DB_TOLERANCE && tolSymbolHeight > 0.0) {
						double yCenter = (box[1][1] + box[0][1]) / 2.0;
						double yDiff = tolSymbolHeight / 2.0;
						box[0][1] = yCenter - yDiff;
						box[1][1] = yCenter + yDiff;
					}

                    /* Gen the disp objs (if needed): */
                    if (!boxonly && !bGenerateBox)
                    {
#ifdef _DOCLASSES_
                        CDoBase::get().setEntity(pTextEntity);
                        CDoBase::get().setViewData(pGrView);
                        CDoBase::get().setPointsData(pGrView->pc, pGrView->lupc, pGrView->pc->next, pGrView->upc-2);
                        CDoBase::get().setFlags(0, 0);
                        CDoBase::get().setDotDir(NULL);
                        if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0)
#else
                        if ((rc=gr_gensolid( pTextEntity, pGrView->pc,pGrView->lupc,
                            pGrView->pc->next,pGrView->upc-2,0,0,NULL,
                            &tdop1,&tdop2,pGrView))!=0)
#endif
                            goto out;
                        if (tdop1!=NULL && tdop2!=NULL) {
                            if (begdo==NULL) begdo=tdop1; else enddo->next=tdop1;
                            enddo=tdop2;
                        }
                    }
                }

                pGrView->upc=0; pGrView->lupc=NULL;  /* Start over */

            }  /* End if (genpc || done) */

            /* Did we just finish a subshape? */
            if (done && ccs->next!=NULL) {
                /* Correct curpt for the "above" value we're restoring. */
                /* (See "CORRECTING curpt" note earlier.): */
                ar1=ccs->next->above/ccs->above;
                if (ar1!=1.0) {
                  curpt[0]*=ar1;
                  curpt[1]*=ar1;
                  curpt[2]*=ar1;  /* In case */
                }

                /* Discard the top link that we just finished: */
                tccp1=ccs->next; delete ccs; ccs=tccp1;

                done=0; /* Continue with the parent. */
                // EBATECH(CNBR) ]- for extended subshape
                if (subsh.extended) {
                    if (psidx<0) break;
                    /* Pop curpt: */
                    curpt[0]=ps[psidx][0];
                    curpt[1]=ps[psidx][1];
                    psidx--;
                    vfactx /= savex;
                    vfacty /= savey;
                }
                // EBATECH(CNBR) ]-
            }

            if (!repmode) skip=(vertonly && !pTextInfo->vert);

            // EBATECH(CNBR) ]- extended subshape Start
            if (cmdcode==7 && subsh.extended && tfc!=NULL && wf!=NULL ) {
                /* create subshape's current char link*/
                if ((tccp1= new struct currchrlink )==NULL)
                    { rc=-3; goto out; }
				memset(tccp1,0,sizeof(struct currchrlink));
                tccp1->forcependown=1;
                /* Set vals for the subshape: */
                tccp1->whichfont=wf;
                tccp1->thisfontchr=tfc;
                tccp1->didx=-1;  /* Loop will inc to 0 */
                tccp1->above=(tccp1->whichfont!=NULL &&
                  tccp1->whichfont->above) ?
                    fabs((sds_real)tccp1->whichfont->above) :
                    defabove;
                tccp1->fact[1]=ccs->fact[1];
                tccp1->fact[0]=ccs->fact[0];
                /* Push it onto the top: */
                tccp1->next=ccs; ccs=tccp1;
                /* Then, apply subsape's scale factor */
                savex = (sds_real)subsh.wd / (sds_real)ccs->whichfont->width;
                savey = (sds_real)subsh.ht / (sds_real)ccs->above;
                vfactx *= savex;
                vfacty *= savey;
            }
            // EBATECH(CNBR) ]-
        }  /* End for each chr def byte */

    }  /* End for each chr */


    /* Do the over and under scoring (process scorell) */

    if (scorell!=NULL) {
        short oscore,uscore,nullpass,got1;
        sds_real begox,begux,begx,endx,scoreht;

        oscore=uscore=0; begox=begux=0.0; begx=endx=scoreht=0.0;
        nullpass=0;  /* 0 to terminated unterminated underscore; */
                     /* 1 to terminated unterminated  overscore. */
        for (;;) {
            got1=0;

            if ((scorell==NULL && nullpass==0) ||
                (scorell!=NULL && !scorell->over)) {

                if (uscore) {
                    uscore=0; scoreht=-0.2*pTextInfo->ht;
                    ar1=scoreht*TanObl;
                    begx=begux+ar1;
                    endx=((scorell==NULL) ?
                        curpt[0]*ccs->fact[0] : scorell->tcsx)+ar1;
                    got1=1;
                } else if (scorell!=NULL) {
                    uscore=1; begux=scorell->tcsx;
                }

            }

            if ((scorell==NULL && nullpass==1) ||
                (scorell!=NULL && scorell->over)) {

                if (oscore) {
                    oscore=0; scoreht=1.2*pTextInfo->ht;
                    ar1=scoreht*TanObl;
                    begx=begox+ar1;
                    endx=((scorell==NULL) ?
                        curpt[0]*ccs->fact[0] : scorell->tcsx)+ar1;
                    got1=1;
                } else if (scorell!=NULL) {
                    oscore=1; begox=scorell->tcsx;
                }

            }

            if (got1 && fabs(begx-endx)>IC_ZRO*pTextInfo->ht) {
                /* Adjust the textbox: */
                if (box[0][0]>begx)    box[0][0]=begx;
                if (box[1][0]<begx)    box[1][0]=begx;
                if (box[0][0]>endx)    box[0][0]=endx;
                if (box[1][0]<endx)    box[1][0]=endx;
                if (box[0][1]>scoreht) box[0][1]=scoreht;
                if (box[1][1]<scoreht) box[1][1]=scoreht;
                bGotBox = true;

				if (bGotBox && pTextEntity->ret_type() == DB_TOLERANCE && tolSymbolHeight > 0.0) {
					double yCenter = (box[1][1] + box[0][1]) / 2.0;
					double yDiff = tolSymbolHeight / 2.0;
					box[0][1] = yCenter - yDiff;
					box[1][1] = yCenter + yDiff;
				}
                /* Gen the disp objs (if needed): */
                if (!boxonly && !bGenerateBox) {
                    /* Set up the pc chain with NCS coords: */
                    if (pGrView->allocatePointsChain(2))
                    {
                        rc = -3;
                        goto out;
                    }
                    pGrView->lupc=pGrView->pc->next;
                    pGrView->pc->d2go=pGrView->lupc->d2go=-1.0;
                    pGrView->pc->swid=pGrView->pc->ewid=
                        pGrView->lupc->swid=pGrView->lupc->ewid=0.0;
                    pGrView->pc->startang=pGrView->pc->endang=
                        pGrView->lupc->startang=pGrView->lupc->endang=IC_ANGLE_DONTCARE;
                    ar1=scoreht*tcsaxis[1][0]+pTextInfo->llpt[0];
                    pGrView->pc->pt[0]  =begx*tcsaxis[0][0]+ar1;
                    pGrView->lupc->pt[0]=endx*tcsaxis[0][0]+ar1;
                    ar1=scoreht*tcsaxis[1][1]+pTextInfo->llpt[1];
                    pGrView->pc->pt[1]  =begx*tcsaxis[0][1]+ar1;
                    pGrView->lupc->pt[1]=endx*tcsaxis[0][1]+ar1;

                    /* Gen: */
#ifdef _DOCLASSES_
                    CDoBase::get().setEntity(pTextEntity);
                    CDoBase::get().setViewData(pGrView);
                    CDoBase::get().setPointsData(pGrView->pc, pGrView->lupc, NULL, 0);
                    CDoBase::get().setFlags(0, 0);
                    CDoBase::get().setDotDir(NULL);
                    if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0)
#else
                    if ((rc=gr_gensolid( pTextEntity, pGrView->pc,pGrView->lupc,
                        NULL,0,0,0,NULL,&tdop1,&tdop2,pGrView))!=0)
#endif
                        goto out;
                    if (tdop1!=NULL && tdop2!=NULL) {
                        if (begdo==NULL) begdo=tdop1; else enddo->next=tdop1;
                        enddo=tdop2;
                    }
                }
            }

            if (scorell==NULL) {
                 if (nullpass>0) break;
                 nullpass++;
            } else {
                tsp1=scorell->next; delete scorell; scorell=tsp1;
            }

        }  /* End of scorell-processing loop */

    }  /* End if scorell!=NULL */

genbox:

    if(bGenerateBox && bGotBox)
    {
        // Use box[] and pGrView->pc to gen the box.
        if(pGrView->npc < 5 && pGrView->allocatePointsChain(5))
        {
            rc = -3;
            goto out;
        }

        pGrView->upc = 5;
        pGrView->lupc = pGrView->pc;

		if (bGotBox && pTextEntity->ret_type() == DB_TOLERANCE && tolSymbolHeight > 0.0) {
			double yCenter = (box[1][1] + box[0][1]) / 2.0;
			double yDiff = tolSymbolHeight / 2.0;
			box[0][1] = yCenter - yDiff;
			box[1][1] = yCenter + yDiff;
		}

        for(fi1 = 0; fi1 < 4; fi1++, pGrView->lupc = pGrView->lupc->next)
        {
            pGrView->lupc->d2go = -1.0;
            pGrView->lupc->swid = pGrView->lupc->ewid = 0.0;
            pGrView->lupc->startang = pGrView->lupc->endang = IC_ANGLE_DONTCARE;

            switch(fi1)
            {
                case 0: GR_PTCPY(ap1, box[0]);
                        break;
                case 1: ap1[0] = box[0][0];
                        ap1[1] = box[1][1];
                        break;
                case 2: GR_PTCPY(ap1, box[1]);
                        break;
                case 3: ap1[0] = box[1][0];
                        ap1[1] = box[0][1];
            }

            // transform from TCS to NCS and set the pc pt
            // TO DO: make correct transformation for all cases (rotated in 3D etc.)
            ap1[0] += ap1[1] * TanObl;

            pGrView->lupc->pt[0] = ap1[0] * tcsaxis[0][0] + ap1[1] * tcsaxis[1][0] + pTextInfo->llpt[0];
            pGrView->lupc->pt[1] = ap1[0] * tcsaxis[0][1] + ap1[1] * tcsaxis[1][1] + pTextInfo->llpt[1];
            pGrView->lupc->pt[2] = pTextInfo->llpt[2];
        }
        pGrView->lupc->d2go = -1.0;
        pGrView->lupc->swid = pGrView->lupc->ewid = 0.0;
        pGrView->lupc->startang = pGrView->lupc->endang = IC_ANGLE_DONTCARE;
        GR_PTCPY(pGrView->lupc->pt, pGrView->pc->pt);
#ifdef _DOCLASSES_
        CDoBase::get().setEntity(pTextEntity);
        CDoBase::get().setViewData(pGrView);
        CDoBase::get().setPointsData(pGrView->pc, pGrView->lupc, pGrView->pc->next, pGrView->upc - 2);
        CDoBase::get().setFlags(1, 0);
        CDoBase::get().setDotDir(NULL);
        if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0)
#else
        if(rc = gr_gensolid(pTextEntity, pGrView->pc, pGrView->lupc, pGrView->pc->next, pGrView->upc - 2, 1, 0, NULL, &tdop1, &tdop2, pGrView))
#endif
            goto out;
        if(tdop1 && tdop2)
        {
            if(!begdo)
                begdo = tdop1;
            else
                enddo->next = tdop1;
            enddo = tdop2;
        }
    }


out:

    deffontchr.def = NULL;  /* Keep destructor from freeing. */

    if(!rc)
    {
        if(ppBegDispObj && ppEndDispObj && !boxonly)
        {
            *ppBegDispObj = begdo;
            *ppEndDispObj = enddo;
            begdo = enddo = NULL;
        }

        if(bGotBox && MinPoint && MaxPoint)
        {
            GR_PTCPY(MinPoint, box[0]);
            GR_PTCPY(MaxPoint, box[1]);
            MinPoint[2] = MaxPoint[2] = 0.0;

            /* Adjust for backward and/or upside down for the caller.   */
            /* (For our use in here, we don't need to because we use    */
            /* the TCS axes, which we adjusted earlier.):               */
            if(pTextInfo->gen & IC_TEXT_GEN_BACKWARD)       /* Backward; swap and negate X's.   */
            {
                ar1 = MinPoint[0];
                MinPoint[0] = - MaxPoint[0];
                MaxPoint[0] = - ar1;
            }
            if(pTextInfo->gen & IC_TEXT_GEN_UPSIDEDOWN)     /* Upside down; swap and negate Y's.*/
            {
                ar1 = MinPoint[1];
                MinPoint[1] = - MaxPoint[1];
                MaxPoint[1] = - ar1;
            }
        }
    }

    if(begdo)
    {
        gr_freedisplayobjectll(begdo);
        begdo = enddo = NULL;
    }

    pGrView->ltsclwblk = savltsclwblk;

    while(ccs)
    {
        tccp1 = ccs->next;
        delete ccs;
        ccs = tccp1;
    }

    while(scorell)
    {
        tsp1 = scorell->next;
        delete scorell;
        scorell = tsp1;
    }

    return rc;
}
