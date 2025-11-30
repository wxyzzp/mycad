/* D:\DEV\PRJ\ICAD\ICADMTEXTDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadMTextDia.cpp                                                 *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "MTextPropSheet.h"
#include "Preferences.h"
//***This is the main window
// EBATECH(watanabe)-[get mtext info only
//CMainWindow::IcadMTextDia(struct SDS_mTextData  *m_pMTextDataptr)
void CMainWindow::IcadMTextDia(
							   struct SDS_mTextData  *m_pMTextDataptr,
							   const  bool            bEntMake // = true [I] flag for creating mtext entity
							   )
							   //]-EBATECH(watanabe)
{
    int ret;
	CMTextPropSheet mtextDlg( IDS_MTEXTPROP, m_pMTextDataptr);
	ret = mtextDlg.DoModal();
    if(ret == IDOK)
	{
		// Bugzilla No. 78394; 09-12-2002 [
		CString text = "";   
		text =  mtextDlg.m_propertiesTab.m_width;
		
		if (text == "(no wrap)")
			mtextDlg.m_pMTextData->rBoxWidth = 0.0;
		else
			mtextDlg.m_pMTextData->rBoxWidth = atof(text);

		mtextDlg.m_text.Replace("\r","\\P");
		mtextDlg.m_text.Replace("\n","");
		
		m_pMTextDataptr->rTextHeight = mtextDlg.m_propertiesTab.m_cstyle.m_tableInfo.GetHeight();
		m_pMTextDataptr->rRotAngle = atof(mtextDlg.m_propertiesTab.m_rotation);
		m_pMTextDataptr->nAlignment = mtextDlg.m_propertiesTab.m_justification + 1;
		//Bugzilla No. 78396; 23-12-2002
		//strncpy(m_pMTextDataptr->szTextStyle, mtextDlg.m_propertiesTab.m_style.GetBuffer(0), 512);
		strncpy(m_pMTextDataptr->szTextStyle,mtextDlg.m_propertiesTab.m_cstyle.m_tableInfo.GetStyleName(), 512);
		m_pMTextDataptr->text = mtextDlg.m_text;
		// Bugzilla No. 78394; 09-12-2002 ]

		/*D.G.*/// the previous 4 lines were under the following condition too...
		//if(mtextDlg.m_classname == "CDialog") //insert info in dialog box; don't make new mtext
		//	return;
		if(!mtextDlg.needCreation())
			return;
		
		sds_point pt1, pt210;
		//take the lesser of the X's
		if(m_pMTextDataptr->ptInsertion[0]>m_pMTextDataptr->ptOppCorner[0])
			pt1[0]=m_pMTextDataptr->ptOppCorner[0];
		else
			pt1[0]=m_pMTextDataptr->ptInsertion[0];
		//take the greater of the Y's
		if(m_pMTextDataptr->ptInsertion[1]>m_pMTextDataptr->ptOppCorner[1])
			pt1[1]=m_pMTextDataptr->ptInsertion[1];
		else
			pt1[1]=m_pMTextDataptr->ptOppCorner[1];
		//take any Z
		pt1[2]=m_pMTextDataptr->ptOppCorner[2];
		struct resbuf rbf, rbt;
		int	ret;
		
		sds_angtof( mtextDlg.m_propertiesTab.m_rotation,-1,&(m_pMTextDataptr->rRotAngle));
		//		sds_distof( m_CMText.csTextHigh,-1,&(m_pMTextDataptr->rTextHeight));
		m_pMTextDataptr->nAlignment=mtextDlg.m_propertiesTab.m_justification + 1;//m_CMText.iAlign+1;
		//	m_pMTextDataptr->nDirection=1;//m_CMText.iDirection+1; EBATECH(CNBR) New MTEXT Dialog cannot change Direction.
		//		strcpy(m_pMTextDataptr->szTextStyle,mtextDlg.m_propertiesTab.m_style);
		
		SDS_getvar(NULL,DB_QUCSXDIR,&rbf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		SDS_getvar(NULL,DB_QUCSYDIR,&rbt,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		//cross the two to get extrusion direction for new ent
		ic_crossproduct(rbf.resval.rpoint,rbt.resval.rpoint,pt210);
		
		rbf.restype=RTSHORT;
		rbf.resval.rint=1;
		rbt.restype=RTSHORT;
		rbt.resval.rint=0;
		//adjust alignment point horizontally
		switch (m_pMTextDataptr->nAlignment){
			//center
		case 2:
		case 5:
		case 8:
			pt1[0]+=0.5*m_pMTextDataptr->rBoxWidth;
			break;
			//right
		case 3:
		case 6:
		case 9:
			pt1[0]+=m_pMTextDataptr->rBoxWidth;
			break;
		default:
			break;
		}
		//adjust alignment point vertically
		switch (m_pMTextDataptr->nAlignment){
			//middle
		case 4:
		case 5:
		case 6:
			pt1[1]-=0.5*fabs(m_pMTextDataptr->ptInsertion[1]-m_pMTextDataptr->ptOppCorner[1]);
			break;
			//bottom
		case 7:
		case 8:
		case 9:
			pt1[1]-=fabs(m_pMTextDataptr->ptInsertion[1]-m_pMTextDataptr->ptOppCorner[1]);
			break;
		default:
			break;
		}
		sds_trans(pt1,&rbf,&rbt,0,pt1);
		sds_entmake(NULL);
		// EBATECH(watanabe)-[get mtext info only
        // only get mtext info
        if (bEntMake == false)
            return;
		//]-EBATECH(watanabe)
		
		struct resbuf *rbb;
		rbb=sds_buildlist(RTDXF0,"MTEXT",
			1,m_pMTextDataptr->text.GetBuffer( m_pMTextDataptr->text.GetLength()),
			10,pt1,
			40,m_pMTextDataptr->rTextHeight,
			41,m_pMTextDataptr->rBoxWidth,
			71,m_pMTextDataptr->nAlignment,
			72,m_pMTextDataptr->nDirection,
			7,m_pMTextDataptr->szTextStyle,
			210,pt210,
			50,m_pMTextDataptr->rRotAngle
			,0);
		if(rbb)
		{
			ret=sds_entmake(rbb);
			ASSERT( ret EQ RTNORM );
		}
		
		IC_RELRB(rbb);

		text.Format( "%d %d %d %d",mtextDlg.m_mtextWinodwRect.left, mtextDlg.m_mtextWinodwRect.top, mtextDlg.m_mtextWinodwRect.right, mtextDlg.m_mtextWinodwRect.bottom);
		UserPreference::s_MtextPosition = text;
	}
}


