/* C:\ICADDEV\PRJ\ICAD\ICADUNDOREDO.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


// ** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadGlobals.h"

#include "IcadDoc.h"
#include "IRasDef.h"/*DNT*/
#include "cvisioraster.h"/*DNT*/
#include "BinChange.h"
#include "HatchChange.h"

#include "vxtabrec.h"
#include "objects.h"
#include "IcadView.h"

//#pragma warning(disable:4146)

#define	UNDO_KEEPASSERT
#include "undo\Undo.h"
#include "undo\UndoMgr.h"
#include "undo\ParentUndoUnit.h"
#include <map>


#ifdef _DEBUG_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef	_ATL_APARTMENT_THREADED
#error
#endif



// *******************************
// MODULE LEVEL class definitions
//


struct SDS_UndoListDef
	{
	char mode;		/*	1=Undo Mark 	   */
					/*	2=Undo Group/BEgin */
					/*	3=Undo End		   */
					/*	4=Setvar Change  [0]=Varname [1]=resbuf */
					/*	5=Redraw drawing on undo */
					/*	6=Regen drawing on undo */
					/*	7=Redraw drawing on redo */
					/*	8=Regen drawing on redo */
					/*	9=Entity/Table/Class New	 [0]=Prev LastEnt [1]=New LastEnt [2]=Free ok */
					/* 10=Entity/Table/Class Change    */
					/* 11=Entity/Table/Class Delete  [0]=DelEnt  */
					/* 12=Command Begin  [0]=Command name  */
					/* 13=Command End	 [0]=Command name	 */
					/* 14=Zoom change	 [0]=SDS_prevview	 */
					/* 15=Redraw ent	 [0]=Refresh ent   (special for chamfer/fillet) */
					/* 16=Do nothing */
					/* 17=Hide on redo */
					/* 18=Shade on redo */
					/* 19=CIcadView Create [0]=pView */
					/* 20=CIcadView Size [0]=X pos [1]=Y pos */
					/* 21=CIcadView Move [0]=X pos [1]=Y pos */
					/* 22=CIcadView Delete [0]=CIcadView [1]=vp hip */
					/* 23=MDI Restore */
					/* 24=MDI Maxamize */
					/* 25=PS/MS Viewport change [0]=Pre viewtab*/
					/* 26=Redraw drawing on undo (one VP) */
					/* 27=Regen drawing on undo (one VP) */
					/* 28=Redraw drawing on redo (one VP) */
					/* 29=Regen drawing on redo (one VP) */
					/* 30=Redefine block [0]=blk tab hip [1]=blk bhip [2]=blk ehip */
					/* 31=Image [0]=sds_name entity [2] char* handle */
	bool undid;
	char flag;		// tabindex (eg, DB_BLOCKTAB) if we deal with tables, otherwise -1
	void *data[3];

	SDS_UndoListDef() : mode(0),undid(0),flag(0)
		{
		data[0] = data[1] = data[2] = 0;
		}
	};

	// Image Undo helpers
enum ImageLoc {IMAGEUNDO, IMAGEREDO};

void ImageAlert(IRas::ErrorCode error, ImageLoc loc);
void ImageRedraw(sds_name name);


bool	SDS_CurrentlyUndoing = false;
/*DG - 17.1.2002*/// Some situations require to have this flag in UndoList of every drawing.
// eg, execution of WCLOSE leaves it non-zero.
//static int	command_end_pending=0;	// semaphore to match BEGIN/END
static int s_tmpGroupLevel;

// Service class optimizing display during undo/redo
class CUndoDispHelper
{
public:
	CUndoDispHelper(){ reset(); }
	void reset() { memset(m_data, 0, sizeof(m_data)); }
	void regen() { m_data[eRegen]++; }
	void redraw() { m_data[eRedraw]++; }
	void regenAll() { m_data[eRegenAll]++; }
	void redrawAll() { m_data[eRedrawAll]++; }
	void process()
	{
		const bool bSaveSDS_CurrentlyUndoing = SDS_CurrentlyUndoing;
		SDS_CurrentlyUndoing = true;
		const bool bSaveSDS_DontBitBlt = SDS_DontBitBlt;
		SDS_DontBitBlt = true;
		if(m_data[eRegenAll])
		{
			cmd_regenall();
		}
		else
		{
			if(m_data[eRegen])
				cmd_regen();
			if(m_data[eRedrawAll])
				cmd_redrawall();
			else if(m_data[eRedraw] && !m_data[eRegen])
				cmd_redraw();
		}
		SDS_CurrentlyUndoing = bSaveSDS_CurrentlyUndoing;
		SDS_DontBitBlt = bSaveSDS_DontBitBlt;
		reset();
	}

private:
	enum EAction
	{
		eRegen,
		eRedraw,
		eRegenAll,
		eRedrawAll,
		eLast
	};

private:
	int m_data[eLast];
};

// This list is owned by the drawing, which must delete it
class UndoList : public AbstractDelete
{
public:
	UndoList();
	~UndoList();
	void* TruePointer() { return this; }
	operator IOleUndoManager*() const {return m_undoMgr;}

	HRESULT	Open();
	HRESULT	Close( bool commit);
	int UndoGroup(db_drawing *dp, bool bUndo, int nGroups);
	int UndoBack(db_drawing *dp);

	class GenericUnit* m_commandBegin;
	/*DG - 17.1.2002*/// semaphore to match COMMAND BEGIN/END;
	// added by moving the global analog (see above)
	int	m_command_end_pending;	
	CUndoDispHelper m_dispHelper;

protected:
	bool RedoEmpty() const;
	bool UndoEmpty() const;
	int UndoGroup(db_drawing *dp, bool bUndo);

protected:
	CComPtr< IOleUndoManager>		m_undoMgr;
};

typedef	HRESULT	TRESULT;
#define	VERIFY_SUCCEEDED( x )	VERIFY( SUCCEEDED( x))


HRESULT
UndoList::Open()
	{
	HRESULT	result;

	VERIFY( SUCCEEDED(
			result = m_undoMgr->Open( NULL)
			));
	return result;
	}


HRESULT
UndoList::Close( bool commit)
	{
	HRESULT	result;

	VERIFY( SUCCEEDED(
			result = m_undoMgr->Close( NULL, commit)
			));
	return result;
	}


static bool
EnumeratorEmpty( IEnumOleUndoUnits *units)
	{
	IOleUndoUnit	*temp;
	ULONG			count;

	VERIFY_SUCCEEDED( units->Next( 1, &temp, &count) );
	if ( !count )
		return true;

	temp->Release();
	return false;
	}


bool
UndoList::RedoEmpty() const
	{
	CComPtr< IEnumOleUndoUnits>	units;
	VERIFY_SUCCEEDED( m_undoMgr->EnumRedoable( &units));

	return EnumeratorEmpty( units);
	}


bool
UndoList::UndoEmpty() const
	{
	CComPtr< IEnumOleUndoUnits>	units;

	VERIFY_SUCCEEDED( m_undoMgr->EnumUndoable( &units));

	return EnumeratorEmpty( units);
	}


int
UndoList::UndoGroup(db_drawing *dp, bool bUndo, int nGroups)
{
	int rc = RTNORM;
	m_dispHelper.reset();
	if(SDS_CURDOC)
	{
		ASSERT(SDS_CURDOC->m_dbDrawing == dp);
		SDS_CURDOC->m_drawHelper.reset();
		SDS_CURDOC->m_drawHelper.setEnable(true);
	}
	try
	{
		for(nGroups = nGroups > 0 ? nGroups : 0; (rc == RTNORM) && nGroups--;)
		{
			rc = UndoGroup(dp, bUndo);
		}
		m_dispHelper.process();
		if(SDS_CURDOC)
		{
			ASSERT(SDS_CURDOC->m_dbDrawing == dp);
			SDS_CURDOC->m_drawHelper.process(SDS_CURDOC);
		}
	}
	catch(...)
	{
		if(SDS_CURDOC)
		{
			SDS_CURDOC->m_drawHelper.setEnable(false);
			SDS_CURDOC->m_drawHelper.process(SDS_CURDOC);
			SDS_CURDOC->m_drawHelper.reset();
		}
		throw;
	}
	SDS_BitBlt2Scr(1);
	return rc;
}

int
UndoList::UndoGroup(db_drawing *dp, bool bUndo)
{
	if(bUndo ? UndoEmpty() : RedoEmpty())
	{
		resbuf	cmdechoRb;
		SDS_getvar(0, DB_QCMDECHO, &cmdechoRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(CMD_CMDECHO_PRINTF(cmdechoRb.resval.rint) && !SDS_DoingLispCode)
			cmd_ErrorPrompt(bUndo ? CMD_ERR_NOUNDO : CMD_ERR_NOREDO, 0);
		return RTNONE;
	}

	CComPtr<IOleUndoManager> pUndoMgr = *this;
	ASSERT(pUndoMgr);
	if(pUndoMgr)
	{
		if(static_cast<CUndoMgr*>(&*pUndoMgr)->getAutoSave())
		{
			static_cast<CUndoMgr*>(&*pUndoMgr)->setAutoSave(false);
			resbuf	savetimeRb;
			SDS_getvar(NULL, DB_QSAVETIME, &savetimeRb, dp, &SDS_CURCFG, &SDS_CURSES);
			if(savetimeRb.resval.rint > 0)
			{
				extern int		SDS_AutoSave;
				writeWarningMessage(4);
				SDS_AutoSave = 1;
				SDS_CMainWindow->m_pvWndAction = (void*)SDS_CURDOC;
				SDS_CMainWindow->m_strWndAction = SDS_CURDOC->GetTitle();
				ExecuteUIAction(ICAD_WNDACTION_FILESAVE);
				SDS_AutoSave = 0;
			}
		}
		SDS_DontBitBlt = SDS_CurrentlyUndoing = true;
		try		/*DG 2.10.2001*/// For restoring of SDS_DontBitBlt, SDS_CurrentlyUndoing in the case of an exception.
		{
			if(bUndo)
				pUndoMgr->UndoTo(0);
			else 
				pUndoMgr->RedoTo(0);
		}
		catch(...)
		{
			SDS_DontBitBlt = SDS_CurrentlyUndoing = false;
			throw;
		}
		SDS_DontBitBlt = SDS_CurrentlyUndoing = false;

		/*D.G.*/// Take care about UNDOCTL's GroupActive bit.
		resbuf	undoctlRb;
		SDS_getvar(0, DB_QUNDOCTL, &undoctlRb, dp, &SDS_CURCFG, &SDS_CURSES);
		if(static_cast<CUndoMgr*>(&*pUndoMgr)->getGroupLevel())
			undoctlRb.resval.rint |= IC_UNDOCTL_GROUPACTIVE;
		else
			undoctlRb.resval.rint &= ~IC_UNDOCTL_GROUPACTIVE;
		SDS_setvar(0, DB_QUNDOCTL, &undoctlRb, dp, &SDS_CURCFG, &SDS_CURSES, 0);
	}
	return RTNORM;
}

int
UndoList::UndoBack(db_drawing *dp)
{
	CComPtr< IOleUndoManager> undoMgr;
	undoMgr = *this;

	// enumerate undo units
	CComPtr< IEnumOleUndoUnits>	units;
	VERIFY_SUCCEEDED( undoMgr->EnumUndoable( &units));

	LONG undoType = -1;	/*D.G.*/// For safety.
	bool done = false;
	// count units preceding and including MARK
	int	count;
	for(count = 0; !done; )
	{
		CComPtr< IOleUndoUnit>	undoUnit;
		VERIFY_SUCCEEDED( units->Next( 1, &undoUnit, NULL) );
		if(undoUnit)
		{
			++count;
			CLSID	classId;
			VERIFY_SUCCEEDED( undoUnit->GetUnitType( &classId, &undoType) );
			if ( undoType == IC_UNDO_MARK || undoType == IC_UNDO_GROUP_BEGIN)	/*D.G.*/// IC_UNDO_GROUP_BEGIN added.
				done = true;
		}
		// done if no MARK
		else
			done = true;
	}
	return UndoGroup(dp, true, count);
}


/**************************************************************************/
/***************** UNDO/REDO Functions ************************************/
/**************************************************************************/

	// utility class for managing string tokens in undo list
	// command and variable names are tokenized to eliminate redundant allocations
class UndoTokens
	{
public:
	typedef CMap< LPCTSTR, LPCTSTR, int, int> TokenMap;
	typedef CArray< CString, CString> TokenArray;

	static int		AddToken( LPCTSTR token);
	static LPCTSTR	RetrieveToken( int);
	static void		Initialize();
	static void		Release();

private:
	static	TokenMap	*m_map;
	static	TokenArray	*m_array;
	static	int			m_useCount;
	};


UndoTokens::TokenMap*	UndoTokens::m_map = NULL;
UndoTokens::TokenArray*	UndoTokens::m_array = NULL;
int						UndoTokens::m_useCount = 0;


	// string comparison function for map
BOOL AFXAPI CompareElements(LPCTSTR* pElement1, LPCTSTR* pElement2)
	{
	return !_tcscmp( *pElement1, *pElement2);
	}

	// create and delete token map as needed
void
UndoTokens::Initialize()
	{
	if ( !m_map )
		{
		ASSERT( m_useCount EQ 0 );
		m_map = new TokenMap;
		m_map->InitHashTable( 911);		// TBD make this dynamic

		ASSERT( !m_array );
		m_array = new TokenArray;
		}

	m_useCount += 1;
	}


void
UndoTokens::Release()
	{
	if ( --m_useCount EQ 0 )
		{
		delete m_map;
		m_map = NULL;
		delete m_array;
		m_array = NULL;
		}
	}


int
UndoTokens::AddToken( LPCTSTR token)
	{
	int	result;
	if ( !m_map->Lookup( token, result) )
		{
								// first add to dynamic array so we can index it
								// and so map refers to a known string
								// (token may not be valid later)
		result = m_array->Add( token);
		token = (*m_array)[ result];
		m_map->SetAt( token, result);

		int	verify;
		VERIFY( m_map->Lookup( token, verify) );
		}

		// This assumes that the value string remains valid until the map is destroyed
	return result;
	}


LPCTSTR
UndoTokens::RetrieveToken( int index)
	{
		// undo tokens can get deleted first in destructors
	if ( !m_array )
		return NULL;

	ASSERT( index < m_array->GetSize() );
	return (*m_array)[ index];
	}


static UndoList*
GetUndoList(db_drawing *flp)
	{
	UndoList *undoList;

	if ( flp->m_undoList )
		undoList = (UndoList *)flp->m_undoList->TruePointer();

	else if ( undoList= new UndoList() )
		{
		flp->m_undoList = undoList;
		UndoTokens::Initialize();	// token list
		}

	ASSERT( undoList);
	return undoList;
	}


int
SDS_UndoGroup(db_drawing *dp, int nGroup)
{
	UndoList* pUndoList = GetUndoList(dp);
	return pUndoList ? pUndoList->UndoGroup(dp, true, nGroup) : RTNORM;
}

int
SDS_RedoGroup(db_drawing *dp)
{
	UndoList* pUndoList = GetUndoList(dp);
	return pUndoList ? pUndoList->UndoGroup(dp, false, 1) : RTNORM;
}

int
SDS_UndoBack(db_drawing *dp)
{
	UndoList* pUndoList = GetUndoList(dp);
	return pUndoList ? pUndoList->UndoBack(dp) : RTNORM;
}

//	UndoUnit
//	provides basic implementation of IOleUndoUnit for all undo tasks
class UndoUnit : public IOleUndoUnit
	{
public:
	UndoUnit() : m_count(0), m_action( UNDO) {}
	virtual ~UndoUnit()	  {}

	//	IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	// IOleUndoUnit
	HRESULT STDMETHODCALLTYPE Do( IOleUndoManager *pUndoManager);
	HRESULT STDMETHODCALLTYPE GetDescription( BSTR *pBstr) = 0;
	HRESULT STDMETHODCALLTYPE GetUnitType( CLSID *pClsid, LONG *plID) = 0;
	HRESULT STDMETHODCALLTYPE OnNextAdd();

	//	leaf actions
	virtual HRESULT STDMETHODCALLTYPE Redo() = 0;
	virtual HRESULT STDMETHODCALLTYPE Undo() = 0;

protected:
	ULONG	m_count;
	enum { UNDO, REDO} m_action;
	};


STDMETHODIMP
UndoUnit:: QueryInterface( REFIID riid, void **ppvObject)
	{
	*ppvObject = NULL;
	return E_NOTIMPL;
	}


STDMETHODIMP_( ULONG)
UndoUnit:: AddRef()
	{
	m_count += 1;
	return m_count;
	}


STDMETHODIMP_( ULONG)
UndoUnit:: Release()
	{
	m_count -=1;

	ULONG	current = m_count;
	if ( current EQ 0 )
		delete this;

	return current;
	}

	// All IntelliCAD undo units are reversible
	// The state is inverted and the unit added back to the manager (on the other stack)
STDMETHODIMP
UndoUnit:: Do( IOleUndoManager *pUndoManager)
	{
	HRESULT	result;

	s_tmpGroupLevel = static_cast<CUndoMgr*>(&*pUndoManager)->getGroupLevel();		/*D.G.*/

	switch ( m_action )
		{
		case REDO:
			{
			result = Redo();
			m_action = UNDO;
			break;
			}
		case UNDO:
			{
			result = Undo();
			m_action = REDO;
			break;
			}
		}

	if ( SUCCEEDED( result))
		{
		result = pUndoManager->Add( this);
		static_cast<CUndoMgr*>(&*pUndoManager)->setGroupLevel(s_tmpGroupLevel);		/*D.G.*/
		}

	return result;
	}

STDMETHODIMP
UndoUnit:: OnNextAdd()
	{
	return E_NOTIMPL;
	}


//	GenericUndo
//
//	provides a hook to oldstyle undo actions which are not worth upgrading

class GenericUnit :  public UndoUnit
	{
public:
	GenericUnit( int mode,void *data0,void *data1,void *data2, db_drawing *flp);
	~GenericUnit();

	HRESULT STDMETHODCALLTYPE Redo();
	HRESULT STDMETHODCALLTYPE Undo();
	HRESULT STDMETHODCALLTYPE GetUnitType(CLSID *pClsid, LONG *plID);
	HRESULT STDMETHODCALLTYPE GetDescription(BSTR* pBstr);

	SDS_UndoListDef	m_data;
	db_drawing		*m_flp;

protected:
	void regen() { GetUndoList(m_flp)->m_dispHelper.regen(); }
	void regenAll() { GetUndoList(m_flp)->m_dispHelper.regenAll(); }
	void redraw() { GetUndoList(m_flp)->m_dispHelper.redraw(); }
	void redrawAll() { GetUndoList(m_flp)->m_dispHelper.redrawAll(); }
	};

UndoList::UndoList() : m_commandBegin( NULL), m_command_end_pending(0)
	{
	CComObject< CUndoMgr>	*instance = NULL;

	VERIFY( SUCCEEDED(
			CComObject< CUndoMgr>::CreateInstance(&instance)
			));

	VERIFY( SUCCEEDED(
			instance->QueryInterface( IID_IOleUndoManager, (void**)&m_undoMgr)
			));
	}


UndoList::~UndoList()
	{
	m_undoMgr.Release();	// release units first, before tokens are released
	UndoTokens::Release();

	delete m_commandBegin;
	}



void
newSDS_SetUndo(int mode,void *data0,void *data1,void *data2,db_drawing *flp)
	{
							// locate (or create if necessary) the undo list for the drawing
	UndoList	*undoList = GetUndoList( flp);

							// Get UndoManager from the list
	CComPtr< IOleUndoManager>	undoMgr;
	if ( undoList )
		undoMgr = *undoList;

	ASSERT( undoMgr);
	if ( undoMgr )
		{
		IOleUndoUnit	*unit = NULL;
		int	groupLevel = static_cast<CUndoMgr*>(&*undoMgr)->getGroupLevel();
		static_cast<CUndoMgr*>(&*undoMgr)->setAutoSave(false);

//TRACE( "mode = %d, String = %s\n", mode, (LPCTSTR)data0 );	// Remove this at some happy day...
		switch ( mode )
			{
			case IC_UNDO_COMMAND_BEGIN:
				{
								// defer addition of BEGIN in case there is a command error
//				ASSERT( !undoList->m_commandBegin );	/*D.G.*/
				if(undoList->m_commandBegin)			// We allow this case now.
					{
					VERIFY( SUCCEEDED( undoList->Open() ));
					VERIFY(SUCCEEDED( undoMgr->Add( undoList->m_commandBegin) ));
					undoList->m_commandBegin = NULL;
					}

				undoList->m_commandBegin = new GenericUnit( mode, data0, data1, data2, flp);

				++(undoList->m_command_end_pending);
				return;
				}

			case IC_UNDO_COMMAND_END:
				{
					// This flag allows us to issue COMMAND_END from more than 1 place.
					// With some dialogs (like Exporer), data is coming from several threads
					// and we need to make sure the BEGIN/END pairs are in synch.
				if ( !undoList->m_command_end_pending )
					return;

				--(undoList->m_command_end_pending);

								// avoid putting empty BEGIN/END bracket into undo stack
				if ( undoList->m_commandBegin )
					{
					delete undoList->m_commandBegin;
					undoList->m_commandBegin = NULL;
					return;
					}
				}

				// *** FALL THROUGH if not UNDO and not empty

			case IC_UNDO_GROUP_END:
				{

				// just return if there was no begining bracket for "undo"
				if(!groupLevel && mode == IC_UNDO_GROUP_END)
					return;

				unit = new GenericUnit( mode, data0, data1, data2, flp);

				ASSERT( unit);
				if ( unit )
					VERIFY(SUCCEEDED( undoMgr->Add( unit) ));

				if(mode == IC_UNDO_COMMAND_END)		/*D.G.*/// We don't open parent units on IC_UNDO_GROUP_BEGIN.
				{									// So don't close anything in this case.
					VERIFY_SUCCEEDED(undoList->Close(true));
					HRESULT	hr = static_cast<CUndoMgr*>(&*undoMgr)->checkStacks(CUndoMgr::eCmdEnd, data0);
					if(hr)
					{
						static_cast<CUndoMgr*>(&*undoMgr)->setAutoSave(true);
						ASSERT(hr > 0L);
					}
				}
				else
				{									/*D.G.*/// Instead of closing we collect all last units since IC_UNDO_GROUP_BEGIN.
					VERIFY_SUCCEEDED(static_cast<CUndoMgr*>(&*undoMgr)->GroupUnits());
					static_cast<CUndoMgr*>(&*undoMgr)->setGroupLevel(--groupLevel);
				}


				if(!groupLevel && mode == IC_UNDO_GROUP_END)
					{
					resbuf	undoctl;
					SDS_getvar(NULL, DB_QUNDOCTL, &undoctl, flp, &SDS_CURCFG, &SDS_CURSES);
					undoctl.resval.rint &= ~IC_UNDOCTL_GROUPACTIVE;		/*D.G.*/// Clear GroupActive bit.
					SDS_setvar(NULL, DB_QUNDOCTL, &undoctl, flp, &SDS_CURCFG, &SDS_CURSES, 0);
					}

				return;	// return directly, since we had to add unit before Close
				}

			default:
				{
								// need to open command if this is first "real" data
				if ( undoList->m_commandBegin )
					{
					VERIFY( SUCCEEDED( undoList->Open() ));
					VERIFY(SUCCEEDED( undoMgr->Add( undoList->m_commandBegin) ));
					undoList->m_commandBegin = NULL;
					}

				switch ( mode )
					{
					case IC_UNDO_GROUP_BEGIN:
						{
						/*D.G.*/// According to the new strategy we don't open new parent unit on IC_UNDO_GROUP_BEGIN.
						// So I commented this out:
						/*VERIFY( SUCCEEDED(
								undoList->Open()
								));*/

						static_cast<CUndoMgr*>(&*undoMgr)->setGroupLevel(++groupLevel);
						}

					case IC_UNDO_MARK:
					case IC_UNDO_SETVAR_CHANGE:
					case IC_UNDO_NEW_ENT_TAB_CLS:
					case IC_UNDO_CHG_ENT_TAB_CLS:
					case IC_UNDO_DEL_ENT_TAB_CLS:
					case IC_UNDO_ZOOM:
					case IC_UNDO_NOP:
					case IC_UNDO_IMAGE:
					case IC_UNDO_REDEFINE_BLOCK:

					case IC_UNDO_ENTITY_REDRAW:
					case IC_UNDO_HIDE_ON_REDO:
					case IC_UNDO_SHADE_ON_REDO:
					case IC_UNDO_CICADVIEW_CREATE:
					case IC_UNDO_CICADVIEW_SIZE:
					case IC_UNDO_CICADVIEW_MOVE:
					case IC_UNDO_CICADVIEW_DELETE:
					case IC_UNDO_MDI_RESTORE:
					case IC_UNDO_MDI_MAXIMIZE:
					case IC_UNDO_PS_MS_VP_CHANGE:
					case IC_UNDO_REDRAW_VP_ON_UNDO:
					case IC_UNDO_REGEN_VP_ON_UNDO:
					case IC_UNDO_REDRAW_VP_ON_REDO:
					case IC_UNDO_REGEN_VP_ON_REDO:
					case IC_UNDO_REDRAW_ON_UNDO:
					case IC_UNDO_REGEN_ON_UNDO:
					case IC_UNDO_REDRAW_ON_REDO:
					case IC_UNDO_REGEN_ON_REDO:
						unit = new GenericUnit( mode, data0, data1, data2, flp);
						break;
					}
				}
			}

		ASSERT( unit);
		if ( unit )
			{
			VERIFY(SUCCEEDED( undoMgr->Add( unit) ));
			if(groupLevel && mode == IC_UNDO_GROUP_BEGIN)
				{
				resbuf	undoctl;
				SDS_getvar(NULL, DB_QUNDOCTL, &undoctl, flp, &SDS_CURCFG, &SDS_CURSES);
				undoctl.resval.rint |= IC_UNDOCTL_GROUPACTIVE;		/*D.G.*/// Set GroupActive bit.
				SDS_setvar(NULL, DB_QUNDOCTL, &undoctl, flp, &SDS_CURCFG, &SDS_CURSES, 0);
				}
			}
		}
	}



int
SDS_SetUndo(int mode, void* data0, void* data1, void* data2, db_drawing* flp)
{
	// UNDO list can't be located if no drawing
	if(!flp)
		return RTNORM;

	resbuf	rb;
	bool	createUndo = true;

	SDS_getvar(NULL, DB_QUNDOCTL, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

	if(SDS_CurrentlyUndoing || !(rb.resval.rint & IC_UNDOCTL_ON) ||
	   cmd_InsideOpenCommand || cmd_InsideNewCommand || cmd_InsideCreatePaperspace ||
	   ((mode == IC_UNDO_COMMAND_BEGIN || mode == IC_UNDO_COMMAND_END) &&
		(*((char*)data0) == '(' 				||
		 strisame((char*)data0, "NEW"/*DNT*/)	||
		 strisame((char*)data0, "OPEN"/*DNT*/)	||
		 strisame((char*)data0, "U"/*DNT*/) 	||
		 strisame((char*)data0, "UNDO"/*DNT*/)	||
		 strisame((char*)data0, "REDO"/*DNT*/)	||
		 strisame((char*)data0, "EXIT"/*DNT*/)	||
		 strisame((char*)data0, "_NEW"/*DNT*/)	||
		 strisame((char*)data0, "_OPEN"/*DNT*/) ||
		 strisame((char*)data0, "_U"/*DNT*/)	||
		 strisame((char*)data0, "_UNDO"/*DNT*/) ||
		 strisame((char*)data0, "_REDO"/*DNT*/) ||
		 strisame((char*)data0, "_EXIT"/*DNT*/) ||
		 (!*(char*)data0 && SDS_CmdHistCur &&
		  (strisame(SDS_CmdHistCur->cmd, "U"/*DNT*/)	||
		   strisame(SDS_CmdHistCur->cmd,"REDO"/*DNT*/)	||
		   strisame(SDS_CmdHistCur->cmd, "_U"/*DNT*/)	||
		   strisame(SDS_CmdHistCur->cmd,"_REDO"/*DNT*/)
		  )
		 )
		)
	   )
	  )
		createUndo = false;

	if(createUndo)
		newSDS_SetUndo(mode, data0, data1, data2, flp);
	else
	{
		// if UNDO is suppressed, it is still necessary to go through the motions of
		// creating and destroying the UNDO unit, since the data pointers may contain
		// things like allocated memory which must be freed
			// make sure this file has valid undo list
			// (for undo tokens)
		UndoList*	undoList = GetUndoList(flp);
		GenericUnit(mode, data0, data1, data2, flp);
	}

	return RTNORM;
}


GenericUnit:: GenericUnit( int mode,void *data0,void *data1,void *data2, db_drawing *flp)
		: m_flp( flp)
	{
	struct SDS_UndoListDef *ul = &m_data;
	ul->mode=mode;

	switch(ul->mode)
		{
		case IC_UNDO_COMMAND_BEGIN: // Command Begin.
		case IC_UNDO_COMMAND_END: // Command End.
			{
			char *str=(char *)data0;
			ul->data[0] = ul->data[1] = NULL;

			if(str)
				{
				// Check for just return "" in the command.
				if(*str==0 && SDS_CmdHistCur)
					str = SDS_CmdHistCur->cmd;

				int	tokenIndex = UndoTokens::AddToken( str);
				ul->data[0] = (void *)tokenIndex;

								// checking that tokens always set properly
				if ( Debug )
					{
					ul->data[1]= new BYTE [strlen(str)+1];
					strcpy((char *)ul->data[1],str);
					}
				}
			}
			break;


		case IC_UNDO_MARK:	// Mark
		case IC_UNDO_GROUP_BEGIN:	 // Group/BEgin
		case IC_UNDO_GROUP_END:	 // End
			break;

		case IC_UNDO_SETVAR_CHANGE:	 // Setvar Change.
			{
			ul->data[0]=data0;

			ul->data[1]=(void *)db_alloc_resbuf();
			memcpy(ul->data[1],data1,sizeof(struct resbuf));
			if(((struct resbuf *)data1)->restype==RTSTR)
				{
				((struct resbuf *)ul->data[1])->resval.rstring= new char [strlen(((struct resbuf *)data1)->resval.rstring)+1];
				strcpy(((struct resbuf *)ul->data[1])->resval.rstring,((struct resbuf *)data1)->resval.rstring);
				}
			}
			break;

		case IC_UNDO_NEW_ENT_TAB_CLS:	 // Entity/Table/Class New.
			{
			ul->data[0]=(void *)data0;
			db_handitem *elp=(db_handitem *)data1;
			if(elp->ret_type()==DB_SEQEND)
				{
				((db_seqend *)elp)->get_mainent(&elp);
				}
			ul->data[1]=(void *)elp;
			ul->flag=(char)data2;
			}
			break;


		case IC_UNDO_CHG_ENT_TAB_CLS: // Entity/Table/Class Change.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			ul->data[2]=(void *)data2;
			break;


		case IC_UNDO_DEL_ENT_TAB_CLS: // Entity/Table/Class Del.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			break;


		case IC_UNDO_ZOOM: // Zoom change			
			ul->data[0]= (void *)new struct SDS_prevview; 			
			memcpy(ul->data[0],data0,sizeof(struct SDS_prevview));
			break;


		case IC_UNDO_REDEFINE_BLOCK:	 // Redefine Block.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			ul->data[2]=(void *)data2;
			break;


		case IC_UNDO_IMAGE:
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			break;


		case IC_UNDO_ENTITY_REDRAW: // Entity Redraw.
			ul->data[0]=(void *)data0;
			break;


		case IC_UNDO_CICADVIEW_CREATE: // CIcadView Create.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			break;


		case IC_UNDO_CICADVIEW_SIZE: // CIcadView Size.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			break;


		case IC_UNDO_CICADVIEW_MOVE: // CIcadView Move.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			break;



		case IC_UNDO_CICADVIEW_DELETE: // CIcadView Delete.
			ul->data[0]=(void *)data0;
			ul->data[1]=(void *)data1;
			ul->data[2]=(void *)data2;
			break;



		case IC_UNDO_REDRAW_ON_UNDO:	 // Redraw Drawing on Undo.
		case IC_UNDO_REGEN_ON_UNDO:	 // Regen Drawing on Undo.
		case IC_UNDO_REDRAW_ON_REDO:	 // Redraw Drawing on Redo.
		case IC_UNDO_REGEN_ON_REDO:	 // Regen Drawing on Redo.
			break;


		case IC_UNDO_HIDE_ON_REDO: // Hide on redo.
		case IC_UNDO_SHADE_ON_REDO: // Shade on redo.
		case IC_UNDO_NOP: // Do nothing.
			break;


		case IC_UNDO_MDI_RESTORE: // MDI Restore.
		case IC_UNDO_MDI_MAXIMIZE: // MDI Maximize.
			break;


		case IC_UNDO_REDRAW_VP_ON_UNDO:	 // Redraw Drawing on Undo (one VP).
		case IC_UNDO_REGEN_VP_ON_UNDO:	 // Regen Drawing on Undo (one VP).
		case IC_UNDO_REDRAW_VP_ON_REDO:	 // Redraw Drawing on Redo (one VP).
		case IC_UNDO_REGEN_VP_ON_REDO:	 // Regen Drawing on Redo (one VP).
			break;


		case IC_UNDO_PS_MS_VP_CHANGE: // PS/MS Viewport change.
			ul->data[0]=(void *)data0;
			break;


		default:
			sds_alert(ResourceString(IDC_ICADUNDOREDO_INTERNAL_UN_4, "Internal undo error." ));
			break;
		}
	}



GenericUnit:: ~GenericUnit()
	{
	struct SDS_UndoListDef *ul = &m_data;

	switch(ul->mode)
		{
		case IC_UNDO_SETVAR_CHANGE:	 // Setvar change.
			if(((struct resbuf *)ul->data[1])->restype==RTSTR)
				{				
				delete [] (((struct resbuf *)ul->data[1])->resval.rstring);
				}
			// VT. Fix. 9-4-2002. Alloc via db_alloc_resbuf so free via db_free_resbuf.
			db_free_resbuf((struct resbuf *)ul->data[1]);
			break;

		case IC_UNDO_NEW_ENT_TAB_CLS:	 // Entity/Table/Class New.
			if(ul->data[2])
			{
				db_handitem*	pDbItem = (db_handitem*)ul->data[1];
				/*DG - 24.1.2002*/// An additional thing for objects:
				// if the owner of the object is a dictionary
				// then remove also an according record in the dic.
				// Note, if we are called from ~db_drawing then we don't do that.
				// EBATECH(CNBR) in addition, dictionary_wdflt have same behavior.
				if(db_is_objecttype(pDbItem->ret_type()))
				{
					db_handitem*		pOwnerObject = NULL;
					db_dictionaryrec*	pRec;
					bool				bFoundRec = false;
					ReactorSubject*		pReactors = pDbItem->getReactor();

					if(pReactors->GetCount() == 1)
					{
						pReactors->ResetIterator();
						pOwnerObject = pReactors->GetCurrentHanditem();
						if(pOwnerObject->ret_type() != DB_DICTIONARY && pOwnerObject->ret_type() != DB_DICTIONARYWDFLT )
							pOwnerObject = NULL;
					}

					if(pOwnerObject && pOwnerObject->ret_type() == DB_DICTIONARY)
					{
						// we don't use db_drawing::dictnext because it changes the state of the drawing
						((db_dictionary*)pOwnerObject)->get_recllends(&pRec, NULL);
						for( ; pRec; pRec = pRec->next)
							if(pRec->hiref->ret_hip(m_flp) == pDbItem)
							{
								bFoundRec = true;
								break;
							}
					}
					else if(pOwnerObject && pOwnerObject->ret_type() == DB_DICTIONARYWDFLT)
					{
						// we don't use db_drawing::dictnext because it changes the state of the drawing
						((db_dictionarywdflt*)pOwnerObject)->get_recllends(&pRec, NULL);
						for( ; pRec; pRec = pRec->next)
							if(pRec->hiref->ret_hip(m_flp) == pDbItem)
							{
								bFoundRec = true;
								break;
							}
					}
					else
					{
						m_flp->get_objllends(&pOwnerObject, NULL);
						for( ; pOwnerObject; pOwnerObject = pOwnerObject->next)
						{
							if(pOwnerObject->ret_type() == DB_DICTIONARY)
							{
								// we don't use db_drawing::dictnext because it changes the state of the drawing
								((db_dictionary*)pOwnerObject)->get_recllends(&pRec, NULL);
								for( ; pRec; pRec = pRec->next)
									if(pRec->hiref->ret_hip(m_flp) == pDbItem)
									{
										bFoundRec = true;
										break;
									}
								if(bFoundRec)
									break;
							}
							else if(pOwnerObject->ret_type() == DB_DICTIONARYWDFLT)
							{
								// we don't use db_drawing::dictnext because it changes the state of the drawing
								((db_dictionarywdflt*)pOwnerObject)->get_recllends(&pRec, NULL);
								for( ; pRec; pRec = pRec->next)
									if(pRec->hiref->ret_hip(m_flp) == pDbItem)
									{
										bFoundRec = true;
										break;
									}
								if(bFoundRec)
									break;
							}
						}
					}

					if(bFoundRec && pOwnerObject->ret_type() == DB_DICTIONARY)
						((db_dictionary*)pOwnerObject)->delrec(pRec->name);
					else if(bFoundRec && pOwnerObject->ret_type() == DB_DICTIONARYWDFLT)
						((db_dictionarywdflt*)pOwnerObject)->delrec(pRec->name);
					else
						_ASSERTE(!"WARNING: an object hasn't a dictionary referencing to the object!");
				}	// it's an object

				db_handitem::delll(pDbItem);

			}	// if(ul->data[2])
			break;

		case IC_UNDO_CHG_ENT_TAB_CLS: // Entity/Table/Class Change.
			{
			struct db_BinChange *pChangeData;

			pChangeData= (struct db_BinChange *)ul->data[1];
			delete pChangeData;
			ul->data[1] = NULL;
			ul->data[2] = NULL;
			}
			break;

		case IC_UNDO_COMMAND_BEGIN: // Command Begin.
		case IC_UNDO_COMMAND_END: // Command End.
			if(ul->data[1])
				{
				LPCTSTR neww = UndoTokens::RetrieveToken( (int)ul->data[0]);
				if ( neww )
					{
					LPCTSTR	old = (const char *)ul->data[1];
					ASSERT( strsame( old, neww) );
					}

				IC_FREE(ul->data[1]);
				}
			break;

		case IC_UNDO_ZOOM: // Zoom Change.
			if(ul->data[0])
				{				
				IC_FREE(ul->data[0]);
				}
			break;

		case IC_UNDO_CICADVIEW_DELETE: // CIcadView Delete.
			{
			CRect *pTemp = (CRect *)ul->data[2];
			if ( CHECKSTRUCTPTR( pTemp ) )
				{
				delete pTemp;
				}
			ul->data[2] = NULL;
			}
			break;

		case IC_UNDO_REDEFINE_BLOCK: // Redefine block.
			db_handitem::delll((db_handitem *)ul->data[1]);
			break;

		case IC_UNDO_MARK:	// Mark
		case IC_UNDO_GROUP_BEGIN:	 // Group/BEgin.
		case IC_UNDO_GROUP_END:	 // End
		case IC_UNDO_REDRAW_ON_UNDO:	 // Redraw Drawing on Undo.
		case IC_UNDO_DEL_ENT_TAB_CLS: // Entity/Table/Class Delete.
		case IC_UNDO_REGEN_ON_UNDO:	 // Regen Drawing on Undo.
		case IC_UNDO_REDRAW_ON_REDO:	 // Redraw Drawing on Redo.
		case IC_UNDO_REGEN_ON_REDO:	 // Regen Drawing on Redo.
		case IC_UNDO_ENTITY_REDRAW: // Redraw Entity.
		case IC_UNDO_NOP: // Do nothing.
		case IC_UNDO_HIDE_ON_REDO: // Hide on redo.
		case IC_UNDO_SHADE_ON_REDO: // Shade on redo.
		case IC_UNDO_CICADVIEW_CREATE: // CIcadView Create.
		case IC_UNDO_CICADVIEW_SIZE: // CIcadView Size.
		case IC_UNDO_CICADVIEW_MOVE: // CIcadView Move.
		case IC_UNDO_MDI_RESTORE: // MDI Restore.
		case IC_UNDO_MDI_MAXIMIZE: // MDI Maximize.
		case IC_UNDO_PS_MS_VP_CHANGE: // PS/MS Viewport change.
		case IC_UNDO_REDRAW_VP_ON_UNDO: // Redraw Drawing on Undo (one VP).
		case IC_UNDO_REGEN_VP_ON_UNDO: // Regen Drawing on Undo (one VP).
		case IC_UNDO_REDRAW_VP_ON_REDO: // Redraw Drawing on Redo (one VP).
		case IC_UNDO_REGEN_VP_ON_REDO: // Regen Drawing on Redo (one VP).
		case IC_UNDO_IMAGE:
			break;

		default:
			sds_alert(ResourceString(IDC_ICADUNDOREDO_INTERNAL_UN_4, "Internal undo error." ));
			break;
		}
	}

// {CB5A47C0-3D85-11d3-9A6F-00104BF51D94}
static const GUID CLSID_IcadUndoUnit =
{ 0xcb5a47c0, 0x3d85, 0x11d3, { 0x9a, 0x6f, 0x0, 0x10, 0x4b, 0xf5, 0x1d, 0x94 } };

STDMETHODIMP
GenericUnit:: GetUnitType( CLSID *pClsid, LONG *plID)
	{
	*pClsid = CLSID_IcadUndoUnit;
	*plID = m_data.mode;
	return S_OK;
	}

/*-------------------------------------------------------------------------*//**
IOleUndoUnit interface method.
Currently it returns command name for command_begin/end undo units.

@author	Dmitry Gavrilov
@param	pBstr	<= description, the caller is responsible for freeing this string
@return	S_OK
*//*--------------------------------------------------------------------------*/
STDMETHODIMP
GenericUnit::GetDescription(BSTR* pBstr)
{
	*pBstr = NULL;
	if(m_data.mode == IC_UNDO_COMMAND_BEGIN || m_data.mode == IC_UNDO_COMMAND_END)
	{
		CString	str(UndoTokens::RetrieveToken((int)m_data.data[0]));
		*pBstr = str.AllocSysString();
	}
	return S_OK;
}

STDMETHODIMP
GenericUnit:: Undo()
	{
	sds_name ename;
	static CPtrList	pl;	// static to enable updating polyline at the end

	struct SDS_UndoListDef * const ul = &m_data;
	db_drawing	*flp = m_flp;
	switch(ul->mode)
		{
/*
		case IC_UNDO_GROUP_BEGIN:	 // Group/BEgin
			if(tomark)
				break;
			return true;
			break;


		case IC_UNDO_GROUP_END:	 // End
			breakoncmd=0;
			break;

		case IC_UNDO_COMMAND_BEGIN: // Command Begin
			if(tomark)
				{
				break;
				}

			if(breakoncmd)
				{
				ul->undid=1;
				return true;
				}
			break;
 */

		// Update (redraw) all polylines in list at the end of undo units processing
		case IC_UNDO_COMMAND_BEGIN: // Command Begin
			{
			POSITION plPos = pl.GetHeadPosition();
			while( plPos != NULL )
			{
				sds_name plEname;
				plEname[0] = (long)pl.GetNext( plPos );
				plEname[1] = (long)flp;
				sds_entupd(plEname);
			}
			pl.RemoveAll();
			break;
			}

		case IC_UNDO_GROUP_BEGIN:	// Group/BEgin
			--s_tmpGroupLevel;	/*D.G.*/
			break;
 		case IC_UNDO_GROUP_END:		// End
			++s_tmpGroupLevel;	/*D.G.*/
			break;
		case IC_UNDO_MARK:			// Undo/Mark
 			break;


		case IC_UNDO_COMMAND_END: // Command End
			{
			const char* str = NULL;
			struct resbuf rb;
			SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(CMD_CMDECHO_PRINTF(rb.resval.rint) && !SDS_WasFromPipeLine)
				{
				str = UndoTokens::RetrieveToken( (int)ul->data[0]);
				sds_printf(ResourceString(IDC_ICADUNDOREDO__NUNDO____S_7, "\nUndo: (%s)" ), str);
				}
			if(SDS_CURDOC)
			{
				ASSERT(SDS_CURDOC->m_dbDrawing == flp);
				if(SDS_CURDOC->m_cmdHelper.isOopsAvailable())
				{
					str = str ? str : UndoTokens::RetrieveToken( (int)ul->data[0]);
					if(str && (strsame(str, "DELETE") || strsame(str, "_DELETE")))
						SDS_CURDOC->m_cmdHelper.setOopsAvailable(false);
				}
			}
			break;
			}

		case IC_UNDO_SETVAR_CHANGE:	 // Setvar Change.
			{
			struct resbuf rb;
			sds_getvar((char *)ul->data[0],&rb);
			if ( !strisame((char *)ul->data[0],"UNDOCTL"/*DNT*/) )	//dont undo a change to the UNDO setvar
				sds_setvar((char *)ul->data[0],(struct resbuf *)ul->data[1]);

			// if we just turned it off

			if ( strisame((char *)ul->data[0],"GRIDMODE"/*DNT*/) &&
				 ((struct resbuf *)ul->data[1])->resval.rint==0)
				redraw();

			// Swap the resbuf with the current resbuf.
			if(((struct resbuf *)ul->data[1])->restype==RTSTR)
				{
				delete [] (((struct resbuf *)ul->data[1])->resval.rstring);
				}
			*((struct resbuf *)ul->data[1])=rb;
			if(strisame((char *)ul->data[0],ResourceString(IDC_ICADUNDOREDO_VIEWDIR_6, "VIEWDIR" )))
				{
				SDS_VarToTab(flp,SDS_CURVIEW->m_pVportTabHip);
				}
			}
			break;


		case IC_UNDO_REDRAW_ON_UNDO:	 // Redraw Drawing on Undo.
			redrawAll();
			break;


		case IC_UNDO_REGEN_ON_UNDO:	 // Regen Drawing on Undo.
			regenAll();
			break;


		case IC_UNDO_REDRAW_ON_REDO:	 // Redraw Drawing on Redo.
			break;


		case IC_UNDO_REGEN_ON_REDO:	 // Regen Drawing on Redo.
			break;


		case IC_UNDO_NEW_ENT_TAB_CLS:	 // New/Table/Class Ent (Unlink the ent from the database)
			{
				ename[0] = (long)ul->data[1];
				ename[1] = (long)flp;

				int	notice = SDS_ADD_NOTICE;
				if(RTERROR == SDS_CallUserCallbackFunc(SDS_CBENTUNDO, (void*)ename, (void*)&notice, NULL))
					break;

				db_handitem	*elp = (db_handitem*)ul->data[1], *bhip, *ehip, *pelp = (db_handitem*)ul->data[0];

				if(!elp)
					break;

				int	hiptype = elp->ret_type();

				// Undraw the ent.
				if(ul->flag == -1 && db_is_entitytype(hiptype))	/*DG - 24.1.2002*/// Redraw only graphical entities.
					sds_redraw(ename, IC_REDRAW_UNDRAW);

				if(hiptype == DB_OLE2FRAME && SDS_CMainWindow)
				{
					// Delete the CIcadCntrItem object associated with this entity.
					SDS_CMainWindow->m_pvWndAction = (void*)elp;
					ExecuteUIAction(ICAD_WNDACTION_DELOLEITEM);
					SDS_CMainWindow->m_pvWndAction = NULL;
					if(!SDS_CMainWindow->m_bWndAction)
						break;
				}

				gr_freedisplayobjectll(elp->ret_disp());
				elp->set_disp(NULL);

				flp->delhanditem(NULL, elp, 0);

				// Delete sub ents too.
				db_handitem*	telp = elp->next;
				while(telp && db_is_subentitytype(telp->ret_type()))
				{
					if(flp->delhanditem(NULL, telp, 0))
						break;
					telp = telp->next;
				}

				if(hiptype == DB_OLE2FRAME && SDS_CMainWindow)
					redrawAll();

				// Set up the links
				if(ul->flag > -1)
				{
					// If it was a table record.
					flp->get_tabrecllends(ul->flag, &bhip, &ehip);
					if(!pelp)
						bhip = NULL;
					else
						pelp->next = NULL;

					// Check for special case of unlinking first ent in DB.
					if(elp == bhip)
						bhip = ehip = NULL;
					else
						for(ehip = elp; ehip && ehip->next; ehip = ehip->next)
							;

					flp->set_tabrecllends(ul->flag, bhip, pelp);

					// Null the tblnext.
					flp->nulltblnext(ul->flag);
				}
				else
				{
					// Link into the database.
					if(db_is_objecttype(hiptype))
					{
						// Remove 'object' types.
						flp->get_objllends(&bhip, &ehip);

						for(pelp = bhip; pelp->next && pelp->next != elp; pelp = pelp->next)
							;
						pelp->next = NULL;

						// Check for special case of unlinking first obj in DB.
						if(elp == bhip)
							bhip = pelp = NULL;

						flp->set_objllends(bhip, pelp);
					}
					else
						{
						flp->get_entllends(&bhip, &ehip);

						if(!pelp)
							bhip = NULL;
						else
						{
							for(pelp = pelp; pelp->next && pelp->next != elp; pelp = pelp->next)
								;
							pelp->next = NULL;
						}

						// Check for special case of unlinking first ent in DB.
						if(elp == bhip)
							bhip = pelp = NULL;

						flp->set_entllends(bhip, pelp);

						// Null the last ent so dwg_entlast will search through the database.
						flp->nulllastmainent();
					}
				}

				// Make sure we mark the VX table as deleted because
				// we unlinked the Viewport entity.
				if(hiptype == DB_VIEWPORT)
				{
					db_handitem*	hip = flp->tblnext(DB_VXTAB, 1);
					while(hip = flp->tblnext(DB_VXTAB, 0))
						if(((db_vxtabrec*)hip)->ret_vpehip() == elp)
							flp->delhanditem(NULL, hip, 0);
				}

				// Set a value in [2] so we free in ~GenericUnit().
				ul->data[2] = ul->data[1];


			// Notify apps after successful undoing.
			SDS_CallUserCallbackFunc(SDS_CBENTUNDONE, static_cast<void*>(ename), static_cast<void*>(&notice), NULL);

			}	// case IC_UNDO_NEW_ENT_TAB_CLS
			break;

		case IC_UNDO_CHG_ENT_TAB_CLS: // Entity/Table/Class Change
			{
			db_handitem *elp;
			sds_point pt1;

			ename[0]=(long)ul->data[0];
			ename[1]=(long)flp;

			int notice = SDS_MODIFY_NOTICE;
			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBENTUNDO,(void *)ename,(void *)&notice,NULL))
				{
				break;
				}

			int enttype = IC_TYPE_FROM_ENAME( ename );

			// Undraw the ent.
			elp=(db_handitem *)ename[0];
			if(elp->ret_type()==DB_VERTEX)
				{
				db_polyline *pPolyline = ((db_vertex *)elp)->retParentPolyline();
				ASSERT( pPolyline != NULL );
				elp = pPolyline;
				}
			else
				{
				elp=NULL;
				}

			if(elp && !pl.Find(elp))
				{
				pl.AddHead(elp);
				}

			if( db_is_entitytype(enttype) &&	/*DG - 9.1.2002*/// Redraw only entities.
				enttype != DB_VERTEX	&&
				enttype != DB_POLYLINE	&&
				enttype != DB_SEQEND	&&
				enttype != DB_OLE2FRAME )
			{
				sds_redraw(ename, IC_REDRAW_UNDRAW);
			}

			db_BinChange *tcl = (struct db_BinChange *)ul->data[1];
			VERIFY( tcl->UndoChanges( flp, (db_handitem *)ename[0] ) );

			// Redraw the ent.
			if(db_is_entitytype(enttype) && ename[0])	/*DG - 9.1.2002*/// Redraw only entities.
				{

				// This is a fix for the fact the set_10 in a polyline sets the Z in all of the vertices.
				// Ugly!!!
				if(enttype==DB_POLYLINE)
					{
					((db_handitem *)ename[0])->get_10(pt1);
					((db_handitem *)ename[0])->set_10(pt1);
					}

				if( enttype==DB_OLE2FRAME)
					{
					SDS_CURDOC->m_pHandItemForRedraw=((db_handitem *)ename[0]);
					ExecuteUIAction( ICAD_WNDACTION_OLEUNDOREDO );
					redrawAll();
					}
				else if ( (enttype == DB_POLYLINE) ||
						  (enttype == DB_VERTEX) ||
						  (enttype == DB_SEQEND))
					{
					// Do nothing
					}
				else
					{
					sds_entupd(ename);
					}
				}

			// update reactors
			db_handitem *entToUpdate = (db_handitem*)ename[0];

			if(entToUpdate->ret_type()==DB_VERTEX)
				entToUpdate = ((db_vertex *)entToUpdate)->retParentPolyline();

			cmd_updateReactors(entToUpdate);

			// Notify apps after successful undoing.
			SDS_CallUserCallbackFunc(SDS_CBENTUNDONE, static_cast<void*>(ename), static_cast<void*>(&notice), NULL);

			}

			break;


		case IC_UNDO_DEL_ENT_TAB_CLS: // Entity/Table/Class delete.
			{
			db_handitem *elp,*hip,*endhip;

			ename[0]=(long)ul->data[0];
			ename[1]=(long)flp;

			int notice = SDS_DELETE_NOTICE;

			//6/98 - This is a hack.  We want the user to have access to the entity whose delete is being undone.
			//Unfortunately, since the entities "deleted" flag is set functions such as sds_entget return NULL.
			//Therefore I am temporarily setting the "deleted" flag to false then resetting it to true after the
			//callback.
			int delflag = ((db_handitem *)ename[0])->ret_deleted();
			((db_handitem *)ename[0])->set_deleted(0);

			int callbackret = SDS_CallUserCallbackFunc(SDS_CBENTUNDO,(void *)ename,(void *) &notice,NULL);
			((db_handitem *)ename[0])->set_deleted(delflag);

			if(callbackret == RTERROR)
				{
				break;
				}

			sds_entdel(ename); // undelete the entity

			// For a viewport, update the VX table to indicate that this item is present again.
			elp=(db_handitem *)ename[0];
			if(elp->ret_type()==DB_VIEWPORT)
				{
				flp->get_tabrecllends(DB_VXTAB,&hip,&endhip);	// you cannot walk this with tblnext because
				while (hip!=NULL)
					{								// it will skip over the deleted entry
					if(((db_vxtabrec *)hip)->ret_vpehip()==elp)
						{
						flp->delhanditem(NULL,hip,1);  // flag as valid
						break;
						}
					hip=hip->next;
					}
				}

			// Notify apps after successful undoing.
			SDS_CallUserCallbackFunc(SDS_CBENTUNDONE, static_cast<void*>(ename), static_cast<void*>(&notice), NULL);

			}
			break;


		case IC_UNDO_ZOOM: // Zoom change
			SDS_ZoomIt(NULL,4,(double *)ul->data[0],NULL,NULL,NULL,NULL,NULL);
			break;


		case IC_UNDO_ENTITY_REDRAW: // Entity redraw.
			ename[0]=(long)ul->data[0];
			ename[1]=(long)flp;
			sds_entupd(ename);
			break;


		case IC_UNDO_NOP: // Do nothing.
			break;


		case IC_UNDO_HIDE_ON_REDO: // Hide on redo.
			break;


		case IC_UNDO_SHADE_ON_REDO: // Shade on redo.
			break;


		case IC_UNDO_CICADVIEW_CREATE: // CIcadView Create.
			{
			CIcadView *pTemp = (CIcadView *)ul->data[0];
			ul->data[0]=NULL;
			if ( CHECKSTRUCTPTR( pTemp ) )
				{
				if ( IsWindow( pTemp->GetSafeHwnd() ) )
					{
					SDS_CMainWindow->m_wndViewClose = pTemp;
					ul->data[1]=(void *)SDS_CMainWindow->m_wndViewClose->m_pVportTabHip;
					SDS_CMainWindow->m_wndViewClose->m_bDontDelInTab=TRUE;
					ExecuteUIAction( ICAD_WNDACTION_CLOSEVIEW );
					}
				}
			}
			break;


		case IC_UNDO_CICADVIEW_SIZE: // CIcadView Size.
			break;


		case IC_UNDO_CICADVIEW_MOVE: // CIcadView Move.
			break;


		case IC_UNDO_CICADVIEW_DELETE: // CIcadView Delete.
			if(ul->data[1]==NULL) break;
				{
				sds_point pt1,pt2;
				((db_handitem *)ul->data[1])->get_10(pt1);
				((db_handitem *)ul->data[1])->get_11(pt2);
				flp->delhanditem(NULL,(db_handitem *)ul->data[1],1);
				if(ul->data[1])
					{
					((db_handitem *)ul->data[1])->set_2((char*)ResourceString(IDC_ICADUNDOREDO__ACTIVE_8, "*ACTIVE" ));
					}


				SDS_CMainWindow->m_pViewTabEntryHip=(db_handitem *)ul->data[1];
				SDS_CMainWindow->m_rectCreateView = new CRect( (CRect *)ul->data[2] );
				ExecuteUIAction( ICAD_WNDACTION_WOPEN );
				SDS_CMainWindow->m_rectCreateView=NULL;

				// cmd_makewindowport(pt1,pt2,(db_handitem *)ul->data[1]);

				ul->data[0]=(void *)SDS_CURVIEW;
				}
			break;


		case IC_UNDO_MDI_RESTORE: // MDI Restore.
			ExecuteUIAction( ICAD_WNDACTION_MDIMAX );
			break;


		case IC_UNDO_MDI_MAXIMIZE: // MDI Maximize.
			ExecuteUIAction( ICAD_WNDACTION_MDIRESTORE );
			break;


		case IC_UNDO_PS_MS_VP_CHANGE: // PS/MS Viewport change.
			if(SDS_CURVIEW==NULL) break;
				{
				db_handitem *hip=SDS_CURVIEW->m_pVportTabHip;
				cmd_MakeVpActive(SDS_CURVIEW,(db_handitem *)ul->data[0],flp);
				ul->data[0]=(void *)hip;
				}
			break;


		case IC_UNDO_REDRAW_VP_ON_UNDO:	 // Redraw Drawing on Undo (one VP).
			redraw();
			break;


		case IC_UNDO_REGEN_VP_ON_UNDO:	 // Regen Drawing on Undo (one VP).
			regen();
			break;


		case IC_UNDO_REDRAW_VP_ON_REDO:	 // Redraw Drawing on Redo (one VP).
			break;


		case IC_UNDO_REGEN_VP_ON_REDO:	 // Regen Drawing on Redo (one VP).
			break;


		case IC_UNDO_REDEFINE_BLOCK:	 // Redefine Block.
			{
			db_handitem *bhip,*ehip;
			((db_blocktabrec *)ul->data[0])->get_blockllends(&bhip,&ehip);
			((db_blocktabrec *)ul->data[0])->set_blockllends((db_handitem *)ul->data[1],(db_handitem *)ul->data[2]);
			ul->data[1]=(void *)bhip;
			ul->data[2]=(void *)ehip;
			regenAll();
			}
			break;


		case IC_UNDO_IMAGE:				//Send hitachi undo notice
			{
				CVisioRaster *pRaster;
				sds_name name;
				name[0] = (long) ul->data[0];
				name[1] = (long) flp;

				pRaster=(CVisioRaster *)((db_imagedef *)name[0])->ret_imagedisp();
				long cookie = (long) ul->data[1];

				IRas::ErrorCode err = pRaster->undo(cookie);
				ImageAlert(err, IMAGEUNDO);

				ImageRedraw(name);
			}
			break;


		default:
			sds_alert(ResourceString(IDC_ICADUNDOREDO_INTERNAL_UN_4, "Internal undo error." ));
			break;
		}

	ASSERT( !ul->undid);
	ul->undid = true;
	return S_OK;
	}


STDMETHODIMP
GenericUnit:: Redo()
	{
	sds_name ename;
	struct resbuf rb;
	static CPtrList	pl;	// static to enable updating polyline at the end

	struct SDS_UndoListDef * const ul = &m_data;
	db_drawing	*flp = m_flp;
	switch(ul->mode)
		{
/*
		case IC_UNDO_GROUP_BEGIN:	 // Group/BEgin.
			breakoncmd=0;
			break;


		case IC_UNDO_GROUP_END:	 // End.
			return true;
			break;


		case IC_UNDO_COMMAND_END: // Command End.
			if(tomark)
				{
				break;
				}
			if(breakoncmd)
				{
				ul->undid=0;
				return true;
				}
			break;
*/


		case IC_UNDO_COMMAND_BEGIN: // Command Begin.
			SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(CMD_CMDECHO_PRINTF(rb.resval.rint) && !SDS_WasFromPipeLine)
				{
				sds_printf(ResourceString(IDC_ICADUNDOREDO__NREDO_____10, "\nRedo: (%s)" ), UndoTokens::RetrieveToken( (int)ul->data[0]));
				}
			break;


		// Update (redraw) all polylines in list at the end of undo units processing
		case IC_UNDO_COMMAND_END:	// Command End.
			{
			POSITION plPos = pl.GetHeadPosition();
			while( plPos != NULL )
			{
				sds_name plEname;
				plEname[0] = (long)pl.GetNext( plPos );
				plEname[1] = (long)flp;
				sds_entupd(plEname);
			}
			pl.RemoveAll();
			break;
			}

		case IC_UNDO_GROUP_BEGIN:	// Group/BEgin.
			++s_tmpGroupLevel;	/*D.G.*/
			break;
		case IC_UNDO_GROUP_END:		// End.
			--s_tmpGroupLevel;	/*D.G.*/
			break;
		case IC_UNDO_MARK:			// Undo/Mark
			break;


		case IC_UNDO_SETVAR_CHANGE:	 // Setvar Change.
			{
			sds_getvar((char *)ul->data[0],&rb);
			sds_setvar((char *)ul->data[0],(struct resbuf *)ul->data[1]);
			// Swap the resbuf with the current resbuf.
			if(((struct resbuf *)ul->data[1])->restype==RTSTR)
				{
				delete [] (((struct resbuf *)ul->data[1])->resval.rstring);
				}
			*((struct resbuf *)ul->data[1])=rb;
			}
			break;


		case IC_UNDO_REDRAW_ON_UNDO:	 // Redraw Drawing on Undo.
			break;


		case IC_UNDO_REGEN_ON_UNDO:	 // Regen Drawing on Undo.
			break;


		case IC_UNDO_REDRAW_ON_REDO:	 // Redraw Drawing on Redo.
			redrawAll();
			break;


		case IC_UNDO_REGEN_ON_REDO:	 // Regen Drawing on Redo.
			regenAll();
			break;


		case IC_UNDO_NEW_ENT_TAB_CLS:	 // New Ent/Table/Class (link it back into the database and redraw)
			{
				db_handitem	*elp = (db_handitem*)ul->data[1], *bhip, *ehip, *endhip, *hip;

				ename[0] = (long)elp;
				ename[1] = (long)flp;

				int	notice = SDS_ADD_NOTICE;

				//6/98 - This is a hack.  We want the user to have access to the entity whose add is being redone.
				// Unfortunately, since the entities "deleted" flag is set, functions such as sds_entget return NULL.
				// Therefore I am temporarily setting the "deleted" flag to false then resetting it to true after the
				// callback.
				int	delflag = ((db_handitem*)ename[0])->ret_deleted();
				((db_handitem*)ename[0])->set_deleted(0);
				int	callbackret = SDS_CallUserCallbackFunc(SDS_CBENTREDO, (void*)ename, (void*)&notice, NULL);
				((db_handitem*)ename[0])->set_deleted(delflag);

				if(callbackret == RTERROR)
					break;

				if(!elp)
					break;

				flp->delhanditem(NULL, elp, 1);

				// make sub ents valid too
				db_handitem*	telp = elp->next;
				while(telp && db_is_subentitytype(telp->ret_type()))
				{
					if(flp->delhanditem(NULL, telp, 1))
						break;
					telp = telp->next;
				}

				if(ul->flag > -1)
				{	// If it was a table record.
					flp->get_tabrecllends(ul->flag, &bhip, &ehip);
					if(!bhip)
						bhip = elp;
					else
						ehip->next = elp;

					if(!ehip)
						ehip = elp;
					if(ehip == ehip->next)
						ehip->next = NULL;	//TODO Bogus Fix

					for(ehip = elp; ehip && ehip->next; ehip = ehip->next)
						;

					flp->set_tabrecllends(ul->flag, bhip, ehip);

					// Null the tblnext.
					flp->nulltblnext(ul->flag);
				}
				else
				{
					/*DG - 24.1.2002*/// For objects we must do things different from ones for entities.
					if(db_is_objecttype(elp->ret_type()))
					{
						flp->get_objllends(&bhip, &ehip);
						ehip->next = elp;

						/*DG - 26.7.2002*/// a misprint?
						for(ehip = elp; ehip->next; ehip = ehip->next)
//						for(ehip = elp->next; ehip; ehip = ehip->next)
							;

						flp->set_objllends(bhip, ehip);
					}
					else
					{
						flp->get_entllends(&bhip, &ehip);
						if(!ul->data[0])
							bhip = (db_handitem*)ul->data[1];
						else
							((db_handitem*)ul->data[0])->next = elp;

						for(ehip = elp; ehip && ehip->next; ehip = ehip->next)
							;

						flp->set_entllends(bhip, ehip);

						// Null the last ent so dwg_entlast will search through the database.
						flp->nulllastmainent();
					}
				}	// not tables cases

				// Set a NULL in [2] so we DON'T free.
				ul->data[2] = NULL;

				// For a viewport, update the VX table to indicate that this item is present again.
				if(elp && elp->ret_type() == DB_VIEWPORT)
				{
					flp->get_tabrecllends(DB_VXTAB, &hip, &endhip);	// you cannot walk this with tblnext because
					while(hip)
					{	// it will skip over the deleted entry
						if(((db_vxtabrec*)hip)->ret_vpehip() == elp)
						{
							flp->delhanditem(NULL, hip, 1);	// flag as valid
							break;
						}
						hip = hip->next;
					}
				}

				// Redraw ent.
				if(ul->flag == -1 && db_is_entitytype(elp->ret_type()))	/*DG - 24.1.2002*/// Redraw only graphical entities.
					sds_entupd(ename);

			// Notify apps after successful redoing.
			SDS_CallUserCallbackFunc(SDS_CBENTREDONE, static_cast<void*>(ename), static_cast<void*>(&notice), NULL);

			}	// case IC_UNDO_NEW_ENT_TAB_CLS
			break;

		case IC_UNDO_CHG_ENT_TAB_CLS: // Entity/Table/Class Change.
			{
			db_handitem *elp;

			ename[0]=(long)ul->data[0];
			ename[1]=(long)flp;

			int notice = SDS_MODIFY_NOTICE;
			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBENTREDO,(void *)ename,(void *) &notice,NULL))
				{
				break;
				}


			int enttype = IC_TYPE_FROM_ENAME( ename );

			// Undraw the ent.
			elp=(db_handitem *)ename[0];
			if(elp->ret_type()==DB_VERTEX)
				{
				db_polyline *pPolyline = ((db_vertex *)elp)->retParentPolyline();
				ASSERT( pPolyline != NULL );
				elp = pPolyline;
				}
			else
				{
				elp=NULL;
				}

			if(elp && !pl.Find(elp))
				{
				pl.AddHead(elp);
				}

			if( db_is_entitytype(enttype) &&	/*DG - 9.1.2002*/// Redraw only entities.
				enttype != DB_VERTEX	&&
				enttype != DB_POLYLINE	&&
				enttype != DB_SEQEND	&&
				enttype != DB_OLE2FRAME )
			{
				sds_redraw(ename, IC_REDRAW_UNDRAW);
			}

			if (enttype == DB_HATCH)
			{
				db_HatchChange *tcl = (db_HatchChange *)ul->data[1];
				VERIFY( tcl->RedoChanges( flp, (db_handitem *)ename[0] ) );
			}
			else
			{
				struct db_BinChange *tcl = (struct db_BinChange *)ul->data[1];
				VERIFY( tcl->RedoChanges( flp, (db_handitem *)ename[0] ) );
			}

			// Redraw the ent.
			if(db_is_entitytype(enttype) && ename[0])	/*DG - 9.1.2002*/// Redraw only entities.
				{
				if( enttype==DB_OLE2FRAME)
					{
					SDS_CURDOC->m_pHandItemForRedraw=((db_handitem *)ename[0]);
					ExecuteUIAction( ICAD_WNDACTION_OLEUNDOREDO );
					redrawAll();
					}
				else if ( (enttype == DB_POLYLINE) ||
						  (enttype == DB_VERTEX) ||
						  (enttype == DB_SEQEND))
					{
					// Do nothing
					}
				else
					{
					sds_entupd(ename);
					}
				}

			// update reactors
			db_handitem *entToUpdate = (db_handitem*)ename[0];

			if(entToUpdate->ret_type()==DB_VERTEX)
				entToUpdate = ((db_vertex *)entToUpdate)->retParentPolyline();

			cmd_updateReactors(entToUpdate);

			// Notify apps after successful redoing.
			SDS_CallUserCallbackFunc(SDS_CBENTREDONE, static_cast<void*>(ename), static_cast<void*>(&notice), NULL);
			}
			break;


		case IC_UNDO_DEL_ENT_TAB_CLS: // Entity/Table/Class delete.
			{
			db_handitem *elp;

			ename[0]=(long)ul->data[0];
			ename[1]=(long)flp;

			int notice = SDS_DELETE_NOTICE;
			if(RTERROR==SDS_CallUserCallbackFunc(SDS_CBENTREDO,(void *)ename,(void *) &notice,NULL))
				{
				break;
				}

			sds_entdel(ename);

			// Make sure we mark the VX table as deleted because
			// we unlinked the Viewport entity.
			elp=(db_handitem *)ul->data[0];
			if(elp->ret_type()==DB_VIEWPORT)
				{
				db_handitem *hip=flp->tblnext(DB_VXTAB,1);
				while(hip=flp->tblnext(DB_VXTAB,0))
					{
					if(((db_vxtabrec *)hip)->ret_vpehip()==elp)
						{
						flp->delhanditem(NULL,hip,0);
						}
					}
				}
			// Notify apps after successful redoing.
			SDS_CallUserCallbackFunc(SDS_CBENTREDONE, static_cast<void*>(ename), static_cast<void*>(&notice), NULL);
			}
			break;


		case IC_UNDO_ZOOM: // Zoom change
			SDS_ZoomIt(NULL,4,(double *)ul->data[0],NULL,NULL,NULL,NULL,NULL);
			break;


		case IC_UNDO_ENTITY_REDRAW: // Entity redraw.
			if(((db_handitem *)ul->data[0])->ret_deleted())
				{
				break;
				}
			ename[0]=(long)ul->data[0];
			ename[1]=(long)flp;
			sds_entupd(ename);
			break;


		case IC_UNDO_NOP: // Do nothing.
			break;


		case IC_UNDO_HIDE_ON_REDO: // Hide on redo.
			cmd_hide();
			break;


		case IC_UNDO_SHADE_ON_REDO: // Shade on redo.
			cmd_shade();
			break;


		case IC_UNDO_CICADVIEW_CREATE: // CIcadView Create.
			if(ul->data[1]==NULL)
				{
				break;
				}
			{
			sds_point pt1,pt2;
			((db_handitem *)ul->data[1])->get_10(pt1);
			((db_handitem *)ul->data[1])->get_11(pt2);
			flp->delhanditem(NULL,(db_handitem *)ul->data[1],1);
			if(ul->data[1])
				{
				((db_handitem *)ul->data[1])->set_2((char*)ResourceString(IDC_ICADUNDOREDO__ACTIVE_8, "*ACTIVE" ));
				}

			cmd_makewindowport(pt1,pt2,(db_handitem *)ul->data[1]);
			ul->data[0]=(void *)SDS_CURVIEW;
			}
			break;


		case IC_UNDO_CICADVIEW_SIZE: // CIcadView Size.
			break;


		case IC_UNDO_CICADVIEW_MOVE: // CIcadView Move.
			break;


		case IC_UNDO_CICADVIEW_DELETE: // CIcadView Delete.
			if(SDS_CMainWindow->m_pCurDoc->m_nOpenViews==1)
				{
				break;
				}
			SDS_CMainWindow->m_wndViewClose=(CIcadView *)ul->data[0];
			ul->data[1]=(void *)SDS_CMainWindow->m_wndViewClose->m_pVportTabHip;
//				SDS_CMainWindow->m_wndViewClose->m_bDontDelInTab=TRUE;
			ExecuteUIAction( ICAD_WNDACTION_CLOSEVIEW );
			break;


		case IC_UNDO_MDI_RESTORE: // MDI Restore.
			cmd_CalcChildRect=TRUE;
			ExecuteUIAction( ICAD_WNDACTION_MDIRESTORE );
			cmd_CalcChildRect=FALSE;
			break;


		case IC_UNDO_MDI_MAXIMIZE: // MDI Maximize.
			cmd_CalcChildRect=TRUE;
			ExecuteUIAction( ICAD_WNDACTION_MDIMAX );
			cmd_CalcChildRect=FALSE;
			break;


		case IC_UNDO_PS_MS_VP_CHANGE: // PS/MS Viewport change.
			if(SDS_CURVIEW==NULL)
				{
				break;
				}
			{
			db_handitem *hip=SDS_CURVIEW->m_pVportTabHip;
			cmd_MakeVpActive(SDS_CURVIEW,(db_handitem *)ul->data[0],flp);
			ul->data[0]=(void *)hip;
			}
			break;


		case IC_UNDO_REDRAW_VP_ON_UNDO:	 // Redraw Drawing on Undo (one VP).
			break;


		case IC_UNDO_REGEN_VP_ON_UNDO:	 // Regen Drawing on Undo (one VP).
			break;


		case IC_UNDO_REDRAW_VP_ON_REDO:	 // Redraw Drawing on Redo (one VP).
			redraw();
			break;


		case IC_UNDO_REGEN_VP_ON_REDO:	 // Regen Drawing on Redo (one VP).
			regen();
			break;


		case IC_UNDO_REDEFINE_BLOCK:	 // Redefine Block.
			{
			db_handitem *bhip,*ehip;
			((db_blocktabrec *)ul->data[0])->get_blockllends(&bhip,&ehip);
			((db_blocktabrec *)ul->data[0])->set_blockllends((db_handitem *)ul->data[1],(db_handitem *)ul->data[2]);
			ul->data[1]=(void *)bhip;
			ul->data[2]=(void *)ehip;
			regenAll();
			}
			break;


		case IC_UNDO_IMAGE:
			{
			CVisioRaster *pRaster;
			sds_name name;
			name[0] = (long)ul->data[0];
			name[1] = (long)flp;

			pRaster=(CVisioRaster *)((db_imagedef *)name[0])->ret_imagedisp();
			long cookie = (long) ul->data[1];

			IRas::ErrorCode err = pRaster->redo(cookie);
			ImageAlert(err, IMAGEREDO);

			ImageRedraw(name);
			}
			break;

		default:
			sds_alert(ResourceString(IDC_ICADUNDOREDO_INTERNAL_UN_4, "Internal undo error." ));
			break;

		}

	ASSERT( ul->undid);
	ul->undid = false;
	return S_OK;
	}


static void
ImageAlert(IRas::ErrorCode error, ImageLoc loc)
{
	int id;

	switch (error){
	case IRas::eFailure:
		if (loc == IMAGEUNDO)
			id = IDS_HITACHI_IMAGE_UNSUCCESS_UNDO;
		else
			id = IDS_HITACHI_IMAGE_UNSUCCESS_REDO;
		break;

	case IRas::eReachedUndoLimit:
		if (loc == IMAGEUNDO)
			id = IDS_HITACHI_IMAGE_LIMIT_UNDO;
		else
			id = IDS_HITACHI_IMAGE_LIMIT_REDO;
		break;

	default:
		return; // do nothing
	}

	TCHAR locStr[128];
	LoadString(IcadSharedGlobals::GetIcadResourceInstance(),id,locStr,128);
	sds_alert(locStr);
}


static void
ImageRedraw(sds_name name)
{
	//redraw
	sds_resbuf *i, *j;
	sds_resbuf *imageDefBuf = sds_entget(name);
	ASSERT(imageDefBuf != NULL);

	bool first = true;
	for (i = imageDefBuf; i; i = i->rbnext) {
		if (i->restype == 330)
		{
			//first 330 group code is pointer to imageDict
			if (first) {
				first = false;
				continue;
			}

			sds_resbuf *imageReactBuf = sds_entget(i->resval.rlname);

			for (j = imageReactBuf; j; j = j->rbnext) {
				if (j->restype == 330)
					sds_redraw(j->resval.rlname, IC_REDRAW_DRAW);
			}

			sds_relrb(imageReactBuf);
		}
	}
	sds_relrb(imageDefBuf);
}


int
SDS_UndoDisplay( db_drawing *dp)
	{
	UndoList	*undoList = GetUndoList( dp);
	if ( undoList )
		{
		CComPtr< IOleUndoManager>	undoMgr;
		undoMgr = *undoList;

		CComPtr< IEnumOleUndoUnits>	units;
		VERIFY_SUCCEEDED( undoMgr->EnumUndoable( &units));
		}

	return RTNORM;
	}


#ifdef	OLDUNDOMGR
static void
SDS_UndoDisplay(db_drawing *flp)
	{
//#ifdef DEBUG
	if(flp==NULL) return;

	for(int ct=0; ct<3; ct++) {
		struct SDS_UndoListDef *ul;
		if(ct==0) {
			sds_printf(ResourceString(IDC_ICADUNDOREDO__NBEG_______0, "\nBEG=====================================================" ));
			ul=(struct SDS_UndoListDef *)flp->UndoBeg;
		} else if(ct==1) {
			sds_printf("\nCUR====================================================="/*DNT*/);
			ul=(struct SDS_UndoListDef *)flp->UndoCur;
		} else if(ct==2) {
			sds_printf("\nEND====================================================="/*DNT*/);
			ul=(struct SDS_UndoListDef *)flp->UndoEnd;
		}

		while(ul) {
			sds_printf("\nUndid = %i   "/*DNT*/,ul->undid);
			switch(ul->mode) {
				case IC_UNDO_MARK:
					sds_printf("Mark"); break;
				case IC_UNDO_GROUP_BEGIN:
					sds_printf("Group/Begin"); break;
				case IC_UNDO_GROUP_END:
					sds_printf("End"); break;
				case IC_UNDO_SETVAR_CHANGE:
					sds_printf("Setvar change (%s)",(char *)ul->data[0]); break;
				case IC_UNDO_REDRAW_ON_UNDO:
					sds_printf("Redraw drawing on undo"); break;
				case IC_UNDO_REGEN_ON_UNDO:
					sds_printf("Regen drawing on undo"); break;
				case IC_UNDO_REDRAW_ON_REDO:
					sds_printf("Redraw drawing on redo"); break;
				case IC_UNDO_REGEN_ON_REDO:
					sds_printf("Regen drawing on redo"); break;
				case IC_UNDO_NEW_ENT_TAB_CLS:
					sds_printf("Entity/Table/Class new (%i)",(int)((db_handitem *)ul->data[1])->ret_type()); break;
				case IC_UNDO_CHG_ENT_TAB_CLS:
					sds_printf("Entity/Table/Class change (%i)",(int)((db_handitem *)ul->data[0])->ret_type()); break;
				case IC_UNDO_DEL_ENT_TAB_CLS:
					sds_printf("Entity/Table/Class delete (%i)",(int)((db_handitem *)ul->data[0])->ret_type()); break;
				case IC_UNDO_COMMAND_BEGIN:
					sds_printf("Command Begin (%s)",(char *)ul->data[0]); break;
				case IC_UNDO_COMMAND_END:
					sds_printf("Command End (%s)",(char *)ul->data[0]); break;
				case IC_UNDO_ZOOM:
					sds_printf("Zoom change"); break;
				case IC_UNDO_ENTITY_REDRAW:
					sds_printf("Entity redraw (%i)",(int)((db_handitem *)ul->data[0])->ret_type()); break;
				case IC_UNDO_NOP:
					sds_printf("Do nothing"); break;
				case IC_UNDO_HIDE_ON_REDO:
					sds_printf("Hide on redo"); break;
				case IC_UNDO_SHADE_ON_REDO:
					sds_printf("Shade on redo"); break;
				case IC_UNDO_CICADVIEW_CREATE:
					sds_printf("CIcadView Create"); break;
				case IC_UNDO_CICADVIEW_SIZE:
					sds_printf("CIcadView Size X=%i  Y=%i",(int)ul->data[0],(int)ul->data[1]); break;
				case IC_UNDO_CICADVIEW_MOVE:
					sds_printf("CIcadView Move X=%i  Y=%i",(int)ul->data[0],(int)ul->data[1]); break;
				case IC_UNDO_CICADVIEW_DELETE:
					sds_printf("CIcadView Delete"); break;
				case IC_UNDO_MDI_RESTORE:
					sds_printf("MDI Restore"); break;
				case IC_UNDO_MDI_MAXIMIZE:
					sds_printf("MDI Maxamize"); break;
				case IC_UNDO_PS_MS_VP_CHANGE:
					sds_printf("PS/MS Viewport change"); break;
				case IC_UNDO_REDRAW_VP_ON_UNDO:
					sds_printf("Redraw drawing on undo (one VP)"); break;
				case IC_UNDO_REGEN_VP_ON_UNDO:
					sds_printf("Regen drawing on undo (one VP)"); break;
				case IC_UNDO_REDRAW_VP_ON_REDO:
					sds_printf("Redraw drawing on redo (one VP)"); break;
				case IC_UNDO_REGEN_VP_ON_REDO:
					sds_printf("Regen drawing on redo (one VP)"); break;
				case IC_UNDO_REDEFINE_BLOCK:
					sds_printf("Redefine block"); break;
				case IC_UNDO_IMAGE:
					sds_printf("Image change");break;
				default:
					sds_alert(ResourceString(IDC_ICADUNDOREDO_INTERNAL_UN_4, "Internal undo error." ));
					break;
			}
			if(ct==0) ul=ul->next;
			else break;
		}
	}
	return;
}
#endif