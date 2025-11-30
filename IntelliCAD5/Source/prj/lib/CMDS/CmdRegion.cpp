// File  :
// Author: Alexey Malov
#include <afx.h>	/*DG - 1.3.2002*/// hack: include MFC before inclusion of windows.h in subsequential includes from CmdQueryTools.h
#include "CmdQueryTools.h"
#include "Geodata.h" //geodata::extract, curves
using geodata::curve;
using geodata::extract;
#include <vector>
#include <memory>
#include <string>
#include <functional>
using namespace std;


class CCmdRegion: public CCmdCommand
{
    CCmdQueryCurves m_queryCurves;

public:
    CCmdRegion();

    virtual int run();
};

CCmdRegion::CCmdRegion()
:
CCmdCommand()
{
    m_pStart = &m_queryCurves;
    m_queryCurves.init(ResourceString(IDC_CMDS3_XCLIP__1, "\nSelect objects: "));
}

int CCmdRegion::run()
{
    if (CCmdCommand::run())
    {
        vector<curve*> curves;
        if (!m_queryCurves.getCurves(curves))
            return RTERROR;
		ptr_vector_holder<curve> hcurves(curves);

        sds_name ename, ncurves;
		xstd<CDbAcisEntityData>::vector regions;
		std::list<geodata::curve*> qualifiedCurves;
        int result = CModeler::get()->region(curves, regions, getCurrColorIndex(), qualifiedCurves);
		if (result > 0) {
			// delete the curves used to create the regions and unhighlight other ones
			//
            m_queryCurves.getEntities(ncurves);
			std::list<geodata::curve*>::iterator theIterator;
			for (long i = 0l; sds_ssname(ncurves,i,ename) == RTNORM; ++i) {
				for (theIterator = qualifiedCurves.begin(); theIterator != qualifiedCurves.end(); ++theIterator) {
					if (ename[0] == (long)(*theIterator)->m_pDbEntity)
						sds_entdel(ename);
					else 
						sds_redraw(ename, IC_REDRAW_UNHILITE);
				}
			}
			int num = curves.size() - qualifiedCurves.size();
			if (num > 0) 
				sds_printf(ResourceString(CMD_REGION_UNUSED_CRV_COUNT, "\n%d open curves are not used to create region(s)"), num);

			// create regions
			for (i = 0; i < regions.size(); i++)
				create(regions[i], ename);

			sds_printf(ResourceString(CMD_REGION_CREATED_COUNT, "\n%d regions have been created"), regions.size());

            return RTNORM;
        }
        else {
            m_queryCurves.getEntities(ncurves);
	        for (long i = 0l; sds_ssname(ncurves, i, ename) == RTNORM; ++i)
				sds_redraw(ename, IC_REDRAW_UNHILITE);

				cmd_ErrorPrompt(CMD_ERR_CREATE_REGIONS,0);

            return RTERROR;
        }
    }
    return m_rc;
}

extern "C"
{
extern short cmd_region(void)
{
	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    CCmdRegion cmd;
    return cmd.run();
}
}
