/** FILENAME:     FileInfo.h- Contains declarations of class FileInfo.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract - This class contains the information of a loaded .dcl file
*			 and a list that contains a Tree object for every dialog,prototype
*			 ,subassembly present in the DCL file
*
* Audit Log 
*
*
* Audit Trail
*
*
*/



#ifndef _FILEINFO_H
#define _FILEINFO_H

#include "tile.h"

class TreeNode;
class Tree;

class FileInfo : public CObject  
{
public:
	
	int FindNodeExists(                          // Checks for existence of node.
		       CString& wszFunctionName			 // i: Name of node to be searched .
						);

	CString get_DCLFileName();                   // Returns the value of m_wszDCLFileName.
	
	BOOL IsIncluded();                           // Return the value of m_bIsIncluded.
		
	int get_DCLFileID();						 //	Returns the value of m_DCLFileID.

	int AddRootNode(                             // Adds a Tree pointer to the m_RootNodes list.
		       Tree *pTree                       // i: Pointer to Tree object.
			   );

	int FindReferencedNode(						  // Get the specified treenode.
							CString& wszTileName, // i: Name of the tile.
							int nType,            // i: Type of the tile.
							TreeNode** ppTreeNode // o: Ptr to ptr to the TreeNode.
							);

	int CheckForUndefinedNodesInTree(                      // Checks for uncompleted nodes.
								   TreeNode *pTreeNode     // i: Node  pointer  to start from.     
								   );

	FileInfo(                          // Constructor for FileInfo class.
			CString& wszDCLFileName,   // i : DCL File name .
			long iDCLFileID,           // i : ID of the .DCL File.
			BOOL bIsIncluded           // i : Flag to indicate wheteher file was included or directly loaded.
			)
			{
				m_wszDCLFileName = wszDCLFileName;
				m_iDCLFileID = iDCLFileID;
				m_bIsIncluded = bIsIncluded;
			}
	Tree* FindReferencedTree(			// Get the specified tree
		CString wszTileName				// i: Name of the tile
		);
	FileInfo();                       // Constructor.
	virtual ~FileInfo();              // Destructor.

private:
	
	
	CObList m_RootNodes;               // A list of the Tree objects.
									   // ( which contain a RootPointer)for every dialog,subassembly,prototype present in the dcl file
	long m_iDCLFileID;                 // Unique FileID (tds_load_dialog returns this id).
	CString m_wszDCLFileName;          // .dcl filename.
	BOOL m_bIsIncluded;                  // To keep track of file, whether it was included with another DCL file or loaded directly 
	
   friend class AuditLevelProcessor; 
	
	
   friend class TreeDebugger;         // Added for debugging purpose, to be removed for final build.
   friend class CDCLDoc;              // Added for debugging purpose, to be removed for final build. 

};

#endif 
