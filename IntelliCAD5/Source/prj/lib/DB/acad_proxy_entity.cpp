/* C:\ICAD\PRJ\LIB\DB\ACAD_PROXY_ENTITY.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "layertabrec.h"

#include "OdaCommon.h"
#include "..\..\..\Licensed\DWGdirect\Include\ResBuf.h" // clash with icadlib\resbuf.h
#include "DbProxyEntity.h"

#if defined( GRDLL )
	#define	GR_CLASS __declspec( dllexport)
	#define	GR_DECLSPEC __declspec( dllexport)
#else
	#if defined( GRSTATIC )
		#define	GR_CLASS
		#define	GR_DECLSPEC
	#else
		#define	GR_CLASS __declspec( dllimport)
		#define	GR_DECLSPEC __declspec( dllimport)
	#endif
#endif

#ifndef	GR_API
	#define	GR_API extern "C" GR_CLASS
#endif
#include "..\gr\DoDef.h"

/***************************** db_acad_proxy_entity *****************************/

db_acad_proxy_entity::db_acad_proxy_entity(void) : db_entity(DB_ACAD_PROXY_ENTITY) {
    pid=aid=databits=datasz=grdatasz=entid=0; data=grdata=NULL;
    hirefll[0]=hirefll[1]=NULL;

	memset(m_extent,0,sizeof(m_extent));
	m_boundingBox = NULL;

	disp = NULL;
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}
db_acad_proxy_entity::db_acad_proxy_entity(db_drawing *dp) :
    db_entity(DB_ACAD_PROXY_ENTITY,dp) {

    pid=aid=databits=datasz=grdatasz=entid=0; data=grdata=NULL;
    hirefll[0]=hirefll[1]=NULL;

	memset(m_extent,0,sizeof(m_extent));
	m_boundingBox = NULL;

	disp = NULL;
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}
db_acad_proxy_entity::db_acad_proxy_entity(const db_acad_proxy_entity &sour) :
    db_entity(sour) {  /* Copy constructor */

    db_hireflink *tp1,*tp2;

    pid=sour.pid; aid=sour.aid; databits=sour.databits; entid=sour.entid;

    datasz=sour.datasz;
    data=NULL;
    if (datasz>0 && sour.data!=NULL) {
        data=new char[datasz];
        memcpy(data,sour.data,datasz);
    }

    grdatasz=sour.grdatasz;
    grdata=NULL;
    if (grdatasz>0 && sour.grdata!=NULL) {
        grdata=new char[grdatasz];
        memcpy(grdata,sour.grdata,grdatasz);
    }

    hirefll[0]=hirefll[1]=NULL;
    for (tp1=sour.hirefll[0]; tp1!=NULL; tp1=tp1->next)
        { tp2=new db_hireflink(*tp1); addhiref(tp2); }

	memset(m_extent,0,sizeof(m_extent));
	m_boundingBox = NULL;

	disp = NULL;
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}

db_acad_proxy_entity::~db_acad_proxy_entity(void) {
//4M Item:150->
/*
    delete [] data; delete [] grdata; db_hireflink::delll(hirefll[0]);
*/
	if (db_grafreedofp!=NULL) 
	{
		db_grafreedofp(disp);
		db_grafreedofp(m_boundingBox);
	}

	sds_relrb(m_pResbufChain);
	OdDbProxyEntityPtr pClass((OdDbProxyEntity*)m_OdClass, kOdRxObjAttach);

	m_pResbufChain = NULL;
	m_OdClass = NULL;

	if (data){
	  delete [] data;
	  data=NULL;
	}

	if (grdata){
	  delete [] grdata;
	  grdata=NULL;
	}

    if (hirefll[0])
      db_hireflink::delll(hirefll[0]);
//<-4M Item:150
}

// This will make a copy of source, so source can be deleted when
// it goes out of scope in OdGiEntityGeometry::~OdGiEntityGeometry()
void db_acad_proxy_entity::set_displayVectors(void* pSource)
{

	struct gr_displayobject *pSourceObject = (struct gr_displayobject *)pSource;

	gr_displayobject	*pDO = NULL;
	int					ndims, iSize;

	for( ; pSourceObject; pSourceObject = pSourceObject->next)
	{
		if(pDO)
		{
#ifdef USE_GREEDYHEAP_FOR_DO
			pDO->next = s_doHeap.malloc();
#else
			pDO->next = new gr_displayobject;
#endif
			pDO = pDO->next;
		}
		else
#ifdef USE_GREEDYHEAP_FOR_DO
			disp = pDO = s_doHeap.malloc();
#else
			disp = pDO = new gr_displayobject;
#endif
		ASSERT(pDO);
		if(!pDO)
			break;
		else
		{
			pDO->type = pSourceObject->type;
			pDO->color = pSourceObject->color;
			pDO->lweight = pSourceObject->lweight;
			pDO->npts = pSourceObject->npts;
			pDO->next = NULL;
			pDO->SetSourceEntity(pSourceObject->GetSourceEntity());

			ndims = ((pSourceObject->type & DISP_OBJ_PTS_3D) != 0) + 2,
			iSize = pSourceObject->npts * ndims;
			
			pDO->chain = new sds_real[iSize];
			memcpy(pDO->chain, pSourceObject->chain, sizeof(sds_real) * iSize);
		}
	}

	return;
}

void* db_acad_proxy_entity::ret_boundingBox(db_drawing* pDb)
{
	if (m_boundingBox != NULL)
		return m_boundingBox;

	m_boundingBox = new gr_displayobject();

	int color;
	get_62(&color);
	if (color == 256)
	{
		color = ((db_layertabrec*)ret_layerhip())->ret_color();
	}

	((struct gr_displayobject*)m_boundingBox)->type = 3;	// 3D and closed.
	((struct gr_displayobject*)m_boundingBox)->next = NULL;
	((struct gr_displayobject*)m_boundingBox)->color = color;
	((struct gr_displayobject*)m_boundingBox)->lweight = 0;
	((struct gr_displayobject*)m_boundingBox)->npts = 4;
	((struct gr_displayobject*)m_boundingBox)->SetSourceEntity((db_entity*)this);
	((struct gr_displayobject*)m_boundingBox)->chain = new double[4 * 3];

	// lower left
	((struct gr_displayobject*)m_boundingBox)->chain[0] = m_extent[0][0];
	((struct gr_displayobject*)m_boundingBox)->chain[1] = m_extent[0][1];
	((struct gr_displayobject*)m_boundingBox)->chain[2] = m_extent[0][2];
	// lower right
	((struct gr_displayobject*)m_boundingBox)->chain[3] = m_extent[1][0];
	((struct gr_displayobject*)m_boundingBox)->chain[4] = m_extent[0][1];
	((struct gr_displayobject*)m_boundingBox)->chain[5] = m_extent[0][2];
	// upper right
	((struct gr_displayobject*)m_boundingBox)->chain[6] = m_extent[1][0];
	((struct gr_displayobject*)m_boundingBox)->chain[7] = m_extent[1][1];
	((struct gr_displayobject*)m_boundingBox)->chain[8] = m_extent[1][2];
	// upper left
	((struct gr_displayobject*)m_boundingBox)->chain[9] = m_extent[0][0];
	((struct gr_displayobject*)m_boundingBox)->chain[10] = m_extent[1][1];
	((struct gr_displayobject*)m_boundingBox)->chain[11] = m_extent[0][2];

/*

	// Need to figure out a way to vectorize the desc string to 
	// add it to the rectangle that gets displayed when PROXYSHOW == 2
	db_handitem* activeMSpaceViewPort = pDb->tblsearch(DB_VPORTTAB, "*ACTIVE",0);
    ASSERT(activeMSpaceViewPort != NULL);


	struct gr_view *view = SDS_CMainWindow->m_pCurView;

	CDbMText* tmpText = new CDbMText(pDb);
	tmpText->set_1(desc);

	char* styleName;
	get_7(&styleName);
	tmpText->set_7(styleName, pDb);
	tmpText->set_10(extent[0]);
	tmpText->set_40(0.125);
	tmpText->set_50(0.0, pDb);
	tmpText->set_71(7);

	((struct gr_displayobject*)boundingBox)->next = (struct gr_displayobject *)tmpText->ret_disp();

	db_charbuflink	CurConfig;
	if (db_makesvbuf(1,&CurConfig,NULL)<1)	return boundingBox;

	struct gr_view currGrView;
	short retVal = gr_initview(m_pdbDrawing, &CurConfig, &currGrView, NULL);
*/
	return m_boundingBox;
}