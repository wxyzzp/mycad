/* D:\ICADDEV\PRJ\LIB\ICADLIB\CONVCOLR.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:38 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 


// THIS FILE HAS BEEN GLOBALIZED!

#include "cmds.h"
#include "icadlib.h"/*DNT*/

short ic_colornum( LPCTSTR colstr) {
/*** This function converts "colstr" into a short value.
 *** Returns: success - corresponding short value.
 ***            error - 7.
 ***/

// If you give this function bogus color names, like purple or pink, it 
// doesn't return 7 as an error.  It hits that atoi(), which returns 0
// when you feed it text, and sets fi1 to 0 (which is BYBLOCK).  We 
// should fix it eventually (I think?), but we don't want to mess with
// it right now, since everything is working with it just fine.
// If you want to check for bogus color names before you hit this function
// (since this won't catch them), use ic_isvalicol(). 
    short fi1;

    if(colstr!=NULL && *colstr!=0) {
        if	   (strisame(colstr,"BYBLOCK"/*DNT*/	)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_BYB_0,"byb")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_BYBLOCK_325,"BYBlock")))     fi1=0;
        else if(strisame(colstr, "RED"/*DNT*/		)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_RED_1,"red")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_R_2,"r")))        fi1=1;
        else if(strisame(colstr, "YELLOW"/*DNT*/	)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_YEL_3,"yel")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_YELLOW_318,"Yellow")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_Y_4,"y")))        fi1=2;
        else if(strisame(colstr, "GREEN"/*DNT*/	)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_GRE_5,"gre")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_GREEN_319,"Green")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_G_6,"g")))        fi1=3;
        else if(strisame(colstr, "CYAN"/*DNT*/		)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_CYA_7,"cya")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_CYAN_320,"Cyan")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_C_8,"c")))        fi1=4;
        else if(strisame(colstr, "BLUE"/*DNT*/		)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_BLU_9,"blu")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_BLUE_321,"Blue")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_B_10,"b")))        fi1=5;
        else if(strisame(colstr, "MAGENTA"/*DNT*/	)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_MAG_11,"mag")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_MAGENTA_322,"Magenta")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_M_12,"m")))        fi1=6;
        else if(strisame(colstr, "WHITE"/*DNT*/	)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_WHI_13,"whi")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_WHITE_323,"White")) ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_W_14,"w")))        fi1=7;
        else if(strisame(colstr, "BYLAYER"/*DNT*/	)   ||
                strisame(colstr, ResourceString(IDC_CONVCOLR_BYL_15,"byl")) ||
                strisame(colstr, ResourceString(IDC_CMDS3_BYLAYER_324,"BYLayer")))     fi1=256;
        else if((fi1=shortCast(atoi(colstr)))<0 || fi1>256) fi1=7;
    } else fi1=7;

    return(fi1);
}

char *ic_colorstr(short colnum,char *resstr) {
//*** This function converts "colnum" into string value "resstr".
//*** Returns: success - Pointer to string value.
//***            error - "white".
//***
//*** NOTE: This function returns a pointer to a static local because
//***       it was designed to pass NULL to "resstr" so a tempory variable
//***       didn't have to be created in the calling function specifically
//***       for this purpose.
//***
    static char fs1[8];
    strcpy(fs1,ResourceString(IDC_CONVCOLR_INVALID_16, "Invalid" ));

    if(colnum<0 || colnum>256) { 
		strcpy(fs1,ResourceString(IDC_CONVCOLR_WHITE_17, "white" )); 
		goto bail; 
	}
    if(colnum==0) strcpy(fs1,ic_byblock);
    else if(colnum==1) strcpy(fs1,ResourceString(IDC_CONVCOLR_RED_1, "red" ));
    else if(colnum==2) strcpy(fs1,ResourceString(IDC_CONVCOLR_YELLOW_18, "yellow" ));
    else if(colnum==3) strcpy(fs1,ResourceString(IDC_CONVCOLR_GREEN_19, "green" ));
    else if(colnum==4) strcpy(fs1,ResourceString(IDC_CONVCOLR_CYAN_20, "cyan" ));
    else if(colnum==5) strcpy(fs1,ResourceString(IDC_CONVCOLR_BLUE_21, "blue" ));
    else if(colnum==6) strcpy(fs1,ResourceString(IDC_CONVCOLR_MAGENTA_22, "magenta" ));
    else if(colnum==7) strcpy(fs1,ResourceString(IDC_CONVCOLR_WHITE_17, "white" ));
    else if(colnum==256) strcpy(fs1,ic_bylayer);
    else sprintf(fs1,"%d"/*DNT*/,colnum);

    bail:

    if(resstr!=NULL) strcpy(resstr,fs1);
    return(fs1);
}


