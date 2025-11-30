/* D:\ITCDEV\PRJ\LIB\ICADLIB\ICADGLOBALS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:38 $
 * 
 * Abstract
 * 
 * Sadly, globals remain a fact of life. This file at least defines a place where they can be
 * defined and at least serve as a warning to their existence.
 * 
 */ 

#ifndef	_ICADGLOBALS_H_
#define	_ICADGLOBALS_H_

								// Are we performing undo/redo processing
extern bool		SDS_CurrentlyUndoing;

								// Are we executing from lisp
extern bool		SDS_DoingLispCode;



								// Suppress screen bitmap update
extern bool		SDS_DontBitBlt;

								// Are we executing a drag operation
extern int		SDS_IsDragging;



extern struct	SDS_CmdHist *SDS_CmdHistCur;
extern bool		SDS_WasFromPipeLine;
extern int		lsp_cmdhasmore;


#endif


