/** FILENAME:     AuditLevelProcessor.cpp- Contains implementation of class AuditLevelProcessor.
`* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*      This class takes care of processing of various rules for various audit levels.
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
#include "tileattributes.h"
#include "tree.h"
#include "constants.h"
#include "main.h"
#include "sds.h"
#include "auditlevel.h"
#include "attributeinfo.h"
#include "listofattributes.h"
#include "resource.h"

extern Main g_Main;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/************************************************************************************
/* + AuditLevelProcessor::AuditLevelProcessor() - 
*
*
*
*
*
*/
AuditLevelProcessor::AuditLevelProcessor()   // Default constructor.
{

}

/************************************************************************************
/* + AuditLevelProcessor::AuditLevelProcessor(int nFileID,int nAuditLevel) - 
*
*    Constructor.
*
*
*
*/
AuditLevelProcessor::AuditLevelProcessor(							// Constructor
										 int nFileID,				// i: FunctionName, used for error messages.  
										 int nAuditLevel			// i: WidgetName, used for error messages.
										 )							// i: KeyName, used for error messages.
	{

	m_nFileID = nFileID;                                            // FileID of file.
	m_nAuditLevel = nAuditLevel;                                    // Auditlevel set in the current file.   
	m_wszKeyName = _T("");                                          // The keyname of current tile being audited.
	m_wszFunctionName = _T("");                                     // The function name of current tile being parsed.
	m_wszWidgetName = _T("");                                       // The widget name of current tile beinmg parsed.
	m_wszParentWidgetName = _T("");                                 // The parent wodget name. 
	m_nIsDefaultCount = 0;                                          // Count if is_default attributes set to true in a dialog definition.   
	m_nIsCancelCount = 0;                                           // Count if is_cancel attributes set to true in a dialog definition.    
    this->m_bKeyAttributeSet = FALSE;                               // Flag, to check whehther key attribute is set.  
  
	}
/************************************************************************************
/* + AuditLevelProcessor::AuditLevelProcessor(CString &wszFuncName, CString &wszWidgetName, CString &wszKeyName) - 
*
*
*      Constructor to set the function name, widgetname anf key.
*
*
*/
AuditLevelProcessor::AuditLevelProcessor(                                      // Constructor.
										 CString &wszFuncName,                 // i: Function name of tile. 
										 CString &wszWidgetName,               // i: Widgetname
										 CString &wszKeyName                   // i: Keyname.
										 )
	{

	this->m_wszFunctionName = wszFuncName;                               
    this->m_wszWidgetName  = wszWidgetName;
    this->m_wszKeyName  = wszKeyName;
	}

AuditLevelProcessor::~AuditLevelProcessor()
{

}


/************************************************************************************
/* + int AuditLevelProcessor::StartProcessing() 
*
*     This function processes all the nodes generated while processing.
*
*
*
*/
int AuditLevelProcessor::StartProcessing()
	{
	POSITION pos;
    pos = g_Main.m_DCLParser.m_ListOfDCLFileData.GetHeadPosition() ;
	FileInfo *pFileInfo;
		 
	// To write the tree details  generated during parsing of DCL files.
	while  ( pos != NULL )  
		{
		pFileInfo =(FileInfo*) g_Main.m_DCLParser.m_ListOfDCLFileData.GetNext(pos);
		    
		// Write the DCL Filename and the fileid.
		if ( pFileInfo )
			{
			// Call the function to write details of each tree generated.
			// This condition is checked so that only the currently loaded file need to be audited.
			if ( m_nFileID == pFileInfo->get_DCLFileID() )
				{
				// Call this function to audit all definitions in this file.
				ParseFile(pFileInfo); 
				}
			} 
		if ( pos == NULL )
			break;
		}

	  return SDS_RTNORM;
	}



/*******************************************************************************************************
/* + int AuditLevelProcessor::ParseTree() - 
*
*    This function parses thru all the nodes all calls the respective functions to do the validations
*
*
*
*/
int AuditLevelProcessor::ParseFile(                            // Parsers all definitions in the corresponding files.
								   FileInfo *pFileInfo         // i: FileInfo pointer
								   )
	{
	
	Tree *pTree;
	TreeNode *pTreeNode;
	int iTemp;
    int iCount =  pFileInfo->m_RootNodes.GetCount(); 
    
	
	if ( !iCount ) 
		return SDS_RTNORM ;
	   
	// The files to be parsed are taken from the file list.
	POSITION position = pFileInfo->m_RootNodes.GetHeadPosition(); 
	    
	for(int iIndex=0 ; iIndex < iCount ; iIndex++) 
		{
		
		// Parsing is done for every definition aka node is the current file.
        pTree = ((Tree*)pFileInfo->m_RootNodes.GetNext(position)); 
		pTreeNode = pTree->m_startTreeNode ;
				
		iTemp = pTree->m_nType;

		// Call this function for all the definitions.
		ParseTree(pTreeNode);

		}
	return SDS_RTNORM;

	}


/************************************************************************************
/* + int AuditLevelProcessor::ParseTree(TreeNode *pTreeNode) - 
*
*    Parses every node present in the file , starting from the root node.
*
*
*
*/
int AuditLevelProcessor::ParseTree(                                // Parses from root node.
								   TreeNode *pTreeNode             // i: Root node to start from. 
								   )
	{


	BOOL bHasChildren;                                     // value to hold whether the current node has children
	TreeNode *pOldNode = NULL;                             // points to the current node being traversed 
	TreeNode *TempOldNode = NULL;                          // holds previous value of pOldNode

			
	pOldNode = pTreeNode ;                                 

	m_wszFunctionName = pTreeNode->m_Tile.get_FunctionName(); 
	TRACE("%d\n",m_wszFunctionName);
	
	// Get the widget name.
	m_wszWidgetName = pTreeNode->m_Tile.get_RenderableTileName();
	

	// Get the key name.
	m_wszKeyName = (pTreeNode->m_Tile.get_ListOfAttributes())->get_Key();

	m_nIsDefaultCount = 0;
	m_nIsCancelCount = 0;

	if ( pTreeNode->m_Tile.get_RenderableTileName() == _T("dialog") )
		{
		this->m_bProcessingDialog = TRUE;
 
	    // Rule: if tile is dialog, then the allow_accept attribute cannot be set.
		if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("allow_accept"))) == SDS_RTNORM )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_ALLOW_ACCEPT_NOT_ALLOWED,2);
			} 
		}
	else
		{
		m_bProcessingDialog = FALSE;
		}
			
    
	bHasChildren  =  pOldNode->HasChild();                // check whether child is present

	if( !bHasChildren )
		{
		ProcessNode(pOldNode);						// display tree nodes details for current node
        return SDS_RTNORM;                         // return ,if only one node in tree to be traversed
		}
	else
		{
		ProcessNode(pOldNode);						// display tree nodes details for current node
		}

	while(1)
		{
		bHasChildren  =  pOldNode->HasChild();               // check whether child is present    
             
		if ( bHasChildren )
			{ 
			pOldNode = pOldNode->GetFirstChild();           // get the first child
			ProcessNode(pOldNode);                // display tree nodes details for current node
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
		        ProcessNode(pOldNode);
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
						ProcessNode(pOldNode);		
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

	if ( pTreeNode->m_Tile.get_RenderableTileName() == _T("dialog") )
		{
		if ( m_nIsDefaultCount > 1 )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_MORE_THAN_ONE_DEFAULT,2);
			}
		
		if ( m_nIsCancelCount > 1 )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_MORE_THAN_ONE_CANCEL,2);
			}
		}

	return SDS_RTNORM;
	}



/************************************************************************************
/* + int AuditLevelProcessor::ProcessNode(TreeNode *pTreeNode) - 
*
*    Process node and apply all audit level rules have to be applied.  
*
*
*
*/
int AuditLevelProcessor::ProcessNode(                      // Process node and apply all audit level rules have to be applied.  
									 TreeNode *pTreeNode   // i: Node to be parsed 
									 )
	{

	// Get the widget name.
	m_wszWidgetName = pTreeNode->m_Tile.get_RenderableTileName();

	// Get the key name.
	m_wszKeyName = (pTreeNode->m_Tile.get_ListOfAttributes())->get_Key();

	// Get the parent widget name.
	if ( pTreeNode->get_Parent() )
		{
		m_wszParentWidgetName = (pTreeNode->get_Parent())->m_Tile.get_RenderableTileName();
		}
	else
		{
        m_wszParentWidgetName = _T("");
		}


	ListOfAttributes *pListAttr;
	
	// Get the pointer to list of attributes.
	pListAttr = (pTreeNode->m_Tile.get_ListOfAttributes());

	for ( int iLoop=0; iLoop < pListAttr->m_ListAttributes.GetSize();iLoop++)
		{
		TileAttributes *pTileAttr = (TileAttributes*)pListAttr->m_ListAttributes.GetAt(iLoop);

		// Data type checking.
		if ( ValidateAttribute(pTileAttr) == SDS_RTERROR)
			{
		     this->GenerateErrorString(pTileAttr->get_AttributeName());  
			}


		// Check whether is_default attribute.
		if ( pTileAttr->get_AttributeName() == _T("is_default") 
			  &&  pTileAttr->get_AttributeValue() == _T("true"))
			{
			this->m_nIsDefaultCount ++; 
			}

		// Check whether is_default attribute.
		if ( pTileAttr->get_AttributeName() == _T("is_cancel") 
			  &&  pTileAttr->get_AttributeValue() == _T("true"))
			{
			this->m_nIsCancelCount ++; 
			}

		 // Check for key attribute.
		 if ( pTileAttr->get_AttributeName() == _T("key") ) 
			{
			this->m_bKeyAttributeSet = TRUE;
			}


		 if ( pTileAttr->get_RedundantInfo() > 0 )
			 {
             if ( m_wszWidgetName == _T("image") && pTileAttr->get_AttributeName() == _T("is_tab_stop") && 
				            pTileAttr->get_AttributeValue() == _T("true") )
				 {
				  continue;
				 }
			 else if ( m_wszWidgetName == _T("image") && pTileAttr->get_AttributeName() == _T("fixed_height") && 
				            pTileAttr->get_AttributeValue() == _T("true") )
				 {
				 continue;
				 }
			 else
				 {
				 AppendHintString(pTileAttr->get_RedundantInfo());
				 }
			 } 

		}

	// the value of m_bKeyAttributeset is made true, if the current widget is not an active tile.
	if ( m_wszWidgetName != _T("button") && m_wszWidgetName != _T("edit_box") && m_wszWidgetName != _T("image_button") 
		&&  m_wszWidgetName != _T("image") && m_wszWidgetName != _T("list_box") && m_wszWidgetName != _T("popup_list") 
		&&  m_wszWidgetName != _T("radio_buttton") 	&& 	m_wszWidgetName != _T("slider") 
		&& m_wszWidgetName != _T("toggle") )
		{
        	this->m_bKeyAttributeSet = TRUE;
		}


	if ( this->m_bProcessingDialog == TRUE && this->m_bKeyAttributeSet == FALSE && m_wszWidgetName != _T("dialog"))
		{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_KEY_NEEDED,2);
		}
		m_bKeyAttributeSet = FALSE;

    

	// Do neccesary validation for button.
	// Rule : text should have either label or width attribute set.
	if ( this->m_bProcessingDialog == TRUE &&
		  pTreeNode->m_Tile.get_RenderableTileName() == _T("text"))
		{
		CheckForTextAttributes(pTreeNode);
		}
   


	// Do neccesary validation for button.
	// Rule : button should have either label ior width attribute set.
	if ( this->m_bProcessingDialog == TRUE &&
		  pTreeNode->m_Tile.get_RenderableTileName() == _T("button"))
		{
		CheckForButtonAttributes(pTreeNode);
		}

	// Do neccesary validation for image/image_button
	// Rule : the tile  should have either height & width or either one of the with aspect ratio.
	if ( this->m_bProcessingDialog == TRUE &&
		  pTreeNode->m_Tile.get_RenderableTileName() == _T("image") 
		  || pTreeNode->m_Tile.get_RenderableTileName() == _T("image_button") )
		{
		CheckForImageButtonAttributes(pTreeNode);
		}
	

	// Do necesary validation for row/column.
	// Rule: row/column cannothave label if it is not boxed.
	if ( pTreeNode->m_Tile.get_RenderableTileName() == _T("row") ||
         pTreeNode->m_Tile.get_RenderableTileName() == _T("column") ||
		 pTreeNode->m_Tile.get_RenderableTileName() == _T("radio_row") ||
         pTreeNode->m_Tile.get_RenderableTileName() == _T("radio_column") )
		{
		if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("label"))) == SDS_RTNORM )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_BOXED_TO_BE_LABELLED,2);
			} 
		}


    // Validation to check whether a radio buttons parent is a radio cluster.
	if ( pTreeNode->m_Tile.get_RenderableTileName() == _T("radio_button") && this->m_bProcessingDialog == TRUE )
		{
         if ( m_wszParentWidgetName !=  _T("radio_row") &&
			  m_wszParentWidgetName !=  _T("radio_column") &&
			  m_wszParentWidgetName !=  _T("boxed_radio_row") &&
			  m_wszParentWidgetName !=  _T("boxed_radio_column") )
			 {
             FormatErrorMessage(IDS_WARNING_HEADER,IDS_PARENT_MUST_BE_RADIO_CLUSTER,2);
			 }
		}

	// Validation : radio_clusters  should only have radio_buttons and spacers.
	if ( m_wszParentWidgetName ==  _T("radio_row") ||
			  m_wszParentWidgetName ==  _T("radio_column") ||
			  m_wszParentWidgetName ==  _T("boxed_radio_row") ||
			  m_wszParentWidgetName ==  _T("boxed_radio_column") )
		{
             if ( pTreeNode->m_Tile.get_RenderableTileName() != _T("radio_button") &&    
				  pTreeNode->m_Tile.get_RenderableTileName() != _T("spacer")  )
				{
			     FormatErrorMessage(IDS_WARNING_HEADER,IDS_ONLY_RADIO_SPACER_IN_CLUSTER,2);
				}
		}
	
			 
	return SDS_RTNORM;

	}



/************************************************************************************
/* + int AuditLevelProcessor::ValidateAttribute(TileAttribute *pTileAttribute) - 
*
*   This function validates the tile attribute and adds a srror string to the errorlist
*   if validation fails.
*
*
*/
int AuditLevelProcessor::ValidateAttribute(                        // Validates the attribute.
							  TileAttributes *pTileAttribute       // i: The tile attribute to be validated.
							  )
	{
    
	BOOL nReturn;
	AttributeInfo *pAttributeInfo;


    TRACE("AttributeName : %s\n ", pTileAttribute->get_AttributeName());

	nReturn = g_Main.m_DCLParser.m_mapAttributeInfo.Lookup(
									((LPCTSTR)pTileAttribute->get_AttributeName()),
									( CObject*& ) pAttributeInfo);

	int nDataType	=  atoi((LPCTSTR)pTileAttribute->get_AttributeType());
	
	if ( nReturn == TRUE )
		{
		if ( nDataType != pAttributeInfo->m_nDataType )
			{
			 // This condition is implemented because, a attribute which can hold real values
			 // can also hold integer values.
			 if ( pAttributeInfo->m_nDataType == REAL_DATA_TYPE && nDataType == INT_DATA_TYPE )
				 {
				 return SDS_RTNORM;
				 }
			 // validation for color, since it can hold a colr value or any integer
			 if ( pTileAttribute->get_AttributeName() == _T("color"))
				{
				if ( nDataType == INT_DATA_TYPE )
					{
					return SDS_RTNORM;
					}
				 }

				
			return SDS_RTERROR;
			}
		else
			{
			if ( nDataType == KEYWORD_DATA_TYPE )
				{

  		        for (int iLoop = 0 ; iLoop < pAttributeInfo->m_arrayAttributeValues.GetSize() ; iLoop++)
					{
					if ( pAttributeInfo->m_arrayAttributeValues.GetAt(iLoop) == pTileAttribute->get_AttributeValue() )
						{ 
						if ( IsPossibleValue( pTileAttribute ) ==  TRUE )
							{
							return SDS_RTNORM;
							}
						else
							{
							if ( pTileAttribute->get_AttributeName() == _T("color"))
								{
								if ( atoi((LPCTSTR)pTileAttribute->get_AttributeValue()) != 0  && 
									pTileAttribute->get_AttributeValue() != _T("0") )
									{
									 return SDS_RTERROR;
									}
								}
								
							return SDS_RTERROR;
							}
						}
					} 

				if ( iLoop ==  pAttributeInfo->m_arrayAttributeValues.GetSize() )
					{
					return SDS_RTERROR;
					}

				}
			return SDS_RTNORM;
			  
			}
		}
	return SDS_RTNORM;

	}
/*************************************************************************************************
/* + int AuditLevelProcessor::GenerateErrorString() - 
*
*   This method genearates an approriate error string for any data type mismatches found.
*
*
*
*/
int AuditLevelProcessor::GenerateErrorString(                     // Generates an error string for data type mismatch.
								CString& wszAttributeName        // i:Attribute name.
										)
	{

	BOOL nReturn;
	AttributeInfo *pAttributeInfo;
	CString wszErrorMessage;
    CString wszResourceString;
	CString wszKeywordList;
	int iLoop ;
	int nCount ;

	nReturn = g_Main.m_DCLParser.m_mapAttributeInfo.Lookup(
									((LPCTSTR)wszAttributeName),
									( CObject*& ) pAttributeInfo);

	wszResourceString.LoadString(IDS_WARNING_HEADER);


	CString wszWidgetName;
	wszWidgetName = this->m_wszWidgetName ;

	// Set the widget name to cluster, for following tiles.
	if ( this->m_wszWidgetName == _T("row") || 
		 this->m_wszWidgetName == _T("column") ||
		 this->m_wszWidgetName == _T("boxed_row") ||
		 this->m_wszWidgetName == _T("boxed_column") )
		{
		wszWidgetName = _T("cluster"); 
		}
	
	// Set the widget name to cluster, for following tiles.
	if ( this->m_wszWidgetName == _T("boxed_radio_row") || 
		 this->m_wszWidgetName == _T("boxed_radio_column") ||
		 this->m_wszWidgetName == _T("radio_row") || 
		 this->m_wszWidgetName == _T("radio_column") )

		{
		wszWidgetName = _T("radio_cluster"); 
		}


	// Format error message.
	wszErrorMessage.Format(wszResourceString,this->m_wszFunctionName,wszWidgetName,this->m_wszKeyName);

	wszResourceString.LoadString(IDS_ATTRIBUTE_INFO);
	
	wszResourceString = wszErrorMessage + wszResourceString;

	wszErrorMessage.Format(wszResourceString,wszAttributeName);

	
	if ( nReturn == TRUE )
		{
		        
		switch( pAttributeInfo->m_nDataType ) 
			{
			case INT_DATA_TYPE:
                // Load the string , which says that int datatype required. 
			    wszResourceString.LoadString(IDS_INT_DATATYPE_REQUIRED); 
				break;
				
			case REAL_DATA_TYPE  : 
				// Load the string , which says that real datatype required.
				wszResourceString.LoadString(IDS_REAL_DATATYPE_REQUIRED); 
				break;

			case STRING_DATA_TYPE :
				// Load the string , which says that string datatype required.
				wszResourceString.LoadString(IDS_STRING_DATATYPE_REQUIRED); 
				break;

			case KEYWORD_DATA_TYPE:


				if ( wszAttributeName == _T("color") )
					{
					// Load the string , which says that INTEGER or COLOR value required.
					wszResourceString.LoadString(IDS_COLOR_DATATYPE_REQUIRED); 
					break;
					}
				

				// Here, since the list of values to be displayed for attributes like alignment etc. are diferrent from the
				// regular tiles, and since they depend upon their parent tiles, some conditions are checked
				// the loop mainipulated to just display the possible values.
				            
				// If the attribute was a keyword , then display the list of possible values.
				nCount = pAttributeInfo->m_arrayAttributeValues.GetSize() ;
				
				iLoop = 0;
                
				if ( wszAttributeName == _T("alignment") )
					{
					nCount = nCount - 3 ;
					if ( m_wszParentWidgetName == _T("row") || m_wszParentWidgetName == _T("boxed_row") )
						{
						// These values are set, because for row or boxed_row
						// the alignment attr can only have top,centered or bottom.
						iLoop = 3;
						nCount += 3 ;
						}
					else if ( m_wszParentWidgetName == _T("column") || m_wszParentWidgetName == _T("boxed_column") )
							{
							// These values are set, because for column or boxed_column
							// the alignment attr can only have left,centered or right.
							iLoop = 0;
							nCount = 3;
							}
					else
						{
						// the alignment attr can only have top,centered,bottom.
						iLoop = 0;
						nCount = 3;
						}

					}


				if ( wszAttributeName == _T("children_alignment") )
					{
					nCount = nCount - 3 ;
					if ( m_wszWidgetName == _T("row") || m_wszWidgetName == _T("boxed_row") )
						{
						// These values are set, because for row or boxed_row
						// the alignment attr can only have top,centered or bottom.
						iLoop = 3;
						nCount += 3 ;
						}
					else if ( m_wszWidgetName == _T("column") || m_wszWidgetName == _T("boxed_column") )
						{
						// These values are set, because for column or boxed_column
						// The alignment attr can only have left,centered or right.
						iLoop = 0;
						nCount = 3;
						}
					else
						{
						// The alignment attr can only have left,centered or right.
						iLoop = 0;
						nCount = 3;
						}
					}

				 
	            // Append the list of possible values to the error string.
				for ( ;iLoop < nCount ;iLoop++)
					{
					wszKeywordList += _T(" ") +  (pAttributeInfo->m_arrayAttributeValues.GetAt(iLoop));
					if ( iLoop == nCount - 2)
						{
						wszKeywordList += _T("  or");
						}
					else if ( iLoop > nCount -2 )
						{
						wszKeywordList += _T(" .");
						}
					else 
						{
						wszKeywordList += _T(" ,");
						}
	
					}
 
				// Load string 
				wszResourceString.LoadString(IDS_ATTRIBUTE_SHOULD_BE);
                wszResourceString += wszKeywordList + _T("\n\n");
				break;
			default:
				ASSERT(FALSE);
			}

	wszErrorMessage += wszResourceString;   
	g_Main.m_DCLParser.AddErrorString(wszErrorMessage,2);
       		
			
	}
	

    return SDS_RTERROR;
	 

	}

	
/************************************************************************************
/* + int AuditLevelProcessor::CheckForButtonAttributes(TreeNode *pTreeNode) - 
*
*
*  This method checks wheher a button tile has label or wodth property set.
*
*
*/
int AuditLevelProcessor::CheckForButtonAttributes(                          // Checks for mandatory buttons properties.
												  TreeNode *pTreeNode       // i: Node having button tile.        
												  )
	{


	if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("label"))) == SDS_RTERROR 
		 && (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("width"))) == SDS_RTERROR )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_BUTTON_NEED_LABEL_OR_WIDTH,2);
			} 

	return SDS_RTNORM;

	}



/************************************************************************************
/* + int AuditLevelProcessor::FormatErrorMessage(CString &wszErrorHeader, CString &wszErrorMessage) - 
*
*     This method formates the error message and adds it to the error log.
*
*
*
*/
int AuditLevelProcessor::FormatErrorMessage(                             // Formats the error message.
											int nErrorHeader,             // i: Error header, differs for level 2 and level 3.
                            				int nErrorMessage,           // i: Specific message. 
											int nAuditLevel              // i: The audit level for which the error is added.
											)
	{
	CString wszResourceString,wszErrorString;
	wszResourceString.LoadString(nErrorHeader);

	CString wszWidgetName = this->m_wszWidgetName;

		// Set the widget name to cluster, for following tiles.
	if ( this->m_wszWidgetName == _T("row") || 
		 this->m_wszWidgetName == _T("column") ||
		 this->m_wszWidgetName == _T("boxed_row") ||
		 this->m_wszWidgetName == _T("boxed_column") )
		{
		wszWidgetName = _T("cluster"); 
		}
	
	// Set the widget name to cluster, for following tiles.
	if ( this->m_wszWidgetName == _T("boxed_radio_row") || 
		 this->m_wszWidgetName == _T("boxed_radio_column") ||
		 this->m_wszWidgetName == _T("radio_row") || 
		 this->m_wszWidgetName == _T("radio_column")  )
		{
		wszWidgetName = _T("radio_cluster"); 
		}
	
    // Format the error message.
	wszErrorString.Format(wszResourceString,this->m_wszFunctionName,wszWidgetName,this->m_wszKeyName);
	wszResourceString.LoadString(nErrorMessage);
	wszErrorString += wszResourceString;
	g_Main.m_DCLParser.AddErrorString(wszErrorString,nAuditLevel);
	
	return SDS_RTNORM;
 
	}


/************************************************************************************
/* + int AuditLevelProcessor::FormatHintForRedAttr(CString &wszFuncName, TreeNode *pSourceNode, TileAttribute *pTileAttribute) - 
*
*       Formats and add the message to the error list for conditions like attribute assignment redundancy.
*
*
*
*/
int AuditLevelProcessor::FormatHintForRedAttr(                                            // Formats the hint.
											  CString &wszFuncName,                       // The definition name.
											  Tile& tile,                      // The node for which condition is met.
											  TileAttributes *pTileAttribute               // Attribute for which the condition is met.
											  )
	{

	CString wszResourceString,wszErrorMessage;
	CString wszValue;  

	CString wszRendTileName,wszKeyName = "";

	wszRendTileName = tile.get_RenderableTileName();
	wszKeyName = tile.get_ListOfAttributes()->get_Key(); 


	if ( wszRendTileName == _T("row") || 
		 wszRendTileName == _T("column") ||
		 wszRendTileName == _T("boxed_row") ||
		 wszRendTileName == _T("boxed_column") )
		{
		wszRendTileName  = _T("cluster"); 
		}
	
	// Set the widget name to cluster, for following tiles.
	if ( wszRendTileName == _T("boxed_radio_row") || 
		 wszRendTileName == _T("boxed_radio_column") ||
		 wszRendTileName == _T("radio_row") || 
		 wszRendTileName == _T("radio_column")  )
		{
		wszRendTileName = _T("radio_cluster"); 
		}
	


	// Formating  of hint depending upon the attributes dataypes.
	if ( pTileAttribute->get_AttributeType() == STRING_DATA_TYPE)
		{
		wszValue = "\"" + pTileAttribute->get_AttributeValue() + "\"" ;
		}
	else
		{
		wszValue = pTileAttribute->get_AttributeValue();
		}


	// Format the message.
	wszResourceString.LoadString(IDS_HINT_HEADER);

	wszErrorMessage.Format(wszResourceString,wszFuncName, wszRendTileName,wszKeyName);
	
	wszResourceString.LoadString(IDS_REDUNDANT_ASSIGNMENT);

    wszResourceString = wszErrorMessage + wszResourceString ;

	wszErrorMessage.Format(wszResourceString,pTileAttribute->get_AttributeName(),wszValue );
  

    
	// Add the error string
	int iIndex = g_Main.m_DCLParser.AddErrorString(wszErrorMessage,3);
	
	pTileAttribute->set_RedundantInfo(iIndex); 


    // Set the hint flag.	
	g_Main.m_DCLParser.set_HintPresent();

	return SDS_RTNORM;


	}	


/************************************************************************************
/* + BOOL AuditLevelProcessor::IsPossibleValue(TileAttributes *pTileAttribute) - 
*
*     Checks whether a paritcular attribute can be set to the corresponding value
*   when it is assigned to particular tile.
*
*
*/
BOOL AuditLevelProcessor::IsPossibleValue(                                    // Validate values.
										  TileAttributes *pTileAttribute      // i: Tile attribute to be validated.  
										  )
	{

	
	BOOL bReturn = TRUE;


	// Check whether "alignment" attribute has the right values
	if ( pTileAttribute->get_AttributeName() == _T("alignment") )
		{
		if ( m_wszParentWidgetName == _T("row") || m_wszParentWidgetName == _T("boxed_row") 
			|| m_wszParentWidgetName == _T("radio_row") || m_wszParentWidgetName == _T("boxed_radio_row") )
			{
			if ( pTileAttribute->get_AttributeValue() != _T("top")     &&
                 pTileAttribute->get_AttributeValue() != _T("bottom")  &&
                 pTileAttribute->get_AttributeValue() != _T("centered")   ) 
				{
				 bReturn = FALSE;
				}
			}
		 else
			 {
			 if ( pTileAttribute->get_AttributeValue() != _T("left")     &&
                 pTileAttribute->get_AttributeValue() != _T("right")  &&
                 pTileAttribute->get_AttributeValue() != _T("centered")   ) 
				{
				 bReturn = FALSE;
				}
			}


		if ( m_wszParentWidgetName == _T("column") || m_wszParentWidgetName == _T("boxed_column") 
			|| m_wszParentWidgetName == _T("radio_column") || m_wszParentWidgetName == _T("boxed_radio_column") )
			{
			if ( pTileAttribute->get_AttributeValue() != _T("left")     &&
                 pTileAttribute->get_AttributeValue() != _T("right")  &&
                 pTileAttribute->get_AttributeValue() != _T("centered")   ) 
				{
				 bReturn = FALSE;
				}
			}
		}

	// Check whether "children_alignment" attribute has the right values
	if ( pTileAttribute->get_AttributeName() == _T("children_alignment")  )
		{
		if ( m_wszWidgetName == _T("row") || m_wszWidgetName == _T("boxed_row") 
			|| m_wszWidgetName == _T("radio_row") || m_wszWidgetName == _T("boxed_radio_row") )
			{
			if ( pTileAttribute->get_AttributeValue() != _T("top")     &&
                 pTileAttribute->get_AttributeValue() != _T("bottom")  &&
                 pTileAttribute->get_AttributeValue() != _T("centered")   ) 
				{
				 bReturn = FALSE;
				}
			}
		else
			{
			if ( pTileAttribute->get_AttributeValue() != _T("left")     &&
                 pTileAttribute->get_AttributeValue() != _T("right")  &&
                 pTileAttribute->get_AttributeValue() != _T("centered")   ) 
				{
				bReturn = FALSE;
				}
			}

		if ( m_wszWidgetName == _T("column") || m_wszWidgetName == _T("boxed_column") 
			|| m_wszWidgetName == _T("radio_column") || m_wszWidgetName == _T("boxed_radio_column") )
			{
			if ( pTileAttribute->get_AttributeValue() != _T("left")     &&
                 pTileAttribute->get_AttributeValue() != _T("right")  &&
                 pTileAttribute->get_AttributeValue() != _T("centered")   ) 
				{
				 bReturn = FALSE;
				}
			}
		}
    
	return bReturn;

	}


/************************************************************************************
/* + int AuditLevelProcessor::CheckForRedundancy(TileAttributes *pTileAttribute) - 
*
*    This method checks for redundancy of tile attribute assignments, with
*   that of its default values.
*
*
*/
int AuditLevelProcessor::CheckForRedundancy(                                       // Checks for redundant asignments. 
											TileAttributes *pTileAttribute         // i: Tile attribute whose default valus has to be compared to. 
											)
	{

	BOOL nReturn;
	AttributeInfo *pAttributeInfo;
   

	nReturn = g_Main.m_DCLParser.m_mapAttributeInfo.Lookup(
									((LPCTSTR)pTileAttribute->get_AttributeName()),
									( CObject*& ) pAttributeInfo);


	// The current attribute value is compared to its default, and if it matches then
	// the redundancy is reported.
	if ( nReturn == TRUE )
		{
		int nDataType	=  atoi((LPCTSTR)pTileAttribute->get_AttributeType());
        
		// This check is only made for keyword attribute values.
		// For other values, the redundancy check is done during assignment itself.
		if ( nDataType == KEYWORD_DATA_TYPE )
			{
			if ( pTileAttribute->get_AttributeValue() == pAttributeInfo->m_wszDefaultValue )
				{
				// If redundant return error.
				return SDS_RTERROR; 
				}
			}
		}
		
	return SDS_RTNORM;

	}



/*****************************************************************************************************************
/* + int AuditLevelProcessor::CheckForImageButtonAttributes(TreeNode *pTreeNode) - 
*
*   Checks whether both the height and width properties or the aspect_ratio and any one of the former is set.
*
*
*
*/
int AuditLevelProcessor::CheckForImageButtonAttributes(                         // Checks for mandatory button attributes.
													   TreeNode *pTreeNode      // i: The node on which validation is done.
													   )
	{
	   
	    BOOL bMandatoryAttributesPresent = FALSE;

		if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("height"))) == SDS_RTNORM
		 && (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("width"))) == SDS_RTNORM )
			{
			bMandatoryAttributesPresent = TRUE;
			} 

		if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("height"))) == SDS_RTNORM
		 && (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("aspect_ratio"))) == SDS_RTNORM )
			{
			bMandatoryAttributesPresent = TRUE;
			} 

		if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("aspect_ratio"))) == SDS_RTNORM
		 && (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("width"))) == SDS_RTNORM )
			{
			bMandatoryAttributesPresent = TRUE;
			} 

		if ( !bMandatoryAttributesPresent )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_IMAGE_NEED_HT_WIDTH_OR_ASPRATIO,2);
			}

		if ( (pTreeNode->m_Tile.get_ListOfAttributes())->IsAllowAccept() == TRUE 
			 && pTreeNode->m_Tile.get_RenderableTileName() == _T("image") )
			{
            FormatErrorMessage(IDS_WARNING_HEADER,IDS_ALLOW_ACCEPT_NOT_ALLOWED,2);
			}

    return SDS_RTNORM;

	}




/************************************************************************************
/* + int AuditLevelProcessor::AppendHintString(int iIndex) - 
*
*  Appends the hint string to the regular error  list. 
*
*
*
*/
int AuditLevelProcessor::AppendHintString(int iIndex)
	{

	CString wszHintString ;

	//Modified Cybage AW 30-07-01
	//Reason : The Hint Strings appended in the ErrorList is one less than the actual
	//Hints present in the program.
	//BugNo : 77818 from Bugzilla
	//if ( iIndex < g_Main.m_DCLParser.m_HintList.GetSize() )
	if ( iIndex <= g_Main.m_DCLParser.m_HintList.GetSize() )	
		{
		wszHintString  = g_Main.m_DCLParser.m_HintList.GetAt(iIndex-1);
		g_Main.m_DCLParser.m_ErrorList.Add(wszHintString); 
		}


	return SDS_RTNORM;

	}



/************************************************************************************
/* + int AuditLevelProcessor::CheckForTextAttributes(TreeNode *pTreeNode) - 
*
*
*  This method checks wheher a text tile has label or width property set.
*
*
*/
int AuditLevelProcessor::CheckForTextAttributes(                          // Checks for mandatory text properties.
												  TreeNode *pTreeNode       // i: Node having text tile.        
												  )
	{


	if ( (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("label"))) == SDS_RTERROR 
		 && (pTreeNode->m_Tile.get_ListOfAttributes())->FindAttribute(CString(_T("width"))) == SDS_RTERROR )
			{
			FormatErrorMessage(IDS_WARNING_HEADER,IDS_TEXT_NEED_LABEL_OR_WIDTH,2);
			} 

	return SDS_RTNORM;

	}









