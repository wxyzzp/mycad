#include "cmds.h"
#include "cmdsAlign.h"
#include "IcadApi.h"
#include "Gr.h"

#define _SUPPORT_ALIGN_

int cmd_getWCStoUCS(CMatrix4& toUCS)
{
	sds_resbuf d;
	CD3 ox, oy, oz, oo;
	SDS_getvar(NULL, DB_QUCSORG, &d, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	oo = d.resval.rpoint;
	SDS_getvar(NULL, DB_QUCSXDIR, &d, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	ox = d.resval.rpoint;
	SDS_getvar(NULL, DB_QUCSYDIR, &d, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	oy = d.resval.rpoint;
	oz = ox % oy;
	toUCS.setValue((sds_real*)ox, (sds_real*)oy, (sds_real*)oz, (sds_real*)oo);
	return 1;
}

int cmd_redrawLine(sds_name line, CD3& linePoint1, CD3& linePoint2)
{
	CMatrix4 toUCS;
	CD3 lp1, lp2;
	cmd_getWCStoUCS(toUCS);
	toUCS.multiplyRight((sds_real*)lp1, (sds_real*)linePoint1);
	toUCS.multiplyRight((sds_real*)lp2, (sds_real*)linePoint2);
	((db_line*)line[0])->set_10((sds_real*)lp1);
	((db_line*)line[0])->set_11((sds_real*)lp2);
	SDS_UpdateEntDisp(line, 0, NULL);
	SDS_RedrawEntity(line, IC_REDRAW_DRAW, NULL, NULL, 1);
	return 1;
}

short cmd_align(void)
{
#ifndef _SUPPORT_ALIGN_
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0); 
	SDS_FreeEventList(0);  
	return(RTNORM); 
#endif
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	sds_name entities;
	int result = sds_pmtssget("", NULL, NULL, NULL, NULL, entities, 1);
	if(result != RTNORM)
		return RTNORM;

	CMatrix4 m, r;
	CD3 sPoints[3];
	CD3 dPoints[3];
	int i, j;
	CD3 dir, rotatedDir;
	sds_matrix transformation;
	memset(transformation, 0, sizeof(sds_matrix));
	transformation[0][0] = transformation[1][1] = transformation[2][2] = 1.0;
	db_line* pLine1 = new db_line();
	db_line* pLine2 = new db_line();
	sds_name line1, line2;
	line1[0] = (long)pLine1;
	line2[0] = (long)pLine2;
	line1[1] = line2[1] = (long)SDS_CURDWG;

	while(true)
	{
		result = sds_getpoint(NULL, ResourceString(IDC_ALIGN_SOURCE1, "\nSpecify first source point: "), (sds_real*)sPoints[0]);
		if(result != RTNORM)
			break;

		result = SDS_dragobject(18, 1, (sds_real*)sPoints[0], NULL, 0.0, 
			ResourceString(IDC_ALIGN_DESTINATION1, "\nSpecify first destination point: "), NULL, (sds_real*)dPoints[0], NULL);
		//result = sds_getpoint(NULL, ResourceString(IDC_ALIGN_DESTINATION1, "\nSpecify first destination point: "), (sds_real*)dPoints[0]);
		m.makeIdentity();
		m(0, 3) = dPoints[0][0] - sPoints[0][0];
		m(1, 3) = dPoints[0][1] - sPoints[0][1];
		m(2, 3) = dPoints[0][2] - sPoints[0][2];
		if(result != RTNORM)
			break;
		cmd_redrawLine(line1, sPoints[0], dPoints[0]);
		
		do
		{
			result = sds_getpoint(NULL, ResourceString(IDC_ALIGN_SOURCE2, "\nSpecify second source point: "), (sds_real*)sPoints[1]);
			if(result != RTNORM)
				break;
		}
		while((sPoints[0] - sPoints[1]).isNullWithin());
		if(result != RTNORM)
			break;
		cmd_redrawLine(line1, sPoints[0], dPoints[0]);

		do
		{
			result = SDS_dragobject(18, 1, (sds_real*)sPoints[1], NULL, 0.0, 
				ResourceString(IDC_ALIGN_DESTINATION2, "\nSpecify second destination point: "), NULL, (sds_real*)dPoints[1], NULL);
			if(result != RTNORM)
				break;
		}
		while((dPoints[0] - dPoints[1]).isNullWithin());
		//result = sds_getpoint(NULL, ResourceString(IDC_ALIGN_DESTINATION2, "\nSpecify second destination point: "), (sds_real*)dPoints[1]);
		if(result != RTNORM)
			break;
		cmd_redrawLine(line1, sPoints[0], dPoints[0]);
		cmd_redrawLine(line2, sPoints[1], dPoints[1]);

		r.setValue(sPoints[1] - sPoints[0], dPoints[1] - dPoints[0]);
		m(0, 3) -= dPoints[0][0];
		m(1, 3) -= dPoints[0][1];
		m(2, 3) -= dPoints[0][2];
		r(0, 3) = dPoints[0][0];
		r(1, 3) = dPoints[0][1];
		r(2, 3) = dPoints[0][2];
		
		do
		{
			result = sds_getpoint(NULL, ResourceString(IDC_ALIGN_SOURCE3, "\nSpecify third source point: "), (sds_real*)sPoints[2]);
			if(result != RTNORM)
				break;
		}
		while((sPoints[0] - sPoints[2]).isNullWithin() || (sPoints[1] - sPoints[2]).isNullWithin());
		if(result == RTCAN || result == RTERROR)
			break;
		cmd_redrawLine(line1, sPoints[0], dPoints[0]);
		cmd_redrawLine(line2, sPoints[1], dPoints[1]);

		if(result == RTNORM)
		{
			m = r * m;
			do
			{
				result = SDS_dragobject(18, 1, (sds_real*)sPoints[2], NULL, 0.0, 
					ResourceString(IDC_ALIGN_DESTINATION3, "\nSpecify third destination point: "), NULL, (sds_real*)dPoints[2], NULL);
				if(result != RTNORM)
					break;
			}
			while((dPoints[0] - dPoints[2]).isNullWithin() || (dPoints[1] - dPoints[2]).isNullWithin());
			//result = sds_getpoint(NULL, ResourceString(IDC_ALIGN_DESTINATION3, "\nSpecify third destination point: "), (sds_real*)dPoints[2]);
			if(result != RTNORM)
				break;
			
			CD3 axis(dPoints[1] - dPoints[0]);
			axis.normalize();
			dir = sPoints[2] - sPoints[1];
			m.multiplyRightUsing33((sds_real*)rotatedDir, (sds_real*)dir);
			rotatedDir = rotatedDir - axis * (rotatedDir * axis);
			dir = dPoints[2] - dPoints[1];
			dir = dir - axis * (dir * axis);

			r.setValue(rotatedDir, dir);
			m(0, 3) -= dPoints[0][0];
			m(1, 3) -= dPoints[0][1];
			m(2, 3) -= dPoints[0][2];
			r(0, 3) = dPoints[0][0];
			r(1, 3) = dPoints[0][1];
			r(2, 3) = dPoints[0][2];
			m = r * m;
			break;
		}
		else
		{
			if(sds_initget(0, ResourceString(IDC_ALIGN_OPTIONYESNO, "Yes No")) != RTNORM)
				return RTERROR;
			
			char answer[IC_ACADBUF];
			result = sds_getkword(ResourceString(IDC_ALIGN_SCALE, "\nScale objects based on alignment points [Yes/No] <No>: "), answer);
			if(result != RTNORM)
			{
				if(result == RTNONE)
					strcpy(answer, ResourceString(IDC_ALIGN_OPTIONNO, "No"));
				else
					break;
			}
			
			if(stricmp(answer, ResourceString(IDC_ALIGN_OPTIONYES, "Yes")) == 0)
			{
				sds_real k = (dPoints[1] - dPoints[0]).norm() / (sPoints[1] - sPoints[0]).norm();
				for(i = 0; i < 3; ++i)
					for(j = 0; j < 4; ++j)
						m(i, j) *= k;
			}
			m = r * m;
			break;
		}
	} // while
	if(result == RTNORM || result == RTNONE)
	{
		for(i = 0; i < 3; ++i)
			for(j = 0; j < 4; ++j)
				transformation[i][j] = m(i, j);
		sds_xformss(entities, transformation);
	}
	delete pLine1;
	delete pLine2;
	sds_ssfree(entities);
	sds_redraw(NULL, IC_REDRAW_DRAW);
	return RTNORM; 
}  
