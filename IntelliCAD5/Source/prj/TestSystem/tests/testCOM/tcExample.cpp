// File: <DevDir>/source/prj/TestSystem/tests/COM/tcExample.cpp
//

#include "tcTools.h"
#include "testunit/autoregistrar.h"


// example collection of tests (a sub-collection of the root one)
class CTsExampleTest: public collection
{
	void test1();
	void test2();

public:
	CTsExampleTest();
};

static tauto_registrar<CTsExampleTest> MAKE_NAME(base_collection());


CTsExampleTest::CTsExampleTest() : collection("Collection 1 of tests for IntelliCAD COM API")
{
	typedef tmethod<CTsExampleTest> test_t;
	add(new test_t("Test 1: first example", this, test1));
	add(new test_t("Test 2: undo", this, test2));
}


// test functions themselves
void
CTsExampleTest::test1()
{
	IIcadDocument*		pDoc;
	IIcadModelSpace*	pMSpace;
	IIcadUtility*		pUtility;
	long				entsCount1, entsCount2;
	IIcadCircle*		pCircle;
	IIcadPoint*			pPoint;
	IIcadLibrary*		pLibrary;
	BSTR				commandStr;

	_tellOnFail2(getIcadApp()->get_ActiveDocument(&pDoc), "IIcadApplication::get_ActiveDocument failed");
	_tellOnFail2(pDoc->get_ModelSpace(&pMSpace), "IIcadDocument::get_ModelSpace failed");
	_tellOnFail2(pDoc->get_Utility(&pUtility), "IIcadDocument::get_Utility failed");
	_tellOnFail2(getIcadApp()->get_Library(&pLibrary), "IIcadApplication::get_Library failed");

	_tellOnFail2(pMSpace->get_Count(&entsCount1), "IIcadModelSpace::get_Count failed");
	_tellOnFail2(pLibrary->CreatePoint(2.0, 2.0, 0.0, &pPoint), "IIcadLibrary::CreatePoint failed");
	_tellOnFail2(pMSpace->AddCircle(pPoint, 2.0, &pCircle), "IIcadModelSpace::AddCircle failed");
	_tellOnFail2(pMSpace->get_Count(&entsCount2), "IIcadModelSpace::get_Count failed");
	_tellIfNot2(entsCount2 == entsCount1 + 1, "adding of a circle doesn't increase entities count");

	commandStr = SysAllocString(L"(command \"zoom\" \"e\")");
	commandRunAndFinish(&commandStr, this, pDoc);
	SysFreeString(commandStr);
}

void
CTsExampleTest::test2()
{
	IIcadDocument*		pDoc;
	IIcadModelSpace*	pMSpace;
	long				entsCount1, entsCount2;
	BSTR				commandStr;

	_tellOnFail2(getIcadApp()->get_ActiveDocument(&pDoc), "IIcadApplication::get_ActiveDocument failed");
	_tellOnFail2(pDoc->get_ModelSpace(&pMSpace), "IIcadDocument::get_ModelSpace failed");

	_tellOnFail2(pMSpace->get_Count(&entsCount1), "IIcadModelSpace::get_Count failed");

	commandStr = SysAllocString(L"(command \"line\" '(0 0) '(3 3) \"\")");
	commandRunAndFinish(&commandStr, this, pDoc);

	_tellOnFail2(pMSpace->get_Count(&entsCount2), "IIcadModelSpace::get_Count failed");
	_tellIfNot2(entsCount2 == entsCount1 + 1, "adding of a line doesn't increase entities count");

	SysReAllocString(&commandStr, L"(command \"u\")");
	commandRunAndFinish(&commandStr, this, pDoc);

	entsCount1 = entsCount2;
	_tellOnFail2(pMSpace->get_Count(&entsCount2), "IIcadModelSpace::get_Count failed");
	_tellIfNot2(entsCount2 == entsCount1 - 1, "undo of adding of a line doesn't decrease entities count");

	SysFreeString(commandStr);
}
