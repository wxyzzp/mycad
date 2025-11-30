#include "stdafx.h"
#include "../registry.h"
#include "../assert.h"
#include "../autoregistrar.h"
using namespace testunit;
#include <vector>
#include <algorithm>
using namespace std;


void foo1()
{}
void foo2()
{}

void _registry()
{
	registry& base1 = registry::get();
	assert(&base1);
	registry& base2 = registry::get();
	assert(&base2);

	assert(&base1 == &base2);
}

static
auto_func_registrar MAKE_NAME("test for class testunit::registry",
							_registry, base_collection());
