/** FILENAME:     ExceptionManager.h- Contains declarations of class CExceptionManager.
* Copyright  (C) Copyright 1998-99.  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract : This class does the overall exception handling
*			 Gradually add all the cases to the _EXCEP_CODES_ and a string 
*			 description for that case in the ExceptionManager.cpp file
*            so that GetExceptionDescription() function returns the proper error string 
*Audit Log 
*
*
*Audit Trail
*
*
*/



#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

#include "stdafx.h"

 

/* ThrowCExceptionManager(int);
*  Function which throws the exception.
*
*
*
*/

void ThrowCExceptionManager(    // This function throws the exception
							int // i : Exception code
							);

/*
/* ThrowCExceptionManager(CString);
*  Function which throws the exception.
*
*
*
*/

void ThrowCExceptionManager(		// This function throws the exception
							CString // i : String describing the exception
							);


// NEED TO KEEP ON ADDING NEW EXCEPTION CODES.
enum _EXCEP_CODES_{
					MEM_ALLOC_FAILED,
					LIST_INSERT_FAILED,
					TREE_CREATE_FAILED,
                    DCL_FILE_NOT_FOUND,
					ERROR_IN_DCL_FILE,
					PARSER_INIT_FAILED,
					DCL_TILES_INIT_FAILED,
					PARSER_LOAD_DCL_FAILED,
					REINIT_NON_DEF_TILE_FAILS,
					DCL_UNLOAD_FAIL,
					SET_AUDIT_LEVEL_FAIL,
					LOG_ERROR_FAILED,
					CIRCULAR_REF_FIND_FAIL,
					AUDIT_LEVEL_CHECK_FAILED,
					ATTRIBUTE_VALIDATE_FAIL,
					GEN_ERR_STRING_FAILED

                   }; 


class CExceptionManager : public CException
{
   DECLARE_DYNAMIC(CExceptionManager)

   public:

      
      inline CExceptionManager(                    //Constructor
								int nExceptionCode // i : Exception Code
								) 
      {
         GetExceptionDescription(nExceptionCode, m_strExceptionDescrip);
      }

      
      inline CExceptionManager(                   //Constructor
								CString strException // i : Exception String
								)
      {
         m_strExceptionDescrip = strException;
      }

      
      void GetExceptionDescription(						// Returns description of exception for the specified code.
									int vnExceptionCode, // i : Exception Code
									CString& vstrExcepDescrip // o : Exception String
									);

	  ~CExceptionManager() 
		  {
		  m_strExceptionDescrip = "";
		  }
      
      int m_nExceptionCode; // Exception code
      CString m_strExceptionDescrip; // Description of the exception
};



#endif
