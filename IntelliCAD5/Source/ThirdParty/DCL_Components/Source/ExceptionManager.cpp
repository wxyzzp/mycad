/** FILENAME:     ExceptionManager.cpp- Contains implementation of class CExceptionManager.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "ExceptionManager.h"

IMPLEMENT_DYNAMIC(CExceptionManager, CException);



/************************************************************************************
/* + CExceptionManager::GetExceptionDescription(int vnExceptionCode, CString& vstrExDescription)
*    Returns description of exception for the specified code.
*
*
*
*
*/

void CExceptionManager::GetExceptionDescription(                     // Returns description of exception for the specified code.
												int vnExceptionCode, // i : Exception Code
												CString& vstrExDescrip // o : Exception String
												)
	{
	CString strDescrip;

	switch (vnExceptionCode)
		{ // add different cases here
			case MEM_ALLOC_FAILED :
							strDescrip = _T("Exception : Memory allocation failed");
							break;
			case LIST_INSERT_FAILED :
							strDescrip = _T("Exception : List Insertion failed");
							break;
            case DCL_FILE_NOT_FOUND:
							strDescrip = _T("Exception : DCL File not found");
							break;
			case ERROR_IN_DCL_FILE:
							strDescrip = _T("Exception : Error in DCL File");
							break;
            case PARSER_INIT_FAILED:
							strDescrip = _T("There was an EXCEPTION  during initialization of parser class");
							break;
			case DCL_TILES_INIT_FAILED:
							strDescrip = _T("There was an EXCEPTION during initialization of primitves/base tiles");
							break;
			case PARSER_LOAD_DCL_FAILED:
							strDescrip = _T("There was an EXCEPTION during loading of DCL file");
							break;
			case REINIT_NON_DEF_TILE_FAILS:
							strDescrip = _T("There was an EXCEPTION during reinitialization of nondefined nodes");
							break;
			case DCL_UNLOAD_FAIL:
							strDescrip = _T("There was an EXCEPTION during unloading of DCL file");
							break;
			case SET_AUDIT_LEVEL_FAIL:
							strDescrip = _T("There was an EXCEPTION during semantic audits of DCL file");
							break;
			case LOG_ERROR_FAILED:
							strDescrip = _T("There was an EXCEPTION during error logging for  errors in DCL file");
							break;
			case CIRCULAR_REF_FIND_FAIL:
							strDescrip = _T("There was an EXCEPTION during tracing circular reference in DCL file");
							break;
			case AUDIT_LEVEL_CHECK_FAILED:
							strDescrip = _T("There was an EXCEPTION during semantic audits of DCL file");
							break;
			case ATTRIBUTE_VALIDATE_FAIL:
							strDescrip = _T("There was an EXCEPTION during validation of atributes in DCL file");
							break;
			case GEN_ERR_STRING_FAILED:
							strDescrip = _T("There was an EXCEPTION during error string generation of DCL file");
							break;
			default:
						    strDescrip = _T("Unknown Exception");
		}

	vstrExDescrip = strDescrip;
	}

/************************************************************************************
/* + ThrowCExceptionManager( int nExceptionCode )              
*    // Function which throws the exception 
*
*
*
*
*/      

void ThrowCExceptionManager(              // Function which throws the exception.
							int nExceptionCode // i : Exception Code
							)
	{
	CExceptionManager *pExpObj = new CExceptionManager(nExceptionCode);
	throw pExpObj; 
	}
/************************************************************************************
/* +  ThrowCExceptionManager( CString strExcepDescrip )             
*   // Overloaded function which throws the exception
*
*
*
*
*/

void ThrowCExceptionManager(               // Overloaded function which throws the exception.
							CString strExcepDescrip // i : Exception String
							)
	{
	CExceptionManager *pExpObj = new CExceptionManager(strExcepDescrip);
	throw pExpObj; 
	}
