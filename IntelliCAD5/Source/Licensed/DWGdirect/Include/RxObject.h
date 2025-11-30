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



#ifndef _ODRXOBJECT_INCLUDED_
#define _ODRXOBJECT_INCLUDED_

//DD:EXPORT_ON

#include "RootExport.h"
#include "OdHeap.h"

class OdRxClass;
class OdRxObject;

enum OdRxObjMod { kOdRxObjAttach };


/** Description:
    Declares the functions required by DWGdirect for classes derived from OdRxObject. Classes derived 
    from OdRxObject should invoke this macro in their class definitions, passing the derived
    class name.
*/
#define ODRX_DECLARE_MEMBERS(ClassName)\
public:\
\
  /* Casts the passed in pointer to a ClassName smart pointer. */ \
  static OdSmartPtr<ClassName> cast(const OdRxObject* pObj)\
  {\
    if (pObj)\
      return OdSmartPtr<ClassName>(((ClassName*)pObj->queryX(ClassName::desc())), kOdRxObjAttach);\
    return (ClassName*)0;\
  }\
\
  static OdRxClass* g_pDesc;\
\
  /* Returns the static OdRxClass description object associated with ClassName. */ \
  static inline OdRxClass* desc() { return g_pDesc; };\
\
  /* Returns the OdRxClass description instance associated with this object. */ \
  virtual OdRxClass* isA() const;\
\
  /* Returns a pointer this object, if this object can be converted to the passed in type, otherwise returns a null pointer. */ \
  virtual OdRxObject* queryX(const OdRxClass* pClass) const;\
\
  /* Creates a new instance of this object type, and returns a smart pointer to this instance. */ \
  static OdRxObjectPtr pseudoConstructor();\
\
  /* Creates a new instance of this object type, and returns a typed smart pointer to this instance. */ \
  static OdSmartPtr<ClassName> createObject() { if (!desc()) throw OdError(eNotInitializedYet); return (ClassName*)(desc()->create().get()); }\
\
  /* Registers this class with DWGdirect. */ \
  static void rxInit();\
\
  /* Unregisters this class with DWGdirect. */ \
  static void rxUninit()


#define EMPTY_CONSTR(Class, pObj) pObj = NULL

#define RXIMPL_CONSTR(CLASS, pX) pX = OdRxObjectImpl<CLASS>::createObject()

#define ODRX_DEFINE_MEMBERS(ClassName)\
/** Returns the class description object (OdRxClass) for this object. */ \
OdRxClass* ClassName::isA() const { return desc(); }\
OdRxClass* ClassName::g_pDesc = 0;

#define ODRX_NO_CONS_DEFINE_MEMBERS(ClassName,ParentClass)\
ODRX_DEFINE_MEMBERS(ClassName)\
\
OdRxObjectPtr ClassName::pseudoConstructor() { return (OdRxObject*)0; }\
\
OdRxObject* ClassName::queryX(const OdRxClass* pClass) const\
{ return ::odQueryXImpl<ClassName,ParentClass >(this, pClass); }\
\
void ClassName::rxInit()\
{\
  if (!ClassName::g_pDesc)\
    ClassName::g_pDesc = ::newOdRxClass(#ClassName, ParentClass::desc());\
}\
\
void ClassName::rxUninit()\
{\
  ::deleteOdRxClass(g_pDesc);\
  g_pDesc = 0;\
}


#define ODRX_CONS_DEFINE_MEMBERS(ClassName,ParentClass,DOCREATE)\
ODRX_DEFINE_MEMBERS(ClassName)\
OdRxObjectPtr ClassName::pseudoConstructor() { OdRxObjectPtr pObj; DOCREATE(ClassName, pObj); return pObj; }\
\
OdRxObject* ClassName::queryX(const OdRxClass* pClass) const\
{ return ::odQueryXImpl<ClassName,ParentClass>(this, pClass); }\
\
void ClassName::rxInit()\
{\
  if (!ClassName::g_pDesc)\
    ClassName::g_pDesc = ::newOdRxClass(#ClassName, ParentClass::desc(), &pseudoConstructor);\
}\
\
void ClassName::rxUninit()\
{\
  ::deleteOdRxClass(g_pDesc);\
  g_pDesc = 0;\
}

#define ODRX_DXF_DEFINE_MEMBERS(ClassName,ParentClass,DOCREATE,DwgVer,MaintVer,nProxyFlags,DxfName,AppName)\
ODRX_DEFINE_MEMBERS(ClassName);\
OdRxObjectPtr ClassName::pseudoConstructor() { OdRxObjectPtr pObj; DOCREATE(ClassName, pObj); return pObj; }\
\
OdRxObject* ClassName::queryX(const OdRxClass* pClass) const\
{ return ::odQueryXImpl<ClassName,ParentClass>(this, pClass); }\
\
void ClassName::rxInit()\
{\
  if (!ClassName::g_pDesc)\
    ClassName::g_pDesc = ::newOdRxClass(#ClassName,ParentClass::desc(),&pseudoConstructor,\
      DwgVer,MaintVer,nProxyFlags,#DxfName,#AppName);\
}\
\
void ClassName::rxUninit()\
{\
  ::deleteOdRxClass(g_pDesc);\
  g_pDesc = 0;\
}


/** Description:

    {group:DD_Namespaces}
*/
namespace OdRx
{
  enum Ordering
  {
    kLessThan     =-1,
    kEqual        = 0,
    kGreaterThan  = 1,
    kNotOrderable = 2
  };
}


/** Description:

    {group:OdRx_Classes} 
*/
class OdRxObjectPtr
{
  OdRxObject*             m_pObject;

  /** Description:
      Increments the reference count of the object referenced by this smart pointer.
  */
  inline void internalAddRef();

  /** Description:
      Assigns the specifed object to this smart pointer.  
      
      Remarks:
      If this smart pointer is currently referencing another object, that object 
      is released prior to the assignment.  The reference count on the passed in 
      object is incremented by one.

      Arguments:
      pObj (I) Pointer to be assigned to this smart pointer.
  */
  inline void assign(const OdRxObject* pObj)
  {
    if (m_pObject != pObj)  // To prevent reference counter dec/inc which
    {                       // may cause object destruction or subClose
      release();
      m_pObject = (OdRxObject*)pObj;
      internalAddRef();
    }
  }

public:
  /** Description:
      Constructor.
  */
  inline OdRxObjectPtr() : m_pObject(0) { }

  /** Description:
      Constructor (const OdRxObject*, OdRxObjMod).  
      
      Remarks:
      Sets this smart pointer to reference the specified
      object, but DOES NOT increment the reference count of the specfied object.
  */
  inline OdRxObjectPtr(const OdRxObject* pObj, OdRxObjMod)
    : m_pObject(const_cast<OdRxObject*>(pObj))
  { }

  /** Description:
      Constructor (const OdRxObject*).  
      
      Remarks:
      Sets this smart pointer to reference the specified
      object, and increments the reference count of the specfied object.
  */
  inline OdRxObjectPtr(const OdRxObject* pObj) : m_pObject((OdRxObject*)pObj) { internalAddRef(); }

  /** Description:
      Constructor (const OdRxObjectPtr&).  
      
      Remarks:
      Sets this smart pointer to reference the specified
      object, and increments the reference count of the specfied object.
  */
  inline OdRxObjectPtr(const OdRxObjectPtr& pObj) : m_pObject((OdRxObject*)pObj.get()) { internalAddRef(); }

  /** Description:
      Releases the reference currently held by this smart pointer (if present), and 
      sets this smart pointer to reference the passed in object. 
      
      Remarks:
      Does not increment the reference count of the specified object.
  */
  inline void attach(const OdRxObject* pObj) { release(); m_pObject = (OdRxObject*)pObj; }

  /** Description:
      Destructor, decrements the reference count of the object referenced by this
      smart pointer.
  */
  inline ~OdRxObjectPtr() { release(); }

  /** Description:
      Decrements the reference count for the object referenced by this smart
      pointer, and gives up this smart pointer's reference to the object.
  */
  inline void release();

  /** Description:
      Returns a pointer to the object referenced by this smart pointer, and 
      gives up this smart pointer's reference to the object.   
      
      Remarks:
      The object's reference count is not modified.
  */
  inline OdRxObject* detach()
  {
    OdRxObject* res = m_pObject;
    m_pObject = NULL;
    return res;
  }

  /** Description:
      Assignment operator.
  */
  inline OdRxObjectPtr& operator = (const OdRxObjectPtr& pObj)
  { assign(pObj); return *this; }

  /** Description:
      Assignment operator.
  */
  inline OdRxObjectPtr& operator = (const OdRxObject* pObj)
  { assign(pObj); return *this; }

  /** Description:
      Returns a const pointer to the object referenced by this smart pointer.
  */
  inline const OdRxObject* get() const { return m_pObject; }

  /** Description:
      Returns a pointer to the object referenced by this smart pointer. 
      
      Remarks:
      This smart pointer object maintains its reference to the object, and the object's
      reference count is not modified.
  */
  inline OdRxObject* get() { return m_pObject; }

  /** Description:
      Returns a pointer to the object referenced by this smart pointer.
  */
  inline OdRxObject* operator ->() { return m_pObject; }

  /** Description:
      Returns a const pointer to the object referenced by this smart pointer.
  */
  inline const OdRxObject* operator ->() const { return m_pObject; }

#ifdef ODA_GCC_2_95
  /** Description:
      Returns a pointer to the object referenced by this smart pointer.

      Remarks:
      This non-standard form is used to eliminate a large number of compiler
      warnings produced by GCC 2.95.3 (GCC 3.X no longer produces these warnings).
  */
  inline operator OdRxObject*() const { return m_pObject; }
#else
  /** Description:
      Returns a pointer to the object referenced by this smart pointer.
  */
  inline operator OdRxObject*() { return m_pObject; }

  /** Description:
      Returns a const pointer to the object referenced by this smart pointer.
  */
  inline operator const OdRxObject*() const { return m_pObject; }
#endif

  /** Description:
      Equality operator.
  */
  inline bool operator==(const OdRxObject* p) const { return (m_pObject==p); }

  /** Description:
      Inequality operator.
  */
  inline bool operator!=(const OdRxObject* p) const { return (m_pObject!=p); }

  /** Description:
      Returns true if this smart pointer contains a null reference, false otherwise.
  */
  inline bool isNull() const { return m_pObject == 0; }
};


/** Description:
    Base class for all DWGdirect object that require runtime type identification.

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdRxObject
{
  /** Description:
      Copy constructor and assignment operator prohibited.
  */
  OdRxObject(const OdRxObject&);
  OdRxObject& operator = (const OdRxObject&);

protected:
  /** Description:
      Declares protected new/delete to avoid direct use of them in OdRxObject derived objects.
  */
  ODRX_HEAP_OPERATORS();

public:
  /** Description:
      Constructor (no arguments).
  */
  inline OdRxObject() { }

  /** Description:
      Returns a pointer this object, if this object can be converted to the passed in type, 
      otherwise returns a null pointer. 
      
      Remarks:
      User must call release on returned pointer if not null.
  */
  virtual OdRxObject* queryX(const OdRxClass* pClass) const;

  /** Description:
      Returns a pointer this object, if this object can be converted to the passed in type, 
      otherwise throws an exception.
      
      Remarks:
      User must call release on returned pointer if not null.
  */
  virtual OdRxObject* x(const OdRxClass* pClass) const;

  /** Description:
      Casts the specified pointer to a smart pointer and returns the smart pointer.
  */
  static inline OdRxObjectPtr cast(const OdRxObject* pObj)
  {
    OdRxObjectPtr pRes;
    if (pObj)
      pRes.attach(pObj->queryX(desc()));
    return pRes;
  }

  /** Description:
      Returns the a pointer to the OdRxClass object that contains a description
      of this object type, for use when the class type is already known.
  */
  static OdRxClass* desc();


  /** Description:
      Returns the a pointer to the OdRxClass object that contains a description
      of this object type, for use when the class type is not known.
  */
  virtual OdRxClass* isA() const;

  /** Description:
      Increments the reference count for this object by one.
  */
  virtual void addRef() = 0;

  /** Description:
      Decrements the reference count for this object by one, and if the reference count
      becomes zero this object is deleted.
  */
  virtual void release() = 0;

  /** Description:
      Returns reference count. OdRxObject::numRefs() returns 1 by default. 
  */
  virtual long numRefs() const;

  /** Description:
      Returns true if this object is an instance of the specified class type or a subclass
      of that type, false otherwise.
  */
  inline bool isKindOf(const OdRxClass* pClass) const
  {
    OdRxObjectPtr pRes;
    pRes.attach(queryX(pClass));
    return (!pRes.isNull());
  }

  /** Description:
      Creates a clone of this object, and returns a pointer to the clone.
  */
  virtual OdRxObjectPtr clone() const;

  virtual void copyFrom(const OdRxObject* pOtherObj);

  /** Description:
      Return true if this object is equal to the specified object, false otherwise.
  */
  virtual bool isEqualTo(const OdRxObject* pOtherObj) const;

  virtual OdRx::Ordering comparedTo(const OdRxObject* pOtherObj) const;

  /** Description:
      Destructor.
  */
  virtual ~OdRxObject();
};


void OdRxObjectPtr::internalAddRef()
{
  if(m_pObject) { m_pObject->addRef(); }
}

void OdRxObjectPtr::release()
{
  if (m_pObject)
  {
    m_pObject->release();
    m_pObject = NULL;
  }
}

#include "SmartPtr.h"

/** Description:
    Contains runtime information about OdRxObject or a subclass of OdRxObject.  
    
    Remarks:
    A pointer to an OdRxClass instance can be obtained for an OdRxObject class by calling 
    the OdRxObject::desc() function.

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdRxClass : public OdRxObject
{
public:
  /** Description:
      Constructor (no arguments).
  */
  OdRxClass() {}

  ODRX_DECLARE_MEMBERS(OdRxClass);
 
  /** Description:
      Adds the specified protocol extension object to this class, associated with the
      specified protocol extension class.
  */
  virtual OdRxObjectPtr addX(OdRxClass* pProtocolClass, OdRxObject* pProtocolObject);

  /** Description:
      Gets the protocol extension object associated with the specified protocol extension class,
      for this class.
  */
  virtual OdRxObjectPtr getX(const OdRxClass* pProtocolClass);

  /** Description:
      Deletes the protocol extension created by addX(), for the specified protocol extension class.
  */
  virtual OdRxObjectPtr delX(OdRxClass* pProtocolClass);
  
  /** Description:
      Creates a new instance of the C++ class that this OdRxClass object represents.

      Return Value:
      A smart pointer to the newly created object.
  */
  virtual OdRxObjectPtr create() const = 0;

  /** Description:
      Returns the application name associated with this class.
  */
  virtual const char* appName() const = 0;

  /** Description:
      Returns the DXF name for this class.  
  */
  virtual const char* dxfName() const = 0;

  /** Description:
      Returns the name of the C++ class that this object represents.
  */
  virtual const char* name() const = 0;

  /** Description:
      Returns the DWG version and maintenance version for this object.

      Arguments:
      pMaintVer (O) Receives the maintenance version.

      Return Value:
      The DWG version.
  */
  virtual OdDb::DwgVersion getClassVersion(OdDb::MaintReleaseVer* pMaintVer = NULL) const = 0;

  /** Description:
      Returns the proxy flags for this class.
  */
  virtual OdUInt32 proxyFlags() const = 0;
  
  /** Description:
      Returns true if this object represents a class dervied from the specified OdRxClass
      object, or if they represent the same class.
  */
  virtual bool isDerivedFrom(const OdRxClass*) const;

  /** Description:
      Returns a pointer to the OdRxClass that represents the parent of the class represented
      by this OdRxClass instance.
  */
  virtual OdRxClass* myParent() const = 0;

};

typedef OdSmartPtr<OdRxClass> OdRxClassPtr;



template <class Class, class Parent>
inline OdRxObject* odQueryXImpl(const Class* pThis, const OdRxClass* pClass)
{
  OdRxObject* pObj = 0;
  if(pClass == Class::desc())
  {
    pObj = (OdRxObject*)pThis;
    pObj->addRef();
  }
  else
  {
    pObj = Class::desc()->getX(pClass).detach();
    if(!pObj)
      pObj = pThis->Parent::queryX(pClass);
  }
  return pObj;
}

FIRSTDLL_EXPORT OdRxObjectPtr odrxCreateObject(const char* szClassName);

typedef OdRxObjectPtr (*OdPseudoConstructorType)();

FIRSTDLL_EXPORT OdRxClass* newOdRxClass(
  const char* szClassName,
  OdRxClass* pBaseClass,
  OdPseudoConstructorType pConstr = 0,
  int DwgVer = 0,
  int MaintVer = 0,
  int nProxyFlags = 0,
  const char* szDxfName = 0,
  const char* szAppName = 0
  );

FIRSTDLL_EXPORT void deleteOdRxClass(OdRxClass* pClass);

typedef OdArray<OdRxObjectPtr> OdRxObjectPtrArray;

//DD:EXPORT_OFF

#endif //_ODRXOBJECT_INCLUDED_


