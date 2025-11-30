//=====================================================================
//
//                     IntelliCAD Raster Support DLL
// Copyright (c) 1997-98 by Hitachi Software Engineering America, Ltd.
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
//  File:  IRasDef.h
//  Desc:  General definitions, enumerations, etc.
//
//  Programmer:  Geoffrey P. Vandegrift
//
//---------------------------------------------------------------------
//
//  
//  8     5/08/98 2:08p Gvandegrift
//  Some new error codes for undo purposes.
//  
//  7     3/30/98 10:16a Gvandegrift
//  Added the monostatus enumeration.
//  
//  6     3/19/98 8:16a Gvandegrift
//  Added an enumeration for passing arbitrary values.
//  
//  5     2/13/98 1:52p Gvandegrift
//  Forced some enumerations to certain values (to match AutoCAD).
//  
//  4     2/09/98 4:32p Gvandegrift
//  Moved the point2d typedef into the IRas name space.
//  
//  3     2/05/98 11:54a Gvandegrift
//  Added unit enumerations.
//  
//  2     1/07/98 4:20p Gvandegrift
//  Added an imagequality enumeration.
//  
//  1     9/18/97 12:05p Gvandegrift
//  Initial revision.
//  
//=====================================================================

#ifndef __IRasDef_h__
#define __IRasDef_h__

struct IRas {

    // The following error codes are not meant to convey
    // much information.  Extended information should be obtained
    // via calls to hiisGetLastError any time a function
    // returns a value other than IRas::eSuccess
    enum ErrorCode {
        eFailure = 0,
        eSuccess = 1,
        eNothingToUndo,
        eNothingToRedo,
        eReachedUndoLimit
    };

    // An enumeration for unit information
    enum Units {
        Unitless = 0,
        Millimeter = 1,
        Centimeter = 2,
        Meter = 3,
        Kilometer = 4,
        Inch = 5,
        Foot = 6,
        Yard = 7,
        Mile= 8,
        NoUnits    // This is here to see if CHiisRaster::m_units has been initialized
    };

    enum Quality {
        Draft = 0,
        High = 1
    };

    // Miscellaneous parameters passed in setVal and getVal
    enum Value {        // What type is contained in the void* on set/getVal
        OriginUnits,    // Units
        MonoStatus,     // BOOL
    };

    typedef struct {
        double x;
        double y;
    } Point2d;
};

#endif // __IRasDef_h__

// eof - IRasDef.h
