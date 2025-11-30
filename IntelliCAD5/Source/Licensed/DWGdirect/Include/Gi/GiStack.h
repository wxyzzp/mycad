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



#ifndef ODGISTACK_H
#define ODGISTACK_H

template <class T> class OdGiStackItem;
template <class T> class OdGiStack;

/** Description:

    {group:OdGi_Classes} 
*/
template <class T>
class OdGiStackItem : public T
{
  friend class OdGiStack<T>;
protected:
  OdGiStackItem* m_pUnder;
  inline OdGiStackItem(OdGiStackItem* pUnder, const T& val)
    : T(val), m_pUnder(pUnder) { }
  inline OdGiStackItem(OdGiStackItem* pUnder) : m_pUnder(pUnder) { }
};

/** Description:

    {group:OdGi_Classes} 
*/
template <class T>
class OdGiStack
{
  typedef OdGiStackItem<T> TItem;
public:
  TItem* m_pTop;
  inline OdGiStack() : m_pTop(0) { }
  inline void push(const T& inVal)
  {
    m_pTop = new TItem(m_pTop, inVal);
  }
  inline T* push()
  {
    m_pTop = new TItem(m_pTop);
    return top();
  }
  inline void pop(T& outVal)
  {
    ODA_ASSERT(m_pTop); // pop from empty stack
    outVal = *m_pTop;
    pop();
  }

  inline const T* top() const { return m_pTop; }
  inline T* top() { return m_pTop; }

  inline void pop()
  {
    TItem* pTop = m_pTop;
    ODA_ASSERT(pTop); // pop from empty stack
    m_pTop = pTop->m_pUnder;
    delete pTop;
  }
  inline ~OdGiStack()
  {
    while(m_pTop)
    {
      pop();
    }
  }

  inline T* beforeTop() const 
  { 
    ODA_ASSERT(m_pTop);
    return m_pTop->m_pUnder;
  }
};

#endif //#ifndef ODGISTACK_H
