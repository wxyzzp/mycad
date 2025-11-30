// File  :
// Author: Alexey Malov
#pragma once
#ifndef _ASCACHE_H_
#define _ASCACHE_H_

#pragma warning (disable: 4786)

#include <map>
#include "DbAcisEntityData.h" //class CDbAcisEntityData
#include <time.h>
#include "AsObject.h"
#include <memory>

struct CAsCacheItemData
{
    //statistic of object's living in the cache

	//duration of item's creation
    clock_t m_tc;
	//item's size
    unsigned long m_size;
	//benefit from storing of the item in the cache
    long m_be;

    // item sate
    enum 
    {
        eNone =			0,
        eMeasurable =	1 << 0,
        eDeleted =		1 << 1
    };
    int m_state;

    bool measurable(bool f = false)
    {
        if (!f)
            return (m_state & eMeasurable) != 0;
        else
            m_state |= eMeasurable;
        return true;
    }

	//default ctor
	CAsCacheItemData():
	m_tc(0),m_be(0),m_size(0),m_state(eNone)
	{}
	//coping ctor
	CAsCacheItemData(const CAsCacheItemData& data):
	m_tc(data.m_tc),
	m_size(data.m_size),
	m_be(data.m_be),
	m_state(data.m_state)
	{}
};

class CAsCacheItem: public CAsCacheItemData
{
    //the item's data
	std::auto_ptr<CAsObj> m_obj;

public:

    //default ctor
    CAsCacheItem():
	CAsCacheItemData()
    {}

	//copying ctor
	CAsCacheItem(const CAsCacheItem& item):
	CAsCacheItemData(item),
	m_obj(item.m_obj)
	{}

	//assign operator
	const CAsCacheItem& operator = (const CAsCacheItem& item)
	{
		if (this == &item)
			return *this;

		m_tc = item.m_tc;
		m_size = item.m_size;
		m_be = item.m_be;
		m_state = item.m_state;
		m_obj = item.m_obj;

		return *this;
	}

	CAsObj* obj()
	{
		return m_obj.get();
	}
	CAsObj* release()
	{
		return m_obj.release();
	}
	void obj(CAsObj* obj)
	{
		m_obj = std::auto_ptr<CAsObj>(obj);
	}
	BODY* body()
	{
		return m_obj->get();
	}
};

class CAsCache
{
    //data:
    // map type
    typedef std::map<const CDbAcisEntityData, CAsCacheItem> ascache;
    // storage of data - sat representation and correspondent entities
    ascache m_room;

    //average activity
    double m_active;
    //cash size in bytes
    unsigned long m_size;
    //maximal room size in bytes
    unsigned long m_maxroom;
    //inactivity, as part from average activity
    double m_sleep;

    clock_t m_time;

    long m_request;
    long m_hit;

    // period of time, to perform configuration
    clock_t m_cycle;

    // lock flag
    bool m_bLocked;

public:
    // default ctor
    CAsCache();
    // dtor
    virtual ~CAsCache();

public: //typedefs

	//type of a key
    typedef CDbAcisEntityData key_t;
	//type of a value, corresponding the key
	typedef CAsCacheItem item_t;
	//type of subvalue
	typedef CAsObj* value_t;
	typedef CAsObj value_ref_t;
	typedef BODY* value_data_t;
    // type for map iterator
    typedef ascache::iterator iterator_t;
	typedef ascache::value_type node_t;

public: //methods
    
	// erase all items in the cache
    void clear();

    /*-------------------------------------------------------------------------*//**
    Get CAsObj.
    There are 2 case: 
     - the object is in the cache already, so return it; 
     - the object is not in the cache so create it from sat-data, 
       put in the cache and return.
    @param => sat data
    @return pointer to object - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    value_t operator [] (const key_t&) throw (exception);

	/*-------------------------------------------------------------------------*//**
	Finds the cache element, it's pair of key and item; returns the pointer to node in map
	@author Alexey Malov
	@param => key
	@return node
	*//*--------------------------------------------------------------------------*/
	iterator_t find(const key_t&) throw (exception);
	/*-------------------------------------------------------------------------*//**
	Finds the item and detach it from map
	@author Alexey Malov
	@param => key
	@return found item
	*//*--------------------------------------------------------------------------*/
	item_t detach(const key_t&) throw (exception);
	/*-------------------------------------------------------------------------*//**
	Detach the node (found earlier)
	@author Alexey Malov
	@param => node
	@return item
	*//*--------------------------------------------------------------------------*/
	item_t detach(iterator_t);
	/*-------------------------------------------------------------------------*//**
	Inserts the item, create new value with the same value_data
	@author Alexey Malov
	@param => key
	@param => item
	*//*--------------------------------------------------------------------------*/
	void insert(const key_t&, item_t&);
	/*-------------------------------------------------------------------------*//**
	Inserts the item, create new value with passed value_data
	@author Alexey Malov
	@param => key
	@param => value_data
	*//*--------------------------------------------------------------------------*/
	void insert(const key_t&, value_data_t);
	/*-------------------------------------------------------------------------*//**
	Inserts item, without creation of new value
	@author Alexey Malov
	@param
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	void update(const key_t&, item_t&);

    /*-------------------------------------------------------------------------*//**
    Get/set lock of the cache.
    Lock means, that any operation, changing map state, is not allowed; 
    it's need to use pointer on the item in the cache.
    If passed flag is false, method return current lock (locked (true)/ unlocked (false))
    If passed flag is true, method set lock and return previous lock state.
    @param => flag: true - set lock; dont change current lock.
    @return lock state: true - locked; fasle - unlocked
    *//*--------------------------------------------------------------------------*/
    bool lock(bool = false);
    /*-------------------------------------------------------------------------*//**
    Unlock the map.
    *//*--------------------------------------------------------------------------*/
    void unlock();
private: //methods
    /*-------------------------------------------------------------------------*//**
    Performs configurating of the map.
    Estimate average item activites in the cache, determine 'lazy' items and delete it.
    Configurating may recurs, in present implemntation depth of recursion is not more then 1.
    @param => recursion depth
    *//*--------------------------------------------------------------------------*/
    void configure(int = 0);
    
    void time(clock_t* = 0);
#if 0
    void dump();
#endif
};

#endif
