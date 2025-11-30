/** FILENAME:     TreeDebugger.h- Contains declarations of class TreeDebugger.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract  
*           This class has methods to output the details of parsed tree to
* a file .The file name constants are defined in "Constants.h"
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#ifndef _TREEDEBUGGER_H
#define _TREEDEBUGGER_H


#include "stdafx.h"

class Tile;
class TreeNode;
class FileInfo;


class TreeDebugger
{

public:


void DisplayAttributes(                                  // display the attributes of the file 
					   Tile *tile                        // i: tile for which the attribute has to be displayed 
					   );

void DisplayTreeNodeDetails(                            // called the  ParseandDisplay method for each  node 
							TreeNode *pOldNode          // node to be displayed
							);

void ParseandDisplay(                                  //  this method traverses the tree 
					 TreeNode *pTreeNode               // i:pointer to node from where traversing starts
					 );

void LogParsedTree(                                   // the file whose definitions are parsed
				   FileInfo *pFileInfo                // i: pointer to Fileinfo holding the file name
				   ); 

};


#endif
