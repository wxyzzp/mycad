///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statement 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_DBSORTENTSTABLE_H
#define OD_DBSORTENTSTABLE_H

#include "DD_PackPush.h"

#include "DbObject.h"

#define STL_USING_UTILITY
#include "OdaSTL.h"

// here Handle is key (first), objectId is value (second)
typedef std::pair<OdDbHandle, OdDbSoftPointerId> HandlePair;
typedef OdArray<HandlePair> HandlePairsArray;

/** Description:
    Represent an entity sorting table object in an OdDbDatabase, which allows 
    the draw order of entities in an OdDbDatabase to be modified.

    Remarks:
    This class contains an OdDbBlockRecord's draw order information.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbSortentsTable : public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbSortentsTable);

  /** Description:
      Constructor (no arguments).
  */
  OdDbSortentsTable();

  /** Description:
      Moves entities with specified object IDs to the bottom.

      Arguments:
      entityIds (I) Array of specified entity ID's.
  */
  void moveToBottom(OdDbObjectIdArray& entityIds);

  /** Description:
      Moves entities with specified object IDs to the top.

      Arguments:
      entityIds (I) Array of specified entity ID's.
  */
  void moveToTop(OdDbObjectIdArray& entityIds); 

  /** Description:
      Moves entities with specified object IDs below the target entity.

      Arguments:
      entityIds (I) Array of specified entity ID's.
      target (I) Target entity.
  */
  void moveBelow(OdDbObjectIdArray& entityIds, OdDbObjectId target);

  /** Description:
      Moves entities with specified object IDs above the target entity.

      Arguments:
      entityIds (I) Array of specified entity ID's.
      target (I) Target entity.
  */
  void moveAbove(OdDbObjectIdArray& entityIds, OdDbObjectId target);

  /** Description:
      Swaps the display order of the specified entities.
     
      Arguments:
      entity1 (I) First entity.
      entity2 (I) Second entity.
  */
  void swapOrder(OdDbObjectId entity1, OdDbObjectId entity2);

  /** Description:
      Returns the Object ID of the block table record this Sortents Table belongs to
      
  */
  OdDbObjectId  blockId() const; 
        
  ///  2005 functions
  ///

  /** Description:
      Returns true if the first entity is drawn before the second, 
      or throws eInvalidInput if either object ID is not in the associated block

      Arguments:
      first (I) First entity.
      second (I) Second entity.
  */
  bool    firstEntityIsDrawnBeforeSecond(OdDbObjectId first, OdDbObjectId second) const;

  /** Description:
      Returns an array of object ID's from the entitiesi in the associated block,
      appearing in the order in which they would be drawn.    
      Caller supplies a presumably empty object id array, and disposes of it after use.

      Arguments:
      ents (O) Array of entity ID's.
      honorSortentsMask (I) - Not supported. 
  */
  void   getFullDrawOrder(OdDbObjectIdArray& ents, OdUInt8 honorSortentsMask = 0) const;

  /** Description:
      Rearranges the object ID's in the input array into their current relative draw order.
      
      Arguments:
      ar (I, O) Array of entity Id's.
      honorSortentsMask (I) - Not supported.
  */
  void   getRelativeDrawOrder(OdDbObjectIdArray& ar, OdUInt8 honorSortentsMask = 0) const;


  /** Description:
      Takes the object ids in the input array and moves them in their current
      associated draw order slots so that they are drawn in the specified order.
      If you wish to move or consolidate the draw order slots of these object ids,
      use the appropriate move*() member function to do so, either before or after
      calling this member.

      Arguments:
      entityIds (I) Array of specified entity ID's.
  */
  void   setRelativeDrawOrder(const OdDbObjectIdArray& ar);

  /** Description:
      Setting absolute draw order of entities. Entity with less handle is drawn first.

      Note that using of this function is not recommended. 

      Arguments:
      srcSorting (I) Source sorting. Array of accordance sort handle -> id (just 
      like SET are stored in file).
  */
  void   setAbsoluteDrawOrder(const HandlePairsArray& srcSorting);

  /** Description:
      Getting absolute draw order of entities. 
      Entity with less handle is drawn first.

      Note that using of this function is not recommended.

      Arguments:
      dstSorting (I) Destination sorting. Array of accordance sort handle -> id (just 
      like SET are stored in file).
  */
  void   getAbsoluteDrawOrder(HandlePairsArray& dstSorting);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void getClassID(void** pClsid) const;
};


typedef OdSmartPtr<OdDbSortentsTable> OdDbSortentsTablePtr;

#include "DD_PackPop.h"

#endif //OD_DBSORTENTSTABLE_H


