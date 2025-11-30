/** FILENAME:     Parser.cpp- Contains implementation of class Parser.
*  Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
*   Abstract
*            This class holds the the list of all DCL files loaded. This is a data member
*            of the Main class. This class has the entry point which creates the parser class
*            (Grammer) for parsing the loaded DCL files.
*
*            This class also has a list of NonDefinedNodes found during parsing. The ReInitNonDefinedNodes
*            deletes these pointers once the node definition is found.
*
*  Audit Log
*
*
*  Audit Trail
*
*
*/


#include "stdafx.h"
#include "resource.h"
#include <stdlib.h>
#include "errno.h"
#include "sds.h"
#include "parser.h"
#include "fileinfo.h"
#include "treenode.h"
#include "tileattributes.h"
#include "constants.h"
#include "exceptionmanager.h"
#include "Tree.h"
#include "errormessage.h"
#include "direct.h"
#include "attributeinfo.h"
#include "auditlevel.h"



// These include files are generated/provided by PCCTS.

#include "tokens.h"
#include "dlexerBase.h"
#include "dlglexer.h"
#include "atoken.h"
#include "grammer.h"

typedef ANTLRCommonToken antlrtoken;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


unsigned int Parser::m_iSeed = 50000;      // Used to give a Unique ID to each DCL file loaded.


#ifdef _TESTEXE
int sds_findfiles(char *filetofind,char *pathfound);
#define pfnsdsFindFiles sds_findfiles
#endif


/************************************************************************************
/* + Parser::Parser() -  Constructor of Parser class
*
*
*
*/
Parser::Parser():m_ListOfDCLFileData(),m_ListOfNonDefinedNodes()      // Constructor of Parser class
	{
	m_bErrorOccured = FALSE;
	m_nAuditLevel = 1;
	m_bHintPresent = FALSE;
	m_bWarningOccurred = FALSE;
	m_nHintCount = 0;
	try{
		// Call this funcion to populate the attribute info map member.
		PopulateAttributeInfo();
		}
	catch(CExceptionManager *pEx)
		{
		TRACE("Error generating an attribute info list\n");
		pEx->Delete();
		}

	}


/************************************************************************************
/* + Parser::~Parser() - Destructor for Parser class
*
*
*
*/
Parser::~Parser()                     // Destructor for Parser class
	{

	//  remove the entries for all loaded files
	POSITION position = m_ListOfDCLFileData.GetHeadPosition();
	for ( int i = 0; i < m_ListOfDCLFileData.GetCount(); i++)
		{
		FileInfo* pFileInfo = (FileInfo*) m_ListOfDCLFileData.GetNext(position);
		delete pFileInfo;
		}

	m_ListOfDCLFileData.RemoveAll();

	//deleting nodes pointed by m_ListofNonDefinedNodes
	position =  m_ListOfNonDefinedNodes.GetHeadPosition();

	for ( i = 0; i <  m_ListOfNonDefinedNodes.GetCount(); i++)
		{
		NonDefinedNode* pNonDefinedNode = (NonDefinedNode*)m_ListOfNonDefinedNodes.GetNext(position);
		delete pNonDefinedNode;
		}
	m_ListOfNonDefinedNodes.RemoveAll();


	//  deleting m_mapAttributeInfo
	CString tempString;
	AttributeInfo *pAttributeInfo;
	position = m_mapAttributeInfo.GetStartPosition() ;
	while ( position )
		{
		m_mapAttributeInfo.GetNextAssoc(position,tempString,(CObject *&)pAttributeInfo);
		delete pAttributeInfo;
		}

	m_mapAttributeInfo.RemoveAll();

	}



/************************************************************************************************************************
/*+ int Parser::DCLParserStartUp(LPTSTR wszDCLFileName) - Entry point function which calls the parser module
*
*      This method creates object of FileInfo class and adds it to the FileInfo list.
*      Then the Grammer class is instantiated.
*
*
*/
int Parser::DCLParserStartUp(                              // entry point function which calls the parser
							 LPTSTR wszDCLFileName,        // i: the name of loaded DCL file name
							 int *nDlgFileId = 0,          // o: Set to value of unique identifier, if bIsIncluded = FALSE.
							 BOOL bIsIncluded			   // i: True for included files,FALSE otherwise.(Default = TRUE).
							 )
	{

	// create a unique id for this particular file,
    // then create a FileInfo and add it the CObList m_pListOfDCLFileData
	int nRet = SDS_RTNORM;                  // Holds the return value.
	CString wszFileName = wszDCLFileName;   // The name of the DCL file to be parsed.
    FILE *dclFile;
	POSITION pos;
	CFileFind filefinder;                   // Used to confirm existence of file.
	CString wszFilePath;                    // Holds the file path of DCL file returned by sds_findfile.
	static CString wszCurrDCLPath;          // This holds the current path of the DCL file.
	                                        // Used to search for the included DCL files in the same path of
	                                        // the DCL file in which it was included.

	// FileInfo pointer
    FileInfo * pDCLFileInfo;

	TRACE("%s",wszFileName);

	int count = wszFileName.Replace(_T('\\'),_T('/'));   // Replace the "\" with "/" for procesing.

	// Set the extension of the DCL file, if no extension is given.
    CString wszTempFileName ;

	int nPosOfSlash = wszFileName.ReverseFind(_T('/'));

	int nPosOfDot = wszFileName.ReverseFind(_T('.'));

	if ( nPosOfDot < nPosOfSlash || nPosOfDot == -1 )
		{
        wszFileName = wszFileName + _T(".dcl");
		}


	// After the extension for the DCL file is set.

	if ( count != 0 )                                    // If the user has given a file with a path.
		{
		if (!filefinder.FindFile(wszFileName))           // Find the file.
			{                                            // If the file is not found in the given directory
			ErrorMessage ErrMessage;                     // give an error.
			CString wszErrorMessage;
			CString wszResourceString;
			wszResourceString.LoadString(IDS_DCL_FILE_NOT_FOUND);   // Load the resource string.
			wszErrorMessage.Format(wszResourceString,wszFileName);
			ErrMessage.Message(wszErrorMessage);                    // Call the method of ErrorMessage class.
			return SDS_RTERROR;
			}
		 }
	else                                                           // If the filename was given without the path.
		{
		// If only the filename was given without the path,
		// first the file is searched in the directory same as that of the file in which it was included, and then
		// they are searched for in the search path of IntelliCAD.

		// The searching in the current directory is done only for included DCL files.


//Elias Item:14->
//        char strPath[_MAX_PATH + 1];
        char strPath[512 + 1];   // see: ic_findinpath(...) in sds_findfile.cpp(110): if (GetCurrentDirectory(IC_ACADBUF, szPathFound) == 0)
//Elias Item:14<-

		DWORD dwBuffLength =  0  ;
		if ( bIsIncluded == TRUE )
			{
			// If the DCL file was included then, search in the same path of that of the parent DCL file.
			char *strFilePart;
			dwBuffLength = _MAX_PATH ;

			// Search for the file.
			dwBuffLength = SearchPath((LPCTSTR)wszCurrDCLPath,(LPCTSTR)wszFileName,NULL,dwBuffLength,strPath,&strFilePart);

			if ( dwBuffLength > _MAX_PATH )
				{
				// If bufflength required to hold the path is greater than the length allocated, then
				// search again with extra lenght string.
				char *strNewPath = new char[ dwBuffLength + 1];
				ZeroMemory(strNewPath,dwBuffLength + 1);
				dwBuffLength = SearchPath((LPCTSTR)wszCurrDCLPath,(LPCTSTR)wszFileName,NULL,dwBuffLength,strNewPath,&strFilePart);
				wszFileName = strNewPath ;
				delete []  strNewPath;
				}
			else
				{
				if ( dwBuffLength  > 0 )
					{
					// If path found, then set the filename with the path.
					wszFileName = strPath;
					}
				}
			}

		// Call icad.exe's sds_findfiles function.

		// If the path was not found, or the filename is the file loaded, then search in the standard search path
		// of IntelliCAD thru the sds_finfile function.
		if ( dwBuffLength == 0 )
			{

			#ifndef _TESTEXE
			// Get the function pointer for sds_finfile.
			HMODULE   hmodule = GetModuleHandle(NULL);
			ASSERT(hmodule);
			typedef int (* fnptr_sds_findfiles)(char *filetofind,char *pathfound);
			fnptr_sds_findfiles pfnsdsFindFiles;
			if(hmodule)
				{
				pfnsdsFindFiles = (fnptr_sds_findfiles)GetProcAddress(hmodule,"sds_findfile");
				ASSERT(pfnsdsFindFiles );
				}
			#endif

			// Get the path of the file.
			if ( pfnsdsFindFiles((LPTSTR)(LPCTSTR)wszFileName,strPath) == SDS_RTERROR )  // Call the sds_findfiles method to get the path of file.
				{
				// If file not found report an error.
				wszFilePath = strPath;			                      // If file not found, then report an error.
				ErrorMessage ErrMessage;
				CString wszErrorMessage;
				CString wszResourceString;
                this->set_ErrorOccured(TRUE);
				wszResourceString.LoadString(IDS_DCL_FILE_NOT_FOUND_IN_SEARCH_PATH);
				// Call the ErrorMessage method.
				wszErrorMessage.Format(wszResourceString,wszFileName);
				ErrMessage.Message(wszErrorMessage);
				return SDS_RTERROR;
				}
			else
				{
				wszFileName = strPath;             // If path was found, append filename to path.
				}
			}

		}

	// Replace all '\' with '/'.
	wszFileName.Replace(_T('\\'),_T('/'));

	//Set the output parameter nDlgFileId value to unique id generated
	if ( bIsIncluded == FALSE )
		{
		// Generate unique id if file is loaded, and it is not
		// an included file.
		Sleep(100);

		static int nFileIDForPrimAndBase =  0;
		nFileIDForPrimAndBase++;

		if ( nFileIDForPrimAndBase <= 2   )
			{
			*nDlgFileId = m_nCurrentFileID = nFileIDForPrimAndBase;
			}
		else
			{
 			*nDlgFileId = m_nCurrentFileID = GenerateUniqueFileID();
			}

		TRACE("THE UNIQUE NUMBER GENERATED IS %d\n",*nDlgFileId);
		}

	try
		{
		// Create a fileinfo object .
	    pDCLFileInfo = new FileInfo(wszFileName,m_nCurrentFileID,bIsIncluded);
		}
	catch(...)
		{
		TRACE0("Memory Allocation failed");
        return SDS_RTERROR;
		}

	ASSERT( pDCLFileInfo );

	// Add FileInfo to the File list.
	try
		{
		// Append fileinfo to the list of dcl files.
		pos  = m_ListOfDCLFileData.AddTail(pDCLFileInfo);
		}
	catch(...)
		{
		TRACE0("List Insertion Fails for File Info\n");
		return SDS_RTERROR;
		}


	if ( bIsIncluded == FALSE )
		{
		// Set the curerently loaded file position in the list.
		// This value is used to find referenced nodes.
		wszCurrDCLPath = wszFileName ;
		wszCurrDCLPath = wszCurrDCLPath.Left(wszCurrDCLPath.ReverseFind(_T('/')));
		m_CurrentFilePosition = pos;
		}


	// Make a call to program function of Grammer.
	TRACE("\nFileName:%s\n",wszFileName);

    dclFile = fopen(wszFileName ,"rt");
	ASSERT( dclFile );

	// An error reported if unable to open DCL files.
	try
		{
		if ( dclFile == NULL )
			{
			TRACE0("Unable To Open DCL file\n");
			ErrorMessage ErrMessage;                     // give an error.
			CString wszErrorMessage;
			CString wszResourceString;
			wszResourceString.LoadString(IDS_UNABLE_TO_OPEN_FILE);   // Load the resource string.
			wszErrorMessage.Format(wszResourceString,wszFileName);
			ErrMessage.Message(wszErrorMessage);                    // Call
   			ThrowCExceptionManager(DCL_FILE_NOT_FOUND);
			}

		// initialization of classes used by parser
		DLGFileInput in(dclFile,wszFileName);
		DLGLexer scanner(&in);
		ANTLRTokenBuffer pipe(&scanner);
		antlrtoken tok;
		scanner.setToken(&tok);

		// Create an instance of parser.
		Grammer grammerParser(&pipe);

		grammerParser.m_pFileInfo = pDCLFileInfo ;

		// Initialize parser
		grammerParser.init();

		// Start first rule
		grammerParser.program();

		// Remove all the nodes stack to report errors.
		grammerParser.lstNodesWithErrors.RemoveAll();
		grammerParser.lstNonContianerNodes.RemoveAll();
		}
	catch(CExceptionManager* pException)
		{
		ASSERT(pException);
		pException->Delete();
		this->m_bErrorOccured = TRUE;
		// This operation was moved to the LoadDCLFile file
		// cleanup section.
//		UnLoadDCLFile(this->m_nCurrentFileID);
//		m_CurrentFilePosition = NULL;
		goto fail;

		}

	// Close the file handle.
	fclose(dclFile);

    return SDS_RTNORM;
fail:
	return SDS_RTERROR;

	}



/************************************************************************************
/* + long Parser::GenerateUniqueFileID() - A unique ID generated for mappping loaded files
*
*
*
*
*
*/
long Parser::GenerateUniqueFileID()                   // A unique ID generated for mappping loaded files
	{

/* Seed the random-number generator with current time so that
     the numbers will be different every time we run.
*/
	long lRandom;
	unsigned int iSeed;
	time_t long_time;
	struct tm * ltm;
	time(&long_time);
	ltm = localtime(&long_time);
	ASSERT( ltm );
	if (ltm)
		{

		iSeed = ltm->tm_sec + ltm->tm_min + ltm->tm_hour + ltm->tm_mday + ltm->tm_mon + ltm->tm_year + ltm->tm_wday + ltm->tm_yday;
		iSeed += ++Parser::m_iSeed ;
		srand(iSeed);
		lRandom = (long)rand();
		}
	else
		{
		iSeed = Parser::m_iSeed;
		Parser::m_iSeed++;
		srand(iSeed);
		lRandom = (long)rand();
		}
    return lRandom;
	}


/*****************************************************************************************************
/* + int Parser::FindReferencedNode(CString& wszTileName, int nType, TreeNode** ppTreeNode) -
*
*        Calls the FileInfo class's corresponding function to find referenced nodes
*
*
*
*/
int Parser::FindReferencedNode(                          // Calls the FileInfo class's corresponding function to find referenced nodes.
							   CString& wszTileName,     // i: Tthe tile name to be searched.
							   int nType,                // i: Type - 0 for dialog,1 for prototype or subassembly.
							   TreeNode** ppTreeNode     // o: This member is made to point to the referenced node if exists.
							   )
	{
	int nRet = SDS_RTERROR;
	Tile tile;
	CString wszTile = wszTileName;
	// Get the count of files already loaded
	int nCount = m_ListOfDCLFileData.GetCount();

	// This condition takes care of parsing the initial two file entries in the file list
	// They are the ibase.dcl and primitives.dcl
	if ( nCount > 2 )
		{
		nCount = 2;
		}

	if ( nCount )
		{
		POSITION position = m_ListOfDCLFileData.GetHeadPosition();
		// Go in a loop and call GetReferencedNode for each of them.
		for ( int i = 0; i < nCount; i++ )
			{
			FileInfo * pFileInfo = (FileInfo *)m_ListOfDCLFileData.GetNext(position);
			nRet = pFileInfo->FindReferencedNode(wszTileName,nType,ppTreeNode);
			if ( !position || (nRet == SDS_RTNORM) )
				{
				break;   // The node has been found (nRet = SDS_RTNORM) or no more trees left to search(nRet = SDS_RTERROR).
				}

			}

		// After searching in the primitives and base dcl resources
		// search for definitions in the loaded files, including the files included in the
		// loaded file.

        position = m_CurrentFilePosition;
        if ( nRet == SDS_RTERROR )
			{
			while (position)
				{
				FileInfo * pFileInfo = (FileInfo *)m_ListOfDCLFileData.GetNext(position);
				// The file ID is the same for all the included files and the loaded file.
				// Therefore, searching is done only in those files.
                if ( pFileInfo->get_DCLFileID() ==  m_nCurrentFileID )
					{
					nRet = pFileInfo->FindReferencedNode(wszTileName,nType,ppTreeNode);
					if ( !position || (nRet == SDS_RTNORM) )
						{
						break;   // The node has been found (nRet = SDS_RTNORM) or no more trees left to search(nRet = SDS_RTERROR).
						}
					}
				}
			 }
		}

	return nRet;
	}




/************************************************************************************
/* + int Parser::ReInitNonDefinedNode(FileInfo * pFileInfo) -
*
*     This method iterates through the list of non-defined nodes,from right to
*  to left and then from left to right.This is put it in loop till non more
*  definitions of the node can be completed.
*
*
*/
int Parser::ReInitNonDefinedNode()                     // Completes the definition of nodes whose references were not found during parsing.
	{

    TreeNode *ppTreeNode = NULL;
	CString wszTileName;
	POSITION position,positionForDeletion;
	int nResult =  SDS_RTERROR;
	BOOL bNonDefinedNodeExists = FALSE;
	NonDefinedNode *pNonDefinedNode = NULL ;
	int iCountOfNonDefinedNodes;
	int iPrevCountofNonDefinedNodes = 0 ;
	int nReturnCircRefExists = FALSE;


	try
		{


		iCountOfNonDefinedNodes = m_ListOfNonDefinedNodes.GetCount();

		while (1)
			{
			iCountOfNonDefinedNodes = m_ListOfNonDefinedNodes.GetCount();

			/* If count of nondefined nodes does not change after iteration
			* then no more definitions can be completed
			* therefore,iterate finally and remove all entries ,while
			* listing down the non defined nodes in Error.Log.
			*/
			if ( iCountOfNonDefinedNodes == iPrevCountofNonDefinedNodes )
				{

				nReturnCircRefExists = ReportCircularReference();

				// Get the latest count of nondefined nodes, since any circular reference of nodes
				// present would have been removed by the call to ReportCircularReference in the
				// previuos statement.
				iCountOfNonDefinedNodes = this->m_ListOfNonDefinedNodes.GetCount();
				position = m_ListOfNonDefinedNodes.GetHeadPosition();

				for ( int iLoop =  0 ; iLoop < iCountOfNonDefinedNodes ; iLoop++ )
					{
					positionForDeletion = position ;
					pNonDefinedNode = (NonDefinedNode*)m_ListOfNonDefinedNodes.GetNext(position);

					// write the error to error log , and remove entries
      				if ( pNonDefinedNode )
						{
						// Call the LogError to write error to Error.log.
						CString wszErrorMessage,wszResourceString;
						wszResourceString.LoadString(IDS_NONDEFINED_TILE);
						CString wszWidgetName = (pNonDefinedNode->get_NodePlaceHolder())->m_Tile.get_TileName();
						if ( wszWidgetName.IsEmpty())
							{
							wszWidgetName = pNonDefinedNode->get_TileNameReferenced();
							}

						wszErrorMessage.Format(wszResourceString,wszWidgetName);


	//                  this->LogErrors(wszErrorMessage);
						if ( m_nAuditLevel != AUDIT_LEVEL_0 )
							{
							this->AddErrorString(wszErrorMessage);
							}
						delete pNonDefinedNode;
						pNonDefinedNode = NULL ;
						m_ListOfNonDefinedNodes.RemoveAt(positionForDeletion);
						bNonDefinedNodeExists = TRUE;
						} // End of if ( pNonDefinedNode )
					}  // End of for loop.

					break;
				} // end of if condition to check for ( iCountOfNonDefinedNodes == iPrevCountofNonDefinedNodes )
			else
				{
				iPrevCountofNonDefinedNodes = iCountOfNonDefinedNodes ;
				}


			//  Iteration is started from the end of list.
			if ( iCountOfNonDefinedNodes )
				{
				position = m_ListOfNonDefinedNodes.GetTailPosition();
				// For each NonDefined node a search  to the corresponding reference is made in all
				// loaded DCL files.

				// Searching first done from the last NonDefinedNode.
      			for ( int iLoop = iCountOfNonDefinedNodes-1; iLoop >= 0 ; iLoop-- )
					{
					positionForDeletion = position ;
					pNonDefinedNode = (NonDefinedNode*)m_ListOfNonDefinedNodes.GetPrev(position);
					wszTileName = pNonDefinedNode->get_TileNameReferenced();

					// This function call searches for references in all the currently loaded DCL Files.
					if ( (pNonDefinedNode->get_NodePlaceHolder())->m_Tile.GetTileNameToBeCloned() == wszTileName )
						{
                  		nResult =  FindReferencedNode(wszTileName,1,&ppTreeNode);
						}
					else
						{
						nResult =  SDS_RTERROR;
						}

					if ( nResult == SDS_RTNORM )
						{
						// Clone the node or tree depending upon whether it is a proptype or sub-assembly
						(pNonDefinedNode->get_NodePlaceHolder())->CloneTree(ppTreeNode,pNonDefinedNode->get_ReferenceCount());
						(pNonDefinedNode->get_NodePlaceHolder())->set_IsComplete(TRUE);

						// check whether the node is not a container , but has some children ,
						// if so..give a error
						if ( (pNonDefinedNode->get_NodePlaceHolder())->m_Tile.IsContainer() == FALSE
								  && (pNonDefinedNode->get_NodePlaceHolder())->HasChild() == TRUE )
							{
							// Call the LogError method to write error to Error.log

							CString wszFuncName = (pNonDefinedNode->get_NodePlaceHolder())->get_DefinitionName();

							CString wszKeyName = ((pNonDefinedNode->get_NodePlaceHolder())->m_Tile.get_ListOfAttributes())->get_Key();

							AuditLevelProcessor audlvlProc(wszFuncName,
												((pNonDefinedNode->get_NodePlaceHolder())->m_Tile).get_RenderableTileName(),
												wszKeyName);

							audlvlProc.FormatErrorMessage(
												  IDS_ERROR_HEADER,
												  IDS_WIDGET_CANNOT_HAVE_CHILDREN,
												  1
												  );

							bNonDefinedNodeExists = TRUE;
							}
							// Once a NonDefinedNode is completed, delete it from list.
							delete pNonDefinedNode;
							m_ListOfNonDefinedNodes.RemoveAt(positionForDeletion);
						}
					}
				}


			// Count of NonDefinedNodes remaining after first traversal.
			iCountOfNonDefinedNodes = m_ListOfNonDefinedNodes.GetCount();

			// Iteration from begining of list.
			if ( iCountOfNonDefinedNodes )
				{
				position = m_ListOfNonDefinedNodes.GetHeadPosition();
				// For each NonDefinedNode a search  to the corresponding reference is made in all
				// loaded DCL files.

				// Searching first done from the first non-defined node.
      			for(int iLoop = iCountOfNonDefinedNodes-1; iLoop >= 0 ; iLoop--)
					{
					positionForDeletion = position ;
					pNonDefinedNode = (NonDefinedNode*)m_ListOfNonDefinedNodes.GetNext(position);

					wszTileName = pNonDefinedNode->get_TileNameReferenced();

					// This function call searches for references in all the currently loaded DCL Files.
					if ( (pNonDefinedNode->get_NodePlaceHolder())->m_Tile.GetTileNameToBeCloned() == wszTileName )
						{
                  		nResult =  FindReferencedNode(wszTileName,1,&ppTreeNode);
						}
					else
						{
						nResult =  SDS_RTERROR;
						}

					if ( nResult == SDS_RTNORM )
						{
						// Clone the node or tree depending upon whether it is a prototype or sub-assembly.
						(pNonDefinedNode->get_NodePlaceHolder())->CloneTree(ppTreeNode,pNonDefinedNode->get_ReferenceCount());
						(pNonDefinedNode->get_NodePlaceHolder())->set_IsComplete(TRUE);

						// Check whether the node is not a container , but has some children ,
						// if so..give a error .
						if ( (pNonDefinedNode->get_NodePlaceHolder())->m_Tile.IsContainer() == FALSE
								  && (pNonDefinedNode->get_NodePlaceHolder())->HasChild() == TRUE )
							{
							// Call the LogError method to write error to Error.log
							CString wszFuncName = (pNonDefinedNode->get_NodePlaceHolder())->get_DefinitionName();

							CString wszKeyName = ((pNonDefinedNode->get_NodePlaceHolder())->m_Tile.get_ListOfAttributes())->get_Key();

							AuditLevelProcessor audlvlProc(wszFuncName,
												((pNonDefinedNode->get_NodePlaceHolder())->m_Tile).get_RenderableTileName(),
												wszKeyName);

							audlvlProc.FormatErrorMessage(
												  IDS_ERROR_HEADER,
												  IDS_WIDGET_CANNOT_HAVE_CHILDREN,
												  1
												  );
							bNonDefinedNodeExists = TRUE;
							} //End of if for validation.
							delete pNonDefinedNode;
							m_ListOfNonDefinedNodes.RemoveAt(positionForDeletion);


						}
					}

				}

				// If no more nodes to define , break from loop.
				if (iCountOfNonDefinedNodes == 0)
					  break;


			} // End of while.


			// Return value depending upon nbNonDefinedNodeExists , which is set to true if there
			// are non-defined nodes present.
			if ( bNonDefinedNodeExists )
				{
				// This condition is given so that, for audit level 0 , only for circular references
				// an error is retruned, or else the dialog generation continues.
				if ( nReturnCircRefExists == FALSE && m_nAuditLevel == AUDIT_LEVEL_0 )
					{
					 return SDS_RTNORM;
					}
				else
					{
					return SDS_RTERROR;
					}
				}
			else
				{
				return SDS_RTNORM;
				}
			}
		catch(CException *e)
			{
			if ( e->IsKindOf(RUNTIME_CLASS(CExceptionManager)) )
				{
				throw e;
				}
			else
				{
				ThrowCExceptionManager(REINIT_NON_DEF_TILE_FAILS);
				}
			}
		return SDS_RTNORM;

		 }


/*************************************************************************************************
/* + int Parser::AddNonDefinedNode(NonDefinedNode *pNonDefinedNode)
*
*         Add the NonDefinedNode to the list of NonDefinedNodes(i.e m_ListOfNonDefinedNode).
*
*
*
*/
int Parser::AddNonDefinedNode(									  // Adds a NonDefinedNode to the m_ListOfNonDefinedNodes datamember.
							  NonDefinedNode *pNonDefinedNode	  // Pointer to the NonDefinedNode object to be added.
							  )
	{
	ASSERT( pNonDefinedNode );
	// Add to the list
	try
		{
		m_ListOfNonDefinedNodes.AddTail(pNonDefinedNode);
		}
	catch(...)
		{
		ThrowCExceptionManager(LIST_INSERT_FAILED);
		}

   return SDS_RTNORM;

	}

/**************************************************************************************************
/* + POSITION Parser::AddFileInfo(FileInfo *pFileInfo) - Adds FileInfo pointer to the list
*
*     This method adds a FileInfo pointer to the m_ListOfDCLFileData.
*
*
*
*/
POSITION Parser::AddFileInfo(								// Adds a FileInfo pointer to the ListOfDCLFileData data member.
							 FileInfo *pFileInfo			// Pointer to the FileInfo  class to be added.
							 )
	{
	ASSERT(pFileInfo);
	POSITION pos;
    // Add to the list.
	try
		{
		pos = m_ListOfDCLFileData.AddTail(pFileInfo);
		}
	catch(...)
		{
       ThrowCExceptionManager(LIST_INSERT_FAILED);
		}

    return pos;


}


/************************************************************************************
/* + Tree* Parser::FindReferencedTree(CString wszTileNam -
*
*	Calls the FileInfo class's corresponding function to find referenced Tree.
*
*
*
*/

Tree* Parser::FindReferencedTree(
								 CString wszTileName, // i: The tile name to be searched.
								 int iDCLFileID		  // i: DCLFileID
								 )
	{
	int nRet = SDS_RTERROR;
	Tile tile;
	CString wszTile = wszTileName;
	Tree * pTree = NULL;
	// Get the count of files already loaded
	int nCount = m_ListOfDCLFileData.GetCount();
	if ( nCount )
		{
		POSITION position = m_ListOfDCLFileData.GetTailPosition();
	// Go in a loop and call GetReferncedNode for each of them.
		for ( int i = 0; i < nCount; i++ )
			{
			FileInfo * pfileInfo = (FileInfo *)m_ListOfDCLFileData.GetPrev(position);
			if ( pfileInfo->get_DCLFileID() == iDCLFileID )
				{
				pTree = pfileInfo->FindReferencedTree(wszTileName);
				if ( !position || (pTree != NULL) )
					{
					return pTree;   // The node has been found (nRet = SDS_RTNORM) or no more trees left to search(nRet = SDS_RTERROR).
					}
				}
			}
		}
	return pTree;

}


/************************************************************************************
/* + BOOL Parser::HasErrorOccured() -
*
*    Returns the value of m_bErrorOccured data member.
*
*
*
*/
BOOL Parser::HasErrorOccured()  // Returns the value of m_bErrorOccured data member.
	{
    return m_bErrorOccured;
	}


/************************************************************************************
/* + int Parser::set_ErrorOccured() -
*
*    Sets the m_bErrorOccured data member.
*
*
*
*/
int Parser::set_ErrorOccured(                      //  Sets the m_bErrorOccured data member.
							 BOOL bErrorFlag       //  Value to be set.
							 )
	{
	m_bErrorOccured = bErrorFlag;
	return SDS_RTNORM;
	}



/*******************************************************************************************************
/* + int Parser::LoadPrimitivesOrBaseDCL(int nDCLToLoad) -
*
*     This method is called from the Init method of Main class.
*
*  a. This method is called to load the primitives.dcl from resource.
*  b. Called if a default path for base.dcl is not chosen by the user.
*
*/
int Parser::LoadPrimitivesOrBaseDCL(                                  // Called to load primitives or base DCL files from resource.
									int nDCLToLoad                    // Resource to load
									)
	{
    //AFX_MANAGE_STATE(AfxGetStaticModuleState())

    int nReturn = SDS_RTERROR;
	LPTSTR lpTempPathName;											// To store the temporary path name.
	LPTSTR lpTempFileName;											// To store the temporary tile name.
	LPVOID pGlobalMem;												// Used to store pointer to loaded resource.
	HGLOBAL hGlobal;												// Handle.
	HRSRC hsrc;														// Handle.
	CString wszResourceString;


	try
		{
		wszResourceString.LoadString(AFX_IDS_APP_TITLE);
		HMODULE hMod = ::GetModuleHandle((LPCTSTR)wszResourceString);           // Get the module handle.
		ASSERT( hMod );
		int nDlgFileId;                                                 // The unique file id given to the loaded file.

		// Load Primitives.dcl or Base.dcl from resouce.
		if (nDCLToLoad == PRIMITIVES_DCL)
			{
				hsrc = FindResource(hMod,MAKEINTRESOURCE(IDR_DCL_PRIMITIVES),"DCL");   // Find the resource for primitives.dcl
			}
		else if (nDCLToLoad == IBASE_DCL)
			{
				hsrc = FindResource(hMod,MAKEINTRESOURCE(IDR_DCL_BASE),"DCL");         // Find the resource for base.dcl.
			}


		ASSERT( hsrc );
		DWORD nCount = SizeofResource(hMod,hsrc);                                     // Get the size of resource.

		hGlobal = LoadResource(hMod,hsrc);                                            // Load the resource, and get the handle to the resource.
		ASSERT( hGlobal );

		if ( hGlobal )
			{
			pGlobalMem = LockResource( hGlobal);                                 // By locking the resource, you can get a void handle.
			ASSERT( pGlobalMem );
			try
				{
				// Get the path of the temporary directory.

				DWORD nBufferLength = 255;
				DWORD nTemp;
				lpTempPathName = new char[nBufferLength];
				nTemp = GetTempPath(nBufferLength,lpTempPathName);

				if ( nTemp > nBufferLength )
					{
					delete lpTempPathName;
					lpTempPathName = new char[nBufferLength];
					nTemp = GetTempPath(nTemp,lpTempPathName);
					}

				// Create temporary file in the temporary directory path returned.
				if ( nTemp != 0	)
					{
					lpTempFileName = new char[_MAX_PATH];
					GetTempFileName(lpTempPathName,_T("ZXC"),0,lpTempFileName);
					}

					CFile baseFile(lpTempFileName,CFile::modeCreate|CFile::modeReadWrite);
					TRACE("Name of temporary file:%s\n",lpTempFileName);
					baseFile.Write(pGlobalMem,nCount);
					baseFile.Close();
				}
			catch(CFileException fexception)
				{
				TRACE("Error loading resource\n");
				delete lpTempPathName;
				delete lpTempFileName;
				return SDS_RTERROR;
				}

			// Call the parser entry function, with the filename
			nReturn = LoadDCLFile(lpTempFileName,&nDlgFileId);
			remove(lpTempFileName);

			delete lpTempPathName;
			delete lpTempFileName;

			}
		}
	catch(CException *e)
		{

		if ( e->IsKindOf(RUNTIME_CLASS(CExceptionManager)) )
			{
			throw e;
			}
		else
			{
			ThrowCExceptionManager(DCL_TILES_INIT_FAILED);
			}

        return SDS_RTERROR;
		}


	return nReturn;


	}




/***************************************************************************************************
/* + int Parser::LoadDCLFile(LPTSTR wszFileToLoad) -
*
*   This method is called from dlg_load_dialog of Main class when a DCL files has to be loaded.
*  This method calls the DCLParserStartUp where the parsing operation starts.The method also
*  does the clean up operation if error occur during parsing.
*
*/
int Parser::LoadDCLFile(                               // Method calls the DCLParserStaruUp and does the clean up operation if error occurs
						LPTSTR wszFileToLoad,          // i: File to load.
						int *nDlgFileId                // o: Unique File ID
						)
	{


	try
		{
		POSITION pos;                               // To store the position of loaded  DCL file in the file list.
		int nReturn = SDS_RTERROR;

		m_nAuditLevel = 1;
		m_nHintCount = 0;

		// Append the filename included to the m_ErrorList, so that this can  be displayed if errors occur
		// during parsing.
		this->m_ErrorList.RemoveAll();
        this->m_HintList.RemoveAll();
		this->AddErrorString(_T("*** DCL semantic audit of ") + CString((LPCTSTR)wszFileToLoad) + _T("***\n\n"),0);


		//deleting nodes pointed by m_ListofNonDefinedNodes
		POSITION position;
		position =  m_ListOfNonDefinedNodes.GetHeadPosition();

		for ( int iLoop = 0; iLoop <  m_ListOfNonDefinedNodes.GetCount(); iLoop++)
			{
			NonDefinedNode* pNonDefinedNode = (NonDefinedNode*)m_ListOfNonDefinedNodes.GetNext(position);
			delete pNonDefinedNode;
			}
		m_ListOfNonDefinedNodes.RemoveAll();


		// Tail position got here , because if a error occurs , then the files following this posiotion value.
		// can be removed from the list.
		pos =  m_ListOfDCLFileData.GetTailPosition();

		// Call the parser entry function, with the filename.
		nReturn = DCLParserStartUp(wszFileToLoad,nDlgFileId,FALSE);

		TRACE("AUDITLEVEL == %d\n",m_nAuditLevel);
		// Call a function to set the Audit Level for the currently parsed file.
		if ( HasErrorOccured() == FALSE )
			{
			SetAuditLevel();
			}
		else
			{
			m_nAuditLevel = 1;
			}

		// If no errors occured during parsing, then find the non-defined nodes.
		if ( nReturn == SDS_RTNORM && HasErrorOccured()  == FALSE )
			{
				nReturn = ReInitNonDefinedNode();
				this->DoAuditLevelProcessing();

			}

		// This value is set here, because while parsing when a non-container tile is found containing
		// child tiles, the flag is not set.This is not set then, so that parsing continues and a total
		// list of such semantic errors in DCL file is displayed in the end.
		if ( m_bNonContianerContains == TRUE )
			{
			this->m_bErrorOccured  = TRUE;
			// Errors can be logged at this place.
			}

		if ( m_nAuditLevel < 3 )
			{
			m_bHintPresent = FALSE;
			}


		if ( m_bWarningOccurred == TRUE || m_bNonContianerContains == TRUE || m_bHintPresent == TRUE )
			{
			LogErrors();
			}

		// If error had occured or tiles used were not defined , then delete the file and its respective
		// includes from the list.
		if	(nReturn == SDS_RTERROR || HasErrorOccured() == TRUE || m_bWarningOccurred == TRUE || m_bHintPresent == TRUE)
			{
			POSITION posPrev;

			if (pos)
				{
				m_ListOfDCLFileData.GetNext(pos);
				}
			else
				{
				pos = m_ListOfDCLFileData.GetHeadPosition();
				}

			do
				{
				posPrev = pos;
				if (pos)
					{
					delete ((FileInfo*)m_ListOfDCLFileData.GetNext(pos));
					m_ListOfDCLFileData.RemoveAt(posPrev);
					}
				} while(pos) ;
			m_CurrentFilePosition = NULL;

			nReturn = SDS_RTERROR;
			}
		else
			{
			nReturn = SDS_RTNORM;
			}


		// Reset the error flag, for next parsing operation
		set_ErrorOccured(FALSE);
		this->m_bNonContianerContains = FALSE;
		this->m_bHintPresent = FALSE;
		this->m_bWarningOccurred = FALSE;

		return nReturn;
		}
	catch(CException *e)
		{
		// Reset the error flag, for next parsing operation
		set_ErrorOccured(FALSE);
		this->m_bNonContianerContains = FALSE;
		this->m_bHintPresent = FALSE;
		this->m_bWarningOccurred = FALSE;
		if ( e->IsKindOf(RUNTIME_CLASS(CExceptionManager)) )
			{
			throw e;
			}
		else
			{
			ThrowCExceptionManager(PARSER_LOAD_DCL_FAILED);
			}

		return SDS_RTERROR;
		}

}


/************************************************************************************
/* + int Parser::Init(CString wszPathOfBaseDCL,
*					  BOOL bGenerateErrorLog = TRUE
*                     CString wszPathForErrorLog = "."
*                     )
*
*          To Initialize the parser.
*
*/
int Parser::Init(								// To Initialize the parser.
	   CString wszPathOfBaseDCL ,	    // i:Full path of IBase.dcl or similar file , Here the user is expected to give the DCL filename also
	   BOOL bGenerateErrorLog  ,			// i:Flag set for generation of DCL log.
	   CString wszPathForErrorLog    // i:Path for Error.log to be generated.
	   )

	{
    int nReturn = SDS_RTNORM;
	int nDlgFileId ;
	CString wszDefaultPathForErrorLog;
	CFileFind  FileFinder;

	// Initialization for Error.log
	// Initialize flag to generate or not to generate errors.

	try
		{
		m_bGenerateErrorLog = bGenerateErrorLog;
		int count = wszPathForErrorLog.Replace(_T('\\'),_T('/'));

		m_wszPathOfErrorLog   = wszPathForErrorLog;

		// Check whether flag has been set to generate errors.
		if ( m_bGenerateErrorLog == TRUE && count != 0 )
			{
			// Check whether the path given for Erro.log exists.
			CString wszValidatePath ;

            wszValidatePath = m_wszPathOfErrorLog.Left(m_wszPathOfErrorLog.ReverseFind(_T('/')));


			char strCurrentWorkDir[_MAX_PATH];
			_getcwd(strCurrentWorkDir,_MAX_PATH);
			if ( _tchdir((LPCTSTR)wszValidatePath)  != 0  )
				{
                // Display an error stating that the given directory is not present.
				ErrorMessage errMessage;
				errMessage.Message(IDS_ERROR_LOG_PATH_NOT_FOUND);

				// Get the application directory
				char strPath[_MAX_PATH];
				ZeroMemory(strPath,_MAX_PATH);
				::GetModuleFileName(::GetModuleHandle(NULL),strPath,_MAX_PATH);
				m_wszPathOfErrorLog = strPath;

				m_wszPathOfErrorLog = m_wszPathOfErrorLog.Left(m_wszPathOfErrorLog.ReverseFind(_T('\\')));
				m_wszPathOfErrorLog += _T('\\' ) ;
				m_wszPathOfErrorLog += ERROR_LOG ;
				remove(m_wszPathOfErrorLog);
				}
			else
				{
				_tchdir(strCurrentWorkDir);
				}
			}

		// Set the value to the path and error.log name.
		// This variable is used in the function LogErrors to write to the respective file.
		if (count == 0)
			{
			// Get the application directory
			char strPath[_MAX_PATH];
			CString wszPath;
			ZeroMemory(strPath,_MAX_PATH);
			::GetModuleFileName(::GetModuleHandle(NULL),strPath,_MAX_PATH);
            wszPath  =   CString(strPath);
			wszPath  = wszPath.Left(wszPath.ReverseFind(_T('\\')));
			m_wszPathOfErrorLog.TrimLeft();
			if  ( m_wszPathOfErrorLog.IsEmpty())
				{
				m_wszPathOfErrorLog += ERROR_LOG ;
				}
			m_wszPathOfErrorLog = wszPath  + _T('\\') + m_wszPathOfErrorLog;
/*			m_wszPathOfErrorLog = m_wszPathOfErrorLog.Left(m_wszPathOfErrorLog.ReverseFind(_T('\\')));
			m_wszPathOfErrorLog += _T('\\' ) ; */

            remove(m_wszPathOfErrorLog);
			TRACE("Name of Error Log : %s",m_wszPathOfErrorLog);
			}


		// Load Primitives.dcl and generate tree.
		nReturn = LoadPrimitivesOrBaseDCL(PRIMITIVES_DCL);
		if ( nReturn == SDS_RTERROR)
			{
			return nReturn;
			}

		// Load IBase.dcl from resource or from the path specified.
		if (wszPathOfBaseDCL.IsEmpty())
			{
			// Load the base definitions from the resource.
			nReturn = LoadPrimitivesOrBaseDCL(IBASE_DCL);
			}
		else
			{
			// Load the base definitions from the given file.
			nReturn = LoadDCLFile(wszPathOfBaseDCL.GetBuffer(wszPathOfBaseDCL.GetLength()),&nDlgFileId);
			}
		}
	catch(CException *e)
		{
		if ( e->IsKindOf(RUNTIME_CLASS(CExceptionManager)) )
			{
			throw e;
			}
		else
			{
			ThrowCExceptionManager(PARSER_INIT_FAILED);
			}
		}

	return nReturn;

	}



/************************************************************************************
/* + int Parser::UnLoadDCLFile(int nDlgFileId) -
*
*        This method is called to unload the DCL file and the files included within
*    it.The nDlgFileId is the unique id represents the DCL file which has to be unloaded.
*    This unique id is given to every loaded DCL file during loading.
*
*/
int Parser::UnLoadDCLFile(						// Unloads the DCL file.
						  int nDlgFileId        // Unique id of DCL file.
						  )
	{
    int nReturn = SDS_RTNORM;
	POSITION pos;
	POSITION prevpos;
	int iCount;
	FileInfo *pFileInfo;

    iCount =  m_ListOfDCLFileData.GetCount();
    pos =  m_ListOfDCLFileData.GetHeadPosition();

	try
		{
		// Search for the required unique file id.
		for(int iLoop = 0 ; iLoop < iCount && pos ; iLoop++)
			{
			prevpos = pos;
			pFileInfo = (FileInfo*)m_ListOfDCLFileData.GetNext(pos);
			// Check if FileID = nDlgFileId.
			if ( pFileInfo->get_DCLFileID() == nDlgFileId)
				{
				// Delete the file object found for the Unique ID
				if (prevpos)
					{
					delete pFileInfo;
					m_ListOfDCLFileData.RemoveAt(prevpos);
					// Once the file is deleted, the files which were included with it are also deleted.
					// For all the included files the IsIncluded() returns TRUE.
					int iIterationCount = 0;
					while(pos)
						{
						prevpos = pos;
						pFileInfo = (FileInfo*)m_ListOfDCLFileData.GetNext(pos);

						if ( pFileInfo->IsIncluded() == FALSE && iIterationCount != 0 )
							{
							return SDS_RTNORM;
							}
						else
							{
							delete pFileInfo;
							m_ListOfDCLFileData.RemoveAt(prevpos);
							iIterationCount = 1;
							}
						} // End of while for recursive unload of DCL files.
					}  // End of if(prevpos).
				}	// End of if condition for checking DlgFileID.
			}  // End of for loop
			return SDS_RTNORM;
		}
	catch(...)
		{
		ThrowCExceptionManager(DCL_UNLOAD_FAIL);
		}

	return SDS_RTNORM;
	}


/************************************************************************************
/* + int Parser::LogErrors(	CString &wszErrorMessage )
*
*   Method called for writing errors to Error.log.
*
*
*
*/
int Parser::LogErrors(                                             // Method called for writing errors to Error.log.
					  CString &wszErrorMessage					   // i: Error message to be wriiten to the Error.log.
					  )
	{

	CStdioFile ErrorOutput;
	CFileException fileexception;
	BOOL bSuccess = TRUE;
    static BOOL bPrevSuccessFlag =TRUE ;


	// Write to error.log only if the falg has been set to generate errors.
	if ( m_bGenerateErrorLog == TRUE )
		{
		// OPen the error.log file
		bSuccess = ErrorOutput.Open((LPCTSTR)m_wszPathOfErrorLog ,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite,&fileexception);

		// If open fails , then generate an error message and call the Message method of the ErrorMessage class.
		if (!bSuccess)
			{
			if ( bPrevSuccessFlag )
				{
				TCHAR errMessage[255];
				ErrorMessage errormessage;
				// Get the error from the framework.
				fileexception.GetErrorMessage(errMessage,255);
				// Call the Message method.
				errormessage.Message(CString(errMessage));
				bPrevSuccessFlag = FALSE;
				}
            return SDS_RTERROR;
		    }

		bPrevSuccessFlag = TRUE;
		// Go toend of file before writing.
		ErrorOutput.SeekToEnd();

		// Write to Error.log .
		ErrorOutput.WriteString((LPCTSTR)wszErrorMessage);

		}

		return SDS_RTNORM;

	}



#ifdef _TESTEXE
/***************************************************************************************************
/* + int sds_findfiles(char *filetofind,char *pathfound) -
*
*
*   This function will be removed after integration.It just simulates the sds_findfiles function
*  which ships with Intellicad.
*
*/
int sds_findfiles(
				  char *filetofind,              // i: File to find.
				  char *pathfound                // o: Path found for the file.
				  )
	{
#pragma message("This function has to be removed during integration with Intellicad")
	strcpy(pathfound,".");

    return SDS_RTNORM;

	}
#endif

/************************************************************************************
/* + int Parser::DisplayError(CString wszErrorMessage) -
*
*   Displays all error and redefinition messages for all level of semantic auditing
*
*
*
*/
int Parser::DisplayError(                                        // Displays a error message.
						 CString wszErrorMessage                // i: String to be displayed.
						 )
{
  CString wszResourceString;
  wszResourceString.LoadString(ID_DCL_MSGBOX_TITLE);

  ::MessageBox(NULL,wszErrorMessage,wszResourceString,MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_ICONSTOP);

  return SDS_RTNORM;

}


/************************************************************************************
/* + int Parser::FindNodeExists(CString &wszFunctionName) -
*
*   This function finds for multiple definitions of nodes and returns an error value
*   of node is present otherwise returns SDS_RTNORM.
*
*
*/
int Parser::FindNodeExists(                                           // CHecks for existence of a node.
						   CString &wszFunctionName                   // i: NOde name to search.
						   )
	{
	int nRet = SDS_RTERROR;
	Tile tile;
	// Get the count of files already loaded
	int nCount = m_ListOfDCLFileData.GetCount();

	// This condition takes care of parsing the initial two file entries in the file list
	// They are the ibase.dcl and primitives.dcl
	if ( nCount > 2 )
		{
		nCount = 2;
		}

	if ( nCount )
		{
		POSITION position = m_ListOfDCLFileData.GetHeadPosition();
		// Go in a loop and call GetReferencedNode for each of them.
		for ( int i = 0; i < nCount; i++ )
			{
			FileInfo * pFileInfo = (FileInfo *)m_ListOfDCLFileData.GetNext(position);
			nRet = pFileInfo->FindNodeExists(wszFunctionName);
			if ( !position || (nRet == SDS_RTNORM) )
				{
				break;   // The node has been found (nRet = SDS_RTNORM) or no more trees left to search(nRet = SDS_RTERROR).
				}

			}

		// After searching in the primitives and base dcl resources
		// search for definitions in the loaded files, including the files included in the
		// loaded file.
        position = m_CurrentFilePosition;
        if ( nRet == SDS_RTERROR )
			{
			while (position)
				{
				FileInfo * pFileInfo = (FileInfo *)m_ListOfDCLFileData.GetNext(position);
				// The file ID is the same for all the included files and the loaded file.
				// Therefore, searching is done only in those files.
                if ( pFileInfo->get_DCLFileID() ==  m_nCurrentFileID )
					{
					nRet = pFileInfo->FindNodeExists(wszFunctionName);
					if ( !position || (nRet == SDS_RTNORM) )
						{
						break;   // The node has been found (nRet = SDS_RTNORM) or no more trees left to search(nRet = SDS_RTERROR).
						}
					}
				}
			 }
		}

	return nRet;

	}





/************************************************************************************
/* + int Parser::get_AuditLevel() -
*
*    Returns the value of m_nAuditLevel, which is the audit level set.
*
*
*
*/
int Parser::get_AuditLevel()    // Returns the value of audit level.
	{
      return m_nAuditLevel;
	}

/************************************************************************************
/* + int Parser::SetAuditLevel() -
*
*   Sets the value of the audit level , corresponding to the values set in the
*   DCL file currently being passed.
*
*
*/
int Parser::SetAuditLevel()
	{


	try
		{
		int nReturn;
		TreeNode *pTreeNode;


		if ( FindReferencedNode(CString("dcl_settings"),1,&pTreeNode) == SDS_RTNORM)
			{
			// Get the value of audit_level.
			nReturn =  ((*pTreeNode).m_Tile.get_ListOfAttributes())->get_AuditLevel();

			if ( nReturn != SDS_RTERROR )
				{
				m_nAuditLevel = nReturn;
				return SDS_RTNORM;
				}

			}

		if ( FindReferencedNode(CString("default_dcl_settings"),1,&pTreeNode) == SDS_RTNORM)
			{
			// Get the value of audit_level.
			nReturn =  ((*pTreeNode).m_Tile.get_ListOfAttributes())->get_AuditLevel();

			if ( nReturn <= 3 )
				{
				m_nAuditLevel = nReturn;
				return SDS_RTNORM;
				}
			}

	    m_nAuditLevel = AUDIT_LEVEL_1 ;
		return SDS_RTNORM;
		}
	catch(...)
		{
		ThrowCExceptionManager(SET_AUDIT_LEVEL_FAIL);
		}
	return SDS_RTNORM;
	}




/************************************************************************************
/* + int Parser::AddErrorString(CString &wszErrorString) -
*
*    Add a entry to the m_ErrorList array.
*
*
*
*/
int Parser::AddErrorString(CString &wszErrorString,int nErrorLevel)
	{
	if ( wszErrorString.GetLength() != 0 )
		{

		switch(nErrorLevel)
			{
			case 0:
				wszErrorString = _T("<0>") + wszErrorString;
				break;
            case 1:
				wszErrorString = _T("<1>") + wszErrorString;
				g_Main.m_DCLParser.set_NonContianerContains();
				break;
			case 2:
				wszErrorString = _T("<2>") + wszErrorString;
				g_Main.m_DCLParser.set_WarningOccured();
				break;
           /*case 3:
				wszErrorString = _T("<3>") + wszErrorString;
				g_Main.m_DCLParser.set_HintPresent();
				break;*/
            case 3:
				wszErrorString = _T("<3>") + wszErrorString;
				g_Main.m_DCLParser.set_HintPresent();
				return ( m_HintList.Add(wszErrorString) + 1 );

			default:
				ASSERT(FALSE);
			}


		m_ErrorList.Add(wszErrorString);
	    return SDS_RTNORM;
		}
	else
		{
		return SDS_RTERROR;
		}
	}



/************************************************************************************
/* + int Parser::LogErrors() -
*
*     Writes all the errors strings present in the m_ErrorList to the Error.log
*
*
*
*/
int Parser::LogErrors()
	{

	CStdioFile ErrorOutput;
	CFileException fileexception;
	BOOL bSuccess = TRUE;
    static BOOL bPrevSuccessFlag =TRUE ;


	try
		{
		// Write to error.log only if the falg has been set to generate errors.
		if ( m_bGenerateErrorLog == TRUE && m_nAuditLevel !=  AUDIT_LEVEL_0  )
			{
			// OPen the error.log file
			bSuccess = ErrorOutput.Open((LPCTSTR)m_wszPathOfErrorLog ,CFile::modeCreate|CFile::modeWrite,&fileexception);

			// If open fails , then generate an error message and call the Message method of the ErrorMessage class.
			if (!bSuccess)
				{
				if ( bPrevSuccessFlag )
					{
					TCHAR errMessage[255];
					ErrorMessage errormessage;
					// Get the error from the framework.
					fileexception.GetErrorMessage(errMessage,255);
					// Call the Message method.
					errormessage.Message(CString(errMessage));
					bPrevSuccessFlag = FALSE;
					}
				return SDS_RTERROR;
				}

			bPrevSuccessFlag = TRUE;
			// Go to end of file before writing.
	//		ErrorOutput.SeekToEnd();

			// Write to Error.log .
			CString wszErrorMessage;
			CString wszAuditLevel;
			wszAuditLevel.Format("%d",m_nAuditLevel);

			ErrorMessage errMessageBox;
            CString wszErrorString,wszResourceString;
			wszResourceString.LoadString(ID_DCL_CHECK_ERROR_FILE);

			wszErrorString.Format(wszResourceString,this->m_wszPathOfErrorLog );
			errMessageBox.Message(wszErrorString);

			for ( int iIndex = 0 ; iIndex < m_ErrorList.GetSize() ; iIndex++ )
				{
				// Get the error message.
				wszErrorMessage = m_ErrorList.GetAt(iIndex) ;

				// Get the flag in error message that specifies the error level.

				if ( wszErrorMessage.Mid(1,1) <=  wszAuditLevel /* && atoi((LPCTSTR)wszErrorMessage.Mid(1,1)) !=  AUDIT_LEVEL_3 */ )
					{
					 wszErrorMessage.Delete(0,3);
					 ErrorOutput.WriteString((LPCTSTR)wszErrorMessage);
					}
				}


/*			// For displaying hints.
			for (  iIndex = 0 ; iIndex < m_ErrorList.GetSize() ; iIndex++ )
				{
				// Get the error message.
				wszErrorMessage = m_ErrorList.GetAt(iIndex) ;


				// Get the flag in error message that specifies the error level.
				if ( wszErrorMessage.Mid(1,1) <=  wszAuditLevel && atoi((LPCTSTR)wszErrorMessage.Mid(1,1)) ==  AUDIT_LEVEL_3 )
					{
					 wszErrorMessage.Delete(0,3);
					 ErrorOutput.WriteString((LPCTSTR)wszErrorMessage);
					}
				} */

			}
			return SDS_RTNORM;
		}
	catch(...)
		{
		ThrowCExceptionManager(LOG_ERROR_FAILED);
		}

	return SDS_RTNORM;
	}


/*************************************************************************************************
/* + BOOL Parser::ReportCircularReference() -
*
*   This function searches for any circular references, and returns false if they are present.
*
*
*
*/
BOOL Parser::ReportCircularReference()
	{


	int nCount = 0;
	NonDefinedNode *pNonDefNodes_1,*pNonDefNodes_2;
	BOOL bReturn = FALSE;


	// The posPrevOuter and posPrevInner variables used below are used for deletion of nodes
	// once the nodes are found to have circular reference.


	try
		{

		POSITION posOuter = NULL , posPrevOuter = NULL;

		// Get the head position  to start with traversal.
		posOuter = this->m_ListOfNonDefinedNodes.GetHeadPosition();

		for ( int iLoop = 0 ; iLoop < this->m_ListOfNonDefinedNodes.GetCount() ; iLoop++ )
			{
			// Get the next node to compare.
			posPrevOuter = posOuter;
			ASSERT(posOuter);
			pNonDefNodes_1 = (NonDefinedNode*) this->m_ListOfNonDefinedNodes.GetNext(posOuter);

			// Initialize posInner to next position from where traversal should start.
			POSITION posInner = posOuter;
			POSITION posPrevInner;
			while (posInner)
				{
				 // Get the node to compare .
				 posPrevInner = posInner ;
				 ASSERT(posInner);
				 pNonDefNodes_2 = (NonDefinedNode*) this->m_ListOfNonDefinedNodes.GetNext(posInner);

				 if ( pNonDefNodes_1->get_TileNameReferenced() == (pNonDefNodes_2->get_NodePlaceHolder())->m_Tile.get_FunctionName()
					  &&
					  pNonDefNodes_2->get_TileNameReferenced() == (pNonDefNodes_1->get_NodePlaceHolder())->m_Tile.get_FunctionName()  )
					 {
					 // This value is returned.
					 bReturn =  TRUE;           // TRUE : Since, there is a circular reference.

					 // Report an error by adding an error to the error string.
					 CString wszResourceString,wszErrorString;
					 wszResourceString.LoadString(IDS_CIRCULAR_REFERENCE);
					 wszErrorString.Format(wszResourceString,
											(pNonDefNodes_1->get_NodePlaceHolder())->m_Tile.get_FunctionName());

					 this->AddErrorString(wszErrorString);

					 wszErrorString.Format(wszResourceString,
													(pNonDefNodes_2->get_NodePlaceHolder())->m_Tile.get_FunctionName());
					 this->AddErrorString(wszErrorString);


					 // Now, delete the non-defined nodes having a circular reference.
					 this->m_ListOfNonDefinedNodes.RemoveAt(posPrevOuter);
					 this->m_ListOfNonDefinedNodes.RemoveAt(posPrevInner);

					 delete pNonDefNodes_1;
					 delete pNonDefNodes_2;

					 // If nodes deleted where present next to each other in the list than
					 // then the iteration should happen from the next level.

					 if (posOuter == posPrevInner )
						 {
						 posOuter = posInner ;
						 }


					 break;
					 }
				}
			}
		}
	catch(...)
		{
		ThrowCExceptionManager(CIRCULAR_REF_FIND_FAIL);
		}


	return bReturn;
	}



/********************************************************************************************
/* + int Parser::DoAuditLevelProcessing() -
*
*   This function takes care of all kinds of audits to be done when semantic auditing level
*   is done.
*
*
*/
int Parser::DoAuditLevelProcessing()    // Processing done for semantic auditing levels.
	{

	AuditLevelProcessor auditlevelproc(this->m_nCurrentFileID,m_nAuditLevel);

	try
		{
		switch(m_nAuditLevel)
			{
			case AUDIT_LEVEL_0:
				break;
			case AUDIT_LEVEL_1:
				break;
			case AUDIT_LEVEL_2:
				auditlevelproc.StartProcessing();
				break;
			case AUDIT_LEVEL_3:
				auditlevelproc.StartProcessing();
				break;
			default:
				ASSERT(FALSE);
			}
		}
	catch(...)
		{
		ThrowCExceptionManager(AUDIT_LEVEL_CHECK_FAILED);
		}

	return SDS_RTNORM;



	}
/************************************************************************************
/* + int Parser::set_NonContianerContains(BOOL bValue) -
*
*    Sets the value of m_bNonContainerContains to the value of bValue.
*   The attribute is used to keep track of any non-container tiles found to contain
*   tiles.
*
*/
int Parser::set_NonContianerContains()
	{
	this->m_bNonContianerContains = TRUE;
    return SDS_RTNORM;
	}




/************************************************************************************
/* + int Parser::PopulateAttributeInfo() -
*
*    This completes the m_mapAttributeInfo variable, and maps all the varaibles
*   required.
*
*
*/
int Parser::PopulateAttributeInfo()
	{

	AttributeInfo *pAttrInfo;
	CStringArray arrayAttrList1,arrayAttrList2,arrayAttrList3,arrayAttrList4,arrayAttrList5;
	CStringArray arrayAttrList6;

    try
		{
		// Add one set of attributes (left, right,centered)
		arrayAttrList1.Add(_T(LEFT_ATTRIB_VALUE));
		arrayAttrList1.Add(_T(CENTERED_ATTRIB_VALUE));
		arrayAttrList1.Add(_T(RIGHT_ATTRIB_VALUE));


		// Add one set of attributes (top,bottom,centered)
		arrayAttrList2.Add(_T(TOP_ATTRIB_VALUE));
		arrayAttrList2.Add(_T(CENTERED_ATTRIB_VALUE));
		arrayAttrList2.Add(_T(BOTTOM_ATTRIB_VALUE));


		// Add one set of attributes (TRUE,FALSE)
		arrayAttrList3.Add(_T(TRUE_ATTRIB_VALUE));
		arrayAttrList3.Add(_T(FALSE_ATTRIB_VALUE));

		// Add one set of attributes (left, right,centered)
		arrayAttrList4.Add(_T(LEFT_ATTRIB_VALUE));
		arrayAttrList4.Add(_T(CENTERED_ATTRIB_VALUE));
		arrayAttrList4.Add(_T(RIGHT_ATTRIB_VALUE));

		arrayAttrList4.Add(_T(TOP_ATTRIB_VALUE));
		arrayAttrList4.Add(_T(CENTERED_ATTRIB_VALUE));
		arrayAttrList4.Add(_T(BOTTOM_ATTRIB_VALUE));

		// Add one set of attributes (left, right,centered)
		arrayAttrList5.Add(_T(VERTICAL_ATTRIB_VALUE));
		arrayAttrList5.Add(_T(HORIZONTAL_ATTRIB_VALUE));

        // Add the set of color attributes, used by the color property.

		// attribute value : dialog_line
		arrayAttrList6.Add(_T(DIALOG_LINE_STR));

		// attribute value : dialog_foreground
		arrayAttrList6.Add(_T(DIALOG_FOREGROUND_STR));

        // attribute value : dialog_background
		arrayAttrList6.Add(_T(DIALOG_BACKGROUND_STR));

        // attribute value : graphics_background
		arrayAttrList6.Add(_T(GRAPHICS_BACKGROUND_STR));

        // attribute value : black
		arrayAttrList6.Add(_T(BLACK_STR));

		// attribute value : red
		arrayAttrList6.Add(_T(RED_STR));

        // attribute value : yellow
		arrayAttrList6.Add(_T(YELLOW_STR));

        // attribute value : green
		arrayAttrList6.Add(_T(GREEN_STR));

        // attribute value : cyan
		arrayAttrList6.Add(_T(CYAN_STR));

		// attribute value : blue
		arrayAttrList6.Add(_T(BLUE_STR));

		// attribute value : magenta
		arrayAttrList6.Add(_T(MAGENTA_STR));

		// attribute value : white
		arrayAttrList6.Add(_T(WHITE_STR));

		// attribute value : graphics_foreground
		arrayAttrList6.Add(_T(GRAPHICS_FOREGROUND_STR));


		// Add the attribute mappings.

		// mapping for "action" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("action"),pAttrInfo);

		// mapping for "alignment" attribute
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("left")),arrayAttrList4);
		this->m_mapAttributeInfo.SetAt(_T("alignment"),pAttrInfo);


		// mapping for "allow_accept"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("allow_accept"),pAttrInfo);

		// mapping for "aspect_ratio"
		pAttrInfo = new  AttributeInfo(REAL_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("aspect_ratio"),pAttrInfo);

		// mapping for "big_increment "
		pAttrInfo = new  AttributeInfo(INT_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("audit_level"),pAttrInfo);


		// mapping for "big_increment "
		pAttrInfo = new  AttributeInfo(INT_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("big_increment"),pAttrInfo);

		// mapping for "children_alignment"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("left")),arrayAttrList4);
		this->m_mapAttributeInfo.SetAt(_T("children_alignment"),pAttrInfo);

		// mapping for "children_fixed_width"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("children_fixed_width"),pAttrInfo);


		// mapping for "children_fixed_height"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("children_fixed_height"),pAttrInfo);


		// mapping for "color"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("white")),arrayAttrList6);
		this->m_mapAttributeInfo.SetAt(_T("color"),pAttrInfo);


		// mapping for "edit_limit"
		pAttrInfo = new  AttributeInfo(INT_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("edit_limit"),pAttrInfo);

		// mapping for "edit_width"
		pAttrInfo = new  AttributeInfo(REAL_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("edit_width"),pAttrInfo);

		// mapping for "fixed_height"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("fixed_height"),pAttrInfo);


		// mapping for "fixed_width"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("fixed_width"),pAttrInfo);


		// mapping for "fixed_width_font"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("fixed_width_font"),pAttrInfo);


		// mapping for "height"
		pAttrInfo = new  AttributeInfo(REAL_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("height"),pAttrInfo);

		// mapping for "initial_focus" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("initial_focus"),pAttrInfo);


		// mapping for "is_bold"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("is_bold"),pAttrInfo);

		// mapping for "is_cancel 	"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("is_cancel"),pAttrInfo);

		// mapping for "is_default"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("is_default"),pAttrInfo);

		// mapping for "is_enabled"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("true")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("is_enabled"),pAttrInfo);

		// mapping for "is_tab_stop"
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("true")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("is_tab_stop"),pAttrInfo);


		// mapping for "key" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("key"),pAttrInfo);


		// mapping for "label" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("label"),pAttrInfo);

		// mapping for "layout" attribute
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("horizontal")),arrayAttrList5);
		this->m_mapAttributeInfo.SetAt(_T("layout"),pAttrInfo);

		// mapping for "list" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("list"),pAttrInfo);

		// mapping for "max_value" attribute
		pAttrInfo = new  AttributeInfo(INT_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("max_value"),pAttrInfo);

		// mapping for "min_value" attribute
		pAttrInfo = new  AttributeInfo(INT_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("min_value"),pAttrInfo);

		// mapping for "mnemonic" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("mnemonic"),pAttrInfo);

		// mapping for "multiple_select" attribute
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("multiple_select"),pAttrInfo);


		// mapping for "password_char" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("password_char"),pAttrInfo);

		// mapping for "small_increment" attribute
		pAttrInfo = new  AttributeInfo(INT_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("small_increment"),pAttrInfo);


		// mapping for "tab_truncate" attribute
		pAttrInfo = new  AttributeInfo(KEYWORD_DATA_TYPE,CString(_T("false")),arrayAttrList3);
		this->m_mapAttributeInfo.SetAt(_T("tab_truncate"),pAttrInfo);

		// mapping for "value" attribute
		pAttrInfo = new  AttributeInfo(STRING_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("value"),pAttrInfo);

		// mapping for "width" attribute
		pAttrInfo = new  AttributeInfo(REAL_DATA_TYPE);
		this->m_mapAttributeInfo.SetAt(_T("width"),pAttrInfo);
		}
	catch(...)
		{
		ThrowCExceptionManager(MEM_ALLOC_FAILED);
		return SDS_RTERROR;
		}


	return SDS_RTNORM;

	}


/************************************************************************************
/* + int Parser::ValidateAttribute(TileAttribute *pTileAttribute) -
*
*   This function validates the tile attribute and adds a error string to the errorlist
*   if validation fails.
*
*
*/
int Parser::ValidateAttribute(                                    // Validates the attribute.
							  TileAttributes *pTileAttribute       // i: The tile attribute to be validated.
							  )
	{

	BOOL nReturn;
	AttributeInfo *pAttributeInfo;


	try
		{
		TRACE("AttributeName : %s\n ", pTileAttribute->get_AttributeName());

		nReturn = this->m_mapAttributeInfo.Lookup(
										((LPCTSTR)pTileAttribute->get_AttributeName()),
										( CObject*& ) pAttributeInfo);

		int nDataType	=  atoi((LPCTSTR)pTileAttribute->get_AttributeType());

		if ( nReturn == TRUE )
			{
			if ( nDataType != pAttributeInfo->m_nDataType )
				{
				 // This condition is implemented because, a attribute which can hold real values
				 // can also hold integer values.
				 if ( pAttributeInfo->m_nDataType == REAL_DATA_TYPE && nDataType == INT_DATA_TYPE )
					 {
					 return SDS_RTNORM;
					 }
				return SDS_RTERROR;
				}
			}

		return SDS_RTNORM;
		}
	catch(...)
		{
		ThrowCExceptionManager(ATTRIBUTE_VALIDATE_FAIL);
		}

	return SDS_RTNORM;

	}


/*************************************************************************************************
/* + int Parser::GenerateErrorString() -
*
*   This method genearates an approriate error string for any data type mismatches found.
*
*
*
*/
int Parser::GenerateErrorString(                                  // Generates an error string for data type mismatch.
								CString& wszAttributeName,        // i:Attribute name.
								CString& wszFileInfo              // i: Misc details like file name, line number
								)
	{

	BOOL nReturn;
	AttributeInfo *pAttributeInfo;
	int iLoop;
	int nCount;
	CString wszKeywordList;


	try
		{
	    // Search for the respective attribute.
		nReturn = this->m_mapAttributeInfo.Lookup(
										((LPCTSTR)wszAttributeName),
										( CObject*& ) pAttributeInfo);

		// If attribute was found
		if ( nReturn == TRUE )
			{
			CString wszResourceString;

			switch( pAttributeInfo->m_nDataType )
				{
				case INT_DATA_TYPE:
					// for attributes which require integer value
					wszResourceString.LoadString(IDS_INT_DATATYPE_REQUIRED);
					wszFileInfo +=  wszResourceString;
					break;

				case REAL_DATA_TYPE  :
					// for attributes which require integer value
					wszResourceString.LoadString(IDS_REAL_DATATYPE_REQUIRED);
					wszFileInfo +=  wszResourceString;
					break;

				case STRING_DATA_TYPE :
					// for attributes which require integer value
					wszResourceString.LoadString(IDS_STRING_DATATYPE_REQUIRED);
					wszFileInfo +=  wszResourceString;
					break;

				case KEYWORD_DATA_TYPE:
					// for attributes which require keyword like true, false,centered..etc..etc.


					nCount = pAttributeInfo->m_arrayAttributeValues.GetSize();

					// Generate the list of attribute values which can be set.
					for (iLoop = 0;iLoop < nCount ;iLoop++)
						{
						wszKeywordList += _T(" ") +  (pAttributeInfo->m_arrayAttributeValues.GetAt(iLoop));
						if ( iLoop == nCount - 2)
							{
							wszKeywordList += _T("  or");
							}
						else if ( iLoop > nCount -2 )
							{
							wszKeywordList += _T(" .");
							}
						else
							{
							wszKeywordList += _T(" ,");
							}

						}

					// Load a resource string.
					wszResourceString.LoadString(IDS_ATTRIBUTE_SHOULD_BE);
					wszFileInfo += wszResourceString + wszKeywordList;


					break;
				default:
					ASSERT(FALSE);


				}

			// Add the error string.
			AddErrorString(wszFileInfo,2);
			}
		}
	catch(...)
		{
		ThrowCExceptionManager(GEN_ERR_STRING_FAILED);
		}


    return SDS_RTERROR;


	}


/************************************************************************************
/* + int Parser::set_WarningOccured() -
*
*    This value is set whenever a audit level 2 condition is met.
*
*
*
*/
int Parser::set_WarningOccured()                  // Sets the value of m_bWarnings occurred.
	{
     m_bWarningOccurred = TRUE;
	 return SDS_RTNORM;
	}


/************************************************************************************
/* + int Parser::set_HintPresent() -
*
*
*    This value is set whenever a audit level 3 condition is met.
*
*
*/
int Parser::set_HintPresent()                   // Sets that value of m_bHintPresent.
	{
		m_bHintPresent =TRUE;
		return SDS_RTNORM;
	}


/************************************************************************************
/* + int Parser::GetHintIndex() -
*
*    Returns the hint message count
*
*
*
*/
int Parser::GetHintIndex()
	{

	return m_HintList.GetSize() + 1;

	}

int Parser::get_HintCount()
	{
	return m_nHintCount;

	}

int Parser::set_HintCount()
	{

	m_nHintCount++;
	return SDS_RTNORM;

	}


/************************************************************************************
/* + int Parser::AddClonedTree(Tree *pTree, int nDlgFileID) -
*
*   Addes the newly cloned tree object to the list of Tree objects of the FileInfo.
*   object.
*
*
*/
int Parser::AddClonedTree(                           // Adds the cloned tree.
						  Tree *pTree,               // i: Tree Added
						  int nDlgFileID             // i: FileID to whixh the Tree object has to be added to.
						  )
	{

	   int nFileCount = this->m_ListOfDCLFileData.GetCount();

       POSITION position =  this->m_ListOfDCLFileData.GetHeadPosition();

	   while ( position )
   		   {
           FileInfo * pFileInfo = (FileInfo *)m_ListOfDCLFileData.GetNext(position);
		   // The file ID is the same for all the included files and the loaded file.
		   // Therefore, searching is done only in those files.
           if ( pFileInfo->get_DCLFileID() ==  nDlgFileID )
			   {
			   pFileInfo->AddRootNode(pTree);
			   break;
			   }
			}

	   return SDS_RTNORM;

	}
