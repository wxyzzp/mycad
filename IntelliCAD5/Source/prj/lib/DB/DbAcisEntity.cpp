// File  : <DevDir>\source\prj\lib\db\DbAcisEntity.cpp
// Author: Dmitry Gavrilov

#include "db.h"
#include "DbAcisEntity.h"
#include "Modeler.h"

using icl::point;

int	CDbAcisEntity::s_EntsCount = 0;

CDbAcisEntity::CDbAcisEntity
(
int type,
db_drawing* pDrw /*= NULL*/
):
db_entity(type, pDrw),
m_pDispList(0)
{
	memset(m_Point, 0, sizeof(m_Point));
	memset(m_Extents, 0, sizeof(m_Extents));

	s_EntsCount++;
}

CDbAcisEntity::CDbAcisEntity
(
const CDbAcisEntity& src
):
db_entity(src),
m_pDispList(0)
{
    m_data = src.getData();
    m_image = src.m_image;

    DB_PTCPY(m_Point, src.m_Point);

    DB_PTCPY(m_Extents[0], src.m_Extents[0]);
	DB_PTCPY(m_Extents[1], src.m_Extents[1]);
    m_pDispList = NULL;

    s_EntsCount++;
}

CDbAcisEntity::~CDbAcisEntity()
{
	if(db_grafreedofp)
		db_grafreedofp(m_pDispList);

    if (!--s_EntsCount)
        CModeler::stop();
}

bool CDbAcisEntity::set_1(char* pData)
{
    m_data.setSat(pData);
	m_data.setDbtype(ret_type());
    return true;
}
bool CDbAcisEntity::get_1(char *p, int maxlen)
{
    int len = min(maxlen, m_data.getSat().size()+1);
    strncpy(p, m_data.getSat().c_str(), len);
    p[len-1] = '\x0';
    return true;
}
bool CDbAcisEntity::get_1(char **p)
{
    *p = const_cast<char*>(m_data.getSat().c_str());
    return true;
}
char **CDbAcisEntity::retp_1(void)  
{
    assert(false);
    return 0;
}

void* CDbAcisEntity::ret_disp()
{
    return m_pDispList; 
}

void CDbAcisEntity::set_disp(void* pNewDispList)
{ 
    m_pDispList = pNewDispList; 
}

void CDbAcisEntity::get_extent
(
sds_point ll, 
sds_point ur
)	
{ 
    DB_PTCPY(ll, m_Extents[0]); 
    DB_PTCPY(ur, m_Extents[1]); 
}

void CDbAcisEntity::set_extent
(
sds_point ll, 
sds_point ur
)	
{ 
    DB_PTCPY(m_Extents[0], ll); 
    DB_PTCPY(m_Extents[1], ur); 
}

const CDbAcisEntityData& CDbAcisEntity::getData() const
{
    return m_data;
}

void CDbAcisEntity::setData(const CDbAcisEntityData& data)
{
    m_data = data;
}

const char* CDbAcisEntity::retImageDataPtr()
{ 
    return m_image.begin();
}

int	CDbAcisEntity::getImageDataSize()
{ 
    return m_image.size();
}

void CDbAcisEntity::getPoint(sds_point pt)
{
    DB_PTCPY(pt, m_Point);  
}

void CDbAcisEntity::setPoint(sds_point newPoint)	
{ 
    DB_PTCPY(m_Point, newPoint); 
}

void
CDbAcisEntity::setImageData
(
char* pImage,
int	nSize
)
{
    m_image.assign(pImage, pImage+nSize);
}

int
CDbAcisEntity::entgetspecific
(
 resbuf**		ppBegRb,
 resbuf**		ppEndRb,
 db_drawing*	pDrw
)
{
    if(!ppBegRb || !ppEndRb)
		return 0;
    
    char* pData;
    get_1(&pData);
    resbuf*	pNewRb = db_newrb(1,'S', pData);
    if (!pNewRb)
		return -1;

    *ppBegRb = *ppEndRb = pNewRb;

    return 0;
}

const icl::point& CDbAcisEntity::draggingStart() const
{
	return m_draggingStart;
}

icl::point& CDbAcisEntity::draggingStart()
{
	return m_draggingStart;
}

const icl::point& CDbAcisEntity::draggingCurr() const
{
	return m_draggingCurr;
}

icl::point& CDbAcisEntity::draggingCurr()
{
	return m_draggingCurr;
}

icl::gvector& CDbAcisEntity::axisZ()
{
	return m_axisZ;
}

const icl::gvector& CDbAcisEntity::axisZ() const
{
	return m_axisZ;
}

icl::gvector& CDbAcisEntity::viewDir()
{
	return m_viewDir;
}

const icl::gvector& CDbAcisEntity::viewDir() const
{
	return m_viewDir;
}
