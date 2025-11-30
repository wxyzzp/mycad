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



#ifndef _ODDBGROUP_INCLUDED_
#define _ODDBGROUP_INCLUDED_

#include "DD_PackPush.h"

#include "DbObject.h"

class OdDbGroupIterator;
class OdCmColor;
class OdDbGroupImpl;
class OdDbGroup;

/** Description:
    Iterator class that provides sequential access to the entities in an OdDbGroup.

    Remarks:
    Only top level modelspace or paperspace entities can be placed into a group.  An entity
    owned by any other block will not be recognized as part of a group by AutoCAD.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbGroupIterator : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdDbGroupIterator);

  /** Description:
      Opens the current object referenced by this iterator, in the specified mode.

      Return Value:
      The opened object (if successful), or a NULL smart pointer.
  */
  virtual OdDbObjectPtr getObject(OdDb::OpenMode) = 0;

  /** Description:
      Returns the Object ID of the current object referenced by this iterator.
  */
  virtual OdDbObjectId objectId() const = 0;

  /** Description:
      Returns true if there are no more objects to iterate through, false otherwise.
  */
  virtual bool done() const = 0;

  /** Description:
      Increments the current object for this iterator to the next object in the group.
  */
  virtual bool next() = 0;

  /*
     virtual void getClassID(void** pClsid) const;
  */
protected:
  /** Description:
      Constructor (no arguments).
  */
  OdDbGroupIterator() {}
};

typedef OdSmartPtr<OdDbGroupIterator> OdDbGroupIteratorPtr;

/** Description:
    Represents a group object within an OdDbDatabase.  

    Remarks:
    Only modelspace and paperspace entities can be contained in a group.  AutoCAD will not recognize
    entities owned by other blocks that are placed into a group.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbGroup: public OdDbObject
{
public:
  ODDB_DECLARE_MEMBERS(OdDbGroup);

  /** Description:
      Constructor (no arguments).
  */
  OdDbGroup();

  /** Description:
      Returns an iterator that provides access to the entities in this group.
  */
  OdDbGroupIteratorPtr newIterator();

  /** Description:
      Returns the description for this group (DXF 300).
  */
  OdString description() const;

  /** Description:
      Sets the description for this group (DXF 300).
  */
  void setDescription(const OdString& grpDesc);

  /** Description:
      Returns the selectable flag for this group (DXF 71).
  */
  bool isSelectable() const;

  /** Description:
      Sets the selectable flag for this group (DXF 71).
  */
  void setSelectable(bool selectable);

  /** Description:
      Returns the name for this group (DXF: stored in parent dictionary).
  */
  OdString name() const;

  /** Description:
      Sets the name for this group (DXF: stored in parent dictionary).
  */
  void setName(const OdString& newName);

  /** Description:
      Returns true if this group is not accessible (for example if it is in an xref).
  */
  bool isNotAccessible() const;

  /** Description:
      Returns true if this group is anonymous, false otherwise (DXF 70).
  */
  bool isAnonymous() const;

  /** Description:
      Sets this group to be anonymous (DXF 70).
  */
  void setAnonymous();

  /** Description:
      Appends the specified object to this group.  

      Arguments:
      id (I) Object ID of OdDbEntity to be added to this group.
  */
  void append(OdDbObjectId id);

  /** Description:
      Appends the specified objects to this group.  

      Arguments:
      ids (I) Object ID array of OdDbEntity objects to be added to this group.
  */
  void append(const OdDbObjectIdArray& ids);

  /** Description:
      Prepends the specified object to this group.  

      Arguments:
      id (I) Object ID of OdDbEntity to be prepended to this group.
  */
  void prepend(OdDbObjectId id);

  /** Description:
      Prepends the specified objects to this group.  

      Arguments:
      ids (I) Object ID array of OdDbEntity objects to be prepended to this group.
  */
  void prepend(const OdDbObjectIdArray& ids);

  /** Description:
      Inserts the specified object into this group, at the specified location.

      Arguments:
      idx (I) Index within this group at which to insert the object.
      id (I) Object ID of OdDbEntity to be inserted.
  */
  void insertAt(OdUInt32 idx, OdDbObjectId id);

  /** Description:
      Inserts the specified objects into this group, at the specified location.

      Arguments:
      idx (I) Index within this group at which to insert the objects.
      ids (I) Object ID array of OdDbEntity objects to be inserted.
  */
  void insertAt(OdUInt32 idx, const OdDbObjectIdArray& ids);

  /** Description:
      Removes the specified object from this group.
  */
  void remove(OdDbObjectId id);

  /** Description:
      Removes the object at the specified index from this group.
  */
  void removeAt(OdUInt32 idx);

  /** Description:
      Removes the specified objects from this group.
  */
  void remove(const OdDbObjectIdArray& ids);

  /** Description:
      Removes the specified objects from this group, starting from the specified
      index.  In order to be removed, each object must appear at or beyond the specified
      index.
  */
  void removeAt(OdUInt32 idx, const OdDbObjectIdArray& ids);

  /** Description:
      Replace one object in this group with another.

      Arguments:
      oldId (I) Object ID of object in the group that is to be replaced.
      newId (I) Object ID of object which will replace oldId.
  */
  void replace(OdDbObjectId oldId, OdDbObjectId newId);

  /** Description:
      Transfers a specified number of items in this group from one location to another.

      Arguments:
      fromIndex (I) Starting index of objects to be moved.
      toIndex (I) Destination index, or the index where the objects will be relocated to.
      numItems (I) Number of objects to move.
  */
  void transfer(OdUInt32 fromIndex, OdUInt32 toIndex, OdUInt32 numItems);

  /** Description:
      Clears the contents of this group, so that it contains no entities.
  */
  void clear();

  /** Description:
      Returns the number of entities in this group.
  */
  OdUInt32 numEntities() const;

  /** Description:
      Returns true if this group contains the specified entity, false otherwise.
  */
  bool has(const OdDbEntity* pEntity) const;

  /** Description:
      Returns the number of entities in this group, and their Object ID's.

      Arguments:
      ids (O) Receives the Object ID's of all entities in this group.

      Return Value:
      The number of entities in this group.
  */
  OdUInt32 allEntityIds(OdDbObjectIdArray& ids) const;

  /** Description:
      Returns the index of the specified object within this group.

      Arguments:
      id (I) Object ID for which the index is to be retrieved.
      idx (O) Index of id within this group.
      @throw OdError(eInvalidInput)
  */
  void getIndex(OdDbObjectId id, OdUInt32& idx) const;

  /** Description:
      Reverses the order of the entities contained in this group.
  */
  void reverse();

  /** Description:
      Sets the color of all entities in this group to the specified color.
  void setColor(const OdCmColor &color);
  */

  /** Description:
      Sets the color of all entities in this group to the specified color index.
  */
  void setColorIndex(OdUInt16 color);

  /** Description:
      Sets the color of all entities in this group to the specified color.
  */
  void setColor(const OdCmColor& color);

  /** Description:
      Sets the layer of all entities in this group to the specified layer.
  */
  void setLayer(const OdString& newVal);

  /** Description:
      Sets the layer of all entities in this group to the specified layer.
  */
  void setLayer(OdDbObjectId newVal);

  /** Description:
      Sets the linetype of all entities in this group to the specified linetype.
  */
  void setLinetype(const OdString& newVal);

  /** Description:
      Sets the linetype of all entities in this group to the specified linetype.
  */
  void setLinetype(OdDbObjectId newVal);

  /** Description:
      Sets the linetype scale of all entities in this group to the specified scale value.
  */
  void setLinetypeScale(double newval);

  /** Description:
      Sets the visibility of all entities in this group to the specified value.
  */
  void setVisibility(OdDb::Visibility newVal);

  /** Description:
      Sets the PlotStyleName of all entities in this group to the specified value.
  */
  void setPlotStyleName(const OdString& newVal);

  /** Description:
      Sets the Lineweight of all entities in this group to the specified value.
  */
  void setLineweight(OdDb::LineWeight lw);

  virtual OdResult dwgInFields(OdDbDwgFiler* pFiler);

  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;

  virtual OdResult dxfInFields(OdDbDxfFiler* pFiler);

  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;

  virtual void copied(const OdDbObject* dbObj, const OdDbObject* newObj);

  virtual void getClassID(void** pClsid) const;

  
  /*
      void setHighlight(bool newVal);
      virtual void applyPartialUndo(OdDbDwgFiler* undoFiler, OdRxClass* classObj);
      virtual void subClose();
      virtual OdResult subErase(bool erasing = true);
      virtual void goodbye(const OdDbObject* dbObj);
  */

};

typedef OdSmartPtr<OdDbGroup> OdDbGroupPtr;

#include "DD_PackPop.h"

#endif //_ODDBGROUP_INCLUDED_


