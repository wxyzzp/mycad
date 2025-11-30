/* C:\ICAD\PRJ\LIB\DB\LTYPETABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

class DB_CLASS db_ltypedashlink 
{
public:
    db_ltypedashlink() { reset(); }
   ~db_ltypedashlink() { delete[] text; }

	/*****************************************************************************************
	* Author:	   Denis Petrov
	* Description: Helper method. Resets class data.
	*****************************************************************************************/
	void reset()
	{
        text = NULL; 
		stylehip = NULL; 
		type = 0;
		shapecd = 0;
        offset[0] = offset[1] = len = rot = 0.0; 
		scale = 1.0;
	}

    char        *text;       /*   9 */
    sds_real     offset[2];  /*  44, 45 */
    sds_real     scale;      /*  46 */
    sds_real     len;        /*  49 */
    sds_real     rot;        /*  50 */
    int          type;       /*  74 */
    int          shapecd;    /*  75 */
    db_handitem *stylehip;   /* 340 */
};

class DB_CLASS db_ltypetabrec : public db_tablerecord {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_ltypetabrec(void);
    db_ltypetabrec(char *pname, db_drawing *dp);
    db_ltypetabrec(const db_ltypetabrec &sour);  /* Copy constructor */
   ~db_ltypetabrec(void);

    /*
    **  Order-dependent stuff:
    **
    **  To create the dash[] array, call set_73() first (to allocate it).
    **  Then call set_dashdata() repeatedly to fill the elements.
    **
    **  Note that there IS no patlen member any more.  set_40() calls the
    **  virtual in db_handitem (which does nothing); get_40() calculates
    **  the total pattern length.
    **
    **  THEN call set_40() to CALCULATE patlen.
    **
    **  (Note that we still have the array forms of get_49()
    **  and set_49() if you want to deal only with the dash
    **  lengths themselves.)
    */

    void get_dashdata(
        int           didx,
        char        **textpp,
        sds_real     *xoffsetp,
        sds_real     *yoffsetp,
        sds_real     *scalep,
        sds_real     *lenp,
        sds_real     *rotp,
        int          *typep,
        int          *shapecdp,
        db_objhandle *stylehandle);

    void set_dashdata(
        int          didx,
        char        *text,
        sds_real     xoffset,
        sds_real     yoffset,
        sds_real     scale,
        sds_real     len,
        sds_real     rot,
        int          type,
        int          shapecd,
        db_objhandle stylehandle,
        db_drawing  *dp);

    bool get_3(char **p) { *p=desc; return true; }
    bool get_3(char *p, int maxlen) {
        if (desc==NULL) *p=0;
        else { strncpy(p,desc,maxlen); p[maxlen]=0; }
        return true;
    }

	char *retp_desc(void) {
		return desc;
	}
	
	bool get_40(sds_real *p) {
        int fi1;
        *p=0.0;
        if (dash!=NULL) for (fi1=0; fi1<ndashes; fi1++) *p+=fabs(dash[fi1].len);
        return true;
    }
 	sds_real ret_patternlength(void) {
        int fi1;
		sds_real ret;
        ret=0.0;
        if (dash!=NULL) for (fi1=0; fi1<ndashes; fi1++) ret+=fabs(dash[fi1].len);
        return ret;
    }
    bool get_49(sds_real *p, int n) {  /* WARNING: Fills an array. */
        int fi1;
        if (p!=NULL && dash!=NULL) {
            for (fi1=0; fi1<n; fi1++)
                p[fi1]=(fi1<ndashes) ? dash[fi1].len : 0.0;
        }
        return true;
    }
    bool get_72(int *p) { *p=aligncd; return true; }
	int ret_aligncd(void) { return (int)aligncd; }
    
	bool get_73(int *p) { *p=ndashes; return true; }
	int ret_ndashes(void) { return ndashes; }

    bool set_3(char *p) {
        db_astrncpy(&desc,(p==NULL) ? db_str_quotequote : p,DB_MAXLTYPEDESCLEN);
        return true;
    }
	db_ltypedashlink *retp_dash(void) { return dash; }
    
	bool set_49(sds_real *p, int n) {
        /* WARNING: Reads an array of n. (Also see set_dashdata().) */

        int fi1;

        if (p!=NULL) {
            for (fi1=0; fi1<ndashes; fi1++)
                { dash[fi1].len=(fi1<n) ? p[fi1] : 0.0; }
        }
        return true;
    }
    bool set_72(int p) { aligncd=(char)p; return true; }
    bool set_73(int p) {  /* Allocates the dash[] array. */
        delete [] dash; dash=NULL; ndashes=0;
        if (p>0) { ndashes=charCast(p); dash=new db_ltypedashlink[ndashes]; }
        return true;
    }

    int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
    int entmod(struct resbuf *modll, db_drawing *dp);
	db_handitem *ret_xrefblkhip(void) { return(xrefblkhip); }
	void set_xrefblkhip(db_handitem *p) { xrefblkhip=p; }

	/*****************************************************************************************
	* Author:		Denis Petrov
	* Description:	Returns number of linetype elements.
	*****************************************************************************************/
	int nElements() const { return ndashes; }
	/*****************************************************************************************
	* Author:		Denis Petrov
	* Description:	Returns reference to structure which containing information about linetype
	*				element with index n. Method doesn't control n on array bounds.
	*****************************************************************************************/
	db_ltypedashlink& element(int n) { return dash[n]; }

  private:

    char             *desc;       /*   3 */
 /* sds_real          patlen;     /*  40 (not used) */
    char              aligncd;    /*  72 */
    char              ndashes;    /*  73 */
    db_ltypedashlink *dash;  /* ndashes of them */
    db_handitem		 *xrefblkhip;  /* pointer to blockheader for xref from which this came, if any */
};


