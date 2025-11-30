#ifndef __ODGICONVEYORENTRYPOINT__
#define __ODGICONVEYORENTRYPOINT__

#include "GiEmptyGeometry.h"

/** Description:
    
    {group:OdGi_Classes} 
*/
class OdGiConveyorEntryPoint : public OdGiConveyorOutput
{
protected:
  OdGiConveyorGeometry* m_pGeometry;
public:
  OdGiConveyorEntryPoint()
    : m_pGeometry(OdGiEmptyGeometry::kVoid)
  { }

  OdGiConveyorEntryPoint(OdGiConveyorGeometry* pGeom)
    : m_pGeometry(pGeom ? pGeom : OdGiEmptyGeometry::kVoid)
  { }

  void setDestGeometry(OdGiConveyorGeometry& destGeometry)
  {
    m_pGeometry = &destGeometry;
  }

  inline OdGiConveyorGeometry& geometry() { return *m_pGeometry; }
};


#endif // __ODGICONVEYORENTRYPOINT__
