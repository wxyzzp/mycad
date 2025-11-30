#ifndef __cmdsDimVarsH__
#define __cmdsDimVarsH__

// These constants used for cmd_dimvars plinkvar[] array in cmdsDimVars.cpp.
const int DIMANG     =-1;
const int DIMADEC    =0;
const int DIMALT     =1;
const int DIMALTD    =2;
const int DIMALTF    =3;
const int DIMALTTD   =4;
const int DIMALTTZ   =5;
const int DIMALTU    =6;
const int DIMALTZ    =7;
const int DIMAPOST   =8;
const int DIMASO     =9;
const int DIMASZ     =10;
const int DIMAUNIT   =11;
const int DIMBLK     =12;
const int DIMBLK1    =13;
const int DIMBLK2    =14;
const int DIMCEN     =15;
const int DIMCLRD    =16;
const int DIMCLRE    =17;
const int DIMCLRT    =18;
const int DIMDEC     =19;
const int DIMDLE     =20;
const int DIMDLI     =21;
const int DIMEXE     =22;
const int DIMEXO     =23;
const int DIMFIT     =24;
const int DIMGAP     =25;
const int DIMJUST    =26;
// EBATECH(watanabe)-[dimldrblk
const int DIMLDRBLK  =27;
// ]-EBATECH(watanabe)
const int DIMLFAC    =28;
const int DIMLIM     =29;
const int DIMPOST    =30;
const int DIMRND     =31;
const int DIMSAH     =32;
const int DIMSCALE   =33;
const int DIMSD1     =34;
const int DIMSD2     =35;
const int DIMSE1     =36;
const int DIMSE2     =37;
const int DIMSHO     =38;
const int DIMSOXD    =39;
const int DIMSTYLE   =40;
const int DIMTAD     =41;
const int DIMTDEC    =42;
const int DIMTFAC    =43;
const int DIMTIH     =44;
const int DIMTIX     =45;
const int DIMTM      =46;
const int DIMTOFL    =47;
const int DIMTOH     =48;
const int DIMTOL     =49;
const int DIMTOLJ    =50;
const int DIMTP      =51;
const int DIMTSZ     =52;
const int DIMTVP     =53;
const int DIMTXSTY   =54;
const int DIMTXT     =55;
const int DIMZIN     =56;
const int DIMUNIT    =57;
const int DIMUPT     =58;
const int DIMTZIN    =59;
const int DIMFRAC	 =60;
const int DIMLUNIT   =61;
// EBATECH(watanabe)-[dimatfit, dimtmove
const int DIMATFIT   =62;
const int DIMTMOVE   =63;
// ]-EBATECH(watanabe)
const int DIMAZIN    =64;
const int DIMALTRND  =65;
const int DIMDSEP    =66;
const int DIMLWD     =67;
const int DIMLWE     =68;
const int DIM_LASTDIMVAR = 69;

#define AD_XD_STRING            1000
#define AD_XD_REAL              1040
#define AD_XD_INT               1070
#define AD_XD_APPOBJHANDLE      1001
#define AD_XD_CONTROL           1002
#define XDATA_CODE_DATABHANDLE  1005

#define DIM_ACADAPP       "ACAD"/*DNT*/
#define DIM_DSTYLE        "DSTYLE"/*DNT*/

struct cmd_dimvals 
{
        BOOL      oride;
        sds_real  rval,
                  roval;
        int       ival,
                  ioval;
        char     *cval,
                 *coval;
};

struct cmd_dimvars 
{
	
    char               *Var;
    short               DXF;
    int                 DescIdx;
    short               type;
    cmd_dimvals			dval;
};

void cmd_geteedvalues(db_handitem *elp,db_drawing *flp);
void cmd_buildEEdList(struct resbuf *link, BOOL bReadDimStyle=FALSE);
//void cmd_buildEEdList(struct resbuf *link);
void cmd_dimSetVars();
short cmd_CleanUpDimVarStrings(void);
void cmd_setplinkvar(db_drawing *flp);

#endif
