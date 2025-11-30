// File  :
// Author: Alexey Malov
#include "stdafx.h"
#include "AsCache.h"
#include "AsModeler.h"
#include "AsTools.h"
#include "AsObject.h"

CAsCache::CAsCache()
:
m_active(0.),
m_size(0),
m_maxroom(100*1024),
m_sleep(0.5),
m_time(0),
m_request(0),
m_hit(0),
m_cycle(60),
m_bLocked(false)
{}

CAsCache::~CAsCache()
{
    clear();
}

void CAsCache::clear()
{
    m_room.clear();
}

CAsCache::iterator_t CAsCache::find
(
const CAsCache::key_t& key
)
throw (exception)
{
	if (key.getSat().empty() || key.getType() == key_t::eUndefined)
		throw exception();

    // auto-tuning (statistic analysing, configurating)
    configure();

    // measure searche time
    clock_t ts = 0;
    time();
	iterator_t i = m_room.insert(node_t(key, item_t())).first;
    time(&ts);

    // increment request number
    m_request++;

	item_t& item = i->second;
    // if object did not exist in the map
    if (!item.obj())
    {
		//create value
        
		// measure creation time
        clock_t tc;
        time();
        item.obj(static_cast<CAsViewer*>(CModeler::get())->create(key));
        time(&tc);
        if (!item.obj())
            throw exception();
        
        item.m_tc = tc;
        item.m_size = key.getSat().size();
        item.m_be = 0;
    }
    else
    {
        // hit
        m_hit++;
        // benefit from cash usage
        item.m_be += item.m_tc-ts;
    }

    return i;
}

CAsCache::value_t CAsCache::operator [] 
(
const CAsCache::key_t& key
)
throw (exception)
{
	return find(key)->second.obj();
}

CAsCache::item_t CAsCache::detach
(
const CAsCache::key_t& key
)
throw (exception)
{
	iterator_t i = find(key);
	item_t item = i->second;
	m_room.erase(i);
	
	return item;
}

CAsCache::item_t CAsCache::detach
(
CAsCache::iterator_t i
)
{
	item_t item = i->second;
	m_room.erase(i);
	
	return item;
}

void CAsCache::insert
(
const CAsCache::key_t& key,
CAsCache::item_t& item
)
{
	value_data_t data = item.body();
	item.obj()->set(0);
	delete item.release();
	item.obj(new value_ref_t);
	item.obj()->set(data);

	m_room.insert(node_t(key, item));
}

void CAsCache::insert
(
const CAsCache::key_t& key,
CAsCache::value_data_t data
)
{
	item_t item;
	item.obj(new value_ref_t);
	item.obj()->set(data);

	m_room.insert(node_t(key, item));
}

void CAsCache::update
(
const CAsCache::key_t& key,
CAsCache::item_t& item
)
{
	m_room.insert(node_t(key, item));
}

void CAsCache::configure(int s /*=0*/)
{
    // if map is empty
    if (m_room.empty())
        return;

    // check elapsed time from last configuring
    static clock_t s_start = clock();
    clock_t end = clock();
    if ((end-s_start)/CLOCKS_PER_SEC < m_cycle)
        return;
    else
        s_start = end;

    // Calculate average activity on the cache
    m_active = 0.;
    m_size = 0;
    iterator_t i;
    for (i = m_room.begin(); i != m_room.end(); i++)
    {
        CAsCacheItem& item = i->second;
        if (!item.measurable())
            continue;

        m_active += item.m_be;
        m_size += item.m_size;
    }
    if (m_size == 0)
        // all objects are not measurable
        m_active = 0.;
    else
        m_active /= m_size;

    if (m_active < 0.)
        m_active = 0.;

    // Remove items, with low activity
    for (i = m_room.begin(); i != m_room.end();)
    {
        CAsCacheItem& item = i->second;
        if (!item.measurable())
        {
            item.measurable(true);
            continue;
        }

        if (item.m_be < m_sleep*m_active*item.m_size)
            i = m_room.erase(i);
        else
        {
            item.m_be = 0;
            i++;
        }
    }
#if 0
    dump();
#endif
    // Free room
    if (m_size > m_maxroom && !s)
        configure(1);
}

void CAsCache::time(clock_t* t /*= 0*/)
{
    if (!t)
        m_time = clock();
    else
        *t = m_time = clock() - m_time;
}

bool CAsCache::lock(bool f/*= flase*/)
{
    if (f)
    {
        f = m_bLocked;
        m_bLocked = true;
    }
    else
    {
        f = m_bLocked;
    }
    return f;
}

void CAsCache::unlock()
{
    m_bLocked = false;
}

#if 0
void CAsCache::dump()
{
#ifdef _DEBUG
    DUMP1("cash size %d\n", m_size/*m_block*/);
    DUMP1("number of items %d\n", m_room.size());
    DUMP1("average activity %e\n", m_active);
    DUMP1("number of requests %d\n", m_request);
    DUMP1("number of hits %d\n", m_hit);
#endif
}
#endif
