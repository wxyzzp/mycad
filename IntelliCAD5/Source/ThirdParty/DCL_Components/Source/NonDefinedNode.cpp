/** FILENAME:     NonDefinedNode.cpp- Contains implementation of class NonDefinedNode.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved. 
*
*
* Abstract 
*          This class has a pointer to the nondefined node and a data member holding
*          the tile whose reference was not found.
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "nondefinednode.h"
#include "treeNode.h"
#include "sds.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/************************************************************************************
/* + NonDefinedNode::NonDefinedNode() -   Constructor of NonDefined node.
*
*
*/
NonDefinedNode::NonDefinedNode()                 // Constructor of NonDefined node.
	{
    m_pTreeNodePlaceHolder = NULL;
	}


/************************************************************************************
/* + NonDefinedNode::~NonDefinedNode() -  Destructor  of NonDefined node
*
*
*/
NonDefinedNode::~NonDefinedNode()      //destructor  of NonDefined node
	{
	}

/************************************************************************************
/* + int NonDefinedNode::get_LineNumber()   
*	                       Get the line number
*
*/

int NonDefinedNode::get_LineNumber()   // Get the line number
	{
	return m_iLineNumberOfUndefinedTile;
	}


/************************************************************************************
/* + int NonDefinedNode::get_ReferenceCount 
*						Get the reference count.
*
*/

int NonDefinedNode::get_ReferenceCount()	// Get the reference count.
	{
	return m_iReferencedCount;
	}


/************************************************************************************
/* + TreeNode* NonDefinedNode::get_NodePlaceHolder
*						  Get the place holder node.
*
*
*/

TreeNode* NonDefinedNode::get_NodePlaceHolder() //  Get the place holder node.
	{
	ASSERT( m_pTreeNodePlaceHolder );
	return m_pTreeNodePlaceHolder;
	}



/************************************************************************************
/* + CString& NonDefinedNode::get_TileNameReferenced  
*    Get the referenced tile name
*
*
*/
CString NonDefinedNode::get_TileNameReferenced()  // Get the referenced tile.
	{
	return m_wszTileNameReferenced;
	}

/**********************************************************************************************************************************************
/* + NonDefinedNode::NonDefinedNode(int iLineNumber, int iReferenceCount, TreeNode *pTreeNodePlaceHolder, CString &strTileName)
*       Constructor
*
*
*/
NonDefinedNode::NonDefinedNode(									// Constructor
							   CString& wszFileName,            // i : DCL FileName.
							   int iLineNumber,                 // i : Line number.
							   int iReferenceCount,				// i : Reference count.
							   TreeNode *pTreeNodePlaceHolder,	// i : Placeholder node.
							   CString &strTileName				// i : TileName.
							   )
	{
	ASSERT( pTreeNodePlaceHolder );
	m_wszFileName = wszFileName;
	m_iLineNumberOfUndefinedTile = iLineNumber;
	m_iReferencedCount = iReferenceCount;
	m_pTreeNodePlaceHolder = pTreeNodePlaceHolder;
	m_wszTileNameReferenced = strTileName;
	

	}




/************************************************************************************
/* + int NonDefinedNode::set_FileName(CString &wszFileName) - 
*
*      Sets the FileName where the tile has been defined.
*
*
*
*/
int NonDefinedNode::set_FileName(						    // Sets the FileName where the tile has been defined.
								 CString &wszFileName       // i:DCL file name 
								 )
	{
    m_wszFileName = wszFileName;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + CString NonDefinedNode::get_FileName()                       
*
*        Returns the FileName where the tile has been defined.
*
*
*
*/

CString NonDefinedNode::get_FileName()                      // Returns the FileName where the tile has been defined.
	{
    return m_wszFileName;
	}
