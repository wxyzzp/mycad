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



#ifndef _ODASMARTPOINTER_INCLUDED_
#define _ODASMARTPOINTER_INCLUDED_

#include "DD_PackPush.h"

class OdRxObject;

/** Description:
    Base class for DWGdirect smart pointers.

    {group:Other_Classes}
*/
class FIRSTDLL_EXPORT OdBaseObjectPtr
{
protected:
  inline  OdBaseObjectPtr() : m_pObject(NULL) {}
  
  OdRxObject* m_pObject;
  
public:
  /** Description:
      Returns the object referenced by this smart pointer object. This
      smart pointer object maintains its reference to the object, and the object's
      reference count is not modified.
  */
  inline OdRxObject* get() const {return m_pObject;}
  
  /** Description:
      Returns true if this smart pointer contains a null reference, false otherwise.
  */
  inline bool isNull() const { return m_pObject == 0; }
};

/** Description:
    DWGdirect smart pointer template class.

    {group:Other_Classes}
*/
template <class T> class OdSmartPtr : public OdBaseObjectPtr
{
  /** Description:
      Increments the reference count for the object referenced by this smart pointer.
  */
  inline void internalAddRef() { if(m_pObject) { ((T*)m_pObject)->addRef(); } }
  
  /** Description:
      Assigns the specifed object to this smart pointer.  If this smart pointer 
      is currently referencing another object, that object is released prior to 
      the assignment.  The reference count on the passed in object is incremented
      by one.

      Arguments:
      pObj (I) Pointer to be assigned to this smart pointer.
  */
  inline void assign(const T* pObj)
  {
    release();
    m_pObject = (OdRxObject*)pObj;
    internalAddRef();
  }
  
  /** Description:
      Performs a "safe" assignment of the passed in object to this smart pointer. If the 
      passed in object is of a compatible type, the assigment succeed, otherwise an 
      OdError(eNotThatKindOfClass) exception is thrown.
  */
  inline void internalQueryX(const OdRxObject* pObj)
  {
    if(pObj)
    {
      OdRxObject* pX = pObj->queryX(T::desc());
      if(pX)
        m_pObject = pX;
      else
        throw OdError(eNotThatKindOfClass);
    }
  }
  
  /** Description:
      Performs a "safe" assignement of the specifed object to this smart pointer.  
      If this smart pointer is currently referencing another object, that object 
      is released prior to the assignment.  The reference count on the passed in 
      object is incremented by one.

      Arguments:
      pObj (I) Pointer to be assigned to this smart pointer.
  */
  inline void assign(const OdRxObject* pObj)
  {
    release();
    internalQueryX(pObj);
  }
  
  // Note: Using of SmartPtr<T> as bool expression produce ambiguous call with some compilers.
  // Use isNull() method instead.

  /** Description:
      Declared private to prevent use. */
  bool operator !() const { ODA_FAIL(); return false; }
  
  /** Description:
      Declared private to prevent use. */
  operator bool() const { ODA_FAIL(); return false; }

  /** Description:
      Declared private to prevent use. */
  operator bool() { ODA_FAIL(); return false; }
  
public:
  /** Description:
      Constructor (no arguments).
  */
  inline OdSmartPtr() { }
  
  /** Description:
      Constructor (T*, OdRxObjMod).  Sets this smart pointer to reference the specified
      object, but DOES NOT increment the reference count of the specfied object.
  */
  inline OdSmartPtr(const T* pObj, OdRxObjMod) {m_pObject = (OdRxObject*)pObj; }
  
  /** Description:
      Constructor (T*).  Sets this smart pointer to reference the specified
      object, and increments the reference count of the specfied object.
  */
  inline OdSmartPtr(const T* pObj) {m_pObject = (OdRxObject*)pObj; internalAddRef(); }
  
  /** Description:
      Constructor (const OdRxObject*).  Sets this smart pointer to reference the specified
      object, and increments the reference count of the specfied object.
  */
  inline OdSmartPtr(const OdRxObject* pObj) { internalQueryX(pObj); }
  
  /** Description:
      Constructor (OdRxObject*, OdRxObjMod).  Sets this smart pointer to reference the specified
      object, but DOES NOT increment the reference count of the specfied object.
  */
  inline OdSmartPtr(OdRxObject* pObj, OdRxObjMod)
  {
    internalQueryX(pObj);
    if(pObj)
      pObj->release();
  }
  
  /** Description:
      Constructor (const OdSmartPtr&).  Sets this smart pointer to reference the specified
      object, and increments the reference count of the specfied object.
  */
  inline OdSmartPtr(const OdSmartPtr& pObj) {m_pObject = (OdRxObject*)pObj.get(); internalAddRef(); }

  /** Description:
      Constructor (const OdRxObjectPtr&).  Sets this smart pointer to reference the specified
      object, and increments the reference count of the specfied object.
  */
  inline OdSmartPtr(const OdRxObjectPtr& pObj) { internalQueryX(pObj.get()); }
  
  /** Description:
      Constructor (const OdBaseObjectPtr&).  Sets this smart pointer to reference the specified
      object, but DOES NOT increment the reference count of the specfied object.
  */
  inline OdSmartPtr(const OdBaseObjectPtr& pObj) { internalQueryX(pObj.get()); }

  /** Description:
      Releases the reference currently held by this smart pointer (if present), and 
      sets this smart pointer to reference the passed in object.  Does not increment the 
      reference count of the specified object.
  */
  inline void attach(const T* pObj) { release(); m_pObject = (OdRxObject*)pObj; }
  
  /** Description:
      Releases the reference currently held by this smart pointer (if present), and 
      sets this smart pointer to reference the passed in object.
  */
  inline void attach(OdRxObject* pObj)
  {
    release();
    internalQueryX(pObj);
    if(pObj)
      pObj->release();
  }
  
  /** Description:
      Destructor. Decrements the reference count of the object referenced by this
      smart pointer.
  */
  inline ~OdSmartPtr() { release(); }
  
  /** Description:
      Decrements the reference count for the object referenced by this smart
      pointer, and gives up this smart pointer's reference to the object.
  */
  inline void release()
  {
    if (m_pObject)
    {
      ((T*)m_pObject)->release();
      m_pObject = NULL;
    }
  }
  
  /** Description:
      Returns a pointer to the object referenced by this smart pointer, and 
      gives up this smart pointer's reference to the object.  The object's reference
      count is not modified.
  */
  inline T* detach()
  {
    T* res = (T*)m_pObject;
    m_pObject = NULL;
    return res;
  }
  
  /** Description:
      Assignment operator.
  */
  inline OdSmartPtr& operator = (const OdSmartPtr& pObj)
  { assign(pObj); return *this; }
    
  /** Description:
      Assignment operator.
  */
  inline OdSmartPtr& operator = (const OdBaseObjectPtr& Obj)
  { assign(Obj.get()); return *this; }
  
  /** Description:
      Assignment operator.
  */
  inline OdSmartPtr& operator = (const T* pObj)
  { assign(pObj); return *this; }
  
  /** Description:
      Returns a const pointer to the object referenced by this smart pointer.
  */
  inline const T* get() const { return (T*)m_pObject; }
  
  /** Description:
      Returns a pointer to the object referenced by this smart pointer.  This
      smart pointer object maintains its reference to the object, and the object's
      reference count is not modified.
  */
  inline T* get() { return (T*)m_pObject; }
  
  /** Description:
      Returns a pointer to the object referenced by this smart pointer.
  */
  inline T* operator ->() { return (T*)m_pObject; }
  
  /** Description:
      Returns a const pointer to the object referenced by this smart pointer.
  */
  inline const T* operator ->() const { return (T*)m_pObject; }
  
#ifdef ODA_GCC_2_95
  /** Description:
      Returns a pointer to the object referenced by this smart pointer.
      This non-standard form is used to eliminate compiler
      warnings produced by GCC 2.95.X (GCC 3.X no longer produces these warnings).
  */
  inline operator T*() const { return (T*)m_pObject; }
  
#else
  /** Description:
      Returns a pointer to the object referenced by this smart pointer.
  */
  inline operator T*() { return (T*)m_pObject; }
  
  /** Description:
      Returns a const pointer to the object referenced by this smart pointer.
  */
  inline operator const T*() const { return (T*)m_pObject; }
#endif
    
  /** Description:
      Equality operator.
  */
  inline bool operator==(const void* p) const { return (m_pObject==p); }

  /** Description:
      Equality operator.
  */
  inline bool operator==(const OdSmartPtr& ptr) const { return operator==((void*)ptr.get()); }
  
  /** Description:
      Inequality operator.
  */
  inline bool operator!=(const void* p) const { return (m_pObject!=p); }

  /** Description:
      Inequality operator.
  */
  inline bool operator!=(const OdSmartPtr& ptr) const { return operator!=((void*)ptr.get()); }
};

#include "DD_PackPop.h"

#endif //_ODASMARTPOINTER_INCLUDED_

