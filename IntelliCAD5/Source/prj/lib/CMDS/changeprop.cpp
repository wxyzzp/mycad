/* CHANGEPROP.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the chprop and related commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "layertabrec.h"
#include "IcadApi.h"
#include "DbAcisEntity.h"
#include "Modeler.h"

// **********
// GLOBALS
//
extern struct cmd_drag_globalpacket cmd_drag_gpak;


int RemoveHatchFromSS(sds_name ss)
{
	bool hatchfound = false;
	long length;

	int error = sds_sslength(ss, &length);

	if (error != RTNORM)
		return error;

	sds_name entity;
	for (long i = 0; i < length; i++) {
		sds_ssname(ss, i, entity);
		db_handitem *hip = (db_handitem*) entity[0];

		if (hip->ret_type() == DB_HATCH) {
			sds_ssdel(entity, ss);
			hatchfound = true;

			//List is now shorter by one.  Need to back up one space in list and check the entity
			//that has filled the deleted position.  Also need to reduce length by one.
			i--;
			length--;
		}
	}

	if (hatchfound) {
		TCHAR locStr[128];
		LoadString(IcadSharedGlobals::GetIcadResourceInstance(),IDS_HATCHEDITERROR,locStr,128);
		sds_printf(locStr);
	}

	sds_sslength(ss, &length);
	if (length < 1)
		return RTNIL;

	return RTNORM;
}

short cmd_changeprops(sds_name ss) {
//*** This function gets called from cmd_change() or cmd_chprop() to change the
//*** color, layer, linetype, linetype scale factor, and thickness of entities.
//*** To change lineweight, too.
//***
//*** RETURNS:	0 - Success.
//***		   -1 - User cancel.
//***		   -2 - RTERROR returned from an ads function.
//***
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	struct resbuf *elist,*tmprb,setgetrb;
	sds_name ss1,ename;
	sds_real fr1,fr2;
	long ssct;
	short rc;
	int ret,assocflg,variesflg,fi1,fi2,highlight,chgflg,nothkflg=0;
	struct chproplist { //*** Linked list of elist pointers structure.
		sds_name ename;
		struct resbuf *entlst; //*** This is a pointer to the entire elist.
		struct resbuf *prop;   //*** This is a pointer to the specific property in the elist we are interested in.
		int elevflg;		   //*** 0==change elev; 1==ignore it; -1==ERROR
		struct chproplist *next;
	} *propbeg=NULL, *propcur=NULL, *proptmp;



	chgflg=0;  //*** This flag is to notify me that the entities need to be modified.
	if(ss!=NULL) {
		SDS_sscpy(ss1,ss);
	} else {
		//*** Get selection set of entities to change.
		if(sds_pmtssget(ResourceString(IDC_CHANGEPROP__NSELECT_ENTI_0, "\nSelect entities to change: " ),NULL,NULL,NULL,NULL,ss1,0)!=RTNORM) return(-2);

		if (RemoveHatchFromSS(ss1) == RTNIL)
		{
			rc = -1;
			goto bail;
		}

		//*** Get highlight mode.
		if(SDS_getvar(NULL,DB_QHIGHLIGHT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
		highlight=setgetrb.resval.rint;
	}

	//*** Set up the linked list with elist pointers from ss1.
	for(ssct=0L;sds_ssname(ss1,ssct,ename)==RTNORM;ssct++) {
		for(;;){
			//*** Check for a Cancel from the user.
			if(sds_usrbrk()) { rc=(-1); goto bail; }


			// check to see if the object is on a locked layer
			// If so, just skip it
			//
			db_handitem *pItem = (db_handitem *)ename[0];
			if ( pItem != NULL )
				{
				
				db_layertabrec *pLayer = (db_layertabrec *)pItem->ret_layerhip();
				ASSERT( pLayer != NULL );
				if ( pLayer != NULL )
					{
					int flags = 0;
					pLayer->get_70( &flags );

					// 0x04 means layer is locked
					//
					if ( flags & IC_LAYER_LOCKED )
						{

						// this break breaks out of the for(;;) loop
						// and back to the next ename
						//
						break;
						}
					}
				}

			if((elist=sds_entget(ename))==NULL) {
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-2); goto bail;
			}
			//*** Allocate a new link in property list.			
			if((proptmp= new struct chproplist )==NULL) {
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-2); goto bail;
			}
			memset(proptmp,0,sizeof(struct chproplist));
			if(propbeg==NULL) {
				propcur=propbeg=proptmp;
			} else {
				propcur=propcur->next=proptmp;
			}
			propcur->entlst=elist;
			propcur->ename[0] = ename[0];
			propcur->ename[1] = ename[1];

			ic_assoc(elist,0);
			if((strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/) ||
				strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
				&& RTNORM==sds_entnext_noxref(ename,ename))
				continue;
			else
				break;
		}
	}
	//*** Input loop.
	for(;;) {
		

		rc=0; variesflg=0;
		if(sds_initget(0,ResourceString(IDC_CHANGEPROP_INITGET_COLOR_1, "Color Elevation|Elevation Layer|LAyer Linetype|LType Linetype_scale|ltScale Line_Weight|LWeight Thickness ~_Color ~_Elevation ~_LAyer ~_LType ~_ltScale ~_LWeight ~_Thickness"))!=RTNORM) {
			rc=(-2);
			goto bail;
		}
		ret=sds_getkword(ResourceString(IDC_CHANGEPROP__NPROPERTY_TO_2, "\nProperty to change:  Color/Elevation/LAyer/LType/ltScale/LWeight/Thickness: " ),fs1);
		if(ret==RTERROR) {
			rc=(-2); goto bail;
		} else if(ret==RTCAN) {
			rc=(-1); goto bail;
		} else if(ret==RTNONE) {
			break;
		}

		if(strisame("COLOR"/*DNT*/,fs1)) {
			assocflg=62;
		} else if(strisame("LAYER"/*DNT*/,fs1)) {
			assocflg=8;
		} else if(strisame("LTYPE"/*DNT*/,fs1)) {
			assocflg=6;
		} else if(strisame("LTSCALE"/*DNT*/,fs1)) {
			assocflg=48;
		} else if(strisame("THICKNESS"/*DNT*/,fs1)) {
			assocflg=39;
		} else if(strisame("ELEVATION"/*DNT*/,fs1)) {
			assocflg=12;
		} else if(strisame("LWEIGHT"/*DNT*/,fs1)) {
			assocflg=370;
		}
		if(12==assocflg || 39==assocflg) {
			//*** All of the elevation/thickness code is done here.
			//*** Get current elevation/thickness.
			if(39==assocflg){
				if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);
				fr1=fr2=setgetrb.resval.rreal;
			}else{
				if(SDS_getvar(NULL,DB_QTHICKNESS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-2);
				fr1=fr2=setgetrb.resval.rreal;
			}
			//*** This loop gets the default elevation/thickness
			fi1=0;
			for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
				//*** What I am trying to acomplish in this loop is two things:
				//*** The first is to set the variesflg variable if the elevations
				//*** between entities vary.  The second is to set a flag if the Z
				//*** coordinates of one entity differ.  When this happens AutoCAD
				//*** does not allow the user to change the elevation of entities with
				//*** differing Z coordinates.
				//NOTE: we don't want to change elev on vertices of 2D plines or
				//			on SEQEND entities.  This also filters out ents for which
				//			we don't have to change elevation(see for fi1=10-18 loop below)
				ic_assoc(proptmp->entlst,0);
				if(strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)){
					ic_assoc(proptmp->entlst,70);
					if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)){
						//don't mess w/3d poly's - elev is on vertex
						proptmp->elevflg=1; //don't mess w/elev
						continue;
					}
				}else if(/*strsame(ic_rbassoc->resval.rstring,"VERTEX") || */ //entmod will have to entmod the verticies so
						 strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/) || 	 //adjust the elevation so entmod doesn't set them
						 strsame(ic_rbassoc->resval.rstring,"3DFACE"/*DNT*/) || 	 //back;
						 strsame(ic_rbassoc->resval.rstring,"3DSOLID"/*DNT*/)){
					proptmp->elevflg=1;
					continue;
				}
				if(variesflg)continue;//if it's already varying, why bother continuing to check?

				if(assocflg==12){
					ic_assoc(proptmp->entlst,0);
					for(tmprb=proptmp->entlst;tmprb!=NULL;tmprb=tmprb->rbnext){
						if(tmprb->restype>=10 && tmprb->restype<=18){
							if(tmprb->restype==11 && RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,"RAY,XLINE,ELLIPSE"/*DNT*/))
								continue;
							if(0==fi1)fr2=tmprb->resval.rpoint[2];
							else if(fr2!=tmprb->resval.rpoint[2])variesflg=1;
							fi1++;
						}
						//Modified Cybage MM 14-01-2002 DD-MM-YYYY [
						//Reason : Fix for Bug# 77956 from BugZilla
						if(tmprb->restype == 38)
						{
							if(0==fi1)fr2=tmprb->resval.rreal;
							else if(fr2!=tmprb->resval.rreal)variesflg=1;
							fi1++;
						}
						//Modified Cybage MM 14-01-2002 DD-MM-YYYY ]
					}
				}else{
					if(0!=ic_assoc(proptmp->entlst,39)){
						proptmp->prop=NULL;
						if(0==fi1)fr2=0.0;
						else if(!icadRealEqual(fr2,0.0))variesflg=1;
					}else{
						proptmp->prop=ic_rbassoc;
						if(0==fi1)fr2=ic_rbassoc->resval.rreal;
						else if(fr2!=ic_rbassoc->resval.rreal)variesflg=1;
					}
					fi1++;
				}
			}
			if(0==fi1){
				if(assocflg==39)sds_printf(ResourceString(IDC_CHANGEPROP__NNO_OBJECTS_10, "\nNo objects selected with changeable thickness." ));
				else sds_printf(ResourceString(IDC_CHANGEPROP__NNO_OBJECTS_11, "\nNo objects selected with changeable elevation." ));
				continue;
			}
			if(variesflg) {
				if(assocflg==39)sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_THICKN_12, "\nNew thickness <varies>: " ));
				else sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_ELEVAT_13, "\nNew elevation <varies>: " ));
			} else {
				sds_rtos(fr2,-1,-1,fs1);
				if(assocflg==39)sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_THICKN_14, "\nNew thickness <%s>: " ),fs1);
				else sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_ELEVAT_15, "\nNew elevation <%s>: " ),fs1);
			}
			for(;;) {
				if(sds_initget(SDS_RSG_OTHER,NULL)!=RTNORM) {
					rc=(-2);
					goto bail;
				}
				if((ret=sds_getdist(NULL,fs2,&fr1))==RTERROR) {
					rc=(-2);
					goto bail;
				} else if(ret==RTNONE || ret==RTNORM) {
					break;
				} else if(ret==RTCAN) {
					rc=(-1);
					goto bail;
				} else {
					cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
					continue;
				}
			}
			if(ret==RTNONE) continue;
			for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
				if(proptmp->elevflg!=0) continue;
				ic_assoc(proptmp->entlst,0);
				if(12==assocflg){
					for(tmprb=proptmp->entlst;tmprb!=NULL;tmprb=tmprb->rbnext){
						if(tmprb->restype<=18 && tmprb->restype>=10){
							if(tmprb->restype==11 && RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,"RAY,XLINE,ELLIPSE"/*DNT*/))
								continue;
							tmprb->resval.rpoint[2]=fr1;
						}
						//Modified Cybage MM 14-01-2002 DD-MM-YYYY [
						//Reason : Fix for Bug# 77956 from BugZilla
						if(tmprb->restype == 38)
							tmprb->resval.rreal=fr1;
						//Modified Cybage MM 14-01-2002 DD-MM-YYYY ]
					}
				}else{//thickness, assocflg==39
					if(strsame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/)){
						nothkflg|=1;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"MLINE"/*DNT*/)){
						nothkflg|=2;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"MTEXT"/*DNT*/)){
						nothkflg|=4;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"3DPOLY"/*DNT*/)){
						nothkflg|=8;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/)){
						nothkflg|=16;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"VIEWPORT"/*DNT*/)){
						nothkflg|=32;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"TOLERANCE"/*DNT*/)){
						nothkflg|=64;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"RAY"/*DNT*/)){
						nothkflg|=128;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"XLINE"/*DNT*/)){
						nothkflg|=256;continue;
					}else if(strsame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/)){
						nothkflg|=512;continue;
					}
					if(proptmp->prop!=NULL) {
						proptmp->prop->resval.rreal=fr1;
					} else {
						for(tmprb=proptmp->entlst; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
						if((tmprb->rbnext=sds_buildlist(39,fr1,0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
					}
				}
			}
			chgflg=1;
			//*** Continue because all of the elevation/thickness code gets done inside this "if" statement.
			continue;
		}
		//NO ELEVATION OR THICKNESS PROPS CHANGED BELOW - THEY'RE DONE ABOVE
		//*** Set the prop element of property linked list.
		for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
			//*** Check for a Cancel from the user.
			if(sds_usrbrk()) { rc=(-1); goto bail; }
			//*** Set the resbuf pointer to NULL if ic_assoc() failed, this may mean BYLAYER.
			if(ic_assoc(proptmp->entlst,assocflg)==0) {
				proptmp->prop=ic_rbassoc;
			} else {
				proptmp->prop=NULL;
			}
		}

		variesflg=0;
		//*** Modify the entity lists with new property values.
		switch(assocflg) {
			case 62: //*** Color
				//*** This loop gets the default color.
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop==NULL) fi1=256; else fi1=proptmp->prop->resval.rint;
					if(proptmp==propbeg) {
						fi2=fi1;
					} else if(fi2!=fi1) {
						variesflg=1; break;
					}
				}
				if(variesflg) {
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_COLOR__16, "\nNew color <varies>: " ));
				} else {
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_COLOR__17, "\nNew color <%s>: " ),ic_colorstr(fi1,NULL));
				}
				do{
					sds_initget(SDS_RSG_OTHER,ResourceString(IDC_CHANGEPROP_INITGET_BYLA_18, "BYLAYER|BYLayer BYBLOCK|BYBlock ~ Red Yellow Green Cyan Blue Magenta White ~_BYLayer ~_BYBlock ~_ ~_Red ~_Yellow ~_Green ~_Cyan ~_Blue ~_Magenta ~_White" ));
					if((ret=sds_getkword(fs2,fs1))==RTCAN) {
						rc=(-1); goto bail;
					}else if(ret==RTNONE){
						break;
					}else if(ret!=RTNORM) {
						rc=(-2); goto bail;
					}else if(ret==RTNORM){
						if(ic_isvalidcol(fs1)){
							break;
						}else{
							cmd_ErrorPrompt(CMD_ERR_COLOR0256,0);
						}
					}
				}while(1);
				if(ret==RTNONE) continue;
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop!=NULL) {
						proptmp->prop->resval.rint=ic_colornum(fs1);

#ifdef BUILD_WITH_ACIS_SUPPORT
						// we need to do more for ACIS entity
						//
						db_handitem *pItem = (db_handitem *)proptmp->ename[0];
						if (pItem->ret_type() == DB_3DSOLID || pItem->ret_type() == DB_REGION ||
							pItem->ret_type() == DB_BODY) {
							CDbAcisEntity* pAcisEntity = static_cast<CDbAcisEntity*>(pItem);

							CModeler* pModeler = CModeler::get();
							if (pModeler->checkFullModeler()) {
								const CDbAcisEntityData& olddata = pAcisEntity->getData();

								int color = proptmp->prop->resval.rint;
								if (color == 0) // BYBLOCK
									color = 7;
								else if (color == 256) { // BYLAYER
								}
							
								CDbAcisEntityData newData;

								pModeler->lockDatabase();
								if (pModeler->colorModelEntity(olddata, color, newData)) {
									resbuf* elist = sds_entget(proptmp->ename);
									ic_assoc(elist, 1);
									char*& dxf1 = ic_rbassoc->resval.rstring;
									IC_FREE(dxf1);			
									dxf1 = new char [newData.getSat().size()+1];
									strcpy(dxf1, newData.getSat().c_str());

									sds_entmod(elist);
									sds_redraw(proptmp->ename,IC_REDRAW_DRAW);
									IC_RELRB(proptmp->entlst);
									proptmp->entlst = elist;
									chgflg = false;
								}
								else {
									int ec = pModeler->getLastErrorCode();

									icl::string message;
									pModeler->getErrorMessage(ec, message);
									if (!message.empty()) {
										sds_printf("\n");
										sds_printf(message.c_str());
									}
									sds_redraw(ename,IC_REDRAW_DRAW);
								}
								pModeler->unlockDatabase();
							}
						}
#endif // BUILD_WITH_ACIS_SUPPORT
					} else {
						ic_assoc(proptmp->entlst,0);
						if (strsame(ic_rbassoc->resval.rstring,"ATTRIB")) continue;  // don't change color on attributes
						for(tmprb=proptmp->entlst; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
						if((tmprb->rbnext=sds_buildlist(62,ic_colornum(fs1),0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
					}
				}
				chgflg=1;
				break;
			case 8: //*** Layer
				for(;;) {
					//*** This loop gets the default layer.
					for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
						strncpy(fs2,proptmp->prop->resval.rstring,sizeof(fs2)-1);
						if(proptmp==propbeg) {
							strncpy(fs1,proptmp->prop->resval.rstring,sizeof(fs1)-1);
						} else if(!strisame(fs1,fs2)) {
							variesflg=1; break;
						}
					}
					if(variesflg) {
						sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_LAYER__20, "\nNew layer <varies>: " ));
					} else {
						sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_LAYER__21, "\nNew layer <%s>: " ),fs1);
					}
					sds_initget(SDS_RSG_OTHER,NULL);
					if((ret=sds_getkword(fs2,fs1))==RTCAN) {
						rc=(-1); goto bail;
					}
					//Modified Cybage MM 14-01-2002 DD-MM-YYYY [
					//Reason : Fix for Bug# 77956 from BugZilla
					else if(ret==RTNONE) {
						break;
					}
					//Modified Cybage MM 14-01-2002 DD-MM-YYYY ]
					else if(ret!=RTNORM) {
						rc=(-2); goto bail;
					}
					if(!*fs1) break;
					strupr(fs1);
					//*** Check to see if fs1 is a valid layer name.
					if((tmprb=sds_tblsearch("LAYER"/*DNT*/,fs1,0))==NULL) {
						sds_printf(ResourceString(IDC_CHANGEPROP__NCANNOT_FIN_22, "\nCannot find layer %s." ),fs1);
						continue;
					}
					IC_RELRB(tmprb);
					if(strchr(fs1,'|')) {
						sds_printf(ResourceString(IDC_CHANGEPROP__NCANNOT_CHA_23, "\nCannot change to an XREFed layer %s." ),fs1);
						continue;
					}
					for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
						if((proptmp->prop->resval.rstring=ic_realloc_char(proptmp->prop->resval.rstring,strlen(fs1)+1))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
						strcpy(proptmp->prop->resval.rstring,fs1);
					}
					chgflg=1;
					break;
				}
				//Modified Cybage MM 14-01-2002 DD-MM-YYYY
				//Reason : Fix for Bug# 77956 from BugZilla
				if(ret==RTNONE) continue;
				break;
			case 6: //*** Linetype
				//Modified Cybage MM 14-01-2002 DD-MM-YYYY
				//Reason : Fix for Bug# 77956 from BugZilla
				for(;;) {
					//*** This loop gets the default linetype.
					for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
						if(proptmp->prop==NULL) {
							strncpy(fs2,"BYLAYER"/*DNT*/,sizeof(fs2)-1);
						} else {
							strncpy(fs2,proptmp->prop->resval.rstring,sizeof(fs2)-1);
						}
						if(proptmp==propbeg) {
							strncpy(fs1,fs2,sizeof(fs1)-1);
						} else if(!strisame(fs1,fs2)) {
							variesflg=1; break;
						}
					}
					if(variesflg) {
						sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_LINETY_24, "\nNew linetype <varies>: " ));
					} else {
						sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_LINETY_25, "\nNew linetype <%s>: " ),fs1);
					}
					sds_initget(SDS_RSG_OTHER,NULL);
					if((ret=sds_getkword(fs2,fs1))==RTCAN) {
						rc=(-1); goto bail;
					}
					//Modified Cybage MM 14-01-2002 DD-MM-YYYY [
					//Reason : Fix for Bug# 77956 from BugZilla
					else if(ret==RTNONE) {
						break;
					}
					//Modified Cybage MM 14-01-2002 DD-MM-YYYY ]
					else if(ret!=RTNORM) {
						rc=(-2); goto bail;
					}
					if(!*fs1) continue;
					if(strchr(fs1,'|')) {
						sds_printf(ResourceString(IDC_CHANGEPROP__NCANNOT_CHA_26, "\nCannot change to an XREFed linetype %s." ),fs1);
						continue;
					}
					//*** Check to see if fs1 is a valid linetype name.
					if(!strisame("BYBLOCK"/*DNT*/,fs1) && !strisame("BYLAYER"/*DNT*/,fs1)){
					   if((tmprb=sds_tblsearch("LTYPE"/*DNT*/,fs1,0))==NULL) {
							//*** Load the linetype from disk
							//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
							resbuf rb;
							SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							if( cmd_ltload( fs1, ( rb.resval.rint == DB_ENGLISH_MEASUREINIT ?
								SDS_LTYPEFILE : SDS_ISOLTYPEFILE), 0) != RTNORM )
							//if( cmd_ltload(fs1, "ICAD.LIN"/*DNT*/, 0) != RTNORM &&
							//	cmd_ltload(fs1, "ICADISO.LIN"/*DNT*/, 0) != RTNORM)	/*D.G.*/// icadiso added.
							//EBATECH(CNBR)]-
							{
								sds_printf(ResourceString(IDC_CHANGEPROP__NLINETYPE___27, "\nLinetype %s not found.  Use LINETYPE command to load it." ),fs1);
								continue;
							}
						}else{
							IC_RELRB(tmprb);
						}
					}

					for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
						if(proptmp->prop!=NULL) {
							if((proptmp->prop->resval.rstring=ic_realloc_char(proptmp->prop->resval.rstring,strlen(fs1)+1))==NULL) {
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								rc=(-2); goto bail;
							}
							strcpy(proptmp->prop->resval.rstring,fs1);
						} else {
							for(tmprb=proptmp->entlst; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
							if((tmprb->rbnext=sds_buildlist(6,fs1,0))==NULL) {
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								rc=(-2); goto bail;
							}
						}
					}
					chgflg=1;
					break;
				//Modified Cybage MM 14-01-2002 DD-MM-YYYY [
				//Reason : Fix for Bug# 77956 from BugZilla
				}
				if(ret==RTNONE) continue;
				break;
				//Modified Cybage MM 14-01-2002 DD-MM-YYYY ]
			case 48: //*** Linetype scale
				//*** This loop gets the default linetype scale.
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop==NULL) fr1=1.0; else fr1=proptmp->prop->resval.rreal;
					if(proptmp==propbeg) {
						fr2=fr1;
					} else if(fr2!=fr1) {
						variesflg=1; break;
					}
				}
				if(variesflg) {
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_LINETY_28, "\nNew linetype scale <varies>: " ));
				} else {
					sds_rtos(fr1,-1,-1,fs1);
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_LINETY_29, "\nNew linetype scale <%s>: " ),fs1);
				}
				for(;;) {
					if(sds_initget(SDS_RSG_OTHER,NULL)!=RTNORM) {
						rc=(-2);
						goto bail;
					}
					if((ret=sds_getdist(NULL,fs2,&fr1))==RTERROR) {
						rc=(-2); goto bail;
					} else if(ret==RTNONE || ret==RTNORM) {
						break;
					} else if(ret==RTCAN) {
						rc=(-1); goto bail;
					} else {
						cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
						continue;
					}
				}
				if(ret==RTNONE) continue;
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop!=NULL) {
						proptmp->prop->resval.rreal=fr1;
					} else {
						for(tmprb=proptmp->entlst; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
						if((tmprb->rbnext=sds_buildlist(48,fr1,0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
					}
				}
				chgflg=1;
				break;
			/*
			case 39: //*** Thickness
				//*** This loop gets the default thickness.
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop==NULL) fr1=0.0; else fr1=proptmp->prop->resval.rreal;
					if(proptmp==propbeg) {
						fr2=fr1;
					} else if(fr2!=fr1) {
						variesflg=1; break;
					}
				}
				if(variesflg) {
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_THICKN_12, "\nNew thickness <varies>: " ));
				} else {
					sds_rtos(fr1,-1,-1,fs1);
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_THICKN_14, "\nNew thickness <%s>: " ),fs1);
				}
				for(;;) {
					if(sds_initget(RSG_OTHER,NULL)!=RTNORM) {
					   rc=(-2);
					   goto bail;
					}
					if((ret=sds_getdist(NULL,fs2,&fr1))==RTERROR) {
						rc=(-2); goto bail;
					} else if(ret==RTNONE || ret==RTNORM) {
						break;
					} else if(ret==RTCAN) {
						rc=(-1); goto bail;
					} else {
						cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
						continue;
					}
				}
				if(ret==RTNONE) continue;
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop!=NULL) {
						proptmp->prop->resval.rreal=fr1;
					} else {
						for(tmprb=proptmp->entlst; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
						if((tmprb->rbnext=sds_buildlist(39,fr1,0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
					}
				}
				chgflg=1;
				break;
				*/
			// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
			case 370: //*** Lweight
				//*** This loop gets the default lineweight.
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop==NULL) fi1=DB_DFWEIGHT; else fi1=proptmp->prop->resval.rint;
					if(proptmp==propbeg) {
						fi2=fi1;
					} else if(fi2!=fi1) {
						variesflg=1; break;
					}
				}
				if(variesflg) {
					sprintf(fs2,ResourceString(IDC_CHANGEPROP_NNEW_LWEIGHT_1, "\nNew Lineweight(-3 to 211) <varies>: " ));
				} else {
					sprintf(fs2,ResourceString(IDC_CHANGEPROP_NNEW_LWEIGHT_2, "\nNew Lineweight(-3 to 211) <%d>: " ),fi1);
				}
				do{
					if((ret=sds_getint(fs2,&fi2))==RTCAN) {
						rc=(-1); goto bail;
					}else if(ret==RTNONE){
						fi2=fi1;
					}else if(ret!=RTNORM) {
						rc=(-2); goto bail;
					}
					if( db_is_validweight(fi2, (DB_BLWMASK|DB_BBWMASK|DB_DFWMASK))) {
						break;
					}
					cmd_ErrorPrompt(CMD_ERR_LWEIGHT,0);
				}while(1);
				if(ret==RTNONE) continue;
				for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
					if(proptmp->prop!=NULL) {
						proptmp->prop->resval.rint=fi2;
					} else {
						for(tmprb=proptmp->entlst; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
						if((tmprb->rbnext=sds_buildlist(370,fi2,0))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
					}
				}
				chgflg=1;
				break;
				// EBATECH(CNBR) ]-
		}
	}
	if(chgflg) {
		fi1=0;
		//*** Modify the entities in the selection set.
		for(proptmp=propbeg; proptmp!=NULL; proptmp=proptmp->next) {
			//*** Check for entities with differing Z coordinates (elevation only).
/*			This is never called so i commented it out.
			if(proptmp->elevflg==-1) {
				if(!fi1) {
					cmd_ErrorPrompt(CMD_ERR_ELEVDIFFZ,0);
					fi1=1; //*** Set this flag so the message only gets displayed once.
				}
				continue;
			}
			//*** Modify entity.
*/
			ic_assoc(proptmp->entlst, 0);
			if (strisame(ic_rbassoc->resval.rstring, "DIMENSION"/*DNT*/))
				{
				struct sds_resbuf rbb;
				SDS_getvar(NULL,DB_QDIMSTYLE,&rbb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

				ic_assoc(proptmp->entlst, 3);
				struct sds_resbuf *thisDimsStyle = sds_tblsearch("DIMSTYLE"/*DNT*/,ic_rbassoc->resval.rstring,0);
				cmd_setActiveDimStyle (thisDimsStyle);
				IC_RELRB(thisDimsStyle);

				if(sds_entmod(proptmp->entlst)!=RTNORM)
					{
					IC_FREE(rbb.resval.rstring);
					rc=(-2); goto bail;
					}


				struct sds_resbuf *DimStyleToRestore = sds_tblsearch("DIMSTYLE"/*DNT*/,rbb.resval.rstring,0);
				cmd_setActiveDimStyle (DimStyleToRestore);
				IC_RELRB(DimStyleToRestore);
				IC_FREE(rbb.resval.rstring);

				}
			else if(sds_entmod(proptmp->entlst)!=RTNORM) {
				rc=(-2); goto bail;
			}
		}
	}

	if(nothkflg&1)	 sds_printf(ResourceString(IDC_CHANGEPROP__NSPLINE_CAN_30, "\nSPLINE cannot have thickness." ));
	if(nothkflg&2)	 sds_printf(ResourceString(IDC_CHANGEPROP__NMLINE_CANN_31, "\nMLINE cannot have thickness." ));
	if(nothkflg&4)	 sds_printf(ResourceString(IDC_CHANGEPROP__NMTEXT_CANN_32, "\nMTEXT cannot have thickness." ));
	if(nothkflg&8)	 sds_printf(ResourceString(IDC_CHANGEPROP__N3DPOLY_CAN_33, "\n3DPOLY cannot have thickness." ));
	if(nothkflg&16)  sds_printf(ResourceString(IDC_CHANGEPROP__NDIMENSION__34, "\nDIMENSION cannot have thickness." ));
	if(nothkflg&32)  sds_printf(ResourceString(IDC_CHANGEPROP__NVIEWPORT_C_35, "\nVIEWPORT cannot have thickness." ));
	if(nothkflg&64)  sds_printf(ResourceString(IDC_CHANGEPROP__NTOLERANCE__36, "\nTOLERANCE cannot have thickness." ));
	if(nothkflg&128) sds_printf(ResourceString(IDC_CHANGEPROP__NRAY_CANNOT_37, "\nRAY cannot have thickness." ));
	if(nothkflg&256) sds_printf(ResourceString(IDC_CHANGEPROP__NXLINE_CANN_38, "\nXLINE cannot have thickness." ));
	if(nothkflg&512) sds_printf(ResourceString(IDC_CHANGEPROP__NELLIPSE_CA_39, "\nELLIPSE cannot have thickness." ));

bail:

	//*** Free the prop linked list.
	for(proptmp=propbeg; proptmp!=NULL; proptmp=propcur) {
		propcur=proptmp->next;
		IC_RELRB(proptmp->entlst);
		IC_FREE(proptmp);
	}
	if(!chgflg && highlight) {
		//*** Unhighlight selection set of entities if entities did not get changed.
		for(ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) sds_redraw(ename,IC_REDRAW_UNHILITE);
	}
	sds_ssfree(ss1);
	return(rc);
}

short cmd_change(void) {
//*** This function changes the properties of existing entities.
//***
//*** RETURNS:	0 - Success.
//***		   -1 - User cancel.
//***		   -2 - RTERROR returned from an ads function.
//***

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	static char changeray;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	struct cmd_drag_globalpacket *dgpak;
	struct cmd_elistll *entbeg,*entcur,*enttmp;
	struct resbuf rb,setgetrb,*tmprb[2],rbucs,rbent;
	sds_name ss1,sstmp;
	sds_point pt1,pt2,chgpt,ptemp;
	sds_real fr1,fr2,snapang;
	sds_matrix mtx;
	RT ret,rc;
	long ssct;
	bool gotone=false;
	int highlight,modflg,fi1,fi2,texthalign,orthomode,msgflg,attdef,dragmode,gotpoint,sstyle;
	db_handitem *elp=NULL;

	dgpak=&cmd_drag_gpak;
	dgpak->reference=0; dgpak->refval=1.0;
	entbeg=entcur=enttmp=NULL; tmprb[0]=tmprb[1]=NULL;
	rc=modflg=msgflg=0;

	//default rbent to wcs, and change if needed.
	rbucs.restype=rbent.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbent.resval.rint=0;

	//*** Get selection set of entities to change.
	//NOTE: can't filter ents here because PROPERTIES option works on all ents
	if((ret=sds_pmtssget(ResourceString(IDC_CHANGEPROP__NSELECT_ENTI_0, "\nSelect entities to change: " ),NULL,NULL,NULL,NULL,ss1,0))==RTERROR)return(-2);
	else if(ret==RTCAN) return(-1);
	else if(ret!=RTNORM) return(-2);

	if (RemoveHatchFromSS(ss1) == RTNIL)
	{
		rc = -1;
		goto bail;
	}

	//*** Get highlight mode.
	if(SDS_getvar(NULL,DB_QHIGHLIGHT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	highlight=setgetrb.resval.rint;
	if(SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	dragmode=setgetrb.resval.rint;
	if(SDS_getvar(NULL,DB_QSNAPSTYL,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	if(setgetrb.resval.rint==1){
		if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
		sstyle=setgetrb.resval.rint;
	}else sstyle=-1;
	if(SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	snapang=setgetrb.resval.rreal;

	restart:
	*fs1=0;
	if(sds_initget(0,ResourceString(IDC_CHANGEPROP_INITGET_SPEC_40, "Special_entities|Entities Properties...|Properties ~Change ~_Entities ~_Properties ~_Change" ))!=RTNORM) {
		rc=(-2);
		goto bail;
	}
	if((ret=internalGetpoint(NULL,ResourceString(IDC_CHANGEPROP__NCHANGE___E_41, "\nChange:  Entities/Properties/<Change point>: " ),chgpt))==RTERROR || ret==RTCAN) {
		if(highlight){
			for(fi1=0;sds_ssname(ss1,fi1,sstmp)==RTNORM;fi1++){
				sds_redraw(sstmp,IC_REDRAW_UNHILITE);
			}
		}
		if(ret==RTERROR)rc=(-2);
		else if(ret==RTCAN)rc=(-1);
		goto bail;
	} else if(ret==RTNORM || ret==RTNONE) { //*** Change point.
		//We're going to do this in three loops -
		//	One to filter out ents not correct for the pt-change option &
		//		unhighlight all the ents.
		//	Two to change all line points.
		//	Third loop will do all the individual changes to the non-
		//		linear entities, prompting for each change individually
		if(ret!=RTNONE)
		{
			fi2=0;
			gotpoint=1;
			for(fi1=0;sds_ssname(ss1,fi1,sstmp)==RTNORM;fi1++){
				//unhighlight ALL the entities....
				if (highlight) sds_redraw(sstmp,IC_REDRAW_DRAW);
				IC_RELRB(tmprb[0]);
				tmprb[0]=sds_entget(sstmp);
				ic_assoc(tmprb[0],0);
				//NOTE:  strstr implies LINE if XLINE specified
				if(NULL==strstr("XLINE,RAY,CIRCLE,ARC,TEXT,ATTDEF,INSERT"/*DNT*/,ic_rbassoc->resval.rstring)){
					sds_ssdel(sstmp,ss1);
					fi2++;
					fi1--;	//don't increment counter
				}
			}
			sds_relrb(tmprb[0]);tmprb[0]=NULL;
			if(fi2==1)sds_printf(ResourceString(IDC_CHANGEPROP__N1_OBJECT_W_42, "\n1 object was filtered out." ));
			else if(fi2>1)sds_printf(ResourceString(IDC_CHANGEPROP__N_I_OBJECTS_43, "\n%i objects were filtered out." ),fi2);

			sds_sslength(ss1,&ssct);
			if(ssct==0L){
				cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
				rc=RTERROR;
				goto bail;
			}
			//*** Allocate the linked list of entity lists.
			if((ret=cmd_ss2elistll(&entbeg,&entcur,ss1,0))!=0) { rc=ret; goto bail; }
		   //*** Get ortho mode.
			if(SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
			orthomode=setgetrb.resval.rint;
			//***LOOP 2***
			//***For linear ents, change appropriate points to the new point.
			for(enttmp=entbeg; enttmp!=NULL; enttmp=enttmp->next) {

				if(RTNORM!=sds_wcmatch(enttmp->grpptr->resval.rstring,"LINE,RAY,XLINE"/*DNT*/)){
					continue;
				}
				gotone=true;
 				sds_trans(chgpt,&rbucs,&rbent,0,ptemp);
				if(strsame(enttmp->grpptr->resval.rstring,"RAY"/*DNT*/) ||
				   strsame(enttmp->grpptr->resval.rstring,"XLINE"/*DNT*/)){
					if(0==changeray) {
						sds_printf(ResourceString(IDC_CHANGEPROP__NCHANGE_POI_44, "\nChange point currently has no affect on Rays or Infinite Lines." ));
						continue;
					}
					else if(1==changeray){
						//change definition point
						ic_assoc(enttmp->entlst,10);
						ic_ptcpy(ic_rbassoc->resval.rpoint,ptemp);
					}else{
						//we're going to move the defining angle for the obj in 11 group
						ic_assoc(enttmp->entlst,10);
						tmprb[0]=ic_rbassoc;
						if(orthomode){
							sds_trans(tmprb[0]->resval.rpoint,&rbent,&rbucs,0,pt1);
							ic_ortho(pt1,snapang,ptemp,pt2,sstyle);
							sds_trans(pt2,&rbucs,&rbent,0,pt2);
						}else{
							ic_ptcpy(pt2,ptemp);
						}
						ic_ptcpy(pt1,tmprb[0]->resval.rpoint);
						if((fr1=sds_distance(pt1,pt2))<CMD_FUZZ)continue;
						ic_assoc(enttmp->entlst,11);
						ic_rbassoc->resval.rpoint[0]=(pt2[0]-pt1[0])/fr1;
						ic_rbassoc->resval.rpoint[1]=(pt2[1]-pt1[1])/fr1;
						ic_rbassoc->resval.rpoint[2]=(pt2[2]-pt1[2])/fr1;
					}
				}else{
					//*** Get the endpoints of the line.
					ic_assoc(enttmp->entlst,10);
					tmprb[0]=ic_rbassoc;
					fr1=sds_distance(ic_rbassoc->resval.rpoint,ptemp);
					ic_assoc(enttmp->entlst,11);
					tmprb[1]=ic_rbassoc;
					fr2=sds_distance(ic_rbassoc->resval.rpoint,ptemp);
					if(fr2>fr1){
						//move the 10 point
						if(orthomode){
							//ortho calcs should be done in current ucs
							sds_trans(tmprb[1]->resval.rpoint,&rbent,&rbucs,0,pt2);
							ic_ortho(pt2,snapang,chgpt,pt1,sstyle);
							sds_trans(pt1,&rbucs,&rbent,0,tmprb[0]->resval.rpoint);
						}else{
							ic_ptcpy(tmprb[0]->resval.rpoint,ptemp);
						}
					}else{
						if(orthomode){
							sds_trans(tmprb[0]->resval.rpoint,&rbent,&rbucs,0,pt1);
							ic_ortho(pt1,snapang,chgpt,pt2,sstyle);
							sds_trans(pt2,&rbucs,&rbent,0,tmprb[1]->resval.rpoint);
						}else{
							ic_ptcpy(tmprb[1]->resval.rpoint,ptemp);
						}
					}
				}
				tmprb[0]=tmprb[1]=NULL;
				if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
				modflg=1;
				msgflg=1;  //we've made one entity mod for lines
			}
		}
		else
		{
			sds_sslength(ss1,&ssct);
			//*** Allocate the linked list of entity lists.
			if((ret=cmd_ss2elistll(&entbeg,&entcur,ss1,0))!=0) { rc=ret; goto bail; }
			gotpoint=0;
		}
		//***LOOP 3***
		//Now, begin to work on all the other entities....
		for(enttmp=entbeg; enttmp!=NULL; enttmp=enttmp->next) {
			//skip the entities we did in the last loop...
			if(RTNORM==sds_wcmatch(enttmp->grpptr->resval.rstring,"LINE,RAY,XLINE"/*DNT*/)){
				sds_redraw(enttmp->ename,IC_REDRAW_UNHILITE);
				continue;
			}

			elp=(db_handitem *)enttmp->ename[0];
			if(strsame("CIRCLE"/*DNT*/,enttmp->grpptr->resval.rstring) ||
			   strsame("ARC"/*DNT*/,enttmp->grpptr->resval.rstring)) {
				//*** If there is only one entity in the selection set, and a change
				//*** point was given, calculate the new radius.
				gotone=true;
				modflg=1;
				if(ssct==1L && 1==gotpoint) {
					if(0==ic_assoc(enttmp->entlst,210)){
						rbent.restype=RT3DPOINT;
						ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
					}
					sds_trans(chgpt,&rbucs,&rbent,0,ptemp);
					if(ic_assoc(enttmp->entlst,10)!=0) { rc=(-2); goto bail; }
					fr1=cmd_dist2d(ptemp,ic_rbassoc->resval.rpoint);
					//*** Set the new radius.
					if(ic_assoc(enttmp->entlst,40)!=0) { rc=(-2); goto bail; }
					ic_rbassoc->resval.rreal=fr1;
					if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
				} else {
					if(1==msgflg) {
						//*** If there is more than one entity in the selection set, ignore change point.
						sds_printf(ResourceString(IDC_CHANGEPROP__NCHANGE_POI_45, "\nChange point ignored for remaining entities." ));
					}
					sds_redraw(enttmp->ename,IC_REDRAW_UNDRAW);
					//*** Get the new radius for the circle/arc
					for(;;) {
						sds_real origrad;
						elp->get_40(&origrad);
						if(strsame(enttmp->grpptr->resval.rstring,"ARC"/*DNT*/)){
							ret=SDS_dragobject(-2,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_40(),
								0.0,ResourceString(IDC_CHANGEPROP__NNEW_RADIUS_46, "\nNew radius for arc: " ),(struct resbuf **)elp->retp_10(),pt1,NULL);
							if(ret==RTNORM)elp->set_40(pt1[0]);
						}else{
							ret=SDS_dragobject(-2,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_40(),
								0.0,ResourceString(IDC_CHANGEPROP__NNEW_RADIUS_47, "\nNew radius for circle: " ),(struct resbuf **)elp->retp_10(),pt1,NULL);
							if(ret==RTNORM)elp->set_40(pt1[0]);
						}
						if(ret!=RTNORM)
							{
							elp->set_40(origrad);
							sds_entmod(enttmp->entlst);
							}
						if(ret==RTERROR) {
							rc=(-2); goto bail;
						} else if(ret==RTCAN) {
							rc=(-1); goto bail;
						} else if(ret==RTNONE) {
							break;
						} else if(ret==RTNORM) {
							//sds_redraw(enttmp->ename,IC_REDRAW_DRAW) doesn't work
							sds_entupd(enttmp->ename);
							break;
						} else {
							cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
							continue;
						}
						break;
					}
					msgflg++;
				}
				continue;
			} else if(strisame("TEXT"/*DNT*/,enttmp->grpptr->resval.rstring) ||
					  strisame("ATTDEF"/*DNT*/,enttmp->grpptr->resval.rstring)) {
				gotone=true;
				modflg=1;
				if(strisame("ATTDEF"/*DNT*/,enttmp->grpptr->resval.rstring))
					attdef=1;
				else
					attdef=0;
				//*** Text insertion point.
				//*** If there is only one entity in the selection set, and a change
				//*** point was given, use it for the new insertion point of the text.
				for(tmprb[0]=enttmp->entlst;tmprb[0]->restype!=72&&tmprb[0]!=NULL;tmprb[0]=tmprb[0]->rbnext);
				if(tmprb[0]->restype==72)texthalign=tmprb[0]->resval.rint;
				if(ssct==1L && gotpoint){
					fi1=texthalign=0;
					for(tmprb[0]=enttmp->entlst;tmprb[0]!=NULL;tmprb[0]=tmprb[0]->rbnext){
						if(tmprb[0]->restype==10) ic_ptcpy(pt1,tmprb[0]->resval.rpoint);
						else if(tmprb[0]->restype==11)ic_ptcpy(pt2,tmprb[0]->resval.rpoint);
						else if(tmprb[0]->restype==73)fi1=tmprb[0]->resval.rint;
						else if(tmprb[0]->restype==210){
							rbent.restype=RT3DPOINT;
							ic_ptcpy(rbent.resval.rpoint,tmprb[0]->resval.rpoint);
						}
					}
					sds_trans(chgpt,&rbucs,&rbent,0,ptemp);
					if(5==texthalign || 3==texthalign){
						//fitted text, so both pt1 & pt2 defined
						if(sds_distance(ptemp,pt2)<sds_distance(ptemp,pt1))
							ic_assoc(enttmp->entlst,11);
						else
							ic_assoc(enttmp->entlst,10);
					}else if((fi1+texthalign)>0){
						if(ic_assoc(enttmp->entlst,11)!=0) { rc=(-2); goto bail; }
					}else{
						if(ic_assoc(enttmp->entlst,10)!=0) { rc=(-2); goto bail; }
					}
					ic_ptcpy(ic_rbassoc->resval.rpoint,ptemp);
					if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
					//*** Re-set pt1 in drag_gpak struct.
					if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
				} else {
					if(1==msgflg) {
						//*** If there is more than one entity in the selection set, ignore change point.
						sds_printf(ResourceString(IDC_CHANGEPROP__NCHANGE_POI_45, "\nChange point ignored for remaining entities." ));
					}
					sds_redraw(enttmp->ename,IC_REDRAW_UNDRAW);
					//*** Get the new insertion point for the text.
					for(;;) {
						sds_point orig10,orig11;
						elp->get_10(orig10);
						elp->get_11(orig11);
						if(attdef==0){
							int halign,valign;
							elp->get_72(&halign);
							elp->get_73(&valign);
							if(halign==0 && valign==0){
								fi1=10;
								ret=SDS_dragobject(-1,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_10(),
									 0.0,ResourceString(IDC_CHANGEPROP__NNEW_POINT__48, "\nNew point for text, or ENTER for no change: " ),NULL,pt1,NULL);
							}else{
								fi1=11;
								ret=SDS_dragobject(-1,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_11(),
									 0.0,ResourceString(IDC_CHANGEPROP__NNEW_POINT__48, "\nNew point for text, or ENTER for no change: " ),NULL,pt1,NULL);
							}
						}else{
							int halign,valign;
							elp->get_72(&halign);
							elp->get_74(&valign);
							if(halign==0 && valign==0){
								fi1=10;
								ret=SDS_dragobject(-1,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_10(),
									 0.0,ResourceString(IDC_CHANGEPROP__NNEW_POINT__49, "\nNew point for attribute text, or ENTER for no change: " ),NULL,pt1,NULL);
							}else{
								fi1=11;
								ret=SDS_dragobject(-1,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_11(),
									 0.0,ResourceString(IDC_CHANGEPROP__NNEW_POINT__49, "\nNew point for attribute text, or ENTER for no change: " ),NULL,pt1,NULL);
							}
						}
						sds_redraw(enttmp->ename,IC_REDRAW_DRAW);
						if(ret!=RTNORM)
							{
							elp->set_10(orig10); // need to restore original point prior to entmod, or the undo for this entmod,
							elp->set_11(orig11); // which is supposed to be putting the point back, will store the changed point
							sds_entmod(enttmp->entlst);
							}
						if(ret==RTERROR) {
							rc=(-2); goto bail;
						} else if(ret==RTCAN) {
							rc=(-1); goto bail;
						} else if(ret==RTNORM) {
							//take appropriate group out of elist so subsequent entmod's don't undo our change
							for(tmprb[0]=enttmp->entlst;tmprb[0]->rbnext!=NULL && tmprb[0]->rbnext->restype!=fi1;tmprb[0]=tmprb[0]->rbnext);
							if(tmprb[0]->rbnext!=NULL){
								tmprb[1]=tmprb[0]->rbnext;
								tmprb[0]->rbnext=tmprb[1]->rbnext;
								tmprb[1]->rbnext=NULL;
								IC_RELRB(tmprb[1]);tmprb[0]=NULL;
							}
							break;
						}else if(ret==RTNONE) {
							break;
						} else {
							cmd_ErrorPrompt(CMD_ERR_PT,0);
							continue;
						}
					}
					msgflg++;
				}
			  //*** Text style.
				if(ic_assoc(enttmp->entlst,7)!=0) { rc=(-2); goto bail; }

				SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(CMD_CMDECHO_PRINTF(rb.resval.rint))
//					sds_printf("\nText style: %s",ic_rbassoc->resval.rstring);
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_TEXT_S_50, "\nNew text style <%s>: " ),ic_rbassoc->resval.rstring);

				if((ret=sds_getstring(0,fs2,fs1))==RTERROR) {
					rc=(-2); goto bail;
				} else if(ret==RTCAN) {
					rc=(-1); goto bail;
				}
				if(*fs1) {
					strupr(fs1);
//					if(tmprb[0]!=NULL){sds_relrb(tmprb[0]);tmprb[0]=NULL;}
					if((tmprb[0]=sds_tblsearch("STYLE"/*DNT*/,fs1,0))==NULL) {
						sds_printf(ResourceString(IDC_CHANGEPROP__NSTYLE__S_I_51, "\nStyle %s is not defined - use STYLE command." ),fs1);
				   } else {
						if((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(fs1)+1))==NULL) {
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							rc=(-2); goto bail;
						}
						strcpy(ic_rbassoc->resval.rstring,fs1);
						//*** Change height of text to the height for the new style (if it is not zero).
						if(ic_assoc(enttmp->entlst,40)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]=ic_rbassoc;
						if(ic_assoc(tmprb[0],40)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						if(!icadRealEqual(ic_rbassoc->resval.rreal,0.0)) {
							tmprb[1]->resval.rreal=ic_rbassoc->resval.rreal;
						}
						//*** Change the oblique angle of the text to the oblique angle of the new style
						if(ic_assoc(enttmp->entlst,51)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]=ic_rbassoc;
						if(ic_assoc(tmprb[0],50)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]->resval.rreal=ic_rbassoc->resval.rreal;
						//*** Change the width factor of the text to the width factor of the new style
						if(ic_assoc(enttmp->entlst,41)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]=ic_rbassoc;
						if(ic_assoc(tmprb[0],41)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]->resval.rreal=ic_rbassoc->resval.rreal;
						//*** Change the generation flags.
						if(ic_assoc(enttmp->entlst,71)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]=ic_rbassoc;
						if(ic_assoc(tmprb[0],71)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
						tmprb[1]->resval.rint=ic_rbassoc->resval.rint;

						IC_RELRB(tmprb[0]);
						if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
						if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
					}
				}
				if(3!=texthalign){	//if text is not aligned
					//*** New height.
					//*** Do not prompt for new height if aligned or
					//***	ifstyle of entity has a height.
					if(ic_assoc(enttmp->entlst,7)!=0) { rc=(-2); goto bail; }
				   if((tmprb[0]=sds_tblsearch("STYLE"/*DNT*/,ic_rbassoc->resval.rstring,0))==NULL) { rc=(-2); goto bail; }
					if(ic_assoc(tmprb[0],40)!=0) { IC_RELRB(tmprb[0]); rc=(-2); goto bail; }
					fr1=ic_rbassoc->resval.rreal;
					IC_RELRB(tmprb[0]);
					if(icadRealEqual(fr1,0.0)) {
						sds_real origheight;
						elp->get_40(&origheight);
						if(ic_assoc(enttmp->entlst,40)!=0) { rc=(-2); goto bail; }
						if(sds_rtos(ic_rbassoc->resval.rreal,-1,-1,fs1)!=RTNORM) { rc=(-2); goto bail; }
						sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_HEIGHT_52, "\nNew height <%s>: " ),fs1);
						sds_redraw(enttmp->ename,IC_REDRAW_UNDRAW);
						//*** Get the new height for the text.
						for(;;) {
							if(attdef==0){
								int halign,valign;
								elp->get_72(&halign);
								elp->get_73(&valign);
								if(halign==0 && valign==0){
									ret=SDS_dragobject(-2,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_40(),
										 0.0,fs2,(struct resbuf **)elp->retp_10(),pt1,NULL);
								}else{
									ret=SDS_dragobject(-2,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_40(),
										 0.0,fs2,(struct resbuf **)elp->retp_11(),pt1,NULL);
								}
								if(ret==RTNORM) elp->set_40(pt1[0]);
							}else{
								int halign,valign;
								elp->get_72(&halign);
								elp->get_74(&valign);
								if(halign==0 && valign==0){
									ret=SDS_dragobject(-2,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_40(),
										 0.0,fs2,(struct resbuf **)elp->retp_10(),pt1,NULL);
								}else{
									ret=SDS_dragobject(-2,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_40(),
										 0.0,fs2,(struct resbuf **)elp->retp_11(),pt1,NULL);
								}
								if(ret==RTNORM) elp->set_40(pt1[0]);
							}
							if(ret!=RTNORM)
								{
								elp->set_40(origheight);
								sds_entmod(enttmp->entlst);
								}
							if(ret==RTERROR) {
								rc=(-2); goto bail;
							} else if(ret==RTCAN) {
								rc=(-1); goto bail;
							} else if(ret==RTNONE) {
								break;
							} else if(ret==RTSTR && ic_isnumstr(fs1,&fr1)) {
								if((fr1)<=0.0) {
									cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
									continue;
								}
							} else if(ret==RTNORM) {
								sds_entupd(enttmp->ename);
								//take appropriate group out of elist so subsequent entmod's don't undo our change
								for(tmprb[0]=enttmp->entlst;tmprb[0]->rbnext!=NULL && tmprb[0]->rbnext->restype!=40;tmprb[0]=tmprb[0]->rbnext);
								if(tmprb[0]->rbnext!=NULL){
									tmprb[1]=tmprb[0]->rbnext;
									tmprb[0]->rbnext=tmprb[1]->rbnext;
									tmprb[1]->rbnext=NULL;
									IC_RELRB(tmprb[1]);tmprb[0]=NULL;
								}
								break;
							} else {
								cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
								continue;
							}
							if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
							break;
						}
					}
				}
			  //*** New rotation angle (for non-aligned & non-fitted text
				if(3!=texthalign && 5!=texthalign){
					ic_assoc(enttmp->entlst,50);
					sds_angtos(ic_rbassoc->resval.rreal,-1,-1,fs1);
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_ROTATI_53, "\nNew rotation angle <%s>: " ),fs1);
					sds_redraw(enttmp->ename,IC_REDRAW_UNDRAW);
					//*** Get the new rotation for the text.
					for(;;) {
						sds_real origrot;
						elp->get_50(&origrot);
						if(attdef==0) {
							int halign,valign;
							elp->get_72(&halign);
							elp->get_73(&valign);
							if(halign==0 && valign==0){
								ret=SDS_dragobject(-3,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_50(),
									 0.0,fs2,(struct resbuf **)elp->retp_10(),pt1,NULL);
							}else{
								ret=SDS_dragobject(-3,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_50(),
									 0.0,fs2,(struct resbuf **)elp->retp_11(),pt1,NULL);
							}
							if(RTNORM==ret) elp->set_50(pt1[0]);
						}else{
							int halign,valign;
							elp->get_72(&halign);
							elp->get_74(&valign);
							if(halign==0 && valign==0){
								ret=SDS_dragobject(-3,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_50(),
									 0.0,fs2,(struct resbuf **)elp->retp_10(),pt1,NULL);
							}else{
								ret=SDS_dragobject(-3,dragmode,(sds_real *)enttmp->ename,(double *)elp->retp_50(),
									 0.0,fs2,(struct resbuf **)elp->retp_11(),pt1,NULL);
							}
							if(RTNORM==ret) elp->set_50(pt1[0]);
						}
						if(ret!=RTNORM)
							{
							elp->set_50(origrot);
							sds_entmod(enttmp->entlst);
							}
						if(ret==RTERROR) {
							rc=(-2); goto bail;
						} else if(ret==RTCAN) {
							rc=(-1); goto bail;
						} else if(ret==RTNONE) {
							break;
							fr1=(atof(fs1)*IC_PI/180);
						} else if(ret==RTNORM) {
							sds_entupd(enttmp->ename);
							//take appropriate group out of elist so subsequent entmod's don't undo our change
							for(tmprb[0]=enttmp->entlst;tmprb[0]->rbnext!=NULL && tmprb[0]->rbnext->restype!=50;tmprb[0]=tmprb[0]->rbnext);
							if(tmprb[0]->rbnext!=NULL){
								tmprb[1]=tmprb[0]->rbnext;
								tmprb[0]->rbnext=tmprb[1]->rbnext;
								tmprb[1]->rbnext=NULL;
								IC_RELRB(tmprb[1]);tmprb[0]=NULL;
							}
							break;
						} else {
							cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
							continue;
						}
					}
				}
			  //*** New text/tag(attdef).
				if(attdef) {
					if(ic_assoc(enttmp->entlst,2)!=0) { rc=(-2); goto bail; }
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_TAG____54, "\nNew tag <%s>: " ),ic_rbassoc->resval.rstring);
				} else {
					if(ic_assoc(enttmp->entlst,1)!=0) { rc=(-2); goto bail; }
					sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_TEXT___55, "\nNew text <%s>: " ),ic_rbassoc->resval.rstring);
				}
				tmprb[1]=ic_rbassoc;
				if((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
					rc=(-2); goto bail;
				} else if(ret==RTCAN) {
					rc=(-1); goto bail;
				}
				if(*fs1) {
					if((tmprb[1]->resval.rstring=ic_realloc_char(tmprb[1]->resval.rstring,strlen(fs1)+1))==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-2); goto bail;
					}
					strcpy(tmprb[1]->resval.rstring,fs1);
					if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
					if(highlight && attdef) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
				}
				if(!attdef) {
					//*** If the entity being changed is text, unhighlight entity and continue.
					if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_UNHILITE);
					continue;
				}
			  //*** New prompt (attdef only)
				if(ic_assoc(enttmp->entlst,3)!=0) { rc=(-2); goto bail; }
				sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_TEXT___55, "\nNew text <%s>: " ),ic_rbassoc->resval.rstring);
				tmprb[1]=ic_rbassoc;
				if((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
					rc=(-2); goto bail;
				} else if(ret==RTCAN) {
					rc=(-1); goto bail;
				}
				if(*fs1) {
					if((tmprb[1]->resval.rstring=ic_realloc_char(tmprb[1]->resval.rstring,strlen(fs1)+1))==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-2); goto bail;
					}
					strcpy(tmprb[1]->resval.rstring,fs1);
					if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
					if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
				}
			  //*** New default value (attdef only)
				if(ic_assoc(enttmp->entlst,1)!=0) { rc=(-2); goto bail; }
				sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_DEFAUL_56, "\nNew default value <%s>: " ),ic_rbassoc->resval.rstring);
				tmprb[1]=ic_rbassoc;
				if((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
					rc=(-2); goto bail;
				} else if(ret==RTCAN) {
					rc=(-1); goto bail;
				}
				if(*fs1) {
					if((tmprb[1]->resval.rstring=ic_realloc_char(tmprb[1]->resval.rstring,strlen(fs1)+1))==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						rc=(-2); goto bail;
					}
					strcpy(tmprb[1]->resval.rstring,fs1);
					if(sds_entmod(enttmp->entlst)!=RTNORM) { rc=(-2); goto bail; }
				}
			  //*** Unhighlight entity and continue;
				if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_UNHILITE);
				continue;
			} else if(strisame("INSERT"/*DNT*/,enttmp->grpptr->resval.rstring)) {
				gotone=true;
				modflg=1;
				//*** Set pt1 and refval in dgpak global structure for dragging.
				if(ic_assoc(enttmp->entlst,10)!=0) { rc=(-2); goto bail; }
				ic_ptcpy(dgpak->pt1,ic_rbassoc->resval.rpoint);
				if(ic_assoc(enttmp->entlst,210)!=0) {
					rbent.restype=RT3DPOINT;
					ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
				}

				//*** Create a selection set with this entity in it for drag().
				if(sds_ssadd(enttmp->ename,NULL,sstmp)!=RTNORM) { rc=(-2); goto bail; }
				//*** If there is only one entity in the selection set, and a change
				//*** point was given, use it for the new insertion point of the block.
				sds_trans(chgpt,&rbucs,&rbent,0,ptemp);
				if(ssct==1L && 1==gotpoint){
				//if(ssct==1L) //HUH??	can't happen!
					//*** Do an xformss() here because an INSERT has sub-entities.
					for(fi1=0; fi1<=3; fi1++) {
						for(fi2=0; fi2<3; fi2++) {	//don't need fi2 any longer
							mtx[fi1][fi2]=0;
						}

						if (fi1 < 3)
							mtx[fi1][3] = ptemp[fi1] - dgpak->pt1[fi1];
					}
					for(fi1=0; fi1<=3; fi1++) mtx[fi1][fi1]=1;

					if(SDS_progxformss(sstmp,mtx,1)!=RTNORM) { rc=(-2); goto bail; }
					//*** Re-set pt1 in drag_gpak struct.
					ic_ptcpy(dgpak->pt1,chgpt);
					if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
				} else {
					if(1==msgflg) {
						//*** If there is more than one entity in the selection set, ignore change point.
						sds_printf(ResourceString(IDC_CHANGEPROP__NCHANGE_POI_45, "\nChange point ignored for remaining entities." ));
					}
					if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
					dgpak->reference=0; dgpak->refval=0.0;
					//*** Turn rubber-banding off.
					dgpak->rband=0;
					//*** Get the new insertion point for the block.
					for(;;) {
						if((ret=cmd_drag(CMD_TRANSLATE_DRAG,sstmp,ResourceString(IDC_CHANGEPROP__NINSERTION__57, "\nInsertion point for block: " ),NULL,pt1,fs1))==RTERROR) {
							rc=(-2); goto bail;
						} else if(ret==RTCAN) {
							rc=(-1); goto bail;
						} else if(ret==RTNORM) {
							//*** Set the new block insertion point.
							//*** Do an xformss() here because an INSERT has sub-entities.
							//DON'T USE fi2 - its horiz justification & we need it later
							for(fi1=0; fi1<=3; fi1++) {
								for(fi2=0; fi2<3; fi2++) {
									mtx[fi1][fi2]=0;
								}

								if (fi1 < 3)
									mtx[fi1][3] = pt1[fi1] - dgpak->pt1[fi1];
							}
							for(fi1=0; fi1<=3; fi1++) mtx[fi1][fi1]=1;

							if(SDS_progxformss(sstmp,mtx,1)!=RTNORM) { rc=(-2); goto bail; }
							//*** Re-set pt1 in drag_gpak struct.
							ic_ptcpy(dgpak->pt1,pt1);
							if(highlight) sds_redraw(enttmp->ename,IC_REDRAW_HILITE);
							break;
						}else if(ret==RTNONE) {
							break;
						} else {
							cmd_ErrorPrompt(CMD_ERR_PT,0);
							continue;
						}
					}
					msgflg++;
				}
			  //*** New rotation angle.
				if(ic_assoc(enttmp->entlst,50)!=0) dgpak->refval=0.0;
				else dgpak->refval=ic_rbassoc->resval.rreal;
				dgpak->reference=1;
				sprintf(fs1,"%f",dgpak->refval*180.0/IC_PI);
				sprintf(fs2,ResourceString(IDC_CHANGEPROP__NNEW_ROTATI_53, "\nNew rotation angle <%s>: " ),fs1);
				dgpak->rband=1;
				//*** Get the new rotation for the block.
				for(;;) {
					if((ret=cmd_drag(CMD_ROTATE_DRAG,sstmp,fs2,NULL,pt1,fs1))==RTERROR) {
						rc=(-2); goto bail;
					} else if(ret==RTCAN) {
						rc=(-1); goto bail;
					} else if(ret==RTNONE) {
						sds_redraw(enttmp->ename,IC_REDRAW_DRAW);
						break;
					} else if(ret==RTSTR && ic_isnumstr(fs1,&fr1)) {
						fr1=(fr1*IC_PI/180);
					} else if(ret==RTNORM) {
						fr1=sds_angle(dgpak->pt1,pt1);
					} else {
						cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
						continue;
					}

					//*** Do an xformss() here because an INSERT has sub-entities.
					if(dgpak->reference) fr1-=dgpak->refval;
					mtx[0][0]=cos(fr1); mtx[0][1]=(-sin(fr1)); mtx[0][2]=0.0; mtx[0][3]=((dgpak->pt1[1]*sin(fr1))+(dgpak->pt1[0]*(1-cos(fr1))));
					mtx[1][0]=sin(fr1); mtx[1][1]=cos(fr1);    mtx[1][2]=0.0; mtx[1][3]=(-(dgpak->pt1[0]*sin(fr1))+(dgpak->pt1[1]*(1-cos(fr1))));
					mtx[2][0]=0.0;		mtx[2][1]=0.0;		   mtx[2][2]=1.0; mtx[2][3]=0.0;
					mtx[3][0]=0.0;		mtx[3][1]=0.0;		   mtx[3][2]=0.0; mtx[3][3]=1.0;
					if(SDS_progxformss(ss1,mtx,1)!=RTNORM) { rc=(-2); goto bail; }
					break;
				}
				if(sds_ssfree(sstmp)!=RTNORM) { rc=(-2); goto bail; }
				continue;

			}
			//*** Walk pointer to the SEQEND if the entity is a complex entity.
			if(ic_assoc(enttmp->entlst,66)==0 && ic_rbassoc->resval.rint==1) {
				while(!strisame(enttmp->grpptr->resval.rstring,"SEQEND"/*DNT*/)) enttmp=enttmp->next;
			}
		}
	}else if(ret==RTKWORD){
		gotone=true;
		sds_getinput(fs1);
		if(strisame(fs1,"PROPERTIES"/*DNT*/)){
			rc=cmd_changeprops(ss1);
			goto bail;
		}else{
			//entities
			if(changeray==0)			// Point change has no effect on Rays or Infinite Lines (like in AutoCAD)
				strncpy(fs1,ResourceString(IDC_CHANGEPROP__NPOINT_CHAN_59, "\nPoint change for Rays and Infinite Lines affects:  Angles/Definition points/<No change>: " ),sizeof(fs1)-1);
			else if(1==changeray)		// Point change keeps the angle and shifts Ray/XLine to new definition point.
				strncpy(fs1,ResourceString(IDC_CHANGEPROP__NPOINT_CHAN_60, "\nPoint change for Rays and Infinite Lines affects:  Angles/No change/<Definition points>: " ),sizeof(fs1)-1);
			else						// Point change keeps definition point and rotates Ray/XLine to new angle.
				strncpy(fs1,ResourceString(IDC_CHANGEPROP__NPOINT_CHAN_61, "\nPoint change for Rays and Infinite Lines affects:  No change/Definition points/<Angles>: " ),sizeof(fs1)-1);

			sds_initget(0,ResourceString(IDC_CHANGEPROP_INITGET_NO_C_62, "No_change|None Change_angles|Angles Change_definition_points|Points ~Definition ~_None ~_Angles ~_Points ~_Definition" ));
			ret=sds_getkword(fs1,fs2);
			if(ret==RTNONE)goto restart;
			if(ret!=RTNORM){rc=-1;goto bail;}
			if(strisame(fs2,"ANGLES"/*DNT*/))
				changeray=2;
			else if((strisame(fs2,"POINTS"/*DNT*/)) || (strisame(fs2,"DEFINITION"/*DNT*/)))
				changeray=1;
			else
				changeray=0;
			goto restart;
		}
	}

	cmd_checkReactors(ss1);

	bail:
	if(!modflg && highlight){
		//*** Unhighlight selection set of entities if entities did not get changed.
		for(enttmp=entbeg; enttmp!=NULL; enttmp=enttmp->next,sds_redraw(enttmp->ename,IC_REDRAW_UNHILITE));
	}
	if(gotone==false){
		cmd_ErrorPrompt(CMD_ERR_NOCHANGEABLE,0);
	}
	cmd_free_elistll(&entbeg,&entcur);
	sds_ssfree(sstmp);
	sds_ssfree(ss1);
	return(rc);
}

short cmd_chprop(void) {
//*** This function changes the color, layer, linetype, linetype scale
//*** factor, and thickness of an entity or entities.
//***
//*** RETURNS:	0 - Success.
//***		   -1 - User cancel.
//***		   -2 - RTERROR returned from an ads function.
//***
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_changeprops(NULL));
}


