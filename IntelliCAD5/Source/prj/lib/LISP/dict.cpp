/* C:\ICAD\PRJ\LIB\LISP\DICT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "objects.h"

int lsp_namedobjdict(void) {
	sds_name en;

	/* (Even ACAD ignores any parameters.) */

	en[0]=en[1]=0; sds_namedobjdict(en);
	lsp_retname(en,RTENAME);
	return 0;
}

int lsp_dictadd(void) {
	int msgn;
	struct resbuf *argp[3];

	msgn=-1;

	if ((argp[0]=lsp_argsll)	 ==NULL) { msgn=2; goto out; }
	if ( argp[0]->restype!=RTENAME) 	 { msgn=1; goto out; }
	if ((argp[1]=argp[0]->rbnext)==NULL) { msgn=2; goto out; }
	if ( argp[1]->restype!=RTSTR ||
		 argp[1]->resval.rstring==NULL)  { msgn=1; goto out; }
	if ((argp[2]=argp[1]->rbnext)==NULL) { msgn=2; goto out; }
	if ( argp[2]->restype!=RTENAME) 	 { msgn=1; goto out; }
	if ( argp[2]->rbnext!=NULL) 		 { msgn=9; goto out; }

	if (sds_dictadd(argp[0]->resval.rlname,
					argp[1]->resval.rstring,
					argp[2]->resval.rlname)==RTNORM)
						lsp_retname(argp[2]->resval.rlname,RTENAME);
	else lsp_retnil();

out:
	if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
	return 0;
}

int lsp_dictremove(void) {	/* Yep: remove, not del like the ADS one. */
	int msgn;
	sds_name en;
	struct resbuf *argp[2],*elist,*trbp1;

	msgn=-1; elist=NULL; en[0]=en[1]=0;

	if ((argp[0]=lsp_argsll)	 ==NULL) { msgn=2; goto out; }
	if ( argp[0]->restype!=RTENAME) 	 { msgn=1; goto out; }
	if ((argp[1]=argp[0]->rbnext)==NULL) { msgn=2; goto out; }
	if ( argp[1]->restype!=RTSTR ||
		 argp[1]->resval.rstring==NULL)  { msgn=1; goto out; }
	if ( argp[1]->rbnext!=NULL) 		 { msgn=9; goto out; }

	/*
	**	This one's weird.  If sds_dictdel() succeeds, we need to
	**	return the ename from the deleted record.  sds_dictdel()
	**	doesn't provide that information.  So, let's get the
	**	ename from an sds_dictsearch():
	*/
	elist=sds_dictsearch(argp[0]->resval.rlname,argp[1]->resval.rstring,0);
	for (trbp1=elist; trbp1!=NULL && trbp1->restype!=-1; trbp1=trbp1->rbnext);
	if (trbp1!=NULL) {
		en[0]=trbp1->resval.rlname[0];
		en[1]=trbp1->resval.rlname[1];
	}

	if (sds_dictdel(argp[0]->resval.rlname,
					argp[1]->resval.rstring)==RTNORM)
						lsp_retname(en,RTENAME);
	else lsp_retnil();

out:
	if (elist!=NULL) lsp_freesuprb(elist);
	if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
	return 0;
}

int lsp_dictrename(void) {
/*
**	ACAD doesn't let you use ACAD_GROUP or ACAD_MLINESTYLE
**	as either name (old or new), but it will let you use
**	ACAD_xxx.  We're not restricting ours, so it's up to
**	the user to avoid renaming dictionary records that
**	shouldn't be renamed.  (If we ever DO restrict, the place
**	to do it is in sds_dictrename(), but we may need code here,
**	too.)
**
**	This is another weird one.	ACAD LISP documentation does
**	not tell what the successful return code is.  (The error
**	return code is nil.)  The sds_ one returns RTNORM or RTERROR.
**	Experimentation shows that the LISP one returns a NEW
**	name -- but not necessarily the one you'd expect.
**	Suppose we have the following DICTIONARY records:
**
**		NAME1	   ename1
**		NAME2	   ename2
**		NAME3	   ename3
**		NAME4	   ename2
**
**	If you rename NAME4 to NAMEX, NAME4 does indeed change to
**	NAMEX -- but the LISP return value is NAME2 (the name of the
**	first record with the same ename as the renamed one). (For all
**	other records, NAMEX is returned.  The rule is, AFTER the
**	change, return the name of the first record with the same
**	ename as the changed record.)  This is reminiscent of dictnext
**	which gets caught in an infinite loop for the same reason.
**
**	Oops!  It's even stranger (must be a bug!), but I'm not going
**	to make ours follow suit to this level (because I'd need to
**	research it further -- and I DO think it's a bug):
**
**		NAME1	  ename1
**		NAME2	  ename2
**		NAME3	  ename3
**		NAME4	  ename2
**		NAME5	  ename2
**
**	If NAME5 is changed to NAMEX, dictrename returns
**
**		NAME2 if NAME2 is ACAD_GROUP
**		NAME4 if NAME2 is ACAD_MLINESTYLE
*/
	int msgn;
	struct resbuf *argp[3];
	db_drawing *thisdwg;
	db_dictionary *thisdict;
	db_dictionaryrec *tdrp1;
	db_handitem *thip1;
	db_dictionarywdflt *thisdicw;	 // Ebatech(cnbr) 2002/9/2 Bugzilla78227

	msgn=-1;

	if ((argp[0]=lsp_argsll)	 ==NULL) { msgn=2; goto out; }
	if ( argp[0]->restype!=RTENAME) 	 { msgn=1; goto out; }
	if ((argp[1]=argp[0]->rbnext)==NULL) { msgn=2; goto out; }
	if ( argp[1]->restype!=RTSTR ||
		 argp[1]->resval.rstring==NULL)  { msgn=1; goto out; }
	if ((argp[2]=argp[1]->rbnext)==NULL) { msgn=2; goto out; }
	if ( argp[2]->restype!=RTSTR ||
		 argp[2]->resval.rstring==NULL)  { msgn=1; goto out; }
	if ( argp[2]->rbnext!=NULL) 		 { msgn=9; goto out; }


	/* Find the record we're going to change and get its hip: */
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (((db_handitem *)argp[0]->resval.rlname[0])->ret_type()!=DB_DICTIONARY)
	//	  { msgn=1; goto out; }
	//if ((thisdict=(db_dictionary *)argp[0]->resval.rlname[0])==NULL ||
	//	  (thisdwg =(db_drawing *)	 argp[0]->resval.rlname[1])==NULL ||
	//	  (tdrp1=thisdict->findrec(argp[1]->resval.rstring,0,thisdwg))==NULL)
	//		  { lsp_retnil(); goto out; }
	//thip1=(tdrp1!=NULL && tdrp1->hiref!=NULL) ?
	//	  tdrp1->hiref->ret_hip(thisdwg) : NULL;
	if (((db_handitem *)argp[0]->resval.rlname[0])->ret_type() == DB_DICTIONARY)
	{
		if ((thisdict=(db_dictionary *)argp[0]->resval.rlname[0])==NULL ||
			(thisdwg =(db_drawing *)   argp[0]->resval.rlname[1])==NULL ||
			(tdrp1=thisdict->findrec(argp[1]->resval.rstring,0,thisdwg))==NULL)
			{ lsp_retnil(); goto out; }
		thip1=(tdrp1!=NULL && tdrp1->hiref!=NULL) ?
			tdrp1->hiref->ret_hip(thisdwg) : NULL;
	}
	else if (((db_handitem *)argp[0]->resval.rlname[0])->ret_type() == DB_DICTIONARYWDFLT )
	{
		if ((thisdicw=(db_dictionarywdflt *)argp[0]->resval.rlname[0])==NULL ||
			(thisdwg =(db_drawing *)   argp[0]->resval.rlname[1])==NULL ||
			(tdrp1=thisdicw->findrec(argp[1]->resval.rstring,0,thisdwg))==NULL)
			{ lsp_retnil(); goto out; }
		thip1=(tdrp1!=NULL && tdrp1->hiref!=NULL) ?
			tdrp1->hiref->ret_hip(thisdwg) : NULL;
	}
	else
	{ msgn=1; goto out; }
	// Ebatech(cnbr) ]-

	/* Do the renaming: */
	if (sds_dictrename(argp[0]->resval.rlname,
					   argp[1]->resval.rstring,
					   argp[2]->resval.rstring)!=RTNORM)
							{ lsp_retnil(); goto out; }

	/* Find the first record whose hip matches thip1 and return */
	/* that record's name: */
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//for (thisdict->get_recllends(&tdrp1,NULL); tdrp1!=NULL &&
	//	  (tdrp1->hiref==NULL ||
	//	   tdrp1->hiref->ret_hip(thisdwg)!=thip1); tdrp1=tdrp1->next);
	if (((db_handitem *)argp[0]->resval.rlname[0])->ret_type() == DB_DICTIONARY)
	{
		for (thisdict->get_recllends(&tdrp1,NULL); tdrp1!=NULL &&
			(tdrp1->hiref==NULL ||
			tdrp1->hiref->ret_hip(thisdwg)!=thip1); tdrp1=tdrp1->next);
	}
	else
	{
		for (thisdicw->get_recllends(&tdrp1,NULL); tdrp1!=NULL &&
			(tdrp1->hiref==NULL ||
			tdrp1->hiref->ret_hip(thisdwg)!=thip1); tdrp1=tdrp1->next);
	}
	// Ebatech(cnbr) ]-

	if (tdrp1==NULL || tdrp1->name==NULL) lsp_retnil();
	else lsp_retstr(tdrp1->name);

out:
	if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
	return 0;
}

int lsp_dictnext(void) {
/*
** Another one with a quirk (bug?).  ACAD allows a 3rd parameter,
** which must be a string, but doesn't seem to use it.	(It's
** apparently not a search parameter like in dictsearch;
** it doesn't locate a record by name.)
*/
	int msgn;
	struct resbuf *elist,*assocll,*argp[3];
	int typew=0;

	msgn=-1;
	elist=assocll=NULL;

	if ((argp[0]=lsp_argsll)	 ==NULL)   { msgn=2; goto out; }
	if ( argp[0]->restype!=RTENAME) 	   { msgn=1; goto out; }
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (((db_handitem *)argp[0]->resval.rlname[0])->ret_type()!=DB_DICTIONARY)
	//										 { msgn=41; goto out; }
	typew = ((db_handitem *)argp[0]->resval.rlname[0])->ret_type();
	if ( typew != DB_DICTIONARY && typew != DB_DICTIONARYWDFLT )
										   { msgn=41; goto out; }
	// Ebatech(cnbr) ]-
	argp[1]=argp[0]->rbnext;
	if (argp[1]!=NULL && (argp[2]=lsp_nextarg(argp[1]))!=NULL) {
		if (argp[2]->restype!=RTSTR ||
			argp[2]->resval.rstring==NULL) { msgn=1; goto out; }
		if (argp[2]->rbnext!=NULL)		   { msgn=9; goto out; }
	}

	elist=sds_dictnext(argp[0]->resval.rlname,
		(argp[1]!=NULL && argp[1]->restype!=RTNIL));

	/* Convert to a LISP association list: */
	if (lsp_convassoc(elist,&assocll,1)==-2) { msgn=7; goto out; }

	/* If it's NULL, set result to nil: */
	if (assocll==NULL || assocll->restype!=RTLB) { lsp_retnil(); goto out; }

	lsp_freesplrb(&lsp_evres);	/* IMPORTANT: Free prev value */
								 /* (since we're not doing an sds_ret...). */

	/* Steal the result from assocll: */
	lsp_evres.restype=RTLB;
	lsp_evres.resval.rstring=NULL;
	lsp_evres.rbnext=assocll->rbnext;
	assocll->rbnext=NULL;  /* Now assocll is just the RTLB link */
						   /* (soon to be freed). */
	assocll->resval.rstring=NULL;

out:
	if (elist  !=NULL) lsp_freesuprb(elist);
	if (assocll!=NULL) lsp_freesuprb(assocll);

	if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
	return 0;
}

int lsp_dictsearch(void) {
/*
** ACAD allows a 4th parameter, which must be a string, but doesn't
** seem to use it (similar to the 3rd dictnext parameter).
*/
	int msgn;
	struct resbuf *elist,*assocll,*argp[4];
	int typew=0;

	msgn=-1;
	elist=assocll=NULL;

	if ((argp[0]=lsp_argsll)	 ==NULL)   { msgn=2; goto out; }
	if ( argp[0]->restype!=RTENAME) 	   { msgn=1; goto out; }
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	//if (((db_handitem *)argp[0]->resval.rlname[0])->ret_type()!=DB_DICTIONARY)
	//										 { msgn=41; goto out; }
	typew = ((db_handitem *)argp[0]->resval.rlname[0])->ret_type();
	if ( typew != DB_DICTIONARY && typew != DB_DICTIONARYWDFLT )
										   { msgn=41; goto out; }
	// Ebatech(cnbr) ]-
	if ((argp[1]=argp[0]->rbnext)==NULL)   { msgn=2; goto out; }
	if ( argp[1]->restype!=RTSTR ||
		 argp[1]->resval.rstring==NULL)    { msgn=1; goto out; }
	argp[2]=argp[1]->rbnext;
	if (argp[2]!=NULL && (argp[3]=lsp_nextarg(argp[2]))!=NULL) {
		if (argp[3]->restype!=RTSTR ||
			argp[3]->resval.rstring==NULL) { msgn=1; goto out; }
		if (argp[3]->rbnext!=NULL)		   { msgn=9; goto out; }
	}

	elist=sds_dictsearch(argp[0]->resval.rlname,argp[1]->resval.rstring,
		(argp[2]!=NULL && argp[2]->restype!=RTNIL));

	/* Convert to a LISP association list: */
	if (lsp_convassoc(elist,&assocll,1)==-2) { msgn=7; goto out; }

	/* If it's NULL, set result to nil: */
	if (assocll==NULL || assocll->restype!=RTLB) { lsp_retnil(); goto out; }

	lsp_freesplrb(&lsp_evres);	/* IMPORTANT: Free prev value */
								 /* (since we're not doing an sds_ret...). */

	/* Steal the result from assocll: */
	lsp_evres.restype=RTLB;
	lsp_evres.resval.rstring=NULL;
	lsp_evres.rbnext=assocll->rbnext;
	assocll->rbnext=NULL;  /* Now assocll is just the RTLB link */
						   /* (soon to be freed). */
	assocll->resval.rstring=NULL;

out:
	if (elist  !=NULL) lsp_freesuprb(elist);
	if (assocll!=NULL) lsp_freesuprb(assocll);

	if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
	return 0;
}
