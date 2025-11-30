// File  :
// Author: Alexey Malov
#pragma warning (disable:4786)
#include "CmdCommand.h"
#include <afx.h>	/*DG - 1.3.2002*/// hack: include MFC before inclusion of windows.h in subsequential includes from CmdQueryTools.h
#include "CmdQueryTools.h"
#include "CmdSolidEdit.h" //CmdQueryDistanceOrPath
#include "Geodata.h" //geodata::extract, curves
using geodata::curve;
using geodata::extract;
#include "DbAcisEntity.h"
#include <string>
#include <vector>
using std::string;

class CCmdQueryPanarEntity: public CCmdQuerySelectionSet
{
public:
	CCmdQueryPanarEntity();
};

CCmdQueryPanarEntity::CCmdQueryPanarEntity()
{
    m_sTypes = "ELLIPSE,CIRCLE,POLYLINE,LWPOLYLINE,SPLINE,REGION"/*DNT*/;
    m_filter.resval.rstring = const_cast<char*>(m_sTypes.c_str());
}

class CCmdExtrude: public CCmdCommand
{
	CCmdQueryPanarEntity m_getPlanarEntities;
	CCmdQueryDistanceOrPath m_getDistanceOrPath;

public:
	CCmdExtrude();

	virtual int run();
};

CCmdExtrude::CCmdExtrude()
{
	m_pStart = &m_getPlanarEntities;
	m_getPlanarEntities.set(0, &m_getDistanceOrPath);
}

int CCmdExtrude::run()
{
	if (CCmdCommand::run())
	{
		const geodata::curve* path = m_getDistanceOrPath.getPath();
		double distance = m_getDistanceOrPath.getDistance();
		double taper = m_getDistanceOrPath.getTaperAngle();

		sds_name planarEntities;
		m_getPlanarEntities.getEntities(planarEntities);
		sds_name ename;
		int unqualifiedCurveCount = 0, operationFailureCount = 0;
		int color = getCurrColorIndex();
		for (long i = 0l; RTNORM == sds_ssname(planarEntities, i, ename); ++i)
		{
			CDbAcisEntityData region;
			db_handitem* item = reinterpret_cast<db_handitem*>(ename[0]);
			if (item->ret_type() == DB_REGION) 
				region = reinterpret_cast<CDbAcisEntity*>(item)->getData();
			else {
				// convert to region
				//
				curve* crv = NULL;
				if (!extract(*item, crv)) {
					sds_redraw(ename, IC_REDRAW_UNHILITE);
					unqualifiedCurveCount++;
					if (crv) 
						delete crv;
					continue;
				}
				if (crv->isClosed()) {
					int oc = CModeler::get()->region(crv, color, region);
					delete crv;
					if (oc <= 0) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						unqualifiedCurveCount++;
						continue;
					}
				}
				else {
					sds_redraw(ename, IC_REDRAW_UNHILITE);
					unqualifiedCurveCount++;
					delete crv;
					continue;
				}
			}

			int rc;
			CDbAcisEntityData solid;
			if (path != 0)
				rc = CModeler::get()->extrude(region, solid, *path, color);
			else
				rc = CModeler::get()->extrude(region, solid, distance, taper, color);

			if (!rc) {
				sds_redraw(ename, IC_REDRAW_UNHILITE);
				operationFailureCount++;
				continue;
			}
			sds_entdel(ename);

			sds_name esolid;
			create(solid, esolid);
		}
		if (unqualifiedCurveCount > 0) 
			sds_printf(ResourceString(CMD_ERR_CURVE_REJECTED, "\n%d selected curves are rejected"), unqualifiedCurveCount);
		
		if (operationFailureCount > 0)
			sds_printf(ResourceString(CMD_ERR_UNABLE_TO_EXTRUDE_CRV, "\nUnable to extrude %d curve(s)"), operationFailureCount);
	}
	return m_rc;
}

short cmd_extrude()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	CCmdExtrude cmd;
	return cmd.run();
}
#pragma warning (default:4786)
