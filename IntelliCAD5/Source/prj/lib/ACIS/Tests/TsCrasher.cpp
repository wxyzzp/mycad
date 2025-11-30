#include "stdafx.h"
#include "TsCrasher.h"
#define _USE_TREADS 0

static
DWORD WINAPI startMonitoring(void* pMonitor);
static
DWORD WINAPI startNextSession(void* pMonitor);

CTsCrasher g_crasher;

CTsCrasher::CTsCrasher()
:
m_hMonitorThread(0),
m_nPossibleExceptions(0),
m_iCurrentException(0),
m_iSession(0),
m_bMonitoring(false),
m_bGenerating(false)
{
#if _USE_THREADS
	m_names[eFinishMonitoring] = "ICAS_FINISHMONITORING";//autoreset
	m_names[ePossibleException] = "ICAS_POSSIBLEEXCEPTION";//autoreset
	m_names[eAccepted] = "ICAS_ACCEPTED";//autoreset
	m_names[eGenerateException] = "ICAS_GENERATEEXCEPTION";
	m_names[eDontGenerateException] = "ICAS_DONTGENERATEEXCEPTION";
	m_names[eWorking] = "ICAS_WORKING";

	// Create events
	for (int i = 0; i < eCount; i++)
	{
		if (i == eWorking)
			m_hEvents[i] = CreateEvent(0, TRUE, FALSE, m_names[i].c_str());
		else
			m_hEvents[i] = CreateEvent(0, FALSE, FALSE, m_names[i].c_str());
	}
#endif
}

CTsCrasher::~CTsCrasher()
{
#if _USE_THREADS
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hEvents[eWorking], 0))
	{
		endMonitoring();
	}

	// Close events
	for (int i = 0; i < eCount; i++)
	{
		CloseHandle(m_hEvents[i]);
	}
#endif
}

DWORD WINAPI startMonitoring(void* pMonitor)
{
	return reinterpret_cast<CTsCrasher*>(pMonitor)->monitoring();
}

#if _USE_THREADS
void CTsCrasher::startMonitoring()
{
	SetEvent(m_hEvents[eWorking]);
	m_hMonitorThread = CreateThread(0, 0, &(::startMonitoring), this, 0, 0);
}
#else
void CTsCrasher::startMonitoring()
{
	m_bMonitoring = true;
	m_iSession = 0;
	m_nPossibleExceptions = 0;
}
#endif
#if _USE_THREADS
void CTsCrasher::endMonitoring()
{
	ResetEvent(m_hEvents[eWorking]);
	SetEvent(m_hEvents[eFinishMonitoring]);
	WaitForSingleObject(m_hMonitorThread, INFINITE);
	CloseHandle(m_hMonitorThread);
}
#else
void CTsCrasher::endMonitoring()
{
	m_bMonitoring = m_bGenerating = false;
}
#endif

DWORD CTsCrasher::monitoring()
{
	m_nPossibleExceptions = 0;
	while (true)
	{
		DWORD dwIndex = WaitForMultipleObjects(2, m_hEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (dwIndex == eFinishMonitoring)
			return m_nPossibleExceptions;

		SetEvent(m_hEvents[eAccepted]);
		m_nPossibleExceptions++;

		SetEvent(m_hEvents[eDontGenerateException]);
	}
}

DWORD WINAPI startNextSession(void* pMonitor)
{
	return reinterpret_cast<CTsCrasher*>(pMonitor)->generating();
}

void CTsCrasher::startSessions()
{
	m_iSession = 0;
}

#if _USE_THREADS
bool CTsCrasher::startNextSession()
{
	if (m_iSession >= m_nPossibleExceptions)
		return false;

	SetEvent(m_hEvents[eWorking]);
	m_hMonitorThread = CreateThread(0, 0, ::startNextSession, this, 0, 0);
	return true;
}
#else
bool CTsCrasher::startNextSession()
{
	if (m_iSession >= m_nPossibleExceptions)
		return false;

	m_iCurrentException = 0;
	m_bGenerating = true;
	return true;
}
#endif

bool CTsCrasher::possibleException()
{
	if (m_bMonitoring)
	{
		m_nPossibleExceptions++;
	}
	else if (m_bGenerating)
	{
		if (m_iCurrentException++ == m_iSession)
		{
			m_iSession++;
			return true;
		}
	}
	return false;
}

bool CTsCrasher::working()
{
	return m_bMonitoring || m_bGenerating;
}

DWORD CTsCrasher::generating()
{
	m_iCurrentException = 0;
	while (true)
	{
		DWORD dwIndex = WaitForMultipleObjects(2, m_hEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (dwIndex == eFinishMonitoring)
			return ++m_iSession;
		SetEvent(m_hEvents[eAccepted]);

		if (m_iSession == m_iCurrentException++)
			SetEvent(m_hEvents[eGenerateException]);
		else
			SetEvent(m_hEvents[eDontGenerateException]);
	}
}
