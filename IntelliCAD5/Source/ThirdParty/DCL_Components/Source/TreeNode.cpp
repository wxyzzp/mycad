/** FILENAME:     TreeNode.cpp- Contains implementation of class TreeNode.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
*  Abstract 
*     This class represents each node in the parsed tree.It stores
*  pointers to its parent and child nodes and also has methods  to     
*  retieve the same.  
* 
*
*  Audit Log 
*
*
*  Audit Trail
*
*
*/


#include "stdafx.h"
#include "constants.h"
#include "treeNode.h"
#include "sds.h"
#include "exceptionmanager.h"
#include "tile.h"
#include "main.h"
#include "fileinfo.h"
#include "parser.h"
#include "ScreenObject.h"
extern Main g_Main;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning (disable : 4355) 

/************************************************************************************
/* + TreeNode::TreeNode():m_Tile - TreeNode Constructor
* 
*       
*
*
*
*/

TreeNode::TreeNode():m_Tile(this) // TreeNode Constructor.
	{
	m_pParentNode = NULL;
	m_iChildCount = 0;
	m_iLastAccessedChild = 0;
	m_nType = 0;
	m_bIsComplete = TRUE;
	m_nDeltaX = 0;
	m_nDeltaY = 0;
	m_nSummatedWidth = 0;
	m_nSummatedHeight = 0;
	m_nChildDiffWidth = 0;
	m_nAllSwatches = 0;
	m_nHorSpaceExpand = 0;
	}


/************************************************************************************
/* + TreeNode::~TreeNode - TreeNode desctructor
*
* 
*   All the child nodes pointed to by the current tree node is deleted here.
*
*
*/
TreeNode::~TreeNode()  // TreeNode destructor
	{
	
	if ( m_iChildCount )
		{
		POSITION position = m_listSiblings.GetHeadPosition();
		for ( int i = 0; i < m_iChildCount; i++)
			{
			TreeNode* treeNode = (TreeNode*)m_listSiblings.GetNext(position);
			delete treeNode;
			}
		m_listSiblings.RemoveAll();
		}
	}


/************************************************************************************
/* + int TreeNode::GetChildCount() - returns the child count
*
*
*
*
*
*/

int TreeNode::GetChildCount() // Returns the child count.
	{
	return m_iChildCount;
	}


/************************************************************************************
/* + BOOL TreeNode::HasChild() - Returns TRUE if child exists or else FALSE
*
*
*
*
*
*/

BOOL TreeNode::HasChild() // Returns TRUE if child exists or else FALSE.
	{
	if ( m_iChildCount )
		return TRUE;
	return FALSE;
	}



/**************************************************************************************************
/* + int TreeNode::AddChild(TreeNode* pChild) - Adds a child tree node. 
*
*
*
*
*
*/
int TreeNode::AddChild(                      // Adds a child tree node. 
					   TreeNode* pChild      // i: Pointer to child tree node to be added. 
					   ) 
	{
	int nRet = SDS_RTNORM;
	ASSERT( pChild );
	pChild->m_pParentNode = this;
	// Add child to the m_ListSiblings list.
	try
		{
		m_listSiblings.AddTail(pChild);
		}
	catch(CMemoryException exception)
		{
		ThrowCExceptionManager(LIST_INSERT_FAILED);
		}
	m_iChildCount++;
	return nRet;
	}


/************************************************************************************
/* + TreeNode* TreeNode::GetChild(int iIndex) - Returns a tree node pointer
*                                               depending upon the index passed.       
*
*
*
*
*/
TreeNode* TreeNode::GetChild(					// Returns a tree node pointer depending upon the index passed    
							 int iIndex			// i: The subscript value for the child treenode to be returned
							 )
	{
	POSITION position;
	position = m_listSiblings.FindIndex(iIndex);
	ASSERT( position );
	if ( position )
		{
		m_iLastAccessedChild = iIndex;
		return ( TreeNode* )m_listSiblings.GetAt(position);
		}
	return NULL;
	}


/*********************************************************************************************************
/* + TreeNode* TreeNode::GetPreviousChild() - Returns a pointer to the previous child node.
*	                        (ie this count is maintained in "m_iLastAccessedChild" member
*
*
*
*
*
*/

TreeNode* TreeNode::GetPreviousChild()    // Returns a pointer to the previous child node.
	{
	POSITION position;
	ASSERT( m_iLastAccessedChild != 0 );
	m_iLastAccessedChild--;
	
	position = m_listSiblings.FindIndex(m_iLastAccessedChild);
	ASSERT( position );
	
	if ( position )
		{
		return ( TreeNode* )m_listSiblings.GetAt(position);
		}
	return NULL;
	}


/************************************************************************************
/* + TreeNode* TreeNode::GetNextChild() - Returns a pointer to the next child node.
*                     (ie this count is maintained in "m_iLastAccessedChild" member.
*
*
*
*
*/
TreeNode* TreeNode::GetNextChild()    // Returns a pointer to the next child node.
	{
	POSITION position;
	m_iLastAccessedChild++;
	
	position = m_listSiblings.FindIndex(m_iLastAccessedChild);
	
	if ( position )
		{
		return ( TreeNode* )m_listSiblings.GetAt(position);
		}
	return NULL;
	}


/************************************************************************************
/* + TreeNode* TreeNode::GetLastChild() - Returns a pointer to last child node.
*
*
*
*
*
*/
TreeNode* TreeNode::GetLastChild()               // Returns a pointer to last child node.
	{
	if ( m_iChildCount )
		{
		m_iLastAccessedChild = m_iChildCount - 1;
		return (TreeNode*)m_listSiblings.GetTail();
		}
	return NULL;
	}

/************************************************************************************
/* + TreeNode* TreeNode::GetFirstChild() - Returns a pointer to the first child node.
*
*
*
*
*
*/
TreeNode* TreeNode::GetFirstChild()                        // Returns a pointer to the first child node.
	{
	if ( m_iChildCount )
		{
		m_iLastAccessedChild = 0;
		return (TreeNode*)m_listSiblings.GetHead();
		}
	return NULL;
}


/************************************************************************************
/* + TreeNode* TreeNode::get_Parent() - Returns a pointer to the parent node.
*
*
*
*
*
*/
TreeNode* TreeNode::get_Parent()          // Returns a pointer to the parent node.
	{
	return m_pParentNode;
	}


/***************************************************************************************************
/* + TreeNode& TreeNode::operator =(TreeNode &pTreeNodeToClone) - 
*             Assignment operator overloaded for treenode,returns a reference to treenode.
*    
*
*
*
*/
TreeNode& TreeNode::operator =(                             // Assignment operator overloaded for treenode,returns a reference to treenode.
							   TreeNode &pTreeNodeToClone   // i: Reference to tree node to be copied.
							   )
	{
	// only cloning the attributes
	m_nType = pTreeNodeToClone.m_nType; 
	m_Tile = pTreeNodeToClone.m_Tile;
	return *this;
	}

/************************************************************************************
/* + int TreeNode::CloneTree(TreeNode *pReferencedTreeNode) - 
*                   Clones the complete tree starting with the the root passed as reference.
*
*
*
*
*/
int TreeNode::CloneTree(                                  // This method clones the complete tree starting with the the root passed as reference.
						TreeNode *pReferencedTreeNode,    // i: A pointer to the root of tree to be cloned.
						int      nReferenceCount          // i: 0 for first reference ,and 1 - for multiple references.
						)
	{

	BOOL bHasChildren;			  // Variable to check for children.
	TreeNode *pOldNode = NULL ;   // Used to point to source node.
	TreeNode *pNewNode = NULL ;   // Points to new node.
	TreeNode *pParent = NULL  ;   // Keeps track of parent  of new node.
	TreeNode *TempOldNode = NULL; // Keeps track of previous value of pOldNode when pOldNode changes.
	CString wszRootNodeName ;

    // To get the definition node   
	TreeNode *tempNode = NULL;
    wszRootNodeName = this->m_Tile.get_FunctionName();  
	tempNode = this->get_Parent();
    while ( tempNode )
		{
		wszRootNodeName = tempNode->m_Tile.get_FunctionName();  
		tempNode = tempNode->get_Parent();
		}
    

	try
		{
		pParent = this;
		pOldNode = pReferencedTreeNode ;

		if ((m_Tile.get_FunctionName()).IsEmpty())
			{
			m_Tile.set_FunctionName(pOldNode->m_Tile.get_FunctionName());  
			}


		bHasChildren  =  pOldNode->HasChild();

		// If the tree has just one node then clone the single node and return.
		if(!bHasChildren)
			{
			if ( nReferenceCount == 0) 	
				{  
	             m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,nReferenceCount);
				}
		    else
			   {
			   m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,nReferenceCount); 
               }

            return SDS_RTNORM;
			}

		if ( nReferenceCount == 0)
			{   
	        m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,0); 
			}
		else
			{ 
		    m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,nReferenceCount); 
			}
	

		while(1)
			{
 
			bHasChildren  =  pOldNode->HasChild();
       
			// If the node has children.
			if (bHasChildren)
				{ 
				pOldNode = pOldNode->GetFirstChild();
	   
				// Create a new node and clone the tile and its attributes.
    			pNewNode = new TreeNode ;
		        pNewNode->m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,0); 
		   
				pParent->AddChild(pNewNode);
				pNewNode->m_pParentNode = pParent ;  
				pParent = pNewNode; 

				}// end of if(bHasChildren)
			else
				{ 
				// If the node does not have a child move to the parent of new node of new tree.
				
				if (pNewNode != NULL)
					{
					pParent = pNewNode->get_Parent();
					}
				
				
				TempOldNode = pOldNode ;
				
				// If the node does not have a child move to the next child of source tree.
				if (pOldNode->get_Parent() != NULL)
					{
					pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
					}
				else
					{
					pOldNode = NULL;
					}
             
				// If next child found.
				if (pOldNode != NULL)
					{
					
		    		pNewNode = new TreeNode ;
					pNewNode->m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,0); 

					// Add node to parent.
					pParent->AddChild(pNewNode);
					pNewNode->m_pParentNode = pParent;  
					pParent = pNewNode; 

					}
				else
					{
					// If next sibling not found then move to the parent in turn.
					pOldNode = TempOldNode;
					pOldNode = pOldNode->get_Parent();
					if ( pParent->get_Parent() )
						pParent = pParent->get_Parent();
					// {{{ dev - 04/06
					// commented and the following statements moved to an if condition below.
/*					if ( pParent->get_Parent() )
						pParent = pParent->get_Parent();  */
					// }}}
                 
					while (1)
						{//  To set next sibling after child level siblings have been traversed.
						if ( pOldNode->get_Parent() )
							{
							TempOldNode = (pOldNode->get_Parent())->GetNextChild();
							}
						else
							{
							TempOldNode = NULL;
							}

						if (TempOldNode != NULL)
							{
							pOldNode = TempOldNode;

							pNewNode = new TreeNode ;

							pNewNode->m_Tile.CopyAttributes(wszRootNodeName,pOldNode->m_Tile,0); 
					 
							pParent->AddChild(pNewNode);
							pNewNode->m_pParentNode = pParent;  
							pParent = pNewNode; 

							break;
							}
						else
							{
							// {{{ dev - 04/06
							if ( pParent->get_Parent() )
								pParent = pParent->get_Parent();  
							// }}}
							if(pOldNode == pReferencedTreeNode)
								break;
                     
							pOldNode = pOldNode->get_Parent();

							// If root of source tree reached then exit.
							if(pOldNode == pReferencedTreeNode)
								break;
					
				    
							}
						}// To set next sibling after chilren level siblings have been traversed--end.	   

					}

					if(pOldNode == pReferencedTreeNode)
						{
						break;
						}
				}
 
			} 

		}//end of try
	catch(...)
		{
		ThrowCExceptionManager(TREE_CREATE_FAILED);
		}
	// end of clone tree
	return TRUE;
	}


/************************************************************************************
/* + int TreeNode::get_TreeNodeType() - Returns the value of m_nType member.
*
*
*
*
*
*/
int TreeNode::get_TreeNodeType() // Returns the value of m_nType member.
	{
    return m_nType;
	}

/************************************************************************************
/* + int TreeNode::set_TreeNodeType() - Sets the value of m_nType data member.
*
*
*
*
*
*/
int TreeNode::set_TreeNodeType(            // Sets the value of m_nType data member.
							  int nType    // i: 0 - dialog , 1 - prototype or sub-assembly.
							  )
	{
    m_nType = nType;
	return SDS_RTNORM;
	}



/************************************************************************************
/* + BOOL TreeNode::IsComplete() - Returns TRUE/FALSE value od m_bIsComplete.
*
*
*
*
*
*/
BOOL TreeNode::IsComplete()       // Returns TRUE/FALSE value od m_bIsComplete.
	{
    return m_bIsComplete;
	}



/******************************************************************************************************
/* + int TreeNode::set_IsComplete(BOOL bIsComplete) -  Sets the value of m_bIsComplet datamember
*
*
*
*
*
*/
int TreeNode::set_IsComplete(                                     // Sets the value of m_bIsComplete
							 BOOL bIsComplete					  // i: TRUE/FALSE  
							 )
	{
	m_bIsComplete = bIsComplete;
	return SDS_RTNORM;
	}


/************************************************************************************
/* + int TreeNode::GetChildIndex(TreeNode *pChildNod )
*	Gets the zero based index of a child in the list of children
*	else returns -1.
*
*
*
*/

int TreeNode::GetChildIndex(						// Gets the zero based index of a child in the list of children
							TreeNode *pChildNode	// i : ChildNode pointer.
							)
	{
	ASSERT( pChildNode );
	int i = 0,iTotalChildren= 0;
	POSITION posFixed,posVariable;
	iTotalChildren = m_listSiblings.GetCount();
	ASSERT( iTotalChildren );
	posFixed = m_listSiblings.Find(pChildNode);      // Find the child node.
	if ( posFixed )                           
		{
		for ( i = 0; i < iTotalChildren; i++ )       // Iterate for all chldren till, the respective child is found.
			{
			posVariable = m_listSiblings.FindIndex(i);
			if ( posVariable == posFixed )
				{
				return i;                          // Return the index of the child position in the list.
				}
			}
			return SDS_RTERROR;
		}
	else
		{
		return SDS_RTERROR;
		}
	
	}


/************************************************************************************
/* + int TreeNode::set_LastAccessedChild(int iIndex) 
*	Function to set the LastAccessed Child.
*
*
*
*
*/

int TreeNode::set_LastAccessedChild(			// Function to set the LastAccessed Child to the specified index.
									int iIndex	// i : Index of the child
									)
	{
	m_iLastAccessedChild = iIndex;
	return SDS_RTNORM;
	}


/************************************************************************************
/* + int TreeNode::get_LastAccessedChild ()
*	Function to retrieve the index of the last accessed child.
*	Before looping the node's children save this index and after 
*	the looping restore its value only when and where needed.
*
*
*/

int TreeNode::get_LastAccessedChild()			// Retrieves the index of the last accessed child.
	{
	return m_iLastAccessedChild;
	}

/************************************************************************************
/* + int TreeNode::set_DeltaX(int ndeltax)  
*
*    Sets the m_nDeltaX value.
*
*
*
*/
int TreeNode::set_DeltaX(                           // Sets the m_nDeltaX value.
						 int ndeltax                // i: value to be set. 
						 )
	{
	m_nDeltaX = ndeltax;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int TreeNode::set_DeltaY(int ndeltay) - 
*
*    Sets the m_nDeltaY value.         
*
*
*
*/
int TreeNode::set_DeltaY(                            // Sets the m_nDeltaY value.        
						 int ndeltay				 // i: value to be set. 
						 )
	{
	m_nDeltaY = ndeltay;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int TreeNode::get_DeltaX() - 
*
*    Gets the m_nDeltaX value.          
*
*
*
*/
int TreeNode::get_DeltaX()                  // Gets the m_nDeltaX value.        
	{
	return m_nDeltaX;
	}

/************************************************************************************
/* + int TreeNode::get_DeltaY() - 
*
*       // Gets the m_nDeltaY value.        
* 
*
*
*/
int TreeNode::get_DeltaY()                  // Gets the m_nDeltaY value.        
	{
	return m_nDeltaY;
	}


/************************************************************************************
/* + CString TreeNode::get_DefinitionName() - 
*
*     Get defintion name of the current node.
* 
*
*
*/
CString TreeNode::get_DefinitionName()
	{
    CString wszRootNodeName;
	// To get the definition node   
	TreeNode *pTreeNode;
    wszRootNodeName = this->m_Tile.get_FunctionName();  
	pTreeNode = this->get_Parent();

	// Iterate recursively and get the root definition name.
	// This name is used for error reporting purpose.
    while ( pTreeNode )
		{
		wszRootNodeName = pTreeNode->m_Tile.get_FunctionName();  
        pTreeNode = pTreeNode->get_Parent();
		}

    
	return wszRootNodeName;
	
	}
/************************************************************************************
/* + int TreeNode::IsNodeASwatch()
*	Function To test whether this particular tile is a swatch
*   or something  derived from a swatch.
*	Returns 1 - If swatch
*			0 - If not a swatch
*
*/

int TreeNode::IsNodeASwatch()
	{
	CString strTile = m_Tile.get_TileName();
	CString strFunction = m_Tile.get_FunctionName();
	int nRes = 0;
	TreeNode* pTreeNode = NULL;
	if ( m_nAllSwatches == 1 )
		return 1;
	if ( strTile == _T("swatch") )
		return 1;
	if ( strFunction == _T("swatch") )
		return 1;
	else if ( strFunction == _T("image_button") )
		return 0;
	else 
		{
		POSITION pos = g_Main.m_DCLParser.m_ListOfDCLFileData.GetTailPosition();
		ASSERT ( pos );
		if ( pos )
			{
			for (  int i = 0; i < g_Main.m_DCLParser.m_ListOfDCLFileData.GetCount(); i++ )
				{
				ASSERT( pos );
				FileInfo *fInfo = (FileInfo*)g_Main.m_DCLParser.m_ListOfDCLFileData.GetPrev(pos);
				ASSERT ( fInfo );
				if ( fInfo )
					{
					fInfo->FindReferencedNode(strTile,1,&pTreeNode);
					
					if ( pTreeNode )
						{
						nRes = pTreeNode->IsNodeASwatch();
						if (nRes == 1)
							{
							break;
							}
						else
							{
							}
						}
					}
				}
			return nRes;
			}
		return nRes;
		}
	}
/************************************************************************************
/* + int TreeNode::CheckItorNot()
*	Checks whether this radio_button has value set to 1(which represents its check state).
*	If its value is 1,go to the previous radio_buttons and set the 
*	CheckVar using set_CheckVar(0) to set the m_nCheckThisRadioBtn to 0.
*
*
*/	

int TreeNode::CheckItorNot()
	{
	TreeNode *pThisNode = this;
	ASSERT(pThisNode);
	TreeNode* pParentNode = this->get_Parent();
	ASSERT(pParentNode);
	BOOL bSet = FALSE;
	if ( pParentNode )
		{
		if ( pParentNode->m_Tile.get_ListOfAttributes()->get_ValueString().IsEmpty() == FALSE )
			{
			if ( pParentNode->m_Tile.get_ListOfAttributes()->get_ValueString() == this->m_Tile.get_ListOfAttributes()->get_Key() ) 
				{
				bSet = TRUE;
				}
			}
		}
	ScreenObject *pScreenObject = pThisNode->m_Tile.get_ScreenObject();
	ASSERT(pScreenObject);
	if ( pScreenObject->m_attriblist->get_ValueString() == CString("1") || bSet )
		{
		
		TreeNode* pParentNode = NULL;
		pParentNode = pThisNode->get_Parent();
		ASSERT(pParentNode);
		int nIndexToRestore = pParentNode->get_LastAccessedChild();
		int index = pParentNode->GetChildIndex(pThisNode);
		for ( int iLoop = 0; iLoop < index; iLoop++ )
			{
			TreeNode* pTreeNodeHolder = pParentNode->GetChild(iLoop);
			ASSERT(pTreeNodeHolder);
			ScreenObject* pScreenObjectHolder = pTreeNodeHolder->m_Tile.get_ScreenObject();
			ASSERT(pScreenObjectHolder);
			if (pTreeNodeHolder->m_Tile.get_RenderableTileName() == _T("radio_button") )
				pScreenObjectHolder->set_CheckVar(0);
			}
		pScreenObject->set_CheckVar(1);
		pParentNode->set_LastAccessedChild(nIndexToRestore);
		}

	// This function sets the action attribute of radio button, thru its parent's actios.
    CopyParentsAction(); 


	return SDS_RTNORM;
	}



/************************************************************************************
/* + int TreeNode::CopyParentsAction() - 
*
*   Copy the parent's action to the children, in case action is not set for the child tile.
*   (for radio button)
*
*
*/
int TreeNode::CopyParentsAction()
	{

	try
		{
		TreeNode *pThisNode = this;
		ASSERT(pThisNode);
		TreeNode* pParentNode = this->get_Parent();
		ASSERT(pParentNode);
		BOOL bSet = FALSE;
		CString wszParentAction;

		int nRetParent = pParentNode->m_Tile.get_ListOfAttributes()->FindAttribute(CString(_T("action")));

		if ( nRetParent  == SDS_RTNORM )
			{
			wszParentAction = pParentNode->m_Tile.get_ListOfAttributes()->get_Action();
			int nRetChild = m_Tile.get_ListOfAttributes()->FindAttribute(CString(_T("action")));
			
			if ( nRetChild == SDS_RTERROR )
				{
				m_Tile.get_ListOfAttributes()->AddAttribute(_T("action"),_T("s"),wszParentAction);
				}
			}
		}
	catch(...)
		{
		TRACE("Error while copying parent action");
		return SDS_RTERROR;
		}


	return SDS_RTNORM;



	}



/************************************************************************************
/* + int TreeNode::IsNodeACluster()
*	Function To test whether this particular tile is a Cluster
*   or something  derived from a cluster.
*	Returns 1 - If cluster
*			0 - If not a cluster
*
*/

int TreeNode::IsNodeACluster()
	{
	CString strTile = m_Tile.get_TileName();
	CString strFunction = m_Tile.get_FunctionName();
	int nRes = 0;
	TreeNode* pTreeNode = NULL;

	
	if ( strTile == _T("cluster") )
		nRes = CLUSTER_T;
	else if ( strFunction == _T("cluster") )
		nRes = CLUSTER_T;
	else if ( strTile == _T("radio_cluster") )
		nRes = RADIO_CLUSTER_T;
	else if ( strFunction == _T("cluster") )
		nRes = RADIO_CLUSTER_T;
	else 
		{
		POSITION pos = g_Main.m_DCLParser.m_ListOfDCLFileData.GetTailPosition();
		ASSERT ( pos );
		if ( pos )
			{
			for (  int i = 0; i < g_Main.m_DCLParser.m_ListOfDCLFileData.GetCount(); i++ )
				{
				ASSERT( pos );
				FileInfo *fInfo = (FileInfo*)g_Main.m_DCLParser.m_ListOfDCLFileData.GetPrev(pos);
				ASSERT ( fInfo );
				if ( fInfo )
					{
					fInfo->FindReferencedNode(strTile,1,&pTreeNode);
					
					if ( pTreeNode )
						{
						nRes = pTreeNode->IsNodeACluster();
						if (nRes)
							{
							break;
							}
						else
							{
							}
						}
					}
				}
			return nRes;
			}
		return nRes;
		}
	return nRes;
	}



/************************************************************************************
/* + int TreeNode::CalcHorSpaceExp()
* Find how much the tile after a spacer0 should move ahead of the spacer. 
* Loop through all the children.This is the parent.
* Find nTotChild = (Active children =Children other than spacers).
* Find nTotSpaces = Total number of spaces.(use the renderable tile name)
* nParentWidth = this nodes width.
* nSumActChildWidth = Active children summation of widths.
* nRemainingWidth = (nParentWidth - nSumActChildWidth)--This has the SPACEXXX still in it.
* nSpacesTBDistributed = Number of SPACEXXX to be distibuted among the spacers.
*                        ( nTotChild - 1) - nTotSpaces
* nWidthTBDistributed = nSpacesTBDistributed * SPACEXXX
* nSpacerShare(this is the most important because this will be added to the tiles left coordinate if the tile follows a spacer.)
*
*
*
*
*/
int TreeNode::CalcHorSpaceExp()
	{
	int nTotChild = 0,nTotSpaces = 0,nTotSpacer1 = 0;;
	int nParentWidth = 0,nSumActChildWidth = 0;
	int nRemainingWidth = 0;
	int nSpacesTBDistributed = 0,nWidthTBDistributed = 0;
	int nSpacerShare = 0;// Set this to the m_nHorSpaceExpand.Let this be 1 pixel
	int nSpacerOccupied = 0;
	

	// This is now done only for horizontal containers because there is no problem with the spaces in a vertical
	// container.
	if ( this->get_DeltaX() > 0 )
		{
		if ( this->m_Tile.get_ListOfAttributes()->get_Layout() == LAYOUT_HORIZONTAL )
			{
			nParentWidth = this->m_Tile.get_ScreenObject()->get_WidthToSet();
			ASSERT( nParentWidth );
			for ( int iIndex = 0; iIndex < this->GetChildCount(); iIndex++ )
				{
				TreeNode* pTempNode = this->GetChild(iIndex);
				ASSERT(pTempNode);
				if ( (pTempNode->m_Tile.get_RenderableTileName() == _T("spacer")) /*&& (pTempNode->m_Tile.get_FunctionName() == _T("spacer_0"))*/
					/*|| (pTempNode->m_Tile.get_RenderableTileName() == _T("spacer")) && (pTempNode->m_Tile.get_FunctionName() == _T("spacer"))*/
					)
					{
					nSpacerOccupied += pTempNode->m_Tile.get_ScreenObject()->get_WidthToSet();
					nTotSpaces++;
					}
				else
					{
					/*if ( (pTempNode->m_Tile.get_RenderableTileName() == _T("spacer")) && (pTempNode->m_Tile.get_FunctionName() == _T("spacer_1")))
						nTotSpacer1++;*/
					nSumActChildWidth += pTempNode->m_Tile.get_ScreenObject()->get_WidthToSet() + pTempNode->get_DeltaX();
					nTotChild++;
					}
				}
			nRemainingWidth = nParentWidth - (nSumActChildWidth + nSpacerOccupied);
			// This code will change for each and every container.
			// So add only the new container case.
			if ( this->m_Tile.get_RenderableTileName() == _T("row") )
				{
				nSpacesTBDistributed = /*( nTotChild - 1) -*/ nTotSpaces;
				if ( nSpacesTBDistributed )
					{
					nWidthTBDistributed = nSpacesTBDistributed * SPACESXROWS;
					}	
				if ( nTotSpaces )
					nSpacerShare = (nWidthTBDistributed + nRemainingWidth)/nTotSpaces;
				/*if ( (nRemainingWidth - nWidthTBDistributed) < ( (nTotChild - 1)*SPACESXROWS ) )
					nSpacerShare = 0;*/
				}
			// Now set the nodes m_nHorSpaceShare.The children will refer to this.If the 
			// child lies after a spacer it should go forward that is move its left
			// coordinate ahead by m_nHorSpaceExpand.
				m_nHorSpaceExpand = nSpacerShare;
			}
		}

		
			return SDS_RTNORM;
		}
