#include "stdAfx.h"

#include "hiparray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CHipArray::Clear()
{
	int idx;
	int count = GetSize();
	for( idx = 0; idx < count; idx++ )
	{
		db_handitem *pTmpHip = GetAt( idx );
		delete pTmpHip;
	}
	RemoveAll();
}
