// File: <DevDir>/source/prj/lib/cmds/cmdsError.cpp
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.

#include "cmds.h"
#include "IcadApi.h"

/*********************************************************************
**	Function:	cmd_ErrorPrompt()							4/15/96
**
**	Purpose:	Keeps all the error message strings in one place.
**
**	Notes:	- When you make up new ones, please try to be consistent
**			in making up the promptID names, and kind to the users.
**			It's easy to confuse these in the promptID names...look out:
**				2DPT	2-Dimensional point, as in Pick a 2D point
**				2NDPT	Second point, as in Pick 2nd point
**				2PTS	2 Points, as in Pick two points
**			When you make up new ones, please follow the above convention.
**
**			- When the command line is not displayed, the error messages
**			appear in a message box so the user doesn't miss them.
**			- Be sure to add new ones to the commented table below.
**
**  Example:
**          cmd_ErrorPrompt(CMD_ERR_YESNO, 0);
*********************************************************************/
/*DG - 30.5.2002*/// I refactored the function: now there are few lines instead of the huge switch;
// the price of this code reducing and performance increasing is calling ResourceString with NULL 2nd parameter.
// I placed promptID and according strings in the table below (see comments after the function).
// Also, I replaced IDC_ERRORPROMPT_ IDs in resources (.h and .rc2 files) with according CMD_ERR_ IDs.
void
cmd_ErrorPrompt
(
 int	promptID,
 short	mode,			// 1 - for message box by default, 0 - for command line (or msg box if no command line), -1 - for command line only
 void*	arg1 /*= NULL*/,
 void*	arg2 /*= NULL*/,
 void*	arg3 /*= NULL*/
)
{
	CString	tmpStr;

	if(promptID == CMD_ERR_NAMETOOLONG || promptID == CMD_ERR_BLKNAMETOOLONG)
		tmpStr.FormatMessage(ResourceString(promptID, NULL), IC_ACADNMLEN - 1);
	else
		tmpStr = ResourceString(promptID, NULL);

	if(tmpStr.IsEmpty())
		tmpStr = ResourceString(CMD_ERR_UNABLE, NULL);

	char	prompt[IC_ACADBUF];
	_snprintf(prompt, sizeof(prompt), tmpStr, arg1, arg2, arg3);

	// Clear the command buffers to stop script files. Exepting we have a notifying message.
	/*DG - 31.5.2002*/// Added messages: CMD_ERR_INSERTACIS, CMD_ERR_INSERTPROXY, CMD_ERR_PASTEACIS,
	// CMD_ERR_PASTEPROXY, CMD_ERR_WBLOCKACIS, CMD_ERR_WBLOCKPROXY, CMD_ERR_INSERTDEF, CMD_ERR_ENTSINSET,
	// CMD_ERR_FLATTENED, CMD_ERR_NOTPARALLEL.
	if( promptID != CMD_ERR_DWGHASACIS &&
		promptID != CMD_ERR_DWGHASPROXY &&
		promptID != CMD_ERR_UNSUPPORTEDA2KENT &&
		promptID != CMD_ERR_GRIDTOOSMALL &&
		promptID != CMD_ERR_GRIDTOOLARGE &&
		promptID != CMD_ERR_ZOOMIN &&
		promptID != CMD_ERR_ZOOMOUT &&
		promptID != CMD_ERR_INSERTACIS &&
		promptID != CMD_ERR_INSERTPROXY &&
		promptID != CMD_ERR_PASTEACIS &&
		promptID != CMD_ERR_PASTEPROXY &&
		promptID != CMD_ERR_WBLOCKACIS &&
		promptID != CMD_ERR_WBLOCKPROXY &&
		promptID != CMD_ERR_XREFADDED &&  
		promptID != CMD_ERR_ENTSINSET &&
		promptID != CMD_ERR_FLATTENED &&
		promptID != CMD_ERR_NOTPARALLEL )
	{
		SDS_FreeEventList(0);
	}

	resbuf	wndlcmdRb;
	SDS_getvar(NULL, DB_QWNDLCMD, &wndlcmdRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	// Windows plays its own sound when the dialog pops up (?), so I moved the sound to command line error messages only.
	if((mode || !wndlcmdRb.resval.rint) && mode != -1)
	{
		if(cmd_InsideOpenCommand || cmd_InsideNewCommand || SDS_WasFromPipeLine)
			goto nomsgbox;
		sds_alert(prompt);
	}
	else
	{
		if(mode != -1)
			cmd_PlaySound(1);
		nomsgbox : ;
		if( promptID == CMD_ERR_DWGHASACIS ||
			promptID == CMD_ERR_DWGHASPROXY ||
			promptID == CMD_ERR_UNSUPPORTEDA2KENT ||
			promptID == CMD_ERR_GRIDTOOSMALL ||
			promptID == CMD_ERR_GRIDTOOLARGE )
		{
			sds_printf("\n%s\n"/*DNT*/, prompt);
		}
		else
			sds_printf("\n%s"/*DNT*/, prompt);
	}

	SDS_SetLastPrompt(tmpStr);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// THE STRINGS TABLE mentioned above, with some comments and grouping
//////////////////////////////////////////////////////////////////////////////////////////////////

//  Really generic ones, or things which aren't supported in current release...
// CMD_ERR_YESNO:			"Yes or No, please."
// CMD_ERR_TRYAGAIN:		"Please try again:"
//  Use this especially where the command prompt is repeated after it.
//  Avoid this where possible, as it doesn't provide any information to the user.
//  I have tried to use it only where the prompt use to say "*Invalid*".
// CMD_ERR_NODWGWINDOW:		"This command is available only when a drawing is open."
// CMD_ERR_NOEFFECT:		"This command had no effect."
// CMD_ERR_UNSUPPORTED:		"This command is not yet supported."
// CMD_ERR_OPTIONNOTSUP:	"This option is not yet supported."
// CMD_ERR_NOTRANSPARENT:	"This command cannot be used transparently."
// CMD_ERR_DONTUSEEND:		"To close IntelliCAD, please select Exit from the File menu."
// CMD_ERR_SHADEPERSP:		"Shading in Perspective mode is not yet supported."
// CMD_ERR_HIDEPERSP:		"Hiding in Perspective mode is not yet supported."
// CMD_ERR_UNUSEDVARS:		"This variable is not yet supported. Value entered will be stored but will have no effect."

//  Unsupported entities...
// CMD_ERR_DWGHASACIS:		"This drawing contains one or more ACIS entities that will not display.\nThese entities WILL be stored and saved back to the drawing."
// CMD_ERR_UNSUPPORTEDA2KENT:	"This drawing contains one or more A2K-specific entities that will not display.\nThese entities WILL be stored and saved back to the drawing."
// CMD_ERR_DWGHASPROXY:		"This drawing contains one or more proxy entities that will not display.\nThese entities WILL be stored and saved back to the drawing."
// CMD_ERR_INSERTACIS:		"This drawing contains one or more ACIS entities.\nThese entities WILL NOT insert and will be ignored."
// CMD_ERR_INSERTPROXY:		"This drawing contains one or more proxy entities.\nThese entities WILL NOT insert and will be ignored."
// CMD_ERR_PASTEACIS:		"This paste from clipboard contains one or more ACIS entities.\nThese entities WILL NOT insert and will be ignored."
// CMD_ERR_PASTEPROXY:		"This paste from clipboard contains one or more proxy entities.\nThese entities WILL NOT insert and will be ignored."
// CMD_ERR_WBLOCKACIS:		"WBLOCKing one or more ACIS entities is not yet supported.\nThese entities WILL NOT WBLOCKed and will be ignored."
// CMD_ERR_WBLOCKPROXY:		"WBLOCKing one or more proxy entities is not yet supported.\nThese entities WILL NOT WBLOCKed and will be ignored."

//  There's nothing there...
// CMD_ERR_EMPTYDWG:		"That command is unavailable in an empty drawing."
// CMD_ERR_NOLASTAXIS:		"No previous axis to use."
// CMD_ERR_NOENT:			"No entity was found there. Please pick again."
// CMD_ERR_NOSELECT:		"No entities selected."
// CMD_ERR_NOCONTINUE:		"There is nothing from which to continue."
// CMD_ERR_NOUNERASE:		"There is nothing to un-erase."
// CMD_ERR_NOUNDO:			"There is nothing to undo."
// CMD_ERR_NOREDO:			"There is nothing to redo."
// CMD_ERR_NOMOREUNDO:		"There is nothing more to undo."
// CMD_ERR_NOPREVZOOM:		"There is no previous zoom."
// CMD_ERR_UNDODISABLE:		"Undo/Redo is disabled, use the UNDO command to enable."
// CMD_ERR_DIMBLKDOESNOTEXIST:	"\nDIMBLK does not exist -- cannot save."
// CMD_ERR_DIMBLK1DOESNOTEXIST:	"\nDIMBLK1 does not exist -- cannot save."
// CMD_ERR_DIMBLK2DOESNOTEXIST:	"\nDIMBLK2 does not exist -- cannot save."
//  EBATECH(watanabe)-[dimldrblk
// CMD_ERR_DIMLDRBLKDOESNOTEXIST:	"\nDIMLDRBLK does not exist -- cannot save."
//  ]-EBATECH(watanabe)

//  User picked the wrong kind of entity...
// CMD_ERR_ENTTYPE:			"Unable to use the selected entity. Please pick again."
//  Mostly used in place of "*Invalid object*"
// CMD_ERR_SELECTION:		"Unable to use that selection.  Please try again:"
// CMD_ERR_CANTDEFUCS:		"Cannot define a coordinate system based upon the selected entity."
// CMD_ERR_PTOUTSIDELIMITS:	"Selected point is outside limits. Please select another point."
// CMD_ERR_NOTINUCS:		"An entity which is defined in the current UCS is needed."
// CMD_ERR_LINEAR:			"A line, arc, or polyline is needed. Please pick again."
// CMD_ERR_LINEARCCIRPLY:	"A line, arc, circle, or polyline is needed. Please pick again."
// CMD_ERR_LINE:			"A line is needed here. Please pick again."
// CMD_ERR_POLYLINE:		"A polyline is needed here. Please pick again."
// CMD_ERR_2DPOLYLINE:		"A 2D polyline is needed. Please pick again."
// CMD_ERR_ARC:				"An arc is needed here. Please pick again."
//  For changing an arc to a circle.
// CMD_ERR_CIRCLE:			"A circle is needed for this. Please pick again."
// CMD_ERR_ARCCIRCLE:		"An arc or circle is needed here. Please pick again."
// CMD_ERR_LINEARCCIRCLE:	"A line, arc, or circle is needed. Please pick again."
// CMD_ERR_DIMENSION:		"A dimension is needed. Please select again."
// CMD_ERR_LINORDANG:		"A Linear, Ordinate, or Angular associative dimension is needed."
// CMD_ERR_COPLANAR:		"Coplanar entities are needed for this operation."
// CMD_ERR_ATTRIBUTE:		"A block with at least one attribute is needed."
// CMD_ERR_FILLETPLINE:		"Unable to fillet polylines with rays,xlines and circles."
// CMD_ERR_FILLETCAP:		"Cap filleting requires that the first entity be a line or ray."
// CMD_ERR_XPLODXREF:		"Externally referenced blocks cannot be exploded"
// CMD_ERR_HATCHBDRY:		"Unable to use the selected entities as a hatch boundary"
// CMD_ERR_TRIMENT:			"Cannot trim the selected entity"
// CMD_ERR_NOFROZEN:		"Current layer cannot be a frozen layer"

//  User entered a number not within the right range...
// CMD_ERR_ANGLE:			"Please select two points or enter a valid numeric angle."
// CMD_ERR_ANGLEPI:			"An angle between 0 and 180 degrees is required."
//  This is the range for color numbers, without BYLAYER or BYBLOCK.
// CMD_ERR_COLOR1255:		"Available colors are 1-255, or you can use a color name (red, blue, etc.)."
//  This is the range for color numbers, including zero, which is BYBLOCK.
// CMD_ERR_COLOR0255:		"Available colors are 0-255, or you can use a color name (red, blue, etc.)."
//  This is the range for color numbers, including 256, which is BYLAYER.
// CMD_ERR_COLOR1256:		"Available colors are 1-256, or you can use a color name (red, blue, etc.)."
//  This is the range for color numbers, including BYBLOCK(0) and BYLAYER(256).
// CMD_ERR_COLOR0256:		"Available colors are 0-256, or you can enter a color name (red, blue, etc.)."
// CMD_ERR_INT0TO16:		"A number between 0 and 16 is needed."
// CMD_ERR_ZOOMPERCENT:		"A number between 1 and 20000 is needed."
// CMD_ERR_POLYSIDES:		"A polygon can have between 3 and 1024 sides."
// CMD_ERR_DIVSEGS:			"You can divide using 2 to 32767 segments:"
// CMD_ERR_SEGSKWORD:		"You can divide using 2 to 32767 segments, or choose an option:"
// CMD_ERR_SURFTABS:		"The valid range is 2 to 32767 divisions:"
// CMD_ERR_INTRANGE:		"An integer (whole number) between -32768 and 32767 is needed:"
// CMD_ERR_DASHDOT:			"A linetype can have between 2 and 12 dash/dot specs:"
// CMD_ERR_SNAPTARGET:		"The snap target height can be any number between 1 and 50:"
// CMD_ERR_MESHSIZE:		"The mesh size can be any integer between 2 and 256."
//  This is the range for N and M mesh sizes.
// CMD_ERR_MESH3D:			"More sections are required to define this 3D entity."
// CMD_ERR_SPHEREDIVS:		"At least two divisions are needed to create this entity."
//  This is the range for the 5 kinds of LUNITS and AUNITS.
// CMD_ERR_UNITTYPE:		"The available unit types are 1 through 5."
//  This is the range for the number of decimal places to display LUNITS & AUNITS.
// CMD_ERR_DECIMALS:		"You can choose between 0 and 8 for number of decimal places:"
//  These are the denominators available to display LUNITS & AUNITS.
// CMD_ERR_DENOMINATOR:		"Please enter 1, 2, 4, 8, 16, 32, 64, 128, or 256 for the greatest denominator:"
// CMD_ERR_ARCSIZE:			"An arc can be any size between 0 and 360 degrees."
//  This is the range for bulge angles
// CMD_ERR_BULGESIZE:		"An angle between -360 and +360 degrees is needed."
//  This is a generic one when nothing else works, like when the range changes.
// CMD_ERR_NUMRANGE:		"The value must be within the specified range."
// CMD_ERR_GRIDTOOSMALL:	"The grid spacing is too small to display in this view."
// CMD_ERR_GRIDTOOLARGE:	"The grid spacing is too large to display in this view."
// CMD_ERR_SYSVARVAL:		"The specified value is not within the system variable range"

//  User entered the wrong kind of number...
// CMD_ERR_NUMERIC:			"A numeric value is needed. Please try again:"
// CMD_ERR_NONZERO:			"A value other than zero is needed. Please try again:"
// CMD_ERR_POSITIVE:		"A positive value is needed. Please try again:"
// CMD_ERR_POSNOZERO:		"A positive, non-zero value is needed. Please try again:"
// CMD_ERR_POSORZERO:		"A positive value, or zero, is needed. Please try again:"

//  User entered the wrong kind of input...
// CMD_ERR_BADARGUMENT:		"Bad argument type."
// CMD_ERR_NOTHINGTODO:		"Nothing to do. Single element array."
// CMD_ERR_DUPENTITY:		"This entity is already selected:"
//  Use especially when the prompt list is displayed again after this message.
// CMD_ERR_KWORD:			"Please choose an option from the list:"
//  Use instead of "Invalid point".
// CMD_ERR_PT:				"A point is needed. Please try again:"
// CMD_ERR_PTKWORD:			"Please pick a point, or choose from the list:"
// CMD_ERR_2DPT:			"A 2D (not 3D) point is needed. Please try again:"
//  Use instead of "Invalid 2D point".
// CMD_ERR_2DPTKWORD:		"Please pick a 2D point, or choose from the list:"
// CMD_ERR_NUMTWO2DPT:		"Please enter a number, or pick two 2D corner points."
// CMD_ERR_ANG2NDPT:		"Enter a numeric angle, or pick a 2nd point:"
// CMD_ERR_ANG2NDPTKWORD:	"Enter a numeric angle, 2nd point, or choose from list:"
// CMD_ERR_ANG2PTS:			"Please enter a numeric angle, or pick two points."
// CMD_ERR_DIST:			"A distance is needed.  Please try again:"
// CMD_ERR_DIST2PTS:		"Please enter a distance, or pick two points."
// CMD_ERR_DIST2PTSKWORD:	"Enter a distance, pick two points, or choose an option."
// CMD_ERR_DIST2NDPT:		"A distance or 2nd point is needed. Please try again:"
// CMD_ERR_DIST2NDPTKWORD:	"Enter a distance, pick a point, or choose an option:"
// CMD_ERR_RADDCIRC:		"Enter a radius, pick a point, or choose the Diameter option:"

//  User entered a name that either isn't the right kind of name, or could not be found...
//  Use the ones that say "FINDXXX" when you cannot locate the file.
//  Use the ones that say "XXXNAME" when the name entered cannot be a valid filename.
// CMD_ERR_FINDHELP:		"Unable to find the requested help file.\n%s\nCheck installation."
// CMD_ERR_FINDSTYLE:		"Unable to find the requested text style."
// CMD_ERR_FINDTEXT:		"Unable to find the requested text style. Please try again.\nYou can use the STYLE command to create a new text style."
// CMD_ERR_NOVPCONFIG:		"Unable to find that viewport configuration."
// CMD_ERR_FINDFILE:		"Unable to find the requested file."
// CMD_ERR_FINDXREF:		"Unable to locate external reference."
// CMD_ERR_BADRENDER:		"Unable to find rendering files (%s), Check installation."
// CMD_ERR_SAVEFILE:		"Unable to Save to the requested file name. The file is open or read-only."
// CMD_ERR_EXPORTFILE:		"Unable to Export a file to itself."
// CMD_ERR_CANTFINDFILE:	"Unable to find the requested file: \"%s\"."
// CMD_ERR_CANTOPENFILE:	"Unable to open the requested file: \"%s\"."

//  FINDFILE2 used in STYLE, where user can specify 2 files simultaneously
// CMD_ERR_FINDFILE2:		"Unable to find the second requested file."
// CMD_ERR_ANYNAME:			"Unable to recognize entry as a valid name. Please try again."
// CMD_ERR_BLOCKNAME:		"Unable to recognize entry as a block name. Please try again."
// CMD_ERR_LAYERNAME:		"Unable to recognize entry as a layer name. Please try again."
// CMD_ERR_FINDBLOCK:		"Unable to find a block with that name. Please try again."
// CMD_ERR_LTYPENAME:		"Unable to recognize entry as a linetype name. Please try again"
//  TODO - this is for linetype patterns... We need a better way to say this.
// CMD_ERR_LTYPEPAT:		"Invalid number or bad continuation."
// CMD_ERR_FINDLTYPE:		"Unable to find a linetype with that name.  --\n-- You can use the LINETYPE command to load it."
// CMD_ERR_FINDLAYER:		"Unable to find a layer with that name. Please try again."
// CMD_ERR_FINDVIEW:		"Unable to find a view with that name. Please try again."
// CMD_ERR_FINDUCS:			"Unable to find a UCS with that name. Please try again."
// CMD_ERR_NOUCSDEFS:		"No previous UCS definitions found."
// CMD_ERR_DELETEUCS:		"Unable to delete UCS."
// CMD_ERR_SAVEUCS:			"Unable to save UCS."
// CMD_ERR_FINDSNAP:		"Unable to find a snapshot with that name."
// CMD_ERR_NOPLANES:		"No planes have been defined in this drawing"
// CMD_ERR_NOPATTERNS:		"No patterns have been defined in this drawing"
// CMD_ERR_FINDPATTERN:		"Unable to find a hatch pattern with that name"
// CMD_ERR_GETDWGNAME:		"Unable to get the name of the current drawing."
// CMD_ERR_OPENDWG:			"An error occurred while attempting to open the requested drawing."
// CMD_ERR_OPENXREF:		"An error occurred while attempting to open a nested external reference."
// CMD_ERR_LOADXREF:		"An error occurred while attempting to load a nested external reference."
// CMD_ERR_UNRECOGSNAP:		"Unable to recognize entity snap mode/s."
// CMD_ERR_UNRECOGVAR:		"Unrecognized variable. Type \"SETVAR,?\" for a list of variables."
// CMD_ERR_UNRECOGCMD:		"Unable to recognize command.  Please try again."
// CMD_ERR_UNRECOGMENU:		"Unable to recognize command in menu."
// CMD_ERR_UNRECOGENTRY:	"Unable to recognize entry.  Please try again."
// CMD_ERR_LOADDIALOG:		"An error occurred while attempting to load the dialog box."
// CMD_ERR_FINDDIMSTYLE:	"Unable to find a dimension style with that name."
// CMD_ERR_NODIMSTYLES:		"No dimension styles have been defined in this drawing."
// CMD_ERR_DISPDIMVARS:		"Unable to display the variables or dimension style \"*UNNAMED\"."
// CMD_ERR_FINDCONVFACT:	"Unable to find proper conversion factor in \"intellicad.unt\"."
// CMD_ERR_NOXREFFOUND:		"Unable to find an XRef by that name."
// CMD_ERR_NOATTRIBS:		"No Attributes were selected."
// CMD_ERR_NOTBFOUND:		"Unable to find a toolbar by that name."
// CMD_ERR_DXFNEWONLY:		"DXFIN is only possible in a NEW empty drawing."

//  User tried to do something that can't be done...
// CMD_ERR_NOCHANGEABLE:	"Entities selected were not changeable."
// CMD_ERR_BADGEOMETRY:		"Unable to create an entity with that geometry."
// CMD_ERR_CANTRENAME:		"That name is reserved by IntelliCAD, and cannot be renamed."
// CMD_ERR_RESERVEDBLOCK:	"That block is reserved by IntelliCAD, and cannot be renamed."
// CMD_ERR_NULLTAG:			"The attribute needs a name (alphanumeric, no spaces)."
// CMD_ERR_ELEVDIFFZ:		"Cannot change elevation of entities with differing Z coordinates."
// CMD_ERR_OBLIQUELG:		"A smaller obliquing angle is needed.  Enter new angle:"
// CMD_ERR_CANTCLOSE:		"Cannot close until two or more segments are drawn."
// CMD_ERR_NOCIRCLE:		"No circle is possible."
// CMD_ERR_CHAMNONPOLY:		"Unable to chamfer a non-polyline entity with itself."
// CMD_ERR_CHAM3DPOLY:		"Unable to chamfer 3D polyline entities."
// CMD_ERR_CHAMDIFFPOLY:	"Unable to chamfer segments from different polylines."
// CMD_ERR_CHAMNONLINEAR:	"Unable to chamfer with a non-linear entity."
// CMD_ERR_CHAMNOLENGTH:	"Unable to chamfer lines of zero length."
// CMD_ERR_CHAMPARALLEL:	"Unable to chamfer lines that are parallel."
// CMD_ERR_CHAMTOOCLOSE:	"Chamfer point too close to start/end of segment."
// CMD_ERR_CHAM2LINEAR:		"Two linear entities are needed for chamfering"
// CMD_ERR_CHAMDISTLG1:		"First distance is too large to perform requested chamfer."
// CMD_ERR_CHAMDISTLG2:		"Second distance is too large to perform requested chamfer."
// CMD_ERR_CHAMDISTLG3:		"Distance is too large to perform requested chamfer on line."
// CMD_ERR_CHAMFERPLADJ:	"Unable to determine which vertex of polyline to chamfer."
// CMD_ERR_CHAMANGLE:		"Chamfer angle is too large to perform requested chamfer."
// CMD_ERR_CHAMPOLYARC:		"Unable to find intersection between line and straight segment of polyline."
// CMD_ERR_FILPOLYSEGS:		"Distinct polyline segments are needed."
// CMD_ERR_FILDIFFPOLY:		"Unable to fillet segments from different polylines."
// CMD_ERR_FILNOTPOSS:		"Unable to fillet the selected entities."
// CMD_ERR_FILCLOSEFAR:		"Fillet point too close to segment start/end, or selected point too far from polyline."
// CMD_ERR_FILTOOCLOSE:		"Fillet point too close to start/end of segment."
// CMD_ERR_FILNOLENGTH:		"Unable to fillet lines of zero length."
// CMD_ERR_FILRADIUSLG:		"Radius is too large to perform requested fillet."
// CMD_ERR_NOINTERSECT:		"No intersection exists between the selected entities."
// CMD_ERR_BREAKENT:		"The selected entity cannot be broken."
// CMD_ERR_BREAKBLOCK:		"Blocks cannot be broken."
// CMD_ERR_BREAKDISTINCT:	"Distinct points are needed to break this entity."
// CMD_ERR_MODENT:			"Unable to modify the selected entity."
// CMD_ERR_EXTENDENT:		"Unable to extend the selected entity."
// CMD_ERR_LENGTHENENT:		"Unable to lengthen the selected entity."
// CMD_ERR_LENCLOSEPOLY:	"A closed polyline cannot be lengthened."
// CMD_ERR_LENBYANG:		"This entity cannot be lengthened using angle."
// CMD_ERR_LENBYDIST:		"This entity cannot be lengthened by the specified distance."
// CMD_ERR_LENBYDYNPT:		"This entity cannot be lengthened using a dynamic point."
// CMD_ERR_CLOSEARC:		"An arc cannot be closed."
// CMD_ERR_NOTATABLET:		"The current pointing device is not a tablet."
// CMD_ERR_HATCHEXPPREV:	"Hatch cannot be exploded in preview mode."
// CMD_ERR_MIXCLOSEOPEN:	"Cannot mix closed and open paths."
// CMD_ERR_2PTSURFACE:		"Two points cannot define a surface."
// CMD_ERR_DIFFPTS:			"The points for this operation need to be distinct."
// CMD_ERR_ONSAMELINE:		"The three points cannot all be along one line."
//  for circles
// CMD_ERR_OFFSMALLRAD:		"The radius is too small to offset to the inside using that distance."
//  for ellipses & splines
// CMD_ERR_OFFSHARPCURVE:	"The entity curves too sharply to offset to this side using that distance."
// CMD_ERR_OFFZEROLEN:		"An entity of zero length cannot be offset."
// CMD_ERR_PEDITPFACE:		"A polyface mesh cannot be edited useing PEDIT."
// CMD_ERR_CLOSEDPLINE:		"The selected polyline is already closed."
// CMD_ERR_CLOSEDMPLINE:	"The selected polyline is already closed in the M direction."
// CMD_ERR_CLOSEDNPLINE:	"The selected polyline is already closed in the N direction."
// CMD_ERR_NOTCLOSEDPLINE:	"The selected polyline is not closed."
// CMD_ERR_NOTCLOSEDNPLINE:	"The selected polyline is not closed in the N direction."
// CMD_ERR_JOINCLOSEDPLINE:	"Unable to join to a closed polyline."
// CMD_ERR_BREAKDELETE:		"Breaking this entity would have deleted it.  Cancelled."
// CMD_ERR_COINCVERPTS:		"Unable to use coincident vertex points for this operation."
// CMD_ERR_NONCOINCENDPTS:	"Lines with coincident endpoints are needed."
// CMD_ERR_PARALLEL:		"Unable to use parallel lines here.  Please pick again."
// CMD_ERR_FITCURVE:		"Unable to fit a curve to the selected polyline."
// CMD_ERR_NOQUICKSNAP:		"Quick is available only when other snap modes are active."
// CMD_ERR_NOVIEWPORTS:		"There are no VIEWPORTS, use the MVIEW command to create some."
// CMD_ERR_NOACTIVEVP:		"All VIEWPORTS are turned OFF, use the MVIEW command to turn ON."
// CMD_ERR_TILEMODE0:		"TILEMODE must be set to Off (0) before using this command."
// CMD_ERR_TILEMODE1:		"TILEMODE must be set to On (1) before using this command."
// CMD_ERR_BDRYSET:			"The selected entities do not define a boundary."
// CMD_ERR_OSNAPENT:		"Unable to locate entity snap point."
// CMD_ERR_XREFEXIST:		"A non-referenced block with that XRef name exists in the drawing."
// CMD_ERR_INSERTXREF:		"Inserting an Externally Referenced drawing (Xref) as an Xref."
// CMD_ERR_BLKREF:			"A block cannot be self-referencing."
// CMD_ERR_XREFREF:			"The current drawing cannot have an External Reference to itself."
// CMD_ERR_DUPFILE:			"The current drawing cannot be inserted into itself."
// CMD_ERR_ZOOMIN:			"You have reached the limits of zooming in.  Unable to zoom in any closer."
// CMD_ERR_ZOOMOUT:			"You have reached the limits of zooming out.  Unable to zoom out any further."
// CMD_ERR_PLINESEGS:		"You have reached the limit of the number of segments per polyline."
// CMD_ERR_XRCURLAY:		"Cannot set an Externally Referenced layer as current layer."
// CMD_ERR_PERSPECTIVE:		"That command is not available in Perspective mode.\nUse DVIEW or VPOINT to turn perspective mode off."
// CMD_ERR_MAXACTVP:		"Maximum number of active view ports reached (check setvar MAXACTVP)."

//  There is a problem somewhere with the drawing or program...
// CMD_ERR_BADDATA:			"Bad data format on entity."
// CMD_ERR_CANTGETDATA:		"Unable to get Entity Data for that entity."
// CMD_ERR_TOOMUCHDATA:		"Too much Entity Data--Ignored."
// CMD_ERR_REMOVEEED:		"Unable to remove old Entity Data."
// CMD_ERR_UPDATEENT:		"Unable to update entity."
// CMD_ERR_CREATEENT:		"Unable to create the requested entity."
// CMD_ERR_CREATEATTDEF:	"Unable to create the requested AttDef entity."
// CMD_ERR_CREATETEXT:		"Unable to create the requested text entity."
// CMD_ERR_CREATEDONUT:		"Unable to create the requested donut."
// CMD_ERR_CREATELINE:		"Unable to create the requested line."
// CMD_ERR_DEFBLOCK:		"Unable to define the requested block(s)."
// CMD_ERR_BUILDENT:		"Unable to build the requested entity or entities."
// CMD_ERR_UNTERMINATED:	"Unterminated complex entity."
// CMD_ERR_INSERTDEF:		"Any drawing definitions in the requested block have been imported."
// CMD_ERR_BADENTNAME:		"Invalid entity name encountered."
//  Can I put the line and donut ones into CREATEENT? (Lots in cmds4)
// CMD_ERR_NOSAVEDVIEWS:	"This drawing contains no saved views."
// CMD_ERR_NOTINPSPACE:		"This command is not available in Paper space."
// CMD_ERR_NOPAPERVIEW:		"Cannot restore a paper space view into model space."
// CMD_ERR_NOMODELVIEW:		"Cannot restore a model space view into paper space."
// CMD_ERR_NODYNZOOM:		"Dynamic zoom is not yet supported."
// CMD_ERR_NODESTROY:		"Handles cannot be disabled once they have been created."
// CMD_ERR_GPS:				"An error has occurred in GPS"
//  I would love to kill these next two.  They're scary, and do not give the user any information they can use.
// CMD_ERR_OPENGLCHOOSE:	"OpenGL error: ChoosePixelFormat failed."
// CMD_ERR_OPENGLSET:		"OpenGL error: SetPixelFormat failed."
// CMD_ERR_MOREMEM:			"More memory is needed to do this operation"
//  If they see this one, they're toast.
// CMD_ERR_UNABLE:			"IntelliCAD is unable to complete this operation."
//  Replace this where possible.  I've only used it when the old message just said "ERROR" (+/-)
// CMD_ERR_NOEXTDWGS:		"No drawings are accessible for querying, use \"Access External Drawings\"."

//  Errors loading/unloading SDS apps
// CMD_ERR_INVALIDDLL:		"Error loading \"%s\": Invalid Dynamic-link library (DLL)."
// CMD_ERR_SYSNOMEM:		"Error loading \"%s\": \nSystem is out of memory, executable file is corrupt,\nor relocations are invalid."
// CMD_ERR_ACCESS_DENIED:	"Error loading \"%s\": Sharing or network-protection error."
// CMD_ERR_OUTOFMEMORY:		"Insufficient memory to start application \"%s\"."
// CMD_ERR_NOT_READY:		"Application \"%s\" requires Microsoft Windows 32-bit extensions."
// CMD_ERR_PROC_NOTFOUND:	"Error loading \"%s\": Unresolved function at run time."
// CMD_ERR_DLLNOTFOUND:		"One of the library files needed to run \"%s\" cannot be found."
// CMD_ERR_DLLLOADPROB:		"System error loading \"%s\" (%ld)."
// CMD_ERR_CANTFINDAPP:		"Error loading application: Could not find %s."
// CMD_ERR_BADENTPOINT:		"Can't get Entry Point function in DLL; Load failed %s"
// CMD_ERR_UNLOADING:		"Error unloading \"%s\" (%ld)."
// CMD_ERR_FINDUNLOAD:		"Error unloading application: Could not find %s."
// CMD_ERR_ENTSINSET:		"Entities in set: %ld"

//  I'm throwing all mine right here and will organize them once I have a bunch made up.
// CMD_ERR_CANTLOADREN:		"Unable to load rendering code (%s)"
// CMD_ERR_FLATTENED:		"%li entities flattened. "
// CMD_ERR_NOTPARALLEL:		"%li entities not parallel to UCS."
// CMD_ERR_CANTFINDMENU:	"Unable to find loaded menu \"%s\"."
// CMD_ERR_CANTFINDDWG:		"Unable to find drawing \"%s\"."
// CMD_ERR_PLEASESPEC:		"Please specify... %s "
// CMD_ERR_BADVPORTNUM:		"Can't set CVPORT to %i"
// CMD_ERR_GETOPENFILE:		"Internal error, GetOpenFileName returned Error # %x"
// CMD_ERR_CANTOPENDWG:		"Error opening drawing \"%s\"."

// CMD_ERR_REBUILDTABLE:	"Error rebuilding %s table."
// CMD_ERR_BUILDTABLE:		"Error building %s table."
// CMD_ERR_CREATEBLOCK:		"Error:  Failed to create block entity (%ld)."
// CMD_ERR_NOTABLEITEM:		"Unable to find %s %s."
// CMD_ERR_ALREADYEXISTS:	"A %s with that name already exists."
// CMD_ERR_WASRENAMEDAS:	"The %s was renamed as %s."

// CMD_ERR_BPOLYPATH:		"The area you selected is invalid."
// CMD_ERR_BPOLYELLIPSESPLINE:		"You cannot use ellipses or splines to create a polyline boundary."
// CMD_ERR_R12HATCHTOOMANYLINES:	"The line segments of the selected hatch pattern are too dense for the specified area.  Try using a larger hatch scale to make the segments less dense or select a hatch pattern with fewer segments."
// CMD_ERR_SELECTHATCH:		"The selected entity is not a hatch entity."

//  Added Cybage MM 25/02/2002 DD/MM/YYYY [
//  Reason: Fix for Bug # 77975 and 77978 
// CMD_ERR_NONZERODIST:		"The distance between the two points must be nonzero."
// CMD_ERR_INVALIDPOINT:	"Invalid Point Entered"
//  Added Cybage MM 25/02/2002 DD/MM/YYYY ]
//  Bugzilla No. 78117; 22/04/2002
// CMD_ERR_EXITDONE:		"Type EXIT or DONE to return to COMMAND prompt."
//  Bugzilla No. 78128; 29/04/2002
// CMD_ERR_INVALIDNAME:		"Invalid Name Entered"

//  Not used anywhere yet.  It's here just in case:
// CMD_ERR_COINCENDPTS:		"Cannot use coincident endpoints for this operation."
// CMD_ERR_NOTCLOSEDMPLINE:	"The selected polyline is not closed in the M direction."
