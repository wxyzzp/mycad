/** FILENAME:     AuditLevelProcessor.h- Contains declarations of class AuditLevelProcessor.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*   This class takes care of processing of various rules for various audit levels.
*
*
* Audit Log 
*
*
* Audit Trail
*
*
*/


#if !defined _AUDITLEVELPROCESSOR_H_
#define _AUDITLEVELPROCESSOR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Tile;
class FileInfo;

class AuditLevelProcessor  
{
public:

	int AppendHintString(                                        // Appends hint string to the regular error list.
				int iIndex                                       // i: Index in Hint list to be appended.   
				);

	int CheckForTextAttributes(                                  // Checks for mandatory text properties.
		  	    TreeNode *pTreeNode                              // i: Node having text tile.        
		        );										  

	int CheckForImageButtonAttributes(                           // Checks for mandatory button attributes.
				TreeNode *pTreeNode							     // i: The node on which validation is done.
				);
	
	int CheckForRedundancy(                                       // Checks for redundant asignments. 
				TileAttributes *pTileAttribute					  // i: Tile attribute whose default valus has to be compared to. 
				);


	BOOL IsPossibleValue(                                         // Validate values.
				TileAttributes *pTileAttribute                    // i: Tile attribute to be validated. 
				);

	AuditLevelProcessor(                                           // Constructor
		        CString& wszFuncName,                              // i: FunctionName, used for error messages.   
				CString& wszWidgetName,                            // i: WidgetName, used for error messages.
				CString& wszKeyName                                // i: KeyName, used for error messages.
				);  
		
	AuditLevelProcessor();                                         // Default constructor.
	
	int FormatHintForRedAttr(                                      // Formats message for the attribute redundancy.
				CString& wszFuncName,                              // i:FunctionName.
				Tile& pTile,                                       // i:Tile name. 
				TileAttributes* pTileAttribute                     // i:Tile attribute to having redundant value
				);
	int FormatErrorMessage(										    // Formats the error message.
				int nErrorHeader,									// i: Error header, differs for level 2 and level 3.
                int nErrorMessage,									// i: Specific message.
				int nAuditLevel										// i: The audit level for which the error is added.
					);

	int CheckForButtonAttributes(                                   // Checks for mandatory buttons properties.
							TreeNode * pTreeNode					// i: Node having button tile.        
							);

	int ValidateAttribute(											// Validates the attribute.
						TileAttributes *pTileAttribute				// i: The tile attribute to be validated.
							  );

	int GenerateErrorString(									    // Generates an error string for data type mismatch.
							CString& wszAttributeName               // i:Attribute name.
								);

	int ProcessNode(                                                // This fucntion does the nodes level processing.
					TreeNode* pTreeNode                             // i: Node to be processed.
					);

	int ParseTree(                                                  // This function does the tree level audits.
				   TreeNode* pTreeNode                              // i: TreeNode to process.
				   );

	int ParseFile(													// Parsers all definitions in the corresponding files.
				 FileInfo *pFileInfo								// i: FileInfo pointer
				 );


	int StartProcessing();											// Start processing of all files.
    	
	AuditLevelProcessor(											// Constructor.
		int nFileID,												// i:FileID to be parsed.
		int nAuditLevel												// i: Audit level for which processing has to be done.
		);

	
	virtual ~AuditLevelProcessor();                                 // Destructor.

private:
	CString m_wszParentWidgetName;

	CString m_wszKeyName;                                           // Keyname. 
	CString m_wszWidgetName;                                        // Widgetname.
	CString m_wszFunctionName;                                      // Functionname.
	BOOL m_bProcessingDialog;                                       // Flag to keep track of whether a dialog definition is being processed. 
	BOOL m_bKeyAttributeSet;                                        // Flag to keep track of key attributes set for each widget.
	int m_nFileID;                                                  // FileID of file to be parsed.
	int m_nIsDefaultCount;                                          // Keep count of is_default attrbute set.  
	int m_nIsCancelCount;                                           // Keep count of is_cancel set.
	int m_nAuditLevel;                                              // The audit level for which auditing is taking place.
};

#endif 
