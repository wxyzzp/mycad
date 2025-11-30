// File: GeLibVersion.cpp
//

#include "GeLibVersion.h"

//////////////////////////////////////////////////
// CGeLibVersion class implementation
//////////////////////////////////////////////////

CGeLibVersion::CGeLibVersion()
{
}

CGeLibVersion::CGeLibVersion(const CGeLibVersion& source)
{
}

CGeLibVersion::CGeLibVersion(unsigned char major, unsigned char minor, unsigned char corrective, unsigned char internal_version)
{
}

unsigned char
CGeLibVersion::majorVersion() const
{
	return '0';
}

unsigned char
CGeLibVersion::minorVersion() const
{
	return '0';
}

unsigned char
CGeLibVersion::correctiveVersion() const
{
	return '0';
}

unsigned char
CGeLibVersion::schemaVersion() const
{
	return '0';
}

CGeLibVersion&
CGeLibVersion::setMajorVersion(unsigned char val)
{
	return *this;
}

CGeLibVersion&
CGeLibVersion::setMinorVersion(unsigned char val)
{
	return *this;
}

CGeLibVersion&
CGeLibVersion::setCorrectiveVersion(unsigned char val)
{
	return *this;
}

CGeLibVersion&
CGeLibVersion::setSchemaVersion(unsigned char val)
{
	return *this;
}

bool
CGeLibVersion::operator == (const CGeLibVersion& otherObj) const
{
	return false;
}

bool
CGeLibVersion::operator != (const CGeLibVersion& otherObj) const
{
	return false;
}

bool
CGeLibVersion::operator < (const CGeLibVersion& otherObj) const
{
	return false;
}

bool
CGeLibVersion::operator <= (const CGeLibVersion& otherObj) const
{
	return false;
}
bool
CGeLibVersion::operator > (const CGeLibVersion& otherObj) const
{
	return false;
}
bool
CGeLibVersion::operator >= (const CGeLibVersion& otherObj) const
{
	return false;
}
