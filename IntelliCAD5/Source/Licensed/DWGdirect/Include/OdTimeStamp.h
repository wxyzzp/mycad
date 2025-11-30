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



#ifndef _ODTIME_INCLUDED_
#define _ODTIME_INCLUDED_

#include "DD_PackPush.h"

/** Description:
    Represents date and time information within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class FIRSTDLL_EXPORT OdTimeStamp
{
public:
  enum InitialValue
  {
    kInitZero          = 1,
    kInitLocalTime     = 2,
    kInitUniversalTime = 3
  };

	/** Constructor.
  */
  OdTimeStamp();

	/** Constructor.
	    @param i Initial value, one of:
	    - kInitZero - Initial value is 0.
	    - kInitLocalTime - Initial value is set to local time.
	    - kInitUniversalTime - Initial value is set to Universal or Greenwich Mean Time.
  */
  OdTimeStamp(InitialValue i);

	/** Returns the date value of this object.
	    @param month (O) Receives the month.
	    @param day (O) Receives the day.
	    @param year (O) Receives the year.
  */
  void getDate(short& month, short& day, short& year) const;

	/** Sets the date value for this object.
	    @param month (I) The month.
	    @param day (I) The day.
	    @param year (I) The year.
  */
  void setDate(short month, short day, short year);

	/** Returns the month.
  */
  short month() const;

	/** Sets the month.
  */
  void setMonth(short);

	/** Returns the day.
  */
  short day() const;

	/** Sets the day.
  */
  void setDay(short);

	/** Returns the year.
  */
  short year() const;

	/** Sets the year.
  */
  void setYear(short);

	/** Gets the time value of this object.
	    @param hour (O) Receives the hour.
	    @param min (O) Receives the minutes.
	    @param sec (O) Receives the seconds.
	    @param msec (O) Receives the milliseconds.
  */
  void getTime(short& hour, short& min, short& sec, short& msec) const;

	/** Sets the time value of this object.
	    @param hour (I) The hour.
	    @param min (I) The minutes.
	    @param sec (I) The seconds.
	    @param msec (I) The milliseconds.
  */
  void setTime(short hour, short min, short sec, short msec);

	/** Returns the hour.
  */
  short hour() const;

	/** Sets the hour.
  */
  void setHour(short);

	/** Returns the minutes.
  */
  short minute() const;

	/** Sets the minutes.
  */
  void setMinute(short);

	/** Returns the seconds.
  */
  short second() const;

	/** Sets the seconds.
  */
  void setSecond(short);

	/** Returns the milliseconds.
  */
  short millisecond() const;

	/** Sets the milliseconds.
  */
  void setMillisecond(short);

	/** Sets this time object to 0.
  */
  void setToZero();

  /** Description:
      Initializes this time object with the current local time.
  */
  void getLocalTime();

	/** Initializes this time object with the current Universal or Greenwich Mean time.
  */
  void getUniversalTime();

	/** Converts this time value from local time to Universal or Greenwich Mean Time.
  */
  void localToUniversal();

	/** Converts this time value from Universal or Greenwich Mean Time, to local time.
  */
  void universalToLocal();

	/** Returns the Julian day value of this date object.
  */
  OdUInt32 julianDay() const;

	/** Sets the Julian day value of this date object.
  */
  void   setJulianDay(OdUInt32 julianDay);

	/** Returns the milliseconds past midnight value for this date object.
  */
  OdUInt32 msecsPastMidnight() const;

	/** Sets the milliseconds past midnight value for this date object.
  */
  void setMsecsPastMidnight(OdUInt32 msec);

	/** Sets this date using a Julian day.
  */
  void setJulianDate(OdUInt32 julianDay, OdUInt32 msec);

	/** Returns the Julian fraction portion of the Julian date.
  */
  double julianFraction() const;

	/** Sets the Julian fraction portion of the Julian date.
  */
  void setJulianFraction(double);

	/** Equality operator.
  */
  bool operator==(const OdTimeStamp&) const;

	/** Greater than operator.
  */
  bool operator>(const OdTimeStamp&) const;

	/** Greater than or equal to operator.
  */
  bool operator>=(const OdTimeStamp&) const;

	/** Less than operator.
  */
  bool operator<(const OdTimeStamp&) const;

	/** Less than or equal to operator.
  */
  bool operator<=(const OdTimeStamp&) const;

  /** Description:
      Addition operator.
  */
  const OdTimeStamp operator+(const OdTimeStamp &date) const;

	/** Subtraction operator.
  */
  const OdTimeStamp operator-(const OdTimeStamp &date) const;

	/** Plus equals operator.
  */
  const OdTimeStamp& operator+=(const OdTimeStamp &date);

	/** Minus equals operator.
  */
  const OdTimeStamp& operator-=(const OdTimeStamp &date);

  /** Description:
      Adds the passed in date to this date, and returns the sum.
  */
  const OdTimeStamp& add(const OdTimeStamp &date);

	/** Subtracts the passed in date from this date, and returns the difference.
  */
  const OdTimeStamp& subtract(const OdTimeStamp &date);

  void ctime( OdString& ) const;

  void strftime(const char *pFormat, OdString &str) const;

  long packedValue() const;
private:
  OdUInt32 m_julianDay;
  OdUInt32 m_msec;
};

inline const OdTimeStamp OdTimeStamp::operator+(const OdTimeStamp& d) const
{
  return OdTimeStamp(*this) += d;
}

inline const OdTimeStamp OdTimeStamp::operator-(const OdTimeStamp& d) const
{
  return OdTimeStamp(*this) -= d;
}

inline bool OdTimeStamp::operator<=(const OdTimeStamp& d) const
{
  return ! operator>(d);
}

inline bool OdTimeStamp::operator>=(const OdTimeStamp& d) const
{
  return ! operator<(d);
}

inline const OdTimeStamp& OdTimeStamp::add(const OdTimeStamp& date)
{
  return operator+=(date);
}

inline const OdTimeStamp& OdTimeStamp::subtract(const OdTimeStamp& date)
{
  return operator-=(date);
}

#include "DD_PackPop.h"

#endif // _ODTIME_INCLUDED_


