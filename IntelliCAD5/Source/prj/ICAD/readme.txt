________________________________________________________________

	README.TXT for IntelliCAD(r) 98 from Visio

	Copyright 1998 by Visio Corporation
________________________________________________________________


Thank you for purchasing IntelliCAD 98.
  
This file contains the most up-to-date information regarding 
IntelliCAD 98.  Information here supersedes the online documentation.  

Note:  When referring to a command or variable, this document 
shows the name in all capital letters (e.g., CIRCLE).

We are constantly working to improve and perfect IntelliCAD.  A few 
remaining problems are listed near the bottom of this document, 
with workarounds when available, so that you can easily avoid them.

________________________________________________________________


Contacting Visio About IntelliCAD 98

Your questions, requests and suggestions drive future development.
If you have questions about using IntelliCAD, or want to make a 
suggestion for a future release of IntelliCAD, please contact us at:

	http://www.visio.com/support 

We also invite your feedback on the documentation and help.  
Your time and comments are very much appreciated.  

You can also register your copy of IntelliCAD online.  From the
Windows Start menu, choose Programs, then IntelliCAD 98, and 
select Register IntelliCAD Online. 

________________________________________________________________


The Following Information is Covered Below:

	Installing IntelliCAD 98
	General notes
	Notes on documentation
	Notes for those who use Autodesk AutoCAD
	Notes for those who use AutoCAD drawings in Visio 5.0 products
	Printer drivers for HPGL and HP-GL/2 plotters
	Frequently Asked Questions
	Limitations and workarounds
	Notes for those developing applications for IntelliCAD
	Instructions for using the AutoCAD Bonus/CADTools

________________________________________________________________


Installing IntelliCAD 98

As with many other programs, to install IntelliCAD 98 under 
Windows NT, you need to have permissions as a local administrator 
for that machine.

While VBA is being installed, the program temporarily places some
files on your default drive (usually C: ), even if you are 
installing IntelliCAD to another drive.  These files will be
deleted when the install is complete.

To install IntelliCAD 98:
	- Close any applications you have open.
	- The IntelliCAD 98 CD will automatically run the install
	  program when you place it in your CD-ROM drive.
	- If your machine does not autorun the install, run
	  setup.exe on the IntelliCAD 98 CD to begin.
	- Follow the prompts in the install program.

After installing IntelliCAD, you may need to reboot your 
machine before all features will work as intended.  You do not 
need to reboot immediately.

________________________________________________________________


General Notes

IntelliCAD 98 includes collections of fonts.  In addition, if 
you have purchased other fonts (.shx or .shp files) for use in 
your .dwg files, you can use those with IntelliCAD.  Just copy 
your font files into the IntelliCAD 98\Fonts directory.

If you are running Windows NT and want to use VBA, you will
need to have at least Windows Service Pack 2.  You can see
which Service Pack you have when you start up your machine;
the information is on the blue screen at the beginning. If
you don't have Service Pack 2 and want to install it, you
can download it from the Microsoft web site.

________________________________________________________________


Notes on Documentation

IntelliCAD 98 includes printed manuals, online documentation, 
as well as context-sensitive help.  

Help is available from any of the commands - just press F1.  
Help is available from each of the dialog boxes as well. 

The following printed and online documents are available:
	- Using IntelliCAD 98 (both printed and online)
	- IntelliCAD 98 for Autodesk AutoCAD Users (printed and 
	  online), a guide for those familiar with Autodesk 
	  AutoCAD that highlights similarities and differences
	- Command Reference (online) that gives details about 
	  commands, tools, aliases, and system variables
	- Developers' Reference, a guide to programming in
	  SDS (online)
	- List of Command Aliases (online)
	- List of System Variables (online)
	- Service and Support Information (online)
	- IntelliCAD Solutions and Service (online), a guide
	  to third party developers and products.
	
Each time you start IntelliCAD, a Tip of The Day is displayed.
These are handy bits of information on using the program.  You
can read through all the tips by clicking the Next Tip button, 
or you can open the file icad.tip as text and read them there.

________________________________________________________________


Notes On Samples and Other Files Included With IntelliCAD 98

All the files installed by the setup program, with the exception
of ICAD.EXE, are also included on your IntelliCAD 98 CD in 
uncompressed form.  If you ever remove a file by accident, you can 
copy it from the CD.  You do not need to reinstall IntelliCAD.  
The directory on the CD is named UNCOMPRESSED.

IntelliCAD is installed with a Sample directory that contains
drawings and other example files.  In addition, there are some
interesting drawings, rendered images, and other example files
on the CD in the directory named EXTRAS.

There is also a directory of scripts installed with IntelliCAD.
You can view these as text to see how scripts are written, or 
to use them as a basis for creating your own custom scripts.  
You can run scripts either by dragging and dropping them in 
IntelliCAD, or from the Tools menu choose Run Script.  Each is 
designed to run in a newly created drawing, but erases any 
existing entities and resets some of the drawing variables to
assure a clean start.  For more information on creating your
own scripts see the Frequently Asked Questions below.

________________________________________________________________


Notes For Those Who Use Autodesk AutoCAD

In addition to the information below, please refer to the
"IntelliCAD for AutoCAD Users" documentation (printed and online).

You will notice many similarities between AutoCAD and IntelliCAD.
We have done everything possible to ensure that AutoCAD users
are able to use IntelliCAD comfortably right from the start.

One difference that will be immediately apparent is that there
is no command line showing.  To display the Command Bar, go to 
the View menu and choose Command Bar.  Once displayed, you can
move the command bar anywhere on your screen.

IntelliCAD 98 provides many new features and improvements over 
features you are used to using in AutoCAD.  We encourage you to 
take some time to explore and try new things - open several of
the sample drawings at once, check out the Drawing Explorer (TM), 
and browse through the Drawing Settings and Dimension Settings 
dialog boxes.

Plotting is handled differently in IntelliCAD.  Take some time to 
familiarize yourself with the Print Settings dialog box.  Be sure
to click Print Setup -> Properties to see the Carousel feature
which allows you to map line weights.  These dialog boxes provide 
the functionality you need to format plotted output.  


Round-Tripping Drawing Data

	IntelliCAD supports round-tripping of drawing data for 
	AutoCAD drawings through R14.  You can open, work with, and 
	save your AutoCAD drawings in IntelliCAD, and vice-versa.
	Every effort has been made to ensure the integrity of
	your data.


Working in IntelliCAD using drawings created in AutoCAD

	There is always the possibility of an existing drawing file
	having been corrupted.  If you have a drawing you created
	in AutoCAD, and there is a problem with it, open it in
	AutoCAD and run the AUDIT command.  If doing that does not 
	solve the problem, use the AutoCAD RECOVER feature to clean 
	up corrupted AutoCAD drawings before bringing them into 
	IntelliCAD.


Command Support

	IntelliCAD supports hundreds of AutoCAD commands, and many
	that AutoCAD does not offer.  In supported commands,  
	the things you already know from using AutoCAD work in
	IntelliCAD (keyboard shortcuts, selection methods, etc.),
	even when it isn't apparent - just enter what you normally
	would.  Script files you have created for AutoCAD using 
	commands supported in IntelliCAD can run without modification.

	IntelliCAD uses some command names that will be new to you.
	To ease your transition we have provided aliases to the 
	commands with which you are familiar.  For instance, when you 
	type DDLMODES, IntelliCAD displays the Drawing Explorer (TM)
	showing layer information for the current drawing.

	Some commands are not supported in IntelliCAD 98:
	  -ASE commands (ASEADMIN, ASELINKS, ASEROWS, ...)
	  -Region-specific commands and commands that work with
	   3D solids ( REGION, UNION, SUBTRACT, MASSPROP, SLICE ...)

	The commands for the creation of 3D Solids (BOX, SPHERE, 
	WEDGE, etc.) are supported; however, rather than creating 
	ACIS solids, IntelliCAD creates polyface meshes.  You can 
	run your scripts that use these commands.  Hide, shade, and
	the rendering features all work with these entities.


System Variables and Drawing Variables

	IntelliCAD uses the variables in your existing drawings 
	(such as ANGBASE, FILLMODE, DONUTID), and the system 
	variables you are used to using (FILEDIA, GRIPSIZE,
	TOOLTIPS, etc.).  You can set them at the command line as
	usual, or use the Drawing Settings, Dimension Settings,
	and Options dialog boxes.

	IntelliCAD 98 supports most of the AutoCAD dimensioning-
	related variables.  With one exception, the variables not 
	supported relate to alternate unit dimensioning and 
	tolerances in dimensions.  These unsupported dimensioning 
	variables display a message when set, and will be retained 
	and round-tripped.  The exception is the variable DIMZIN,
	which controls the display of leading zeros in dimension
	text.  DIMZIN is not supported, and no message is displayed.


Entity Creation and Display

	There are a few AutoCAD R14 entities that cannot be created,
	and some that cannot be displayed in IntelliCAD.  If they 
	already exist in an AutoCAD drawing that you edit using 
	IntelliCAD, they will remain in your drawing file unaltered,
	and will be available again when you open your drawing in
	AutoCAD.

	These AutoCAD entities will be displayed, but cannot be 
	created in IntelliCAD 98:
	  -MLines (Multiple Lines)
	  -Shapes
	  -Images - BMP, CALS Type I, PCX, TIFF, JPEG and GIF 
	   formats are displayed.  

	These entities cannot be created or displayed in IntelliCAD 98:
	  -AutoCAD R14 Hatches  (IntelliCAD does create pre-R14 
	   hatch blocks)
	  -3D Solid and Regions
	  -AutoCAD proxy entities created by third-party applications.


Copying entities between IntelliCAD and AutoCAD

	IntelliCAD and AutoCAD save entities to the Windows clipboard
	in different formats.  While you can copy and paste entities
	within each program, it is not possible to copy and paste
	between them at this time.  

	If you need to copy some entities between the two, you can do 
	so by saving the entities into an independent drawing (using 
	WBLOCK) which you can insert into a drawing in the other 
	program, and explode back into its component entities.


Automatically loading your LISP files

	To load LISP routines automatically at startup, enter them
	in icad.lsp.


Using the AutoCAD Bonus CADTools in IntelliCAD

	If you use the AutoCAD bonus CADTools AutoLISP programs, you
	will be happy to know you can use them in IntelliCAD also.
	For complete instructions on how to do this, see the notes
	at the bottom of this document.

________________________________________________________________


Using Visio 5.0 Products With IntelliCAD Or AutoCAD Drawings

Modifying Visio 5.0 DWG Component Files

	If you have previously installed a Visio 5.0 program 
	(such as Visio Standard 5.0, Visio Professional 5.0 or 
	Visio Technical 5.0), installing IntelliCAD 98 will 
	update the Visio 5.0 DWG file handling components and 
	related files.  Updating these files ensures that you 
	can take advantage of the IntelliCAD 98 DWG file format 
	compatibility through R14.  After IntelliCAD has updated 
	the Visio components, you can use Visio 5.0 products to 
	view drawings created with IntelliCAD 98 or AutoCAD R14.  
	You can also double-click an inserted IntelliCAD drawing to 
	edit it in place.


Installing IntelliCAD modifies the following DWG Component files 
in the Visio 5.0 product directories:

	Visio root directory:
		Drwex.DLL
		VisioDWG.DLL
	Visio root directory (Visio Technical - only):
		Icad.dwg
	Solutions\Visio Extras directory (Visio Technical - only): 
		Dwgcnv32.VSL
		Dwgcnvt.HLP 


Importing AutoCAD or IntelliCAD drawings 

	If you have previously installed Visio Standard 5.0 or 
	Visio Professional 5.0 and have not yet installed the 5.0b 
	Service Release, you cannot import AutoCAD or IntelliCAD 
	files by using the File > Open > AutoCAD Drawing menu 
	command.  You can still import an AutoCAD or IntelliCAD 
	drawing by using the Insert > Object/IntelliCAD Drawing 
	or Insert > Object/Create from File (DWG) commands.  
	We recommend that you can install the Visio 5.0b Service 
	Release patch found on the Visio Web site at www.visio.com 
	to enable importing with the File > Open command.

	Similarly, if you install IntelliCAD 98 and then install 
	Visio Standard 5.0 or Visio Professional 5.0, you will 
	have access to only one of the commands for importing 
	AutoCAD or IntelliCAD drawings.  We recommend installing 
	the Visio 5.0b Service Release after installing your 
	Visio 5.0 product.


For the latest information on Service Releases and other product 
information, be sure to check the Visio Web site (www.visio.com).

________________________________________________________________


Printer Drivers For HPGL And HP-GL/2 Plotters

If you use plotters, you will need to install the WinLINE 
printer driver before you can create final-quality prints/plots.

This driver supports custom paper sizes, custom line weights, 
screen-color to plotted-color mapping, and other advanced features.

We include the WinLINE printer driver with IntelliCAD.  For complete 
instructions on installing the WinLINE driver on your computer see 
the file readme.install.nt.txt or readme.install.w95.txt (the 
appropriate version is installed based upon your operating system) 
in your directory:
	Intellicad 98\drivers\printers

________________________________________________________________


Frequently Asked Questions


Q:	How do I assign line weights for printing my drawings?

A:	IntelliCAD includes the WinLINE printer driver to enable
	you to use many advanced printer and plotter features,
	including printed line weights.  

	First, read the section above on how to install the 
	WinLINE driver.  The options described here will be 
	available after you install the driver.
	
	To assign line weights, go to the File menu and choose
	Print Setup.  From the Print Setup dialog box, click
	Properties.  In this dialog box choose the Carousel 
	option.  Whether or not your printer or plotter has a 
	physical pen carousel, the carousel metaphor is used to
	assign line widths to particular pens/colors.

	For more detailed information, refer to your Using
	IntelliCAD 98 manual.  See the chapter on Formatting and
	Printing Paper Drawings.


Q:	How do I return to my own customized menus after using
	other menus I have imported?

A:	Before you import other menus, save your customized menu
	setup by exporting it to a menu file (.icm).  From the
	Customize dialog box, on the menus tab, choose Export.
	Later, when you want to return to your own menus, Import
	them from your menu file.

	You can revert to the original menus, toolbars, aliases,
	and keyboard shortcuts supplied with IntelliCAD 98 at any 
	time by going to the Customize dialog box and clicking the 
	Reset button from the appropriate tab.  Only the items on
	that tab will be reset.


Q:	Can I keep my usual menus visible and add on a few more?

A:	Yes.  In the Customize dialog box, select the Menus tab,
	and click Import to bring up the dialog box for importing
	menus.  There is a check box called Append To Current Menu.  
	When it is checked, new menus are added to the menu bar, 
	and your existing ones are still there for you to use.

	If you want to replace your existing menus entirely, 
	deselect that checkbox.
	
	Remember, if you will want to return to your original 
	menu settings later, be sure to export your menus before 
	appending or replacing menus.  (See Q/A above.)


Q:	How do I record a script?

A:	Scripts record your command line entries and coordinates
	entered using the mouse.  To record a script, go to the
	Tools menu, and choose Record Script.  Everything you enter
	at the command line, and points you select using the mouse
	will be recorded.

	When you are finished recording your script, go to the Tools
	menu, and choose Stop Recording.  To use your script, choose
	Run Script.

	Do not select commands from the toolbars or menus while you
	are recording a script - enter them at the command line.
	The script recorder will record the command macro from the
	tool or menu command, and command macros do not play back
	correctly in script files.

	When recording a script, IntelliCAD uses the points you
	actually select.  Drawing orthogonally (ORTHOMODE on) or
	using running entity snaps will effect what you see on the
	screen while you are recording, but the points recorded in
	the script will be those you selected with the mouse.

	One-time entity snaps are recorded and used in scripts.

	If you make a mistake, or want to change your script, you 
	can edit it in any text editor.

________________________________________________________________


Limitations And Workarounds

By making this list available, we hope to help you avoid problems 
we think you may experience, and to give you alternate ways to 
achieve the desired result.  

    -------------------
	If a drawing is saved in paper space, the preview image
	in the Open Drawing dialog will display incorrectly.

	This does not harm the drawing itself.  If a correct
	preview image is critical, save the drawing in model space,
	with TILEMODE set to On (the default in a new drawing).
    -------------------
	IntelliCAD 98 does not create linetypes that include text
	or other special characters.  If an existing drawing has 
	these linetypes, the characters in the lines will not 
	display in IntelliCAD.  This is only a limitation in
	creating and displaying these linetypes.

	These linetypes, and the entities that use them will be 
	saved in the drawing unharmed.  Also, you can use these 
	existing linetypes, just like any others in your drawing,
 	to create new entities.
    -------------------
	Some commands that request text input will take menu and 
	toolbar macros using transparent commands as the text input.  
	If, at a prompt requesting that you enter text, you enter a 
	transparent command, either directly, or from the toolbars or 
	menus, the command (such as 'PMTHIST) will be accepted as
	the text.

	To avoid this, do not use transparent commands from a prompt
	requesting text input.
    -------------------
	Multiple UNDO and REDO commands do not always have the
	correct effect on model space viewports (MVIEW) and paper 
	space (TILEMODE).  It may appear that you have done one 
	more or one fewer undo or redo than you intended.

	If you will be working with MVIEW and TILEMODE, and expect
	that you might want to return to the current state of the
	drawing, save the drawing first.
    -------------------
	In some circumstances, when tiling paper space drawings
	the drawing will not display in the window.

	To restore the correct display, click the Redraw tool 
	on the View toolbar.
    -------------------
	If you use a 256 color display, you may see a drawing 
	displayed with the wrong colors when switching between
	IntelliCAD and an application that changes the Windows 
	color palette.

	To restore the correct display, click the Redraw tool 
	on the View toolbar, or toggle back to the other application, 
	then return to IntelliCAD.
    -------------------
	When printing in Windows 95, if Preview does not work, the 
	default printing device may be configured incorrectly.

	To configure your printer, go the Windows Explorer and
	select Printers.
    -------------------
	There are some limitations when using true ellipses or splines:
	- IntelliCAD cannot trim, or extend entities using a 
	  true ellipse or spline. 
	- IntelliCAD cannot break a spline.
	- Some entity snaps cannot use a true ellipse (e.g., tangent).
	- IntelliCAD cannot use a true ellipse or spline as a hatch 
	  boundary.  

	For ellipses, there is a simple	way to avoid these problems.  
	Ellipses can be created two ways, as true ellipses, or 
	as polylines.  All of the above features work fine using 
	ellipses that are created as polylines.  To create ellipses 
	as polylines, in the Drawing Settings dialog box, under 
	Entity Creation > Ellipses, select the "Create ellipses 
	as polylines." option. 
    -------------------
	Hide, Shade, and the rendering features are not available in
	paper space, or in model space viewports.  Entities in an
	externally referenced drawing (XRef) are also unaffected by 
	these commands.
	
	You can hide, shade, and render in the default workspace, 
	which is model space with TILEMODE off (floating viewports).

	To hide, shade, and render externally referenced drawings (XRefs),
	type XREF and choose the Insert option to convert XRefs to
	normal entities.
    -------------------
	Nested externally referenced drawings (XRefs) in drawings
	created in IntelliCAD may not display correctly if the
	drawing is opened in AutoCAD.

	To display nested XRefs, AutoCAD needs to perform a full
	read on the XRef'ed drawings.
    -------------------
	IntelliCAD enables you to open many drawings at the same time.
	In extreme cases, it is possible to reach the virtual memory
	limit of your machine.  In this case, Windows will display 
	a message box to let you know.  

	If you experience this, simply close IntelliCAD.  It may
	take a few seconds to release virtual memory, so wait until
	the program has stopped accessing your hard disk before
	restarting IntelliCAD.

	If you work with unusually large numbers of drawings and 
	find that you are running out of virtual memory, you may 
	want to exit and re-enter IntelliCAD from time to time.
    -------------------
	If you work with drawings created in AutoCAD, and those 
	drawings contain groups (a particular collection of entities),
	you can work with those groups in IntelliCAD.  If you copy a
	group, in IntelliCAD, save the drawing and open it in AutoCAD,
	the group will be displayed but cannot be edited.

	To edit the entities in the group, when working in AutoCAD,
	turn groups off (in AutoCAD, use Ctrl-A).  If groups are 
	turned back on, the entities will once again be inaccessible.
    -------------------
	The Make Oblique dimensioning tool (the same as the Oblique
	option of the command DIMEDIT) may corrupt dimensions on which
	it is used.  

	If this occurs, the dimension will be displayed incorrectly.
	You can use UNDO to revert to the original dimension.
    -------------------
	When opening a drawing saved in paper space, if the layer on
	which a viewport was created is frozen, model space entities 
	may not display correctly.
	
	To make the entities redisplay, thaw the frozen viewport layers, 
	then refreeze them.
    -------------------
	Dimension text whose style uses a fixed (non-zero) text height 
	will initially display correctly.  However, if these dimensions 
	are updated or edited IntelliCAD may ignore the specified height 
	in the style and instead display the dimension using the default 
	height for dimension text (stored in the DIMTXT variable).

	This is only a problem with displaying the text at the right
	height.  The record of the text is saved in the drawing file
	correctly.  

	To force the text to display at the desired height, set the
	value of DIMTXT to the fixed height value prior to editing
	or updating the dimension.

	If you are sharing files with people using Autodesk AutoCAD, 
	do not attempt to fix the appearance of the text - it will 
	display correctly when opened in AutoCAD.	
    -------------------
	Setting DIMUNIT equal to 8 should cause dimensions to use the 
	same numeric display style and decimal separator as specified 
	in your Windows Regional Settings.   Instead, it displays 
	dimension units formatted as decimals as if DIMUNIT=2.

	To cause units in dimensions to format correctly, choose one
	of the other DIMUNIT settings, 1-7. 
    -------------------
	If a drawing inserted as a block contains a dimension which 
	uses a dimension style having other than the default settings, 
	and you explode the block, the dimension style data on that 
	dimension may be lost.  Further editing of the dimension may 
	cause IntelliCAD to crash.

	If you are importing the block to insert entities into a
	drawing, you can avoid this problem by opening the drawing
	containing the entities (including dimensions) then copy and
	paste them into the current drawing.
    -------------------
	Attempting to Restore some saved arrangements of windows 
	(viewports) may be interpreted as a request to close the 
	drawing.    
	 
	Click Cancel to avoid closing the last window, and therefor
	the drawing.  Your other windows will remain closed however.
    -------------------
	In the Drawing Settings dialog box, under 3D settings, the 
	check box "Show plan view when changing UCS" does not always
	result in the correct display of a plan view.

	To display the plan view after changing the UCS when this box
	is checked, REGEN the drawing, or use ZOOM > Extents.  Also, 
	the PLAN command displays the plan view of either the WCS or 
	UCS, as specified.
    -------------------
	MText formatted as vertical (top to bottom) text is displayed
	horizontally (but saves to the drawing database as vertical).

	This is a display problem only.  If you open an existing 
	drawing with vertically formatted MText, it will display
	horizontally, but is saved back to the drawing file as 
	vertical (top to bottom) MText.
    -------------------
	There are problems in Windows 95 with inserting a Microsoft 
	PowerPoint presentation as an OLE object in an IntelliCAD 
	drawing, then running it in place.  Some menu items will 
	cause IntelliCAD to crash.

	To insert and use a PowerPoint presentation in an IntelliCAD
	drawing, insert the object as an icon (check the Display As 
	Icon check box).  To launch PowerPoint and work with the file, 
	double click the icon in the drawing.
    -------------------
	Moving justified text using grips can cause the text to rotate.

	To move justified text without rotating it, use the MOVE command.
    -------------------
	Stretching, moving, or exploding a dimension in a UCS other
	than the one in which it was created can cause the dimension 
	to move to an incorrect location.

	To move a dimension, switch to its UCS and move it.  To switch
	to the UCS for any particular entity, type UCS and choose the
	Entity option, then select the entity (a dimension in this 
	case) in whose UCS you want to work.
    -------------------
	Scaling or rotating a diameter dimension may shift the position
	of the dimension.

	You can relocate the dimension using the MOVE command.
    -------------------


The following will be supported at some time in the future, but are 
not available in this release:

    -------------------
	There is no option to Remove Hidden Lines for plotting 
	3D drawings.
    -------------------
	There is no command line interface for printing/plotting.
    -------------------
	BHATCH is not supported.  The hatches created in IntelliCAD 
	are not associative.
    -------------------
	The VPLAYER command is not supported.
    -------------------
	In snapshots (slides, or SLD files), some 3D entities do not
	display correctly.
    -------------------
	When entities are pasted into a drawing via the Windows 
	clipboard, they are always pasted into the WCS, not the
	current UCS.
    -------------------
	Tablets and digitizers are not currently supported.
    -------------------
	Dynamic Zoom, and realtime panning and zooming are not
	currently supported.
    -------------------


________________________________________________________________


Notes For Those Developing Applications For IntelliCAD



SDS and LISP Programming Languages


	If you develop applications in Microsoft Developer 
	Studio, you can define your own list of keywords, 
	functions, etc. to be highlighted in the editor.  
	The file is USERTYPE.DAT.  

	IntelliCAD 98 supplies a USERTYPE.DAT file you can use
	that will enable you to highlight the SDS functions
	in the editor.  This file in on the IntelliCAD 98 CD
	in the directory EXTRAS\SDS.  To use the file, 
	copy it to the directory on your hard disk where 
	MSDEV.EXE is located.

	If you already have a USERTYPE file, you can manually
	append the contents of the one supplied with IntelliCAD.


    -------------------
	The sds_cvunit() and (cvunit) functions do not support
	three or more expressions in the FROM or TO arguments.
	Using three or more expressions may return the incorrect
	number.
    -------------------
	Using sds_entnext() on an XRef will give you the ENDBLK 
	instead of stepping through the XRef's database.
    -------------------
	

Dialog Control Language (DCL)

    -------------------
	IntelliCAD does not supply default values for tiles in 
	dialog boxes. For example, an image_tile must have the 
	height or width, and an aspect ratio specified, or both
	the height and width, in the DCL code.	If these values
	are missing, the tile will not display.
    -------------------
	IntelliCAD does not support audit_level error checking
	in DCL files.
    -------------------


Visual Basic for Applications (VBA)

VBA is included as a preview of future functionality:
	- A preliminary diagram of the object model is in your 
	  IntelliCAD 98\API\VB directory.  
	- The Microsoft Visual Basic Editor is included with 
	  IntelliCAD.  To launch it, go to the Tools menu,
	  select Visual Basic (Preview), then choose Visual
	  Basic Editor.
	- The Macros dialog box, accessible from the VBARUN 
	  command, is not functional. 



________________________________________________________________


Instructions For Using The AutoCAD Bonus/CADTools

There is a set of LISP programs called AutoCAD Bonus/CADTools
that ships with AutoCAD.  If you use AutoCAD, and have these tools,
you can use them in IntelliCAD as well.

To run the majority of the AutoCAD Bonus/CADTools utility programs 
in IntelliCAD 98, make the following changes to the file ac_bonus.lsp. 
These changes simply comment out any reference to loading any bonus 
ARX applications, and disable the error handling utility function 
called (init_bonus_error).

1.  On line number 239, just above this line:
	(autoarxacedload "ARCTEXT.arx" '("ARCTEXT" "ATEXT"))
    insert the following two characters   ;| 	
    (this is a semicolon and a vertical bar or 'shift backslash')

2.  On line number 249, just below this line:
	(autoarxacedload "SYSVDLG.arx" '("SYSDLG"))
    insert the following two characters   |;   
    (this is a vertical bar or 'shift backslash' and a semicolon)

NOTE:  These two inserted lines will have the effect of commenting 
       out all of the lines in between them, and they will be ignored 
       by LISP.  The end result should look like this:

	;|
	(autoarxacedload "ARCTEXT.arx" '("ARCTEXT" "ATEXT"))
	.
	.			
	.
	(autoarxacedload "SYSVDLG.arx" '("SYSDLG"))
	|;


3.  Locate the function (init_bonus_error) in the file around line 
    407 and insert the following lines just before the function:

	(defun init_bonus_error ( lst / ss undo_init)
	    (princ) ; Disable error handler specific to AutoCAD ARX
	)
	;|

4.  Locate the closing parenthesis for the original (init_bonus_error) 
    function around line 523.  You will see the line:

	); defun init_bonus_error

	Just below this line insert  |;


NOTE:  These two steps (3 and 4) have the effect of commenting out 
       the entire (init_bonus_error) function, thereby forcing
       ac_bonus.lsp to use the substitute routine we placed above 
       it in step #3 above.  The end result should look like this:

	(defun init_bonus_error ( lst / ss undo_init)
	    (princ) ; Disable error handler specific to AutoCAD ARX
	)
	;|
	(defun init_bonus_error) (
	.
	.
	.
	); defun init_bonus_error
	|;



________________________________________________________________


Copyright (c) 1998 by Visio Corporation

AutoCAD and Autodesk are registered trademarks of Autodesk, Inc.

WinLINE and WinLINE NT are registered trademarks of Software
Mechanics Pty Ltd. 

ACIS is a registered trademark of SPATIAL TECHNOLOGY, INC.

Portions copyright 1991-1998 Compuware Corporation.

International Space Station engineering courtesy of NASA JSC.
Earth photo in Space Station rendering courtesy of NASA.
You can visit the NASA web sites at:
	http://images.jsc.nasa.gov/	NASA Earth photos
	http://station.nasa.gov/	Intl. Space Station information  

Third-Party Trademarks:  All other trademarks, trade names or 
company names referenced herein are used for identification only 
and are the property of their respective owners.

________________________________________________________________

Updated:  21 March, 1998   5:00 p.m.
  
