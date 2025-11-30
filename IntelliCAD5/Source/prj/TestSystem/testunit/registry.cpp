#include "stdafx.h"
#include "registry.h"
using namespace testunit;

registry& registry::get()
{
	static registry s_registry;
	return s_registry;
}
