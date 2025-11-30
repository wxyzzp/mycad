// File  : <DevDir>\source\prj\lib\db\AcisModeler.h
// Author: Dmitry Gavrilov

#pragma once

#ifndef _ACISMODELER_H_
#define _ACISMODELER_H_


class DB_CLASS CAcisModeler
{
public:
	~CAcisModeler()
	{
		if(!stop())
			ASSERT(0);
	}
	static bool		start();
	static bool		stop();
	static HMODULE	getAcisDllHandle();
};


#endif // _ACISMODELER_H_
