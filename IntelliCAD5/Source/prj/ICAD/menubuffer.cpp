// F:\DEV\PRJ\ICAD\MENUBUFFER.CPP
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the menu queue
// 
//
#include "Icad.h" /*DNT*/




static struct resbuf *zMenuQueuePtr;
static struct resbuf *zMenuQueueEndPtr;



bool
IsMenuQueueEmpty( void )
	{
	return ( zMenuQueuePtr == NULL );
	}


class MenuQueueState 
	{
	public:
		struct resbuf *tempb;
		struct resbuf *tempc;
	};

MenuQueueState *
PushAndClearMenuQueue( void )
	{
	MenuQueueState *pRetval = new MenuQueueState;

	pRetval->tempb = zMenuQueuePtr;
	pRetval->tempc = zMenuQueueEndPtr;

	zMenuQueuePtr = zMenuQueueEndPtr = NULL;

	return pRetval;
	}

void
RestoreMenuQueue( MenuQueueState *pState )
	{
	ASSERT( CHECKSTRUCTPTR( pState ) );

	zMenuQueuePtr = pState->tempb;
	zMenuQueueEndPtr = pState->tempc;

	delete pState;
	}

void 
SDS_FreeFirstMenuBuf(void) 
	{
    struct sds_resbuf *trb;

    if(IsMenuQueueEmpty()) 
		{
		return;
		}

    trb=zMenuQueuePtr;
    zMenuQueuePtr=zMenuQueuePtr->rbnext;
    if(IsMenuQueueEmpty()) 
		{
		zMenuQueueEndPtr=NULL;
		}
    trb->rbnext=NULL;
    sds_relrb(trb);
	}

void
FlushMenuQueue( void )
	{
	while(!IsMenuQueueEmpty()) 
		{
		SDS_FreeFirstMenuBuf();
		}
	zMenuQueuePtr=zMenuQueueEndPtr=NULL;
	}



void
AddItemToMenuQueue( struct resbuf *pItem )
	{
	if(IsMenuQueueEmpty()) 
		{
		// Add to the beginning (all there is)
		//
		zMenuQueueEndPtr=zMenuQueuePtr=pItem;
		} 
	else 
		{
		// Add to the end of the current queue
		//
		zMenuQueueEndPtr=zMenuQueueEndPtr->rbnext=pItem;
		}

	if ( zMenuQueueEndPtr != NULL )
		{
		// make sure the end of the queue really is
		//
		while( zMenuQueueEndPtr->rbnext != NULL ) 
			{
			zMenuQueueEndPtr=zMenuQueueEndPtr->rbnext;
			}
		}
	}

struct resbuf *
PeekMenuQueueItem( void )
	{
	return zMenuQueuePtr;
	}

struct resbuf *
PopMenuQueueItem( void )
	{
	struct resbuf *pRetval = zMenuQueuePtr;
	if ( zMenuQueuePtr != NULL )
		{
		if ( zMenuQueueEndPtr == zMenuQueuePtr )
			{
			zMenuQueueEndPtr = zMenuQueuePtr->rbnext;
			}
		zMenuQueuePtr = zMenuQueuePtr->rbnext;
		}

	// Don't let the popped item continue to point into the queue
	//
	if ( pRetval != NULL )
		{
		pRetval->rbnext = NULL;
		}

	return pRetval;
	}

bool
NextMenuQueueItemMatches( const char *pStr )
	{
	bool bRetval = false;

	if ( zMenuQueuePtr != NULL )
		{
		if ( zMenuQueuePtr->restype == RTSTR ) 
			{
			if ( strisame( pStr, zMenuQueuePtr->resval.rstring ) )
				{
				bRetval = true;
				}
			}
		}

	return bRetval;
	}

bool
IsNextMenuQueueItemPause( void )
	{
	bool bRetval = false;
	if ( zMenuQueuePtr != NULL )
		{
		if ( zMenuQueuePtr->restype == RTSTR ) 
			{
			if ( zMenuQueuePtr->resval.rstring[0] == '\\' /*DNT*/ )
				{
				bRetval = true;
				}
			}
		}
	return bRetval;
	}

const char *
PeekMenuQueueString( void )
	{
	const char *pRetval = NULL;
	if ( zMenuQueuePtr != NULL )
		{
		if ( zMenuQueuePtr->restype == RTSTR ) 
			{
			pRetval = zMenuQueuePtr->resval.rstring;
			}
		}
	return pRetval;
	}

