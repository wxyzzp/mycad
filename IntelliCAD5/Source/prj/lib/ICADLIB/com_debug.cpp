/* **********************************************************
 * PUBLIC FUNCTION
 *
 * BOOL RegCLSIDQueryValue( REFCLSID rClsID,
 * 						LPSTR lpszSubKey,
 *						LPSTR lpszValue,
 *						LONG FAR *lpcb )
 *
 * PURPOSE
 *	To get a value associated with a CLSID in the registration database
 *
 * PARAMETERS
 *	rClsID	  --   Class ID being queried
 *	lpszSubKey --  The key under the Class ID being queried
 *	lpszValue  --  String holding the return value
 *	lpcb	 --	   LONG containing the length of the returned string
 *
 * RETURN VALUE
 * BOOL bSuccess
 *
 */

static LPCSTR zszCLSID = "CLSID";

BOOL
RegCLSIDQueryValue( REFCLSID rClassId,
					LPSTR lpszSubKey,
					LPSTR lpszValue,
					LONG FAR *lpcb )
{
	HKEY hKeyCLSID = (HKEY)0;
	BOOL bRetval = FALSE;

//	ASSERT( CHECKSTR( lpszSubKey ) );
//	ASSERT( CHECKPTR( lpszValue ) );
//	ASSERT( CHECKSTRUCTPTR( lpcb ) );

	USES_CONVERSION;
	// EBATECH(CNBR) 2002/9/24 Use RegOpenKeyEx instead of RegOpenKey
	LONG regResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, zszCLSID, 0, KEY_READ, &hKeyCLSID );
	//LONG regResult = RegOpenKey( HKEY_CLASSES_ROOT, zszCLSID, &hKeyCLSID );
	if ( regResult == ERROR_SUCCESS )
	{
		LPOLESTR lpszOleClassId;

		HRESULT hResult = StringFromCLSID( rClassId, &lpszOleClassId );


		if ( SUCCEEDED( hResult ) )
		{
			LPSTR lpszClassId = OLE2A( lpszOleClassId );


			HKEY hFullKey;
			regResult = RegOpenKeyEx( hKeyCLSID, lpszClassId, 0, KEY_READ, &hFullKey );
			//regResult = RegOpenKey( hKeyCLSID, lpszClassId, &hFullKey );
			if ( regResult == ERROR_SUCCESS )
			{
				regResult = RegQueryValue( hFullKey, lpszSubKey, lpszValue, lpcb );
				if ( regResult == ERROR_SUCCESS )
				{
					bRetval = TRUE;
				}
				else
				{
					bRetval = FALSE;
					*lpcb = 0;
				}
			}
			RegCloseKey( hFullKey );

		}
		RegCloseKey( hKeyCLSID );
	}

	return bRetval;
}



typedef HRESULT (FAR PASCAL *LPFNDLLGETCLASSOBJECT)(REFCLSID rClsId,REFIID riid,LPVOID FAR *lplpI);

///////////////////////////////////////////////////////////
// HELPER FUNCTION -- debug only???
//
// HRESULT MyCoGetClassObject( REFCLSID classId,
//								DWORD dwClsCtx,
//								LPVOID lpReserved,
//								REFIID riidInterface,
//								LPVOID FAR *lplpInterface )
//
// PURPOSE
// My own implementation of CoGetClassObject, for debugging purposes
//
//
//
HRESULT
MyCoGetClassObject( REFCLSID classId,
					DWORD dwClsCtx,
					LPVOID lpReserved,
					REFIID riidInterface,
					LPVOID FAR *lplpInterface )
{

	HRESULT hRetval = ResultFromScode( E_UNEXPECTED );

	ASSERT( classId != GUID_NULL );

	LONG lLen = _MAX_PATH;
	char szBuffer[ _MAX_PATH + 2];

	USES_CONVERSION;
	// Get the Server name from the registry
	// NOTE :  Ignore dwClsCtx and assume InProcServer32
	//
	//
	if ( RegCLSIDQueryValue( classId,
							// This is a registry key used internally
							// DO NOT TRANSLATE!!!!
							//
							"InProcServer32",
							(LPSTR)szBuffer,
							&lLen ) )
	{
		// Must pass an OLESTR to CoLoadLibrary
		//
		CComBSTR lpOleServerName( A2OLE(szBuffer) );
		if ( lpOleServerName )
		{
			HINSTANCE hLib = CoLoadLibrary( lpOleServerName, TRUE );
			ASSERT( hLib != NULL );

			// Now we can get rid of the string
			//
			if ( hLib )
			{
				// If we loaded the library, get and call its DllGetClassObject
				// method
				//
				LPFNDLLGETCLASSOBJECT lpfnDllProc = (LPFNDLLGETCLASSOBJECT)
								GetProcAddress( hLib,
							// DO NOT TRANSLATE
							// This is part of COM
							//
								"DllGetClassObject" );

				if ( lpfnDllProc != NULL )
				{
					hRetval = lpfnDllProc( classId, riidInterface, lplpInterface );
					ASSERT( SUCCEEDED( hRetval ) );
				}
			}
		}
	}


	return hRetval;
}


HRESULT
MyCreateInstance( REFCLSID classid,
					LPUNKNOWN lpOuterUnknown,
					DWORD dwClsCtx,
					REFIID riid,
					LPVOID FAR* ppvNewInstance )
{
	HRESULT hResult;

	ASSERT( CHECKPTR( ppvNewInstance ) );


	{
		// the control does not support licensing so use IClassFactory
		// CreateInstance to create an instance.
		//
		LPCLASSFACTORY lpClassFactory = NULL;

		hResult = MyCoGetClassObject( classid,	 // CLSID for the object
									dwClsCtx, 	 // Context
									NULL,		 // Reserved
									IID_IClassFactory,	// Interface
									(LPVOID FAR *)&lpClassFactory 	// return
									);

		if ( SUCCEEDED( hResult ) )
	 	{
			hResult = lpClassFactory->CreateInstance( lpOuterUnknown,
													riid,
													ppvNewInstance );
			lpClassFactory->Release();
		}

	}

	return hResult;
}
