// File  :
// Author: Alexey Malov
#include "CmdCommand.h"
#undef X
#undef Y
#undef T

int CCmdCommand::run()
{
    if (!m_pStart)
        return 0;

    CCmdQuery* pQuery = m_pStart;

    try
    {
        // Query sequence
        while (true)
        {
            // Execute a query
            CCmdQuery* next = pQuery->query();

            // Recieve code returned by query
            m_rc = pQuery->rc();

            // Is this an end?
            if (!next)
                break;
       
            pQuery = next;
        }
    }
    catch (const exception&)
    {
        m_rc = pQuery->rc();
        return 0;
    }
    catch (...)
    {
        m_rc = pQuery->rc();
        return 0;
    }

    return 1;
}

CCmdQuery* CCmdCommand::query() throw (exception)
{
	run();
    if (m_rc != RTNORM)
        throw exception();
    return 0;
}

CCmdQueryEx::CCmdQueryEx()
:
CCmdQuery(),
m_accept(0)
{}

const std::string& CCmdQueryEx::getPrompt() const
{
    return m_sPrompt;
}

void CCmdQueryEx::setPrompt(const char* szPrompt)
{
    if (szPrompt)
        m_sPrompt.assign(szPrompt);
}

int CCmdQueryEx::getAccept() const
{
    return m_accept;
}

void CCmdQueryEx::setAccept(int accept)
{
    m_accept = accept;
}

const std::string& CCmdQueryEx::getKeys() const
{
    return m_sKeys;
}

void CCmdQueryEx::setKeys(const char* szKeys)
{
    if (szKeys)
        m_sKeys.assign(szKeys);
}

void CCmdQueryEx::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    if (szPrompt)
        m_sPrompt.assign(szPrompt);

    if (szKeys)
        m_sKeys.assign(szKeys);

    m_accept = accept;
}


