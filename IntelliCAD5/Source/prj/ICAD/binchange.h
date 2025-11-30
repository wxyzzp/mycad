/* File: <DevDir>\source\prj\icad\binchange.h
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 */ 

#ifndef	__BinChange_H_
#define	__BinChange_H_

#include "xstd.h"

// ****************************
// struct db_BinChange
// 
// TODO:  Should this should really be a class hierarchy with virtual methods??
// Because of the static methods it seems a little tricky, so I'm punting...
// If you do decide to switch to virtual methods, be sure to virtualize the destructor
// UndoChanges, RedoChanges...

struct db_BinChange
{
	// ********
	// INTERNAL DATA TYPES
	//
	public:
		enum ChangeType
			{
			NONE = 0,
			DEFAULTBINARY,
			ALLOCATEDBINARY,
			STRING,
			DBSTRING,
			XDATA,
			SUBOBJECT,
			BASIC_STRING
			};

	// *************
	// PUBLIC CLASS METHODS
	//
	// 
	static bool CacheChangedData(db_BinChange** pBeginList,
								db_BinChange** pCurrentNode,
								void* oldval,
								void* newval,
								int len,
								ChangeType type = NONE);
	static bool CacheChangedData(struct db_BinChange **pBeginList, 
								 struct db_BinChange **pCurrentNode, 
							     icl::string* oldValue, 
								 icl::string* newValue);
	static bool CacheChangedBinaryData(db_BinChange** ppBeginList,
								db_BinChange** ppCurrentNode,
								void** ppOldval,
								void** ppNewval,
								int oldLen);
	static bool CacheChangedEED(db_BinChange** pBeginList,
								db_BinChange** pCurrentNode,
								db_handitem* pNewElement,
								db_handitem* pOldElement);
	static bool AddNewLink(db_BinChange** pBeginList,
								db_BinChange** pCurrentNode,
								db_BinChange* pNewNode);


	// ********
	// CTORS AND DTORS
	public:
		db_BinChange(ChangeType type);
		virtual ~db_BinChange();

	// *********
	// Debugging and validity methods
	//
	public:
		bool IsValid();

	// ********
	// PUBLIC OPERATIONS
	//
	public:
		virtual bool UndoChanges(db_drawing* pDrawing, db_handitem* pItem);
		virtual bool RedoChanges(db_drawing* pDrawing, db_handitem* pItem);
		db_BinChange* GetNextLink();
		ChangeType	GetType();
		int			GetLength();

	// *********
	// internal operations
	//
	protected:
		bool		cacheBinaryChange(void* newval, void* oldval, int len);
		bool		cacheStringChange(void* newval, void* oldval);
		bool		cacheDbStringChange(void* newval, void* oldval);


		// subobjects should implement so that the subobjects can handle their own changes
		virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem) {return false;}

	// *********
	// INTERNAL DATA
	//
	protected:
		ChangeType	m_NodeType;
		void*		m_pWhereData;
		void*		m_pSavedData;
		int			m_iDataLength;

		db_BinChange*	m_pNextNode;
};

#endif // __BinChange_H_
