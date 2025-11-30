/* F:\BLD\PRJ\LIB\CMDS\CMDS8.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "commandqueue.h"/*DNT*/
#include "listcoll.h"/*DNT*/
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "BPolyBuilder.h"/*DNT*/
#include "GeoConvert.h"/*DNT*/
#include "DbAcisEntity.h"
#include "Modeler.h"
#include <vector>
#include <utility>
using std::vector;
using std::pair;
#include "xstd.h" //xstd::vector
#include "CmdQueryTools.h"

#if defined(SDS_NEWDB)
	#define gra_view	 gr_view
	#define gra_initview gr_initview
	#define gra_rp2ucs	 gr_rp2ucs
	#define gra_ucs2rp	 gr_ucs2rp
	#define dwg_entlink	 db_handitem
#else
/*D*/#include "dxfrw.h"
#endif
//4M Item:126
//#define __BUILD_WITH_OLD_DVIEW_CAMERA_POINT__
//<-4M Item:126


sds_real sds_dist2d (sds_point p1,sds_point p2);//proto for f'n found in sds1

//** Defines
#define CMD_DVIEWBLOCK ResourceString(IDC_CMDS8_DVIEWBLK_0, "DVIEWBLK" )

//** Global Variables
extern int	  lsp_cmdhasmore;

struct cmd_elistll_ptr {			//this struct will point to main entity's elistll ptr
	struct cmd_elistll *elistll;
	struct cmd_elistll_ptr *next;
};

//=========================================================================================
//************************************ Protos *********************************************
//=========================================================================================

//externals
extern sds_point cmd_lastpt;	//*** Stores the last point entered by line or arc command. System vars are readonly.
extern sds_real  cmd_lastangle; //*** Stores angle from last arc or line entered. System vars are readonly.
char cmd_dview_target;			//*** For Dview command, 2=rotate target, 1= (default)rotate camera, 0= value on exit so
								//		dialog won't change for call from any other cmd
sds_name cmd_dview_sset;		//*** For Dview command, set of objects to be viewed.  If all objects, both longs==0L

// internal globals for this file
//sds_real cmd_dview_xyang,cmd_dview_zang;	//XY and Z angles for DVIEW callback f'n
//short cmd_dview_cameramode;					//CAmera mode for DVIEW (0, 1, & 2 bits used)
											//Bit 0 = angle in XY plane has been locked
											//Bit 1 = angle from XY plane has been locked
											//Bit 2 = on if we're moving target, not camera


short cmd_tempcheck(void);
//short cmd_dviewcamera(sds_name sset);
//short cmd_dviewtarget(sds_name sset); NOTE: just use camera and set bit 2 on global flag
short cmd_dviewdistance(void);
short cmd_dviewpoints(void);
short cmd_dviewpan(void);
short cmd_dviewzoom(void);
short cmd_dviewtwist(void);
//BugZilla No. 78254; 12-08-2002
//short cmd_dviewclip(void);
short cmd_dviewhide(void);
short cmd_dviewoff(void);
short cmd_dviewundo(void);
short cmd_dviewexit(void);
void  cmd_pick_spot(sds_real *sa,sds_real *ea,sds_real *fr3,sds_real *fr4,sds_real *fr5,short *flag);
//short cmd_dview_mkdviewblk(void);
int   cmd_dview_dragmatrix(sds_point dragpt, sds_matrix mt);
void  cmd_matrix_multiply(sds_matrix m1, sds_matrix m2, sds_matrix mresult);
int   cmd_bpoly_buildpoly(sds_point ipt, sds_name ssbdry);
int	  cmd_bpoly_leftent(sds_point e0p0, sds_point e0p1, int e0type, sds_point intpt, sds_name ent1, sds_name ent2);
int   cmd_bpoly_ent_to_2pt(sds_name ent, sds_point p1,sds_point p2,int *type);

// statics
static
int requestYesOrNo(const char* prompt, bool& bYes);

//===========================================================================================
//==========================================================================================
//***********************Begining of functions**********************************************
//==========================================================================================
//============================================================================================

short cmd_bhatch(void)
{
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;


	ExecuteUIAction( ICAD_WNDACTION_BHATCH );
	return(RTNORM);
}

short cmd_subtract(void)
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT rc = RTNORM;
	sds_name ssBlank;
	sds_name ssTool;
	do
	{
		resbuf filter;
		filter.restype = 0;
		filter.resval.rstring = "REGION,3DSOLID,BODY"/*DNT*/;
		filter.rbnext = 0;
		long i, j;
		sds_name ename;

		vector<CDbAcisEntityData> blanks;
		for (;;)
		{
			rc = sds_pmtssget(ResourceString(IDC_CMDS8_SELECT_ACIS_OBJECT_TO_SUBTRACT_FROM, "\nSelect ACIS object to subtract from: "), 0, 0, 0, &filter, ssBlank, 1);
			if (rc != RTNORM)
				return rc;

			for (i = 0; sds_ssname(ssBlank, i, ename)==RTNORM; i++)
				sds_redraw(ename,IC_REDRAW_UNHILITE);

			for (i = 0l; RTNORM == sds_ssname(ssBlank, i, ename); i++)
			{
				CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
				// make sure the entities selected were not deleted. The sds_pmtssget() can return 
				// the deleted entities when "PREVIOUS" option was used to select the entities.
				if (!pEntity->ret_deleted()) 
					blanks.push_back(pEntity->getData());
			}
			if (blanks.size())
				break;
		}

		vector<CDbAcisEntityData> tools;
		for (;;)
		{
			rc = sds_pmtssget(ResourceString(IDC_CMDS8_SELECT_ACIS_OBJECTS_TO_SUBTRACT, "\nSelect ACIS objects to subtract: "), 0, 0, 0, &filter, ssTool, 1);
			if (rc != RTNORM)
				return rc;

			for (i = 0l; RTNORM == sds_ssname(ssTool, i, ename); i++)
			{
				CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
				// make sure the entities selected were not deleted. The sds_pmtssget() can return 
				// the deleted entities when "PREVIOUS" option was used to select the entities.
				if (!pEntity->ret_deleted())
					tools.push_back(pEntity->getData());
			}
			if (tools.size())
				break;
		}

		xstd<CDbAcisEntityData>::vector results;
		if (!CModeler::get()->subtract(blanks, tools, results))
		{
			rc = RTERROR;
			break;
		}
		assert(results.size() == blanks.size());

		// delete tool bodies before update the blank bodies in order for the screen
		// to be redrew properly in the area of overlapping. But this can not always 
		// guarantee the area will be drew properly as long as XOR technique is used 
		// to redraw the screen. SWH
		for (i = 0; RTNORM == sds_ssname(ssTool, i, ename); i++)
			sds_entdel(ename);

		for (j = 0, i = 0; RTNORM == sds_ssname(ssBlank, i, ename); i++)
		{
			CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
			if (!pEntity->ret_deleted() && !results[j].getSat().empty())
			{
				// Update blank solid
				resbuf* p = sds_entget(ename);
				ic_assoc(p, 1);
				char*& dxf1 = ic_rbassoc->resval.rstring;
				IC_FREE(dxf1);
				dxf1 = new char [results[j].getSat().size()+1];
				strcpy(dxf1, results[j++].getSat().c_str());

				sds_entmod(p);
			}
			else
			{
				sds_entdel(ename);
			}
		}
	}
	while (false);

	sds_ssfree(ssTool);
	sds_ssfree(ssBlank);

	return rc;
}

short cmd_intersect(void)
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT rc = RTNORM;
	sds_name ss;
	do
	{
		resbuf filter;
		filter.restype = 0;
		filter.resval.rstring = "REGION,3DSOLID,BODY"/*DNT*/;
		filter.rbnext = 0;

		do
		{
			rc = sds_pmtssget(ResourceString(IDC_CMDS8__NSELECT_ACIS_OBJE_6, "\nSelect ACIS object to intersect: " ),NULL,NULL,NULL,&filter,ss,1);
			if (rc != RTNORM)
				return rc;

			long nObj;
			if ((rc = sds_sslength(ss,&nObj)) != RTNORM || nObj < 2)
			{
				sds_printf(ResourceString(IDC_CMDS8_YOU_MUST_SELECT_2_OBJECTS_TO_INTERSECT, "You must select at least 2 objects to intersect"));
				continue;
			}
			break;
		}
		while (true);

		sds_name ename;
		std::vector<CDbAcisEntityData> objs;
		long i;
		for(i = 0l; RTNORM == sds_ssname(ss, i, ename); i++)
		{
			CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
			// make sure the entities selected were not deleted. The sds_pmtssget() can return 
			// the deleted entities when "PREVIOUS" option was used to select the entities.
			if (!pEntity->ret_deleted())
				objs.push_back(pEntity->getData());
		}

		CDbAcisEntityData result;
		if (!CModeler::get()->intersect(objs, result))
		{
			rc = RTERROR;
			break;
		}

		rc = sds_ssname(ss, 0, ename);
		if (rc == RTNORM)
		{
			//  Create result
			create(result);

			// Delete operands
			for (i = 0; RTNORM == sds_ssname(ss, i, ename); i++)
				sds_entdel(ename);

			// update the screen
			//
			sds_entlast(ename);
		    sds_redraw(ename, IC_REDRAW_DRAW);
		}
	}
	while (false);

	sds_ssfree(ss);

	return rc;
}

short cmd_union(void)
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT rc = RTNORM;
	sds_name ss;
	do
	{
		resbuf filter;
		filter.restype = 0;
		filter.resval.rstring = "REGION,3DSOLID,BODY"/*DNT*/;
		filter.rbnext = 0;

		do
		{
			rc = sds_pmtssget(ResourceString(IDC_CMDS8__NSELECT_ACIS_OBJE_8, "\nSelect ACIS objects to union: " ),NULL,NULL,NULL,&filter,ss,1);
			if (rc != RTNORM)
				return rc;

			long nObj;
			if ((rc = sds_sslength(ss,&nObj)) != RTNORM || nObj < 2)
			{
				sds_printf(ResourceString(IDC_CMDS8_YOU_MUST_SELECT_AT_9, "You must select at least 2 objects to union" ));
				continue;
			}
			break;
		}
		while (true);

		bool mixType = false;
		sds_name ename, ename1;
		std::vector<CDbAcisEntityData> objs;
		CDbAcisEntityData::EType prevEntityType;
		for(long i = 0l; RTNORM == sds_ssname(ss, i, ename); i++)
		{
			CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
			// make sure the entities selected were not deleted. The sds_pmtssget() can return 
			// the deleted entities when "PREVIOUS" option was used to select the entities.
			if (!pEntity->ret_deleted()) {
				const CDbAcisEntityData& entityData = pEntity->getData();
				CDbAcisEntityData::EType entityType = entityData.getType();
				if (!mixType && entityType != prevEntityType)
					mixType = true;
				objs.push_back(entityData);
				prevEntityType = entityType;
			}
		}

		CDbAcisEntityData result;
		if (!CModeler::get()->unite(objs, result))
		{
			rc = RTERROR;
			break;
		}

		rc = sds_ssname(ss, 0, ename);
		if (rc == RTNORM)
		{
			SDS_SetUndo(IC_UNDO_GROUP_BEGIN,NULL,(void*)1,NULL,SDS_CURDWG); 
			// Delete operands
			for (long i = (mixType) ? 0 : 1; RTNORM == sds_ssname(ss, i, ename1); i++)
				sds_entdel(ename1);

			if (!mixType) {
				// Update blank solid
				resbuf* p = sds_entget(ename);
				ic_assoc(p, 1);
				char*& dxf1 = ic_rbassoc->resval.rstring;
				IC_FREE(dxf1);
				dxf1 = new char [ result.getSat().size()+1 ];
				strcpy(dxf1, result.getSat().c_str());

				sds_entmod(p);	
			}
			else 
				create(result, ename);

			SDS_SetUndo(IC_UNDO_GROUP_END,NULL,(void*)1,NULL,SDS_CURDWG); 
		}
	}
	while (false);

	sds_ssfree(ss);

	return rc;
}

namespace icl
{
inline void destroy(std::pair<long,long>*){}
}

short cmd_interfere(void)
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	RT rc = RTNORM;
	sds_name ss1;
	sds_name ss2;
	do
	{
		resbuf filter;
		filter.restype = 0;
		filter.resval.rstring = "REGION,3DSOLID,BODY"/*DNT*/;
		filter.rbnext = 0;

		std::vector<CDbAcisEntityData> objs1;
		{
			rc = sds_pmtssget(ResourceString(IDC_CMDS8_SELECT_FIRST_SET, "\nSelect first set of Acis objects: "), NULL, NULL, NULL, &filter, ss1, 1);
			if (rc != RTNORM)
				return rc;

			sds_name ename;
			for (long i = 0l; RTNORM == sds_ssname(ss1, i, ename); i++)
			{
				// get sat-data
				CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
				// make sure the entities selected were not deleted. The sds_pmtssget() can return 
				// the deleted entities when "PREVIOUS" option was used to select the entities.
				if (!pEntity->ret_deleted())
					objs1.push_back(pEntity->getData());

				// unhighlight
				sds_redraw(ename, IC_REDRAW_UNHILITE);
			}
		}

		std::vector<CDbAcisEntityData> objs2;
		{
			rc = sds_pmtssget(ResourceString(IDC_CMDS8_SELECT_SECOND_SET, "\nSelect second set of Acis objects: "), NULL, NULL, NULL, &filter, ss2, 1);
			if (rc != RTNORM)
				return rc;

			sds_name ename;
			for (long i = 0l; RTNORM == sds_ssname(ss2, i, ename); i++)
			{
				// get sat data
				CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);
				// make sure the entities selected were not deleted. The sds_pmtssget() can return 
				// the deleted entities when "PREVIOUS" option was used to select the entities.
				if (!pEntity->ret_deleted())
					objs2.push_back(pEntity->getData());

				// unhighlight
				sds_redraw(ename, IC_REDRAW_UNHILITE);
			}
		}

		xstd<pair<long, long> >::vector pairs;
		xstd<CDbAcisEntityData>::vector result;
		{
			sds_printf(ResourceString(IDC_CMDS8_COPARING_SOLIDS_AGAINST_SOLIDS, "Comparing %d solids against %d solids."), objs1.size(), objs2.size());

			if (!CModeler::get()->interfere(objs1, objs2, pairs, result))
			{
				rc = RTERROR;
				break;
			}

			sds_printf(ResourceString(IDC_CMDS8_INTERFERING_PAIRS, "Interfering pairs: %d"), pairs.size());
			if (pairs.size() < 1)
				break;
		}

		// Create interferent solids?
		{
			bool bCreate;
			rc = requestYesOrNo(ResourceString(IDC_CMDS8_CREATE_INTERFERENCE_SOLIDS, "\nCreate interference solids?"), bCreate);
			if (rc != RTNORM)
				break;

			if (bCreate)
			{
				for (int i = 0, n = result.size(); i < n; i++)
				{
					sds_name ename;
					create(result[i], ename);
				}
			}
		}

		// Highlight pairs of interfering solids?
		{
			bool bHighlight;
			rc = requestYesOrNo(ResourceString(IDC_CMDS8_HIGHLIGHT_PAIRS_OF_INTERFERING_SOILDS, "\nHighlight pairs of interfering solids?"), bHighlight);
			if (rc != RTNORM)
				break;

			if (bHighlight)
			{
				long i = 0;
				long n = pairs.size();
				do
				{
					sds_name ent1, ent2;
					if ((rc = sds_ssname(ss1, pairs[i].first, ent1)) != RTNORM ||
						(rc = sds_ssname(ss2, pairs[i].second, ent2)) != RTNORM)
						break;
					(++i)%=n;

					sds_redraw(ent1, IC_REDRAW_HILITE);
					sds_redraw(ent2, IC_REDRAW_HILITE);

					rc = sds_initget(SDS_RSG_NODOCCHG, ResourceString(IDC_CMDS8_NEXT_EXIT, "Next eXit ~_Next ~_eXit"));
					if (rc != RTNORM)
						break;

					char answer[IC_ACADBUF];
					rc = sds_getkword(ResourceString(IDC_CMDS8_ENTER_AN_OPTION_NEXT_PAIR_EXIT, "Enter an option: <Next pair>/eXit: "), answer);

					sds_redraw(ent1, IC_REDRAW_UNHILITE);
					sds_redraw(ent2, IC_REDRAW_UNHILITE);

					if (rc == RTERROR || rc == RTCAN)
						break;

					if (rc != RTNONE)
					{
						if (strisame("EXIT"/*DNT*/, answer))
							break;
					}
				}
				while (true);
				if (rc != RTNORM)
					break;
			}
		}
	}
	while (false);

	sds_ssfree(ss1);
	sds_ssfree(ss2);

	return rc;
}

static
int requestYesOrNo(const char* prompt, bool& bYes)
{
	int rc = sds_initget(SDS_RSG_NODOCCHG, ResourceString(IDC_QUERYTOOLS_YESNO, "Yes No ~_Yes ~_No"));
	if (rc != RTNORM)
		return rc;

	char exprompt[IC_ACADBUF];
	strcpy(exprompt, prompt);
	strcat(exprompt, ResourceString(IDC_QUERYTOOLS_YESNO2, " Yes/<No>: "));

	char answer[IC_ACADBUF];
	rc = sds_getkword(exprompt, answer);
	if (rc == RTERROR || rc == RTCAN)
		return rc;

	if (rc != RTNONE)
	{
		bYes = strisame("YES"/*DNT*/, answer);
	}
	else
	{
		bYes = false;
	}

	return RTNORM;
}

//clears old Entity Set and gets new selection set
short cmd_getEntitySet(HanditemCollection &collection)
{
	sds_name selectionSet, entity;
	short result;

	collection.ClearList();

	//ssget does not handle "OR" filters so will need to filter later
	result = sds_pmtssget(NULL, NULL, NULL, NULL, NULL, selectionSet, 0, false, true);
//	result = sds_ssget(NULL, NULL, NULL, NULL, selectionSet);

	//sds_pmtssget not unhighlighting so added same fix in ss_get
	struct resbuf rb;
	sds_name ename;
	if(result != RTCAN)
		{
		SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint)
			{
			//*** Unhighlight selection set of entities.
			for(long ssct=0L; sds_ssname(selectionSet,ssct,ename)==RTNORM; ssct++) sds_redraw(ename,4);
			}
		}

	if (result != RTNORM)
		return result;

	long length;
	sds_sslength(selectionSet, &length);

	for (int i = 0; i < length; i++)
	{
		result = sds_ssname(selectionSet, i, entity);

		if (result == RTNORM)
		{	//don't need to filter the list for space or deleted flags since selected objects are already
			//correct
			collection.AddItem(SDS_CURDWG, (db_handitem*)(entity[0]));
		}
	}

	sds_ssfree(selectionSet);
	return result;
}

short cmd_getAllEntities(HanditemCollection &collection)
{
	resbuf tilemode;
	resbuf cvport;
	HanditemCollection::SpaceFilter space;

	db_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	db_getvar(NULL,DB_QTILEMODE,&tilemode,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if((tilemode.resval.rint == 0) && (cvport.resval.rint == 1))
		space = HanditemCollection::PAPERSPACE;

	else
		space = HanditemCollection::MODELSPACE;

	bool result = collection.CopyList(SDS_CURDWG, SDS_CURDWG->entnext_noxref(NULL), FALSE, space, TRUE, TRUE);

	if (result)
		return RTNORM;

	return RTERROR;
}

//helper function to draw new HanditemCollecton -
//*** since this resets the next pointers it is not meant for a boundary collection
void cmd_drawHICollection(HanditemCollection *selectedEnts, int howToDraw)
{
	if (selectedEnts)
	{
		db_handitem *entity = selectedEnts->GetNext(NULL);
		db_handitem *position;
		db_handitem *last;

		while (entity)
		{
			position = entity;

			// need to make next pointers of polyline point to vertices
			// and endseq and before passing to redraw.  Also do not want
			// individual vertices or endseq to be sent to redraw.
			if (entity->ret_type() == DB_POLYLINE)
			{
				last = entity;

				do {
					position = selectedEnts->GetNext(position);
					last->next = position;
					last = position;
				} while (position->ret_type() != DB_SEQEND);

				last->next = NULL;
			}

			//line will be deleted when drawing it is added to is destroyed
			cmd_redrawEntity(entity, howToDraw);

			entity = selectedEnts->GetNext(position);
		}
	}
}

void cmd_drawBoundaryCollection(HanditemCollection *selectedEnts, int howToDraw)
{
	if (selectedEnts)
	{
		db_handitem *entity = selectedEnts->GetNext(NULL);
		db_handitem *position;

		while (entity)
		{
			position = entity;

			//line will be deleted when drawing it is added to is destroyed
			cmd_redrawEntity(entity, howToDraw);

			entity = selectedEnts->GetNext(position);
		}
	}
}


short cmd_boundaryPointSelection(sds_point pt, CBoundaryBuilder &polyBuilder, HanditemCollection &boundaryEnts)
{
	//This function assumes the passed parameter, sds_point pt is in UCS
	struct resbuf rbFrom, rbTo;
	ads_point ptFrom;

	//Convert the click point from UCS to ECS
	//CGeoToDWG and CDWGToGeo now operate in ECS.  Save us from conversion headaches.

	ptFrom[0] = pt[0]; ptFrom[1] = pt[1]; ptFrom[2] = pt[2];
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 1;		//From UCS


	//Set rbTo.rpoint to the z axis of current UCS (treat this as an entity's extrusion vector)
	SdsGetUcsZ(rbTo.resval.rpoint);

	rbTo.restype = RT3DPOINT;	//To ECS
	sds_trans(ptFrom, &rbFrom, &rbTo, 0, pt);

	RC rc = polyBuilder.ClickPoint(pt[0], pt[1]);

	if (rc == SUCCESS)
	{
		bool selected;
		rc = polyBuilder.PersistMostRecentlySelected(selected);

		if (rc == SUCCESS)
		{

			if (selected)
			{
				HanditemCollection *selectedEnts = polyBuilder.getHanditemCollection();

				if (selectedEnts)
					cmd_drawHICollection(selectedEnts, IC_REDRAW_HILITE);
				else
					rc = FAILURE;
			}

			else
			{
				cmd_drawBoundaryCollection(&boundaryEnts, IC_REDRAW_UNHILITE);

				polyBuilder.Persist();

				HanditemCollection *selectedEnts = polyBuilder.getHanditemCollection();
				if (selectedEnts)
					cmd_drawHICollection(selectedEnts, IC_REDRAW_HILITE);

				else
					rc = FAILURE;
			}
		}
	}

	if (rc == E_NotAPolyline)
	{
		cmd_ErrorPrompt(CMD_ERR_BPOLYELLIPSESPLINE,0);

	}

	else if (rc != SUCCESS)
	{
		cmd_ErrorPrompt(CMD_ERR_BPOLYPATH,0);
	}

	if (rc == SUCCESS)
		return RTNORM;

	return RTERROR;
}


short cmd_boundaryAccept(CBoundaryBuilder &polyBuilder)
{
	RC rc = polyBuilder.Persist();

	if (rc == SUCCESS)
	{

		HanditemCollection *selectedEnts = polyBuilder.getHanditemCollection();

		if (selectedEnts)
		{
			//unhighlight selected entities
			cmd_drawHICollection(selectedEnts, IC_REDRAW_UNHILITE);

			db_handitem *entity = selectedEnts->GetNext(NULL);
			db_handitem *lastent = SDS_CURDWG->ret_lastent();
			int entityType;
			int undoarea = -1;

			while (entity)
			{
				// ensure entity is not destroyed when list is destroyed since it is being
				// added to the drawing.
				selectedEnts->SetDestroyHI(entity, false);

				// Add entity to drawing
				int result = SDS_CURDWG->addhanditem(entity);

				// Set the Undo.
				entityType = entity->ret_type();
				if ((result == 0) && ((entityType == DB_SEQEND) || (entityType == DB_LWPOLYLINE) || (entityType == DB_HATCH)))
				{
					SDS_SetUndo(IC_UNDO_NEW_ENT_TAB_CLS,(void *) lastent, (void *)entity, (void *) undoarea, SDS_CURDWG);

					// don't want pending polyline entities
					lastent = SDS_CURDWG->ret_lastent();
				}

				entity = selectedEnts->GetNext(entity);
			}
		}
	}

	// Set the drawing modified flag.
	SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);

	if (rc == SUCCESS)
		return RTNORM;

	return RTERROR;
}

short cmd_boundaryCancel(CBoundaryBuilder &polyBuilder)
{
	// need to redraw drawing instead of just created items in case items are different colors
	// Redrawing boundary items is acceptable for dialog since boundary cannot be changed but not
	// for -boundary command since the boundary set can be changed midstream.
	sds_redraw(NULL,IC_REDRAW_DRAW);
	return RTNORM;
}

short cmd_dashBPoly(void)
{
	sds_point pt;
	char inputStr[IC_ACADBUF];
	int result = RTNORM;

	C3Point ptNormal;
	GetUcsZ(ptNormal);

	RC polyresult;
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBPolyBuilder polyBuilder( ptNormal, polyresult);
#else
	CBPolyBuilder polyBuilder(0.00001, ptNormal, polyresult);
#endif		
	if (!SUCCEEDED(polyresult))
		return RTERROR;

	IslandOption islands = Nested_Islands;
	HanditemCollection boundaryEnts;
	bool boundaryAll = true;
	bool boundaryAllSet = false;
	bool boundarySetModified = true;
	bool pointPicked = false;

	while ((result != RTERROR) && (result != RTCAN) && (result != RTNONE))
	{
		if ((result = sds_initget(0,ResourceString(IDC_BPOLY_INIT_GET1,"Advanced ~_Advanced")))!=RTNORM)
			break;

		result = internalGetpoint(NULL,ResourceString(IDC_BPOLY_PROMPT1, "\nAdvanced/<Internal point>: "),pt);

		if (result == RTKWORD)
		{
			if ((result = sds_getinput(inputStr)) != RTNORM) {
				break;
			}

			if(strisame(inputStr, "ADVANCED"/*DNT*/)) {
				while ((result != RTERROR) && (result != RTCAN) && (result != RTNONE))
				{
					// accept object even though there is only one object for sds compatiblity
					if ((result = sds_initget(0,ResourceString(IDC_BPOLY_INIT_GET2, "Boundary Island ~Object eXit ~_Boundary ~_Island ~_Object ~_eXit")))!=RTNORM)
						break;

					//Note: Boundary can currently only create polylines so Object type needs to be left out of prompt.
					//We still need to accept and handle this case for sds inpu
					sds_getkword(ResourceString(IDC_BPOLY_PROMPT2, "\nBoundary set/Island detection/<eXit>: "), inputStr);

					if (strisame(inputStr, "BOUNDARY"/*DNT*/))
					{
						if ((result = sds_initget(0, ResourceString(IDC_BPOLY_INIT_GET3, "New All ~_New ~_All")))!=RTNORM)
							break;

						sds_getkword(ResourceString(IDC_BPOLY_PROMPT3, "\nSpecify boundary set. New/<All>: "), inputStr);


						if (strisame(inputStr, "NEW"/*DNT*/))
						{
							boundarySetModified = true;
							boundaryAll = false;
							boundaryAllSet = false;

							result = cmd_getEntitySet(boundaryEnts);
						}


						else if (strisame(inputStr, "ALL"/*DNT*/))
						{
							// get boundary set when point is selected so that we aren't copying the entire
							// set more than once if user changes mind.
							boundaryAll = true;
							boundarySetModified = true;
						}
					}

					else if (strisame(inputStr, "ISLAND"/*DNT*/))
					{
						if ((result = sds_initget(0,ResourceString(IDC_BPOLY_INIT_GET4, "Yes No Outer ~_Yes ~No ~Outer")))!=RTNORM)
							break;

						sds_getkword(ResourceString(IDC_BPOLY_PROMPT4, "\nDo you want Island Detection? No/Outer only/<Yes>: "), inputStr);

						if (strisame(inputStr, "YES"/*DNT*/))
							islands = Nested_Islands;

						else if (strisame(inputStr, "NO"/*DNT*/))
							islands = Ignore_Islands;

						if (strisame(inputStr, "OUTER"/*DNT*/))
							islands = Outer_Only;
					}

					else if (strisame(inputStr, "OBJECT"/*DNT*/))
					{
						//Accept Region even thought we don't support it since we need to accept it from acad
						if ((result = sds_initget(0,ResourceString(IDC_BPOLY_INIT_GET5, "Polyline ~Region ~_Polyline ~_Region")))!=RTNORM)
							break;

						sds_getkword(ResourceString(IDC_BPOLY_PROMPT5, "\n<Polyline>: "), inputStr);

						if (strisame(inputStr, "REGION"/*DNT*/))
							result = RTERROR; // no error message since only availble through sds
					}

					else // default or Exit just continue
						break;
				}
			}
		}

		else if (result == RTNORM) // user input a point create boundary
		{
			// Only add boundary set if boundaryAll is selected and the set has not yet been filled.
			if (boundaryAll && !boundaryAllSet)
			{
				cmd_getAllEntities(boundaryEnts);
				boundaryAllSet = true;
			}

			if (boundarySetModified)
			{


				C3Point ptNormal;
				GetUcsZ(ptNormal);

				polyBuilder.AcceptHanditemColl(SDS_CURDWG, boundaryEnts, ptNormal);
				boundarySetModified = false;
			}

			// Set island detection
			polyBuilder.SetIslandDetection(islands);

			result = cmd_boundaryPointSelection(pt, polyBuilder, boundaryEnts);

			//At least one boundary was selected.
			pointPicked = true;
		}
	}

	if ((result == RTNONE) && pointPicked) //Accept entties
	{
		result = cmd_boundaryAccept(polyBuilder);
	}

	else if (result == RTCAN)  // Cancel so unhighlight selected entities
	{
		result = cmd_boundaryCancel(polyBuilder);
	}

	return result;

	//Original Boomeranger bpoly.
/*	sds_name ssbdry;
	int ret=RTNORM;
	struct resbuf rb, *rbp1;
	long fl1;
	sds_point p1,p2,p3,p4;
	sds_real fr1,fr2;


	rbp1=NULL;

	rb.restype=0;rb.rbnext=NULL;
	rb.resval.rstring= const_cast <char*> "LINE,ARC,CIRCLE,POLYLINE,RAY,XLINE,SOLID,TRACE,3DFACE";
	ret=sds_pmtssget(ResourceString(IDC_CMDS8__NSELECT_ENTITIES_11, "\nSelect entities to use in boundary <Enter for visible screen>: " ),NULL,NULL,NULL,&rb,ssbdry,0);
	if(ret==RTCAN) return(ret);
	if(ret==RTERROR){
		//if user hit Enter to have us pick objects,
		//rather then get EVERYTHING in a huge drawing and test it all, we use the visible screen
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint&1)
			SDS_getvar(NULL,DB_QTARGET,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		else
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(p1,rb.resval.rpoint);
		struct gra_view *view=SDS_CURGRVW;
		gra_ucs2rp(p1,p4,view);
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fr1=rb.resval.rreal/2.0;
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fr2=fr1*rb.resval.rpoint[0]/rb.resval.rpoint[1];
		p1[0]=p4[0]+fr2;
		p1[1]=p4[1]+fr1;
		p1[2]=0.0;
		gra_rp2ucs(p1,p1,view);
		p2[0]=p4[0]+fr2;
		p2[1]=p4[1]-fr1;
		p2[2]=0.0;
		gra_rp2ucs(p2,p2,view);
		p3[0]=p4[0]-fr2;
		p3[1]=p4[1]-fr1;
		p3[2]=0.0;
		gra_rp2ucs(p3,p3,view);
		p4[0]-=fr2;
		p4[1]+=fr1;
		p4[2]=0.0;
		gra_rp2ucs(p4,p4,view);
		//define the entities we'll use.
		rb.restype=0;rb.rbnext=NULL;
		rb.resval.rstring= const_cast <char*> "LINE,ARC,CIRCLE,POLYLINE,RAY,XLINE,SOLID,TRACE,3DFACE";
		if(rbp1!=NULL)sds_relrb(rbp1);
		if((rbp1=sds_buildlist(RT3DPOINT,p1,RT3DPOINT,p2,RT3DPOINT,p3,RT3DPOINT,p4,0))==NULL){
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=RTERROR;
			goto exit;
		}
		sds_pmtssget(NULL,"CP",rbp1,NULL,&rb,ssbdry,0);
	}
	if((ret=sds_sslength(ssbdry,&fl1))!=RTNORM || fl1==0L){
		ret=RTERROR;
		cmd_ErrorPrompt(CMD_ERR_BDRYSET,0);
		goto exit;
	}
	if((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS8__NPOINT_AROUND_WH_13, "\nPoint around which to build polygon: " ),p1))!=RTNORM)goto exit;
	sds_printf(ResourceString(IDC_CMDS8__NBPOLY_IS_NOT_YE_14, "\nBpoly is not yet completed." ));

//	if((ret=cmd_bpoly_buildpoly(p1,ssbdry))!=RTNORM){
//		cmd_ErrorPrompt(CMD_ERR_BDRYSET,0);
//		goto exit;
//	}

	exit:
	sds_ssfree(ssbdry);
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	return(ret);
*/
}

short cmd_bpoly(void) // {cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); SDS_FreeEventList(0);  return(RTNORM); }
{

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_BPOLY );

	return(RTNORM);
}


// The following function, "cmd_lweight" needs to set the correct prop sheet and dialog.
extern int svar_ChildDialog;

short cmd_lweight(void) 
{
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	svar_ChildDialog = DV_LWEIGHT;
	ExecuteUIAction(ICAD_WNDACTION_SETVAR);

	return(RTNORM);
}


int cmd_bpoly_ent_to_2pt(sds_name ent, sds_point p1,sds_point p2,int *type){
	//takes entity ent (LINE<ARC,CIRCLE,VERTEX)and converts it to 2point form
	struct resbuf *rbp1,*rbp2;
	int ret=RTNORM;
	sds_point p3,p4;
	sds_name etemp;
	sds_real fr1;
	int enttype;

	rbp1=rbp2=NULL;

	if(NULL==(rbp1=sds_entget(ent))){ret=RTERROR;goto exit;}
	enttype=IC_TYPE_FROM_ENAME(ent);
	if(enttype==DB_LINE) {
		*type=1;
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(rbp1,11);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
	}else if(enttype==DB_ARC || enttype==DB_CIRCLE) {
		if(enttype==DB_CIRCLE) *type=2;
		else *type=1;
		for(rbp2=rbp1;rbp2!=NULL;rbp2=rbp2->rbnext){
			if(rbp2->restype==10)ic_ptcpy(p1,rbp2->resval.rpoint);
			else if(rbp2->restype==40)p2[0]=rbp2->resval.rreal;
			else if(rbp2->restype==40)p2[1]=rbp2->resval.rreal;
			else if(rbp2->restype==40)p2[2]=rbp2->resval.rreal;
		}
		rbp2=NULL;
	}else if(enttype==DB_VERTEX) {
		for(rbp2=rbp1;rbp2!=NULL;rbp2=rbp2->rbnext){
			if(rbp2->restype==10)ic_ptcpy(p1,rbp2->resval.rpoint);
			if(rbp2->restype==42)fr1=rbp2->resval.rreal;
		}
		rbp2=NULL;
		cmd_pline_entnext(ent,etemp,-1);
		if(NULL==(rbp2=sds_entget(etemp))){ret=RTERROR;goto exit;}
		ic_assoc(rbp2,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if(!icadRealEqual(fr1,0.0)){
			*type=1;
			//convert bulge to arc
			ic_ptcpy(p4,p2);
			ic_ptcpy(p3,p1);
			ic_bulge2arc(p3,p4,fr1,p1,&p2[0],&p2[1],&p2[2]);
		}else *type=0;
		IC_RELRB(rbp2);
	}else ret=RTERROR;

	exit:
	IC_RELRB(rbp1);
	IC_RELRB(rbp2);
	return(ret);
}
int cmd_bpoly_buildpoly(sds_point ipt, sds_name ssbdry){
	//given selection set ssbdry and interior point ipt, this f'n builds a polygon surrounding
	//	ipt. This is done by starting @ ipt and going along snapang to the nearest crossing.
	//	We trace this CCW, and if it closes AROUND the point, we're done.  If not, it starts
	//	again at ipt and goes at snapang+90deg and tries there.  Same for 180 and 270 degrees.
	//	This way, we don't need the user to help w/ray casting.
	//	Returns RTNORM if polygon successfully built, RTERROR if unable to build pline
	struct resbuf rb,*plinebeg,*plinecur,*ptsllist,*rbp1,*rbp2;
	int ret=RTNORM,direction,type1;//,type2,arcdir1,arcdir2;
	sds_real screenht,screenwid,snapang,angsin,angcos,fr1,fr2;
	sds_point p0,p1,p2,p3,p4;
	long nfound;//,segcount;
	sds_name e1;//,e2;

	plinebeg=plinecur=ptsllist=rbp1=rbp2=NULL;

	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	screenht=rb.resval.rreal;
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	screenwid=screenht*rb.resval.rpoint[0]/rb.resval.rpoint[1];
	SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	snapang=rb.resval.rreal;


	for(direction=0;direction<4;direction++){
		switch (direction){
			case 0: break;						//start at 3 o'clock
			case 1: snapang+=IC_PI;break;		//try 9 o'clock next
			case 2: snapang-=IC_PI/2.0; break;	//next try 12 o'clock
			case 3: snapang+=IC_PI;break;		//last try 6 o'clock
		}
		angsin=sin(snapang);angcos=cos(snapang);
		ic_ptcpy(p0,ipt);
		p0[0]+=10.0*screenwid*angcos;	//search well beyond the screen edge
		p0[1]+=10.0*screenht*angsin;	// for ray casting pt
		if(ptsllist!=NULL){sds_relrb(ptsllist);ptsllist=NULL;}
		//NOTE: if entxss last param &1==0, pts are ucs
		if(0!=ic_entxss(ipt,p0,ssbdry,CMD_FUZZ*screenht,&ptsllist,&nfound,2) || nfound==0 || ptsllist==NULL){
			ret=RTERROR;
			goto exit;
		}
		//if we got this far, there's something in the direction we're looking.
		//grab the nearest point and try to build a polygon w/it.
		fr1=-1.0;
		for(rbp1=ptsllist;rbp1!=NULL && rbp1->rbnext!=NULL;rbp1=rbp1->rbnext->rbnext){
			fr2=sds_dist2d(ipt,rbp1->rbnext->resval.rpoint);
			if(icadRealEqual(fr2,fr1)){
				//if both ents cross our line at the same point, check which
				//has the "leftmost" angle at intersection
				if(1==cmd_bpoly_leftent(ipt,p0,0,p1,e1,rbp1->resval.rlname)){
					ic_ptcpy(p1,rbp1->rbnext->resval.rpoint);
					ic_encpy(e1,rbp1->resval.rlname);
				}
			}else if(fr2<fr1 || fr1<0.0){
				ic_ptcpy(p1,rbp1->rbnext->resval.rpoint);
				ic_encpy(e1,rbp1->resval.rlname);
			}
		}
		sds_relrb(ptsllist);
		ptsllist=rbp1=NULL;
		//p1 (copy to p0) represents our starting point, and e1 represents our starting entity
		//now get e1's data in the proper format and go from there
		ic_ptcpy(p0,p1);
		IC_RELRB(rbp1);IC_RELRB(rbp2);
		if(RTNORM!=cmd_bpoly_ent_to_2pt(e1,p1,p2,&type1)){
			if(NULL==(rbp1=sds_entget(e1))){ret=RTERROR;goto exit;}
			if(IC_TYPE_FROM_ENAME(e1)==DB_TRACE ||
				IC_TYPE_FROM_ENAME(e1)==DB_SOLID ||
				 IC_TYPE_FROM_ENAME(e1)==DB_3DFACE) {
				type1=0;
				//NOTE:3DFACEs are different, but we should set 'em all up the same
				for(rbp2=rbp1;rbp2!=NULL;rbp2=rbp2->rbnext){
					if(rbp2->restype==10)ic_ptcpy(p1,rbp2->resval.rpoint);
					else if(rbp2->restype==11)ic_ptcpy(p2,rbp2->resval.rpoint);
					else if(rbp2->restype==12){
						if(IC_TYPE_FROM_ENAME(e1)==DB_3DFACE)
							ic_ptcpy(p4,rbp2->resval.rpoint);
						else
							ic_ptcpy(p3,rbp2->resval.rpoint);
					}else if(rbp2->restype==13){
						if(IC_TYPE_FROM_ENAME(e1)==DB_3DFACE)
							ic_ptcpy(p3,rbp2->resval.rpoint);
						else
							ic_ptcpy(p4,rbp2->resval.rpoint);
					}
				}
				//These entities have to be treated differently because they're
				// really 4 entities for our purposes.	We're at p0, and we need
				// to find out which segment we're on and work CCW to the next intersection
			}else{
				ret=RTERROR;goto exit;
			}
		}
		//intersect e1 w/set to get next point, and so on, to..
		//setup the llist of pts & bulges going ccw around the array
	/*	for(segcount=0;;segcount++){
			cmd_bpoly_nextseg(p1,p2,type1,p0,e2,p3,p4,&type2,&arcdir);
			if(segcount){
				if(type1==1){
					if(plinecur==NULL){
						plinebeg=plinecur=sds_buildlist(10,p5,42,0.0,0);
					}
				}
			}
		}
*/

	}
	//if we got here, we tried all 4 corners and didn't get anything.
	ret=RTERROR;

	exit:
	sds_entmake(NULL);
	return(ret);
}

int cmd_bpoly_leftent(sds_point e0p0, sds_point e0p1, int e0type, sds_point intpt, sds_name ent1, sds_name ent2){
	//Given entity of type e0type (0=line, 1=arc, 2=circle) defined by e0p0 & e0p1
	//	(e0p1 is rr,sa,ea for arc) and intersection point p1, determine which entity
	//	ent1 or ent2 makes the largest "left hand turn" for bpoly.
	//RETURNS:	0 if ent1 is leftmost turn
	//			1 if ent2 is leftmost turn
	//			RTERROR otherwise
	sds_real direction,ang1,ang2,fr1,fr2;
	struct resbuf *rbp1,*rbptmp;
	int ret=RTERROR,fi1,fi2;
	sds_point p1,p2,p3,p4;

	rbp1=NULL;
	//get the direction we're going
	if(e0type==0)
		direction=sds_angle(e0p0,e0p1);
	else{
		direction=IC_PI/2.0+sds_angle(e0p0,intpt);
		ic_normang(&direction,NULL);
	}

	for(fi1=0;fi1<2;fi1++){
		if(fi1==0){
			if((rbp1=sds_entget(ent1))==NULL) goto exit;
		}else{
			if((rbp1=sds_entget(ent2))==NULL) goto exit;
		}
		ic_assoc(rbp1,0);
		if(IC_TYPE_FROM_ENAME(ent1)==DB_TRACE ||
			IC_TYPE_FROM_ENAME(ent1)==DB_SOLID ||
			 IC_TYPE_FROM_ENAME(ent1)==DB_3DFACE) {
			//NOTE:3DFACEs are different, but we should set 'em all up the same
			for(rbptmp=rbp1;rbptmp!=NULL;rbptmp=rbptmp->rbnext){
				if(rbptmp->restype==10)ic_ptcpy(p1,rbptmp->resval.rpoint);
				else if(rbptmp->restype==11)ic_ptcpy(p2,rbptmp->resval.rpoint);
				else if(rbptmp->restype==12){
					if(IC_TYPE_FROM_ENAME(ent1)==DB_3DFACE)
						ic_ptcpy(p4,rbptmp->resval.rpoint);
					else
						ic_ptcpy(p3,rbptmp->resval.rpoint);
				}else if(rbptmp->restype==13){
					if(IC_TYPE_FROM_ENAME(ent1)==DB_3DFACE)
						ic_ptcpy(p3,rbptmp->resval.rpoint);
					else
						ic_ptcpy(p4,rbptmp->resval.rpoint);
				}
			}
			if(icadPointEqual(intpt,p1)){
				if(fi1==0){
					ang1=sds_angle(p1,p3);
				}else{
					ang2=sds_angle(p1,p3);
				}
			}else if(icadPointEqual(intpt,p2)){
				if(fi1==0){
					ang1=sds_angle(p2,p1);
				}else{
					ang2=sds_angle(p2,p1);
				}
			}else if(icadPointEqual(intpt,p3)){
				if(fi1==0){
					ang1=sds_angle(p3,p4);
				}else{
					ang2=sds_angle(p3,p4);
				}
			}else if(icadPointEqual(intpt,p4)){
				if(fi1==0){
					ang1=sds_angle(p4,p2);
				}else{
					ang2=sds_angle(p4,p2);
				}
			}else{
				//intersection ISN'T  on a vertex, so check all 4 lines
				//p1-p2
				if(!icadPointEqual(p1,p2) && icadRealEqual((p1[1]-p2[1])/(p1[0]-p2[0]),(intpt[1]-p2[1])/(intpt[0]-p2[0]))){
					if(fi1==0) ang1=sds_angle(p2,p1);
					else	ang2=sds_angle(p2,p1);
				//p3-p4
				}else if(!icadPointEqual(p3,p4) && icadRealEqual((p4[1]-p3[1])/(p4[0]-p3[0]),(intpt[1]-p3[1])/(intpt[0]-p3[0]))){
					if(fi1==0) ang1=sds_angle(p3,p4);
					else	ang2=sds_angle(p3,p4);
				//p1-p3
				}else if(!icadPointEqual(p1,p3) && icadRealEqual((p3[1]-p1[1])/(p3[0]-p1[0]),(intpt[1]-p1[1])/(intpt[0]-p1[0]))){
						if(fi1==0) ang1=sds_angle(p1,p3);
						else	ang2=sds_angle(p1,p3);
				//p2-p4
				}else if(!icadPointEqual(p2,p4) && icadRealEqual((p2[1]-p4[1])/(p2[0]-p4[0]),(intpt[1]-p4[1])/(intpt[0]-p4[0]))){
						if(fi1==0) ang1=sds_angle(p4,p2);
						else	ang2=sds_angle(p4,p2);
				}
			}
		}else{
			if(fi1==0)
				cmd_bpoly_ent_to_2pt(ent1,p1,p2,&fi2);
			else
				cmd_bpoly_ent_to_2pt(ent2,p1,p2,&fi2);
			if(fi2==0){
				//a line-type ent
				fr1=sds_angle(intpt,p1);
				fr2=sds_angle(intpt,p2);
				ic_normang(&direction,&fr1);
				ic_normang(&direction,&fr2);
				if((fr2<fr1 && !icadPointEqual(intpt,p2))|| icadPointEqual(intpt,p1)){
					if(fi1==0){
						ang1=fr2;
					}else{
						ang2=fr2;
					}
				}else{
					if(fi1==0){
						ang1=fr1;
					}else{
						ang2=fr1;
					}
				}
			}else{
				fr1=sds_angle(p1,intpt);
				//get vector from ctr to intersection rotated by 90deg CCW &
				//	call it p4. This is tangent vector toward arc end.
				p4[1]=intpt[0]-p1[0];
				p4[0]=p1[1]-intpt[1];
				//cross this w/vector from e0p0 to intpt.  If pos Z, take p2
				if((fr2=(intpt[0]-e0p0[0])*p4[1]-p4[0]*(intpt[1]-e0p0[1]))>CMD_FUZZ){
					fr1+=IC_PI/2.0;
				}else{ //if(fr2<CMD_FUZZ)
					fr1-=IC_PI/2.0;
				}
				/*else{
					//tangent condition - take cross product of direction vector &
					//	vector from center to intersection
					p4[0]=intpt[0]-p1[0];
					p4[1]=intpt[1]-p1[1];
					if(((intpt[0]-e0p0[0])*p4[1]-p4[0]*(intpt[1]-e0p0[1]))<CMD_FUZZ)
						ic_ptcpy(p4,p1);
					else
						//arc is tangent and doesn't go to the left at all...
						ic_ptcpy(p4,p2);
				} */
				if(fi1==0){
					ang1=fr1;
				}else{
					ang2=fr1;
				}
			}
		}
	}
	ic_normang(&direction,&ang1);
	ic_normang(&direction,&ang2);
	if(ang2<ang1){
		ret=1;
	}else{
		ret=0;
	}
	exit:
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	return(ret);
}


void cmd_pick_spot(sds_real *sa,sds_real *ea,sds_real *fr3,sds_real *fr4,sds_real *fr5,short *flag) {

	//check if the selected angle is between start angle and end angle
	if(cmd_ang_betw(*fr3,*sa,*ea)) {
		ic_normang(sa,fr3);
		*fr4=(*fr3)-(*sa);
		ic_normang(fr3,ea);
		*fr5=(*ea)-(*fr3);
		if((*fr5)>(*fr4)) {
			*flag=50;
		}else{
			*flag=51;
		}
	}else{
	//check if the start angle is between selected angle and end angle
	//if(cmd_ang_betw(*sa,*fr3,*ea)) {	//don't need this,
		ic_normang(ea,fr3);
		*fr4=(*fr3)-(*ea);
		ic_normang(fr3,sa);
		*fr5=(*sa)-(*fr3);
		if((*fr5)>(*fr4)) {
			*flag=51;
		}else{
			*flag=50;
		}
	}
	//don't need this,
	//check if the end angle is between start angle and selected angle
	/*if(cmd_ang_betw(*ea,*sa,*fr3)) {
		ic_normang(ea,fr3);
		*fr4=(*fr3)-(*ea);
		ic_normang(fr3,sa);
		*fr5=(*sa)-(*fr3);
		if((*fr5)>(*fr4)) {
			*flag=51;
		}else{
			*flag=50;
		}
	} */
}

//=========================================================================================

short cmd_tolerance(void) {

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_TOLERANCE );
	return(RTNORM);
}

int cmd_lengthen_pline_start(sds_name pline,sds_real dist) {
	struct resbuf *rbp1,*rbp2,rb;
 	short ret,rc=RTNORM;
	sds_name vname1,vname2;
	sds_point pt1,pt2,pt3;
	sds_real totlen,bulg,segdist,endwth,startwth,curlen,rad,sa,ea,angseg,newsa,newbulg;
	rb.restype=RTSTR;
	rb.resval.rstring=const_cast<char*>(ResourceString(DNT_CMDS8___15, "*") );
	rb.rbnext=NULL;
	rbp1=rbp2=NULL;

	sds_entmake(NULL);
	if((rbp1=sds_entgetx(pline,&rb))==NULL){ret=RTERROR;goto bail;}
	if(sds_entmake(rbp1)!=RTNORM){ret=RTERROR;goto bail;}
	sds_relrb(rbp1);
	rbp1=NULL;

	if(dist<0.0) {	//shorten the pline
		db_edata(pline,NULL,NULL,&totlen,NULL);
		curlen=0.0;
		if(totlen<fabs(dist)) {
			sds_printf(ResourceString(IDC_CMDS8__NINVALID_LENGTH_16, "\nInvalid length" )); // In what way is the length invalid?
			rc=RTNORM;
			goto bail;
		}
		ic_encpy(vname1,pline);
		while(1) {
			if((ret=sds_entnext_noxref(vname1,vname1))!=RTNORM){rc=ret;goto bail;}
			if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
			if((rbp1=sds_entget(vname1))==NULL){rc=RTERROR;goto bail;}

			ic_assoc(rbp1,0);
			if(strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)){rc=RTERROR;goto bail;}
			ic_assoc(rbp1,42);
			bulg=ic_rbassoc->resval.rreal;
			ic_assoc(rbp1,10);
			ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);

			if((ret=sds_entnext_noxref(vname1,vname2))!=RTNORM){rc=ret;goto bail;}
			if(rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
			if((rbp2=sds_entget(vname2))==NULL){ret=RTERROR;goto bail;}

			ic_assoc(rbp2,0);
			if(strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)){rc=RTERROR;goto bail;}
			ic_assoc(rbp2,10);
			ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);

			if(icadPointEqual(pt1,pt2))continue;
			//straight segment
			if(icadRealEqual(bulg,0.0)){
				segdist=sds_distance(pt1,pt2);
				if((curlen+segdist)<-dist){
					curlen+=segdist;
					continue;
				}else{
					sds_polar(pt1,sds_angle(pt1,pt2),(-dist-curlen),pt3);
					ic_assoc(rbp1,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,pt3);
					ic_assoc(rbp1,41);
					endwth=ic_rbassoc->resval.rreal;
					ic_assoc(rbp1,40);
					startwth=ic_rbassoc->resval.rreal;
					ic_rbassoc->resval.rreal=startwth+((endwth-startwth)*(-dist-curlen)/segdist);
					if((ret=sds_entmake(rbp1))!=RTNORM){rc=ret;goto bail;}
					break;
				}

			}else{	//curved segment
				if((ret=ic_bulge2arc(pt1,pt2,bulg,pt3,&rad,&sa,&ea))!=0){rc=ret;goto bail;}
				ic_normang(&sa,&ea);
				segdist=rad*(ea-sa);
				if((curlen+segdist)<-dist){
					curlen+=segdist;
					continue;
				}else{
					//set the new ending width
					ic_assoc(rbp1,41);
					endwth=ic_rbassoc->resval.rreal;
					ic_assoc(rbp1,40);
					startwth=ic_rbassoc->resval.rreal;
					angseg=(-dist-curlen)/rad;	//new included angle for segment
					startwth=startwth+(endwth-startwth)*angseg/(ea-sa);
					ic_rbassoc->resval.rreal=startwth;
					//set the new bulge
					if(bulg>0.0){  //if CCW
						newsa=sa+angseg;
				   		newbulg=tan((ea-newsa)/4.0);
					}else{		   //if CW
				   		newsa=ea-angseg;
				   		newbulg=-(tan((newsa-sa)/4.0));
					}
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=newbulg;
					//set the new starting point
					sds_polar(pt3,newsa,rad,pt2);
					ic_assoc(rbp1,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,pt2);
					//make it
					if((ret=sds_entmake(rbp1))!=RTNORM){rc=ret;goto bail;}
					break;
				}
			}
		}
		cmd_make_pline_end(vname1,0,pt1);
		sds_entdel(pline);
		sds_entlast(pline);
		sds_redraw(pline,IC_REDRAW_DRAW);
	}else{ //make the pline bigger
//		  db_edata(pline,NULL,NULL,&totlen,NULL);
//		  curlen=0.0;
//		  if(totlen<fabs(dist)) {
//			  sds_printf("\nInvalid length");
//			  rc=RTNORM;
//			  goto bail;
//		  }
//		  ic_encpy(vname1,pline);
		while(1) {
			if((ret=sds_entnext_noxref(pline,vname1))!=RTNORM){rc=ret;goto bail;}
			if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
			if((rbp1=sds_entget(vname1))==NULL){rc=RTERROR;goto bail;}
			ic_assoc(rbp1,0);

			if(strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)){rc=RTERROR;goto bail;}
			ic_assoc(rbp1,42);
			bulg=ic_rbassoc->resval.rreal;
			ic_assoc(rbp1,10);
			ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);

			if((ret=sds_entnext_noxref(vname1,vname2))!=RTNORM){rc=ret;goto bail;}
			if(rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
			if((rbp2=sds_entget(vname2))==NULL){ret=RTERROR;goto bail;}

			ic_assoc(rbp2,0);
			if(strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)){rc=RTERROR;goto bail;}
			ic_assoc(rbp2,10);
			ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);

			if(icadPointEqual(pt1,pt2))continue;
			//straight segment
			if(icadRealEqual(bulg,0.0)){
				//set the new start point
				sds_polar(pt1,sds_angle(pt2,pt1),dist,pt3);
				ic_assoc(rbp1,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt3);
				if((ret=sds_entmod(rbp1))!=RTNORM){rc=ret;goto bail;}
				if((ret=sds_entupd(pline))!=RTNORM){rc=ret;goto bail;}
				break;
			}else{	//curved segment
				if((ret=ic_bulge2arc(pt1,pt2,bulg,pt3,&rad,&sa,&ea))!=0){rc=ret;goto bail;}
				ic_normang(&sa,&ea);
				segdist=rad*(ea-sa);
				angseg=dist/rad;
				if((ea-sa+angseg)>=IC_TWOPI){
					ea-=IC_TWOPI;
				}
				//set the new bulge
				if(bulg>0.0){  //if CCW
					newsa=sa-angseg;
					newbulg=tan((ea-newsa)/4.0);
				}else{		   //if CW
					newsa=ea+angseg;
					newbulg=-(tan((newsa-sa)/4.0));
				}
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulg;
				//set the new starting point
				sds_polar(pt3,newsa,rad,pt2);
				ic_assoc(rbp1,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt2);
				//make it
				if((ret=sds_entmod(rbp1))!=RTNORM){rc=ret;goto bail;}
				if((ret=sds_entupd(pline))!=RTNORM){rc=ret;goto bail;}
				break;
			}
		}
	}
	rc=RTNORM;
bail:
	if(rbp1!=NULL){
		sds_relrb(rbp1);
		rbp1=NULL;
	}
	if(rbp2!=NULL){
		sds_relrb(rbp2);
		rbp2=NULL;
	}
	return(rc);

}


//===========================================================================================

short cmd_lengthen(void) {
	/*For simple entities, we'll always do an entmod, but for plines we might
		do an entmake.	This happens if we shorten a pline from either the start
		or from the end.  Undo's elistll->modflag will be left at 0 for all
		entmods, but set to 1 for plines made w/entmake.  For entmakes, we'll
		set elistll->ename to the name of the OLD entity.
	*/

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char cmdbar,prompt[IC_ACADBUF],fs1[IC_ACADBUF],tmp[IC_ACADBUF],tmp1[IC_ACADBUF];
	struct resbuf *rb=NULL, *rbptemp=NULL,rbapp;
	short rc=0,ret=0,flag=0;
	int lenmode,errorflag=0,angdir;
	sds_name ent,vname;
	struct resbuf setgetrb,rbucs,rbent,rbwcs;
	sds_point pt0,pt1,pt2,pt3,pt4,vec;
	sds_real length=0.0,percent=100.0,dist=0.0,angbase,fr1,fr2,fr3,fr4,fr5,ang,rad,sa,ea;
	struct cmd_elistll *elist_llbeg, *elist_llcur;
	struct cmd_elistll_ptr *undo_beg, *undo_cur, *undo_temp;

	static sds_real cmd_lengthen_lastlengthi=0.0;  //This stores the last incremental length entered.
	static sds_real cmd_lengthen_lastlengtht=0.0;  //This stores the last total length entered.
	static sds_real cmd_lengthen_lastpercent=100.0; //This stores the last percent entered.
	static sds_real cmd_lengthen_lastanglei=0.0;		//Last Incremental Angle
	static sds_real cmd_lengthen_lastanglet=0.0;		//Last Total Angle

	elist_llbeg=elist_llcur=NULL;
	undo_beg=undo_cur=undo_temp=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	rbapp.restype=RTSTR;
	rbapp.rbnext=NULL;
	rbapp.resval.rstring=const_cast<char*>(ResourceString(DNT_CMDS8___15, "*") );
	SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	angdir=setgetrb.resval.rint;
	SDS_getvar(NULL,DB_QANGBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	angbase=setgetrb.resval.rreal;

	SDS_getvar(NULL,DB_QWNDLCMD,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	cmdbar=(char)setgetrb.resval.rint;

	mode:
	while(1) {	//This loop is for returning the current length
		//NOTE: if there's an angle, then there's an associated length,but not vice versa
		if(0!=cmdbar || 0.0==cmd_lengthen_lastlengtht){
			sprintf(prompt,ResourceString(IDC_CMDS8__NEDIT_LENGTH___D_18, "\nEdit length:  DYnamic/Increment/Percent/Total/<Select entity to list length>: " ));
		}else{
			sds_rtos(cmd_lengthen_lastlengtht,-1,-1,tmp);
			if(0.0==cmd_lengthen_lastanglet)
				sprintf(prompt,ResourceString(IDC_CMDS8__NEDIT_LENGTH__LE_19, "\nEdit length (length=%s):  DYnamic/Increment/Percent/Total/<Select entity to list length>: " ),tmp);
			else{
				sds_angtos(cmd_lengthen_lastanglet,-1,-1,tmp1);
				sprintf(prompt,ResourceString(IDC_CMDS8__NEDIT_LENGTH__LE_20, "\nEdit length (length=%s,angle=%s):  DYnamic/Increment/Percent/Total/<Select entity to list length>: " ),tmp,tmp1);
			}
		}
		if(undo_beg==NULL){
			if(sds_initget(0,ResourceString(IDC_CMDS8__DELTA_DYNAMIC_DY_21, "~DElta Dynamic|DYnamic Increment Percent Total ~_DElta ~_DYnamic ~_Increment ~_Percent ~_Total" ))!=RTNORM) {rc=(RTERROR); goto bail;}
		}else{
			if(sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS8__DELTA_DYNAMIC_DY_22, "~DElta Dynamic|DYnamic Increment Percent Total ~ ` ~_DElta ~_DYnamic ~_Increment ~_Percent ~_Total ~_" ))!=RTNORM) {rc=(RTERROR); goto bail;}
		}
		if((ret=sds_entsel(prompt,ent,pt0))==RTERROR) {
			rc=(RTERROR);goto bail;
		} else if(ret==RTCAN) {
			rc=(RTCAN);goto bail;
		} else if(ret==RTNONE) {
			rc=(RTNORM);goto bail;
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
			if(strisame(fs1,"DELTA"/*DNT*/) || strisame(fs1,"INCREMENT"/*DNT*/))lenmode=1;
			else if(strisame(fs1,"TOTAL"/*DNT*/))lenmode=2;
			else if(strisame(fs1,"PERCENT"/*DNT*/))lenmode=4;
			else if(strisame(fs1,"DYNAMIC"/*DNT*/))lenmode=8;
			else goto bail;	//"Done"
			break;
		} else {
			SDS_getvar(NULL,DB_QWNDLCMD,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			cmdbar=(char)setgetrb.resval.rint;
			if((rb=sds_entget(ent))==NULL) {
				cmd_ErrorPrompt(CMD_ERR_BADENTNAME,0);
				rc=(RTERROR);goto bail;}
			//find out what kind of entity and return current length
			ic_assoc(rb,0);
			//if it is an ARC print the length and included angle
			if(strisame("ARC"/*DNT*/,ic_rbassoc->resval.rstring)) {
				ic_assoc(rb,40);
				rad=ic_rbassoc->resval.rreal;
				ic_assoc(rb,50);
				sa=ic_rbassoc->resval.rreal;
				ic_assoc(rb,51);
				ea=ic_rbassoc->resval.rreal;
				ic_normang(&sa,&ea);
				if(cmdbar){
					sds_rtos(rad*(ea-sa),-1,-1,tmp);
					sds_angtos(ea-sa,-1,-1,tmp1);
					sds_printf(ResourceString(IDC_CMDS8__NCURRENT_LENGTH__29, "\nCurrent length: %s, included angle: %s" ),tmp,tmp1);
				}
				cmd_lengthen_lastlengtht=rad*(ea-sa);
				cmd_lengthen_lastanglet=ea-sa;
			//if it is a LINE pirnt the current length
			} else if(strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring)) {
				ic_assoc(rb,10);
				ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				ic_assoc(rb,11);
				ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
				if(cmdbar){
					sds_rtos(sds_distance(pt1,pt2),-1,-1,tmp);
					sds_printf(ResourceString(IDC_CMDS8__NCURRENT_LENGTH__30, "\nCurrent length: %s" ),tmp);
				}
				cmd_lengthen_lastlengtht=sds_distance(pt1,pt2);
			//if it is a POLYLINE
//4M Spiros Item:123->
// Calculate the length of an LWPOLYLINE too
//			  } else if(strisame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
			} else if(strisame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring) || strisame("LWPOLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
//4M Spiros Item:123<-
				//AT THIS STAGE, REPORT DATA FOR CLOSED AND SPLINED PLINES.
				//	WE WON'T ALLOW LENGTHENING, BUT WE CAN GET DATA HERE
				/*
				ic_assoc(rb,70);
				if(ic_rbassoc->resval.rint & (1+2+4)){
					cmd_ErrorPrompt(CMD_ERR_LENGTHENENT,0); // "Unable to lengthen the selected entity."  Close enough?
					continue;
				}*/
				if((ret=db_edata(ent,NULL,NULL,&length,NULL))!=0) {rc=(RTERROR);goto bail;}
				if(cmdbar){
					sds_rtos(length,-1,-1,tmp);
					sds_printf(ResourceString(IDC_CMDS8__NCURRENT_LENGTH__30, "\nCurrent length: %s" ),tmp);
				}
				cmd_lengthen_lastlengtht=length;
			//if it is something else print cannot lengthen this entity
			} else if(strisame("CIRCLE"/*DNT*/,ic_rbassoc->resval.rstring)) {
				ic_assoc(rb,40);
				if(cmdbar){
					sds_rtos(IC_TWOPI*ic_rbassoc->resval.rreal,-1,-1,tmp);
					sds_printf(ResourceString(IDC_CMDS8__NCURRENT_CIRCUMF_33, "\nCurrent circumference: %s" ),tmp);
				}
				cmd_lengthen_lastlengtht=IC_TWOPI*ic_rbassoc->resval.rreal;
			} else {
				cmd_ErrorPrompt(CMD_ERR_LENGTHENENT,0);
				rc=(RTNORM);goto bail;
			}
		}
	}

	//Now begin ent modification using different modes...
	if(0==(lenmode & 8)){	//non-dynamic (static) modes...
		if(lenmode & (1+2)) { //delta and total
			if(lenmode&1){
				sds_rtos(cmd_lengthen_lastlengthi,-1,-1,tmp);
				if(undo_beg!=NULL){
					if(sds_initget(RSG_NOZERO+SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS8_ANGLE___34, "Angle ` ~_Angle" ))!=RTNORM) {rc=(RTERROR); goto bail;}
				}else{
					if(sds_initget(RSG_NOZERO,ResourceString(IDC_CMDS8_ANGLE_35, "Angle ~_Angle" ))!=RTNORM) {rc=(RTERROR); goto bail;}
				}
				sprintf(prompt,ResourceString(IDC_CMDS8__NANGLE____ENTER__36, "\nAngle/<Enter length increment (%s)>: " ),tmp);
			}else{
				sds_rtos(cmd_lengthen_lastlengtht,-1,-1,tmp);
				if(undo_beg!=NULL){
					if(sds_initget(RSG_NOZERO+RSG_NONEG+SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS8_ANGLE___34, "Angle ` ~_Angle" ))!=RTNORM) {rc=(RTERROR); goto bail;}
				}else{
					if(sds_initget(RSG_NOZERO+RSG_NONEG,ResourceString(IDC_CMDS8_ANGLE_35, "Angle ~_Angle" ))!=RTNORM) {rc=(RTERROR); goto bail;}
				}
				sprintf(prompt,ResourceString(IDC_CMDS8__NANGLE____ENTER__37, "\nAngle/<Enter total length (%s)>: " ),tmp);
			}
			ret=sds_getdist(NULL,prompt,&length);	//get dist
		}else if(lenmode & 4){	//percent mode
			sprintf(tmp,"%f",cmd_lengthen_lastpercent);
			sprintf(prompt,ResourceString(IDC_CMDS8__NENTER_PERCENT_L_38, "\nEnter percent length <%s>: " ),tmp);
			if(sds_initget(RSG_NOZERO+RSG_NONEG,NULL)!=RTNORM) {rc=(RTERROR); goto bail;}
			ret=sds_getreal(prompt,&percent);  //get result
		}
		if(ret==RTERROR) {
			rc=(RTERROR);goto bail;
		} else if(ret==RTCAN) {
			rc=(RTCAN);goto bail;
		} else if(ret==RTNONE) {
			if(lenmode&1)
				length=cmd_lengthen_lastlengthi;  //copy 'em all - we don't know which mode
			else
				length=cmd_lengthen_lastlengtht;
			percent=cmd_lengthen_lastpercent;
		} else if(ret==RTKWORD) {	//can't happen in percent mode
			//if kword angle then prompt for the angle
			sds_getinput(fs1);
			if(strisame(fs1,"DONE"/*DNT*/))goto bail;
			if(lenmode&1){
				//correct angtos's use of angbase & angdir
				if(angdir==0)
					fr1=cmd_lengthen_lastanglei+angbase;
				else
					fr1=cmd_lengthen_lastanglei-angbase;
				if(angdir==1)fr1=-fr1;
				sds_angtos(fr1,-1,-1,tmp);
				sprintf(prompt,ResourceString(IDC_CMDS8__NENTER_ANGLE_INC_40, "\nEnter angle increment <%s>: " ),tmp);
			}else{
				//correct angtos's use of angbase & angdir
				if(angdir==0)
					fr1=cmd_lengthen_lastanglet+angbase;
				else
					fr1=cmd_lengthen_lastanglet-angbase;
				if(angdir==1)fr1-=IC_TWOPI;
				sds_angtos(fr1,-1,-1,tmp);
				sprintf(prompt,ResourceString(IDC_CMDS8__NENTER_TOTAL_ANG_41, "\nEnter total angle <%s>: " ),tmp);
			}
			if((ret=sds_getorient(NULL,prompt,&ang))==RTERROR) {
				rc=RTERROR;goto bail;
			} else if(ret==RTCAN) {
				rc=RTCAN;goto bail;
			} else if(ret==RTNONE) {
				if(lenmode&1)
					ang=cmd_lengthen_lastanglei;
				else
					ang=cmd_lengthen_lastanglet;
			} else if(ret==RTNORM) {
				if(lenmode&1)
					cmd_lengthen_lastanglei=ang;
				else
					cmd_lengthen_lastanglet=ang;
			}
			lenmode+=16;
		}
		if(lenmode & 1)cmd_lengthen_lastlengthi=length;
		if(lenmode & 2)cmd_lengthen_lastlengtht=length;
		if(lenmode & 4)cmd_lengthen_lastpercent=percent;
	}

	//stay in this loop and keep selecting entities and modifing them
		while(1) {
			//??cmd_lengthen_lastlength=length;
			if(errorflag && undo_beg!=NULL){ // if operation was unsuccessful on last ent,
				//remove elist from undo list
				for(undo_temp=undo_beg;(undo_temp->next!=undo_cur)&&(undo_temp->next!=NULL);undo_temp=undo_temp->next);
				cmd_free_elistll(&undo_cur->elistll,NULL);
				if(undo_cur!=undo_beg){
					delete undo_cur;
					undo_cur=undo_temp;
					undo_cur->next=NULL;
				}else{
					delete undo_beg;
					undo_cur=undo_beg=NULL;
				}
			}
			errorflag=0;
			if(undo_beg!=NULL){
				if(sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS8_EDIT_MODE_MODE____42, "Edit_mode|Mode ~ Undo ` ~_Mode ~_ ~_Undo" ))!=RTNORM) {rc=(RTERROR); goto bail;}
				ret=sds_entsel(ResourceString(IDC_CMDS8__NMODE___UNDO_____43, "\nMode/Undo/<Select entity to change>: " ),ent,pt0);
			}else{
				if(sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS8_EDIT_MODE_MODE____44, "Edit_mode|Mode ~ ~Undo ` ~_Mode ~_ ~_Undo" ))!=RTNORM) {rc=(RTERROR); goto bail;}
				ret=sds_entsel(ResourceString(IDC_CMDS8__NMODE____SELECT__45, "\nMode/<Select entity to change>: " ),ent,pt0);
			}
			if(ret==RTERROR) {
				rc=(RTERROR);goto bail;
			} else if(ret==RTCAN) {
				rc=(RTCAN);goto bail;
			} else if(ret==RTNONE) {
				rc=(RTNORM);goto bail;
			} else if(ret==RTKWORD) {
				sds_getinput(fs1);
				if(strisame(fs1,"MODE"/*DNT*/))goto mode;
				if(strisame(fs1,"DONE"/*DNT*/))goto bail;
				//undo last operation
				if(undo_beg==NULL){
					cmd_ErrorPrompt(CMD_ERR_NOMOREUNDO,0);
				}else{
					undo_temp=undo_cur;
					if(undo_cur->elistll->modflag==0){	//simple entity
						do{
							if(sds_entmod(undo_temp->elistll->entlst)!=RTNORM){ret=(-__LINE__);goto bail;}
						}while((undo_temp->elistll=undo_temp->elistll->next)!=NULL);
					}else{	//a pline made w/entmake
						//delete current entity
						if(sds_entdel(undo_cur->elistll->ename)!=RTNORM){ret=(-__LINE__);goto bail;}
						//remake the old entity
						do{
							if(sds_entmake(undo_temp->elistll->entlst)!=RTNORM){sds_entdel(ent);ret=(-__LINE__);goto bail;}
						}while((undo_temp->elistll=undo_temp->elistll->next)!=NULL);
					}
					//step to element just before cur
					for(undo_temp=undo_beg;(undo_temp->next!=undo_cur)&&(undo_temp->next!=NULL);undo_temp=undo_temp->next);
					cmd_free_elistll(&undo_cur->elistll,NULL);
					if(undo_cur!=undo_beg){
						delete undo_cur;
						undo_cur=undo_temp;
						undo_cur->next=NULL;
					}else{
						delete undo_beg;
						undo_cur=undo_beg=NULL;
					}
				}
				continue;
			} else if(ret==RTNORM) {
				//retain ent data for later undo!
				//setup undo list for current status of entity
				elist_llbeg=elist_llcur=NULL;
				cmd_alloc_elistll(&elist_llbeg,&elist_llcur,ent,0);
				if(undo_beg==NULL){
					//*** Allocate a new link in list.					
					if((undo_temp= new struct cmd_elistll_ptr )==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						ret=(-__LINE__); rc=RTERROR;
						goto bail;
					}
					memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
					undo_beg=undo_cur=undo_temp;
					undo_cur->elistll=elist_llbeg;
				}else{					
					if((undo_temp= new struct cmd_elistll_ptr )==NULL) {
						cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
						ret=(-__LINE__); rc=RTERROR;
						goto bail;
					}
					memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
					undo_cur->next=undo_temp;
					undo_cur=undo_cur->next;
					undo_cur->elistll=elist_llbeg;
				}
			}
			//clear the resbuf if not clear already
			if(rb!=NULL) {sds_relrb(rb); rb=NULL;}
			//entget selected entity
			if((rb=sds_entgetx(ent,&rbapp))==NULL) {cmd_ErrorPrompt(CMD_ERR_BADENTNAME,0);rc=(RTERROR);goto bail;}
			//find out what type it is
			ic_assoc(rb,0);
			//if ARC find out which angle to move -- the closest angle to the pick
			if(strisame("ARC"/*DNT*/,ic_rbassoc->resval.rstring)) {
				for(rbptemp=rb;rbptemp!=NULL;rbptemp=rbptemp->rbnext){
					if(rbptemp->restype==10)ic_ptcpy(pt3,rbptemp->resval.rpoint);
					else if(rbptemp->restype==40)rad=rbptemp->resval.rreal;
					else if(rbptemp->restype==50)sa=rbptemp->resval.rreal;
					else if(rbptemp->restype==51)ea=rbptemp->resval.rreal;
				}
				rbptemp=NULL;
				//make the selected point into the selected angle
				fr3=sds_angle(pt3,pt0);
				//find start or end closest to the pick point return 50 for start and 51 for end
				cmd_pick_spot(&sa,&ea,&fr3,&fr4,&fr5,&flag);
				sds_polar(pt3,sa,rad,pt1);
				sds_polar(pt3,ea,rad,pt2);
				if(lenmode & 8){	//dynamic change mode...
					sds_redraw(ent,IC_REDRAW_UNDRAW);
					if(flag==50){
						ret=SDS_dragobject(31,1,pt2,pt3,rad,ResourceString(IDC_CMDS8__NNEW_STARTING_AN_47, "\nNew starting angle: " ),NULL,pt0,NULL);
						ic_assoc(rb,flag);
						ic_rbassoc->resval.rreal=sa=sds_angle(pt3,pt0);
					}else{
						ret=SDS_dragobject(9,1,pt1,pt3,rad,ResourceString(IDC_CMDS8__NNEW_ENDING_ANGL_48, "\nNew ending angle: " ),NULL,pt0,NULL);
						ic_assoc(rb,flag);
						ic_rbassoc->resval.rreal=ea=sds_angle(pt3,pt0);
					}
					if(ret!=RTNORM) {
						sds_redraw(ent,IC_REDRAW_DRAW);
						rc=RTERROR;
						goto bail;
					}
				}else{
					//static change mode
					//select the proper group code to modify
					ic_assoc(rb,flag);
					ic_normang(&sa,&ea);
					//if the user entered a length
					if(0==(lenmode&16)) {//standard length format
						if(flag==50) {
							if(lenmode & 1)sa=ic_rbassoc->resval.rreal-=length/rad;
							else if(lenmode & 4)sa=ic_rbassoc->resval.rreal=ea-(ea-sa)*(percent/100.0);
							else if(lenmode & 2)sa=ic_rbassoc->resval.rreal=ea-length/rad;
						}else{
							if(lenmode & 1)sa=ic_rbassoc->resval.rreal+=length/rad;
							else if(lenmode & 4)ea=ic_rbassoc->resval.rreal=sa+(ea-sa)*(percent/100.0);
							else if(lenmode & 2)ea=ic_rbassoc->resval.rreal=sa+length/rad;
						}
					//if the user entered an angle
					}else{
						if(flag==50) {
							if(lenmode & 1)ic_rbassoc->resval.rreal-=ang;
							else if(lenmode & 2)ic_rbassoc->resval.rreal=ea-ang;
						}else{
							if(lenmode & 1)ic_rbassoc->resval.rreal+=ang;
							else if(lenmode & 2)ic_rbassoc->resval.rreal=sa+ang;
						}
					}
				}
				ic_normang(&sa,&ea);
				if((ea-sa)>=IC_TWOPI) {
					cmd_ErrorPrompt(CMD_ERR_ARCSIZE,0);
					errorflag=1;
				}else{
					if(sds_entmod(rb)!=RTNORM){sds_entdel(ent);rc=RTERROR;goto bail;}
				}

			//if the user selected a line using a length
			} else if(strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring)) {  //if LINE and not angle
			//if the user selected a line using an angle
				if(lenmode & 16){
					cmd_ErrorPrompt(CMD_ERR_LENBYANG,0);
					errorflag=1;
					continue;
				}
				ic_assoc(rb,10);
				ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				ic_assoc(rb,11);
				ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
				fr1=sds_distance(pt1,pt2);
				rbent.restype=RTSHORT;
				rbent.resval.rint=2;//dcs
				sds_trans(pt1,&rbwcs,&rbent,0,pt3);
				sds_trans(pt2,&rbwcs,&rbent,0,pt4);
				sds_trans(pt0,&rbucs,&rbent,0,pt0);
				if(cmd_dist2d(pt0,pt4)<cmd_dist2d(pt0,pt3)){
					//move end point pt2
					ic_assoc(rb,11);
					vec[0]=pt2[0]-pt1[0];vec[1]=pt2[1]-pt1[1];vec[2]=pt2[2]-pt1[2];
					if(lenmode & 1){
						cmd_polar(pt2,vec,length,pt3);
					}else if(lenmode & 2){
						cmd_polar(pt1,vec,length,pt3);
					}else if(lenmode & 4){
						cmd_polar(pt1,vec,(fr1*percent/100.0),pt3);
					}else if(lenmode & 8){
						sds_trans(pt1,&rbwcs,&rbucs,0,pt1);
						sds_trans(pt2,&rbwcs,&rbucs,0,pt2);
						sds_redraw(ent,IC_REDRAW_UNDRAW);
						if((ret=SDS_dragobject(38,1,pt1,pt2,NULL,ResourceString(IDC_CMDS8__NNEW_END_POINT_F_49, "\nNew end point for line: " ),&rbptemp,pt3,NULL))!=RTNORM){
							sds_redraw(ent,IC_REDRAW_DRAW);
							goto bail;
						}
						ic_assoc(rbptemp,11);
						ic_ptcpy(pt3,ic_rbassoc->resval.rpoint);
						sds_relrb(rbptemp);rbptemp=NULL;
						ic_assoc(rb,11);
					}
				}else{
					//move first point pt1
					ic_assoc(rb,10);
					vec[0]=pt1[0]-pt2[0];vec[1]=pt1[1]-pt2[1];vec[2]=pt1[2]-pt2[2];
					if(lenmode & 1){
						cmd_polar(pt1,vec,length,pt3);
					}else if(lenmode & 2){
						cmd_polar(pt2,vec,length,pt3);
					}else if(lenmode & 4){
						cmd_polar(pt2,vec,(fr1*percent/100.0),pt3);
 					}else if(lenmode & 8){
						sds_redraw(ent,IC_REDRAW_UNDRAW);
						sds_trans(pt1,&rbwcs,&rbucs,0,pt1);
						sds_trans(pt2,&rbwcs,&rbucs,0,pt2);
						if((ret=SDS_dragobject(38,1,pt2,pt1,NULL,ResourceString(IDC_CMDS8__NNEW_START_POINT_50, "\nNew start point for line: " ),&rbptemp,pt3,NULL))!=RTNORM){
							sds_redraw(ent,IC_REDRAW_DRAW);
							goto bail;
						}
						ic_assoc(rbptemp,11);
						ic_ptcpy(pt3,ic_rbassoc->resval.rpoint);
						sds_relrb(rbptemp);rbptemp=NULL;
						ic_assoc(rb,10);
					}
				}
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt3);
				if(sds_entmod(rb)!=RTNORM){sds_entdel(ent);rc=RTERROR;goto bail;}
			//if the user selected a POLYLINE
			} else if(strisame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) { // if POLYLINE
				//NOTE: this is the ONE case where we may do an ENTMAKE, not an ENTMOD!
				//	We'll set the elistll->modflag to 1 & set ->ename to NEW ent name
				/*if(lenmode & 16){
					cmd_ErrorPrompt(CMD_ERR_LENBYANG,0);
					continue;
				} */
				ic_assoc(rb,70);
				if(ic_rbassoc->resval.rint & 1){
					cmd_ErrorPrompt(CMD_ERR_LENCLOSEPOLY,0);
					errorflag=1;
					continue;
				}else if(ic_rbassoc->resval.rint & (2+4)){
					cmd_ErrorPrompt(CMD_ERR_LENGTHENENT,0);
					errorflag=1;
					continue;
				}
				if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)){
					rbent.restype=RTSHORT;
					rbent.resval.rint=0;
				}else{
					ic_assoc(rb,210);
					ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
					rbent.restype=RT3DPOINT;
				}

				if((ret=db_edata(ent,NULL,NULL,&fr1,NULL))!=0) {rc=(RTERROR);goto bail;}

				//get next entity after pline and put it in the resbuf
				if((ret=sds_entnext_noxref(ent,vname))!=RTNORM) {rc=ret;goto bail;}
				if(rb!=NULL) {sds_relrb(rb); rb=NULL;}
				if((rb=sds_entget(vname))==NULL) {cmd_ErrorPrompt(CMD_ERR_BADENTNAME,0);rc=(RTERROR);goto bail;}

				//get starting point of the polyline
				sds_trans(pt0,&rbucs,&rbent,0,pt0);

				ic_assoc(rb,10);
				ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				if((ret=cmd_pline_dist(ent,pt1,pt0,&dist))!=0) {rc=(RTERROR);goto bail;}
				if(dist<(fr1/2.0)) { //move the start point
					if((lenmode & 1+16)==1){
						ret=cmd_lengthen_pline_start(ent,length);//uses ENTMAKE if length < 0
					}else if((lenmode & 2+16)==2){
						ret=cmd_lengthen_pline_start(ent,length-fr1);
					}else if(lenmode & 4){
						if(percent<100.0){
							fr2=-(fr1*(1.0-percent/100.0));
						}else{
							fr2=fr1*percent/100.0-fr1;
						}
						ret=cmd_lengthen_pline_start(ent,fr2);
					}
					if((length<0.0 && (lenmode & 3)!=0)||
						(percent<100.0 && (lenmode & 4)!=0)){
						//if we did an entmake...
						undo_cur->elistll->modflag=1;	//setup undo correctly
						sds_entlast(ent);
						ic_encpy(undo_cur->elistll->ename,ent);
					}
					if(lenmode & 8+16){//dynamic or arc drag on beginning of pline
						if((ret=sds_entnext_noxref(vname,vname))!=RTNORM) {rc=ret;goto bail;}
						//NOTE: don't free rbptemp here - garbage ptr to NON-allocated memory
						if((rbptemp=sds_entget(vname))==NULL) {cmd_ErrorPrompt(CMD_ERR_BADENTNAME,0);rc=(RTERROR);goto bail;}
						ic_assoc(rbptemp,10);
						ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
						ic_assoc(rb,42);
						fr2=ic_rbassoc->resval.rreal;
						sds_relrb(rbptemp);rbptemp=NULL;
						if(fabs(fr2)<CMD_FUZZ){//straight segment
							if(lenmode & 16){
								cmd_ErrorPrompt(CMD_ERR_LENBYANG,0);
								errorflag=1;
								continue;
							}
							ret=SDS_dragobject(2,1,pt2,pt0,sds_angle(pt2,pt1),ResourceString(IDC_CMDS8__NNEW_START_POINT_51, "\nNew start point: " ),&rbptemp,pt0,NULL);
							if(rbptemp!=NULL){
								ic_assoc(rbptemp,11);
								ic_ptcpy(pt3,ic_rbassoc->resval.rpoint);
								fr2=0.0;
								sds_relrb(rbptemp);
								rbptemp=NULL;
							}
						}else{	//curved segment
							if(ic_bulge2arc(pt1,pt2,fr2,pt0,&pt4[0],&pt4[1],&pt4[2])){
								cmd_ErrorPrompt(CMD_ERR_LENGTHENENT,0);
								rc=RTERROR;
								goto bail;
							}
							if(fr2<0.0){	//clockwise start of pline
								if((lenmode&16)==0){   //dynamic
									ret=SDS_dragobject(9,1,pt2,pt0,NULL,ResourceString(IDC_CMDS8__NNEW_STARTING_AN_47, "\nNew starting angle: " ),NULL,pt3,NULL);
									if(ret==RTNORM){
										fr2=sds_angle(pt0,pt3);
									}
								}else{
									ret=RTNORM;
									if(lenmode&1){
										fr2=pt4[2]+ang/pt4[0];;
									}else if(lenmode&2){
										fr2=pt4[1]+ang/pt4[0];
									}
								}
								sds_polar(pt0,fr2,pt4[0],pt3);
								ic_normang(&pt4[1],&fr2);
								if(fr2-pt4[1]>IC_TWOPI){
									cmd_ErrorPrompt(CMD_ERR_ARCSIZE,0);
									errorflag=1;
									continue;
								}
								fr2=-tan((fr2-pt4[1])/4.0);
							}else{			//ccw start of pline
								if((lenmode&16)==0){	//dynamic
									ret=SDS_dragobject(31,1,pt2,pt0,NULL,ResourceString(IDC_CMDS8__NNEW_STARTING_AN_47, "\nNew starting angle: " ),NULL,pt3,NULL);
									if(ret==RTNORM){
										fr2=sds_angle(pt0,pt3);
									}
								}else{
									ret=RTNORM;
									if(lenmode&1){
										fr2=pt4[1]-length/pt4[0];
									}else if(lenmode&2){
										fr2=pt4[2]-length/pt4[0];
									}
								}
								sds_polar(pt0,fr2,pt4[0],pt3);
								ic_normang(&fr2,&pt4[2]);
								if(pt4[2]-fr2>IC_TWOPI){
									cmd_ErrorPrompt(CMD_ERR_ARCSIZE,0);
									errorflag=1;
									continue;
								}
								fr2=tan((pt4[2]-fr2)/4.0);
							}
						}
						if(ret!=RTNORM){rc=ret;goto bail;}
						ic_assoc(rb,10);
						ic_ptcpy(ic_rbassoc->resval.rpoint,pt3);
						ic_assoc(rb,42);
						ic_rbassoc->resval.rreal=fr2;
						if((ret=sds_entmod(rb))!=RTNORM){rc=ret;goto bail;}
						sds_entupd(ent);//NOTE: no entmod, so don't reset any vars
					}
				}else{ //if(dist>=(fr1/2.0)) move the end point
					if((lenmode & 1+16)==1)ret=cmd_lengthen_pline_end(ent,length);
					else if((lenmode & 2+16)==2)ret=cmd_lengthen_pline_end(ent,length-fr1);
					else if(lenmode & 4){
						if(percent<100.0){
							fr2=-(fr1*(1.0-percent/100.0));
						}else{
							fr2=fr1*percent/100.0-fr1;
						}
						ret=cmd_lengthen_pline_end(ent,fr2);
					}
					if(((length < 0.0)&&(lenmode & 3))||
						((percent<100.0)&&(lenmode & 4))){
						//if we did an entmake...
						undo_cur->elistll->modflag=1;	//setup undo correctly
						sds_entlast(ent);
						ic_encpy(undo_cur->elistll->ename,ent);
					}
					if(lenmode & (8+16)){//dynamic drag on end of pline
						if(rb!=NULL){sds_relrb(rb);rb=NULL;}
						cmd_pline_next2lastvtx(ent,vname);
						if((rb=sds_entget(vname))==NULL){ret=RTERROR;goto bail;}
						ic_assoc(rb,10);
						ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
						ic_assoc(rb,42);
						fr2=ic_rbassoc->resval.rreal;
						if((ret=sds_entnext_noxref(vname,vname))!=RTNORM) {rc=ret;goto bail;}
						//NOTE: don't free rbptemp here - garbage ptr to NON-allocated memory
						if((rbptemp=sds_entget(vname))==NULL) {cmd_ErrorPrompt(CMD_ERR_BADENTNAME,0);rc=(RTERROR);goto bail;}
						ic_assoc(rbptemp,10);
						ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
						//Don't free rbptemp - need it later
						if(fabs(fr2)<CMD_FUZZ){//straight segment
							if(lenmode&16){
								cmd_ErrorPrompt(CMD_ERR_LENBYANG,0);
								errorflag=1;
								continue;
							}
							ret=SDS_dragobject(2,1,pt1,pt0,sds_angle(pt1,pt2),ResourceString(IDC_CMDS8__NNEW_END_POINT___52, "\nNew end point: " ),NULL,pt3,NULL);
							if(ret==RTNORM){
								ic_ptlndist(pt3,pt1,pt2,&fr2,pt3);
								fr2=0.0;
							}
						}else{	//curved segment
							if(ic_bulge2arc(pt1,pt2,fr2,pt0,&pt4[0],&pt4[1],&pt4[2])){
								cmd_ErrorPrompt(CMD_ERR_LENGTHENENT,0);
								rc=RTERROR;
								goto bail;
							}
							if(fr2>0.0){	//ccw end of pline
								if((lenmode&16)==0){
									ret=SDS_dragobject(5,1,pt0,pt1,NULL,ResourceString(IDC_CMDS8__NNEW_ENDING_ANGL_48, "\nNew ending angle: " ),NULL,pt3,NULL);
									if(ret==RTNORM)
										fr2=sds_angle(pt0,pt3);
								}else{
									ret=RTNORM;
									if(lenmode&1){
										fr2=pt4[2]+ang/pt4[0];
									}else if(lenmode&2){
										fr2=pt4[1]+ang/pt4[0];
									}
								}
								sds_polar(pt0,fr2,pt4[0],pt3);
								ic_normang(&pt4[1],&fr2);
								if(fr2-pt4[1]>IC_TWOPI){
									cmd_ErrorPrompt(CMD_ERR_ARCSIZE,0);
									errorflag=1;
									continue;
								}
								fr2=tan((fr2-pt4[1])/4.0);
							}else{			//clockwise end of pline
								if((lenmode&16)==0){
									ret=SDS_dragobject(31,1,pt1,pt0,NULL,ResourceString(IDC_CMDS8__NNEW_ENDING_ANGL_48, "\nNew ending angle: " ),NULL,pt3,NULL);
									if(ret==RTNORM)
										fr2=sds_angle(pt0,pt3);
								}else{
									ret=RTNORM;
									if(lenmode&1){
										fr2=pt4[1]-length/pt4[0];
									}else if(lenmode&2){
										fr2=pt4[2]-length/pt4[0];
									}
								}
								sds_polar(pt0,fr2,pt4[0],pt3);
								ic_normang(&fr2,&pt4[2]);
								if(pt4[2]-fr2>IC_TWOPI){
									cmd_ErrorPrompt(CMD_ERR_ARCSIZE,0);
									errorflag=1;
									continue;
								}
								fr2=-tan((pt4[2]-fr2)/4.0);
							}
						}
						if(ret!=RTNORM){rc=ret;goto bail;}
						ic_assoc(rbptemp,10);
						ic_ptcpy(ic_rbassoc->resval.rpoint,pt3);
						ic_assoc(rb,42);
						ic_rbassoc->resval.rreal=fr2;
						if((ret=sds_entmod(rb))!=RTNORM){rc=ret;goto bail;}
						if((ret=sds_entmod(rbptemp))!=RTNORM){rc=ret;goto bail;}
						sds_entupd(ent);	//again, no entmake, so don't reset any vars
						IC_RELRB(rbptemp);
					}
				}
			} else if(strisame("CIRCLE"/*DNT*/,ic_rbassoc->resval.rstring)) {
				if(lenmode & 16){
					cmd_ErrorPrompt(CMD_ERR_LENBYANG,0);
					errorflag=1;
					continue;
				}
				ic_assoc(rb,40);
				if(lenmode & 1)ic_rbassoc->resval.rreal+=length/IC_TWOPI;
				else if(lenmode & 2)ic_rbassoc->resval.rreal=length/IC_TWOPI;
				else if(lenmode & 4)ic_rbassoc->resval.rreal*=percent/100.0;
				else if(lenmode & 8){
					ic_assoc(rb,10);
					ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
					if((ret=SDS_dragobject(1,1,pt1,pt2,fr1,ResourceString(IDC_CMDS8__NNEW_RADIUS_FOR__53, "\nNew radius for circle: " ),NULL,pt3,NULL))!=RTNORM){
						rc=RTERROR;
						goto bail;
					}
					ic_assoc(rb,40);
					ic_rbassoc->resval.rreal=sds_distance(pt1,pt3);
				}
				if((ret=sds_entmod(rb))!=RTNORM){sds_entdel(ent);rc=ret;goto bail;}
			} else {
				//if it is not one of these the report error
				cmd_ErrorPrompt(CMD_ERR_LENGTHENENT,0);
				errorflag=1;
			}
		}

bail:
	sds_entmake(NULL);
	undo_temp=undo_cur=undo_beg;
	while(undo_cur!=NULL){
		undo_temp=undo_cur->next;
		cmd_free_elistll(&undo_cur->elistll,NULL);
		delete undo_cur;
//		undo_cur=NULL;
		undo_cur=undo_temp;
	}
	undo_beg=NULL;
	if(rb!=NULL){sds_relrb(rb);rb=NULL;}
	return(rc);
}

//====================================================================================

void cmd_polar(sds_point startpt,sds_point vec,sds_real length,sds_point endpt){

	//make the vector a unit vector
	cmd_rpt_div(vec,sqrt(pow(vec[0],2)+pow(vec[1],2)+pow(vec[2],2)),vec);
	//add the segment to get the new point
	endpt[0]=startpt[0]+vec[0]*length;
	endpt[1]=startpt[1]+vec[1]*length;
	endpt[2]=startpt[2]+vec[2]*length;
}

//====================================================================================

short cmd_dview(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
	short ret=0;
	struct resbuf *rbp1=NULL,setgetrb;
	sds_point pt1={0.0,0.0,0.0},olducsxdir,olducsydir;
	sds_name etemp;
	long fl1;

	if(SDS_CURDWG->ret_nmainents()<=SDS_CURDWG->ret_ntabrecs(DB_VXTAB,1)){
		cmd_ErrorPrompt(CMD_ERR_EMPTYDWG,0);
		return(RTERROR);
	}


	SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(olducsxdir,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(olducsydir,setgetrb.resval.rpoint);

	//get the picks
	//if((ret=sds_pmtssget("\nENTER to use DVIEWBLOCK/<Select entities to see in preview>: ",NULL,NULL,NULL,NULL,ssname,0))==RTCAN) {goto exit;}
	if((ret=sds_pmtssget(ResourceString(IDC_CMDS8__NENTER_FOR_ALL_E_55, "\nENTER for all entities/<Select entities to see in preview>: " ),NULL,NULL,NULL,NULL,cmd_dview_sset,0))==RTCAN) {goto exit;}
	else if(ret==RTERROR){
		/*
		//they want to use the ICAD view symbol...
		//we have to create a set to drag - insert a block and
		//call it DVIEWBLK
		sds_grclear();
		setgetrb.resval.rstring=NULL;
		setgetrb.rbnext=NULL;
		SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		viewmode=setgetrb.resval.rint;
		if(viewmode&1){
			if((ret=SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { goto exit; }
		}else{
			if((ret=SDS_getvar(NULL,DB_QVIEWCTR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { goto exit; }
		}
		ic_ptcpy(targetpt,setgetrb.resval.rpoint);
	   if((rbp1=sds_tblsearch("BLOCK"DNT,CMD_DVIEWBLOCK,0))==NULL) {
			//build the dviewblk
			if((ret=cmd_dview_mkdviewblk())!=RTNORM)goto exit;
		}else{
			sds_relrb(rbp1);
			rbp1=NULL;
		}
		SDS_getvar(NULL,DB_QVIEWSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	   if((rbp1=sds_buildlist(RTDXF0,"INSERT"DNT,2,CMD_DVIEWBLOCK,10,targetpt,
					41,setgetrb.resval.rreal/5.0,42,setgetrb.resval.rreal/5.0,43,
					setgetrb.resval.rreal/5.0,0))==NULL){ret=RTERROR;goto exit;}
		ret=sds_entmake(rbp1);
		sds_relrb(rbp1);
		rbp1=NULL;
		if(ret!=RTNORM) goto exit;
		sds_entlast(ent);
		sds_ssadd(ent,NULL,ssname);
		iconmode=1;*/
		cmd_dview_sset[0]=cmd_dview_sset[1]=0L;
	}


	//	NOTE: we need to switch to WCS if worlduview==1
	if(SDS_getvar(NULL,DB_QWORLDVIEW,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(setgetrb.resval.rint){
		if(ret=SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)goto exit;
		ic_ptcpy(olducsxdir,setgetrb.resval.rpoint);
		if(ret=SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)goto exit;
		ic_ptcpy(olducsydir,setgetrb.resval.rpoint);
		if(fabs(olducsxdir[0])<1.0-CMD_FUZZ || fabs(olducsydir[1])<1.0-CMD_FUZZ){
			sds_printf(ResourceString(IDC_CMDS8__NSWITCHING_TO_WC_58, "\nSwitching to WCS." ));
			setgetrb.restype=RT3DPOINT;
			setgetrb.resval.rpoint[1]=setgetrb.resval.rpoint[2]=0.0;
			setgetrb.resval.rpoint[0]=1.0;
			sdsengine_setvar("UCSXDIR"/*DNT*/,&setgetrb);
			setgetrb.resval.rpoint[0]=0.0;
			setgetrb.resval.rpoint[1]=1.0;
			sdsengine_setvar("UCSYDIR"/*DNT*/,&setgetrb);
			setgetrb.restype=RTSTR;
			setgetrb.resval.rstring=fs1;
			fs1[0]=0;
			ret=sdsengine_setvar("UCSNAME"/*DNT*/,&setgetrb);
		}
	}



	while(1) {
		if(cmd_dview_sset[0]!=0L){
			sds_grclear();
			for(fl1=0;RTNORM==sds_ssname(cmd_dview_sset,fl1,etemp) &&
					RTNORM==sds_redraw(etemp,IC_REDRAW_DRAW);fl1++);
		}
		LOAD_COMMAND_OPTIONS_2(IDC_CMDS8_ROTATE_CAMERA_CAM_62)
		if(sds_initget(0, LOADEDSTRING)!=RTNORM) {goto exit;}
		if((ret=sds_getpoint(NULL,ResourceString(IDC_CMDS8__NDVIEW___CAMERA__63, "\nDView:  CAmera/TArget/TWist/Distance/POints/Off/Hide/PAn/Zoom/<eXit>: " ),pt1))==RTERROR) {
//		  if(sds_initget(0,"Rotate_Camera|CAmera Rotate_Target|TArget Twist_view|TWist ~ Distance_to_Target|Distance X,Y,Z_Points|POints Clipping_planes|CLip Perspective_off|Off ~ Hide Pan|PAn Zoom ~ Undo Exit|eXit")!=RTNORM) {goto exit;}
//		  if((ret=sds_getpoint(NULL,"\nDView:  CAmera/TArget/TWist/Distance/POints/CLip/Off/Hide/PAn/Zoom/Undo/<eXit>: ",pt1))==RTERROR) {
			goto exit;
		} else if(ret==RTCAN) {
			goto exit;
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
		} else if(ret==RTNONE) {
			strcpy(fs1,"EXIT"/*DNT*/);
		} else if(ret==RTNORM) {
			if((ret=sds_getpoint(NULL,ResourceString(IDC_CMDS8__NENTER_DIRECTION_65, "\nEnter direction and magnitude angles: " ),pt1))==RTERROR) {
				goto exit;
			}
		}


		if(strisame("CAMERA"/*DNT*/,fs1)) {
			cmd_dview_target=1;
			ret=cmd_viewctl();
			//cmd_dview_cameramode=0;
			//if((ret=cmd_dviewcamera(ssname))!=RTNORM)continue;
		} else if(strisame("TARGET"/*DNT*/,fs1)) {
			cmd_dview_target=2;
			ret=cmd_viewctl();
			//cmd_dview_cameramode=4;	//set bit 2 to reverse view direction
			//if((ret=cmd_dviewcamera(ssname))!=RTNORM)continue;
			//if((ret=cmd_dviewtarget(ssname))!=RTNORM) goto exit;
		} else if(strisame("DISTANCE"/*DNT*/,fs1)) {
			if((ret=cmd_dviewdistance())!=RTNORM) {
				continue;
			}
		} else if(strisame("POINTS"/*DNT*/,fs1)) {
			if((ret=cmd_dviewpoints())!=RTNORM) {
				continue;
			}
		} else if(strisame("PAN"/*DNT*/,fs1)) {
			if((ret=cmd_dviewpan())!=RTNORM) {
				continue;
			}
		} else if(strisame("ZOOM"/*DNT*/,fs1)) {
			if((ret=cmd_dviewzoom())!=RTNORM) {
				continue;
			}
		} else if(strisame("TWIST"/*DNT*/,fs1)) {
			if((ret=cmd_dviewtwist())!=RTNORM) {
				continue;
			}
		} else if(strisame("CLIP"/*DNT*/,fs1)) {
			cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0);
			continue;
//			  if((ret=cmd_dviewclip())!=RTNORM) {
//				  continue;
//			  }
		} else if(strisame("HIDE"/*DNT*/,fs1)) {
			if((ret=cmd_dviewhide())!=RTNORM) {
				continue;
			}
		} else if(strisame("OFF"/*DNT*/,fs1)) {
			if((ret=cmd_dviewoff())!=RTNORM) {
				continue;
			}
		} else if(strisame("UNDO"/*DNT*/,fs1)) {
			cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0);
			continue;
//			  if((ret=cmd_dviewundo())!=RTNORM) {
//				continue;
//			  }
		} else if(strisame("EXIT"/*DNT*/,fs1) || ret==RTNONE) {
			if((ret=cmd_dviewexit())==RTNORM) {
				goto exit;
			} else {
				goto exit;
			}
		}
		if(ret==RTNORM && !strisame(fs1,"TARGET"/*DNT*/) && !strisame(fs1,"CAMERA"/*DNT*/))
			SDS_ZoomIt(NULL,1,NULL,NULL,NULL,NULL,NULL,NULL);
	}
	ret=(RTNORM);

exit:
	sds_ssfree(cmd_dview_sset);
	cmd_dview_sset[0]=cmd_dview_sset[1]=0L;
	cmd_dview_target=0;

	setgetrb.restype=RT3DPOINT;
	ic_ptcpy(setgetrb.resval.rpoint,olducsxdir);
	sdsengine_setvar("UCSXDIR"/*DNT*/,&setgetrb);
	ic_ptcpy(setgetrb.resval.rpoint,olducsydir);
	sdsengine_setvar("UCSYDIR"/*DNT*/,&setgetrb);

	//if(iconmode){
	//	sds_entdel(ent);
	//	sds_redraw(NULL,IC_REDRAW_DRAW);
	//}
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	return(ret);
}

//=================================================================================
/*short cmd_dviewcamera(sds_name sset) {
	//Adjusting the camera/target has 2 parts: 1)Setting the angle FROM the XY plane, and
	//	2)Setting the rotation axis about the Z axis.  The user can lock down the
	//	first angle or let it float.  Vertical cursor movement controls item 1, and
	//	horizontal cursor movement controls item 2.  We'll allow them to lock down
	//	either angle prior to selecting the second.(Acad only allows locking of item
	//	1 for selection of item 2.
	//NOTE: Bits for cmd_dview_cameramode are as follows:
	//	bitwise flag to tell us which angle(s) we have.
	//	Bit 0 = angle in XY plane has been locked
	//	Bit 1 = angle from XY plane has been locked
	//	bit 2 = set if we're moving target, not camera
	//	NOTE ON BIT 2: almost no changes req'd in this f'n.  Callback for
	//		sds_draggen will take care of everything.

	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],prompt[IC_ACADBUF];
	short ret=RTNORM,planemode;
	sds_real fr1,fr2,viewxsize,viewysize,viewdirsize;
	sds_point targetpt={0.0,0.0,0.0},pt;
	struct resbuf setgetrb;

	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;
	SDS_getvar(NULL,DB_QVIEWDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewdirsize=sds_distance(targetpt,setgetrb.resval.rpoint);
	if(fabs(setgetrb.resval.rpoint[0])>CMD_FUZZ)
		fr1=atan(setgetrb.resval.rpoint[1]/setgetrb.resval.rpoint[0]);
	else if(setgetrb.resval.rpoint[1]>=0.0)
		fr1=IC_PI;
	else
		fr1=-IC_PI;
	fr2=sqrt(setgetrb.resval.rpoint[0]*setgetrb.resval.rpoint[0]+
		setgetrb.resval.rpoint[1]*setgetrb.resval.rpoint[1]);
	if(fabs(fr2)>CMD_FUZZ)
		fr2=atan(setgetrb.resval.rpoint[2]/fr2);
	else if(setgetrb.resval.rpoint[2]>=0.0)
		fr2=IC_PI;
	else
		fr2=-IC_PI;

	SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(setgetrb.resval.rint&1){
		if((ret=SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { goto exit; }
	}else{
		if((ret=SDS_getvar(NULL,DB_QVIEWCTR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { goto exit; }
	}
	ic_ptcpy(targetpt,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QVIEWSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewysize=setgetrb.resval.rreal;
	SDS_getvar(NULL,DB_QSCREENSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewxsize=viewysize*setgetrb.resval.rpoint[0]/setgetrb.resval.rpoint[1];
	//we now have the selection set to view w/camera...

	planemode=0;//controls which plane we're currently asking for angle in:
				//0=angle in XY plane, 1=angle from XY plane

	while(1){
		if(planemode==0){
			sds_angtos(fr2,-1,-1,fs2);
			if(sds_initget(RSG_OTHER|SDS_RSG_NOPMENU,ResourceString(IDC_CMDS8_Z_ROTATION_ZROTAT_77, "Z_Rotation|ZRotation ~Rotation ~Toggle" ))!=RTNORM) goto exit;
			sprintf(prompt,ResourceString(IDC_CMDS8__NROTATION_ANGLE__78, "\nRotation angle about Z axis/Enter angle relative to XY plane <%s>: " ),fs2);
		}else{
			sds_angtos(fr1,-1,-1,fs2);
			if(sds_initget(RSG_OTHER|SDS_RSG_NOPMENU,ResourceString(IDC_CMDS8_X_Y_PLANE_XYPLANE_79, "X-Y_Plane|XYPlane ~Angle ~Toggle" ))!=RTNORM) goto exit;
			sprintf(prompt,ResourceString(IDC_CMDS8__NANGLE_RELATIVE__80, "\nAngle relative to XY plane/Enter rotation angle about Z axis <%s>: " ),fs2);
		}
		ret=sds_draggen(sset,prompt,1,cmd_dview_dragmatrix,pt);
		if(ret==RTERROR || ret==RTCAN) goto exit;
		if(ret==RTKWORD || ret==RTSTR){
			if((ret=sds_getinput(fs1))!=RTNORM) goto exit;
			if(RTNORM==sds_wcmatch(fs1,ResourceString(IDC_CMDS8_XYPLANE_ANGLE_TOG_81, "XYPlane,Angle,Toggle,ZRotation,Rotation" ))){
				//a key word
				//NOTE: if they manually want to toggle betw angles, lose all data & set
				//	cameramode back to 0.  If they enter an angle, we'll keep the data
				planemode=!planemode;
				cmd_dview_cameramode=0;
				continue;
			}
			//else an angle to convert....
			if((ret=sds_angtof(fs1,-1,&fr1))!=RTNORM){
				cmd_ErrorPrompt(CMD_ERR_ANG2NDPT,0);
				continue;
			}
			if(!planemode){
				//angle specified is for "elevation" above XY plane
				if((cmd_dview_cameramode & 2)==0) cmd_dview_cameramode+=2;
				planemode=!planemode;
				cmd_dview_zang=fr1;
			}else{
				//angle specified is for angle in XY plane
				if((cmd_dview_cameramode & 1)==0) cmd_dview_cameramode+=1;
				planemode=!planemode;
				cmd_dview_xyang=fr1;
			}
			if((cmd_dview_cameramode & 3)==3){
				//ret=RTNORM;
				break;
			}
		}else if(ret==RTNORM){
			if((cmd_dview_cameramode & 2)==0){
				cmd_dview_zang=IC_PI*(pt[1]-targetpt[1])/viewysize;
			}
			if((cmd_dview_cameramode & 1)==0){
				cmd_dview_xyang=IC_PI*(pt[0]-targetpt[0])/viewxsize;
			}
			break;
		}
	}
	//Now we have cmd_dview_zang & cmd_dview_xyang
	//viewdirsize has MAGNITUDE of current viewdir var
	pt[0]=cos(cmd_dview_xyang);
	pt[1]=sin(cmd_dview_xyang);
	pt[2]=tan(cmd_dview_zang);
	//pt[2]=sin(cmd_dview_zang);
	if(cmd_dview_cameramode & 4){	//if we're moving target, not camera...
		pt[0]=-pt[0];
		pt[1]=-pt[1];
		pt[2]=-pt[2];
	}
	fr1=1.0+pt[2]*pt[2];
	setgetrb.resval.rpoint[0]=pt[0]*viewdirsize/fr1;
	setgetrb.resval.rpoint[1]=pt[1]*viewdirsize/fr1;
	setgetrb.resval.rpoint[2]=pt[2]*viewdirsize/fr1;
	setgetrb.restype=RT3DPOINT;
	ret=sds_setvar("VIEWDIR" ),&setgetrb);
exit:
	return(ret);
}*/

/*int cmd_dview_dragmatrix(sds_point dragpt, sds_matrix mtx) {
	//callback f'n for dview's CAMERA option
	struct resbuf setgetrb;
	sds_matrix mxy,mz,mt,mtemp;
	sds_point targetpt, pt;
	sds_real angsinxy,angcosxy,angsinz,angcosz,vsizex,vsizey;
	struct gra_view *view=SDS_CURGRVW;

	gra_ucs2rp(dragpt,pt,view);

	//***NOTE: code here at top of f'n may be replaced by global vars
	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;
	if(SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(setgetrb.resval.rint&1){
		if(SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	}else{
		if(SDS_getvar(NULL,DB_QVIEWCTR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	}
	ic_ptcpy(targetpt,setgetrb.resval.rpoint);
	if(SDS_getvar(NULL,DB_QVIEWSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	vsizey=setgetrb.resval.rreal;
	if(SDS_getvar(NULL,DB_QSCREENSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	vsizex=vsizey*setgetrb.resval.rpoint[0]/setgetrb.resval.rpoint[1];
	//***

	if(cmd_dview_cameramode & 1){
		angsinxy=sin(cmd_dview_xyang);
		angcosxy=cos(cmd_dview_zang);
	}else{
		angsinxy=sin(IC_PI*(pt[0]-targetpt[0])/vsizex);
		angcosxy=cos(IC_PI*(pt[0]-targetpt[0])/vsizex);
	}
	if(cmd_dview_cameramode & 2){
		angsinz=sin(cmd_dview_zang);
		angcosz=cos(cmd_dview_zang);
	}else{
		angsinz=sin(IC_PI*(pt[1]-targetpt[1])/vsizey);
		angcosz=cos(IC_PI*(pt[1]-targetpt[1])/vsizey);
	}
	if(cmd_dview_cameramode & 4){
		//if we're actually moving the TARGET, not the camera, the angles are
		//the opposite direction.  Cos(x + pi)=-Cos(x), and Sin(x + pi)=-Sin(x)
		//so we have to reverse the sign of the calcs
		angsinz=-angsinz;
		angsinxy=-angsinxy;
		angcosz=-angcosz;
		angcosxy=-angcosxy;
	}
	mt[0][1]=mt[0][2]=mt[1][0]=mt[1][2]=mt[2][0]=mt[2][1]=mt[3][0]=mt[3][1]=mt[3][2]=0.0;
	mt[0][0]=mt[1][1]=mt[2][2]=mt[3][3]=1.0;
	mt[0][3]=targetpt[0];
	mt[1][3]=targetpt[1];
	mt[2][3]=targetpt[2];

	//rotation in XZ plane (to be done 1st);
	mz[0][0]=mz[2][2]=angcosz;
	mz[0][2]=angsinz;
	mz[2][0]=-angsinz;
	mz[0][1]=mz[1][0]=mz[1][2]=mz[2][1]=mz[3][0]=mz[3][1]=mz[3][2]=0.0;
	mz[0][3]=mz[1][3]=mz[2][3]=0.0;
	mz[1][1]=mz[3][3]=1.0;

	//rotation in XY plane (to be done 2nd);
	mxy[0][0]=mxy[1][1]=angcosxy;
	mxy[0][1]=-angsinxy;
	mxy[1][0]=angsinxy;
	mxy[0][2]=mxy[0][3]=mxy[1][2]=mxy[1][3]=mxy[2][0]=mxy[2][1]=mxy[2][3]=mxy[3][0]=mxy[3][1]=mxy[3][2]=0.0;
	mxy[2][2]=mxy[3][3]=1.0;

	cmd_matrix_multiply(mt,mz,mtemp);
	cmd_matrix_multiply(mtemp,mxy,mz);	//overwrite mz
	mt[0][3]=-targetpt[0];
	mt[1][3]=-targetpt[1];
	mt[2][3]=-targetpt[2];
	cmd_matrix_multiply(mz,mt,mtx);

	return(RTNORM);
}*/

void cmd_matrix_multiply(sds_matrix m1, sds_matrix m2, sds_matrix mresult){
	sds_real total;
	short i,row,column;

	for(row=0;row<4;row++){
		for(column=0;column<4;column++){
			total=0.0;
			for(i=0;i<4;i++){
				total+=m1[row][i]*m2[i][column];
			}
			mresult[row][column]=total;
		}
	}
}


/*short  cmd_dview_mkdviewblk(void){
	struct resbuf *rbp1, *rbpstart, *rbpend, *rbptemp, *rbpcolor;
	sds_point normal={0.0,0.0,0.0};
	short ret=RTNORM,topcolor,sidecolor,bottomcolor,fi1;
	sds_real axislen=1.0,cubelen=0.4,textsize=0.6;

	rbp1=rbpstart=rbpend=rbptemp=rbpcolor=NULL;
	//topcolor=5;
	//sidecolor=3;
	//bottomcolor=1;
	topcolor=256;
	sidecolor=256;
	bottomcolor=256;


   if((rbp1=sds_buildlist(RTDXF0,"BLOCK"DNT,2,CMD_DVIEWBLOCK,70,0,10,normal,0))==NULL)
		{ret=RTERROR;goto exit;}
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	//make the three axes....
	if((rbp1=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,normal,11,normal,62,256,0))==NULL)
		{ret=RTERROR;goto exit;}
	for(rbptemp=rbp1;rbptemp!=NULL;rbptemp=rbptemp->rbnext){
		if(rbptemp->restype==10)rbpstart=rbptemp;
		else if(rbptemp->restype==11)rbpend=rbptemp;
		else if(rbptemp->restype==62)rbpcolor=rbptemp;
	}
	rbpstart->resval.rpoint[0]=cubelen;
	rbpend->resval.rpoint[0]=axislen;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	rbpstart->resval.rpoint[0]=rbpend->resval.rpoint[0]=0.0;
	rbpend->resval.rpoint[1]=axislen;
	rbpstart->resval.rpoint[1]=cubelen;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	rbpstart->resval.rpoint[1]=rbpend->resval.rpoint[1]=0.0;
	rbpend->resval.rpoint[2]=axislen;
	rbpstart->resval.rpoint[2]=cubelen;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;

	//draw the top/bottom of the cube
	for(fi1=0;fi1<2;fi1++){
		if(fi1==0){
			rbpcolor->resval.rint=bottomcolor;
			rbpstart->resval.rpoint[2]=rbpend->resval.rpoint[2]=0.0;
		}else{
			rbpcolor->resval.rint=topcolor;
			rbpstart->resval.rpoint[2]=rbpend->resval.rpoint[2]=cubelen;
		}
		rbpstart->resval.rpoint[0]=rbpstart->resval.rpoint[1]=rbpend->resval.rpoint[1]=0.0;
		rbpend->resval.rpoint[0]=cubelen;
		if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
		rbpstart->resval.rpoint[0]=rbpstart->resval.rpoint[1]=cubelen;
		if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
		rbpend->resval.rpoint[0]=0.0;
		rbpend->resval.rpoint[1]=cubelen;
		if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
		rbpstart->resval.rpoint[0]=rbpstart->resval.rpoint[1]=0.0;
		if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	}
	//draw the sides of the cube
	rbpcolor->resval.rint=sidecolor;
	rbpstart->resval.rpoint[2]=0.0;
	rbpend->resval.rpoint[2]=cubelen;
	rbpstart->resval.rpoint[0]=rbpstart->resval.rpoint[1]=rbpend->resval.rpoint[0]=rbpend->resval.rpoint[1]=0.0;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	rbpstart->resval.rpoint[0]=rbpend->resval.rpoint[0]=cubelen;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	rbpstart->resval.rpoint[1]=rbpend->resval.rpoint[1]=cubelen;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	rbpstart->resval.rpoint[0]=rbpend->resval.rpoint[0]=0.0;
	if((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=rbpstart=rbpend=rbpcolor=rbptemp=NULL;}
   if((rbp1=sds_buildlist(RTDXF0,"ENDBLK"DNT,0))==NULL)
		{ret=RTERROR;goto exit;}
	if((ret=sds_entmake(rbp1))!=RTKWORD)
		ret=RTERROR;
	else
		ret=RTNORM;
exit:
	if(rbp1!=NULL){sds_relrb(rbp1);rbp1=rbpstart=rbpend=rbpcolor=rbptemp=NULL;}
	return(ret);
}*/

//================================================================================
/*short cmd_dviewtarget(sds_name sset) {
	char fs1[IC_ACADBUF],prompt[IC_ACADBUF];
	sds_real ang=0.0;
	short ret=0;
	sds_point targetpt={0.0,0.0,0.0};
	struct resbuf setgetrb;

	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;
	if((ret=SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	ic_ptcpy(targetpt,setgetrb.resval.rpoint);

	while(1) {
		if(ret=sds_initget(0,ResourceString(IDC_CMDS8_TOGGLE_84, "Toggle" ))!=RTNORM) return(ret);
		sprintf(prompt,ResourceString(IDC_CMDS8__NTOGGLE_ANGLE_IN_85, "\nToggle angle in/Enter angle from XY plane <>: " ));
		if((ret=sds_getorient(targetpt,prompt,&ang))==RTERROR) {					// this will do, rewrite the command
			return(ret);														// line in clearer language.
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
		} else if(ret==RTCAN) {
			return(ret);
		}
		if(ret=sds_initget(0,ResourceString(IDC_CMDS8_TOGGLE_84, "Toggle" ))!=RTNORM) return(ret);
		sprintf(prompt,ResourceString(IDC_CMDS8__NTOGGLE_ANGLE_FR_86, "\nToggle angle from/Enter angle in X plane <>: " ));
		if((ret=sds_getorient(targetpt,prompt,&ang))==RTERROR) {
			return(ret);
		} else if(ret==RTNONE) {
			break;
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
		} else if(ret==RTCAN) {
			return(ret);
		}
	}
	return(RTNORM);
}*/

//================================================================================

short cmd_dviewdistance(void) {
	short ret=0;
	char prompt[IC_ACADBUF],fs1[IC_ACADBUF];
	sds_point p1,p2,p3,p4,targetold,targetnew;
	sds_real dist1,dist2;
	struct resbuf setgetrb;
	int vmode;
	CIcadView *pView=SDS_CURVIEW;
	struct gra_view *view=SDS_CURGRVW;

	if((ret=SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	vmode=setgetrb.resval.rint;
	//we need to zoom extents for this f'n
	SDS_getvar(NULL,DB_QEXTMIN,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p1,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QEXTMAX,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p2,setgetrb.resval.rpoint);
	if(0==(vmode&1)){
		setgetrb.resval.rint=vmode|1;//set perspective mode ON, & keep old value in vmode
		//setgetrb.restype=RTSHORT;
		SDS_setvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_getvar(NULL,DB_QVIEWCTR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		//we're going to take viewctr and copy it into p3 for use as target
		ic_ptcpy(targetnew,setgetrb.resval.rpoint);
		dist1=0.5*(p1[2]+p2[2]);
		//now correct pt3 along viewdir so that it has the correct Z
		cmd_elev_correct(dist1,targetnew,1,NULL);
		SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(targetold,setgetrb.resval.rpoint);
		ic_ptcpy(setgetrb.resval.rpoint,targetnew);
		SDS_setvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_CURDOC->m_RegenListView=NULL;
		gra_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,NULL);
	}

	SDS_rpextents(p1,p2,p3,p4,NULL);
	/*
	//note: pt1 and pt5 are now in real projection plane coordinates
	p1[0]=(p3[0]+p4[0])/2.0;
	p1[1]=(p3[1]+p4[1])/2.0;
	//convert p1 (ctr of new view) into ucs coordinates
	*/
	//gra_rp2ucs(p1,p2,view);
	//setup real value to use as viewsize
	p1[0]=max(p4[1]-p3[1],fabs(p2[1]-p1[1]));

	sds_point pointtemp;
	view->GetCenterProjection( pointtemp );
	gr_rp2ucs(pointtemp,p2,view);

	//restore vmode & target to orig setting
	setgetrb.restype=RTSHORT;
	setgetrb.resval.rint=vmode;
	SDS_setvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	if(0==(vmode&1)){
		setgetrb.restype=RT3DPOINT;
		ic_ptcpy(setgetrb.resval.rpoint,targetold);
		SDS_setvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}else{
		//if we were already in perspective, maintain existing target
		ic_ptcpy(targetnew,p2);
	}
	vmode|=1;
	SDS_ZoomIt(NULL,1,targetnew,&p1[0],&vmode,NULL,NULL,NULL);

	if((ret=SDS_getvar(NULL,DB_QVIEWDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	ic_ptcpy(p1,setgetrb.resval.rpoint);
	dist1=sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2]);
	sds_rtos(dist1,-1,-1,fs1);
	sprintf(prompt,ResourceString(IDC_CMDS8__NNEW_CAMERA_TO_T_87, "\nNew camera-to-target distance <%s>: " ),fs1);
	sds_initget(0,NULL);
	if((ret=sds_getdist(p2,prompt,&dist2))!=RTNORM && ret!=RTNONE) {
		//needs undo support
		SDS_ZoomIt(NULL,0,NULL,NULL,NULL,NULL,NULL,NULL);
		return(ret);
	}
	if(ret==RTNORM){
		p1[0]*=dist2/dist1;
		p1[1]*=dist2/dist1;
		p1[2]*=dist2/dist1;
	}
	/*
	SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	p2[2]=setgetrb.resval.rreal;
	ic_ptcpy(setgetrb.resval.rpoint,p2);
	setgetrb.restype=RT3DPOINT;
	SDS_setvar(NULL,DB_QVIEWCTR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	*/
	SDS_ZoomIt(NULL,1,NULL,NULL,NULL,p1,NULL,NULL);

	return(RTNORM);
}

//================================================================================

short cmd_dviewpoints(void) {
	char prompt[IC_ACADBUF];
	short ret=0,viewmode;
	sds_point targetpt={0.0,0.0,0.0},camerapt={0.0,0.0,0.0};
	struct resbuf setgetrb;

	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;
	if((ret=SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	viewmode=setgetrb.resval.rint;
	if(viewmode&1){
		if((ret=SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	}else{
		if((ret=SDS_getvar(NULL,DB_QVIEWCTR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	}
	ic_ptcpy(targetpt,setgetrb.resval.rpoint);
	if((ret=SDS_getvar(NULL,DB_QVIEWDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	ic_ptcpy(camerapt,setgetrb.resval.rpoint);

//4M Spiros Item:126->
//Because camera should be the 2nd point and not the direction
#ifndef __BUILD_WITH_OLD_DVIEW_CAMERA_POINT__
	camerapt[0]+=targetpt[0];
	camerapt[1]+=targetpt[1];
	camerapt[2]+=targetpt[2];
#endif
//4M Spiros Item:126<-
	sprintf(prompt,ResourceString(IDC_CMDS8__NENTER_TARGET_PO_88, "\nEnter target point <%.4f, %.4f, %.4f>: " ),targetpt[0],targetpt[1],targetpt[2]);
	if((ret=sds_getpoint(targetpt,prompt,targetpt))!=RTNORM && ret!=RTNONE) { return(ret); }
	sprintf(prompt,ResourceString(IDC_CMDS8__NENTER_CAMERA_PO_89, "\nEnter camera point <%.4f, %.4f, %.4f>: " ),camerapt[0],camerapt[1],camerapt[2]);
	if((ret=sds_getpoint(targetpt,prompt,camerapt))!=RTNORM && ret!=RTNONE) { return(ret); }

	camerapt[0]-=targetpt[0];
	camerapt[1]-=targetpt[1];
	camerapt[2]-=targetpt[2];

	if(viewmode&1)viewmode=1;
	else viewmode=2;
	SDS_ZoomIt(NULL,viewmode,targetpt,NULL,NULL,camerapt,NULL,NULL);

	return(RTNORM);
}

//================================================================================

short cmd_dviewpan(void) {
	sds_point pt1={0.0,0.0,0.0},pt2={0.0,0.0,0.0};
	char prompt[IC_ACADBUF];
	short ret=0;
	struct gra_view *view=SDS_CURGRVW;
	struct resbuf rbdcs, rbucs;

	if((ret=SDS_getvar(NULL,DB_QVIEWMODE,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	if(0==(rbucs.resval.rint&1))return(cmd_pan());

	//NOTE: don't use cmd_pan() because it's not allowed in perspective and
	//	has far too many options on it that we may not be able to do here
	sprintf(prompt,ResourceString(IDC_CMDS8__NDISPLACEMENT_BA_90, "\nDisplacement base point: " ));
	if((ret=sds_getpoint(NULL,prompt,pt1))!=RTNORM) { return(ret); }
	sprintf(prompt,ResourceString(IDC_CMDS8__NSECOND_POINT___91, "\nSecond point: " ));
	if((ret=sds_getpoint(NULL,prompt,pt2))!=RTNORM) { return(ret); }
	gr_ucs2rp(pt1,pt1,view);
	gr_ucs2rp(pt2,pt2,view);
	//we're moving target, get the negative of the vector between the 2 pts
	//	and call it pt1
	pt1[0]-=pt2[0];
	pt1[1]-=pt2[1];
	pt1[2]=0.0;
	rbdcs.restype=rbucs.restype=RTSHORT;
	rbdcs.resval.rint=2;
	rbucs.resval.rint=1;
	sds_trans(pt1,&rbdcs,&rbucs,1,pt2);
	if((ret=SDS_getvar(NULL,DB_QTARGET,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	rbucs.resval.rpoint[0]+=pt2[0];
	rbucs.resval.rpoint[1]+=pt2[1];
	rbucs.resval.rpoint[2]+=pt2[2]; /*D.G.*/// was pt[1]
	SDS_ZoomIt(NULL,1,rbucs.resval.rpoint,NULL,NULL,NULL,NULL,NULL);

	return(ret);
}

//================================================================================

short cmd_dviewzoom(void) {
	//adjusts lenslength variable
	sds_real zfact=0.0;
	char prompt[IC_ACADBUF],fs1[IC_ACADBUF];
	short ret=0;
	int vmode;
	struct resbuf setgetrb;
	CIcadView *pView=SDS_CURVIEW;

	if((ret=SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	vmode=setgetrb.resval.rint;
	if(vmode&1){
		if((ret=SDS_getvar(NULL,DB_QLENSLENGTH,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
		sds_initget(RSG_NONEG|RSG_NOZERO,ResourceString(IDC_CMDS8__20_MM_20__35_MM__92, "_20_mm|20 _35_mm|35 _50_mm|50 _80_mm|80 120mm|120 ~_20 ~_35 ~_50 ~_80 ~_120" ));
		sds_rtos(setgetrb.resval.rreal,2,0,fs1);
		sprintf(prompt,ResourceString(IDC_CMDS8__NLENS_LENGTH__MM_93, "\nLens length (mm) to use for viewing <%s> " ),fs1);
	}else{
		if((ret=SDS_getvar(NULL,DB_QVIEWSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
		sds_initget(RSG_NONEG|RSG_NOZERO,NULL);
		sds_rtos(setgetrb.resval.rreal,-1,-1,fs1);
		sprintf(prompt,ResourceString(IDC_CMDS8__NVIEW_SIZE___S___94, "\nView size <%s> " ),fs1);
	}
	if((ret=sds_getreal(prompt,&zfact))==RTNONE){
		return(RTNORM);
	}else if(ret!=RTNORM && ret!=RTKWORD){
		return(ret);
	}
	if(ret==RTKWORD){
		sds_getinput(fs1);
		sds_distof(fs1,2,&zfact);//this should work on ALL key words
	}
	setgetrb.resval.rreal=zfact;
	if(vmode&1)
		ret=sds_setvar("LENSLENGTH"/*DNT*/,&setgetrb);
	else
		ret=sdsengine_setvar("VIEWSIZE"/*DNT*/,&setgetrb);
	SDS_CURDOC->m_RegenListView=NULL;
	gra_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,NULL);
	return(ret);
}

//================================================================================

short cmd_dviewtwist(void) {
	sds_real twist=0.0,fr1;
	struct resbuf setgetrb;
	char prompt[IC_ACADBUF],szTmp[IC_ACADBUF];
	short ret=0;
	int fi1;
	CIcadView *pView=SDS_CURVIEW;

	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;
	if((ret=SDS_getvar(NULL,DB_QANGBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	fr1=setgetrb.resval.rreal;
	if((ret=SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	fi1=setgetrb.resval.rint;
	if((ret=SDS_getvar(NULL,DB_QVIEWTWIST,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	//offset angto's use of angdir and angbase
	if(fi1==0)
		fr1=setgetrb.resval.rreal+fr1;
	else
		fr1=setgetrb.resval.rreal-fr1;
	if(fi1==1)fr1-=IC_TWOPI;
	sds_angtos(fr1,-1,-1,szTmp);
	sprintf(prompt,ResourceString(IDC_CMDS8__NNEW_VIEW_TWIST__97, "\nNew view twist <%s>: " ),szTmp);
	if((ret=sds_getorient(NULL,prompt,&twist))!=RTNORM) { return(ret); }

	setgetrb.restype=RTREAL;
	setgetrb.resval.rreal=twist;
	if((ret=sdsengine_setvar("VIEWTWIST"/*DNT*/,&setgetrb))!=RTNORM) { return(ret); }
	SDS_CURDOC->m_RegenListView=NULL;
	gra_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,NULL);

	return(RTNORM);
}

//================================================================================
//BugZilla No. 78254; 12-08-2002
/*
short cmd_dviewclip(void) {
	char fs1[IC_ACADBUF],prompt[IC_ACADBUF],result[IC_ACADBUF];
	short ret=0;
	sds_real dist=0.0;
	sds_point targetpt={0.0,0.0,0.0};
	struct resbuf setgetrb;

	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;
	if((ret=SDS_getvar(NULL,DB_QTARGET,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	ic_ptcpy(targetpt,setgetrb.resval.rpoint);

	sprintf(prompt,ResourceString(IDC_CMDS8__NFRONT___BACK____99, "\nFront/Back/<Off>: " ));
	if(sds_initget(0,ResourceString(IDC_CMDS8_CLIPPING_OFF_OFF_100, "Clipping off|Off ~ Front Back ~_Off ~_ ~_Front ~_Back" ))!=RTNORM) return(RTERROR);
	if((ret=sds_getstring(0,prompt,result))==RTERROR) {
		return(ret);
	} else if(ret==RTCAN) {
		return(ret);
	}
	if(strisame("FRONT"/*DNT* /,result)) {
		sprintf(prompt,ResourceString(IDC_CMDS8__NTOGGLE_ANGLE_FR_86, "\nToggle angle from/Enter angle in X plane <>: " ));
		if(sds_initget(0,ResourceString(IDC_CMDS8_FRONT_CLIPPING_O_102, "Front_Clipping_on|ON Front_Clipping_off|OFf ~_ON ~_OFf" ))!=RTNORM) return(RTERROR);					// this will do, rewrite the command
		if((ret=sds_getdist(targetpt,prompt,&dist))==RTERROR) {					// line in clearer language.
			return(RTERROR);
		} else if(ret==RTNONE) {
			return(ret);
		} else if(ret==RTCAN) {
			return(ret);
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
			if(strisame("ON"/*DNT* /,fs1)) { return(RTNORM); }
			if(strisame("OFF"/*DNT* /,fs1)) { return(RTNORM); }
		}
	}
	if(strisame("BACK"/*DNT* /,result)) {
		sprintf(prompt,ResourceString(IDC_CMDS8__NTOGGLE_ANGLE_FR_86, "\nToggle angle from/Enter angle in X plane <>: " ));
		if(sds_initget(0,ResourceString(IDC_CMDS8_BACK_CLIPPING_ON_106, "Back_Clipping_on|ON Back_Clipping_off|OFf ~_ON ~_OFf" ))!=RTNORM) return(RTERROR);
		if((ret=sds_getdist(targetpt,prompt,&dist))==RTERROR) {
			return(RTERROR);
		} else if(ret==RTNONE) {
			return(ret);
		} else if(ret==RTCAN) {
			return(ret);
		} else if(ret==RTKWORD) {
			sds_getinput(fs1);
			if(strsame("ON"/*DNT* /,fs1)) { return(RTNORM); }
			if(strsame("OFF"/*DNT* /,fs1)) { return(RTNORM); }
		}
	}

	return(RTNORM);
}
*/
//BugZilla No. 78254; 12-08-2002
//================================================================================

short cmd_dviewhide(void) {
	short ret=0;

	ret=cmd_hide();

	return(ret);
}

//================================================================================

short cmd_dviewoff(void) {
	struct resbuf setgetrb;
	short ret;
	CIcadView *pView=SDS_CURVIEW;

	if((ret=SDS_getvar(NULL,DB_QVIEWMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) { return(ret); }
	if(setgetrb.resval.rint&1){
		setgetrb.resval.rint-=1;
		ret=sdsengine_setvar("VIEWMODE"/*DNT*/,&setgetrb);
		SDS_CURDOC->m_RegenListView=NULL;
		gra_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,NULL);
	}
	return(RTNORM);
}

//================================================================================

short cmd_dviewundo(void) {

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short ret=0;

	ret=cmd_undo();

	return(ret);
}

//================================================================================

short cmd_dviewexit(void) {

	return(RTNORM);
}


//=================================================================================
// ATTRIBUTE EXTRACTION
// global insert counter
static int inscount;
static int recno;

void atttranslatematrix(sds_real *pt0,sds_matrix mat)
{
/*
  double tmat[4][4];

  identity(tmat);
  memcpy(tmat[3],pt0,3*sizeof(double));
  matmult(tmat,mat,mat);
*/
  mat[3][0]=mat[0][0]*pt0[0]+mat[1][0]*pt0[1]+mat[2][0]*pt0[2]+mat[3][0];
  mat[3][1]=mat[0][1]*pt0[0]+mat[1][1]*pt0[1]+mat[2][1]*pt0[2]+mat[3][1];
  mat[3][2]=mat[0][2]*pt0[0]+mat[1][2]*pt0[1]+mat[2][2]*pt0[2]+mat[3][2];
  mat[3][3]=mat[0][3]*pt0[0]+mat[1][3]*pt0[1]+mat[2][3]*pt0[2]+mat[3][3];
}

void attmoveforrotsc(double x,double y,double z,sds_matrix mat)
{
/*
  double tmat[4][4];

  identity(tmat);
  tmat[3][0]=x;
  tmat[3][1]=y;
  tmat[3][2]=z;
  matmult(mat,tmat,mat);
*/
  mat[0][0]+=mat[0][3]*x;
  mat[0][1]+=mat[0][3]*y;
  mat[0][2]+=mat[0][3]*z;
  mat[1][0]+=mat[1][3]*x;
  mat[1][1]+=mat[1][3]*y;
  mat[1][2]+=mat[1][3]*z;
  mat[2][0]+=mat[2][3]*x;
  mat[2][1]+=mat[2][3]*y;
  mat[2][2]+=mat[2][3]*z;
  mat[3][0]+=mat[3][3]*x;
  mat[3][1]+=mat[3][3]*y;
  mat[3][2]+=mat[3][3]*z;
}

void attscalematrix(double xscale,double yscale,double zscale,sds_matrix mat)
{
/*	double tmat[4][4]; */
  double xt,yt,zt;
  register short i;

  xt=mat[3][0]; yt=mat[3][1]; zt=mat[3][2];
  attmoveforrotsc(-xt,-yt,-zt,mat);    /* trans to origin */

/*	identity(tmat);
  tmat[0][0]=xscale;
  tmat[1][1]=yscale;
  tmat[2][2]=zscale;
  matmult(tmat,mat,mat);
*/

  for (i=0; i<4; i++) {
	mat[0][i]*=xscale;
	mat[1][i]*=yscale;
	mat[2][i]*=zscale;
  }

  attmoveforrotsc(xt,yt,zt,mat);	   /* and back */
}

void attrotatematrixz(double angle,sds_matrix mat)
/* rotates about z axis only!! */
{
  double tmat[4][4];
/*	double xt,yt,zt; */
  double cosa,sina;

/*
  xt=mat[3][0]; yt=mat[3][1]; zt=mat[3][2];
  moveforrotsc(-xt,-yt,-zt,mat);

  identity(tmat);
  tmat[0][0]=cos(angle);
  tmat[0][1]=sin(angle);
  tmat[1][0] = -tmat[0][1];
  tmat[1][1]=tmat[0][0];
  matmult(tmat,mat,mat);
*/
  memcpy(tmat,mat,sizeof(sds_matrix));
  cosa=cos(angle);
  sina=sin(angle);
  tmat[0][0]=mat[0][0]*cosa+mat[1][0]*sina;
  tmat[0][1]=mat[0][1]*cosa+mat[1][1]*sina;
  tmat[0][2]=mat[0][2]*cosa+mat[1][2]*sina;
  tmat[0][3]=mat[0][3]*cosa+mat[1][3]*sina;
  tmat[1][0]=mat[1][0]*cosa-mat[0][0]*sina;
  tmat[1][1]=mat[1][1]*cosa-mat[0][1]*sina;
  tmat[1][2]=mat[1][2]*cosa-mat[0][2]*sina;
  tmat[1][3]=mat[1][3]*cosa-mat[0][3]*sina;
  memcpy(mat,tmat,sizeof(sds_matrix));
/*
  moveforrotsc(xt,yt,zt,mat);
*/
	   /* and back */
}

void atttransformpoint3d(sds_real *pakpoint,sds_matrix mat,sds_real *pakpoint2)
{
  short i;
  double tpoint[4];

  for (i=0; i<3; i++)
	tpoint[i]=
	  pakpoint[0]*mat[0][i]+
	  pakpoint[1]*mat[1][i]+
	  pakpoint[2]*mat[2][i]+
				  mat[3][i];
  memcpy(pakpoint2,tpoint,(size_t)3*sizeof(sds_real));
/*	for (i=0; i<4; i++) pakpoint2[i]=tpoint[i]; */
}

// print strings to buffer
short cmd_prtxstr2buf(char *tmp,char cQuote,char cDelim,char *outfilter,char *outputline,
					  struct cmd_xtemplate *pXtmp,int *overflowflag) {
	short rc=(RTNORM),i=0;
	char tmpstr[IC_ACADBUF];
	char tempbuf[1024];
	int outlinelen,tempbuflen;

	strcpy(tmpstr,tmp);

	if (strlen(tmpstr)>(unsigned int)pXtmp->sCharWidth) {
		*overflowflag=1;
	}

	if(pXtmp->cTypeChar=='C'/*DNT*/) {
		tmpstr[pXtmp->sCharWidth]='\0'/*DNT*/;
		if(strisame("CDF"/*DNT*/,outfilter))
			sprintf(tempbuf,"%c%-s%c"/*DNT*/,cQuote,tmpstr,cQuote);
		else  // SDF
			sprintf(tempbuf,"%-*s"/*DNT*/,pXtmp->sCharWidth,tmpstr);

		outlinelen=strlen(outputline);
		tempbuflen=strlen(tempbuf);
		if (tempbuflen+outlinelen>8190) {
			rc=RTERROR;
		}
		else {
			strcpy(&outputline[outlinelen],tempbuf);
			if (strisame("CDF"/*DNT*/,outfilter) && pXtmp->next!=NULL) {  // add the delimiter
				outlinelen+=tempbuflen;
				outputline[outlinelen]=cDelim;
				outputline[outlinelen+1]=0;
			}
		}
	}else{
		sds_printf(ResourceString(IDC_CMDS8_ERROR_IN_STRING_PRINTING, "\nError %s in string printing"),tmpstr);
		rc=(RTERROR);
	}

	return(rc);
}

// print numbers to buffer
short cmd_prtxreal2buf(sds_real tmpreal,char cDelim,char *outfilter,char *outputline,
					   struct cmd_xtemplate *pXtmp) {
	short rc=RTNORM;
	char szTmp[IC_ACADBUF];
	char tempbuf[1024];
	int outlinelen,tempbuflen;

	sds_rtos(tmpreal,-1,pXtmp->sDecWidth,szTmp);

	//CDF
	if(strisame("CDF"/*DNT*/,outfilter)) {
		if(tmpreal<0.0){
			sprintf(tempbuf,"%.*s"/*DNT*/,pXtmp->sCharWidth,szTmp);
		}else{
			sprintf(tempbuf," %.*s"/*DNT*/,pXtmp->sCharWidth,szTmp);
		}
	}else{ //SDF
		sprintf(tempbuf,"%*.*s"/*DNT*/,pXtmp->sCharWidth,pXtmp->sCharWidth,szTmp);
	}

	outlinelen=strlen(outputline);
	tempbuflen=strlen(tempbuf);
	if (tempbuflen+outlinelen>8190) {
		rc=RTERROR;
	}
	else {
		strcpy(&outputline[outlinelen],tempbuf);
		if (strisame("CDF"/*DNT*/,outfilter) && pXtmp->next!=NULL) {  // add the delimiter
			outlinelen+=tempbuflen;
			outputline[outlinelen]=cDelim;
			outputline[outlinelen+1]=0;
		}
	}
	return(rc);
}

// print numbers to buffer
short cmd_prtxint2buf(int tmpint,char cDelim,char *outfilter,char *outputline,
					   struct cmd_xtemplate *pXtmp) {
	short rc=RTNORM;
	char szTmp[IC_ACADBUF];
	char tempbuf[1024];
	int outlinelen,tempbuflen;

	sprintf(szTmp,"%-d"/*DNT*/,tmpint);

	//CDF
	if(strisame("CDF"/*DNT*/,outfilter)) {
		sprintf(tempbuf," %.*s"/*DNT*/,pXtmp->sCharWidth,szTmp);
	}else{ //SDF
		sprintf(tempbuf,"%*.*s"/*DNT*/,pXtmp->sCharWidth,pXtmp->sCharWidth,szTmp);
	}

	outlinelen=strlen(outputline);
	tempbuflen=strlen(tempbuf);
	if (tempbuflen+outlinelen>8190) {
		rc=RTERROR;
	}
	else {
		strcpy(&outputline[outlinelen],tempbuf);
		if (strisame("CDF"/*DNT*/,outfilter) && pXtmp->next!=NULL) {  // add the delimiter
			outlinelen+=tempbuflen;
			outputline[outlinelen]=cDelim;
			outputline[outlinelen+1]=0;
		}
	}
	return(rc);
}

short cmd_templinklist(const char *fileloc,char *cQuote,char *cDelim,struct cmd_xtemplate **pXbeg,int *reterror) {
	FILE *templatefile=NULL;
	struct cmd_xtemplate *pXtmp=NULL,*pXcur=NULL;
	short rc=0,ret=0,n=0,i=0;
	int ch=0;
	short sFieldType;
	char szFieldName[IC_ACADBUF];
	char cTypeChar;
	short sDecWidth=0,sCharWidth=0;
	char fieldbuf[4096],formatbuf[4096];
	char holdchar;

	*reterror=0;
	//open template file in read mode
	if((templatefile = fopen(fileloc,"r"/*DNT*/))==NULL){
		sds_printf( ResourceString(IDC_CMDS8__NTHE_FILE__S_CO_109, "\nThe file %s could not be opened.\n" ),fileloc);
		rc=RTERROR;
		goto bail;
	}

	//step through the template file and gather information
	while(feof(templatefile)==0) {
		fscanf(templatefile," %4095s %c%4095s\n",fieldbuf,&cTypeChar,formatbuf);

// TODO -- need to give an error message here
		if (strlen(fieldbuf)==0) {
			*reterror=IDS_INVALID_ATTEXT_FIELD;
			rc=RTERROR;
			goto bail;
		}

		if(sds_usrbrk()) {rc=RTCAN;goto bail;}	//check for break

		strcpy(szFieldName,fieldbuf);
		//check for type of template field
		if(strnisame(fieldbuf,"BL:"/*DNT*/,3)) {
			sFieldType=ATTX_BL;
		}else if(strnisame(fieldbuf,"C:Q"/*DNT*/,3)){
			sFieldType=ATTX_QUOTE;
		}else if(strnisame(fieldbuf,"C:D"/*DNT*/,3)){
			sFieldType=ATTX_DELIM;
		}else{
			sFieldType=ATTX_OTHER;
		}

		//get the type
		if(sFieldType==ATTX_QUOTE){
			*cQuote=fieldbuf[3];
			continue;
		}else if(sFieldType==ATTX_DELIM){
			*cDelim=fieldbuf[3];
			continue;
		}

// TODO -- need to give an error message here
		if (strlen(formatbuf)!=6) {
			*reterror=IDS_INVALID_ATTEXT_FORMAT;
			rc=RTERROR;
			goto bail;
		}

		cTypeChar=toupper(cTypeChar);
		if (cTypeChar!='C' && cTypeChar!='N') {
			*reterror=IDS_INVALID_ATTEXT_TYPE;
			rc=RTERROR;
			goto bail;
		}

		for (i=0; i<6; i++)
			if (!(formatbuf[i]>='0' && formatbuf[i]<='9')) {
				*reterror=IDS_INVALID_ATTEXT_FORMAT;
				rc=RTERROR;
				goto bail;
			}

		holdchar=formatbuf[3];
		formatbuf[3]=0;
		sCharWidth=atoi(formatbuf);
		formatbuf[3]=holdchar;
		sDecWidth=atoi(&formatbuf[3]);

		//make a new link for the field		
		if((pXtmp= new struct cmd_xtemplate )==NULL) {
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=RTERROR;
			goto bail;
		}
		memset(pXtmp,0,sizeof(struct cmd_xtemplate));
		if((pXtmp->szFieldName= new char [strlen(szFieldName)+1] )==NULL) {
			delete pXtmp;
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=RTERROR;
			goto bail;
		}
		memset(pXtmp->szFieldName,0,strlen(szFieldName)+1);
		//file link
		strcpy(pXtmp->szFieldName,szFieldName);
		pXtmp->sDecWidth=sDecWidth;
		pXtmp->sCharWidth=sCharWidth;
		pXtmp->sFieldType=sFieldType;
		pXtmp->cTypeChar=cTypeChar;
		//after filling cmd_db_tmp make it the current
		if((*pXbeg)==NULL) {
		   (*pXbeg)=pXcur=pXtmp;
			pXcur->next=NULL;
		}else{
			pXcur->next=pXtmp;
			pXcur=pXcur->next;
			pXcur->next=NULL;
		}
	}

	rc=RTNORM;
bail:
	if(templatefile!=NULL) {
		fclose(templatefile);
		templatefile=NULL;
	}
	return(rc);
}

short processattribs(db_handitem *inship,struct cmd_xtemplate *pXbeg, char *outputline,
					 FILE *outputfile,char cQuote,char cDelim,char *outfilter,sds_point curpt,sds_real totalrotang,
					 sds_real totalxscale,sds_real totalyscale,sds_real totalzscale,int level)
{
	short rc=RTNORM;
	db_handitem *hip;
	db_insert *insptr=(db_insert *)inship;
	db_attrib *attptr;
	struct cmd_xtemplate *pXcur=NULL,*pXtmp=NULL;
	char *cptr;
	sds_point pt;
	short foundthisone,foundanyattribtag=0;
	int overflowflag=0;
	char hanbuf[17];

	outputline[0]=0;
	//step thru to find matching attribute, print if match
	for(pXtmp=pXbeg; pXtmp!=NULL; pXtmp=pXtmp->next) {

		//if this is a BL: type, print from the insert info
		if(pXtmp->sFieldType==ATTX_BL) {
			   //start checking for templates to print
			if(strisame("BL:NAME"/*DNT*/,pXtmp->szFieldName)) {
				insptr->get_2(&cptr);
				cmd_prtxstr2buf(cptr,cQuote,cDelim,outfilter,outputline,pXtmp,&overflowflag);
			}else if(strisame("BL:LEVEL"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxint2buf(level,cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:NUMBER"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxint2buf(inscount,cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:X"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(curpt[0],cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:Y"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(curpt[1],cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:Z"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(curpt[2],cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:HANDLE"/*DNT*/,pXtmp->szFieldName)) {
				insptr->RetHandle().ToAscii(hanbuf);
				cmd_prtxstr2buf(hanbuf,cQuote,cDelim,outfilter,outputline,pXtmp,&overflowflag);
			}else if(strisame("BL:LAYER"/*DNT*/,pXtmp->szFieldName)) {
				insptr->get_8(&cptr);
				cmd_prtxstr2buf(cptr,cQuote,cDelim,outfilter,outputline,pXtmp,&overflowflag);
			}else if(strisame("BL:ORIENT"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(totalrotang*180.0/IC_PI,cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:XSCALE"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(totalxscale,cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:YSCALE"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(totalyscale,cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:ZSCALE"/*DNT*/,pXtmp->szFieldName)) {
				cmd_prtxreal2buf(totalzscale,cDelim,outfilter,outputline,pXtmp);
// I bet these extrudes need rework also -- get the normal from the matrix, maybe?	The Z row?
			}else if(strisame("BL:XEXTRUDE"/*DNT*/,pXtmp->szFieldName)) {
				insptr->get_210(pt);
				cmd_prtxreal2buf(pt[0],cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:YEXTRUDE"/*DNT*/,pXtmp->szFieldName)) {
				insptr->get_210(pt);
				cmd_prtxreal2buf(pt[1],cDelim,outfilter,outputline,pXtmp);
			}else if(strisame("BL:ZEXTRUDE"/*DNT*/,pXtmp->szFieldName)) {
				insptr->get_210(pt);
				cmd_prtxreal2buf(pt[2],cDelim,outfilter,outputline,pXtmp);
			}
		}

		//otherwise loop over the attributes and see if one matches this entry
		else if(pXtmp->sFieldType==ATTX_OTHER) {
			hip=insptr->next;

			foundthisone=0;
			// step through the attribs
			while(hip->ret_type()==DB_ATTRIB) {
				attptr=(db_attrib *)hip;
				if(sds_usrbrk()) { rc=(RTCAN);goto bail; }	//check for break

				//get the attrib tag
				attptr->get_2(&cptr);
				if(strisame(cptr,pXtmp->szFieldName)) {
					foundthisone=1;
					attptr->get_1(&cptr);  // get and print the value
					if(pXtmp->cTypeChar=='N'/*DNT*/||pXtmp->cTypeChar=='n'/*DNT*/) {
						cmd_prtxreal2buf(atof(cptr),cDelim,outfilter,outputline,pXtmp);
					}else if(pXtmp->cTypeChar=='C'/*DNT*/||pXtmp->cTypeChar=='c'/*DNT*/) {
						cmd_prtxstr2buf(cptr,cQuote,cDelim,outfilter,outputline,pXtmp,&overflowflag);
					}
					break;	// out of while -- no need to keep looking
				}
				hip=hip->next;
			} // while -- loop over the attribs following the insert

			if (!foundthisone) {	// if not found, search the constant attributes in the block def
				db_handitem *bthip,*firstenthip,*lastenthip;
				db_attdef *attdefptr;

				bthip=insptr->ret_bthip();	 // get the block table pointer
				((db_blocktabrec *)bthip)->get_blockllends(&firstenthip,&lastenthip); // and the entity pointers
				while (firstenthip!=NULL && firstenthip->ret_type()!=DB_ENDBLK) {
					// if it's an attdef
					if (firstenthip->ret_type()==DB_ATTDEF) {
						attdefptr=(db_attdef *)firstenthip;
						// and constant, and matches the field being examined
						if ((((db_attdef *)firstenthip)->ret_70() & IC_ATTDEF_CONST) &&
							strisame(attdefptr->ret_2(),pXtmp->szFieldName)) {
							foundthisone=1;
							if(pXtmp->cTypeChar=='N'/*DNT*/) {
								cmd_prtxreal2buf(atof(attdefptr->ret_1()),cDelim,outfilter,outputline,pXtmp);
							}else if(pXtmp->cTypeChar=='C'/*DNT*/) {
								cmd_prtxstr2buf(attdefptr->ret_1(),cQuote,cDelim,outfilter,outputline,pXtmp,&overflowflag);
							}
							break;	// out of while -- no need to keep looking
						}
					}
					firstenthip=firstenthip->next;
				} // while looking for constant attributes
			} // if (!foundthisone) search the constant attributes

			if (foundthisone) foundanyattribtag=1;
			else {	// if we did not find it, write the default values (' ' or 0.0)
				if (pXtmp->cTypeChar=='N') cmd_prtxreal2buf(0.0,cDelim,outfilter,outputline,pXtmp);
				else if (pXtmp->cTypeChar=='C') cmd_prtxstr2buf(" ",cQuote,cDelim,outfilter,outputline,pXtmp,&overflowflag);
			}
		} // ATTX_OTHER case

	} // loop over the desired attributes from the template file
bail:
// if we found any matching attribute, flush
	if (foundanyattribtag) {
		fprintf(outputfile,"%s\n",outputline);
		recno++;
		if (overflowflag) sds_printf(ResourceString(IDC_CMDS8_FIELD_OVERFLOW_IN_RECORD, "\nField overflow in record %d"),recno);
	}
	return(rc);
}


short processinsert(db_handitem *inship,struct cmd_xtemplate *pXbeg, char *outputline,
					FILE *outputfile,char cQuote, char cDelim, char *outfilter,sds_matrix transmat,sds_real totalrotang,
					sds_real totalxscale, sds_real totalyscale, sds_real totalzscale,short level)
{
// Note: this function is recursive

// inship		pointer to insert to be processed
// pXbeg		start of list of template items to be searched for
// outputline	passed-in buffer to hold the output
// outputfile	file being written
// cQuote		the quote character
// cDelim		the delimiter character
// outfilter	string ("CDF" or "SDF") indicating the type of extraction
// transmat		transformation matrix
// totalrotang	total rotation angle at this level
// total?scale	total scale factors
// level		depth of recursive descent

	short rc=RTNORM;
	short ret;
	db_insert *insptr;
	db_handitem *bthip,*firstenthip,*lastenthip,*origfirstenthip;
	int attribsfollow;
	int blockflags;
	int numrows,numcols,i,j;
	sds_real rowdist,coldist,rotang,xscale,yscale,zscale;
	sds_point inspt,curpt;
	sds_matrix passtransmat;
	sds_point transpt;

	inscount++;
	insptr=(db_insert *)inship;
	insptr->get_66(&attribsfollow);
	bthip=insptr->ret_bthip();	 // get the block table pointer
	bthip->get_70(&blockflags);
	((db_blocktabrec *)bthip)->get_blockllends(&origfirstenthip,&lastenthip); // and the entity pointers

	insptr->get_70(&numcols);
	insptr->get_71(&numrows);
	insptr->get_44(&coldist);
	insptr->get_45(&rowdist);
	insptr->get_10(inspt);
	insptr->get_50(&rotang);
	totalrotang+=rotang;
	insptr->get_41(&xscale);
	insptr->get_42(&yscale);
	insptr->get_43(&zscale);
	if (xscale!=yscale || xscale!=zscale) totalxscale=totalyscale=totalzscale=0.0;
	else {
		totalxscale*=xscale;
		totalyscale*=yscale;
		totalzscale*=zscale;
	}

	if (numcols<1) numcols=1;
	if (numrows<1) numrows=1;

	for (i=0; i<numcols; i++) {
		for (j=0; j<numrows; j++) {
			// first process the attributes following this insert (or constant attributes which
			// live as attdefs in the block def), if any

			if (attribsfollow || (blockflags & IC_BLOCK_HASATTRIBUTES)) {
				curpt[0]=inspt[0]+i*coldist;
				curpt[1]=inspt[1]+j*rowdist;
				curpt[2]=inspt[2];
				atttransformpoint3d(curpt,transmat,transpt);
				ret=processattribs(inship,pXbeg,outputline,outputfile,cQuote,cDelim,outfilter,transpt,totalrotang,
					totalxscale,totalyscale,totalzscale,level);
				if (ret!=RTNORM) {
					rc=RTERROR;
					goto bail;
				}
			}

			// now process any inserts in the definition of this block

			firstenthip=origfirstenthip;
			while (firstenthip!=NULL && firstenthip->ret_type()!=DB_ENDBLK) {
				if (firstenthip->ret_type()==DB_INSERT) {
					memcpy(passtransmat,transmat,sizeof(sds_matrix));
					atttranslatematrix(inspt,passtransmat);
					attrotatematrixz(rotang,passtransmat);
					attscalematrix(xscale,yscale,zscale,passtransmat);
					processinsert(firstenthip,pXbeg,outputline,outputfile,cQuote,cDelim,outfilter,
						passtransmat,totalrotang,totalxscale,totalyscale,totalzscale,level+1);
				}
				if (firstenthip!=NULL) firstenthip=firstenthip->next;
			}
		}
	}
bail:
	return(rc);
}



short dotheextract(sds_name ssname,struct cmd_xtemplate *pXbeg,char *outputline,
				   FILE *outputfile,char cQuote,char cDelim,char *outfilter)
//ssname -- selection set of inserts
//outputline -- new'd area to be used to buffer the output
//outputfile -- the file to which the attributes are being written
//cQuote -- the quote character
//cDelim -- the delimiter (typically comma)
//outfilter -- string indicating the type of extraction ("Cdf" or "Sdf")
{
	short rc=RTNORM;
	int i,j,k;
	sds_name insentname;
	struct resbuf *insresbuf=NULL,*insnamerb;
	db_handitem *inship;
	short ret;
	sds_matrix transmat;

	inscount=0;
	recno=0;

	//step through the inserts.
	for (i=0; sds_ssname(ssname,i,insentname)==RTNORM; i++){
		if(sds_usrbrk()) {rc=RTCAN;goto bail;} //check for break
// seems redundant with the below
		if((insresbuf=sds_entget(insentname))==NULL) {rc=RTERROR;goto bail;}

		//make sure that we have an insert
		ic_assoc(insresbuf,0);
		if((!strisame(ic_rbassoc->resval.rstring,"INSERT"/*DNT*/))) {rc=(RTERROR); goto bail;}

		// set up matrix
		for (j=0; j<4; j++)
			for (k=0; k<4; k++)
				transmat[j][k]=(j==k)?1.0:0.0;

		insnamerb=ic_ret_assoc(insresbuf,-1);
		inship=(db_handitem *)(insnamerb->resval.rlname[0]);
		if ((ret=processinsert(inship,pXbeg,outputline,outputfile,cQuote,cDelim,outfilter,
			transmat,0.0,1.0,1.0,1.0,1))!=RTNORM) {
			rc=RTERROR;
			goto bail;
		}

		IC_RELRB(insresbuf);
		insresbuf=NULL;
	}
bail:
	if (insresbuf!=NULL) IC_RELRB(insresbuf);
	sds_printf("\n%d records extracted",recno);
	return(rc);
}

//=======================================================================================
short cmd_attext(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	sds_name ssname,ent1;
	char tmpstr[IC_ACADBUF],outfilter[IC_ACADBUF],fileloc[IC_ACADBUF],cQuote,cDelim;
	struct cmd_xtemplate *pXcur=NULL,*pXtmp=NULL,*pXbeg=NULL;
	struct resbuf *entmask=NULL,*insresbuf=NULL,tmpfile,setgetrb;
	short endflag=0,entclose=0,pflag=0,rc=0,selmode=0;
	int tmpint=0,ret=0;
	long i=0,sslen;
	FILE *outputfile=NULL;
	dwg_entlink **elp=NULL;
	cQuote='\'';
	cDelim=',';
	char *outputline=NULL;
	int reterror;

	outputline=new char[8192];
	if (outputline==NULL) {
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=RTERROR;
		goto bail;
	}

	sprintf(tmpstr,ResourceString(IDC_CMDS8__NSELECT_ENTITIE_113, "\nSelect Entities (E), or extract attibutes as: DXF/SDF/<CDF>?: " ));
	if((ret=sds_initget(0,ResourceString(IDC_CMDS8_SELECT_ENTITIES__114, "Select_entities|Entities ~ Extract_as_DXF|Dxf Extract_as_CDF|Cdf Extract_as_SDF|Sdf ~_Entities ~_ ~_Dxf ~_Cdf ~_Sdf" )))!=RTNORM) {rc=ret;goto bail;}
	if((ret=sds_getkword(tmpstr,outfilter))==RTERROR) {
		rc=ret;goto bail;
	} else if(ret==RTCAN) {
		rc=ret;goto bail;
	} else if(ret==RTNONE) {
		strcpy(outfilter,"CDF"/*DNT*/);
	}
	//build selection set filter
	if((entmask=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,0))==NULL) {rc=RTERROR;goto bail;}
	if(strisame(outfilter,"ENTITIES"/*DNT*/)) {
		//get the picks
		if(sds_pmtssget(NULL,NULL,NULL,NULL,entmask,ssname,0)!=RTNORM) {rc=RTERROR;goto bail;}
			sprintf(tmpstr,ResourceString(IDC_CMDS8_EXTRACT_ATTRIBUT_117, "Extract attributes as: DXF/SDF/<CDF>: " ));
			if((ret=sds_initget(0,ResourceString(IDC_CMDS8_EXTRACT_AS_DXF_D_118, "Extract_as_DXF|Dxf Extract_as_CDF|Cdf Extract_as_SDF|Sdf ~_Dxf ~_Cdf ~_Sdf" )))!=RTNORM) {rc=ret;goto bail;}
		if((ret=sds_getkword(tmpstr,outfilter))==RTERROR) {
			rc=ret;goto bail;
		} else if(ret==RTCAN) {
			rc=ret;goto bail;
		} else if(ret==RTNONE) {
			strcpy(outfilter,"CDF"/*DNT*/);
		}
	}else{
		//get all selections in the dwg
		if(sds_ssget(ResourceString(IDC_CMDS8_X_119, "X"/*DNT*/ ),NULL,NULL,entmask,ssname)!=RTNORM) {rc=RTERROR;goto bail;}
	}
	//for dxf out
	if(strisame(outfilter,"DXF"/*DNT*/)){
		//get output file
		//check to see if the user wanted a dialog box or a command line prompt
		*fileloc=0;
		if((ret=SDS_getvar(NULL,DB_QFILEDIA,&tmpfile,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
		if( tmpfile.resval.rint &&
			GetActiveCommandQueue()->IsEmpty() &&
			GetMenuQueue()->IsEmpty() &&
			!lsp_cmdhasmore) {
			dia1:
			if(sds_getfiled(ResourceString(IDC_CMDS8_SELECT_OUTPUT_FI_121, "Select Output File" ),ResourceString(IDC_CMDS8___DXX_122, "*.dxx" ),ResourceString(IDC_CMDS8_DXX_123, "dxx" ),1,&tmpfile)!=RTNORM) {rc=ret;goto bail;}
			if(tmpfile.restype==RTSTR) {
				strcpy(fileloc,tmpfile.resval.rstring); IC_FREE(tmpfile.resval.rstring);
			}
			else { goto typeit1; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
		} else {
			typeit1:
			if((ret=sds_getstring(1,ResourceString(IDC_CMDS8__NENTER_OUTPUT_F_124, "\nEnter output file: " ),fileloc))!=RTNORM) {rc=ret;goto bail;}
			if(*fileloc=='~'/*DNT*/) goto dia1;
		}

		ChangeSlashToBackslashAndRemovePipe(fileloc);

		//get length
		sds_sslength(ssname,&sslen);
		//make a link list of entities		
		elp= new dwg_entlink * [sslen] ;
		memset(elp,0,sizeof(dwg_entlink*)*sslen);
		for(i=0L;sds_ssname(ssname,i,ent1)==RTNORM;i++){
			elp[i]=(dwg_entlink*)ent1[0];
		}
		//get current prec
		if((ret=SDS_getvar(NULL,DB_QLUPREC,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
		//dxf out
		if((ret=SDS_getvar(NULL,DB_QMAKEBAK,&tmpfile,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
		ret=SDS_CMainWindow->m_pFileIO.drw_savefile(SDS_CURDWG,sslen,elp,fileloc,IC_DXF,IC_ACAD2004,0,setgetrb.resval.rint,1,1,tmpfile.resval.rint,TRUE);
		rc=RTNORM;
		goto bail;
	}

	//get the template file
	//check to see if the user wanted a dialog box or a command line prompt
	*fileloc=0;
	if((ret=SDS_getvar(NULL,DB_QFILEDIA,&tmpfile,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
	if( tmpfile.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia2:
		if(sds_getfiled(ResourceString(IDC_CMDS8_SELECT_TEMPLATE__125, "Select Template File" ),ResourceString(IDC_CMDS8_TEMPLATE_TXT_126, "TEMPLATE.TXT" ),ResourceString(IDC_CMDS8_TXT_127, "TXT" ),0,&tmpfile)!=RTNORM) {rc=ret;goto bail;}
		if(tmpfile.restype==RTSTR) {
			strcpy(fileloc,tmpfile.resval.rstring); IC_FREE(tmpfile.resval.rstring);
		}
		else { goto typeit2; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit2:
		if((ret=sds_getstring(1,ResourceString(IDC_CMDS8__NTEMPLATE_FILE__128, "\nTemplate file to load: " ),fileloc))!=RTNORM) {rc=ret;goto bail;}
		if(*fileloc=='~') goto dia2;
	}

	ChangeSlashToBackslashAndRemovePipe(fileloc);

	//get the template file and build linked list
	if((ret=cmd_templinklist(fileloc,&cQuote,&cDelim,&pXbeg,&reterror))!=RTNORM) {
		if (reterror!=0) {
// go ahead and use the already-allocated outputline array for this error
			LoadString(IcadSharedGlobals::GetIcadResourceInstance(),reterror,outputline,8192);
			sds_printf("\n%s\n",outputline);
		}
		rc=ret;
		goto bail;
	}

	//go through the linked template list and verify the format
//	  if((ret=cmd_tempcheck())!=RTNORM) {rc=(RTERROR); goto bail;}

	//get output file
	//check to see if the user wanted a dialog box or a command line prompt
	*fileloc=0;
	if((ret=SDS_getvar(NULL,DB_QFILEDIA,&tmpfile,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;}
	if( tmpfile.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia3:
		if(sds_getfiled(ResourceString(IDC_CMDS8_SELECT_OUTPUT_FI_121, "Select Output File" ),ResourceString(IDC_CMDS8___OUT_129, "*.out" ),ResourceString(IDC_CMDS8_OUT_130, "out" ),1,&tmpfile)!=RTNORM) {rc=ret;goto bail;}
		if(tmpfile.restype==RTSTR) {
			strcpy(fileloc,tmpfile.resval.rstring); IC_FREE(tmpfile.resval.rstring);
		}
		else { goto typeit3; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit3:
		if((ret=sds_getstring(1,ResourceString(IDC_CMDS8__NENTER_OUTPUT_F_124, "\nEnter output file: " ),fileloc))!=RTNORM) {rc=ret;goto bail;}
		if(*fileloc=='~') goto dia3;
	}

	ChangeSlashToBackslashAndRemovePipe(fileloc);

	//open output file in read/append mode
	if((outputfile = fopen(fileloc,"w"/*DNT*/)) == NULL ) {
		sds_printf(ResourceString(IDC_CMDS8__NTHE_FILE__S_CO_109, "\nThe file %s could not be opened.\n" ),fileloc);
		rc=RTERROR;
		goto bail;
	}

	rc=dotheextract(ssname,pXbeg,outputline,outputfile,cQuote,cDelim,outfilter);

bail:
	sds_ssfree(ssname);
	while(pXbeg!=NULL) {
		pXcur=pXbeg->next;
		if(pXbeg->szFieldName) IC_FREE(pXbeg->szFieldName);
		delete pXbeg;
		pXbeg=pXcur;
	}
	if (outputline!=NULL) delete [] outputline;
	if (outputfile!=NULL) {
		fclose(outputfile);
		outputfile=NULL;
	}
	IC_RELRB(insresbuf);
	IC_RELRB(entmask);
	return(rc);
}

//read the linked list and check for proper format
/*
short cmd_tempcheck(void) {
	char *errmsgc=ResourceString(IDC_CMDS8__NERROR_READING__152, "\nError reading template: %s %c" );		 // these both get used in
	char *errmsgi=ResourceString(IDC_CMDS8__NERROR_READING__153, "\nError reading template: %s %i" );		// a bunch of sds_printf()s below...

	for(cmd_db_cur=cmd_db_tmp=cmd_db_beg;cmd_db_tmp!=NULL;cmd_db_cur=cmd_db_tmp) {
		cmd_db_tmp=cmd_db_tmp->next;
		if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_NAME_132, "BL:NAME" ))) {
			if(cmd_db_cur->typ!='C') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp!=0) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_X_133, "BL:X" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp<0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_Y_134, "BL:Y" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_Z_135, "BL:Z" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_NUMBER_154, "BL:NUMBER" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp!=0) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_HANDLE_136, "BL:HANDLE" ))) {
			if(cmd_db_cur->typ!='C') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp!=0) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_LAYER_137, "BL:LAYER" ))) {
			if(cmd_db_cur->typ!='C') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp!=0) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_ORIENT_138, "BL:ORIENT" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_XSCALE_139, "BL:XSCALE" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_YSCALE_140, "BL:YSCALE" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_ZSCALE_141, "BL:ZSCALE" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_XEXTRUDE_142, "BL:XEXTRUDE" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_YEXTRUDE_143, "BL:YEXTRUDE" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strisame(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL_ZEXTRUDE_144, "BL:ZEXTRUDE" ))) {
			if(cmd_db_cur->typ!='N') {sds_printf(errmsgc,cmd_db_cur->fname,cmd_db_cur->typ); return(RTERROR);}
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strnicmp(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL__110, "BL:" ),3) && (cmd_db_cur->typ=='C')) {
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp!=0) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}else if(strnicmp(cmd_db_cur->fname,ResourceString(IDC_CMDS8_BL__110, "BL:" ),3) && (cmd_db_cur->typ=='N')) {
			if(cmd_db_cur->flen < 0 && cmd_db_cur->flen > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->flen); return(RTERROR);}
			if(cmd_db_cur->dp < 0 && cmd_db_cur->dp > 999) {sds_printf(errmsgi,cmd_db_cur->fname,cmd_db_cur->dp); return(RTERROR);}
		}
	}
	return(RTNORM);

}
*/
//=======================================================================================



