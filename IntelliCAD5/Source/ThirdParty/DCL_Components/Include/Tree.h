/** FILENAME:     Tree.h- Contains declarations of class CTree.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract  
*        This class holds a single datamember to the a root node of the tree.
*         
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#ifndef _TREE_H
#define _TREE_H

#include "tree.h"
class ScreenObject;

class TreeNode;

class Tree:public CObject  
{
public:
	int SetRenderableTileNameForCluster(					// Set the renderable tile name for a cluster to row,column depending on the layout.
										TreeNode* pTreeNode, // i : Node to be set.
										int nRes			 // i : Whether this is a cluster or a radio_cluster(CLUSTER_T,RADIO_CLUSTER_T)
										);
	CString pDialogAddress;							// Holds the dialogId when the dialog is shown on the screen.
													// (It is equal to the address of the ScreenObject in the dialog object.)
	int m_nHasFixedWidthChild;						// Set if any child has fixed_width = true else has value 0.	
	int ReAdjust();									// Function that increases the parent's height or width .
													// This is called after AdjustWidthHeight() and before CalcPosition().
	HWND hDialog;							// Holds the dialog's object.
	int Exit();										// Traverse the tree to delete all the existing ScreenObjects.
	void TilesDisplayRect(							// Prints the required parameters to the Debug window.
		TreeNode * pOldNode							// i : Input node.
		);
	void PrintDisplayRects();						// Contains the tree traversal logic and calls
													// TilesDisplayRect() on reaching every node.
	
	
	int PositionDisplayRect(						// Algo 3 makes use of this function.Actual poition of the controls with respect to the top left edge of the dialog is done here.
							TreeNode *pThisNode		// i : Input node.
							);			
	int ReCalcDimensions(							// Algo 2 makes use of this function.Distbuting the width and height should be done here.
						TreeNode* pOldNode			// i : Input node.
						);				
	int LoopAllChildrenToCalcSize(					// Algo 1 makes use of this function.Loop all the Children and set the nodes width and height to that of max(summation,AssignedValue)
								TreeNode* pParentNode // i : Input node.
								);	
	int CreateTreeNodeScreenObj(					// Construct objects for every node but does not create windows window.
		TreeNode * pNode							// i : Input node.
		);
	TreeNode * get_StartTreeNode();					// Get the Root Node of the tree.
	int set_StartTreeNode(							// Set the Root Node of the tree to the specified node.
		TreeNode * pTreeNode						// i : Specified node.
		);		
	
	int get_TreeType();								// Gets the type of the tree.
	int set_TreeType(								// Sets the type of the tree to the specified type.
		int nType									// i : Type of the tree.
		);				
	int Init();										// Create the ScreenObjects only.
	int CreateOnScreenObjects();					// Loop and Call create on every object. 
	int CalcPosition();								// Calculate the actual positions of the control.
	int AdjustWidthHeight();						// Distribute the extra area of the parent within the children.
	int CalcWidthHeight();							// Calculate the width and height of the controls by traversing the tree.
	//Modified SAU 12/07/2000  
	//Reason:DCL does not handle multiple is_cancel attributes correctly
	ScreenObject* GetCancelScrObj();                //This function is used to GET which button(screen object) is pressed
													//when the user presses the cancel key (ESC). 
	Tree(											// constructor of tree  class
		TreeNode *pTreeNode,						
		int nTreeType
		);            
	virtual ~Tree();								// destructor of tree class

private:
	ScreenObject * m_pCancelScrObj;
	int m_nType;					// Holds the type ( 0 - dialog,1 - prototype)
	int IsdefaultSet;				// Set if there is default button in the dialog else value is 0.
	//Modified SAU 12/07/2000   [
	//Reason:DCL does not handle multiple is_cancel attributes correctly
	int Cancel_default;
    int State;
	//Modified SAU 12/07/2000   ]
	TreeNode* m_startTreeNode;      // a pointer to the starting node of the tree.
	CMapStringToPtr * m_keytoObject;	
	

	friend class AuditLevelProcessor;
	friend class TreeDebugger;
	friend class CDCLDoc;
};

#endif 
