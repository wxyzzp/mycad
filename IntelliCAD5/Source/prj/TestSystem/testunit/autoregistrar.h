#pragma once
#include "test.h"
#include "registry.h"
#include "object.h"

namespace testunit
{

//Class creates object of specified type 
//and registrates the object in registry of tests
template <typename _Ty>
class tauto_registrar: public object
{
public:
	tauto_registrar(collection& suite = registry::get())
	{
		suite.add(new _Ty);
	}
	tauto_registrar(const char* name, collection& suite = registry::get())
	{
		suite.add(new _Ty(name));
	}
};

//Class create testunit::func for specified function
//and registrates the object in registry of tests
class auto_func_registrar: public object
{
public:
	auto_func_registrar(const char* name, func::test_func_t _foo, collection& suite = registry::get())
	{
		suite.add(new func(name, _foo));
	}
};

}

#define CONCATANATE(s1, s2) s1##s2
#define CONCATANATE2(s1, s2) CONCATANATE(s1,s2)

//make static variable of testunit::tauto_registrar<_te>, 
//which will create test of type _te, with name #_te and add it to root colelction
#define REGISTER_TEST(_te) static testunit::tauto_registrar<_te> CONCATANATE2(s_test,__LINE__)(#_te)

//make static variable of testunit::tauto_registrar<_te>, 
//which will create test of type _te, class _te should have default constructor,
//test will be added to the root colelction
#define REGISTER_TEST2(_te) static testunit::tauto_registrar<_te> CONCATANATE2(s_test,__LINE__)

#define REGISTER_FUNC(_fu) static testunit::auto_func_registrar CONCATANATE2(s_func,__LINE__)(#_fu, _fu)
#define MAKE_NAME CONCATANATE2(_variable,__LINE__)
