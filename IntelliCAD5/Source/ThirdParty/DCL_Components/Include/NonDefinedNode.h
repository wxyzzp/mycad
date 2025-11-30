/*
*  FILENAME:     NonDefinedNode.h- Contains declarations of class CNonDefinedNode.
*  Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*  Abstract 
*         This class has a pointer to the nondefined node and a data member holding
*   the tile whose reference was not found.
* 
*  Audit Log 
*
* 
*  Audit Trail
*
*
*/

#ifndef _NONDEFINEDNODE_H
#define _NONDEFINEDNODE_H

class TreeNode;

class NonDefinedNode : public CObject  
{
public: 
	
	CString get_FileName();                        // Get the file name where the nondefined node was found.

	int set_FileName(                              // Set the value of m_wszFileName.
				CString &wszFileName               // i:File name.
				);
	NonDefinedNode( 
		            CString& wszFileName,           // i : DCL File name.    
					int iLineNumber,                // i : Line number.
					int iReferenceCount,            // i : Reference count.
					TreeNode* pTreeNodePlaceHolder, // i : Placeholder node.
					CString& strTileName            // i : TileName.
					);
	CString get_TileNameReferenced();				// Get the referenced tile name.
	TreeNode* get_NodePlaceHolder();				// Get the place holder node.
	int get_ReferenceCount();						// Get the reference count.
	int get_LineNumber();							// Get the line number.
	NonDefinedNode();								// Constructor of NonDefined node.
	virtual ~NonDefinedNode();						// Destructor  of NonDefined node.
private:
	CString m_wszFileName;
	int m_iLineNumberOfUndefinedTile;				// Line number in DCL File where where undefined tile is used.
    int m_iReferencedCount;							// This is used while copying attributes.
	TreeNode * m_pTreeNodePlaceHolder;				// Pointer to the placeholder node.
	CString m_wszTileNameReferenced;				// Tile name referenced.
	

};

#endif 
