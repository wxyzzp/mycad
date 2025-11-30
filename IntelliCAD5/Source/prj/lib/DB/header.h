// DWG header definition -- automatically generated -- do not edit
// Copyright (c) 1999, Visio Corporation.  All rights reserved.

#ifndef _INC_HEADER_H
#define _INC_HEADER_H

class DB_CLASS db_header
	{
public:
	db_header(void);
	~db_header(void);

private:
	short		acisoutver;
	short		aflags;
	sds_real	angbase;
	short		angdir;
	sds_real	area;
	short		attmode;
	short		attreq;
	short		aunits;
	short		auprec;
	short		axismode;
	sds_real	axisunit[2];
	sds_real	backz;
	short		blipmode;
	short		cecolor;
	sds_real	celtscale;
	db_handitem *	celtype;
	sds_real	chamfera;
	sds_real	chamferb;
	sds_real	chamferc;
	sds_real	chamferd;
	short		chammode;
	sds_real	circlerad;
	db_handitem *	clayer;
	short		cmdactive;
	short		cmdecho;
	char *		cmdnames;
	short		cmljust;
	sds_real	cmlscale;
	db_handitem *	cmlstyle;
	short		coords;
	short		cvport;
	short		cyclecurr;
	short		cyclekeep;
	short		dbmod;
	short		delobj;
	short		diastat;
	short		dimadec;	// EBATECH(CNBR) Bugzilla#78443 2003/2/7
	short		dimalt;
	short		dimaltd;
	sds_real	dimaltf;
	short		dimalttd;
	short		dimalttz;
	short		dimaltu;
	short		dimaltz;
	char *		dimapost;
	short		dimaso;
	sds_real	dimasz;
	short		dimaunit;
	short		dimazin; // EBATECH(CNBR) Bugzilla#78443 2003/2/7
	char *		dimblk;
	char *		dimblk1;
	char *		dimblk2;
	// EBATECH(watanabe)-[dimldrblk
	char *		dimldrblk;
	// ]-EBATECH(watanabe)
	sds_real	dimcen;
	short		dimclrd;
	short		dimclre;
	short		dimclrt;
	short		dimdec;
	sds_real	dimdle;
	sds_real	dimdli;
	sds_real	dimexe;
	sds_real	dimexo;
	short		dimfit;
	sds_real	dimgap;
	short		dimjust;
	sds_real	dimlfac;
	short		dimlim;
	char *		dimpost;
	sds_real	dimrnd;
	short		dimsah;
	sds_real	dimscale;
	short		dimsd1;
	short		dimsd2;
	short		dimse1;
	short		dimse2;
	short		dimsho;
	short		dimsoxd;
	db_handitem *	dimstyle;
	short		dimtad;
	short		dimtdec;
	sds_real	dimtfac;
	short		dimtih;
	short		dimtix;
	sds_real	dimtm;
	short		dimtofl;
	short		dimtoh;
	short		dimtol;
	short		dimtolj;
	sds_real	dimtp;
	sds_real	dimtsz;
	sds_real	dimtvp;
	db_handitem *	dimtxsty;
	sds_real	dimtxt;
	short		dimtzin;
	short		dimunit;
	short		dimupt;
	short		dimzin;
	short		dispsilh;
	sds_real	distance;
	sds_real	donutid;
	sds_real	donutod;
	short		dragmode;
	short		dwgcodepage;
	short		dwgtitled;
	short		edgemode;
	sds_real	elevation;
	short		explmode;
	sds_point	extmax;
	sds_point	extmin;
	sds_real	facetres;
	short		fastzoom;
	sds_real	filletrad;
	short		fillmode;
	short		fittype;
	short		flatland;
	sds_real	frontz;
	sds_point	gpslastpt;
	short		gpsmode;
	short		gpspaused;
	char *		gpsplayed;
	short		gridmode;
	sds_real	gridunit[2];
	short		handles;
	db_objhandle	handseed;
	short		highlight;
	sds_real	hpang;
	short		hpbound;
	short		hpdouble;
	char *		hpname;
	sds_real	hpscale;
	sds_real	hpspace;
	short		hpstyle;
	sds_point	insbase;
	char *		insname;
	short		islastcmdpt;
	short		isolines;
	sds_real	lastangle;
	sds_real	lastcmdang;
	sds_point	lastcmdpt;
	sds_point	lastpoint;
	sds_real	lenslength;
	short		limcheck;
	sds_real	limmax[2];
	sds_real	limmin[2];
	sds_real	ltscale;
	short		ltsclwblk;
	short		lunits;
	short		luprec;
	short		maxactvp;
	short		measurement;
	short		menuecho;
	char *		menuname;
	short		mirrtext;
	sds_real	offsetdist;
	short		orthomode;
	short		osmode;
	short		pdmode;
	sds_real	pdsize;
	short		pellipse;
	sds_real	perimeter;
	short		pickstyle;
	short		plinegen;
	sds_real	plinewid;
	short		polysides;
	short		proxygraphics;
	short		psltscale;
	sds_real	p_elevation;
	sds_point	p_extmax;
	sds_point	p_extmin;
	sds_point	p_insbase;
	short		p_limcheck;
	sds_real	p_limmax[2];
	sds_real	p_limmin[2];
	db_handitem *	p_ucsname;
	sds_point	p_ucsorg;
	sds_point	p_ucsxdir;
	sds_point	p_ucsydir;
	short		qtextmode;
	short		regenmode;
	short		saveimages;
	char *		savename;
	short		shadedge;
	short		shadedif;
	char *		shpname;
	sds_real	sketchinc;
	short		skpoly;
	sds_real	snapang;
	sds_real	snapbase[2];
	short		snapisopair;
	short		snapmode;
	short		snapstyl;
	sds_real	snapunit[2];
	short		splframe;
	short		splinesegs;
	short		splinetype;
	short		surftab1;
	short		surftab2;
	short		surftype;
	short		surfu;
	short		surfv;
	sds_point	target;
	sds_real	tdcreate;
	sds_real	tdindwg;
	sds_real	tdupdate;
	sds_real	tdusrtimer;
	sds_real	textangle;
	short		textqlty;
	sds_real	textsize;
	db_handitem *	textstyle;
	sds_real	thickness;
	short		tilemode;
	sds_real	tracewid;
	short		treedepth;
	short		trimmode;
	short		ucsfollow;
	short		ucsicon;
	db_handitem *	ucsname;
	sds_point	ucsorg;
	sds_point	ucsxdir;
	sds_point	ucsydir;
	short		undoctl;
	short		undomarks;
	short		unitmode;
	short		useri1;
	short		useri2;
	short		useri3;
	short		useri4;
	short		useri5;
	sds_real	userr1;
	sds_real	userr2;
	sds_real	userr3;
	sds_real	userr4;
	sds_real	userr5;
	char *		users1;
	char *		users2;
	char *		users3;
	char *		users4;
	char *		users5;
	short		usrtimer;
	sds_real	viewaspect;
	sds_point	viewctr;
	sds_point	viewdir;
	short		viewmode;
	sds_real	viewsize;
	sds_real	viewtwist;
	short		visretain;
	sds_point	vsmax;
	sds_point	vsmin;
	short		worlducs;
	short		worldview;
	short		writestat;
	short		zoompercent;

public:

	short		RetAcisoutver(db_drawing *dp);
	short		SetAcisoutver(db_drawing *,short);
	short		RetAcisoutverDefault(void) { return 120; }

	short		RetAflags(db_drawing *dp);
	short		SetAflags(db_drawing *,short);
	short		RetAflagsDefault(void) { return 0; }

	sds_real	RetAngbase(db_drawing *dp);
	short		SetAngbase(db_drawing *,sds_real);
	sds_real	RetAngbaseDefault(void) { return 0; }

	short		RetAngdir(db_drawing *dp);
	short		SetAngdir(db_drawing *,short);
	short		RetAngdirDefault(void) { return 0; }

	sds_real	RetArea(db_drawing *dp);
	short		SetArea(db_drawing *,sds_real);
	sds_real	RetAreaDefault(void) { return 0; }

	short		RetAttmode(db_drawing *dp);
	short		SetAttmode(db_drawing *,short);
	short		RetAttmodeDefault(void) { return 1; }

	short		RetAttreq(db_drawing *dp);
	short		SetAttreq(db_drawing *,short);
	short		RetAttreqDefault(void) { return 1; }

	short		RetAunits(db_drawing *dp);
	short		SetAunits(db_drawing *,short);
	short		RetAunitsDefault(void) { return 0; }

	short		RetAuprec(db_drawing *dp);
	short		SetAuprec(db_drawing *,short);
	short		RetAuprecDefault(void) { return 0; }

	short		RetAxismode(db_drawing *dp);
	short		SetAxismode(db_drawing *,short);
	short		RetAxismodeDefault(void) { return 0; }

	void		GetAxisunit(db_drawing *, sds_point);
	short		SetAxisunit(db_drawing *,sds_point);
	void		GetAxisunitDefault(sds_point);

	sds_real	RetBackz(db_drawing *dp);
	short		SetBackz(db_drawing *,sds_real);
	sds_real	RetBackzDefault(void) { return 0; }

	short		RetBlipmode(db_drawing *dp);
	short		SetBlipmode(db_drawing *,short);
	short		RetBlipmodeDefault(void) { return 0; }

	short		RetCecolor(db_drawing *dp);
	short		SetCecolor(db_drawing *,short);
	short		RetCecolorDefault(void) { return 255; }

	sds_real	RetCeltscale(db_drawing *dp);
	short		SetCeltscale(db_drawing *,sds_real);
	sds_real	RetCeltscaleDefault(void) { return 1; }

	db_handitem *	RetCeltype(db_drawing *dp);
	short		SetCeltype(db_drawing *,db_handitem *);

	sds_real	RetChamfera(db_drawing *dp);
	short		SetChamfera(db_drawing *,sds_real);
	sds_real	RetChamferaDefault(void) { return 0; }

	sds_real	RetChamferb(db_drawing *dp);
	short		SetChamferb(db_drawing *,sds_real);
	sds_real	RetChamferbDefault(void) { return 0; }

	sds_real	RetChamferc(db_drawing *dp);
	short		SetChamferc(db_drawing *,sds_real);
	sds_real	RetChamfercDefault(void) { return 0; }

	sds_real	RetChamferd(db_drawing *dp);
	short		SetChamferd(db_drawing *,sds_real);
	sds_real	RetChamferdDefault(void) { return 0; }

	short		RetChammode(db_drawing *dp);
	short		SetChammode(db_drawing *,short);
	short		RetChammodeDefault(void) { return 0; }

	sds_real	RetCirclerad(db_drawing *dp);
	short		SetCirclerad(db_drawing *,sds_real);
	sds_real	RetCircleradDefault(void) { return 0; }

	db_handitem *	RetClayer(db_drawing *dp);
	short		SetClayer(db_drawing *,db_handitem *);

	short		RetCmdactive(db_drawing *dp);
	short		SetCmdactive(db_drawing *,short);
	short		RetCmdactiveDefault(void) { return 0; }

	short		RetCmdecho(db_drawing *dp);
	short		SetCmdecho(db_drawing *,short);
	short		RetCmdechoDefault(void) { return 1; }

	char *		RetCmdnames(db_drawing *dp);
	short		SetCmdnames(db_drawing *,char *);

	short		RetCmljust(db_drawing *dp);
	short		SetCmljust(db_drawing *,short);
	short		RetCmljustDefault(void) { return 0; }

	sds_real	RetCmlscale(db_drawing *dp);
	short		SetCmlscale(db_drawing *,sds_real);
	sds_real	RetCmlscaleDefault(void) { return 1; }

	db_handitem *	RetCmlstyle(db_drawing *dp);
	short		SetCmlstyle(db_drawing *,db_handitem *);

	short		RetCoords(db_drawing *dp);
	short		SetCoords(db_drawing *,short);
	short		RetCoordsDefault(void) { return 1; }

	short		RetCvport(db_drawing *dp);
	short		SetCvport(db_drawing *,short);
	short		RetCvportDefault(void) { return 2; }

	short		RetCyclecurr(db_drawing *dp);
	short		SetCyclecurr(db_drawing *,short);
	short		RetCyclecurrDefault(void) { return 5; }

	short		RetCyclekeep(db_drawing *dp);
	short		SetCyclekeep(db_drawing *,short);
	short		RetCyclekeepDefault(void) { return 1; }

	short		RetDbmod(db_drawing *dp);
	short		SetDbmod(db_drawing *,short);
	short		RetDbmodDefault(void) { return 0; }

	short		RetDelobj(db_drawing *dp);
	short		SetDelobj(db_drawing *,short);
	short		RetDelobjDefault(void) { return 1; }

	short		RetDiastat(db_drawing *dp);
	short		SetDiastat(db_drawing *,short);
	short		RetDiastatDefault(void) { return 1; }

	// EBATECH(CNBR) -[ Bugzilla#78443
	short		RetDimadec(db_drawing *dp);
	short		SetDimadec(db_drawing *,short);
	short		RetDimadecDefault(void) { return -1; }
	// EBATECH(CNBR) ]-

	short		RetDimalt(db_drawing *dp);
	short		SetDimalt(db_drawing *,short);
	short		RetDimaltDefault(void) { return 0; }

	short		RetDimaltd(db_drawing *dp);
	short		SetDimaltd(db_drawing *,short);
	short		RetDimaltdDefault(void) { return 2; }

	sds_real	RetDimaltf(db_drawing *dp);
	short		SetDimaltf(db_drawing *,sds_real);
	sds_real	RetDimaltfDefault(void) { return 25.4; }

	short		RetDimalttd(db_drawing *dp);
	short		SetDimalttd(db_drawing *,short);
	short		RetDimalttdDefault(void) { return 2; }

	short		RetDimalttz(db_drawing *dp);
	short		SetDimalttz(db_drawing *,short);
	short		RetDimalttzDefault(void) { return 0; }

	short		RetDimaltu(db_drawing *dp);
	short		SetDimaltu(db_drawing *,short);
	short		RetDimaltuDefault(void) { return 2; }

	short		RetDimaltz(db_drawing *dp);
	short		SetDimaltz(db_drawing *,short);
	short		RetDimaltzDefault(void) { return 0; }

	char *		RetDimapost(db_drawing *dp);
	short		SetDimapost(db_drawing *,char *);

	short		RetDimaso(db_drawing *dp);
	short		SetDimaso(db_drawing *,short);
	short		RetDimasoDefault(void) { return 1; }

	sds_real	RetDimasz(db_drawing *dp);
	short		SetDimasz(db_drawing *,sds_real);
	sds_real	RetDimaszDefault(void) { return 0.18; }

	short		RetDimaunit(db_drawing *dp);
	short		SetDimaunit(db_drawing *,short);
	short		RetDimaunitDefault(void) { return 0; }

	// EBATECH(CNBR) -[ Bugzilla#78443
	short		RetDimazin(db_drawing *dp);
	short		SetDimazin(db_drawing *,short);
	short		RetDimazinDefault(void) { return 0; }
	// EBATECH(CNBR) ]-

	char *		RetDimblk(db_drawing *dp);
	short		SetDimblk(db_drawing *,char *);

	char *		RetDimblk1(db_drawing *dp);
	short		SetDimblk1(db_drawing *,char *);

	char *		RetDimblk2(db_drawing *dp);
	short		SetDimblk2(db_drawing *,char *);

	// EBATECH(watanabe)-[dimldrblk
	char *		RetDimldrblk(db_drawing *dp);
	short		SetDimldrblk(db_drawing *,char *);
	// ]-EBATECH(watanabe)

	sds_real	RetDimcen(db_drawing *dp);
	short		SetDimcen(db_drawing *,sds_real);
	sds_real	RetDimcenDefault(void) { return 0.09; }

	short		RetDimclrd(db_drawing *dp);
	short		SetDimclrd(db_drawing *,short);
	short		RetDimclrdDefault(void) { return 0; }

	short		RetDimclre(db_drawing *dp);
	short		SetDimclre(db_drawing *,short);
	short		RetDimclreDefault(void) { return 0; }

	short		RetDimclrt(db_drawing *dp);
	short		SetDimclrt(db_drawing *,short);
	short		RetDimclrtDefault(void) { return 0; }

	short		RetDimdec(db_drawing *dp);
	short		SetDimdec(db_drawing *,short);
	short		RetDimdecDefault(void) { return 4; }

	sds_real	RetDimdle(db_drawing *dp);
	short		SetDimdle(db_drawing *,sds_real);
	sds_real	RetDimdleDefault(void) { return 0; }

	sds_real	RetDimdli(db_drawing *dp);
	short		SetDimdli(db_drawing *,sds_real);
	sds_real	RetDimdliDefault(void) { return 0.38; }

	sds_real	RetDimexe(db_drawing *dp);
	short		SetDimexe(db_drawing *,sds_real);
	sds_real	RetDimexeDefault(void) { return 0.18; }

	sds_real	RetDimexo(db_drawing *dp);
	short		SetDimexo(db_drawing *,sds_real);
	sds_real	RetDimexoDefault(void) { return 0.0625; }

	short		RetDimfit(db_drawing *dp);
	short		SetDimfit(db_drawing *,short);
	short		RetDimfitDefault(void) { return 3; }

	sds_real	RetDimgap(db_drawing *dp);
	short		SetDimgap(db_drawing *,sds_real);
	sds_real	RetDimgapDefault(void) { return 0.09; }

	short		RetDimjust(db_drawing *dp);
	short		SetDimjust(db_drawing *,short);
	short		RetDimjustDefault(void) { return 0; }

	sds_real	RetDimlfac(db_drawing *dp);
	short		SetDimlfac(db_drawing *,sds_real);
	sds_real	RetDimlfacDefault(void) { return 1; }

	short		RetDimlim(db_drawing *dp);
	short		SetDimlim(db_drawing *,short);
	short		RetDimlimDefault(void) { return 0; }

	char *		RetDimpost(db_drawing *dp);
	short		SetDimpost(db_drawing *,char *);

	sds_real	RetDimrnd(db_drawing *dp);
	short		SetDimrnd(db_drawing *,sds_real);
	sds_real	RetDimrndDefault(void) { return 0; }

	short		RetDimsah(db_drawing *dp);
	short		SetDimsah(db_drawing *,short);
	short		RetDimsahDefault(void) { return 0; }

	sds_real	RetDimscale(db_drawing *dp);
	short		SetDimscale(db_drawing *,sds_real);
	sds_real	RetDimscaleDefault(void) { return 1; }

	short		RetDimsd1(db_drawing *dp);
	short		SetDimsd1(db_drawing *,short);
	short		RetDimsd1Default(void) { return 0; }

	short		RetDimsd2(db_drawing *dp);
	short		SetDimsd2(db_drawing *,short);
	short		RetDimsd2Default(void) { return 0; }

	short		RetDimse1(db_drawing *dp);
	short		SetDimse1(db_drawing *,short);
	short		RetDimse1Default(void) { return 0; }

	short		RetDimse2(db_drawing *dp);
	short		SetDimse2(db_drawing *,short);
	short		RetDimse2Default(void) { return 0; }

	short		RetDimsho(db_drawing *dp);
	short		SetDimsho(db_drawing *,short);
	short		RetDimshoDefault(void) { return 1; }

	short		RetDimsoxd(db_drawing *dp);
	short		SetDimsoxd(db_drawing *,short);
	short		RetDimsoxdDefault(void) { return 0; }

	db_handitem *	RetDimstyle(db_drawing *dp);
	short		SetDimstyle(db_drawing *,db_handitem *);

	short		RetDimtad(db_drawing *dp);
	short		SetDimtad(db_drawing *,short);
	short		RetDimtadDefault(void) { return 0; }

	short		RetDimtdec(db_drawing *dp);
	short		SetDimtdec(db_drawing *,short);
	short		RetDimtdecDefault(void) { return 4; }

	sds_real	RetDimtfac(db_drawing *dp);
	short		SetDimtfac(db_drawing *,sds_real);
	sds_real	RetDimtfacDefault(void) { return 1; }

	short		RetDimtih(db_drawing *dp);
	short		SetDimtih(db_drawing *,short);
	short		RetDimtihDefault(void) { return 1; }

	short		RetDimtix(db_drawing *dp);
	short		SetDimtix(db_drawing *,short);
	short		RetDimtixDefault(void) { return 0; }

	sds_real	RetDimtm(db_drawing *dp);
	short		SetDimtm(db_drawing *,sds_real);
	sds_real	RetDimtmDefault(void) { return 0; }

	short		RetDimtofl(db_drawing *dp);
	short		SetDimtofl(db_drawing *,short);
	short		RetDimtoflDefault(void) { return 0; }

	short		RetDimtoh(db_drawing *dp);
	short		SetDimtoh(db_drawing *,short);
	short		RetDimtohDefault(void) { return 1; }

	short		RetDimtol(db_drawing *dp);
	short		SetDimtol(db_drawing *,short);
	short		RetDimtolDefault(void) { return 0; }

	short		RetDimtolj(db_drawing *dp);
	short		SetDimtolj(db_drawing *,short);
	short		RetDimtoljDefault(void) { return 1; }

	sds_real	RetDimtp(db_drawing *dp);
	short		SetDimtp(db_drawing *,sds_real);
	sds_real	RetDimtpDefault(void) { return 0; }

	sds_real	RetDimtsz(db_drawing *dp);
	short		SetDimtsz(db_drawing *,sds_real);
	sds_real	RetDimtszDefault(void) { return 0; }

	sds_real	RetDimtvp(db_drawing *dp);
	short		SetDimtvp(db_drawing *,sds_real);
	sds_real	RetDimtvpDefault(void) { return 0; }

	db_handitem *	RetDimtxsty(db_drawing *dp);
	short		SetDimtxsty(db_drawing *,db_handitem *);

	sds_real	RetDimtxt(db_drawing *dp);
	short		SetDimtxt(db_drawing *,sds_real);
	sds_real	RetDimtxtDefault(void) { return 0.18; }

	short		RetDimtzin(db_drawing *dp);
	short		SetDimtzin(db_drawing *,short);
	short		RetDimtzinDefault(void) { return 0; }

	short		RetDimunit(db_drawing *dp);
	short		SetDimunit(db_drawing *,short);
	short		RetDimunitDefault(void) { return 2; }

	short		RetDimupt(db_drawing *dp);
	short		SetDimupt(db_drawing *,short);
	short		RetDimuptDefault(void) { return 0; }

	short		RetDimzin(db_drawing *dp);
	short		SetDimzin(db_drawing *,short);
	short		RetDimzinDefault(void) { return 0; }

	short		RetDispsilh(db_drawing *dp);
	short		SetDispsilh(db_drawing *,short);
	short		RetDispsilhDefault(void) { return 0; }

	sds_real	RetDistance(db_drawing *dp);
	short		SetDistance(db_drawing *,sds_real);
	sds_real	RetDistanceDefault(void) { return 0; }

	sds_real	RetDonutid(db_drawing *dp);
	short		SetDonutid(db_drawing *,sds_real);
	sds_real	RetDonutidDefault(void) { return 0.5; }

	sds_real	RetDonutod(db_drawing *dp);
	short		SetDonutod(db_drawing *,sds_real);
	sds_real	RetDonutodDefault(void) { return 1; }

	short		RetDragmode(db_drawing *dp);
	short		SetDragmode(db_drawing *,short);
	short		RetDragmodeDefault(void) { return 2; }

	short		RetDwgcodepage(db_drawing *dp);
	short		SetDwgcodepage(db_drawing *,short);
// EBATECH(CNBR) -[ Set Default DWGCODEPAGE from ActiveCodepage
	//short		RetDwgcodepageDefault(void) { return 30; }
	short		RetDwgcodepageDefault(void)
	{
		short code;
		char samp[32];
		sprintf(samp, "ANSI_%d", GetACP());
		if(!adAsciiCodePageToShort(samp,&code))
			code = 30;
		return code;
	}
// EBATECH(CNBR) ]-

	short		RetDwgtitled(db_drawing *dp);
	short		SetDwgtitled(db_drawing *,short);
	short		RetDwgtitledDefault(void) { return 1; }

	short		RetEdgemode(db_drawing *dp);
	short		SetEdgemode(db_drawing *,short);
	short		RetEdgemodeDefault(void) { return 0; }

	sds_real	RetElevation(db_drawing *dp);
	short		SetElevation(db_drawing *,sds_real);
	sds_real	RetElevationDefault(void) { return 0; }

	short		RetExplmode(db_drawing *dp);
	short		SetExplmode(db_drawing *,short);
	short		RetExplmodeDefault(void) { return 1; }

	void		GetExtmax(db_drawing *, sds_point);
	short		SetExtmax(db_drawing *,sds_point);
	void		GetExtmaxDefault(sds_point);

	void		GetExtmin(db_drawing *, sds_point);
	short		SetExtmin(db_drawing *,sds_point);
	void		GetExtminDefault(sds_point);

	sds_real	RetFacetres(db_drawing *dp);
	short		SetFacetres(db_drawing *,sds_real);
	sds_real	RetFacetresDefault(void) { return 0.5; }

	short		RetFastzoom(db_drawing *dp);
	short		SetFastzoom(db_drawing *,short);
	short		RetFastzoomDefault(void) { return 1; }

	sds_real	RetFilletrad(db_drawing *dp);
	short		SetFilletrad(db_drawing *,sds_real);
	sds_real	RetFilletradDefault(void) { return 0; }

	short		RetFillmode(db_drawing *dp);
	short		SetFillmode(db_drawing *,short);
	short		RetFillmodeDefault(void) { return 1; }

	short		RetFittype(db_drawing *dp);
	short		SetFittype(db_drawing *,short);
	short		RetFittypeDefault(void) { return 2; }

	short		RetFlatland(db_drawing *dp);
	short		SetFlatland(db_drawing *,short);
	short		RetFlatlandDefault(void) { return 0; }

	sds_real	RetFrontz(db_drawing *dp);
	short		SetFrontz(db_drawing *,sds_real);
	sds_real	RetFrontzDefault(void) { return 0; }

	void		GetGpslastpt(db_drawing *, sds_point);
	short		SetGpslastpt(db_drawing *,sds_point);
	void		GetGpslastptDefault(sds_point);

	short		RetGpsmode(db_drawing *dp);
	short		SetGpsmode(db_drawing *,short);
	short		RetGpsmodeDefault(void) { return 0; }

	short		RetGpspaused(db_drawing *dp);
	short		SetGpspaused(db_drawing *,short);
	short		RetGpspausedDefault(void) { return 1; }

	char *		RetGpsplayed(db_drawing *dp);
	short		SetGpsplayed(db_drawing *,char *);

	short		RetGridmode(db_drawing *dp);
	short		SetGridmode(db_drawing *,short);
	short		RetGridmodeDefault(void) { return 0; }

	void		GetGridunit(db_drawing *, sds_point);
	short		SetGridunit(db_drawing *,sds_point);
	void		GetGridunitDefault(sds_point);

	short		RetHandles(db_drawing *dp);
	short		SetHandles(db_drawing *,short);
	short		RetHandlesDefault(void) { return 1; }

	db_objhandle	RetHandseed(db_drawing *dp);
	short		SetHandseed(db_drawing *,db_objhandle);
	void		GetHandseedDefault(sds_point);

	short		RetHighlight(db_drawing *dp);
	short		SetHighlight(db_drawing *,short);
	short		RetHighlightDefault(void) { return 1; }

	sds_real	RetHpang(db_drawing *dp);
	short		SetHpang(db_drawing *,sds_real);
	sds_real	RetHpangDefault(void) { return 0; }

	short		RetHpbound(db_drawing *dp);
	short		SetHpbound(db_drawing *,short);
	short		RetHpboundDefault(void) { return 1; }

	short		RetHpdouble(db_drawing *dp);
	short		SetHpdouble(db_drawing *,short);
	short		RetHpdoubleDefault(void) { return 0; }

	char *		RetHpname(db_drawing *dp);
	short		SetHpname(db_drawing *,char *);

	sds_real	RetHpscale(db_drawing *dp);
	short		SetHpscale(db_drawing *,sds_real);
	sds_real	RetHpscaleDefault(void) { return 1; }

	sds_real	RetHpspace(db_drawing *dp);
	short		SetHpspace(db_drawing *,sds_real);
	sds_real	RetHpspaceDefault(void) { return 1; }

	short		RetHpstyle(db_drawing *dp);
	short		SetHpstyle(db_drawing *,short);
	short		RetHpstyleDefault(void) { return 0; }

	void		GetInsbase(db_drawing *, sds_point);
	short		SetInsbase(db_drawing *,sds_point);
	void		GetInsbaseDefault(sds_point);

	char *		RetInsname(db_drawing *dp);
	short		SetInsname(db_drawing *,char *);

	short		RetIslastcmdpt(db_drawing *dp);
	short		SetIslastcmdpt(db_drawing *,short);
	short		RetIslastcmdptDefault(void) { return 0; }

	short		RetIsolines(db_drawing *dp);
	short		SetIsolines(db_drawing *,short);
	short		RetIsolinesDefault(void) { return 4; }

	sds_real	RetLastangle(db_drawing *dp);
	short		SetLastangle(db_drawing *,sds_real);
	sds_real	RetLastangleDefault(void) { return 0; }

	sds_real	RetLastcmdang(db_drawing *dp);
	short		SetLastcmdang(db_drawing *,sds_real);
	sds_real	RetLastcmdangDefault(void) { return 0.0; }

	void		GetLastcmdpt(db_drawing *, sds_point);
	short		SetLastcmdpt(db_drawing *,sds_point);
	void		GetLastcmdptDefault(sds_point);

	void		GetLastpoint(db_drawing *, sds_point);
	short		SetLastpoint(db_drawing *,sds_point);
	void		GetLastpointDefault(sds_point);

	sds_real	RetLenslength(db_drawing *dp);
	short		SetLenslength(db_drawing *,sds_real);
	sds_real	RetLenslengthDefault(void) { return 50; }

	short		RetLimcheck(db_drawing *dp);
	short		SetLimcheck(db_drawing *,short);
	short		RetLimcheckDefault(void) { return 0; }

	void		GetLimmax(db_drawing *, sds_point);
	short		SetLimmax(db_drawing *,sds_point);
	void		GetLimmaxDefault(sds_point);

	void		GetLimmin(db_drawing *, sds_point);
	short		SetLimmin(db_drawing *,sds_point);
	void		GetLimminDefault(sds_point);

	sds_real	RetLtscale(db_drawing *dp);
	short		SetLtscale(db_drawing *,sds_real);
	sds_real	RetLtscaleDefault(void) { return 1; }

	short		RetLtsclwblk(db_drawing *dp);
	short		SetLtsclwblk(db_drawing *,short);
	short		RetLtsclwblkDefault(void) { return 0; }

	short		RetLunits(db_drawing *dp);
	short		SetLunits(db_drawing *,short);
	short		RetLunitsDefault(void) { return 2; }

	short		RetLuprec(db_drawing *dp);
	short		SetLuprec(db_drawing *,short);
	short		RetLuprecDefault(void) { return 4; }

	short		RetMaxactvp(db_drawing *dp);
	short		SetMaxactvp(db_drawing *,short);
	short		RetMaxactvpDefault(void) { return 64; } // Old value was 16.

	short		RetMeasurement(db_drawing *dp);
	short		SetMeasurement(db_drawing *,short);
	short		RetMeasurementDefault(void) { return 0; }

	short		RetMenuecho(db_drawing *dp);
	short		SetMenuecho(db_drawing *,short);
	short		RetMenuechoDefault(void) { return 0; }

	char *		RetMenuname(db_drawing *dp);
	short		SetMenuname(db_drawing *,char *);
	char *		RetMenunameDefault(void) { return "icad"; }

	short		RetMirrtext(db_drawing *dp);
	short		SetMirrtext(db_drawing *,short);
	short		RetMirrtextDefault(void) { return 1; }

	sds_real	RetOffsetdist(db_drawing *dp);
	short		SetOffsetdist(db_drawing *,sds_real);
	sds_real	RetOffsetdistDefault(void) { return -1; }

	short		RetOrthomode(db_drawing *dp);
	short		SetOrthomode(db_drawing *,short);
	short		RetOrthomodeDefault(void) { return 0; }

	short		RetOsmode(db_drawing *dp);
	short		SetOsmode(db_drawing *,short);
	short		RetOsmodeDefault(void) { return 0; }

	short		RetPdmode(db_drawing *dp);
	short		SetPdmode(db_drawing *,short);
	short		RetPdmodeDefault(void) { return 0; }

	sds_real	RetPdsize(db_drawing *dp);
	short		SetPdsize(db_drawing *,sds_real);
	sds_real	RetPdsizeDefault(void) { return 0; }

	short		RetPellipse(db_drawing *dp);
	short		SetPellipse(db_drawing *,short);
	short		RetPellipseDefault(void) { return 0; }

	sds_real	RetPerimeter(db_drawing *dp);
	short		SetPerimeter(db_drawing *,sds_real);
	sds_real	RetPerimeterDefault(void) { return 0; }

	short		RetPickstyle(db_drawing *dp);
	short		SetPickstyle(db_drawing *,short);
	short		RetPickstyleDefault(void) { return 1; }

	short		RetPlinegen(db_drawing *dp);
	short		SetPlinegen(db_drawing *,short);
	short		RetPlinegenDefault(void) { return 0; }

	sds_real	RetPlinewid(db_drawing *dp);
	short		SetPlinewid(db_drawing *,sds_real);
	sds_real	RetPlinewidDefault(void) { return 0; }

	short		RetPolysides(db_drawing *dp);
	short		SetPolysides(db_drawing *,short);
	short		RetPolysidesDefault(void) { return 4; }

	short		RetProxygraphics(db_drawing *dp);
	short		SetProxygraphics(db_drawing *,short);
	short		RetProxygraphicsDefault(void) { return 1; }

	short		RetPsltscale(db_drawing *dp);
	short		SetPsltscale(db_drawing *,short);
	short		RetPsltscaleDefault(void) { return 1; }

	sds_real	RetP_elevation(db_drawing *dp);
	short		SetP_elevation(db_drawing *,sds_real);
	sds_real	RetP_elevationDefault(void) { return 0; }

	void		GetP_extmax(db_drawing *, sds_point);
	short		SetP_extmax(db_drawing *,sds_point);
	void		GetP_extmaxDefault(sds_point);

	void		GetP_extmin(db_drawing *, sds_point);
	short		SetP_extmin(db_drawing *,sds_point);
	void		GetP_extminDefault(sds_point);

	void		GetP_insbase(db_drawing *, sds_point);
	short		SetP_insbase(db_drawing *,sds_point);
	void		GetP_insbaseDefault(sds_point);

	short		RetP_limcheck(db_drawing *dp);
	short		SetP_limcheck(db_drawing *,short);
	short		RetP_limcheckDefault(void) { return 0; }

	void		GetP_limmax(db_drawing *, sds_point);
	short		SetP_limmax(db_drawing *,sds_point);
	void		GetP_limmaxDefault(sds_point);

	void		GetP_limmin(db_drawing *, sds_point);
	short		SetP_limmin(db_drawing *,sds_point);
	void		GetP_limminDefault(sds_point);

	db_handitem *	RetP_ucsname(db_drawing *dp);
	short		SetP_ucsname(db_drawing *,db_handitem *);

	void		GetP_ucsorg(db_drawing *, sds_point);
	short		SetP_ucsorg(db_drawing *,sds_point);
	void		GetP_ucsorgDefault(sds_point);

	void		GetP_ucsxdir(db_drawing *, sds_point);
	short		SetP_ucsxdir(db_drawing *,sds_point);
	void		GetP_ucsxdirDefault(sds_point);

	void		GetP_ucsydir(db_drawing *, sds_point);
	short		SetP_ucsydir(db_drawing *,sds_point);
	void		GetP_ucsydirDefault(sds_point);

	short		RetQtextmode(db_drawing *dp);
	short		SetQtextmode(db_drawing *,short);
	short		RetQtextmodeDefault(void) { return 0; }

	short		RetRegenmode(db_drawing *dp);
	short		SetRegenmode(db_drawing *,short);
	short		RetRegenmodeDefault(void) { return 1; }

	short		RetSaveimages(db_drawing *dp);
	short		SetSaveimages(db_drawing *,short);
	short		RetSaveimagesDefault(void) { return 0; }

	char *		RetSavename(db_drawing *dp);
	short		SetSavename(db_drawing *,char *);

	short		RetShadedge(db_drawing *dp);
	short		SetShadedge(db_drawing *,short);
	short		RetShadedgeDefault(void) { return 3; }

	short		RetShadedif(db_drawing *dp);
	short		SetShadedif(db_drawing *,short);
	short		RetShadedifDefault(void) { return 70; }

	char *		RetShpname(db_drawing *dp);
	short		SetShpname(db_drawing *,char *);

	sds_real	RetSketchinc(db_drawing *dp);
	short		SetSketchinc(db_drawing *,sds_real);
	sds_real	RetSketchincDefault(void) { return 0.1; }

	short		RetSkpoly(db_drawing *dp);
	short		SetSkpoly(db_drawing *,short);
	short		RetSkpolyDefault(void) { return 0; }

	sds_real	RetSnapang(db_drawing *dp);
	short		SetSnapang(db_drawing *,sds_real);
	sds_real	RetSnapangDefault(void) { return 0; }

	void		GetSnapbase(db_drawing *, sds_point);
	short		SetSnapbase(db_drawing *,sds_point);
	void		GetSnapbaseDefault(sds_point);

	short		RetSnapisopair(db_drawing *dp);
	short		SetSnapisopair(db_drawing *,short);
	short		RetSnapisopairDefault(void) { return 0; }

	short		RetSnapmode(db_drawing *dp);
	short		SetSnapmode(db_drawing *,short);
	short		RetSnapmodeDefault(void) { return 0; }

	short		RetSnapstyl(db_drawing *dp);
	short		SetSnapstyl(db_drawing *,short);
	short		RetSnapstylDefault(void) { return 0; }

	void		GetSnapunit(db_drawing *, sds_point);
	short		SetSnapunit(db_drawing *,sds_point);
	void		GetSnapunitDefault(sds_point);

	short		RetSplframe(db_drawing *dp);
	short		SetSplframe(db_drawing *,short);
	short		RetSplframeDefault(void) { return 0; }

	short		RetSplinesegs(db_drawing *dp);
	short		SetSplinesegs(db_drawing *,short);
	short		RetSplinesegsDefault(void) { return 8; }

	short		RetSplinetype(db_drawing *dp);
	short		SetSplinetype(db_drawing *,short);
	short		RetSplinetypeDefault(void) { return 5; }

	short		RetSurftab1(db_drawing *dp);
	short		SetSurftab1(db_drawing *,short);
	short		RetSurftab1Default(void) { return 6; }

	short		RetSurftab2(db_drawing *dp);
	short		SetSurftab2(db_drawing *,short);
	short		RetSurftab2Default(void) { return 6; }

	short		RetSurftype(db_drawing *dp);
	short		SetSurftype(db_drawing *,short);
	short		RetSurftypeDefault(void) { return 6; }

	short		RetSurfu(db_drawing *dp);
	short		SetSurfu(db_drawing *,short);
	short		RetSurfuDefault(void) { return 6; }

	short		RetSurfv(db_drawing *dp);
	short		SetSurfv(db_drawing *,short);
	short		RetSurfvDefault(void) { return 6; }

	void		GetTarget(db_drawing *, sds_point);
	short		SetTarget(db_drawing *,sds_point);
	void		GetTargetDefault(sds_point);

	sds_real	RetTdcreate(db_drawing *dp);
	short		SetTdcreate(db_drawing *,sds_real);
	sds_real	RetTdcreateDefault(void) { return 2444240; }

	sds_real	RetTdindwg(db_drawing *dp);
	short		SetTdindwg(db_drawing *,sds_real);
	sds_real	RetTdindwgDefault(void) { return 0; }

	sds_real	RetTdupdate(db_drawing *dp);
	short		SetTdupdate(db_drawing *,sds_real);
	sds_real	RetTdupdateDefault(void) { return 2444240; }

	sds_real	RetTdusrtimer(db_drawing *dp);
	short		SetTdusrtimer(db_drawing *,sds_real);
	sds_real	RetTdusrtimerDefault(void) { return 0; }

	sds_real	RetTextangle(db_drawing *dp);
	short		SetTextangle(db_drawing *,sds_real);
	sds_real	RetTextangleDefault(void) { return 0.0; }

	short		RetTextqlty(db_drawing *dp);
	short		SetTextqlty(db_drawing *,short);
	short		RetTextqltyDefault(void) { return 50; }

	sds_real	RetTextsize(db_drawing *dp);
	short		SetTextsize(db_drawing *,sds_real);
	sds_real	RetTextsizeDefault(void) { return 0.2; }

	db_handitem *	RetTextstyle(db_drawing *dp);
	short		SetTextstyle(db_drawing *,db_handitem *);

	sds_real	RetThickness(db_drawing *dp);
	short		SetThickness(db_drawing *,sds_real);
	sds_real	RetThicknessDefault(void) { return 0; }

	short		RetTilemode(db_drawing *dp);
	short		SetTilemode(db_drawing *,short);
	short		RetTilemodeDefault(void) { return 1; }

	sds_real	RetTracewid(db_drawing *dp);
	short		SetTracewid(db_drawing *,sds_real);
	sds_real	RetTracewidDefault(void) { return 0.05; }

	short		RetTreedepth(db_drawing *dp);
	short		SetTreedepth(db_drawing *,short);
	short		RetTreedepthDefault(void) { return 3020; }

	short		RetTrimmode(db_drawing *dp);
	short		SetTrimmode(db_drawing *,short);
	short		RetTrimmodeDefault(void) { return 1; }

	short		RetUcsfollow(db_drawing *dp);
	short		SetUcsfollow(db_drawing *,short);
	short		RetUcsfollowDefault(void) { return 0; }

	short		RetUcsicon(db_drawing *dp);
	short		SetUcsicon(db_drawing *,short);
	short		RetUcsiconDefault(void) { return 1; }

	db_handitem *	RetUcsname(db_drawing *dp);
	short		SetUcsname(db_drawing *,db_handitem *);

	void		GetUcsorg(db_drawing *, sds_point);
	short		SetUcsorg(db_drawing *,sds_point);
	void		GetUcsorgDefault(sds_point);

	void		GetUcsxdir(db_drawing *, sds_point);
	short		SetUcsxdir(db_drawing *,sds_point);
	void		GetUcsxdirDefault(sds_point);

	void		GetUcsydir(db_drawing *, sds_point);
	short		SetUcsydir(db_drawing *,sds_point);
	void		GetUcsydirDefault(sds_point);

	short		RetUndoctl(db_drawing *dp);
	short		SetUndoctl(db_drawing *,short);
	short		RetUndoctlDefault(void) { return 5; }

	short		RetUndomarks(db_drawing *dp);
	short		SetUndomarks(db_drawing *,short);
	short		RetUndomarksDefault(void) { return 0; }

	short		RetUnitmode(db_drawing *dp);
	short		SetUnitmode(db_drawing *,short);
	short		RetUnitmodeDefault(void) { return 0; }

	short		RetUseri1(db_drawing *dp);
	short		SetUseri1(db_drawing *,short);
	short		RetUseri1Default(void) { return 0; }

	short		RetUseri2(db_drawing *dp);
	short		SetUseri2(db_drawing *,short);
	short		RetUseri2Default(void) { return 0; }

	short		RetUseri3(db_drawing *dp);
	short		SetUseri3(db_drawing *,short);
	short		RetUseri3Default(void) { return 0; }

	short		RetUseri4(db_drawing *dp);
	short		SetUseri4(db_drawing *,short);
	short		RetUseri4Default(void) { return 0; }

	short		RetUseri5(db_drawing *dp);
	short		SetUseri5(db_drawing *,short);
	short		RetUseri5Default(void) { return 0; }

	sds_real	RetUserr1(db_drawing *dp);
	short		SetUserr1(db_drawing *,sds_real);
	sds_real	RetUserr1Default(void) { return 0; }

	sds_real	RetUserr2(db_drawing *dp);
	short		SetUserr2(db_drawing *,sds_real);
	sds_real	RetUserr2Default(void) { return 0; }

	sds_real	RetUserr3(db_drawing *dp);
	short		SetUserr3(db_drawing *,sds_real);
	sds_real	RetUserr3Default(void) { return 0; }

	sds_real	RetUserr4(db_drawing *dp);
	short		SetUserr4(db_drawing *,sds_real);
	sds_real	RetUserr4Default(void) { return 0; }

	sds_real	RetUserr5(db_drawing *dp);
	short		SetUserr5(db_drawing *,sds_real);
	sds_real	RetUserr5Default(void) { return 0; }

	char *		RetUsers1(db_drawing *dp);
	short		SetUsers1(db_drawing *,char *);

	char *		RetUsers2(db_drawing *dp);
	short		SetUsers2(db_drawing *,char *);

	char *		RetUsers3(db_drawing *dp);
	short		SetUsers3(db_drawing *,char *);

	char *		RetUsers4(db_drawing *dp);
	short		SetUsers4(db_drawing *,char *);

	char *		RetUsers5(db_drawing *dp);
	short		SetUsers5(db_drawing *,char *);

	short		RetUsrtimer(db_drawing *dp);
	short		SetUsrtimer(db_drawing *,short);
	short		RetUsrtimerDefault(void) { return 1; }

	sds_real	RetViewaspect(db_drawing *dp);
	short		SetViewaspect(db_drawing *,sds_real);
	sds_real	RetViewaspectDefault(void) { return 1.0; }

	void		GetViewctr(db_drawing *, sds_point);
	short		SetViewctr(db_drawing *,sds_point);
	void		GetViewctrDefault(sds_point);

	void		GetViewdir(db_drawing *, sds_point);
	short		SetViewdir(db_drawing *,sds_point);
	void		GetViewdirDefault(sds_point);

	short		RetViewmode(db_drawing *dp);
	short		SetViewmode(db_drawing *,short);
	short		RetViewmodeDefault(void) { return 0; }

	sds_real	RetViewsize(db_drawing *dp);
	short		SetViewsize(db_drawing *,sds_real);
	sds_real	RetViewsizeDefault(void) { return 9; }

	sds_real	RetViewtwist(db_drawing *dp);
	short		SetViewtwist(db_drawing *,sds_real);
	sds_real	RetViewtwistDefault(void) { return 0; }

	short		RetVisretain(db_drawing *dp);
	short		SetVisretain(db_drawing *,short);
	short		RetVisretainDefault(void) { return 0; }

	void		GetVsmax(db_drawing *, sds_point);
	short		SetVsmax(db_drawing *,sds_point);
	void		GetVsmaxDefault(sds_point);

	void		GetVsmin(db_drawing *, sds_point);
	short		SetVsmin(db_drawing *,sds_point);
	void		GetVsminDefault(sds_point);

	short		RetWorlducs(db_drawing *dp);
	short		SetWorlducs(db_drawing *,short);
	short		RetWorlducsDefault(void) { return 1; }

	short		RetWorldview(db_drawing *dp);
	short		SetWorldview(db_drawing *,short);
	short		RetWorldviewDefault(void) { return 1; }

	short		RetWritestat(db_drawing *dp);
	short		SetWritestat(db_drawing *,short);
	short		RetWritestatDefault(void) { return 1; }

	short		RetZoompercent(db_drawing *dp);
	short		SetZoompercent(db_drawing *,short);
	short		RetZoompercentDefault(void) { return 100; }

	short		Setvar(db_drawing *dp, char *name, struct resbuf *rb);
	short		Getvar(db_drawing *dp, char *name, struct resbuf *rb);
	short		SetViewctrAsIs(db_drawing *, sds_point);
	short		SetViewdirAsIs(db_drawing *, sds_point);
	short		SetTargetAsIs(db_drawing *, sds_point);
	short		SetInsbaseAsIs(db_drawing *, sds_point);
	short		SetP_insbaseAsIs(db_drawing *, sds_point);
	short		SetElevationAsIs(db_drawing *, sds_real);
	short		SetP_elevationAsIs(db_drawing *, sds_real);
	};

#endif
