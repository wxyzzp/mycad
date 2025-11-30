Usage of InteliCAD Test System (Its).

The Its is intended for programmers, performing unit testing.

Its consists of 3 following parts:
1- testunit library; it implements framework for developing and running tests,
2- GUI application, for running tests and reporting its results,
3- tests, written with usage of testunit.

1. Library testunit.

All classes and functions are defined in namespace testunit.
Let's describe classes, composing library.

Exceptions.

testunit::exception - excpected failure, thrown from function testunit::_assert 
if asserted condition is failed. With usage redefined macro assert, testunit::exception
recieves filename and line number, where assertion was failed.
So the most convinient way of using of testunit::exception is macro assert, though
direct throwing is not prohibited.

testunit implements translator of structured exceptions into C++ exceptions.
testunit::sexception is a result of translating structure exception into C++ exception.
To install translator of structured exceptions in some block you should create object 
of class testunit::sexception_translator_installer at the begining of block, or use
macros INSTALL_EXCTRANS.

Errors.

testunit::failure - describes expected error, failed assertion. It inherits 
testunit::exception and contains pointer to failed test.

testunit::crash - describes unexpected error, any exception thrown from user code, 
or translated structure exception. It contains pointer to crashed test.

Memory management.

All classes in testunit are inherited from class testunit::object. It redefines
operators new, delete, delete[]. It's possible that objects (tests, exceptions)
is created at one module(library) and will be destroyed at another module; also 
it's possible, the modules has different implementation of memory management; and
it can lead to crashes easely. Redefinition of memory allocation/deallocation operators
is need to be sure, that all dynamic memory allocation/deallocations for testunit:object
are use one implemenattion, from testunit library.

Tests.

testunit::test - provides common abstraction of a test, which have a name and 
can be runned. It's a base  abstract class for all types of tests.

testunit::testcase - single test, for which provided handlers for all exceptions and 
translator of structured exceptions. Inherits testunit::test and implements 
method run(), adding translation of structured exceptions, exception catching and reporting, 
also introduce addition pure virtual methods: 
pre_action - method called before test,
action - the test,
post_action - method called after test.

testunit::collection - collection of tests. Inherits testunit::test, 
implements method run(), to call run() of subtests. testunit::collection delete
all subtest when destoying.

testcase::func - wrapper for function of type void()(void). Makes more convinient 
writing of small tests as separate functions. Inherits testunit::testcase, implement 
method action() simply calling wrapped function.

testunit::tmethod - wrapper for method of type void(_Ty::*)(void), where _Ty is arbitrary class.
Makes convinient creation tests from methods of class. It's used in creation of collections
of tests from methods of one class. Inherits testunit::testcase, implement
method action() simply calling wrapped method.

Registry and Registration.

testunit::registry
To be achieved in run-time, all tests are stored into one registry. The registry 
implements pattern singleton and always exist in single exemplar. The instance of
registry can be achieved through static method testunit::registry::get().
Inherits testunit::collection.

For more visual representation of structure of all registered tests, it can be usefull
to group test from one module(library) into one common collection of tests. 
The file registry.h contains declaration and example implementation of function 
base_collection(), returning one testunit::collection, which can be used as root
for any test in a library. You can implement it in your library and use in all files.

testunit::tauto_registrar - provide registration of a test in registrty, just create
static object of the registrar and it will create and put the test in registry.
It can be used with macro REGISTER_TEST(className).

testunit::auto_func_registrar - similar tauto_registrar, but create only testunit::func tests.
It can be used with macro REGISTER_TEST(functionName).

Output stream.

To provide run-time environment with information about test running, testunit provides
class testunit::ostream. Class provides output for build-in types and for 
classes testunit::failure, testunit::crash, testunit::test. There is global instance
of testunit::ostream named testunit::tout. It's used in output of information about
all exceptions, and can be used from any user test. Output for testunit::test is called
for any test before its execution (what's used in implementation of progress bar).

Class testunit::ostream has several target for output: cout, debugger output window,
given file. Also testunit::ostream provides customization possibility through 
implementation of abstract class testunit::ostream::custom.

Examples Of Usage.

// Function performs testing of class integer
void testInteger()
{
	integer a = 1, b = 2;

	assert(a+b == 3);
	assert(a-b == -1);
	assert(a*b == 2);
	assert(a/b == 0);
}
//Register the test
static auto_func_register s_fr("Test of class integer", testInteger);
//or using macros, in this case function name will be used as test name
REGISTER_FUNC(testInteger);

//Class simple test of class point
class CTestPoint: public testunit::testcase
{
	//tests
	void testConstructor();
	void testAddition();
	void testSubtraction();
	void testMultiplication();

public:
	//constructor
	CTestPoint();

	//entry point
	virtual void action();
};

CTestPoint:CTestPoint()
:testcase("Test of class point")
{}

void CTestPoint::action()
{
	testConstructor();
	testAddition();
	testSubtraction();
	testMultiplication();
}

//register test CTestPoint
static tauto_registrar<CTestPoint> s_rt;
//or using macro
REGISTER_TEST2(CTestPoint);

In previouse example implement one test. It will exist in a registry as one test and
in tree view in GUI application as one test, though it has 4 subtests. If some subtest
will fails exception will be catched only in testcase::run, and remaining subtests 
will be omitted. The benefit of such way is all tests has access to common instance of
CTestPoint.
There is another aproach to implement such tests with usage of class testunit::tmethod.

class CTestPoint: public testunit::collection
{
	//tests
	void testConstructor();
	void testAddition();
	void testSubtraction();
	void testMultiplication();

public:
	//constructor
	CTestPoint();
};
//register test
static tautoregistrar<CTestPoint> MAKE_NAME("Test class point");

CTestPoint:CTestPoint()
:collection("Test of class point")
{
	typedef tmethod<CTestPoint> test_t;
	add(new test_t("Test construction", this, testConstructor));
	add(new test_t("Test addition", this, testAddition));
	add(new test_t("Test subtraction", this, testSubtraction));
	add(new test_t("Test multiplication", this, testMultiplication));
}
//or using macros
IMPLEMENT_CTOR_BEGIN(CTestPoint)
	ADD_TEST_METHOD(testConstruction)
	ADD_TEST_METHOD(testAddition)
	ADD_TEST_METHOD(testSubtraction)
	ADD_TEST_METHOD(testMultiplication)
IMPLEMENT_CTOR_END

In this case CTestPoint is collection of testunit::tmethod tests. Each call to method
is wrapped into try/catch block.

2. GUI Application.

It has quite simple interface. It's SDI programm, consisting of 2 view: 
tree view of all tests on left side and text view with results on right side.

Menu consists of 3 items: Tests, View, Help.
Submenu Tests include 2 commands: 
Run - runs currently selected test in tree view,
Exit - close the program.

Submenu View include following commands:
checked item Toolbar, Status bar allow to hide or to show these interface elements.
checked items Crashes, Failures, Messages allow to hide or to show corresponding
elements of resulting output.

Also application has toolbar with only button to run tests.
Status bar contains progress bar, showing test running progress.

3. Yours test.

Logically, that unit tests should be part of tested project, in order to have access
to not exported classes and functions. To separate library code from tests, them can be
placed in separate directory (for example ./Tests); also we can create special folder
Tests in File View of the Project in Developer Studio's Workspace. Think, it's useful
to create special project configuration (Win32 Debug Test) for test building, it allows
to exclude files with tests from ordinal building.

4. Notes.

Ordinal way to register the tests is to create static objects of registrars. If tests
are allocated in dll, these objects will be created during dll loading; all right with it.
To test static libraries, their code should be linked with whole application.
In order, to force linker to link object files from the library to application (or dll),
this application should use (or refer to) objects from the object file in static library.
But application doesn't directly uses objects from static library, so linker just
leave static library (with all tests) alone from test system.
To force linker to link tests from static library, I propose following hint.
In every cpp-file with tests in your static-library define some global variable:
bool g_useModuleTestDimensions;
then in file ItsLinkStaticLibraries.cpp assign value to your variable: 
extern bool g_useModuleTestDimensions;
g_useModuleTestDimensions = true;
Linker will be happy to find external symbol and to link module where symbol was defined.
If you know another, more convinient way to get round this issue, please let me know too.

Dmitry Gavrilov proposed more shorter way to force linkage:
#pragma comment (linker, "symbol name")
but the imported object should be created any way; 
plus don't forget, "symbol name" is not variable name, because C++ compliliers and linkers use
name decorating, though imported object can be declared as extern "C".

good lack,
Alexey Malov.
param@tesis.nnov.ru
