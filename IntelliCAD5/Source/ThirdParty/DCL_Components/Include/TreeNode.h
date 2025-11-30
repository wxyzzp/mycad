/* FILENAME:     TreeNode.h- Contains declarations of class TreeNode.
*  Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*  Abstract :  
*       This class represents each node in the parsed tree.It stores
*  pointers to its parent and child nodes and also has methods  to     
*  retieve the same. 
* 
*  Audit Log 
*
*
*  Audit Trail
*
*
*/

#ifndef _TREENODE_H
#define _TREENODE_H

#include "tile.h"

class TreeDebugger;
class FileInfo;
class Tile;

class TreeNode : public CObject 
{
public:
	int CalcHorSpaceExp();			// Find how much the tile after a spacer0 should move ahead of the spacer. 
	int m_nHorSpaceExpand;			// Variable that holds the value of the above functions calculation.A child will check this to find how much to move after the space_0.
	int IsNodeACluster();			// To Test whether the node is a cluster or something derived from it.
	int CopyParentsAction();        // Copy the parent's action to the children.
	int CheckItorNot();				// Takes care of checking only the last radio_button in a parent if other radio_buttons also have value set to 1.
	int m_nAllSwatches;				// Variable set to 1 if all its children are swatches.
	int IsNodeASwatch();			// To Test whether the node is swatch or something derived from it.
	int m_nSummatedHeight;			// A parent node stores children summated heights.
	int m_nChildDiffWidth;			// Extra difference that needs to be added to the parent.
	int m_nSummatedWidth;			// A parent node stores children summated widths.
	
	CString get_DefinitionName();
	int get_DeltaY();				// Get the deltaY value.
	int get_DeltaX();				// Get the deltaX value.
	int set_DeltaY(					// Set the deltaY value to the specified value.
		int ndeltay					// i : specified value.
		);
	int set_DeltaX(					// Set the deltaX value to the specified value.
		int ndeltax					// i : specified value.
		);

	int get_LastAccessedChild();	 // Retrieves the index of the last accessed child.
	int set_LastAccessedChild(		 // Function to set the LastAccessed Child to the specified index.
				int iIndex			 // i : Index of the child
				);
	int GetChildIndex(				 // Gets the zero based index of the specified child in the list.
				TreeNode *pChildNode // i : ChildNode pointer.
				);
	BOOL IsComplete();               // Returns TRUE/FALSE value of m_bIsComplete

	int set_TreeNodeType(			 // Sets the value of m_nType 
				int nType            // i: Type 0 - for dialog, 1 - for subassembly/prototype
				);

	int get_TreeNodeType();          // Returns the value of m_nType.

	TreeNode* get_Parent();          // Returns a pointer to the parent node.
	
	TreeNode* GetFirstChild();       // Returns a pointer to the first child node.
	
	TreeNode* GetLastChild ();       // Returns a pointer to last child node.
	
	TreeNode* GetNextChild ();       // Returns a pointer to the next child node.
	                                 // (ie this count is maintained in "m_iLastAccessedChild" member.
	
	TreeNode* GetPreviousChild ();   // Returns a pointer to the previous child node.
	                                 // (ie this count is maintained in "m_iLastAccessedChild" member.
	
	TreeNode* GetChild (             // Returns a pointer to the treenode based on index.
		                int iIndex   // i: The subscript value for the child treenode to be returned.
						); 
	
	int AddChild(                    // Adds a child to the current tree node.
		         TreeNode * pSibling // i: Pointer to child tree node to be added.
				 );
	
	BOOL HasChild ();                // Returns a BOOL stating whether the current node has a child or not.  
	
	int GetChildCount ();            // Returns the child count.
	
	TreeNode& operator =(            // Assignment operator overloaded for treenode,returns a reference to treenode.
		  TreeNode &pTreeNodeToClone // i: Reference to tree node to be copied.
		    	); 
	TreeNode();                      // Consrtuctot of treenode.

	virtual ~TreeNode();             // Destructor of tree node.

	int CloneTree(                     // This method clones the complete tree starting with the the root passed as reference.
		TreeNode* pReferencedTreeNode, // i: A pointer to the root of tree to be cloned. 
		int   nReferenceCount          // i: 0 for first reference ,and 1 - reference not found after parsing,2- multiple references. 
                            );

	int set_IsComplete(                // Sets the value of m_bIsComplete
				BOOL bIsComplete       // i: TRUE/FALSE  
				);
	Tile m_Tile;                       // Holds the data of the tile 

private:
	int m_nDeltaY;						// Extra expansion for keeping vertical spaces between tiles.
	int m_nDeltaX;						// Extra expansion for keeping horizontal spaces between tiles.
	int m_nType;                       // Holds the value 
	                                   // 0 for dialog , 1 for protoype or subassembly
	BOOL m_bIsComplete ;               // Holds the status of node , whether completed or not
protected:
	
	TreeNode* m_pParentNode;           // Pointer to the parent node,if parent node does not exist then NULL 
	CObList m_listSiblings;            // List containing pointers to the child nodes
	int m_iChildCount;                 // Holds the number of children under the node 
	int m_iLastAccessedChild;          // Keeps track of the lastt accessed child


	friend class Grammer;			   // The Grammer class is a friend of this class.
	friend class FileInfo;             // Added for debugging purpose,to be removed for final build
	friend class TreeDebugger;         // Added for debugging purpose,to be removed for final build

};

#endif 
