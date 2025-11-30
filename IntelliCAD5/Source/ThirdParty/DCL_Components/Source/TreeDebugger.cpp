/** FILENAME:     TreeDebugger.cpp- Contains implementation of class TreeDebugger.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*      This class has methods which can be used to wite information about the oarsed tree
*  to a text file.
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
#include "tileattributes.h"
#include "tree.h"
#include "constants.h"
#include "sds.h"

#include "treedebugger.h"


/*********************************************************************************************
/* + void TreeDebugger::DisplayAttributes(Tile *tile) - Displays the attributes of the file 
*
*
*
*
*
*/
void TreeDebugger::DisplayAttributes(                    // display the attributes of the file 
									 Tile *tile          // i: tile for which the attribute has to be displayed 
									 )
	{
    // the list of attributes for the passed tile pointer is displayed
	int iCount = tile->m_ListOfAttributes.m_ListAttributes.GetSize();
	FILE *fp = fopen(TEST_OUT,"a+");

	if ( !fp )
		{
		TRACE("Unable to open test file");
		return;
		}

	fprintf(fp,"\n");
       
	for(int  iIndex=0 ;  iIndex < iCount ;  iIndex++)
		{
		// get each attribute object and display
        TileAttributes *pTileAttributes = (TileAttributes *)
			             tile->m_ListOfAttributes.m_ListAttributes.GetAt(iIndex);
            
		fprintf(fp,"\t\t"); 
        fprintf(fp,"%s = %s ; %s \n",
				    pTileAttributes->get_AttributeName(),  
		            pTileAttributes->get_AttributeValue(),
					pTileAttributes->get_AttributeType()
					);

		}
        fclose(fp);

	}

	   
	 

/************************************************************************************
/* + void TreeDebugger::DisplayTreeNodeDetails(TreeNode *pOldNode) - 
*         Called the  ParseandDisplay method for each  node to be displayed
*
*
*
*
*/
void TreeDebugger::DisplayTreeNodeDetails(                           // called the  ParseandDisplay method for each  node
										  TreeNode *pOldNode         // node to be  displayed
										  )
	{
	  
 	FILE *fp = fopen(TEST_OUT,"a+");

	if ( !fp )
		{
		TRACE("Unable to open test file");
		return;
		}

	// the list of tile properties for the passed tile pointer is displayed 
	fprintf(fp,"\n\n");
	fprintf(fp,"FunctionName :  %s\n", pOldNode->m_Tile.m_wszFunctionName); 
	fprintf(fp,"TileName :  %s\n", pOldNode->m_Tile.m_wszTileName); 
	fprintf(fp,"RenderableTileName :  %s\n", pOldNode->m_Tile.m_wszRenderableTileName); 
	if (pOldNode->get_Parent())
		{
        fprintf(fp,"Parent :  %s\n", (pOldNode->get_Parent())->m_Tile.m_wszTileName);  
		}

	fprintf(fp,"Contianer :  %d\n",pOldNode->m_Tile.m_bContainer);
    fprintf(fp,"DialogDefinition :  %d\n", pOldNode->m_Tile.m_bDialogDefinition); 
    fprintf(fp,"Renderable :  %d\n", pOldNode->m_Tile.m_bRenderable ); 
    fprintf(fp,"Completed :  %d\n", pOldNode->m_bIsComplete); 
	fclose(fp);

	DisplayAttributes(&(pOldNode->m_Tile)); 

	}


/******************************************************************************************************
/* + void TreeDebugger::ParseandDisplay(TreeNode *pTreeNode) - 
*          This method traverses the tree and calls the DisplayTreeNodeDetails method for each node   
*
*    This method traverses the tree in TOP-TO-BOTTOM and LEFT-TO-RIGHT direction and calls the 
*  DisplayTreeNode details method for each node.
*
*/
void TreeDebugger::ParseandDisplay(                              //  this method traverses the tree    
								   TreeNode *pTreeNode           // i:pointer to node from where traversing starts
								   )
	{
	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;                          // holds previous value of pOldNode
	
	pOldNode = pTreeNode ;                                 
    
	bHasChildren  =  pOldNode->HasChild();                // check whether child is present

	if( !bHasChildren )
		{
		DisplayTreeNodeDetails(pOldNode);           // display tree nodes details for current node
        return;                                     // return ,if only one node in tree to be traversed
		}
	else
		{
		DisplayTreeNodeDetails(pOldNode);             // display tree nodes details for current node
		}

	while(1)
		{
		bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
             
		if ( bHasChildren )
			{ 
			pOldNode = pOldNode->GetFirstChild();           // get the first child
			DisplayTreeNodeDetails(pOldNode);                // display tree nodes details for current node
			}// end of if(bHasChildren)
        else
			{ 
		    // if child not found go to the next sibling
		 	TempOldNode = pOldNode ;                      
			if ( pOldNode->get_Parent() != NULL )
				pOldNode = (pOldNode->get_Parent())->GetNextChild() ;
			else
				pOldNode = NULL;

            if ( pOldNode != NULL )
				{
		        DisplayTreeNodeDetails(pOldNode);
				}		
			else
				{
				// if sibling noit found go to parent
				pOldNode = TempOldNode;
				pOldNode = pOldNode->get_Parent();
			                    
				while (1)
					{//  to set next sibling after child level siblings have been traversed
				   
					if ( pOldNode->get_Parent() )
						TempOldNode = (pOldNode->get_Parent())->GetNextChild();
					else
						TempOldNode = NULL;
                   
					// keep moving up the tree till sibling is found
					if ( TempOldNode != NULL )
						{
						pOldNode = TempOldNode;
						DisplayTreeNodeDetails(pOldNode);		
						break;
						}
					else
						{
						// break if the traversing operation reaches where it started 
						if ( pOldNode == pTreeNode )
							break;
                     					 
						pOldNode = pOldNode->get_Parent();
					 
						if ( pOldNode == pTreeNode )
			             break;
						}
					}// to set next sibling after chilren level siblings have been traversed--end	   

	           
				}// end check for has child

				if(pOldNode == pTreeNode)
					break;
			}
		} 
	}


/************************************************************************************
/* + void TreeDebugger::LogParsedTree(FileInfo *pFileInfo)  - 
*               Calls the ParseandDisplay method for each definiton in the DCL File
*
*
*
*
*/
void TreeDebugger::LogParsedTree(                                  // the file whose definitions are parsed
								 FileInfo *pFileInfo               // i: pointer to Fileinfo holding the file name
								 ) 
	{
	Tree *pTree;
	TreeNode *pTreeNode;
	int iTemp;
    int iCount =  pFileInfo->m_RootNodes.GetCount(); 
	FILE *fp;
       
	if ( !iCount ) 
		return ;
	   
	POSITION position = pFileInfo->m_RootNodes.GetHeadPosition(); 
	    
	for(int iIndex=0 ; iIndex < iCount ; iIndex++) 
		{
        pTree = ((Tree*)pFileInfo->m_RootNodes.GetNext(position)); 
		pTreeNode = pTree->m_startTreeNode ;
		fp = fopen(TEST_OUT,"a+");
        fprintf(fp,"\n****New TreeNode***\n");

		if ( !fp )
			{
			TRACE("Unable fo open Test.out");
			break;
			}

		iTemp = pTree->m_nType;
		fprintf(fp,"%d",pTree->m_nType);
		fclose(fp);
		ParseandDisplay(pTreeNode);
		}
	  	   
	}	  
