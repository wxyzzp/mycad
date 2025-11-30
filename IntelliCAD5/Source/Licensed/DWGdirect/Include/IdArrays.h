#ifndef _IDARRAY_H_
#define _IDARRAY_H_

#include "OdArray.h"
#include "DbTypedId.h"

typedef OdArray<OdDbSoftPointerId> OdSoftPtrIdArray;
typedef OdArray<OdDbHardPointerId> OdHardPtrIdArray;
typedef OdArray<OdDbSoftOwnershipId> OdSoftOwnIdArray;
typedef OdArray<OdDbHardOwnershipId> OdHardOwnIdArray;
typedef OdArray<OdDbObjectId>       OdDbObjectIdArray;

typedef OdArray<OdDbTypedId> OdTypedIdsArray;


#endif //_IDARRAY_H_

