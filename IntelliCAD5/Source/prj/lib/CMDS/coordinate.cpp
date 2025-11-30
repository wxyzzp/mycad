/* G:\ICADDEV\PRJ\LIB\CMDS\COORDINATE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadApi.h"

short cmd_units(void)    { 
	// TODO - Check the CMD_ERRORPROMPT messages to be sure they are still
	// accurate w/ including the new values.
	// please check Radians, Grads, & Surveyor's Units, & Scientific
	// Do compass directions work regardless of ANGBASE?
	// Why do EWNS entries come back so messed up on the next pass?

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	int fi1,fi2;
	RT ret;
    sds_real fr1;
    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf rb;

    sds_textscr();

    for( ;; ) {											
        sds_printf(ResourceString(IDC_COORDINATE__NUNIT_MODE___0, "\nUnit mode (LUNITS):" ));
        sds_printf(ResourceString(IDC_COORDINATE__N1__SCIENTIF_1, "\n1. Scientific    4.225E+01" ));
        sds_printf(ResourceString(IDC_COORDINATE__N2__DECIMAL__2, "\n2. Decimal       42.25" ));
        sds_printf(ResourceString(IDC_COORDINATE__N3__ENGINEER_3, "\n3. Engineering   3'-6.25\"" ));
        sds_printf(ResourceString(IDC_COORDINATE__N4__ARCHITEC_4, "\n4. Architectural 3'-6 1/4\"" ));
        sds_printf(ResourceString(IDC_COORDINATE__N5__FRACTION_5, "\n5. Fractional    42 1/4" ));
//        sds_printf("\n6. Mapping       Lat 32d46'17\",Lon 117d02'18\",Elv 240'");

        if(SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { 
			return(RTERROR); 
		}
        sprintf(fs1,ResourceString(IDC_COORDINATE__N_NUNIT_MODE_6, "\n\nUnit mode 1-5 <%i>: " ),rb.resval.rint);
       if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_SCIEN_7, "Scientific Decimal Engineering Architectural Fractional ~_Scientific ~_Decimal ~_Engineering ~_Architectural ~_Fractional" ))!=RTNORM) 
			return(RTERROR);
    	if(RTCAN==(ret=sds_getint(fs1,&fi1))) return(ret);
		if(ret==RTKWORD) {
			if(sds_getinput(fs2)!=RTNORM) { return(RTERROR); }
			if(strisame("SCIENTIFIC"/*DNT*/,fs2)) { 
                rb.resval.rint=1;
			} else if(strisame("DECIMAL"/*DNT*/,fs2)) { 
                rb.resval.rint=2;
			} else if(strisame("ENGINEERING"/*DNT*/,fs2)) { 
                rb.resval.rint=3;
			} else if(strisame("ARCHITECTURAL"/*DNT*/,fs2)) { 
                rb.resval.rint=4;
			} else if(strisame("FRACTIONAL"/*DNT*/,fs2)) { 
                rb.resval.rint=5;
//			} else if(strsame("Mapping",fs2))			{ rb.resval.rint=6;
			}
			if(sds_setvar("LUNITS"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
		}
        if(ret==RTNORM) {
//            if(fi1>6 || fi1<1) {	// Change back to this if we add mapping back in.
            if(fi1> 5 || fi1<1) {
				cmd_ErrorPrompt(CMD_ERR_UNITTYPE,0);
                continue;
            }
            rb.resval.rint=fi1;
            if(sds_setvar("LUNITS"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
        } 
        break;
    }

    for( ;; ) {
        if(SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
		fi1=rb.resval.rint;
        if(SDS_getvar(NULL,DB_QLUPREC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
//		if(fi1<=3 || fi1==6){		// This is in case we add mapping back in
		if(fi1<=3){
			sprintf(fs1,ResourceString(IDC_COORDINATE__NNUMBER_OF__13, "\nNumber of decimal places for LUNITS 0-8 <%i>: " ),rb.resval.rint);		
		}else{
			sprintf(fs1,ResourceString(IDC_COORDINATE__NENTER_GREA_14, "\nEnter greatest denominator to use for fractional display\n1, 2, 4, 8, 16, 32, 64, 128, or 256 <%i>: " ),(1<<rb.resval.rint));
			// How does the above use the right numbers?  (It does, but how?) 
			if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_X_1__15, "x/1|ONE x/2|TWO x/4|Four x/8|Eight x/16|Sixteen x/32|THIrtytwo x/64|SF x/128|OTE x/256|TFS ~_ONE ~_TWO ~_Four ~_Eight ~_Sixteen ~_THIrtytwo ~_SF ~_OTE ~_TFS" ))!=RTNORM) 
                return(RTERROR);
			// If we can lose the %i, we can use CMD_ERR_DENOMINATOR here.
		}
		if(RTCAN==(ret=sds_getint(fs1,&fi2))) return(ret);
		if(ret==RTKWORD) {
			if(sds_getinput(fs2)!=RTNORM) { return(RTERROR); }
			if(strisame("ONE"/*DNT*/,fs2))			            { rb.resval.rint=0;
			} else if(strisame("TWO"/*DNT*/,fs2))			    { rb.resval.rint=1;
			} else if(strisame("FOUR"/*DNT*/,fs2))		        { rb.resval.rint=2;
			} else if(strisame("EIGHT"/*DNT*/,fs2))		    { rb.resval.rint=3;
			} else if(strisame("SIXTEEN"/*DNT*/,fs2))		    { rb.resval.rint=4;
			} else if(strisame("THIRTYTWO"/*DNT*/,fs2))	    { rb.resval.rint=5;
			} else if(strisame("SF"/*DNT*/,fs2))			    { rb.resval.rint=6;
			} else if(strisame("OTE"/*DNT*/,fs2))			    { rb.resval.rint=7;
			} else if(strisame("TFS"/*DNT*/,fs2))			    { rb.resval.rint=8;
			}
			if(sds_setvar("LUPREC"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
		} else if(ret==RTNORM) {
			if(fi1<=3 || fi1==6){
				if(fi2>8 || fi2<0) {
					cmd_ErrorPrompt(CMD_ERR_DECIMALS,0);
					continue;
				}
			}else{
				if( (fi2!=1)&&(fi2!=2)&&(fi2!=4)&&
					(fi2!=8)&&(fi2!=16)&&(fi2!=32)&&
					(fi2!=64)&&(fi2!=128)&&(fi2!=256) ){
					cmd_ErrorPrompt(CMD_ERR_DENOMINATOR,0);
					continue;
				}
				switch (fi2){
					case   1:  fi2=0;break;
					case   2:  fi2=1;break;
					case   4:  fi2=2;break;
					case   8:  fi2=3;break;
					case  16:  fi2=4;break;
					case  32:  fi2=5;break;
					case  64:  fi2=6;break;
					case 128:  fi2=7;break;
					case 256:  fi2=8;break;
				}
			}
            rb.resval.rint=fi2;
            if(sds_setvar("LUPREC"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
        }
        break;
    }


	// AUNITS are confusing because (to match AutoCAD) the number the user enters is 
	// 1-5, which sets the AUNITS setvar to 0-4.
    for( ;; ) {
        sds_printf(ResourceString(IDC_COORDINATE__NANGULAR_UN_25, "\nAngular unit mode (AUNITS):" ));	
        sds_printf(ResourceString(IDC_COORDINATE__N1__DECIMAL_26, "\n1. Decimal degrees           90.0" ));
        sds_printf(ResourceString(IDC_COORDINATE__N2__DEGREES_27, "\n2. Degrees/minutes/seconds   90d0'0" ));
        sds_printf(ResourceString(IDC_COORDINATE__N3__GRADS___28, "\n3. Grads                     100.00g" ));
        sds_printf(ResourceString(IDC_COORDINATE__N4__RADIANS_29, "\n4. Radians                   1.57r" ));
        sds_printf(ResourceString(IDC_COORDINATE__N5__SURVEYO_30, "\n5. Surveyor's units          N 00d0'0\"E" ));

        if(SDS_getvar(NULL,DB_QAUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
        sprintf(fs1,ResourceString(IDC_COORDINATE__N_NANGULAR__31, "\n\nAngular unit mode 1-5 <%i>: " ),(rb.resval.rint+1));
       if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_DECI_32, "Decimal_degrees|DECimal Degrees/minutes/seconds|DMS Grads Radians Surveyor's_units|SUnits ~_DECimal ~_DMS ~_Grads ~_Radians ~_SUnits" ))!=RTNORM) 
		   return(RTERROR);
    	if(RTCAN==(ret=sds_getint(fs1,&fi1))) 
			return(ret);
		if(ret==RTKWORD) {
			if(sds_getinput(fs2)!=RTNORM) { 
				return(RTERROR); 
			}
			if(strisame("DECimal"/*DNT*/,fs2))             { rb.resval.rint=0;
			} else if(strisame("DMS"/*DNT*/,fs2))			{ rb.resval.rint=1;
			} else if(strisame("GRADS"/*DNT*/,fs2))		{ rb.resval.rint=2;
			} else if(strisame("RADIANS"/*DNT*/,fs2))      { rb.resval.rint=3;
			} else if(strisame("SUNITS"/*DNT*/,fs2))		{ rb.resval.rint=4;
			}
            if(sds_setvar("AUNITS"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
		} else if (ret==RTNORM) {
            if(fi1>5 || fi1<1) {
                cmd_ErrorPrompt(CMD_ERR_UNITTYPE,0);
                continue;
            }
            rb.resval.rint=(fi1-1);
            if(sds_setvar("AUNITS"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
        }
        break;
    }

    for( ;; ) {
        if(SDS_getvar(NULL,DB_QAUPREC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
        sprintf(fs1,ResourceString(IDC_COORDINATE__NNUMBER_OF__38, "\nNumber of decimal places for angular units 0-8 <%i>: " ),rb.resval.rint);	
		// If we can lose the %i, we can use CMD_ERR_DECIMALS here.
    	if(RTCAN==(ret=sds_getint(fs1,&fi1))) return(ret);
        if(ret==RTNORM) {
            if(fi1>8 || fi1<0) {
                cmd_ErrorPrompt(CMD_ERR_DECIMALS,0);
                continue;
            }
            rb.resval.rint=fi1;
            if(sds_setvar("AUPREC"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
        }
        break;
    }
    if(SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
	fi1=rb.resval.rint;
    if(SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }
	//angtos corrects for angbase & angdir, so correct for it
	if(fi1==0)
		rb.resval.rreal+=rb.resval.rreal;
	else
		rb.resval.rreal=0.0;
	sds_angtos(rb.resval.rreal,-1,-1,fs2);
    sprintf(fs1,ResourceString(IDC_COORDINATE__NANGLE_0_DI_39, "\nAngle 0 direction <%s>: " ),fs2);
	if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_EAST_40, "East North West South ~_East ~_North ~_West ~_South" ))!=RTNORM) 
        return(RTERROR);
	if(RTCAN==(ret=sds_getorient(NULL,fs1,&fr1))) return(ret);
	if(ret==RTKWORD) {
		if(sds_getinput(fs2)!=RTNORM) { return(RTERROR); }
		if(strisame("EAST"/*DNT*/,fs2))		    { rb.resval.rreal=0.0;
		} else if(strisame("NORTH"/*DNT*/,fs2))    { rb.resval.rreal=(IC_PI/2.0);
		} else if(strisame("WEST"/*DNT*/,fs2))		{ rb.resval.rreal=IC_PI;
		} else if(strisame("SOUTH"/*DNT*/,fs2))    { rb.resval.rreal=(3*(IC_PI/2.0));
		}
        if(sds_setvar("ANGBASE"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }  // ANGBASE is in radians
	} else if (ret==RTNORM) {
        rb.resval.rreal=fr1;
        if(sds_setvar("ANGBASE"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
    }

    sprintf(fs1,ResourceString(IDC_COORDINATE__NDO_YOU_WAN_45, "\nDo you want angles measured clockwise? <%s>: " ),fi1 ? ResourceString(IDC_COORDINATE_Y_46, "Y" ) : ResourceString(IDC_COORDINATE_N_47, "N" ));
	if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_YES__48, "Yes._Clockwise|Yes No._Counter-clockwise|No ~_Yes ~_No" ))!=RTNORM) 
        return(RTERROR);
	if(RTCAN==(ret=sds_getkword(fs1,fs2))) return(ret);
	if(ret==RTNORM){
		if(strisame("YES"/*DNT*/,fs2)) rb.resval.rint=1;
		if(strisame("NO"/*DNT*/,fs2))   rb.resval.rint=0; 
		rb.restype=RTSHORT;
		if(sds_setvar("ANGDIR"/*DNT*/,&rb)!=RTNORM) { return(RTERROR); }
	}

    sds_graphscr();

    return(RTNORM); 
}

short cmd_isoplane(void) { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf setgetrb;
	RT ret;

    if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }

	if(setgetrb.resval.rint==0){
	    if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_TOGG_51, "Toggle|TOGgle ~ Keep_using_Left|Left ~ Switch_to_Right|Right Switch_to_Top|Top ~0 ~1 ~2 ~_TOGgle ~_ ~_Left ~_ ~_Right ~_Top ~_0 ~_1 ~_2" ))!=RTNORM) 
            return(-2);
		sprintf(fs2,ResourceString(IDC_COORDINATE__NISOPLANE___52, "\nIsoplane (Left):  Left/Right/<Switch to Top>: " ));
	}else if(setgetrb.resval.rint==1){
	    if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_TOGG_53, "Toggle|TOGgle ~ Keep_using_Top|Top ~ Switch_to_Left|Left Switch_to_Right|Right ~0 ~1 ~2 ~_TOGgle ~_ ~_Top ~_ ~_Left ~_Right ~_0 ~_1 ~_2" ))!=RTNORM) 
            return(-2);
		sprintf(fs2,ResourceString(IDC_COORDINATE__NISOPLANE___54, "\nIsoplane (Top):  Top/Left/<Switch to Right>: " ));
	}else{
	    if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_TOGG_55, "Toggle|TOGgle ~ Keep_using_Right|Right ~ Switch_to_Left|Left Switch_to_Top|Top ~0 ~1 ~2 ~_TOGgle ~_ ~_Right ~_ ~_Left ~_Top ~_0 ~_1 ~_2" ))!=RTNORM) 
            return(-2);
		sprintf(fs2,ResourceString(IDC_COORDINATE__NISOPLANE___56, "\nIsoplane (Right):  Right/Top/<Switch to Left>: " ));
	}

    if((ret=sds_getkword(fs2,fs1))==RTERROR) {
        return(ret);
    } else if(ret==RTCAN) {
        return(ret);
    }

    if (strisame("LEFT" /*DNT*/,fs1) || strisame("0" /*DNT*/,fs1)) {
        setgetrb.resval.rint=0;
    } else if(strisame("TOP" /*DNT*/,fs1) || strisame("1" /*DNT*/,fs1)) {
        setgetrb.resval.rint=1;
    } else if(strisame("RIGHT" /*DNT*/,fs1) || strisame("2"/*DNT*/,fs1)) {
        setgetrb.resval.rint=2;
    } else {
        ++setgetrb.resval.rint;
        if(setgetrb.resval.rint>=3) setgetrb.resval.rint=0;
    } 

    if(sds_setvar("SNAPISOPAIR"/*DNT*/,&setgetrb)!=RTNORM) return(RTERROR);

	return(RTNORM); 
}

short cmd_coordinate(void) { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf setgetrb;
	RT ret;

    if(SDS_getvar(NULL,DB_QCOORDS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { return(RTERROR); }

	if(setgetrb.resval.rint==0){
	    if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_TOGG_60, "Toggle ~ Keep_Coordinates_Off|OFf ~ Turn_Coordinates_On|ON Use_Angle/Distance|Angle ~Distance ~0 ~1 ~2 ~_Toggle ~_ ~_OFf ~_ ~_ON ~_Angle ~_Distance ~_0 ~_1 ~_2" ))!=RTNORM) 
            return(-2);
		sprintf(fs2,ResourceString(IDC_COORDINATE__NCOORDINATE_61, "\nCoordinates are off:  ON/Angle/Distance/<OFF>: " ));
	}else if(setgetrb.resval.rint==1){
	    if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_TOGG_62, "Toggle ~ Keep_Coordinates_On|ON ~ Turn_Coordinates_Off|OFf Use_Angle/Distance|Angle ~Distance ~0 ~1 ~2 ~_Toggle ~_ ~_ON ~_ ~_OFf ~_Angle ~_Distance ~_0 ~_1 ~_2" ))!=RTNORM) 
            return(-2);
		sprintf(fs2,ResourceString(IDC_COORDINATE__NCOORDINATE_63, "\nCoordinates are on:  OFF/Angle/Distance/<ON>: " ));
	}else{
	    if(sds_initget(0,ResourceString(IDC_COORDINATE_INITGET_TOGG_64, "Toggle ~ Keep_using_Ang/Dist|Angle ~ Turn_Coordinates_On|ON Turn_Coordinates_Off|OFf ~Distance ~0 ~1 ~2 ~_Toggle ~_ ~_Angle ~_ ~_ON ~_OFf ~_Distance ~_0 ~_1 ~_2" ))!=RTNORM) 
            return(-2);
		sprintf(fs2,ResourceString(IDC_COORDINATE__NCOORDINATE_65, "\nCoordinates are set to Angle/Distance:  ON/OFF/<Angle/Distance>: " ));
	}

    if((ret=sds_getkword(fs2,fs1))==RTERROR) {
        return(ret);
    } else if(ret==RTCAN) {
        return(ret);
    } else if(ret==RTNONE) {
        return(ret);
    }

    if (strisame("OFF"/*DNT*/,fs1) || strisame("0"/*DNT*/,fs1)) {
        setgetrb.resval.rint=0;
    } else if(strisame("ON"/*DNT*/,fs1) || strisame("1"/*DNT*/,fs1)) {
        setgetrb.resval.rint=1;
    } else if(strisame("ANGLE"/*DNT*/,fs1) || strisame("DISTANCE" /*DNT*/,fs1) || strisame("2"/*DNT*/,fs1)) {
        setgetrb.resval.rint=2;
    } else if (strisame("TOGGLE"/*DNT*/,fs1)) {
        ++setgetrb.resval.rint;
        if(setgetrb.resval.rint>2) setgetrb.resval.rint=0;
    } 

    if(sds_setvar("COORDS"/*DNT*/,&setgetrb)!=RTNORM) return(RTERROR);

	return(RTNORM); 
}



