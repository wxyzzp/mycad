#pragma once
#include "../AsModeler.h"
#include "../ACIS.h"
#include "xstd.h"
#include "testunit/test.h"
#include <string>
#include <list>
#include <functional>

template <typename _Ty>
class TTsMethod: public testunit::tmethod<_Ty>
{
public:
	TTsMethod(const std::string& name, _Ty* _P, testunit::tmethod<_Ty>::tmethod_t _M)
		:testunit::tmethod<_Ty>(name, _P, _M){}

	void pre_action()
	{
		createModeler();
	}
};

const icl::string errorMessage();

inline
CAsModeler* modeler()
{
	return static_cast<CAsModeler*>(CModeler::get());
}

void freeDispObjects(sds_dobjll* p);

class CTsTest;
std::list<CTsTest*>& tests();

class CTsFinally
{
public:
	virtual ~CTsFinally(){}
};

class CTsFinallyClearModelerData: public CTsFinally
{
public:
	virtual ~CTsFinallyClearModelerData()
	{
		modeler()->clear();
	}
};

#define INSTALL_FINALLY_CLEANER CTsFinallyClearModelerData __st_cleaner;
#define crasher g_crasher
