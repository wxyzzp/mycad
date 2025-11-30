/* C:\ICAD\PRJ\LIB\DB\STYLETABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

/************************** db_styletabrec **************************/

#include "db.h"
#include "styletabrec.h"
#include "TTFInfoGetter.H"

/*F*/
db_styletabrec::db_styletabrec(void) :
	db_tablerecord(DB_STYLETABREC,db_str_quotequote) {

	fontpn=bigfontpn=NULL;
	db_astrncpy(&fontpn,db_str_txt,-1);
	lastht=0.2; xscl=1.0; fixedht=obl=0.0; gen=0;
	font=bigfont=NULL;
	xrefblkhip=NULL;
	localName=NULL;	// EBATECH(CNBR)
	}
db_styletabrec::db_styletabrec(char *pname, db_drawing *dp) :
	db_tablerecord(DB_STYLETABREC,pname) {

	fontpn=bigfontpn=NULL;
	db_astrncpy(&fontpn,db_str_txt,-1);
	lastht=0.2; xscl=1.0; fixedht=obl=0.0; gen=0;
	font=bigfont=NULL;
	xrefblkhip=NULL;
	localName=NULL;	// EBATECH(CNBR)
}
db_styletabrec::db_styletabrec(const db_styletabrec &sour) :
	db_tablerecord(sour) {	/* Copy constructor */

	fontpn	 =NULL;
	if (sour.fontpn!=NULL)
		db_astrncpy(&fontpn   ,sour.fontpn	 ,-1);
	bigfontpn=NULL;
	if (sour.bigfontpn!=NULL)
		db_astrncpy(&bigfontpn,sour.bigfontpn,-1);
	// EBATECH(CNBR) 2001/06/11 -[ localName member
	localName = NULL;
	if (sour.localName!=NULL)
		db_astrncpy(&localName,sour.localName,-1);
	// EBATECH(CNBR) ]-

	fixedht=sour.fixedht;
	xscl=sour.xscl;
	lastht=sour.lastht;
	obl=sour.obl;
	gen=sour.gen;
	font=sour.font;
	bigfont=sour.bigfont;
	xrefblkhip=sour.xrefblkhip;
	}

db_styletabrec::~db_styletabrec(void) {
	if( fontpn ) delete [] fontpn;
	if( bigfontpn ) delete [] bigfontpn;
	if( localName ) delete [] localName;
}

int db_styletabrec::entmod(struct resbuf *modll, db_drawing *dp)
{
	int rc = db_tablerecord::entmod(modll, dp);
	if(!rc)
		setFontFamilyEED(); // localName should be set
	return rc;
}

bool db_styletabrec::hasTrueTypeFont(
	void)
	{
// to do - maybe just check for code in the font that hangs off of the style record

	// do the easy check first
#ifdef _STYLE_FONT_NAME_
	if ((0 == ic_chkext(fontpn, "shx"/*DNT*/)) ||
		(0 == ic_chkext(fontpn, "shp"/*DNT*/)))
		return false;
	else if ((0 == ic_chkext(fontpn, "ttf"/*DNT*/)) ||
			 (0 == ic_chkext(fontpn, "ttc"/*Win2000*/)))
			return true;
#else
	if ((0 == ic_chkext(font ? font->pn : NULL, "shx"/*DNT*/)) ||
		(0 == ic_chkext(font ? font->pn : NULL, "shp"/*DNT*/)))
		return false;
	else if ((0 == ic_chkext(font ? font->pn : NULL, "ttf"/*DNT*/)) ||
			 (0 == ic_chkext(font ? font->pn : NULL, "ttc"/*Win2000*/)))
			return true;
#endif

	// sometimes the font doesn't have an extension,
	// so check for the EED we expect for a TrueType font
	resbuf *apps = sds_buildlist(RTSTR, "ACAD"/*DNT*/, 0);	// the family is stored in ACAD eed
	ASSERT(apps);

	bool hasTTF = false;
	if (apps)
		{
		resbuf *eedrb = get_eed(apps, NULL);
		if (eedrb)
			{
			if (ic_ret_assoc(eedrb, 1000) && ic_ret_assoc(eedrb, 1071))
				hasTTF = true;

			sds_relrb(eedrb);
			}

		sds_relrb(apps);
		}

	return hasTTF;
	}

bool db_styletabrec::getFontFamily(
	char *family)											// out - returns family
	{

	ASSERT(family);
	bool success;
	if( localName )
	{
		strcpy(family, localName );
		success = true;
	}
	else
	{
		strcpy(family, "Arial"/*DNT*/); 	// default to Arial
		success = false;
	}
	return success;
	}

bool db_styletabrec::setFontFamily(
	const char *family,
	db_drawing *drawing)
	{

	ASSERT(family && drawing);

	if ((NULL == family) || (NULL == drawing))
		return false;

	// EBATECH(CNBR) -[ convert local name to global name and style
	long stylenumber;
	char lname[IC_ACADBUF];
	char globalName[IC_ACADBUF];

	strcpy(lname,family);
	if( CTTFInfoGetter::get().getGlobalStyleByLocalName( lname, stylenumber, globalName ) ==  0){
		db_astrncpy( &localName, NULL, -1 );
		ensureNoTrueTypeEED(drawing);
		return false;
	}
	db_astrncpy( &localName, family, -1 );
	// EBATECH(CNBR) ]-

	resbuf *apps = sds_buildlist(RTSTR, "ACAD"/*DNT*/, 0);	// the family is stored in ACAD eed
	ASSERT(apps);

	bool success = true;
	if (apps)
		{
		resbuf *eedrb = get_eed(apps, NULL);
		if (NULL == eedrb)
			{
			if (!addNewTrueTypeEED(globalName, 34, drawing))	// no EED, so build it; 34 is default for Arial, which we use if there's nothing else
				success = false;
			}
		else
			{
			resbuf *familyrb = ic_ret_assoc(eedrb, 1000);	// to do - could get wrong value if ACAD changes format
			ASSERT(familyrb);

			if (familyrb)
				{
				delete [] familyrb->resval.rstring;
				familyrb->resval.rstring = new char [strlen(globalName) + 1];
				ASSERT(familyrb->resval.rstring);

				if (familyrb->resval.rstring)
					{
					strcpy(familyrb->resval.rstring, globalName);
					if (0 != set_eed(eedrb, drawing))
						success = false;
					}
				else
					success = false;
				}
			else
				success = false;

			sds_relrb(eedrb);
			}

		sds_relrb(apps);
		}
	else
		success = false;

	return success;
	}

bool db_styletabrec::getFontStyleNumber(
	long *number)											// out - returns number
	{

	ASSERT(number);

	*number = 34L;											// default to the value for Arial Regular

	resbuf *apps = sds_buildlist(RTSTR, "ACAD"/*DNT*/, 0);	// the family is stored in ACAD eed
	ASSERT(apps);

	bool success = true;
	if (apps)
		{
		resbuf *eedrb = get_eed(apps, NULL);

		if (eedrb)
			{
			resbuf *familyrb = ic_ret_assoc(eedrb, 1071);	// to do - could get wrong value if ACAD changes format
			ASSERT(familyrb);

			if (familyrb)
				*number = familyrb->resval.rlong;
			else
				success = false;

			sds_relrb(eedrb);
			}
		else
			success = false;

		sds_relrb(apps);
		}
	else
		success = false;

	return success;
	}

bool db_styletabrec::setFontStyleNumber(
	const char *family,
	const long styleNumber,
	db_drawing *drawing)
	{

	ASSERT(family && drawing);

	if ((NULL == family) || (NULL == drawing))
		return false;

	// EBATECH(CNBR) -[ convert local name to global name and style
	long stylenumber;
	char lname[IC_ACADBUF];
	char globalName[IC_ACADBUF];

	strcpy(lname,family);
	if( CTTFInfoGetter::get().getGlobalStyleByLocalName( lname, stylenumber, globalName ) ==  0){
		db_astrncpy( &localName, NULL, -1 );
		ensureNoTrueTypeEED(drawing);
		return false;
	}
	db_astrncpy( &localName, family, -1  );
	// EBATECH(CNBR) ]-

	resbuf *apps = sds_buildlist(RTSTR, "ACAD"/*DNT*/, 0);			// the style number is stored in ACAD eed
	ASSERT(apps);

	bool success = true;
	if (apps)
		{
		resbuf *eedrb = get_eed(apps, NULL);
		if (NULL == eedrb)
			{
			if (!addNewTrueTypeEED(globalName, styleNumber, drawing))	// no EED, so build it; 34 is default for Arial, which we use if there's nothing else
				success = false;
			}
		else
			{
			resbuf *styleNumberRb = ic_ret_assoc(eedrb, 1071);		// to do - could get wrong value if ACAD changes format
			ASSERT(styleNumberRb);

			if (styleNumberRb)
				{
				styleNumberRb->resval.rlong = styleNumber;
				if (0 != set_eed(eedrb, drawing))
					success = false;
				}
			else
				success = false;

			sds_relrb(eedrb);
			}

		sds_relrb(apps);
		}
	else
		success = false;

	return success;
	}

// EBATECH(CNBR) -[
bool db_styletabrec::setFontFamilyEED(
	void)
{
	/*DG - 5.5.2003*/// There are short TTF names, e.g. "Swis721 LtCn BT" (like listed in HKLM\SOFTWARE\Microsoft\Shared Tools\Panose
	// and contained in LOGFONT::lfFaceName) and long ones, e.g. "Swiss 721 Light Condensed BT (TrueType)" (listed in
	// HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts and contained in ENUMLOGFONTEX::elfFullName).
	// Icad operates with long names here, so we should use LF_FULLFACESIZE for array size instead of LF_FACESIZE,
	// but I use even IC_ACADBUF for safety.
	char lname[IC_ACADBUF];
	bool success = false;
	char* globalName=NULL;
	long stylenumber;
	resbuf *apps = sds_buildlist(RTSTR, "ACAD"/*DNT*/, 0);	// the family is stored in ACAD eed

	ASSERT(apps);

	if (apps){
		resbuf *eedrb = get_eed(apps, NULL);
		if (eedrb){
			resbuf *globalrb = ic_ret_assoc(eedrb, 1000);
			ASSERT(globalrb);
			if (globalrb){
				db_astrncpy(&globalName, globalrb->resval.rstring, -1);
				resbuf *familyrb = ic_ret_assoc(eedrb, 1071);
				ASSERT(familyrb);
				if (familyrb){
					stylenumber = familyrb->resval.rlong;
					success = true;
				}
			}
			sds_relrb(eedrb);
		}
		sds_relrb(apps);
	}

	if(success){
		if( CTTFInfoGetter::get().getLocalNameByGlobalName(globalName, stylenumber, lname )){
			db_astrncpy(&localName, lname, -1);
		}else{
			success = false;
			if(CTTFInfoGetter::get().isResolveLocalNames())
			{
				ASSERT(!localName);
				CTTFInfoGetter::get().addGlobalToLocal(stylenumber, globalName, &localName);
			}
		}
	}
	if(!success){
		if(!CTTFInfoGetter::get().isResolveLocalNames())
		{
			if( getTTFontNameByFile(lname, fontpn)){
				db_astrncpy(&localName, lname, -1);
				success = true;
			}
		}
		else
		{
			ASSERT(!localName);
			CTTFInfoGetter::get().addNameByFile(fontpn, &localName);
		}
	}
	if(globalName){
		delete [] globalName;
	}
	return success;
}
// EBATECH(CNBR) ]-

bool db_styletabrec::isObliqued(
	void)
	{
	return (0.0 == obl) ? false : true;
	}

bool db_styletabrec::isVertical(
	void)
	{
	int flags;
	get_70(&flags);

	return (flags & 4) ? true : false;
	}

bool db_styletabrec::isBackwards(
	void)
	{
	return (((unsigned char) gen) & 2) ? true : false;
	}

bool db_styletabrec::isUpsideDown(
	void)
	{
	return (((unsigned char) gen) & 4) ? true : false;
	}

bool db_styletabrec::addNewTrueTypeEED(
	const char *family,
	const long styleNumber,
	db_drawing *drawing)
	{


	// WARNING!!!!!!!	Be careful when using this function.  The assumption is that it is being
	//					used when there is no existing EED for the ACAD app.  If there is any,
	//					it will be gone when set_eed() is done.  For IntelliCAD 2000, all there
	//					was in the ACAD EED was the font family and the font style number.	We
	//					also expect that the font family will be a string that will appear first
	//					and that the font style number will be a long that will appear second.
	//					If AutoCAD changes this, our code will have to be modified to handle
	//					both the new scenario for the new format and the old scenario for
	//					existing drawings.


	ASSERT(family && drawing);

	if ((NULL == family) || (NULL == drawing))
		return false;

	resbuf *newEED = sds_buildlist(-3,
									1001, "ACAD"/*DNT*/,		// store in ACAD eed
									1000, family,
									1071, styleNumber,
									0);
	ASSERT(newEED);

	bool success = true;
	if (newEED)
		{
		if (0 != set_eed(newEED, drawing))
			success = false;

		sds_relrb(newEED);
		}
	else
		success = false;

	return success;
	}

bool db_styletabrec::ensureNoTrueTypeEED(
	db_drawing *drawing)
	{

	ASSERT(drawing);

	if (NULL == drawing)
		return false;

	resbuf *apps = sds_buildlist(RTSTR, "ACAD"/*DNT*/, 0);	// the TrueType info is stored in ACAD eed
	ASSERT(apps);

	bool success = true;
	if (apps)
		{
		resbuf *eedrb = get_eed(apps, NULL);
		if (eedrb)
			{
			resbuf *newEED = sds_buildlist(-3,
											1001, "ACAD"/*DNT*/,
											0);

			ASSERT(newEED);
			if (newEED)
				{
				if (0 != set_eed(newEED, drawing))
					success = false;

				sds_relrb(newEED);
				}
			else
				success = false;

			sds_relrb(eedrb);
			}

		sds_relrb(apps);
		}
	else
		success = false;

	return success;
	}

