// File  :
// Author: Alexey Malov
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "allocator.h"

namespace icl
{
void* allocate(size_t n)
{
	return new char[n];
}

void deallocate(void* p)
{
	if (p) delete [] p;
}
}

char * ic_realloc_char(char * psz, size_t nBytes)
{
	char * ptr = new char [nBytes];
	size_t nLength = strlen(psz) + 1;
	memcpy(ptr, psz, nLength < nBytes ? nLength : nBytes );
	delete [] psz;
	return ptr;
}