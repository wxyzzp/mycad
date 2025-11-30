#ifndef __GI_VIEWPORT__
#define __GI_VIEWPORT__

#include "RxObject.h"

class OdGePoint2d;
class OdGePoint3d;
class OdGeVector3d;
class OdDbStub;
class OdGeMatrix3d;

#include "DD_PackPush.h"

/** Description:
    Represents a viewport within the DWGdirect vectorization framework.  Client code can query this
    object for information about the current view, in order to create the proper 
    viewport-dependent geometric representation for an object.

    {group:OdGi_Classes} 
*/
class FIRSTDLL_EXPORT OdGiViewport : public OdRxObject
{ 
public:
  ODRX_DECLARE_MEMBERS(OdGiViewport);
  
  /** Description:
      Returns the current transformation from model space to eye space.

      See Also:

      o Coordinate Systems
  */
  virtual OdGeMatrix3d getModelToEyeTransform() const = 0;

  /** Description:
      Returns the current transformation from eye space to model space.

      See Also:

      o Coordinate Systems
  */
  virtual OdGeMatrix3d getEyeToModelTransform() const = 0;

  /** Description:
      Returns the current transformation from world space to eye space.

      See Also:

      o Coordinate Systems
  */
  virtual OdGeMatrix3d getWorldToEyeTransform() const = 0;

  /** Description:
      Returns the current transformation from world space to eye space.

      See Also:

      o Coordinate Systems
  */
  virtual OdGeMatrix3d getEyeToWorldTransform() const = 0;

  /** Description:
      Returns true if perspective mode is on for this viewport, false otherwise.
  */
  virtual bool isPerspective() const = 0;

  /** Description:
      Transforms a specified point from eye coordinates to normalized device coordinates
      (or in other words, applies the current perspective transformation to the point).

      Arguments:
        p (I/O) Point to be transformed.

      Return Value:
      true if a perspective transformation is active, and the point was successfully transformed.
      Otherwise false, indicating that a perspective transform is not active, or the point is too
      close to or behind the camera.

      See Also:

      o Coordinate Systems
  */
  virtual bool doPerspective(OdGePoint3d& p) const = 0;

  /** Description:
      Transforms a specified point from normalized device coordinates to eye coordinates
      (or in other words, applies the inverse of the current perspective transformation to the point).

      Arguments:
        p (I/O) Point to be transformed.

      Return Value:
      true if a perspective transformation is active, and the point was successfully transformed.
      Otherwise false, indicating that a perspective transform is not active, or the point is too
      close to or behind the camera.

      See Also:

      o Coordinate Systems
  */
  virtual bool doInversePerspective(OdGePoint3d&) const = 0;
  
  /** Description:
      Calculates the number of device pixels in a WCS unit square.

      Arguments
        givenWorldpt (I) WCS point at which to determine the number of deviced pixels (used in perspective mode only, see Remarks).
        pixelArea (O) Receives the number of device pixels in a WCS unit square.

      Remarks:
      In perspective mode, a WCS unit square contains fewer pixels as it moves further away from
      the camera, so the givenWorldpt argument is provided to specify the exact location at which
      the calculation is to be performed.
       
      This function can be used to determine if the geometry generated for an object will 
      be smaller than the size of a pixel, in which case it may be possible to 
      use an optimized form of the geometry since details will not be discernable.
  */
  virtual void getNumPixelsInUnitSquare(const OdGePoint3d& givenWorldpt, OdGePoint2d& pixelArea) const = 0;
  
  /** Description:
      Returns the camera or eye location for this viewport, in WCS.
  */
  virtual OdGePoint3d getCameraLocation() const = 0;

  /** Description:
      Returns the camera target point for this viewport, in WCS.  The camera target is the 
      point at which the camera is aimed.
  */
  virtual OdGePoint3d getCameraTarget() const = 0;

  /** Description:
      Returns the camera "up vector" for this viewport, in WCS.  The up vector specifies the 
      orientation of the camera (changing the up vector rotates the camera around the vector 
      formed between the camera location and camera target).
  */
  virtual OdGeVector3d getCameraUpVector() const = 0;

  /** Description:
      Returns the vector from the camera target point to the camera location.
  */
  virtual OdGeVector3d viewDir() const = 0;
  
  /** Description:
  */
  virtual OdUInt32 viewportId() const = 0;

  /** Description:
  */
  virtual OdInt16  acadWindowId() const = 0;

  /** Description:
      Returns the lower left and upper right coordinates of this viewport, in 
      Normalized Device Coordinates.

      Arguments:
        lower_left (O) Receives the lower left corner of this viewport.
        uppre_right (O) Receives the upper right corner of this viewport.

      Remarks:
      This information can be used to position fixed-size or fixed-position 
      items on a viewport.

      See Also:

      o Coordinate Systems      
  */
  virtual void getViewportDcCorners(OdGePoint2d& lower_left, OdGePoint2d& upper_right) const = 0;
  
  /** Description:
      Returns information about the front and back clipping planes used by this viewport.

      Arguments:
        clip_front (O) Set to true if a front clipping plane is active, false otherwise.
        clip_back (O) Set to true if a back clipping plane is active, false otherwise.
        front (O) Receives the Z value of the front clipping plane (in eye coordinates), if the front clip is active.
        back (O) Receives the Z value of the back clipping plane (in eye coordinates), if the back clip is active.

      Return Value:
      Returns true if either clip_front or clip_back (or both) were set to true, false otherwise.

      Remarks:
      Clipping planes are always perpendicular to the view direction. 
      Geometry in front of the front clipping plane or in back of the back clipping plane 
      is not displayed, if the respective clipping plane is active.

  */
  virtual bool getFrontAndBackClipValues(bool& clip_front, bool& clip_back, double& front, double& back) const = 0;
  
  /** Description:
      Returns a multiplier that is used to scale all linetypes in this viewport.
  */
  virtual double linetypeScaleMultiplier() const = 0;
  
  /** Description:
      Returns a tolerance value used to determine if a non-continous linetype should be rendered
      as continuous, in cases where the pattern is so small that it will appear as continous anyway.
      If the length of the entire pattern (WCS) is less than this tolerance value, then
      the linetype will be rendered as continuous.
  */
  virtual double linetypeGenerationCriteria() const = 0;

  /** Description:
      Returns true if the passed in layer is is not frozen (either in this viewport or globally),
      false otherwise.
  */
  virtual bool layerVisible(OdDbStub* idLayer) const = 0;

};

#include "DD_PackPop.h"

#endif // __GI_VIEWPORT__
