#pragma once
#include <windows.h>
#include <string>

class CTsCrasher
{
	friend DWORD WINAPI startMonitoring(void*);
	friend DWORD WINAPI startNextSession(void* pMonitor);

	enum
	{
		eFinishMonitoring,
		ePossibleException,
		eAccepted,
		eGenerateException,
		eDontGenerateException,
		eWorking,
		eCount
	};
	HANDLE m_hEvents[eCount];
	std::string m_names[eCount];

	HANDLE m_hMonitorThread;

	int m_nPossibleExceptions;
	int m_iCurrentException;
	int m_iSession;
	bool m_bMonitoring;
	bool m_bGenerating;

	DWORD monitoring();
	DWORD generating();

public:
	CTsCrasher();
	~CTsCrasher();

	void startMonitoring();
	void startSessions();
	bool startNextSession();
	void endMonitoring();
	
	bool possibleException();
	bool working();
};

extern CTsCrasher g_crasher;
