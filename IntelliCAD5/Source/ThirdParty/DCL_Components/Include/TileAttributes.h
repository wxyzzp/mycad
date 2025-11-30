/** FILENAME:     TileAttributes.h- Contains declarations of class CTileAttributes.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*       This class holds the attibute name,attribute type and attribute value of a tile. 
*   Thess attributes are stored in the list stored in Tile class
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#ifndef _TILEATTRIBUTES_H
#define _TILEATTRIBUTES_H

class TileAttributes : public CObject  
{
public:
	int get_RedundantInfo();
	int set_RedundantInfo(int nRedundantInfo);
	CString get_AttributeValue ();  // Returns the attributes value as string.
	CString get_AttributeType ();   // Returns the attributes type  as string.
	CString get_AttributeName();    // Returns the attributes name  as string.


	int set_AttributeValue (        // Sets the attributes value.
                 CString& wszValue  // i: Attribute value.
						 ); 
	int set_AttributeType (         //  Sets the attributes type.
	             CString& wszType   //  i: Attribute type.
					  );
	int set_AttributeName(          // Sets the attributes name.
		         CString& wszName   // i: Attribute name.
						 );

	TileAttributes();               // Constructor of TileAttribute class.

	TileAttributes(                 // Constructor of TileAttribute class.
		            CString name,   // i: Attribute name.
					CString type,   // i: Attribute type.
					CString value   // i: Attribute value.
					);  
	
	virtual ~TileAttributes();      // Destructor of TileAttributes class.

private:
	int m_nRedundantInfo;
	CString m_wszAttributeValue;	// Attribute value.
	CString m_wszAttributeName;		// Attribute name.
	CString m_wszAttributeType;		// Attribute type.
};

#endif 
