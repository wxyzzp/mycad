#ifndef __DictionaryChangeH__
#define __DictionaryChangeH__

#include "binchange.h"

/*---------------------------------------------------------------------------*//**
@author Denis Petrov

@description This class is intended to work with changes in two dictionaries.
*//*--------------------------------------------------------------------------*/
class db_DictionaryChange: public db_BinChange
{
public:
	db_DictionaryChange(ChangeType type);
	virtual ~db_DictionaryChange();

	static bool CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
		void *oldval, void *newval, int len);

protected:

	virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem);
};

#endif
