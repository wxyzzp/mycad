// TsIcadAcis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <list>
using std::list;
#include <iostream>
using std::cout;
#include "TsTest.h"
#include <fstream>
using std::ofstream;
#include "TsException.h"

#undef main
int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	ofstream log("test.log");
	log << "Start test-session\n";

	if (!loadModeler())
	{
		cout << "Can't load modeler";
		log << "Can't load modeler";
		return -1;
	}

	for (std::list<CTsTest*>::iterator i = tests().begin(); i != tests().end(); ++i)
	{
		try
		{
//			if ((*i)->flag())
				(**i)();
		}
		catch (const CTsExceptionAcisObject& e)
		{
			log << e.where() << ": " << e.what() << "\n";
			{
				icl::string name(e.where());
				char buff[33];
				name += itoa(reinterpret_cast<int>(*i), buff, 10);
				ofstream of(name.c_str());
				of << e.data();
			}
		}
		catch (const CTsException& e)
		{
				log << e.where() << ": " << e.what() << "\n";
		}
		catch (const exception& e)
		{
			log << e.what() << "\n";
		}
		catch (...)
		{
			log << "Undefined exception\n";
		}
	}

	log << "Finish test-session";

	{
		OFSTRUCT of;
		of.cBytes = sizeof(of);
		of.fFixedDisk = 1;
		of.nErrCode = -1;
		strcpy(of.szPathName, "./debug.log");
		HANDLE hReport = (HANDLE)OpenFile("debug.log", &of, OF_CREATE|OF_WRITE);

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, hReport);
		//_CrtDumpMemoryLeaks();

		//CloseHandle(hReport);
	}

	return 0;
}
