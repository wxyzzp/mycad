// File  : 
// Author: Alexey Malov
#include "sds.h"
#undef T

inline
CConstIterator::CConstIterator(const sds_dobjll* p)
:m_p(const_cast<sds_dobjll*>(p)), m_first(m_p)
{}

inline
CConstIterator CConstIterator::operator ++ ()
{
    if (!m_p) return 0;
    m_p = m_p->next;
    return m_p;
}

inline
CConstIterator CConstIterator::operator ++ (int)
{
    if (!m_p) return 0;
    sds_dobjll* t = m_p;
    m_p = m_p->next;
    return t;
}

inline
const sds_dobjll* CConstIterator::operator -> () const
{
    return m_p;
}

inline
const sds_dobjll& CConstIterator::operator * () const
{
    return *m_p;
}

inline
CConstIterator::operator const sds_dobjll* () const
{
    return m_p;
}

inline
const sds_dobjll* CConstIterator::get() const
{
    return m_p;
}

inline
const sds_dobjll* CConstIterator::first() const
{
    return m_first;
}

inline
CIterator::CIterator()
:CConstIterator(0), m_prev(0)
{}

inline
CIterator::CIterator(sds_dobjll* p)
:CConstIterator(p), m_prev(0)
{}

inline
CIterator CIterator::operator ++ ()
{
    if (!m_p) return 0;
    m_prev = m_p;
    m_p = m_p->next;
    return m_p;
}

inline
CIterator CIterator::operator ++ (int)
{
    if (!m_p) return 0;
    m_prev = m_p;
    m_p = m_p->next;
    return m_prev;
}

inline
sds_dobjll* CIterator::operator -> ()
{
    return m_p;
}

inline
sds_dobjll& CIterator::operator * ()
{
    return *m_p;
}

inline
CIterator::operator sds_dobjll* ()
{
    return m_p;
}

inline
void CIterator::set(sds_dobjll* p)
{
    m_p = p;
    if (m_prev)
        m_prev->next = m_p;
    if (!m_first)
        m_first = m_p;
}

inline
sds_dobjll* CIterator::first() 
{
    return m_first;
}
