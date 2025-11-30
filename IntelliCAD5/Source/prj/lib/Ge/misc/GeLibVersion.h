// File: GeLibVersion.h
//
// CGeLibVersion
//

#pragma once


#include "GePch.h"


class GE_API CGeLibVersion
{
public:
	CGeLibVersion();
	CGeLibVersion(const CGeLibVersion& source);
	CGeLibVersion(unsigned char major, unsigned char minor, unsigned char corrective, unsigned char internal_version);

	unsigned char	majorVersion		() const;
	unsigned char	minorVersion		() const;
	unsigned char	correctiveVersion	() const;
	unsigned char	schemaVersion		() const;
	CGeLibVersion&	setMajorVersion		(unsigned char val);
	CGeLibVersion&	setMinorVersion		(unsigned char val);
	CGeLibVersion&	setCorrectiveVersion(unsigned char val);
	CGeLibVersion&	setSchemaVersion	(unsigned char val);
	bool			operator ==			(const CGeLibVersion& otherObj) const;
	bool			operator !=			(const CGeLibVersion& otherObj) const;
	bool			operator <			(const CGeLibVersion& otherObj) const;
	bool			operator <=			(const CGeLibVersion& otherObj) const;
	bool			operator >			(const CGeLibVersion& otherObj) const;
	bool			operator >=			(const CGeLibVersion& otherObj) const;

	static const CGeLibVersion	s_release0_95;
	static const CGeLibVersion	s_releaseSed;
	static const CGeLibVersion	s_releaseTah;

/* original code:
private:
    unsigned char   mVersion[10];*/
};
