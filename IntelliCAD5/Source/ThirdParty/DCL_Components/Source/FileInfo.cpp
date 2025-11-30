/** FILENAME:     FileInfo.cpp- Contains implementation of class FileInfo.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract - Implementation of the FileInfo class
*
*
*
* Audit Log 
*
*
* Audit Trail
*
*
*/


#include "stdafx.h"
#include "fileinfo.h"
#include "treenode.h"
#include "tree.h"
#include "nondefinednode.h"
#include "sds.h"
#include "exceptionmanager.h"
#include "main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/************************************************************************************
/* + FileInfo::FileInfo - Default constructor for the FileInfo object
*
*
*/

FileInfo::FileInfo()
	{
	
	m_wszDCLFileName = _T(" ");
	m_iDCLFileID = 0;
	m_bIsIncluded = TRUE;
	}

	
/************************************************************************************
/* + FileInfo::~FileInfo - Destructor for the FileInfo object
*   Delete nodes pointed by m_RootNodes
*	Delete nodes pointed by m_ListofNonDefinedNodes
*
*/

FileInfo::~FileInfo()
	{
    
    //deleting nodes pointed by m_RootNodes
	POSITION position = m_RootNodes.GetHeadPosition();
	for ( int iIndex = 0; iIndex < m_RootNodes.GetCount(); iIndex++)
		{
    	Tree* pTree = (Tree*) m_RootNodes.GetNext(position);

			if ( pTree )
				{
				pTree->Exit();
				}

	    extern Main g_Main;
		g_Main.m_hdlgtoTree.RemoveKey(pTree->pDialogAddress);	/*DG - 11.2.2003*/// The map entry should be removed as well (e.g. if we unload a dialog, but then try to load it again).
		delete pTree;
		}
		m_RootNodes.RemoveAll();
	}



/*******************************************************************************************
/* + FileInfo::FindReferencedNode(CString& wszTileName, int nType, TreeNode** ppTreeNode )- 
*    To get a TreeNode given the tilename and the type.The type could be
*     0 - dialog
*	  1 - subassembly or prototype
*     Return Value - SDS_RTNORM if ppTreeNode contains a valid address
*                  - SDS_RTERROR if ppTreeNode is NULL
*/

int FileInfo::FindReferencedNode(						// Get the TreeNode.
								 CString& wszTileName,  // i: Name of the tile.
								 int nType,				// i: Type of the tile.
								 TreeNode** ppTreeNode  // o: Pointer to pointer to TreeNode.
								 )
	{
	int nRet = SDS_RTERROR;
	CString wszTile = wszTileName;
	if ( !m_RootNodes.IsEmpty() )
		{
		POSITION position = m_RootNodes.GetHeadPosition();
		if ( position )
			{
			for ( int iIndex = 0; iIndex < m_RootNodes.GetCount(); iIndex++ )
				{  
				Tree * pTree =(Tree *) m_RootNodes.GetNext(position);
				//check the type first then only do string comparison
				if ( pTree->get_TreeType() == nType )
					{
					if ( wszTile == (pTree->get_StartTreeNode())->m_Tile.get_FunctionName()  && (pTree->get_StartTreeNode())->IsComplete())
						{
						if ( CheckForUndefinedNodesInTree(pTree->get_StartTreeNode()) == RTNORM )   
							{  
							*ppTreeNode = pTree->get_StartTreeNode();   //yes the function names match
							(*ppTreeNode)->m_bIsComplete = TRUE ; 
							nRet = SDS_RTNORM;
							break;
							}
						}
					}	
				} // End of If type matches condition
			} // End of for loop to iterate through the list of root nodes.
		}											
		
	return nRet;
	}

/************************************************************************************
/* + Tree* FileInfo::FindReferencedTree(CString wszTileNam - 
*
*	Get the specified tree
*	
*
*
*/

Tree* FileInfo::FindReferencedTree(
								   CString wszTileName // i:Name of the tile
								   )
	{
	int nRet = SDS_RTERROR;
	int nType = 0;
	CString wszTile = wszTileName;
	if ( !m_RootNodes.IsEmpty() )
		{
		POSITION position = m_RootNodes.GetHeadPosition();
		if ( position )
			{
			for ( int iIndex = 0; iIndex < m_RootNodes.GetCount(); iIndex++ )
				{  
				Tree * pTree =(Tree *) m_RootNodes.GetNext(position);
				//check the type first then only do string comparison
				if ( pTree->get_TreeType() == nType )
					{
					if ( wszTile == (pTree->get_StartTreeNode())->m_Tile.get_FunctionName()  && (pTree->get_StartTreeNode())->IsComplete())
						{
							//if ( CheckForUndefinedNodesInTree(pTree->get_StartTreeNode()) == RTNORM )   
							//	{  
								return ( pTree);   //yes the function names match
							//	}
						}
					}	
				} // End of If type matches condition
			}     // End of for loop to iterate through the list of root nodes.
		}																
	
	return NULL;
	}


/************************************************************************************
/* + int FileInfo::CheckForUndefinedNodesInTree(TreeNode *pTreeNode)          
* 												
*    This function is used to check if there are any undefined nodes in the tree to be cloned.
*  If there are any nondefined nodes then return RT_ERROR.
*
*
*/
int FileInfo::CheckForUndefinedNodesInTree(                     // Checks for uncompleted nodes.
								   TreeNode *pTreeNode          // Node  pointer  to start from.
								   )
	{
     
	BOOL bHasChildren;                                    // Pointer to traverse thru the tree.
	TreeNode *pOldNode = NULL;                            // Pointer to store the previous value of pOldNode.
	TreeNode *TempOldNode = NULL;                          
	     
	pOldNode = pTreeNode ;                                // Initialize OldNode. 
 
	bHasChildren  =  pOldNode->HasChild();                // Check for children.

	if(!bHasChildren)                                     // If tree to be cloned has a single node.   
		{
		if ( pOldNode->m_bIsComplete == FALSE ) 	   // If node not complete return SDS_RTERROR.
			{
			return SDS_RTERROR;
			}
		else
			{
			return SDS_RTNORM;
			}
		}
	else
		{
		if ( pOldNode->m_bIsComplete == FALSE ) 	       // If node not complete return SDS_RTERROR.
			{
			return SDS_RTERROR;
			}
		}

	while(1)
		{
 		bHasChildren  =  pOldNode->HasChild();                  // Check whether child is present.    
             
	    if (bHasChildren)
			{ 
		    pOldNode = pOldNode->GetFirstChild();			   //  Get the first child.
	       
			if ( pOldNode->m_bIsComplete == FALSE ) 	  
				{
				return SDS_RTERROR;
				}

			}// End of if(bHasChildren).
		else
			{ 
		    // If child not found go to the next sibling.
		 	TempOldNode = pOldNode ;                      
			if (pOldNode->get_Parent() != NULL)
				{
				pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
				}
			else
				{
				pOldNode = NULL;
				}

             if (pOldNode != NULL)
				{
		        if ( pOldNode->m_bIsComplete == FALSE ) 	  // If node not complete return SDS_RTERROR.
					{
					return SDS_RTERROR;
					}
				}	
			else
				{
				// If sibling noit found go to parent.
				pOldNode = TempOldNode;
				pOldNode = pOldNode->get_Parent();
			                    
				while (1)
					{// To set next sibling after child level siblings have been traversed.
					if (pOldNode->get_Parent())
						{
						TempOldNode = (pOldNode->get_Parent())->GetNextChild();
						}
					else
						{
						TempOldNode = NULL;
						}
                   
					// Keep moving up the tree till sibling is found.
					if (TempOldNode != NULL)
						{
						pOldNode = TempOldNode;
						if ( pOldNode->m_bIsComplete == FALSE ) 	  
							{
							return SDS_RTERROR;
							}
						break;
						}
					else
						{
						// Break if the traversing operation reaches where it started. 
						if (pOldNode == pTreeNode)
							{
							break;
							}

                     					 
				         pOldNode = pOldNode->get_Parent();
					 
						if (pOldNode == pTreeNode)
							{
							break;
							}
						}
					}// To set next sibling after chilren level siblings have been traversed--end.	   

	           
				} // End check for has child

				if (pOldNode == pTreeNode)
					{
					break;
					}
         }
	} 
	return SDS_RTNORM	;
	}


/********************************************************************************************************
/* + int FileInfo::AddRootNode(Tree *pTree) - Add the Tree object pointer to the m_RootNodes list.
* 
*            
*
*/
int FileInfo::AddRootNode(                        // Adds a Tree pointer to the m_RootNodes list.
						  Tree *pTree             // i: Pointer to Tree object.
						  )
	{

	ASSERT(pTree);
	// Add to the root nodes list
	try
		{
		m_RootNodes.AddTail(pTree); 
		}
	catch(...)
	   {
	   ThrowCExceptionManager(LIST_INSERT_FAILED);
	   }
    
	return SDS_RTNORM;

	}


/************************************************************************************
/* + int FileInfo::get_DCLFileID() - 
*      
*     Returns the value of m_DCLFileID.
*
*
*
*/
int FileInfo::get_DCLFileID()          // Returns the value of m_DCLFileID.
	{
    return m_iDCLFileID;
	}



/************************************************************************************
/* + BOOL FileInfo::IsIncluded() - 
*
*    Return the value of m_bIsIncluded.
*
*
*
*/
BOOL FileInfo::IsIncluded()       // Return the value of m_bIsIncluded.
	{
	return m_bIsIncluded;
	}


/************************************************************************************
/* + CString FileInfo::get_DCLFileName() - 
*
*    Returns the value of m_wszDCLFileName.
*
*
*
*/
CString FileInfo::get_DCLFileName()   //  Returns the value of m_wszDCLFileName.
	{
    return m_wszDCLFileName;
	}

/************************************************************************************
/* + int FileInfo::FindNodeExists(CString& wszFunctionName) - 
*
*   This function finds for multiple definitions of nodes and returns an error value 
*   of node is present otherwise returns SDS_RTNORM.
*
*/
int FileInfo::FindNodeExists(                            // Checks for existence of node.
							 CString& wszFunctionName    // i: Name of node to be searched . 
							 )
	{

	int nRet = SDS_RTERROR;
	CString wszTile = wszFunctionName;
	if ( !m_RootNodes.IsEmpty() )
		{
		POSITION position = m_RootNodes.GetHeadPosition();
		if ( position )
			{
			for ( int iIndex = 0; iIndex < m_RootNodes.GetCount(); iIndex++ )
				{  
				Tree * pTree =(Tree *) m_RootNodes.GetNext(position);
				//check the type first then only do string comparison
				if ( wszTile == (pTree->get_StartTreeNode())->m_Tile.get_FunctionName())
					{
						{  
						nRet = SDS_RTNORM;
						break;
						}
					}
				}	
			} // End of for loop to iterate through the list of root nodes.
		}											
		
	return nRet;


	}	
