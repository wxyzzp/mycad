// File  :
// Author: Alexey Malov
#pragma once
#ifndef _XSTD_H_
#define _XSTD_H_

#include "allocator.h"
#include <vector>
#include <string>

// containers, to use as parameters during Acis modeler calls
template <typename type>
struct xstd
{
    typedef std::vector< type, icl::allocator<type> > vector;
};

namespace icl
{
    typedef std::basic_string< char, std::char_traits<char>, icl::allocator<char> > string;
}

#endif
