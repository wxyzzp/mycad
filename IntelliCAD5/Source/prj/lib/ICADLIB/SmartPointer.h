/* G:\ICADDEV\PRJ\LIB\ICADLIB\SMARTPOINTER.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Self garbage-collecting pointer template.
 * Does not clean up circular pointer references.
 * 
 */ 

#ifndef	_SMARTPOINTER_H
#define _SMARTPOINTER_H

#ifndef	_INC_TCHAR
#include <tchar.h>
#endif

//	AbstractSweeper allows polymorphic destruction of an object
//	This facilitates queueing objects for deferred destruction

struct AbstractSweeper
	{
	virtual ~AbstractSweeper(){};
	};


//	SmartPointer creates a template eliminating the need for storage management
//	Any object can be associated with the pointer, provided a default constructor
//	exists. 
//
//	Leaf objects are allocated with a reference count, which is maintained by SmartPointer.
//	Leaf objects can only be dereferenced via operator->, and will be freed automatically
//	when no references exist.
//
//	To create a leaf object, use
//
//	SmartPointer<Leaf>	pointer();				// uninitialized
//	SmartPointer<Leaf>	pointer( Leaf(...));	// initialized


template <class T> class SmartPointer
	{
										// the reference counted allocation
	template< class T> struct Leaf
		{
		T		leaf;
		int		count;
							Leaf() : count(1) {}
							Leaf(const T& initial) : leaf( initial), count(1) {}
		};

										// the Sweeper object can polymorphically delete the leaf object
	template< class T> struct Sweeper : AbstractSweeper
		{
		Sweeper( Leaf<T>* unreferenced) : garbage( unreferenced)	{}
		~Sweeper() 
			{ 
			delete garbage;
			}

		Leaf<T>*	garbage;
		};

public:
										// construct an uninitialized leaf
	SmartPointer( bool allocate = false) : reference( allocate ? new Leaf<T>() : NULL)
		{
		}

										// construct an initialized leaf by copying the initial values
	SmartPointer( const T *initial) : reference( new Leaf<T>(*initial))
		{
		}

										// construct an initialized leaf by copying the initial values
	SmartPointer( const T& initial) : reference( new Leaf<T>(initial))
		{
		}

										// destroy this reference
										// if last reference, destroy the leaf
	~SmartPointer()
		{
		if ( reference )
			{
			if ( reference->count > 1 )
				reference->count -= 1;
			else 
				ReleaseReference();
			}
		}


	void ReleaseReference()
		{
										// decrement the reference count and delete if unreferenced
		reference->count -= 1;
		if ( reference->count )
			ASSERT( reference->count > 0 );
		else
			{							// NOTE: could be queued for deferred thread
			Sweeper<T>	sweep(reference);
			}

		reference = NULL;
		}


	SmartPointer( const SmartPointer<T>&other)
		{
		reference = other.reference;
		if ( reference )
			reference->count += 1;
		}


										// make	a copy of the SmartPointer by incrementing refence count
	SmartPointer<T>& operator=( const SmartPointer<T>&other)
		{
										// first release any current reference
		if ( reference )
			{
			if ( reference->count > 1 )
				reference->count -= 1;
			else 
				ReleaseReference();
			}

		if ( other.reference )
			{
			reference = other.reference;
			reference->count += 1;
			}

		return *this;
		}

										// access leaf data members 
	T* operator->()
		{
		return &reference->leaf;
		}

										// THIS IS DANGEROUS
										//		The reference must not live longer than
										//		the SmartPointer generating it!!
	T& operator*()
		{
		return reference->leaf; 
		}

	operator bool()	const
		{
		return reference != NULL;
		}

										// check for multiple references
										// used in copyOnWrite checks
	int References() const
		{
		return reference->count;
		}

	BOOL IsShared()	const
		{
		return References() > 1;
		}

private:
	Leaf<T>		*reference;
	};


	//
	//	SmartBuffer is  a similar concept, but maintains a resizeable byte array
	//	
	//	const access to the array is allowed by cast
	//	non-const access requires a Memcpy call, which ensures buffer is available and large enough

class SmartBuffer
	{
public:
										SmartBuffer( size_t	size, const void *data = NULL, BOOL copyOnWrite=FALSE);
										operator const char*()
											{
											return smartData->data;
											}
	virtual	char *						Memcpy( const void *source, size_t count, size_t offset=0);
	size_t								Size() { return smartData->size;}
	virtual char *						Strcat( const void *source, size_t count);

private:
										// 
	struct Buffer
		{
				Buffer() : data( NULL), size(0) {}
				~Buffer() 
					{
					delete [] data;
					}
		char	*data;
		size_t	size;
		};


	SmartPointer< Buffer>	smartData;
	BOOL					copyOnWrite;
	};


	//
	//	String is a SmartBuffer for string handling

class String : public SmartBuffer
	{
public:
	String() : SmartBuffer( sizeof TCHAR)
		{
		Memcpy( __T(""), sizeof TCHAR);
		}
	String( const TCHAR *text)
			: SmartBuffer( _tcslen( text) + 1, text)
		{
		}
	char *Strcpy( const TCHAR *text, size_t offset=0)
		{
		return (char *)Memcpy( text, (_tcslen( text) + 1) * sizeof TCHAR, offset);
		}
	};

#endif	// _SMARTPOINTER_H


