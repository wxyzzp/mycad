/* C:\ICAD\PRJ\LIB\DB\LEADER.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
// EBATECH(watanabe)-[update leader according to modifying annotation
#include "UpdateLeader.h"
//]- EBATECH(watanabe)

//Bugzilla No. 78013 ; 01-04-2002 
#include "dimstyletabrec.h"/*DNT*/
#include "dbmtext.h"

/**************************** db_leader *****************************/

extern int g_nClosingDrawings;

/*F*/
db_leader::db_leader(void) : db_entity_with_extrusion(DB_LEADER) {

    dship=NULL;
    vert=NULL;
    antextht=antextwd=0.0;
    arrow=pathtype=hookdir=hook=0; creation=3; nverts=0; bbcolor=DB_BLCOLOR;
    hdir[0]=1.0;
	//Bugzilla No. 78044 ; 25-03-2002 
    //hdir[1]=hdir[2]=anoffset[0]=anoffset[1]=anoffset[2]=txtoffset[0]=txtoffset[1]=
	//	txtoffset[2]=offsettoblockinspt[0]=offsettoblockinspt[1]=offsettoblockinspt[2]=0.0;
	hdir[1]=hdir[2]=txtoffset[0]=txtoffset[1]=txtoffset[2]=
		offsettoblockinspt[0]=offsettoblockinspt[1]=offsettoblockinspt[2]=0.0;
    annothip=NULL;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_leader::db_leader(db_drawing *dp) : db_entity_with_extrusion(DB_LEADER,dp) {
    dship=(dp!=NULL) ? dp->ret_currenttabrec(DB_QDIMSTYLE) : NULL;
    vert=NULL;
    antextht=antextwd=0.0;
    arrow=pathtype=hookdir=hook=0; creation=3; nverts=0; bbcolor=DB_BLCOLOR;
    hdir[0]=1.0;
	//Bugzilla No. 78044 ; 25-03-2002 
    //hdir[1]=hdir[2]=anoffset[0]=anoffset[1]=anoffset[2]=txtoffset[0]=txtoffset[1]=
	//	txtoffset[2]=offsettoblockinspt[0]=offsettoblockinspt[1]=offsettoblockinspt[2]=0.0;
	hdir[1]=hdir[2]=txtoffset[0]=txtoffset[1]=txtoffset[2]=
		offsettoblockinspt[0]=offsettoblockinspt[1]=offsettoblockinspt[2]=0.0;
    annothip=NULL;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_leader::db_leader(const db_leader &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */


    nverts=sour.nverts; vert=NULL;
    if (nverts>0 && sour.vert!=NULL) {
        vert=new sds_point[nverts];
        memcpy(vert,sour.vert,nverts*sizeof(sds_point));
    }

    DB_PTCPY(hdir,sour.hdir); 
	//Bugzilla No. 78044 ; 25-03-2002 
	//DB_PTCPY(anoffset,sour.anoffset);
	DB_PTCPY(offsettoblockinspt,sour.offsettoblockinspt);
	//Bugzilla No. 78013 ; 01-04-2002
	DB_PTCPY(txtoffset,sour.txtoffset);
    dship   =sour.dship   ; antextht=sour.antextht; antextwd=sour.antextwd;
    arrow   =sour.arrow   ; pathtype=sour.pathtype; creation=sour.creation;
    hookdir =sour.hookdir ;     hook=sour.hook    ;  bbcolor=sour.bbcolor;
    annothip=sour.annothip;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_leader::~db_leader(void) {

	db_handitem * pAnnotation;
	this->get_340(&pAnnotation);

	if (0 == g_nClosingDrawings && pAnnotation &&
		pAnnotation->getReactor() && pAnnotation->getReactor()->FindReactor(this))
		pAnnotation->removeReactor(RetHandle());

    delete [] vert; 

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);

	// 5/99 Note that once leaders are really implemented with association to the
	// text the leader will have to use the 340 code to clean up the association from the other
	// entity to the leader.
}

/*F*/
int db_leader::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    short fsh1;
    int rc,fi1;
    sds_name en;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 3 (dimstyle name) */
    fcp1=NULL;
    if (dship!=NULL) dship->get_2(&fcp1);
    if ((sublist[0]=sublist[1]=db_newrb(3,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }

    /* 71 (arrow) */
    fsh1=(short)arrow;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 72 (pathtype) */
    fsh1=(short)pathtype;
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (creation) */
    fsh1=(short)creation;
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 74 (hookdir) */
    fsh1=(short)hookdir;
    if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 75 (hook) */
    fsh1=(short)hook;
    if ((sublist[1]->rbnext=db_newrb(75,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (antextht) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&antextht))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (antextwd) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',&antextwd))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 76 (nverts) */
    fsh1=(short)nverts;
    if ((sublist[1]->rbnext=db_newrb(76,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    if (vert!=NULL) {
        for (fi1=0; fi1<nverts; fi1++) {
            /* 10 (vert[fi1]) */
            if ((sublist[1]->rbnext=db_newrb(10,'P',vert[fi1]))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;
        }
    }

    /* 77 (bbcolor) */
    fsh1=(short)bbcolor;
    if ((sublist[1]->rbnext=db_newrb(77,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 340 (annotation entity's ename) */
    en[0]=(long)annothip;
    en[1]=(long)dp;
    if ((sublist[1]->rbnext=db_newrb(340,'N',en))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 211 (hdir) */
    if ((sublist[1]->rbnext=db_newrb(211,'P',hdir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	//Bugzilla No. 78044 ; 25-03-2002 
	///* 212 (anoffset) */
    //if ((sublist[1]->rbnext=db_newrb(212,'P',anoffset))==NULL) { rc=-1; goto out; }
	/* 212 (offsettoblockinspt) */
    if ((sublist[1]->rbnext=db_newrb(212,'P',offsettoblockinspt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	//Bugzilla No. 78013 ; 01-04-2002
	/*213 (txtoffset) */
	if ((sublist[1]->rbnext=db_newrb(213,'P',txtoffset))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
int db_leader::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int did10,cnt,pass,rc;
    sds_point *parray;
    struct resbuf *curlink,*trbp1;
    db_handitem *thiship;


    rc=0; thiship=(db_handitem *)this;
    parray=NULL;
    did10=0;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    curlink=modll;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (curlink!=NULL &&
        (curlink->restype==-1 ||
         curlink->restype== 0 ||
         curlink->restype== 5)) curlink=curlink->rbnext;

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {

        /* Call the correct setter: */
        switch (curlink->restype) {

            case   3: thiship->set_3(curlink->resval.rstring,dp); break;

            case  10:
                if (did10) break;
                delete [] parray; parray=NULL;
                did10=1;
                for (pass=0; pass<2; pass++) {  /* 0 : count; 1 : collect */
                    if (pass) {
                        if (cnt<1) break;
                        parray=new sds_point[cnt];
                    }
                    cnt=0;
                    for (trbp1=curlink; trbp1!=NULL; trbp1=trbp1->rbnext) {
                        if (trbp1->restype!=10) continue;
                        if (pass) DB_PTCPY(parray[cnt],trbp1->resval.rpoint);
                        cnt++;
                    }
                }
                thiship->set_10(parray,cnt);
                break;

            case  40: thiship->set_40(curlink->resval.rreal); break;
            case  41: thiship->set_41(curlink->resval.rreal); break;

            case  71: thiship->set_71(curlink->resval.rint); break;
            case  72: thiship->set_72(curlink->resval.rint); break;
            case  73: thiship->set_73(curlink->resval.rint); break;
            case  74: thiship->set_74(curlink->resval.rint); break;
            case  75: thiship->set_75(curlink->resval.rint); break;
            case  76: break;  /* Set by 10.  Just avoid "default". */
            case  77: thiship->set_77(curlink->resval.rint); break;

            case 210: thiship->set_210(curlink->resval.rpoint); break;
            case 211: thiship->set_211(curlink->resval.rpoint); break;
            case 212: thiship->set_212(curlink->resval.rpoint); break;
			//Bugzilla No. 78013 ; 01-04-2002
			case 213: thiship->set_213(curlink->resval.rpoint); break;

            case 340: this->set_340andAssociate((db_handitem *)curlink->resval.rlname[0]); break;

            default:  /* The common entity or table record groups. */
                rc=thiship->entmod1common(&curlink,dp);
                break;
        }
    }

    if (db_lasterror) { rc=-6; goto out; }

    if (!rc) {
        /* For the arrays, clear them if they never appeared in modll: */
        if (!did10) thiship->set_10((sds_point *)NULL,0);

        /* Do the EED: */
        if (curlink!=NULL) rc=set_eed(curlink,dp);
    }

out:
    delete [] parray;

    return rc;
}

void db_leader::BreakAssociation(db_handitem * entity, bool dispMessage)
	{
	// EBATECH(watanabe)-[remove annotation entity
	this->annothip = NULL;  // annotation entity's ename
	this->set_73(3);        // creation
	this->set_75(0);        // hook

	// redraw leader
	CUpdateLeader* pUpdLeader = IcadSharedGlobals::GetLeaderFactory()->CreateUpdateLeader(this);
	pUpdLeader->Redraw();
	delete pUpdLeader;
	//]- EBATECH(watanabe)

	BreakAssociation(dispMessage);
	}

void db_leader::BreakAssociation(bool dispMessage)
	{
	if (0 == g_nClosingDrawings && annothip)
		annothip->removeReactor(this->RetHandle());

	annothip = NULL;
	}

// EBATECH(watanabe)-[update leader according to modifying annotation
bool db_leader::ReactorUpdate(db_objhandle &reactor)
{
	CUpdateLeader* pUpdLeader =
			IcadSharedGlobals::GetLeaderFactory()->CreateUpdateLeader(this);
	pUpdLeader->ModifyLeader(&reactor);
	delete pUpdLeader;
	// EBATECH(watanabe)-[FIX:show old leader too
	//pUpdLeader->Redraw();
	// ]-EBATECH(watanabe)
	return true;
}
//]- EBATECH(watanabe)

//Bugzilla No. 78013 ; 01-04-2002 [
//get the required dimension variables (DIMGAP,DIMSCALE & DIMATAD ) from the 
//leader's dimension style or EED
bool db_leader::GetDimvars(sds_real& dDimgap, sds_real& dDimscale,int& iDimtad)
{
  dDimgap = 0.09;
  iDimtad = 0;
  //sachin
  dDimscale = 1.0;

  // get DIMSTYLE rec ptr
  db_dimstyletabrec* dship;
  this->get_3((db_handitem **)&dship);
  if (dship == NULL)
    {
      return false;
    }

  // DIMGAP
  dship->get_147(&dDimgap);

  // DIMTAD
  dship->get_77(&iDimtad);

  //DIMSCALE
  dship->get_40(&dDimscale);

  // from EED
  struct resbuf appacadrb;
  appacadrb.restype = RTSTR;
  appacadrb.rbnext = NULL;
  appacadrb.resval.rstring = "ACAD";
  struct resbuf* rbEed = this->get_eed(&appacadrb, NULL);
  struct resbuf* rb    = rbEed;

  while ((rb != NULL) &&
         (rb->restype != 1000 || stricmp(rb->resval.rstring, "DSTYLE")))
    {
      rb = rb->rbnext;
    }

  // Matching { for the closing one below
  for (; rb != NULL && (rb->restype != 1002 || *rb->resval.rstring != '}'); rb = rb->rbnext)
    {
      if (rb->restype != 1070 || rb->rbnext == NULL)
        {
          continue;
        }

      int iCode = rb->resval.rint;
      rb = rb->rbnext;
      switch (iCode)
        {
          case 77:
              if (rb->restype == 1070)
                {
                  iDimtad = rb->resval.rint;
                }
              break;

          case 147:
              if (rb->restype == 1040)
                {
                  dDimgap = rb->resval.rreal;
                }
              break;
		  case 40:
              if (rb->restype == 1040)
                {
                  dDimscale = rb->resval.rreal;
                }
              break;
        }
    }

  sds_relrb(rbEed);

  return true;
}

//This function called from Icadgrips.cpp to calculate the value of DXF 213 
//when the last leader vertex is dragged
void db_leader::Calculate213()
{
	db_handitem * anno;
	this->get_340(&anno);
	if(anno!=NULL)
	{
		sds_point inspt;
		anno->get_10(inspt);

		sds_point txtoffset ;
	
		txtoffset[0]= vert[(nverts-1)][0] - inspt[0];
		txtoffset[1]= vert[(nverts-1)][1] - inspt[1];
		txtoffset[2]= vert[(nverts-1)][2] - inspt[2];
		
		int type = anno->ret_type();

		if (type == DB_MTEXT)
		{
			// get DIMGAP from leader
			sds_real dimgap;
			sds_real dimscale;
			int      dimtad;
			
			GetDimvars(dimgap,dimscale,dimtad);
			dimgap = fabs(dimgap);

			int hookflg;
			this->get_74(&hookflg);

			if(hookflg == 0)
			{
				dimgap = - dimgap;
			}

			// hook dir
			sds_point dir;
			this->get_211(dir);

			txtoffset[0] -= (dir[0] * dimgap * dimscale); 
			txtoffset[1] -= (dir[1] * dimgap * dimscale);
			txtoffset[2] -= (dir[2] * dimgap * dimscale);
			
			if (dimtad != 0)
			{
				txtoffset[1] += (fabs(dimgap) * dimscale);
			}
		}
		else if (type == DB_TOLERANCE)
		{
			//don't do anything already calculated
		}
		else if (type == DB_INSERT)
		{
			sds_point offsettoblockinspt;
			this->get_212(offsettoblockinspt);

			txtoffset[0] -= offsettoblockinspt[0] ;
			txtoffset[1] -= offsettoblockinspt[1] ;
			txtoffset[2] -= offsettoblockinspt[2] ;

		}	
				
		this->set_213(txtoffset);
	}
}
//Bugzilla No. 78013 ; 01-04-2002 ]

bool db_leader::set_340(db_handitem *p)
{
	if(annothip && annothip != p && m_createAssocFlag)
		BreakAssociation();
	annothip=p;
	return true;
}

bool db_leader::notifyDeleted(db_handitem* hip)
{
	if(hip == this || annothip && hip == annothip)
		return true;
	return false;
}