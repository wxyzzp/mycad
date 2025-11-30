/** FILENAME:     ListOfAttributes.cpp- Contains implementation of class CListOfAttributes.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*		This holds the list of all attributes for a tile.This class is a data member
*  of Tile class
*
*	NOTE :- 1 ) frist the attribute is looked in the list m_ListAttributes which holdes
*			the attributes set in the dcl file
*			2 )  if not found int the dcl file array then the default Attributes list is looked
*				if not found then SDS_RTERROR is returnrd
*
* Audit Log 
*
*
* Audit Trail
*
*
*/

#include "stdafx.h"
#include "ListOfAttributes.h"
#include "TileAttributes.h"
#include "sds.h"
#include "Constants.h"
#include "Main.h"
#include "Tile.h"
#include "ExceptionManager.h"
#include "TreeNode.h"
#include "ScreenObject.h"
#include "auditlevel.h"
#include "math.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern Main g_Main;



/************************************************************************************
/* + ListOfAttributes::ListOfAttributes()
*	this function Constructes the ListOfAttributes object
*/

ListOfAttributes::ListOfAttributes():m_IsDefault(0)
	{
	
	}
/************************************************************************************
/* + ListOfAttributes::ListOfAttributes()
*
*	this function Constructes the ListOfAttributes object
*
*
*
*/

ListOfAttributes::ListOfAttributes(
								   Tile * ptile // i:pointer to tile object
								   ):m_IsDefault(0)
	{
	m_pTile=ptile;
	m_nCompress= 1;


	}
/************************************************************************************
/* + ListOfAttributes::ListOfAttributes()
 *	this function Destructes the ListOfAttributes object
 */

ListOfAttributes::~ListOfAttributes()
	{

//	POSITION position = m_ListAttributes.GetHeadPosition();
	for ( int index = 0; index < m_ListAttributes.GetSize(); index++)
		{
    		TileAttributes* pTileAttributes = (TileAttributes*) m_ListAttributes.GetAt(index);
			delete pTileAttributes;
		}
		m_ListAttributes.RemoveAll();
	}
/************************************************************************************
/* + int ListOfAttributes::AddAttribute(CString name, CString type ,CString value)
*
*	adds a attribute to the attribute list
*	
*	
*		        
*/

int ListOfAttributes::AddAttribute(
								   CString name,	// i: attribute name
								   CString type ,	// i: attribute type
								   CString value	// i: attribute value
								   )
	{
	TileAttributes * pAttrib;
	int Size = m_ListAttributes.GetSize();
	try
		{
		if(Size<=0)
			{
			pAttrib= new TileAttributes(name,type,value);
			m_ListAttributes.Add(pAttrib);
			}
		else
			{
			for(int index=0; index < Size ;index++)
				{
				if( ( (TileAttributes *) m_ListAttributes[index])->get_AttributeName() == name )
					{
					((TileAttributes *) m_ListAttributes[index])->set_AttributeValue(value);
					break;
					}
				}
			if(index==Size)
				{
				pAttrib= new TileAttributes(name,type,value);
				m_ListAttributes.Add(pAttrib);
				}
			}
		}
	catch(...)
		{
		ThrowCExceptionManager(LIST_INSERT_FAILED);
		}
	return(0);
	}


/************************************************************************************
/* + int ListOfAttributes::AddAttribute(TileAttributes * pAttrib,int nReferencedCount) 
*
*	*	adds a attribute to the attribute list
*	
*	     
*
*   If the nReferencedCount is 1, then attributes need only be added if they are not found.
*  Replacing of attributes not neccessary for this value.
*/

int ListOfAttributes::AddAttribute(
								   TileAttributes * pAttrib, //	i: pointer to attribute to be added
								   int nReferencedCount		 // i: 0 - for first reference,1 - for multiple references 
								   )
	{
	ASSERT(pAttrib);
    int nReturn = SDS_RTNORM; 
	int Size = m_ListAttributes.GetSize();
	AuditLevelProcessor audlvl;
	try 
		{
		if(Size<=0)
			{
			m_ListAttributes.Add(pAttrib);
			if ( audlvl.CheckForRedundancy(pAttrib) == SDS_RTERROR )
					{
					g_Main.m_DCLParser.set_HintCount();
//					((TileAttributes *) m_ListAttributes[index])->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());
					pAttrib->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());

					} 
			}
		else
			{   
			// Search for attribute.
			for(int index=0; index < Size ;index++)
				{
				if( ( (TileAttributes *) m_ListAttributes[index])->get_AttributeName() == pAttrib->get_AttributeName() )
					{
					//Modified Cybage SBD 27/08/2001 DD/MM/YYYY [
					//Reason :Fix for Bug No 77834
					//if ( pAttrib->get_AttributeValue()  == ((TileAttributes *) m_ListAttributes[index])->get_AttributeValue() )
					//	{
					//	g_Main.m_DCLParser.set_HintCount();
//						((TileAttributes *) m_ListAttributes[index])->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());
					//	pAttrib->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());
					//	nReturn = SDS_RTERROR;
					//	} 
					//Modified Cybage SBD 27/08/2001 DD/MM/YYYY ]
			  		if ( nReferencedCount == 0 )
						{
//						((TileAttributes *) m_ListAttributes[index])->set_AttributeValue(pAttrib->get_AttributeValue());
						pAttrib->set_AttributeValue(pAttrib->get_AttributeValue());
						}
					else
						{
						pAttrib->set_AttributeValue(((TileAttributes *) m_ListAttributes[index])->get_AttributeValue());
						}

/*					if ( audlvl.CheckForRedundancy(pAttrib) == SDS_RTERROR )
						{
						g_Main.m_DCLParser.set_HintCount();
//						((TileAttributes *) m_ListAttributes[index])->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());
						pAttrib->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());
						} */

//						delete pAttrib;   
					    delete  ((TileAttributes *) m_ListAttributes[index]);
                        m_ListAttributes.RemoveAt(index);   
					
      					break;
            		}
				}
/*			if(index==Size)
				{ */
				m_ListAttributes.Add(pAttrib);
				if ( audlvl.CheckForRedundancy(pAttrib) == SDS_RTERROR )
					{
					g_Main.m_DCLParser.set_HintCount();
//					((TileAttributes *) m_ListAttributes[index])->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());
					pAttrib->set_RedundantInfo(g_Main.m_DCLParser.get_HintCount());

					} 
//				}

			}
		}
	catch(...)
		{
		ThrowCExceptionManager(LIST_INSERT_FAILED);
		}
		return nReturn;
	}
/************************************************************************************
/* + int ListOfAttributes::IsChildFixedHeight()
*
*	Returns the IsChildFixedHeight Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsChildFixedHeight() // Returns the IsChildFixedHeight Attribute of the tile
	{
	CString strRet ;
	int index ,found = 0;

	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("children_fixed_height"))
			{
			strRet = pTileAttrib->get_AttributeValue();
			found = 1;
			m_IsDefault =0;
			break;
			}
		}
	// if not found get from Default Attributes
	if (!found)
		{
		m_IsDefault=1;
		return SDS_RTERROR;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return SDS_RTERROR;

	}
/************************************************************************************
/* + int ListOfAttributes::IsChildFixedWidth() 
*
*	Returns the IsChildFixedWidth Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsChildFixedWidth() // Returns the IsChildFixedWidth Attribute of the tile
	{
	CString strRet ;
	int index, found = 0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("children_fixed_width"))
			{
			strRet = pTileAttrib->get_AttributeValue();
			found = 1;
			m_IsDefault =0;
			break;
			}
		
		}
	// if not found get from Default Attributes
	if (!found)
		{
		m_IsDefault=1;
		return SDS_RTERROR;
		}
	if(strRet == _T("true"))
		{
		return true;
		}
	else if(strRet == _T("false"))
		{
		return false;
		}
	else
		return SDS_RTERROR;
	}
/************************************************************************************
/* + int ListOfAttributes::get_ChildrenAlignment()
*
*	Returns the get_ChildrenAlignment Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::get_ChildrenAlignment() // Returns the get_ChildrenAlignment Attribute of the tile
	{
	CString strRet ;
	int index, found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("children_alignment"))
			{
			strRet = pTileAttrib->get_AttributeValue();
			found = 1;
			m_IsDefault =0;
			break;
			}
		}
	//if not fuund return DEFAULT
	if ( !found )
		{
		m_IsDefault =1;
		return SDS_RTERROR;
		}
	if (strRet == _T("left"))
		{
		return(ALIGN_LEFT);
		}
	else if (strRet == _T("right"))
		{
		return(ALIGN_RIGHT);
		}
	else if (strRet == _T("centered"))
		{
		return(ALIGN_CENTERED);
		}

	else if (strRet == _T("top"))
		{
		return(ALIGN_TOP);
		}
	else if (strRet == _T("bottom"))
		{
		return(ALIGN_BOTTOM);
		}
	else if (strRet == _T("horizontal"))
		{
		return(ALIGN_HORIZONTAL);
		}
	else if (strRet == _T("vertical"))
		{
		return(ALIGN_VERTICAL);
		}
	else
		return SDS_RTERROR;

	}
/************************************************************************************
/* + int ListOfAttributes::get_MinValue()
*
*	Returns the get_MinValue Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::get_MinValue() // Returns the get_MinValue Attribute of the tile
	{
	CString strRet ;
	int index , found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("min_value"))
			{
			found = 1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			int val = atoi(strRet);

            /* {{{ Gurudev - DT: 7/2/99 4:24:33 PM
			if(val>0)
				return(val);
			else 
				return 0;
			Gurudev - Dt:  7/2/99 4:24:37 PM }}} */ 

			// {{{ Gurudev - DT: 7/2/99 4:24:42 PM
			return (val);
            //  Gurudev - Dt:  7/2/99 4:24:55 PM }}}  
			
			}
		
		}
	// if not found return default
	if ( !found)
		{
		m_IsDefault =1;
		return 0;
		}
	return 0;
	}
/************************************************************************************
/* + int ListOfAttributes::get_MaxValue() 
*
*	Returns the get_MaxValue Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::get_MaxValue() // Returns the get_MaxValue Attribute of the tile
	{
	CString strRet ;
	int found =0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( int index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("max_value"))
			{
			found =1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			int val  = atoi(strRet);
            			
			/* {{ Gurudev - DT: 7/2/99 4:07:41 PM
			if(val >0) 
				return(val);
			else 
				return 1000; 
			 Gurudev - Dt:  7/2/99 4:10:34 PM }}} */

			// {{{ Gurudev - DT: 7/2/99 4:12:12 PM
			if ( val == 0 )
				{
				// Return the default value
				return 10000;
				}
			else
				{
				return (val);
				}
			// Gurudev - Dt:  7/2/99 4:12:28 PM }}} 
					
			}
		
		}
	// if not found return default
	if ( !found )
		{
		m_IsDefault =1;
		return 10000; 
		}
	return 10000;
	}
/************************************************************************************
/* + int ListOfAttributes::get_SliderLayOut() 
*
*	Returns the get_SliderLayOut Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::get_SliderLayOut() // Returns the get_SliderLayOut Attribute of the tile
	{
	CString strRet ;
	int found = 0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( int index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("layout"))
			{
			strRet = pTileAttrib->get_AttributeValue();
			found = 1;
			m_IsDefault =0;
			break;
			}
		}
	// if not found return from default attributes list
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("layout"),strRet);
	
		if(error==SDS_RTERROR)
			return ALIGN_HORIZONTAL;
		}
	if (strRet == _T("horizontal"))
		{
		return(ALIGN_HORIZONTAL);
		}

	else if (strRet == _T("vertical"))
		{
		return(ALIGN_VERTICAL);
		}

	else
		return ALIGN_HORIZONTAL;

	

	}
/************************************************************************************
/* + int ListOfAttributes::get_SmallIncrement() 
*
*	Returns the get_SmallIncrement Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::get_SmallIncrement() // Returns the get_SmallIncrement Attribute of the tile
	{
	CString strRet ;
	int index , found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("small_increment"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			int val = atoi(strRet);
			if(val>0)
				return(val);
			else 
				return 1;
			
			}
		
		}
	// if not found return default
	if ( !found )
		{
		// If the attribute was not set, then depending upon the "max_value" and "min_value" values set the
		// small_increment value

		m_IsDefault =1;
		int nVal;
		int nRange = abs(get_MaxValue() - get_MinValue());
		nVal = (int)(ceil(nRange/100));
        if ( nVal < 1 )
			return 1;
		else
			return nVal;
		}
	return 1;

	}
/************************************************************************************
/* + int ListOfAttributes::get_BigIncrement() 
*
*	Returns the get_BigIncrement Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::get_BigIncrement() // Returns the get_BigIncrement Attribute of the tile
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("big_increment"))
			{
			found = 1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			int val = atoi(strRet);
			if(val>0)
				return(val);
			else 
				return 1;
							
			}
		
		}
	// if not found return default
	if ( !found )
		{
		// If the attribute was not set, then depending upon the "max_value" and "min_value" values set the
		// big_increment value
		m_IsDefault =1;
		int nVal ;
		int nRange = abs(get_MaxValue() - get_MinValue());
		nVal = (int)(ceil(nRange/10));
		if ( nVal < 1 )
			{
			return 1;
			}
		else
			{
			return (nVal);
			}
		}

	return 1;
	}
/************************************************************************************
/* + int ListOfAttributes::IsErrorTile()
*
*	Returns the IsErrorTile Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsErrorTile() // Returns the IsErrorTile Attribute of the tile
	{
	CString strRet ;
	int index, found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("is_error_tile"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;		
			}
		
		}
	// if not found return from default list of attributes
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("is_error_tile"),strRet);
		if(error==SDS_RTERROR)
			return false;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return false;


	}
/************************************************************************************
/* + int ListOfAttributes::IsBold()
*
*	Returns the IsBold Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsBold() // Returns the IsBold Attribute of the tile
	{
	CString strRet ;
	int index , found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("is_bold"))
			{
			found = 1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break ;
			}
		
		}
	// if not found return from default attributes
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("is_bold"),strRet);
	
		if(error==SDS_RTERROR)
			return false;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return false;


	}
/************************************************************************************
/* + CString ListOfAttributes::GetTabsList() 
*
*	Returns the GetTabsList Attribute of the tile from the attributes list
*
*
*
*/

CString ListOfAttributes::GetTabsList() // Returns the GetTabsList Attribute of the tile
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("tabs"))
			{
			found = 1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			}
		
		}
	// if not found get from default list of attributes
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("allow_accept"),strRet);
	
		if(error==SDS_RTERROR)
			return CString("");

		else
			return strRet;
		}
	return CString("");
	}
/************************************************************************************
/* + int ListOfAttributes::IsAllowAccept()
*
*	Returns the IsAllowAccept Attribute of the tile from the attributes list
*
*
*
*/

BOOL ListOfAttributes::IsAllowAccept() // Returns the IsAllowAccept Attribute of the tile
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("allow_accept"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// get from default attribute  list
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("allow_accept"),strRet);
	
		if(error==SDS_RTERROR)
			return false;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return false;

	}
/************************************************************************************
/* + int ListOfAttributes::IsDefault()
*
*	Returns the IsDefault Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsDefault() // Returns the IsDefault Attribute of the tile
	{
	CString strRet ;
	int index , found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("is_default"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// get form default attribute list
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("is_default"),strRet);
	
		if(error==SDS_RTERROR)
			return false;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return false;

	}
/************************************************************************************
/* + int ListOfAttributes::IsCanceled()
*
*	Returns the IsCanceled Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsCanceled() // Returns the IsCanceled Attribute of the tile
	{
	CString strRet ;
	int index, found =0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("is_cancel"))
			{
			found = 1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// get from default list of attributes
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("is_cancel"),strRet);
	
		if(error==SDS_RTERROR)
			return false;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return false;

	}
/************************************************************************************
/* + int ListOfAttributes::IsEnabled() 
*
*	Returns the IsEnabled Attribute of the tile from the attributes list
*
*
*
*/

int ListOfAttributes::IsEnabled() // Returns the IsEnabled Attribute of the tile
	{
	CString strRet ;
	int index , found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("is_enabled"))
			{
			found =1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// return from default list of attribute
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("is_enabled"),strRet);
	
		if(error==SDS_RTERROR)
			return true;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return true;

	}


/************************************************************************************
/* + int ListOfAttributes::IsTabstop()
*	Returns the IsTabstop Attribute of the tile from the attributes list
*
*/

int ListOfAttributes::IsTabstop() // Returns the IsTabstop Attribute of the tile
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("is_tab_stop"))
			{
			found =1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
					
			}
		
		}
	// return from default attribute list
	if ( !found )
		{
		m_IsDefault =1;
		int error = g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("is_tab_stop"),strRet);
	
		if(error==SDS_RTERROR)
			return true;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return true;

	}
/************************************************************************************
/* + CString ListOfAttributes::get_Mnemonic()
*	Returns the mnemonic Attribute of the tile from the attributes list
*
*/

CString ListOfAttributes::get_Mnemonic() // Returns the mnemonic Attribute of the tile
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("mnemonic"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			}
		
		}
	// return from default attributes.
	if ( !found )
		{
		m_IsDefault =1;
		int error = g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("action"),strRet);
	
		if(error==SDS_RTNORM)
			return strRet;
		else
			return CString("");
		}
	return CString("");
	}

/************************************************************************************
/* + CString ListOfAttributes::get_Action()
*
*	Returns the name of the callback function from the attributes list
*
*
*
*/

CString ListOfAttributes::get_Action() // Returns the name of the callback function 
	{
	CString strRet ;
	int index, found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("action"))
			{
			found = 1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			
			}
		
		}
	// return default
	if ( !found )
		{
		m_IsDefault =1;
		int error = g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("action"),strRet);
	
		if(error==SDS_RTNORM)
			return strRet;
		else
			return CString("");
		}
	return CString("");
	}

/************************************************************************************
/* + CString ListOfAttributes::get_Key()
*	Returns the Key Attribute of the tile from the attributes list
*
*/

CString ListOfAttributes::get_Key() // Returns the Key Attribute of the tile
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("key"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			
			}
		
		}
	// return default 
	if ( !found )
		{
		m_IsDefault =1;
		int error = g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("key"),strRet);
	
		if(error==SDS_RTNORM)
			return strRet;
		else
			return CString();
		}
	return CString();
	}	

/************************************************************************************
/* + CString ListOfAttributes::get_ValueString()
*	Returns the value Attribute of the tile ,the return type is CString object 
*	from the attributes list
*
*/

CString ListOfAttributes::get_ValueString() // Returns the value Attribute of the tile ,the return type is CString object
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("value"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			
			}
		
		}
	// return default
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("value"),strRet);
	
		if(error==SDS_RTNORM)
			return strRet;
		else
			return CString("");
		}
	return CString();
	}

/************************************************************************************
/* + int ListOfAttributes::get_ValueInt() 
*	Returns the value Attribute of the tile ,the return type is int
*	from the attributes list
*/

int ListOfAttributes::get_ValueInt() // Returns the value Attribute of the tile ,the return type is int
	{
	CString strRet ;
	int index, found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("value"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			if ( !strRet.IsEmpty()  ) 
				{
				return atoi(strRet);
				}
			else
				{
				return SDS_RTERROR;
				}
			
			}
		
		}
	int val;
	// return default
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("value"),val);
	
		if(error==SDS_RTNORM)
			return val;
		else
			return SDS_RTERROR;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + CString& ListOfAttributes::get_Label()
*	Returns the Label Attribute of the tile from the attributes list
*/

CString ListOfAttributes::get_Label() // Returns the Label Attribute of the tile
	{
	static CString strRet ;
	int index , found=0;;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		found=1;
		m_IsDefault =0;
		if ( pTileAttrib->get_AttributeName() == _T("label"))
			{
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			}
			
		}
	// return default
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("label"),strRet);
	
		if(error==SDS_RTNORM)
			return strRet;
		else
			return CString("");
		}
	return (CString(""));
	}

/************************************************************************************
/* + int ListOfAttributes::get_Width()
*	Returns the Width Attribute of the tile from the attributes list
*/

int ListOfAttributes::get_Width() // Returns the Width Attribute of the tile
	{
	CString strRet ;
	int index ,found=0 ;
	int width ; 
	double val;
// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("width"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			m_nCompress=0;
			
			width = pixelX(atof(strRet));
			
			if(width <=0)
				found =0;
			
			break;
			}
		
		}

	// get default
	if ( !found || (strRet == _T("0")) || (strRet.GetAt(0) == _T('-')))
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("width"),val);
		m_nCompress=1;
		if(error==SDS_RTNORM)
			{
			width = pixelX(val);
			}
		else
			return SDS_RTERROR;
		}

	// find the width of label in pixel 
	double	Lable_Width = 0;
	CString Label = get_Label();
	Label.TrimLeft();
	if (!Label.IsEmpty())
		{
		m_nCompress=0;
		CSize size;
		CDC dc;
		dc.CreateIC ("DISPLAY",NULL,NULL,NULL);
		size = dc.GetOutputTextExtent(get_Label());
		Lable_Width = size.cx +24; 
		}
	// depending upon the type control the label width is taken in calculation of width
	
	if(m_pTile->get_RenderableTileName() == _T("edit_box") )
		{
		// in edit box actual width is equal to label width + edit width
		// the factor 12 gives space between the label and edit control.
		int editWidth = pixelX(get_editWidth()) +12;
		int tempwidth ;
		CString Label = get_Label();
		Label.TrimLeft ();
		
		if (Label.IsEmpty())
			{
			if(editWidth<width)
				return (width);
			else
				return (editWidth);

			}

		else
			{
			if(editWidth< 1)
				// if edit width is not specified the default width of edit control is (6*9)
				// 16 and 12 is correction factor to place the control properly
				tempwidth =(int)(Lable_Width + 6*9 - 16);
			else
				tempwidth =(int)(Lable_Width + editWidth - 12 );
			// always return the greatest of the width ie the actual width or the width given in dcl file
			if(tempwidth < width)
				{
				return(width);
				}
			else
				{
				return (tempwidth);
				}
			}
		}
	
		
	if( m_pTile->get_RenderableTileName() == _T("popup_list"))
		{
		// in popup_list actual width is equal to label width + edit width(width of list control)
		// the factor 12 gives space between the label and edit control.
		int editWidth = pixelX(get_editWidth()) + 12;
		int tempwidth ;
		if (get_Label().IsEmpty())
			{
			if(editWidth<width)
				return (width);
			else
				return (editWidth);
			
			}

		else
			{
			// if edit width is not specified the default width of edit control is (6*9)
			if(editWidth<width)
				tempwidth =(int)(Lable_Width +6*9); 
			else
				tempwidth =	(int)(Lable_Width + editWidth);
			// always return the greatest of the width ie the actual width or the width given in dcl file
			if(tempwidth < width)
				{
				return(width);
				}
			else
				{
				return (tempwidth);
				}
			}
		}
	else if(m_pTile->get_RenderableTileName() == _T("list_box"))
		{
		// no change in width of list_box 
		return (width);
		}
	
	else if(m_pTile->get_RenderableTileName() == _T("text") || m_pTile->get_RenderableTileName() == _T("errtile"))
		{
		// factor 24 is used to reduce the extra space between controls 
		// retrun the greatest of( label width or actual width from dcl)
		if((Lable_Width-24) > width)
			{
			width =(int)( Lable_Width - 24) ;
			}
		return (width);
		}
	else if(m_pTile->get_RenderableTileName() == _T("text_part") )
		{
		// factor 24 is used to reduce the extra space between controls 
		// retrun the greatest of( label width or actual width from dcl)
		if((Lable_Width-24) > width)
			{
			width =(int)( Lable_Width - 24) ;
			}
		return (width);
		}
	else if(m_pTile->get_RenderableTileName() == _T("dialog"))
		{
		// retrun the greatest of( label width or actual width from dcl)
		
		if(Lable_Width > width)
			{
			width = (int)(Lable_Width) ;
			}
		return (width - 8);
		}
	else if(m_pTile->get_RenderableTileName() == _T("boxed_row") || m_pTile->get_RenderableTileName() == _T("boxed_column") ||
			m_pTile->get_RenderableTileName() == _T("boxed_radio_row") || m_pTile->get_RenderableTileName() == _T("boxed_radio_column"))
		{
		// factor 12 is used to reduce the extra space  
		// retrun the greatest of( label width or actual width from dcl)
		if((Lable_Width-12) > width)
			{
			width = (int)(Lable_Width - 12) ;
			}
		return (width);
		}
	else if(m_pTile->get_RenderableTileName() == _T("radio_button") || m_pTile->get_RenderableTileName() == _T("toggle"))
		{
		// factor 12 is used to reduce the extra space  
		// retrun the greatest of( label width or actual width from dcl)
		if((Lable_Width-12) > width)
		{
		width =(int) Lable_Width - 12;
		}
		return (width);
		}
	else if(m_pTile->get_RenderableTileName() == _T("button"))
		{
		// factor 12 is used to reduce the extra space  
		// retrun the greatest of( label width or actual width from dcl)
		// Code added for not letting the width of a button be lesser than a 
		// specific value(default value.)
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("width"),val);
		int nDefWidth = 0;
		if ( error == SDS_RTNORM )
			{
			nDefWidth = pixelX(val);
			}
		if (width < nDefWidth)width = nDefWidth;
		if((Lable_Width-12) > width)
			{
			width =(int) Lable_Width -12;
			}
		return (get_buttonWidth(width));
		}
	else if (m_pTile->get_RenderableTileName() == _T("image") || m_pTile->get_RenderableTileName() == _T("image_button") )
		{
		// if height was not found, then check whether the width and aspect ratio is given.
		if ( !found )
			{
			double nAspectRatio,nHeight;
			int nCountOfAttribFound = 0 ;

			// This loop first gets the value of aspect_ratio and height.
			for ( int iLoop = 0; iLoop < m_ListAttributes.GetSize(); iLoop++ )
				{
				TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[iLoop];
				if ( pTileAttrib->get_AttributeName() == _T("aspect_ratio")  )
					{
					nAspectRatio = atof((LPCTSTR)pTileAttrib->get_AttributeValue());
					if (nAspectRatio > 0)
						nCountOfAttribFound ++ ;
					}
				if ( pTileAttrib->get_AttributeName() == _T("height") )
					{
					nHeight = atof((LPCTSTR)pTileAttrib->get_AttributeValue());
					if (nHeight > 0)
					nCountOfAttribFound ++ ;
					}
				if ( nCountOfAttribFound == 2 ) 
					{
					break;
					}
				}

			if ( nCountOfAttribFound == 2 && nAspectRatio > 0 )
				{
				//width  =  nHeight * nAspectRatio ;
				/* {{{ Gurudev - DT: 7/7/99 2:48:37 PM
				width  = (int)(nHeight / nAspectRatio) ;
				 Gurudev - Dt:  7/7/99 2:48:41 PM }}} */ 
				// {{{ Gurudev - DT: 7/7/99 2:48:53 PM
				//2000-10-25 EBATECH(FUTA) [
				//aspectratio has same behavier of AutoCad.
				//width  = (int)(nHeight * nAspectRatio) ;
				width  = (int)(nHeight / nAspectRatio) ;
				//2000-10-25 EBATECH(FUTA) ]
				// Gurudev - Dt:  7/7/99 2:49:01 PM }}}  
				width  = pixelY(width);
				}
	    	}
		}
	else if(Lable_Width > width)
		{
		width =(int) Lable_Width;
		}
	
	 	
		return (width);
	}
/************************************************************************************
/* + int ListOfAttributes::get_Height() 
*	Returns the Height Attribute of the tile from the attributes list
*/

int ListOfAttributes::get_Height()   //	Returns the Height Attribute of the tile
	{
	CString strRet ;
	double height ;
	int index ,found=0;

	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("height"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			int val = (int)atof(strRet);
			if(val<=0)
				found=0;
			break;
			}
		
		}
// get from default even when the value is less then 0
	if ( !found || (strRet == _T("0")) || (strRet.GetAt(0) == _T('-')))
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("height"),strRet);
	
		if(error==SDS_RTERROR)
				return SDS_RTERROR;
		
		
		}
	

	if(m_pTile->get_RenderableTileName() == _T("list_box") )
		{
		// calculate approxcimate height so that it does not clip the string
		// in the list control 
		int temp;
		height = atof(strRet);
		temp = (int)(height/2) ;
		height =((temp * 2) +1)* 14;
	
		if(!get_Label().IsEmpty())
			height +=  14;
		
		}
	//Added to solve the height problem of popup list
	// Start - Santosh
	else if(m_pTile->get_RenderableTileName() == _T("popup_list"))
		{
		// 1.5 is an extea factor to match the height with AUTO cad
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("height"),strRet);
		if ( error == SDS_RTNORM )
			{
			height = pixelY((atof(strRet)));
			}
		}

	// End
	else if(m_pTile->get_RenderableTileName() == _T("edit_box"))
		{
		// 1.5 is an extea factor to match the height with AUTO cad
		height = pixelY((atof(strRet) * 1.5));
		}
	else if(m_pTile->get_RenderableTileName() == _T("paragraph"))
		{
		//  for paragraph the height is 1 char height
		height = pixelY(1);
		}
	else if(m_pTile->get_RenderableTileName() == _T("boxed_column") ||m_pTile->get_RenderableTileName() == _T("boxed_radio_column"))
		{
		// factor 2 is used accomadate the lable in the boxed style
		height = (pixelY(atof(strRet)));
		height += 2;
		}
	else if(m_pTile->get_RenderableTileName() == _T("slider"))
		{
		// in slider the height is fixed in horizontal direction that is ( 1.25 DLU)
		if(get_Layout() == LAYOUT_HORIZONTAL )
			height = (pixelY(1.25));
		else
			height = (pixelY(atof(strRet)));
		}
	else if (m_pTile->get_RenderableTileName() == _T("image") || m_pTile->get_RenderableTileName() == _T("image_button") )
		{
		// if height was not found, then check whether the width and aspect ratio is given.
		if ( !found )
			{
			double nAspectRatio,nWidth;
			int nCountOfAttribFound = 0 ;

			// This loop first gets the value of aspect_ratio and height.
			for ( int iLoop = 0; iLoop < m_ListAttributes.GetSize(); iLoop++ )
				{
				TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[iLoop];
				if ( pTileAttrib->get_AttributeName() == _T("aspect_ratio")  )
					{
					nAspectRatio = atof((LPCTSTR)pTileAttrib->get_AttributeValue());
					if (nAspectRatio > 0)
						nCountOfAttribFound ++ ;
					}
				if ( pTileAttrib->get_AttributeName() == _T("width") )
					{
					nWidth = atof((LPCTSTR)pTileAttrib->get_AttributeValue());
					if (nWidth > 0)
					nCountOfAttribFound ++ ;
					}
				if ( nCountOfAttribFound == 2 ) 
					{
					break;
					}
				}

			if ( nCountOfAttribFound == 2 && nAspectRatio > 0 )
				{
				//2000-10-25 EBATECH(FUTA) [
				//aspectratio has same behavier of AutoCad.
				//height =  nWidth / nAspectRatio ;
				height =  nWidth * nAspectRatio ;
				//2000-10-25 EBATECH(FUTA) ]
				height = pixelX(height);
				}
			else
				height = (pixelY(atof(strRet)));

    		}
		else
			height = (pixelY(atof(strRet)));
		}
	else
		height = (pixelY(atof(strRet)));

	return ((int)height);
	}
/************************************************************************************
/* + int ListOfAttributes::IsFixedHeight()
*
*	Returns the IsFixedHeight Attribute of the tile from the attributes list
*
*
*/

int ListOfAttributes::IsFixedHeight() // Returns the IsFixedHeight Attribute of the tile
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("fixed_height"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// if not found get from the parent else default
	if ( !found )
		{
		m_IsDefault =1;
		TreeNode* parent = m_pTile->m_pTreeNode->get_Parent ();
		if(parent->m_Tile.IsContainer ())
			{
			int childrenfixedHeight ; 
			// getting from parent 
			childrenfixedHeight = (parent->m_Tile.get_ListOfAttributes ())->IsChildFixedHeight();
			if(childrenfixedHeight == SDS_RTERROR)
				{
				int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("fixed_height"),strRet);
				if(error==SDS_RTERROR)
					return false;
				}
			else
				return childrenfixedHeight;
			}
		
		}
	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return false;
	

	}

/************************************************************************************
/* + int ListOfAttributes::IsFixedWidth()
*
*
*	Returns the IsFixedWidth Attribute of the tile from the attributes list
*
*
*/
int ListOfAttributes::IsFixedWidth() // Returns the IsFixedWidth Attribute of the tile
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("fixed_width"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			
			break;
			}
		
		}
	// get from parent if not found in parent then return from default list
	if ( !found )
		{
		m_IsDefault =1;

		TreeNode* parent = m_pTile->m_pTreeNode->get_Parent ();
		if(parent->m_Tile.IsContainer ())
			{
			int childrenfixedWidth ; 
			// looking in parent
			childrenfixedWidth = (parent->m_Tile.get_ListOfAttributes ())->IsChildFixedWidth ();
			if(childrenfixedWidth == SDS_RTERROR)
				{
				// get from default
				int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("fixed_width"),strRet);
				if(error==SDS_RTERROR)
					return false;
				}
			else
				return childrenfixedWidth;
			}
		}

	if (strRet == _T("true"))
		{
		return true;
		}

	else if (strRet == _T("false"))
		{
		return false;
		}

	else
		return false;
	}

/************************************************************************************
/* + int ListOfAttributes::get_Alignment()
*
*
*	Returns the Alignment Attribute of the tile from the attributes list
*
*
*/
int ListOfAttributes::get_Alignment() // Returns the Alignment Attribute of the tile
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("alignment"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		}
	// if not found get from parent or default
	if ( !found )
		{
		m_IsDefault =1;
		TreeNode* parent = m_pTile->m_pTreeNode->get_Parent ();
		if(parent->m_Tile.IsContainer ())
			{
			int alignement ; 
			// loooking in parent
			alignement = (parent->m_Tile.get_ListOfAttributes ())->get_ChildrenAlignment ();
			if(alignement == SDS_RTERROR)
				return ALIGN_DEFAULT;
			else
				return alignement;

			}
		
		
		}
		
	if (strRet == _T("left"))
		{
		return(ALIGN_LEFT);
		}

	else if (strRet == _T("right"))
		{
		return(ALIGN_RIGHT);
		}

	else if (strRet == _T("centered"))
		{
		return(ALIGN_CENTERED);
		}

	else if (strRet == _T("top"))
		{
		return(ALIGN_TOP);
		}

	else if (strRet == _T("bottom"))
		{
		return(ALIGN_BOTTOM);
		}

	else if (strRet == _T("horizontal"))
		{
		return(ALIGN_HORIZONTAL);
		}

	else if (strRet == _T("vertical"))
		{
		return(ALIGN_VERTICAL);
		}

	else
		return ALIGN_DEFAULT;
			

	}

/************************************************************************************
/* + CObArray& ListOfAttributes::get_ListAttributes() 
*
*	returns the pointer to the list of Attributes
*
*
*
*/

CObArray& ListOfAttributes::get_ListAttributes()
	{
	return this->m_ListAttributes;
	}
/************************************************************************************
/* + int ListOfAttributes::pixelY (double height)
*
*	Converts the Dialog Units to piXel in Y Direction
*
*
*
*/

int ListOfAttributes::pixelY (
							  double height // Height to convert
							  )
	{
	double factor = 13;   //HIWORD(GetDialogBaseUnits()); // = 
	return (int(factor * height));
	}
/************************************************************************************
/* + int ListOfAttributes::pixelX (double width)
*
*	Converts the Dialog Units to piXel in X Direction
*
*
*
*/

int ListOfAttributes::pixelX (
							  double width // width to convert
							  )
	{
	double factor  = 6; // = LOWORD(GetDialogBaseUnits());
	return (int(factor * width));
	}
/************************************************************************************
/* + int ListOfAttributes::get_Layout - 
*
*	Returns the value of Layout attribute set for the DCL file.
*
*
*
*/

int ListOfAttributes::get_Layout()
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("layout"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		}
	// return default
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("layout"),strRet);
	
		if(error==SDS_RTERROR)
			return SDS_RTERROR;
		}
	if (strRet == _T("vertical"))
		{
		return(LAYOUT_VERTICAL);
		}

	else if (strRet == _T("horizontal"))
		{
		return(LAYOUT_HORIZONTAL);
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + CString ListOfAttributes::get_List()
*
*	Returns the value of List attribute set for the DCL file.
*
*
*
*/

CString ListOfAttributes::get_List()
	{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("list"))
			{
			//m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			}
		}
	//  if not found return NULL
	return CString();
	}
/************************************************************************************
/* + int ListOfAttributes::get_editWidth - 
*
*	Returns the value of editWidth attribute set for the DCL file.
*
*
*
*/

int ListOfAttributes::get_editWidth()
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("edit_width"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			
			}
		
		}
	// if not found or is less then 0 return from defautl list
	if ( !found|| (strRet == _T("0")) || (strRet.GetAt(0) == _T('-')) )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("edit_width"),strRet);
	
		if(error==SDS_RTERROR)
			return SDS_RTERROR;
		}


	return atoi(strRet);


	}
/************************************************************************************
/* + int ListOfAttributes::get_editLimit - 
*
*	Returns the value of editLimit attribute set for the DCL file.
*
*
*
*/

int ListOfAttributes::get_editLimit()
	{
	CString strRet ;
	int index,found=0;
// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("edit_limit"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			
			}
		
		}
	// if not found return default 
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("edit_limit"),strRet);
	
		if(error==SDS_RTERROR)
			return 132;
		}

	int editlimit = atoi(strRet);
	if(editlimit>=0)
		return editlimit ;
	else
		return 132;
	}
/************************************************************************************
/* + int ListOfAttributes::IsMultipleSelect - 
*
*	Returns the value of MultipleSelect attribute set for the DCL file.
*
*
*
*/

int ListOfAttributes::IsMultipleSelect()
{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("multiple_select"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// return form default list
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("multiple_select"),strRet);
	
		if(error==SDS_RTERROR)
			return false;
		}

	if (strRet == _T("true"))
		{
		return true;
		}

	else if (strRet == _T("false"))
		{
		return false;
		}

	else
		return false;

}
/************************************************************************************
/* + int ListOfAttributes::IstabTruncate - 
*
*	Returns the value of tabTruncate attribute set for the DCL file.
*
*
*
*/

int ListOfAttributes::IstabTruncate()
{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("tab_truncate"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			break;
			}
		
		}
	// return default
	if ( !found )
		{
		m_IsDefault =1;
		int error=g_Main.defaultAttrib.GetDefaultValue(m_pTile->get_RenderableTileName(),CString("tab_truncate"),strRet);
	
		if(error==SDS_RTERROR)
			return false;
		}

	if (strRet == _T("true"))
		{
		return true;
		}

	else if (strRet == _T("false"))
		{
		return false;
		}

	else
		return false;

}






/************************************************************************************
/* + int get_AuditLevel() - 
*
*      Returns the value of audit_level attribute set for the DCL file.
*
*
*
*/
int ListOfAttributes::get_AuditLevel()                     // Returns the value of the audit_level attribute.
	{
	CString strRet ;
	int  nAuditLevel;
	int index , found=0;

// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("audit_level"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
				
			break;
			}
		
		}
     
	 // If the attribute was not set.
	 if ( !found )
		 {
		 m_IsDefault =1;
		 return SDS_RTERROR;
		 }

	 nAuditLevel = atoi((LPCTSTR)strRet);

	 if ( nAuditLevel < 0 || nAuditLevel > 3)
		 {
		 return SDS_RTERROR;
		 }
	 	
	return nAuditLevel;
	}

     

/************************************************************************************
/* + int FindAttribute(CString& wszAttributeName,TreeNode* pTreeeNode); - 
*   
*     Finds the attribute and returns SDS_RTNORM OR SDS_RTERROR , depending 
*    upon whether the attribute is found or not.
*
*
*/
int ListOfAttributes::FindAttribute(                                         // Finds an attribute and returns SDS_RTNORM.
				  CString& wszAttributeName               // i:Attribute to find.
				  )
	{

	int nReturn = SDS_RTERROR;

	for ( int iLoop=0;iLoop < this->m_ListAttributes.GetSize();iLoop++)
		{
		
		TileAttributes* pTileAttributes =(TileAttributes*) this->m_ListAttributes.GetAt(iLoop); 
		
		if ( wszAttributeName == pTileAttributes->get_AttributeName())
			{
			 nReturn = SDS_RTNORM; 
			 break;
			}
		}
	
			
	return nReturn;

	}


/************************************************************************************
/* + CString ListOfAttributes::get_PasswordChar() - 
*
*
*	Returns the value of Password_Char attribute set for the DCL file.
*
*
*/
CString ListOfAttributes::get_PasswordChar()
{
	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("password_char"))
			{
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			}
		}
	// return alt +home
	// Don't use right close double quote mark  EBATECH 
	return CString("");

}

/************************************************************************************
/* + int ListOfAttributes::CanCompress - 
*
*	function which returns the state of m_nCompress flag
*
*
*
*/

int ListOfAttributes::CanCompress()
{
	return m_nCompress;
}

/************************************************************************************
/* + int ListOfAttributes::get_buttonWidth(int widt - 
*
*	returns the width of the button
*
*
*
*/

int ListOfAttributes::get_buttonWidth(int width)
	{
	if(m_pTile->get_ScreenObject ()->nColumnTile == LASTTILE )
		{
		if(IsFixedWidth())
			return width;
		else
			return (width);
		}
	else
	if(IsFixedWidth())	
		return width;
	else
		return width;
	}

/************************************************************************************
/* + int ListOfAttributes::IsDefault - 
*
*	function which returns the state of m_IsDefault flag
*	note :-the  following  functions give the states of the property immediatly after the call to read the property
*
*
*/

int ListOfAttributes::IsFromDefault()
	{
	return m_IsDefault;
	}


/************************************************************************************
/* + COLOREF ListOfAttributes::get_Color()  - 
*
*    Returns the color attribute set for an image or image button.
*
*
*
*/
CString ListOfAttributes::get_Color() 
	{

	CString strRet ;
	int index ,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("color") )
			{
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			}
		}
	// return default "7"
	return CString("7"); 
	}

/************************************************************************************
/* + CString ListOfAttributes::get_initialfocus - 
*
*	Returns the initialfocus attribute set for an dialog.
*
*
*
*/

CString ListOfAttributes::get_initialfocus() //Returns the initialfocus attribute set for an dialog.
	{
	CString strRet ;
	int index,found=0;
	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("initial_focus"))
			{
			found=1;
			m_IsDefault =0;
			strRet = pTileAttrib->get_AttributeValue();
			return strRet;
			
			}
		
		}
	//  return NULL if not found
	return CString();
	}	


/************************************************************************************
/* + BOOL ListOfAttributes::get_FixedWidthFont() - 
*
*    This method returns the fixed_width_font value that is set to either TRUE/FALSE 
*   
*
*
*/
BOOL ListOfAttributes::get_FixedWidthFont()
	{

	CString strRet ;
	int index ,found = 0;

	// check in the dcl file fore the attribute (which are stored in m_ListAttributes )
	for ( index = 0; index < m_ListAttributes.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)m_ListAttributes[index];
		if ( pTileAttrib->get_AttributeName() == _T("fixed_width_font"))
			{
			strRet = pTileAttrib->get_AttributeValue();
			found = 1;
			m_IsDefault =0;
			break;
			}
		}
	// if not found get from Default Attributes
	if (!found)
		{
		m_IsDefault=1;
		return false;
		}

	if(strRet == _T("true"))
		{
		return true;
		}

	else if(strRet == _T("false"))
		{
		return false;
		}

	else
		return SDS_RTERROR;

	}


/************************************************************************************
/* + BOOL ListOfAttributes::IsRadioGroup() - 
*
* 
*   This returns true if the tile name is type of radio group
*
*
*/
BOOL ListOfAttributes::IsRadioGroup()
	{
	
	CString wszRendTileName = this->m_pTile->get_RenderableTileName();  

	if ( wszRendTileName == _T("boxed_radio_column") ||
		 wszRendTileName == _T("boxed_radio_row") ||
		 wszRendTileName == _T("radio_column") ||
		 wszRendTileName == _T("radio_row") )
		{
		return TRUE;
		}
	else
		{
		return FALSE;
		}

	}


/************************************************************************************
/* + CString ListOfAttributes::GetParentKey() - 
*
*   This returns the parent tile's key.
*
*
*
*/
CString ListOfAttributes::GetParentKey()
	{

	return this->m_pTile->m_pTreeNode->get_Parent()->m_Tile.get_ListOfAttributes()->get_Key();   

	}
