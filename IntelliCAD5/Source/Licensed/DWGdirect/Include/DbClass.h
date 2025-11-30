///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statement 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////


#ifndef _ODDBCLASS_INCLUDED_
#define _ODDBCLASS_INCLUDED_

#include "RxObject.h"

#include "DD_PackPush.h"

/** Description:
    Contains pseudo constructor information for OdDbObject or a subclass of OdDbObject.

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdDbClass : public OdRxClass
{
public:
  ODRX_DECLARE_MEMBERS(OdDbClass);

  /** Description:
      Constructor (no arguments).
  */
  OdDbClass() : m_pConstr(0) {}

  /** Description:
      OdRxClass overridden method.
  */
  OdRxObjectPtr create() const { return m_pConstr(); }

  /** Description:
      Gets current constructor of the C++ class that this OdRxClass object represents.

      Return Value:
      A pointer to the constructor.
  */
  OdPseudoConstructorType getPseudoConstructor() const { return m_pConstr; }

  /** Description:
      Sets constructor of the C++ class that this OdRxClass object represents.
  */
  void setPseudoConstructor(OdPseudoConstructorType pConstr) { m_pConstr = pConstr; }

private:
  OdPseudoConstructorType m_pConstr;
};

typedef OdSmartPtr<OdDbClass> OdDbClassPtr;


/** Description:
*/
#define ODDB_PSEUDO_DECLARE_MEMBERS(ClassName) \
protected:\
  ClassName(OdDbObjectImpl* pImpl);\
public: \
  ODRX_DECLARE_MEMBERS(ClassName);\
  static OdPseudoConstructorType g_pMainConstr;\
  OdRxClass* saveAsClass(OdRxClass* pClass) const

/** Description:
*/
#define ODDB_PSEUDO_DEFINE_MEMBERS(ClassName,ParentClass,PseudoBaseClass,DOCREATE)\
ODRX_DEFINE_MEMBERS(ClassName)\
\
OdPseudoConstructorType ClassName::g_pMainConstr = 0;\
\
OdRxClass* ClassName::saveAsClass(OdRxClass*) const\
{\
  return PseudoBaseClass::saveAsClass(PseudoBaseClass::desc());\
}\
\
OdRxObjectPtr ClassName::pseudoConstructor()\
{\
  OdRxObjectPtr pObj; DOCREATE(ClassName, pObj); return pObj;\
}\
\
OdRxObject* ClassName::queryX(const OdRxClass* pClass) const\
{\
  return ::odQueryXImpl<ClassName,ParentClass>(this, pClass);\
}\
\
void ClassName::rxInit()\
{\
  if (!ClassName::g_pDesc)\
  {\
    OdRxClass* pParent = ParentClass::desc();\
    OdDbClassPtr pPseudoBase = PseudoBaseClass::desc();\
    if (pParent->isA()!=pPseudoBase.get() && !pParent->isDerivedFrom(pPseudoBase.get()))\
    {\
      throw OdError(eNotThatKindOfClass);\
    }\
    g_pDesc = ::newOdRxClass(#ClassName, ParentClass::desc(), &pseudoConstructor);\
    g_pMainConstr = pPseudoBase->getPseudoConstructor();\
    pPseudoBase->setPseudoConstructor(ClassName::pseudoConstructor);\
  }\
}\
\
void ClassName::rxUninit()\
{\
  OdDbClassPtr pDbClass = PseudoBaseClass::desc();\
  pDbClass->setPseudoConstructor(g_pMainConstr);\
  ::deleteOdRxClass(g_pDesc);\
  g_pDesc = 0;\
}

#include "DD_PackPop.h"

#endif //_ODDBCLASS_INCLUDED_

