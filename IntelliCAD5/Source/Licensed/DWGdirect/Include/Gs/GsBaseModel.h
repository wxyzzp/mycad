#ifndef __ODGSMODELIMPL_H__
#define __ODGSMODELIMPL_H__


class OdGsNode;
class OdGsBaseVectorizeView;

#include "GsExport.h"

#include "Gi/GiDrawable.h"
#include "Gs/GsModel.h"
#include "UInt8Array.h"
#include "Gi/GiCommonDraw.h"
#include "StaticRxObject.h"


class OdGsBaseModel;

/** Description:
    Serves as a placeholder for a drawable object, that can be used to 
    implement custom caching support for rendering applications.  

    Remarks:
    For example, a client application can create a custom OdGsNode descendent object
    for each drawable object in a database, and can store cached rendering information
    in the descendent.  This cached information can then be fed into the DWGdirect
    framework during a redraw (instead of making the normal calls to worldDraw and 
    viewportDraw), to greatly improve the speed of redraws.

    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsNode : public OdRxObject
{
  friend class OdGsBaseModel;

  mutable OdGsNode*     m_pPrev;
  mutable OdGsNode*     m_pNext;
protected:

  OdGsBaseModel*        m_pModel;
  OdDbStub*             m_underlayingDrawableId;
  mutable OdGiDrawable* m_pUnderlayingDrawable;

  void clearDrawable()
  {
    if(m_pUnderlayingDrawable)
    {
      m_pUnderlayingDrawable->release();
      m_pUnderlayingDrawable = 0;
    }
    m_underlayingDrawableId = 0;
  }
public:
  ODRX_DECLARE_MEMBERS(OdGsNode);

  /** Remarks:
      OdGsNode objects perform no reference counting.
  */
  void addRef() {}

  /** Remarks:
      OdGsNode objects perform no reference counting.
  */
  void release() {}

  OdGsNode(OdGsBaseModel* pModel, const OdGiDrawable* pUnderlayingDrawable);
  virtual ~OdGsNode();

  /** Description:
      Returns true if this node serves as a container for other nodes, false otherwise.
  */
  virtual bool isContainer() const;

  /** Description:
      Instructs this node to vectorize itself into the specified view object, using
      cached data if possible.

      Arguments:
        pView (I/O) View into which this object should vectorize itself.
  */
  virtual void draw(OdGsBaseVectorizeView* pView) = 0;

  /** Description:
      Returns true if this cached data in this node is valid, or false otherwise 
      (indicating that the cached data must be regenerated).
  */
  virtual bool valid() const = 0;

  /** Description:
      Invalidates the cached data within this node, so that it will be regenerated
      the next time it is accessed.
  */
  virtual void invalidate(OdGsModel::InvalidationHint hint = OdGsModel::kInvalidateAll) = 0;

  /** Description:
      Opens the OdGiDrawable object associated with this node (prior to vectorizing it).
  */
  bool open() const;

  /** Description:
      Closes the OdGiDrawable object associated with this node (after vectorizing it).
  */
  void close() const;

  /** Description:
      Returns the underlying OdGiDrawable object associated with this node.
  */
  OdGiDrawable* underlayingDrawable() { return m_pUnderlayingDrawable; }

  /** Description:
      Instructs this node to vectorize itself into the specified view object.
      This method should generate and stored cached rendering data if possible.

      Arguments:
        nDrawableFlags (I) Drawable flags returned by the call to setAttributes on this drawable object.
        pDrawable (I) Original drawable object associated with this node.
        pView (I/O) View into which this object should vectorize itself.
  */
  virtual void drawNode(OdUInt32 nDrawableFlags, 
    const OdGiDrawable* pDrawable, 
    OdGsBaseVectorizeView* pView) = 0;
};

typedef OdArray<OdGsNode*, OdMemoryAllocator<OdGsNode*> > OdGsNodePtrArray;


/** Description:
    OdGsNode descendent that can serve as a container for other OdGsNode objects.

    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsContainerNode : public OdGsNode
{
protected:
  OdGsContainerNode(OdGsBaseModel* pModel, const OdGiDrawable* pUnderlayingDrawable)
    : OdGsNode(pModel, pUnderlayingDrawable)
  {
  }
public:
  ODRX_DECLARE_MEMBERS(OdGsContainerNode);

  /** Remarks:
      Returns true.
  */
  virtual bool isContainer() const;

  /** Description:
      Adds a child node to this container.
  */
  virtual void addChild(OdGsNode* pNode) = 0;

  /** Description:
      Removes the specified child node from this container.
  */
  virtual void removeChild(OdGsNode* pNode) = 0;
};


/** Description:
    Model class that can be used to coordinate custom caching support for 
    DWGdirect vectorization applications.  Clients should derive their
    custom model classes from this class.

    {group:OdGs_Classes} 
*/
class GS_TOOLKIT_EXPORT OdGsBaseModel : public OdGsModel
{
  OdGiContext*  m_pGiContext;
  OdGsNode*     m_pNodes;

  friend class OdGsNode;

  /** Description:
      Adds the specified node to this model.
  */
  void addNode(OdGsNode* pNew);

  /** Description:
      Creates and returns a new node.
      
      Arguments:
        pDrawable (I) Drawable object to associate with the new node.
        bContainer (I) If true, this function calls createContainerNode to create the node,
          otherwise it calls createEntityNode.
  */
  OdGsNode* createGsNode(const OdGiDrawable* pDrawable, bool bContainer);

  /** Description:
      Returns the node associated with the passed in drawable object.

      Arguments
        pDrawable (I) Drawable object associated with the desired node.
        bContainer (I) If true, pDrawable is associated with a container node.

      Remarks:
      
      o If the node associated with pDrawable is a container node, and bContainer is true,
        this function invalidates and returns the container node.
      o If pDrawable does not have an associated node, then one is created and returned 
        (by calling createGsNode).
      o If pDrawable has an associated non-container or entity node, detach() is called
        on the old node, and a new node is created and returned via createGsNode.
   */
  OdGsNode* getGsNode(const OdGiDrawable* pDrawable, bool bContainer);

protected:

  OdGsBaseModel();
  ~OdGsBaseModel();

  /** Description:
      Deletes the passed in OdGsNode.
  */
  virtual void deleteWrapperObject(OdGsNode* pNode);

  /** Description:
      Creates and returns a container node, that has the passed in object
      as its underlying drawable object.  Client applications should override this 
      function, if they wish to use a custom OdGsContainerNode.
  */
  virtual OdGsContainerNode* createContainerNode(const OdGiDrawable* pUnderlayingDrawable);

  /** Description:
      Creates and returns a new node, that has the passed in object
      as its underlying drawable object.  By overriding this function, client applications
      feed their custom caching nodes into the DWGdirect vectorization framework.
  */
  virtual OdGsNode* createEntityNode(const OdGiDrawable* pUnderlayingDrawable) = 0;

public:
  class NodePredicate
  {
  public:
    virtual void operator()(OdGsNode* pNode) = 0;
  };

  void forEachNode(NodePredicate& doAction);

  static OdDbStub*      kNullId;
  static OdGiDrawable*  kNullDrawable;

  /** Arguments:
        pParentNode (I) If non-NULL, the newly created node will be added to this parent.
        nDrawableFlags (I) Flags returned by the most recent call to OdGiDrawable::setAttributes.
  */
  virtual OdGsNode* getGsNode(OdGsContainerNode* pParentNode, 
    const OdGiDrawable* pDrawable, 
    OdUInt32 nDrawableFlags);

  /** Description:
      Removes the specified node from the model, and calls deleteWrapperObject for the node.
  */
  virtual void detach(OdGsNode* pCache);

  /** Description:
      Opens the passed in object.
  */
  OdGiDrawablePtr open(OdDbStub* objectId);

  void setGiContext(OdGiContext* pGiContext);

  /** Description:
      Returns the OdGiContext associated with this model.
  */
  OdGiContext* giContext() { return m_pGiContext; }

  void onAdded(OdGiDrawable* pAdded, OdGiDrawable* pParent);
  void onAdded(OdGiDrawable* pAdded, OdDbStub* parentID);
  
  void onModified(OdGiDrawable* pModified, OdGiDrawable* pParent);
  void onModified(OdGiDrawable* pModified, OdDbStub* parentID);
  
  void onErased(OdGiDrawable* pErased, OdGiDrawable* pParent);
  void onErased(OdGiDrawable* pErased, OdDbStub* parentID);

  void invalidate(InvalidationHint hint);
};

inline OdGiDrawablePtr OdGsBaseModel::open(OdDbStub* objectId)
{
  ODA_ASSERT(m_pGiContext);
  return m_pGiContext->openDrawable(objectId);
}

inline bool OdGsNode::open() const
{
  if(m_underlayingDrawableId && !m_pUnderlayingDrawable)
  {
    m_pUnderlayingDrawable = m_pModel->open(m_underlayingDrawableId).detach();
  }
  return (m_pUnderlayingDrawable!=0);
}

inline void OdGsNode::close() const
{
  if(m_underlayingDrawableId && m_pUnderlayingDrawable)
  {
    m_pUnderlayingDrawable->release();
    m_pUnderlayingDrawable = 0;
  }
}


#endif // __ODGSMODELIMPL_H__
