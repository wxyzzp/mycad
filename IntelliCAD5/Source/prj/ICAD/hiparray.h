#pragma once

#include "db.h"

class CHipArray : public CArray<db_handitem*, db_handitem*>
{
public:
	void Clear();
};
