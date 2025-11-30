//  OURBASE.H                                                        
//                                                                   
// .............................................................................
//
// Copyright (c) 1998 by Visio Corporation.  All rights reserved.
//
// The Software is subject to the license agreement that accompanies 
// or is included with the Software, which specifies the permitted 
// and prohibited uses of the Software. Any unauthorized duplication 
// or use of Visio Corporation Software, in whole or in part, in print, 
// or in any other storage and retrieval system is prohibited.
// 
// To the maximum extent permitted by applicable law, Visio Corporation
// and its suppliers disclaim any and all warranties and conditions,
// either express or implied, including, without limitation, implied
// warranties of merchantability, fitness for a particular purpose,
// title, and non-infringement, and those arising out of usage of trade
// or course of dealing, concerning these materials.  These materials
// are provided "as is" without warranty of any kind.
// .............................................................................


// Includes
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include "sds.h"

// Defines
#define ob_DCLFILE "ourbase.dcl"
#define ob_HLPFILE "ourbase.hlp"
#define ob_EXEFILE "ourbase.dll"

// Global Vars
sds_hdlg      ob_hdlg;
short         ob_dclloaded;
int           ob_dclid;

// Protos 
void  ob_about(void);
void  ob_cleanup(void);
short ob_dofuncs(void);
void  ob_dt(void);
int   ob_func1(void);
int   ob_func2(void);
int   ob_func3(void);
int   ob_func4(void);
int   ob_func5(void);
int   ob_func6(void);
int   ob_func7(void);
int   ob_func8(void);
int   ob_func9(void);
int   ob_func10(void);
int   ob_func11(void);
int   ob_func12(void);
int   ob_func13(void);
int   ob_func14(void);
int   ob_func15(void);
int   ob_func16(void);
int   ob_func17(void);
int   ob_func18(void);
int   ob_func19(void);
int   ob_func20(void);
int   ob_func21(void);
int   ob_func22(void);
int   ob_func23(void);
// 30's
int   ob_func30(void);
int   ob_func31(void);
int   ob_func32(void);
void  ob_listdxf(struct resbuf *elist);
// 40's & 50's
int   ob_func40(void);
int   ob_func41(void);
int   ob_func42(void);
int   ob_func43(void);
int   ob_func44(void);
int   ob_func45(void);
int   ob_func46(void);
int   ob_func47(void);
int   ob_func48(void);
int   ob_func49(void);
int   ob_func50(void);
// 60's 
int   ob_func60(void);
// 70's 
int   ob_func70(void);

void  ob_init(void);
short ob_loadfuncs(void);
void  ob_test(void);
