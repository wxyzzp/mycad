/* C:\ICAD\PRJ\LIB\DB\ENTITIES.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _INC_ENTITIES
#define _INC_ENTITIES
#include <TCHAR.H>
#include "..\..\Icad\IcadUtil.h"

extern int g_nClosingDrawings;


class DB_CLASS db_3dface : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_3dface(void);
    db_3dface(db_drawing *dp);
    db_3dface(const db_3dface &sour);  /* Copy constructor */
   ~db_3dface(void);

    int ret_is3pt(void) {
        return icadPointEqual(pt[2],pt[3]);
    }

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    bool get_ptbyidx(sds_point p, int i) {
        if (i>-1 && i<4) DB_PTCPY(p,pt[i]); return true;
    }

    sds_real *retp_10(void) { return pt[0]; }
    sds_real *retp_11(void) { return pt[1]; }
    sds_real *retp_12(void) { return pt[2]; }
    sds_real *retp_13(void) { return pt[3]; }

    bool get_10(sds_point p) { p[0]=pt[0][0]; p[1]=pt[0][1]; p[2]=pt[0][2]; return true; }
    bool get_11(sds_point p) { p[0]=pt[1][0]; p[1]=pt[1][1]; p[2]=pt[1][2]; return true; }
    bool get_12(sds_point p) { p[0]=pt[2][0]; p[1]=pt[2][1]; p[2]=pt[2][2]; return true; }
    bool get_13(sds_point p) { p[0]=pt[3][0]; p[1]=pt[3][1]; p[2]=pt[3][2]; return true; }
    bool get_70(int *p)      { *p=(unsigned char)flags; return true; }

    bool set_10(sds_point p) { pt[0][0]=p[0]; pt[0][1]=p[1]; pt[0][2]=p[2]; return true; }
    bool set_11(sds_point p) { pt[1][0]=p[0]; pt[1][1]=p[1]; pt[1][2]=p[2]; return true; }
    bool set_12(sds_point p) { pt[2][0]=p[0]; pt[2][1]=p[1]; pt[2][2]=p[2]; return true; }
    bool set_13(sds_point p) { pt[3][0]=p[0]; pt[3][1]=p[1]; pt[3][2]=p[2]; return true; }
    bool set_70(int  p)      { flags=(char)p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_point pt[4];  /* 10 - 13 */
    char      flags;  /* 70 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist. */
                          /*   extent[0] is the lower left  pt. */
                          /*   extent[1] is the upper right pt. */
                          /*   Z's are significant if disp has UCS (3D) */
                          /*   objects. */
};

class DB_CLASS db_acad_proxy_entity : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_acad_proxy_entity(void);
    db_acad_proxy_entity(db_drawing *dp);
    db_acad_proxy_entity(const db_acad_proxy_entity &sour);  /* Copy constructor */
   ~db_acad_proxy_entity(void);  /* Implicitly virtual */

    int  ret_entid(void)  { return entid; }
    void set_entid(int p) { entid=p; }

    void get_hirefllends(db_hireflink **firstpp, db_hireflink **lastpp) {
        if (firstpp!=NULL) *firstpp=hirefll[0];
        if (lastpp !=NULL) *lastpp =hirefll[1];
    }
    void set_hirefllends(db_hireflink *firstp, db_hireflink *lastp) {
        hirefll[0]=firstp; hirefll[1]=lastp;
    }

    int ret_nhirefs(void) {
        int rc;
        db_hireflink *tp;

        for (tp=hirefll[0],rc=0; tp!=NULL; tp=tp->next,rc++);
        return rc;
    }

    void addhiref(db_hireflink *toadd) {
        if (toadd!=NULL) {
            if (hirefll[0]==NULL) hirefll[0]=toadd; else hirefll[1]->next=toadd;
            hirefll[1]=toadd;
        }
    }

    bool get_90(int *p) { *p=pid     ; return true; }
    bool get_91(int *p) { *p=aid     ; return true; }
    bool get_92(int *p) { *p=grdatasz; return true; }
    bool get_93(int *p) { *p=databits; return true; }

    char *ret_grdata(void) { return grdata; }
    char *ret_data(void)   { return data; }
    int   ret_datasz(void) { return datasz; }

    bool set_90(int p) { pid=p     ; return true; }
    bool set_91(int p) { aid=p     ; return true; }
    bool set_93(int p) { databits=p; return true; }
    void set_grdata(char *pgrdata, int pgrdatasz) {  /* Also sets grdatasz (92). */
        delete [] grdata; grdata=NULL; grdatasz=0;
        if (pgrdata!=NULL && pgrdatasz>0) {
            grdatasz=pgrdatasz; grdata=new char[grdatasz];
            memcpy(grdata,pgrdata,grdatasz);
        }
    }
    void set_data(char *pdata, int pdatasz) {  /* Also sets datasz. */
        delete [] data; data=NULL; datasz=0;
        if (pdata!=NULL && pdatasz>0) {
            datasz=pdatasz; data=new char[datasz]; memcpy(data,pdata,datasz);
        }
    }

	bool  ret_description(char**p) { *p = m_desc; return true; }
	bool  set_description(char* description, int len)
	{
		strncpy(m_desc, description, (len > 255) ? 255 : len);
		m_desc[strlen(m_desc) - 1] = '\0';
		return true;
	}

    void *ret_disp(void)    { return disp; }
	// Note, I have intentionally named this set_displayVectors() instead of set_disp()
	// so that it won't be called by older IntelliCAD code. This should only be called
	// once (for now) when the proxy is read in from disk. This is because we are reading
	// the vectors from the OdDbDatabase file, and then closing that file. This in essence
	// make proxy entities in IntelliCAD un-editable. Someday we may need to re-visit this.
    void  set_displayVectors(void *p);

	void* ret_boundingBox(db_drawing* pDb);

    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,m_extent[0]); DB_PTCPY(ur,m_extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(m_extent[0],ll); DB_PTCPY(m_extent[1],ur); }

	void  setResbufChain(struct sds_resbuf* p) { m_pResbufChain = p; }
	struct sds_resbuf* getResbufChain() { return m_pResbufChain; }
	void  setOdDbClass(void* p) { m_OdClass = p; }
	void* getOdDbClass() { return m_OdClass; }

  private:

    int           entid;       /* The DWG internal ent type */

    int           pid;         /*  90 */
    int           aid;         /*  91 */
    int           grdatasz;    /*  92 */
    int           databits;    /*  93 */
    char         *grdata;
    int           datasz;
    char         *data;
    db_hireflink *hirefll[2];

    void      *disp;      /* Disp obj llist (gr_displayobject *) */

	// Holds the data for an OdDb-data.
    sds_point  m_extent[2]; /* The extents of the disp llist; see db_3dface. */
	char  m_desc[255];
	void* m_boundingBox;
	void* m_OdClass;
	struct sds_resbuf* m_pResbufChain;
};


class DB_CLASS db_arc : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_arc(void);
    db_arc(db_drawing *dp);
    db_arc(const db_arc &sour);  /* Copy constructor */
   ~db_arc(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real  *retp_10(void) { return cent; }
    sds_real  *retp_40(void) { return &rad; }

    bool get_10(sds_point p) { p[0]=cent[0]; p[1]=cent[1]; p[2]=cent[2]; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }
    bool get_40(sds_real *p) { *p=rad; return true; }
    bool get_50(sds_real *p) { *p=ang[0]; return true; }
    bool get_51(sds_real *p) { *p=ang[1]; return true; }

    bool set_10(sds_point p) { cent[0]=p[0]; cent[1]=p[1]; cent[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p) { rad=(p>=0.0) ? p : -p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); ang[0]=p; return true; }
    bool set_51(sds_real p) { ic_normang(&p,NULL); ang[1]=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real   thick;   /*  39 */

    sds_point  cent;    /*  10 */
    sds_real   rad;     /*  40 */
    sds_real   ang[2];  /*  50, 51 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_arcalignedtext : public db_entity_with_extrusion {

public:
	db_arcalignedtext(void);
    db_arcalignedtext(db_drawing *dp);
    db_arcalignedtext(const db_arcalignedtext &sour);  /* Copy constructor */
   ~db_arcalignedtext(void);

    void set_text(char *t) { strcpy(text,t); }
	void get_text(char *t) { strcpy(t,text); }
	void set_fontname(char *t) { strcpy(fontname,t); }
	void get_fontname(char *t) { strcpy(t,fontname); }
	void set_bigfontname(char *t) { strcpy(bigfontname,t); }
	void get_bigfontname(char *t) { strcpy(t,bigfontname); }
	void set_textstyleobjhandle(db_objhandle oh) { textstyleobjhandle=oh; }
	db_objhandle ret_textstyleobjhandle(void) { return textstyleobjhandle; }
	void set_pt0(sds_point p) { ic_ptcpy(pt0,p); }
	void get_pt0(sds_point p) { ic_ptcpy(p,pt0); }
	void set_radius(sds_real r) { radius=r; }
	sds_real ret_radius(void) { return radius; }
	void set_widthfactor(sds_real r) { widthfactor=r; }
	sds_real ret_widthfactor(void) { return widthfactor; }
	void set_height(sds_real r) { height=r; }
	sds_real ret_height(void) { return height; }
	void set_charspacing(sds_real r) { charspacing=r; }
	sds_real ret_charspacing(void) { return charspacing; }
	void set_offsetfromarc(sds_real r) { offsetfromarc=r; }
	sds_real ret_offsetfromarc(void) { return offsetfromarc; }
	void set_rightoffset(sds_real r) { rightoffset=r; }
	sds_real ret_rightoffset(void) { return rightoffset; }
	void set_leftoffset(sds_real r) { leftoffset=r; }
	sds_real ret_leftoffset(void) { return leftoffset; }
	void set_startangle(sds_real r) { startangle=r; }
	sds_real ret_startangle(void) { return startangle; }
	void set_endangle(sds_real r) { endangle=r; }
	sds_real ret_endangle(void) { return endangle; }
	void set_charorder(short s) { charorder=s; }
	short ret_charorder(void) { return charorder; }
	void set_direction(short s) { direction=s; }
	short ret_direction(void) { return direction; }
	void set_alignment(short s) { alignment=s; }
	short ret_alignment(void) { return alignment; }
	void set_side(short s) { side=s; }
	short ret_side(void) { return side; }
	void set_bold(short s) { bold=s; }
	short ret_bold(void) { return bold; }
	void set_italic(short s) { italic=s; }
	short ret_italic(void) { return italic; }
	void set_underline(short s) { underline=s; }
	short ret_underline(void) { return underline; }
	void set_charset(short s) { charset=s; }
	short ret_charset(void) { return charset; }
	void set_pitch(short s) { pitch=s; }
	short ret_pitch(void) { return pitch; }
	void set_fonttype(short s) { fonttype=s; }
	short ret_fonttype(void) { return fonttype; }
	void set_wizardflag(short s) { wizardflag=s; }
	short ret_wizardflag(void) { return wizardflag; }
	void set_color(long l) { color=l; }
	long ret_color(void) { return color; }

	// ODT specific - To be removed when DWGdirect output works.
	void set_grblob(char *gr) { grblob=gr; }
	char *ret_grblob(void) { return grblob; }
	void set_grblobsz(int sz) { grblobsz=sz; }
	int ret_grblobsz(void) { return grblobsz; }

	void set_arcobjhandle(db_objhandle oh) { arcobjhandle=oh; }
	db_objhandle ret_arcobjhandle(void) { return arcobjhandle; }

	private:
		char text[1024];
		char fontname[512];
		char bigfontname[512];
		db_objhandle textstyleobjhandle;
		sds_point pt0;
		sds_real radius;
		sds_real widthfactor;
		sds_real height;
		sds_real charspacing;
		sds_real offsetfromarc;
		sds_real rightoffset;
		sds_real leftoffset;
		sds_real startangle;
		sds_real endangle;
		short  charorder;
		short  direction;
		short  alignment;
		short  side;
		short  bold;
		short  italic;
		short  underline;
		short  charset;
		short  pitch;
		short  fonttype;
		long   color;
		short  wizardflag;

		// ODT specific - To be removed when DWGdirect output works.
		char * grblob;
		int    grblobsz;

		db_objhandle arcobjhandle;
};


class DB_CLASS db_attdef : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_attdef(void);
    db_attdef(db_drawing *dp);
    db_attdef(const db_attdef &sour);  /* Copy constructor */
   ~db_attdef(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return ipt; }
    sds_real *retp_11(void) { return apt; }
    sds_real *retp_40(void) { return &ht; }
    sds_real *retp_50(void) { return &rot; }
	char  ret_70(void) { return ( unsigned )flags; }
	char *ret_1(void) {return defval; }
	char *ret_2(void) { return tag; }

    bool get_1(char *p, int maxlen) {
        if (defval==NULL) *p=0;
        else { strncpy(p,defval,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_1(char **p) { *p=defval; return true; }
    bool get_2(char *p, int maxlen) {
        if (tag==NULL) *p=0;
        else { strncpy(p,tag,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_2(char **p) { *p=tag; return true; }
    bool get_3(char *p, int maxlen) {
        if (prompt==NULL) *p=0;
        else { strncpy(p,prompt,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_3(char **p) { *p=prompt; return true; }
    bool get_7(char *p, int maxlen)
        { if (tship==NULL) *p=0; else tship->get_2(p,maxlen); return true; }
    bool get_7(char **p) {
        if (tship!=NULL) tship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_7(db_handitem **p) { *p=tship; return true; }
    bool get_10(sds_point p) { p[0]=ipt[0]; p[1]=ipt[1]; p[2]=ipt[2]; return true; }
    bool get_11(sds_point p) { p[0]=apt[0]; p[1]=apt[1]; p[2]=apt[2]; return true; }
    bool get_39(sds_real *p) { *p=thick ; return true; }
    bool get_40(sds_real *p) { *p=ht    ; return true; }
    bool get_41(sds_real *p) { *p=xscl  ; return true; }
    bool get_50(sds_real *p) { *p=rot   ; return true; }
    bool get_51(sds_real *p) { *p=obl   ; return true; }
    bool get_70(int *p)      { *p=(unsigned char)flags ; return true; }
    bool get_71(int *p)      { *p=gen   ; return true; }
    bool get_72(int *p)      { *p=hjust ; return true; }
    bool get_73(int *p)      { *p=fldlen; return true; }
    bool get_74(int *p)      { *p=vjust ; return true; }

    bool set_1(char *p) {
        db_astrncpy(&defval,(p==NULL) ? db_str_quotequote : p,-1);
        return true;
    }
    bool set_2(char* p) {
        db_astrncpy(&tag,(p==NULL) ? db_str_quotequote : p,-1);
        ic_trim(tag,"bme"/*DNT*/); _tcsupr(tag);
        return true;
    }
    bool set_3(char *p) {
        db_astrncpy(&prompt,(p==NULL) ? db_str_quotequote : p,-1);
        return true;
    }

	bool set_7(char *p, db_drawing *dp) {
	    db_handitem *thip1;

		if (p==NULL || !*p || dp==NULL ||
			(thip1=dp->findtabrec(DB_STYLETAB,p,1))==NULL)
				{ db_lasterror=OL_ESNVALID; return false; }
	    tship=thip1; return true;
	}

	bool set_7(db_handitem *p) { tship=p; return true; }
    bool set_10(sds_point p) { ipt[0]=p[0]; ipt[1]=p[1]; ipt[2]=p[2]; return true; }
    bool set_11(sds_point p) { apt[0]=p[0]; apt[1]=p[1]; apt[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p) { ht=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QTEXTSIZE].defval)); return true; }
    bool set_41(sds_real p) { xscl=(icadRealEqual(p,0.0)) ? 1.0 : p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); rot=p; return true; }
    bool set_51(sds_real p) { ic_normang(&p,NULL); obl=p; return true; }
    bool set_70(int p) { flags=charCast(p); return true; }
    bool set_71(int p) { gen=charCast(p); return true; }
    bool set_72(int p) { hjust=charCast(p); return true; }
    bool set_73(int p) { fldlen=charCast((p>=0) ? p : -p); return true; }
    bool set_74(int p) { vjust=charCast(p); return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real     thick;   /*  39 */

    char        *defval;  /*   1 */
    char        *tag;     /*   2 */
    char        *prompt;  /*   3 */
    db_handitem *tship;   /*   7 */
    sds_point    ipt;     /*  10 */
    sds_point    apt;     /*  11 */
    sds_real     ht;      /*  40 */
    sds_real     xscl;    /*  41 */
    sds_real     rot;     /*  50 */
    sds_real     obl;     /*  51 */
    char         flags;   /*  70 */
    char         gen;     /*  71 */
    char         hjust;   /*  72 */
    int          fldlen;  /*  73 */
    char         vjust;   /*  74 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_attrib : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_attrib(void);
    db_attrib(db_drawing *dp);
    db_attrib(const db_attrib &sour);  /* Copy constructor */
   ~db_attrib(void);

    /*
    **  Return the hip of the INSERT entity:
    */
    db_handitem *ret_main(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return ipt; }
    sds_real *retp_11(void) { return apt; }
    sds_real *retp_40(void) { return &ht; }
    sds_real *retp_50(void) { return &rot; }

    bool get_1(char *p, int maxlen) {
        if (val==NULL) *p=0;
        else { strncpy(p,val,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_1(char **p) { *p=val; return true; }
    bool get_2(char *p, int maxlen) {
        if (tag==NULL) *p=0;
        else { strncpy(p,tag,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_2(char **p) { *p=tag; return true; }
    bool get_7(char *p, int maxlen)
        { if (tship==NULL) *p=0; else tship->get_2(p,maxlen); return true; }
    bool get_7(char **p) {
        if (tship!=NULL) tship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_7(db_handitem **p) { *p=tship; return true; }
    bool get_10(sds_point p) { p[0]=ipt[0]; p[1]=ipt[1]; p[2]=ipt[2]; return true; }
    bool get_11(sds_point p) { p[0]=apt[0]; p[1]=apt[1]; p[2]=apt[2]; return true; }
    bool get_39(sds_real *p) { *p=thick ; return true; }
    bool get_40(sds_real *p) { *p=ht    ; return true; }
    bool get_41(sds_real *p) { *p=xscl  ; return true; }
    bool get_50(sds_real *p) { *p=rot   ; return true; }
    bool get_51(sds_real *p) { *p=obl   ; return true; }
    bool get_70(int *p)      { *p=(unsigned char)flags ; return true; }
    bool get_71(int *p)      { *p=gen   ; return true; }
    bool get_72(int *p)      { *p=hjust ; return true; }
    bool get_73(int *p)      { *p=fldlen; return true; }
    bool get_74(int *p)      { *p=vjust ; return true; }

    bool set_1(char *p) {
        db_astrncpy(&val,(p==NULL) ? db_str_quotequote : p,-1);
        return true;
    }
    bool set_2(char* p) {
        db_astrncpy(&tag,(p==NULL) ? db_str_quotequote : p,-1);
        ic_trim(tag,"bme"/*DNT*/); _tcsupr(tag);
        return true;
    }
	bool set_7(char *p, db_drawing *dp) {
		db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
		    (thip1=dp->findtabrec(DB_STYLETAB,p,1))==NULL)
			    { db_lasterror=OL_ESNVALID; return false; }
	    tship=thip1; return true;
	}
    bool set_7(db_handitem *p) { tship=p; return true; }
    bool set_10(sds_point p) { ipt[0]=p[0]; ipt[1]=p[1]; ipt[2]=p[2]; return true; }
    bool set_11(sds_point p) { apt[0]=p[0]; apt[1]=p[1]; apt[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p) { ht=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QTEXTSIZE].defval)); return true; }
    bool set_41(sds_real p) { xscl=(icadRealEqual(p,0.0)) ? 1.0 : p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); rot=p; return true; }
    bool set_51(sds_real p) { ic_normang(&p,NULL); obl=p; return true; }
    bool set_70(int p) { flags=charCast(p); return true; }
    bool set_71(int p) { gen=charCast(p); return true; }
    bool set_72(int p) { hjust=charCast(p); return true; }
    bool set_73(int p) { fldlen=charCast((p>=0) ? p : -p); return true; }
    bool set_74(int p) { vjust=charCast(p); return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real     thick;   /*  39 */

    char        *val;     /*   1 */
    char        *tag;     /*   2 */
    db_handitem *tship;   /*   7 */
    sds_point    ipt;     /*  10 */
    sds_point    apt;     /*  11 */
    sds_real     ht;      /*  40 */
    sds_real     xscl;    /*  41 */
    sds_real     rot;     /*  50 */
    sds_real     obl;     /*  51 */
    char         flags;   /*  70 */
    char         gen;     /*  71 */
    char         hjust;   /*  72 */
    int          fldlen;  /*  73 */
    char         vjust;   /*  74 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_block : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  friend class db_blocktabrec;

  public:

    db_block(void);
    db_block(db_drawing *dp);
    db_block(const db_block &sour);  /* Copy constructor */
   ~db_block(void);


    db_handitem *ret_bthip(void) { return bthip; }
    void set_bthip(db_blocktabrec *p) { bthip=p; }

    int   ret_looked4xref(void) { return looked4xref; }
    void  set_looked4xref(int p) { looked4xref=(char)p; }
    db_drawing *ret_xrefdp(void) { return xrefdp; }
	void set_xrefdp(db_drawing *p) {
	    delete xrefdp; xrefdp=p;
	    if (p==NULL) looked4xref=0;
	}

    /* Need a way to just set the pointer to NULL without freeing the */
    /* drawing like set_xrefdp(NULL) does.  Caller must reassign the pointer */
    /* using set_xrefdp() on a new block tab record. */
    void  null_xrefdp(void) { xrefdp=NULL; looked4xref=0; }

    bool get_firstentinblock(sds_name p, db_drawing *dp) {
        if (bthip!=NULL) bthip->get_firstentinblock(p,dp); else p[0]=p[1]=0;
        return true;
    }
    bool get_firstentinblock(db_handitem **p) {
        if (bthip!=NULL) bthip->get_firstentinblock(p); else *p=NULL;
        return true;
    }
    bool get_1(char *p, int maxlen) {
        if (xrefpn==NULL) *p=0;
        else { strncpy(p,xrefpn,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_1(char **p) { *p=xrefpn; return true; }

	char *retp_xrefpn(void) { return xrefpn; }

	bool get_2(char *p, int maxlen) {
        if (name==NULL) *p=0; else { strncpy(p,name,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_2(char **p) { *p=name; return true; }
    bool get_3(char *p, int maxlen) {
        if (xrefpn==NULL) *p=0;
        else { strncpy(p,xrefpn,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_3(char **p) { *p=xrefpn; return true; }
    bool get_10(sds_point p) { DB_PTCPY(p,base); return true; }
	double *retp_insbase(void) { return base; }
    bool get_70(int *p) { *p=(unsigned char)tflags; return true; }
    int ret_flags(void) { return tflags; }


    bool set_1(char *p) {
        db_astrncpy(&xrefpn,(p==NULL) ? db_str_quotequote : p,DB_PATHNAMESZ);
        return true;
    }
    bool set_3(char *p) {
        db_astrncpy(&xrefpn,(p==NULL) ? db_str_quotequote : p,DB_PATHNAMESZ);
        return true;
    }
    bool set_10(sds_point p) { DB_PTCPY(base,p); return true; }

	bool set_2(char* p) {
	    db_astrncpy(&name,(p==NULL) ? db_str_quotequote : p,IC_ACADNMLEN-1);
		if (bthip!=NULL && bthip->ret_type()==DB_BLOCKTABREC)
			db_astrncpy(&(bthip->m_pName),name,-1);
	    return true;
	}

	bool set_2(char *name, db_drawing *dp) {
/*
**  If it's an anonymous block; add the number.
**
**  This one calls it's own set_2(char *), which will set the
**  db_blocktabrec "name", too.
*/
	    char *astr=NULL;

		// check to see if name generation is on for wblock
		if (dp->GeneratingAnonNames())
			{
			astr=db_makeanonname(name,dp,0);
			set_2(astr);
			delete [] astr;
			}
		else
			set_2(name);

	    return true;
	}

	bool set_70(int p) {
	    tflags=(char)p;
		if (bthip!=NULL && bthip->ret_type()==DB_BLOCKTABREC)
			bthip->m_flags=tflags;
	    return true;
	}

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    /*
    **  Some explanation is required.  Here it is. When entmaking
    **  a BLOCK, all of the data needs to be stored in the BLOCK
    **  entity (because the table record doesn't exist yet).
    **  Therefore, db_blocktabrec and db_block have to duplicate the
    **  name and flags data.  The getters can use their own members,
    **  but the SETTERS have to ATTEMPT to set each other's members as
    **  well, to keep them in agreement (but NOT via the other's
    **  setters, or we'll have infinite recursion). When finally fully
    **  established, each will have a pointer to the other:
    **  db_blocktabrec::entll[0] will point to the db_block, and
    **  db_block::bthip will point to the db_blocktabrec.
    **
    **  Here's the scenario (we hope).  sds_entmake() will "new" a
    **  db_block and entmod it to set the data in the db_block (at which
    **  point bthip will be NULL).  It will then call
    **  db_drawing::addhanditem(), which will link it onto the pending
    **  chain (db_drawing::pendentmake).  When addhanditem() finally
    **  gets the db_endblk, it will check the block table for a record
    **  that has the same name as the db_block.  If it exists,
    **  db_block::bthip will be set to point to that record.  If it doesn't
    **  exist, addhanditem() will create the table record and point
    **  bthip at it.  Then, addhanditem() will set the table record's
    **  name and flags to make them agree.  Finally, addhanditem()
    **  will delete the old db_blocktabrec::entll[0] (if it exists)
    **  and tack on the pending llist (db_drawing::pendentmake[0]) --
    **  whose first link is the db_block.
    */

    char      *name;   /*  2 */
    char       tflags; /* 70 */

    char       looked4xref;  /* Already checked for xrefpn once. */
    char      *xrefpn; /*  1 and/or 3 */
    db_drawing      *xrefdp; /* A dp for the xref (must be cast). */
    sds_point  base;   /* 10 */

    db_blocktabrec *bthip;  /* The corresponding block table record */
};


class DB_CLASS db_circle : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_circle(void);
    db_circle(db_drawing *dp);
    db_circle(const db_circle &sour);  /* Copy constructor */
   ~db_circle(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return cent; }
    sds_real *retp_40(void) { return &rad; }

    bool get_10(sds_point p) { p[0]=cent[0]; p[1]=cent[1]; p[2]=cent[2]; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }
    bool get_40(sds_real *p) { *p=rad; return true; }

    bool set_10(sds_point p) { cent[0]=p[0]; cent[1]=p[1]; cent[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p) { rad=(p>=0.0) ? p : -p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real   thick;   /*  39 */

    sds_point  cent;    /*  10 */
    sds_real   rad;     /*  40 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


// forward references needed by db_dimension::get_texthandle
class db_text;
class CDbMText;

class DB_CLASS db_dimension : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_dimension(void);
    db_dimension(db_drawing *dp);
    db_dimension(const db_dimension &sour);  /* Copy constructor */
   ~db_dimension(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

	// at most one of the pText and pMtext parameters will be non-NULL after get_texthandle is called.
    bool  get_texthandle(db_text * &pText, CDbMText * &pMtext);

    sds_real *retp_10(void) { return pt[0]; }
    sds_real *retp_11(void) { return pt[1]; }
    sds_real *retp_12(void) { return pt[2]; }
    sds_real *retp_13(void) { return pt[3]; }
    sds_real *retp_14(void) { return pt[4]; }
    sds_real *retp_15(void) { return pt[5]; }
    sds_real *retp_16(void) { return pt[6]; }
    sds_real *retp_40(void) { return &leadlen; }

    bool get_1(char *p, int maxlen) {
        if (dimtext==NULL) *p=0;
        else { strncpy(p,dimtext,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_1(char **p) { *p=dimtext; return true; }
    bool get_2(char *p, int maxlen)
        { if (bthip==NULL) *p=0; else bthip->get_2(p,maxlen); return true; }
    bool get_2(char **p) {
        if (bthip!=NULL) bthip->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_2(db_handitem **p) { *p=bthip; return true; }
    bool get_3(char *p, int maxlen)
        { if (dship==NULL) *p=0; else dship->get_2(p,maxlen); return true; }
    bool get_3(char **p) {
        if (dship!=NULL) dship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_3(db_handitem **p) { *p=dship; return true; }
    bool get_10(sds_point p) { p[0]=pt[0][0]; p[1]=pt[0][1]; p[2]=pt[0][2]; return true; }
    bool get_11(sds_point p) { p[0]=pt[1][0]; p[1]=pt[1][1]; p[2]=pt[1][2]; return true; }
    /* Just get the ECS point; let entget do its own transforming: */
    bool get_12(sds_point p) { p[0]=pt[2][0]; p[1]=pt[2][1]; p[2]=pt[2][2]; return true; }
    bool get_13(sds_point p) { p[0]=pt[3][0]; p[1]=pt[3][1]; p[2]=pt[3][2]; return true; }
    bool get_14(sds_point p) { p[0]=pt[4][0]; p[1]=pt[4][1]; p[2]=pt[4][2]; return true; }
    bool get_15(sds_point p) { p[0]=pt[5][0]; p[1]=pt[5][1]; p[2]=pt[5][2]; return true; }
    bool get_16(sds_point p) { p[0]=pt[6][0]; p[1]=pt[6][1]; p[2]=pt[6][2]; return true; }
    bool get_40(sds_real *p) { *p=leadlen; return true; }
    bool get_41(sds_real *p) { *p=lineFact; return true; }
	//Bugzilla No. 78310 ; 07-10-2002
	bool get_42(sds_real *p) { *p=measurement  ; return true; }
    bool get_50(sds_real *p) { *p=ang    ; return true; }
    bool get_51(sds_real *p) { *p=hdir   ; return true; }
    bool get_52(sds_real *p) { *p=slant  ; return true; }
    bool get_53(sds_real *p) { *p=textrot; return true; }
    bool get_54(sds_real *p) { *p=insrot;  return true; }
    bool get_70(int *p) { *p=(unsigned char)dimtype; return true; }
    bool get_71(int *p) { *p=(unsigned char)attachment; return true; }
    bool get_72(int *p) { *p=(unsigned char)spacing; return true; }

    bool set_1(char *p) {
        db_astrncpy(&dimtext,(p==NULL) ? db_str_quotequote : p,-1);
        return true;
    }
    bool set_2(db_handitem *p) { bthip=p; return true; }
	bool set_2(char *p, db_drawing *dp) {
	    db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
		    (thip1=dp->findtabrec(DB_BLOCKTAB,p,1))==NULL)
			    { db_lasterror=OL_ESNVALID; return false; }
	    bthip=thip1; return true;
	}
	bool set_3(char *p, db_drawing *dp) {
		db_handitem *thip1;

		dship=NULL;
		if (p==NULL || !*p) return true; // NULL dimstyle  is legal
		if ((thip1=dp->findtabrec(DB_DIMSTYLETAB,p,1))==NULL)
			    { db_lasterror=OL_ESNVALID; return false; }
	    dship=thip1; return true;
	}
    bool set_3(db_handitem *p) { dship=p; return true; }
    bool set_10(sds_point p) { pt[0][0]=p[0]; pt[0][1]=p[1]; pt[0][2]=p[2]; return true; }
    bool set_11(sds_point p) { pt[1][0]=p[0]; pt[1][1]=p[1]; pt[1][2]=p[2]; return true; }
    /* Just set the ECS point; let entmod do its own transforming: */
    bool set_12(sds_point p) { pt[2][0]=p[0]; pt[2][1]=p[1]; pt[2][2]=p[2]; return true; }
    bool set_13(sds_point p) { pt[3][0]=p[0]; pt[3][1]=p[1]; pt[3][2]=p[2]; return true; }
    bool set_14(sds_point p) { pt[4][0]=p[0]; pt[4][1]=p[1]; pt[4][2]=p[2]; return true; }
    bool set_15(sds_point p) { pt[5][0]=p[0]; pt[5][1]=p[1]; pt[5][2]=p[2]; return true; }
    bool set_16(sds_point p) { pt[6][0]=p[0]; pt[6][1]=p[1]; pt[6][2]=p[2]; return true; }
    bool set_40(sds_real p) { leadlen=(p>=0.0) ? p : -p; return true; }
    bool set_41(sds_real p) { lineFact=p; return true; }
	//Bugzilla No. 78310 ; 07-10-2002
	bool set_42(sds_real p) { measurement=p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); ang    =p; return true; }
    bool set_51(sds_real p) { ic_normang(&p,NULL); hdir   =p; return true; }
    bool set_52(sds_real p) { slant=p; return true; }
    bool set_53(sds_real p) { ic_normang(&p,NULL); textrot=p; return true; }
    bool set_54(sds_real p) { ic_normang(&p,NULL); insrot=p; return true; }
    bool set_70(int p) { dimtype=(unsigned char)p; return true; }
    bool set_71(int p) { attachment=(unsigned char)p; return true; }
    bool set_72(int p) { spacing=(unsigned char)p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

  private:

    char               *dimtext;  /*   1 */
    class db_handitem  *bthip;    /*   2 */
    class db_handitem  *dship;    /*   3 */
    sds_point           pt[7];    /*  10 - 16.  The 12 ([2]) is ECS here  */
                                  /*    (as doc'd) AS IN THE DWG.  But,   */
                                  /*    as presented to the user (entget/ */
                                  /*    entmake/mod) -- it's WCS.  We     */
                                  /*    know this because we use it as    */
                                  /*    the ECS insertion point of the    */
                                  /*    block in gr_getdispobjs() (which  */
                                  /*    works), and transforming it       */
                                  /*    ECS->WCS gives the point ACAD     */
                                  /*    reports in an entget.             */
    sds_real            leadlen;  /*  40 */
    sds_real            lineFact; /*  41 */
	sds_real            measurement;/*  42 */ //Bugzilla No. 78310 ; 07-10-2002
    sds_real            ang;      /*  50 */
    sds_real            hdir;     /*  51 */
    sds_real            slant;    /*  52 */
    sds_real            textrot;  /*  53 */
    sds_real            insrot;   /*  54 The rotation for the INSERT */
    unsigned char       dimtype;  /*  70 */
    unsigned char       attachment; /*  71 */
    unsigned char       spacing;	/*  72 */
	
    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_ellipse : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_ellipse(void);
    db_ellipse(db_drawing *dp);
    db_ellipse(const db_ellipse &sour);  /* Copy constructor */
   ~db_ellipse(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return cent; }
    sds_real *retp_11(void) { return smavect; }
    sds_real *retp_40(void) { return &min_maj; }
    sds_real *retp_41(void) { return param; }
    sds_real *retp_42(void) { return param+1; }

    bool get_10(sds_point p) { p[0]=cent[0]; p[1]=cent[1]; p[2]=cent[2]; return true; }
    bool get_11(sds_point p) { p[0]=smavect[0]; p[1]=smavect[1]; p[2]=smavect[2]; return true; }
    bool get_40(sds_real *p) { *p=min_maj; return true; }
    bool get_41(sds_real *p) { *p=param[0]; return true; }
    bool get_42(sds_real *p) { *p=param[1]; return true; }

    bool set_10(sds_point p) { cent[0]=p[0]; cent[1]=p[1]; cent[2]=p[2]; return true; }
    bool set_11(sds_point p) {
        if (icadRealEqual(p[0],0.0) && icadRealEqual(p[1],0.0) && icadRealEqual(p[2],0.0)) {
            smavect[0]=1.0; smavect[1]=smavect[2]=0.0;
        } else {
            smavect[0]=p[0]; smavect[1]=p[1]; smavect[2]=p[2];
        }
        return true;
    }
    bool set_40(sds_real p) { min_maj=(p>0.0) ? p : ((p<0.0) ? -p : 0.5); return true; }
    bool set_41(sds_real p) { param[0]=p; return true; }  /* Don't normalize */
    bool set_42(sds_real p) { param[1]=p; return true; }  /* Don't normalize */

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

    void acadize(void);

  private:

    sds_point              cent;      /*  10 */
    sds_point              smavect;   /*  11 */
    sds_real               min_maj;   /*  40 */
    sds_real               param[2];  /*  41,42 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_endblk : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_endblk(void);
    db_endblk(db_drawing *dp);
    db_endblk(const db_endblk &sour);  /* Copy constructor */

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

};


/*
**  Note that all points are 2D.  Because of this, the complex
**  (non-DXF) setters and getters use separate X and Y sds_real
**  pass parameters (instead of sds_points) for maximum
**  versatility.
**
**  It helps to know how a HATCH is defined:
**
**    A HATCH entity consists of one or more paths (or loops) that
**    define the hatching boundaries.
**
**    Each path can have one or more edges.
**
**    An edge can be one of the following types: line, arc, elliptic
**    arc, spline, polyline (called pline here).  A path may consist
**    of a mixture of the first four types of edges -- or ONE SINGLE
**    polyline edge. ACAD treats the polyline case as a special case;
**    we don't.  A polyline is simply one of the five edge-types. We
**    do, however, adhere to the rule that a path can have n edges of
**    the first four types -- or -- ONE edge of type polyline.
**
**  The simple (not-an-array-of-classes) database items have the
**  usually DXF-based getters and setters.  There are two other
**  more complicated arrays:
**
**  The patline[] array:
**
**    This is an array of definition data for hatch-lines.
**    Each element has an array of sds_reals describing dashes.
**    The getters and setters aren't too complicated.  See the prototypes
**    in the db_hatch class.
**
**  The path[] array:
**
**    Each path element can have an array of edges.
**    Each edge has one of the five types (discussed above)
**    and has a union of pointers to the edge-type-specific object
**    that applies.  Each of the edge-type-specific classes defines
**    that type of edge and can have arrays of its own.  This
**    leads to some ugly identifiers like
**
**      path[p].edge[e].u.pline->vert[v].bulge
**
**    but the user shouldn't have to resort to this.  The getters
**    and setters break it down so the user can deal with simple
**    variables.
**
**    HOWEVER, be careful using the SETTERS.  They are ORDER-DEPENDENT,
**    but in obvious ways (i.e., don't try to set edge data until
**    you've created the path[] array and the specific edge[] array
**    inside one of the path[] elements).
**
**    Good luck with this one.
**
**  None of the subclasses have copy constructors;
**  the db_hatch copy constructor handles it all.  (Not sure if or how
**  a copy constructor can copy an array, anyway.)
*/
class DB_CLASS db_hatchlinelink {

  public:

    db_hatchlinelink(void) { pt[0][0]=pt[0][1]=pt[1][0]=pt[1][1]=0.0; }

   	void  operator = (const db_hatchlinelink& lineedge)
		{
			pt[0][0] = lineedge.pt[0][0];
			pt[0][1] = lineedge.pt[0][1];
			pt[1][0] = lineedge.pt[1][0];
			pt[1][1] = lineedge.pt[1][1];
		}

	bool  operator == (const db_hatchlinelink& lineedge) const
		{
			if (icadRealEqual(pt[0][0],lineedge.pt[0][0]) && icadRealEqual(pt[0][1],lineedge.pt[0][1]) &&
				icadRealEqual(pt[1][0],lineedge.pt[1][0]) && icadRealEqual(pt[1][1],lineedge.pt[1][1]))

				return true;

			return false;
		}

    sds_real pt[2][2];  /* 10,11 */
};

class DB_CLASS db_hatcharclink {

  public:

    db_hatcharclink(void) {
        cent[0]=cent[1]=ang[0]=0.0; rad=ang[1]=1.0; ccw=1;
    }

	void operator = (const db_hatcharclink &arcedge)
	{
		cent[0] = arcedge.cent[0];
		cent[1] = arcedge.cent[1];
		rad = arcedge.rad;
		ang[0] = arcedge.ang[0];
		ang[1] = arcedge.ang[1];
		ccw = arcedge.ccw;
	}

	bool operator == (const db_hatcharclink &arcedge) const
	{
		if (icadRealEqual(cent[0],arcedge.cent[0]) && icadRealEqual(cent[1],arcedge.cent[1]) && 
			icadRealEqual(rad,arcedge.rad) && icadAngleEqual(ang[0],arcedge.ang[0]) && 
			icadRealEqual(ang[1],arcedge.ang[1]) && ccw == arcedge.ccw)
			return true;

		return false;
	}

    sds_real cent[2];  /* 10 */
    sds_real rad;      /* 40 */
    sds_real ang[2];   /* 50,51 */
    char     ccw;      /* 73 */
};

class DB_CLASS db_hatchellarclink {

  public:

    db_hatchellarclink(void) {
        cent[0]=cent[1]=smavect[1]=param[0]=0.0;
        smavect[0]=param[1]=1.0; min_maj=0.5; ccw=1;
    }

	void operator = (const db_hatchellarclink &ellarcedge) {
		cent[0] = ellarcedge.cent[0];
		cent[1] = ellarcedge.cent[1];
		smavect[0] = ellarcedge.smavect[0];
		smavect[1] = ellarcedge.smavect[1];
		min_maj = ellarcedge.min_maj;
		param[0] = ellarcedge.param[0];
		param[1] = ellarcedge.param[1];
		ccw = ellarcedge.ccw;
	}

	bool operator == (const db_hatchellarclink &ellarcedge) const {
		if (icadRealEqual(cent[0],ellarcedge.cent[0]) && 
			icadRealEqual(cent[1],ellarcedge.cent[1]) && 
			icadRealEqual(smavect[0],ellarcedge.smavect[0]) &&
			icadRealEqual(smavect[1],ellarcedge.smavect[1]) && 
			icadRealEqual(min_maj,ellarcedge.min_maj) && 
			icadRealEqual(param[0],ellarcedge.param[0]) &&
			icadRealEqual(param[1],ellarcedge.param[1]) && ccw == ellarcedge.ccw)

			return true;

		return false;
	}

    sds_real cent[2];     /* 10 */
    sds_real smavect[2];  /* 11 */
    sds_real min_maj;     /* 40 */
    sds_real param[2];    /* 50,51 */
    char     ccw;         /* 73 */
};

class DB_CLASS db_hatchsplinecontrollink {  /* For db_hatchsplinelink below. */

  public:

	db_hatchsplinecontrollink(void) { pt[0]=pt[1]=0.0; wt=1.0;}

	void operator = (const db_hatchsplinecontrollink &controlpt) {
		pt[0] = controlpt.pt[0];
		pt[1] = controlpt.pt[1];
		wt = controlpt.wt;
	}

	bool operator == (const db_hatchsplinecontrollink &controlpt) const {
		if (icadRealEqual(pt[0],controlpt.pt[0]) && icadRealEqual(pt[1],controlpt.pt[1]) && wt == controlpt.wt)
			return true;

		return false;
	}

	bool operator != (const db_hatchsplinecontrollink &controlpt) const {
		return !(*this == controlpt);
	}

    sds_real pt[2];  /* 10 */
    sds_real wt;    /* 42 */
};
class DB_CLASS db_hatchsplinelink {

  public:

    db_hatchsplinelink(void) {
        knot=NULL; control=NULL; flags=0; deg=3; nknots=ncpts=0;
    }
	~db_hatchsplinelink(void) { delete [] knot; delete [] control; }

	void operator = (const db_hatchsplinelink &splineedge);
	bool operator == (const db_hatchsplinelink &splineedge) const;


    sds_real                  *knot;    /* 40 (nknots of them) */
    char                       flags;   /* 73,74 (Rational and Periodic coded */
                                        /*       as in SPLINE: xxxxxRPx) */
    int                        deg;     /* 94 */
    int                        nknots;  /* 95 */
    int                        ncpts;   /* 96 */
    db_hatchsplinecontrollink *control; /* ncpts of them */
};

class DB_CLASS db_hatchplinevertlink {

  public:
    db_hatchplinevertlink(void) { pt[0]=pt[1]=bulge=0.0; }

	void operator = (const db_hatchplinevertlink &vert)
	{
		pt[0] = vert.pt[0];
		pt[1] = vert.pt[1];
		bulge = vert.bulge;
	}

	bool operator == (const db_hatchplinevertlink &vert) const
	{
		if (icadRealEqual(pt[0], vert.pt[0]) && icadRealEqual(pt[1], vert.pt[1]) && 
			icadRealEqual(bulge, vert.bulge))
			return true;

		return false;
	}

	bool operator != (const db_hatchplinevertlink &vert) const {
		return !(*this == vert);
	}

    sds_real pt[2];  /* 10 */
    sds_real bulge;  /* 42 */
};

/*
**  We force a pline to be a single "edge" in our database -- rather
**  than define a separate scenario like ACAD does.
*/
class DB_CLASS db_hatchplinelink {

  public:

    db_hatchplinelink(void) { flags=0; nverts=0; vert=NULL; }
	~db_hatchplinelink(void) { delete [] vert; }

	void operator = (const db_hatchplinelink &plineedge)
	{
		flags = plineedge.flags;
		nverts = plineedge.nverts;

		if (vert)
			delete [] vert;

		if (nverts) {
			vert = new db_hatchplinevertlink[nverts];
			for (int i = 0; i < nverts; i++)
				vert[i] = plineedge.vert[i];
		}
	}

	bool operator == (const db_hatchplinelink &plineedge) const
	{
		if (flags != plineedge.flags || nverts != plineedge.nverts)
			return false;

		for (int i = 0; i < nverts; i++)
			if (vert[i] != plineedge.vert[i])
				return false;

		return true;
	}

    char                   flags;   /* 72,73  Our bit-coding for hasBulges */
                                    /*        and Closed: xxxxxBC */
    int                    nverts;  /* 93 */
    db_hatchplinevertlink *vert;    /* (nverts of them) */
};

class DB_CLASS db_hatchedgelink {

  public:

    db_hatchedgelink(void) { type=DB_HATCHEDGEUNDEFINED; u.line=NULL; }  /* Any one of them should do. */
   ~db_hatchedgelink(void);

   	void operator = (const db_hatchedgelink& edge);
	bool operator == (const db_hatchedgelink& edge) const;
	bool operator != (const db_hatchedgelink& edge) const {return !(*this == edge);}

    int type;  /* 72 (We use DB_HATCHEDGEPLINE for pline; not used for pline */
               /* form in entget/entmake.) */
    union {
        db_hatchlinelink   *line;
        db_hatcharclink    *arc;
        db_hatchellarclink *ellarc;
        db_hatchsplinelink *spline;
        db_hatchplinelink  *pline;
    } u;
};

class DB_CLASS db_hatchboundobjs
{
public:
	db_hatchboundobjs(): m_next(NULL),m_boundaryObj(NULL) {}
	//Copy constructor note that next is set to NULL
	db_hatchboundobjs(db_hatchboundobjs &boundobj):
	m_next(NULL), m_boundaryObj(boundobj.m_boundaryObj) {}
	virtual ~db_hatchboundobjs() {}

	void CleanAssociations(db_objhandle &hatchHandle, db_handitem* hatch)
	{
		if (0 == g_nClosingDrawings && m_boundaryObj)
		{
			if(!hatch || m_boundaryObj->getReactor() &&
				m_boundaryObj->getReactor()->FindReactor(hatch))
				m_boundaryObj->removeReactor(hatchHandle);
		}
	}

	//do not delete handitem returned - actual pointer to object
    db_handitem *retBoundaryObj() {return m_boundaryObj;}
    db_objhandle retBoundaryObjHand()
	{
		return m_boundaryObj ? m_boundaryObj->RetHandle() : db_objhandle();
	}

    bool getBoundaryObj(db_handitem **newObj)
	{
		*newObj = m_boundaryObj;
		return m_boundaryObj != NULL;
	}

    bool getBoundaryObjHand(db_objhandle *hand)
    {
		if(m_boundaryObj)
			*hand =  m_boundaryObj->RetHandle();
		else
			hand->Nullify();
		return true;
	}

    bool setBoundaryObj(db_handitem *newObj)
	{
		m_boundaryObj = newObj;
		return newObj != NULL;
	}

	db_hatchboundobjs *getNext() {return m_next;}
	bool setNext(db_hatchboundobjs *next) {m_next = next; return true;}
	db_handitem **retp_FixupLoc(void) { return &m_boundaryObj; }

protected:
	db_handitem *m_boundaryObj;		//  330 numBoundaryObj of object handles that hatch is associated to
	db_hatchboundobjs *m_next;
};

class DB_CLASS db_hatchboundobjslink
{
public:
	db_hatchboundobjslink(db_drawing *drawing) : m_first(NULL), m_last(NULL), m_drawing(drawing) {}
	db_hatchboundobjslink(db_hatchboundobjslink &boundobjlist);

	virtual ~db_hatchboundobjslink();

	void CleanAssociations (db_objhandle &hatchHandle, db_handitem* hatch);
	void CreateAssociations(db_handitem *hatch);

	// When a hatch entitiy is initially created we want to make sure duplicate entries are not
	// added when more than one path uses the same boundary object.
	void CreateAssocNoDups(db_handitem *hatch);

	bool operator == (const db_hatchboundobjslink &objlist) const;

	//copies input string so method user is resposible for handeling memory destruction of input value
	bool addBoundaryObj(char *newObj);
	bool addBoundaryObj(db_handitem *newObj);

	bool addBoundaryFixup(db_drawing *dp,db_objhandle newObj);
	bool addBoundaryFixup(void ***fixuploc);

	//returns true if removed, false if not found
	bool removeBoundaryObj(const db_objhandle& obj);
	bool removeBoundaryObj(db_handitem *obj);
	//returns true if found
	bool findBoundaryObj(const db_objhandle &objhandle) const;

	//Input current position pointer.  If position is NULL the position returned is the first link.
	//If input postion is last link the returned position is NULL
	//The character string is just a pointer to the handle and should not be allocated or deleted
	//If the input position is the last position newObj returns NULL
	//True is returned if newObj is filled otherwise false
	bool getNext(db_hatchboundobjs **position, db_objhandle *objHand);
	bool getNext(db_hatchboundobjs **position, db_handitem **newObj);

	//Input current position pointer.  If position is NULL the position returned is the first link.
	//If input postion is last link the returned position is NULL
	//True is returned if newObj is filled otherwise false
	bool getNextObj(db_hatchboundobjs **position);

protected:
	db_hatchboundobjs *m_first;
	db_hatchboundobjs *m_last;
	db_drawing *m_drawing;
};

class DB_CLASS db_hatchpathlink {

  public:
    db_hatchpathlink() : boundObjs(NULL) {flags=0; nedges=0; edge=NULL; numBoundObjs=0;}
	db_hatchpathlink(db_hatchpathlink &pathlink);

    ~db_hatchpathlink()
		{
			if (edge)
			{
				delete [] edge;
				edge = NULL;
			}

			if (boundObjs)
			{
				delete boundObjs;
				boundObjs = NULL;
			}
		}

	void CleanAssociations (db_objhandle &hatchHandle, db_handitem* hatch)
		{if (boundObjs) boundObjs->CleanAssociations(hatchHandle, hatch);}

	bool operator == (const db_hatchpathlink &pathlist) const
	{
		if (flags != pathlist.flags || nedges != pathlist.nedges || numBoundObjs != pathlist.numBoundObjs)
			return false;

		for (int i = 0; i < nedges; i++)
			if (edge[i] != pathlist.edge[i])
				return false;

		return boundObjs == pathlist.boundObjs;
	}

	bool operator != (const db_hatchpathlink &pathlist) const {return !(*this == pathlist);}

	// note this does not copy associations since there will be no reactor back to the new path
	void operator = (const db_hatchpathlink& edge);


    int	flags;					/*  92 */
	int	nedges;					/*  93 (We count a pline as 1 edge; not used */
							    /*    for pline form in entget/entmake.) */
    db_hatchedgelink *edge;		/*  nedges of them. */
	int	numBoundObjs;			//  97 number of source boundary objects
	db_hatchboundobjslink *boundObjs;
};

class DB_CLASS db_hatchpatlinelink {

  public:

    db_hatchpatlinelink(void) {
        ang=base[0]=base[1]=offset[0]=offset[1]=0.0;
        ndashes=0; dash=NULL;
    }

	~db_hatchpatlinelink(void) { if (dash) delete [] dash; }

	void operator = (const db_hatchpatlinelink &patList);
	bool operator == (const db_hatchpatlinelink &patList) const;
	bool operator != (const db_hatchpatlinelink &patList) const {return !(*this == patList);}

    sds_real  ang;       /* 53    */
    sds_real  base[2];   /* 43,44 */
    sds_real  offset[2]; /* 45,46 */
    int       ndashes;   /* 79    */
    sds_real *dash;      /* 49 (ndashes of them) */
};


class DB_CLASS db_hatch : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:
    db_hatch(void);
    db_hatch(db_drawing *dp);
    db_hatch(const db_hatch &sour);  /* Copy constructor */
   ~db_hatch(void);

	int compareEntity(db_BinChange **begll, db_BinChange **curll, db_hatch *newHatch);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }


    bool get_2(char **p) { *p=patname; return true; }
    bool get_10(sds_point p) { p[0]=p[1]=0.0; p[2]=elev; return true; }
    bool get_10(sds_point *p, int n) {  /* The seed array */
        int fi1;
        if (p!=NULL) {
            for (fi1=0; fi1<n; fi1++) {
                if (fi1<nseeds) {
                    p[fi1][0]=seed[fi1][0];
                    p[fi1][1]=seed[fi1][1];
                    p[fi1][2]=elev;
                } else p[fi1][0]=p[fi1][1]=p[fi1][2]=0.0;
            }
        }
        return true;
    }
    bool get_41(sds_real *p) { *p=scale    ; return true; }
    bool get_47(sds_real *p) { *p=pixelsz  ; return true; }
    bool get_52(sds_real *p) { *p=ang      ; return true; }
    bool get_70(int      *p) { *p=solidfill; return true; }
    bool get_71(int      *p) { *p=assoc    ; return true; }
    bool get_75(int      *p) { *p=style    ; return true; }
    bool get_76(int      *p) { *p=pattype  ; return true; }
    bool get_77(int      *p) { *p=isdouble ; return true; }
    bool get_78(int      *p) { *p=npatlines; return true; }
    bool get_91(int      *p) { *p=npaths   ; return true; }
    bool get_98(int      *p) { *p=nseeds   ; return true; }

    bool set_2(char* p) { db_astrncpy(&patname,p,-1); return true; }
    bool set_10(sds_point p) { elev=p[2]; return true; }
    bool set_10(sds_point *p, int n) {  /* Sets nseeds, too. */
        int fi1;
        delete [] seed; seed=NULL; nseeds=0;
        if (p!=NULL && n>0) {
            nseeds=n;
            seed=new sds_real[nseeds][2];
            for (fi1=0; fi1<nseeds; fi1++) {
                seed[fi1][0]=p[fi1][0];
                seed[fi1][1]=p[fi1][1];
            }
        }
        return true;
    }
    bool set_41(sds_real p) { scale=p; return true; }
    bool set_47(sds_real p) { pixelsz=p; return true; }
    bool set_52(sds_real p) { ang=p; ic_normang(&ang,NULL); return true; }
    bool set_70(int p) { solidfill=(p!=0); return true; }
    bool set_71(int p) { assoc=(p!=0); return true; }
    bool set_75(int p) { style=(p<0) ? 0 : ((p>2) ? 2 : p); return true; }
    bool set_76(int p) { pattype=(p<0) ? 0 : ((p>2) ? 2 : p); return true; }
    bool set_77(int p) { isdouble=(p!=0); return true; }

	// additional seed-related functions
	bool set_seed(sds_point p, int i);
	bool get_seed(sds_point p, int i);
	bool del_seed(int i);


               /***** THE patline[] ARRAY GETTER *****/

  /* Get patline[pidx]'s values, including a ptr to patline[pidx].dash[]: */
    void get_patline(
        int        pidx,
        sds_real  *angp,
        sds_real  *basexp,
        sds_real  *baseyp,
        sds_real  *offsetxp,
        sds_real  *offsetyp,
        int       *ndashesp,
        sds_real **dashpp);

	db_hatchpatlinelink* ret_patline(int i) {if (i < 0 || i > npatlines )
														return NULL;
													return &(patline[i]);}


              /***** THE patline[] ARRAY SETTERS *****/

  /* Allocate patline[] and set npatlines: */
    bool set_78(int p);
  /* Set patline[pidx]'s values and allocate patline[pidx].dash[]: */
    void set_patline(
        int      pidx,
        sds_real ang,
        sds_real basex,
        sds_real basey,
        sds_real offsetx,
        sds_real offsety,
        int      ndashes);
  /* Set patline[lidx].dash[didx]: */
    void set_patlinedash(int pidx, int didx, sds_real dashval);

	// intended for undo/redo sets the pattern line to the pattern
	void set_patline(int index, db_hatchpatlinelink *patLine);

                /***** THE path[] ARRAY GETTERS *****/

  /* Get the path[pidx] members (except the edge[] array and the boundObjs array): */
    void get_path(int pidx, int *flagsp, int *nedgesp, int *numBoundaryObjs);
	db_hatchpathlink * ret_path(int i) {if (i < 0 || i > npaths )
														return NULL;
													return &(path[i]);}


  /* Get path objects in the aggregate */
	void get_pathedgeline(int pidx, int eidx, db_hatchlinelink * & pLine);
	void get_pathedgearc(int pidx, int eidx, db_hatcharclink * & pArc);
	void get_pathedgeellarc(int pidx, int eidx, db_hatchellarclink * & pEllarc);
	void get_pathedgespline(int pidx, int eidx, db_hatchsplinelink * & pSpline);
	void get_pathedgepline(int pidx, db_hatchplinelink * & pPolyline);


  // Get the path[pidx] bound objects. Returns bound objects in boundObjs - does not deallocate anything
  // passed in as a boundObjs
	void get_pathboundobj(int pidx, db_hatchboundobjslink **boundObjs) {*boundObjs = path[pidx].boundObjs;}

  /* Return path[pidx].edge[eidx].type: */
    int ret_pathedgetype(int pidx, int eidx);
  /* Get the path[pidx].edge[eidx].u.line members: */
    void get_pathedgeline(
        int       pidx,
        int       eidx,
        sds_real *p0xp,
        sds_real *p0yp,
        sds_real *p1xp,
        sds_real *p1yp);
  /* Get the path[pidx].edge[eidx].u.arc members: */
    void get_pathedgearc(
        int       pidx,
        int       eidx,
        sds_real *centxp,
        sds_real *centyp,
        sds_real *radp,
        sds_real *sangp,
        sds_real *eangp,
        int      *ccwp);
  /* Get the path[pidx].edge[eidx].u.ellarc members: */
    void get_pathedgeellarc(
        int       pidx,
        int       eidx,
        sds_real *centxp,
        sds_real *centyp,
        sds_real *smavectxp,
        sds_real *smavectyp,
        sds_real *min_majp,
        sds_real *sparamp,
        sds_real *eparamp,
        int      *ccwp);
  /* Get the path[pidx].edge[eidx].u.spline members -- including a ptr */
  /* to the knot[] array, but excluding the control array: */
    void get_pathedgespline(
        int       pidx,
        int       eidx,
        int      *rationalp,
        int      *periodicp,
        int      *degp,
        int      *nknotsp,
        int      *ncptsp,
        sds_real **knotpp);
  /* Get the path[pidx].edge[eidx].u.spline->control[cidx] point: */
    void get_pathedgesplinecontrol(
        int       pidx,
        int       eidx,
        int       cidx,
        sds_real *ptxp,
        sds_real *ptyp);

  /* Get the path[pidx].edge[eidx].u.spline->control[cidx] weight: */
    void get_pathedgesplineweight(int pidx, int eidx, int cidx, sds_real *wtp);

  /* Get the path[pidx].edge[0].u.pline members --  */
  /* excluding the vert[] array: */
  /* A polyline edge is only possible when it is the only edge present so no edge needs to be specified*/
    void get_pathedgepline(
        int  pidx,
        int *closedp,
        int *hasbulgesp,
        int *nvertsp);
  /* Get the path[pidx].edge[0].u.pline->vert[vidx] members: */
  /* A polyline edge is only possible when it is the only edge present so no edge needs to be specified*/
    void get_pathedgeplinevert(
        int  pidx,
        int  vidx,
        sds_real *ptxp,
        sds_real *ptyp,
        sds_real *bulgep);

                /***** THE path[] ARRAY SETTERS *****/

  /* Allocate path[] and set npaths: */
    bool set_91(int p);
  /* Set path[pidx]'s values and allocate path[pidx].edge[]*/
    void set_path(int pidx, int flags, int nedges);

	// intended for undo/redo sets the path to the path link at this position
	void set_path(int index, db_hatchpathlink *path);

	// Set path[pidx]'s boundary objects list
	// *** Intended to be used from file load where associations are already set up
	// Note that a pointer to the objsList is stored in this class and will be destroyed on
	// destruction or when a new objsList is assigned.  The user should not destroy this
	// object.  Also, if the numObjs is 0 the objsList should be NULL.
	void set_pathboundaryobj(int pidx, int numObjs, db_hatchboundobjslink *objsList);

	// Set path[pidx]'s boundary objects list
	// *** Intended to be used when a hatch is created/modified as the associations are cleaned and set
	// Note that a pointer to the objsList is stored in this class and will be destroyed on
	// destruction or when a new objsList is assigned.  The user should not destroy this
	// object.  Also, if the numObjs is 0 the objsList should be NULL.

	// The last parameter allow dups is intended to be set to true on initial hatch creation. When
	// a hatch entitiy is created we want to make sure duplicate entries are not added when more
	// than one path uses the same boundary object.  When hatches are copied for undo, duplicates are
	// needed, so the association defaults to true;

	void set_pathBoundaryAndAssociate(int pidx, int numObjs, db_hatchboundobjslink *objsList, bool allowDups = TRUE);

  /* This is a utility for the setters below: */
    void deledge(int pidx, int eidx);
  /* Set path[pidx].edge[eidx] for a line edge: */
    void set_pathedgeline(
        int pidx,
        int eidx,
        sds_real p0x,
        sds_real p0y,
        sds_real p1x,
        sds_real p1y);
  /* Set path[pidx].edge[eidx] for an arc edge: */
    void set_pathedgearc(
        int      pidx,
        int      eidx,
        sds_real centx,
        sds_real centy,
        sds_real rad,
        sds_real sang,
        sds_real eang,
        int      ccw);
  /* Set path[pidx].edge[eidx] for an elliptic arc edge: */
    void set_pathedgeellarc(
        int      pidx,
        int      eidx,
        sds_real centx,
        sds_real centy,
        sds_real smavectx,
        sds_real smavecty,
        sds_real min_maj,
        sds_real sparam,
        sds_real eparam,
        int      ccw);
  /* Set path[pidx].edge[eidx] for a spline edge. */
  /* The knot[] and control[] arrays get allocated -- but not filled. */
    void set_pathedgespline(
        int pidx,
        int eidx,
        int rational,
        int periodic,
        int deg,
        int nknots,
        int ncpts);
  /* Set path[pidx].edge[eidx].u.spline->knot[kidx]: */
    void set_pathedgesplineknot(
        int      pidx,
        int      eidx,
        int      kidx,
        sds_real knotval);
  /* Set the path[pidx].edge[eidx].u.spline->control[cidx] members: */
    void set_pathedgesplinecontrol(
        int      pidx,
        int      eidx,
        int      cidx,
        sds_real ptx,
        sds_real pty);

	/* Get the path[pidx].edge[eidx].u.spline->control[cidx] weight: */
    void set_pathedgesplineweight(
        int       pidx,
        int       eidx,
        int       cidx,
        sds_real  wt);

  /* Set path[pidx].edge[0] for a pline edge. */
  /* The vert[] array gets allocated -- but not filled. */
    void set_pathedgepline(
        int pidx,
        int closed,
        int hasbulges,
        int nverts);
  /* Set the path[pidx].edge[0].u.pline->vert[vidx] members: */
    void set_pathedgeplinevert(
        int      pidx,
        int      vidx,
        sds_real ptx,
        sds_real pty,
        sds_real bulge);

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

	virtual bool ReactorUpdate (db_objhandle &reactor);

	//Removes associations from hatch and cleans up associations from boundary object to hatch
	//some associated entities display a message when the association is broken. FALSE to turn off.
	virtual void BreakAssociation(bool dispMessage = true);

	//break the association that is input then call break associations to break other associations (avoids
	//circular reference)
	virtual void BreakAssociation(db_handitem *entity, bool dispMessage = true);

	//Basically, it checks if the associated entities are in the selection set. Some entities like hatch
	//will need to implement this function so it will break the assoc if the boundary is not in the selection
	//set.  Other entities like leader do not break the association on commands like rotate so they do not
	//need implement.
	virtual void CheckAssociationInSet(db_drawing *drawing, const sds_name selSet);

	virtual bool notifyDeleted(db_handitem* hip);

	// return true if the given object belongs to boundary path
	bool findBoundaryObj(const db_objhandle &objhandle) const;

	// used when all paths should be removed  - helper for set_91 and destructor
	void RemovePaths();

  private:

    char                *patname;    /*   2 */
    sds_real             elev;       /*  10 (Don't confuse with seed[].) */
    sds_real           (*seed)[2];   /*  10 (Don't confuse with elev.) (nseeds of them.) */
    sds_real             scale;      /*  41 */
    sds_real             pixelsz;    /*  47 */
    sds_real             ang;        /*  52 */
    char                 solidfill;  /*  70 */
    int                  assoc;      /*  71 */
    int                  style;      /*  75 */
    int                  pattype;    /*  76 */
    char                 isdouble;   /*  77 */
    int                  npatlines;  /*  78 */
    db_hatchpatlinelink *patline;    /*     (npatlines of them) */
    int                  npaths;     /*  91 */
    db_hatchpathlink    *path;       /*     (npaths of them) */
    int                  nseeds;     /*  98 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_image : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_image(void);
    db_image(db_drawing *dp);
    db_image(const db_image &sour);  /* Copy constructor */
   ~db_image(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    char *ret_grdata(void)   { return grdata; }
    int   ret_grdatasz(void) { return grdatasz; }
    void set_grdata(char *pgrdata, int pgrdatasz) {  /* Also sets grdatasz. */
        delete [] grdata; grdata=NULL; grdatasz=0;
        if (pgrdata!=NULL && pgrdatasz>0) {
            grdatasz=pgrdatasz; grdata=new char[grdatasz];
            memcpy(grdata,pgrdata,grdatasz);
        }
    }

    bool get_10(sds_point p) { DB_PTCPY(p,pt); return true; }
    bool get_11(sds_point p) { DB_PTCPY(p,pixelvect[0]); return true; }
    bool get_12(sds_point p) { DB_PTCPY(p,pixelvect[1]); return true; }
    bool get_13(sds_point p) { p[0]=imagesz[0]; p[1]=imagesz[1]; p[2]=0.0; return true; }
    bool get_14(sds_point *p, int n) {
        int fi1;
        if (p!=NULL) {
            for (fi1=0; fi1<n; fi1++) {
                if (fi1<nclipverts && clipvert!=NULL) {
                    p[fi1][0]=clipvert[fi1][0]; p[fi1][1]=clipvert[fi1][1];
                } else p[fi1][0]=p[fi1][1]=0.0;
                p[fi1][2]=0.0;
            }
        }
        return true;
    }
    bool get_70(int *p) { *p=dispprops; return true; }
    bool get_71(int *p) { *p=boundarytype; return true; }
    bool get_90(int *p) { *p=classver; return true; }
    bool get_91(int *p) { *p=nclipverts; return true; }
    bool get_280(int *p) { *p=clipon; return true; }
    bool get_281(int *p) { *p=brightness; return true; }
    bool get_282(int *p) { *p=contrast; return true; }
    bool get_283(int *p) { *p=fade; return true; }
    bool get_340(db_handitem **p) { *p=defref; return true; }
    bool get_340hand(db_objhandle *p)
        {
		if (defref!=NULL)
			*p=defref->RetHandle();
		else
			p->Nullify();
		return true;
		}
    bool get_360(db_handitem **p) { *p=defreactref; return true; }
    bool get_360hand(db_objhandle *p)
        {
		if (defreactref!=NULL)
			*p=defreactref->RetHandle();
		else
			p->Nullify();
		return true;
		}

    bool set_10(sds_point p) { DB_PTCPY(pt,p); return true; }
    bool set_11(sds_point p) { DB_PTCPY(pixelvect[0],p); return true; }
    bool set_12(sds_point p) { DB_PTCPY(pixelvect[1],p); return true; }
    bool set_13(sds_point p) { imagesz[0]=p[0]; imagesz[1]=p[1]; return true; }
    bool set_14(sds_point *p, int n) {  /* Sets 91, too. */
        int fi1;
        delete [] clipvert; clipvert=NULL; nclipverts=0;
        if (p!=NULL && n>1) {
            nclipverts=n; clipvert=new sds_real[nclipverts][2];
            for (fi1=0; fi1<nclipverts; fi1++)
                { clipvert[fi1][0]=p[fi1][0]; clipvert[fi1][1]=p[fi1][1]; }
        }
        return true;
    }
    bool set_70(int p) { dispprops=p; return true; }
    bool set_71(int p) { boundarytype=(p<2) ? 1 : 2; return true; }
    bool set_90(int p) { classver=p; return true; }
    /* 91 set in set_14() above. */
    bool set_280(int p) { clipon=(p!=0); return true; }
    bool set_281(int p) { brightness=charCast((p<0) ? 0 : ((p>100) ? 100 : p)); return true; }
    bool set_282(int p) { contrast=charCast((p<0) ? 0 : ((p>100) ? 100 : p)); return true; }
    bool set_283(int p) { fade=charCast((p<0) ? 0 : ((p>100) ? 100 : p)); return true; }
    bool set_340(db_handitem *p) { defref=p; return true; }
	bool set_340byhand(db_objhandle p, db_drawing *dp) {
	    db_handitem *thip1;

		if ((thip1=dp->handent(p))==NULL)
			{ db_lasterror=OL_EHANDLE; return false; }
	    defref=thip1; return true;
	}
    bool set_360(db_handitem *p) { defreactref=p; return true; }
	bool set_360byhand(db_objhandle p, db_drawing *dp) {
	    db_handitem *thip1;

		if ((thip1=dp->handent(p))==NULL)
			{ db_lasterror=OL_EHANDLE; return false; }
	    defreactref=thip1; return true;
	}

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

  private:

    sds_point    pt;            /*  10    */
    sds_point    pixelvect[2];  /*  11,12 */
    sds_real     imagesz[2];    /*  13    */
    sds_real   (*clipvert)[2];  /*  14 (Array of 2D points) */
    int          dispprops;     /*  70    */
    int          boundarytype;  /*  71    */
    int          classver;      /*  90    */
    int          nclipverts;    /*  91    */
    char         clipon;        /* 280    */
    char         brightness;    /* 281    */
    char         contrast;      /* 282    */
    char         fade;          /* 283    */
    db_handitem *defref;        /* 340    */
    db_handitem *defreactref;   /* 360    */
    char        *grdata;        /* MarComp needs this */
    int          grdatasz;      /* MarComp needs this */

    void        *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point    extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_insert : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_insert(void);
    db_insert(db_drawing *dp);
    db_insert(const db_insert &sour);  /* Copy constructor */
   ~db_insert(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    db_blocktabrec *ret_bthip(void) { return bthip; }

	bool retAttribsFollow( void )
		{
		return ( afollow != 0 );
		}

    sds_real *retp_10(void) { return pt; }
    sds_real *retp_41(void) { return scl; }
    sds_real *retp_42(void) { return scl+1; }
    sds_real *retp_43(void) { return scl+2; }
    sds_real *retp_50(void) { return &rot; }

    bool get_2(char *p, int maxlen)
        { if (bthip==NULL) *p=0; else bthip->get_2(p,maxlen); return true; }
    bool get_2(char **p) {
        if (bthip!=NULL) bthip->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_2(db_handitem **p) { *p=bthip; return true; }
    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_41(sds_real *p) { *p=scl[0]; return true; }
    bool get_42(sds_real *p) { *p=scl[1]; return true; }
    bool get_43(sds_real *p) { *p=scl[2]; return true; }
    bool get_44(sds_real *p) { *p=colsp; return true; }
    bool get_45(sds_real *p) { *p=rowsp; return true; }
    bool get_50(sds_real *p) { *p=rot; return true; }
    bool get_66(int *p) { *p=afollow; return true; }
    bool get_70(int *p) { *p=ncols; return true; }
    bool get_71(int *p) { *p=nrows; return true; }

	bool set_2(char *p, db_drawing *dp) {
		db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
		    (thip1=dp->findtabrec(DB_BLOCKTAB,p,1))==NULL)
			    { db_lasterror=OL_ESNVALID; return false; }
	    bthip=(db_blocktabrec *)thip1; return true;
	}

	bool set_8(char *p, db_drawing *dp);
    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
    bool set_41(sds_real p) { scl[0]=(icadRealEqual(p,0.0)) ? 1.0 : p; return true; }
    bool set_42(sds_real p) { scl[1]=(icadRealEqual(p,0.0)) ? scl[0] : p; return true; }
    bool set_43(sds_real p) { scl[2]=(icadRealEqual(p,0.0)) ? scl[0] : p; return true; }
    bool set_44(sds_real p) { colsp=p; return true; }
    bool set_45(sds_real p) { rowsp=p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); rot=p; return true; }
    bool set_66(int p) { afollow=(p!=0); return true; }
    bool set_70(int p) { ncols=(short)p< 1 ? 1 : p; return true; }
    bool set_71(int p) { nrows=(short)p< 1 ? 1 : p; return true; }
	bool set_scales(sds_real xscl, sds_real yscl, sds_real zscl) { scl[0]=xscl; scl[1]=yscl; scl[2]=zscl; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Set a clipping spatial_filter object for this insert.
	 * Returns:	None.
	 ********************************************************************************/
	void SetFilter
	(
	 db_handitem* pFilter	// =>
	)
	{
		m_pFilter = pFilter;
	}

	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Get a clipping spatial_filter object for this insert (if any).
	 * Returns:	A pointer to the clipping spatial filter object or NULL.
	 ********************************************************************************/
	db_handitem* GetFilter
	(
	 db_drawing* pDrawing	// =>
	)
	{
		if(m_pFilter)
			return m_pFilter;
		else
			return (m_pFilter = FindFilter(pDrawing));
	}

	void			DeleteFilter(db_drawing* pDrawing);
	db_handitem*	CreateNewFilter(db_drawing* pDrawing);

  private:

	db_handitem*	FindFilter(db_drawing* pDrawing);


    db_blocktabrec *bthip; /*   2 */
    sds_point    pt;       /*  10 */
    sds_real     scl[3];   /*  41 - 43 */
    sds_real     colsp;    /*  44 */
    sds_real     rowsp;    /*  45 */
    sds_real     rot;      /*  50 */
    char         afollow;  /*  66 */
    short          ncols;    /*  70 */
    short          nrows;    /*  71 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
	db_handitem*	m_pFilter;		// clipping spatial_filter object
};


class DB_CLASS db_leader : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_leader(void);
    db_leader(db_drawing *dp);
    db_leader(const db_leader &sour);  /* Copy constructor */
   ~db_leader(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_point *retp_10array(void) { return vert; }
    sds_real *retp_10(int idx)
        { return (idx>-1 && idx<nverts && vert!=NULL) ? vert[idx] : NULL; }

    bool get_3(char *p, int maxlen)
        { if (dship==NULL) *p=0; else dship->get_2(p,maxlen); return true; }
    bool get_3(char **p) {
        if (dship!=NULL) dship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_3(db_handitem **p) { *p=dship; return true; }
    bool get_10(sds_point *p, int n) {  /* WARNING: Fills an array. */
        int fi1;

        if (n>nverts) n=nverts;
        if (vert!=NULL && n>0) {
            for (fi1=0; fi1<n; fi1++) {
                p[fi1][0]=vert[fi1][0];
                p[fi1][1]=vert[fi1][1];
                p[fi1][2]=vert[fi1][2];
            }
        }
        return true;
    }
    bool get_40(sds_real *p) { *p=antextht; return true; }
    bool get_41(sds_real *p) { *p=antextwd; return true; }
    bool get_71(int *p) { *p=arrow;  return true; }
    bool get_72(int *p) { *p=pathtype;  return true; }
    bool get_73(int *p) { *p=creation;  return true; }
    bool get_74(int *p) { *p=hookdir;  return true; }
    bool get_75(int *p) { *p=hook;  return true; }
    bool get_76(int *p) { *p=nverts; return true; }
    bool get_77(int *p) { *p=bbcolor;  return true; }
    bool get_211(sds_point p) { p[0]=hdir[0]; p[1]=hdir[1]; p[2]=hdir[2]; return true; }
	//Bugzilla No. 78044 ; 25-03-2002 [
    //bool get_212(sds_point p) { p[0]=anoffset[0]; p[1]=anoffset[1]; p[2]=anoffset[2]; return true; }
	bool get_212(sds_point p) { p[0]=offsettoblockinspt[0]; p[1]=offsettoblockinspt[1]; p[2]=offsettoblockinspt[2]; return true; }
	//Bugzilla No. 78013 ; 01-04-2002
    //bool get_txtoffset(sds_point p) { p[0]=txtoffset[0]; p[1]=txtoffset[1]; p[2]=txtoffset[2]; return true; }
	bool get_213(sds_point p) { p[0]=txtoffset[0]; p[1]=txtoffset[1]; p[2]=txtoffset[2]; return true; }
    //bool get_offsettoblockinspt(sds_point p) { p[0]=offsettoblockinspt[0]; p[1]=offsettoblockinspt[1]; p[2]=offsettoblockinspt[2]; return true; }
    bool get_340(db_handitem **p) { *p=annothip; return true; }
    bool get_340hand(db_objhandle *p)
        {
		if (annothip!=NULL)
			*p=annothip->RetHandle();
		else
			p->Nullify();
		return true;
		}
	void ** retp_annothip(void){return (void**) &annothip;}

	bool set_3(char *p, db_drawing *dp) {
		db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
	        (thip1=dp->findtabrec(DB_DIMSTYLETAB,p,1))==NULL)
	            { db_lasterror=OL_ESNVALID; return false; }
		dship=thip1;
		return true;
	}

	bool set_3(db_handitem *p) { dship=p; return true; }
    bool set_10(sds_point *p, int n) {  /* WARNING: Sets an array. */
        int fi1;

        delete [] vert; vert=NULL; nverts=0;
        if (n>0) {
            vert=new sds_point[n]; nverts=n;
            for (fi1=0; fi1<n; fi1++) {
                vert[fi1][0]=p[fi1][0];
                vert[fi1][1]=p[fi1][1];
                vert[fi1][2]=p[fi1][2];
            }
        }
        return true;
    }
    bool set_40(sds_real p) { antextht=p; return true; }
    bool set_41(sds_real p) { antextwd=p; return true; }
    bool set_71(int p) { arrow=(p!=0); return true; }
    bool set_72(int p) { pathtype=(p!=0); return true; }
    bool set_73(int p) { creation=charCast((p>-1 && p<4) ? p : 3); return true; }
    bool set_74(int p) { hookdir=(p!=0); return true; }
    bool set_75(int p) { hook=(p!=0); return true; }
//  bool set_76(int p) { nverts=p; return true; }  Set via set_10().
    bool set_77(int p) { bbcolor=(p>-1 && p<257) ? p : DB_BLCOLOR; return true; }
    bool set_211(sds_point p) { hdir[0]=p[0]; hdir[1]=p[1]; hdir[2]=p[2]; return true; }
    //Bugzilla No. 78044 ; 25-03-2002
	//bool set_212(sds_point p) { anoffset[0]=p[0]; anoffset[1]=p[1]; anoffset[2]=p[2]; return true; }
	bool set_212(sds_point p) { offsettoblockinspt[0]=p[0]; offsettoblockinspt[1]=p[1]; offsettoblockinspt[2]=p[2]; return true; }
	//Bugzilla No. 78013 ; 01-04-2002
    //bool set_txtoffset(sds_point p) { txtoffset[0]=p[0]; txtoffset[1]=p[1]; txtoffset[2]=p[2]; return true; }
	bool set_213(sds_point p) { txtoffset[0]=p[0]; txtoffset[1]=p[1]; txtoffset[2]=p[2]; return true; }
    //bool set_offsettoblockinspt(sds_point p) { offsettoblockinspt[0]=p[0]; offsettoblockinspt[1]=p[1]; offsettoblockinspt[2]=p[2]; return true; }
    bool set_340(db_handitem *p);

	bool set_340byhand(db_objhandle p, db_drawing *dp) {
	    db_handitem *thip1;

		if ((thip1=dp->handent(p))==NULL)
			{ db_lasterror=OL_EHANDLE; return false; }

		if (annothip)
			BreakAssociation();

	    annothip=thip1; return true;
	}

	// when creating by drawing (instead of load) need to add association
	bool set_340andAssociate(db_handitem *hip)
			{
			if (set_340(hip)) {
				//Add assocation from hip back to hatch
				if(hip && m_createAssocFlag)
					hip->addReactor(this->RetHandle(), this, ReactorItem::HARD_POINTER);
				return true;
			}
			return false;
			}

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

	void BreakAssociation(db_handitem * entity, bool dispMessage = true);

	void BreakAssociation(bool dispMessage = true);

	// EBATECH(watanabe)-[update leader according to modifying annotation
	virtual bool ReactorUpdate (db_objhandle &reactor);
	//]- EBATECH(watanabe)

	virtual bool notifyDeleted(db_handitem* hip);

	//Bugzilla No. 78013 ; 01-04-2002
	void Calculate213();
	bool GetDimvars(sds_real& dDimgap, sds_real& dDimscale,int& iDimtad);

  private:

    class db_handitem *dship;    /*   3 */
    sds_point         *vert;     /*  10  (an array of nverts pts) */
    sds_real           antextht; /*  40 */
    sds_real           antextwd; /*  41 */
    char               arrow;    /*  71 */
    char               pathtype; /*  72 */
    char               creation; /*  73 */
    char               hookdir;  /*  74 */
    char               hook;     /*  75 */
    int                nverts;   /*  76 */
    int                bbcolor;  /*  77 */
    sds_point          hdir;     /* 211 */
	//Bugzilla No. 78044 ; 25-03-2002
    //sds_point          anoffset; /* 212 */
    class db_handitem *annothip; /* 340 */
	sds_point			txtoffset; /* 213 */
	sds_point			offsettoblockinspt; /*212*/
    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */

};


class DB_CLASS db_line : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_line(void);
    db_line(db_drawing *dp);
    db_line(const db_line &sour);  /* Copy constructor */
	~db_line(void);

	virtual int check(db_drawing* pDrawing);
    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return pt[0]; }
    sds_real *retp_11(void) { return pt[1]; }

    bool get_10(sds_point p) { p[0]=pt[0][0]; p[1]=pt[0][1]; p[2]=pt[0][2]; return true; }
    bool get_11(sds_point p) { p[0]=pt[1][0]; p[1]=pt[1][1]; p[2]=pt[1][2]; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }

    bool set_10(sds_point p) { pt[0][0]=p[0]; pt[0][1]=p[1]; pt[0][2]=p[2]; return true; }
    bool set_11(sds_point p) { pt[1][0]=p[0]; pt[1][1]=p[1]; pt[1][2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real   thick;  /*  39 */

    sds_point  pt[2];  /*  10, 11 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_lwpolyline : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_lwpolyline(void);
    db_lwpolyline(db_drawing *dp);
    db_lwpolyline(const db_lwpolyline &sour);  /* Copy constructor */
	~db_lwpolyline(void);

	virtual int check(db_drawing* pDrawing);
    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur)
		{
		DB_PTCPY(ll,extent[0]);
		DB_PTCPY(ur,extent[1]);
		}
    void  set_extent(sds_point ll, sds_point ur)
		{
		DB_PTCPY(extent[0],ll);
		DB_PTCPY(extent[1],ur);
		}

    bool haswidths(void) { return (wid  !=NULL); }  /* No virtual */
    bool hasbulges(void) { return (bulge!=NULL); }  /* No virtual */

    bool get_10(sds_point p, int idx)
		{
        if (vert!=NULL && nverts>0 && idx>-1 && idx<nverts)
            {
			p[0]=vert[idx][0];
			p[1]=vert[idx][1];
			}
		else
			p[0]=p[1]=0.0;
        p[2]=elev;
        return true;
		}

    double *ret_10(int idx)
		{
        if (vert!=NULL && nverts>0 && idx>-1 && idx<nverts)
			return vert[idx];
		else
			return NULL;
		}

	bool get_10(sds_point *p, int n)   /* WARNING: Fills an array. */
		{
        int fi1;

        if (n>nverts) n=nverts;
        if (vert!=NULL && n>0)
			{
            for (fi1=0; fi1<n; fi1++)
				{
                p[fi1][0]=vert[fi1][0];
                p[fi1][1]=vert[fi1][1];
                p[fi1][2]=elev;
				}
	        }
        return true;
	    }
    bool get_38(sds_real *p) { *p=elev; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }
    bool get_40(sds_real *p, int idx)
		{
        if (wid!=NULL && nverts>0 && idx>-1 && idx<nverts)
			*p=wid[idx][0];
        else
			*p=constwid;
        return true;
	    }
    bool get_40array(sds_real *p, int n) 	/* WARNING: Fills an array. */
		{
        int fi1;

        if (wid==NULL)
			{
            for (fi1=0; fi1<n; fi1++)
				p[fi1]=constwid;
			}
		else
			{
            if (n>nverts)
				n=nverts;
            for (fi1=0; fi1<n; fi1++)
				p[fi1]=wid[fi1][0];
			}
        return true;
	    }
    bool get_41(sds_real *p, int idx)
		{
        if (wid!=NULL && nverts>0 && idx>-1 && idx<nverts)
			*p=wid[idx][1];
        else
			*p=constwid;
        return true;
		}
    bool get_41array(sds_real *p, int n)   /* WARNING: Fills an array. */
        {
		int fi1;

        if (wid==NULL)
			{
            for (fi1=0; fi1<n; fi1++)
				p[fi1]=constwid;
			}
		else
			{
            if (n>nverts)
				n=nverts;
            for (fi1=0; fi1<n; fi1++)
				p[fi1]=wid[fi1][1];
			}
        return true;
	    }
    bool get_42(sds_real *p, int idx)
		{
        if (bulge!=NULL && nverts>0 && idx>-1 && idx<nverts)
			*p=bulge[idx];
        else
			*p=0.0;
        return true;
		}
    bool get_42array(sds_real *p, int n)  /* WARNING: Fills an array. */
		{
		int fi1;

        if (bulge==NULL)
			{
            memset(p,0,n*sizeof(p[0]));
			}
		else
			{
            if (n>nverts)
				n=nverts;
            for (fi1=0; fi1<n; fi1++)
				p[fi1]=bulge[fi1];
			}
        return true;
	    }
    bool get_43(sds_real *p) { *p=constwid; return true; }
    bool get_70(int *p) { *p=(unsigned char)flags; return true; } // Bugizlla#78524 2003/5/15
    //bool get_70(int *p) { *p=flags; return true; }
    bool get_90(int *p) { *p=nverts; return true; }

    unsigned char ret_70(void) { return flags; }
    long ret_90(void) { return nverts; }
	double ret_43(void) { return constwid; }
	double ret_38(void) { return elev; }
	double ret_39(void) { return thick; }

	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

    /*
    **  WARNING:  The setters are very order-dependent.
    **  The array-setting set_10() deletes the wid and bulge arrays
    **  and zeros flags (except for the PLINEGEN bit).  set_43()
    **  deletes the wid array.  To define a new LWPOLYLINE, you should
    **  probably call the setters in the following order:
    **
    **      set_10() (array form)
    **      set_43()
    **      set_40(), set_41(), set_42() (array forms, any order)
    **      The others, in any order
    **      (Note there's no set_90(); the set_10() handles that.)
    */
    bool set_10(sds_point p, int idx)   /* Specific point */
		{
		if (vert!=NULL && nverts>0 && idx>-1 && idx<nverts)
            {
			vert[idx][0]=p[0];
			vert[idx][1]=p[1];
			vert[idx][2]=0.0;  // to satisfy Purify
//			set_38(p[2]);	/*D.Gavrilov*/// NOTE that some functions must set
							// an elevation (38) by calculated z-coord of one of
							// lwpline's vertices (for example cmd_explode_makesubents
							// and cmd_array_func). Otherwise this z-coord will be lost.
							// BUT we shouldn't do it here generally.
			}
        return true;
	    }
    bool set_10(sds_point *p, int n)
		{
        /* WARNING: This one nukes everything to do with the vertices. */
        int fi1;

        delete [] vert; vert=NULL; nverts=0;
        delete [] wid; wid=NULL; delete [] bulge; bulge=NULL;
        flags&=0x80;  /* Zero all but PLINEGEN. */

        if (n<2)
			{
            nverts=2;
			vert=new sds_point[nverts];
            if (n==1)
				{
                vert[0][0]=vert[1][0]=p[0][0];
                vert[0][1]=vert[1][1]=p[0][1];
				vert[0][2]=0.0;
				}
			else
				vert[0][0]=vert[0][1]=vert[1][0]=vert[1][1]=vert[0][2]=vert[1][2]=0.0;
			}
		else
			{
            nverts=n;
			vert=new sds_point[nverts];
            for (fi1=0; fi1<nverts; fi1++)
                {
				vert[fi1][0]=p[fi1][0];
				vert[fi1][1]=p[fi1][1];
				vert[fi1][2]=0.0;
				}
	        }

        return true;
    }
    bool set_38(sds_real p) { elev=p;  return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p, int idx)  /* Specific vertex */
		{
        int fi1;

        if (nverts>0 && idx>-1 && idx<nverts)
			{
            if (wid==NULL)
				{
                if (p!=constwid)
					{
                    wid=new sds_real[nverts][2];
                    for (fi1=0; fi1<nverts; fi1++)
                        wid[fi1][0]=wid[fi1][1]=constwid;
                    wid[idx][0]=p;
					}
	            }
			else
				{
                wid[idx][0]=p;
                for (fi1=0; fi1<nverts && wid[fi1][0]==wid[0][0] &&
                    wid[fi1][1]==wid[0][0]; fi1++);
                if (fi1>=nverts)
					{
					constwid=wid[0][0];
					delete [] wid;
					wid=NULL;
					}
	            }
			}
        return true;
		}
    bool set_40(sds_real *p, int n)   /* Array */
		{
        int fi1;

        if (nverts>0 && n>0)
			{
            if (wid==NULL)
				{
                wid=new sds_real[nverts][2];
                for (fi1=0; fi1<nverts; fi1++)
                    wid[fi1][0]=wid[fi1][1]=constwid;
				}
            if (n>nverts)
				n=nverts;
            for (fi1=0; fi1<n; fi1++)
				wid[fi1][0]=p[fi1];
            for (fi1=0; fi1<nverts && wid[fi1][0]==wid[0][0] &&
                wid[fi1][1]==wid[0][0]; fi1++);
            if (fi1>=nverts)
				{
				constwid=wid[0][0];
				delete [] wid;
				wid=NULL;
				}
	        }
        return true;
		}
    bool set_41(sds_real p, int idx)  /* Specific vertex */
		{
        int fi1;

        if (nverts>0 && idx>-1 && idx<nverts)
			{
            if (wid==NULL)
				{
                if (p!=constwid)
					{
                    wid=new sds_real[nverts][2];
                    for (fi1=0; fi1<nverts; fi1++)
                        wid[fi1][0]=wid[fi1][1]=constwid;
                    wid[idx][1]=p;
					}
	            }
			else
				{
                wid[idx][1]=p;
                for (fi1=0; fi1<nverts && wid[fi1][0]==wid[0][0] &&
                    wid[fi1][1]==wid[0][0]; fi1++);
                if (fi1>=nverts)
					{
					constwid=wid[0][0];
					delete [] wid;
					wid=NULL;
					}
				}
	        }
        return true;
		}
    bool set_41(sds_real *p, int n)   /* Array */
		{
        int fi1;

        if (nverts>0 && n>0)
			{
            if (wid==NULL)
				{
                wid=new sds_real[nverts][2];
                for (fi1=0; fi1<nverts; fi1++)
                    wid[fi1][0]=wid[fi1][1]=constwid;
				}
            if (n>nverts)
				n=nverts;
            for (fi1=0; fi1<n; fi1++)
				wid[fi1][1]=p[fi1];
            for (fi1=0; fi1<nverts && wid[fi1][0]==wid[0][0] &&
                wid[fi1][1]==wid[0][0]; fi1++);
            if (fi1>=nverts)
				{
				constwid=wid[0][0];
				delete [] wid;
				wid=NULL;
				}
	        }
        return true;
		}
    bool set_42(sds_real p, int idx)   /* Specific vertex */
		{
        int fi1;

        if (nverts>0 && idx>-1 && idx<nverts)
			{
            if (bulge==NULL)
				{
                if (!icadRealEqual(p,0.0))
					{
                    bulge=new sds_real[nverts];
                    memset(bulge,0,nverts*sizeof(bulge[0]));
                    bulge[idx]=p;
					}
				}
			else
				{
                bulge[idx]=p;
                for (fi1=0; fi1<nverts && icadRealEqual(bulge[fi1],0.0); fi1++);
                if (fi1>=nverts)
					{
					delete [] bulge;
					bulge=NULL;
					}
				}
			}
        return true;
	    }
    bool set_42(sds_real *p, int n)   /* Array */
		{
        int fi1;

        if (nverts>0 && n>0)
			{
            if (bulge==NULL)
				{
                bulge=new sds_real[nverts];
                memset(bulge,0,nverts*sizeof(bulge[0]));
				}
            if (n>nverts)
				n=nverts;
            for (fi1=0; fi1<n; fi1++)
				bulge[fi1]=p[fi1];
            for (fi1=0; fi1<nverts && icadRealEqual(bulge[fi1],0.0); fi1++);
            if (fi1>=nverts)
				{
				delete [] bulge;
				bulge=NULL;
				}
			}
        return true;
		}
    bool set_43(sds_real p)   /* WARNING: Deletes the wid array. */
		{
		delete [] wid;
		wid=NULL;
		constwid=p;
		return true;
	    }
    bool set_70(int p)
		{
		flags=(char)(p & 0x81);
		return true;
		}
    /* No set_90().  nverts is set by set_10() (array form). */


  private:
	
    unsigned char        flags;     //  70    ACAD's simple ones -- not Marcomp's
    int         nverts;    //  90
    sds_real    thick;     //  39
    sds_real    elev;      //  38
    sds_real    constwid;  //  43

    sds_point  (*vert);		//  10     Array of points      (nverts of them), may be NULL
							// z coords are really just placeholders for use by SDS_drag*(), which use ic_ptcpy all the time
							// they should not be relied upon or used -- use the elev field
	sds_real  (*wid)[2];   //  40,41  Array of width-pairs (nverts of them), may be NULL
    sds_real   *bulge;     //  42     Array of bulges      (nverts of them), may be NULL

    void       *disp;      // Disp obj llist (gr_displayobject *)
    sds_point   extent[2]; // The extents of the disp llist; see db_3dface.
};


class db_mlelemlink {

  public:

    db_mlelemlink(void);
   ~db_mlelemlink(void);

    int       nlpars;  /* 74 : # of line paramenters */
    sds_real *lpar;    /* 41 : nlpars line parameters */
    int       napars;  /* 75 : # of area parameters */
    sds_real *apar;    /* 42 : napars area parameters */
};

class db_mlvertlink {

  public:

    db_mlvertlink(void);
   ~db_mlvertlink(void);

    sds_point      pt;       /* 11 */
    sds_point      dir;      /* 12 */
    sds_point      miterdir; /* 13 */
    db_mlelemlink *elem;     /* db_mline::nelems of them */
};


class DB_CLASS db_mline : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_mline(void);
    db_mline(db_drawing *dp);
    db_mline(const db_mline &sour);  /* Copy constructor */
   ~db_mline(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    db_mlvertlink *ret_vert(void) { return vert; }

    bool get_2(char *p, int maxlen)
        { if (mlship==NULL) *p=0; else mlship->get_2(p,maxlen); return true; }
    bool get_2(char **p) {
        if (mlship!=NULL) mlship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_2(db_handitem **p) { *p=mlship; return true; }
    bool get_340name(char *p, int maxlen)
        { if (mlship==NULL) *p=0; else mlship->get_2(p,maxlen); return true; }
    bool get_340name(char **p) {
        if (mlship!=NULL) mlship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_340hand(db_objhandle *p)
        {
		if (mlship!=NULL)
			*p=mlship->RetHandle();
		else
			p->Nullify();
		return true;
		}
    bool get_340(db_handitem **p) { *p=mlship; return true; }

    bool get_10(sds_point p) { DB_PTCPY(p,start); return true; }
    bool get_40(sds_real *p) { *p=scl; return true; }
    bool get_70(int *p) { *p=just;    return true; }
    bool get_71(int *p) { *p=flags; return true; }
    bool get_72(int *p) { *p=nverts;  return true; }
    bool get_73(int *p) { *p=nelems;  return true; }

    /*
    **  HERE COME THE DIFFICULT GETTERS.  READ CAREFULLY.
    **  THE CALLER IS EXPECTED TO HAVE ALREADY ALLOCATED THE ARRAYS
    **  FOR THE ONES THAT FILL ARRAYS.
    */

    /*
    **  11 : Fills array of pts p (nverts of them).
    */
    bool get_11(sds_point *p) {
        int fi1;
        if (vert!=NULL)
            for (fi1=0; fi1<nverts; fi1++) DB_PTCPY(p[fi1],vert[fi1].pt);
        return true;
    }
    /*
    **  12 : Fills array of pts (vectors) p (nverts of them).
    */
    bool get_12(sds_point *p) {
        int fi1;
        if (vert!=NULL)
            for (fi1=0; fi1<nverts; fi1++) DB_PTCPY(p[fi1],vert[fi1].dir);
        return true;
    }
    /*
    **  13 : Fills array of pts (vectors) p (nverts of them).
    */
    bool get_13(sds_point *p) {
        int fi1;
        if (vert!=NULL)
            for (fi1=0; fi1<nverts; fi1++) DB_PTCPY(p[fi1],vert[fi1].miterdir);
        return true;
    }
    /*
    **  74 : Gets the number of line params for vertex vidx, element eidx.
    */
    bool get_74(int *p, int vidx, int eidx) {
        if (vidx<0) vidx=0; else if (vidx>=nverts) vidx=nverts-1;
        if (eidx<0) eidx=0; else if (eidx>=nelems) eidx=nelems-1;
        if (vert!=NULL && vidx>-1 && vert[vidx].elem!=NULL && eidx>-1)
            *p=vert[vidx].elem[eidx].nlpars;
        return true;
    }
    /*
    **  75 : Gets the number of area params for vertex vidx, element eidx.
    */
    bool get_75(int *p, int vidx, int eidx) {
        if (vidx<0) vidx=0; else if (vidx>=nverts) vidx=nverts-1;
        if (eidx<0) eidx=0; else if (eidx>=nelems) eidx=nelems-1;
        if (vert!=NULL && vidx>-1 && vert[vidx].elem!=NULL && eidx>-1)
            *p=vert[vidx].elem[eidx].napars;
        return true;
    }
    /*
    **  41 : Fills array of line params (nlpars of them) for vertex
    **       vidx, element eidx.
    */
    bool get_41(sds_real *p, int vidx, int eidx) {
        if (vidx<0) vidx=0; else if (vidx>=nverts) vidx=nverts-1;
        if (eidx<0) eidx=0; else if (eidx>=nelems) eidx=nelems-1;
        if (vert!=NULL && vidx>-1 && vert[vidx].elem!=NULL && eidx>-1 &&
            vert[vidx].elem[eidx].nlpars>0)
                memcpy(p,vert[vidx].elem[eidx].lpar,
                    vert[vidx].elem[eidx].nlpars*sizeof(sds_real));
        return true;
    }
    /*
    **  42 : Fills array of area params (napars of them) for vertex
    **       vidx, element eidx.
    */
    bool get_42(sds_real *p, int vidx, int eidx) {
        if (vidx<0) vidx=0; else if (vidx>=nverts) vidx=nverts-1;
        if (eidx<0) eidx=0; else if (eidx>=nelems) eidx=nelems-1;
        if (vert!=NULL && vidx>-1 && vert[vidx].elem!=NULL && eidx>-1 &&
            vert[vidx].elem[eidx].napars>0)
                memcpy(p,vert[vidx].elem[eidx].apar,
                    vert[vidx].elem[eidx].napars*sizeof(sds_real));
        return true;
    }


    /* Don't use set_2() to set mlship -- use set_340. */
    bool set_340(db_handitem *p) { mlship=p; return true; }
	bool set_340byhand(db_objhandle p, db_drawing *dp) {
	    db_handitem *thip1;

		if ((thip1=dp->handent(p))==NULL)
			{ db_lasterror=OL_EHANDLE; return false; }
	    mlship=thip1; return true;
	}

    bool set_10(sds_point p) { DB_PTCPY(start,p); return true; }
    bool set_40(sds_real p) { scl=(icadRealEqual(p,0.0)) ? 1.0 : p; return true; }
    bool set_70(int p) { just=(p<0) ? 0 : ((p>2) ? 2 : p); return true; }
	bool set_71(int p) { flags=p; return true; }

    /*
    **  HERE COME THE DIFFICULT SETTERS.  READ CAREFULLY.
    **
    **  THE CALLER BUILDING AN MLINE SHOULD PROBABLY CALL THESE
    **  IN THE FOLLOWING ORDER:
    **
    **    set_72() : Set nverts and alloc the vert array.
    **    set_73() : Set nelems and alloc the elem array in each vert element.
    **    set_11() : Set the vert[].pt       elements via an array.
    **    set_12() : Set the vert[].dir      elements via an array.
    **    set_13() : Set the vert[].miterdir elements via an array.
    **    For each vertex (vidx) {
    **      For each element (eidx) {
    **        set_74() : Set vert[vidx].elem[eidx].nlpars and alloc
    **                     the vert[vidx].elem[eidx].lpar array.
    **        set_41() : Set the vert[vidx].elem[eidx].lpar array.
    **        set_75() : Set vert[vidx].elem[eidx].napars and alloc
    **                     the vert[vidx].elem[eidx].apar array.
    **        set_42() : Set the vert[vidx].elem[eidx].apar array.
    **      }
    **    }
    */
    /* Defs of setters 72-75 had to be moved out of the class def; see db1.cpp. */
    bool set_72(int p);
    bool set_73(int p);
    bool set_11(sds_point *p) {  /* Expects p[] to have at least nverts elements. */
        int fi1;
        for (fi1=0; fi1<nverts; fi1++) DB_PTCPY(vert[fi1].pt,p[fi1]);
        return true;
    }
    bool set_12(sds_point *p) {  /* Expects p[] to have at least nverts elements. */
        int fi1;
        for (fi1=0; fi1<nverts; fi1++) DB_PTCPY(vert[fi1].dir,p[fi1]);
        return true;
    }
    bool set_13(sds_point *p) {  /* Expects p[] to have at least nverts elements. */
        int fi1;
        for (fi1=0; fi1<nverts; fi1++) DB_PTCPY(vert[fi1].miterdir,p[fi1]);
        return true;
    }
    /* Defs of setters 72-75 had to be moved out of the class def; see db1.cpp. */
    bool set_74(int p, int vidx, int eidx);
    bool set_75(int p, int vidx, int eidx);
    bool set_41(sds_real *p, int vidx, int eidx) {
        /* Expects p[] to have at least vert[vidx.elem[eidx].nlpars elements. */
        if (vidx>-1 && vidx<nverts && eidx>-1 && eidx<nelems &&
            vert!=NULL && vert[vidx].elem!=NULL &&
            vert[vidx].elem[eidx].nlpars>0) {

            memcpy(vert[vidx].elem[eidx].lpar,p,
                vert[vidx].elem[eidx].nlpars*sizeof(sds_real));
        }
        return true;
    }
    bool set_42(sds_real *p, int vidx, int eidx) {
        /* Expects p[] to have at least vert[vidx.elem[eidx].napars elements. */
        if (vidx>-1 && vidx<nverts && eidx>-1 && eidx<nelems &&
            vert!=NULL && vert[vidx].elem!=NULL &&
            vert[vidx].elem[eidx].napars>0) {

            memcpy(vert[vidx].elem[eidx].apar,p,
                vert[vidx].elem[eidx].napars*sizeof(sds_real));
        }
        return true;
    }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

  private:

    db_handitem   *mlship;   /*   2,340 : The MLINESTYLE dictionary rec */
    sds_point      start;    /*  10 */
    db_mlvertlink *vert;     /*     : nverts of them */
    sds_real       scl;      /*  40 */
    int            just;     /*  70 */
    int            flags;    /*  71 */
    int            nverts;   /*  72 */
    int            nelems;   /*  73 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};

class DB_CLASS db_ole2frame : public db_entity {
/*
**
**  Need to research.  At least rectangle corners and data buffer.
**
*/

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_ole2frame(void);
    db_ole2frame(db_drawing *dp);
    db_ole2frame(const db_ole2frame &sour);  /* Copy constructor */
   ~db_ole2frame(void);

    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    char  ret_unk1(void) { return unk1; }
    char  ret_unk2(void) { return unk2; }

	// ODT specific - To be removed when DWGdirect output works.
    char *ret_unk3(void) { return unk3; }

    void get_ur(sds_point p) { DB_PTCPY(p,corner[1]); }
    void get_ll(sds_point p) { DB_PTCPY(p,corner[3]); }

    bool get_3(char **p) { *p=desc; return true; }
    bool get_10(sds_point p) { DB_PTCPY(p,corner[0]); return true; }
    bool get_11(sds_point p) { DB_PTCPY(p,corner[2]); return true; }
    bool get_70(int *p) { *p=ver; return true; }
    bool get_71(int *p) { *p=oletype; return true; }
    bool get_72(int *p) { *p=tilemodedesc; return true; }
    bool get_90(int *p) { *p=datasz; return true; }
    bool get_310(char **p) { *p=data; return true; }

    void set_unk1(int p) { unk1=(char)p; }
    void set_unk2(int p) { unk2=(char)p; }

	// ODT specific - To be removed when DWGdirect output works.
    void set_unk3(char *p) { memcpy(unk3,p,sizeof(unk3)); }

    void set_ur(sds_point p) { DB_PTCPY(corner[1],p); DB_PTCPY(extent[1],p); }
    void set_ll(sds_point p) { DB_PTCPY(corner[3],p); DB_PTCPY(extent[0],p); }

    bool set_3(char *p) { db_astrncpy(&desc,p,-1); return true; }
    bool set_10(sds_point p) { DB_PTCPY(corner[0],p); return true; }
    bool set_11(sds_point p) { DB_PTCPY(corner[2],p); return true; }
    bool set_70(int p) { ver=p; return true; }
    bool set_71(int p) { oletype=(p<1) ? 1 : ((p>3) ? 3 : p); return true; }
    bool set_72(int p) { tilemodedesc=(p!=0); return true; }
    /* 90 set by 310 below. */
    bool set_310(char *p, int n) {  /* Also sets 90. */
        delete [] data; data=NULL; datasz=0;
        if (p!=NULL && n>0)
            { datasz=n; data=new char[datasz]; memcpy(data,p,datasz); }
        return true;
    }

	// DWGdirect calls.
	void set_pixelwidth(short s) { pixelWidth = s; }
	void set_pixelheight(short s) { pixelHeight = s; }
	void set_modelflag(int i) { modelFlag = i; }
	void set_colorsnum(int i) { colorsNum = i; }
	void set_counter(int i) { counter = i; }
	void set_unknown6(int i) { unknown6 = i; }
	void set_unknown7(short s) { unknown7 = s; }
	void set_aspect(int i) { aspect = i; }
	void set_outputquality(int i) { quality = i; }

	unsigned short	get_pixelwidth() { return pixelWidth; }
	unsigned short	get_pixelheight() { return pixelHeight; }
	int				get_modelflag() { return modelFlag; }
	unsigned int	get_colorsnum() { return colorsNum; }
	unsigned int	get_counter() { return counter; }
	unsigned int	get_unknown6() { return unknown6; }
	short			get_unknown7() { return unknown7; }
	unsigned int	get_aspect() { return aspect; }
	unsigned int	get_outputquality() { return quality; }

  private:

    char       unk1;
    char       unk2;

	// ODT specific - To be removed when DWGdirect output works.
    char       unk3[26];	    
	// DWGdirect specific
	unsigned short	pixelWidth;
	unsigned short	pixelHeight;
	int				modelFlag;
	unsigned int	colorsNum;
	unsigned int	counter;
	unsigned int	unknown6;
	short			unknown7;
	unsigned int	aspect;
	unsigned int	quality;

    char      *desc;          /*   3 */
    sds_point  corner[4];     /*  10(ul),ur,11(lr),ll */
    int        ver;           /*  70 */
    int        oletype;       /*  71 */
    int        tilemodedesc;  /*  72 */

    int        datasz;        /*  90 */
    char      *data;          /* 310 */

    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_point : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_point(void);
    db_point(db_drawing *dp);
    db_point(const db_point &sour);  /* Copy constructor */
   ~db_point(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return pt; }

    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }
    bool get_50(sds_real *p) { *p=rot; return true; }

    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); rot=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real   thick;  /*  39 */

    sds_point  pt;     /*  10 */
    sds_real   rot;    /*  50 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_polyline : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_polyline(void);
    db_polyline(db_drawing *dp);
    db_polyline(const db_polyline &sour);  /* Copy constructor */
   ~db_polyline(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
	sds_real ret_elev(void) { return elev; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    bool get_10(sds_point p) { p[0]=p[1]=0.0; p[2]=elev; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }
    bool get_40(sds_real *p) { *p=defwid[0]; return true; }
    bool get_41(sds_real *p) { *p=defwid[1]; return true; }
    bool get_70(int *p) { *p=(unsigned char)flags; return true; }
    bool get_71(int *p) { *p=count[0]; return true; }
    bool get_72(int *p) { *p=count[1]; return true; }
    bool get_73(int *p) { *p=dens[0]; return true; }
    bool get_74(int *p) { *p=dens[1]; return true; }
    bool get_75(int *p) { *p=stype; return true; }

	bool count_ordinary_vertices(int * nverts);

    sds_real ret_39(void) { return thick; }
	sds_real ret_40(void) { return defwid[0]; }
	sds_real ret_41(void) { return defwid[1]; }
    int ret_70(void) { return (int)flags; }
    int ret_71(void) { return count[0]; }
    int ret_72(void) { return count[1]; }
    int ret_73(void) { return dens[0]; }
    int ret_74(void) { return dens[1]; }
	int ret_75(void) { return stype; }

	bool set_8(char *p, db_drawing *dp);
    bool set_10(sds_point p) {
        /* Only set it if it's 2D, and then step through the VERTEXs */
        /* and set their Z-values, too: */
        if (!(flags&'\x58')) {  /* 2D */
            sds_point ap1;
            db_handitem *thip1;

            elev=p[2];
            for (thip1=this->next; thip1!=NULL &&
                thip1->ret_type()==DB_VERTEX; thip1=thip1->next)
                    { thip1->get_10(ap1); ap1[2]=elev; thip1->set_10(ap1); }
        }
        return true;
    }
    bool set_39(sds_real p) { thick=(flags&'\x58') ? 0.0 : p; return true; }
    bool set_40(sds_real p) { defwid[0]=(p>=0.0) ? p : 0.0; return true; }
    bool set_41(sds_real p) { defwid[1]=(p>=0.0) ? p : 0.0; return true; }
    bool set_70(int p) { flags=(char)p; if (flags&'\x58') thick=0.0; return true; }
    bool set_71(int p) { count[0]=p; return true; }
    bool set_72(int p) { count[1]=p; return true; }
    bool set_73(int p) { dens[0]=p; return true; }
    bool set_74(int p) { dens[1]=p; return true; }
    bool set_75(int p) { stype=charCast((p==5 || p==6 || p==8) ? p : 0); return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

	virtual int check(db_drawing* pDrawing);

  private:

    sds_real  thick;      /*  39 */

    sds_real  elev;       /*  10 (just the Z-coord.) */
    sds_real  defwid[2];  /*  40, 41 */
	
    unsigned char flags;      /*  70 */
    int       count[2];   /*  71, 72 */
    int       dens[2];    /*  73, 74 */
    char      stype;      /*  75 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_ray : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_ray(void);
    db_ray(db_drawing *dp);
    db_ray(const db_ray &sour);  /* Copy constructor */
   ~db_ray(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real  *retp_10(void) { return pt; }
    sds_real  *retp_11(void) { return vect; }

    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_11(sds_point p) { p[0]=vect[0]; p[1]=vect[1]; p[2]=vect[2]; return true; }

    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
	// EBATECH(CNBR) -[ 2002/6/20 Bugzilla 78216 Vector must unitized.
    bool set_11(sds_point p) {
		sds_real sum;
		sum = sqrt( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] );
        if (icadRealEqual(sum,0.0)) {
            vect[0]=1.0; vect[1]=vect[2]=0.0;
        } else {
            vect[0]=p[0]/sum; vect[1]=p[1]/sum; vect[2]=p[2]/sum;
        }
        return true;
    }
    /*
    bool set_11(sds_point p) {
        if (p[0]==0.0 && p[1]==0.0 && p[2]==0.0) {
            vect[0]=1.0; vect[1]=vect[2]=0.0;
        } else {
            vect[0]=p[0]; vect[1]=p[1]; vect[2]=p[2];
        }
        return true;
    }
	*/
	// EBATECH(CNBR) ]-

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_point pt;    /* 10 */
    sds_point vect;  /* 11 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_rtext : public db_entity_with_extrusion {
public:
	db_rtext(void);
    db_rtext(db_drawing *dp);
    db_rtext(const db_rtext &sour);  /* Copy constructor */
   ~db_rtext(void);

	void set_pt0(sds_point p) { ic_ptcpy(pt0,p); }
	void get_pt0(sds_point p) { ic_ptcpy(p,pt0); }
	void set_rotang(sds_real r) { rotang=r; }
	sds_real ret_rotang(void) { return rotang; }
	void set_height(sds_real r) { height=r; }
	sds_real ret_height(void) { return height; }
	void set_textstyleobjhandle(db_objhandle oh) { textstyleobjhandle=oh; }
	db_objhandle ret_textstyleobjhandle(void) { return textstyleobjhandle; }
	void set_typeflag(short s) { typeflag=s; }
	short ret_typeflag(void) { return typeflag; }

	void set_contents(char *s) { strcpy(contents,s); }
	void get_contents(char *s) { strcpy(s,contents); }

	private:
		sds_point pt0;
		sds_real rotang;
		sds_real height;
		db_objhandle textstyleobjhandle;
		short typeflag;
		char contents[1024];
};


class DB_CLASS db_seqend : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_seqend(void);
    db_seqend(db_drawing *dp);
    db_seqend(const db_seqend &sour);  /* Copy constructor */


    bool get_mainent(sds_name p, db_drawing *dp) {
        p[0]=(long)mainhip; p[1]=(long)dp;
        return true;
    }
    bool get_mainent(db_handitem **p) { *p=mainhip; return true; }

    bool set_mainent(db_handitem *p) { mainhip=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    class db_handitem *mainhip;  /* -2 */
};

// EBATECH(CNBR) -[ 2002/4/24 Support invisual SHAPE entity.
class DB_CLASS db_shape : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_shape(void);
    db_shape(db_drawing *dp);
    db_shape(const db_shape &sour);  /* Copy constructor */
   ~db_shape(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real  *retp_10(void) { return pt; }
    sds_real *retp_40(void) { return &sz; }
    sds_real *retp_41(void) { return &xscl; }
    sds_real *retp_50(void) { return &rot; }

    virtual void set_tship(db_handitem* p) { tship = p; }
    db_handitem *ret_tship(void) { return tship; }

	bool set_tshipbyhand(db_objhandle hand, db_drawing *dp) {
	    db_handitem *thip1;

		if ((thip1=dp->handent(hand))==NULL)
	        { db_lasterror=OL_EHANDLE; return false; }
	    tship=thip1; return true;
	}
    bool get_2(char *p, int maxlen) {
        if (shname==NULL) *p=0;
        else { strncpy(p,shname,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_2(char **p) { *p= ( shname == NULL ? db_str_quotequote : shname ); return true; }
    bool get_2(int *p)   { *p=shnum; return true; }
    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_39(sds_real *p) { *p=thick; return true; }
    bool get_40(sds_real *p) { *p=sz; return true; }
    bool get_41(sds_real *p) { *p=xscl; return true; }
    bool get_50(sds_real *p) { *p=rot; return true; }
    bool get_51(sds_real *p) { *p=obl; return true; }

	bool set_2(char *p, db_drawing *dp) { return set_name( p, dp );}
    bool set_2(char* p){ return set_name( p ); }
    bool set_2(int p){ return set_code( p ); }
    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p) { sz=(p>0.0) ? p : ((p<0.0) ? -p : 1.0); return true; }
    bool set_41(sds_real p) { xscl=(p>0.0) ? p : ((p<0.0) ? -p : 1.0); return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); rot=p; return true; }
    bool set_51(sds_real p) { obl=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real     thick;   /*  39 */

	char        *shname;  /*   2 */
    db_handitem *tship;   /*   2 */
    int          shnum;   /*   2 */
    sds_point    pt;      /*  10 */
    sds_real     sz;      /*  40 */
    sds_real     xscl;    /*  41 */
    sds_real     rot;     /*  50 */
    sds_real     obl;     /*  51 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */

	bool set_name(char *p, db_drawing *dp);
	bool set_name(char* p );
	bool set_code(int p );
};
// EBATECH(CNBR) ]-

class DB_CLASS db_solid : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_solid(void);
    db_solid(db_drawing *dp);
    db_solid(const db_solid &sour);  /* Copy constructor */
   ~db_solid(void);

    int ret_is3pt(void) {
        return icadPointEqual(pt[2],pt[3]);
    }

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    bool get_ptbyidx(sds_point p, int i) {
        if (i>-1 && i<4) DB_PTCPY(p,pt[i]); return true;
    }

    sds_real *retp_10(void) { return pt[0]; }
    sds_real *retp_11(void) { return pt[1]; }
    sds_real *retp_12(void) { return pt[2]; }
    sds_real *retp_13(void) { return pt[3]; }

    bool get_10(sds_point p) { p[0]=pt[0][0]; p[1]=pt[0][1]; p[2]=pt[0][2]; return true; }
    bool get_11(sds_point p) { p[0]=pt[1][0]; p[1]=pt[1][1]; p[2]=pt[1][2]; return true; }
    bool get_12(sds_point p) { p[0]=pt[2][0]; p[1]=pt[2][1]; p[2]=pt[2][2]; return true; }
    bool get_13(sds_point p) { p[0]=pt[3][0]; p[1]=pt[3][1]; p[2]=pt[3][2]; return true; }
    bool get_39(sds_real *p) { *p=thick ; return true; }

    bool set_10(sds_point p) { pt[0][0]=p[0]; pt[0][1]=p[1]; pt[0][2]=p[2]; return true; }
    bool set_11(sds_point p) { pt[1][0]=p[0]; pt[1][1]=p[1]; pt[1][2]=p[2]; return true; }
    bool set_12(sds_point p) { pt[2][0]=p[0]; pt[2][1]=p[1]; pt[2][2]=p[2]; return true; }
    bool set_13(sds_point p) { pt[3][0]=p[0]; pt[3][1]=p[1]; pt[3][2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real   thick;  /*  39 */

    sds_point  pt[4];  /*  10 - 13 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class db_splinecontrollink {  /* For spline below. */

  public:

    db_splinecontrollink(void) { pt[0]=pt[1]=pt[2]=0.0; wt=1.0; }

    sds_point pt;  /*  10 */
    sds_real  wt;  /*  41 */
};

class DB_CLASS db_spline : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_spline(void);
    db_spline(db_drawing *dp);
    db_spline(const db_spline &sour);  /* Copy constructor */
   ~db_spline(void);

    void  setDefinitionData(sds_point* pCtrlPoints, 
							int numCtrlPoints,
		                    sds_real* pKnotVector, 
							int numKnots,
							sds_real* pWeights = NULL,  // Null for nonrational
							bool deleteFitPoints = false)
	{
		set_10(pCtrlPoints, numCtrlPoints);
		set_40(pKnotVector, numKnots);
		if (pWeights) 
			setIsRational(false);
		else
			setIsRational(true);

		set_41(pWeights, numCtrlPoints);
		scenario = 1;

		if (deleteFitPoints) {
			delete [] fpt;
			fpt = NULL;
			nfpts = 0;
		}
	}

    void  setDefinitionData(int degree,
							bool periodic,
							bool closed,
							sds_point* pCtrlPoints, 
							int numCtrlPoints,
		                    sds_real* pKnotVector, 
							int numKnots,
							sds_real* pWeights = NULL,  // Null for rational
							bool deleteFitPoints = false)
	{
		deg = degree;
		setPeriodic(periodic);
		setIsClosed(closed);
		setDefinitionData(pCtrlPoints, numCtrlPoints, pKnotVector, numKnots, pWeights, deleteFitPoints);
	}

	void  setPeriodic(bool isPeriodic) { flags = (isPeriodic) ? flags | 2 : flags & ~2; }
	bool  isPeriodic() const { return (flags & 2) ? true : false; }

	void  setIsClosed(bool isClosed) { flags = (isClosed) ? flags | 1 : flags & ~1; }
	bool  isClosed() const { return (flags & 1) ? true : false; }

	void  setIsRational(bool isRational) {flags = (isRational) ? flags | 4 : flags & ~4; }
	bool  isRational() const { return (flags & 4) ? true : false; }

	void  deleteFitPoints() { if (fpt) { delete [] fpt; fpt = NULL;  nfpts = 0; } }

	// return control and weight at a vertex
	void  getControlPoint(sds_point &pt, int i) const { 
			ASSERT(i >=0 && i < ncpts);
			pt[0] = control[i].pt[0]; 
			pt[1] = control[i].pt[1];
			pt[2] = control[i].pt[2];
	}

	void  getKnot(sds_real &knotValue, int i) const {
			ASSERT(i >=0 && i < nknots);
			knotValue = knot[i]; 
	}

	void  getWeight(sds_real &weight, int i) const {
			ASSERT(i >=0 && i < ncpts);
			weight = control[i].wt;
	}


    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(int idx) {
        return (idx>-1 && idx<ncpts && control!=NULL) ?
            control[idx].pt : NULL;
    }
    sds_real *retp_11(int idx)
        { return (idx>-1 && idx<nfpts && fpt!=NULL) ? fpt[idx] : NULL; }
    sds_real *retp_12(void) { return tandir[0]; }
    sds_real *retp_13(void) { return tandir[1]; }

    int ret_scenario(void) { return scenario; }

    bool get_10(sds_point *p, int n) {  /* WARNING: Fills an array. */
        int fi1;

        if (n>ncpts) n=ncpts;
        if (control!=NULL && n>0) {
            for (fi1=0; fi1<n; fi1++) {
                p[fi1][0]=control[fi1].pt[0];
                p[fi1][1]=control[fi1].pt[1];
                p[fi1][2]=control[fi1].pt[2];
            }
        }
        return true;
    }
    bool get_11(sds_point *p, int n) {  /* WARNING: Fills an array. */
        if (n>nfpts) n=nfpts;
        if (fpt!=NULL && n>0) memcpy(p,fpt,n*sizeof(sds_point));
        return true;
    }
    bool get_12(sds_point p) { p[0]=tandir[0][0]; p[1]=tandir[0][1]; p[2]=tandir[0][2]; return true; }
    bool get_13(sds_point p) { p[0]=tandir[1][0]; p[1]=tandir[1][1]; p[2]=tandir[1][2]; return true; }
    bool get_40array(sds_real *p, int n) {  /* WARNING: Fills an array. */
        if (n>nknots) n=nknots;
        if (knot!=NULL && n>0) memcpy(p,knot,n*sizeof(sds_real));
        return true;
    }
    bool get_41array(sds_real *p, int n) {  /* WARNING: Fills an array. */
        int fi1;

        if (n>ncpts) n=ncpts;
        if (control!=NULL && n>0) for (fi1=0; fi1<n; fi1++) p[fi1]=control[fi1].wt;
        return true;
    }
    bool get_42(sds_real *p) { *p=ktol; return true; }
    bool get_43(sds_real *p) { *p=ctol; return true; }
    bool get_44(sds_real *p) { *p=ftol; return true; }
    bool get_70(int *p) { *p=(unsigned char)flags; return true; }
    bool get_71(int *p) { *p=(unsigned char)deg; return true; }
    bool get_72(int *p) { *p=nknots; return true; }
    bool get_73(int *p) { *p=ncpts; return true; }
    bool get_74(int *p) { *p=nfpts; return true; }


    void set_scenario(int p) { scenario=p; }

    /*
    **  WARNING: set_10() and set_41() use the same array of structs --
    **  but set_10() is the allocator (and sets ncpts).
    **  Call set_10() BEFORE set_41().
    */
	// set control points
    bool set_10(sds_point *p, int n) {  /* WARNING: Sets an array. */
        int fi1;

        if (n!=ncpts || p==NULL) { delete [] control; control=NULL; ncpts=0; }
        if (n>0 && p!=NULL) {
            if (control==NULL) { control=new db_splinecontrollink[n]; ncpts=n; }
            for (fi1=0; fi1<n; fi1++) {
                control[fi1].pt[0]=p[fi1][0];
                control[fi1].pt[1]=p[fi1][1];
                control[fi1].pt[2]=p[fi1][2];
            }
        }
        return true;
    }
	// set fit points
    bool set_11(sds_point *p, int n) {  /* WARNING: Sets an array. */
        if (n!=nfpts || p==NULL) { delete [] fpt; fpt=NULL; nfpts=0; }
        if (n>0 && p!=NULL) {
            if (fpt==NULL) { fpt=new sds_point[n]; nfpts=n; }
            memcpy(fpt,p,n*sizeof(sds_point));
        }
        return true;
    }
    bool set_12(sds_point p) {
        tandir[0][0]=p[0]; tandir[0][1]=p[1]; tandir[0][2]=p[2];
        return true;
    }
    bool set_13(sds_point p) {
        tandir[1][0]=p[0]; tandir[1][1]=p[1]; tandir[1][2]=p[2];
        return true;
    }
	// set the value of knot vector
    bool set_40(sds_real *p, int n) {  /* WARNING: Sets an array. */
        if (n!=nknots || p==NULL) { delete [] knot; knot=NULL; nknots=0; }
        if (n>0 && p!=NULL) {
            if (knot==NULL) { knot=new sds_real[n]; nknots=n; }
            memcpy(knot,p,n*sizeof(sds_real));
        }
        return true;
    }
	// set weights
    bool set_41(sds_real *p, int n) {  /* WARNING: Call set_10() FIRST. */
        if (n>0 && control!=NULL) {
			int i;
			if (p!=NULL)
                for (i=0; i<ncpts; i++)
                     control[i].wt=(i<n) ? p[i] : 1.0;
			else 
				for (i=0; i<ncpts; i++)
					control[i].wt= 1.0;
		}
        return true;
    }
    bool set_42(sds_real p) { ktol=(p>=0.0) ? p : 1.0E-7; return true; }
    bool set_43(sds_real p) { ctol=(p>=0.0) ? p : 1.0E-7; return true; }
    bool set_44(sds_real p) { ftol=(p>=0.0) ? p : -p; return true; }
    bool set_70(int p) { flags=charCast(p); return true; }
    bool set_71(int p) { deg=charCast((p>0) ? p : 0); return true; }
//  bool set_72(int p) { nknots=(p>0) ? p : 0; return true; } Set by set_40().
//  bool set_73(int p) { ncpts =(p>0) ? p : 0; return true; } Set by set_10() and set_41().
//  bool set_74(int p) { nfpts =(p>0) ? p : 0; return true; } Set by set_11().

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);

  private:

    /*
    **  Here's an odd one that relates to what data was present in the
    **  DWG file:
    **
    **      1 : Only control points (10's) were present (no fit points (11's)).
    **          (This may be the NURBS scenario.)
    **
    **      2 : Only fit points (11's) were present (no control points (10's)).
    **          (This may be the "fit" scenario.)
    **
    **  This member should be set when the SPLINE is read from disk.
    **  (Later, we MAY calculate the missing points like ACAD does,
    **  thus losing the distinction; that's why the reader has to set this
    **  flag.)
    */
    int                          scenario;
    db_splinecontrollink        *control;    /*  10, 41 (alloc'd for ncpts) */
    sds_point                   *fpt;        /*  11 (alloc'd for nfpts) */
    sds_point                    tandir[2];  /*  12, 13 */
    sds_real                    *knot;       /*  40 (alloc'd for nknots) */
    sds_real                     ktol;       /*  42 */
    sds_real                     ctol;       /*  43 */
    sds_real                     ftol;       /*  44 */
    char                         flags;      /*  70 */
    char                         deg;        /*  71 */
    int                          nknots;     /*  72 */
    int                          ncpts;      /*  73 */
    int                          nfpts;      /*  74 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_text : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_text(void);
    db_text(db_drawing *dp);
    db_text(const db_text &sour);  /* Copy constructor */
   ~db_text(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    char     **retp_1(void)  { return &val; }
    sds_real  *retp_10(void) { return ipt; }
    sds_real  *retp_11(void) { return apt; }
    sds_real  *retp_40(void) { return &ht; }
    sds_real  *retp_50(void) { return &rot; }

    bool get_1(char *p, int maxlen) {
        if (val==NULL) *p=0;
        else { strncpy(p,val,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_1(char **p) { *p=val; return true; }
    bool get_7(char *p, int maxlen)
        { if (tship==NULL) *p=0; else tship->get_2(p,maxlen); return true; }
    bool get_7(char **p) {
        if (tship!=NULL) tship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_7(db_handitem **p) { *p=tship; return true; }
    bool get_10(sds_point p) { p[0]=ipt[0]; p[1]=ipt[1]; p[2]=ipt[2]; return true; }
    bool get_11(sds_point p) { p[0]=apt[0]; p[1]=apt[1]; p[2]=apt[2]; return true; }
    bool get_39(sds_real *p) { *p=thick ; return true; }
    bool get_40(sds_real *p) { *p=ht    ; return true; }
    bool get_41(sds_real *p) { *p=xscl  ; return true; }
    bool get_50(sds_real *p) { *p=rot   ; return true; }
    bool get_51(sds_real *p) { *p=obl   ; return true; }
    bool get_71(int *p)      { *p=gen   ; return true; }
    bool get_72(int *p)      { *p=hjust ; return true; }
    bool get_73(int *p)      { *p=vjust ; return true; }

    bool set_1(char *p) {
        db_astrncpy(&val,(p==NULL) ? db_str_quotequote : p,-1);
        return true;
    }

	bool set_7(char *p, db_drawing *dp) {
		db_handitem *thip1;

	    if (p==NULL || !*p || dp==NULL ||
		    (thip1=dp->findtabrec(DB_STYLETAB,p,1))==NULL)
			    { db_lasterror=OL_ESNVALID; return false; }
	    tship=thip1; return true;
	}

    bool set_7(db_handitem *p) { tship=p; return true; }
    bool set_10(sds_point p) { ipt[0]=p[0]; ipt[1]=p[1]; ipt[2]=p[2]; return true; }
    bool set_11(sds_point p) { apt[0]=p[0]; apt[1]=p[1]; apt[2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }
    bool set_40(sds_real p) { ht=(p>0.0) ? p : ((p<0.0) ? -p : atof(db_oldsysvar[DB_QTEXTSIZE].defval)); return true; }
    bool set_41(sds_real p) { xscl=(icadRealEqual(p,0.0)) ? 1.0 : p; return true; }
    bool set_50(sds_real p) { ic_normang(&p,NULL); rot=p; return true; }
    bool set_51(sds_real p) { ic_normang(&p,NULL); obl=p; return true; }
    bool set_71(int p) { gen=charCast(p); return true; }
    bool set_72(int p) { hjust=charCast(p); return true; }
    bool set_73(int p) { vjust=charCast(p); return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real     thick;   /*  39 */

    char        *val;     /*   1 */
    db_handitem *tship;   /*   7 */
    sds_point    ipt;     /*  10 */
    sds_point    apt;     /*  11 */
    sds_real     ht;      /*  40 */
    sds_real     xscl;    /*  41 */
    sds_real     rot;     /*  50 */
    sds_real     obl;     /*  51 */
    char         gen;     /*  71 */
    char         hjust;   /*  72 */
    char         vjust;   /*  73 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_tolerance : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_tolerance(void);
    db_tolerance(db_drawing *dp);
    db_tolerance(const db_tolerance &sour);  /* Copy constructor */
   ~db_tolerance(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return pt; }

    void get_textht(sds_real *p) { *p=textht; }
    bool get_1(char *p, int maxlen) {
        if (primtext==NULL) *p=0;
        else { strncpy(p,primtext,maxlen); p[maxlen]=0; }
        return true;
    }
    bool get_1(char **p) { *p=primtext; return true; }
    bool get_3(char *p, int maxlen)
        { if (dship==NULL) *p=0; else dship->get_2(p,maxlen); return true; }
    bool get_3(char **p) {
        if (dship!=NULL) dship->get_2(p); else *p=db_str_quotequote;
        return true;
    }
    bool get_3(db_handitem **p) { *p=dship; return true; }
    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_11(sds_point p) { p[0]=xdir[0]; p[1]=xdir[1]; p[2]=xdir[2]; return true; }

    void set_textht(sds_real p) { textht=(icadRealEqual(p,0.0)) ? 0.2 : p; }
    bool set_1(char *p) {
        db_astrncpy(&primtext,(p==NULL) ? db_str_quotequote : p,-1);
        return true;
    }

	bool db_tolerance::set_3(char *p, db_drawing *dp) {
	    db_handitem *thip1;

		if (p==NULL || !*p || dp==NULL ||
			(thip1=dp->findtabrec(DB_DIMSTYLETAB,p,1))==NULL)
				{ db_lasterror=OL_ESNVALID; return false; }
	    dship=thip1; return true;
	}

    bool set_3(db_handitem *p) { dship=p; return true; }
    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
    bool set_11(sds_point p) {
        if (icadRealEqual(p[0],0.0) && icadRealEqual(p[1],0.0) && icadRealEqual(p[2],0.0)) {
            xdir[0]=1.0; xdir[1]=xdir[2]=0.0;
        } else {
            xdir[0]=p[0]; xdir[1]=p[1]; xdir[2]=p[2];
        }
        return true;
    }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real           textht;   /* In DWG only */
    char              *primtext; /*   1 */
    class db_handitem *dship;    /*   3 */
    sds_point          pt;       /*  10 */
    sds_point          xdir;     /*  11 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


class DB_CLASS db_trace : public db_entity_with_extrusion {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_trace(void);
    db_trace(db_drawing *dp);
    db_trace(const db_trace &sour);  /* Copy constructor */
   ~db_trace(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    bool get_ptbyidx(sds_point p, int i) {
        if (i>-1 && i<4) DB_PTCPY(p,pt[i]); return true;
    }

    sds_real *retp_10(void) { return pt[0]; }
    sds_real *retp_11(void) { return pt[1]; }
    sds_real *retp_12(void) { return pt[2]; }
    sds_real *retp_13(void) { return pt[3]; }

    bool get_10(sds_point p) { p[0]=pt[0][0]; p[1]=pt[0][1]; p[2]=pt[0][2]; return true; }
    bool get_11(sds_point p) { p[0]=pt[1][0]; p[1]=pt[1][1]; p[2]=pt[1][2]; return true; }
    bool get_12(sds_point p) { p[0]=pt[2][0]; p[1]=pt[2][1]; p[2]=pt[2][2]; return true; }
    bool get_13(sds_point p) { p[0]=pt[3][0]; p[1]=pt[3][1]; p[2]=pt[3][2]; return true; }
    bool get_39(sds_real *p) { *p=thick ; return true; }

    bool set_10(sds_point p) { pt[0][0]=p[0]; pt[0][1]=p[1]; pt[0][2]=p[2]; return true; }
    bool set_11(sds_point p) { pt[1][0]=p[0]; pt[1][1]=p[1]; pt[1][2]=p[2]; return true; }
    bool set_12(sds_point p) { pt[2][0]=p[0]; pt[2][1]=p[1]; pt[2][2]=p[2]; return true; }
    bool set_13(sds_point p) { pt[3][0]=p[0]; pt[3][1]=p[1]; pt[3][2]=p[2]; return true; }
    bool set_39(sds_real p) { thick=p; return true; }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

  private:

    sds_real   thick;  /*  39 */

    sds_point  pt[4];  /*  10 - 13 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


//10/98
//The each vertex sets the default start(40) and end(41) width to 0.  Externally a not setting these values should
//result in defaulting to the polyline start and end width. There is currently no way to tell if the start or end
//width has been set or not. Therefore, if the start and end width is not explicitly set, the user will end up with a
//width of 0.  This may become an issue when we create a COM interface into the DB objects.  We will need to make sure the
//user does not have to specify the same width the polyline uses in each vertex.

#pragma pack (push)
#pragma pack (1)
class DB_CLASS db_vertex : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_vertex(void);
    db_vertex(db_drawing *dp);
    db_vertex(const db_vertex &sour);  /* Copy constructor */
   ~db_vertex(void);

    sds_real  *retp_10(void) { return pt; }

    db_handitem *ret_main(void) {
    /*
    **  Return the hip of the POLYLINE entity:
    */
        db_handitem *thip1,*rethip;

        rethip=NULL;
        for (thip1=this; thip1!=NULL && thip1->ret_type()==DB_VERTEX; thip1=thip1->next);
        if (thip1!=NULL && thip1->ret_type()==DB_SEQEND)
            ((db_seqend *)thip1)->get_mainent(&rethip);
        return rethip;
    }

    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_40(sds_real *p) { *p=wid[0]; return true; }
    bool get_41(sds_real *p) { *p=wid[1]; return true; }
    bool get_42(sds_real *p) { *p=bulge; return true; }
    bool get_50(sds_real *p) { *p=tandir; return true; }
    bool get_70(int *p) { *p=(unsigned char)flags; return true; }
    bool get_71(int *p) { *p=(vidx!=NULL) ? vidx[0] : 0; return true; }
    bool get_72(int *p) { *p=(vidx!=NULL) ? vidx[1] : 0; return true; }
    bool get_73(int *p) { *p=(vidx!=NULL) ? vidx[2] : 0; return true; }
    bool get_74(int *p) { *p=(vidx!=NULL) ? vidx[3] : 0; return true; }

    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
    bool set_40(sds_real p)
		{
		wid[0]=(p>=0.0) ? p : -p;
		return true;
		}
	bool set_41(sds_real p)
		{
		wid[1]=(p>=0.0) ? p : -p;
		return true;
		}
    bool set_42(sds_real p)
		{
		bulge=p;
		return true;
		}
    bool set_50(sds_real p) { ic_normang(&p,NULL); tandir=p; return true; }
    bool set_70(int p) { flags=(char)p; return true; }
    bool set_71(int p) {
        if (vidx==NULL) { vidx=new int[4]; vidx[1]=vidx[2]=vidx[3]=0; }
        vidx[0]=p; return true;
    }
    bool set_72(int p) {
        if (vidx==NULL) { vidx=new int[4]; vidx[0]=vidx[2]=vidx[3]=0; }
        vidx[1]=p; return true;
    }
    bool set_73(int p) {
        if (vidx==NULL) { vidx=new int[4]; vidx[0]=vidx[1]=vidx[3]=0; }
        vidx[2]=p; return true;
    }
    bool set_74(int p) {
        if (vidx==NULL) { vidx=new int[4]; vidx[0]=vidx[1]=vidx[2]=0; }
        vidx[3]=p; return true;
    }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);

	db_polyline *retParentPolyline( void )
		{
		return m_pParentPolyline;
		}
	void setParentPolyline( db_polyline *pParent )
		{
		ASSERT( pParent->ret_type() == DB_POLYLINE );
		m_pParentPolyline = pParent;
		}

  private:

	db_polyline		*m_pParentPolyline;

    sds_point pt;      /* 10 */
    sds_real  wid[2];  /* 40, 41 */
    sds_real  bulge;   /* 42 */
    sds_real  tandir;  /* 50 */
    int      *vidx;    /* 71 - 74 (only alloc'd for pface face def VERTEXs) */
    char      flags;   /* 70 */
};

#pragma pack (pop)


class DB_CLASS db_wipeout : public db_entity {
	public:
		db_wipeout(void);
		db_wipeout(db_drawing *dp);
		db_wipeout(const db_wipeout &sour);  /* Copy constructor */
	   ~db_wipeout(void);

		void set_classversion(long cv) { classversion=cv; }
		long ret_classversion(void) { return classversion; }
		void set_pt0(sds_point p) { ic_ptcpy(pt0,p); }
		void get_pt0(sds_point p) { ic_ptcpy(p,pt0); }
		void set_size(sds_real *r) { size[0]=r[0]; size[1]=r[1]; }
		void get_size(sds_real *r) { r[0]=size[0]; r[1]=size[1]; }
		void set_uvec(sds_point p) { ic_ptcpy(uvec,p); }
		void get_uvec(sds_point p) { ic_ptcpy(p,uvec); }
		void set_vvec(sds_point p) { ic_ptcpy(vvec,p); }
		void get_vvec(sds_point p) { ic_ptcpy(p,vvec); }
		void set_clipping(unsigned short c) { clipping=c; }
		unsigned short ret_clipping(void) { return clipping; }
		void set_brightness(unsigned short c) { brightness=c; }
		unsigned short ret_brightness(void) { return brightness; }
		void set_contrast(unsigned short c) { contrast=c; }
		unsigned short ret_contrast(void) { return contrast; }
		void set_fade(unsigned short c) { fade=c; }
		unsigned short ret_fade(void) { return fade; }
		void set_clipboundtype(unsigned short c) { clipboundtype=c; }
		unsigned short ret_clipboundtype(void) { return clipboundtype; }
		void set_displayprops(unsigned short c) { displayprops=c; }
		unsigned short ret_displayprops(void) { return displayprops; }
		void set_numclipverts(long cv);
		long ret_numclipverts(void) { return numclipverts; }
		void set_clipVert(int index, sds_real *r);
		void get_clipVert(int index, sds_real *r);
		void set_rectclipvert0(sds_real *r) { rectclipvert0[0]=r[0]; rectclipvert0[1]=r[1]; }
		void get_rectclipvert0(sds_real *r) { r[0]=rectclipvert0[0]; r[1]=rectclipvert0[1]; }
		void set_rectclipvert1(sds_real *r) { rectclipvert1[0]=r[0]; rectclipvert1[1]=r[1]; }
		void get_rectclipvert1(sds_real *r) { r[0]=rectclipvert1[0]; r[1]=rectclipvert1[1]; }
		void set_polyclipvertblob(char *p) { polyclipvertblob=p; }
		char *ret_polyclipvertblob(void) { return polyclipvertblob; }
		void set_grblob(char *p) { grblob=p; }
		char *ret_grblob(void) { return grblob; }
		void set_grblobsz(int sz) { grblobsz=sz; }
		int ret_grblobsz(void) { return grblobsz; }
		void set_imagedefobjhandle(db_objhandle oh) { imagedefobjhandle=oh; }
		db_objhandle ret_imagedefobjhandle(void) { return imagedefobjhandle; }
		void set_imagedefreactorobjhandle(db_objhandle oh) { imagedefreactorobjhandle=oh; }
		db_objhandle ret_imagedefreactorobjhandle(void) { return imagedefreactorobjhandle; }

		void  setResbufChain(struct sds_resbuf* p) { m_pResbufChain = p; }
		struct sds_resbuf* getResbufChain() { return m_pResbufChain; }

	private:
		long classversion;
		sds_point pt0;
		sds_real size[2];
		sds_point uvec,vvec;
		unsigned short clipping,brightness,contrast,fade,clipboundtype;
		unsigned short displayprops;
		long numclipverts;
		sds_real rectclipvert0[2],rectclipvert1[2];
		sds_point* m_pClipPoints;
		char * polyclipvertblob;
		char *grblob;
		int grblobsz;
		db_objhandle imagedefobjhandle,imagedefreactorobjhandle;

		struct sds_resbuf* m_pResbufChain;
} ;


class DB_CLASS db_xline : public db_entity {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_xline(void);
    db_xline(db_drawing *dp);
    db_xline(const db_xline &sour);  /* Copy constructor */
   ~db_xline(void);

    void *ret_disp(void)    { return disp; }
    void  set_disp(void *p) { disp=p; }
    void  get_extent(sds_point ll, sds_point ur) { DB_PTCPY(ll,extent[0]); DB_PTCPY(ur,extent[1]); }
    void  set_extent(sds_point ll, sds_point ur) { DB_PTCPY(extent[0],ll); DB_PTCPY(extent[1],ur); }

    sds_real *retp_10(void) { return pt; }
    sds_real *retp_11(void) { return vect; }

    bool get_10(sds_point p) { p[0]=pt[0]; p[1]=pt[1]; p[2]=pt[2]; return true; }
    bool get_11(sds_point p) { p[0]=vect[0]; p[1]=vect[1]; p[2]=vect[2]; return true; }

    bool set_10(sds_point p) { pt[0]=p[0]; pt[1]=p[1]; pt[2]=p[2]; return true; }
	// EBATECH(CNBR) -[ 2002/6/20 Bugzilla 78216 Vector must unitized.
    bool set_11(sds_point p) {
		sds_real sum;
		sum = sqrt( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] );
        if (icadRealEqual(sum,0.0)) {
            vect[0]=1.0; vect[1]=vect[2]=0.0;
        } else {
            vect[0]=p[0]/sum; vect[1]=p[1]/sum; vect[2]=p[2]/sum;
        }
        return true;
    }
    /*
    bool set_11(sds_point p) {
        if (p[0]==0.0 && p[1]==0.0 && p[2]==0.0) {
            vect[0]=1.0; vect[1]=vect[2]=0.0;
        } else {
            vect[0]=p[0]; vect[1]=p[1]; vect[2]=p[2];
        }
        return true;
    }
	*/
	// EBATECH(CNBR) ]-

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);


  private:

    sds_point pt;    /* 10 */
    sds_point vect;  /* 11 */

    void      *disp;      /* Disp obj llist (gr_displayobject *) */
    sds_point  extent[2]; /* The extents of the disp llist; see db_3dface. */
};


#endif // _INC_ENTITIES
