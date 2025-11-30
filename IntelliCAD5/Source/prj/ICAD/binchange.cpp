/* File: <DevDir>\source\prj\icad\binchange.cpp
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 */ 

#include "Icad.h"/*DNT*/
#include "BinChange.h"
#include "xstd.h"

// *************************************************************************
// *************************************************************************
// *************************************************************************
// CLASS IMPLEMENATION 
//
// db_BinChange
//


// CTOR
//
db_BinChange::db_BinChange( db_BinChange::ChangeType type )
	{
	ASSERT( type != NONE );

	m_NodeType = type;
	m_pWhereData = NULL;
	m_pSavedData = NULL;
	m_iDataLength = 0;

	m_pNextNode = NULL;
	}


// DTOR
//
db_BinChange::~db_BinChange()
{
	switch(m_NodeType)
	{
	case DEFAULTBINARY	:
	case STRING			:
	case DBSTRING		:
		delete [] m_pSavedData;
		break;

	case BASIC_STRING	:
		delete m_pSavedData;
		break;
	case ALLOCATEDBINARY:
		delete [] m_pSavedData;	/*DG - 6.10.2001*/// We use 'new []' in CacheChangedBinaryData.
		break;

	case XDATA			:
		db_handitem::free_dup_eed(m_pSavedData);
		break;

	// Added Cybage VSB 14/01/2002 [
    // Reason: Fix for Bug No: 77962 from BugZilla, Debug Assert statement
	case SUBOBJECT :	
	if ( m_pSavedData != NULL )
		IC_FREE(m_pSavedData);
	break;
	// Added Cybage VSB 14/01/2002 ]

	default				:
		_ASSERTE(!"WARNING: Wrong db_BinChange object type!");
	}

	delete m_pNextNode;
}

// **********************
// PUBLIC METHOD -- class db_BinChange
//
//
bool
db_BinChange::IsValid( void )
	{
	if ( !CHECKSTRUCTPTR( this ) )
		{
		return false;
		}

	if ( m_NodeType == NONE )
		{
		return false;
		}
	else if ( m_NodeType == DEFAULTBINARY )
		{
		if ( !CHECKDATA( this->m_pSavedData, this->m_iDataLength ) )
			{
			return false;
			}
		}
	else if ( m_NodeType == STRING )
		{
		if ( !CHECKSTR( this->m_pSavedData ) )
			{
			return false;
			}
		}
	else if ( m_NodeType == XDATA )
		{
		// ???  Can be NULL or a void * to opaque data
		}

	else if ( m_NodeType == SUBOBJECT )
		{
		// ???  Can be NULL or a void * to opaque data
		}

	else
		{
		// Unimplemented type
		ASSERT( false );
		return false;
		}
	// Add more validity checks here

	return true;
	}

// **********************
// PUBLIC METHOD -- class db_BinChange
//
db_BinChange *
db_BinChange::GetNextLink( void )
	{
	return m_pNextNode;
	}


// **********************
// PUBLIC METHOD -- class db_BinChange
//
db_BinChange::ChangeType
db_BinChange::GetType( void )
	{
	return m_NodeType;
	}

// ***********************
// PUBLIC METHOD -- class db_BinChange
//
int
db_BinChange::GetLength( void )
	{
	ASSERT( this->IsValid() );
	ASSERT( this->GetType() == DEFAULTBINARY );

	return m_iDataLength;
	}


// ***********************
// PUBLIC METHOD -- class db_BinChange
//
//
bool 
db_BinChange::UndoChanges( db_drawing *pDrawing, db_handitem *pItem )
	{
	db_BinChange *pCurrent = NULL;

	for(pCurrent = this; pCurrent!=NULL; pCurrent=pCurrent->GetNextLink())
		{
		if( pCurrent->GetType() == DEFAULTBINARY ) 
			{ 
			ASSERT( pCurrent->GetLength() > 0 );

			void *pTempData = NULL;
			
			// Just a mem address.
			pTempData = new BYTE [ pCurrent->GetLength() ];
			::memcpy( pTempData, pCurrent->m_pWhereData, pCurrent->GetLength() );
			::memcpy( pCurrent->m_pWhereData, pCurrent->m_pSavedData, pCurrent->GetLength() );
			IC_FREE( pCurrent->m_pSavedData );
 			pCurrent->m_pSavedData=pTempData;
 			} 
		else if ( pCurrent->GetType() == ALLOCATEDBINARY ) 
		{
			void **ppTmp = (void**)pCurrent->m_pWhereData;
			void *pTmp = (void*)*ppTmp;
			*ppTmp = pCurrent->m_pSavedData;
			pCurrent->m_pSavedData = pTmp;
		}
		else if ( pCurrent->GetType() == BASIC_STRING ) 
		{
			icl::string tempString(*(icl::string*)pCurrent->m_pWhereData);
			icl::string currString(*(icl::string*)pCurrent->m_pSavedData);
			*(icl::string*)pCurrent->m_pWhereData = currString;
			*(icl::string*)pCurrent->m_pSavedData = tempString;
		}
		else if ( pCurrent->GetType() == STRING ) 
			{

			char *pTempString = NULL;
			char *pCurrentString = (char *)pCurrent->m_pSavedData;

			// Deal with strings.
			// Create a new allocated string.
			pTempString = new char [::strlen( pCurrentString )+1 ];
			::strcpy( pTempString, pCurrentString );

			// Realloc a saved string for the undo list.
			pCurrent->m_pSavedData = (void *)ic_realloc_char( (char *)pCurrent->m_pSavedData, ::strlen( *(char **)pCurrent->m_pWhereData)+1 );
			
			::strcpy( (char *)pCurrent->m_pSavedData, *(char **)pCurrent->m_pWhereData );

			// Free the old sting in the table and
			// point it to the string string created above.
			IC_FREE( *(void **)pCurrent->m_pWhereData );
			*(void **)pCurrent->m_pWhereData = pTempString;
			}
		else if ( pCurrent->GetType() == XDATA ) 
			{
			// Save off the current eed
			//
			void *pTempEED = pItem->dup_eed();

			// restore what we have saved
			//
			pItem->restore_dup_eed( pCurrent->m_pSavedData );


			// Stick the saved eed back into the list for later redo
			//
			pCurrent->m_pSavedData = pTempEED;
			}
		else if (pCurrent->GetType() == SUBOBJECT)
			{
			pCurrent->SwitchChanges(pDrawing, pItem);
			}
		else if (pCurrent->GetType() == DBSTRING) 
			{
			char* pTempString = NULL;
			char* pCurrentString = (char*)pCurrent->m_pSavedData;
			CDbString* pText = (CDbString*)pCurrent->m_pWhereData;
			// Deal with strings.
			// Create a new allocated string.
			pTempString = new char [ strlen(pCurrentString) + 1];
			strcpy(pTempString, pCurrentString);

			// Realloc a saved string for the undo list.
			pCurrent->m_pSavedData = (void *)ic_realloc_char((char *)pCurrent->m_pSavedData, pText->length() + 1);
			strcpy((char*)pCurrent->m_pSavedData, pText->c_str());

			// Free the old sting in the table and
			// point it to the string string created above.
			*pText = pTempString;
			IC_FREE(pTempString);
			}
		else
			{
			// No handler for this type yet....
			//
			ASSERT( FALSE );
			}
		}


	return true;
	}


// ***********************
// PUBLIC METHOD -- class db_BinChange
//
//
bool 
db_BinChange::RedoChanges( db_drawing *pDrawing, db_handitem *pItem )
	{
	db_BinChange *pCurrent = NULL;

	for( pCurrent = this; pCurrent != NULL; pCurrent = pCurrent->GetNextLink() ) 
		{
		if( pCurrent->GetType() == DEFAULTBINARY ) 
			{ 
			void *pTempData = NULL;
			// Just a mem address.
			pTempData = new BYTE [pCurrent->GetLength() ];
			::memcpy( pTempData, pCurrent->m_pWhereData, pCurrent->GetLength() );
			::memcpy( pCurrent->m_pWhereData, pCurrent->m_pSavedData, pCurrent->GetLength() );
			IC_FREE( pCurrent->m_pSavedData );
			pCurrent->m_pSavedData = pTempData;
			} 
		else if ( pCurrent->GetType() == ALLOCATEDBINARY ) 
		{
			void **ppTmp = (void**)pCurrent->m_pWhereData;
			void *pTmp = (void*)*ppTmp;
			*ppTmp = pCurrent->m_pSavedData;
			pCurrent->m_pSavedData = pTmp;
		}
		else if ( pCurrent->GetType() == BASIC_STRING ) 
		{
			icl::string tempString(*(icl::string*)pCurrent->m_pWhereData);
			icl::string currString(*(icl::string*)pCurrent->m_pSavedData);
			*(icl::string*)pCurrent->m_pWhereData = currString;
			*(icl::string*)pCurrent->m_pSavedData = tempString;
		}
		else if ( pCurrent->GetType() == STRING ) 
			{
			char *pTempString = NULL;
			char *pCurrentString = (char *)pCurrent->m_pSavedData;
			// Deal with strings.
			// Create a new allocated string.
			pTempString = new char [::strlen( pCurrentString )+1];
			::strcpy( pTempString, pCurrentString );

			// Realloc a saved string for the undo list.
			pCurrent->m_pSavedData = (void *)ic_realloc_char( (char *)pCurrent->m_pSavedData, ::strlen(*(char **)pCurrent->m_pWhereData)+1 );
			::strcpy((char *)pCurrent->m_pSavedData,*(char **)pCurrent->m_pWhereData );

			// Free the old sting in the table and
			// point it to the string string created above.
			IC_FREE(*(void **)pCurrent->m_pWhereData );
			*(void **)pCurrent->m_pWhereData = (void *)pTempString;
			}
		else if ( pCurrent->GetType() == XDATA ) 
			{
			// Save off the current eed
			//
			void *pTempEED = pItem->dup_eed();

			// restore what we have saved from the undo op
			//
			pItem->restore_dup_eed( m_pSavedData );

			// Stick the saved eed back into the list for later undo again
			//
			m_pSavedData = pTempEED;
			}
		else if (pCurrent->GetType() == SUBOBJECT)
			{
			pCurrent->SwitchChanges(pDrawing, pItem);
			}
		else if (pCurrent->GetType() == DBSTRING) 
			{
			char* pTempString = NULL;
			char* pCurrentString = (char*)pCurrent->m_pSavedData;
			CDbString* pText = (CDbString*)pCurrent->m_pWhereData;
			// Deal with strings.
			// Create a new allocated string.
			pTempString = new char [strlen(pCurrentString) + 1];
			strcpy(pTempString, pCurrentString);

			// Realloc a saved string for the undo list.
			pCurrent->m_pSavedData = (void *)ic_realloc_char((char *)pCurrent->m_pSavedData, pText->length() + 1);
			strcpy((char*)pCurrent->m_pSavedData, pText->c_str());

			// Free the old sting in the table and
			// point it to the string string created above.
			*pText = pTempString;
			IC_FREE(pTempString);
			}
		else
		{
			ASSERT(FALSE);
		}
		}

	return true;
	}

// **********************
// INTERNAL METHOD
//
bool db_BinChange::cacheDbStringChange(void* newval, void* oldval)
{
	ASSERT(this->GetType() == DBSTRING );

	const char* pString = ((CDbString*)oldval)->c_str();
	ASSERT(CHECKSTR(pString));

	m_pWhereData = newval;
	if(m_pSavedData)
		IC_FREE(m_pSavedData);
	m_pSavedData = new BYTE[ strlen(pString) + 1];
	strcpy((char*)m_pSavedData, pString);
	m_iDataLength = 0;

	return true;
	}

bool
db_BinChange::cacheStringChange( void *newval, void *oldval )
	{
	ASSERT( this->GetType() == STRING );

	char *pString = *((char **)oldval);

	ASSERT( CHECKSTR( pString ) );

	m_pWhereData = newval;
	if(m_pSavedData)
		IC_FREE(m_pSavedData);
	m_pSavedData = new BYTE[ ::strlen( pString )+1 ];
	::strcpy((char *)m_pSavedData, pString );
	m_iDataLength = 0;

	return true;
	}

// *************************
// INTERNAL METHOD -- class db_BinChange
//
bool
db_BinChange::cacheBinaryChange( void *newval, void *oldval, int len )
	{

	ASSERT( this->GetType() == DEFAULTBINARY );
	ASSERT( CHECKDATA( newval, abs(len) ) );

	m_pWhereData = newval;
	if(m_pSavedData)
		IC_FREE(m_pSavedData);
	m_pSavedData = new BYTE[ abs(len) ];
	::memcpy( m_pSavedData, oldval, abs(len) );
	m_iDataLength = abs(len);

	return true;
	}


// ***************************
// CLASS METHOD -- db_BinChange
//
// This method is a CLASS METHOD (aka static)
//
bool 
db_BinChange::AddNewLink( struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, struct db_BinChange *pNewNode ) 
	{
	if( pNewNode != NULL )
		{
		// Let's assume we're only adding one node at a time
		// (for now)
		//
		ASSERT( pNewNode->m_pNextNode == NULL );

		if( *pBeginList==NULL )
			{
			*pBeginList = *pCurrentNode = pNewNode;
			} 
		else 
			{
			(*pCurrentNode)->m_pNextNode = pNewNode;
			*pCurrentNode = (*pCurrentNode)->GetNextLink();

			// Current should now being pointing to the one we just added
			//
			ASSERT( *pCurrentNode == pNewNode );
			}
		}

	return true;
	}


// ***************************
// CLASS METHOD -- db_BinChange
//
// This method is a CLASS METHOD (aka static)
//
bool 
db_BinChange::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
							   icl::string* pOldValue, icl::string* pNewValue) 
{
	struct db_BinChange *pTempChangeNode = NULL;
	if (!pOldValue || !pNewValue) {
		return false;
	}
	if (strcmp(pOldValue->c_str(),pNewValue->c_str())) {
		pTempChangeNode = new db_BinChange( db_BinChange::BASIC_STRING );
		pTempChangeNode->m_pWhereData = pNewValue;
		pTempChangeNode->m_pSavedData = new icl::string(*pOldValue);
		pTempChangeNode->m_iDataLength = 0;
	} 

	if (pTempChangeNode != NULL ) {
		db_BinChange::AddNewLink( pBeginList, pCurrentNode, pTempChangeNode );
	}
	return true;
}

// ***************************
// CLASS METHOD -- db_BinChange
//
// This method is a CLASS METHOD (aka static)
//
bool 
db_BinChange::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, void *oldval, void *newval, int len, ChangeType type/* = NONE */) 
	{
	
	struct db_BinChange *pTempChangeNode = NULL;
	if(oldval==NULL || newval==NULL) 
		return false;

	if(type == DBSTRING)
	{
		if(((CDbString*)oldval)->compare((*((CDbString*)newval))) != 0)
		{
			pTempChangeNode = new db_BinChange(db_BinChange::DBSTRING);
			VERIFY(pTempChangeNode->cacheDbStringChange(newval, oldval));
		}
		if(pTempChangeNode != NULL)
			db_BinChange::AddNewLink(pBeginList, pCurrentNode, pTempChangeNode);
		return true;
	}
	else
		if(type != NONE)
		{
			ASSERT(FALSE);
			return false;
		}

	if(len==0) 
		{
		if((*(char **)oldval)==NULL || (*(char **)newval)==NULL) 
			{
			return false;
			}
		if(strcmp(*(char **)oldval,*(char **)newval)) 
		{
			pTempChangeNode = new db_BinChange( db_BinChange::STRING );
			VERIFY( pTempChangeNode->cacheStringChange( newval, oldval ) );
			}
		} 
	else 
		{
		if(len<0 || memcmp(oldval,newval,abs(len))) 
			{
			pTempChangeNode = new db_BinChange( db_BinChange::DEFAULTBINARY );

			VERIFY( pTempChangeNode->cacheBinaryChange( newval, oldval, len ) );
			}
		}

	if( pTempChangeNode != NULL )
		{
		db_BinChange::AddNewLink( pBeginList, pCurrentNode, pTempChangeNode );
		}
	return true;
	}

// Andy Graphov - 1st implementation (5.11.1999).
/*DG - 6.10.2001*/// 1. Use 'new[]' instead of 'malloc' for m_pSavedData because most of DB classes use 'new[]' and switching
// of m_pSavedData and m_pWhereData pointers in future undo/redo'ing will be correct for using of 'delete[]' in DB after all.
// 2. Process correctly null pointers and zero values of input parameters.
bool
db_BinChange::CacheChangedBinaryData
(
 db_BinChange**	ppBeginList,
 db_BinChange**	ppCurrentNode,
 void**			pOldval,
 void**			pNewval,
 int			oldLen
)
{
	if((!oldLen || !*pOldval) && !*pNewval)
		return false;

	db_BinChange*	pNewChangeNode = NULL;

	if(!oldLen || !*pOldval || !*pNewval || memcmp(*pOldval, *pNewval, oldLen))
	{
		pNewChangeNode = new db_BinChange(db_BinChange::ALLOCATEDBINARY);
		void*	pTmpData = (oldLen && *pOldval) ? new char [oldLen] : NULL;

		if(pTmpData || !oldLen || !*pOldval)
		{
			if(pTmpData)
				memcpy(pTmpData, *pOldval, oldLen);
			pNewChangeNode->m_pSavedData = pTmpData;
			pNewChangeNode->m_pWhereData = pNewval;
		}
		else
		{
			delete pNewChangeNode;
			pNewChangeNode = NULL;
		}
	}

	if(pNewChangeNode)
		db_BinChange::AddNewLink(ppBeginList, ppCurrentNode, pNewChangeNode);

	return true;
}

// ***************************
// CLASS METHOD -- db_BinChange
//
// This method is a CLASS METHOD (aka static)
//
bool 
db_BinChange::CacheChangedEED(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
							  db_handitem *pNewElement, db_handitem *pOldElement )
	{
	if ( !pNewElement->equal_eed( pOldElement ) )
		{
		db_BinChange *pNewNode = new db_BinChange( XDATA );

		void *pOldEED = pOldElement->dup_eed();

		pNewNode->m_pSavedData = pOldEED;

		db_BinChange::AddNewLink( pBeginList, pCurrentNode, pNewNode );

		}
	return true;
	}

//
// end of db_BinChange Implementation
// *************************************************************************
// *************************************************************************
// *************************************************************************


