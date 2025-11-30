#ifndef __GroupChangeH__
#define __GroupChangeH__

#include "binchange.h"
class CDbGroup;

/*---------------------------------------------------------------------------*//**
@author Denis Petrov

@description This class is intended to work with changes in two groups.
*//*--------------------------------------------------------------------------*/
class db_GroupChange: public db_BinChange
{
public:
	db_GroupChange(ChangeType type);
	virtual ~db_GroupChange();

	static bool CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
		void *oldval, void *newval, int len);

protected:

	virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem);
};

class db_LeaderChange: public db_BinChange
{
public:
	db_LeaderChange(ChangeType type): db_BinChange(type){}
	virtual ~db_LeaderChange();

	static bool CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
		void *oldval, void *newval, int len);

protected:

	virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem);
};

#endif