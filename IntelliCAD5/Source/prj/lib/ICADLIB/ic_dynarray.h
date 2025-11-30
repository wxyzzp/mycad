// IC_DYNARRAY.H
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	A queue template of general usefulness
// 
#ifndef _IC_DYNARRAY_H
#define _IC_DYNARRAY_H


// see matching undef at the end
//
#ifndef ASSERT
	#define IC_DYNARRAY_H_ASSERT	
	#define ASSERT( a )			assert( (a) )
#endif

/* ***********************************************************
 * CLASS DECLARATION
 *
 * ic_dynarray
 *
 * PURPOSE
 *	This is a basic class for maintaining collections of objects
 * This class does not deal with the allocation or
 * deallocation of the objects themselves.
 *
 */
template <class elementType>
class ic_dynarray
	{	
	
	/* ***********************************************
	 * PUBLIC CONSTRUCTOR METHODS
	 * ***********************************************/
	public:
		// Constructor with initialization of granularity. Default
		// of 10 seems as reasonable as any
		//
		ic_dynarray( unsigned int nGran = 10 )
			{
			ASSERT( nGran != 0 );

		    m_nGran = nGran;
		    m_nEntries = 0;
		    m_nSize = nGran;

			m_pElements = (elementType *)::malloc( m_nGran * sizeof( elementType ) );
			ASSERT( m_pElements != NULL );
			}

		~ic_dynarray( void )
			{
			::free( m_pElements );
			m_pElements = NULL;
			m_nEntries = -1;
			m_nSize = -1;
			m_nGran = -1;
			}

	/* ***********************************************
	 * PUBLIC CONTROL AND ACCESS METHODS
	 * **********************************************/
	public:
		// Simple accessor for the entry count
		// (note the "size" is nobody's business)
		//
		unsigned int Number( void )
			{ 
		 	return m_nEntries; 
			}

		// What could be simpler than adding and removing
		// elements?
		//
		bool Add( elementType pItem )
			{
		    if (m_nEntries == m_nSize)
				{
				if (!expand( ))
					{
			    	return( FALSE );
					}
				}

			m_pElements[ m_nEntries ] = pItem;
			m_nEntries = m_nEntries+1;

		    return( TRUE );
			}

		// Note this version of remove DOES NOT deallocate
		// the element, it merely takes it out of the queue
		//
		void Remove( unsigned int nItem )
			{ 
			compress( nItem ); 
			}

		// Getting and setting existing elements is also pretty
		// basic.  NOTE: these methods allow for loop iteration
		//
		bool Get( unsigned int nItem, elementType *theelement )
			{		
			if ( nItem < m_nEntries )
				{
				*theelement=m_pElements[nItem];
				return true;
				}		
			ASSERT( FALSE );
			return false;
			}


		// Getting and setting existing elements is also pretty
		// basic.  NOTE: these methods allow for loop iteration
		//
		elementType *GetArray() const
			{		
		    return( m_pElements );
			}


		void Set( unsigned int nItem, elementType lpItem )
			{
			if ( nItem < m_nEntries )
				{
				m_pElements[nItem] = lpItem;
				}
			}

	/* ***************
	 * PRIVATE METHODS
	 ****************/
	private:

		// We need to reset the size by either increasing or
		// decreasing
		//
		bool changeSize( unsigned int nNewSize )
			{
			unsigned int wMemSize = nNewSize * sizeof( elementType );

			elementType* lpNew = (elementType*)::realloc( m_pElements, wMemSize );
			ASSERT( lpNew != NULL );
			if ( lpNew != NULL )
				{
				m_pElements = lpNew;
				m_nSize = nNewSize;
				}
	
		    return( lpNew != NULL );
			}

		// When the addition of elements requires the table to be expanded
		// this is the method that does it
		//
		bool expand( )
			{
			unsigned int wNewSize = m_nSize + m_nGran;
	
			return ( changeSize( wNewSize ) );
			}

		// This method actually reduces the size of the table, compressing
		// the array to keep all the used entries consecutive
		//
		void compress( unsigned int nIndex )
			{
			if (nIndex == -1)
				{
				return;
				}
				
			elementType* lpElement = m_pElements + nIndex;

			unsigned int wEntriesAfterIndex = ( m_nEntries - ( nIndex + 1 ) );
		    if ( wEntriesAfterIndex > 0)
			    {
				::memmove( lpElement, lpElement + 1, wEntriesAfterIndex * sizeof( elementType ) );
				}
		    m_nEntries--;

			// If we have decreased the size below a granularity chunk,
			// new size is the same as number of entries (since we normally
			// decrement the size one entry at a time, this will stay at
			// a granularity boundary)
			//
			if ( ( m_nEntries > 0 ) &&
				 ( m_nEntries <= ( m_nSize - m_nGran) ) )
				{
				// verify my assumption about granularity
				//
				ASSERT( (m_nEntries % m_nGran) == 0 );
	
				changeSize( m_nEntries );
				}
			}

	/**********************************
	 * DATA MEMBERS (all private)
	 */
	private:
		// Granularity is the amount the queue will be expanded by
		// whenever it must grow.  This parameters allows control
		// over size/speed optimization issues
		//
    	unsigned int				m_nGran;			

		// The total size of the table, including unused entries
		//
		unsigned int				m_nSize;			

		// The total # of entries in the table, only counting those
		// actually in use
		//
		unsigned int				m_nEntries;		// # of entries in table

		// The offset of the table itself
		//
		elementType *				m_pElements;		// Table of elements
	};






#ifdef IC_DYNARRAY_H_ASSERT
	#undef ASSERT
#endif

#endif // _IC_DYNARRAY_H

