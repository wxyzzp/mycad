/** FILENAME:     Tree.cpp- Contains implementation of class CTree.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
Abstract 
*        This class holds a single data member to the a root node of the tree.
*        Contains the basic algorithms for calculating,adjusting width/heights of the
*		 tiles and then the actual positions of their display area
*		 on the dialog with respect to the left corner of the dialog.
*Audit Log 
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "resource.h"
#include "tree.h"
#include "treenode.h"
#include "sds.h"
#include "Button.h"
#include "Dialog.h"
#include "constants.h"
#include "InactiveCtrl.h"
#include "editbox.h"
#include "listbox.h"
#include "popuplist.h"
#include "Text.h"
#include "spacer.h"
#include "slider.h"
#include "errormessage.h"
#include "Main.h"
#include "ExceptionManager.h"
#include "parser.h"
#include "imagebutton.h"



extern Main g_Main;
#ifdef _DEBUG
const bool Debug = TRUE;
#else
const bool Debug = FALSE;
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/************************************************************************************
/* + Tree::Tree() -  constructor of tree  class
*
*
*
*
*
*/
Tree::Tree(						//  constructor of tree  class
		   TreeNode *pTreeNode, //	i : Start Node.
		   int nTreeType		//	i : Type of the tree.
		   )  
	{
	m_startTreeNode = pTreeNode;
	m_nType         = nTreeType;
	IsdefaultSet = 0;
	m_nHasFixedWidthChild = 0;
	m_pCancelScrObj = NULL;
	//Modified SAU 12/07/2000 [
	//Reason :DCL does not handle multiple is_cancel attributes correctly
	Cancel_default=0;
    State=0;
	//Modified SAU 12/07/2000 ]
	}


/************************************************************************************
/* + Tree::~Tree() -  destructor of tree class
*
*
*
*
*
*/
Tree::~Tree()  // destructor of tree class
	{  
	ASSERT(m_startTreeNode);
    delete m_startTreeNode;
	}


/************************************************************************************
/* + int Tree::CalcWidthHeight()
*	The tree is 
*	traversed bottom to top and every parent's DisplayRect is set to
*	the summation of the width and height of all its children.
*	This function actually has the traversal logic and the actual
*	summation of widths and heights is done in LoopAllChildrenToCalcSize().
*/

int Tree::CalcWidthHeight()
	{
	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;    
	TreeNode *pTreeNode = NULL;
	try
		{
		pTreeNode = pOldNode = get_StartTreeNode();
		ASSERT( pOldNode);
	 
		bHasChildren  =  pOldNode->HasChild();                // check whether child is present
		if ( !bHasChildren )
			{
			return SDS_RTNORM;                                     // return ,if only one node in tree to be traversed
			}
		else
			{
			
			}
label:
		bHasChildren = pOldNode->HasChild();
		if ( bHasChildren )                           // yes the node has children
			{
			pOldNode = pOldNode->GetFirstChild();
			ASSERT( pOldNode );
			// Read the width and height from the ListOfAttributes.
			// Set the ScreenObjects DisplayRect(),m_nDisplayWidthToSet and m_nDisplayHeightToSet members.
			long lWidth = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Width();
			ASSERT (lWidth > 0);
			long lHeight = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Height();
			ASSERT (lHeight > 0);
 			pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,lWidth /*+ EXTRAWH*/,lHeight /*+ EXTRAHT*/);
			pOldNode->set_DeltaX(0);
			pOldNode->set_DeltaY(0);
			pOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(lWidth);
			pOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(lHeight);
			goto label;
			}
		else
			{
label1:
			TempOldNode = pOldNode;
			if ( pOldNode->get_Parent() != NULL )
				{
				pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
				}
			else
				{
				long lWidth = (long)TempOldNode->m_Tile.get_ListOfAttributes()->get_Width();
				ASSERT (lWidth > 0);
				long lHeight = (long)TempOldNode->m_Tile.get_ListOfAttributes()->get_Height();
				TempOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,lWidth,lHeight);
				ASSERT (lHeight > 0);
				TempOldNode->set_DeltaX(0);
				TempOldNode->set_DeltaY(0);
				TempOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(lWidth);
				TempOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(lHeight);
				pOldNode = NULL;
				}
			if ( pOldNode != NULL )
				{
				long lWidth = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Width();
				ASSERT (lWidth > 0);
				long lHeight = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Height();
				ASSERT (lHeight > 0);
				pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,lWidth,lHeight);
				pOldNode->set_DeltaX(0);
				pOldNode->set_DeltaY(0);
				pOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(lWidth);
				pOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(lHeight);
				goto label;
				}
			else
				{
				pOldNode = TempOldNode->get_Parent();
				ASSERT( pOldNode );
				LoopAllChildrenToCalcSize(pOldNode);
				if ( pOldNode == pTreeNode )
					{
					// Check whether the dialog fits on the screen.If it does not 
					// fit on the screen show an error message and return RTERROR
					CRect Dlg = pTreeNode->m_Tile.get_ScreenObject()->GetDisplayRect();
					Dlg.NormalizeRect();
					int nDlgDeltaX = pTreeNode->get_DeltaX();
					int nDlgDeltaY = pTreeNode->get_DeltaY();
					long DlgWidth = (long)Dlg.Width();
					long DlgHt = (long)Dlg.Height();
					if ( DlgHt < DLGMINHEIGHT )
						{
						pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,DlgWidth,DLGMINHEIGHT);
						DlgHt = DLGMINHEIGHT;
						}
					pTreeNode->m_Tile.get_ScreenObject()->set_HeightToSet(DlgHt);
					pTreeNode->m_Tile.get_ScreenObject()->set_WidthToSet(DlgWidth);
					HWND DskTop = GetDesktopWindow();
					RECT rectDskTop;
					::GetClientRect(DskTop,&rectDskTop);
					if ( ((DlgWidth + nDlgDeltaX) > (rectDskTop.right-rectDskTop.left))
						|| ((DlgHt + nDlgDeltaY) > (rectDskTop.bottom-rectDskTop.top)) )
						{	
						ErrorMessage errMessageBox;
						errMessageBox.Message(ID_DCL_DLGTOOLARGE);
						return SDS_RTERROR;
						}
					}
				else
					{
					goto label1;
					}
				}
			
			}
		}
	catch(...)
		{
		TRACE(_T("CalcWidthHeight failed..."));
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}

	/************************************************************************************
	/* + int Tree::AdjustWidthHeight()
	*	If there is any 
	*	extra area in the parent it is to be divided amongst the
	*	children.This function contains the traversal loop.It
	*	calls ReCalcDimensions() in which the actual extra space distibution is done.
	*
	*
	*/
	int Tree::AdjustWidthHeight()
	{
	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;  // holds previous value of pOldNode
	TreeNode *pTreeNode;
	try
		{
	     		 
		pTreeNode = pOldNode = m_startTreeNode ;                                 
     
		bHasChildren  =  pOldNode->HasChild();                // check whether child is present

		if(!bHasChildren)
			{
			return SDS_RTNORM;                                     // return ,if only one node in tree to be traversed
			}
		 else
			{
			  
			}

		while(1)
			{

			bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
        
			if (bHasChildren)
				{ 
		   		pOldNode = pOldNode->GetFirstChild();           // get the first child
				ReCalcDimensions(pOldNode);
				}// end of if(bHasChildren)
		   else
				{ 
				// if child not found go to the next sibling
		 		TempOldNode = pOldNode ;                      
				if ( pOldNode->get_Parent() != NULL )
					pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
				else
					pOldNode = NULL;

				 if (pOldNode != NULL)
					{
		     		ReCalcDimensions(pOldNode);
					}		
				else
					{
					// if sibling noit found go to parent
					pOldNode = TempOldNode;
					pOldNode = pOldNode->get_Parent();
									
					while (1)
						{//  to set next sibling after child level siblings have been traversed
					   
						if (pOldNode->get_Parent())
							TempOldNode = (pOldNode->get_Parent())->GetNextChild();
					   else
							TempOldNode = NULL;
               
						// keep moving up the tree till sibling is found
						if (TempOldNode != NULL)
							{
							pOldNode = TempOldNode;
							ReCalcDimensions(pOldNode);
							break;
							}
						else
							{
               
							// break if the traversing operation reaches where it started 
							if(pOldNode == pTreeNode)
								break;
                     					 
							pOldNode = pOldNode->get_Parent();
						 
							if(pOldNode == pTreeNode)
								break;
							}
						}// to set next sibling after chilren level siblings have been traversed--end	   

				   
					}// end check for has child

					if(pOldNode == pTreeNode)
						break;
				}
			} 
		}
	catch(...)
		{
		TRACE(_T("AdjustWidthHeight failed..."));
		return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}

	/************************************************************************************
	/* + int Tree::CalcPosition()
	*	
	*	Actual DisplayRects positions of the controls with respect to the left corner 
	*   of the dialog are calculated in this pass.
	*	This function contains the traversal loop.
	*	This function makes call to the PositionDisplayRect() function
	*	which actually calculates the positions.
	*/

	int Tree::CalcPosition()
	{
	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;							// holds previous value of pOldNode
	TreeNode *pTreeNode = NULL;
	try
		{
	     		 
		pOldNode = pTreeNode = m_startTreeNode;                                 
     
		bHasChildren  =  pOldNode->HasChild();                // check whether child is present

		if(!bHasChildren)
			{
			PositionDisplayRect(pOldNode);
			return SDS_RTNORM;                                     // return ,if only one node in tree to be traversed
			}
		else
			{
			PositionDisplayRect(pOldNode);
			}

		while(1)
			{

			bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
         
			if (bHasChildren)
				{ 
		   		pOldNode = pOldNode->GetFirstChild();           // get the first child
				PositionDisplayRect(pOldNode);
				}// end of if(bHasChildren)
		   else
				{ 
				// if child not found go to the next sibling
		 		TempOldNode = pOldNode ;                      
				if (pOldNode->get_Parent() != NULL)
					pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
				else
					pOldNode = NULL;

				if (pOldNode != NULL)
					{
					PositionDisplayRect(pOldNode);
					}		
				else
					{
					// if sibling noit found go to parent
					pOldNode = TempOldNode;
					pOldNode = pOldNode->get_Parent();
									
					while (1)
						{//  to set next sibling after child level siblings have been traversed
					   
						if (pOldNode->get_Parent())
							TempOldNode = (pOldNode->get_Parent())->GetNextChild();
						else
							TempOldNode = NULL;
               
						// keep moving up the tree till sibling is found
						if (TempOldNode != NULL)
							{
							pOldNode = TempOldNode;
							PositionDisplayRect(pOldNode);
							break;
							}
						else
							{
							// break if the traversing operation reaches where it started 
							if(pOldNode == pTreeNode)
								 break;
                     				 
							pOldNode = pOldNode->get_Parent();
						 
							if(pOldNode == pTreeNode)
								 break;
							}
						}// to set next sibling after chilren level siblings have been traversed--end	   

				   
					}// end check for has child

					if(pOldNode == pTreeNode)
						{
						//pOldNode->m_Tile.get_ScreenObject()->InflateDisplayRect(NL,NT,NR,NB);
						break;
						}
				}
			} 

		}
	catch(...)
		{
		TRACE(_T("CalcPosition failed..."));
		return SDS_RTERROR;
		}
	return SDS_RTNORM;

	}
	/************************************************************************************
	/* + int Tree::CreateOnScreenObjects ()
	*	Loop To create the OnScreen objects.
	*
	*	in this the tree is traversed from top to down and the CreateOnScreenObject 
	*	method on every screenobject in the tree is called which sreates the window associated 
	*	with the Object.
	*/

	int Tree::CreateOnScreenObjects()
	{
	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;                          // holds previous value of pOldNode
	TreeNode *pTreeNode = m_startTreeNode;
	CWnd * pWnd = NULL;
	     		 
	pOldNode = pTreeNode ;                                 
     
	bHasChildren  =  pOldNode->HasChild();                // check whether child is present

	if(!bHasChildren)
		{
		CWnd * pwnd= AfxGetApp( )->m_pMainWnd;
		pOldNode->m_Tile.get_ScreenObject()->CreateOnScreenObject(AfxGetApp( )->m_pMainWnd);
		return SDS_RTNORM;                                     // return ,if only one node in tree to be traversed
		}
	else
		{
		CWnd * pwnd= AfxGetApp( )->m_pMainWnd;
		pOldNode->m_Tile.get_ScreenObject()->CreateOnScreenObject(pwnd);				
		pWnd = pOldNode->m_Tile.get_ScreenObject()->GetWindow();
		ASSERT( pWnd ); 
		hDialog = pWnd->m_hWnd;
		}

	while(1)
		{

		bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
        
		if (bHasChildren)
			{ 
			pOldNode = pOldNode->GetFirstChild();           // get the first child
			pOldNode->m_Tile.get_ScreenObject()->CreateOnScreenObject(pWnd);				
			}// end of if(bHasChildren)
		else
			{ 
			// if child not found go to the next sibling
			TempOldNode = pOldNode ;                      
			if (pOldNode->get_Parent() != NULL)
				pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
			else
				pOldNode = NULL;

			 if (pOldNode != NULL)
				{
				pOldNode->m_Tile.get_ScreenObject()->CreateOnScreenObject(pWnd);				
				}		
			else
				{
				// if sibling noit found go to parent
				pOldNode = TempOldNode;
				pOldNode = pOldNode->get_Parent();
								
				while (1)
					{//  to set next sibling after child level siblings have been traversed
				   
					if (pOldNode->get_Parent())
						TempOldNode = (pOldNode->get_Parent())->GetNextChild();
					else
						TempOldNode = NULL;
               
					// keep moving up the tree till sibling is found
					if (TempOldNode != NULL)
						{
						pOldNode = TempOldNode;
						pOldNode->m_Tile.get_ScreenObject()->CreateOnScreenObject(pWnd);				
						break;
						}
					else
						{
                 
						// break if the traversing operation reaches where it started 
						if(pOldNode == pTreeNode)
							break;
                     					 
						pOldNode = pOldNode->get_Parent();
					 
						if(pOldNode == pTreeNode)
							break;
						}
					}// to set next sibling after chilren level siblings have been traversed--end	   

			   
				}// end check for has child

				if(pOldNode == pTreeNode)
					{
					//g_Main.m_nObjectsCreated = 1;
					break;
					}

			}
		} 

	return SDS_RTNORM;
	}
	//Modified SAU 12/07/2000 [
	//Reason:DCL does not handle multiple is_cancel attributes correctly			
	/*********************************************************************
   /* ScreenObject* Tree::GetCancelScrObj()
	* This function is used to GET which button(screen object) is pressed
	* when the user presses the cancel key (ESC). 
	*/
	ScreenObject* Tree::GetCancelScrObj()
	{
			  return m_pCancelScrObj;
	}
	//Modified SAU 12/07/2000 ]
	/*******************************************************************
	/* + int Tree::set_TreeType(int nType) 
	*	Sets the tree type.
	*   0 - dialog.
	*	1 - prototype.
	*
	*
	*/
	int Tree::set_TreeType(					// Sets the tree type.
					  int nType				// i: 0 for dialog , 1 for prototype/sub-assembly
					  )
	{
	m_nType = nType;
	return SDS_RTNORM;
	}
	/************************************************************************************
	/* + int Tree::get_TreeType() 
	*	Gets the tree type.
	*
	*
	*
	*
	*/
	int Tree::get_TreeType()          // Gets the tree type, returns 
	{								 // 0 for dialog , 1 for prototype/sub-assembly
	return (m_nType); 
	}


	/************************************************************************************
	/* + int Tree::set_StartTreeNode(TreeNode *pTreeNode)
	*
	*     Sets the m_startTreeNode to the value of pTreeNode.This datamember 
	*  points to the root node of a definition.
	*
	*
	*/
	int Tree::set_StartTreeNode(						// Set the Start TreeNode.
							TreeNode *pTreeNode		//	i : StartTreeNode.
							)
	{
	ASSERT(pTreeNode);
	m_startTreeNode  = pTreeNode ;
	return SDS_RTNORM;
	}

	/************************************************************************************
	/* + TreeNode * Tree::get_StartTreeNode() 
	*
	*     Returns the pointer to the root node the m_startTreeNode is pointing to.
	*
	*
	*
	*/
	TreeNode * Tree::get_StartTreeNode()   // Returns a pointer to the root node.
	{
	return (m_startTreeNode);
	}
	/************************************************************************************
	/* + int Tree::Init()
	*
	*	This method creates screenobject  but does not create the window associated with it.
	*   the tree it traversed from top to down and depending upon the tilename the objects are created
	*
	*
	*/

	int Tree::Init()
	{

	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;                          // holds previous value of pOldNode


	// code commented by pankaj 9 june 99
	/*	if(!g_Main.m_keytoObject.IsEmpty())
		{
		g_Main.m_keytoObject.RemoveAll();
		}*/
	pOldNode = m_startTreeNode ;                                 
     
	bHasChildren  =  pOldNode->HasChild();                // check whether child is present

	if(!bHasChildren)
		{
		CreateTreeNodeScreenObj(pOldNode);           // create screen object
		return SDS_RTNORM;                                     // return ,if only one node in tree to be traversed
		}
	else
		{
		CreateTreeNodeScreenObj(pOldNode);             // create screen object
		}

	while(1)
		{

		bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
       
		if (bHasChildren)
			{ 
		   
			pOldNode = pOldNode->GetFirstChild();           // get the first child
			CreateTreeNodeScreenObj(pOldNode);                // create screen object
		
			 }// end of if(bHasChildren)
		else
			{ 
			// if child not found go to the next sibling
			TempOldNode = pOldNode ;                      
			if (pOldNode->get_Parent() != NULL)
				pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
			else
				pOldNode = NULL;

			if (pOldNode != NULL)
				{
				CreateTreeNodeScreenObj(pOldNode);
				}		
			else
				{
				// if sibling noit found go to parent
				pOldNode = TempOldNode;
				pOldNode = pOldNode->get_Parent();
								
				while (1)
					{//  to set next sibling after child level siblings have been traversed
				   
					if (pOldNode->get_Parent())
						TempOldNode = (pOldNode->get_Parent())->GetNextChild();
					else
						TempOldNode = NULL;
               
					// keep moving up the tree till sibling is found
					if (TempOldNode != NULL)
						{
						pOldNode = TempOldNode;
						CreateTreeNodeScreenObj(pOldNode);		
						// call new for all screen object
						break;
						}
					else
						{
                 
						// break if the traversing operation reaches where it started 
						if(pOldNode == m_startTreeNode)
							break;
                     					 
						 pOldNode = pOldNode->get_Parent();
					 
						if(pOldNode == m_startTreeNode)
							break;
						}
					}// to set next sibling after chilren level siblings have been traversed--end	   

			   
				}// end check for has child

				if(pOldNode == m_startTreeNode)
					break;
			}
		}
	if (IsdefaultSet == 0)
		{
			ErrorMessage error;
			error.Message(CString("Dialog has neither an OK nor a CANCEL button"));
			return SDS_RTERROR;
		}
	return SDS_RTNORM;
	}
	/************************************************************************************
	/* + int Tree::CreateTreeNodeScreenObj(TreeNode *pNode)
	*	Just call "new" for every Node and construct the object.
	*	This is the first thing to be done.
	*	Then the 3 loops of the algorithm.
	*	Finally call CreateOnScreenObjects which calss creat on every object.
	*
	*/

int Tree::CreateTreeNodeScreenObj(					// Constructs the respective object.
							  TreeNode *pNode	// i : Input node.
							  )
{
	CString TileName;
	int Parent_layout ;
	// while creating the screen object is parent layout is set in the constructor which is used in positioncontrol method of screenobject
	// the pointer to list of attributes in the screenobject is initalised after the object is created
	try
	{
		TileName = pNode->m_Tile.get_RenderableTileName();
		
		if ( pNode->m_Tile.get_RenderableTileName().IsEmpty() )
		{
			int nRes = pNode->IsNodeACluster();
			if ( nRes )		// Yes the node is a cluster or something from a cluster
			{
				SetRenderableTileNameForCluster(pNode,nRes);
			}
			TileName = pNode->m_Tile.get_RenderableTileName();
		} 

		if(TileName == _T("button"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject (new Button(PUSHBUTTON,Parent_layout ));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			
			// check for default button in dialog
			if (pNode->m_Tile.get_ListOfAttributes()->IsDefault() 
				|| pNode->m_Tile.get_ListOfAttributes()->IsCanceled() )
				IsdefaultSet= 1;

			// This code is used to set the last button in the dialog box to default or cancel.

			if (pNode->m_Tile.get_ListOfAttributes()->IsCanceled()) 
			{
				
				//Modified SAU 12/07/2000 [
				//Reason :DCL does not handle multiple is_cancel attributes correctly
	
				if(!(pNode->m_Tile.get_ListOfAttributes()->IsDefault())
					&& (Cancel_default==1 || m_pCancelScrObj==NULL))
				{
					m_pCancelScrObj=pNode->m_Tile.get_ScreenObject();
					Cancel_default=0;  
					State=1;  
				}
				else if(pNode->m_Tile.get_ListOfAttributes()->IsDefault()
					&& (State==0 || m_pCancelScrObj==NULL))
				{
					m_pCancelScrObj=pNode->m_Tile.get_ScreenObject();
					Cancel_default=1;
				}
				pNode->m_Tile.get_ScreenObject()->set_Cancel(TRUE);
			    //Modified SAU 12/07/2000 ]
			}
		}
		else if(TileName == _T("dialog"))
		{
			pNode->m_Tile.set_ScreenObject( new Dialog());
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			//**********************************************************//
			// code added by pankaj 9 june 99 for mapping dialog to a map of key to screen object
			// the ID is the actual address of the Dialog object
			pDialogAddress.Format("%ld",pNode->m_Tile.get_ScreenObject());
			((Dialog *)pNode->m_Tile.get_ScreenObject())->DialogID = pDialogAddress;
			m_keytoObject = new CMapStringToPtr();

			CString keyname;
			keyname.Format("%ld",atoi((LPCTSTR)pDialogAddress));

			g_Main.m_hdlgtoDialog.SetAt ((LPCTSTR)keyname/*(LPCTSTR)pDialogAddress*/,m_keytoObject);
			g_Main.m_hdlgtoTree.SetAt  ((LPCTSTR)keyname /*(LPCTSTR)pDialogAddress*/,this);
			//**********************************************************//
		}
		else if(TileName == _T("boxed_column"))
		{
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PCOLUMN,BOXED));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("boxed_radio_column"))
		{
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PCOLUMN,BOXED,RADIO));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("boxed_radio_row"))
		{
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW,BOXED,RADIO));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("boxed_row"))
		{
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW,BOXED));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("column"))
		{
			CString label = pNode->m_Tile.get_ListOfAttributes()->get_Label();
			if (label.IsEmpty ())
			{
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PCOLUMN));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
			else
			{
				pNode->m_Tile.set_TileName(CString("boxed_column"));
				pNode->m_Tile.set_RenderableTileName(CString("boxed_column"));
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PCOLUMN,BOXED));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
		}
		else if(TileName == _T("row"))
		{
			CString label = pNode->m_Tile.get_ListOfAttributes()->get_Label();
			if (label.IsEmpty ())
			{
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
			else
			{
				pNode->m_Tile.set_TileName (CString("boxed_row"));
				pNode->m_Tile.set_RenderableTileName (CString("boxed_row"));
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW,BOXED));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
		}
		else if(TileName == _T("radio_column"))
		{
			CString label = pNode->m_Tile.get_ListOfAttributes()->get_Label();
			if ( label.IsEmpty() )
			{
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PCOLUMN));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
			else
			{
				pNode->m_Tile.set_TileName (CString("boxed_radio_column"));
				pNode->m_Tile.set_RenderableTileName (CString("boxed_radio_column"));
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PCOLUMN,BOXED));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
		}
		else if(TileName == _T("radio_row"))
		{
			CString label = pNode->m_Tile.get_ListOfAttributes()->get_Label();
			if ( label.IsEmpty() )
			{
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
			else
			{
				pNode->m_Tile.set_RenderableTileName (CString("boxed_radio_row"));
				pNode->m_Tile.set_TileName (CString("boxed_radio_row"));
				pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW,BOXED));
				pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
			}
		}
		else if(TileName == _T("concatenation"))
		{
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW,UNBOXED,NOTRADIO,1));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("paragraph"))
		{
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW,UNBOXED,NOTRADIO,0,1));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("edit_box"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new EditBox(Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("popup_list"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new PopUpList(Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("list_box"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new ListBox(Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("radio_button"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Button(RADIOBUTTON, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("toggle"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Button(CHECKBOX, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if (TileName == _T("image"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new ImageButton(IMAGE_TILE, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if (TileName == _T("image_button"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new ImageButton(IMAGEBUTTON_TILE, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("errtile"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Text(PERRTILE, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}	
		else if(TileName == _T("text"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Text(PTEXT, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("text_part"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Text(PTEXTPART, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("slider"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Slider(Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("spacer"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Spacer(PSPACER, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("spacer_0"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Spacer(PSPACER0, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else if(TileName == _T("spacer_1"))
		{
			Parent_layout= pNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Layout();
			pNode->m_Tile.set_ScreenObject( new Spacer(PSPACER1, Parent_layout));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}
		else
		{
			// does nothing 
			pNode->m_Tile.set_ScreenObject( new InactiveCtrl(PROW));
			pNode->m_Tile.get_ScreenObject()->m_attriblist = pNode->m_Tile.get_ListOfAttributes() ;
		}

		// an entry in the map of key to screenobject is  is made here
		// while adding a new entry we check of key redifination 
		CString Key = pNode->m_Tile.get_ListOfAttributes()->get_Key() ; 
		void * temp ;
		Key.TrimLeft ();
		if (!Key.IsEmpty ())
		{
			if(! m_keytoObject->Lookup(Key , temp))
				m_keytoObject->SetAt ( Key ,(void*) pNode->m_Tile.get_ScreenObject());
			else
			{
				CString Error;
				Error.Format("%s : redefining key", Key);
				AfxMessageBox(Error);
			}
		}
	}
	catch(...)
	{
		ThrowCExceptionManager(MEM_ALLOC_FAILED);
	}

	return SDS_RTNORM;

}

	/************************************************************************************
	/* + int Tree::LoopAllChildrenToCalcSize(TreeNode *pParentNode) 
	*	This function is called in CalcWidthHeight.Actual work of summing
	*	all childrens width/height depending on the layout of the parent
	*	is done in this.At the end maximum(SummationofChildren,AssignedDClValue)
	*	is taken and the parents DisplayRect is set to this size.Also the extra spacing
	*	to keep spaces between children is done here and thus deltaX and deltaY are initialised.
	*/

	int Tree::LoopAllChildrenToCalcSize(						// Loop all the children to find parents height and width.
									TreeNode *pParentNode	//	i : Parent Node.
									)
	{	
	TreeNode *pTempNode;						// A holder node.
	int nMaxHeight/* maximum height*/ = 0,nMaxWidth = 0/* maximum width*/;
	int nAggregateHeight = 0/*Aggregated height of the children*/,nAggregateWidth = 0/*Aggregated Width of the children*/;
	int nParentWidth/*Parents ScreenObjects rect width*/ = 0,nParentHeight/*Parents ScreenObjects rect height*/ = 0;
	int nParentAssignedWidth = 0/* Parents width as assigned in the dcl file*/,nParentAssignedHeight = 0,nChildAssignedWidth = 0,nChildAssignedHeight = 0;
	int iCount = 0;
	int i = 0;
	int nChildSummationAsWidth = 0,nChildSummationAsHeight = 0;
	ASSERT( pParentNode );
	CRect nParentRect;
	nParentRect = pParentNode->m_Tile.get_ScreenObject()->GetDisplayRect();
	nParentWidth = nParentRect.Width();
	nParentHeight = nParentRect.Height();
	nParentAssignedWidth = pParentNode->m_Tile.get_ListOfAttributes()->get_Width();
	nParentAssignedHeight = pParentNode->m_Tile.get_ListOfAttributes()->get_Height();
	BOOL bSetFirstRadioBtn = FALSE;				// If the First radio button's m_nIsFirstRadioBtn is set , then set bSetFirstRadioBtn to TRUE.


	int nSpaceChildren = 0;
	// DeltaXToSet and DeltaYToSet contain the x-direction and y-direction increment for that tile.
	int nDeltaXToSet = 0, nDeltaYToSet = 0;
	int nDefaultDeltaX;
	int nDefaultDeltaY;
	int nTempChildDeltaX,nTempChildMaxDeltaX = 0;
	int nTempChildDeltaY,nTempChildMaxDeltaY = 0;
	CString strThisNodesParent;
	
	strThisNodesParent = pParentNode->m_Tile.get_RenderableTileName();
	// Initialise the default deltaX and deltaY depending on the tile.
	if ( (strThisNodesParent == _T("row")) || (strThisNodesParent == _T("column")) )
		{
		nDefaultDeltaX = 0;
		nDefaultDeltaY = 0;
		}
	else if ( (strThisNodesParent == _T("dialog")) )
		{
		nDefaultDeltaX = 2 * SPACESX;
		nDefaultDeltaY = 2 * SPACESY;
		}
	else if ( (strThisNodesParent == _T("boxed_row")) )
		{
		nDefaultDeltaX = 2 * SPACESXBOXROW;
		nDefaultDeltaY = SPACESYBOXROWTOP + SPACESYBOXROWBOTTOM;
		}
	else if ( (strThisNodesParent == _T("boxed_radio_row")) )
		{
		nDefaultDeltaX = 2 * SPACESXBOXRADIOROW;
		nDefaultDeltaY = SPACESYBOXRADIOROWTOP + SPACESYBOXRADIOROWBOTTOM;
		}
	else if ( (strThisNodesParent == _T("boxed_column")) )
		{
		nDefaultDeltaX = 2 * SPACESXBOXCOL;
		nDefaultDeltaY = SPACESYBOXCOLTOP + SPACESYBOXCOLBOTTOM;
		}
	else if ( (strThisNodesParent == _T("boxed_radio_column")) )
		{
		nDefaultDeltaX = 2 * SPACESXBOXRADIOCOL;
		nDefaultDeltaY = SPACESYBOXRADIOCOLTOP + SPACESYBOXRADIOCOLBOTTOM;
		}
	else if ( (strThisNodesParent == _T("radio_row")) )
		{
		nDefaultDeltaX = 0;
		nDefaultDeltaY = 0;
		}
	else if ( (strThisNodesParent == _T("radio_column")) )
		{
		nDefaultDeltaX = 0;
		nDefaultDeltaY = 0;
		}
	else if ( (strThisNodesParent == _T("paragraph")) )
		{
		nDefaultDeltaX = 0;
		nDefaultDeltaY = 0;
		}
	else if ( (strThisNodesParent == _T("concatenation")) )
		{
		nDefaultDeltaX = 0;
		nDefaultDeltaY = 0;
		}
	else
		{
		nDefaultDeltaX = 2 * SPACESX;
		nDefaultDeltaY = SPACESYFIRSTNONDLGCHILD + SPACESYOTHERNONDIALOG;
		}
	// Find the layout of the tile.
	int iLayOut = pParentNode->m_Tile.get_ListOfAttributes()->get_Layout();
	// Total number of children this tile has got.
	iCount = pParentNode->GetChildCount();
	int iSwatches = 0;
	int nSlider = 0;
	CString strTile;
	if ( iCount > 0 )
		{
		switch (iLayOut)
			{
			case LAYOUT_VERTICAL :

				for ( i = 0; i < iCount; i++ )
					{
					CRect lRect;
					pTempNode = pParentNode->GetChild(i);
					ASSERT( pTempNode );
					
					if ( (pTempNode->m_Tile.get_RenderableTileName() == _T("image_button") ) || (pTempNode->m_Tile.IsContainer() == TRUE) )  
						{
						int nRes = pTempNode->IsNodeASwatch();
						if ( nRes == 1 )
							iSwatches++;
						}
					nTempChildDeltaX = pTempNode->get_DeltaX();
					if (pTempNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE)
						{
						m_nHasFixedWidthChild = 1;
						}
					nTempChildDeltaY = pTempNode->get_DeltaY();
					nTempChildMaxDeltaY = nTempChildMaxDeltaY + nTempChildDeltaY;
					lRect = pTempNode->m_Tile.get_ScreenObject()->GetDisplayRect();
					if ( (nMaxWidth + nTempChildMaxDeltaX) < (lRect.Width() + nTempChildDeltaX) )
						{
						nMaxWidth = __max(lRect.Width(),nMaxWidth);			// store the last best pair whose (width + deltax) is maximum
						nTempChildMaxDeltaX = nTempChildDeltaX;
						}
					nAggregateHeight = nAggregateHeight + lRect.Height();
					
					strTile = pTempNode->m_Tile.get_RenderableTileName();
					// If a parent has number of radio_buttons and all of them have a value set to 1,
					// only the last radio_button in each parent should be checked.
					// This function call sets the appropriate values which are
					// checked before setting Check on the radio_button.
					if ( (strTile == _T("radio_button")) )
						{
						pTempNode->CheckItorNot();
						}
					if ( (strTile == _("radio_button")) && (bSetFirstRadioBtn == FALSE ))
						{
						pTempNode->m_Tile.get_ScreenObject()->m_nIsFirstRadioBtn = 1;
						bSetFirstRadioBtn = TRUE;
						}

					pTempNode->m_Tile.get_ScreenObject()->set_ColumnTile(FIRSTTILE);
					// Adjustment for spaces is done here.
					if ( (strTile == _T("spacer")) || (strTile == _T("spacer_0")) || (strTile == _T("spacer_1")) )
						nSpaceChildren++;
					if ( i == (iCount-1) )
						{
						pTempNode->m_Tile.get_ScreenObject()->set_ColumnTile(LASTTILE);
						nMaxWidth = __max(nMaxWidth + nTempChildMaxDeltaX,nParentAssignedWidth);
						nAggregateHeight = __max(nAggregateHeight + nTempChildMaxDeltaY ,nParentAssignedHeight);
						
						nDeltaXToSet = __max(nDefaultDeltaX,nDefaultDeltaX);
						if ( strThisNodesParent == _T("dialog") )
							{
							nDeltaXToSet =  nDefaultDeltaX;
							nDeltaYToSet = (1 * SPACESY) + ((((iCount - nSpaceChildren)-nSpaceChildren) -1) * SPACESYTILES) + SPACESYOK;
							}
						else if ( (strThisNodesParent == _T("column")) )
							{
							nDeltaYToSet =  ( (iCount - nSpaceChildren) - 1 )*SPACESYCOLUMN;
							}
						else if ( (strThisNodesParent == _T("radio_column")) )
							{
							nDeltaYToSet = ( (iCount - nSpaceChildren) - 1 )*SPACESYRADIOCOLUMN;
							}
						else if ( strThisNodesParent == _T("boxed_column") )
							{
							nDeltaXToSet = nDefaultDeltaX ;
							nDeltaYToSet = ( (iCount - nSpaceChildren) - 1 )*SPACESYBOXCOLBETWEEN + SPACESYBOXCOLTOP + SPACESYBOXCOLBOTTOM;
							}
						else if ( strThisNodesParent == _T("boxed_radio_column") )
							{							
							nDeltaYToSet = ( (iCount - nSpaceChildren) - 1 )*SPACESYBOXRADIOCOLBETWEEN + SPACESYBOXRADIOCOLTOP + SPACESYBOXRADIOCOLBOTTOM;
							}
						else if ( strThisNodesParent == _T("paragraph") )
							{
							nDeltaYToSet = nDefaultDeltaY ;
							}
						else
							{
							nDeltaYToSet = (1 * SPACESYFIRSTNONDLGCHILD) + (iCount * SPACESYOTHERNONDIALOG );
							}
						if ( iSwatches == iCount )
							{
							nDeltaYToSet = nDefaultDeltaY;
							pParentNode->m_nAllSwatches = 1;
							}
						if ( nDeltaYToSet < 0 )
							nDeltaYToSet = 0;
						if ( nDeltaXToSet < 0 )
							nDeltaXToSet = 0;
						// Set the nodes deltaX,deltaY,DisplayRect,m_nDisplayWidthToSet,m_nDisplayHeight.
						// Deltas are not added to the width,height because they should not be considered in calculations to be done ahead.
						pParentNode->set_DeltaX(nDeltaXToSet);
						pParentNode->set_DeltaY(nDeltaYToSet);
						
						pParentNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,nMaxWidth,nAggregateHeight);
						pParentNode->m_Tile.get_ScreenObject()->set_WidthToSet(nMaxWidth);
						pParentNode->m_Tile.get_ScreenObject()->set_HeightToSet(nAggregateHeight);
						}
					}
				break;
				
			case LAYOUT_HORIZONTAL :
					
					for ( i = 0; i < iCount; i++ )
						{	
						
						CRect lRect;
						pTempNode = pParentNode->GetChild(i);

						ASSERT( pTempNode );
						if ( pTempNode->m_Tile.get_RenderableTileName() == _T("image_button") || (pTempNode->m_Tile.IsContainer() == TRUE))
							{
							int nRes = pTempNode->IsNodeASwatch();
							if ( nRes == 1 )
								iSwatches++;
							}
						nTempChildDeltaX = pTempNode->get_DeltaX();
						
						if (pTempNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE)
							{
							m_nHasFixedWidthChild = 1;
							}
						nTempChildMaxDeltaX = nTempChildMaxDeltaX + nTempChildDeltaX;
						
						nTempChildDeltaY = pTempNode->get_DeltaY();
						nTempChildMaxDeltaY = __max(nTempChildMaxDeltaY,nTempChildDeltaY);
						lRect = pTempNode->m_Tile.get_ScreenObject()->GetDisplayRect();
						if ( (lRect.Height() + nTempChildDeltaY) > (nMaxHeight + nTempChildMaxDeltaY))
							{
							nMaxHeight = lRect.Height();            // find the tallest combination within children.
							nTempChildMaxDeltaY = nTempChildDeltaY;
							}
						
						nAggregateWidth = nAggregateWidth + lRect.Width();
						
						strTile = pTempNode->m_Tile.get_RenderableTileName();
						// If a parent has number of radio_buttons and all of them have a value set to 1,
						// only the last radio_button in each parent should be checked.
						// This function call sets the appropriate values which are
						// checked before setting Check on the radio_button.
						if ( (strTile == _T("slider")) )
							{
							nSlider++;
							}
						if ( (strTile == _T("radio_button")) )
							{
							pTempNode->CheckItorNot();
							}
						if ( (strTile == _("radio_button")) && (bSetFirstRadioBtn == FALSE) )
							{
							pTempNode->m_Tile.get_ScreenObject()->m_nIsFirstRadioBtn = 1;
							bSetFirstRadioBtn = TRUE;
							}
						pTempNode->m_Tile.get_ScreenObject()->set_ColumnTile(FIRSTTILE);
						if ( (strTile == _T("spacer")) || (strTile == _T("spacer_0")) || (strTile == _T("spacer_1")) )
							nSpaceChildren++;
						if ( i == (iCount-1) )
							{
							pTempNode->m_Tile.get_ScreenObject()->set_ColumnTile(LASTTILE);
							nAggregateWidth = __max(nAggregateWidth+nTempChildMaxDeltaX ,nParentAssignedWidth);
							nMaxHeight = __max(nMaxHeight + nTempChildMaxDeltaY ,nParentAssignedHeight);
							nDeltaYToSet = __max(nDefaultDeltaY, nDefaultDeltaY);
							if ( (strThisNodesParent == _T("row")) || (strThisNodesParent == _T("radio_row")) )
								{
								nDeltaXToSet = ( (iCount-nSpaceChildren) - 1) * SPACESXROWS + (nSpaceChildren * SPACERX);
								if ( (iCount-nSpaceChildren) == (nSpaceChildren + 1) )
									{
									nDeltaXToSet = 0;
									}
								CString strFunctionalParentOfThis = pParentNode->get_Parent()->m_Tile.get_FunctionName();
								if ( (strFunctionalParentOfThis == _T("ok_cancel")) || (strFunctionalParentOfThis == _T("ok_cancel_help"))
									|| (strFunctionalParentOfThis == _T("ok_cancel_help_info")) || (strFunctionalParentOfThis == _T("ok_cancel_help_errtile"))
									|| (strFunctionalParentOfThis == _T("ok_only")) || (strFunctionalParentOfThis == _T("ok_cancel_err"))
									)
									{
									nDeltaXToSet = 0;
									}
								}
							else if ( (strThisNodesParent == _T("radio_row")) )
								{
								nDeltaXToSet = 0;
								}
							else if ( (strThisNodesParent == _T("boxed_row")) )
								{
								nDeltaXToSet = ( (iCount-nSpaceChildren) + 1) * SPACESXBOXROW  + (nSpaceChildren * SPACERX) ;
								}
							else if ( (strThisNodesParent == _T("boxed_radio_row")) )
								{
								nDeltaXToSet = ( (iCount - nSpaceChildren) + 1) * SPACESXBOXRADIOROW + (nSpaceChildren * SPACERX);
								}
							else if ( strThisNodesParent == _T("concatenation") )
								{
								nDeltaXToSet = nDefaultDeltaX;
								}
							else
								{
								nDeltaXToSet = ((iCount - nSpaceChildren) + 1) * SPACESX + (nSpaceChildren * SPACERX);
								}
							if ( iCount == iSwatches )
								{
								nDeltaXToSet = nDefaultDeltaX;
								pParentNode->m_nAllSwatches = 1;
								}
							if ( nDeltaYToSet < 0 )
								nDeltaYToSet = 0;
							if ( nDeltaXToSet < 0 )
								nDeltaXToSet = 0;
							nDeltaXToSet += (nSlider*SPACESX);
							// Set the nodes deltaX,deltaY,DisplayRect,m_nDisplayWidthToSet,m_nDisplayHeightToSet.
							// Deltas are not added to width or height because thety should not be there in calculations.
							pParentNode->set_DeltaX(nDeltaXToSet);
							pParentNode->set_DeltaY(nDeltaYToSet);
							pParentNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,nAggregateWidth,nMaxHeight);	
							pParentNode->m_Tile.get_ScreenObject()->set_WidthToSet(nAggregateWidth);
							pParentNode->m_Tile.get_ScreenObject()->set_HeightToSet(nMaxHeight);
							}
						}
					break;
			default : 
					ASSERT(FALSE);
			}
		
		}
	return SDS_RTNORM;
	}

	/************************************************************************************
	/* + int Tree::ReCalcDimensions(TreeNode *pOldNode)
	*	Actual distribution of extra parent area is done here.
	*	Depending on whether the node is a container and the layout
	*	of the parent,the division of the appropriate entity ie width or height is done.
	*	While deciding on which child gets how much extra space we need to take into
	*	consideration the ratio of the childs width to the summation of all sibling widths or
	*	the ratio of the childs height to the summation of all sibling heights depending the layout of the parent.
	*   If it is first child go into the loop to calculate SummatedChildWidths 
	*   or SummatedChildHeights depending on parent layout and set the Parents m_nSummatedWidth,m_nSummatedHeight members so that the reaining children use that value.
	*   (This has been done because the first tiles Width/height changes when the next childs width/height is calculated which leads to inconsistencies.)
	*/

	int Tree::ReCalcDimensions(							// Function where redistribution of extra parent space is done.
						   TreeNode *pOldNode		//	i : Node for whom the calculation is to be done.
						   )
	{
	ASSERT( pOldNode );
	int iTotalChildren = 0;/*Count of the siblings*/
	int nWidth = 0,nHeight = 0; /*These values are set and used at last*/
	int nAggregateHeight = 0,nAggregateWidth = 0;
	int iParentLayOut/*Layout of the parent*/;
	CRect lRect/*Parents display rect*/,lChildRect/*Childs display rect*/;
	int nParentHeight/*Height of parents display rect*/ = 0,nParentWidth/*Width of parents display rect*/ = 0;
	int nThisChildHeight/*Height of this node's display rect*/ = 0, nThisChildWidth/*Width of this node's display rect*/ = 0;
	TreeNode* ptrParentNode/*ParentNode*/ = NULL;
	TreeNode* ptrThisNode/*This node*/ = pOldNode;
	TreeNode *pHolderNode = NULL;
	int nSummatedChildHeight/*Summation of all siblings heights from List of attributes*/ = 0, nSummatedChildWidth/*Summation of all siblings widths from List of attributes*/ = 0; // These will be denominators in the factor deciding who gets how much extra space of parent.
	int nActualChildHeight/*Height of the node from List of Attributes*/ = 0, nActualChildWidth/*Width of the node from List of Attributes*/ = 0;	// These will be numerators in the factor deciding who gets how much extra space.
	BOOL bSetDirect = FALSE;
	ptrParentNode = ptrThisNode->get_Parent();
	ASSERT( ptrParentNode );
	int j = 0;
	int iIndexToRestore = ptrParentNode->get_LastAccessedChild();
	nParentHeight = ptrParentNode->m_Tile.get_ScreenObject()->get_HeightToSet();
	nParentWidth = ptrParentNode->m_Tile.get_ScreenObject()->get_WidthToSet();
	ASSERT( nParentHeight );
	ASSERT( nParentWidth );
	iTotalChildren = ptrParentNode->GetChildCount();
	iParentLayOut = ptrParentNode->m_Tile.get_ListOfAttributes()->get_Layout();
	int nWidthToDistribute = 0;int nHeightToDistribute = 0;
	pHolderNode = ptrParentNode->GetChild(iTotalChildren - 1);
	ASSERT( pHolderNode );
	int nLastsWidth = 0, nLastsHeight = 0;
	BOOL bGiveLastWidth = TRUE;
	BOOL bGiveLastHeight = TRUE;
	int iIndex = ptrParentNode->GetChildIndex(ptrThisNode);
	// Get the RenderableTileName of this node.
	CString strTile = ptrThisNode->m_Tile.get_RenderableTileName();
	// Get the FunctionName of this node.
	CString strFunction = ptrThisNode->m_Tile.get_FunctionName();
	// If it is a spacer,text,ok_xxx subassembly don't go into calculation loop.
	// Directly set the width,height to whatever it is.
	
	if ( (strTile == _T("spacer")) || (strTile == _T("spacer_0")) || (strTile == _T("spacer_1")) )
		{
		nWidth = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Width();
		nHeight = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Height();
		bSetDirect = TRUE;
		}
	/*
	if ( (strTile == _T("text_part")) || (strTile == _T("text")) || (strTile == _T("errtile")) )
		{
		nWidth = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Width();
		nHeight = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Height();
		bSetDirect = TRUE;
		}*/
	if ( (strFunction == _T("ok_cancel")) || (strFunction == _T("ok_only"))
		|| (strFunction == _T("ok_cancel_help_errtile")) || (strFunction == _T("ok_cancel_err"))
		|| (strFunction == _T("ok_cancel_help")) || (strFunction == _T("ok_cancel_help_info")))
		{
		nWidth = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Width();
		nHeight = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Height();
		bSetDirect = TRUE;
		}
	if ( bSetDirect == FALSE )
		{
		// Some calculations are necessary if it is last child.
		// They are done below.
		//Condition : last child in a horizontal container has fixed_width true.
		//Condition : last child in a vertical container has fixed_height true.
		if ( iIndex == (iTotalChildren - 1) )
			{
			if ( iParentLayOut == LAYOUT_HORIZONTAL )
				{
				// Parent is a horizontal container.
				// Check for fixed_width=true.
				// If fixed_width is true directly set the width/height.(Height depends on whether fh is true or false)
				// If fixed_width is false go in for normal width distribution.
				// Whenever fixed_height is true do not forget to add deltaY to nHeight because while setting Height at the end of this function
				// deltaY is again subtracted.
				// If the tile is an active tile give all it all that parent has.Do not check for 
				// fixed_width and fixed_height of active tiles.
				if ( ptrThisNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE )
					{
					nWidth = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Width() ;
					if ( ptrThisNode->m_Tile.IsContainer() == TRUE )
						{
						if ( ptrThisNode->m_Tile.get_ListOfAttributes()->IsFixedHeight() == TRUE )
							{
							nHeight = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Height() + ptrThisNode->get_DeltaY();
							}
						else
							{
							nHeight = nParentHeight ;
							}
						}
					else
						{
						nHeight = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Height();
						}
					bSetDirect = TRUE;
					}
				}
			else	// Parent is a vertical container.
				{
				// Check for fixed_height=true.
				// If fixed_height is true directly set the width/height.(Width depends on whether fw is true or false)
				// If fixed_height go in for normal distribution.
				if ( ptrThisNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE )
					{
					nWidth = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Width() ;
					}
				else
					{
					nWidth = nParentWidth - ptrThisNode->get_DeltaX();
					}
				if ( ptrThisNode->m_Tile.get_ListOfAttributes()->IsFixedHeight() == TRUE )
					{
					nHeight = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Height() + ptrThisNode->get_DeltaY();
					bSetDirect = TRUE;
					}
				else
					{
					bSetDirect = FALSE;
					}
				}
			}
		}

	if ( bSetDirect == FALSE )
		{
		// Set the nWidthToDistribute in case parent is a horizontal container.
		// If the last child has fixed_Width = true don't consider its width in distribution width,else if 
		// fixed_width=false entire parent width is available for distribution.
		if ( iParentLayOut == LAYOUT_HORIZONTAL )
			{
			if ( pHolderNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE )
				{
				nLastsWidth = pHolderNode->m_Tile.get_ScreenObject()->GetDisplayRect().Width() + pHolderNode->get_DeltaX();
				nWidthToDistribute = nParentWidth - nLastsWidth; 
				bGiveLastWidth = FALSE;
				}
			else
				{
				nWidthToDistribute = nParentWidth ;
				}
			}
		else
			{
		// Set the nHeightToDistribute in case parent is a vertical container.
		// If the last child has fixed_Height = true don't consider its height in distribution height,else if
		// fixed_height is false entire parent's height is available for distribution.

			if ( pHolderNode->m_Tile.get_ListOfAttributes()->IsFixedHeight() == TRUE )
				{
				nLastsHeight = pHolderNode->m_Tile.get_ScreenObject()->GetDisplayRect().Height() + pHolderNode->get_DeltaY();
				nHeightToDistribute = nParentHeight - nLastsHeight;
				bGiveLastHeight = FALSE;
				}
			else
				{
				nHeightToDistribute = nParentHeight ;
				}
			}
		/*Loop for calculating nSummatedChildHeight & nSummatedChildWidth*/
		
		if ( ((iParentLayOut == LAYOUT_HORIZONTAL) && (ptrParentNode->m_nSummatedWidth == 0))
			|| ((iParentLayOut == LAYOUT_VERTICAL) && (ptrParentNode->m_nSummatedHeight == 0)))
			{
			for ( j = 0; j < iTotalChildren; j++ )
				{
				pHolderNode = ptrParentNode->GetChild(j);
				ASSERT( pHolderNode );	
				if ( iParentLayOut == LAYOUT_VERTICAL )
					{
					if ( pHolderNode->m_Tile.IsContainer() == TRUE )
						{
						CRect lContainerRect = pHolderNode->m_Tile.get_ScreenObject()->GetDisplayRect();
						nSummatedChildHeight = nSummatedChildHeight + lContainerRect.Height() + pHolderNode->get_DeltaY() ;
						}
					else
						{
						nSummatedChildHeight = nSummatedChildHeight + pHolderNode->m_Tile.get_ListOfAttributes()->get_Height();
						}
					ASSERT( nSummatedChildHeight );
					}
				else
					{
					if ( pHolderNode->m_Tile.IsContainer() == TRUE )
						{
						CRect lContainerRect = pHolderNode->m_Tile.get_ScreenObject()->GetDisplayRect();
						nSummatedChildWidth = nSummatedChildWidth + lContainerRect.Width() + pHolderNode->get_DeltaX();
						}
					else
						{
						int nTempWidth = pHolderNode->m_Tile.get_ListOfAttributes()->get_Width();
						nSummatedChildWidth = nSummatedChildWidth + nTempWidth ;//+ pHolderNode->m_Tile.get_ListOfAttributes()->get_Width();
						}
					ASSERT( nSummatedChildWidth );
					}
				}
			
			if ( iParentLayOut == LAYOUT_HORIZONTAL )
				{
				// Set the nSummatedChildWidth and parents m_nSummatedWidth which will be used by other children.
				if ( bGiveLastWidth == FALSE )
					{
					nSummatedChildWidth = nSummatedChildWidth - nLastsWidth;
					}
				else
					{
					nSummatedChildWidth = nSummatedChildWidth;
					}
				ptrParentNode->m_nSummatedWidth = nSummatedChildWidth;
				}
			else
				{
				// Set the nSummatedChildHeight and parents m_nSummatedHeight which will be used by other children.
				if ( bGiveLastHeight == FALSE )
					{
					nSummatedChildHeight = nSummatedChildHeight - nLastsHeight;
					}
				else
					{
					nSummatedChildHeight = nSummatedChildHeight;
					}
				ptrParentNode->m_nSummatedHeight = nSummatedChildHeight;
				
				}
			
			}
		// In case the parent's m_nSummatedWidth/m_nSummatedHEight are set use them to initialise
		// nSummatedChildHeight/nSummatedChildWidth.
		else
			{
			if ( iParentLayOut == LAYOUT_VERTICAL )
				{
				nSummatedChildHeight = ptrParentNode->m_nSummatedHeight;
				ASSERT(nSummatedChildHeight);
				}
			else
				{
				nSummatedChildWidth = ptrParentNode->m_nSummatedWidth;
				ASSERT(nSummatedChildWidth);
				}
			}

		/*Check if the node IsContainer.If yes take readings from DisplayRect()*/
		/*Get the node's actual height and width from its list of attributes.*/
		if ( ptrThisNode->m_Tile.IsContainer() == TRUE )
			{
			CRect lThisNodesRect = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect();
			nActualChildHeight = lThisNodesRect.Height() + ptrThisNode->get_DeltaY();
			nActualChildWidth = lThisNodesRect.Width();
			}
		else
			{
			nActualChildHeight = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Height();
			nActualChildWidth = ptrThisNode->m_Tile.get_ListOfAttributes()->get_Width();
			}
		
		ASSERT( nActualChildHeight );
		ASSERT( nActualChildWidth );

		if ( ptrThisNode->m_Tile.get_TileName() != _T("dialog") )
			{
			if ( ptrThisNode->m_Tile.IsContainer() )
				{				// yes the parent is a container
				if ( iParentLayOut == LAYOUT_VERTICAL )
					{
					nHeight = (nActualChildHeight * nHeightToDistribute)/nSummatedChildHeight;
					if ( ptrThisNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE )
						{
						nWidth = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Width();
						}
					else
						{
						nWidth = nParentWidth - ptrThisNode->get_DeltaX();
						}
					}
				else
					{
					if ( ptrThisNode->m_Tile.get_ListOfAttributes()->IsFixedHeight() == TRUE )
						{
						nHeight = ptrThisNode->m_Tile.get_ScreenObject()->GetDisplayRect().Height() + ptrThisNode->get_DeltaY();// this deltaY is added because 
						// at end of this function we are deducting deltay from height.So to keep height as it is this is done.
						}
					else
						{
						nHeight = nParentHeight ;				
						}
						nWidth = (nActualChildWidth * nWidthToDistribute)/nSummatedChildWidth;
					}
				}
			else
				{            // it is not a container
				switch (iParentLayOut)
					{
					case LAYOUT_VERTICAL :
						nWidth = nParentWidth;
						nHeight = ((nActualChildHeight * nHeightToDistribute)/nSummatedChildHeight);
						break;

					case LAYOUT_HORIZONTAL :
						nHeight = nActualChildHeight;//nParentHeight;
						nWidth = ((nActualChildWidth * nWidthToDistribute)/nSummatedChildWidth);
						break;
					default : 
						ASSERT(FALSE);
					}
				}
			}
		}

		ASSERT( nWidth );
		ASSERT( nHeight );

		int nDeltaX = 0,nDeltaY = 0;
		if ( !m_nHasFixedWidthChild )
			{
			}
		else
			{
			if ( (ptrThisNode->GetChildCount() == 1 ) && (ptrThisNode->m_Tile.IsContainer() == FALSE))
				nDeltaX = 2 * SPACESXBOXROW;
			}

		nDeltaY = ptrThisNode->get_DeltaY();	
		
		// Set the m_nDisplayWidthToSet,m_DisplayHeightToSet,DisplayRect.
		ptrThisNode->m_Tile.get_ScreenObject()->set_WidthToSet(nWidth - nDeltaX);
		ptrThisNode->m_Tile.get_ScreenObject()->set_HeightToSet(nHeight - nDeltaY);
		ptrThisNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,(nWidth - nDeltaX),(nHeight - nDeltaY));
		
		ptrParentNode->set_LastAccessedChild(iIndexToRestore);
		return SDS_RTNORM;
	}

	/************************************************************************************
	/* + int Tree::PositionDisplayRect(TreeNode *pThisNode)
	*	This function is called from within CalcPositions().
	*	Here we obtain the index of this node from the parent node.
	*	Then depending on the layout of the parent we loop through all 
	*	the children to the left of this node( in case of horizontal parent layout)
	*	and go on adding the required x-direction offset for this child  depending on the left children
	*	widths OR 	we loop through all the children to the	top of this node
	*	( in case of vertical parent layout)
	*	and go on adding the required y-direction offset for this child depending on the top
	*	children heights.The controls get the position with respect to the dialog's left
	*	corner.
	*	Call get_LastAccessedSibling() before the loop and restore that index at end
	*	by calling set_LastAccessedSibling(index).
	*/

	int Tree::PositionDisplayRect(						// Calculate the actual positions of the DisplayRects.
							  TreeNode *pThisNode   // i : Node to be calculated.
							  )
	{
	ASSERT( pThisNode );
	static CRect DlgReferenceRect;		// Parent's Reference DisplayRect to calculate child's display rect.
	CRect ReferenceRect;
	int nNewWidthToSet = 0;
	int nNewHeightToSet = 0;
	int nXOffsetWrtParent = 0;
	int nYOffsetWrtParent = 0;
	TreeNode *pParentNode = NULL;
	CString strFunctionName = _T("");
	CString strParentFunctionName = _T("");
	CString strFunctionalGrandParentOfThis = _T("");
	CString strGreatGParentFName = _T("");
	CString strGreatGParentRName = _T("");


	// In case it is a dialog set the DlgReferenceRect,then add the DLGEXTRAWIDTH and DLGEXTRAHT.
	if ( pThisNode->m_Tile.get_TileName() == _T("dialog") )
		{
		int nWidthIncrement = pThisNode->get_DeltaX();
		int nHeightIncrement = pThisNode->get_DeltaY();
		int lThisRectWidth = pThisNode->m_Tile.get_ScreenObject()->get_WidthToSet();
		int lThisRectHeight = pThisNode->m_Tile.get_ScreenObject()->get_HeightToSet();
		nNewWidthToSet = lThisRectWidth + nWidthIncrement ;
		nNewHeightToSet = lThisRectHeight + nHeightIncrement;
		CRect tempDlgRect( STARTX, STARTY, nNewWidthToSet + STARTX,nNewHeightToSet + STARTY);
		DlgReferenceRect = tempDlgRect;
		nNewWidthToSet = nNewWidthToSet + DLGEXTRAWIDTH;;
		nNewHeightToSet = nNewHeightToSet + DLGEXTRAHEIGHT;;
		pThisNode->m_Tile.get_ScreenObject()->SetRectForDisplay(STARTX, STARTY,nNewWidthToSet,nNewHeightToSet);
		return SDS_RTNORM;
		}

	pParentNode = pThisNode->get_Parent();
	ASSERT( pParentNode );
	CString strRenderableTileName = pThisNode->m_Tile.get_RenderableTileName();
	CString strFunctionalName = pThisNode->m_Tile.get_FunctionName();
	/*Initialise the great parent of this tile*/
	if ( pParentNode->get_Parent() )
		{
		TreeNode* pTempStore = pParentNode;
		pParentNode = pParentNode->get_Parent();
		while ( pParentNode )
			{
			strGreatGParentFName = pParentNode->m_Tile.get_FunctionName();
			if ( strGreatGParentFName.IsEmpty() == TRUE )
				pParentNode = pParentNode->get_Parent();
			else
				break;
			}
		strGreatGParentRName = pParentNode->m_Tile.get_RenderableTileName();
		pParentNode = pTempStore;
		}
	
	TreeNode *pChildNode = NULL;
	int iTempLastAccessed = 0;/*The loop changes this value so to restore it.*/
	int iLoop = 0, i = 0;
	int iLayOut;
	int iTotalSiblings = 0;
	BOOL bIsParentContainer/*Check the layout of the parent*/= FALSE;
	BOOL bIsParentADialog = FALSE,bIsParentARow = FALSE,bIsParentAColumn = FALSE,bIsParentABoxRow = FALSE,
		 bIsParentABoxRadioRow = FALSE,bIsParentABoxColumn = FALSE,bIsParentABoxRadioColumn = FALSE,
		 bIsParentARadioRow = FALSE,bIsParentARadioColumn = FALSE,bIsParentAParagraph = FALSE,bIsParentAConcat = FALSE;

	// Not to add extra spaces if the previous tile is a spacer.
	// If the previous tile is a spacer this value should be TRUE

	BOOL bIsThisASpace = FALSE,bIsPreviousTileASpace = FALSE,bIsSpacer = FALSE;

	CString strParentTileName = pParentNode->m_Tile.get_RenderableTileName();
	// Check the parent's tile name and set the corresponding flag.
	// eg. If the parent is a dialog set the bIsParentADialog to TRUE.
	if ( (pParentNode->m_Tile.get_ListOfAttributes()->get_Layout() == LAYOUT_VERTICAL) || (pParentNode->m_Tile.get_ListOfAttributes()->get_Layout() == LAYOUT_HORIZONTAL) )
		{
		bIsParentContainer = TRUE;
		if ( strParentTileName == _T("row") )
			bIsParentARow = TRUE;
		else if ( strParentTileName == _T("concatenation") )
			{
			bIsParentAConcat = TRUE;
			}
		else if ( strParentTileName == _T("column") )
			{
			bIsParentAColumn = TRUE;
			strParentFunctionName = pParentNode->m_Tile.get_FunctionName();
			}
		else if ( strParentTileName == _T("paragraph") )
			bIsParentAParagraph = TRUE;
		else if ( strParentTileName == _T("boxed_column") )
			bIsParentABoxColumn = TRUE;
		else if ( strParentTileName == _T("boxed_row") )
			bIsParentABoxRow = TRUE;
		else if ( strParentTileName == _T("boxed_radio_row") )
			bIsParentABoxRadioRow = TRUE;
		else if ( strParentTileName == _T("boxed_radio_column") )
			bIsParentABoxRadioColumn = TRUE;
		else if ( strParentTileName == _T("dialog") )
			bIsParentADialog = TRUE;
		else if ( strParentTileName == _T("radio_row") )
			bIsParentARadioRow = TRUE;
		else if ( strParentTileName == _T("radio_column") )
			bIsParentARadioColumn = TRUE;
		}
	if ( (strRenderableTileName == _T("spacer") || strFunctionalName == _T("spacer_1")) ) 
		{
		bIsThisASpace = TRUE;
		}
	// Get the total number of siblings of this node.
	iTotalSiblings = pParentNode->GetChildCount();
	iTempLastAccessed = pParentNode->get_LastAccessedChild();
	// Get the parent's layout.
	iLayOut = pParentNode->m_Tile.get_ListOfAttributes()->get_Layout();
	// What is the index of this child (0 to (TotalSiblings-1))
	iLoop = pParentNode->GetChildIndex(pThisNode);
	if ( iLoop == SDS_RTERROR )
		return SDS_RTERROR;
	// Get the function name of this tile.This is later used to check for okxxx subassemblies.
	strFunctionName = pThisNode->m_Tile.get_FunctionName();
	// Set whether this tile is the firsttile,middletile,lasttile in the parent
	// because this information is needed when displaying the tile.
	// eg. The first tile(FIRSTTILE) in a column is showed at the top and at the 
	// the last tile(LASTTILE) is shown at the bottom.Also the middle tiles should br treated as 
	// FIRSTTILE so that they are positioned properly.This value is used for that.

	if ( bIsParentContainer == TRUE )
		{
		if ( bIsParentARow || bIsParentAColumn || bIsParentABoxColumn )
			{
			if ( iLoop == 0  )
				{
				pThisNode->m_Tile.get_ScreenObject()->set_ColumnTile(FIRSTTILE);
				}
			else if ( iLoop == (iTotalSiblings - 1) )
				{
				pThisNode->m_Tile.get_ScreenObject()->set_ColumnTile(LASTTILE);
				}
			else
				{
				pThisNode->m_Tile.get_ScreenObject()->set_ColumnTile(FIRSTTILE);
				}
			}
		else
			{
			if ( iLoop == 0  )
				{
				pThisNode->m_Tile.get_ScreenObject()->set_ColumnTile(FIRSTTILE);
				}
			else if ( iLoop == (iTotalSiblings - 1) )
				{
				pThisNode->m_Tile.get_ScreenObject()->set_ColumnTile(LASTTILE);
				}
			else
				{
				pThisNode->m_Tile.get_ScreenObject()->set_ColumnTile(MIDDLETILE);
				}
			}
		
		}
	// Initialise the ReferenceRect and lParentRect.ReferenceRect is used to calculate 
	// the position of the child tiles.The ReferenceRect for parent as a dialog should be
	// the DisplayRect without addition of the DLGEXTRAWIDTH,DLGEXTRAHT.
	CRect lParentRect;
	if ( bIsParentADialog )	
		{
		ReferenceRect = lParentRect = DlgReferenceRect;
		bIsParentADialog = TRUE;
		}
	else
		{
		ReferenceRect = lParentRect = pParentNode->m_Tile.get_ScreenObject()->GetDisplayRect();
		}

	// Calculate the offset of this child in the parent .
	// This is done by getting the previous child which is always properly
	// set because to set the DisplayRect we traverse the tree left to right
	// and top to bottom,and depending on the parent's layout offset this 
	// tile by previous childs width(parent layout = horizontal) or height
	// (parent layout = vertical).Thus the nXOffsetWrtParent and nYOffsetWrtParent
	// are initialised.
	if ( iLoop > 0 )
		{
		pChildNode = pParentNode->GetChild(iLoop - 1); // get the previous sibling.
		ASSERT( pChildNode );
		if ( (pChildNode->m_Tile.get_RenderableTileName() == _T("spacer") || pChildNode->m_Tile.get_RenderableTileName() == _T("spacer_1")) )
			bIsPreviousTileASpace = TRUE;
		CRect lRect = pChildNode->m_Tile.get_ScreenObject()->GetDisplayRect();
		if ( iLayOut == LAYOUT_VERTICAL )
			{
			nXOffsetWrtParent = 0;
			nYOffsetWrtParent = lRect.bottom - lParentRect.top;
			}
		else
			{
			nYOffsetWrtParent = 0;
			nXOffsetWrtParent = lRect.right - lParentRect.left;
			}
		}
	// Read this tiles DisplayRect and then add initially calculated deltax and deltay to calculate 
	// the newWidthToSet and newHeightToSet.
	CRect nThisNodesRect = pThisNode->m_Tile.get_ScreenObject()->GetDisplayRect();
			
	nNewWidthToSet = nThisNodesRect.Width() + pThisNode->get_DeltaX();
	nNewHeightToSet = nThisNodesRect.Height() + pThisNode->get_DeltaY();
	ASSERT( nNewWidthToSet );
	ASSERT( nNewHeightToSet );

	int nNewTop,nNewLeft;

	if ( pThisNode->m_Tile.IsContainer() == TRUE )
		{
		// Santosh
		// This call should take care of the spacers.
		pThisNode->CalcHorSpaceExp();
		// Santosh
		if ( iLayOut == LAYOUT_VERTICAL )
			{
			// This is is entered if the parent is a 
			// dialog,column,boxed_column,boxed_radio_column,radio_column.
			// The major axis of the parent is vertical and the child will be moved 
			// along the minor axis.
			CPoint CenterPoint,BottomPoint;
			if ( pThisNode->m_Tile.get_ListOfAttributes()->IsFixedWidth() == TRUE )
				{
				int nAlignment = pThisNode->m_Tile.get_ListOfAttributes()->get_Alignment();
				// The code in this switch will position the tile along the parents minor axis.(here horizontal ie either left,right,centered)
				switch ( nAlignment )
					{
					case ALIGN_TOP :
					case ALIGN_BOTTOM :
					case ALIGN_DEFAULT :
					case ALIGN_LEFT:
						if ( bIsParentADialog )
							{
							nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
							}
						else if ( bIsParentAColumn )
							{
							nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
							}
						else if ( bIsParentABoxColumn )
							{
							nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXCOL;
							}
						else if ( bIsParentAParagraph )
							{
							nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXPARAGRAPH;
							}
						else if ( bIsParentABoxRadioColumn )
							{
							nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOCOL;
							}
						break;
					case ALIGN_RIGHT:
						BottomPoint = ReferenceRect.BottomRight();
						if ( bIsParentAColumn )
							{
							nNewLeft = BottomPoint.x - nNewWidthToSet;
							}
						else if ( bIsParentABoxColumn )
							{
							nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESX);
							}
						else if ( bIsParentAParagraph )
							{
							nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESXPARAGRAPH);
							}
						else if ( bIsParentABoxRadioColumn )
							{
							nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESXBOXRADIOCOL);
							}
						else
							nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESX );
						
						break;
					case ALIGN_CENTERED :
						CenterPoint = ReferenceRect.CenterPoint();
						nNewLeft = CenterPoint.x - (nNewWidthToSet / 2);
						break;
					default : 
						ASSERT(FALSE);
					
					}
				// Now calculate the top coordinate of the tile.
				// The spacing between the top and first child differs from that of the remaining children.
				if ( bIsParentADialog )
					{
					if ( iLoop > 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYTILES;
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESY;
						}
											
					}
				// Stick the first tile in a column to its top and the last to its bottom.
				else if ( bIsParentAColumn )
					{
				
						{
						if ( iLoop == 0 )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent ;
							}
						else if ( iLoop == (iTotalSiblings - 1) )
							{
							nNewTop = ReferenceRect.BottomRight().y - nNewHeightToSet;
							}
						else
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYCOLUMN;
							}
						}
					}
				// The first child in a box column is displaced from the top ,
				// then the middle tiles have a different spacing and the last tile has a different
				// spacing from the bottom.Hence SPACESYBOXCOLTOP,SPACESYBOXCOLBETWEEN,SPACESYBOXCOLBOTTOM.
				else if ( bIsParentABoxColumn )
					{
					if ( iLoop == 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXCOLTOP ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet + SPACESYBOXCOLBOTTOM);
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXCOLBETWEEN;
						}

					}
				else if ( bIsParentAParagraph )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					}
				else if ( bIsParentABoxRadioColumn )
					{
					if ( iLoop == 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOCOLTOP ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet + SPACESYBOXRADIOCOLBOTTOM);
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOCOLBETWEEN;
						}
					}
				else  
					{
					if ( Debug )
						AfxMessageBox(_T("Need to debug..."));
					
					if ( iLoop > 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYOTHERNONDIALOG;
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYFIRSTNONDLGCHILD;
						}
					}
				}
			else
				{
				// Calculation of the Left coordinate begins.
				// Displace all the tiles in a dialog SPACESX from left border.
				if ( bIsParentADialog )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
					}
				// Stick all the tiles in a column to the left border.
				else if ( bIsParentAColumn )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
					}
				// Displace all the tiles in a boxedcolumn SPACESXBOXCOL from the left border.
				else if ( bIsParentABoxColumn )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXCOL;
					}
				// Displace all the tiles in a paragraph SPACESXPARAGRAPH from the left border.
				else if ( bIsParentAParagraph )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXPARAGRAPH;
					}
				else if ( bIsParentABoxRadioColumn )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOCOL;
					}

				// Calculation of the left coordinate ends.

				// Calculation of the top coordinate starts.
				if ( bIsParentADialog )
					{
					if ( iLoop > 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYTILES;
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESY;
						}
										
					}
				else if ( bIsParentAColumn )
					{
					if ( iLoop == 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewTop = ReferenceRect.BottomRight().y - nNewHeightToSet;
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYCOLUMN;
						}
					}
				else if ( bIsParentABoxColumn )
					{
					if ( iLoop == 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXCOLTOP ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet + SPACESYBOXCOLBOTTOM);
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXCOLBETWEEN;
						}

					}
				else if ( bIsParentAParagraph )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					}
				else if ( bIsParentABoxRadioColumn )
					{
					if ( iLoop == 0 )
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOCOLTOP ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet + SPACESYBOXRADIOCOLBOTTOM);
						}
					else
						{
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOCOLBETWEEN;
						}
					}
				// Calculation for the top point ends.
				}
			// Code for showing the ok_cancel,ok_cancel_help_errtile,ok_cancel_help,ok_cancel_help_info at the bottom of the parent 
			// and centered.
			if ( (strFunctionName == _T("ok_cancel")) || (strFunctionName == _T("ok_cancel_help_errtile"))
				|| (strFunctionName == _T("ok_cancel_help_info")) || (strFunctionName == _T("ok_only"))
				|| (strFunctionName == _T("ok_cancel_help"))|| (strFunctionName == _T("ok_cancel_err")) )
					{
					nNewLeft = ReferenceRect.CenterPoint().x - (nNewWidthToSet/2);
					}
			}
		else				// this is a container and parent has a horizontal layout.
			{
			// The parent has a horizontal layout.Minor axis is vertical.So alignment could be top,centered,bottom.
			CPoint CenterPoint,BottomPoint;
			if ( pThisNode->m_Tile.get_ListOfAttributes()->IsFixedHeight() == TRUE )
				{
				int nAlignment = pThisNode->m_Tile.get_ListOfAttributes()->get_Alignment();
				// Calculating the Left coordinate starts.
				if ( bIsParentABoxRow )
					{
					if ( iLoop == 0 )
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXROW ;
					else if ( iLoop == (iTotalSiblings - 1) )
						nNewLeft = ReferenceRect.BottomRight().x - ( nNewWidthToSet + SPACESXBOXROW );
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXROW ;
						}
					}
				else if ( bIsParentARow )
					{
					if ( iLoop == 0 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewLeft = ReferenceRect.BottomRight().x - nNewWidthToSet;
						}
					else 
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXROWS;
						}
					}
				else if ( bIsParentAConcat )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXCONCAT;
					}
				else if ( bIsParentABoxRadioRow )
					{
					if ( iLoop == 0 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOROW ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewLeft = ReferenceRect.BottomRight().x - (nNewWidthToSet + SPACESXBOXRADIOROW );
						}
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOROW;
						}
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOROWTOP;
					}
				else
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
					}
					// Calculating the left coordinate ends.
				// Calculating the top coordinate starts.
				switch ( nAlignment )
					{
					case ALIGN_DEFAULT:
					case ALIGN_LEFT:
					case ALIGN_RIGHT:
					case ALIGN_CENTERED :
						CenterPoint = ReferenceRect.CenterPoint();
						if ( bIsParentARow || bIsParentAConcat )
							{
							nNewTop = CenterPoint.y - (nNewHeightToSet / 2);
							}
						if ( bIsParentABoxRow ) 
							{
							nNewTop = CenterPoint.y - (nNewHeightToSet / 2)+OFFSET2;
							}
						if ( bIsParentABoxRadioRow )
							{
							nNewTop = CenterPoint.y - (nNewHeightToSet / 2);
							}
						break;
					case ALIGN_TOP : 
						if ( bIsParentARow )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent ;
							}
						else if ( bIsParentABoxRow )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXROWTOP + OFFSET2;
							}
						else if ( bIsParentAConcat )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent;
							}
						else if ( bIsParentABoxRadioRow )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOROWTOP;
							}
						else 
							{
							if ( iLoop > 0 )
								{
								nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYFIRSTNONDLGCHILD;//SPACESYOTHERNONDIALOG;
								}
							else
								{
								nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYFIRSTNONDLGCHILD;
								}
							}
						break;
					case ALIGN_BOTTOM :
						BottomPoint = ReferenceRect.BottomRight();
						if ( bIsParentARow )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet);
							}
						else if ( bIsParentABoxRow )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet + SPACESYBOXROWBOTTOM);
							}
						else if ( bIsParentAConcat )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet);
							}
						else if ( bIsParentABoxRadioRow )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet + SPACESYBOXRADIOROWBOTTOM);
							}
						else
							{
							if ( iLoop > 0 )
								{
								nNewTop = BottomPoint.y - (nNewHeightToSet + SPACESYFIRSTNONDLGCHILD);//SPACESYOTHERNONDIALOG );
								}
							else
								{
								nNewTop = BottomPoint.y - (nNewHeightToSet + SPACESYFIRSTNONDLGCHILD );
								}
							}
						break;
					default : 
						ASSERT(FALSE);
					}
				// Calculating the top coordinate ends.
				}
			else
				{
				// Calculating the left coordinate starts.
				if ( bIsParentABoxRow )
					{
					if ( iLoop == 0 )
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXROW ;
					else if ( iLoop == (iTotalSiblings - 1) )
						nNewLeft = ReferenceRect.BottomRight().x - ( nNewWidthToSet + SPACESXBOXROW );
					else
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXROW;
					}
				else if ( bIsParentARow )
					{
					if ( iLoop == 0 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewLeft = ReferenceRect.BottomRight().x - nNewWidthToSet;
						}
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXROWS;
						}
				
					}
				else if ( bIsParentAConcat )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
					}
				else if ( bIsParentABoxRadioRow )
					{
					if ( iLoop == 0 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOROW ;
						}
					else if ( iLoop == (iTotalSiblings - 1) )
						{
						nNewLeft = ReferenceRect.BottomRight().x - (nNewWidthToSet + SPACESXBOXRADIOROW );
						}
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOROW;
						}
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOROWTOP;
					}
				else
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
					}
				// Calculating the left coordinate ends.

				// Calculating the top coordinate starts.
				if ( bIsParentARow )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent ;
					}
				else if ( bIsParentABoxRow )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXROWTOP ;
					}
				else if ( bIsParentAConcat )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					}
				else if ( bIsParentABoxRadioRow )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOROWTOP;
					}
			
				// Calculating the top coordinate ends.
				}
			}
		}
	else	//it is not a container.
		{
		if ( iLayOut == LAYOUT_VERTICAL )		// this is not a container & parent has a vertical layout.
			{
			// Parent is a dialog.
			if ( bIsParentADialog )				// this is not a container  & parent is dialog.
				{
				if ( iLoop > 0 )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYTILES;
					}
				else
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESY;
					}
				}
			// Parent is a column.
			else if ( bIsParentAColumn )			// this is not a container  & parent is column.
				{
				nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
				if ( iLoop == 0 )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					}
				else if ( iLoop == (iTotalSiblings -1) )
					{
					nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet);
					}
				else
					{
					if ( pParentNode->m_nAllSwatches == 1 )
						nNewTop = ReferenceRect.top + nYOffsetWrtParent ;
					else
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYCOLUMN;
					}

				}
			// Parent is a paragraph.
			else if ( bIsParentAParagraph )			// this is not a container  & parent is column.
				{
				nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
				if ( iLoop == 0 )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					}
				else
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYPARAGRAPH;
					}

				}
			// Parent is a radio_Column.
			else if ( bIsParentARadioColumn )			// this is not a container  & parent is column.
				{
				nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
				if ( iLoop == 0 )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					}
				else if ( iLoop == (iTotalSiblings - 1) )
					{
					nNewTop = ReferenceRect.BottomRight().y - nNewHeightToSet;
					}
				else
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYRADIOCOLUMN;
					}
				}
			// Parent is a box_column.
			else if ( bIsParentABoxColumn )		// this is not a container  & parent is boxed_column.
				{
				nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXCOL;
				if ( iLoop == 0 )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXCOLTOP ;
					}
				else if ( iLoop == (iTotalSiblings - 1) )
					{
					nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet + SPACESYBOXCOLBOTTOM);
					}
				else
					{
					if ( pParentNode->m_nAllSwatches == 1 )
						nNewTop = ReferenceRect.top + nYOffsetWrtParent;
					else
						nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXCOLBETWEEN;
					}

				}
			// Parent is a box radio_column.
			else if ( bIsParentABoxRadioColumn )	// this is not a container  & parent is boxed_radio_column.
				{
				nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOCOL;
				if ( iLoop == 0 )
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOCOLTOP;
					}
				else if ( iLoop == (iTotalSiblings - 1) )
					{
					nNewTop = ReferenceRect.BottomRight().y - (nNewHeightToSet + SPACESYBOXRADIOCOLBOTTOM);
					}
				else
					{
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOCOLBETWEEN;
					}
				}
			// In case of parent having a vertical layout,alignment could be left,centered,right.
			// This was handled later hence in the above code also there is left coordinate calculated.
			// That worked fine but in cases where alignment is explicitly specified the left coordinate has to change
			// and the left has to be manipulated.Hence this code has been added.
			int nAlignment = pThisNode->m_Tile.get_ListOfAttributes()->get_Alignment();
			CPoint CenterPoint,BottomPoint;
			switch ( nAlignment )
				{
				case ALIGN_TOP :
				case ALIGN_BOTTOM :
				case ALIGN_DEFAULT :
				case ALIGN_LEFT:
					if ( bIsParentADialog )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESX;
						}
					else if ( bIsParentAColumn )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
						}
					else if ( bIsParentABoxColumn )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXCOL;
						}
					else if ( bIsParentAParagraph )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXPARAGRAPH;
						}
					break;
				case ALIGN_RIGHT:
					BottomPoint = ReferenceRect.BottomRight();
					if ( bIsParentAColumn )
						{
						nNewLeft = BottomPoint.x - nNewWidthToSet;
						}
					else if ( bIsParentABoxColumn )
						{
						nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESX);
						}
					else if ( bIsParentAParagraph )
						{
						nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESXPARAGRAPH);
						}
					else
						nNewLeft = BottomPoint.x - (nNewWidthToSet + SPACESX );
					break;
				case ALIGN_CENTERED :
					CenterPoint = ReferenceRect.CenterPoint();
					nNewLeft = CenterPoint.x - (nNewWidthToSet / 2);
					break;
				default : 
					ASSERT(FALSE);
				}

			}
		else		// this tile is not a container and the parent has a horizontal layout.
			{
			// Layout of the parent is horizontal.Minor axis vertical.Hence alignment could be top,centered,bottom.
			int nAlignment = pThisNode->m_Tile.get_ListOfAttributes()->get_Alignment();
			CPoint CenterPoint,BottomPoint;
			// Parent is a row or a radio_row.
			if ( bIsParentARow || bIsParentARadioRow )			// tile not a container and parent is a row.
				{
				nNewTop = ReferenceRect.top + nYOffsetWrtParent;
				if ( iLoop == 0 )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
					}
				else if ( iLoop == (iTotalSiblings -1) )
					{
					nNewLeft = ReferenceRect.BottomRight().x - (nNewWidthToSet );
					}
				else
					{
					if ( pParentNode->m_nAllSwatches == 1 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
						}
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXROWS;
						}
					if ( bIsThisASpace ) 
						nNewLeft -= SPACESXROWS;
					}
				/*CString*/ strFunctionalGrandParentOfThis = pThisNode->get_Parent()->get_Parent()->m_Tile.get_FunctionName();
				if ( (strFunctionalGrandParentOfThis == _T("ok_cancel")) || (strFunctionalGrandParentOfThis == _T("ok_cancel_help"))
					|| (strFunctionalGrandParentOfThis == _T("ok_cancel_help_info")) || (strFunctionalGrandParentOfThis == _T("ok_cancel_help_errtile"))
					|| (strFunctionalGrandParentOfThis == _T("ok_only")) || (strFunctionalGrandParentOfThis == _T("ok_cancel_err")))
					{
					if ( iLoop == 0 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
						}
					else if ( iLoop == (iTotalSiblings -1) )
						{
						nNewLeft = ReferenceRect.BottomRight().x - (nNewWidthToSet );
						}
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
						}
					}
				}
			// Parent is a box row.
			else if ( bIsParentABoxRow )
				{				
				if ( iLoop == 0 )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXROW ;
					}
				else if ( iLoop == (iTotalSiblings - 1) )
					{
					nNewLeft = ReferenceRect.BottomRight().x - ( nNewWidthToSet + (SPACESXBOXROW));
					}
				else
					{
					if ( pParentNode->m_nAllSwatches == 1 )
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
						}
					else
						{
						nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXROW;
						}
					}
					
				nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXROWTOP ;
				}
			// Parent is a box radio row.
			else if ( bIsParentABoxRadioRow )
				{
				if ( iLoop == 0 )
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOROW ;
					}
				else if ( iLoop == (iTotalSiblings - 1) )
					{
					nNewLeft = ReferenceRect.BottomRight().x - (nNewWidthToSet + SPACESXBOXRADIOROW );
					}
				else
					{
					nNewLeft = ReferenceRect.left + nXOffsetWrtParent + SPACESXBOXRADIOROW;
					}
					nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXRADIOROWTOP;
				}
			// Parent is a Concatenation.
			else if ( bIsParentAConcat )
				{
				nNewLeft = ReferenceRect.left + nXOffsetWrtParent;
				nNewTop = ReferenceRect.top + nYOffsetWrtParent;
				}
			// Calculating the top coordinate when alignment is specified.
			// This code was added later and the top coordinate calculation
			// in the above cases still is there.
			switch ( nAlignment )
					{
					case ALIGN_DEFAULT:
					case ALIGN_LEFT:
					case ALIGN_RIGHT:
					case ALIGN_CENTERED :
						CenterPoint = ReferenceRect.CenterPoint();
						if ( bIsParentARow || bIsParentAConcat )
							{
							nNewTop = CenterPoint.y - (nNewHeightToSet / 2);
							}
						if ( bIsParentABoxRow )
							{
							nNewTop = CenterPoint.y - (nNewHeightToSet / 2) + OFFSET2;
							}
						break;
					case ALIGN_TOP : 
						if ( bIsParentARow )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent ;
							}
						else if ( bIsParentABoxRow )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent + SPACESYBOXROWTOP + OFFSET2;
							}
						else if ( bIsParentAConcat )
							{
							nNewTop = ReferenceRect.top + nYOffsetWrtParent;
							}
						break;
					case ALIGN_BOTTOM :
						BottomPoint = ReferenceRect.BottomRight();
						if ( bIsParentARow )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet);
							}
						else if ( bIsParentABoxRow )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet + SPACESYBOXROWBOTTOM);
							}
						else if ( bIsParentAConcat )
							{
							nNewTop = BottomPoint.y - (nNewHeightToSet);
							}
						break;
					default : 
						ASSERT(FALSE);
					}
			}

			if ( strRenderableTileName == _T("slider"))
				{
				nNewLeft = /*ReferenceRect.left + nXOffsetWrtParent +*/ nNewLeft;
				nNewTop += /*ReferenceRect.top + nYOffsetWrtParent +*/ SPACESYSLIDER;
				}
			if ( strFunctionName == _T("ok_button") && (bIsParentARow == FALSE) )
				{
				nNewLeft = ReferenceRect.CenterPoint().x - (nNewWidthToSet/2);
				}
			
		}
		
		// Start of spaces	
			if ( iLoop > 0 )
				{
				if ( bIsPreviousTileASpace /*|| bIsThisASpace*/ )
					{
					pChildNode = pParentNode->GetChild(iLoop - 1); // get the previous sibling.
					ASSERT( pChildNode );
					CRect prevRect = pChildNode->m_Tile.get_ScreenObject()->GetDisplayRect();
					if (bIsParentADialog )
						{
						nNewTop = prevRect.bottom;
						}
					else if (bIsParentAColumn )
						{
						nNewTop = prevRect.bottom;
						}
					else if (bIsParentABoxColumn )
						{
						nNewTop = prevRect.bottom;
						}
					else if ( bIsParentAParagraph )
						{
						nNewTop = prevRect.bottom;
						}
				
					else if ( bIsParentARow )
						{
						if (/* strFunctionalName == _T("spacer_1") 
							|| */(strGreatGParentFName.Find(_T("ok_cancel")) != -1 ) 
							)
							nNewLeft = prevRect.right;
						//else if ( strFunctionalName == _T("spacer_0") )
						//	nNewLeft = prevRect.right /*+ SPACESXROWS*/;
						else
							nNewLeft = prevRect.right + pParentNode->m_nHorSpaceExpand;
						}
					else if ( bIsParentABoxRow )
						{
						//if ( strFunctionalName == _T("spacer_1") 
						//	|| (strGreatGParentFName.Find(_T("ok_cancel")) != -1 ) 
						//	)
							nNewLeft = prevRect.right + pParentNode->m_nHorSpaceExpand;
						//else
						//	nNewLeft = prevRect.right + SPACESXBOXROW;
						}
					else if ( bIsParentAConcat )
						{
						nNewLeft = prevRect.right;
						}
						
					}
				if ( bIsThisASpace )
					{
					pChildNode = pParentNode->GetChild(iLoop - 1); // get the previous sibling.
					ASSERT( pChildNode );
					CRect prevRect = pChildNode->m_Tile.get_ScreenObject()->GetDisplayRect();
					if (bIsParentADialog )
						{
						nNewTop = prevRect.bottom;
						}
					else if (bIsParentAColumn )
						{
						nNewTop = prevRect.bottom;
						}
					else if (bIsParentABoxColumn )
						{
						nNewTop = prevRect.bottom;
						}
					else if ( bIsParentAParagraph )
						{
						nNewTop = prevRect.bottom;
						}
					if ( bIsPreviousTileASpace )
						{
						//if ( bIsParentARow )
						//	{
							nNewLeft = prevRect.right;
						//	}

						}
				}
				}
			//}
			// End of spaces 
	// Set the display rect for this tile.
	pThisNode->m_Tile.get_ScreenObject()->SetRectForDisplay(nNewLeft,nNewTop,nNewWidthToSet,nNewHeightToSet);
	pParentNode->set_LastAccessedChild(iTempLastAccessed);
	return SDS_RTNORM;
	}


	/************************************************************************************
	/* + void Tree::PrintDisplayRects()
	*	Debugging support.
	*
	*
	*
	*
	*/
	#ifdef _DEBUG
	void Tree::PrintDisplayRects()
	{
	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;							// holds previous value of pOldNode
	TreeNode *pTreeNode = NULL;
	     		 
	pOldNode = pTreeNode = m_startTreeNode;                                 
     
	bHasChildren  =  pOldNode->HasChild();                // check whether child is present

	if(!bHasChildren)
		{
		TilesDisplayRect(pOldNode);							// return ,if only one node in tree to be traversed
		}
	else
		{
		TilesDisplayRect(pOldNode);							// display tree nodes details for current node
		}

	while(1)
		{

		bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
         
		if (bHasChildren)
			{ 
			pOldNode = pOldNode->GetFirstChild();           // get the first child
			TilesDisplayRect(pOldNode);
			}
	   else
			{ 
			// if child not found go to the next sibling
			TempOldNode = pOldNode ;                      
			if (pOldNode->get_Parent() != NULL)
				pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
			else
				pOldNode = NULL;

			if (pOldNode != NULL)
				{
				TilesDisplayRect(pOldNode);
				}		
			else
				{
				// if sibling noit found go to parent
				pOldNode = TempOldNode;
				pOldNode = pOldNode->get_Parent();
								
				while (1)
					{//  to set next sibling after child level siblings have been traversed
				   
					if (pOldNode->get_Parent())
						TempOldNode = (pOldNode->get_Parent())->GetNextChild();
					else
						TempOldNode = NULL;
               
					// keep moving up the tree till sibling is found
					if (TempOldNode != NULL)
						{
						pOldNode = TempOldNode;
						TilesDisplayRect(pOldNode);
						break;
						}
					else
						{
						// break if the traversing operation reaches where it started 
						if(pOldNode == pTreeNode)
							 break;
                     			 
						pOldNode = pOldNode->get_Parent();
					 
						if(pOldNode == pTreeNode)
							 break;
						}
					}// to set next sibling after chilren level siblings have been traversed--end	   

			   
				}// end check for has child

				if(pOldNode == pTreeNode)
					{
					break;
					}
			}
		}

	}

		 





	/*******************************************************************
	/* + void Tree::TilesDisplayRect(TreeNode *pOldNode)
	*	Debugging support.
	*
	*
	*
	*
	*/

	void Tree::TilesDisplayRect(TreeNode *pOldNode)
	{
	CRect lRect = pOldNode->m_Tile.get_ScreenObject()->GetDisplayRect();
	CString strTileName = pOldNode->m_Tile.get_TileName();
	int dx = pOldNode->get_DeltaX();
	int dy = pOldNode->get_DeltaY();
	if ( strTileName.IsEmpty() == TRUE )
		strTileName = pOldNode->m_Tile.get_FunctionName();
	//strTileName = strTileName + pOldNode->m_Tile.get_ListOfAttributes()->get_Label();
	afxDump << strTileName <<" "; 
	afxDump << "width(" <<lRect.left<<","<<lRect.right<<")"<<" height("<<lRect.top<<","<<lRect.bottom<<")"<< dx << " "<< dy <<"\n";

	}
	#endif


/************************************************************************************
/*
Recursive function to delete the screenobjects of a node and all its children.
*/
static void exitNode (TreeNode *pNode)
{
	pNode->m_Tile.set_ScreenObject(NULL);
	for ( int i=0, n=pNode->GetChildCount (); i<n; i++ )
	{
		TreeNode *pChild = pNode->GetChild (i);
		if ( pChild )
			exitNode (pChild);
	}
}

/************************************************************************************
/* + int Tree::Exit()
*	Delete all the ScreenObjects contained in the Tile.
*
*
*
*
*/
int Tree::Exit()											//	Delete all the ScreenObjects contained in the Tile.
{
	if ( !m_startTreeNode )
		return SDS_RTERROR;
	TreeNode *pNode = m_startTreeNode; 
	exitNode (pNode);
	return SDS_RTNORM;

}
/************************************************************************************
/* + int Tree::ReAdjust()
*	This pass is after AdjustWidthHeight().If the parent needs
*	some increase in width or height to accomodate its children it is done here.
*
*
*
*/
	int Tree::ReAdjust()
	{	
	 BOOL bHasChildren;                                     // value to hold whether the current node has children
	 TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	 TreeNode *TempOldNode = NULL;    
	 TreeNode *pTreeNode = NULL;
	 int Diff = 0;
 
	// Get the root node of the tree.
	 pTreeNode = pOldNode = get_StartTreeNode();
	 ASSERT( pOldNode);
	 bHasChildren  =  pOldNode->HasChild();                // check whether child is present
	 if ( !bHasChildren )
		{
		return SDS_RTNORM;                                     // return ,if only one node in tree to be traversed
		}
	 else
		{	
		}
	label:
	 bHasChildren = pOldNode->HasChild();
	 if ( bHasChildren )                           // yes the node has children
		 {
		 pOldNode = pOldNode->GetFirstChild();
		 ASSERT( pOldNode );
		 Diff = 0;
		 goto label;
		 }
	 else
		 {
	label1:	 long lWidth = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Width();
		 long lHeight = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Height();
		 
		 int nDisplayWidth = pOldNode->m_Tile.get_ScreenObject()->get_WidthToSet();
		 int nDisplayHeight = pOldNode->m_Tile.get_ScreenObject()->get_HeightToSet();
		 if ( lHeight > nDisplayHeight )
			 {
			nDisplayHeight = lHeight;
			 }
		 pOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(nDisplayHeight);
		 // If the width in list of attributes is greater than the display rects width
		 // increase the display width by that much amount by setting the m_nChildDiffWidth variable.
		 // m_nChildDiffWidth contains the amount by which the node incremented to accomodate its child.
		 if ( lWidth > nDisplayWidth )
			{
			pOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(lWidth);
			Diff += lWidth - nDisplayWidth;
			pOldNode->m_nChildDiffWidth = Diff;
			pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,lWidth,nDisplayHeight);
			}
		 TempOldNode = pOldNode;
		 if ( pOldNode->get_Parent() != NULL )
			{
			pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
			}
		else
			{
			long lWidth = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Width();
			long lHeight = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Height();
					 
			int nDisplayWidth = pOldNode->m_Tile.get_ScreenObject()->get_WidthToSet();
			int nDisplayHeight = pOldNode->m_Tile.get_ScreenObject()->get_HeightToSet();
			if ( lHeight > nDisplayHeight )
				{
				nDisplayHeight = lHeight;
				}
			pOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(nDisplayHeight);
			// If the width in list of attributes is greater than the display rects width
			// increase the display width by that much amount by setting the m_nChildDiffWidth variable.
			// m_nChildDiffWidth contains the amount by which the node incremented to accomodate its child.
			if ( lWidth > nDisplayWidth )
				{
				pOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(lWidth);
				Diff += lWidth - nDisplayWidth;
				pOldNode->m_nChildDiffWidth = Diff;
				pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,lWidth,nDisplayHeight);
				}
			pOldNode = NULL;
			}
		if ( pOldNode != NULL )
			{
			long lWidth = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Width();
			long lHeight = (long)pOldNode->m_Tile.get_ListOfAttributes()->get_Height();
					 
			int nDisplayWidth = pOldNode->m_Tile.get_ScreenObject()->get_WidthToSet();
			int nDisplayHeight = pOldNode->m_Tile.get_ScreenObject()->get_HeightToSet();	
			if ( lHeight > nDisplayHeight )
				nDisplayHeight = lHeight;
			pOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(nDisplayHeight);
			// If the width in list of attributes is greater than the display rects width
			// increase the display width by that much amount by setting the m_nChildDiffWidth variable.
			// m_nChildDiffWidth contains the amount by which the node incremented to accomodate its child.
			if ( lWidth > nDisplayWidth )
				{
				pOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(lWidth);
				Diff += lWidth - nDisplayWidth;
				pOldNode->m_nChildDiffWidth = Diff;
				pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,lWidth,nDisplayHeight);
				}
			goto label;
			}
		else
			{
			pOldNode = TempOldNode->get_Parent();
			ASSERT( pOldNode );

			int nOrgWidth = pOldNode->m_Tile.get_ScreenObject()->GetDisplayRect().Width();
			int nOrgHeight = pOldNode->m_Tile.get_ScreenObject()->GetDisplayRect().Height();
			Diff = 0;
			for ( int nIndex = 0;nIndex < pOldNode->GetChildCount();nIndex++ )
				{
				TreeNode * pHolder = pOldNode->GetChild(nIndex);
				ASSERT( pHolder );
				Diff += pHolder->m_nChildDiffWidth;
				}
			pOldNode->m_nChildDiffWidth = Diff;
			nOrgWidth = nOrgWidth + Diff ;
			pOldNode->m_Tile.get_ScreenObject()->set_HeightToSet(nOrgHeight);
			pOldNode->m_Tile.get_ScreenObject()->set_WidthToSet(nOrgWidth);
			pOldNode->m_Tile.get_ScreenObject()->SetRectForDisplay(0,0,nOrgWidth,nOrgHeight);
			if ( pOldNode == pTreeNode )
				{
				return SDS_RTNORM;
				}
			else
				{
				Diff = 0;
				goto label1;
				}
			}
			
		 }
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Tree::SetRenderableTileNameForCluster(TreeNode* )
*	Set the renderable Tile name of cluster to:
*	row : if layout = horizontal
*	column : if layout = vertical.
*
*
*
*/

int Tree::SetRenderableTileNameForCluster(
										  TreeNode *pTreeNode,	// i : Node to be operated on.
										  int nResTile				// i : Identifier for a cluster,radio_cluster.
										  )
	{
	ASSERT( pTreeNode );
	int nRes = SDS_RTERROR;
	if ( pTreeNode )
		{

		int iLayOut = pTreeNode->m_Tile.get_ListOfAttributes()->get_Layout();
		switch (iLayOut)
			{
			case LAYOUT_VERTICAL:
				if ( nResTile == CLUSTER_T)
					pTreeNode->m_Tile.set_RenderableTileName(CString(_T("column")));
				else if ( nResTile == RADIO_CLUSTER_T)
					pTreeNode->m_Tile.set_RenderableTileName(CString(_T("radio_column")));
				nRes = SDS_RTNORM;
				break;
			case LAYOUT_HORIZONTAL:
			default : 
				if ( nResTile == CLUSTER_T)
					pTreeNode->m_Tile.set_RenderableTileName(CString(_T("row")));
				else if ( nResTile == RADIO_CLUSTER_T)
					pTreeNode->m_Tile.set_RenderableTileName(CString(_T("radio_row")));
				nRes = SDS_RTNORM;
				break;
			}
		}
	return nRes;
	}
