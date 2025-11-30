#ifndef _ODEDUSERIO_H_INCLUDED_
#define _ODEDUSERIO_H_INCLUDED_

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


#include "OdPlatform.h"
#include "DbExport.h"
#include "RxObject.h"
#include "OdString.h"


class OdGePoint3d;


/** Description:
    Interface class that defines the base level I/O functions necessary for the 
    execution of a custom command.  

    Remarks:
    By creating a subclass of OdEdBaseIO and overriding the getString and putString
    functions, a client application can provide I/O interaction to a custom command object
    from a custom data source.

    {group:OdEd_Classes}
*/
class FIRSTDLL_EXPORT OdEdBaseIO : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdEdBaseIO);

  /** Description:
      Class that will be thrown from getString if user cancelled input.

      See Also:
      OdEdUserIO::setKeywords
      OdEdUserIO::getKeyword

      Remarks:
      For example, if setKeywords is called prior to a call to getInt with, this function
      will be thrown if one of the specified keywords is input in response to the getInt
      query.
  */
  class InterruptException { };

  /** Description:
      Returns a string obtained in some manner from this custom data source.

      Arguments:
      bAllowSpaces (I) Not used.

      Remarks:
      This function must throw InterruptException exception to cancel the input.

      See Also:
      OdEdUserIO::InterruptException
  */
  virtual OdString getString(bool bAllowSpaces) = 0;

  /** Description:
      Writes the specified string to this custom data source.
  */
  virtual void putString(const OdChar* string) = 0;
};

/** Description:
    Class that provides I/O functionality to a custom command object.

    Remarks:
    OdEdUserIO provides a set of higher level I/O routines, such as getPoint, getInt, etc.
    that can be used in custom command objects.

    {group:OdEd_Classes}
*/
class FIRSTDLL_EXPORT OdEdUserIO : public OdRxObject
{ 
public:
  ODRX_DECLARE_MEMBERS(OdEdUserIO);

  /** Description:
      Class that will be thrown if one of the specified keywords is input in response
      to any query fuction (except getKeyword).

      See Also:
      OdEdUserIO::setKeywords
      OdEdUserIO::getKeyword

      Remarks:
      For example, if setKeywords is called prior to a call to getInt with, this function
      will be thrown if one of the specified keywords is input in response to the getInt
      query.
  */
  class KeywordException
  {
    int      m_nKeywordIndex;
    OdString m_sKeyword;
  public:
    KeywordException(int nKeywordIndex, const OdString& sKeyword)
      : m_nKeywordIndex(nKeywordIndex)
      , m_sKeyword(sKeyword)
    {}

    /** Description:
        Returns index of the input keyword in the list supplied by setKeywords.
    */
    int keywordIndex() const { return m_nKeywordIndex; }
    /** Description:
        Returns keyword that was supplied as input.
    */
    const OdString& keyword() const { return m_sKeyword; }
  };

  /** Description:
      Sets a list of keywords that will be recognized as keyword input by this IO object.

      Arguments:
      pKwList (I) Null terminated string containing keywords, separated by spaces within
        the string.
      bThrowKeyword (I) If true, KeywordException object will be thrown if one of the keywords
        in pKwList is entered as input.

      See Also:
      OdEdUserIO::getKeyword
      OdEdUserIO::KeywordException
  */
  virtual void setKeywords(const OdChar* pKwList, bool bThrowKeyword = false) = 0;

  /** Description:
      Outputs the specified prompt string, and then reads in a keyword and returns the index
      of that keyword in the list of keywords supplied by the setKeyword function.

      Arguments:
      prompt (I) String that is output prior to requesting the keyword value.  If this string
        is empty, a default prompt string is sent to the output.
      pKeyword (I) If non-zero, receives the keyword that was entered (if found in the 
        keyword list).

      Remarks:
      This function uses the underlying OdEdBaseIO functions putString and getString for low
      level output and input.  

      Return Value:
      Index of the entered keyword (if found in the keyword list), otherwise -1.
  */
  virtual int getKeyword(const OdChar* prompt, const OdChar* defVal = 0, OdString* pKeyword = 0) = 0;

  /** Description:
      Outputs the specified prompt string, and then reads in 3 double values separated by commas,
      returning these 3 values in an OdGePoint3d object.

      Arguments:
      prompt (I) String that is output prior to requesting the point values.  If this string
        is empty, a default prompt string is sent to the output.
      pBasePt (I) Unused.
      pDefVal (I) Default return value to be used if an empty string is received as input
        during this operation.

      Remarks:
      This function uses the underlying OdEdBaseIO functions putString and getString for low
      level output and input.  
  */
  virtual OdGePoint3d getPoint   (const OdChar* prompt,
                                  const OdGePoint3d* pBasePt = 0,
                                  const OdGePoint3d* pDefVal = 0) = 0;
                               
  /** Description:
      Outputs the specified prompt string, and then reads in and returns an integer value.

      Arguments:
      prompt (I) String that is output prior to requesting the integer value.  If this string
        is empty, a default prompt string is sent to the output.

      Remarks:
      This function uses the underlying OdEdBaseIO functions putString and getString for low
      level output and input.  
  */
  virtual int         getInt     (const OdChar* prompt) = 0;

  /** Description:

      Arguments:
      defVal (I) Default return value to be used if an empty string is received as input
        during this operation.
  */
  virtual int         getInt     (const OdChar* prompt, int defVal) = 0;
                                 
  /** Description:
      Outputs the specified prompt string, and then reads in and returns a double value.

      Arguments:
      prompt (I) String that is output prior to requesting the double value.  If this string
        is empty, a default prompt string is sent to the output.

      Remarks:
      This function uses the underlying OdEdBaseIO functions putString and getString for low
      level output and input.  
  */
  virtual double      getReal    (const OdChar* prompt) = 0;

  /** Description:

      Arguments:
      defVal (I) Default return value to be used if an empty string is received as input
        during this operation.
  */
  virtual double      getReal    (const OdChar* prompt, double defVal) = 0;
                                 
  /** Description:
      Returns a string obtained in some manner from this custom data source.

      Arguments:
      prompt (I) String that is output prior to requesting the string value.  If this string
        is empty, a default prompt string is sent to the output.
      defVal (I) Default return value to be used if an empty string is received as input
        during this operation.

      Remarks:
      This function uses the underlying OdEdBaseIO functions putString and getString for low
      level output and input.  
  */
  virtual OdString    getString  (bool bAllowSpaces, const OdChar* prompt, const OdChar* defVal = 0) = 0;
                                 
  /** Description:
      Writes the specified string to this custom data source.
  */
  virtual void        putString  (const OdChar* string) = 0;
};


#endif //#ifndef _ODEDUSERIO_H_INCLUDED_

