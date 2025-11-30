#pragma once
#ifndef _CMDCOMMAND_H_
#define _CMDCOMMAND_H_

#include "sds.h"
#undef T
#undef X
#undef Y
#undef Z
#include <string>
#include <exception>
#include <assert.h>

class CCmdQuery
{
protected:
    // Code returned from SDS function
    int m_rc;

public:
    CCmdQuery():m_rc(RTNORM)
    {}
    
    virtual CCmdQuery* query() throw (std::exception) = 0;

    int rc() const
    {
        return m_rc;
    }
};

class CCmdQueryEx: public CCmdQuery
{
protected:
    friend class CCmdCommand;

    // Accept mode
    int m_accept;
    // Keys
    std::string m_sKeys;
    // Command prompt
    std::string m_sPrompt;

public:
    CCmdQueryEx();
    void init(const char* prompt, const char* keys = 0, int acceptmode = 0);

    int getAccept() const;
    void setAccept(int);

    const std::string& getKeys() const;
    void setKeys(const char*);

    const std::string& getPrompt() const;
    void setPrompt(const char*);
};

/*-------------------------------------------------------------------------*//**
Class CCmdCommand - base for interactive command implementation
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdCommand: public CCmdQuery
{
protected:
    CCmdQuery* m_pStart;

public:
    CCmdCommand()
        :
    CCmdQuery(),
    m_pStart(0)
    {}

    virtual int run();
    virtual CCmdQuery* query();
};

/*-------------------------------------------------------------------------*//**
class TCmdQueryLink - base for linking qeries between themselves
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template <int N>
class TCmdQueryLink
{
protected:
    // Command evolution variants
    CCmdQuery* m_pNextQuery[N];

public:
    TCmdQueryLink()
    {
        for (int i = 0; i < N; i++)
            m_pNextQuery[i] = 0;
    }
    virtual CCmdQuery* next(int i = 0)
    {
        assert((i>=0) && (i<N));
        return m_pNextQuery[i];
    }
    int ways()
    {
        return N;
    }
    CCmdQuery* get(int i)
    {
        assert((i>=0) && (i<N));
        return m_pNextQuery[i];
    }
    void set(int i, CCmdQuery* p)
    {
        assert((i>=0) && (i<N));
        m_pNextQuery[i] = p;
    }
};

#endif
