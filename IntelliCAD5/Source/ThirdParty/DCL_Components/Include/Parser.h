/*
*  FILENAME:     Parser.h- Contains declarations of class Parser.
*  Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*  Abstract  
*       This class holds the the list of all DCL files loaded. This is a data member
*  of the Main class. This class has the entry point which creates the parser class 
*  (Grammer) for parsing the loaded DCL files.
*
*  Audit Log 
*
*
*  Audit Trail
*
*
*/

#ifndef _PARSER_H
#define _PARSER_H

class TreeNode;
class FileInfo;
class NonDefinedNode;
class Tree;
class TileAttributes;


class Parser  
{
public:
	int AddClonedTree(                    // Adds the cloned tree.
			   Tree * pTree,              // i: Tree Added   
			   int nDlgFileID             // i: FileID to whixh the Tree object has to be added to.
			   );
	int set_HintCount();
	int get_HintCount();
	int m_nHintCount;
	int GetHintIndex();
	int set_HintPresent();                // Sets the value of m_bHintPresent varaible. 
	
	int set_WarningOccured();             // Sets the value of m_bWarningOccurred.
	

	int GenerateErrorString(              // Generates an error string for data type mismatch.
		CString& wszAttributeName,        // i:Attribute name.
		CString& wszFileInfo              // i: Misc details like file name, line number 
				);
	int ValidateAttribute(                // Validates the attribute.   
		TileAttributes *pTileAttribute    // i: The tile attribute to be validated.
				);

	int PopulateAttributeInfo();          // Populat3es the Attribute info. 
	
	int set_NonContianerContains();        // Sets the value of m_bNonContianer attribute
	 
	int DoAuditLevelProcessing();          // Does the audit level processing.

	BOOL ReportCircularReference();         // This function searches for any circular references,
	                                        // and returns false if they are present.

	int LogErrors();                        // The errors are read from the m_ErrorList and written to Error.log.

	
	int AddErrorString(                     // Adds a error string to the m_ErrorList data member.
				CString& wszErrorString,    // i: Error string to be added.  
                int nErrorLevel = 1         // The error level i.e athe audit level for which the error hjas to be displayed.  
				);


	int SetAuditLevel();                   // Sets the audit level. 

	int get_AuditLevel();                  // Returns the current audit level
	
	int FindNodeExists(                    // Checks existence of a node.    
				CString& wszFunctionName   // i: Name of node to search.
				);

	int DisplayError(                       // Displays a error message.  
			    CString wszErrorMessage    // i: String to be displayed.
				);
	
	int LogErrors(                          // Method called for writing errors to Error.log.
				CString& wszErrorMessage    // i: Error message to be wriiten to the Error.log.
				);
	
	int UnLoadDCLFile(                      // Method to unload a DCL file and all its included files. 
					  int nDlgFileId		// i: File ID of the file to be unloaded.
					  );      

    int Init(                               // To Initialize the parser.
		CString wszPathOfBaseDCL = _T(""),  // i:Full path of IBase.dcl or similar file , Here the user is expected to give the DCL filename also
	    BOOL bGenerateErrorLog = TRUE  ,	// i:Flag set for generation of DCL log.
	    CString wszPathForErrorLog  = _T("")// i:Path for Error.log to be generated. 
	     );


	int LoadDCLFile(                        // Method calls the DCLParserStaruUp and does the clean up operation if error occurs
		LPTSTR wszFileToLoad,				// i: File to load.  
		int *nDlgFileId						// o: Unique File ID
		);
	
	int LoadPrimitivesOrBaseDCL(            // Loads the Primitives.DCL  or IBase.DCL from Resource
			int nDCLToLoad                  // i: Resource to load 
			);

	int set_ErrorOccured(					// Sets the m_bErrorOccured data member.
		      BOOL bErrorFlag				// i: Value to be set.  
		      );

	BOOL HasErrorOccured();                 // Returns value of m_bErrorOccured.

	Tree* FindReferencedTree(				// Calls the FileInfo class's corresponding function to find referenced Tree.
		CString wszTileName,				// i: The tile name to be searched.
		int iDCLID							// i :DCLFile ID.	
		);

	int FindReferencedNode(					// Calls the FileInfo class's corresponding function to find referenced nodes.
		CString& wszTileName,				// i: The tile name to be searched.
		int nType,							// Type - 0 for dialog,1 for prototype or subassembly.
		TreeNode** ppTreeNode				// o: This member is made to point to the referenced node if exists.
		);

	long GenerateUniqueFileID ();			// A unique ID generated for mappping loaded files.

	int  DCLParserStartUp(					// Entry point function which calls the parser.
		 LPTSTR wszDCLFileName,			    // i: The name of loaded DCL file name.
		 int *nDlgFileId,                   // o: Set to value of unique identifier, if bIsIncluded = FALSE.
		 BOOL bIsIncluded = TRUE            // i: True for included files,FALSE otherwise.
		 );

    int ReInitNonDefinedNode(				// Completes the definition of nodes whose references are not found during parsing.   
	//	FileInfo* pFileInfo					// i: Pointer to FileInfo class holding the list of nondefined nodes.
		);
	
	Parser();								// Constructor of Parser class.
	
	virtual ~Parser();						// Destructor for Parser class.

	POSITION AddFileInfo(                   // Adds a FileInfo pointer to the ListOfDCLFileData data member.
		FileInfo *pFileInfo                 // Pointer to the FileInfo  class to be added.
		);

	int AddNonDefinedNode(					// Adds a NonDefinedNode to the m_ListOfNonDefinedNodes datamember.
		NonDefinedNode *pNonDefinedNode     // Pointer to the NonDefinedNode object to be added. 
		);
	static unsigned int m_iSeed;			// For dates less than 1970 and greater than 2038 this variable is used to generate the Unique ID.
		
private:	
	
	CMapStringToOb m_mapAttributeInfo;
	
	BOOL m_bNonContianerContains;

	CStringArray m_HintList;                // This array holds the list of hints, which are shown if audit level is set to 3. 
	
	CStringArray m_ErrorList;               // This array holds the list of errors.
	
	int m_nAuditLevel;                      // Holds the value of DCL Audit level to be used while parsing.

	POSITION m_CurrentFilePosition;			// Value of the position of currently loaded file is stored here.
											// This value is used while searching for references.	
	int m_nCurrentFileID;					// This variable stores the unique id of file being parsed.
	CString m_wszPathOfErrorLog;            // Has the path where Error.log has to be generated.
	BOOL m_bGenerateErrorLog;               // Flag to check whether Error.log has to be generated.
	BOOL m_bErrorOccured;                   // stores the status of DCL file during parsing,TRUE when error traced
	CObList m_ListOfNonDefinedNodes;		// A list of NonDefinedNode pointers.
	CObList m_ListOfDCLFileData;			// A list of FileInfo pointers.
	CString m_wszCurrentFileName;           // Name of DCL file being parsed.
	                                        // This value is used during Error logging, to display the filename.   

    BOOL m_bWarningOccurred;                 // True , if warnings have occured(for level 2).
	BOOL m_bHintPresent;                     // True , if hints have to be given(for level 3). 
	


	friend class AuditLevelProcessor;
	friend class CDCLDoc;                   // Added for debugging purpose, to be removed in final build   
	friend class CDCLParsrApp;				// Added for debugging purpose, to be removed in final build  
	friend class TreeNode;
	friend class Main;                      
	
	
};

#endif 
