// COMMANDQUEUE.CPP
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the command queue class
// 
//
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
#include "commandqueue.h" /*DNT*/

// Random number used to make sure the object is valid
//
const int CommandQueue::iClassCheckVal = 798;

CommandQueue::CommandQueue( void )
	{

	this->m_iCheckVal = CommandQueue::iClassCheckVal;

	this->m_pStartPtr = NULL;
	this->m_pEndPtr = NULL;

	}

CommandQueue::~CommandQueue( void )
	{
	this->Flush();
	this->m_iCheckVal = 0;
	}

bool
CommandQueue::isValid( void )
	{
	SYNCHRONIZEMETHOD();
	if ( !BaseLockableObject::isValid() )
		{
		return false;
		}

	if ( !CHECKSTRUCTPTR( this ) )
		{
		return false;
		}

	if ( this->m_iCheckVal != CommandQueue::iClassCheckVal )
		{
		return false;
		}

	if ( this->m_pStartPtr != NULL )
		{
		if ( !CHECKSTRUCTPTR( this->m_pStartPtr ) )
			{
			return false;
			}
		if ( this->m_pEndPtr == NULL )
			{
			return false;
			}
		if ( this->m_pEndPtr->rbnext != NULL )
			{
			return false;
			}
		}
	else
		{
		if ( this->m_pEndPtr != NULL )
			{
			return false;
			}
		}

	return true;

	}


void
CommandQueue::Flush( void )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	if ( this->m_pStartPtr != NULL )
		{
		sds_relrb( this->m_pStartPtr );
		}
	this->m_pStartPtr = NULL;
	this->m_pEndPtr = NULL;

	ASSERT( this->IsValid() );
	}

void 
CommandQueue::FreeNext(void) 
	{
	SYNCHRONIZEMETHOD();

	ASSERT( this->IsValid() );

    if(this->IsEmpty()) 
		{
		return;
		}

    struct sds_resbuf *trb;

    trb=this->m_pStartPtr;
    this->m_pStartPtr=this->m_pStartPtr->rbnext;

    if(this->IsEmpty()) 
		{
		this->m_pEndPtr=NULL;
		}

    trb->rbnext=NULL;
    ::sds_relrb(trb);

	ASSERT( this->IsValid() );
	}

resbuf *
CommandQueue::AdvanceNext( void )
	{
	SYNCHRONIZEMETHOD();

	this->FreeNext();

	return this->PeekItem();
	}

void
CommandQueue::AddItem( struct resbuf *pItem )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	if(this->IsEmpty()) 
		{
		// Add to the beginning (all there is)
		//
		this->m_pEndPtr = this->m_pStartPtr = pItem;
		} 
	else 
		{
		// Add to the end of the current queue
		//
		this->m_pEndPtr = this->m_pEndPtr->rbnext = pItem;
		}

	if ( this->m_pEndPtr != NULL )
		{
		// make sure the end of the queue really is
		//
		while( this->m_pEndPtr->rbnext != NULL ) 
			{
			this->m_pEndPtr = this->m_pEndPtr->rbnext;
			}
		}

	ASSERT( this->IsValid() );
	}

void
CommandQueue::AddItemToFront( struct resbuf *pItem )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( pItem != NULL ); 
	ASSERT( pItem->rbnext == NULL );
	ASSERT( this->IsValid() );

	pItem->rbnext = this->m_pStartPtr;
	this->m_pStartPtr = pItem;
	if ( this->m_pEndPtr == NULL )
		{
		this->m_pEndPtr = this->m_pStartPtr;
		}

	ASSERT( this->IsValid() );
	}

void
CommandQueue::Copy( CommandQueue *pOther )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	ASSERT( pOther->IsValid() );

	this->Flush();

	this->Concat( pOther );

	ASSERT( this->IsValid() );
	}

void
CommandQueue::Concat( CommandQueue *pOther )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	ASSERT( pOther != this );  // maybe we should allow to concat to itself, but....
	ASSERT( pOther->IsValid() );

	if ( pOther->PeekItem() != NULL )
		{
		this->AddItem( ::SDS_ResBufCopy( pOther->PeekItem() ) );
		}

	ASSERT( this->IsValid() );
	}

struct resbuf *
CommandQueue::PopItem( void )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	struct resbuf *pRetval = this->m_pStartPtr;
	if ( this->m_pStartPtr != NULL )
		{
		if ( this->m_pEndPtr == this->m_pStartPtr )
			{
			this->m_pEndPtr = this->m_pStartPtr->rbnext;
			}
		this->m_pStartPtr = this->m_pStartPtr->rbnext;
		}

	// Don't let the popped item continue to point into the queue
	//
	if ( pRetval != NULL )
		{
		pRetval->rbnext = NULL;
		}

	ASSERT( this->IsValid() );
	return pRetval;
	}

bool
CommandQueue::IsCommandPause( void )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	bool bRetval = false;
	if ( this->m_pStartPtr != NULL )
		{
		if ( this->m_pStartPtr->restype == RTSTR ) 
			{
			if ( this->m_pStartPtr->resval.rstring[0] == '\\' /*DNT*/ )
				{
				bRetval = true;
				}
			}
		}

	ASSERT( this->IsValid() );
	return bRetval;
	}

const char *
CommandQueue::PeekString( void )
	{
	SYNCHRONIZEMETHOD();
	ASSERT( this->IsValid() );

	const char *pRetval = NULL;
	if ( this->m_pStartPtr != NULL )
		{
		if ( this->m_pStartPtr->restype == RTSTR ) 
			{
			pRetval = this->m_pStartPtr->resval.rstring;
			}
		}

	ASSERT( this->IsValid() );
	return pRetval;
	}

