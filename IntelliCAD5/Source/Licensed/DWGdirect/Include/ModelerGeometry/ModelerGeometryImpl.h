#ifndef _ODMODELERGEOMETRYIMPL_INCLUDED_
#define _ODMODELERGEOMETRYIMPL_INCLUDED_

#include "ModelGeomExport.h"
#include "RxModule.h"
#include "Ed/EdCommandStack.h"

class wrTriangulationParams;

/** Description:

    {group:Other_Classes}
*/
class MODELERGEOM_EXPORT ModelerModule : public OdRxModule
{
  OdSmartPtr<OdEdCommand> m_pProperties;
  OdSmartPtr<OdEdCommand> m_pAcisIn;
  OdSmartPtr<OdEdCommand> m_pAcisOut;
protected:
  void initApp();
  void uninitApp();
};

typedef OdSmartPtr<ModelerModule> ModelerModulePtr;

#endif //_ODMODELERGEOMETRYIMPL_INCLUDED_
