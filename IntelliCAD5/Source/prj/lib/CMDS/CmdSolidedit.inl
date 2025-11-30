// File  :
// Author: Alexey Malov

// Solid processing

inline
CCmdSolidImprint::CCmdSolidImprint()
:
CCmdSolidProcess(),
m_getImprinter(&m_get3dSolid)
{
    m_deleteSource.init(ResourceString(IDC_SOLIDEDIT_DELETESOURCEOBJECT, "\nDelete the source object"));

	m_pStart = &m_getImprinter;
	m_getImprinter.set(0, &m_deleteSource);
}

inline
CCmdQuery* CCmdSolidProcess::query() throw(exception)
{
    run();

	sds_name ename;
	m_get3dSolid.getEntityName(ename);
	SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
    return next();
}

// Edge processing
inline
CCmdEdgesColor::CCmdEdgesColor()
:
CCmdEdgesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryColor);
}

inline
CCmdEdgesCopy::CCmdEdgesCopy()
:
CCmdEdgesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryDisplacement);
}


//Face processing
inline
CCmdFacesTaper::CCmdFacesTaper()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryAxis);
    m_queryAxis.set(0, &m_queryAngle);
    m_queryAngle.init(ResourceString(IDC_SOLIDEDIT_SPECIFYTAPERANGLE, "\nSpecify the taper angle: "));
}

inline
CCmdFacesExtrude::CCmdFacesExtrude()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryDistOrPath);
}

inline
CCmdFacesMove::CCmdFacesMove()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryDisplacement);
}

inline
CCmdFacesOffset::CCmdFacesOffset()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryOffsetDistance);

    m_queryOffsetDistance.init(ResourceString(IDC_SOLIDEDIT_SPECIFYOFFSETDIST, "\nSpecify offset distance: "), 0, RSG_NONULL|RSG_NOZERO);
    m_queryOffsetDistance.set(0, 0);
}

inline
CCmdFacesRotate::CCmdFacesRotate()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_getRotationAxis);
    m_getRotationAxis.set(0, &m_getAngle);

    m_getAngle.init(ResourceString(IDC_SOLIDEDIT_SPECIFYROTATIONANGLE, "\nSpecify the rotation angle: "), 0, RSG_NONULL|RSG_NOZERO);
}

inline
CCmdFacesDelete::CCmdFacesDelete()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
}

inline
CCmdFacesCopy::CCmdFacesCopy()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryDisplacement);
    m_queryDisplacement.set(0, 0);
}

inline
CCmdFacesColor::CCmdFacesColor()
:
CCmdFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_queryColor);
}

// Misc
inline
const geodata::curve* CCmdQueryDistanceOrPath::getPath() const
{
    return m_pPath;
}

inline
double CCmdQueryDistanceOrPath::getDistance() const
{
    return m_getDist.getValue();
}

inline
double CCmdQueryDistanceOrPath::getTaperAngle() const
{
    return (-1. * m_getTaperAngle.getValue());
}

inline
bool CCmdQuery3dSolid::isAppropriate() const
{
	if (isKindOf(getEntity(), DB_3DSOLID, -1) || isKindOf(getEntity(), DB_BODY, -1)) {
//		SDS_RedrawEntity(m_entity,IC_REDRAW_HILITE,NULL,NULL,1);
		return true;
	}
	else
		return false;
}

inline
const CDbAcisEntityData& CCmdQuery3dSolid::getAcisEntityData() const
{
	CDbAcisEntity* e = static_cast<CDbAcisEntity*>(getEntity());
	return e->getData();
}

inline
bool CCmdQueryImprinter::isCurve() const
{
	return isKindOf(getEntity(), DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1);
}

inline
bool CCmdQueryImprinter::is3DSolid() const
{
	return isKindOf(getEntity(), DB_3DSOLID, -1);
}


inline
const CDbAcisEntityData& CCmdQueryImprinter::getAcisEntityData() const
{
	CDbAcisEntity* e = static_cast<CDbAcisEntity*>(getEntity());
	return e->getData();
}

inline
bool CCmdQueryImprinter::isAppropriate() const
{
	return m_pGet3dSolid->getEntity() != getEntity() && (isCurve() || is3DSolid());
}

inline
void CCmdUndoProcess::modify(sds_name ename, const CDbAcisEntityData& obj)
{
    push(ename, obj, CUndoAtom::eModify);
}

inline
void CCmdUndoProcess::create(const CDbAcisEntityData& obj)
{
    sds_name ename = {0,0};
    push(ename, obj, CUndoAtom::eCreate);
}

inline
void CCmdUndoProcess::erase(sds_name ename)
{
	CDbAcisEntityData obj;
    push(ename, obj, CUndoAtom::eErase);
}

inline
void CCmdUndoProcess::push(sds_name ename, const CDbAcisEntityData& obj, int mode)
{
    CUndoAtom atom(ename, obj, mode);
    m_undo.push(atom);
}
