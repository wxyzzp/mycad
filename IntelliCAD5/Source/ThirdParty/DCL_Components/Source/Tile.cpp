/** FILENAME:     Tile.cpp- Contains implementation of class Tile.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
* Abstract 
*      This class represents the tiles defined in the DCL file.  
*      This class is  a datamember of the TreeNode class.The data stores in this
*      class include  
*      a. Tile properties giving information like Containable,Renderable...etc..,  Log 
*      b. A list which contians a list of pointers to all the attributes set for the tile
*
* Audit
*
* Audit Trail
*
*
*/


#include "stdafx.h"
#include "tile.h"
#include "tileattributes.h"
#include "sds.h"
#include "exceptionmanager.h"
#include "auditlevel.h"
#include "constants.h"
#include "ScreenObject.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning (disable : 4355) 

CString Tile::m_ArrayOfRendTiles [] = {
							_T("boxed_column"),
							_T("boxed_row"),
							_T("boxed_radio_row"),
							_T("boxed_radio_column"),
							_T("button"),
							_T("column"),
							_T("concatenation"),
							_T("dialog"),
							_T("edit_box"),
							_T("image"),
							_T("image_button"),
							_T("list_box"),
							_T("paragraph"),
							_T("popup_list"),
							_T("radio_button"),
							_T("radio_column"),
							_T("radio_row"),
							_T("row"),
							_T("slider"),
							_T("spacer"),
							_T("toggle"),
							_T("text"),
							_T("text_part")
							};
int Tile:: m_nArrayCount = 23;



/************************************************************************************
/* + Tile::Tile() - Constructor of Tile class
*
*
*
*
*
*/
Tile::Tile(CString& wszTileName):m_ListOfAttributes(this)            // Constructor of Tile class.
	{
	m_bContainer = 0;
	m_bDialogDefinition = 0;
	m_bRenderable = 0;
	m_wszTileName = _T("");
	m_wszFunctionName = _T("");
	m_wszRenderableTileName = _T("");
	m_pScreenObject = NULL;
	// Adds the referenced tile name to the list of referenced tiles
	AddReferencedTile(wszTileName);
	}

/************************************************************************************
/* + Tile::~Tile() - Destructor of Tile class.
*
*
*/
Tile::~Tile()        // Destructor of Tile class.
	{
	
	}

/***********************************************************************************************
* + BOOL Tile::IsRenderable( CString& wszName ) - Checks whether tile is renderable.
*          
*        Returns TRUE if tile is renderable or FALSE.
*
*
*/
BOOL Tile::IsRenderable(                                    // Returns TRUE if tile is renderable or FALSE.
						CString& wszName					// Reference to the string(tilename)to be checked.  
						)
	{
	CString szName = wszName;
	m_bRenderable = FALSE;
	for ( int i = 0; i < m_nArrayCount; i++ )
		{  
		// Iterate through the list in the array and return TRUE , if given string is a renderable tile name.
		if ( szName == m_ArrayOfRendTiles[i] )
			{
			m_bRenderable = TRUE;
			break;
			}
		}

	return m_bRenderable;
	}


/************************************************************************************
/* + CString& Tile::get_RenderableTileName() - Returns the TileName
*
*
*/
CString Tile::get_RenderableTileName()  // Returns the TileName.
	{
	return m_wszRenderableTileName;
	}



/************************************************************************************************
/* + int Tile::operator =(Tile& RefTile) - Assignment operator overloaded to copy tile. 
*
*
*/
int Tile::operator =(                      // Assignment operator overloaded to copy tile.
					 Tile& RefTile         // i: Reference to tile to be copied.
					 )
	{

    if (m_wszTileName.IsEmpty()) 
	    m_wszTileName = RefTile.m_wszTileName;
    if (m_wszRenderableTileName.IsEmpty()) 
		m_wszRenderableTileName = RefTile.m_wszRenderableTileName;
	m_bRenderable = RefTile.m_bRenderable;
	m_bContainer = RefTile.m_bContainer;
	return SDS_RTNORM;
	}


/******************************************************************************************************************
/* + int Tile::CopyAttributes(CObArray& ListofAttributesToCopy) - Prepares a list of attributes 
*                                                                 from a node to be cloned 
*
*      This function creates a new attribute object and calls the AddAttribute object
*   of LIstoFAttributes class
*
*/
int Tile::CopyAttributes(                                      // Prepares a list of attributes from a node to be cloned.
						 CString& wszFuncName,                 // i: The name of root definition. 
						 Tile& tileAttributeSource ,		   // i: Reference to list of attributes to be cloned.  
						 int nReferencedCount                  // i: 0  - for first reference , 1 - more than one reference.  
						 )                         
	{
    TileAttributes *pTileAttributes; 
	CObArray ListofAttributesToCopy ;
	int nReturn = SDS_RTNORM;
	AuditLevelProcessor audLvlProc;

	try
		{
		if ((get_FunctionName()).IsEmpty())
			{
			set_FunctionName(tileAttributeSource.get_FunctionName());  
			}
 
		if (nReferencedCount == 0 || nReferencedCount == 1) 
			(*this) = (tileAttributeSource); 
			 
		// Create a attribute array, and copy the attributes of the source tile.
		ListofAttributesToCopy.Copy(tileAttributeSource.m_ListOfAttributes.get_ListAttributes());  

		int iSize = ListofAttributesToCopy.GetSize(); 
         
		// Create attributes objects for all the attributes to be cloned.
		for ( int iIndex = 0 ; iIndex < iSize ; iIndex++ )
			{
			pTileAttributes = new TileAttributes
								   (
								   ((TileAttributes *) ListofAttributesToCopy[iIndex])->get_AttributeName(), 
								   ((TileAttributes *) ListofAttributesToCopy[iIndex])->get_AttributeType(),
								   ((TileAttributes *) ListofAttributesToCopy[iIndex])->get_AttributeValue()
								   );

							 
			// Call AddAttribute method to add or replace attributes.
			nReturn = m_ListOfAttributes.AddAttribute(pTileAttributes,nReferencedCount);

			if ( nReturn == SDS_RTERROR )
				{
				audLvlProc.FormatHintForRedAttr(wszFuncName,
												tileAttributeSource,
					                            (TileAttributes *) ListofAttributesToCopy[iIndex]);
				}

            if ( pTileAttributes->get_AttributeType() = KEYWORD_DATA_TYPE )
				{
				if ( audLvlProc.CheckForRedundancy(pTileAttributes) == SDS_RTERROR )
					{
					audLvlProc.FormatHintForRedAttr(wszFuncName,
												tileAttributeSource,
					                            (TileAttributes *) ListofAttributesToCopy[iIndex]);

					}
				}




			}
		this->RemoveReferencedTile(); 
		return nReturn;
		}
	catch(...)
		{
		TRACE("Error during copying of attributes");
		return FALSE;
       	}

	
	}


/************************************************************************************************
/* + int Tile::CopyTilePropertyOnly(Tile& RefTile) - Method to copy tile properties alone  
*
*
*/
int Tile::CopyTilePropertyOnly(                       // method to copy tile properties alone 
					            Tile& RefTile         // i: reference to tile to be copied
					            )
	{  
    if (m_wszTileName.IsEmpty()) 
		m_wszTileName = RefTile.m_wszTileName;
	if (m_wszRenderableTileName.IsEmpty()) 
		m_wszRenderableTileName = RefTile.m_wszRenderableTileName;
	m_bRenderable = RefTile.m_bRenderable;
	m_bContainer = RefTile.m_bContainer;
    
	return SDS_RTNORM;
	}


/************************************************************************************
/* + CString Tile::get_FunctionName() - Returns a the function name.
*
*
*/
CString Tile::get_FunctionName()                   // Returns a the function name.
	{
	return m_wszFunctionName;
	}


/*******************************************************************************************************
/* + int Tile::set_FunctionName(CString& wszFunctionName) - Sets the value of m_wszFunctionName.
*
*
*/
int Tile::set_FunctionName(                                        // Sets the value of m_wszFunctionName.
						   CString& wszFunctionName				   // i:Function Name.
						   )
	{
    m_wszFunctionName =  wszFunctionName;
	return SDS_RTNORM;
	}


/************************************************************************************
/* + CString Tile::get_TileName() - Returns the value of m_wszTileName.
*
*
*/
CString Tile::get_TileName()             // Returns the value of m_wszTileName.
	{
	return m_wszTileName;
	}



/************************************************************************************
/* + int Tile::set_TileName(CString& wszTileName) - Sets the value of m_wszTileName.
*
*
*/
int Tile::set_TileName(                                 // Sets the value of m_wszTileName. 
					   CString& wszTileName				// i: TileName.
					   )
	{
	m_wszTileName = wszTileName;
	return SDS_RTNORM;
	}


/*************************************************************************************************
/* + int Tile::set_Renderable(BOOL bIsRenderable) - Sets the value of m_bRenderable
*
*
*/
int Tile::set_Renderable(                               // Sets the value of m_wszRenderableTileName.
						 BOOL bIsRenderable				// i: Renderable Tile name.
						 )
	{
    m_bRenderable = bIsRenderable;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + BOOL Tile::IsRenderable() - Returns the value of m_bRenderable.
*
*
*/
BOOL Tile::IsRenderable()              // Returns the value of m_bRenderable.
	{
	return m_bRenderable;
	}


/************************************************************************************
/* + int Tile::set_Container(BOOL bIsContainer) - Sets the value of m_bContainer
*
*
*/
int Tile::set_Container(                                      // Sets the value of m_wszRenderableTileName.
						BOOL bIsContainer					  // i: TRUE/FALSE.
						)
	{
	m_bContainer = bIsContainer;
	return SDS_RTNORM;
	}



/************************************************************************************
/* + BOOL Tile::IsContainer() - Returns the TRUE/FALSE value of m_bContainer
*
*
*
*/
BOOL Tile::IsContainer()               //  Returns the TRUE/FALSE value of m_bContainer
	{
	return m_bContainer;
	}



/**************************************************************************************************************
/* + int Tile::set_DialogDefinition(BOOL bIsDialogDefinition) - Sets the value of m_bDialogDefinition
*
*
*/
int Tile::set_DialogDefinition(                                    // Sets the value of m_bDialogDefinition
							   BOOL bIsDialogDefinition			   // i:TRUE/FALSE
							   )
	{ 
	m_bDialogDefinition = bIsDialogDefinition;
	return SDS_RTNORM;
	}



/*******************************************************************************************************************
/* + int Tile::AddAttributes( TileAttributes *pTileAttributes,int nReferenceCount) 
*       
*           Adds the Attribute to the tiles list.
*
*/
int Tile::AddAttributes(                                            // Adds the attributes list to the current tile.
						TileAttributes *pTileAttributes,			// i: Attributes to be added.
						int nReferenceCount							// i: 0 - for first reference,1 - for reference before definition,2 - for multiple reference before definition found
						)
	{
    return (this->m_ListOfAttributes.AddAttribute(pTileAttributes,nReferenceCount));
	
	}



/**************************************************************************************************************
/* + int Tile::set_RenderableTileName(CString& wszRenderableTileName) - Sets the value of m_wszRenderableTileName
*
*
*
*/
int Tile::set_RenderableTileName(                                      // Sets the value of m_wszRenderableTileName. 
								 CString& wszRenderableTileName		   // i: Renderable Tile name.
								 )
	{
	m_wszRenderableTileName = wszRenderableTileName;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + ListOfAttributes * Tile::get_ListOfAttributes() - 
*
*     Get a pointer ti the list of m_ListOfAttributes.
*
*
*
*/
ListOfAttributes * Tile::get_ListOfAttributes()         //  Get a pointer ti the list of m_ListOfAttributes.
	{
	return (&m_ListOfAttributes);
	}


/************************************************************************************
/* + ScreenObject * Tile::get_ScreenObject() - 
*
*     Get the value of m_pScreenObject.
* 
*
*
*/
ScreenObject * Tile::get_ScreenObject()      // Get the value of m_pScreenObject.
	{
	return m_pScreenObject;
	}


/************************************************************************************
/* + int Tile::set_ScreenObject(ScreenObject *pScreenObj)  
*
*     Sets the m_pScreenObject.
*
*
*
*/
int Tile::set_ScreenObject(                                      // Sets the m_pScreenObject.
						   ScreenObject *pScreenObj              // i: Screen Object pointer
						   )
{
	if ( m_pScreenObject )
	{
		try
		{
			delete m_pScreenObject;
		}
		catch(...)
		{
			//NOTE
			// I don't know why we have these exceptions, it happens while
			// deleting an EditBox, MFC throws an access violation in ~CCmdTarget,
			// file CMDTARG.CPP, line 51 : ((COleDispatchImpl*)&m_xDispatch)->Disconnect();
			TRACE(_T("failed deleting ScreenObject in Tile"));
		}
	}
	m_pScreenObject = pScreenObj;
	return SDS_RTNORM;
}


/**********************************************************************************************************
/* + int Tile::AddReferencedTile(CString &wszTileName) - 
*        
*           Adds a tilename to the m_ListOfTilesReferenced.The addition is done to the head of the list.
*     The value of the head in the list decides whether the tile's attributes have to be copied or not.
*
*
*/
int Tile::AddReferencedTile(                              // Adds a tilename to the list.
							CString &wszTileName          // Name of referenced tile.
							)
	{
	try
		{
		    // Add the referecned tile name to the list
			this->m_ListOfReferencedTiles.AddTail((LPCTSTR)wszTileName);
		}
	catch(...)
		{
		 ThrowCExceptionManager(LIST_INSERT_FAILED);
		}
				
	return SDS_RTNORM;
	}



/************************************************************************************
/* + CString Tile::GetTileNameToBeCloned() - 
*
*      Returns the value tilename to be cloned.
*
*
*
*/
CString Tile::GetTileNameToBeCloned()  // Returns the value tilename to be cloned.
	{
    CString wszTileName;
    if ( this->m_ListOfReferencedTiles.GetCount() > 0 )      
		{    
		// Return the tile name to be cloned
		wszTileName = (LPCTSTR)m_ListOfReferencedTiles.GetHead(); 
		return wszTileName;
		}
	else
		{
		return _T("");
		}

	}

/************************************************************************************
/* + int Tile::RemoveReferencedTile() - 
*
*        Removes the referenced tile entry from the list.This method removes the first 
*    entry found in the list.
*
*
*/
int Tile::RemoveReferencedTile()          // Removes the referenced tile entry in the list.
	{
	if ( this->m_ListOfReferencedTiles.GetCount() > 0 )      
		{    
		// Once the tile has been cloned then remove it from list.
		this->m_ListOfReferencedTiles.RemoveHead(); 
		}
	else
		{
		return SDS_RTERROR;
		}

	return SDS_RTNORM;
	}

/************************************************************************************
/* + Tile::Tile() - Constructor of Tile class
*
*
*
*
*
*/
Tile::Tile():m_ListOfAttributes(this)            // Constructor of Tile class.
	{
	m_bContainer = 0;
	m_bDialogDefinition = 0;
	m_bRenderable = 0;
	m_wszTileName = _T("");
	m_wszFunctionName = _T("");
	m_wszRenderableTileName = _T("");
	m_pScreenObject = NULL;
	}


Tile::Tile(TreeNode *parent):m_ListOfAttributes(this)
{
	m_pTreeNode = parent;
	m_bContainer = 0;
	m_bDialogDefinition = 0;
	m_bRenderable = 0;
	m_wszTileName = _T("");
	m_wszFunctionName = _T("");
	m_wszRenderableTileName = _T("");
	m_pScreenObject = NULL;
}
