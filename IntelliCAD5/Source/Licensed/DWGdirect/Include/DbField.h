///////////////////////////////////////////////////////////////////////////////
// Copyright c 2002, Open Design Alliance Inc. ("Open Design") 
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
//      DWGdirect c 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////

#ifndef OD_DBFIELD_H
#define OD_DBFIELD_H

#include "DD_PackPush.h"

#include "DbObject.h"
#include "DbFieldValue.h"

class OdDbField;
typedef OdArray<OdDbField*> OdDbFieldArray;


/** Description:

    {group:Other_Classes}
*/
typedef struct OdFd
{
  // Enum for acdbEvaluateFields
  enum EvalFields
  {
    kEvalRecursive      = (0x1 << 0)    // Recursively evaluate complex objects
  };
  
} OdFd;

class OdDbField;
typedef OdSmartPtr<OdDbField> OdDbFieldPtr;

//*************************************************************************
// OdDbField
//*************************************************************************

/** Description:

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbField : public OdDbObject
{
public:
  enum State
  {
    kInitialized        = (0x1 << 0),       // Field has been initialized by the evaluator
    kCompiled           = (0x1 << 1),       // Field has beene compiled
    kModified           = (0x1 << 2),       // Field code has been modified, but not yet evaluated
    kEvaluated          = (0x1 << 3),       // Field has been evaluated
    kHasCache           = (0x1 << 4),       // Field has evaluated cache
    kHasFormattedString = (0x1 << 5)        // For internal use only. Field has cached formatted string. 
  };
  
  enum EvalOption
  {
    kDisable            = 0,                // Disable evaluation of field
    kOnOpen             = (0x1 << 0),       // Evaluate on drawing open
    kOnSave             = (0x1 << 1),       // Evaluate on drawing save
    kOnPlot             = (0x1 << 2),       // Evaluate on drawing plot
    kOnEtransmit        = (0x1 << 3),       // Evaluate on drawing etransmit
    kOnRegen            = (0x1 << 4),       // Evaluate on drawing regen
    kOnDemand           = (0x1 << 5),       // Evaluate on demand
    kAutomatic          = (kOnOpen | kOnSave | kOnPlot | 
                           kOnEtransmit | kOnRegen | kOnDemand)    // No restriction
  };
  
  enum EvalContext
  {
    kOpen               = (0x1 << 0),       // Field being evaluated on drawing open
    kSave               = (0x1 << 1),       // Field being evaluated on drawing save
    kPlot               = (0x1 << 2),       // Field being evaluated on drawing plot
    kEtransmit          = (0x1 << 3),       // Field being evaluated on drawing etransmit
    kRegen              = (0x1 << 4),       // Field being evaluated on drawing regen
    kDemand             = (0x1 << 5),       // Field being evaluated on demand
    kPreview            = (0x1 << 6)        // Field being evaluated for preview
  };
  
  enum EvalStatus
  {
    kNotYetEvaluated    = (0x1 << 0),       // Not yet evaluated
    kSuccess            = (0x1 << 1),       // Evaluated successfully
    kEvaluatorNotFound  = (0x1 << 2),       // Evaluator not found
    kSyntaxError        = (0x1 << 3),       // Field code syntax error
    kInvalidCode        = (0x1 << 4),       // Invalid field code
    kInvalidContext     = (0x1 << 5),       // Invalid context to evaluate field
    kOtherError         = (0x1 << 6)        // Evaluation error
  };
  
  enum FieldCodeFlag
  {
    kFieldCode          = (0x1 << 0),       // Get raw field code. Used only in getFieldCode().
    kEvaluatedText      = (0x1 << 1),       // Get evaluated text. Used only in getFieldCode().
    kEvaluatedChildren  = (0x1 << 2),       // Get field code with evaluated text for child fields. Used only in getFieldCode().
    kObjectReference    = (0x1 << 3),       // Get field code as object reference. Used only in getFieldCode().
    kAddMarkers         = (0x1 << 4),       // Include markers around field codes. Used only in getFieldCode().
    kEscapeBackslash    = (0x1 << 5),       // Convert single backslashes to double backslashes. Used only in getFieldCode().
    kStripOptions       = (0x1 << 6),       // Strip the standard options from field code. Used only in getFieldCode().
    kPreserveFields     = (0x1 << 7),       // Try to preserve existing fields. Used only in setFieldCode().
    kTextField          = (0x1 << 8)        // Treat the field as text with embedded fields. Used only in setFieldCode().
  };
  
  enum FilingOption
  {
    kSkipFilingResult   = (0x1 << 0)        // Don't file field value
  };
  
public:
  ODDB_DECLARE_MEMBERS(OdDbField);
  
  OdDbField(void);

  // OdDbField(const OdString& fieldCode, bool bTextField = false);
  // ~OdDbField(void);
  
  
  OdResult setInObject (OdDbObject* pObj, const OdString& propName);
  OdResult postInDatabase(OdDbDatabase* pDb);
  
  OdDbField::State state (void) const;
  OdDbField::EvalStatus evaluationStatus() const; 
                                          // OdUInt32* errCode = NULL, 
                                         // OdString* errMsg = NULL) const;
  OdDbField::EvalOption evaluationOption(void) const;
  OdResult setEvaluationOption(OdDbField::EvalOption nEvalOption);
  OdDbField::FilingOption filingOption(void) const;
  OdResult setFilingOption(OdDbField::FilingOption nOption);
  
  OdString evaluatorId (void) const;
  OdResult setEvaluatorId(const OdString& evaluatorId);
 
  bool isTextField (void) const;

  // OdResult convertToTextField (void);

  // OdString getFieldCode     (OdDbField::FieldCodeFlag nFlag, OdArray<OdDbField*>* pChildFields = NULL, 
  //                           OdDb::OpenMode mode = OdDb::kForRead) const;

  OdString getFieldCode(OdDbField::FieldCodeFlag nFlag);

  //                      OdArray<OdDbField*>* pChildFields = NULL, 
  //                      OdDb::OpenMode mode = OdDb::kForRead) const;
  // OdResult setFieldCode(OdString& pszFieldCode, OdDbField::FieldCodeFlag nFlag = (OdDbField::FieldCodeFlag) 0,
  //                      OdDbFieldArray* pChildFields = NULL);
  
  OdUInt32  childCount (void) const;
  OdDbFieldPtr getChild  (OdUInt32 iIndex, OdDb::OpenMode mode);

  OdString getFormat (void) const;
  OdResult setFormat (const OdString& format);
          
  // OdResult evaluate    (int nContext, OdDbDatabase* pDb, 
  //                      int* pNumFound     = NULL, int* pNumEvaluated = NULL);
  
  
  OdFieldValue::DataType dataType (void) const;
            
  OdString getValue (void) const;
  OdResult getValue (OdFieldValuePtr& value) const;
            
  /*
  bool  hasHyperlink            (void) const;
  OdResult getHyperlink(char** pszName,
                        char** pszDescription, 
                        char** pszSubLocation,
                        char** pszDisplayString) const;
  OdResult setHyperlink(const char* pszName, 
                        const char* pszDescription, 
                        const char* pszSubLocation = NULL);
  OdResult removeHyperlink(void);
  */
  
  OdFieldValuePtr getData (const OdString& key) const;
  OdResult setData (const OdString& key, const OdFieldValuePtr& pData);

public:
  // Base class overrides

  virtual OdResult dwgInFields (OdDbDwgFiler* pFiler);
  virtual void dwgOutFields(OdDbDwgFiler* pFiler) const;
  virtual OdResult dxfInFields (OdDbDxfFiler* pFiler);
  virtual void dxfOutFields(OdDbDxfFiler* pFiler) const;
  
  // virtual OdResult subClose();
};

#include "DD_PackPop.h"
#endif // OD_DBFIELD_H
