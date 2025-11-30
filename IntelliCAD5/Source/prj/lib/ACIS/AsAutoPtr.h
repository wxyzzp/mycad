// File  :
// Author: Alexey Malov
#pragma once
#ifndef _ASAUTOPTR_H_
#define _ASAUTOPTR_H_

#include "AsTools.h" //checkOutcome()
#include <vector>
#include <utility>

/*-------------------------------------------------------------------------*//**
Default class for destroying object under CAutoPtr
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template <typename _T>
class TDelete
{
public:
    static void destroy(_T* p)
    {
        delete p;
    }
};

/*-------------------------------------------------------------------------*//**
Destroyer of arrays under CAutoPtr.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template <typename _T>
class TDeleteArray
{
public:
    static void destroy(_T* p)
    {
        delete [] p;
    }
};

/*-------------------------------------------------------------------------*//**
Destroyer for Acis objects.
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
class CDeleteEntity
{
public:
    static void destroy(ENTITY* p)
    {
        if (p)
            _checkOutcome(api_del_entity(p));
    }
	void operator () (ENTITY* p)
	{
		if (p)
			api_del_entity(p);
	}
};

template <typename _T>
class TLose
{
public:
	static void destroy(_T* p)
	{
		p->lose();
	}
};

// Destroyer for FILE.
class CCloseFile
{
public:
    static void destroy(FILE* p)
    {
        fclose(p);
    }
};

/*-------------------------------------------------------------------------*//**
Smart pointer with parametrized method of underlying object destroying.
Destroyer is second template's parameter, it has to have satic method destroy().
Default destroyer just calls operator delete.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template < typename _T, typename _D = TDelete<_T> >
class TAutoPtr
{
public:
	typedef _T type;
    typedef _D free;

	TAutoPtr(type* p = 0)
    :
    m_bHolder(p != 0),
    m_ptr(p)
    {}

	TAutoPtr(const TAutoPtr<_T,_D>& y)
    :
    m_bHolder(y.m_bHolder),
    m_ptr(y.release())
    {}

    const TAutoPtr<type,free>& operator = (const TAutoPtr<type,free>& y)
    {
        if (this != &y)
        {
            if (m_ptr != y.get())
            {
                if (m_bHolder)
                    free::destroy(m_ptr);
				m_bHolder = y.m_bHolder;
            }
			else if (y.m_bHolder)
				m_bHolder = true;
			m_ptr = y.release();
        }
		return (*this);
    }

	const TAutoPtr<type, free>& operator = (type* ptr)
	{
		if (m_ptr != ptr)
		{
			if (m_bHolder)
				free::destroy(m_ptr);
		}
		m_bHolder = true;
		m_ptr = ptr;

		return *this;
	}

    virtual ~TAutoPtr()
    {
        if (m_bHolder)
            free::destroy(m_ptr);
    }

    type& operator*() const
    {
        return *get();
    }
	type* operator->() const
    {
        return get();
    }
	type*& init()
	{
		if (m_bHolder)
			free::destroy(m_ptr);
		m_bHolder = true;
		return m_ptr;
	}
	type* get() const
	{
        return m_ptr;
    }
	type* release() const
	{
        m_bHolder = false;
		return m_ptr;
    }
    operator type* () const
    {
        return get();
    }

protected:

	mutable bool m_bHolder;
	type* m_ptr;
};
// Smart pointers for ENTITY and BODY objects.
typedef TAutoPtr<ENTITY, CDeleteEntity> CEntityPtr;
typedef TAutoPtr<BODY, CDeleteEntity> CBodyPtr;
typedef TAutoPtr<EDGE, CDeleteEntity> CEdgePtr;
typedef TAutoPtr<CBodyPtr, TDeleteArray<CBodyPtr> > CBodyArray;
// Smart pointer for FILE.
typedef TAutoPtr<FILE, CCloseFile> CFilePtr;
// Smart pointer for char
typedef TAutoPtr<char> CCharPtr;

template <typename _T, typename _D>
class THolder: public TAutoPtr<_T, _D>
{
public:
    THolder(type* p = 0)
    :TAutoPtr<_T, _D>(p)
    {}
	THolder(const THolder<_T,_D>& y)
    :TAutoPtr<_T, _D>(y)
    {}

    void set(type* p)
    {
        if (p != m_ptr)
		{
			if (m_ptr && m_bHolder)
				free::destroy(m_ptr);
			m_ptr = p;
		}
		m_bHolder = true;
    }
};
// Holders
typedef THolder<ENTITY, CDeleteEntity> CEntityHolder;
typedef THolder<BODY, CDeleteEntity> CBodyHolder;
typedef THolder<EDGE, CDeleteEntity> CEdgeHolder;

template<typename _Ty>
class TAsArrayBackInsertIterator: public std::iterator<std::output_iterator_tag, void, void>
{
public:
	explicit TAsArrayBackInsertIterator(_Ty* Arr)
	:_Arr(Arr) {}

	TAsArrayBackInsertIterator<_Ty>& operator=
	(
	const _Ty& _V
	)
	{
		*(_Arr++) = _V;
		return *this;
	}

	TAsArrayBackInsertIterator<_Ty>& operator*()
	{
		return *this;
	}

	TAsArrayBackInsertIterator<_Ty>& operator++()
	{
		return *this;
	}

	TAsArrayBackInsertIterator<_Ty> operator++(int)
	{
		return *this;
	}

protected:
	_Ty* _Arr;
};

template <typename _Ty>
TAsArrayBackInsertIterator<_Ty> array_back_inserter(_Ty* Arr)
{
	return TAsArrayBackInsertIterator<_Ty>(Arr);
}

template <typename _Ty, typename _De>
class TAsDeletePtrVector
{
	const std::vector<_Ty*>& _Arr;
	bool _Own;
	_De _D;
public:
	TAsDeletePtrVector(const std::vector<_Ty*>& Arr, _De D):
	  _Arr(Arr),_Own(true), _D(D){}

	virtual ~TAsDeletePtrVector()
	{if (_Own)
		for_each(_Arr.begin(),_Arr.end(), _D);}

	void release()
	{_Own = false;}
};

template <typename _Ty>
class TAsDeleteEntityVector: public TAsDeletePtrVector<_Ty, CDeleteEntity>
{
public:
	TAsDeleteEntityVector(const std::vector<_Ty*>& Arr):
	  TAsDeletePtrVector<_Ty, CDeleteEntity>(Arr, CDeleteEntity())
	  {}
};

typedef TAsDeleteEntityVector<BODY> CBodyArrayHolder;
typedef TAsDeleteEntityVector<EDGE> CEdgeArrayHolder;

template <typename _Ty>
class vector_handler
{
protected:
	std::vector<_Ty>& _vec;
	bool _ex;
public:
	vector_handler(std::vector<_Ty>& vec):
	  _vec(vec),_ex(true){}
	void release()
	{_ex = false;}
};

template <typename _Ent>
void attach
(
std::vector<THolder<_Ent, CDeleteEntity> >& holders,
ENTITY_LIST& ents
)
{
    holders.resize(ents.count());
	int i = 0;
	ENTITY* e = 0;
    for (ents.init(); e = ents.next();)
        holders[i++].set(static_cast<_Ent*>(e));
}

template <typename _Ent>
void attach
(
std::vector<THolder<_Ent, CDeleteEntity> >& holders,
_Ent** first,
_Ent** last
)
{
	int n = abs(first-last);
	holders.resize(n);
	for (int i = 0;first != last; ++first)
		holders[i++].set(*first);
}

template <typename _Ent>
void detach
(
std::vector<THolder<_Ent, CDeleteEntity> >& holders
)
{
    for (int i = 0, n = holders.size(); i < n; ++i)
        holders[i].release();
}

#endif
