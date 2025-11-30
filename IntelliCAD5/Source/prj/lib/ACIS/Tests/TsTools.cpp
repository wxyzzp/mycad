#include "stdafx.h"
#include "../AsModeler.h"
#include "../ACIS.h"
#include "TsTools.h"
#include "testunit/registry.h"

const icl::string errorMessage()
{
	icl::string em;
	int ec = modeler()->getLastErrorCode();
	if (0 != ec)
	{
		icl::string em;
		modeler()->getErrorMessage(ec, em);
		return em.c_str();
	}
	return em;
}

void freeDispObjects(sds_dobjll* p)
{
	while (p)
	{
		free(p->chain);
		sds_dobjll* t = p->next;
		free(p);
		p = t;
	}
}

testunit::collection& base_collection()
{
	using namespace testunit;
	class base_test_collection: public collection
	{
	public:
		base_test_collection()
			:collection("Collection of tests for Acis.dll")
		{
			testunit::registry::get().add(this);
		}
		virtual void run()
		{
			//initialize modeler
			createModeler();
			//run tests
			collection::run();
		}
	};
	static base_test_collection* s_base = new base_test_collection;
	return *s_base;
}

