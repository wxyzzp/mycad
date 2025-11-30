// **************************************
// VECT.CPP
//
// Implements a very lightweight dynamic array or vector collection class
//
//
//

#include "icadlib.h"

/*
 *  Construct new vector: initialize values
 */
ic_VectorCollection::ic_VectorCollection( unsigned int nGran )
	{
	ASSERT( nGran != 0 );

    this->m_nGran = nGran;
    this->m_nEntries = 0;
    this->m_nSize = nGran;
	
	this->m_pElements = new void *[ this->m_nGran ];
	ASSERT( this->m_pElements != NULL );
	}

/*
 *  Destroy a vector
 */
ic_VectorCollection::~ic_VectorCollection( void )
	{					 	
	IC_FREE( this->m_pElements );
	}

/* *************************************************************
 * PRIVATE METHOD -- class ic_VectorCollection
 *
 *
 */
bool 
ic_VectorCollection::changeSize( unsigned int wNewSize )
	{
	unsigned int wMemSize = wNewSize * sizeof( void * );

	//void **lpNew = (void **)::realloc( this->m_pElements, wMemSize );
	void **lpNew = new void * [wNewSize];
	ASSERT( lpNew != NULL );
	memcpy(lpNew,this->m_pElements,this->m_nSize * sizeof( void * ));
	delete [] this->m_pElements;
	if ( lpNew != NULL )
		{
		this->m_pElements = lpNew;
		this->m_nSize = wNewSize;
		}


    return( lpNew != NULL );
	}

/*
 *  Expand the vector to next multiple of granularity
 */
bool 
ic_VectorCollection::expand( )
	{
	unsigned int wNewSize = this->m_nSize + this->m_nGran;
	
	return ( this->changeSize( wNewSize ) );
	}

/*
 *  Compress the vector by copying down to element the given element
 */
void 
ic_VectorCollection::compress(  unsigned int nIndex )
	{
	if (nIndex == -1)
		{
		return;
		}
				
	void **lpElement = this->m_pElements + nIndex;

	unsigned int wEntriesAfterIndex = ( this->m_nEntries - ( nIndex + 1 ) );
    if ( wEntriesAfterIndex > 0)
	    {
		::memcpy( lpElement, lpElement + 1,
					 wEntriesAfterIndex * sizeof( void * ) );
		}
    this->m_nEntries--;

	// If we have decreased the size below a granularity chunk,
	// new size is the same as number of entries (since we normally
	// decrement the size one entry at a time, this will stay at
	// a granularity boundary)
	//
	if ( ( this->m_nEntries > 0 ) &&
		 ( this->m_nEntries <= ( this->m_nSize - this->m_nGran) ) )
		{
		// verify my assumption about granularity
		//
		ASSERT( (this->m_nEntries % this->m_nGran) == 0 );

		VERIFY( this->changeSize( this->m_nEntries ) );
		}
	}

/*
 *  Get an element from the vector
 */
void *
ic_VectorCollection::Get( unsigned int nItem )
	{		
	if ( nItem < this->m_nEntries )
		{
		return this->m_pElements[nItem];
		}		
	ASSERT( FALSE );
    return( NULL );
	}

/*
 *  Set an element in the vector
 */
void
ic_VectorCollection::Set( unsigned int nItem, void * lpItem )
	{
	if ( nItem < this->m_nEntries )
		{
		this->m_pElements[nItem] = lpItem;
		}
	}

/*
 *  Add an element to the end of a vector
 */
bool 
ic_VectorCollection::Add( void * lpItem )
	{
    if (this->m_nEntries == this->m_nSize)
		{
		if (!this->expand( ))
			{
	    	return( FALSE );
			}
		}

	this->m_pElements[ this->m_nEntries ] = lpItem;
	this->m_nEntries = this->m_nEntries+1;

    return( TRUE );
	}



