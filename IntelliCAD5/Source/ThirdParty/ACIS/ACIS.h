//*********************************************************************
//*  ACIS.h                                                           *
//*  Copyright (C) 1994,1995,1996,1997,1998 by Visio Corporation.     *
//*                                                                   *
//*********************************************************************

#pragma once
#pragma message( "Including " __FILE__ )

//** Includes
#include "sds.h"
#include "gra.h"

#if defined(SDS_NEWDB)
int ACIS_CreateAcisObj(void *SatBack,int Region);	
int ACIS_CreateBlock(sds_point pt1,sds_point pt2, sds_real high,db_drawing *flp);	
int ACIS_CreateSphere(sds_point cent,double rad,db_drawing *flp);
int ACIS_CreateTorus(sds_point Cent,double Tordia, double Tubdia,db_drawing *flp);
int ACIS_CreateWedge(sds_point pt1,sds_point pt2,sds_real high,db_drawing *flp);
int ACIS_CreateDish(sds_point pt0,sds_real rad,db_drawing *flp);	
int ACIS_CreateDome(sds_point pt0,sds_real rad,db_drawing *flp);	
int ACIS_CreatePyramid(sds_point bpt1,sds_point bpt2,sds_point bpt3,sds_point bpt4,sds_point tpt1,sds_point tpt2,sds_point tpt3,sds_point tpt4,db_drawing *flp);
int ACIS_CreateCone(sds_point pt0,sds_real bdia,sds_real tdia,sds_real high,db_drawing *flp);
int ACIS_CreateMesh(sds_point pt1,sds_point pt2,sds_point pt3,sds_point pt4,int msize,int nsize,db_drawing *flp);
int ACIS_GetSAT(db_handitem *elp,char **SatBack);
int ACIS_XformObject(db_handitem *elp, sds_matrix genmat);
int ACIS_BoolObjects(db_handitem *Blank, db_handitem *Tool,char Mode);
int ACIS_CopyObjects(db_handitem *Source, db_handitem *Dest);
int ACIS_Init();
int ACIS_Exit();
int ACIS_getdispobjs(db_handitem **p_elpp,struct gra_displayobject **p_begdopp,struct gra_displayobject **p_enddopp,db_drawing *p_flp,struct gra_view *p_viewp,short p_mode);
#else
/*D*/int ACIS_CreateAcisObj(void *SatBack,int Region);	
/*D*/int ACIS_CreateBlock(sds_point pt1,sds_point pt2, sds_real high,struct dwg_filelink *flp);	
/*D*/int ACIS_CreateSphere(sds_point cent,double rad,struct dwg_filelink *flp);
/*D*/int ACIS_CreateTorus(sds_point Cent,double Tordia, double Tubdia,struct dwg_filelink *flp);
/*D*/int ACIS_CreateWedge(sds_point pt1,sds_point pt2,sds_real high,struct dwg_filelink *flp);
/*D*/int ACIS_CreateDish(sds_point pt0,sds_real rad,struct dwg_filelink *flp);	
/*D*/int ACIS_CreateDome(sds_point pt0,sds_real rad,struct dwg_filelink *flp);	
/*D*/int ACIS_CreatePyramid(sds_point bpt1,sds_point bpt2,sds_point bpt3,sds_point bpt4,sds_point tpt1,sds_point tpt2,sds_point tpt3,sds_point tpt4,struct dwg_filelink *flp);
/*D*/int ACIS_CreateCone(sds_point pt0,sds_real bdia,sds_real tdia,sds_real high,struct dwg_filelink *flp);
/*D*/int ACIS_CreateMesh(sds_point pt1,sds_point pt2,sds_point pt3,sds_point pt4,int msize,int nsize,struct dwg_filelink *flp);
/*D*/int ACIS_GetSAT(struct dwg_entlink *elp,char **SatBack);
/*D*/int ACIS_XformObject(struct dwg_entlink *elp, sds_matrix genmat);
/*D*/int ACIS_BoolObjects(struct dwg_entlink *Blank, struct dwg_entlink *Tool,char Mode);
/*D*/int ACIS_CopyObjects(struct dwg_entlink *Source, struct dwg_entlink *Dest);
/*D*/int ACIS_Init();
/*D*/int ACIS_Exit();
/*D*/int ACIS_getdispobjs(struct dwg_entlink **p_elpp,struct gra_displayobject **p_begdopp,struct gra_displayobject **p_enddopp,struct dwg_filelink *p_flp,struct gra_view *p_viewp,short p_mode);
#endif

