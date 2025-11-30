// DWG sysvar definition -- automatically generated -- do not edit
// Copyright (c) 1999, Visio Corporation.  All rights reserved.

#ifndef _INC_SYSVAR_H
#define _INC_SYSVAR_H

class DB_CLASS db_sysvar
	{
public:
	db_sysvar(void);
	~db_sysvar(void);

private:
	short		aperture;
	short		auditctl;
	short		automenuload;
	char *		basefile;
	short		bkgcolor;
	short		bkgredraw;
	short		cmddia;
	char *		cmdlntext;
	short		colorx;
	short		colory;
	short		colorz;
	char *		dctcust;
	char *		dctmain;
	short		dimss;
	short		dimsspos;
	short		dragopen;
	short		dragp1;
	short		dragp2;
	short		errorno;
	short		expert;
	short		explevel;
	char *		explstblk;
	char *		explstds;
	char *		explstlay;
	char *		explstlt;
	char *		explstts;
	char *		explstucs;
	char *		explstvw;
	short		fflimit;
	short		filedia;
	char *		fontalt;
	char *		fontmap;
	short		globcheck;
	short		gripblock;
	short		gripcolor;
	short		griphot;
	short		grips;
	short		gripsize;
	short		handudcopy;
	short		isavepercent;
	char *		lastprompt;
	short		logfilemode;
	char *		logfilename;
	char *		loginname;
	short		longfname;
	short		lwpolyout;
	short		macrorec;
	short		macrotrace;
	short		makebak;
	long	maxobjmem;
	short		maxsort;
	short		mbuttonpan;
	short		menuctl;
	char *		modemacro;
	char *		mtexted;
	short		nfilelist;
	short		pfacevmax;
	long	phandle;
	short		pickadd;
	short		pickauto;
	short		pickbox;
	short		pickdrag;
	short		pickfirst;
	short		plinetype;
	char *		plotid;
	short		plotrotmode;
	short		plotter;
	short		popups;
	short		progbar;
	short		projmode;
	short		promptmenu;
	char *		psprolog;
	short		psquality;
	short		qaflags;
	short		rasterpreview;
	short		re_init;
	char *		savefile;
	short		savetime;
	short		screenboxes;
	short		screenmode;
	sds_real	screenpos[2];
	sds_real	screensize[2];
	short		scrlhist;
	short		sortents;
	char *		srchpath;
	short		tabmode;
	short		tbcolor;
	short		tbsize;
	char *		tempprefix;
	short		texteval;
	short		textfill;
	short		tipofday;
	short		tooltips;
	long	treemax;
	short		ucsiconpos;
	short		wndbtexp;
	short		wndlcmd;
	short		wndlmain;
	short		wndlmdi;
	short		wndlscrl;
	short		wndlstat;
	short		wndltexp;
	short		wndltext;
	sds_real	wndpcmd[2];
	sds_real	wndpform[2];
	sds_real	wndpmain[2];
	sds_real	wndppmenu[2];
	sds_real	wndptexp[2];
	sds_real	wndptext[2];
	sds_real	wndscmd[2];
	sds_real	wndsform[2];
	sds_real	wndsmain[2];
	sds_real	wndstexp[2];
	sds_real	wndstext[2];
	short		xloadctl;
	char *		xloadpath;
	short		xrefctl;
	char *		_vernum;

public:

	short		RetAperture(void);
	short		SetAperture(short);
	short		RetApertureDefault(void) { return 10; }

	short		RetAuditctl(void);
	short		SetAuditctl(short);
	short		RetAuditctlDefault(void) { return 0; }

	short		RetAutomenuload(void);
	short		SetAutomenuload(short);
	short		RetAutomenuloadDefault(void) { return 1; }

	char *		RetBasefile(void);
	short		SetBasefile(char *);
	char *		RetBasefileDefault(void) { return "icad.dwt"; }

	short		RetBkgcolor(void);
	short		SetBkgcolor(short);
	short		RetBkgcolorDefault(void) { return 256; }

	short		RetBkgredraw(void);
	short		SetBkgredraw(short);
	short		RetBkgredrawDefault(void) { return 0; }

	short		RetCmddia(void);
	short		SetCmddia(short);
	short		RetCmddiaDefault(void) { return 1; }

	char *		RetCmdlntext(void);
	short		SetCmdlntext(char *);
	char *		RetCmdlntextDefault(void) { return ":"; }

	short		RetColorx(void);
	short		SetColorx(short);
	short		RetColorxDefault(void) { return 1; }

	short		RetColory(void);
	short		SetColory(short);
	short		RetColoryDefault(void) { return 3; }

	short		RetColorz(void);
	short		SetColorz(short);
	short		RetColorzDefault(void) { return 5; }

	char *		RetDctcust(void);
	short		SetDctcust(char *);

	char *		RetDctmain(void);
	short		SetDctmain(char *);

	short		RetDimSS();
	short		SetDimSS( short DimSS );

	short		RetDimSSPos();
	short		SetDimSSPos( short dimSSPos );

	short		RetDragopen(void);
	short		SetDragopen(short);
	short		RetDragopenDefault(void) { return 1; }

	short		RetDragp1(void);
	short		SetDragp1(short);
	short		RetDragp1Default(void) { return 10; }

	short		RetDragp2(void);
	short		SetDragp2(short);
	short		RetDragp2Default(void) { return 25; }

	short		RetErrorno(void);
	short		SetErrorno(short);
	short		RetErrornoDefault(void) { return 0; }

	short		RetExpert(void);
	short		SetExpert(short);
	short		RetExpertDefault(void) { return 0; }

	short		RetExplevel(void);
	short		SetExplevel(short);
	short		RetExplevelDefault(void) { return 3; }

	char *		RetExplstblk(void);
	short		SetExplstblk(char *);

	char *		RetExplstds(void);
	short		SetExplstds(char *);

	char *		RetExplstlay(void);
	short		SetExplstlay(char *);

	char *		RetExplstlt(void);
	short		SetExplstlt(char *);

	char *		RetExplstts(void);
	short		SetExplstts(char *);

	char *		RetExplstucs(void);
	short		SetExplstucs(char *);

	char *		RetExplstvw(void);
	short		SetExplstvw(char *);

	short		RetFflimit(void);
	short		SetFflimit(short);
	short		RetFflimitDefault(void) { return 0; }

	short		RetFiledia(void);
	short		SetFiledia(short);
	short		RetFilediaDefault(void) { return 1; }

	char *		RetFontalt(void);
	short		SetFontalt(char *);
	char *		RetFontaltDefault(void) { return "txt"; }

	char *		RetFontmap(void);
	short		SetFontmap(char *);
	char *		RetFontmapDefault(void) { return "icad.fmp"; }

	short		RetGlobcheck(void);
	short		SetGlobcheck(short);
	short		RetGlobcheckDefault(void) { return 0; }

	short		RetGripblock(void);
	short		SetGripblock(short);
	short		RetGripblockDefault(void) { return 0; }

	short		RetGripcolor(void);
	short		SetGripcolor(short);
	short		RetGripcolorDefault(void) { return 3; }

	short		RetGriphot(void);
	short		SetGriphot(short);
	short		RetGriphotDefault(void) { return 1; }

	short		RetGrips(void);
	short		SetGrips(short);
	short		RetGripsDefault(void) { return 1; }

	short		RetGripsize(void);
	short		SetGripsize(short);
	short		RetGripsizeDefault(void) { return 3; }

	short		RetHandudcopy(void);
	short		SetHandudcopy(short);
	short		RetHandudcopyDefault(void) { return 0; }

	short		RetIsavepercent(void);
	short		SetIsavepercent(short);
	short		RetIsavepercentDefault(void) { return 50; }

	char *		RetLastprompt(void);
	short		SetLastprompt(char *);

	short		RetLogfilemode(void);
	short		SetLogfilemode(short);
	short		RetLogfilemodeDefault(void) { return 0; }

	char *		RetLogfilename(void);
	short		SetLogfilename(char *);
	char *		RetLogfilenameDefault(void) { return "icad.log"; }

	char *		RetLoginname(void);
	short		SetLoginname(char *);
	char *		RetLoginnameDefault(void) { return "IntelliCAD User"; }

	short		RetLongfname(void);
	short		SetLongfname(short);
	short		RetLongfnameDefault(void) { return 1; }

	short		RetLwpolyout(void);
	short		SetLwpolyout(short);
	short		RetLwpolyoutDefault(void) { return 1; }

	short		RetMacrorec(void);
	short		SetMacrorec(short);
	short		RetMacrorecDefault(void) { return 0; }

	short		RetMacrotrace(void);
	short		SetMacrotrace(short);
	short		RetMacrotraceDefault(void) { return 0; }

	short		RetMakebak(void);
	short		SetMakebak(short);
	short		RetMakebakDefault(void) { return 1; }

	long	RetMaxobjmem(void);
	short		SetMaxobjmem(long);
	long	RetMaxobjmemDefault(void) { return 2147483647; }

	short		RetMaxsort(void);
	short		SetMaxsort(short);
	short		RetMaxsortDefault(void) { return 200; }

	short		RetMbuttonpan();
	short		SetMbuttonpan(short MButtonPan);

	short		RetMenuctl(void);
	short		SetMenuctl(short);
	short		RetMenuctlDefault(void) { return 1; }

	char *		RetModemacro(void);
	short		SetModemacro(char *);

	char *		RetMtexted(void);
	short		SetMtexted(char *);

	short		RetNfilelist(void);
	short		SetNfilelist(short);
	short		RetNfilelistDefault(void) { return 4; }

	short		RetPfacevmax(void);
	short		SetPfacevmax(short);
	short		RetPfacevmaxDefault(void) { return 4; }

	long	RetPhandle(void);
	short		SetPhandle(long);
	long	RetPhandleDefault(void) { return 0; }

	short		RetPickadd(void);
	short		SetPickadd(short);
	short		RetPickaddDefault(void) { return 1; }

	short		RetPickauto(void);
	short		SetPickauto(short);
	short		RetPickautoDefault(void) { return 1; }

	short		RetPickbox(void);
	short		SetPickbox(short);
	short		RetPickboxDefault(void) { return 3; }

	short		RetPickdrag(void);
	short		SetPickdrag(short);
	short		RetPickdragDefault(void) { return 0; }

	short		RetPickfirst(void);
	short		SetPickfirst(short);
	short		RetPickfirstDefault(void) { return 1; }

	short		RetPlinetype(void);
	short		SetPlinetype(short);
	short		RetPlinetypeDefault(void) { return 2; }

	char *		RetPlotid(void);
	short		SetPlotid(char *);

	short		RetPlotrotmode(void);
	short		SetPlotrotmode(short);
	short		RetPlotrotmodeDefault(void) { return 1; }

	short		RetPlotter(void);
	short		SetPlotter(short);
	short		RetPlotterDefault(void) { return 0; }

	short		RetPopups(void);
	short		SetPopups(short);
	short		RetPopupsDefault(void) { return 1; }

	short		RetProgbar(void);
	short		SetProgbar(short);
	short		RetProgbarDefault(void) { return 1; }

	short		RetProjmode(void);
	short		SetProjmode(short);
	short		RetProjmodeDefault(void) { return 1; }

	short		RetPromptmenu(void);
	short		SetPromptmenu(short);
	short		RetPromptmenuDefault(void) { return 1; }

	char *		RetPsprolog(void);
	short		SetPsprolog(char *);

	short		RetPsquality(void);
	short		SetPsquality(short);
	short		RetPsqualityDefault(void) { return 75; }

	short		RetQaflags(void);
	short		SetQaflags(short);
	short		RetQaflagsDefault(void) { return 0; }

	short		RetRasterpreview(void);
	short		SetRasterpreview(short);
	short		RetRasterpreviewDefault(void) { return 1; }

	short		RetRe_init(void);
	short		SetRe_init(short);
	short		RetRe_initDefault(void) { return 0; }

	char *		RetSavefile(void);
	short		SetSavefile(char *);
	char *		RetSavefileDefault(void) { return ".SV$"; }

	short		RetSavetime(void);
	short		SetSavetime(short);
	short		RetSavetimeDefault(void) { return 60; }

	short		RetScreenboxes(void);
	short		SetScreenboxes(short);
	short		RetScreenboxesDefault(void) { return 26; }

	short		RetScreenmode(void);
	short		SetScreenmode(short);
	short		RetScreenmodeDefault(void) { return 1; }

	void		GetScreenpos(sds_point);
	short		SetScreenpos(sds_point);
	void		GetScreenposDefault(sds_point);

	void		GetScreensize(sds_point);
	short		SetScreensize(sds_point);
	void		GetScreensizeDefault(sds_point);

	short		RetScrlhist(void);
	short		SetScrlhist(short);
	short		RetScrlhistDefault(void) { return 256; }

	short		RetSortents(void);
	short		SetSortents(short);
	short		RetSortentsDefault(void) { return 96; }

	char *		RetSrchpath(void);
	short		SetSrchpath(char *);

	short		RetTabmode(void);
	short		SetTabmode(short);
	short		RetTabmodeDefault(void) { return 0; }

	short		RetTbcolor(void);
	short		SetTbcolor(short);
	short		RetTbcolorDefault(void) { return 1; }

	short		RetTbsize(void);
	short		SetTbsize(short);
	short		RetTbsizeDefault(void) { return 0; }

	char *		RetTempprefix(void);
	short		SetTempprefix(char *);

	short		RetTexteval(void);
	short		SetTexteval(short);
	short		RetTextevalDefault(void) { return 0; }

	short		RetTextfill(void);
	short		SetTextfill(short);
	short		RetTextfillDefault(void) { return 1; }

	short		RetTipofday(void);
	short		SetTipofday(short);
	short		RetTipofdayDefault(void) { return 1; }

	short		RetTooltips(void);
	short		SetTooltips(short);
	short		RetTooltipsDefault(void) { return 1; }

	long	RetTreemax(void);
	short		SetTreemax(long);
	long	RetTreemaxDefault(void) { return 10000000; }

	short		RetUcsiconpos(void);
	short		SetUcsiconpos(short);
	short		RetUcsiconposDefault(void) { return 0; }

	short		RetWndbtexp(void);
	short		SetWndbtexp(short);
	short		RetWndbtexpDefault(void) { return 0; }

	short		RetWndlcmd(void);
	short		SetWndlcmd(short);
	short		RetWndlcmdDefault(void) { return 0; }

	short		RetWndlmain(void);
	short		SetWndlmain(short);
	short		RetWndlmainDefault(void) { return 2; }

	short		RetWndlmdi(void);
	short		SetWndlmdi(short);
	short		RetWndlmdiDefault(void) { return 2; }

	short		RetWndlscrl(void);
	short		SetWndlscrl(short);
	short		RetWndlscrlDefault(void) { return 0; }

	short		RetWndlstat(void);
	short		SetWndlstat(short);
	short		RetWndlstatDefault(void) { return 1; }

	short		RetWndltexp(void);
	short		SetWndltexp(short);
	short		RetWndltexpDefault(void) { return 1; }

	short		RetWndltext(void);
	short		SetWndltext(short);
	short		RetWndltextDefault(void) { return 0; }

	void		GetWndpcmd(sds_point);
	short		SetWndpcmd(sds_point);
	void		GetWndpcmdDefault(sds_point);

	void		GetWndpform(sds_point);
	short		SetWndpform(sds_point);
	void		GetWndpformDefault(sds_point);

	void		GetWndpmain(sds_point);
	short		SetWndpmain(sds_point);
	void		GetWndpmainDefault(sds_point);

	void		GetWndppmenu(sds_point);
	short		SetWndppmenu(sds_point);
	void		GetWndppmenuDefault(sds_point);

	void		GetWndptexp(sds_point);
	short		SetWndptexp(sds_point);
	void		GetWndptexpDefault(sds_point);

	void		GetWndptext(sds_point);
	short		SetWndptext(sds_point);
	void		GetWndptextDefault(sds_point);

	void		GetWndscmd(sds_point);
	short		SetWndscmd(sds_point);
	void		GetWndscmdDefault(sds_point);

	void		GetWndsform(sds_point);
	short		SetWndsform(sds_point);
	void		GetWndsformDefault(sds_point);

	void		GetWndsmain(sds_point);
	short		SetWndsmain(sds_point);
	void		GetWndsmainDefault(sds_point);

	void		GetWndstexp(sds_point);
	short		SetWndstexp(sds_point);
	void		GetWndstexpDefault(sds_point);

	void		GetWndstext(sds_point);
	short		SetWndstext(sds_point);
	void		GetWndstextDefault(sds_point);

	short		RetXloadctl(void);
	short		SetXloadctl(short);
	short		RetXloadctlDefault(void) { return 1; }

	char *		RetXloadpath(void);
	short		SetXloadpath(char *);

	short		RetXrefctl(void);
	short		SetXrefctl(short);
	short		RetXrefctlDefault(void) { return 0; }

	short		RetZoomfactor();
	short		SetZoomfactor(short ZoomFactor);

	char *		Ret_vernum(void);
	short		Set_vernum(char *);
	char *		Ret_vernumDefault(void) { return "?"; }

	short		Setvar(db_drawing *dp, char *name, struct resbuf *rb);
	short		Getvar(db_drawing *dp, char *name, struct resbuf *rb);
	};

#endif
