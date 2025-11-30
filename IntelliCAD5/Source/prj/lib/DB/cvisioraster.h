//=====================================================================
//
//                     IntelliCAD Raster Support DLL
//   Copyright (c) 1997 by Hitachi Software Engineering America, Ltd.
//                          All Rights Reserved
//
//                  Hitachi Software Company Confidential
//
//  This code and all associated files are the proprietary property of
//  Hitachi Software.  No portion or portions may be used without the
//  express written permission of Hitachi Software.
//
//---------------------------------------------------------------------
//
//  File:  CVisioRaster.h
//  Desc:  CVisioRaster definition which is an abstract base class
//          for CHiisRaster.
//
//  Programmer:  Geoffrey P. Vandegrift
//
//---------------------------------------------------------------------
//
//  
//  9     3/04/98 10:32a Gvandegrift
//  Added setVal and getVal methods as a "catch-all"
//  
//  8     2/13/98 1:49p Gvandegrift
//  - Added undo/redo calls.
//  - Changed parameters being passed by drawRaster.
//  
//  7     1/14/98 11:38a Gvandegrift
//  Added closeRaster/openRaster methods to allow access to the pixelmap
//  and palette.
//  
//  6     1/07/98 4:17p Gvandegrift
//  Added readHeader and getModTime methods.
//  
//  5     12/10/97 2:39p Gvandegrift
//  Added nMode param to drawRaster method.
//  
//  4     9/25/97 11:16a Gvandegrift
//  Added writeImage method.
//  
//  3     9/22/97 9:22a Gvandegrift
//  Changed how sds_point's are being passed.
//  
//  2     9/18/97 1:39p Gvandegrift
//  Removed some stray text.
//  
//  1     9/18/97 12:05p Gvandegrift
//  Initial revision.
//  
//=====================================================================

#ifndef __CVISIORASTER_H__
#define __CVISIORASTER_H__

class CVisioRaster {
public:
    CVisioRaster(void) {};
    virtual ~CVisioRaster(void) {};

    // Set the units currently being used by the drawing.  Must be called 
    // before loading the image.
    virtual void setUnits(const IRas::Units& units) = 0;

    // Load an image from a file
    virtual IRas::ErrorCode loadImage(
        const char *szFileName,                 // File name of image
        int (*updatePercent)(short nPercent),   // Pointer to progress function (see below)
        const char *szDLLPath) = 0;             // Path to reader/writer dlls

    // Load an image from a byte stream
    virtual IRas::ErrorCode loadImage(
        unsigned char *pcByteStream,            // Pointer to a byte stream that contains the raster info
        long lBufSize,                          // Size of the byte stream
        const char *szFileType,                 // Type of file (matching the strings returned by hiisGetRasterReadFileFilter)
        int (*updatePercent)(short nPercent),   // Pointer to progress function (see below)
        const char *szDLLPath) = 0;             // Path to reader dlls

    // Read the header information from a raster file
    virtual IRas::ErrorCode readHeader(
        const char *szFileName,                 // File name of image
        const char *szDLLPath) = 0;             // Path to reader/writer dlls

    // Write the image currently contained in the object
    virtual IRas::ErrorCode writeImage(
        const char *szFileName,                 // File name of image to write
        int (*updatePercent)(short nPercent),   // Pointer to progress function (see below)
        const char *szDLLPath) = 0;             // Path to reader/writer dlls

    // Unloads raster image (frees the pixelmap buffer)
    virtual void unloadImage(void) = 0;

    // Read position info from a position file.  Must be called AFTER
    // one of the loadImage overloads.
    virtual IRas::ErrorCode loadPosition(const char *szFileName) = 0;  // Position file name

    // Read position info from a byte stream.  Must be called AFTER
    // one of the loadImage overloads
    virtual IRas::ErrorCode loadPosition(
        unsigned char *pcByteStream,    // Pointer to byte stream containing position file info
        long lBufSize,                  // Size of the byte stream
        const char *szFileType) = 0;    // Type of file (matching the strings returned by the hiisGetPositionFileFilter)

    // Display or plot raster image
    virtual IRas::ErrorCode drawRaster(
        const CDC& dc,                      // Device context to draw to
        const sds_point& ptRasterLL,        // Lower left of raster in device coords
        const sds_point& ptRasterUL,        // Upper left
        const sds_point& ptRasterUR,        // Upper right
        const sds_point& ptRasterLR,        // Lower right
        const sds_point& ptViewLL,          // Lower left of "viewport" in device coords
        const sds_point& ptViewUR,          // Upper right
        const long& nBackgroundColor,       // RGB value of background (necessary for Fade)
        const long& nMonoColor,             // RGB color used to display the "on" bits of a monochrome image
        const int& nMode,		            // Draw mode (see sds_redraw description)
		const IRas::Quality& quality,       // Image quality (draft or high)
		struct sds_resbuf *rbImageDef       // IMAGEDEF entity resbuf (has clipping boundary, fade, etc.)
		) const = 0;
    // Note:  The ptRasterXX values should be set according to the orientation
    // of the image before it has been rotated in any way.  

    // See if the image is loaded
    virtual unsigned char isLoaded(void) const = 0;

    // Open and close the image definition for third parties
    virtual void *openRaster(long *nUndoId) = 0;
    virtual void closeRaster(void) = 0;
    
    // Undo redo
    virtual IRas::ErrorCode undo(const long &nUndoId = -1) = 0;
    virtual IRas::ErrorCode redo(const long &nUndoId = -1) = 0;

    // Some "catch-all" methods
    virtual IRas::ErrorCode setVal(const int& nFlag, void *pVal) = 0;
    virtual IRas::ErrorCode getVal(const int& nFlag, void *pVal) = 0;

    // The following methods must be called after an image has been
    // loaded.  If a position file is loaded after an image, they will
    // return information obtained from the position file.

    // Get the default origin (provided by image header or position file)
    virtual void getOrigin(sds_point pt) const = 0;
    // Return the default scale (provided by image header or position file)
    virtual void getScale(sds_point pt) const = 0;
    // Return the default rotation in radians (provided by image header or position file)
    virtual double getRotation(void) const = 0;
    // Get default raster units
    virtual IRas::Units getUnits(void) const = 0;
    // Get x and y resolution of image in dpi
    virtual void getResolution(sds_point pt) const = 0;
    // Get the size of the image in Kilobytes
    virtual double getSize(void) const = 0;
    // Get color depth in bits per pixel
    virtual short getColorDepth(void) const = 0;
    // Get the x and y pixel dimensions
    virtual void getPixelDimensions(sds_point pt) const = 0;
    // Get default monochrome color
    virtual void getMonoColor(long *pRGB) const = 0;
    // Get last modification time for file
    virtual char *getModTime() const = 0;

private:
    // Define these next two so the compiler won't
    CVisioRaster(const CVisioRaster& arg);
    const CVisioRaster& operator=(const CVisioRaster&);
};

//
// The progress callback has the following format:
//
// int updatePercent(short nPercent);
//
// The nPercent parameter will be set to how far along the read 
// process is.  It has a range of 0 - 100.
// 
// A return value is expected.  A return value of 0 means that the
// read process should be cancelled.  Otherwise, it will continue.
//

#endif // __CVISIORASTER_H__

// eof - CVisioRaster.h
