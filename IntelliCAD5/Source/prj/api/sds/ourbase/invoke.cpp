//  INVOKE.CPP                                                        
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



#include "sds.h"
#include "string.h"


int main(int argc, char **argv) {
    short linkstat;
	short fcode;
	struct sds_resbuf *argbuf,*trb;

    sds_init(argc,argv); 
    sds_printf("\nLoading INVOKE application.");

    for(;;) {
        if ((linkstat=sds_link(RSRSLT))<0) {
             sds_printf("\nERROR: Unable to link with IntelliCAD - %d", linkstat);
             sds_exit(-1);
        }
        
        sds_printf("\nLink Status = %d",linkstat);

        switch(linkstat) {
            case RQXLOAD: 
				if (!sds_defun("C:INVOKETEST",1))  return(-1);
				break;
            case RQXUNLD: 
				break;
            case RQSUBR : 
				if((fcode=sds_getfuncode())<0) return(0);
			    switch(fcode) {
					case 1: 
						sds_printf("\nTesting sds_getargs...  ");
						for(argbuf=trb=sds_getargs(); trb!=NULL; trb=trb->rbnext) {
							sds_printf("\nThe type is %i.",trb->restype);
							switch(trb->restype){
								case RTREAL:    sds_printf("\n    The argument is a real. Its value is %f. ",trb->resval.rreal);	
												break;								
								case RTPOINT:   sds_printf("\n    The argument is a 2D point. Its x and y values are %f,%f. ",trb->resval.rpoint[0],trb->resval.rpoint[1]);	
												break;								
								case RTSHORT:   sds_printf("\n    The argument is a short. Its value is %i. ",trb->resval.rint);	
												break;		 						
								case RTSTR:     sds_printf("\n    The argument is a string.  The string is \"%s\".",trb->resval.rstring);
												break;
								case RT3DPOINT:	sds_printf("\n	  The argument is a 3D point.  Its x, y, and z values are %f,%f,%f. ",trb->resval.rpoint[0],trb->resval.rpoint[1],trb->resval.rpoint[2]);
												break;
								case RTLONG:	sds_printf("\n    The argument is a long.  It's value is %ld. ",trb->resval.rlong);
												break;
								default:		sds_printf("\n    The argument is not one of the usual types.");
							}
						}
						sds_printf("\nDone testing sds_getargs.");
						sds_relrb(argbuf);
						sds_retvoid();
						break;
				}
				break;
            case RQEND  : 
				break;
            case RQQUIT : 
				break;
            case RQSAVE :
            default     : break;
        }
    }
    return(0);
}

