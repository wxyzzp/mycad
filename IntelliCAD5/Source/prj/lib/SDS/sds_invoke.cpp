//*********************************************************************
//*  sds_invoke.CPP                                                    *
//*  Copyright (C) 1997, 1998 by Visio Corporation                    *
//*                                                                   *
//*********************************************************************
// Support for sds_invoke and sds_getargs
// This code was written this way to fix a bug in sds_getargs():
//	The problem was that if sds_invoke() called a function in an sds app,
// and the sds app then called sds_command() or some other function, it very
// likely cleared out the lsp_argsll, which is where sds_getargs() was getting
// its return value.
//
//	By looking through the code it appeared that the thing to do was to keep
// track of the specific arguments passed to sds_invoke() in addition to the lisp
// arguments.  Then if lsp_argsll was cleared out, we could try and get the
// last invoke arguments anyhow.  When sds_invoke() exited it should get rid of the
// arguments and everything should be toasty.
// If lisp makes additional calls intermediate to the call to a particular function,
// lsp_argsll should be set right and sds_getargs() should get those--so this implementation
// only goes with the invoke args if it can' find the lsp_argsll
//
// To implement this I created a class private to this mode CInvokeHandler, that
// does the work of the old sds_invoke() and sds_getargs() but tracks additional
// info.  
// 
//
// Further problems:  re-entry of sds_invoke()
// If more than one thread calls into sds_invoke() or it is simply recursed, we're 
// going to blow it.  
// 
// Recursion could be fixed by making CInvokeHandler keep a stack of arguments, but
// this seemed excessive at this moment.
//
// Longer term, lsp_argsll should not be a global variable!
//
//*********************************************************************

//** Includes


#include "Icad.h"/*DNT*/
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "commandatom.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "commandqueue.h" /*DNT*/



extern int       SDS_FunctionID;
extern struct resbuf *lsp_argsll;  


typedef CArray< struct resbuf *, struct resbuf * > TArgumentStack;

// **********************************
// Hidden class
//
// CInvokeHandler
//
// Not used outside of this module
//
class CInvokeHandler
	{
	// ctors and dtors
	//
	public:
		CInvokeHandler( void );
		virtual ~CInvokeHandler( void );

	// operations
	//
	public:
		struct resbuf *	sds_getargs(void); 
		int sds_invoke(const struct sds_resbuf *prbArguments, struct sds_resbuf **pprbReturn);

	// *****************
	// internal helper methods
	//
	private:
		struct resbuf * retLastInvokeArguments( void );
		bool pushInvokeArguments( struct resbuf *prbArgs );
		bool popInvokeArguments( void );

	// internal data
	//
	private:
		TArgumentStack m_ArgumentStack;

	};

// *******************
// PRIVATE MODULE LEVEL VARIABLE
//
static CInvokeHandler zInvokeHandler;


// ********************************
// CTOR
//
CInvokeHandler::CInvokeHandler( void )
	{
	}

// *********************************
// DTOR
//
CInvokeHandler::~CInvokeHandler( void )
	{
	for( int i = 0; i < m_ArgumentStack.GetSize(); i++ )
		{
		::lsp_freesuprb( m_ArgumentStack[i] );
		m_ArgumentStack[i] = 0;
		}
	m_ArgumentStack.RemoveAll();
	}

// *********************************
// helper method -- class CInvokeHandler
//
// retLastInvokeArguments
//
struct resbuf *
CInvokeHandler::retLastInvokeArguments( void )
	{
	// We add new elements to the end of the array, so
	// return very last element
	//
	int i = m_ArgumentStack.GetSize();
	if ( i < 1 )
		{
		return NULL;
		}
	else
		{
		return m_ArgumentStack[ i - 1 ];
		}
	}

// *********************************
// helper method -- class CInvokeHandler
//
// popInvokeArguments
//
bool
CInvokeHandler::popInvokeArguments( void )
	{
	int i = m_ArgumentStack.GetSize();
	ASSERT( i > 0 );
	if ( i > 0 )
		{
		::lsp_freesuprb( m_ArgumentStack[ i-1 ] );
		m_ArgumentStack[ i-1 ] = NULL;
		m_ArgumentStack.RemoveAt( i - 1 );
		return true;
		}

	return false;
	}

// *********************************
// helper method -- class CInvokeHandler
//
// pushInvokeArguments
//
bool
CInvokeHandler::pushInvokeArguments( struct resbuf *prbArgs )
	{
	struct resbuf *temprb = ::sds_newrb(SDS_RTERROR);
	::lsp_copysplrb(temprb,prbArgs);
	m_ArgumentStack.Add( temprb );

	return true;
	}
// **********************************
// PUBLIC METHOD -- class CInvokeHandler
//
// sds_getargs
//
// Notice how we try to get the lsp arguments first,
// and only default to invoke arguments if lsp_getargs()
// finds none
//
struct resbuf *
CInvokeHandler::sds_getargs(void) 
	{
	struct resbuf *trb = NULL;

	// First try and get the lisp arguments
	//
	if(::lsp_getargs()!=NULL)
		{
	   	trb=::sds_newrb(0);
		::lsp_copysplrb(trb,lsp_getargs());
		}
	else

		{
		if ( this->retLastInvokeArguments() == NULL )
			{
			return NULL;
			}
		else
			{
		   	trb=::sds_newrb(0);
			::lsp_copysplrb(trb,this->retLastInvokeArguments());
			}
		}


	if(trb->restype==SDS_RTERROR) 
		{ 
		::sds_relrb(trb); 
		return(NULL); 
		}

	return(trb);
	}

// *******************************
// PUBLIC METHOD -- class CInvokeHandler
//
// sds_invoke
//
//
int 
CInvokeHandler::sds_invoke(const struct sds_resbuf *prbArguments, struct sds_resbuf **pprbReturn) 
	{

	int iRetval = RTNORM;

    int (*fnptr)(void);
    SDSApplication *ptempApp,*pcurApp;
	class commandAtomObj *alink;

    if(prbArguments==NULL || 
		prbArguments->restype!=RTSTR) 
		{
		return(RTERROR);
		}
    if(NULL==(alink=::lsp_findatom(prbArguments->resval.rstring,0))) 
		{
		return(RTERROR);
		}
#ifdef LOCALISE_ISSUE
// Do these need translating - the command itself will be translated.
// Don't know about the thread id though.
#endif
    // Find the thread this function belongs to.
	if(strisame(ResourceString(IDC_SDS_INVOKE_ICAD_COLORDLG_0, "icad_colordlg" ) ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ICAD_HELPDLG_1, "icad_helpdlg" )  ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ICAD_SORTLIST_2, "icad_sortlist" ) ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ICAD_STRLSORT_3, "icad_strlsort" ) ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ACAD_STRLSORT_4, "acad_strlsort" ) ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ACAD_COLORDLG_5, "acad_colordlg" ) ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ACAD_HELPDLG_6, "acad_helpdlg" )  ,prbArguments->resval.rstring) || 
	   strisame(ResourceString(IDC_SDS_INVOKE_ACAD_SORTLIST_7, "acad_sortlist" ) ,prbArguments->resval.rstring)) 
		{ 
		ptempApp=SDSApplication::GetEngineApplication();
		} 
	else 
		{
		if(alink->rb.restype!=LSP_RTXSUB) 
			return(RTERROR);

		for(ptempApp=SDSApplication::GetEngineApplication(); ptempApp!=NULL; ptempApp=ptempApp->GetNextLink()) 
			{
			if(ptempApp->GetMainThread()->IsSame( alink->ThreadID ) )
				break;
			}
		}


    // Better have found the thread.
    if(ptempApp==NULL) 
		return(RTERROR);
    
	// We put the arguments into lsp_argsll
	//
	struct resbuf *trb = ::sds_newrb(SDS_RTERROR);
	::lsp_copysplrb(trb,prbArguments->rbnext);
    ::lsp_freesuprb(lsp_argsll);
	lsp_argsll=trb;

	// But because lsp_argsll gets cleared out so often by
	// commands and other things, we also store it here, so that
	// sds_getargs() is more likely to be able to get the right arguments
	//
	this->pushInvokeArguments( trb );
	
    fnptr=LSP_FNPCAST (alink->rb.resval.rstring);
    if((alink->rb.restype==LSP_RTSUBR || 
		alink->rb.restype==LSP_RTXSUB) && 
		fnptr) 
		{
		(*fnptr)();
		} 
	else 
		{
		if(alink->HasFuncPtr()) 
			{
			// Call the callback type functions.
			alink->CallFunction();
			} 
		else 
			{
			SDS_FunctionID=alink->id;
			pcurApp=SDSApplication::GetActiveApplication();

			ASSERT( pcurApp != ptempApp );

			// Can't invoke a function in your own DLL unless it has been sds_regfunc'd, in 
			// which case it has a fnptr and went through above.  So we return RTERROR
			//
			ptempApp->GetCommandQueue()->Copy( pcurApp->GetCommandQueue() );
			pcurApp->GetCommandQueue()->Flush();

			::SDS_PushLastThread( pcurApp );
			// ****************************************
			// THREADSWAP !!!!!!!!!!!!!!!!!
				{
				SDSApplication *pStartApp = ptempApp;
				SDSApplication *pStopApp = pcurApp;
	
				::SDS_ThreadSwap( pStartApp->GetMainThread(),pcurApp->GetMainThread());		
				}
			}
		}


	this->popInvokeArguments();

	// Where did lsp_argsll get deallocated????
	//
	lsp_argsll=NULL;

	if((iRetval == RTNORM) &&
		(pprbReturn != NULL)) 
		{
		*pprbReturn=::sds_newrb(0);
		struct resbuf *trb;
		extern struct resbuf lsp_evres;  /* A SPECIAL "SUPER" RESBUF -- SEE NOTE ABOVE */
		// We need to remove the RTLB and RTLE if the invoked function called sds_rellist()
		if(lsp_evres.restype==RTVOID || 
			lsp_evres.restype==RTNIL) 
			{ 
			*pprbReturn=NULL;
			} 
		else 
			{
			if(lsp_evres.restype==RTLB) 
				{ 
				if(lsp_evres.rbnext->restype==RTLE) 
					{
					*pprbReturn=NULL;
					} 
				else 
					{
					::lsp_copysplrb(*pprbReturn,lsp_evres.rbnext);
					for(trb=*pprbReturn; trb; trb=trb->rbnext) 
						{
						if(trb->rbnext && 
							trb->rbnext->restype==RTLE && 
							trb->rbnext->rbnext==NULL) 
							{
							::sds_relrb(trb->rbnext);
							trb->rbnext=NULL;
							break;
							}
						}
					}
				} 
			else 
				{
				::lsp_copysplrb(*pprbReturn,&lsp_evres);
				}
			}
		}
    return(iRetval);
	}


// **********************************
// PUBLIC FUNCTION
//
// sds_getargs
//
// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- how do we handle this?
//
struct resbuf *
sdsengine_getargs(void) 
	{

	return zInvokeHandler.sds_getargs();
	}

// *******************************
// PUBLIC FUNCTION
//
// sds_invoke
//
// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- how do we handle this?
//
int 
sdsengine_invoke(const struct sds_resbuf *prbArguments, struct sds_resbuf **pprbReturn) 
	{

	return zInvokeHandler.sds_invoke( prbArguments, pprbReturn );
	}


