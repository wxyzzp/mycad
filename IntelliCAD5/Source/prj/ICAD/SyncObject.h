#ifndef SYNC_OBJECT_H
#define SYNC_OBJECT_H

#pragma once

class WndActionEvent
{
public:

	WndActionEvent(BOOL bInitiallyOwn = FALSE, 
		  BOOL bManualReset = FALSE,
		  LPCTSTR lpszName = NULL, 
		  LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
	virtual ~WndActionEvent();
	
	virtual BOOL Lock(DWORD dwTimeout = INFINITE);
	virtual BOOL Unlock();
	operator HANDLE() const;

	BOOL SetEvent();
	BOOL PulseEvent();
	BOOL ResetEvent();

	void SetReturnVal(LRESULT lRsult);
	LRESULT GetReturnVal();

protected:
	LRESULT m_lResult;
	HANDLE  m_hObject;
};

/*****************************************************************************\
*
*	Inline definitions
*
\******************************************************************************/
inline WndActionEvent::WndActionEvent(BOOL bInitiallyOwn/*=FALSE*/, 
				  BOOL bManualReset/*= FALSE*/,
				  LPCTSTR lpszName/*=NULL*/, 
				  LPSECURITY_ATTRIBUTES lpsaAttribute/*=NULL*/) 
	{
	m_hObject = ::CreateEvent(lpsaAttribute, bManualReset, bInitiallyOwn, lpszName);
	ASSERT(m_hObject != NULL);
	}
	
inline WndActionEvent::~WndActionEvent()
	{
	if (m_hObject != NULL)
		::CloseHandle(m_hObject);
	m_hObject = NULL;
	}

inline WndActionEvent::operator HANDLE() const 
	{ 
	return m_hObject; 
	}

inline void 
WndActionEvent::SetReturnVal(LRESULT lResult)
	{
	m_lResult = lResult;
	}

inline LRESULT 
WndActionEvent::GetReturnVal()
	{
	return m_lResult;
	}

inline BOOL 
WndActionEvent::Lock(DWORD dwTimeout/*=INFINITE*/)
	{
	DWORD dwRet;
	MSG msg;
	
	while (true)
		{
		dwRet = MsgWaitForMultipleObjects(1,			// Nnumber of handles 
										&m_hObject,
										FALSE,			// Wait for all or wait for one
										dwTimeout,		// Time-out interval in milliseconds 
										QS_ALLINPUT);	// Type of input events to wait for 
		if (dwRet == WAIT_OBJECT_0)
			{
			// The sync. object is signaled.
			return TRUE;
			}
		else if (dwRet == WAIT_OBJECT_0 + 1)
			{
			// A message(s) is available in the thread's input queue. 
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		else
			return FALSE; // All other cases: WAIT_TIMEOUT & WAIT_ABANDONED
		}
	}

inline BOOL 
WndActionEvent::Unlock() 
	{ 
	return TRUE; 
	}

inline BOOL 
WndActionEvent::SetEvent()
	{ 
	ASSERT(m_hObject != NULL); 
	return ::SetEvent(m_hObject);
	}

inline BOOL 
WndActionEvent::PulseEvent()
	{ 
	ASSERT(m_hObject != NULL); 
	return ::PulseEvent(m_hObject); 
	}

inline BOOL 
WndActionEvent::ResetEvent()
	{ 
	ASSERT(m_hObject != NULL); 
	return ::ResetEvent(m_hObject); 
	}
#endif
