#ifndef __IdBufferChangeH__
#define __IdBufferChangeH__

#include "binchange.h"
class db_idbuffer;

/*---------------------------------------------------------------------------*//**
@author Chikahiro Masami

@description This class is intended to work with changes in two IDBUFFERs.
*//*--------------------------------------------------------------------------*/
class db_IdBufferChange: public db_BinChange
{
public:
	db_IdBufferChange(ChangeType type);
	virtual ~db_IdBufferChange();

	static bool CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode,
		void *oldval, void *newval, int len);

protected:

	virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem);
};

#endif
