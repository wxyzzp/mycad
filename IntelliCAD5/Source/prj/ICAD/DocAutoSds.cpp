/* INTELLICAD\PRJ\ICAD\DOCAUTOSDS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * These are the automation helper functions of CIcadDoc.  The Automation layer
 * calls these functions of IcadDoc rather than calling the sds functions directly.
 * 
 */ 

#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "sdsthreadjob.h"/*DNT*/
#include "sdsthreadjobsink.h"/*DNT*/
#include "sdsthread.h"/*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "inputeventqueue.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "cmdsHatch.h"
#include "CloneHelper.h"
#include "listcoll.h"/*DNT*/
#include "insert.h"/*DNT*/

extern LONG SDS_Inside3rdPartyFunc;
BOOL g_bCallInitGet = TRUE;	//this is a global to this file that is used in the following way
//if g_bCallInitGet == TRUE, then all GetXXX functions must do their own initget, and clean up after themselves (by cancelling the command)
//if g_bCallInitGet == FALSE, then the user has already called initget, so the GetXXX function should not.  This also indicates
//							  that the GetXXX function should not cancel the command, since the user may do a GetInput...

int SDS_GetAngDist(const sds_point ptStart, const char *szMsg, double *pdResult, getangdist_mode nMode);

HANDLE CIcadDoc::GetVbaSdsEvent()
	{
	if (!m_hVbaSdsEvent)
		{
		m_hVbaSdsEvent = ::CreateEvent( NULL,	// security attributes
										FALSE,	// this means an auto-reset object
										FALSE,	// initially in non-signaled state
										NULL	// unnamed object (we'll just keep the handle)
									  );
		}
	
	return m_hVbaSdsEvent;
	}

void CIcadDoc::PostJobToMainEngineThread (SDSThreadJob *pJob)
	{
	ASSERT( SDSApplication::GetEngineApplication() != NULL );
	ASSERT( SDSApplication::GetEngineApplication()->GetMainThread() != NULL );
	
	//Get the main Engine thread.  the sds function will be run on that thread
	SDSThread *pMainThread = SDSApplication::GetEngineApplication()->GetMainThread();
	
	//create an event that will be used to signal the termination of the sds function
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	LONG save_state = SDS_Inside3rdPartyFunc;
	SDS_Inside3rdPartyFunc = 1;
	
	//post the job to the engine thread.  SendJob will not work, because the UI thread gets
	//suspended and will not allow user input - Icad will lock up.
	pMainThread->PostJob( pJob );
	
	//activate the main window, since we are about to do some interactive work with Intellicad
	SDS_CMainWindow->SetActiveWindow();
	
	//Since PostJob is not synchronous, we now wait in this loop until the Job signals the event
	::ProcessMsgWaitForObject( hIcadVbaEvent );

	SDS_Inside3rdPartyFunc = save_state;

	//Once the job has been executed, we get out of the loop above and can return synchronously
	return;
	}

HRESULT CIcadDoc::AUTO_angtof (const char *szAngle, int nUnitType, double *pdAngle)
	{
	if (sdsengine_angtof(szAngle, nUnitType, pdAngle) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_distof (const char *szDistance, int nUnitType, double *pdDistance)
	{
	if (sdsengine_distof(szDistance, nUnitType, pdDistance) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_rtos (double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType)
	{
	if (sdsengine_rtos(dNumber, nUnitType, nPrecision, szNumber, iDimType) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_angtos (double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	if (sdsengine_angtos(dAngle, nUnitType, nPrecision, szAngle) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cvunit (double dUnits, const char *szOldUnit, const char *szNewUnit, double *pdNewNum)
	{
	if (sdsengine_cvunit(dUnits, szOldUnit, szNewUnit, pdNewNum) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_osnap (const sds_point ptAperCtr, const char *szSnapModes, sds_point ptPoint)
	{
	if (sdsengine_osnap(ptAperCtr, szSnapModes, ptPoint) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_findfile (const char *szLookFor, char *szPathFound)
	{
	if (sds_findfile (szLookFor, szPathFound) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}



class GetAngDistJob : public SDSThreadJob
	{
	public:
		//constructor
		GetAngDistJob( const sds_point ptStart, const char *szMsg, double *pdResult, int nMode, HANDLE event, long *retcode )
			{
			if (ptStart)
				{
				m_pt[0] = ptStart[0];
				m_pt[1] = ptStart[1];
				m_pt[2] = ptStart[2];
				m_bNoFirstPoint = FALSE;
				}
			else
				m_bNoFirstPoint = TRUE;
			m_msg = szMsg;
			m_mode = nMode;
			m_result = pdResult;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			SDS_SetCursor(IDC_ICAD_CROSS);
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			if (m_bNoFirstPoint)
				*m_retcode = SDS_GetAngDist(NULL, m_msg, m_result, (enum getangdist_types_enum)m_mode);
			else
				*m_retcode = SDS_GetAngDist(m_pt, m_msg, m_result, (enum getangdist_types_enum)m_mode);
			
			SDS_SetCursor(SDS_GetPickCursor());

			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}

			g_bCallInitGet = TRUE;
			
			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		bool		m_bNoFirstPoint;
		sds_point	m_pt;
		const char *m_msg;
		int			m_mode;
		double *	m_result;
		HANDLE		m_hVbaEvent;
		long *		m_retcode;
	};


HRESULT CIcadDoc::AUTO_GetAngDistOrient (const sds_point ptStart, const char *szMsg, double *pdResult, getangdist_mode nMode)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetAngDistJob *pJob = new GetAngDistJob (ptStart, szMsg, pdResult, nMode, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}


class GetCornerJob : public SDSThreadJob
	{
	public:
		//constructor
		GetCornerJob(const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite, bool bApplySnapping, HANDLE event, long *retcode )
			{
			m_pt[0] = ptStart[0];
			m_pt[1] = ptStart[1];
			m_pt[2] = ptStart[2];
			m_msg = szCornerMsg;
			m_result = ptOpposite;
			m_bApplySnapping = bApplySnapping;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			SDS_SetCursor(IDC_ICAD_CROSS);
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_getcorner(m_pt, m_msg, m_result, m_bApplySnapping, false);
			
			SDS_SetCursor(SDS_GetPickCursor());

			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}
			
			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		sds_point	m_pt;
		const char *m_msg;
		bool		m_bApplySnapping;
		double *	m_result;
		HANDLE		m_hVbaEvent;
		long *		m_retcode;
	};


HRESULT CIcadDoc::AUTO_getcorner(const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite, bool bApplySnapping)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetCornerJob *pJob = new GetCornerJob (ptStart, szCornerMsg, ptOpposite, bApplySnapping, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

class GetInputJob : public SDSThreadJob
	{
	public:
		//constructor
		GetInputJob(char *szMsg, HANDLE event, long *retcode )
			{
			m_msg = szMsg;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			*m_retcode = sdsengine_getinput (m_msg);
			
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
			sds_printf("\n");

			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		char *m_msg;
		HANDLE		m_hVbaEvent;
		long *		m_retcode;
	};

HRESULT CIcadDoc::AUTO_getinput(char *szEntry)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetInputJob *pJob = new GetInputJob (szEntry, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

class GetIntJob : public SDSThreadJob
	{
	public:
		//constructor
		GetIntJob(const char *szMsg, int *pInt, HANDLE event, long *retcode )
			{
			m_msg = szMsg;
			m_result = pInt;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );

			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_getint (m_msg, m_result);
			
			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}

			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const char *	m_msg;
		int *			m_result;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_getint(const char *szIntMsg, int *pnInteger)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetIntJob *pJob = new GetIntJob (szIntMsg, pnInteger, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

class GetPointJob : public SDSThreadJob
	{
	public:
		//constructor
		GetPointJob(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint, HANDLE event, long *retcode )
			{
			m_pt = ptReference;
			m_msg = szPointMsg;
			m_result = ptPoint;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

 			SDS_SetCursor(IDC_ICAD_CROSS);
 
			*m_retcode = sdsengine_getpoint (m_pt, m_msg, m_result, false);
			
			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}

			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const double *	m_pt;
		const char *	m_msg;
		double *		m_result;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_getpoint(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetPointJob *pJob = new GetPointJob (ptReference, szPointMsg, ptPoint, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

class GetRealJob : public SDSThreadJob
	{
	public:
		//constructor
		GetRealJob(const char *szMsg, double *pReal, HANDLE event, long *retcode )
			{
			m_msg = szMsg;
			m_result = pReal;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_getreal (m_msg, m_result);
			
			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}
			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const char *	m_msg;
		double *		m_result;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_getreal(const char *szRealMsg, double *pdReal)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetRealJob *pJob = new GetRealJob (szRealMsg, pdReal, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

class GetStringJob : public SDSThreadJob
	{
	public:
		//constructor
		GetStringJob (int swSpaces, const char *szMsg, char *szString, HANDLE event, long *retcode )
			{
			m_swSpaces = swSpaces;
			m_msg = szMsg;
			m_result = szString;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_getstring (m_swSpaces, m_msg, m_result, false);
			
			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}
			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		int				m_swSpaces;
		const char *	m_msg;
		char *			m_result;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_getstring(int swSpaces, const char *szStringMsg, char *szString)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetStringJob *pJob = new GetStringJob (swSpaces, szStringMsg, szString, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_initget(int bsAllowed, const char *szKeyWordList)
	{
	if (sdsengine_initget(bsAllowed, szKeyWordList) == RTNORM)
		{
		g_bCallInitGet = FALSE;
		return NOERROR;
		}
	else
		return E_FAIL;
	}

class GetKeywordJob : public SDSThreadJob
	{
	public:
		//constructor
		GetKeywordJob (const char *szMsg, char *szString, HANDLE event, long *retcode )
			{
			m_msg = szMsg;
			m_result = szString;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_getkword (m_msg, m_result);

			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}
			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const char *	m_msg;
		char *			m_result;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_getkword(const char *szKWordMsg, char *szKWord)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	GetKeywordJob *pJob = new GetKeywordJob (szKWordMsg, szKWord, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_grdraw(const sds_point ptFrom, const sds_point ptTo, int nColor, int swHighlight)
	{
	if (sdsengine_grdraw(ptFrom, ptTo, nColor, swHighlight) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_prompt(const char *szPromptMsg)
	{
	char	command[IC_ACADBUF];
	int n;
	
	//replace vbCr and vbCrLf with vbLf to enable sdsengine_prompt to handle it
	sprintf(command,"%s"/*DNT*/, szPromptMsg);
	for(n = 0; command[n] != 0; n++) 
		{ 
		if (command[n] == 13)	//vbCr, vbCrLf
			command[n] = 10;	//vbLf
		}

	if (sdsengine_prompt (command) == RTNORM)
		{
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);	//clear the command line
		return NOERROR;
		}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint)
	{
	sdsengine_polar(ptPolarCtr, dAngle, dDistance, ptPoint);
	return NOERROR;
	}

HRESULT CIcadDoc::AUTO_graphscr(void)
	{
	if (sdsengine_graphscr() == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_textscr(void)
	{
	if (sdsengine_textscr() == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_wcmatch(const char *szCompareThis, const char *szToThis)
	{
	if (sds_wcmatch(szCompareThis, szToThis) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

class EntselJob : public SDSThreadJob
	{
	public:
		//constructor
		EntselJob (const char *szMsg, sds_name nmEntity, sds_point ptSelected, HANDLE event, long *retcode )
			{
			m_msg = szMsg;
			m_result = nmEntity;
			m_pt = ptSelected;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_entsel(m_msg, m_result, m_pt, false);

			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}
			g_bCallInitGet = TRUE;
			
			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const char *	m_msg;
		long *			m_result;
		double *		m_pt;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_entsel(const char *szSelectMsg, sds_name nmEntity, sds_point ptSelected)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	EntselJob *pJob = new EntselJob (szSelectMsg, nmEntity, ptSelected, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

const double zerotol = 0.000000001; 
#define NEARZERO(x) (fabs (x) < zerotol) ? 0.0 : x

class NentselJob : public SDSThreadJob
	{
	public:
		//constructor
		NentselJob (const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, sds_point ptECStoWCS[4], struct sds_resbuf **pprbNestBlkList, HANDLE event, long *retcode )
			{
			m_msg = szNEntMsg;
			m_result = nmEntity;
			m_pt = ptEntPoint;
			m_matrix1 = ptECStoWCS[0];
			m_matrix2 = ptECStoWCS[1];
			m_matrix3 = ptECStoWCS[2];
			m_matrix4 = ptECStoWCS[3];
			m_list = pprbNestBlkList;
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );
			
			sds_point m_matrix[4];

			if (g_bCallInitGet)
				sds_initget(0,""/*DNT*/); // Reset sds_initget()

			*m_retcode = sdsengine_nentsel(m_msg, m_result, m_pt, m_matrix, m_list);

			m_matrix1[0] = NEARZERO (m_matrix[0][0]);
			m_matrix1[1] = NEARZERO (m_matrix[0][1]);
			m_matrix1[2] = NEARZERO (m_matrix[0][2]);
			m_matrix2[0] = NEARZERO (m_matrix[1][0]);
			m_matrix2[1] = NEARZERO (m_matrix[1][1]);
			m_matrix2[2] = NEARZERO (m_matrix[1][2]);
			m_matrix3[0] = NEARZERO (m_matrix[2][0]);
			m_matrix3[1] = NEARZERO (m_matrix[2][1]);
			m_matrix3[2] = NEARZERO (m_matrix[2][2]);
			m_matrix4[0] = NEARZERO (m_matrix[3][0]);
			m_matrix4[1] = NEARZERO (m_matrix[3][1]);
			m_matrix4[2] = NEARZERO (m_matrix[3][2]);
			
			if (g_bCallInitGet || (*m_retcode != RTKWORD))
				{
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
				sds_printf("\n");
				}
			g_bCallInitGet = TRUE;

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const char *	m_msg;
		long *			m_result;
		double *		m_pt;
		double *		m_matrix1;
		double *		m_matrix2;
		double *		m_matrix3;
		double *		m_matrix4;
		struct resbuf **m_list;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_nentsel(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, sds_point ptECStoWCS[4], struct sds_resbuf **pprbNestBlkList)
	{
	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	NentselJob *pJob = new NentselJob (szNEntMsg, nmEntity, ptEntPoint, ptECStoWCS, pprbNestBlkList, hIcadVbaEvent, &retcode);
	
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	if (retcode == RTNORM)
		return NOERROR;
	else if (retcode == RTKWORD)
		return E_INVALIDARG;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_trans(const sds_point From, const struct sds_resbuf *CoordFrom, const struct sds_resbuf *CoordTo, int swVectOrDisp, sds_point To)
	{
	if (sds_trans (From, CoordFrom, CoordTo, swVectOrDisp, To) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_ssadd(const sds_name nmEntToAdd, const sds_name nmSelSet, sds_name nmNewSet)
	{
	if (sds_ssadd (nmEntToAdd, nmSelSet, nmNewSet) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_ssname(const sds_name nmSelSet, long lSetIndex, sds_name nmEntity)
	{
	if (sds_ssname(nmSelSet, lSetIndex, nmEntity) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_ssfree (sds_name nmSetToFree)
	{
	if (sds_ssfree (nmSetToFree) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_sslength (sds_name nmSet, long * Length)
	{
	if (sds_sslength (nmSet, Length) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

class SelectOnScreenJob : public SDSThreadJob
	{
	public:
		//constructor
		SelectOnScreenJob (const char *szMsg, const char* szMode, void *pFirstPoint, const void* pSecondPoint, const struct resbuf *rbFilter, sds_name sset, HANDLE event, long *retcode, const int iOmitFromPrevSS, bool bIncludeInvisible, bool bIncludeLockedLayers)
			{
			m_msg = szMsg;
			m_mode = szMode;
			m_pFirstPoint = pFirstPoint;
			m_pSecondPoint = pSecondPoint;
			m_filter = (char *)rbFilter;
			m_result = sset;
			m_hVbaEvent = event;
			m_retcode = retcode;
			m_iOmitFromPrevSS = iOmitFromPrevSS;
			m_bIncludeInvisible = bIncludeInvisible;
			m_bIncludeLockedLayers = bIncludeLockedLayers;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );

			sds_printf("\n");
			
			*m_retcode = sds_pmtssget(m_msg, m_mode, m_pFirstPoint, m_pSecondPoint, (const struct resbuf *)m_filter, m_result, m_iOmitFromPrevSS, m_bIncludeInvisible, m_bIncludeLockedLayers, false);
			
//			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
			sds_printf("\n");

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		const char *	m_msg;
		const char *	m_mode;
		void *			m_pFirstPoint;
		const void *	m_pSecondPoint;
		char *			m_filter;
		int		m_iOmitFromPrevSS;
		bool			m_bIncludeInvisible;
		bool			m_bIncludeLockedLayers;
		long *			m_result;
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_pmtssget(const char *szSelMsg,const char *szSelMethod, void *pFirstPoint, const void *pSecondPoint, const struct sds_resbuf *prbFilter, 
							sds_name nmNewSet, const int iOmitFromPrevSS, bool bIncludeInvisible, bool bIncludeLockedLayers)
{
//	if (szSelMsg)	//this is the interactive selection
//		{

	HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

	long retcode;
	SelectOnScreenJob *pJob = new SelectOnScreenJob (szSelMsg, szSelMethod, pFirstPoint, pSecondPoint, prbFilter, nmNewSet, hIcadVbaEvent, &retcode, iOmitFromPrevSS, bIncludeInvisible, bIncludeLockedLayers);
		
	PostJobToMainEngineThread ((SDSThreadJob *)pJob);

	return NOERROR;
//		}
/*	else	//this is the programmatic selection
		{
		if (sds_pmtssget(szSelMsg, szSelMethod, pFirstPoint, pSecondPoint, prbFilter, nmNewSet, iOmitFromPrevSS, bIncludeInvisible, bIncludeLockedLayers, false) == RTNORM)
			return NOERROR;
		else
			return E_FAIL;

		}
*/	
}


HRESULT CIcadDoc::AUTO_ssxss (sds_name ss0, sds_name ss1, sds_real fuzz, struct resbuf **ptlistp, int mode3d, long *nskipped0p, long *nskipped1p, IntersectOptions interOption)
	{
	/*DG - 31.10.2001*/// Added the interOption parameter for ic_ssxss.
	if (ic_ssxss (ss0, ss1, fuzz, ptlistp, mode3d, nskipped0p, nskipped1p, static_cast<EInterExtendOption>(interOption)) == 0)
		return NOERROR;
	else
		return E_FAIL;
	}

class ZoomJob : public SDSThreadJob
	{
	public:
		//constructor
		ZoomJob (HANDLE event, long *retcode )
			{
			m_hVbaEvent = event;
			m_retcode = retcode;
			}
		
		virtual int Execute( void )
			{
			// The whole purpose is to make sure we are on the main engine thread, so ASSERT that fact
			ASSERT( SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() );

			sds_printf("\n");
			
			*m_retcode = cmd_zoom();
			
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"^C^C^C"/*DNT*/);
			sds_printf("\n");

			//once the job has executed, signal the event, so that the poster of the job can continue
			VERIFY( ::SetEvent( m_hVbaEvent ) );
			
			return 1;
			}
	
	protected:
		virtual bool isValid( void )
			{
			return true;
			}
	
	private:
		HANDLE			m_hVbaEvent;
		long *			m_retcode;
	};

HRESULT CIcadDoc::AUTO_cmd_zoom(CIcadView *pView, sds_point zoomcenter, double zoomfactor, int ScaleType)
	{
	int ret;
	if (pView == NULL)
		{
		HANDLE hIcadVbaEvent = CIcadDoc::GetVbaSdsEvent();

		long retcode;
		ZoomJob *pJob = new ZoomJob (hIcadVbaEvent, &retcode);
		
		PostJobToMainEngineThread ((SDSThreadJob *)pJob);

		return NOERROR;
		}
	else
		{
		if (ScaleType == -1)	//none defined
			{
			//[----IntelliKorea 2001/7/2
			ret = SDS_ZoomIt(/*m_dbDrawing*/NULL, 2, zoomcenter, &zoomfactor, NULL, NULL, NULL, NULL);
			//     IntelliKorea 2001/7/2----]
			}
		else
			{
			CString strScaleType;
			if (ScaleType == 0)	//vicZoomScaledAbsolute
				strScaleType = "X";
			else if (ScaleType == 1) //vicZoomScaledRelative
				strScaleType = "X";
			else if (ScaleType == 2) //vicZoomScaledRelativePSpace
				strScaleType = "P";

			CString strZoomFactor;
			strZoomFactor.Format("%f", zoomfactor);

			CString command = "ZOOM;" + strZoomFactor + strScaleType;
			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)command.GetBuffer(0)/*DNT*/);

			ret = RTNORM;
			}

		if (ret != RTNORM)
			return E_FAIL;
		}

	return NOERROR;
	}

HRESULT CIcadDoc::AUTO_cmd_zoom_extents(CIcadView *pView, int CheckLimits)
	{
	if (cmd_zoom_extents(CheckLimits) != RTNORM)
		return E_FAIL;

	return NOERROR;
	}

HRESULT CIcadDoc::AUTO_cmd_wblock_entmake(sds_name sset, sds_point insbase, char *fname, char *FileType, int* pnFileVersion)
	{
	long LenSelSet;

	//if the selection set is empty, put all entities into the selection set - this may occur during Export
	if ((RTNORM != sds_sslength(sset, &LenSelSet)) || (LenSelSet == 0) )
		{
		if (sds_pmtssget(NULL,"X"/*DNT*/,NULL,NULL,NULL,sset,0)!=RTNORM)
			return E_INVALIDARG;
		}

	char fileType = FileType ? (*FileType) : IC_DWG;
	int fileVersion = pnFileVersion ? (*pnFileVersion) : IC_ACAD2004;

	CCloneHelper ch(fileType, fileVersion);
	if(insbase)
		ch.setBasePoint(insbase);
	if(ch.wblock(sset, fname) == 0)
		return NOERROR;

	return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_box_create (sds_point pt1,sds_point pt2, sds_real high, db_handitem **hip)
	{
	if (cmd_box_create (pt1, pt2, high) == RTNORM)
		{
		sds_name ename;
		sds_entlast(ename);
		*hip = (db_handitem *)ename[0];
		return NOERROR;
		}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_cone_create (sds_point pt0,sds_real bdia,sds_real tdia,sds_point pthigh,int nseg, db_handitem **hip)
	{
	if (cmd_cone_create (pt0, bdia, tdia, pthigh, nseg) == RTNORM)
		{
		sds_name ename;
		sds_entlast(ename);
		*hip = (db_handitem *)ename[0];
		return NOERROR;
		}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_cyl_create (sds_point pt0,sds_real dia,sds_point pthigh,int nseg, db_handitem **hip)
	{
	if (cmd_cyl_create (pt0, dia, pthigh, nseg) == RTNORM)
		{
		sds_name ename;
		sds_entlast(ename);
		*hip = (db_handitem *)ename[0];
		return NOERROR;
		}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_sphere_create (sds_point pt0,sds_real rad,int longseg,int latseg, int domemode, db_handitem **hip)
	{
	if (cmd_sphere_create (pt0, rad, longseg, latseg, domemode) == RTNORM)
		{
		sds_name ename;
		sds_entlast(ename);
		*hip = (db_handitem *)ename[0];
		return NOERROR;
		}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_torus_create (sds_point pt0,sds_real tordia,sds_real tubedia,int torseg,int tubseg, db_handitem **hip)
	{
	if (cmd_torus_create (pt0, tordia, tubedia, torseg, tubseg) == RTNORM)
		{
		sds_name ename;
		sds_entlast(ename);
		*hip = (db_handitem *)ename[0];
		return NOERROR;
		}
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_wedge_create (sds_point pt1,sds_point pt2, sds_real high,double ang, db_handitem **hip)
	{
	if (cmd_wedge_create (pt1, pt2, high, ang) == RTNORM)
		{
		sds_name ename;
		sds_entlast(ename);
		*hip = (db_handitem *)ename[0];
		return NOERROR;
		}
	else
		return E_FAIL; 
	}

HRESULT CIcadDoc::AUTO_cmd_array_polar (sds_name ssetin, int nObjects, double fillangle, double anglebetween, sds_point basept, sds_point ptonaxis, bool rotate, sds_name ssetout)
	{
	struct polar_array_info pinfo;
	int mode;

	pinfo.FillAngle = fillangle;
	pinfo.nItems = nObjects;
	pinfo.AngleBetweenItems = anglebetween;
	pinfo.RotateItems = rotate;
	ic_ptcpy (pinfo.BasePoint, basept);
	if (ptonaxis)
		{
		if (icadPointEqual(basept,ptonaxis))
			mode = IC_ARRAYMODE_2D;
		else
			mode = IC_ARRAYMODE_3D;
		ic_ptcpy (pinfo.PointOnAxis, ptonaxis);
		}
	else
		{
		mode = IC_ARRAYMODE_2D;
		ic_ptcpy (pinfo.PointOnAxis, basept);
		}


	if (cmd_array_func (mode, ssetin, NULL, NULL, NULL, false, NULL, &pinfo, ssetout) == RTNORM)
		return NOERROR;

	return E_FAIL;
	}

HRESULT CIcadDoc::AUTO_cmd_array_rectangular (sds_name ssetin, int nRows, int nCols, int nLevs, double dRows, double dCols, double dLevs, sds_name ssetout)
	{
	struct rect_array_info rinfo;
	int mode;

	rinfo.nRows = nRows;
	rinfo.nColumns = nCols;
	rinfo.nLevels = nLevs;
	rinfo.RowDist = dRows;
	rinfo.ColDist = dCols;
	rinfo.LevDist = dLevs;

	if (nLevs > 1)
		mode = IC_ARRAYMODE_3D;
	else
		mode = IC_ARRAYMODE_2D;

	if (cmd_array_func (mode, ssetin, NULL, NULL, NULL, false, &rinfo, NULL, ssetout) == RTNORM)
		return NOERROR;

	return E_FAIL;
	}


HRESULT CIcadDoc::AUTO_cmd_explode_objects (sds_name ssetin, sds_name ssetout)
	{
	if (cmd_explode_objects (0, false, ssetin, ssetout) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

int cmd_offset_func(sds_name ename, double offset, int bothmode, int plmode, sds_point dirIndicator, bool bInteractive, sds_name ssetout=NULL);

HRESULT CIcadDoc::AUTO_cmd_offset_func(sds_name ename, double offset, int bothmode, int plmode, sds_point dirIndicator, sds_name ssetout)
	{
	if (cmd_offset_func(ename, offset, bothmode, plmode, dirIndicator, false, ssetout) == RTNORM)
		return NOERROR;
	else
		return E_FAIL;
	}

//The code for CreateImage was originally written by Mark Barrow

HRESULT CIcadDoc::CreateImage (CString strFile, CString strDictName, sds_point ptIn, db_handitem **hip)
	{
	/*
	**  Example code that creates an IMAGE entity in ICAD.
	**
	**  IMAGE entities and their OBJECTs are tied together in this
	**  amazing wad of spaghetti.  (Was this REALLY necessary, Autodesk?)
	**
	**  Here's an overview of the wad:
	**
	**    The Named Object Dictionary points to the image DICTIONARY.
	**
	**    The image DICTIONARY points back to the Named Object DICTIONARY
	**      and forward to the IMAGESDEFs (one for each image file).
	**
	**    Each IMAGEDEF points back to the image DICTIONARY and forward
	**      to each of the IMAGEDEF_REACTORS (one for each instance
	**      of this picture on the screen).
	**
	**    Each IMAGEDEF_REACTOR points to its IMAGE entity.  (No reference
	**      back to its container this time (the IMAGEDEF).
	**
	**    Each IMAGE entity points back to its IMAGEDEF_REACTOR --
	**      AND back to the IMAGEDEF for the appropriate file.
	**
	**  Let's make a batch.  I've hard-coded values in this example,
	**  based on what I see in ACAD.
	*/
    int rc, nRes;
    sds_name nod_ename,idict_ename,idef_ename,idefr_ename,dummy_ename;
    sds_name image_ename;
    sds_point pt[6];
    struct resbuf *elist[2],*trbp1;
	
	
    rc=0; 
	elist[0]=elist[1]=NULL; 
	dummy_ename[0]=dummy_ename[1]=0;
	
    /* Get the ename of the Named Object Dictionary: */
    if (sds_namedobjdict(nod_ename)!=RTNORM) 
		{ 
		rc=-1; 
		goto out; 
		}
	
    /* Does it already have an image dictionary? */
    if (elist[0]!=NULL) 
		{ 
		sds_relrb(elist[0]); 
		elist[0]=NULL; 
		}
	
    elist[0]=sds_dictsearch(nod_ename,"ACAD_IMAGE_DICT",0);
	
    if (elist[0]!=NULL) 
		{  /* Yes.  Get its ename (-1 group). */
		
        for (trbp1=elist[0]; trbp1!=NULL &&
            trbp1->restype!=-1; trbp1=trbp1->rbnext);
        if (trbp1==NULL) 
			{  /* No ename (-1 group)??? */
			rc=-2; 
			goto out; 
			} 
        idict_ename[0]=trbp1->resval.rlname[0];
        idict_ename[1]=trbp1->resval.rlname[1];
		
		} 
	else 
		{  /* No.  Make it and get its ename. */
		
        /* Build elist[0] for an empty image DICTIONARY object: */
        if (elist[0]!=NULL) 
			{
			sds_relrb(elist[0]); 
			elist[0]=NULL; 
			}
        if ((elist[0]=sds_buildlist(
            RTDXF0,"DICTIONARY",
			102,"{ACAD_REACTORS",
			330,nod_ename,          /* Point back to parent. */
			102,"}",
			0))==NULL) 
			{ 
			rc=-3; 
			goto out; 
			}
		
        /* Make it and get its ename: */
        if (sds_entmakex(elist[0],idict_ename)!=RTNORM) 
			{ 
			rc=-4; 
			goto out; 
			}
		
        /* Add the record to the Named Objects Dictionary, pointing to it: */
        if (sds_dictadd(nod_ename,"ACAD_IMAGE_DICT",idict_ename)!=RTNORM)
            { 
			rc=-5; 
			goto out; 
			}
		}
	
	/* Now the image dictionary exists, and its ename is in idict_ename. */
	
	
	/* Make an IMAGEDEF object and add a pointer to it */
	/* in the image dictionary: */
	
	/* Build elist[0] for an IMAGEDEF object: */
	
	pt[0][0]=180.0;  /* Pixels wide (as defined in the file, I think). */
	pt[0][1]=358.0;  /* Pixels tall (as defined in the file, I think). */
	pt[0][2]=0.0;
	
	pt[1][0]=pt[1][1]=5.0/900.0;  /* Default width and height of a pixel */
	pt[1][2]=0.0;                 /*   in drawing units. */
	
	
	if (elist[0]!=NULL) 
		{ 
		sds_relrb(elist[0]); 
		elist[0]=NULL; 
		}
	if ((elist[0]=sds_buildlist(
		RTDXF0,"IMAGEDEF",
		102,"{ACAD_REACTORS",
		330,idict_ename,  /* Point back to parent. */
		330,dummy_ename,  /* Place-holder.  Will eventually point to the */
		/*   IMAGEDEF_REACTOR.  (1 for each IMAGE entity */
		/*   that will reference this file.) */
		102,"}",
		90,0,            /* Class version.  0 for R14. */
		1,strFile,  /* Pathname of image file. */
		//             1,"d:\\mystuff\\images\\testmono.bmp",  /* Pathname of image file. */
		10,pt[0],        /* Image size in pixels. */
		11,pt[1],        /* Pixel size in drawing units. */
		280,1,            /* Image is loaded. */
		281,0,            /* Resolution units (no units). */
		0))==NULL) { rc=-3; goto out; }
	
	/* Make it and get its ename: */
	if (sds_entmakex(elist[0],idef_ename)!=RTNORM) 
		{ 
		rc=-4; 
		goto out; 
		}
	
	/* Add a record to the image dictionary, pointing to it. */
	/* The name is usually the filename, but doesn't have to be.) */
	//    if (sds_dictadd(idict_ename,"file1",idef_ename)!=RTNORM)
	if (sds_dictadd(idict_ename,strDictName,idef_ename)!=RTNORM)
		{ 
		//rc=-5; 
		//goto out; do not fail here because when inserting the same image twice we may already have a dictionary entry,
		}
	
	
	/* Make an IMAGEDEF_REACTOR object and fill in the pointer to it */
	/* in the IMAGEDEF object: */
	
	/* Build elist[1] for an IMAGEDEF_REACTOR object: */
	if (elist[1]!=NULL) 
		{ 
		sds_relrb(elist[1]); 
		elist[1]=NULL; 
		}
	
	if ((elist[1]=sds_buildlist(
		RTDXF0,"IMAGEDEF_REACTOR",
		90,2,            /* Class version.  (This time TWO means R14!) */
		330,dummy_ename,  /* Place-holder.  Will eventually point to the */
		/*   IMAGE entity. */
		0))==NULL) { rc=-3; goto out; }
	
	/* Make it and get its ename: */
	if (sds_entmakex(elist[1],idefr_ename)!=RTNORM) 
		{ 
		rc=-4; 
		goto out; 
		}
	
	/* Modify elist[0] (the IMAGEDEF) so we can use it in an entmod */
	/* to make the IMAGEDEF point to this new IMAGEDEF_REACTOR: */
	
	/* Add the IMAGEDEF's ename (-1 group) to the top of elist[0]. */
	/* (Could just entget the IMAGEDEF, but why bother?  This is easy.) */
	if ((trbp1=sds_newrb(-1))==NULL) 
		{ 
		rc=-3; 
		goto out; 
		}
	
	trbp1->resval.rlname[0]=idef_ename[0];
	trbp1->resval.rlname[1]=idef_ename[1];
	trbp1->rbnext=elist[0];
	elist[0]=trbp1;
	
	/* Find that 2nd 330 group (the dummy) and fill in the IMAGEDEF_REACTOR  */
	/* ename: */
	for (trbp1=elist[0]; trbp1!=NULL && trbp1->restype!=330; trbp1=trbp1->rbnext);
	if (trbp1==NULL || (trbp1=trbp1->rbnext)==NULL || trbp1->restype!=330)
		{ /* Should never. */
		rc=-6; 
		goto out; 
		}  
	trbp1->resval.rlname[0]=idefr_ename[0];
	trbp1->resval.rlname[1]=idefr_ename[1];
	
	/* Entmod the IMAGEDEF: */
	if (sds_entmod(elist[0])!=RTNORM) 
		{ 
		rc=-4; 
		goto out; 
		}
	
	
	/* Make an IMAGE entity and fill in the pointer to it */
	/* in the IMAGEDEF_REACTOR object: */
	
	/* Build elist[0] for an IMAGE entity: */
	
	/* First, set the points we're going to use in the buildlist: */
	
	pt[0][0]=ptIn[0]; 
	pt[0][1]=ptIn[1]; 
	pt[0][2]=ptIn[2];  /* Insertion pt */
	
	pt[1][0]=5.0/900.0; 
	pt[1][1]=pt[1][2]=0.0; /* Horizontal (U) pixel vector */
	
	pt[2][1]=5.0/900.0; 
	pt[2][0]=pt[2][2]=0.0; /* Vertical   (V) pixel vector */
	
	pt[3][0]=200.0; 
	pt[3][1]=200.0; 
	pt[3][2]=0.0;  /* Image size in pixels (from the file?) */
	
	/* These next two are the corners of the clipping rectangle, apparently */
	/* in pixels.  ACAD docs the defaults as */
	/* (-0.5,-0.5),(size.x-0.5,size.y-0.5): */
	pt[4][0]=pt[4][1]=-0.5; 
	pt[4][2]=0.0;
	pt[5][0]=pt[3][0]-0.5; 
	pt[5][1]=pt[3][1]-0.5; 
	pt[5][2]=0.0;
	
	if (elist[0]!=NULL) 
		{ 
		sds_relrb(elist[0]); 
		elist[0]=NULL; 
		}
	
	if ((elist[0]=sds_buildlist(
		RTDXF0,"IMAGE",
		90,0,            /* Back to 0 meaning R14 */
		10,pt[0],        /* See the point definitions above. */
		11,pt[1],
		12,pt[2],
		13,pt[3],
		340,idef_ename,   /* The IMAGEDEF */
		70,7,            /* Flags: Show IMAGE; */
		/*        Show IMAGE when not aligned with screen; */
		/*        Use clipping boundaries */
		280,0,            /* Clipping off */
		281,50,           /* Brightness */
		282,50,           /* Contrast */
		283,0,            /* Fade */
		360,idefr_ename,  /* The IMAGEDEF_REACTOR */
		71,1,            /* Clipping boundary is a rectangle */
		91,2,            /* Number of clipping vertices that follow */
		14,pt[4],        /* See the point definitions above. */
		14,pt[5],
		0))==NULL) { rc=-3; goto out; }
	
	/* Make it and get its ename: */
	nRes = sds_entmakex(elist[0],image_ename);
	if (nRes != RTNORM) 
		{ 
		rc=-4; 
		goto out; 
		}
	
	/* Modify elist[1] (the IMAGEDEF_REACTOR) so we can use it in an entmod */
	/* to make the IMAGEDEF_REACTOR point to this new IMAGE entity: */
	
	/* Add the IMAGEDEF_REACTOR's ename (-1 group) to the top of elist[1]. */
	/* (Could just entget the IMAGEDEF_REACTOR, but why bother?  This is easy.) */
	if ((trbp1=sds_newrb(-1))==NULL) 
		{ 
		rc=-3; 
		goto out; 
		}
	
	trbp1->resval.rlname[0]=idefr_ename[0];
	trbp1->resval.rlname[1]=idefr_ename[1];
	trbp1->rbnext=elist[1];
	elist[1]=trbp1;
	
	/* Find the 330 group (the dummy) and fill in the IMAGE entity ename: */
	for (trbp1=elist[1]; trbp1!=NULL && trbp1->restype!=330; trbp1=trbp1->rbnext);
	if (trbp1==NULL) 
		{ /* Should never. */
		rc=-6; 
		goto out; 
		}  
	trbp1->resval.rlname[0]=image_ename[0];
	trbp1->resval.rlname[1]=image_ename[1];
	
	/* Entmod the IMAGEDEF_REACTOR: */
	if (sds_entmod(elist[1])!=RTNORM) 
		{ 
		rc=-4; 
		goto out; 
		}
	
	
	/* The spaghetti is DONE (I hope). */

	sds_name ename;
	sds_entlast(ename);
	*hip = (db_handitem *)ename[0];
	return NOERROR;
	
out:
	/* Do the dishes: */
	if (elist[0]!=NULL) 
		{ 
		sds_relrb(elist[0]); 
		elist[0]=NULL; 
		}
	if (elist[1]!=NULL) 
		{ 
		sds_relrb(elist[1]); 
		elist[1]=NULL; 
		}
	
	return rc;
}

HRESULT CIcadDoc::AttachXRef (CString& FileName, CString& BlockName, sds_point ptOrigin, double xscale, double yscale, double zscale, double rotation, bool bOverlay, db_handitem **inserthip)
	{
	char blkfile[IC_ACADBUF], blkname[IC_ACADBUF], newblkname[IC_ACADBUF];
	sds_name ename;

	strcpy (blkname, FileName.GetBuffer(0));

	if (FindTheBlockFile(blkname,blkfile)!=RTNORM)				// find the file's true path using sds_findfile
		return E_FAIL;
	
	if (BlockName.IsEmpty() || (!ic_isvalidname(BlockName.GetBuffer(0))))
		{
		if (ProcessInsertBlockName(blkfile,newblkname,blkfile)!=RTNORM)	// takes blkname and returns block name, and path if any
			return E_FAIL;
		}
	else
		strcpy (newblkname, BlockName.GetBuffer(0));

	sds_resbuf *param
		=sds_buildlist(1, blkname , 2, newblkname, 10, ptOrigin, 41,xscale, 42, yscale, 43, zscale, 50, rotation, 0);
	short ret = xref_add_new(bOverlay,param);
	// (We has defined all parameters and therefore they will not be
	// requested from the user )
	sds_relrb(param);

	if (ret == RTERROR)
		return E_FAIL;

	sds_entlast(ename);
	*inserthip = (db_handitem *)ename[0];

	return NOERROR;

	}

/*
 The code for XRefOperations is extracted from cmds3.cpp - cmd_xref.
 Valid XRef operations
 1 - Reload
 2 - Unload
 3 - Bind
 4 - Detach
 5 - EditPath
 */
HRESULT CIcadDoc::XRefOperations (int Operation, CString FileName, db_handitem *blockhip)
	{
	switch (Operation)
		{
		case 1: //Reload
			{
			sds_name ssinserts, ename;
			long xrefcnt;
			char *name;
			struct resbuf *rb;

			blockhip->set_xrefdp(NULL);

			blockhip->get_2 (&name);
			/*====BUG:=================================================================== 
				In this place in old version there was a bug: need be "RTDXF0" instead of "0"
			old code:
				rb = sds_buildlist(0, "INSERT"/=DNT=/,2, name,0);
			============================================================================*/
			rb = sds_buildlist(RTDXF0, "INSERT"/*DNT*/,2, name,0);

			if (RTNORM != sds_pmtssget(NULL, "X"/*DNT*/, NULL, NULL, rb, ssinserts, 1))
				return E_FAIL;

			for (xrefcnt = 0L; RTNORM == sds_ssname(ssinserts, xrefcnt, ename); xrefcnt++)
				sds_entupd(ename);	//this reloads xref dependency

			sds_ssfree(ssinserts);

			return NOERROR;
			}
			break;

		case 2: //Unload
			{

			}
			break;

		case 3: //Bind
			{
			char *name;
			blockhip->get_2 (&name);

			if (cmd_xref_uncouple(name, IC_UNCOUPLE_BIND, true) < 0)
				return E_FAIL;
			else
				return NOERROR;
			}
			break;

		case 4: //Detach
			{
			char *name;
			blockhip->get_2 (&name);

			if (cmd_xref_uncouple(name, IC_UNCOUPLE_DETACH, true) < 0)
				return E_FAIL;
			else
				return NOERROR;
			}
			break;

		case 5: //EditPath
			{
			return E_NOTIMPL;
			}
			break;

		default:
			return E_FAIL;
		}


	return NOERROR;

	}

int ConvertSDSSelSetToHandItemCollection (sds_name selectionSet, db_drawing *dwg, HanditemCollection &collection)
	{
	int result = RTERROR;
	long length;
	sds_name entity;
	sds_sslength(selectionSet, &length);

	for (int i = 0; i < length; i++)
		{
		result = sds_ssname(selectionSet, i, entity);

		if (result == RTNORM)
			{	//don't need to filter the list for space or deleted flags since selected objects are already
			//correct
			collection.AddItem(dwg, (db_handitem*)(entity[0]));
			}
		}
	
	return result;
	}

HRESULT CIcadDoc::ApplyHatch (sds_name ssetin, db_hatch *hip)
	{
	HanditemCollection Ents;

	if (ConvertSDSSelSetToHandItemCollection(ssetin, this->m_dbDrawing, Ents) != RTNORM)
		return E_INVALIDARG;

	if (cmd_ApplyHatchToBoundryObjects(&Ents, hip))
		return S_OK;
	else
		return E_FAIL;
	}

HRESULT CIcadDoc::FindHatchPattern (CString PatternName)
	{
	cmd_hpdefll *patternDef = NULL;
	int numlines;

	bool ret = cmd_FindPatternDef(PatternName.GetBuffer(0), &numlines, &patternDef);
	if (patternDef)
		cmd_hatch_freehpdefll(&patternDef);

	return ((ret == true) ? S_OK : E_FAIL);
	}



