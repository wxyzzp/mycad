// File  : 
// Author: Alexey Malov
#pragma once

#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <new>

namespace icl
{

void* allocate(size_t);
void deallocate(void*);

template <typename type1, typename type2>
inline
void construct(type1* p, const type2& v)
{
    new ((void*)p) type1(v); 
}

template <typename type> 
inline
void destroy(type* p)
{
    p->~type();
}

// TEMPLATE CLASS allocator
template <typename type>
class allocator 
{
public:
	typedef size_t size_type;
	typedef size_t difference_type;
	typedef type* pointer;
	typedef const type* const_pointer;
	typedef type& reference;
	typedef const type& const_reference;
	typedef type value_type;

    pointer address(reference o) const
    {
        return (&o); 
    }
	const_pointer address(const_reference o) const
	{
        return (&o);
    }
	pointer allocate(size_type n, const void *)
	{
        return ((type*)icl::allocate(n*sizeof(type)));
    }
	char* _Charalloc(size_type n)
	{
        return ((pointer)icl::allocate(n*sizeof(char)));
    }
	void deallocate(void* p, size_t)
	{
        icl::deallocate(p);
    }
	void construct(pointer p, const_reference v)
	{
        icl::construct(p, v); 
    }
	void destroy(pointer p)
	{
        icl::destroy(p);
    }
	size_t max_size() const
	{
        size_t n = (size_t)(-1) / sizeof(type);
		
        return n > 0? n: 1;
    }
};

template <typename type1, typename type2> 
inline
bool operator == (const icl::allocator<type1>&, const icl::allocator<type2>&)
{
    return (true); 
}

template<typename type1, typename type2>
inline
bool operator != (const icl::allocator<type1>&, const icl::allocator<type2>&)
{
    return (false); 
}

// empty specializations for template fuction destroy()
inline void destroy(char*){}
inline void destroy(wchar_t*){}
class point;
inline void destroy(point*){}
class gvector;
inline void destroy(gvector*){}

}//namespace icl

#endif
