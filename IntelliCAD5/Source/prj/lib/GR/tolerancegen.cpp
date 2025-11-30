/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract 
 * 
 * tolerance generation functions
 * 
 */ 

#include "gr.h"
#include "gr_view.h"
#include "tolerancegen.h"
#include "styletabrec.h"
#include "linetypegen.h"
#include "pc2dispobj.h"

/*
**  This struct is just used by the next two functions for tracking
**  text parameters for text fragments from formatted text buffers
**  from MTEXT and TOLERANCE entities.
*/
// 6/98 - in reality this is only used for tolerance - so included in tolerance file.
struct gr_fragstacklink {
    gr_textgenparslink tgpar;      /* tgpar.val not used here */
    int                color;      /* Not a part of tgpar */
    char               overline;
    char               underline;

    gr_fragstacklink  *next;
};


int gr_makefrag(
	db_tolerance	  *pTolerance,
    gr_ftextfraglink **newfragpp,
    gr_fragstacklink  *stk,
    char              *astr,
    int               *aidxp,
    char               paragraphended,
    char               rowended,
    char               boxended,
    gr_view           *viewp,
	CDC				  *dc,
    db_drawing        *dp) {
/*
**  This is an extension of gr_getftextfrags() (in lieu of a gosub),
**  so read that function first to understand this one.
**  My usual pass parameter safety checks aren't performed here
**  because I know how gr_getftextfrags() is setting it up.
**  (Putting this one first avoids the need for a prototype.)
*/
    int aidx,nspaces[2],rc,fi1,fi2;
    sds_real ar1;
    struct gr_ftextfraglink *newfrag;


    rc=0; newfrag=NULL;
    aidx=*aidxp;  /* For simplicity */

    /* Create a frag link: */
    if ((newfrag= new gr_ftextfraglink )==NULL)
        { rc=-3; goto out; }
	memset(newfrag,0,sizeof(gr_ftextfraglink));

    /* Copy the current parameters BEFORE we set newfrag->tgpar.val: */
    memcpy(&newfrag->tgpar,&stk->tgpar,sizeof(newfrag->tgpar));
    newfrag->tgpar.val=NULL;  /* To be safe */
    newfrag->color=stk->color;
    newfrag->eopara=paragraphended;
    newfrag->eotolrow=rowended;
    newfrag->eotolbox=boxended;

    /* Take the characters captured in astr -- and prepend the overline */
    /* and/or underline toggles, if necessary (but not if astr is ""): */
    if ((newfrag->tgpar.val= new char [	
        aidx+1+3*(aidx>0)*(stk->overline+stk->underline)])==NULL) { rc=-3; goto out; }
    fi1=0;
    if (aidx>0) {
        if (stk->overline) {
            newfrag->tgpar.val[fi1++]='%';
            newfrag->tgpar.val[fi1++]='%';
            newfrag->tgpar.val[fi1++]='o';
        }
        if (stk->underline) {
            newfrag->tgpar.val[fi1++]='%';
            newfrag->tgpar.val[fi1++]='%';
            newfrag->tgpar.val[fi1++]='u';
        }
    }
    astr[aidx]=0; strcpy(newfrag->tgpar.val+fi1,astr);

    /* Determine the textbox: */
    if (aidx>0) {
        /*
        **  Leading and trailing spaces are a problem.  We are going
        **  to solve it, for now, by adding capital H's just long enough
        **  to do the textbox determination.  (Just true spaces, for now --
        **  not any other white space chars.)  Note that the TOLERANCE
        **  symbol font has capital letters; it's just the lower case
        **  letters that are used as symbols.
        */

        /* Count leading [0] and trailing [1] spaces and substitute H's: */
        nspaces[0]=nspaces[1]=0;
        for (fi1=0; newfrag->tgpar.val[fi1]==' '; fi1++)
            { nspaces[0]++; newfrag->tgpar.val[fi1]='H'; }
        if (newfrag->tgpar.val[fi1]) {  /* Hit a non-space before the end. */
            while (newfrag->tgpar.val[fi1]) fi1++;
            for (fi1--; fi1>-1 && newfrag->tgpar.val[fi1]==' '; fi1--)
                { nspaces[1]++; newfrag->tgpar.val[fi1]='H'; }
        }

        /* Get the textbox: */
        if ((rc=gr_textgen(pTolerance,
						  &newfrag->tgpar,
						  1,
						  dp,
						  viewp,
						  dc,
						  NULL,
						  NULL,
						  newfrag->textboxpt[0],
						  newfrag->textboxpt[1]))!=0)
							goto out;

        /* Put the spaces back: */
        for (fi1=0; fi1<nspaces[0]; fi1++) newfrag->tgpar.val[fi1]=' ';
        while (newfrag->tgpar.val[fi1]) fi1++;
        for (fi1--,fi2=0; fi2<nspaces[1]; fi2++)
            newfrag->tgpar.val[fi1-fi2]=' ';

        /* Make SURE textboxpt[0] is lower left and textboxpt[1] is */
        /* upper right so that we can avoid later checks or confusion: */
        if (newfrag->textboxpt[0][0]>newfrag->textboxpt[1][0]) {
            ar1=newfrag->textboxpt[0][0];
            newfrag->textboxpt[0][0]=newfrag->textboxpt[1][0];
            newfrag->textboxpt[1][0]=ar1;
        }
        if (newfrag->textboxpt[0][1]>newfrag->textboxpt[1][1]) {
            ar1=newfrag->textboxpt[0][1];
            newfrag->textboxpt[0][1]=newfrag->textboxpt[1][1];
            newfrag->textboxpt[1][1]=ar1;
        }
    }

    aidx=0;  /* Important.  Start filling astr from the beginning again. */

out:
    *aidxp=aidx;

    if (rc) {
        if (newfrag!=NULL) {
            if (newfrag->tgpar.val!=NULL) IC_FREE(newfrag->tgpar.val);
            delete newfrag; newfrag=NULL;
        }
    }

    /* Set *newfragpp (but free old first if it never got linked in): */
    if (*newfragpp!=NULL) {
        if ((*newfragpp)->tgpar.val!=NULL) IC_FREE((*newfragpp)->tgpar.val);
        delete *newfragpp;
    }
    *newfragpp=newfrag;

    return rc;
}


int gr_getftextfrags(
	db_tolerance	  *pTolerance,
    char               *ftbuffer,     /* The formatted text */
    gr_ftextfraglink  **ftfragsllpp,  /* Addr of ptr for the llist of frags */
    gr_textgenparslink *defparp,      /* The default parameters */
    int                 defcolor,     /* The default text color */
    gr_view            *viewp,        /* The view data (including .ment) */
	CDC				   *dc,			  /* Device context in case we have TrueType text */
    db_drawing         *dp) {
/*
**  An extension of gr_tol2doll() (and therefore of gr_getdispobjs())
**  for interpreting MTEXT and TOLERANCE formatted text.
**
**  Reads a buffer of text formatted for MTEXT or TOLERANCE (ftbuffer)
**  and creates a llist of fragments (lfragsll) such that each fragment
**  has a consistent set of text parameters (and a textbox).
**  The default parameters are in *defparp.  viewp->ment is the entity
**  being generated (and viewp has the current view data).
**
**  THE CALLER MUST FREE *ftfragsllpp IF THIS FUNCTION SETS IT TO NON-NULL.
**  (Should be no problem, since gr_tol2doll() is expected to be
**  the only caller, currently.)
**
**  Returns: gr_getdispobjs() codes.
*/
    char *astr,boxended,rowended,paragraphended,takechar,istol,fc1;
    int astrsz,aidx,done,ftidx,rc,fi1;
    sds_real ar1;

    /* Llist of fragments: */
    gr_ftextfraglink *fragsll[2],*newfrag;

    /* A stack of text parameter settings: */
    gr_fragstacklink *stk,*tstkp1;

    #define MAKEFRAG if ((rc=gr_makefrag(pTolerance,&newfrag,stk,astr,&aidx,paragraphended,rowended,boxended,viewp,dc,dp))!=0) goto out


    rc=0; stk=NULL; fragsll[0]=fragsll[1]=newfrag=NULL; astr=NULL;


    /* Start the allocated string for collecting characters: */
    astrsz=IC_ACADBUF;    
	if ((astr= new char [astrsz])==NULL) { rc=-3; goto out; }

    /* Start stk with the defaults.  (The top link will always */
    /* be the current settings.) */
    if ((stk= new gr_fragstacklink )==NULL)
        { rc=-3; goto out; }
	memset(stk,0,sizeof(gr_fragstacklink));
    memcpy(&stk->tgpar,defparp,sizeof(stk->tgpar));
    stk->tgpar.val=NULL;  /* Precaution */
    stk->color=defcolor;

    istol=(viewp->ment->ret_type()==DB_TOLERANCE);

    aidx=0;
    for (ftidx=done=0; !done /* See case 0 */; ftidx++) {  /* Walk formatted text */

      boxended=rowended=paragraphended=takechar=0;

      switch (ftbuffer[ftidx]) {

        case 0:  /* End of ftbuffer; do final frag. */

          done=1; MAKEFRAG;

          break;

        case '{' :

          /* Push current parameter settings by copying the top link */
          /* adding the copy to the top: */

           if ((tstkp1= new gr_fragstacklink)==NULL)
              { rc=-3; goto out; }
          memcpy(tstkp1,stk,sizeof(*tstkp1));
          tstkp1->tgpar.val=NULL;  /* Precaution */
          tstkp1->next=stk; stk=tstkp1;

          /* No parameters changed. */

          break;

        case '}' :

          /* Pop the parameters stack by deleting the top link: */

          if (stk->next!=NULL) {
            /* Were going to compare stk and stk->next, so make their */
            /* "next" ptrs agree so that THEY aren't the discrepancy: */
            tstkp1=stk->next; stk->next=tstkp1->next;
            fi1=memcmp(stk,tstkp1,sizeof(*stk));
            stk->next=tstkp1;
            if (fi1) {
                MAKEFRAG;
                tstkp1=stk; stk=stk->next;
                delete tstkp1;  /* tstkp1->tgpar.val not used */
            }
          }

          break;

        case '%':

          if (ftbuffer[ftidx+1]=='%' && ftbuffer[ftidx+2]=='v' && istol) {
            /* Start a new TOLERANCE box. */

            ftidx+=2; boxended=1;
            MAKEFRAG;

            /* Reset stk to defaults by freeing all links except the deepest, */
            /* and setting its values back to the defaults: */
            while (stk!=NULL && stk->next!=NULL)
                { tstkp1=stk->next; delete stk; stk=tstkp1; }
            memset(stk,0,sizeof(*stk));
            memcpy(&stk->tgpar,defparp,sizeof(stk->tgpar));
            stk->tgpar.val=NULL;  /* Precaution */
            stk->color=defcolor;
          } else takechar=1;

          break;

        case '\n':

          if (istol) {
            rowended=1;
            MAKEFRAG;

            /* Reset stk to defaults by freeing all links except the deepest, */
            /* and setting its values back to the defaults: */
            while (stk!=NULL && stk->next!=NULL)
                { tstkp1=stk->next; delete stk; stk=tstkp1; }
            memset(stk,0,sizeof(*stk));
            memcpy(&stk->tgpar,defparp,sizeof(stk->tgpar));
            stk->tgpar.val=NULL;  /* Precaution */
            stk->color=defcolor;
          }
          /* No effect for MTEXT.  Skip. */

          break;

        case '\\' :

          switch (ftbuffer[ftidx+1]) {

            case 'A':  /* Used in DIMENSIONS */

              /* TODO: Don't wrap? */
              /* For now, just skip numeric field and semicolon: */

              ftidx+=2;  /* 1 past the 'A' */
              while (ftbuffer[ftidx] && isdigit((unsigned char) ftbuffer[ftidx])) ftidx++;
              if (ftbuffer[ftidx]!=';') ftidx--;  /* ';' or last digit */

              break;

            case 'C':  /* Color */

              ftidx+=2;  /* 1 past the 'C' */
              fi1=atoi(ftbuffer+ftidx);
              if (fi1<DB_BBCOLOR || fi1>DB_BLCOLOR) fi1=DB_BLCOLOR;
              if      (fi1==DB_BLCOLOR) fi1=viewp->blcolor;
              else if (fi1==DB_BBCOLOR) fi1=viewp->bbcolor;

              while (ftbuffer[ftidx] && isdigit((unsigned char) ftbuffer[ftidx])) ftidx++;
              if (ftbuffer[ftidx]!=';') ftidx--;  /* ';' or last digit */

              if (fi1!=stk->color) {
                MAKEFRAG;
                stk->color=fi1;
              }

              break;

            case 'F':  /* Font */

              {
                int wanttolfont,begidx;
                db_fontlink *tfp=NULL;


                begidx=(ftidx+=2);  /* Beginning of the pathname. */

                /* Set fi1 to 1st char of fname, and ftidx to the dot */
                /* (or where the dot WOULD be): */
                while (ftbuffer[ftidx] && ftbuffer[ftidx]!=';') ftidx++;
                for (fi1=ftidx; fi1>=begidx && ftbuffer[fi1]!=IC_SLASH; fi1--)
                    if (ftbuffer[fi1]=='.') ftidx=fi1;
                fi1++;

                /* Looking for the TOLERANCE font? */
                fc1=ftbuffer[ftidx]; ftbuffer[ftidx]=0;
                wanttolfont=(strisame(ftbuffer+fi1,gr_tolfontnm));
                ftbuffer[ftidx]=fc1;

                /* Set ftidx one char beyond the end (';' or '0'): */
                while (ftbuffer[ftidx] && ftbuffer[ftidx]!=';') ftidx++;

                /* Set the font and/or get a ptr to it: */
                fc1=ftbuffer[ftidx]; ftbuffer[ftidx]=0;
                rc=db_setfontusingmap(ftbuffer+begidx,NULL,&tfp,IC_NO_ALLOW_ICAD_FNT,dp);
                ftbuffer[ftidx]=fc1;
                if (rc==-3) goto out;
                /*
                **  If we got any other error (<0) -- or we were looking
                **  for the TOLERANCE font and got the default (+1) --
                **  set tfp to NULL and clear rc:
                */
                if (rc<0 || (wanttolfont && rc==+1)) { tfp=NULL; rc=0; }

                /* If ftidx is on the '0', back it up to the last char: */
                if (!ftbuffer[ftidx]) ftidx--;

                /* If font is changing, take the frag and update the */
                /* current font setting: */
                if (tfp!=stk->tgpar.font) {
                  MAKEFRAG;
                  stk->tgpar.font=tfp;
                }
              }

              break;

            case 'H':  /* Text height or relative factor */

              ftidx+=2;  /* 1 past the 'H' */
              ar1=fabs(atof(ftbuffer+ftidx));

              while (ftbuffer[ftidx] && ftbuffer[ftidx]!=';') ftidx++;
              if (!ftbuffer[ftidx]) ftidx--;  /* ';' or last char in ftbuffer */

              if (ftbuffer[ftidx]==';' && toupper(ftbuffer[ftidx-1])=='X')
                ar1*=stk->tgpar.ht;
              if (!icadRealEqual(ar1,0.0) && ar1!=stk->tgpar.ht) {
                MAKEFRAG;
                stk->tgpar.ht=ar1;
              }

              break;

            case 'L':

              ftidx++;  /* On the L */
              if (!stk->underline) {
                MAKEFRAG;
                stk->underline=1;
              }

              break;

            case 'l':

              ftidx++;  /* On the l */
              if (stk->underline) {
                MAKEFRAG;
                stk->underline=0;
              }

              break;

            case 'O':

              ftidx++;  /* On the O */
              if (!stk->overline) {
                MAKEFRAG;
                stk->overline=1;
              }

              break;

            case 'o':

              ftidx++;  /* On the o */
              if (stk->overline) {
                MAKEFRAG;
                stk->overline=0;
              }

              break;

            case 'P':

              ftidx++;  /* On the P */
              paragraphended=1;
              MAKEFRAG;

              break;

            case 'Q':  /* Oblique angle (in deg) */

              ftidx+=2;  /* 1 past the 'Q' */
              ar1=fabs(atof(ftbuffer+ftidx));

              while (ftbuffer[ftidx] && ftbuffer[ftidx]!=';') ftidx++;
              if (!ftbuffer[ftidx]) ftidx--;  /* ';' or last char in ftbuffer */

              ar1*=IC_PI/180.0; ic_normang(&ar1,NULL);

              if (ar1!=stk->tgpar.obl) {
                MAKEFRAG;
                stk->tgpar.obl=ar1;
              }

              break;

            case 'S':  /* Stacked */

              /* TODO: Not now, Kato.  Just walk to the semicolon. */

              while (ftbuffer[ftidx] && ftbuffer[ftidx]!=';') ftidx++;
              if (!ftbuffer[ftidx]) ftidx--;  /* ';' or last char in ftbuffer */

              break;

            case 'W':  /* Width factor */

              ftidx+=2;  /* 1 past the 'W' */
              ar1=fabs(atof(ftbuffer+ftidx));

              while (ftbuffer[ftidx] && ftbuffer[ftidx]!=';') ftidx++;
              if (!ftbuffer[ftidx]) ftidx--;  /* ';' or last char in ftbuffer */

              if (ar1<0.0) ar1=-ar1;

              if (ar1>0.0 && ar1!=stk->tgpar.wfact) {
                MAKEFRAG;
                stk->tgpar.wfact=ar1;
              }

              break;

            default: takechar=1; break;  /* Take the backslash. */

          }  /* End switch char after backslash. */

          break;

        default: takechar=1; break;  /* Take normal char */

      }  /* End switch (ftbuffer[ftidx]) */

      if (takechar) {
        if (aidx>=astrsz) {
          astrsz+=IC_ACADBUF;
          if ((astr=ic_realloc_char(astr,astrsz))==NULL) { rc=-3; goto out; }
        }
        astr[aidx++]=ftbuffer[ftidx];
      }

      if (newfrag!=NULL) {
        if (fragsll[0]==NULL) fragsll[0]      =newfrag;
        else                  fragsll[1]->next=newfrag;
        fragsll[1]=newfrag;
        newfrag=NULL;  /* Important */
      }

    }  /* End for each ftbuffer char */



out:
    if (astr!=NULL) IC_FREE(astr);

    /* Free the parameter stack.  Note that the ->tgpar.val elements */
    /* were never used and therefore don't need freeing. */
    while (stk!=NULL) { tstkp1=stk->next; delete stk; stk=tstkp1; }

    /* Free newfrag if it's allocated and never got added to fragsll: */
    if (newfrag!=NULL) {
        if (newfrag->tgpar.val!=NULL) IC_FREE(newfrag->tgpar.val);
        delete newfrag; newfrag=NULL;
    }

    if (rc) {
        while (fragsll[0]!=NULL) {
            if (fragsll[0]->tgpar.val!=NULL) IC_FREE(fragsll[0]->tgpar.val);
            fragsll[1]=fragsll[0]->next; delete fragsll[0];
            fragsll[0]=fragsll[1];
        }
        fragsll[0]=fragsll[1]=NULL;
    }

    if (ftfragsllpp!=NULL) *ftfragsllpp=fragsll[0];

    return rc;

    #undef MAKEFRAG
}


int gr_gentolline(
	db_tolerance	  *pTolerance,
    sds_point         *tolpt,     /* Array of two TOLCS pts */
    sds_point          ecsipt,    /* The ECS TOLERANCE insertion pt */
    sds_real           xdrot_cs,  /* Sine   of TOLERANCE rotation in the ECS */
    sds_real           xdrot_sn,  /* Cosine of TOLERANCE rotation in the ECS */
    int                linecolor, /* From DIMSTYLE or EED override */
    gr_view           *viewp,     /* Current view */
	db_drawing* pDrawing,
	CDC* pDC,
    gr_displayobject **doll       /* Array of two disp obj ptrs for the llist */
    ) {
/*
**  This is an extension of gr_tol2doll(); nothing else should call it.
**  It generates the disp objs for one line in the TOLERANCE line-work.
**
**  A lot of the work (including setting up the point chain (pc))
**  has been done already by gr_tol2doll().
**
**  Returns: gr_getdispobjs() codes.
*/
    int savcolor,rc,fi1;
    sds_point ecspt[2];
    struct gr_displayobject *tbegdo,*tenddo;


    rc=0;

    /* Set viewp->color to linecolor TEMPORARILY; fix before leaving. */
    savcolor=viewp->color;
    viewp->color=linecolor;

    /* Rotate both pts about TOLCS origin and translate to ecsipt */
    /* to get the ECS pts: */
    for (fi1=0; fi1<2; fi1++) {
        ecspt[fi1][0]=
            tolpt[fi1][0]*xdrot_cs-tolpt[fi1][1]*xdrot_sn+ecsipt[0];
        ecspt[fi1][1]=
            tolpt[fi1][1]*xdrot_cs+tolpt[fi1][0]*xdrot_sn+ecsipt[1];
    }

    ecspt[0][2]=ecspt[1][2]=ecsipt[2];  /* Set Z same as ins pt. */

    /* Put them into the pc: */
    if (viewp->ltsclwblk) {
        GR_PTCPY(viewp->pc->pt      ,ecspt[0]);
        GR_PTCPY(viewp->pc->next->pt,ecspt[1]);
    } else {
        gr_ncs2ucs(ecspt[0],viewp->pc->pt      ,0,viewp);
        gr_ncs2ucs(ecspt[1],viewp->pc->next->pt,0,viewp);
    }

    /* Set d2go: */
    viewp->pc->d2go=GR_DIST(viewp->pc->pt,viewp->pc->next->pt);

    /* Make the disp objs (with ltype): */
    if ((rc=gr_doltype( pTolerance, &tbegdo,&tenddo,viewp,pDrawing,pDC))!=0) goto out;

    /* Tack 'em on: */
    if (tbegdo!=NULL && tenddo!=NULL) {
        if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
        doll[1]=tenddo;
    }

out:
    viewp->color=savcolor;

    return rc;
}


int gr_tol2doll(
	db_tolerance			 *pTolerance,
    struct gr_displayobject **begdopp,
    struct gr_displayobject **enddopp,
    gr_view           *viewp,
	CDC						 *dc,
    db_drawing               *dp) {
/*
**  An extension of gr_getdispobjs().
**  Allocates a display objects llist for the TOLERANCE currently being
**  created via gr_textgen(). *begdopp is pointed to the first link;
**  *enddopp is pointed to the last link.
**
**  One TOLERANCE has rows.  Each row has boxes.  Each box has text
**  fragments where the text parameters are constant.
**
**  The (simplified) strategy is as follows:
**
**    Get the default text parameters from the DIMSTYLE and its
**      DIMTXSTY.
**
**    Break the formatted text into a llist of fragments such that
**      each text fragment has a set of text parameters that are
**      valid for the whole fragment.
**
**    Use the fragments' textboxes to draw the TOLERANCE boxes and
**      fill them with the proper text.
**
**  Minimal error-checking is done here; that's up to gr_getdispobjs().
**
**  Returns: gr_getdispobjs() codes.
**
** History
** EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight DIMLWD(371)
** 01/04/2003 Tolerance color and Lineweight is the below...
** It is AutoCAD 2002 behavier. But I believe Additional mark shows as TOLERANCE LW.
**                   | COLOR            |LINEWEIGHT     |NOTE
**  Main Mark        | TOLERANCE COLOR  |TOLERANCE LW   |It's all okay.
**  Addiotional Mark | TOLERANCE COLOR  |DIMSTYLE DIMLWD|Lweight should be same as Main.
**  Frame            | DIMSTYLE DIMCLRD |DIMSTYLE DIMLWD|It's okay. In A2K Property window 
**                                                       can modify DIMCLRD for TOLERANCE.
*/
    char *fcp1;
    int linecolor,textcolor,entcolor,ridx,bidx,fidx,nrows,rc,fi1;
    sds_real gap,spacingfact,dimscale,ar1,ar2,ar3;
    sds_point ecsipt,tolpt[2],ap1;
    db_handitem *dship,*tship,*thip1;
    struct resbuf *eedrbll,appacadrb,*trbp1;
    gr_textgenparslink defpar;
    gr_ftextfraglink  *ftfragsll,*lofrag,*hifrag,*tfragp1,*tfragp2;
    gr_displayobject *doll[2],*tbegdo,*tenddo;
	int lweight,tlweight,savlweight;

    /*
    **  The following is for row and box data in the TOLERANCE Coordinate System
    **  (TOLCS), whose origin is the midpoint of the left edge of the
    **  first row and whose axes are parallel to the box edges.
    */
    struct boxelem {
        sds_real           right;    /* Right edge's X (1st box's X is 0.0) */
        sds_real           baseline; /* Text baseline's Y */
        gr_ftextfraglink **frag;     /* Array of ptrs to frags in this box */
                                     /*   (pointing into ftfragsll). */
        int                nfrags;   /* # of elements in frag[] */
    };
    struct rowelem {
        sds_real  bottom;  /* Bottom edge's Y (top row's top is -bottom) */
        boxelem  *box;     /* Array of boxes in this row */
        int       nboxes;  /* # of elements in box[] */
    } *row;  /* An array of rows in this TOLERANCE. */

    #define MAKELINE if ((rc=gr_gentolline(pTolerance,tolpt,ecsipt,xdrot_cs,xdrot_sn,linecolor,viewp,dp,dc,doll))!=0) goto out


    rc=0; eedrbll=NULL; dship=tship=NULL; ftfragsll=NULL;
    gap=0.0; linecolor=textcolor=-1; entcolor=DB_BLCOLOR; row=NULL; nrows=0;
    doll[0]=doll[1]=NULL;
    savlweight=viewp->lweight; lweight=DB_BBWEIGHT; pTolerance->get_370(&tlweight);

	// moved by Denis Petrov from gr_getdispobjs()
// MR 9/30/98
//	I disagree with the analysis below.  The actual fact is that the
//	xdir vector is simply stored in the WCS rather than the entity's ECS.
//	I solved this problem in OpenDWG Viewkit by getting the transformation
//	matrix from the extrusion, inverting it, and then transforming the
//	x direction vector using that matrix, thus putting it into the ECS (and
//	therefore the z coordinate is 0.0).
//	At that point you can simply find the rotation angle as you would
//	with any 2D vector.
//
//	However, that is not what is done here.  It is a bit difficult at
//	this point to evaluate how painful it would be to simply pull this
//	code and replace it with something else.

/*
**  Some new R13 planar entities have WCS points, an extrusion
**  direction vector, and an X-direction vector -- and the
**  extrusion is not necessarily normal to the X-direction.
**  ACAD seems to get the TRUE plane normal (extrusion vector)
**  by crossing the extrusion with the xdir to get the ydir,
**  and then crossing the xdir with the ydir to get the true
**  extrusion direction.  (Yep.  For displaying, ACAD corrects
**  the extrusion direction, not the xdir; I checked.)
**
**  We need to set up entaxis for these entities as if they
**  are ECS entities (at least for MTEXT and TOLERANCE, so
**  that we can use gr_textgen() to get the display objects).
	*/
	
    sds_point entaxis[3];
	sds_point xdxdir;
    sds_real xdrot, xdrot_cs, xdrot_sn;
	/* Get the "given" unit zdir (entaxis[2]) and unit xdir (xdxdir): */
	pTolerance->get_210(entaxis[2]);
	ar1 = GR_LEN(entaxis[2]);
	if(icadRealEqual(ar1,0.0))
	{
		entaxis[2][0] = 0.0;
		entaxis[2][1] = 0.0;
		entaxis[2][2] = 1.0;
	}
	else 
		if (!icadRealEqual(ar1,1.0))
			GR_UNITIZE(entaxis[2],ar1);
		
        viewp->ment->get_11(xdxdir);
		ar1 = GR_LEN(xdxdir);
        if(icadRealEqual(ar1,0.0))
		{
			xdxdir[0] = 1.0;
			xdxdir[1] = 0.0;
			xdxdir[2] = 0.0;
		}
        else 
			if(!icadRealEqual(ar1,1.0))
				GR_UNITIZE(xdxdir,ar1);
			
	// DP: I wanna check is this true what
	// DP: 'the extrusion is not necessarily normal to the X-direction'
	// DP: (see comments above)
	assert(fabs(ic_dotproduct(entaxis[2], xdxdir)) < .1e-10 
		&& "Please send this file to ITC developers team.");
			
	/* Determine the true unit ydir: */
	ic_crossproduct(entaxis[2],xdxdir,ap1);
	if (icadRealEqual((ar1=GR_LEN(ap1)),0.0))
	{
		/* Oops! The 2 given vectors are parallel.  Invent a new */
		/* xdir normal to the zdir (almost the arbitrary axis algorithm): */
		
		/* Find idx of smallest abs coord of zdir: */
		fi1 = (fabs(entaxis[2][1]) < fabs(entaxis[2][0]));
		if(fabs(entaxis[2][2]) < fabs(entaxis[2][fi1]))
			fi1 = 2;
		xdxdir[fi1] = 0.0;
		if(fi1 == 0)
		{
			xdxdir[1] = -entaxis[2][2];
			xdxdir[2] = entaxis[2][1];
		}
		else 
		{
			if(fi1 == 1)
			{
				xdxdir[0] = -entaxis[2][2];
				xdxdir[2] = entaxis[2][0];
			}
			else
			{
				xdxdir[0] = -entaxis[2][1];
				xdxdir[1] = entaxis[2][0];
			}
		}
		ar1 = GR_LEN(xdxdir);
		if(!icadRealEqual(ar1,0.0) && !icadRealEqual(ar1,1.0))
			GR_UNITIZE(xdxdir,ar1);
		
		/* NOW set the true unit ydir: */
		ic_crossproduct(entaxis[2], xdxdir, ap1);
		ar1=GR_LEN(ap1);
		if (icadRealEqual(ar1,0.0))
			// Should never, but... 
			ap1[1]=1.0; ap1[0]=ap1[2]=0.0;
	} // ar1=GR_LEN(ap1)==0.0
	
	if(!icadRealEqual(ar1,0.0) && !icadRealEqual(ar1,1.0))
		GR_UNITIZE(ap1, ar1);
	
	/* Determine the true unit zdir: */
	ic_crossproduct(xdxdir, ap1, entaxis[2]);
	int extruok=(!icadRealEqual(entaxis[2][0],0.0) || 
				 !icadRealEqual(entaxis[2][1],0.0) || 
				 !icadRealEqual(entaxis[2][2],1.0));

    if (extruok)
		{
        if (icadRealEqual((ar1=GR_LEN(entaxis[2])),0.0))
			{
            extruok=0;
			}
		else
			{
            if (!icadRealEqual(ar1,1.0))
				{
				GR_UNITIZE(entaxis[2],ar1);
				}

            if (icadRealEqual(entaxis[2][2],1.0))
				{
                extruok=0;
				}
			else
				{
                if (ic_arbaxis(entaxis[2],entaxis[0],entaxis[1],entaxis[2])==-1)
					extruok=0; // shouldn't happen
				}
	        }
		} // end of if (extruok)


    if (!extruok)
		{  /* DON'T MAKE THIS AN "else" TO THE ONE ABOVE! */
        ic_identity(entaxis);
	    }

	/* Determine the rotation that xdxdir implies in the entaxis system: */
	xdrot_cs=ic_dotproduct(xdxdir,entaxis[0]);  /* cos */
	ic_crossproduct(entaxis[0],xdxdir,ap1);
	xdrot_sn=GR_LEN(ap1);             /* fabs(sin) */
	if (ic_dotproduct(ap1,entaxis[2])<0.0)
	{
		xdrot_sn=-xdrot_sn;  /* sin */
	}
	xdrot=atan2(xdrot_sn,xdrot_cs);
	// end of movement
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
	CMatrix4 insertTransformation(viewp->m_transformation);
	viewp->m_transformation = viewp->m_transformation * entityTransformation;

    /*
    **  When determining spacing between internal frags in a TOL box,
    **  we're currently using a factor that will be applied to the
    **  text height of the left frag:
    */
    spacingfact=0.2;

    /* Init defpar (the default (initial) text parameters): */
    memset(&defpar,0,sizeof(defpar));
    defpar.ht=0.2; defpar.wfact=1.0;
    defpar.rot=xdrot;
    defpar.absxsclp=viewp->absxsclp;

    /* Set the ACAD app name (and never relrb it): */
    appacadrb.rbnext=NULL;
    appacadrb.restype=RTSTR;
    appacadrb.resval.rstring="ACAD";

    /* Set DIMSTYLE rec ptr: */
    viewp->ment->get_3(&dship);
    /* If NULL, try for STANDARD: */
    if (dship==NULL) dship=dp->findtabrec(DB_DIMSTYLETAB,gr_str_standard,1);


                 /* SET VALUES FROM THE DIMSTYLE: */

              /* OVERRIDE DIMSTYLE VALUES USING EED */

	defpar.ht = 0;
	tship=NULL;
    if (dship!=NULL) {
        dship->get_40(&dimscale);
		dship->get_140(&defpar.ht);
        dship->get_147(&gap);
        dship->get_176(&linecolor);
        dship->get_178(&textcolor);
        dship->get_340(&tship);
        dship->get_371(&lweight);
    }

              /* OVERRIDE DIMSTYLE VALUES USING EED */

    eedrbll=viewp->ment->get_eed(&appacadrb,NULL);

    trbp1=eedrbll; while (trbp1!=NULL && (trbp1->restype!=1000 ||
        stricmp(trbp1->resval.rstring,"DSTYLE"))) trbp1=trbp1->rbnext;
    /* Matching { for the closing one below */
    for (; trbp1!=NULL && (trbp1->restype!=1002 ||
        *trbp1->resval.rstring!='}'); trbp1=trbp1->rbnext) {

        if (trbp1->restype!=1070 || trbp1->rbnext==NULL) continue;

        fi1=trbp1->resval.rint; trbp1=trbp1->rbnext;
        switch (fi1) {
            case 40:
                if (trbp1->restype!=1040) break;
				dimscale=trbp1->resval.rreal;
                break;
            case 140:
                if (trbp1->restype!=1040) break;
				defpar.ht=trbp1->resval.rreal;
                break;
            case 147:
                if (trbp1->restype!=1040) break;
                gap=trbp1->resval.rreal;
                break;
            case 176:
                if (trbp1->restype!=1070) break;
                linecolor=trbp1->resval.rint;
                break;
            case 178:
                if (trbp1->restype!=1070) break;
                textcolor=trbp1->resval.rint;
                break;
            case 340:
                if (trbp1->restype!=1005) break;
                if ((thip1=dp->handent(trbp1->resval.rstring))!=NULL &&
                    thip1->ret_type()==DB_STYLETABREC) tship=thip1;
                break;
            case 371:
                if (trbp1->restype!=1070) break;
                lweight=trbp1->resval.rint;
                break;
        }
    }


	if (tship) {
		sds_real tempht;
		(db_styletabrec *)tship->get_40(&tempht);
		if (!icadRealEqual(tempht,0.0)) defpar.ht=tempht;
	}


    /* Check ranges: */
    if (icadRealEqual(dimscale,0.0)) dimscale=1.0; else if (dimscale<0.0) dimscale=-dimscale;
    if (icadRealEqual(defpar.ht,0.0)) defpar.ht=0.2; else if (defpar.ht<0.0) defpar.ht=-defpar.ht;
    if (gap<0.0) gap=-gap;

    /* Scale the sizes by dimscale: */
	//BugZilla No. 77888; 08-06-2002
    defpar.ht*=dimscale;
    gap*=dimscale;


    /*
    **  Resolve the colors.  ACAD seems to use an special, undocumented
    **  rule: If the DIMSTYLE (or the EED overrides, presumably) say
    **  BYBLOCK, use the color of the TOLERANCE entity itself.
    */
    viewp->ment->get_62(&entcolor);
    if (linecolor==DB_BBCOLOR) linecolor=entcolor;
    if (linecolor<DB_BBCOLOR || linecolor>DB_BLCOLOR) linecolor=DB_BLCOLOR;
    if      (linecolor==DB_BLCOLOR) linecolor=viewp->blcolor;
    else if (linecolor==DB_BBCOLOR) linecolor=viewp->bbcolor;
    if (textcolor==DB_BBCOLOR) textcolor=entcolor;
    if (textcolor<DB_BBCOLOR || textcolor>DB_BLCOLOR) textcolor=DB_BLCOLOR;
    if      (textcolor==DB_BLCOLOR) textcolor=viewp->blcolor;
    else if (textcolor==DB_BBCOLOR) textcolor=viewp->bbcolor;

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	if( lweight == DB_BBWEIGHT )
		viewp->ment->get_370(&lweight);
	if( lweight == DB_BBWEIGHT )
		lweight = viewp->bblweight;
	if( lweight == DB_BLWEIGHT )
		lweight = viewp->bllweight;
	if(!db_is_validweight(lweight, DB_DFWMASK))
		lweight = DB_DFWEIGHT;
	//viewp->lweight = lweight;	// Set later.. 2003/4/1
	// for mark and tolerance
	if( tlweight == DB_BBWEIGHT )
		tlweight = viewp->bblweight;
	if( tlweight == DB_BLWEIGHT )
		tlweight = viewp->bllweight;
	if(!db_is_validweight(tlweight, DB_DFWMASK))
		tlweight = DB_DFWEIGHT;
	// EBATECH(CNBR) ]-

        /* SET TEXT PARAMETER DEFAULTS FROM THE TEXT STYLE */

    /* If STYLE not set, try for STANDARD: */
    if (tship==NULL) tship=dp->findtabrec(DB_STYLETAB,gr_str_standard,1);

    /* defpar.ht       set earlier from DIMSTYLE (140). */
    /* defpar.rot      set earlier from xdrot. */
    /* defpar.absxsclp set earlier from viewp. */
    if (tship!=NULL) {
        tship->get_71(&defpar.gen);
        tship->get_70(&defpar.vert); defpar.vert=((defpar.vert&0x04)!=0);
        tship->get_41(&defpar.wfact);
        tship->get_50(&defpar.obl);
        defpar.font   =tship->ret_font();
        defpar.bigfont=tship->ret_bigfont();
    }
    if (defpar.font==NULL) defpar.font=dp->ret_fontll();  /* Last ditch */


                   /* GET THE LLIST OF FRAGMENTS */

    viewp->ment->get_1(&fcp1);
    if (fcp1==NULL || !*fcp1) goto out;  /* Blank? */
    if ((rc=gr_getftextfrags( pTolerance, fcp1,&ftfragsll,&defpar,textcolor,viewp,dc,dp))!=0)
        goto out;

    /*
    **  Some of the frags may be blank, containing only end-of-row or
    **  end-of-box markers.  Compress out each blank frag, copying its
    **  end-of-row/end-of-box markers to the frag just before it.  Use
    **  flexible loop control; tfragps may get reset to NULL.
    */
    tfragp1=tfragp2=NULL;
    for (;;) {
        if (tfragp2==NULL) { tfragp1=NULL   ; tfragp2=ftfragsll;     }
        else               { tfragp1=tfragp2; tfragp2=tfragp2->next; }
        if (tfragp2==NULL) break;

        if (tfragp2->tgpar.val!=NULL && *tfragp2->tgpar.val) continue;

        /* tfragp2 is a blank one if we got to here. */

        /* Assign end-of stuff to prev frag: */
        if (tfragp1!=NULL) {
            if      (tfragp2->eotolrow) tfragp1->eotolrow=1;
            else if (tfragp2->eotolbox) tfragp1->eotolbox=1;
        }

        /* Remove tfragp2's link: */
        if (tfragp1==NULL) ftfragsll=tfragp2->next;
        else           tfragp1->next=tfragp2->next;
        if (tfragp2->tgpar.val!=NULL) IC_FREE(tfragp2->tgpar.val);
        delete tfragp2;

        /* Prepare to step to next link to check by pointing to prev: */
        tfragp2=tfragp1;  /* May be NULL; that's okay; see the stepper. */
    }

    if (ftfragsll==NULL) goto out;


#if defined(GR_DUMPTOL)
    {
        FILE *fptr;

        if ((fptr=fopen("c:\\t\\----test.chk"/*DNT*/,"at"/*DNT*/))!=NULL) {
            fputs("\n\n***********************\n\n",fptr);

            for (tfragp1=ftfragsll; tfragp1!=NULL; tfragp1=tfragp1->next) {
                fprintf(fptr,"\n     tgpar.val : |%s|\n",(tfragp1->tgpar.val==NULL) ? "NULL" : tfragp1->tgpar.val);
                fprintf(fptr,  "tgpar.font->pn : |%s|\n",(tfragp1->tgpar.font==NULL || tfragp1->tgpar.font->pn==NULL) ? "NULL" : tfragp1->tgpar.font->pn);
                fprintf(fptr,  "         color : %d\n",tfragp1->color);
                fprintf(fptr,  "        eopara : %d\n",tfragp1->eopara);
                fprintf(fptr,  "      eotolrow : %d\n",tfragp1->eotolrow);
                fprintf(fptr,  "      eotolbox : %d\n",tfragp1->eotolbox);
                fprintf(fptr,  "     textboxpt : (%G,%G,%G) (%G,%G,%G)\n",
                    tfragp1->textboxpt[0][0],
                    tfragp1->textboxpt[0][1],
                    tfragp1->textboxpt[0][2],
                    tfragp1->textboxpt[1][0],
                    tfragp1->textboxpt[1][1],
                    tfragp1->textboxpt[1][2]);
            }

            fclose(fptr);
        }
    }
#endif


           /* GET THE DATA CONCERNING THE ROWS AND BOXES */


    /*
    **  Note that to avoid confusion with eotolbox and eotolrow,
    **  eotolrow has precedence.  We check for end-of-row with
    **  (lastfrag || eotolrow).  We check for end-of-box with
    **  (lastfrag || eotolrow || eotolbox).
    */

    /* Count the rows: */
    nrows=0;
    for (tfragp1=ftfragsll; tfragp1!=NULL; tfragp1=tfragp1->next)
        if (tfragp1->eotolrow || tfragp1->next==NULL) nrows++;

    /* Alloc row[]: */
    if ((row= new rowelem [nrows])==NULL)
        { rc=-3; goto out; }
	memset(row,0,sizeof(rowelem)*nrows);

    /* For each row, count the boxes and alloc them: */
    ridx=0;
    for (tfragp1=ftfragsll; tfragp1!=NULL; tfragp1=tfragp1->next) {
        if (tfragp1->eotolbox || tfragp1->eotolrow || tfragp1->next==NULL) {
            if (ridx<nrows) row[ridx].nboxes++;
            if (tfragp1->eotolrow || tfragp1->next==NULL) {
                if (ridx<nrows && (row[ridx].box= new boxelem [row[ridx].nboxes] )==NULL) { rc=-3; goto out; }
				memset(row[ridx].box,0,sizeof(boxelem)*row[ridx].nboxes);
                ridx++;
            }
        }
    }

    /* For each box, count the frags and alloc the frag[] array: */
    ridx=bidx=0;
    for (tfragp1=ftfragsll; tfragp1!=NULL; tfragp1=tfragp1->next) {
        if (ridx<nrows && bidx<row[ridx].nboxes) row[ridx].box[bidx].nfrags++;
        if (tfragp1->eotolbox || tfragp1->eotolrow || tfragp1->next==NULL) {
            if (ridx<nrows && bidx<row[ridx].nboxes &&
                (row[ridx].box[bidx].frag= new gr_ftextfraglink *[row[ridx].box[bidx].nfrags])==NULL) { rc=-3; goto out; }
			memset(row[ridx].box[bidx].frag,0,sizeof(gr_ftextfraglink *)*row[ridx].box[bidx].nfrags);
            bidx++;
            if (tfragp1->eotolrow || tfragp1->next==NULL) { ridx++; bidx=0; }
        }
    }

    /* Assign the frags to their boxes: */
    ridx=bidx=fidx=0;
    for (tfragp1=ftfragsll; tfragp1!=NULL; tfragp1=tfragp1->next) {
        if (ridx<nrows && bidx<row[ridx].nboxes &&
            fidx<row[ridx].box[bidx].nfrags)
                row[ridx].box[bidx].frag[fidx]=tfragp1;
        fidx++;
        if (tfragp1->eotolbox || tfragp1->eotolrow || tfragp1->next==NULL) {
            bidx++; fidx=0;
            if (tfragp1->eotolrow || tfragp1->next==NULL) { ridx++; bidx=0; }
        }
    }

    /*
    **  We should now have the row[] array set up such that there is
    **  at least one row, every row has at least one box, and
    **  every box has at least one fragment.  The code below this
    **  point will rely on this.
    */

    /* Scan row[] and set the distance elements for the rows and boxes: */
    for (ridx=0; ridx<nrows; ridx++) {

        if (row[ridx].nboxes<1) continue;  /* No boxes (precaution) */

        /* First, let's find the Y of the bottom edge for this row */
        /* (row[ridx].bottom).  We do it by finding the largest vertical */
        /* textbox range in any one box in this row: */
        ar3=0.0;  /* Largest vertical text box range in this row. */
        for (bidx=0; bidx<row[ridx].nboxes; bidx++) {
            if (row[ridx].box[bidx].nfrags<1) continue;  /* No frags (precaution) */
            for (fidx=0; fidx<row[ridx].box[bidx].nfrags; fidx++) {
                /* Capture lowest and highest textbox Y's in this box. */
                /* IMPORTANT: Doing it in this manner keeps the frag's */
                /* baselines aligned in this box: */
                if (fidx<1) {
                    ar1=row[ridx].box[bidx].frag[0]->textboxpt[0][1];
                    ar2=row[ridx].box[bidx].frag[0]->textboxpt[1][1];
                } else {
                    if (ar1>row[ridx].box[bidx].frag[fidx]->textboxpt[0][1])
                        ar1=row[ridx].box[bidx].frag[fidx]->textboxpt[0][1];
                    if (ar2<row[ridx].box[bidx].frag[fidx]->textboxpt[1][1])
                        ar2=row[ridx].box[bidx].frag[fidx]->textboxpt[1][1];
                }
            }
            if (ar3<ar2-ar1) ar3=ar2-ar1;
        }
        ar3+=gap+gap;  /* Add the top and bottom gap. */
        /* Use ar3 to set the bottom edge for this row: */
        row[ridx].bottom=(ridx<1) ? -0.5*ar3 : row[ridx-1].bottom-ar3;

        /* Now that we have the row's bottom edge, we can set each box's */
        /* baseline relative to that.  (We'll also set each box's right */
        /* edge as we go.) */
        for (bidx=0; bidx<row[ridx].nboxes; bidx++) {

            if (row[ridx].box[bidx].nfrags<1) continue;  /* No frags (precaution) */

            /* Init this box's right edge to the prev box's right edge: */
            row[ridx].box[bidx].right=(bidx<1) ?
                0.0 : row[ridx].box[bidx-1].right;

            /* Init ptrs to the lowest and highest frags in this box: */
            lofrag=hifrag=NULL;

            for (fidx=0; fidx<row[ridx].box[bidx].nfrags; fidx++) {
                /* Add leading gap: */
                row[ridx].box[bidx].right+=fabs((fidx<1) ?
                    gap : row[ridx].box[bidx].frag[fidx-1]->tgpar.ht*spacingfact);

                /* Add the frag's textbox width: */
                row[ridx].box[bidx].right+=
                    row[ridx].box[bidx].frag[fidx]->textboxpt[1][0]-
                    row[ridx].box[bidx].frag[fidx]->textboxpt[0][0];

                /* Capture lowest and highest frags: */
                if (fidx<1) lofrag=hifrag=row[ridx].box[bidx].frag[0];
                else {
                    if (lofrag->textboxpt[0][1]>
                        row[ridx].box[bidx].frag[fidx]->textboxpt[0][1])
                            lofrag=row[ridx].box[bidx].frag[fidx];
                    if (hifrag->textboxpt[1][1]<
                        row[ridx].box[bidx].frag[fidx]->textboxpt[1][1])
                            hifrag=row[ridx].box[bidx].frag[fidx];
                }
            }

            /* Add final gap to this box's right edge: */
            row[ridx].box[bidx].right+=gap;

            /* Use the lowest and highest frags to determine where this box's */
            /* baseline will be: */
            if (lofrag!=NULL && hifrag!=NULL) {
                /* This row's height: */
                ar1=(ridx<1) ?
                    -2.0*row[0].bottom : row[ridx-1].bottom-row[ridx].bottom;

                /* Height of a "textbox" containing all frags in this box: */
                ar2=hifrag->textboxpt[1][1]-lofrag->textboxpt[0][1];

                /* This box's baseline: */
                row[ridx].box[bidx].baseline=
                    row[ridx].bottom+0.5*(ar1-ar2)-lofrag->textboxpt[0][1];
            } else {  /* Should never, but set a safe default */
                row[ridx].box[bidx].baseline=(ridx<1) ?
                    0.0 : 0.5*(row[ridx-1].bottom+row[ridx].bottom);
            }

        }  /* End for each box */

    }  /* End for each row */


#if defined(GR_DUMPTOL)
    {
        FILE *fptr;

        if ((fptr=fopen("c:\\t\\----test.chk"/*DNT*/,"at"/*DNT*/))!=NULL) {
            fputs("\n\n",fptr);
            for (ridx=0; ridx<nrows; ridx++) {
                fprintf(fptr,"Row %d:\n",ridx);
                fprintf(fptr,"  bottom: %G\n",row[ridx].bottom);
                for (bidx=0; bidx<row[ridx].nboxes; bidx++) {
                    fprintf(fptr,"  Box %d:\n",bidx);
                    fprintf(fptr,"    baseline: %G\n",row[ridx].box[bidx].baseline);
                    fprintf(fptr,"    right: %G\n",row[ridx].box[bidx].right);
                    for (fidx=0; fidx<row[ridx].box[bidx].nfrags; fidx++) {
                        fprintf(fptr,"    Frag %d:\n",fidx);
                        fprintf(fptr,"      tgpar.val: |%s|\n",
                            (row[ridx].box[bidx].frag[fidx]==NULL ||
                             row[ridx].box[bidx].frag[fidx]->tgpar.val==NULL) ?
                                "NULL" : row[ridx].box[bidx].frag[fidx]->tgpar.val);
                        fprintf(fptr,"      tgpar.font->pn: |%s|\n",
                            (row[ridx].box[bidx].frag[fidx]==NULL ||
                             row[ridx].box[bidx].frag[fidx]->tgpar.font==NULL ||
                             row[ridx].box[bidx].frag[fidx]->tgpar.font->pn==NULL) ?
                                "NULL" : row[ridx].box[bidx].frag[fidx]->tgpar.val);
                    }
                }
            }

            fclose(fptr);
        }
    }
#endif


                     /* GENERATE THE LINE-WORK */

	viewp->lweight = lweight;	// set lineweight for frames here! 2003/4/1

    /* Set up for 1 line per disp obj: */
    if (viewp->allocatePointsChain(2)) 
	{ 
		rc = -3; 
		goto out; 
	}
    viewp->pc->swid=viewp->pc->ewid=0.0;
	viewp->pc->startang=viewp->pc->endang=IC_ANGLE_DONTCARE;
    viewp->pc->next->swid=viewp->pc->next->ewid=0.0;
	viewp->pc->next->startang=viewp->pc->next->endang=IC_ANGLE_DONTCARE;
    viewp->pc->next->d2go=-1.0;
    viewp->lupc=viewp->pc->next;

    /*
    **  We need the 10-pt: the TOLERANCE insertion point, which
    **  specifies the midpoint of the left edge of the first row
    **  (the TOLCS origin).
    **  It's WCS, so we need to transform it to the entaxis system
    **  so that we can treat TOLERANCE like an ECS entity:
    */
    viewp->ment->get_10(ap1);
    ecsipt[0]=ap1[0]*entaxis[0][0]+ap1[1]*entaxis[0][1]+ap1[2]*entaxis[0][2];
    ecsipt[1]=ap1[0]*entaxis[1][0]+ap1[1]*entaxis[1][1]+ap1[2]*entaxis[1][2];
    ecsipt[2]=ap1[0]*entaxis[2][0]+ap1[1]*entaxis[2][1]+ap1[2]*entaxis[2][2];

    /*
    **  We're going to go one better than ACAD and generated the line-work
    **  in a non-overlapping fashion.
    */

    /* To generate the line-work, set up the two tolpt[]s with the */
    /* proper TOLCS X and Y.  The Z is ecsipt[2] and is set in */
    /* gr_gentolline(). */

    tolpt[0][2]=tolpt[1][2]=0.0;  /* Just in case.  Shouldn't be used. */

    /* The top edge of the first row: */
    tolpt[0][0]=0.0;
    tolpt[1][0]=row[0].box[row[0].nboxes-1].right;
    tolpt[0][1]=tolpt[1][1]=-row[0].bottom;
    MAKELINE;

    /* Internal horizontal lines: */
    for (ridx=1; ridx<nrows; ridx++) {
        /* Get idx of the longer row -- this one, or the previous one: */
        fi1=ridx-(row[ridx-1].box[row[ridx-1].nboxes-1].right>
                  row[ridx  ].box[row[ridx  ].nboxes-1].right);

        /* Make a line this long at the bottom of the prev row: */
        tolpt[0][0]=0.0;
        tolpt[1][0]=row[fi1].box[row[fi1].nboxes-1].right;
        tolpt[0][1]=tolpt[1][1]=row[ridx-1].bottom;
        MAKELINE;
    }

    /* The bottom of the last row: */
    fi1=nrows-1;
    tolpt[0][0]=0.0;
    tolpt[1][0]=row[fi1].box[row[fi1].nboxes-1].right;
    tolpt[0][1]=tolpt[1][1]=row[fi1].bottom;
    MAKELINE;

    /* The left edge: */
    tolpt[0][0]=tolpt[1][0]=0.0;
    tolpt[0][1]=-row[0].bottom;
    tolpt[1][1]=row[nrows-1].bottom;
    MAKELINE;

    /* The right edges of each of the boxes: */
    for (ridx=0; ridx<nrows; ridx++) {
        for (bidx=0; bidx<row[ridx].nboxes; bidx++) {
            tolpt[0][0]=tolpt[1][0]=row[ridx].box[bidx].right;
            tolpt[0][1]=(ridx<1) ? -row[0].bottom : row[ridx-1].bottom;
            tolpt[1][1]=row[ridx].bottom;
            MAKELINE;
        }
    }


                    /* GENERATE THE TEXT FRAGS */

	viewp->lweight = tlweight;	// set lineweight for mark and tolerance here! 2003/4/1

    /*
    **  Note that ap1 is used as the TOLCS llpt.  The box "for" loop
    **  initializes it for the first frag in that box, and the frag
    **  "for" loop's incrementation expression increments the X
    **  for each successive frag.
    */
    for (ridx=0; ridx<nrows; ridx++) {
        for (bidx=0; bidx<row[ridx].nboxes; bidx++) {
            ap1[0]=((bidx<1) ?
                0.0 : row[ridx].box[bidx-1].right)+gap;  /* Left edge + gap. */
            ap1[1]=row[ridx].box[bidx].baseline;
            for (fidx=0; fidx<row[ridx].box[bidx].nfrags; ap1[0]+=
                row[ridx].box[bidx].frag[fidx]->textboxpt[1][0]-
                row[ridx].box[bidx].frag[fidx]->textboxpt[0][0]+
                row[ridx].box[bidx].frag[fidx]->tgpar.ht*spacingfact,fidx++) {

                /* Transform the llpt from TOLCS to ECS and put it in */
                /* row[ridx].box[bidx].frag[fidx]->tgpar.llpt */
                /* (rotate by xdrot and translate by ecsipt): */
                row[ridx].box[bidx].frag[fidx]->tgpar.llpt[0]=
                    ap1[0]*xdrot_cs-ap1[1]*xdrot_sn+ecsipt[0];
                row[ridx].box[bidx].frag[fidx]->tgpar.llpt[1]=
                    ap1[1]*xdrot_cs+ap1[0]*xdrot_sn+ecsipt[1];
                row[ridx].box[bidx].frag[fidx]->tgpar.llpt[2]=ecsipt[2];
                    /* Same elevation in ECS as insertion pt. */
                    /* (ECS and TOLCS differ only in */
                    /* XY-translation and rotation.) */

                /* Temporarily force the color, and generate the disp objs: */
                fi1=viewp->color;
                viewp->color=row[ridx].box[bidx].frag[fidx]->color;
                rc=gr_textgen(pTolerance,
							 &row[ridx].box[bidx].frag[fidx]->tgpar,
							 0,
							 dp,
							 viewp,
							 dc,
							 &tbegdo,
							 &tenddo,
							 NULL,
							 NULL);
                viewp->color=fi1;
                if (rc) goto out;

                /* Tack 'em on: */
                if (tbegdo!=NULL && tenddo!=NULL) {
                    if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
                    doll[1]=tenddo;
                }
            }
        }
    }


    #undef MAKELINE

out:
	viewp->m_transformation = insertTransformation;
    if (row!=NULL) {
        for (ridx=0; ridx<nrows; ridx++) {
            if (row[ridx].box!=NULL) {
                for (bidx=0; bidx<row[ridx].nboxes; bidx++)
                    if (row[ridx].box[bidx].frag!=NULL)
                        IC_FREE(row[ridx].box[bidx].frag);
                IC_FREE(row[ridx].box);
            }
        }
        IC_FREE(row);
    }
    if (eedrbll!=NULL) sds_relrb(eedrbll);
    while (ftfragsll!=NULL) {
        if (ftfragsll->tgpar.val!=NULL) IC_FREE(ftfragsll->tgpar.val);
        tfragp1=ftfragsll->next; delete ftfragsll; ftfragsll=tfragp1;
    }
    if (begdopp!=NULL) *begdopp=doll[0];
    if (enddopp!=NULL) *enddopp=doll[1];

	viewp->lweight = savlweight;	// restore view's lineweight

    return rc;
}


