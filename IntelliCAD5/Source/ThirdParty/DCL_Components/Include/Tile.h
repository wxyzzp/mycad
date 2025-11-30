/** FILENAME:     Tile.h- Contains declarations of  Tile class.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
*  Abstract 
*         This class represents the tiles defined in the DCL file.  
*  This class is  a datamember of the TreeNode class.The data stores in this
*  class include  
*  a. Tile properties giving information like Containable,Renderable...etc.., 
*  b. A list which contians a list of pointers to all the attributes set for the tile
*  
*  Audit Log 
*
*
*  Audit Trail
*
*
*/



#ifndef _TILE_H
#define _TILE_H

#include "listofattributes.h"


class ScreenObject;
class TreeDebugger;
class TreeNode;
class TileAttributes;

class Tile : public CObject  
{
public:
	TreeNode* m_pTreeNode;
	Tile (TreeNode* parent);
	Tile();

	int RemoveReferencedTile();                            // Used to remove referenced tile from list once its attributes have been cloned.

	CString GetTileNameToBeCloned();                       // Returns the value tilename to be cloned.

	int AddReferencedTile(                                  // Adds a tilename to the list.
		       CString &wszTileName							// Name of referenced tile.
			   );
	int set_ScreenObject(ScreenObject * pScreenObj);
	ScreenObject * get_ScreenObject();
	int set_RenderableTileName(                             // Sets the value of m_wszRenderableTileName.
		  CString& wszRenderableTileName                    // i: Renderable Tile name.
		  );

	int AddAttributes(                                     // Adds the attributes list to the current tile.
		         TileAttributes *pTileAttributes,          // i: Attributes to be added.
				 int nReferenceCount                       // i: 0 - for first reference,1 - for reference before definition,2 - for multiple reference before definition found
				  );

	int set_DialogDefinition(                              // Sets the value of m_bDialogDefinition
		         BOOL  bIsDialogDefinition                 // i:TRUE/FALSE
				 );

	BOOL IsContainer();                                    // Returns TRUE/FALSE(value of m_bContainer)  

	int set_Container(                                     // Sets the value of m_wszRenderableTileName.
		         BOOL bIsContainer                         // i: TRUE/FALSE.
				 );

	BOOL IsRenderable();                                   // Returns TRUE/FALSE for m_bRenderable.
	
	int set_Renderable(                                    // Sets the value of m_wszRenderableTileName. 
		         BOOL bIsRenderable                        // i: TRUE/FALSE.
				 );
   
	int set_TileName(                                      // Sets the value of m_wszTileName. 
		         CString &wszTileName                      // i: TileName.
				 );

	CString get_TileName();                                // Returns the value of m_wszFunctionName.
   
	int set_FunctionName(                                  // Sets the value of m_wszFunctionName.
		         CString &wszFunctionName                  // i:Function Name.
				 );
	
	CString get_FunctionName();                            // Returns the value of m_wszFunctionName.
		
	int operator =(										   // Assignment operator overloaded to copy tile.
		       Tile &                                      // i: Reference to tile to be copied.
				 );               
	               
	CString get_RenderableTileName ();                     // Returns the TileName.
	
	BOOL IsRenderable (									   // Returns TRUE if tile is renderable or FALSE.
		       CString&						               // Reference to the string(tilename)to be checked.  
			     );  
	
	int CopyAttributes(									   // Prepares a list of attributes from a node to be cloned.
		     CString& wszFuncName,                         // i: The name of root definition. 
		     Tile& tileAttributeSource,					   // i:Reference to list of attributes to be cloned.  
             int nReferencedCount						   // i: 0  - for first reference , 1 - more than one reference.
		        ) ;											
															
	int CopyTilePropertyOnly(							   // Method to copy tile properties alone.
		        Tile& RefTile							   // i: Reference to tile to be copied.
				        ) ;									
	ListOfAttributes * get_ListOfAttributes();				// Method to get the list of Attributes object pointer
															

	Tile(CString &wszTileNameReferenced);			       // Constructor of Tile class.
	virtual ~Tile();									   // Destructor of Tile class.
															
															
private:													
															
	static CString m_ArrayOfRendTiles[];				   // Aan array of names of all the renderable tiles.
	static int m_nArrayCount;							   // Total number of elements in the array = 23.
	ScreenObject* m_pScreenObject;						   // On-screen object.
	ListOfAttributes m_ListOfAttributes;                   // List of attributes.
	CString m_wszTileName;                                 // Name of the tile.
	CString m_wszFunctionName;					           // Function name.
	CString m_wszRenderableTileName;			           // Renderable tile name.
	BOOL m_bRenderable;                                    // TRUE if renderable.
	BOOL m_bContainer;							           // TRUE if its a container.
	BOOL m_bDialogDefinition;                              // TRUE if a dialog definition. 
	CStringList m_ListOfReferencedTiles;                      // Has list of referenced tiles.
                                                           // Used to keep the ordeer in which attributes are colend.

	
	friend class TreeDebugger;                             // Added for debugging purpose, to be removed in final build.
	friend class CCtrlListAttributes;                      // Added for debugging purpose, to be removed in final build. 
	friend class CDCLDoc;                                  // Added for debugging purpose, to be removed in final build. 
	friend class CDCLView;                                 // Added for debugging purpose, to be removed in final build. 

};

#endif 

