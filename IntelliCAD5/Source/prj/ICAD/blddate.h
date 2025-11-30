#define MLBLDDATE 13/10/04
#define MLBLDTIME 11:50:31 AM
#define MLBLDVERS "5.0.20.0"
#define BLDVERS "5.0"

// Added Cybage VSB 13/08/2001 [
// Reason: implement Bugzilla:77822
// Replace 'PLACE NAME OF ITC MEMBER HERE' with actual VENDORNAME. Currently the maximum size 
// for Vendorname is 59. This size can be changed after small modification in db1.cpp.
#define VENDORNAME "PLACE NAME OF ITC MEMBER HERE"
//this ends up in the windows explorer dll properties dialog
#define VERSIONINFO_FILEVERSION 5,0,20,0
#define VERSIONINFO_FILEVERSION_STR "5,0,20,0\0"

#define VERSIONINFO_BUILDTYPE "BETA\0"
#define VERSIONINFO_COPYRIGHT "Copyright © 1997-2002 ITC.\0"
#define VERSIONINFO_TRADEMARK "IntelliCAD is registered trademark of ITC.\0"
#define VERSIONINFO_COMPANY   "ITC\0"

#define VERSIONINFO_COMMENTS  "\0"
#define VERSIONINFO_PRIVATEBUILD "\0"
	//"\0" or BETA\0 or ALPHA\0. empty string is for RELEASE

// Added Ebatech Cmasami 13/01/2003
// add #ifndef/#endif macro to prevent duplicate macro warning 
#define  VERSIONINFO_MAINPRODUCT "ITC IntelliCAD® 5 Beta\0"


