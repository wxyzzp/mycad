// **********************************************
// CMD_VPLAYER.CPP
//
// Implements VPLAYER command
//
//
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include <assert.h>

#include "IcadApi.h"/*DNT*/
#include "IcadDoc.h"
#include "vxtabrec.h"


#define TRACEMSGS	0
#if TRACEMSGS
	#define DEBUGMSG( s, p )   { char t[256]; ::sprintf( t, (s), p );	::OutputDebugString( t ); }
#else
	#define DEBUGMSG( s, p )	
#endif

// HACK -- global variable external declaration
// This is the way it has been done in the past, and for a little bit longer
//
extern bool    SDS_DontDrawVPGuts;


class zViewportList
	{
	public:
		zViewportList( void )
			{
			m_iCount = 0;
			m_pList = NULL;
			}
		~zViewportList( void )
			{
			if ( m_pList != NULL )
				{
				delete [] m_pList ;
				}
			m_iCount = 0;
			m_pList = NULL;
			}
		bool Add( db_viewport *pNew )
			{
			if ( m_pList == NULL )
				{
				m_pList =  new db_viewport *[1];
				}
			else
				{
				db_viewport **temp;
				temp = new db_viewport *[m_iCount + 1];
				memcpy(temp, m_pList, sizeof(db_viewport *) * m_iCount);
				delete [] m_pList;
				m_pList = temp;
				}
			if ( m_pList != NULL )
				{
				m_pList[ m_iCount ] = pNew;
				m_iCount++;
				return true;
				}
			return false;
			}
		int Count( void )
			{
			return m_iCount;
			}
		db_viewport *Get( int index )
			{
			if ( index < m_iCount )
				{
				return m_pList[ index ];
				}
			return NULL;
			}

		
	private:
		int m_iCount;
		db_viewport **m_pList;
	};

class Cvplayer
	{
	// ctors and dtors
	public:
		Cvplayer( db_drawing *pDrawing );
		~Cvplayer( void );

	// public operations
	//
	public:
		int		cmd_vplayer( void );


		
	// constants
	//
	private:
		enum processType
			{
			PROCESSNONE = 0,
			PROCESSFREEZE,
			PROCESSTHAW,
			PROCESSRESET
			};
		
	// command options
	//
	private:
		int		processLayers( const char *cszPrompt, processType ProcessType );
		int		listFrozenLayers( void );

		int		createNewFrozenLayers( void );
		int		changeDefaultViewportVisibility( void );



	// helper methods
	//
	private:
		int				chooseViewports( zViewportList **ppList );
		int				selectSingleViewport( db_viewport **ppRetViewport );
		bool			inModelSpaceMode( void );
		int				getLayerList( const char *szPrompt, char *szResult, bool bMatchAgainstExistingLayers = true );
		int				getDefaultFreezeOrThawFromUser( bool *pbFreeze );
		bool			layer_check(char *layerlst);
		bool			createNewFrozenLayer( char *szNewLayerName );

	// data members
	//
	private:
		db_drawing *m_pDrawing;		
	};

// *********************************
// CTOR -- class Cvplayer
//
Cvplayer::Cvplayer( db_drawing *pDrawing )
	{
	ASSERT( pDrawing != NULL );

	m_pDrawing = pDrawing;
	}

// *********************************
// DTOR -- class Cvplayer
//
Cvplayer::~Cvplayer( void )
	{
	m_pDrawing = NULL;
	}


// ****************************
// HELPER METHOD
//
// inModelSpaceMode
//
// 
bool
Cvplayer::inModelSpaceMode( void )
	{
    struct resbuf cvport;
	if(SDS_getvar(NULL,DB_QCVPORT,&cvport,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
		ASSERT( false );
		return true;		//???
		}
	if ( cvport.resval.rint == 1 )
		{
		return false;
		}
	else
		{
		return true;
		}
	}

// *******************
// HELPER METHOD
//
//
#ifdef LOCALISE_ISSUE
// translation of freeze won't always have F as the first letter.
#endif
int
Cvplayer::getDefaultFreezeOrThawFromUser( bool *pbFreeze )
	{
    char szTemp[IC_ACADBUF];

	*pbFreeze = false;

	int ret = ::sds_getstring( 1, ResourceString(IDC_CMD_VPLAYER_SETTING_FRO_10, "Setting Frozen/<Thawed>: " ), szTemp );
	if ( ret == RTNORM )
		{
		::strupr( szTemp );
		if ( szTemp[0] == 'F' )
			{
			*pbFreeze = true;
			}
		}

	return ret;
	}

// ********************
// HELPER METHOD
//
// chooseViewports
//
//
int
Cvplayer::chooseViewports( zViewportList **pList )
	{
    char szTemp[IC_ACADBUF];

	*pList = NULL;

	int ret = ::sds_initget(0,ResourceString(IDC_CMD_VPLAYER_ALL_ALL_SEL_14, "All|All Select|Select Current|Current ~_All ~_Select ~_Current" ));
	if ( ret == RTNORM )
		{
		ret = sds_getkword( ResourceString(IDC_CMD_VPLAYER__NALL_SELEC_13, "\nAll/Select/<Current>: " ), szTemp);

		// default 
		//
		if ( ret == RTNONE )
			{
			ret = RTNORM;
			}
		// ********************
		// ALL VIEWPORTS
		//
		if (strisame( szTemp, "ALL"/*DNT*/ ) )
			{

			*pList = new zViewportList;
			ASSERT( pList != NULL );

			DEBUGMSG(ResourceString(IDC_CMD_VPLAYER_CHOOSEVIEWP_18, "chooseViewports All \n" ),NULL);
			// Walk the drawing's viewport table
			//
			db_handitem *hip=m_pDrawing->tblnext(DB_VXTAB,1);
			while(hip=m_pDrawing->tblnext(DB_VXTAB,0)) 
				{
				int iVportNumber;
				if(hip->ret_deleted()) 
					{
					continue;
					}
				db_viewport *pViewport = (db_viewport *)((db_vxtabrec *)hip)->ret_vpehip();
				ASSERT( pViewport->ret_type() == DB_VIEWPORT );
				
				// 69 is viewport number.  1 means paperspace viewport
				//
				pViewport->get_69(&iVportNumber);
				// Don't grab the paperspace viewport
				//
				if ( iVportNumber != 1 )
					{

					DEBUGMSG(ResourceString(IDC_CMD_VPLAYER_CHOOSEVIEWP_19, "chooseViewports All viewport %d\n" ),iVportNumber );

					(*pList)->Add( pViewport );
					}
				}
			return ret;
			// END OF ALL
			}
		// **************************
		// SELECT VIEWPORTS
		//
		else if ( strisame( szTemp, "SELECT"/*DNT*/ ) )
			{

			*pList = new zViewportList;
			ASSERT( pList != NULL );

			db_viewport *pViewport = NULL;
			while( ret == RTNORM )
				{
				// Get them 1 at a time
				//
				ret = selectSingleViewport( &pViewport );
				if ( ret == RTNORM )
					{
					if ( pViewport == NULL )
						{
						break;
						}
					(*pList)->Add( pViewport );
					}
				}
			// Unhighlight all the selected viewports
			//
			for( int i = 0; i < (*pList)->Count() ; i++ )
				{
				sds_name viewportname;
				viewportname[0] = (long)(*pList)->Get( i );
				viewportname[1] = (long)m_pDrawing;

				::SDS_DontDrawVPGuts=TRUE;
				::SDS_RedrawEntity( viewportname,IC_REDRAW_UNHILITE,NULL,NULL,0);
				::SDS_DontDrawVPGuts=FALSE;
				}

			if ( ret != RTNORM )
				{
				delete *pList;
				*pList = NULL;
				}

			return ret;
			// END OF SELECT VIEWPORTS
			}
		else
			// *************************
			// CURRENT VIEWPORT
			//
			{
			*pList = new zViewportList;

			db_viewport *pViewport = SDS_CURDOC->GetCurrentViewport();
			if ( pViewport != NULL )
				{
				(*pList)->Add( pViewport );
				}
			return ret;
			}

		}
	return ret;
	}



// *****************************
// HELPER METHOD -- class Cvplayer
//
// selectSingleViewport
//
//
int
Cvplayer::selectSingleViewport( db_viewport **pRetViewport )
	{
	long iTemp;

	*pRetViewport = NULL;
	bool bmspace = inModelSpaceMode();
	if ( bmspace )
		{
		::cmd_pspace();
		}

	sds_name selectionSet;
	iTemp = ::sds_pmtssget(ResourceString(IDC_CMD_VPLAYER__NSELECT_A_V_4, "\nSelect a viewport : " ),""/*DNT*/,NULL,NULL,NULL,selectionSet,1);
	if( iTemp==RTNORM )
		{
		long lLen;
		::sds_sslength(selectionSet,&lLen);
		if ( lLen == 1 )
			{
			sds_name entityname;
			iTemp = ::sds_ssname( selectionSet, 0, entityname );
			if ( iTemp == RTNORM )
				{
				db_handitem *handitem = (db_handitem *)entityname[0];
				if ( handitem->ret_type() == DB_VIEWPORT )
					{
					*pRetViewport = (db_viewport *)handitem;
					}
				else
					{
					iTemp = RTERROR;
					}
				}
			}	
		else
			{
			iTemp = RTERROR;
			}
		}
	long ssIndex;
	sds_name enameTemp;

	// unhighlight selected entities
	//
	for(ssIndex=0L; ::sds_ssname(selectionSet,ssIndex,enameTemp)==RTNORM; ssIndex++)
		{
		// Don't unhighlight the one we selected, that's the caller's job
		//
		if ( (iTemp == RTNORM) &&
			 (enameTemp[0] == (long)pRetViewport) )
			{
			}
		else
			{
			::SDS_DontDrawVPGuts=TRUE;
			::SDS_RedrawEntity( enameTemp,IC_REDRAW_UNHILITE,NULL,NULL,0);
			::SDS_DontDrawVPGuts=FALSE;
			}
		}

	::sds_ssfree( selectionSet );

	if ( bmspace )
		{
		::cmd_mspace();
		}

	if ( iTemp == RTCAN )
		{
		return RTCAN;
		}
	else
		{
		return RTNORM;
		}
	
	}

bool 
Cvplayer::layer_check(char *layerlst)
	{
	char	szTemp[IC_ACADBUF];

    //check for null
	//
    if(layerlst==NULL) 
		{ 
		return false;
		}

	bool	bRetval = true;

	strcpy( szTemp, layerlst );

	char *pStart = szTemp;
	while( pStart != NULL )
		{
		char *pParse = ::strchr( pStart, ',');
		if ( pParse != NULL )
			{
			*pParse = 0;
			}
		db_layertabrec *pLayer = (db_layertabrec *)this->m_pDrawing->findtabrec(DB_LAYERTAB, pStart,1);

		if ( pLayer == NULL )
			{
			sds_printf(ResourceString(IDC_CMD_VPLAYER_NO_LAYERS_F_11, "No layers found matching pattern %s.\n" ), pStart );
			bRetval = false;
			break;
			}

		// pParse points to NULL or the 0-terminator of the last symbol
		//
		pStart = pParse;
		if ( pStart != NULL )
			{
			pStart++;
			}

		// Just double check we're not out of range
		//
		if ( pStart > (szTemp + IC_ACADBUF))
			{
			break;
			}
		}

    return( bRetval );
}

// ****************************
// HELPER METHOD -- class Cvplayer
//
// getLayerList 
//
// Used wherever we need Layer(s)
//
int
Cvplayer::getLayerList( const char *szPrompt, char *szResult, bool bMatchAgainstExistingLayers )
	{
	char szBuffer[IC_ACADBUF];
	int ret = ::sds_getstring( 1, szPrompt, szBuffer );
	szResult[0] = 0;

	if ( ret == RTNORM )
		{
		bool bFirst = true;

		// layer check expects upper case
		//
		::strupr( szBuffer );

		if ( bMatchAgainstExistingLayers )
			{

			db_layertabrec *pLayer = (db_layertabrec *)this->m_pDrawing->tblnext(DB_LAYERTABREC, 1 );
			while( pLayer != NULL )
				{
	
				char *szLayerName = NULL;
				pLayer->get_2( &szLayerName );
	

				// Use sds_wcmatch to match szLayerName against anything in szBuffer
				//
				if ( sds_wcmatch( szLayerName, szBuffer ) == RTNORM )
					{
					// Concatenate together a , delimited list of layers that match the wild-cards
					//
					if ( !bFirst )
						{
						strcat( szResult, ","/*DNT*/);
						}
					bFirst = false;
					strcat( szResult, szLayerName );
					}



				// Get the next layer
				//
				pLayer = (db_layertabrec *)this->m_pDrawing->tblnext( DB_LAYERTABREC, 0 );
				} // end of while( pLayer != NULL );
			}
		else
			{
			// These are new layers, so just copy the names
			//
			strcpy( szResult, szBuffer );
			}	
		}

	return ret;
	}

// *****************************
// HELPER METHOD -- class Cvplayer
//
// listLayers
//
//
int
Cvplayer::listFrozenLayers( void )
	{
	int ret = RTNORM;

	db_viewport *pViewport = NULL;
	// needs to select a single viewport
	// then list it's frozen layers
	ret = selectSingleViewport( &pViewport );

	if ( (ret == RTNORM) &&
		 (pViewport != NULL) )
		{
		ASSERT( pViewport->ret_type() == DB_VIEWPORT );

		int iLayerCount = pViewport->GetVpLayerFrozenLayerCount( );
		int i;


		int iVportNumber;
		pViewport->get_69(&iVportNumber);
		// better not be the paperspace viewport
		//
		ASSERT( iVportNumber != 1 );

		::sds_printf("\n\n"/*DNT*/);
		if ( iLayerCount < 1 )
			{
			::sds_printf(ResourceString(IDC_CMD_VPLAYER_NO_LAYERS_A_15, "No layers are frozen in viewport %d\n" ),iVportNumber);
			}
		else
			{
			::sds_printf(ResourceString(IDC_CMD_VPLAYER_THE_FOLLOWI_16, "The following layers are frozen in viewport %d\n" ),iVportNumber);
			}

		for( i = 0; i < iLayerCount; i++ )
			{
			char *pszLayerName = pViewport->GetVpLayerFrozenLayerName( i );
			if ( pszLayerName != NULL )
				{
				::sds_printf("%s\n"/*DNT*/,pszLayerName);
				}
			}

		// Now unhighlight the selected viewport
		//
		sds_name viewportname;
		viewportname[0] = (long)pViewport;
		viewportname[1] = (long)m_pDrawing;
		::SDS_DontDrawVPGuts=TRUE;
		::SDS_RedrawEntity( viewportname,IC_REDRAW_UNHILITE,NULL,NULL,0);
		::SDS_DontDrawVPGuts=FALSE;
		}

	return ret;
	}


// *****************************
// HELPER METHOD -- class Cvplayer
//
// processLayers
//
//
int
Cvplayer::processLayers( const char *cszPrompt, processType ProcessType )
	{
   char szTemp[IC_ACADBUF];

	int ret = getLayerList( cszPrompt, szTemp );
	if ( ret == RTNORM && 
		layer_check( szTemp ))
		{
		zViewportList *pList;
		ret = chooseViewports( &pList );
		if ( pList == NULL )
			{
			return ret;
			}
		// getLayerList guarantees the list is validly formed
		//
		char *pStart = szTemp;
		while( pStart != NULL )
			{
			char *pParse = ::strchr( pStart, ',');
			if ( pParse != NULL )
				{
				*pParse = 0;
				}


			// pStart should now contain a nice 0-terminated layer name
			//
			// Walk the list of viewports
			//
			bool bChanged=false;
			for( int i = 0; i < pList->Count(); i++ )
				{
				db_viewport *pViewport = pList->Get( i );
				if ( (pViewport != NULL) &&
					 (pViewport->ret_type() == DB_VIEWPORT) )
					{

					// Do what's needed based on parameter passed in
					//
					switch( ProcessType )
						{
						case PROCESSFREEZE:
							pViewport->VpLayerFreezeLayer( pStart, this->m_pDrawing , &bChanged);
							break;
						case PROCESSTHAW:
							pViewport->VpLayerThawLayer( pStart, this->m_pDrawing , &bChanged);
							break;
						case PROCESSRESET:
							pViewport->VpLayerResetLayer( pStart, this->m_pDrawing , &bChanged);
							break;
						default:
							ASSERT( false );
							delete pList;
							return RTERROR;
						}
					}
				}

			if (bChanged)
				SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);

			// pParse points to NULL or the 0-terminator of the last symbol
			//
			pStart = pParse;
			if ( pStart != NULL )
				{
				pStart++;
				}

			// Just double check we're not out of range
			//
			if ( pStart > (szTemp + IC_ACADBUF))
				{
				break;
				}
			}	// end of while( pStart != NULL )

		delete pList;
		}


	return ret;
	}



bool
Cvplayer::createNewFrozenLayer( char *szNewLayerName )
	{
	struct resbuf *prbNewLayer;
	if ( szNewLayerName[0] == 0 )
		{
		return false;
		}

	// LAYER, 70 flag of 2 means vplayer frozen by default
	//
	// DO NOT INTERNATIONALIZE these strings
	//
	prbNewLayer = ::sds_buildlist(RTDXF0,"LAYER"/*DNT*/,2,szNewLayerName,70,2,62,7,6,"CONTINUOUS"/*DNT*/,0);
	if ( prbNewLayer != NULL )
		{
		if ( ::SDS_tblmesser(prbNewLayer,IC_TBLMESSER_MAKE,this->m_pDrawing) != RTNORM )
			{
			::sds_printf( ResourceString(IDC_CMD_VPLAYER_LAYER_WITH__12, "Layer with name %s already exists!\n" ), szNewLayerName );
			}
		}

	::sds_relrb( prbNewLayer );

	return (prbNewLayer != NULL);
	}

// *****************************
// HELPER METHOD -- class Cvplayer
//
// createNewFrozenLayers
//
//
int
Cvplayer::createNewFrozenLayers( void )
	{
    char szTemp[IC_ACADBUF];

	int ret = getLayerList( ResourceString(IDC_CMD_VPLAYER__NNEW_FROZEN_8, "\nNew frozen layer name(s): " ), szTemp, false );
	if ( ret == RTNORM )
		{
		// getLayerList guarantees the list is validly formed
		//
		char *pStart = szTemp;
		while( pStart != NULL )
			{
			char *pParse = ::strchr( pStart, ',');
			if ( pParse != NULL )
				{
				*pParse = 0;
				}
			if (createNewFrozenLayer( pStart ))
				{
				// Add the new frozen layer to the viewports
				//

				// Walk the drawing's viewport table
				//
				db_handitem *hip=m_pDrawing->tblnext(DB_VXTAB,1);
				while(hip=m_pDrawing->tblnext(DB_VXTAB,0)) 
					{
					int iVportNumber;
					bool bChanged=false;
					if(hip->ret_deleted()) 
						{
						continue;
						}
					db_viewport *pViewport = (db_viewport *)((db_vxtabrec *)hip)->ret_vpehip();
					ASSERT( pViewport->ret_type() == DB_VIEWPORT );
				
					// 69 is viewport number.  1 means the paperspace viewport
					//
					pViewport->get_69(&iVportNumber);
					// Don't munge the paperspace viewport
					//
					if ( iVportNumber != 1 )
						{
						pViewport->VpLayerFreezeLayer( pStart, m_pDrawing , &bChanged);
						if (bChanged)
							SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
						}
					}
				}
			// pParse points to NULL or the 0-terminator of the last symbol
			//
			pStart = pParse;
			if ( pStart != NULL )
				{
				pStart++;
				}

			// Just double check we're not out of range
			//
			if ( pStart > (szTemp + IC_ACADBUF))
				{
				break;
				}
			}

		}


	return ret;
	}
// *****************************
// HELPER METHOD -- class Cvplayer
//
// changeDefaultViewportVisibility
//
//
int
Cvplayer::changeDefaultViewportVisibility( void )
	{
    char szTemp[IC_ACADBUF];

	int ret = getLayerList( ResourceString(IDC_CMD_VPLAYER__NLAYER_NAME_9, "\nLayer names(s): " ), szTemp );
	if ( ret == RTNORM &&
		layer_check( szTemp ))
		{

		bool bFreeze = false;
		ret = getDefaultFreezeOrThawFromUser( &bFreeze );
		if ( ret != RTNORM )
			{
			return ret;
			}

		// getLayerList guarantees the list is validly formed
		//
		char *pStart = szTemp;
		while( pStart != NULL )
			{
			char *pParse = ::strchr( pStart, ',');
			if ( pParse != NULL )
				{
				*pParse = 0;
				}
			db_layertabrec *pLayer = (db_layertabrec *)this->m_pDrawing->findtabrec(DB_LAYERTAB, pStart,1);

			if ( pLayer != NULL )
				{
				ASSERT( pLayer->ret_type() == DB_LAYERTABREC );

				pLayer->setVpLayerFrozenByDefault( bFreeze );
				}
			// pParse points to NULL or the 0-terminator of the last symbol
			//
			pStart = pParse;
			if ( pStart != NULL )
				{
				pStart++;
				}

			// Just double check we're not out of range
			//
			if ( pStart > (szTemp + IC_ACADBUF))
				{
				break;
				}
			}
		}

	return ret;
	}
// ****************************
// PUBLIC METHOD -- class Cvplayer
//
// This is the main entry point to this class
//
Cvplayer::cmd_vplayer( void )
	{
 
	if (cmd_iswindowopen() == RTERROR)
		{
		return RTERROR;
		}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char szTemp[IC_ACADBUF];
    struct resbuf setgetrb;
    int ret;
    short rc = RTNORM;


	// ****************
	// VPLAYER only works if Tilemode is off !!!
	//
	int tilemode;
    if( SDS_getvar(NULL,DB_QTILEMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM ) 
		{ 
		SDS_FreeEventList(0);  
		return RTERROR;
		}
	
	tilemode = setgetrb.resval.rint;
	if ( tilemode != 0 )
		{
		cmd_ErrorPrompt(CMD_ERR_TILEMODE0,0); 
		SDS_FreeEventList(0);  
		return RTNORM;
		}


    for(;;) 
		{
		LOAD_COMMAND_OPTIONS_2(IDC_CMD_VPLAYER_LIST_FROZEN__2)
        if(sds_initget(0, LOADEDSTRING )!=RTNORM) 
			{
			rc = -2;		// What is -2 anyhow???
			break;
			}
		ret = sds_getkword( ResourceString(IDC_CMD_VPLAYER__N_LIST___FR_3, "\n List/Freeze/Thaw/Reset/New frozen layers/Default visibility settings : " ),szTemp );
        if(ret==RTERROR) 
			{
            rc = -2;
			break;
			}
		else if(ret==RTCAN) 
			{
            rc = -1;
			break;
			} 
		else if(ret==RTNONE) 
			{
            break;
	        }


		if(strisame("LIST"/*DNT*/,szTemp) || strisame("?"/*DNT*/,szTemp))
			{
			ret = this->listFrozenLayers();


	        } 
		else if(strisame("FREEZE"/*DNT*/,szTemp)) 
			{
			ret = this->processLayers( ResourceString(IDC_CMD_VPLAYER__NSELECT_THE_5, "\nSelect the layer(s) to freeze: " ), PROCESSFREEZE );

			} 
		else if(strisame("THAW"/*DNT*/,szTemp)) 
			{
			ret = this->processLayers( ResourceString(IDC_CMD_VPLAYER__NSELECT_THE_6, "\nSelect the layer(s) to thaw: " ), PROCESSTHAW );

			} 
		else if(strisame("RESET"/*DNT*/,szTemp))
			{
			ret = this->processLayers( ResourceString(IDC_CMD_VPLAYER__NSELECT_THE_7, "\nSelect the layer(s) to reset: " ), PROCESSRESET );
			} 
		else if(strisame("NEWFRZ"/*DNT*/,szTemp)) 
			{

			ret = this->createNewFrozenLayers();
			} 
		else if(strisame("DEFAULT"/*DNT*/,szTemp)) 
			{

			ret = this->changeDefaultViewportVisibility();
	        }
		else if (strisame("VPVISDFLT"/*DNT*/,szTemp))
		{

			ret = this->changeDefaultViewportVisibility();
		}

        	if(ret==RTERROR) 
		{
			rc = -2;
			SDS_FreeEventList(0); 
			break;
		} 
		else if(ret==RTCAN) 
		{
            		continue;
	        }
	}

	//this should only be considered in case of problems. It's removed here because it cancels lisp functions that call vplayer.
	//SDS_FreeEventList(0);  
	return(rc); 

}


// ********************************
// PUBLIC FUNCTION 
//
// Basically this is the whole entry point
// to this module
//
short cmd_vplayer(void) 
	{ 

	Cvplayer theCommand( SDS_CURDWG );

	RT rc = theCommand.cmd_vplayer();

	cmd_redrawall();

	return rc;
	}


