/* D:\ICADDEV\PRJ\ICAD\DCLPRIVATE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1.10.1 $ $Date: 2003/03/09 00:06:30 $ $Author: master $
 * 
 * Abstract
 * 
 * Private interface with DCL.DLL
 * 
 */ 

#ifndef __DCLPRIVATE
#define __DCLPRIVATE
class ScreenObject;

extern "C"
{
int dlg_init_dialog(				                         // To Initialize the parser.
				 LPCTSTR wszPathOfBaseDCL ,					 // i:Path of IBase.dcl 
				 BOOL bGenerateErrorLog  ,				     // i:Flag set for generation of DCL log.
				 LPCTSTR wszPathForErrorLog, 			     // i:Path for Error.log to be generated. )
				 void func( sds_callback_packet*)			 // i:Dialog action callback
				 );
}

struct LISP_DCL_DATA
	{
	LPCTSTR	key;								// value for $key
	LPCTSTR	action;								// lisp action expression to evaluate
	};

#endif

/* Revision Log
 * 
 * $Log: dclprivate.h,v $
 * Revision 1.1.1.1.10.1  2003/03/09 00:06:30  master
 * Build 17 Part 1
 *
 * Revision 1.1.1.1.12.1  2002/12/23 10:59:44  cyberage
 * ACIS Scripts
 *
 * Revision 1.1.1.1  2000/01/19 13:41:58  martinw
 * ITC - initial revision
 *
 *	Revision 6.5  1999/08/23  11:57:27  PeterK
 *	Build #150
 *
 *	Revision 6.2  1999/07/13  09:29:03  PeterK
 *	Force new version
 *
 *	Revision 6.1  1999/07/08  13:18:50  PeterK
 *	Hookup LISP callbacks with new DCL
 *
 * 
 */ 

struct _sds_hdlg{
	int p;
	~_sds_hdlg(){};
	};
struct _sds_htile{
	ScreenObject* p;
	//~_sds_htile(){};
	};
