#pragma once

#ifdef TESTUNIT
#define testunit_api __declspec(dllexport)
#else
#define testunit_api __declspec(dllimport)
#endif
