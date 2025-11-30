/* PRJ\ICAD\CONFIGURE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * This is a bottleneck file for configuration options (such as VBA) that require
 * installation of an optional SDK on the building machine.
 *
 */

/********************** HTML HELP **********************/
//	Use of HTML help requires building with HTMLhelp.h (included with the Microsoft VBA6 SDK
//	if you don't implement VBA6 you can install HTML Help Workshop.)
//	and installing Microsoft runtime support for HTML help.

//	By commenting out the following #define these dependencies are eliminated.
#define BUILD_WITH_HTMLHELP

// This define determines if we want to build with licensed components.
//#define BUILD_WITH_LICENSED_COMPONENTS

#ifdef BUILD_WITH_LICENSED_COMPONENTS
	/********************** VBA **********************/
	//	Support for VBA requires building in the presence of Microsoft VBA 6 SDK.

	//	By commenting out the following #define this dependency is eliminated.
	#define BUILD_WITH_VBA

	/********************** RENDERING **********************/
	//	Use of Lightworks rendering DLLs requires a license from them, and the presence
	//	of the dlls.  By commenting out the following #define, Intellicad will disable
	//	all access to the rendering functionality
	#define BUILD_WITH_RENDERING_SUPPORT

	/********************** RASTER **********************/
	//	Use of Hitachi raster DLLs to display images requires a license from them, and the
	//	presence of the dlls.  By commenting out the following #define, Intellicad will disable
	//	the loading of the Hitachi DLLs.
	#define BUILD_WITH_RASTER_SUPPORT

	/********************** SPELL CHECKING **********************/
	//	The Spell checker in Intellicad first searches for the Microsoft Word Spell checker and uses
	//	that if it is available.  If not, it searches for an INSO Spell checker.  If that is not
	//	available, spell checking fails.  Using these external spell checkers has licensing implications.

	//	By commenting out the following #define, the Word spell checker will not be loaded
	//#define BUILD_WITH_WORD_SPELLCHECK

	//	By commenting out the following #define, the Inso spell checker will not be loaded
	#define BUILD_WITH_INSO_SPELLCHECK

	//	By commenting out both of the above #defines, spell checking will be completely disabled.

	/********************* ACIS *****************************/
	// Enabling ACIS support. Use of Acis requires a presence of acis.dll

	// By commenting out the following #define, all functionality based on Acis will be eliminated.
	#define BUILD_WITH_ACIS_SUPPORT

	// Uncommenting following #define limits usage of Acis to viewing of Acis models
	// #define BUILD_WITH_LIMITED_ACIS_SUPPORT


#endif // BUILD_WITH_LICENSED_COMPONENTS

/********************** XRM **********************/
// Enabling XRM command. Use XRM of requires the presence of xrm.dll
//#define BUILD_WITH_XRM_SUPPORT

/********************** QUICK RENDERING **********************/
//	Use of NSTLab quick rendering DLLs requires the presence of the DLLs.
//	By commenting out the following #define, Intellicad will disable
//	all access to the quick rendering functionality
//#define BUILD_WITH_QRENDERING_SUPPORT

/********************** FLYOVER SNAPPING **********************/
//	Enabling flyover snapping support in IntelliCAD.
//	By commenting out the following #define, Intellicad will disable
//	all flyover snapping functionality, leaving a stub instead
#define BUILD_WITH_FLYOVERSNAP_SUPPORT

/********************** IMAGEMENU **********************/
// Enabling imagemenu command. Use IMAGEMENU of requires the presence of
// imagemenu.dll
#define BUILD_WITH_IMAGEMENU_SUPPORT

/********************** TESTDRIVE  **********************/
//	If the authentication mechanism supports a demo or
//	testdrive mode, the following #define will cripple the
//	software and not allow saving or printing drawings if
//	the software is in testdrive
#define CRIPPLE_TESTDRIVE

