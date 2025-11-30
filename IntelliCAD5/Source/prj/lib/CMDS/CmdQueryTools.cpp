// File  :
// Author: Alexey Malov
#include "Gr.h"
#include "sds.h" //sds_ssfree, sds_name
#include "db.h" //db_hitype2str
#include "IcadView.h"
#include "Icad.h"
#include "CmdQueryTools.h"
#include "Modeler.h"
#include "transf.h"
#include "geodata.h"
using namespace icl;

CCmdQueryInt::CCmdQueryInt()
:
CCmdQueryEx(),
TCmdQueryLink<1>(),
m_iAnswer(0)
{}

void CCmdQueryInt::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);
    m_iAnswer = 0;
}

CCmdQuery* CCmdQueryInt::query() throw (exception)
{
    m_rc = sds_initget(m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    m_rc = sds_getint(m_sPrompt.c_str(), &m_iAnswer);

	if (!(m_accept & RSG_NONULL) && m_rc == RTNONE)
		return next();

    if (m_rc != RTNORM)
        throw exception();

    return next();
};

int CCmdQueryInt::getValue() const
{
    return m_iAnswer;
}

void CCmdQueryInt::setValue(int val)
{
    m_iAnswer = val;
}

CCmdQueryDist::CCmdQueryDist()
:
CCmdQueryEx(),
TCmdQueryLink<1>(),
m_dAnswer(0.)
{}

void CCmdQueryDist::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);
    m_dAnswer = 0.;
}

CCmdQuery* CCmdQueryDist::query() throw (exception)
{
    m_rc = sds_initget(m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    m_rc = sds_getdist(0, m_sPrompt.c_str(), &m_dAnswer);

	if (!(m_accept & RSG_NONULL) && m_rc == RTNONE)
		return next();

    if (m_rc != RTNORM)
        throw exception();

    return next();
}

double CCmdQueryDist::getValue() const
{
    return m_dAnswer;
}

void CCmdQueryDist::setValue(double val)
{
    m_dAnswer = val;
}

CCmdQueryEntity::CCmdQueryEntity()
:
CCmdQueryEx(),
TCmdQueryLink<1>()
{
    m_entity[0] = 0;
    m_entity[1] = 0;
}

void CCmdQueryEntity::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);
    m_entity[0] = 0;
    m_entity[1] = 0;
}

CCmdQuery* CCmdQueryEntity::query() throw (exception)
{
    do
    {
        m_rc = sds_initget(m_accept, m_sKeys.c_str());
        if (m_rc != RTNORM)
            throw exception();

        m_rc = sds_entsel(m_sPrompt.c_str(), m_entity, m_pt);

        switch (m_rc)
        {
        case RTERROR:
		{
			struct resbuf rb;
			SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (rb.resval.rint==OL_ENTSELPICK)
				continue;
			else
				throw exception();
		}
        case RTCAN:
            throw exception();
        case RTNORM:
			if (isAppropriate()) {
			    sds_name ename;
				getEntityName(ename);
				sds_redraw(ename,IC_REDRAW_HILITE);
				return next();
			}
        case RTKWORD:
            if (!(m_accept & RTKWORD)) {
                char str[512];
                if (sds_getinput(str) == RTERROR)
                    throw exception();

                if (strisame(str,"LAST"/*DNT*/) && sds_entlast(m_entity) == RTNORM) {
			        if (isAppropriate()) {
			            sds_name ename;
				        getEntityName(ename);
				        sds_redraw(ename,IC_REDRAW_HILITE);
                        m_rc = RTNORM;
				        return next();
			        }
                }
            }
        default:
            throw exception();
		}
    }
    while (true);

    return 0;
}

db_handitem* CCmdQueryEntity::getEntity() const
{
    return reinterpret_cast<db_handitem*>(m_entity[0]);
}

void CCmdQueryEntity::getEntityName(sds_name ename)
{
    ename[0] = m_entity[0];
    ename[1] = m_entity[1];
}

CCmdQuerySelectionSet::CCmdQuerySelectionSet()
:
CCmdQueryEx(),
TCmdQueryLink<1>()
{
    m_ssEntities[0] = 0;
    m_ssEntities[1] = 0;
    m_filter.restype = RTSTR;
    m_filter.resval.rstring = 0;
}

CCmdQuerySelectionSet::~CCmdQuerySelectionSet()
{
    sds_ssfree(m_ssEntities);
}

void CCmdQuerySelectionSet::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);
    m_ssEntities[0] = 0;
    m_ssEntities[1] = 0;
}

const std::string& CCmdQuerySelectionSet::getFilter()
{
	return m_sTypes;
}

void CCmdQuerySelectionSet::setFilter(const char* szTypes)
{
	m_sTypes = szTypes;
}

void CCmdQuerySelectionSet::getEntities(sds_name ents)
{
    ents[0] = m_ssEntities[0];
    ents[1] = m_ssEntities[1];
}

CCmdQuery* CCmdQuerySelectionSet::query() throw (exception)
{
    m_rc = sds_initget(m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

	if (!m_sTypes.empty())
		m_filter.resval.rstring = const_cast<char*>(m_sTypes.c_str());

    const char* szSelectionMethod = 0;
    void* pFirstPoint = 0;
    void* pSecondPoint = 0;
    const int bOmitFromPrevSS = 0;
    bool bIncludeInvisible = true;
    bool bIncludeLockedLayers = false;
    bool bDirectCall = true;
	m_filter.rbnext = 0;
    m_rc = sds_pmtssget(m_sPrompt.c_str(),
                        szSelectionMethod,
                        pFirstPoint,
                        pSecondPoint,
                        &m_filter,
                        m_ssEntities,
                        bOmitFromPrevSS,
                        bIncludeInvisible,
                        bIncludeLockedLayers,
                        bDirectCall);
    if (m_rc != RTNORM)
        throw exception();

    return next();
}

CCmdQueryPoint::CCmdQueryPoint()
:
CCmdQueryEx(),
TCmdQueryLink<1>()
{
    init(ResourceString(IDC_QUERYTOOLS_SPECIFYPOINT, "Specify a point"));
}

void CCmdQueryPoint::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);
    m_point.set(0., 0., 0.);
    m_base.set(0.,0.,0.);
    m_bUseBase = false;
}

void CCmdQueryPoint::useBasePoint(const point* base)
{
    if (base)
    {
        m_base = *base;
        m_bUseBase = true;
    }
    else
    {
        m_bUseBase = false;
    }
}

CCmdQuery* CCmdQueryPoint::query() throw (exception)
{
    m_rc = sds_initget(m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    if (m_bUseBase)
        m_rc = sds_getpoint(m_base, m_sPrompt.c_str(), m_point);
    else
        m_rc = sds_getpoint(0, m_sPrompt.c_str(), m_point);

	if (!(m_accept & RSG_NONULL) && m_rc == RTNONE)
		return next();

    if (m_rc != RTNORM)
        throw exception();

    return next();
}

point CCmdQueryPoint::getValue() const
{
    return m_point;
}

void CCmdQueryPoint::setValue(const point& val)
{
    m_point = val;
}

CCmdQueryVector::CCmdQueryVector()
:
CCmdQueryEx(),
TCmdQueryLink<1>()
{}

CCmdQuery* CCmdQueryVector::query() throw (exception)
{
    m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_VECTOR, "Vector"));
    m_rc = sds_initget(RSG_NONULL | m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    point pt1, pt2;
    m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SPECIFY_BASEPOINT_VECTOR, "\nSpecify <Base point>/Vector: "));
    m_rc = sds_getpoint(0, m_sPrompt.c_str(), pt1);
    assert(m_rc != RTNONE);

    m_rc = sds_initget(RSG_NONULL | m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    if (m_rc == RTKWORD)
    {
        // input vector
        m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SPECIFY_VECTOR, "\nSpecify vector: "));
        m_rc = sds_getpoint(0, m_sPrompt.c_str(), m_vector);
        assert(m_rc != RTNONE);

        if (m_rc != RTNORM)
            throw exception();

        return next();
    }
    else if (m_rc == RTNORM)
    {
        // input second point
        m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SPECIFY_ENDPOINT,	"\nSpecify end point: "));
        m_rc = sds_getpoint(pt1, m_sPrompt.c_str(), pt2);
        assert(m_rc != RTNONE);

        if (m_rc != RTNORM)
            throw exception();

        m_vector = pt2 - pt1;

        return next();
    }
    throw exception();
}

gvector CCmdQueryVector::getValue() const
{
    return m_vector;
}

void CCmdQueryVector::setValue(const gvector& val)
{
    m_vector = val;
}

// check entity type
bool isKindOf(const db_handitem* pe, ...)
{
    va_list types;
    va_start(types, pe);

    int type = const_cast<db_handitem*>(pe)->ret_type();
    int itype;
    while ((itype = va_arg(types, int)) != -1)
    {
        if (type == itype)
            break;
    }

    va_end (types);

    return itype != -1;
}

int create
(
const CDbAcisEntityData& obj,
sds_name ename
)
{
    resbuf* elist = sds_buildlist(RTDXF0, db_hitype2str(obj.getDbtype()), 1, obj.getSat().c_str(), 0);
    if (!elist)
        return 0;

    if (RTNORM != sds_entmake(elist) ||
        RTNORM != sds_entlast(ename))
    {
        sds_relrb(elist);
        return 0;
    }
    if (RTNORM != sds_relrb(elist))
    {
        sds_entdel(ename);
        return 0;
    }

    return 1;
}


int create
(
sds_name ename,						// output:			entitiy name
const CDbAcisEntityData& obj,		// input:			acis object
int color,							// input:			color
const char* szLayer,				// optional input:	layer name
const char* szLinetype				// optional input:	linetype
)
{
    resbuf* elist = NULL;

	if (szLayer && !szLinetype)
	    elist = sds_buildlist(RTDXF0, db_hitype2str(obj.getDbtype()), 1, obj.getSat().c_str(),
							  8, szLayer, 62, color, 0);
	else if (szLayer && szLinetype)
		elist = sds_buildlist(RTDXF0, db_hitype2str(obj.getDbtype()), 1, obj.getSat().c_str(),
							  8, szLayer, 6, szLinetype, 62, color, 0);
	else
		elist = sds_buildlist(RTDXF0, db_hitype2str(obj.getDbtype()), 1, obj.getSat().c_str(),
							  62, color, 0);

    if (!elist)
        return 0;

    if (RTNORM != sds_entmake(elist) ||
        RTNORM != sds_entlast(ename))
    {
        sds_relrb(elist);
        return 0;
    }
    if (RTNORM != sds_relrb(elist))
    {
        sds_entdel(ename);
        return 0;
    }

    return 1;
}

int create
(
const CDbAcisEntityData& obj
)
{
	sds_name ename;
	return create(obj, ename);
}

CCmdQueryAxis::CCmdQueryAxis()
:
CCmdQueryEx(),
TCmdQueryLink<1>()
{}

void CCmdQueryAxis::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);

    m_root.set(0., 0., 0.);
    m_dir.set(0., 0., 0.);
}

CCmdQuery* CCmdQueryAxis::query() throw (exception)
{
    m_getPoint.init(ResourceString(IDC_QUERYTOOLS_SPECIFY_BASEPOINT,	"\nSpecify the base point: "));
    m_getPoint.query();
    m_root = m_getPoint.getValue();

    m_getPoint.init(ResourceString(IDC_QUERYTOOLS_SPECIFY_ANOTHERPOINT_ALONG_AXIS,"\nSpecify another point along the axis: "));
    m_getPoint.useBasePoint(&m_root);
    m_getPoint.query();
    m_dir = m_getPoint.getValue() - m_root;

    return next();
}

point CCmdQueryAxis::getRoot() const
{
    return m_root;
}

void CCmdQueryAxis::setRoot(const point& val)
{
    m_root = val;
}

gvector CCmdQueryAxis::getDirection() const
{
    return m_dir;
}

void CCmdQueryAxis::setDirection(const gvector& val)
{
    m_dir = val;
}

CCmdQueryAngle::CCmdQueryAngle()
:
CCmdQueryEx(),
TCmdQueryLink<1>(),
m_angle(0.)
{}

void CCmdQueryAngle::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);
    m_angle = 0.;
}

double CCmdQueryAngle::getValue() const
{
    return m_angle;
}

void CCmdQueryAngle::setValue(double val)
{
    m_angle = val;
}

CCmdQuery* CCmdQueryAngle::query() throw (exception)
{
    m_rc = sds_initget(m_accept, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    m_rc = SDS_getangleneg(0, m_sPrompt.c_str(), &m_angle);

	if (!(m_accept & RSG_NONULL) && m_rc == RTNONE)
		return next();

    if (m_rc != RTNORM)
        throw exception();

    return next();
}

CCmdQueryCurves::CCmdQueryCurves()
:CCmdQuerySelectionSet()
{
    m_sTypes = "LINE,ARC,ELLIPSE,CIRCLE,POLYLINE,LWPOLYLINE,SPLINE"/*DNT*/;
    m_filter.resval.rstring = const_cast<char*>(m_sTypes.c_str());
}

CCmdQueryCurves::getCurves(std::vector<geodata::curve*>& curves)
{
    sds_name ename;
    sds_name ncurves;
    getEntities(ncurves);
	for (long icrv = 0l; sds_ssname(ncurves, icrv, ename) == RTNORM; ++icrv)
    {
        db_handitem* item = reinterpret_cast<db_handitem*>(ename[0]);
        geodata::curve* crv;
        if (!geodata::extract(*item, crv))
        {
            for (int i = 0, n = curves.size(); i < n; ++i)
                delete curves[i];

            return 0;
        }
        curves.push_back(crv);
    }

    return 1;
}

bool CCmdQueryAcisEntity::isAppropriate() const
{
	return isKindOf(getEntity(), DB_3DSOLID, DB_BODY, DB_REGION, -1);
}

const CDbAcisEntityData& CCmdQueryAcisEntity::getAcisEntityData() const
{
	return getEntity()->getData();
}

CDbAcisEntity* CCmdQueryAcisEntity::getEntity() const
{
	return static_cast<CDbAcisEntity*>(CCmdQueryEntity::getEntity());
}


int CCmdQueryFaces::select(bool bAll) throw (exception)
{
	if (!TCmdQuerySubents<HFACE>::select(true)) { // don't change to pass "bAll" value!!!
		if (m_rc == RTVOID) { // users may select a point in the face.
			resbuf filter;
		    filter.restype = 0;
		    filter.resval.rstring = getFillet();
		    filter.rbnext = 0;
			if (m_dbitem == NULL) {
				sds_name ss;
				m_rc = sds_ssget("X", NULL, NULL, &filter, ss);
				if (m_rc != RTNORM) {
					if (m_rc == RTERROR)
						m_rc = RTVOID; 
					return 0;
				}
				long ssSize = 0L;
				m_rc = sds_sslength(ss, &ssSize);
				if (m_rc != RTNORM) {
					sds_ssfree(ss);
					return 0;
				}
				sds_name ename;
				for (register i = 0; i < ssSize; i++) {
					// we loop through each ACIS entity to test which one contains
					// face selected.
					//
					m_rc = sds_ssname(ss, i, ename);
					if (m_rc != RTNORM) {
						sds_ssfree(ss);
						throw exception();
					}
					CDbAcisEntity* pAcisEnt = reinterpret_cast<CDbAcisEntity*>(ename[0]);
						
					m_dbitem = pAcisEnt;
					m_obj = m_dbitem->getData();
					m_ename[0] = ename[0];
					m_ename[1] = ename[1];
					if (!pick()) {
						m_dbitem = NULL;
						m_ename[0] = m_ename[1] = 0;
					}
					else { // find selected ACIS entities
						sds_ssfree(ss);
						return 1;
					}
				}
				// no face is selected.
				sds_ssfree(ss);
				return 0;
			}
			else {
				if (!pick()) { // may be select a face, which is on the different solid
					cmd_ErrorPrompt(selectOnWrongEntityErrorIndex(), 0);
					return 0; 
				}
				return 1;
			}
		}
		return 0;
	}
	return 1;
}




int CCmdQueryFaces::pickSubent(gvector& dir, double prec)
{
	int mode = CModeler::eByEdge | CModeler::eByVertex |
		CModeler::eByFace;
    return CModeler::get()->pickFace(m_obj, m_pt, dir, prec, mode, m_last);
}

int CCmdQueryFaces::getAllSubents()
{
    return CModeler::get()->getFaces(m_obj, m_last);
}


int CCmdQueryEdges::pickSubent(gvector& dir, double prec)
{
    return CModeler::get()->pickEdge(m_obj, m_pt, dir, prec, CModeler::eOnly, m_last);
}

int CCmdQueryEdges::getAllSubents()
{
    return CModeler::get()->getEdges(m_obj, m_last);
}
CCmdQuery* CCmdQueryColor::query() throw (exception)
{
    if (SDS_GetColorDialog(m_color, &m_color, 0) != RTNORM)
        throw exception();

    return next();
}

CCmdQueryYesOrNo::CCmdQueryYesOrNo()
:
CCmdQueryEx(),
TCmdQueryLink<1>(),
m_bAnswer(false)
{}

CCmdQueryYesOrNo::CCmdQueryYesOrNo
(
const char* sPrompt,
int accept
)
:
CCmdQueryEx(),
TCmdQueryLink<1>(),
m_bAnswer(false)
{
	init(sPrompt, "", accept);
}

CCmdQuery* CCmdQueryYesOrNo::query() throw (exception)
{
    m_rc = sds_initget(SDS_RSG_NODOCCHG, ResourceString(IDC_QUERYTOOLS_YESNO, "Yes No ~_Yes ~_No"));
	if (m_rc != RTNORM)
		return 0;

	m_sPrompt += ResourceString(IDC_QUERYTOOLS_YESNO2, " Yes/<No>: ");

    char answer[IC_ACADBUF];
    m_rc = sds_getkword(m_sPrompt.c_str(), answer);
	if (m_rc != RTNORM &&
        m_rc != RTNONE)
        throw exception();

    if (m_rc == RTNONE)
    {
        m_bAnswer = false;
    }
    else
    {
        m_bAnswer = strisame("YES", answer);
    }

    return next();
}

CCmdQueryCommandCallback::CCmdQueryCommandCallback()
:m_pCommand(0), m_callback(0)
{}

CCmdQueryCommandCallback::CCmdQueryCommandCallback
(
CCmdCommand* pCommand,
method_t callback
)
:m_pCommand(pCommand), m_callback(callback)
{}

void CCmdQueryCommandCallback::init
(
CCmdCommand* pCommand,
method_t callback
)
{
	m_pCommand = pCommand;
	m_callback = callback;
}

CCmdQuery* CCmdQueryCommandCallback::query() throw (exception)
{
	return (m_pCommand->*m_callback)();
}

int ucs2wcs(transf& t)
{
    if (!wcs2ucs(t))
        return 0;
    t.inverse();
    return 1;
}

transf ucs2wcs()
{
    return wcs2ucs().inverse();
}

int wcs2ucs(transf& t)
{
    resbuf rb;
    if (SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
        return 0;
    gvector xaxis(rb.resval.rpoint);

    if (SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
        return 0;
    gvector yaxis(rb.resval.rpoint);

	if (SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
        return 0;
    point orig(rb.resval.rpoint);

    t.coordinate(orig, xaxis, yaxis);

    return 1;
}

transf wcs2ucs()
{
    resbuf rb;
    if (SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
        return scaling(0.); // return degenerated matrix
    gvector xaxis(rb.resval.rpoint);

    if (SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
        return scaling(0.); // return degenerated matrix
    gvector yaxis(rb.resval.rpoint);

	if (SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
        return scaling(0.); // return degenerated matrix
    point orig(rb.resval.rpoint);

    return coordinate(orig, xaxis, yaxis);
}

int wcs2ucs2wcs
(
transf& t
)
{
    transf w2u;
    if (!wcs2ucs(w2u))
        return 0;

    transf u2w(w2u);
    u2w.inverse();

    // make maultiplication without creating of temporary matrices
    w2u *= (t *= u2w);
    t = w2u;

    return 1;
}

int checkSuccess(int rc, int success_value, ...) throw (int)
{
    va_list values;
    va_start(values, success_value);

	do
	{
		if (rc == success_value)
		{
			va_end(values);
			return rc;
		}
	}
    while ((success_value = va_arg(values, int)) != 0);

    va_end(values);
	throw rc;
}

int checkNoFailure(int rc, int fail_value, ...) throw (int)
{
    va_list values;
    va_start(values, fail_value);

	do
	{
		if (rc == fail_value)
		{
			va_end(values);
			throw rc;
		}
	}
    while ((fail_value = va_arg(values, int)) != 0);

    va_end(values);
	return rc;
}

void printAcisError()
{
	if (!CModeler::checkLimitedModeler())
		return;

	CModeler* pModeler = CModeler::get();

	int ec = pModeler->getLastErrorCode();

	icl::string message;
	pModeler->getErrorMessage(ec, message);
	if (!message.empty())
	{
		sds_printf("\n");
		sds_printf(message.c_str());
	}
}


#if 0
transf wcs2ocs(const gvector& zaxis)
{
    double l = zaxis.norm();
    if (l < resabs())
		return scaling(0.);

    gvector z = zaxis;
    z.normalize(l);

	gvector x, y;
    if (z == gvector(0.,0.,1.))
    {
        x.set(1., 0., 0.);
        y.set(0., 1., 0.);
    }
	else
	{
		double cap = 1./64.;  /* square polar cap. */

		if (fabs(z[0]) < cap &&
			fabs(z[1]) < cap)
		{
			// ECSX = WCSY X ECSZ
			x.set(z[2], 0., -z[0]);
		}
		else
		{
			// ECSX = WCSZ X ECSZ
			x.set(-z[1], z[0], 0.);
		}

		if (fabs(x.normalize()) < resabs())
			return scaling(0.);

		y = z*x;
	}

	return coordinate(point(0.,0.,0.), x, y);
}

transf ocs2wcs(const icl::gvector& zaxis)
{
	return wcs2ocs(zaxis).inverse();
}




#endif
