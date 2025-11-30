
#pragma once

namespace icl
{

//////////////////////////////////////////////////////////////////////
// arena:
// helper struct for greedy_heap class: handles real memory pool

template <typename type_t>
struct arena
{
	type_t*			_pool;	// real data here
	arena<type_t>*	_next;	// next arena in the list

	arena(size_t nobj);
	~arena();
};


template <typename type_t>
arena<type_t>::arena(size_t nobj) : _next(0)
{
	assert(nobj > 0);

	size_t size = nobj;
	_pool = new type_t[nobj];

	type_t* p = _pool;
	type_t* n = p+1;
	for (int i = nobj-1; i--; p = n++)
		*(void**)p = n;
	*(void**)p = 0;
}

template <typename type_t>
arena<type_t>::~arena()
{
	delete [] _pool;
}


//////////////////////////////////////////////////////////////////////
// greedy_heap:
// allocates memory with big chunks, then just manipulates pointers when asked for alloc/free;
// now uses own thread synchronization

template <typename type_t>
class greedy_heap
{
public:
	greedy_heap(size_t offset = 0, size_t = 4096);
	~greedy_heap();

	void destroy();

	type_t* malloc();
	void free(type_t*);

	type_t* mallocl(size_t n);	// allocates a single-linked list of n type_t's; uses _offset for linking
	void freel(type_t* p);		// frees the list

private:
	arena<type_t>*	_arena;		// list of arenas containing data pools
	type_t*			_head;		// points to he 1st free element in arenas (not necessary with the lowest address!)

	size_t	_arena_size;		// the current arena pool size in sizeof(type_t)'s
	size_t	_offset;			// if type_t is a linked list node (like class A{ ... A* next; ...}),
								// then _offset is the offset of linking field (A::next) in this class object
	CRITICAL_SECTION _section;	// locking guard for this heap

	class lock
	{
	public:
		lock(CRITICAL_SECTION* pSection) : _pSection(pSection)	{ EnterCriticalSection(pSection); }
		~lock()													{ LeaveCriticalSection(_pSection); }
	private:
		CRITICAL_SECTION* _pSection;
	};
};


template <typename type_t>
greedy_heap<type_t>::greedy_heap(size_t offset, size_t nobj) : _arena(0), _head(0), _arena_size(nobj), _offset(offset)
{
	assert(_arena_size > 0);

	InitializeCriticalSection(&_section);
	lock	_lock(&_section);
	_arena = new arena<type_t>(_arena_size);
	_head = _arena->_pool;
}

template <typename type_t>
greedy_heap<type_t>::~greedy_heap()
{
	destroy();
	DeleteCriticalSection(&_section);
}

template <typename type_t>
void greedy_heap<type_t>::destroy()
{
	lock	_lock(&_section);
	while (_arena)
	{
		arena<type_t>* a = _arena->_next;
		delete _arena;
		_arena = a;
	}
	_arena = 0;
	_arena_size = 4096;
	_head = 0;
	_offset = 0;
}

template <typename type_t>
type_t* greedy_heap<type_t>::malloc()
{
	lock	_lock(&_section);
	if (!_head)
	{
		_arena_size *= 2;
		arena<type_t>* a = new arena<type_t>(_arena_size);
		a->_next = _arena;
		_arena = a;

		_head = (type_t*)_arena->_pool;
	}

	type_t* p = _head;
	_head = (type_t*)(*(void**)_head);

	return p;
}

template <typename type_t>
void greedy_heap<type_t>::free(type_t* p)
{
	lock	_lock(&_section);
	*(void**)p = _head;
	_head = p;// VT. Fix. 9-4-2002
}

template <typename type_t>
type_t* greedy_heap<type_t>::mallocl(size_t n)
{
	// TO DO: simplify things by avoiding calls of malloc here; these calls are unwelcome with some synchronization mechanisms
	lock	_lock(&_section);
	if(!n)
		return 0;
	type_t	*pHead = 0, *pLast = malloc();
	if(!pLast)
		return 0;
	pHead = pLast;
	for(size_t i = n-1; i--; )
	{
		if(!(*(type_t**)((char*)pLast + _offset) = malloc()))
		{
			*(type_t**)((char*)pLast + _offset) = 0;
			freel(pHead);
			return 0;
		}
		pLast = *(type_t**)((char*)pLast + _offset);
	}
	*(type_t**)((char*)pLast + _offset) = 0;
	return pHead;
}

template <typename type_t>
void greedy_heap<type_t>::freel(type_t* p)
{
	lock	_lock(&_section);
	if(!p)
		return;
	type_t* pHead = p;
	while(*(type_t**)p = *(type_t**)((char*)p + _offset))	// not '==' !
		p = *(type_t**)p;
	*(type_t**)p = _head;
	_head = pHead;
}

}	// namespace icl
