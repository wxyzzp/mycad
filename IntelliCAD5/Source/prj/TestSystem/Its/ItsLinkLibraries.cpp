
// this file forces linking of libraries with its.exe

// acis.dll
#pragma comment(linker, "/include:_createModeler")

// testCOM.dll
#pragma comment(linker, "/include:_exportSymbol")
