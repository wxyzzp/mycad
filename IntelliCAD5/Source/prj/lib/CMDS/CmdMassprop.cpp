#include "CmdCommand.h"
#include <afx.h>	/*DG - 1.3.2002*/// hack: include MFC before inclusion of windows.h in subsequential includes from CmdQueryTools.h
#include "CmdQueryTools.h"
#include "DbAcisEntity.h"
#include "Modeler.h"
#include "gvector.h"
#include "transf.h"
#include "ntype.h"
using namespace icl;
#undef min
#undef max
#include <valarray>
#include <fstream>
using namespace std;
#include "CmdOut.h"


struct CMasspropData
{
	// bounding box
	point m_low, m_high;
	// volume
	double m_volume;
	// center of gravity
	point m_cofg;

	typedef ntype<double,3> double3;
	// moments of inertia in wcs
	double3 m_mofi;
	// products of inertia: XY, YZ, ZX
	double3 m_pofi;
	// radii of gyration
	double3 m_rofg;
	
	// principal moments of inertia
	double3 m_pmofi;
	// principal axes of inertia
	ntype<gvector,3> m_paofi;
};

class CCmdMassprop: public CCmdCommand
{
	//atom queries
	//select acis object:
	CCmdQuerySelectionSet m_getAcisEntities;

	CCmdQueryYesOrNo m_needToWriteToFile;

	char* getFilenameDialog();

public:
	CCmdMassprop();
	virtual int run();
};

CCmdMassprop::CCmdMassprop()
{
	m_pStart = &m_getAcisEntities;
	m_getAcisEntities.setFilter("3DSOLID,REGION,BODY"/*DNT*/);
};

char* CCmdMassprop::getFilenameDialog()
{
	resbuf fileName;
	int opt = 3; // EBATECH(CNBR) 2002/4/30 disable type button( option +2)
	if (SDS_GetFiled("Create Mass and Area Properties File", NULL, "mpr"/*DNT*/, opt, &fileName, NULL, NULL) != RTNORM ||
		fileName.restype != RTSTR || !fileName.resval.rstring || !*fileName.resval.rstring)
		return 0;

	return fileName.resval.rstring;
}

template <typename _Os>
void writeInfo
(
_Os& out,
CMasspropData& data
)
{
	using namespace cmdout;
	
	// Mass
	out << description(ResourceString(IDC_MASSPROP_MASS, "Mass:")) << real(data.m_volume);
	// Volume
	out << description(ResourceString(IDC_MASSPROP_VOLUME, "Volume:")) << real(data.m_volume);
	// Bounding box
	out << description(ResourceString(IDC_MASSPROP_BOUNDINGBOX, "Bounding Box:")) 
		<< description(ResourceString(IDC_MASSPROP_LOWERBOUND, "Lower Bound:")) << data.m_low
		<< description(ResourceString(IDC_MASSPROP_UPPERBOUND, "Upper Bound:")) << data.m_high;
	// Centroid
	out << description(ResourceString(IDC_MASSPROP_CENTROID, "Centroid:")) << data.m_cofg;
	// Moments of intercia
	out << description(ResourceString(IDC_MASSPROP_MOMENTSOFINERTIA, "Moments of inertia:")) << point(data.m_mofi.m_a);
	// Products of inertia
	out << description(ResourceString(IDC_MASSPROP_PRODUTSOFINERTIA, "Products of inertia:"))
		<< description(ResourceString(IDC_MASSPROP_XY, "XY:")) << real(data.m_pofi[0])
		<< description(ResourceString(IDC_MASSPROP_YZ, "YZ:")) << real(data.m_pofi[1])
		<< description(ResourceString(IDC_MASSPROP_ZX, "ZX:")) << real(data.m_pofi[2]);
	// Radii of gyration
	out << description(ResourceString(IDC_MASSPROP_RADIIOFGIRATION, "Radii of gyration:")) << point(data.m_rofg.m_a);
	// Principal moments and X-Y-Z directions about centroid
	out << description(ResourceString(IDC_MASSPROP_PRINTIPALMOMENTS, "Principal moments and X-Y-Z directions about centroid:"))
		<< description(ResourceString(IDC_MASSPROP_I, "I:")) << real(data.m_pmofi[0]) << ResourceString(IDC_MASSPROP_ALONG, " along ") << data.m_paofi[0]
		<< description(ResourceString(IDC_MASSPROP_J, "J:")) << real(data.m_pmofi[1]) << ResourceString(IDC_MASSPROP_ALONG, " along ") << data.m_paofi[1]
		<< description(ResourceString(IDC_MASSPROP_K, "K:")) << real(data.m_pmofi[2]) << ResourceString(IDC_MASSPROP_ALONG, " along ") << data.m_paofi[2];

}

int CCmdMassprop::run()
{
	CModeler* modeler = CModeler::get();
	if (!modeler->canModify())
		//unsupported by modeler
		return RTERROR;

	if (CCmdCommand::run())
	{
		long i, n;
		long nitems;
		sds_name ents;
		m_getAcisEntities.getEntities(ents);
		sds_sslength(ents, &nitems);

		vector<CDbAcisEntityData> objs(nitems);
		sds_name ename;
		transf w2u = wcs2ucs();
		vector<point> lows(nitems), highs(nitems);
		for (i = 0; RTNORM == sds_ssname(ents, i, ename); ++i)
		{
			CDbAcisEntity* item = reinterpret_cast<CDbAcisEntity*>(ename[0]);

			// get bounding box
			if (!modeler->getBoundingBox(item->getData(), lows[i], highs[i], w2u))
				return RTERROR;

			objs[i] = item->getData();
		}

		CMasspropData data;
		data.m_low = lows[0];
		data.m_high = highs[0];
		for (i = 1, n = lows.size(); i < n; i++)
		{
			data.m_low.x() = _cpp_min(data.m_low.x(), lows[i].x());
			data.m_low.y() = _cpp_min(data.m_low.y(), lows[i].y());
			data.m_low.z() = _cpp_min(data.m_low.z(), lows[i].z());
			
			data.m_high.x() = _cpp_max(data.m_high.x(), highs[i].x());
			data.m_high.y() = _cpp_max(data.m_high.y(), highs[i].y());
			data.m_high.z() = _cpp_max(data.m_high.z(), highs[i].z());
		}

		// get mass properties
		if (!modeler->getMassProperties(objs, data.m_volume, data.m_cofg, data.m_mofi, 
										data.m_pofi, data.m_rofg, data.m_pmofi, data.m_paofi))
			return RTERROR;

		// print out
		{
			CCmdOut out;
			out.openPromptWindow();
			writeInfo(out, data);
		}

		CCmdQueryYesOrNo saveOrNot(ResourceString(IDC_MASSPROP_WRITETOFILE, "\nWrite analysis to a file?"), 0);
		try
		{
			saveOrNot.query();
		}
		catch (const exception&)
		{
			return saveOrNot.rc();
		}
		if (saveOrNot.value())
		{
			char* filename = getFilenameDialog();
			ofstream fout(filename);
			IC_FREE(filename);
			
			if (fout.good())
			{
				fout << ResourceString(IDC_MASSPROP_HEADER, " ----------------   SOLIDS    ---------------- ") << endl;
				writeInfo(fout, data);
			}
		}
	}
	return m_rc;
}

short cmd_massprop()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	CCmdMassprop cmd;
	return cmd.run();
}

