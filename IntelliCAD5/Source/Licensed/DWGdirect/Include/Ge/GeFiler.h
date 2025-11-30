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
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GE_FILER_H
#define OD_GE_FILER_H

//
// Description:
//
// This file implements the class OdGeFiler, which defines the
// abstract interface for serialization requirements of ge entities. 
// An application that requires input/output of the definition of
// ge entities in a format defined by the application should provide 
// an implementation of this class.  An instance of this class is
// provided as an argument to the entity serialization functions, 
// thus allowing the input/output format to be controlled by the 
// client of gelib. An example of usage would be as follows.
//
// OdRfDwgFiler  format;
// OdGeVersion   dbVersion;
// OdGeLine3d    newLine;
// if (acmeGetGelibImageVersion(entity, dbVersion) == OdMe::eOk) 
// {
//     OdGeDwgIO::read(format, dbVersion, newLine) 
//     OdGeDwgIO::write(format, newLine)
// }
//
     
     
class OdDbDwgFiler;
     
/** Description:

    {group:OdGe_Classes} 
*/
class OdGeFiler
{
protected:
  OdGeFiler();
public:
    // Read/write functions.
    //
    virtual
    void      readbool(bool*) = 0; 
    virtual
    void      writebool(bool) = 0;

    virtual
    void      readBool(bool*) = 0; 
    virtual
    void      writeBool(bool) = 0;

    virtual
    void      readChar(char*) = 0; 
    virtual
    void      writeChar(char) = 0;
     
    virtual
    void      readShort(short*) = 0; 
    virtual
    void      writeShort(short) = 0;
     
    virtual
    void      readLong(long*) = 0; 
    virtual
    void      writeLong(long) = 0;
     
    virtual
    void      readUChar(unsigned char*) = 0; 
    virtual
    void      writeUChar(unsigned char) = 0;
     
    virtual
    void      readUShort(unsigned short*) = 0; 
    virtual
    void      writeUShort(unsigned short) = 0;
     
    virtual
    void      readULong(unsigned long*) = 0; 
    virtual
    void      writeULong(unsigned long) = 0;
          
    virtual
    void      readDouble(double*) = 0; 
    virtual
    void      writeDouble(double) = 0;
     
    virtual
    void      readPoint2d(OdGePoint2d*) = 0; 
    virtual
    void      writePoint2d(const OdGePoint2d&) = 0;
     
    virtual
    void      readPoint3d(OdGePoint3d*) = 0; 
    virtual
    void      writePoint3d(const OdGePoint3d&) = 0;
     
    virtual
    void      readVector2d(OdGeVector2d*) = 0; 
    virtual
    void      writeVector2d(const OdGeVector2d&) = 0;
     
    virtual
    void      readVector3d(OdGeVector3d*) = 0; 
    virtual
    void      writeVector3d(const OdGeVector3d&) = 0;
     
    // This method must be implemented in order to read/write 
    // external curves and surfaces (and nurb surfaces). 
    virtual
    OdDbDwgFiler*          dwgFiler();
};

inline
OdDbDwgFiler* OdGeFiler::dwgFiler()
{
    return NULL;
}

#endif // OD_GE_FILER_H

