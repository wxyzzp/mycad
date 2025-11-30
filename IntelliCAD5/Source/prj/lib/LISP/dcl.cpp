/* C:\ICAD\PRJ\LIB\LISP\DCL.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "icadapi.h"
#include "dclprivate.h"
#include <wtypes.h>


/**************** BELOW IS ALL THE DCL STUFF ****************/

struct lsp_dialog_handle {
    sds_hdlg hand;
    struct lsp_dialog_handle *prev;
} *lsp_dialog_handle_c;

struct lsp_callback_list {
    sds_hdlg hdlg;
    char *key;
    char *exec;
    struct lsp_callback_list *next;

} *lsp_callback_list_b, *lsp_callback_list_c;


/* protos for the dialog stuff */
int  lsp_add_cb(sds_hdlg hdlg, char *key, char *exec);
int  lsp_dim_tile(int mode);
int  lsp_do_image(int mode);
int  lsp_free_cb(void);
void lsp_free_dialog_handles(void);

#ifdef	NOCYBERDCL
int dlg_get_key_string( char *szkeyString, long lInfoMaxLength) {
    wcstombs(szkeyString,dlg_KeyValue,lInfoMaxLength);
    return SDS_RTNORM;
}
#endif

void			   
lsp_init_dialog(
				LPCTSTR baseDCL ,					// i:Path of IBase.dcl 
				BOOL generateErrorLog,				// i:Flag set for generation of DCL log.
				LPCTSTR errorLogPath 			    // i:Path for Error.log to be generated. )
				)
	{
	dlg_init_dialog( baseDCL, generateErrorLog, errorLogPath, lsp_dia_callback);
	}


/* This is our call back function for all dialogs */
void lsp_dia_callback(sds_callback_packet *cpkt) {

		//	reset module state, since we are being called back from DCL DLL
	AFX_MANAGE_STATE(AfxGetAppModuleState());

    struct resbuf *callrb=NULL,rb1;
	int icancel=0;
	sds_hdlg		activeDialog = cpkt->dialog;
    int errcode=0;

    if(sds_usrbrk()) SDS_CancelAllPending(0);

		// get key and action strings from DCL

	TCHAR	keyString[IC_ACADBUF];
	TCHAR	actionString[IC_ACADBUF];
    TCHAR	cancelString[IC_ACADBUF];
	LPCTSTR	key=NULL;
	LPCTSTR	actionAttr=NULL, actionTile=NULL, defaultAction=NULL;


	if ( dlg_get_attr_string( cpkt->tile, "key", keyString, sizeof( keyString)-1) EQ RTNORM )
		key = keyString;

	if ( dlg_get_attr_string( cpkt->tile, "action", actionString, sizeof( actionString)-1) EQ RTNORM )
		actionAttr = actionString;
   
	//Modified SAU 12/07/2000   [
	//Reason:REGRESSION is_cancel attribute is ignored
	if( dlg_get_attr_string( cpkt->tile, "is_cancel", cancelString, sizeof( actionString)-1) EQ RTNORM)
        if(actionAttr!= NULL && cancelString !=NULL &&!strcmp(cancelString,"true") && !strcmp(actionAttr,"(done_dialog 0)")) 
			icancel=1; 
	
	   
		// search for callback if key specified.
    struct lsp_callback_list *tp=NULL;
	if (key && !icancel)
		{
	//Modified SAU 12/07/2000    ]
		for(tp=lsp_callback_list_b; tp!=NULL; tp=tp->next)
			{
			if( tp->hdlg==activeDialog )
				{
				if ( strsame( tp->key,key) )
					actionTile = tp->exec;
				else if ( strsame( tp->key, "") )
					defaultAction = tp->exec;
				}
			}
		}
    
		// determine which action is in effect
	LPCTSTR	action=NULL;
	if ( actionTile )
		action = actionTile;
	else if ( actionAttr )
		action = actionAttr;
	else if ( defaultAction )
		action = defaultAction;

	        // invoke action expression
	if ( action )
		{
        rb1.restype=RTSTR;
        rb1.resval.rstring=const_cast<char*>(key);// key;
        if(lsp_defatom("$key"/*DNT*/,0,&rb1,1)) { errcode=1; goto out; }
        rb1.resval.rstring=cpkt->value;
        if(lsp_defatom("$value"/*DNT*/,0,&rb1,1)) { errcode=2; goto out; }
        rb1.resval.rstring=(char *)cpkt->client_data;
        if(lsp_defatom("$data"/*DNT*/,0,&rb1,1)) { errcode=3; goto out; }

        rb1.restype=RTSHORT;
        rb1.resval.rint=cpkt->reason;
        if(lsp_defatom("$reason"/*DNT*/,0,&rb1,1)) { errcode=4; goto out; }

        rb1.restype=RTLONG;
        rb1.resval.rlong=cpkt->x;
        if(lsp_defatom("$x"/*DNT*/,0,&rb1,1)) { errcode=5; goto out; }
        rb1.resval.rlong=cpkt->y;
        if(lsp_defatom("$y"/*DNT*/,0,&rb1,1)) { errcode=6; goto out; }

		if(lsp_xstr2xll(action,&callrb)) { errcode=7; goto out; }

		int	eval = lsp_lispev1(callrb,NULL,NULL);
		if(	eval && eval != -4 )		// -4 indicates quit
			{ errcode=8; goto out; }

		if(callrb) { lsp_freesuprb(callrb); callrb=NULL; }
		}

    out: ;
	if(callrb) { sds_relrb(callrb); callrb=NULL; }
    if(errcode) sds_printf("\nERROR: In interpreter callback (%d)."/*DNT*/,errcode);

    return;
}


int lsp_new_dialog(void) {
    int msgn,gotpt,ret;
    int fi1;
    char *func;
    sds_point pt;
    sds_hdlg handle;
    struct resbuf *argp[4];
    struct lsp_dialog_handle *tp;

    msgn=-1;
    gotpt=0; func=""/*DNT*/; fi1=0;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if(argp[1]->restype!=RTSHORT && argp[1]->restype!=RTLONG) { msgn=1; goto out; }
    if(argp[1]->restype==RTSHORT) fi1=argp[1]->resval.rint;
    if(argp[1]->restype==RTLONG)  fi1=argp[1]->resval.rlong;

    argp[2]=lsp_nextarg(argp[1]);
    if(argp[2]) {
        if(argp[2]->restype!=RTSTR) { msgn=1; goto out; }
        func=argp[2]->resval.rstring;

        argp[3]=lsp_nextarg(argp[2]);
        if(argp[3]) {
            lsp_convptlist(argp[3],0);
            if (argp[3]->restype!=RT3DPOINT && argp[3]->restype!=RTPOINT)
                { msgn=1; goto out; }  /* Not a pt */
            ic_ptcpy(pt,argp[3]->resval.rpoint); gotpt=1;

            if(lsp_nextarg(argp[3])!=NULL) { msgn=9; goto out; }
        }
    }

    if(gotpt) {
        ret=dlg_new_positioned_dialog(argp[0]->resval.rstring,fi1,(CLIENTFUNC)lsp_dia_callback,(int)(pt[0]),(int)(pt[1]),&handle);
    } else {
        ret=dlg_new_dialog(argp[0]->resval.rstring,fi1,(CLIENTFUNC)lsp_dia_callback,&handle);
    }

	if ( ret == RTERROR )
		{
		msgn = 52;
		goto out;
		}

    if ((tp= new struct lsp_dialog_handle )==NULL) {
        msgn=7;  goto out;
    }
	memset(tp,0,sizeof(struct lsp_dialog_handle));

    tp->hand=handle;

    if(lsp_dialog_handle_c==NULL) {
        tp->prev=NULL;
        lsp_dialog_handle_c=tp;
    } else {
        tp->prev=lsp_dialog_handle_c;
        lsp_dialog_handle_c=tp;
    }

    if(*func && lsp_add_cb(lsp_dialog_handle_c->hand,""/*DNT*/,func)) goto out;

    if(RTERROR==ret) lsp_retnil(); else lsp_rett();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_action_tile(void) {
    int msgn;
    struct resbuf *argp[2];
    char *func;

    msgn=-1;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if(argp[1]->restype!=RTSTR) { msgn=1; goto out; }
    func=argp[1]->resval.rstring;

    if(lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
  
	if(lsp_add_cb(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,func)) goto out;

    if(RTERROR==dlg_action_tile(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,(CLIENTFUNC)lsp_dia_callback)) lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_set_tile(void) {
    int msgn;
    struct resbuf *argp[3];

    msgn=-1;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if(argp[1]->restype!=RTSTR) { msgn=1; goto out; }

    if(lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
	//Modified Cybage SBD 30/03/2001 DD/MM/YYYY [
	//Reason : Fix for Bug No. 77761 from Bugzilla 
	//dlg_set_tile(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,argp[1]->resval.rstring);
	//lsp_retnil();
    if(RTERROR==dlg_set_tile(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,argp[1]->resval.rstring)) lsp_retnil();
    else lsp_retstr(argp[1]->resval.rstring);
	//Modified Cybage SBD 30/03/2001 DD/MM/YYYY ]

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

extern "C"/*DNT*/ void lsp_dia_setvars(sds_callback_packet *cpkt,char *key) {
    struct resbuf rb1;

    rb1.restype=RTSTR;
    rb1.resval.rstring=key;// key;
    if(lsp_defatom("$key"/*DNT*/,0,&rb1,1)) return;
    rb1.resval.rstring=cpkt->value;
    if(lsp_defatom("$value"/*DNT*/,0,&rb1,1)) return;
    rb1.resval.rstring=(char *)cpkt->client_data;
    if(lsp_defatom("$data"/*DNT*/,0,&rb1,1)) return;

    rb1.restype=RTSHORT;
    rb1.resval.rint=cpkt->reason;
    if(lsp_defatom("$reason"/*DNT*/,0,&rb1,1)) return;

    rb1.restype=RTLONG;
    rb1.resval.rlong=cpkt->x;
    if(lsp_defatom("$x"/*DNT*/,0,&rb1,1)) return;
    rb1.resval.rlong=cpkt->y;
    if(lsp_defatom("$y"/*DNT*/,0,&rb1,1)) return;

    return;
}

int lsp_add_cb(sds_hdlg hdlg, char *key, char *exec) {
    int msgn;
    struct lsp_callback_list *tp;

    msgn=-1;

    if ((tp= new struct lsp_callback_list )==NULL) {
        msgn=7;  goto out;
    }
	memset(tp,0,sizeof(struct lsp_callback_list));

    tp->next=NULL;  tp->hdlg=hdlg;

    if((tp->key= new char [strlen(key)+1] )==NULL) { msgn=7;  goto out; }
    strcpy(tp->key,key);

    if((tp->exec= new char [ strlen(exec)+10] )==NULL) { msgn=7;  goto out; }
	memset(tp->exec,0,strlen(exec)+10);
    strcpy(tp->exec,"(progn "/*DNT*/);
    strcat(tp->exec,exec);
    strcat(tp->exec,")"/*DNT*/);

    if(!lsp_callback_list_b) lsp_callback_list_b=lsp_callback_list_c=tp;
    else lsp_callback_list_c=lsp_callback_list_c->next=tp;

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_free_cb(void) {
    struct lsp_callback_list *tp;

    for(lsp_callback_list_c=lsp_callback_list_b; lsp_callback_list_c!=NULL; ) {
        tp=lsp_callback_list_c;
        lsp_callback_list_c=lsp_callback_list_c->next;
        IC_FREE(tp->key);  IC_FREE(tp->exec);  delete tp;
    }
    lsp_callback_list_b=lsp_callback_list_c=NULL;
    return 0;
}

int lsp_add_list(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if(RTNORM==dlg_add_list(lsp_argsll->resval.rstring)) lsp_retint(1);
    else lsp_retint(-1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_client_data_tile(void) {
    int msgn;
    struct resbuf *argp[2];

    msgn=-1;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if(argp[1]->restype!=RTSTR) { msgn=1; goto out; }

    if(lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    if(RTERROR==dlg_client_data_tile(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,argp[1]->resval.rstring)) lsp_retnil();
    lsp_rett();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_dim_tile(int mode) {
    int msgn;
	short x,y;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    if(RTERROR==dlg_dimensions_tile(lsp_dialog_handle_c->hand,lsp_argsll->resval.rstring,&x,&y)) lsp_retnil();
    else lsp_retint(mode ? y : x);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_dimx_tile(void) {
    return(lsp_dim_tile(0));
}

int lsp_dimy_tile(void) {
    return(lsp_dim_tile(1));
}

int lsp_get_attr(void) {
    int msgn;
    struct resbuf *argp[3];
    char val[IC_ACADBUF];

    msgn=-1;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if(argp[1]->restype!=RTSTR) { msgn=1; goto out; }

    if(lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    if(RTERROR==dlg_get_attr(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,argp[1]->resval.rstring,val,sizeof(val)-1)) lsp_retnil();
    else lsp_retstr(val);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_get_tile(void) {
    int msgn;
	//Modified	SAU 31/05/2000
	//char val[IC_ACADBUF]; 
	char val[IC_ACADBUF*6];

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    if(RTERROR==dlg_get_tile(lsp_dialog_handle_c->hand,lsp_argsll->resval.rstring,val,sizeof(val)-1)) lsp_retnil();
    else lsp_retstr(val);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_done_dialog(void) 
	{
    int msgn,stat;
    int x,y;
    struct resbuf rb;
    struct lsp_dialog_handle *tp;

    msgn=-1;
    stat=DLGOK;

    if(lsp_argsll!=NULL) 
		{
        if( lsp_argsll->restype!=RTSHORT && 
			lsp_argsll->restype!=RTLONG)
            { 
			msgn=1; 
			goto out; 
			}
        if(lsp_argsll->restype==RTSHORT) 
			{
			stat=lsp_argsll->resval.rint;
			}
        if(lsp_argsll->restype==RTLONG)  
			{
			stat=(int)lsp_argsll->resval.rlong;
			}
        if (lsp_argsll->rbnext!=NULL)   
			{ 
			msgn=9; 
			goto out; 
			}
	    }

    rb.restype=RTPOINT;  
	rb.resval.rpoint[0]=0.0;  
	rb.resval.rpoint[1]=0.0;

    if(!lsp_dialog_handle_c) 
		{ 
		lsp_retval(&rb); 
		goto out; 
		}

    if(RTERROR==dlg_done_positioned_dialog(lsp_dialog_handle_c->hand,stat,&x,&y))
        { 
		lsp_retnil(); 
		goto out; 
		}

    tp=lsp_dialog_handle_c->prev;
    delete lsp_dialog_handle_c;
    lsp_dialog_handle_c=tp;

    rb.restype=RTPOINT;  
	rb.resval.rpoint[0]=x;  
	rb.resval.rpoint[1]=y;
    lsp_retval(&rb);

out:
    if (msgn>-1) 
		{ 
		lsp_fnerrmsg(msgn); 
		return -1; 
		}
    return 0;
}

int lsp_end_image(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }
    dlg_end_image();
    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_end_list(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }
    dlg_end_list();
    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_do_image(int mode) {
    int msgn,ct,nub[5];
    struct resbuf *argp[6];

    msgn=-1;

    for(ct=0,argp[0]=lsp_argsll; ct<5; argp[ct+1]=lsp_nextarg(argp[ct]),++ct) {
        if(argp[ct]==NULL) { msgn=2; goto out; }
        if(argp[ct]->restype!=RTSHORT && argp[ct]->restype!=RTLONG) { msgn=1; goto out; }
        if(argp[ct]->restype==RTSHORT) nub[ct]=argp[ct]->resval.rint;
        if(argp[ct]->restype==RTLONG)  nub[ct]=(int)argp[ct]->resval.rlong;
    }
    if(lsp_nextarg(argp[4])!=NULL) { msgn=9; goto out; }

    if(!mode) dlg_fill_image(nub[0],nub[1],nub[2],nub[3],nub[4]);
    else    dlg_vector_image(nub[0],nub[1],nub[2],nub[3],nub[4]);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_fill_image(void) {
    return(lsp_do_image(0));
}

int lsp_vector_image(void) {
    return(lsp_do_image(1));
}

int lsp_load_dialog(void) {
    int msgn;
    int fi1;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if(RTNORM==dlg_load_dialog(lsp_argsll->resval.rstring,&fi1)) lsp_retint(fi1);
    else lsp_retint(-1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_mode_tile(void) {
    int msgn,fi1;
    struct resbuf *argp[2];

    msgn=-1; fi1=0;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))==NULL) { msgn=2; goto out; }
    if(argp[1]->restype!=RTSHORT && argp[1]->restype!=RTLONG) { msgn=1; goto out; }
    if(argp[1]->restype==RTSHORT) fi1=argp[1]->resval.rint;
    if(argp[1]->restype==RTLONG)  fi1=(int)argp[1]->resval.rlong;

    if(lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    dlg_mode_tile(lsp_dialog_handle_c->hand,argp[0]->resval.rstring,fi1);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_slide_image(void) {
    int msgn,ct,nub[4];
    struct resbuf *argp[5];
    char *sld;

    msgn=-1;

    for(ct=0,argp[0]=lsp_argsll; ct<4; argp[ct+1]=lsp_nextarg(argp[ct]),++ct) {
        if(argp[ct]==NULL) { msgn=2; goto out; }
        if(argp[ct]->restype!=RTSHORT && argp[ct]->restype!=RTLONG) { msgn=1; goto out; }
        if(argp[ct]->restype==RTSHORT) nub[ct]=argp[ct]->resval.rint;
        if(argp[ct]->restype==RTLONG)  nub[ct]=(int)argp[ct]->resval.rlong;
    }
    if((argp[4]=lsp_nextarg(argp[3]))==NULL) { msgn=2; goto out; }
    if(argp[4]->restype!=RTSTR) { msgn=1; goto out; }
    sld=argp[4]->resval.rstring;

    if(lsp_nextarg(argp[4])!=NULL) { msgn=9; goto out; }

    dlg_slide_image(nub[0],nub[1],nub[2],nub[3],sld);
    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_start_dialog(void) {
    int msgn;
    int fi1;

    msgn=-1;
    fi1=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    lsp_cb_recurs++;
    dlg_start_dialog(lsp_dialog_handle_c->hand,&fi1);
    
	if ( lsp_cb_recurs EQ 0 )
		return -1;		// quit was evaluated

	lsp_cb_recurs--;
    lsp_retint(fi1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_start_image(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    if(RTERROR==dlg_start_image(lsp_dialog_handle_c->hand,lsp_argsll->resval.rstring)) lsp_retint(1);
    else lsp_retint(-1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_start_list(void) {
    int msgn,oper,idx;
    struct resbuf *argp[3];

    msgn=-1;
    oper=LIST_NEW;
    idx=0;

    if((argp[0]=lsp_argsll)==NULL)           { msgn=2; goto out; }
    if(argp[0]->restype!=RTSTR)              { msgn=1; goto out; }

    if((argp[1]=lsp_nextarg(argp[0]))!=NULL) {
        if(argp[1]->restype!=RTSHORT && argp[1]->restype!=RTLONG) { msgn=1; goto out; }
        if(argp[1]->restype==RTSHORT) oper=argp[1]->resval.rint;
        if(argp[1]->restype==RTLONG)  oper=(int)argp[1]->resval.rlong;

        if((argp[2]=lsp_nextarg(argp[1]))!=NULL) {
            if(argp[2]->restype!=RTSHORT && argp[2]->restype!=RTLONG) { msgn=1; goto out; }
            if(argp[2]->restype==RTSHORT) idx=argp[2]->resval.rint;
            if(argp[2]->restype==RTLONG)  idx=(int)argp[2]->resval.rlong;
            if(lsp_nextarg(argp[2])!=NULL) { msgn=9; goto out; }
        }
    }

    if(!lsp_dialog_handle_c) { lsp_retnil(); goto out; }
    if(RTERROR==dlg_start_list(lsp_dialog_handle_c->hand,lsp_argsll->resval.rstring,oper,idx)) lsp_retint(1);
    else lsp_retint(-1);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_term_dialog(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }
    dlg_term_dialog();

    lsp_free_dialog_handles();

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_unload_dialog(void) {
    int msgn;
    int fi1;

    msgn=-1; fi1=0;

    if(lsp_argsll==NULL)           { msgn=2; goto out; }
    if(lsp_argsll->restype!=RTSHORT && lsp_argsll->restype!=RTLONG) {
        msgn=1; goto out;
    }
    if(lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if(lsp_argsll->restype==RTSHORT) fi1=lsp_argsll->resval.rint;
    if(lsp_argsll->restype==RTLONG)  fi1=lsp_argsll->resval.rlong;

    dlg_unload_dialog(fi1);
    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

void lsp_free_dialog_handles(void) {
    struct lsp_dialog_handle *tp;

    while(NULL!=lsp_dialog_handle_c) {
        tp=lsp_dialog_handle_c->prev;
        delete lsp_dialog_handle_c;
        lsp_dialog_handle_c=tp;
    }

	lsp_free_cb();
    return;
}



