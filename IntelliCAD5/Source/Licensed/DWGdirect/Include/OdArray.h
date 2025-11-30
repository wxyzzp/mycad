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



#ifndef ODARRAY_H_INCLUDED
#define ODARRAY_H_INCLUDED

#include <new>

#include "DD_PackPush.h"

#include "OdArrayPreDef.h"
#include "OdHeap.h"

/** Description:

    {group:Other_Classes}
*/
template <class T> class OdMemoryAllocator
{
public:
  typedef unsigned int size_type;
  static inline void copy(T* pCopy, const T* pSource, size_type nCount)
  {
    memcpy(pCopy, pSource, nCount * sizeof(T));
  }
  static inline void move(T* pCopy, const T* pSource, size_type nCount)
  {
    memmove(pCopy, pSource, nCount * sizeof(T));
  }
  static inline void construct(T* pElement, const T& val = T())
  {
    *pElement = val;
  }
  static inline void constructn(T* pElements, size_type nCount, const T& val)
  {
    while(nCount--)
    {
      pElements[nCount] = val;
    }
  }
  static inline void constructn(T* pDest, const T* pSource, size_type nCount)
  {
    copy(pDest, pSource, nCount);
  }
  static inline void constructn(T* , size_type)
  {
    // DOES NOTHING
  }
  static inline void destroy(T*)
  {
    // DOES NOTHING
  }
  static inline void destroy(T*, size_type )
  {
    // DOES NOTHING
  }
};


/** Description:

    {group:Other_Classes}
*/
template <class T> class OdObjectsAllocator
{
public:
  typedef unsigned int size_type;
  static inline void copy(T* pDest, const T* pSource, size_type nCount)
  {
    while(nCount--)
    {
      *pDest = *pSource;
      pDest++;
      pSource++;
    }
  }
  static inline void move(T* pDest, const T* pSource, size_type nCount)
  {
    if (pDest <= pSource || pDest >= pSource + nCount)
    {
      copy(pDest, pSource, nCount);
    }
    else
    {
      while(nCount--)
      {
        pDest[nCount] = pSource[nCount];
      }
    }
  }
  static inline void construct(T* p)
  {
#ifdef new
#undef new
#endif
    ::new (p) T;
  }
  static inline void construct(T* p, const T& val)
  {
#ifdef new
#undef new
#endif
    ::new (p) T(val);
  }
  static inline void constructn(T* p, size_type nCount, const T& val)
  {
    while(nCount--)
    {
      construct(p+nCount, val);
    }
  }
  static inline void constructn(T* p, size_type nCount)
  {
    while(nCount--)
    {
      construct(p+nCount);
    }
  }
  static inline void constructn(T* pDest, const T* pSource, size_type nCount)
  {
    while(nCount--)
    {
      construct(pDest, *pSource);
      pDest++;
      pSource++;
    }
  }
  static inline void destroy(T* pElement)
  {
    pElement->~T();
    pElement = 0;
  }
  static inline void destroy(T* pElements, size_type nCount)
  {
    while(nCount--)
    {
      destroy(pElements + nCount);
    }
  }
};

/** Description:

    {group:Other_Classes}
*/
struct FIRSTDLL_EXPORT OdArrayBuffer
{
  typedef unsigned int size_type;

  mutable size_type m_nRefCounter;
  int               m_nGrowBy;
  size_type         m_nAllocated;
  size_type         m_nLength;

  static OdArrayBuffer g_empty_array_buffer;
};



/** Description:

    {group:Other_Classes}
*/
template <class T, class A> class OdArray
{
public:
  typedef typename A::size_type size_type;
  typedef T* iterator;
  typedef const T* const_iterator;
private:
  struct Buffer : OdArrayBuffer
  {
    T* data() const { return (T*)(this+1); }

    static Buffer* allocate(size_type nLength2Allocate, int nGrowBy)
    {
      size_type nBytes2Allocate = sizeof(Buffer) + nLength2Allocate * sizeof(T);
      if(nBytes2Allocate > nLength2Allocate)
      {
        Buffer* pBuffer = (Buffer*)::odrxAlloc(nBytes2Allocate);
        pBuffer->m_nRefCounter = 1;
        pBuffer->m_nGrowBy     = nGrowBy;
        pBuffer->m_nAllocated  = nLength2Allocate;
        pBuffer->m_nLength     = 0;
        return pBuffer;
      }
      ODA_FAIL(); // size_type overflow
      throw OdError(eOutOfMemory);
    }
    static Buffer* _default()
    {
      return (Buffer*)&g_empty_array_buffer;
    }
    void release()
    {
      ODA_ASSERT(m_nRefCounter);
      if((--m_nRefCounter)==0 && this != _default())
      {
        A::destroy(data(), m_nLength);
        ::odrxFree(this);
      }
    }
    void addref() const { ++m_nRefCounter; }
  };
  class reallocator
  {
    Buffer* m_pBuffer;
  public:
    inline reallocator() : m_pBuffer(Buffer::_default()) { m_pBuffer->addref(); }
    inline void reallocate(OdArray* pArray, size_type nNewLen)
    {
      if(!pArray->referenced())
      {
        if(nNewLen > pArray->physicalLength())
        {
          m_pBuffer->release();
          m_pBuffer = pArray->buffer();
          m_pBuffer->addref();
          pArray->copy_buffer(nNewLen);
        }
      }
      else
      {
        pArray->copy_buffer(nNewLen);
      }
    }
    inline ~reallocator() { m_pBuffer->release(); }
  };
  friend class reallocator;
  const_iterator begin_const() const { return begin(); }
  iterator begin_non_const() { return begin(); }
  const_iterator end_const() { return end(); }
  iterator end_non_const() { return end(); }
  void copy_before_write(size_type len)
  {
    if(referenced() || len > physicalLength())

    {
      copy_buffer(len);
    }
  }
  void copy_if_referenced() { if(referenced()) { copy_buffer(physicalLength()); } }
  void copy_buffer(size_type len)
  {
    Buffer* pOldBuffer = buffer();
    int nGrowBy = pOldBuffer->m_nGrowBy;
    size_type len2;
    if(nGrowBy > 0)
    {
      len2 = len + nGrowBy;
      len2 = ((--len2) / nGrowBy) * nGrowBy;
    }
    else
    {
      len2 = pOldBuffer->m_nLength;
      len2 = len2 + -nGrowBy * len2 / 100;
      if(len2 < len)
      {
        len2 = len;
      }
    }
    Buffer* pNewBuffer = Buffer::allocate(len2, nGrowBy);
    len = odmin(pOldBuffer->m_nLength, len);
    A::constructn(pNewBuffer->data(), pOldBuffer->data(), len);
    pNewBuffer->m_nLength = len;
    m_pData = pNewBuffer->data();
    pOldBuffer->release();
  }
  inline void assertValid(size_type index) const { if(!isValid(index)) { rise_error(eInvalidIndex); } }
  static inline void rise_error(OdResult e) { ODA_FAIL(); throw OdError(e); }
public:
  // STL-like interface
  iterator begin() { if(!empty()) { copy_if_referenced(); return data(); } return 0; }
  const_iterator begin() const { if(!empty()) { return data(); } return 0; }
  iterator end() { if(!empty()) { copy_if_referenced(); return data() + length(); } return 0; }
  const_iterator end() const { if(!empty()) { return data()+length(); } return 0; }
  void insert(iterator before, const_iterator first, const_iterator afterLast)
  {
    size_type len = length();
    size_type index = before - begin_const();
    if(index <= len && afterLast>=first)
    {
      if(afterLast > first)
      {
        size_type num2copy = afterLast - first;
        reallocator r;
        r.reallocate(this, len + num2copy);
        A::constructn(m_pData + len, first, num2copy);
        buffer()->m_nLength = len + num2copy;
        T* pDest = m_pData + index;
        if(index != len)
        {
          A::move(pDest + num2copy, pDest, len - index);
        }
        A::copy(pDest, first, afterLast - first);
      }
    }
    else
    {
      rise_error(eInvalidInput);
    }
  }
  void resize(size_type nNewSize, const T& val)
  {
    size_type len = length();
    int d = nNewSize - len;
    if(d>0)
    {
      reallocator r;
      r.reallocate(this, nNewSize);
      A::constructn(m_pData + len, d, val);
    }
    else if(d<0)
    {
      d=-d;
      if(!referenced())
      {
        A::destroy(m_pData + nNewSize, d);
      }
      else
      {
        copy_buffer(nNewSize);
      }
    }
    buffer()->m_nLength = nNewSize;
  }
  void resize(size_type nNewSize)
  {
    size_type len = length();
    int d = nNewSize - len;
    if(d>0)
    {
      copy_before_write(len+d);
      A::constructn(m_pData + len, d);
    }
    else if(d<0)
    {
      d=-d;
      if(!referenced())
      {
        A::destroy(m_pData + nNewSize, d);
      }
      else
      {
        copy_buffer(nNewSize);
      }
    }
    buffer()->m_nLength = nNewSize;
  }
  iterator insert(iterator before, size_type n2Insert, const T& val)
  {
    size_type len = length();
    size_type index = before - begin_const();
    reallocator r;
    r.reallocate(this, len + n2Insert);
    A::constructn(m_pData + len, n2Insert, val);
    buffer()->m_nLength = len + n2Insert;
    T* pData = data();
    pData += index;
    if(index != len)
    {
      A::move(pData + n2Insert, pData, len - index);
    }
    while(n2Insert--)
    {
      pData[n2Insert] = val;
    }
    return begin_non_const()+index;
  }
  size_type size() const { return buffer()->m_nLength; }
  bool empty() const { return size() == 0; }
  size_type capacity() const { return buffer()->m_nAllocated; }
  void reserve(size_type n) { if(physicalLength() < n) { setPhysicalLength(n); } }
	void assign(const_iterator first, const_iterator afterLast)
  { erase(begin_non_const(), end_non_const()); insert(begin_non_const(), first, afterLast); }
	iterator erase(iterator first, iterator afterLast)
  {
    size_type i = first - begin_const();
    if(first != afterLast)
    {
      removeSubArray(i, afterLast-begin_const()-1);
    }
    return begin_non_const()+i;
  }
  iterator erase(iterator _at)
  {
    size_type i = _at - begin_const();
    removeAt(i);
    return begin_non_const()+i;
  }
  void clear() { erase(begin_non_const(), end_non_const()); }
  void push_back(const T& val) { insert(end_non_const(), val); }
	iterator insert(iterator before, const T& val = T())
  {
    size_type index = before - begin_const();
		insertAt(index, val);
		return (begin_non_const() + index);
  }  

  // ARX-like interface
  bool contains(const T& value, size_type start) const
  { size_type dummy; return find(value, dummy, start); }
  
  size_type length() const { return buffer()->m_nLength; }
  
  bool isEmpty() const { return length() == 0; }
  
  size_type logicalLength() const { return length(); }
  
  size_type physicalLength() const { return buffer()->m_nAllocated; }
  
  int growLength() const { return buffer()->m_nGrowBy; }
  
  const T* asArrayPtr() const { return data(); }

  const T* getPtr() const { return data(); }

  T* asArrayPtr() { copy_if_referenced(); return data(); }
  
  const T& operator [](size_type i) const { assertValid(i); return m_pData[i]; }
  T& operator [](size_type i) { assertValid(i); copy_if_referenced(); return m_pData[i]; }

  T& at(size_type i) { assertValid(i); copy_if_referenced(); return *(data() + i); }
  const T& at(size_type i) const { assertValid(i); return *(data() + i); }
  
  OdArray& setAt(size_type i, const T& value)
  { assertValid(i); copy_if_referenced(); m_pData[i] = value; return *this; }
  const T& getAt(size_type i) const { assertValid(i); return *(data() + i); }
  
  T& first() { return *begin(); }
  const T& first() const { return *begin(); }
  
  T& last() { return at(length()-1); }
  const T& last() const { return at(length()-1); }
  
  size_type append(const T& value) { insertAt(length(), value); return length()-1; }
  
  iterator append() { size_type i = append(T()); return begin_non_const() + i; }
  
  OdArray& removeFirst() { return removeAt(0); }
  
  OdArray& removeLast() { return removeAt(length()-1); }
  
  OdArray& setGrowLength(int nGrowBy)
  {
    if(nGrowBy != 0)
    {
      copy_if_referenced();
      buffer()->m_nGrowBy = nGrowBy;
    }
    else
    {
      ODA_FAIL();
    }
    return *this;
  }

  explicit OdArray(size_type nPhysicalLength, int nGrowBy = 8) : m_pData(0)
  {
    if(nGrowBy != 0)
    {
      m_pData = Buffer::allocate(nPhysicalLength, nGrowBy)->data();
    }
    else
    {
      ODA_FAIL();
      *this = OdArray<T,A>();
    }
  }
  
  OdArray() : m_pData(Buffer::_default()->data()) { buffer()->addref(); }
  
  OdArray(const OdArray& src) : m_pData((T*)src.data()) { buffer()->addref(); }
  
  ~OdArray() { buffer()->release(); }
  
  OdArray& operator =(const OdArray& src)
  {
    src.buffer()->addref();
    buffer()->release();
    m_pData = src.m_pData;
    return *this;
  }
  
  bool operator ==(const OdArray& cpr) const
  {
    if(length() == cpr.length())
    {
      for(size_type i = 0; i < length(); i++)
      {
        if(at(i) != cpr[i])
        {
          return false;
        }
      }
      return true;
    }
    return false;
  }
  
  OdArray& setAll(const T& value)
  {
    copy_if_referenced();
    T* pData = data();
    size_type n = length();
    while(n)
    {
      pData[--n] = value;
    }
    return *this;
  }
  
  OdArray& append(const OdArray& otherArray)
  {
    insert(end_non_const(), otherArray.begin(), otherArray.end());
    return *this;
  }
  
  OdArray& insertAt(size_type index, const T& value)
  {
    size_type len = length();
    if(index == len)
    {
      resize(len+1, value);
    }
    else if(index < len)
    {
      reallocator r;
      r.reallocate(this, len+1);
      A::construct(m_pData + len);
      ++(buffer()->m_nLength);
      A::move(m_pData + index + 1, m_pData + index, len - index);
      m_pData[index] = value;
    }
    else
    {
      rise_error(eInvalidIndex);
    }
    return *this;
  }
  
  OdArray& removeAt(size_type index)
  {
    assertValid(index);
    size_type len = length();
    if(index < --len)
    {
      copy_if_referenced();
      T* pData = data();
      A::move(pData + index, pData + index + 1, len - index);
    }
    resize(len);
    return *this;
  }
  
  OdArray& removeSubArray(size_type startIndex, size_type endIndex)
  {
    if(!isValid(startIndex) || startIndex > endIndex)
    {
      rise_error(eInvalidIndex);
    }
    size_type len = length();
    copy_if_referenced();
    T* pData = data();
    ++endIndex;
    size_type n2remove = endIndex - startIndex;
    A::move(pData + startIndex, pData + endIndex, len - endIndex);
    A::destroy(pData + startIndex, n2remove);
    buffer()->m_nLength -= n2remove;
    return *this;
  }
  
  bool find(const T& value, size_type& index, size_type start) const
  {
    if(!empty())
    {
      assertValid(start);
      size_type len = length();
      const T* pData = data();
      for(size_type i = start; i<len; ++i)
      {
        if(pData[i] == value)
        {
          index = i;
          return true;
        }
      }
    }
    return false;
  }
  
  OdArray& setLogicalLength(size_type n)
  {
    resize(n);
    return *this;
  }
  
  OdArray& setPhysicalLength(size_type n)
  {
    if(n==0)
    {
      *this = OdArray<T, A>();
    }
    else if(n != physicalLength())
    {
      copy_buffer(n);
    }
    return *this;
  }
  
  OdArray& reverse()
  {
    if(!empty())
    {
      copy_if_referenced();
      T tmp;
      iterator iter1 = begin_non_const();
      iterator iter2 = end_non_const();
      --iter2;
      while(iter1 < iter2)
      {
        tmp = *iter1;
        *iter1 = *iter2;
        *iter2 = tmp;
        ++iter1;
        --iter2;
      }
    }
    return *this;
  }
  
  OdArray& swap(size_type i1, size_type i2)
  {
    if(!isValid(i1) || !isValid(i2))
    {
      rise_error(eInvalidIndex);
    }    
    if(i1 != i2)
    {
      const T tmp = at(i1);
      at(i1) = at(i2);
      at(i2) = tmp;
    }
    return *this;
  }
  
  bool remove(const T& value, size_type start = 0)
  {
    size_type i = 0;
    if(find(value, i, start))
    {
      removeAt(i);
      return true;
    }
    return false;
  }
private:

  T*    m_pData;

  bool isValid(size_type i) const { return (i < length()); }

  T* data() { return (length() ? m_pData : 0); }

  const T* data() const { return m_pData; }

  Buffer* buffer()
  {
    return (((Buffer*)m_pData) - 1);
  }
  const Buffer* buffer() const
  {
    return (const Buffer*)(((Buffer*)m_pData) - 1);
  }
  bool referenced() const
  {
    return (buffer()->m_nRefCounter>1);
  }
};

#include "DD_PackPop.h"

#endif // ODARRAY_H_INCLUDED



