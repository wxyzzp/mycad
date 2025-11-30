// File  :
// Author: Alexey Malov
#pragma once
#ifndef _NTYPE_H_
#define _NTYPE_H_

#include <assert.h>

namespace icl
{
    /*-------------------------------------------------------------------------*//**
    Array of objects in auto memory.
    *//*--------------------------------------------------------------------------*/
    template <typename type, int n>
    struct ntype
    {
        // data
        type m_a[n];

        // ctors
        ntype()
        {}
        ntype(const type an[n])
        {
            for (int i = n; i--;)
                m_a[i] = an[i];
        }
        ntype(const ntype& an)
        {
            for (int i = n; i--;)
                m_a[i] = an[i];
        }
        // accessors
        type& operator [] (const int i)
        {
            assert(i>=0 && i<n);
            return m_a[i];
        }
        const type& operator [] (const int i) const
        {
            assert(i>=0 && i<4);
            return m_a[i];
        }
		// query
		int size() const
		{
			return n;
		}
    };

}


#endif
