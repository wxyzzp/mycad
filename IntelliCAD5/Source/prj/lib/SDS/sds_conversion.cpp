/* LIB\SDS\SDS_CONVERSION.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *	Conversion Functions
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "sds_engine.h" /*DNT*/
#include "cmds.h"/*DNT*/
#include <fcntl.h>
#include <locale.h> // EBATECH(CNBR)

#define Velocity_of_light	299792458e-1
#define Electron_charge 	1.60209e-19
#define Plancks_constant	6.6260755e-34
#define Boltzmans_constant	1.380658e-23
#define Electron_mass		9.1093897e-31
#define SDS_ALPHA_CHAR		1
#define SDS_DIGIT_CHAR		2

struct SDS_CvFundamental {
	char   **szUnitNames,
			*szDiriveString;
	int 	 iStrCntr;
	sds_real dC_VelocityOfLight,
			 dC_ElectronCharge,
			 dC_PlancksConstant,
			 dC_BoltzmansConstant,
			 dC_ElectronMass,
			 dC_Multiplier,
			 dC_AdditiveOffset;
	struct SDS_CvFundamental *next;
};

static SDS_CvFundamental SDS_FundamentalCompA,
						 SDS_FundamentalCompB,
						 SDS_FundamentalHold;

//Simple SDS_CvFundamental Resource Manager
class CvFundamentalPtr
{
public:
	CvFundamentalPtr() : m_pCvFundamental(NULL) {};
	~CvFundamentalPtr()
		{
		struct SDS_CvFundamental *pTemp, *pCurrent;

 		pCurrent = m_pCvFundamental;
		while (pCurrent)
			{
			pTemp = pCurrent->next;

			for(int iCntrIndex=0;iCntrIndex<pCurrent->iStrCntr;++iCntrIndex)
				{
				delete [] pCurrent->szUnitNames[iCntrIndex];
				}

			delete [] pCurrent->szUnitNames;
			delete [] pCurrent->szDiriveString;
			delete pCurrent;
			pCurrent = pTemp;
			}
		}
	operator SDS_CvFundamental*() { return m_pCvFundamental; }
	CvFundamentalPtr& operator =(SDS_CvFundamental* pCvFundamental)
		{
		m_pCvFundamental = pCvFundamental;
		return *this;
		}

private:
	CvFundamentalPtr(const CvFundamentalPtr&);
	CvFundamentalPtr& operator=(const CvFundamentalPtr&);

	SDS_CvFundamental *m_pCvFundamental;
};
//SDS_CvFundamental *SDS_BeginFundamental=NULL
static CvFundamentalPtr SDS_BeginFundamental;

static char *sds_szCVString;

//** Functions

/****************************************************************************/
/********************* Conversion Functions *********************************/
/****************************************************************************/

char	*sds_CVParseString(const char *szValue);
sds_real sds_eval(sds_real r1,sds_real r2,short action,short power);
short	 sds_getactiontype(char *string,short *index);
void	 sds_copyfund(struct SDS_CvFundamental *FrmCvptr,struct SDS_CvFundamental *ToCvptr);
BOOL	 sds_CVCalculateValue(short taction, const char *szFrmUnit,const char *pCToUnit,double *fNewValue);
//BOOL	sds_CVCalculateValue(double CVNum, const char *szOldUnit,struct SDS_CvFundamental *pCNewUnit,double *fNewValue);
int 	 sds_CVSearchLink(struct SDS_CvFundamental **pCVunitLink,char *szUnitName);
BOOL	 sds_CVunitMakeLink(struct SDS_CvFundamental **ppLinkPtr);
int 	 sds_dms2rad(char *dmsstr, sds_real *result);
int 	 sds_rad2dms(sds_real radval, int prec, char *result);
int 	 sds_angtof_convert(int ignoremode,const char *szAngle, int nUnitType, double *pdAngle);

// *****************************************
// This is an SDS External API
//
int sdsengine_angtof(const char *szAngle, int nUnitType, double *pdAngle)
	{
	return SDS_angtof_convert(0,szAngle,nUnitType,pdAngle);
	}

// EBATECH(CNBR) -[ Get degree sign in any languages.
char *degree_sign()
{
	static wchar_t src[2] = { 0x00b0, 0x0000 };
	static char dest[8];
	int sts = WideCharToMultiByte( GetACP(), 0, src, -1, dest, sizeof(dest), NULL, NULL );
	if( sts == 0 )
	{
		dest[0] = (char)0xb0;
		dest[1] = 0x00;
	}
	return dest;
}
// EBATECH(CNBR) ]

int
SDS_angtof_convert
(
 int			mode,
 const char*	szAngle,
 int			nUnitType,
 double*		pdAngle
)
{
//***************************************************************************
//*** This function converts "szAngle", which represents an angle in the display
//*** format specified by "nUnitType" (huh-huh), into a double-precision floating-
//*** point value.	The result is placed in "pdAngle" expressed in radians.
//*** If "nUnitType" (huh-huh) is (-1) then the value set by AUNITS is used.
//***
//*** If Mode&1, f'n ignores setting of ANGBASE
//*** If Mode&2, f'n does not normalize result to have fabs(pdAngle)<IC_TWOPI
//*** If Mode&4, f'n ignores setting of ANGDIR
//***
//*** Returns: RTNORM - success; otherwise - RTERROR.
//***************************************************************************
	char		*fs1, fc1, *fcp1, *fcp2;
	sds_real	fr1, angbase;
	resbuf		rb;
	int			angdir;
	// EBATECH(CNBR)-[ In Korean, degree mark is 2bytes.
	char* deg_sign = degree_sign();
	// ]-EBATECH

	if(!szAngle)
		return RTERROR;

	CString str(szAngle);
	str.Replace(degree_sign(), "d");  // In Korean, degree mark is 2bytes. 
	fs1 = str.GetBuffer(0);
	//fs1 = (char*)calloc(1, strlen(szAngle) + 1);
	//strcpy(fs1, szAngle);

	angbase = SDS_getvar(NULL, DB_QANGBASE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rreal : 0.0;
	angdir = SDS_getvar(NULL, DB_QANGDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rint : 0;

#ifdef LOCALISE_ISSUE
	// The following code assumptions are dangerous.
#endif
	//NOTE: REGARDLESS of what they pass, if we find a	marker, we know they want a certain
	//			conversion.  Markers are:
	//				R or r			= radians
	//				N,n,S,s,E,e,W,w	= survey
	//				G or g			= gradians
	//				"				= dd/mm/ss
	//				d or D			= Decimal degrees
	//			We'll check for D,d after " - if no seconds, assume decimal degrees

		 if(strchr(fs1, 'N'/*DNT*/) || strchr(fs1, 'n'/*DNT*/) || strchr(fs1, 'S'/*DNT*/) || strchr(fs1, 's'/*DNT*/) ||
			strchr(fs1, 'E'/*DNT*/) || strchr(fs1, 'e'/*DNT*/) || strchr(fs1, 'W'/*DNT*/) || strchr(fs1, 'w'/*DNT*/))
			nUnitType = 4;
	else if(strchr(fs1, 'G'/*DNT*/) || strchr(fs1, 'g'/*DNT*/))
			nUnitType = 2;
	else if(strchr(fs1, 'R'/*DNT*/) || strchr(fs1, 'r'/*DNT*/))
			nUnitType = 3;
	else if(strchr(fs1, '"'/*DNT*/) || strchr(fs1, '\''/*DNT*/))
			nUnitType = 1;
	//EBATECH(CNBR) In Korean, degres mark is 2bytes
	//else if(strchr(fs1, 'D'/*DNT*/) || strchr(fs1, 'd'/*DNT*/) || strchr(fs1, deg_sign[0]))
	else if(strchr(fs1, 'D'/*DNT*/) || strchr(fs1, 'd'/*DNT*/) || strstr(fs1, deg_sign))
			nUnitType = 0;
	else if(nUnitType < 0)
			nUnitType = SDS_getvar(NULL, DB_QAUNITS, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ?
						rb.resval.rint : 0;
	else if(nUnitType > 4)
		nUnitType = 4;

	switch(nUnitType)
	{
		case 0:
			if(!isdigit((unsigned char)*fs1) && *fs1 != '-'/*DNT*/ && *fs1 != '.'/*DNT*/)
				goto bailbad;
			if(fcp1 = strchr(fs1, 'D'/*DNT*/))
				*fcp1 = 0;
			if(fcp1 = strchr(fs1, 'd'/*DNT*/))
				*fcp1 = 0;
			//EBATECH(CNBR) [ In Korean degree sign is 2 bytes.
			//if(fcp1 = strchr(fs1, deg_sign[0]))
			//	*fcp1 = 0;
			if(fcp1 = strstr(fs1, deg_sign))
				*fcp1 = 0;
			//EBATECH(CNBR) ]
			// If all characters are NOT digits, bail.
			fcp1 = fs1;  ++fcp1;	// Already checked the first character.
			while(*fcp1)
			{
				if(!isdigit((unsigned char)*fcp1) && *fcp1 != '.'/*DNT*/)
					goto bailbad;
				else
					++fcp1;
			}
			fr1 = atof(fs1) * IC_PI / 180.0;
			break;

		case 1:
			if(sds_dms2rad(fs1, &fr1) != RTNORM)
				goto bailbad;
			break;

		case 2:
			if(!isdigit((unsigned char)*fs1) && *fs1 != '-'/*DNT*/ && *fs1 != '.'/*DNT*/)
				goto bailbad;
			fr1 = atof(fs1) * IC_PI / 200.0;
			break;

		case 3:
			if(!isdigit((unsigned char)*fs1) && *fs1 != '-'/*DNT*/ && *fs1 != '.'/*DNT*/)
				goto bailbad;
			fr1 = atof(fs1);
			break;

		case 4:
			switch(*fs1)
			{
				case 'N'/*DNT*/:
				case 'n'/*DNT*/:
					// Find out what second char in string is.
					switch(fs1[1])
					{
						case 0:
							fr1 = IC_PI * 0.5;
							goto bailgood;
						case ' '/*DNT*/:
							fcp1 = fs1 + 2;
							break;
						default:
							if(isdigit((unsigned char)fs1[1]))
								fcp1 = fs1 + 1;
							else
								goto bailbad;
					}

					// This loop points fcp2 to the end of the DMS string
					for(fcp2 = fcp1 + 1;
						isdigit((unsigned char)*fcp2) || *fcp2 == 'd'/*DNT*/ || *fcp2 == 'D'/*DNT*/ || *fcp2 == '\''/*DNT*/ || *fcp2 == '\"'/*DNT*/;
						++fcp2)
						;

					// Convert DMS string to radians
					fc1 = *fcp2;
					*fcp2 = 0;
					if(sds_dms2rad(fcp1, &fr1) != RTNORM || fr1 < 0.0 || fr1 > IC_PI * 0.5)
						goto bailbad;
					*fcp2 = fc1;
					fcp1 = fcp2;

					if(*fcp1 == ' '/*DNT*/)
						++fcp1;
					if(fcp1[1])
						goto bailbad;

					switch(*fcp1)
					{
						case 'E'/*DNT*/:
						case 'e'/*DNT*/:
							fr1 = IC_PI * 0.5 - fr1;
							goto bailgood;
						case 'W'/*DNT*/:
						case 'w'/*DNT*/:
							fr1 = IC_PI* 0.5 + fr1;
							goto bailgood;
						default:
							goto bailbad;
					}

				case 'S'/*DNT*/:
				case 's'/*DNT*/:
					// Find out what second char in string is.
					switch(fs1[1])
					{
						case 0:
							fr1 = IC_PI + IC_PI * 0.5;
							goto bailgood;
						case ' '/*DNT*/:
							fcp1 = fs1 + 2;
							break;
						default:
							if(isdigit((unsigned char)fs1[1]))
								fcp1 = fs1 + 1;
							else
								goto bailbad;
					}

					// This loop points fcp2 to the end of the DMS string
					for(fcp2 = fcp1 + 1;
						isdigit((unsigned char)*fcp2) || *fcp2 == 'd'/*DNT*/ || *fcp2 == 'D'/*DNT*/ || *fcp2 == '\''/*DNT*/ || *fcp2 == '\"'/*DNT*/ || *fcp2 == '.'/*DNT*/;
						++fcp2)
						;

					// Convert DMS string to radians
					fc1 = *fcp2;
					*fcp2 = 0;
					if(sds_dms2rad(fcp1, &fr1) != RTNORM || fr1 < 0.0 || fr1 > IC_PI * 0.5)
						goto bailbad;
					*fcp2 = fc1;
					fcp1 = fcp2;

					if(*fcp1 == ' '/*DNT*/)
						++fcp1;
					if(fcp1[1])
						goto bailbad;

					switch(*fcp1)
					{
						case 'E'/*DNT*/:
						case 'e'/*DNT*/:
							fr1 = IC_PI + IC_PI * 0.5 + fr1;
							goto bailgood;
						case 'W'/*DNT*/:
						case 'w'/*DNT*/:
							fr1 = IC_PI + IC_PI * 0.5 - fr1;
							goto bailgood;
						default:
							goto bailbad;
					}

				case 'E'/*DNT*/:
				case 'e'/*DNT*/:
					fr1 = 0.0;
					goto bailgood;

				case 'W'/*DNT*/:
				case 'w'/*DNT*/:
					fr1 = IC_PI;
					goto bailgood;

				default:
					// If we make it this far, treat input as if it were a decimal angle.
					if(!isdigit((unsigned char)*fs1) && *fs1 != '-'/*DNT*/ && *fs1 != '.'/*DNT*/)
						goto bailbad;
					if(fcp1 = strchr(fs1, 'D'/*DNT*/))
						*fcp1 = 0;
					if(fcp1 = strchr(fs1, 'd'/*DNT*/))
						*fcp1 = 0;
					// EBATECH In Korean, degree mark is 2bytes [
					//if(fcp1 = strchr(fs1, deg_sign[0]))
					//	*fcp1 = 0;
					if(fcp1 = strstr(fs1, deg_sign))
						*fcp1 = 0;
					//EBATECH(CNBR) ]
					// If all characters are NOT digits, bail.
					fcp1 = fs1;  ++fcp1;	// Already checked the first character.
					while(*fcp1)
					{
						if(!isdigit((unsigned char)*fcp1) && *fcp1 != '.'/*DNT*/)
							goto bailbad;
						else
							++fcp1;
					}
					fr1 = atof(fs1) * IC_PI / 180.0;
					goto bailgood;

			}	// switch(*fs1)

		default:
			goto bailbad;
	}	// switch(nUnitType)

bailgood :

	if(fabs(fr1) > 1.0 / IC_ZRO)
		goto bailbad;
	if(!(mode & 1))
	{
		if(angdir)
			fr1 = angbase - fr1;
		else
			fr1 += angbase;
	}

	if(!(mode & 2))
		ic_normang(&fr1, NULL);	//Normalize BEFORE accounting for angdir

	if(!(mode & 4) && angdir && (mode & 1))
		fr1 = IC_TWOPI - fr1;

	*pdAngle = fr1;
	//IC_FREE(fs1);

	return RTNORM;

bailbad :

	//IC_FREE(fs1);

	return RTERROR;
}

int sds_dms2rad(char *dmsstr, sds_real *result) {
//*****************************************************************************
//*** This function converts "dmsstr" (which contains degrees, minutes, and
//*** seconds in the form 12d12'1.23") to radians and sets "result" to it.
//*** Returns: success - RTNORM; bad string in dmsstr - RTERROR.
//*****************************************************************************
	char *fs1=NULL,*fcp1;
	sds_real fr1,deg,min,sec;
	int caseflg=0; //*** This flag keeps track of which cases in the switch have
					 //*** been completed.
	int fi1;
	sds_real negmul=1.0;

	fr1=deg=min=sec=0.0;
	if(dmsstr==NULL) return (RTERROR);
	if (dmsstr[0]=='-')
		{
		dmsstr++;  // skip the minus
		negmul=-1.0;
		}

	CString str(dmsstr);
	// degree_sign have alredy replaced by "d" in SDS_angtof_convert()
	// while sds_dms2rad() is used only from SDS_angtof_convert() no need to call str.Replace(degree_sign(), "d"); here
	ASSERT(str.Find(degree_sign(), 0) == -1); 
	fs1 = str.GetBuffer(0);
	//fs1=(char *)calloc(1,strlen(dmsstr)+1);
	//strcpy(fs1,dmsstr);

	for(fi1=0,fcp1=fs1; fs1[fi1]!=0; fi1++,fcp1=fs1+fi1) {
		if(fi1==0 && fs1[0]=='-'/*DNT*/)fi1=1;		//account for leading minus sign
		for(;isdigit((unsigned char) fs1[fi1]) || fs1[fi1]=='.'/*DNT*/; fi1++);	//account for decimal fractions
		switch(fs1[fi1]) {
			case 0:
				//*** This is a default, if nothing(d,',") is trailing the first array of digits.
				if(caseflg) goto bail;
				(fs1[fi1])=0; deg=atof(fcp1);
				*result=negmul*(deg*IC_PI/180.0);
				//IC_FREE(fs1);
				return(RTNORM);
			case 'd'/*DNT*/: case 'D'/*DNT*/:
				//*** This should be the fist case entered, only; if not, bail.
				if(caseflg) goto bail;
				(fs1[fi1])=0; deg=atof(fcp1);
				caseflg=1;
				break;
			case '\''/*DNT*/:
				//*** This should be the fist case entered, or only after
				//*** degrees.
				if(caseflg>1) goto bail;
				(fs1[fi1])=0; min=atof(fcp1);
				deg+=(min/60.0);
				caseflg+=2;
				break;
			case '\"'/*DNT*/:
				//*** This should be the fist case entered, or only after
				//*** minutes or degrees.
				if(caseflg>3) goto bail;
				sec=atof(fcp1);
				deg+=(sec/3600.0);
				caseflg+=4;
				break;
			default: goto bail;
		}
	}
	fr1=negmul*(deg*IC_PI/180.0);
	ic_normang(&fr1,NULL);

	//IC_FREE(fs1);
	*result=fr1;
	return(RTNORM);

	bail:

	//IC_FREE(fs1);
	return(RTERROR);
}

// EBATECH(watanbe)-[degree mark needs localize
static
BOOL
ReplaceDegreeMark(
  const int ignoremode,
  char*     szAngle
)
{
  if (ignoremode & 2)
    {
      CString str = szAngle;
      str.Replace("d", "%%d");
      str.Replace("D", "%%d");
	// EBATECH(CNBR)-[ In Korean, degree mark is 2bytes.
	char* deg_sign = degree_sign();
	  str.Replace("%%d", deg_sign);
	// ]-EBATECH
      strcpy(szAngle, str.GetBuffer(str.GetLength()));
    }
  return TRUE;
}
// ]-EBATECH(watanabe)

// NOTE:  Despite the name, this NOT directly callable from external apps
// NOTE:  Add dimazin(0-3, default=-1) Bugzilla#78443 DIMAZIN 2003/2/6
// NOTE:  Add dimdsep(0-255, default=0) 2003/3/31 DIMDSEP
static int
sds_angtos_base(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle,BOOL mode, int dimazin=-1, int dimdsep=0 ) {
//sds_angtos_base(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle,BOOL mode)
/*
**	We try to follow the SPIRIT of the real function.  To duplicate
**	it would be almost impossible, requiring LOTS of experimentation.
**	Not worth it.
**
**	Note that the real function uses ACAD system variables in certain
**	cases.	Ours doesn't (yet).
*****
**	I added the ACAD system variable stuff and the case 1 and case 4
**	code.  JJC 3/27/95
*****
**	8/13/96 Added checking for ANGBASE and ANGDIR
*****
*****
**	Added ignoremode&1 to ignore current settings for angbase & angdir
**	Added ignoremode&2 to use degree symbol instead of 'd' in degree conversion
**	Added ignoremode&4 to allow the end angle to be 2pi instead of 0
*****
**	Added dimazin argments for dimension override system variable.
**	if dimazin == -1, get DIMZIN system variable for 0 suppress control.
*/
	char *fcp1,*fcp2;
	struct resbuf rb;
	int rc,fi1,unitmode,dimzin;
	sds_real fr1;

	rc=RTNORM;

	if (szAngle==NULL) return RTERROR;

	*szAngle=0;

	if (nUnitType<0) {
		if(SDS_getvar(NULL,DB_QAUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) nUnitType=rb.resval.rint;
		else nUnitType=0;
	} else if (nUnitType>4) nUnitType=4;

	if (nPrecision<0) {
		if(SDS_getvar(NULL,DB_QAUPREC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) nPrecision=rb.resval.rint;
		else nPrecision=0;
	}

	if(0==(ignoremode&1) && SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM)
	{	
		sds_real angbase = rb.resval.rreal; // Vladimir V. // support of CW direction was added
		if(SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM && rb.resval.rint==1)
			dAngle = angbase - dAngle;
		else
			dAngle += angbase;
	}

	if(ignoremode&4){
		if(dAngle<0.0) {
			do{dAngle+=IC_TWOPI;}while(dAngle<0.0);
		}else if(dAngle>IC_TWOPI) {
			do{dAngle-=IC_TWOPI;}while(dAngle>IC_TWOPI);
		}
	}else{
		ic_normang(&dAngle,NULL);  //Normalize BEFORE accounting for angdir
	}

	fi1=max(nPrecision,10);
	if(!(ignoremode&4)){
		if(dAngle<pow(10.0,-fi1)){
			dAngle=0.0;
		}else if((IC_TWOPI-dAngle)<pow(10.0,-fi1)){
			dAngle=0.0;
		}
	}
	unitmode=(SDS_getvar(NULL,DB_QUNITMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? rb.resval.rint : 0;

	// EBATECH(CNBR) -[ Bugzilla#78443 DIMZIN use by angtos. DIMAZIN use by object.
	switch( dimazin )
	{
	case 0:	// don't remove
		dimzin = 0;
		break;
	case 1:	// remove head of zeros
		dimzin = 4;
		break;
	case 2:	// remove trail of zeros
		dimzin = 8;
		break;
	case 3:	// remove both head and trail of zeros
		dimzin = 12;
		break;
	default:
		SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		dimzin=rb.resval.rint;
		break;
	}
	//SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//dimzin=rb.resval.rint;
	// EBATECH(CNBR) ]-

	switch (nUnitType) {
		case 0: case 2: case 3:  /* Degrees; Grads; Radians */

			if (nUnitType!=3) dAngle*=180.0/IC_PI;
			if (nUnitType==2) dAngle*=10.0/9.0;
			fi1=(dAngle<1.0) ? 30 : 15; if (nPrecision>fi1) nPrecision=fi1;

			sprintf(szAngle,"%0.*f"/*DNT*/,nPrecision,dAngle);
			// EBATECH(CNBR) -[ Bugzilla#78443 DIMZIN use by angtos. DIMAZIN use by object.
			if( mode )
			{
				if( dAngle < 1.0 && (dimzin & 4 ))
				{
					fcp2=strchr(szAngle,'.'/*DNT*/);
					if ( fcp2 )
					{
						fi1 = strlen(fcp2);
						memmove(szAngle, fcp2, fi1);
						szAngle[fi1] = 0;
					}
				}
				if((dimzin & 8 ) != 0) 
				//if (mode && (dimzin==8 || dimzin==12))
				{
					// supress trailing zeros
					for( fcp2 = szAngle + strlen(szAngle) -1 ; *fcp2 == '0' && fcp2 != szAngle ; --fcp2 );
					if( *fcp2 != '.')
					{
						*(fcp2+1) = 0; 
					}
					else
					{
						*fcp2 = 0;
					}
					//fcp2=strchr(szAngle,'.'/*DNT*/);
					//if ( fcp2 )
					//	{
					//	while(fcp1=strrchr((fcp2+1),'0'/*DNT*/)) { if (fcp1) *fcp1=0; }
					//	// progesoft
					//	// fcp2 is always a dot so you can check only if the next 
					//	// char is null terminator, if this you must truncate also dot
					//	//if (fcp1==fcp2 || *fcp2=='.'/*DNT*/) *fcp2=0;
					//	if (fcp1==fcp2 || *(fcp2+1)==0/*DNT*/) *fcp2=0;
					//	// end Progesoft
					//}
				}
				if( *szAngle == 0 )
				{
					strcpy( szAngle, "0"/*DNT*/);
				}
			}
			// ]- EBATECH(CNBR) ]-

			if (nUnitType==2 || nUnitType==3) {
				fcp1=szAngle; while  (*fcp1) fcp1++;
				*fcp1++=(nUnitType==2) ? 'g'/*DNT*/ : 'r'/*DNT*/; *fcp1=0;
			}
			// EBATECH(CNBR) -[ (angtos num 0) no degree sign
			//if (nUnitType==0)
			//{
			//	char degree[2] = {(char)'\xb0', '\0'};/*DNT*/
			//	strcat(szAngle, degree);
			//}
			// EBATECH ]-
			// EBATECH(CNBR) -[ 2003/3/31 DIMDSEP
			if( dimdsep != 0 && dimdsep != '.' && ( fcp1 = strchr(szAngle, '.')) != NULL )
			{
				*fcp1 = (char)dimdsep;
			}
			// EBATECH(CNBR) ]-
			break;

		case 1:  /* DMS */
			rc=sds_rad2dms(dAngle,nPrecision,szAngle);

			// EBATECH(watanbe)-[degree mark needs localize
			//if(ignoremode&2){
			//	for(fcp1=szAngle;*fcp1!=0;fcp1++){
			//		// EBATECH(CNBR) -[ degree mark needs localize
			//		if(*fcp1=='d'/*DNT*/ || *fcp1=='D'/*DNT*/)
			//			*fcp1=deg_sign[0];
			//		// EBATECH ]-
			//	}
			//}
			ReplaceDegreeMark(ignoremode, szAngle);
			// ]-EBATECH(watanabe)
			break;

		case 4:  /* Surveyor's units (bearings) */
			//NOTE: If UNITMODE is 0, include spaces in string. If 1, no spaces!
			{
			char fs1[IC_ACADBUF];
#ifdef LOCALISE_ISSUE
// Possible to set up 4 strings, N, S, E, W, and pass them into the
// sprintf as parameters - removes need for translating in the formating
// arg.
#endif
			if(icadAngleEqual(dAngle,0.0) || icadAngleEqual(dAngle,IC_TWOPI)) {
				//Modified SAU 14/06/2000			[
				//Reason:(angtos) does not handle precision correctly
				// for surveyor's units
				if(nPrecision==0 && unitmode==0)
					sprintf(szAngle,"N 90d E"/*DNT*/);
				else if(nPrecision==0 && unitmode)
					sprintf(szAngle,"N90dE"/*DNT*/);
				else
					sprintf(szAngle,"E"/*DNT*/);
			} else if(dAngle>0.0 && dAngle<(IC_PI/2.0)) {
				fr1=((IC_PI/2.0)-dAngle);
				if(sds_rad2dms(fr1,nPrecision,fs1)!=RTNORM) return(RTERROR);
				if(unitmode)
					sprintf(szAngle,"N%sE"/*DNT*/,fs1);
				else
					sprintf(szAngle,"N %s E"/*DNT*/,fs1);
			} else if(dAngle==(IC_PI/2.0)) {
				if(nPrecision==0 && unitmode==0)
					sprintf(szAngle,"N 0d W"/*DNT*/);
				else if(nPrecision==0 && unitmode)
					sprintf(szAngle,"N0dW"/*DNT*/);
				else
				{
					*szAngle='N'/*DNT*/;
					szAngle[1]=0;
				}
			} else if(dAngle>(IC_PI/2.0) && dAngle<IC_PI) {
				fr1=(dAngle-(IC_PI/2.0));
				if(sds_rad2dms(fr1,nPrecision,fs1)!=RTNORM) return(RTERROR);
				if(unitmode)
					sprintf(szAngle,"N%sW"/*DNT*/,fs1);
				else
					sprintf(szAngle,"N %s W"/*DNT*/,fs1);
			} else if(dAngle==IC_PI) {
				if(nPrecision==0 && unitmode==0)
					sprintf(szAngle,"S 90d W"/*DNT*/);
				else if(nPrecision==0 && unitmode)
					sprintf(szAngle,"S90dW"/*DNT*/);
				else
				{
					*szAngle='W'/*DNT*/;
					szAngle[1]=0;
				}
			} else if(dAngle>IC_PI && dAngle<((3.0*IC_PI)/2.0)) {
				fr1=(((3.0*IC_PI)/2.0)-dAngle);
				if(sds_rad2dms(fr1,nPrecision,fs1)!=RTNORM) return(RTERROR);
				if(unitmode)
					sprintf(szAngle,"S%sW"/*DNT*/,fs1);
				else
					sprintf(szAngle,"S %s W"/*DNT*/,fs1);
			} else if(dAngle==((3.0*IC_PI)/2.0)) {
				if(nPrecision==0 && unitmode==0)
					sprintf(szAngle,"S 0d E"/*DNT*/);
				else if(nPrecision==0 && unitmode)
					sprintf(szAngle,"S0dE"/*DNT*/);
				else
				{
					*szAngle='S'/*DNT*/;
					szAngle[1]=0;
				}
			} else if(dAngle>((3.0*IC_PI)/2.0) && dAngle<IC_TWOPI) {
				fr1=(dAngle-((3.0*IC_PI)/2.0));
				if(sds_rad2dms(fr1,nPrecision,fs1)!=RTNORM) return(RTERROR);
				if(unitmode)
					sprintf(szAngle,"S%sE"/*DNT*/,fs1);
				else
					sprintf(szAngle,"S %s E"/*DNT*/,fs1);
			}
			//Modified SAU 14/06/2000					   ]

			// EBATECH(watanabe)-[degree mark needs localize
			//if(ignoremode&2){
			//	for(fcp1=szAngle;*fcp1!=0;fcp1++){
			//		// EBATECH(CNBR) -[ degree mark needs localize
			//		if(*fcp1=='d'/*DNT*/ || *fcp1=='D'/*DNT*/)
			//			*fcp1=deg_sign[0];
			//		// EBATECH ]-
			//	}
			//}
			ReplaceDegreeMark(ignoremode, szAngle);
			// ]-EBATECH(watanabe)
			}
			break;
	}

	return rc;
} // end of sds_angtos_base



// *****************************************
// This is an SDS External API
//
int sdsengine_angtos(double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	return(sdsengine_angtos_convert(2,dAngle,nUnitType,nPrecision,szAngle));
	}

// *****************************************
// This is an SDS External API
//
int sdsengine_angtos_end(double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	return(sds_angtos_base(4,dAngle,nUnitType,nPrecision,szAngle,0));
	}

// *****************************************
// This is an SDS External API
//
int sdsengine_angtos_absolute(double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	return(sdsengine_angtos_convert(3,dAngle,nUnitType,nPrecision,szAngle));
	}

// *****************************************
// This is an SDS External API
// EBATECH(CNBR) -[ Bugzilla#78443 DIMAZIN 2002/3/31 DIMDSEP
int sdsengine_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle, int nDimazin, int nDimdsep)
//int sdsengine_angtos_dim(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	return(sds_angtos_base(ignoremode,dAngle,nUnitType,nPrecision,szAngle,1,nDimazin,nDimdsep));
	}
// EBATECH(CNBR) ]-
// *****************************************
// This is an SDS External API
//
int sdsengine_angtos_convert(int ignoremode, double dAngle, int nUnitType, int nPrecision, char *szAngle)
	{
	return(sds_angtos_base(ignoremode,dAngle,nUnitType,nPrecision,szAngle,0));
	}

int sds_rad2dms(sds_real radval, int prec, char *result) {
//*****************************************************************************
//*** This function converts "radval" expressed in radians to a DMS string
//*** (which contains degrees, minutes, and seconds in the form 12d12'1.23")
//*** and sets "result" to it.	The "prec" argument selects the number of
//*** decimal places of precision to include in the string.
//*** Returns: success - RTNORM; otherwise - RTERROR.
//*****************************************************************************
	char fs1[IC_ACADBUF];
	sds_real fr1;
	int fi1,fi2,fi3;

	if(result==NULL) return(RTERROR);
	*result=0;

	fr1=radval;
	if(fr1>IC_TWOPI) {
		fi1=(int)(fr1/IC_TWOPI);
		fr1-=(fi1*IC_TWOPI);
	}

	fr1*=(180.0/IC_PI);

	if(prec<0) return(RTERROR);
	/*** Degrees ***/
	fi1=(int)fr1;
	if((fr1-=fi1)>=0.5 && prec<1) fi1++;
	sprintf(result,"%dd"/*DNT*/,fi1);
	if(prec>=1) { /*** Minutes ***/
		fr1*=60.0;
		fi2=(int)fr1;
		if((fr1-=fi2)>=0.5 && prec<3) fi2++;
		if(fi2==60) {
			fi1++;
			sprintf(result,"%dd"/*DNT*/,fi1);
			fi2=0;
		}
		sprintf(fs1,"%d'"/*DNT*/,fi2);
		strcat(result,fs1);
	}
	if(prec>=3) { /*** Seconds ***/
		fr1*=60.0;
		if(prec>4) {
			fi1=prec-4;
			if(fi1<0) fi1=0;
			if(fi1>13) fi1=13;

			sprintf(fs1,"%0.*f\""/*DNT*/,fi1,fr1);
		} else {
			fi3=(int)fr1;
			if((fr1-fi3)>=0.5) fi3++;
			if(fi3==60) {
				fi2++;
				if(fi2==60) { fi1++; fi2=0; }
				sprintf(result,"%dd%d'"/*DNT*/,fi1,fi2);
				fi3=0;
			}
			sprintf(fs1,"%d\""/*DNT*/,fi3);
		}
		strcat(result,fs1);
	}

	return(RTNORM);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Function SDS_CVUNIT
//
//	This function does somthing cool.
//
// *****************************************
// This is an SDS External API
//
int sdsengine_cvunit(double dUnits, const char *szOldUnit, const char *szNewUnit, double *pdNewNum) {


	ASSERT( OnEngineThread() );

	char					  szFname[IC_ACADBUF],
							 *szDataBuffer=NULL,
							 *szTempBuffer=NULL,
							 *szOldCnvUnt=NULL,
							 *sznewCnvUnt=NULL,
							 *fcp1;
	int 					  iFileHandle,
							  iFileLength,
							  iBufIndex,
							  iStrIndex,
							  iPrevIndex,
							  iCntrIndex,
							  iTempIndex;
	struct SDS_CvFundamental *pCVunitptr,
							 *pCVBaseptr1=NULL,
							 *pCVBaseptr2=NULL;
	BOOL					  bEqualsignFlag,
							  bDotflag;
	double					  fvalue1,
							  fvalue2;

	if (NULL==(sds_szCVString=(_TCHAR *)new char[IC_ACADBUF+1])) return(RTERROR);
	if (NULL==(szTempBuffer  =(_TCHAR *)new char[IC_ACADBUF+1])) return(RTERROR);

	if (!SDS_BeginFundamental) {
		// Make sure the unit file is in the proper location.
		if ((sds_findfile(SDS_UNITFILE,szFname))!=RTNORM) {
   			cmd_ErrorPrompt(CMD_ERR_CANTFINDFILE,0,SDS_UNITFILE);
			delete sds_szCVString; sds_szCVString=NULL;
			return(RTERROR);
		}
		// Open file TEXT and READ ONLY.
		if (-1==(iFileHandle=_open(szFname,_O_BINARY|_O_RDONLY))) {
   			cmd_ErrorPrompt(CMD_ERR_CANTOPENFILE,0,szFname);
			delete sds_szCVString; sds_szCVString=NULL;
			return(RTERROR);
		}
		// Get the file size.
		if (-1L==(iFileLength=_filelength(iFileHandle))) goto err;
		// Allocate a buffer large enough to read the whole file in.
		if (NULL==(szDataBuffer=(_TCHAR *)new char[((iFileLength+1)*sizeof(_TCHAR))])) goto err;
		// Set the memory to zero.
		memset(szDataBuffer,(_TCHAR)0,(iFileLength+1)*sizeof(_TCHAR));
		// Read the entire file into the buffer.
		if (-1==_read(iFileHandle,szDataBuffer,iFileLength)) goto err;
		// Close the file.
		_close(iFileHandle);

		// Read each character in the bufer.
		for(iBufIndex=0;iBufIndex<iFileLength;++iBufIndex) {
			switch(szDataBuffer[iBufIndex]) {
				case ';'/*DNT*/:
					// If a semicolon is read then read to the end of line. This would be a comment.
					for(++iBufIndex;iBufIndex<iFileLength;++iBufIndex) { if (szDataBuffer[iBufIndex]=='\n'/*DNT*/) break; }
					break;
				case '*'/*DNT*/:
					// Create a link in the list.
					if (!sds_CVunitMakeLink(&pCVunitptr)) goto err;
					// Read the characters after an astris.
					for(pCVunitptr->iStrCntr=1,iPrevIndex=iStrIndex=0,++iBufIndex;iBufIndex<iFileLength;++iBufIndex) {
						// If a semicolon is read, then read to the end of line.
						if (szDataBuffer[iBufIndex]==';'/*DNT*/) {
							for(++iBufIndex;iBufIndex<iFileLength;++iBufIndex) { if (szDataBuffer[iBufIndex]=='\n'/*DNT*/) break; }
						}
						if (szDataBuffer[iBufIndex]==' '/*DNT*/)  continue;
						if (szDataBuffer[iBufIndex]=='\n'/*DNT*/) break;
						if (szDataBuffer[iBufIndex]=='\r'/*DNT*/) continue;
						// If a comma is read then increment the number of strings counter, and set the iPrevIndex
						// to the begining of the last string read.
						if (szDataBuffer[iBufIndex]==','/*DNT*/) { ++pCVunitptr->iStrCntr; iPrevIndex=iStrIndex+1; }
						if (szDataBuffer[iBufIndex]=='('/*DNT*/) {
							// Set (bDotflag) to TRUE if the characters inside the '()' are seperated by a dot. If so then don't create
							// a single string with the characters before '('.
							for(bDotflag=TRUE,iTempIndex=iBufIndex;iTempIndex<iFileLength && szDataBuffer[iTempIndex]!=')'/*DNT*/;++iTempIndex) {
								if (szDataBuffer[iTempIndex]=='.'/*DNT*/) bDotflag=FALSE;
							}
							if (bDotflag) {
								// Append a comma after the previous character string.
								sds_szCVString[iStrIndex++]=','/*DNT*/;
								// Copy the previous string,and append it to the end.
								for(iCntrIndex=iPrevIndex;sds_szCVString[iCntrIndex]!=','/*DNT*/;++iCntrIndex) {
									sds_szCVString[iStrIndex++]=sds_szCVString[iCntrIndex];
								}
								++pCVunitptr->iStrCntr;
							} else {
								   sds_szCVString[iStrIndex]=0;
								   strcpy(szTempBuffer,sds_szCVString);
							}
							for(++iBufIndex;iBufIndex<iFileLength && szDataBuffer[iBufIndex]!=')'/*DNT*/;++iBufIndex) {
								sds_szCVString[iStrIndex++]=szDataBuffer[iBufIndex];
								if (szDataBuffer[iBufIndex+1]=='.'/*DNT*/) {
									sds_szCVString[iStrIndex++]=','/*DNT*/;
									if (bDotflag) {
										for(++iBufIndex,iCntrIndex=iPrevIndex;sds_szCVString[iCntrIndex]!=','/*DNT*/;++iCntrIndex) sds_szCVString[iStrIndex++]=sds_szCVString[iCntrIndex];
									} else {
										for(++iBufIndex,iCntrIndex=iPrevIndex;szTempBuffer[iCntrIndex];++iCntrIndex) sds_szCVString[iStrIndex++]=szTempBuffer[iCntrIndex];
									}
									++pCVunitptr->iStrCntr;
								}
							}
						} else {
							sds_szCVString[iStrIndex++]=szDataBuffer[iBufIndex];
						}
					}
					sds_szCVString[iStrIndex]=0;
					fcp1=strtok(sds_szCVString,","/*DNT*/);
					if (NULL==(pCVunitptr->szUnitNames=(_TCHAR **)new char*[pCVunitptr->iStrCntr+1])) goto err;
					for(iCntrIndex=0;fcp1;++iCntrIndex) {
						iStrIndex=strlen(fcp1);
						if (NULL==(pCVunitptr->szUnitNames[iCntrIndex]=(_TCHAR *)new char[(iStrIndex+1)])) goto err;
						strcpy(pCVunitptr->szUnitNames[iCntrIndex],fcp1);
						ic_trim(pCVunitptr->szUnitNames[iCntrIndex],"e"/*DNT*/);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
					}
					for(bEqualsignFlag=TRUE,iStrIndex=0,++iBufIndex;iBufIndex<iFileLength;++iBufIndex) {
						// If a semicolon is read then read to the end of line.
						if (szDataBuffer[iBufIndex]==';'/*DNT*/) {
							for(++iBufIndex;iBufIndex<iFileLength;++iBufIndex) { if (szDataBuffer[iBufIndex]=='\n'/*DNT*/) break; }
						}
						if (szDataBuffer[iBufIndex]=='\n'/*DNT*/) break;
						if (szDataBuffer[iBufIndex]=='\r'/*DNT*/) continue;
						if (bEqualsignFlag) {
	   						if (szDataBuffer[iBufIndex]=='='/*DNT*/)  bEqualsignFlag=FALSE;
						if (szDataBuffer[iBufIndex]==' '/*DNT*/)  continue;
							if (szDataBuffer[iBufIndex]=='\t'/*DNT*/) continue;
						}
						sds_szCVString[iStrIndex++]=szDataBuffer[iBufIndex];
					}
					if (*sds_szCVString=='='/*DNT*/ || *sds_szCVString=='^'/*DNT*/) {
						if (*sds_szCVString=='^'/*DNT*/) {
							if (NULL==(pCVunitptr->szDiriveString=(_TCHAR *)new char[iStrIndex+1])) goto err;
							strncpy(pCVunitptr->szDiriveString,(sds_szCVString),(iStrIndex));
						} else {
							if (NULL==(pCVunitptr->szDiriveString=(_TCHAR *)new char[iStrIndex])) goto err;
							strncpy(pCVunitptr->szDiriveString,(sds_szCVString+1),(iStrIndex-1));
						}
						ic_trim(pCVunitptr->szDiriveString,"e"/*DNT*/);
					} else {
						fcp1=strtok(sds_szCVString,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_VelocityOfLight=(double)atof(fcp1);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_ElectronCharge=(double)atof(fcp1);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_PlancksConstant=(double)atof(fcp1);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_BoltzmansConstant=(double)atof(fcp1);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_ElectronMass=(double)atof(fcp1);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_Multiplier=(double)atof(fcp1);
						fcp1=strtok(NULL,","/*DNT*/); if (!fcp1) break;
						pCVunitptr->dC_AdditiveOffset=(double)atof(fcp1);
					}
					break;
			}
		}
		close(iFileHandle);
	}

	// This code changes the (exponent synonyms) to the actual exponent. for example.
	// (CVUNIT 1 "SQ FEET" "SQ YARDS" would be changed to (cvunit 1 "FEET^2" "YARDS^2")
	szOldCnvUnt=sds_CVParseString(szOldUnit);
	sznewCnvUnt=sds_CVParseString(szNewUnit);

	// Call for the From Unit.
	if ((sds_CVCalculateValue(0,szOldCnvUnt,NULL,&fvalue1))==FALSE) goto err;
	// Copy over the fundamental units the the (SDS_FundamentalCompB) structure.
	sds_copyfund(&SDS_FundamentalCompA,&SDS_FundamentalCompB);
	// Call for the To Unit.
	if ((sds_CVCalculateValue(0,sznewCnvUnt,NULL,&fvalue2))==FALSE) goto err;
	if (SDS_FundamentalCompB.dC_VelocityOfLight  !=SDS_FundamentalCompA.dC_VelocityOfLight	 ||
		SDS_FundamentalCompB.dC_ElectronCharge	 !=SDS_FundamentalCompA.dC_ElectronCharge	 ||
		SDS_FundamentalCompB.dC_PlancksConstant  !=SDS_FundamentalCompA.dC_PlancksConstant	 ||
		SDS_FundamentalCompB.dC_BoltzmansConstant!=SDS_FundamentalCompA.dC_BoltzmansConstant ||
		SDS_FundamentalCompB.dC_ElectronMass	 !=SDS_FundamentalCompA.dC_ElectronMass) goto err;
	(*pdNewNum)=(((fvalue1/*+SDS_FundamentalCompA.dC_AdditiveOffset-SDS_FundamentalCompB.dC_AdditiveOffset*/)/fvalue2)*dUnits);

	if (szOldCnvUnt) delete szOldCnvUnt;
	if (sznewCnvUnt) delete sznewCnvUnt;

	if (szTempBuffer)	delete szTempBuffer;
	if (szDataBuffer)	delete szDataBuffer;
	if (sds_szCVString) { delete sds_szCVString; sds_szCVString=NULL; }

	return(RTNORM);

	err:
		if (szOldCnvUnt) delete szOldCnvUnt;
		if (sznewCnvUnt) delete sznewCnvUnt;

		if (szDataBuffer) delete szDataBuffer;
		if (szTempBuffer)	delete szTempBuffer;
		close(iFileHandle);
		if (sds_szCVString) { delete sds_szCVString; sds_szCVString=NULL; }
		return(RTERROR);
}


char *sds_CVParseString(const char *szValue) {
	char					  *szValString=NULL,
							  *szConvString=NULL,
							  *szExponent=NULL;
	short					  ConvIndex,
							  StrIndex,
							  fi1,
							  fi2;
	struct SDS_CvFundamental *pCVunitLink=NULL;

	if (!szValue) return(NULL);

	if (NULL==(szValString =(_TCHAR *)new char[IC_ACADBUF])) return(NULL);
	if (NULL==(szConvString=(_TCHAR *)new char[IC_ACADBUF])) goto err;
	if (NULL==(szExponent  =(_TCHAR *)new char[IC_ACADBUF])) goto err;

	for(*szExponent=*szConvString=0,ConvIndex=StrIndex=fi1=0;szValue[fi1];++fi1) {
		switch(szValue[fi1]) {
			case '^'/*DNT*/:
			case '/'/*DNT*/:
			case ' '/*DNT*/:
			case '*'/*DNT*/:
				szValString[StrIndex]=0;
				if (!isalpha((unsigned char) *szValString)) {
					for(fi2=0;szValString[fi2];ConvIndex++,++fi2) {
						szConvString[ConvIndex]=szValString[fi2];
					}
				} else {
					sds_CVSearchLink(&pCVunitLink,szValString);
					if (pCVunitLink) {
						if (pCVunitLink->szDiriveString && *pCVunitLink->szDiriveString=='^'/*DNT*/) {
							strcpy(szExponent,pCVunitLink->szDiriveString);
							StrIndex=0;
							continue;
						}
						for(fi2=0;szValString[fi2];ConvIndex++,++fi2) {
							szConvString[ConvIndex]=szValString[fi2];
						}
						if (*szExponent) {
							for(fi2=0;szExponent[fi2];ConvIndex++,++fi2) {
								szConvString[ConvIndex]=szExponent[fi2];
							}
							*szExponent=0;
						}
					}
				}
				szConvString[ConvIndex++]=szValue[fi1];
				if (szValue[fi1]=='^'/*DNT*/) szConvString[ConvIndex++]=szValue[++fi1];
				szConvString[ConvIndex]=0;
				StrIndex=0;
				break;
			default:
				szValString[StrIndex++]=szValue[fi1];
				break;
		}
	}
	szValString[StrIndex]=0;
	for(fi2=0;szValString[fi2];ConvIndex++,++fi2) {
		szConvString[ConvIndex]=szValString[fi2];
	}
	if (*szExponent) {
		for(fi2=0;szExponent[fi2];ConvIndex++,++fi2) {
			szConvString[ConvIndex]=szExponent[fi2];
		}
	}
	szConvString[ConvIndex]=0;

	if (szValString)  delete szValString;
	if (szExponent)   delete szExponent;
	return(szConvString);

	err:
		if (szValString)  delete szValString;
		if (szConvString) delete szConvString;
		if (szExponent)   delete szExponent;
		return(NULL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
// (actiontype : 1 = Multiply
// & taction)  : 2 = Divide
//			   : 3 = exponential
//			   : 0 = set equal.
//
BOOL sds_CVCalculateValue(short taction, const char *szFrmUnit,const char *pCToUnit,double *fNewValue) {
	struct SDS_CvFundamental *pCVunitLink=NULL,
							 *pCVunitComp=NULL,
							  CVunitTemp;
	short					  fi1,
							  fi2,
							  ValStrIndex,
							  actiontype=0,
							  tempactiontype,
							  powerval=0,
							  istringflag=0;
	char					 *szValStringA=NULL,
							 *szValStringB=NULL,
							 *expstring   =NULL;
	sds_real				  CalcValueA=0.0,
							  CalcValueB=0.0;

	if (!szFrmUnit && !(*szFrmUnit)) return(NULL);
	// Allocate enough space for any variable or value to fit.
	if (NULL==(szValStringA=(_TCHAR *)new char[strlen(szFrmUnit)+IC_ACADBUF])) return(NULL);
	if (NULL==(szValStringB=(_TCHAR *)new char[strlen(szFrmUnit)+IC_ACADBUF])) goto err;
	if (NULL==(expstring   =(_TCHAR *)new char[strlen(szFrmUnit)+IC_ACADBUF])) goto err;

	// Parse through the szFrmUnit string.
	for(ValStrIndex=fi1=0;szFrmUnit[fi1];++fi1) {
		switch(szFrmUnit[fi1]) {
			case '^'/*DNT*/:
				switch(istringflag) {
					case 0:
						if (szFrmUnit[++fi1]) {
							taction=3;
							for(fi2=0;szFrmUnit[fi1] && szFrmUnit[fi1]!=' '/*DNT*/;++fi1,++fi2) expstring[fi2]=szFrmUnit[fi1];
							expstring[fi2]=0;
							powerval=atoi(expstring);
						}
						break;
					case 1:
						szValStringB[ValStrIndex++]=szFrmUnit[fi1];
						break;
				}
				break;
			case '/'/*DNT*/:
			case ' '/*DNT*/:
			case '*'/*DNT*/:
				if ((tempactiontype=sds_getactiontype((char *)szFrmUnit,&fi1))==-1) goto err;
				// (istringflag) is (0) when (szValStringA) is filled (and/or) the first (action='/, ,*') has been read.
				// (istringflag) is (1) when (szValStringA) and (szValStringB) is filled and (action='/, ,*') has been read.
				// (istringflag) is (2) when (szValStringA) and (szValStringB) is filled and (action='/, ,*') has been read a second time.
				switch(istringflag) {
					case 0:
						// Terminate the valuestring. If no string was read then this will set element[0] to NULL.
						szValStringA[ValStrIndex]=0;
						// If (szValStringA) was not filled then the first character must have been a space.
						if (szValStringA && *szValStringA) istringflag=1; else continue;
						if (istringflag==1) actiontype=tempactiontype;
						// Reset (ValStrIndex) for (szValStringB).
						ValStrIndex=0;
						// If the (actiontype) is a division everthing after the division sign needs to be evaluated as one term. So
						// it is passed as one eqation in the (szValStringB) variable.
						if (actiontype==2) {

							// New addition to the code. ---------------------------------------
							if (taction==3) {
								strcat(szValStringA,"^"/*DNT*/); strcat(szValStringA,expstring);
								taction=0;
							}

							for(++fi1;szFrmUnit[fi1];++fi1,++ValStrIndex) szValStringB[ValStrIndex]=szFrmUnit[fi1];
							szValStringB[ValStrIndex]=0;
						}
						break;
					case 1:
						// If at this point, there must be more than two terms, evaluate the first two terms.
						szValStringB[ValStrIndex]=0;
						if ((sds_CVCalculateValue(actiontype,szValStringA,szValStringB,&CalcValueA))==FALSE) goto err;
						istringflag=2;
						ValStrIndex=0;
						actiontype=tempactiontype;

						// If the (actiontype) is a division everthing after the division sign needs to be evaluated as one term. So
						// it is passed as one eqation in the (szValStringB) variable.
						if (actiontype==2) {
							for(++fi1;szFrmUnit[fi1];++fi1,++ValStrIndex) szValStringA[ValStrIndex]=szFrmUnit[fi1];
							szValStringA[ValStrIndex]=0;
						}
						break;
					case 2:
						// If at this point, there must have been more than two terms, unlike case (1), this code
						// evaluates all the following terms.
						szValStringB[ValStrIndex]=0;
						if ((sds_CVCalculateValue(FALSE,szValStringB,NULL,&CalcValueB))==FALSE) goto err;
						CalcValueA=sds_eval(CalcValueA,CalcValueB,actiontype,powerval);
						ValStrIndex=0;
						actiontype=tempactiontype;
						// If the (actiontype) is a division everthing after the division sign needs to be evaluated as one term. So
						// it is passed as one eqation in the (szValStringB) variable.
						if (actiontype==2) {
							for(;szFrmUnit[fi1];++fi1,++ValStrIndex) szValStringA[ValStrIndex]=szFrmUnit[fi1];
							szValStringA[ValStrIndex]=0;
						}
						break;
				}
				break;
			default:
				switch(istringflag) {
					case 0: szValStringA[ValStrIndex++]=szFrmUnit[fi1]; break;
					case 1:
					case 2: szValStringB[ValStrIndex++]=szFrmUnit[fi1]; break;
				}
				break;
		}
		if (!szFrmUnit[fi1]) break;
	}

	// If (istringflag) is 0 then only (szValStringA) has been filled. 1 or 2 then (szValStringA and szValStringB) has been filled.
	 switch(istringflag) {
		case 0:
			szValStringA[ValStrIndex++]=0;
			// If the string does not begin with an alpha character then it will be translated as a numeric.
			if (!isalpha((unsigned char) *szValStringA)) {
				CalcValueA=atof(szValStringA);
				sds_copyfund(NULL,&SDS_FundamentalCompA);
				SDS_FundamentalCompA.dC_Multiplier=CalcValueA;
			} else {
				// Get the link in the list, where the szValStringA is defined.
				sds_CVSearchLink(&pCVunitLink,szValStringA);
				if (!pCVunitLink) goto err;
				// If the is not a (pCVunitLink->szDiriveString) then this must be a base unit then calculate value.
				if (pCVunitLink->szDiriveString) {
					if ((sds_CVCalculateValue(FALSE,pCVunitLink->szDiriveString,NULL,&CalcValueA))==FALSE) goto err;
				} else {
					CalcValueA=pCVunitLink->dC_Multiplier;
					pCVunitComp=pCVunitLink;

					sds_copyfund(pCVunitComp,&SDS_FundamentalCompA);
				}
			}
			break;
		case 1:
			szValStringB[ValStrIndex]=0;
			// If ((istringflag) is true then both (szValStringA) and (szValStringB) have been filled.
			if ((sds_CVCalculateValue(actiontype,szValStringA,szValStringB,&CalcValueA))==FALSE) goto err;
			break;
		case 2:
			if (actiontype!=2) {
				szValStringB[ValStrIndex]=0;
				// If ((istringflag) is true then both (szValStringA) and (szValStringB) have been filled.
				if ((sds_CVCalculateValue(FALSE,szValStringB,NULL,&CalcValueB))==FALSE) goto err;
				CalcValueA=sds_eval(CalcValueA,CalcValueB,actiontype,powerval);
			} else {
				if ((sds_CVCalculateValue(FALSE,szValStringA,NULL,&CalcValueB))==FALSE) goto err;
				CalcValueA=sds_eval(CalcValueA,CalcValueB,actiontype,powerval);
			}
	}

	// Evaluate the (pCToUnit) term.
	if (pCToUnit && *pCToUnit) {
		sds_copyfund(&SDS_FundamentalCompA,&CVunitTemp);
		if ((sds_CVCalculateValue(FALSE,pCToUnit,NULL,&CalcValueB))==FALSE) goto err;
		switch(taction) {
			case 1:
				SDS_FundamentalCompA.dC_VelocityOfLight  +=CVunitTemp.dC_VelocityOfLight;
				SDS_FundamentalCompA.dC_ElectronCharge	 +=CVunitTemp.dC_ElectronCharge;
				SDS_FundamentalCompA.dC_PlancksConstant  +=CVunitTemp.dC_PlancksConstant;
				SDS_FundamentalCompA.dC_BoltzmansConstant+=CVunitTemp.dC_BoltzmansConstant;
				SDS_FundamentalCompA.dC_ElectronMass	 +=CVunitTemp.dC_ElectronMass;
				break;
			case 2:
				SDS_FundamentalCompA.dC_VelocityOfLight  -=CVunitTemp.dC_VelocityOfLight;
				SDS_FundamentalCompA.dC_ElectronCharge	 -=CVunitTemp.dC_ElectronCharge;
				SDS_FundamentalCompA.dC_PlancksConstant  -=CVunitTemp.dC_PlancksConstant;
				SDS_FundamentalCompA.dC_BoltzmansConstant-=CVunitTemp.dC_BoltzmansConstant;
				SDS_FundamentalCompA.dC_ElectronMass	 -=CVunitTemp.dC_ElectronMass;
				break;
		}
		(*fNewValue)=sds_eval(CalcValueA,CalcValueB,taction,powerval);
	} else {
		switch(taction) {
			case 3:
				(*fNewValue)=pow(CalcValueA,powerval);
				SDS_FundamentalCompA.dC_VelocityOfLight  *=powerval;
				SDS_FundamentalCompA.dC_ElectronCharge	 *=powerval;
				SDS_FundamentalCompA.dC_PlancksConstant  *=powerval;
				SDS_FundamentalCompA.dC_BoltzmansConstant*=powerval;
				SDS_FundamentalCompA.dC_ElectronMass	 *=powerval;
				break;
			default:
				(*fNewValue)=CalcValueA;
				break;
		}
	}

	if (expstring)	  delete expstring;
	if (szValStringA) delete szValStringA;
	if (szValStringB) delete szValStringB;
	return(TRUE);
	err:
		if (expstring)	  delete expstring;
		if (szValStringA) delete szValStringA;
		if (szValStringB) delete szValStringB;
		return(FALSE);
}

sds_real sds_eval(sds_real r1,sds_real r2,short action,short power) {
	sds_real retval;

	switch(action) {
		case 1:
			retval=(r1*r2);
			break;
		case 2:
			retval=(r1/r2);
			break;
		case 3:
			retval=pow(r1,power);
			retval=(retval/r2);
			break;
		default:
			retval=r1;
			break;
	}
	return(retval);
}

short sds_getactiontype(char *string,short *index) {
	short fi1,
		  tempactiontype,
		  leveltype;
	char  tempbuffer[IC_ACADBUF];

	// When any of the normal action type characters (/, ,*) are found store the string in a temporary variable for parsing.
	for(fi1=0;string[(*index)];++(*index),++fi1) {
		// If any other character is found break out of the loop.
		if (string[(*index)]!='*'/*DNT*/ && string[(*index)]!=' '/*DNT*/ && string[(*index)]!='/'/*DNT*/) { --(*index); break; }
		// Store character in temporary variable.
		tempbuffer[fi1]=string[(*index)];
	}
	tempbuffer[fi1]=0;
	// tempbuffer will have some string in it at this point some examples are (' / ','	 ',' *','/') the types of strings that will
	// cause an error are, for example (' / * ',' //') and so on, ones that make no sense, that is what most if this code is checking for.
	if (*tempbuffer) {
		for(leveltype=fi1=0;tempbuffer[fi1];++fi1) {
			switch(tempbuffer[fi1]) {
				case '/'/*DNT*/:
					if (leveltype==2) { tempactiontype=-1; break; }
					leveltype=2;
					tempactiontype=2;
					break;
				case ' '/*DNT*/:
					if (!leveltype) {
						leveltype=1;
						tempactiontype=1;
					}
					break;
				case '*'/*DNT*/:
					if (leveltype==2) { tempactiontype=-1; break; }
					leveltype=2;
					tempactiontype=1;
					break;
			}
			if (tempactiontype==-1) break;
		}
	}

	return(tempactiontype);
}

void sds_copyfund(struct SDS_CvFundamental *FrmCvptr,struct SDS_CvFundamental *ToCvptr) {
	if (!FrmCvptr && !ToCvptr) return;

	if (!FrmCvptr) {
		ToCvptr->dC_VelocityOfLight  =0;
		ToCvptr->dC_ElectronCharge	 =0;
		ToCvptr->dC_PlancksConstant  =0;
		ToCvptr->dC_BoltzmansConstant=0;
		ToCvptr->dC_ElectronMass	 =0;
		ToCvptr->dC_Multiplier		 =0;
		ToCvptr->dC_AdditiveOffset	 =0;
	} else if (FrmCvptr) {
		ToCvptr->dC_VelocityOfLight  =FrmCvptr->dC_VelocityOfLight;
		ToCvptr->dC_ElectronCharge	 =FrmCvptr->dC_ElectronCharge;
		ToCvptr->dC_PlancksConstant  =FrmCvptr->dC_PlancksConstant;
		ToCvptr->dC_BoltzmansConstant=FrmCvptr->dC_BoltzmansConstant;
		ToCvptr->dC_ElectronMass	 =FrmCvptr->dC_ElectronMass;
		ToCvptr->dC_Multiplier		 =FrmCvptr->dC_Multiplier;
		ToCvptr->dC_AdditiveOffset	 =FrmCvptr->dC_AdditiveOffset;
	}
}

/* THIS IS THE OLD CODE.
BOOL sds_CVCalculateValue(double CVNum, const char *szOldUnit,struct SDS_CvFundamental *pCNewUnit,double *fNewValue) {
	struct SDS_CvFundamental *pCVunitLink=NULL,
							 *PCVunitTemp=NULL;
	int 					  fi1,
							  fi2,
							  fi3,
							  iActiontype,
							  bSwitchstate;
	char					 *szValString,
							  cLastAction;
	BOOL					  bTypeFlag;
	double					  fUnitValue1,
							  fUnitValue2,
							  fTempValue;

	// Get the link in the list, where the szUnitNames matches the szOldUnit variable.
	sds_CVSearchLink(&pCVunitLink,(char *)szOldUnit);
	if (!pCVunitLink) return(FALSE);

	if (pCVunitLink->szDiriveString) {
		// Allocate enough space for any variable or value to fit.
		if (NULL==(szValString=(_TCHAR *)new char[strlen(pCVunitLink->szDiriveString)+1])) return(FALSE);
		// Parse through the (pCVunitLink->szDiriveString) string, This string was found for the definition for (szOldUnit).
		for(cLastAction=bSwitchstate=bTypeFlag=fi2=fi1=0;pCVunitLink->szDiriveString[fi2];++fi2) {
			switch(pCVunitLink->szDiriveString[fi2]) {
				case '^':
					break;
				case  '/': // Division
				case  '*': // Multiplication
				case '\t': // Tab
				case  ' ': // Space.
					// Continue if the first character in the string is a space.
					if (pCVunitLink->szDiriveString[fi2]==' ' && !fi1) continue;
					// Put a NULL at the end of the string and reset the fi1 varaible.
					szValString[fi1++]=0; fi1=0;
					// Find the unit in the array of unit names related to current node.
					for(fi3=0;fi3<pCNewUnit->iStrCntr;++fi3) {
						if (!strcmp(szValString,pCNewUnit->szUnitNames[fi3])) break;
					}
					// If the unit was found set all the flags.
					if (fi3<pCNewUnit->iStrCntr) {
						// Set the type of the flag to be none.
						bTypeFlag=0;
						fUnitValue1=1;
						bSwitchstate=1;
					} else {
						// If the string is a character string we need to look at what defines that string.
						if (bTypeFlag==SDS_ALPHA_CHAR) {
							if (bSwitchstate==1) {
								if (!sds_CVCalculateValue(CVNum,szValString,pCNewUnit,&fUnitValue2)) return(FALSE);
								bSwitchstate=2;
							} else {
								if (!sds_CVCalculateValue(CVNum,szValString,pCNewUnit,&fUnitValue1)) return(FALSE);
								bSwitchstate=1;
							}
						// If the string is a value then convert to float.
						} else {
							if (bSwitchstate==1) {
								fUnitValue2=atof(szValString);
								bSwitchstate=2;
							} else {
								fUnitValue1=atof(szValString);
								bSwitchstate=1;
							}
						}
					}
					// Reset the type of value flag.
					bTypeFlag=0;
					// Set (iActiontype) to the math that needs to take place.
					switch(cLastAction) {
						case  '*': // Multiplication
						case '\t': // Tab
						case  ' ': // Space
							iActiontype=0;
							break;
						case  '/': // Divide
							iActiontype=1;
							break;
						default:
							break;
					}
					if (bSwitchstate==2) {
						switch(iActiontype) {
							case 0:
								fUnitValue1*=fUnitValue2;
								break;
							case 1:
								fUnitValue1/=fUnitValue2;
								break;
						}
						bSwitchstate=1;
					}
					// Set the type of math needed to be done.
					cLastAction=pCVunitLink->szDiriveString[fi2];
					break;
				default:
					if (!bTypeFlag) {
						if (isalpha((unsigned char) pCVunitLink->szDiriveString[fi2])) bTypeFlag=SDS_ALPHA_CHAR;
						else if (isdigit((unsigned char) pCVunitLink->szDiriveString[fi2])) bTypeFlag=SDS_DIGIT_CHAR;
						else return(FALSE);
					}
					if (isalpha((unsigned char) pCVunitLink->szDiriveString[fi2]) && bTypeFlag!=SDS_ALPHA_CHAR &&
					pCVunitLink->szDiriveString[fi2]!='e' && pCVunitLink->szDiriveString[fi2]!='E') continue;
					else if (isdigit((unsigned char) pCVunitLink->szDiriveString[fi2]) && bTypeFlag!=SDS_DIGIT_CHAR) continue;
					szValString[fi1++]=pCVunitLink->szDiriveString[fi2];
					break;
			}
			if (pCVunitLink->szDiriveString[fi2]=='^') {
				szValString[fi1]=0;
				sds_CVSearchLink(&PCVunitTemp,(char *)szValString);
				if (!PCVunitTemp->szDiriveString) {
					if (pCVunitLink->szDiriveString[fi2+1]) { ++fi2; continue; }
				} else break;
			}
		}
		// Put a NULL at the end of the string and reset the fi1 varaible.
		szValString[fi1++]=0; fi1=0;
		if (pCNewUnit->iStrCntr) {
			// Find the unit in the array of unit names related to current node.
			for(fi3=0;fi3<pCNewUnit->iStrCntr;++fi3) {
				if (!strcmp(szValString,pCNewUnit->szUnitNames[fi3])) break;
			}
			// If the unit was found set all the flags.
			if (fi3<pCNewUnit->iStrCntr) {
				(*fNewValue)=fUnitValue1;
				return(TRUE);
			}
			// If the string is a character string we need to look at what defines that string.
			if (bTypeFlag==SDS_ALPHA_CHAR) {
				if (bSwitchstate==1) {
					if (!sds_CVCalculateValue(CVNum,szValString,pCNewUnit,&fUnitValue2)) return(FALSE);
					bSwitchstate=2;
				} else {
					if (!sds_CVCalculateValue(CVNum,szValString,pCNewUnit,&fUnitValue1)) return(FALSE);
					bSwitchstate=1;
				}
			// If the string is a value then convert to float.
			} else {
				if (bSwitchstate==1) {
					fUnitValue2=atof(szValString);
					bSwitchstate=2;
				} else {
					fUnitValue1=atof(szValString);
					bSwitchstate=1;
				}
			}
			// If (pCVunitLink->szDiriveString) is (^2) or (^3).
			if (pCVunitLink->szDiriveString[fi2]=='^' && pCVunitLink->szDiriveString[fi2+1]) {
				++fi2;
				fTempValue=atof(pCVunitLink->szDiriveString+fi2);
				fUnitValue2=pow(fUnitValue2,fTempValue);
			}
			// Set (iActiontype) to the math that needs to take place.
			switch(cLastAction) {
				case  '*': // Multiplication
				case '\t': // Tab
				case  ' ': // Space
					iActiontype=0;
					break;
				case  '/': // Divide
					iActiontype=1;
					break;
				default:
					break;
			}
			if (bSwitchstate==2) {
				switch(iActiontype) {
					case 0:
						fUnitValue1*=fUnitValue2;
						break;
					case 1:
						fUnitValue1/=fUnitValue2;
						break;
				}
				bSwitchstate=1;
			}
		}
	} else if (pCNewUnit->szUnitNames) {
		if (!sds_CVCalculateValue(TRUE,*(pCNewUnit->szUnitNames),pCVunitLink,&fUnitValue2)) return(FALSE);
		(*fNewValue)=1/fUnitValue2;
		return(TRUE);
	} else return(FALSE);

	// return the value calcualted.
	(*fNewValue)=fUnitValue1;
	return(TRUE);
}
*/

int sds_CVSearchLink(struct SDS_CvFundamental **pCVunitLink,char *szUnitName) {
	struct SDS_CvFundamental *pCVunitptr;
	int 					  iStrIndex=-1,
							  iCntrIndex;

	for(pCVunitptr=SDS_BeginFundamental;pCVunitptr!=NULL;pCVunitptr=pCVunitptr->next) {
		for(iCntrIndex=0;iCntrIndex<pCVunitptr->iStrCntr;++iCntrIndex) { if (strisame(szUnitName,pCVunitptr->szUnitNames[iCntrIndex])) break; }
		if (iCntrIndex<pCVunitptr->iStrCntr) break;
	}

	iStrIndex=iCntrIndex;
	(*pCVunitLink)=pCVunitptr;

	return(iStrIndex);
}

BOOL sds_CVunitMakeLink(struct SDS_CvFundamental **ppLinkPtr) {
	struct SDS_CvFundamental *pTemp;

  	for(pTemp=SDS_BeginFundamental;pTemp!=NULL&&pTemp->next!=NULL;pTemp=pTemp->next);
	if (NULL==((*ppLinkPtr)=(struct SDS_CvFundamental *)new struct SDS_CvFundamental)) return(FALSE);
	// Set the memory to zero.
	memset((*ppLinkPtr),0,sizeof(struct SDS_CvFundamental));
	if(!pTemp) {
		SDS_BeginFundamental=(*ppLinkPtr);
	} else {
		pTemp->next=(*ppLinkPtr);
	}

	return(TRUE);
}

#ifdef ORIGINALVERSION
// *****************************************
// This is an SDS External API
//
int sdsengine_distof(const char *szDistance, int nUnitType, double *pdDistance) {

	char *fs1,*fcp1,*fcp2,*fcp3;
	struct resbuf rb;
	int wholenum,wholefeet,isfrac,sign,loopct;
	sds_real val,feetval;

	fs1=fcp1=fcp2=fcp3=NULL; wholenum=wholefeet=isfrac=0; val=feetval=0.0;
	if(szDistance==NULL) return(RTERROR);
	fs1= new char [strlen(szDistance)+1];
	strcpy(fs1,szDistance);

	if(nUnitType<0) {
		if(SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) nUnitType=rb.resval.rint;
		else nUnitType=0;
	} else if(nUnitType>5) nUnitType=5; //*** uh huh-huh huh-huh huh-huh huh-huh huh-huh huh-huh huh-huh

	sign=1;
	if(*fs1=='-'/*DNT*/) { sign=(-1); fcp2=fs1+1; }
	else if(*fs1=='+'/*DNT*/) fcp2=fs1+1;
	else if(!isdigit((unsigned char) *fs1) && *fs1!='.'/*DNT*/) goto bailbad;
	else fcp2=fs1;
	for(fcp1=fcp2; isdigit((unsigned char) *fcp1) || *fcp1=='+'/*DNT*/ || *fcp1=='.'/*DNT*/; fcp1++);
	//if the first non-digit is a ' or " (feet or inch marker) switch unittype to 4 (Architectural).
	//We want to be able to accept Arch. input even if we are not in Arch. mode.
	if((*fcp1=='\''/*DNT*/) || (*fcp1=='"'/*DNT*/)) nUnitType=4;

	for(loopct=0; loopct<2; loopct++) { //*** This loop is used when inches follow feet only.
		if(*fcp1=='\''/*DNT*/ && (nUnitType==3 || nUnitType==4)) {
			//*** Only get the amount of "feet" here if eng. or arch. units
			//*** AND it is a whole number not a decimal. (1'-OK; 1.23'-keep going).
			if(icadRealEqual((atof(fcp2)-atoi(fcp2)),0.0)) {
				wholefeet=atoi(fcp2);
				fcp1++;
				//if(isdigit((unsigned char) *fcp1) || *fcp1==0) fcp2=fcp1;
				if(isdigit((unsigned char) *fcp1) || *fcp1==0 || *fcp1=='.'/*DNT*/) fcp2=fcp1;	//inches may lead w/decimal
				else if(*fcp1==' '/*DNT*/ || *fcp1=='-'/*DNT*/) fcp2=(++fcp1);
				else goto bailbad;
				//for(fcp1++;*fcp1=='-' || *fcp1=='.' || isdigit((unsigned char) *fcp1); fcp1++);
				for(;*fcp1=='-'/*DNT*/ || *fcp1=='.'/*DNT*/ || isdigit((unsigned char) *fcp1); fcp1++);//don't pre-increment fcp1
			}
		}
		if(*fcp1==' '/*DNT*/ || *fcp1=='-'/*DNT*/) {
			//*** If there is a space(' ') or a dash('-') then it is a whole number
			//*** for a fraction or an error.
			wholenum=atoi(fcp2);
			fcp2=fcp1+1;
			for(fcp1++; isdigit((unsigned char) *fcp1); fcp1++);
		}
		//*** Fractions seem to be valid for all units (huh-huh).
		if(*fcp1=='/'/*DNT*/) {
			//*** Make sure there are only digits BEFORE the / sign.
			*fcp1=0;
			for(fcp1=fcp2; *fcp1==' '/*DNT*/ || *fcp1=='+'/*DNT*/ || *fcp1=='-'/*DNT*/ ||
						  isdigit((unsigned char) *fcp1); fcp1++);
			if(*fcp1!=0) goto bailbad;
			//*** fcp1+1 points to the denominator, fcp2 points to the
			//*** numerator, and fcp3 points to the end of the string.
			val=(atof(fcp2)/atof(fcp1+1));
			val+=(sds_real)wholenum;
//			  val*=sign;
			isfrac=1;
			//*** Set fcp1 to the next non-digit after the / sign.
			for(fcp1++; isdigit((unsigned char) *fcp1); fcp1++);
		} else if(*fcp1==0) {
			val=atof(fcp2);
		}
		if(loopct && *fcp1!='"'/*DNT*/) goto bailbad;
		//*** At this point fcp1 is pointing to the first non-digit or the character following the fraction.
		switch(nUnitType) {
			case 1:
			case 2:
			case 5:
				switch(*fcp1) {
					case 0: break;
					case 'e'/*DNT*/: case 'E'/*DNT*/:
						for(fcp1++; isdigit((unsigned char) *fcp1) || *fcp1=='+'/*DNT*/ || *fcp1=='-'/*DNT*/; fcp1++);
						if(*fcp1!=0) goto bailbad;
						val=atof(fs1);
						break;
					default: goto bailbad;
				}
				break;
			case 3:
			case 4:
				switch(*fcp1) {
					case 0:
						val+=(wholefeet*12.0);
						break;
					case '\''/*DNT*/:
						if(!isfrac) val=atof(fs1);
						else val+=(sds_real)wholefeet;
						val*=12.0; //*** feet to decimal.
						//*** Find out what is following the feet.
						for(fcp2=(++fcp1); *fcp1==' '/*DNT*/ || *fcp1=='-'/*DNT*/ || *fcp1=='.'/*DNT*/ ||
										   isdigit((unsigned char) *fcp1); fcp1++);
						if(isfrac && *fcp1!=0) goto bailbad;
						if(*fcp1!=0) {
							if(!icadRealEqual((val-(int)val),0.0)) goto bailbad;
							feetval=val; continue;
						}
						break;
					case '"'/*DNT*/:
						if(!isfrac) val=atof(fcp2);
						val+=feetval;
						val+=(wholefeet*12.0);
						break;
					default: goto bailbad;
				}
				break;
			default: goto bailbad;
		}
		break;
	}

	*pdDistance=(val*sign);
	IC_FREE(fs1);
	return(RTNORM);

	bailbad:

	IC_FREE(fs1);
	return(RTERROR);
}
#endif


void gathervalue(char **cptr,double *val,int *digitsread)
{
	*val=0.0;
	*digitsread=0;
	while (isdigit((unsigned char) **cptr)) {
		*val=(*val)*10.0+(double)((**cptr)-'0'/*DNT*/);
		(*cptr)++;
		(*digitsread)++;
	}
}


int parse_as_architectural(char *cptr,double *retval)
{
// simulate an fsa
// nodes that are fallen-through to have unused labels, which the compiler doesn't like,
// so I have commented the labels out.

	sds_real inchval=0.0,numerator=0.0, denominator=0.0, fracpart=0.0, feetval=0.0;
	int digitsread;

	*retval=0.0;

	if ('.' != *cptr && !isdigit((unsigned char) *cptr))
		return(RTERROR);					// must start with a 0..9 or '.' to go to node 1

	gathervalue(&cptr,&inchval,&digitsread);	// NODE 1 --read the initial set of digits
#ifdef LOCALISE_ISSUE
// The feet & inches symbols are not used on a global basis
// ACAD may force this.
// May be worth stripping out the symbols, and building
//	them into the sprintf's as extra parameters?
#endif
	if ((*cptr)=='\'') goto handlefeet;			// feet indicator
	if ((*cptr)=='-'/*DNT*/ || (*cptr)==' '/*DNT*/) goto handlefraction;		// fraction indicator
	if ((*cptr)==0 || (*cptr)=='"'/*DNT*/) goto successnode;
	if ((*cptr)=='.'/*DNT*/) goto decimalfeetorin;		// feet or inches has decimal value
	if ((*cptr)=='/'/*DNT*/) {							// we were actually reading a numerator, so move it to the numerator, clear
		numerator=inchval;						// inchval and read the denominator
		inchval=0.0;
		goto checkvaliddigitafterslash;
	}
	else return RTERROR;

handlefeet:										// NODE 2 -- handle feet indicator
	cptr++;										// skip the quote
	feetval=inchval;							// whoops! what we thought were inches were actually feet
	inchval=0.0;								// so set feetval and re-init inchval
	if ((*cptr)==0) goto successnode;			// just feet is OK
	if ((*cptr)=='-'/*DNT*/ || (*cptr)==' '/*DNT*/) {			// handle the weird case of X'-Y-A/B'' (where either or both hyphens can be spaces)
		cptr++;									// skip this hyphen/space and drop through
	}
	if (!isdigit((unsigned char) *cptr)) return RTERROR;		// otherwise need a digit here

//readinches:
	gathervalue(&cptr,&inchval,&digitsread);	// NODE 3 -- had feet, now read inches
	if ((*cptr)==0 || (*cptr)=='"') goto successnode; // done
	if ((*cptr)=='.'/*DNT*/) goto decimalin;			// inches given in decimal
	if ((*cptr)=='-'/*DNT*/ || (*cptr)==' '/*DNT*/) goto handlefraction;		// now process the fraction
	if ((*cptr)=='/'/*DNT*/) {							// we were actually reading a numerator, so move it to the numerator, clear
		numerator=inchval;						// inchval and read the denominator
		inchval=0.0;
		goto checkvaliddigitafterslash;
	}
	return RTERROR;								// else failure

handlefraction:									// NODE 4 -- just received a minus indicating a fraction follows
	cptr++;										// skip the minus
	if (!isdigit((unsigned char) *cptr)) return RTERROR;
												// fall through on digit to node 5

//gathernumerator:								// NODE 5 -- gather numerator
	gathervalue(&cptr,&numerator,&digitsread);
	if ((*cptr)!='/'/*DNT*/) return RTERROR;			// otherwise fall through to node 6

checkvaliddigitafterslash:						// NODE 6 -- just rec'd / gather denominator
	cptr++;
	if (!isdigit((unsigned char) *cptr)) return RTERROR;		// otherwise fall through to node 7

//gatherdenominator:							// NODE 7 -- gather denominator
	gathervalue(&cptr,&denominator,&digitsread);
	if (icadRealEqual(denominator,0.0)) return RTERROR;		// fail on actual typed-in 0 denominator
//	if (numerator>=denominator) return RTERROR; // must be reduced; AutoCAD does not accept num>=denom
												// WRONG!  turns out it doesn't accept it from the kbd, but it will
												// do, for instance, (distof "6/5" 5)
	if ((*cptr)==0 || (*cptr)=='"'/*DNT*/) goto successnode;
	return RTERROR;

decimalfeetorin:
	cptr++;										// skip the decimal point
	if ((*cptr)==0) goto decimalfeetorindone;	// we're finished
	if (!isdigit((unsigned char) *cptr)) return RTERROR;		// otherwise must be a digit
	gathervalue(&cptr,&fracpart,&digitsread);	// get the fractional part
	while (digitsread--) fracpart/=10.0;

decimalfeetorindone:
	if ((*cptr)==0 || (*cptr)=='"'/*DNT*/) {			// see whether we had inches or feet
		inchval+=fracpart;
		goto successnode;
	}
	else if ((*cptr)=='\''/*DNT*/) {
		cptr++;
		if ((*cptr)!=0) return RTERROR;			// no other characters valid in this case
		feetval=inchval+fracpart;
		inchval=0.0;
		goto successnode;
	}
	else return RTERROR;

decimalin:										// had feet, now handle inches given in decimal
	cptr++;
	if ((*cptr)==0 || (*cptr)=='"'/*DNT*/) goto decimalindone;
	if (!isdigit((unsigned char) *cptr)) return RTERROR;
	gathervalue(&cptr,&fracpart,&digitsread);
	while (digitsread--) fracpart/=10.0;

decimalindone:
	if ((*cptr)=='"'/*DNT*/ && (cptr[1])!=0) return RTERROR;
	if ((*cptr)==0 || (*cptr)=='"'/*DNT*/) {
		*retval=feetval*12+inchval+fracpart;
		return RTNORM;
	}
	else return RTERROR;

successnode:
	if ((*cptr)=='"'/*DNT*/) {
		cptr++;
		if ((*cptr)!=0) return RTERROR;			// characters beyond the " is an error
	}
	if (icadRealEqual(denominator,0.0)) denominator=1.0;		// already checked for actual 0 denom above; this is if we didn't read one
	*retval=feetval*12+inchval+numerator/denominator;
	return(RTNORM);
}

int parse_as_decimal(char *cptr,double *retval)
{
// another fsa simulation
	sds_real intpart=0.0,fracpart=0.0,exponent=0.0,expsign=1.0,fracmult=1.0;
	int digitsread;

	*retval=0.0;

	if (*cptr=='.'/*DNT*/) {										// only has fractional part
		if (cptr[1]==0) return RTERROR;						// some digits must follow to be legal
		goto fracnode;
	}

	if (!isdigit((unsigned char) *cptr)) return RTERROR;					// must have a digit or decimal

//intnode:
	gathervalue(&cptr,&intpart,&digitsread);				// gather the integer part

	if ((*cptr)==0) goto successnode;						// no fraction, done
	if ((*cptr)=='.'/*DNT*/) goto fracnode;						// process the fraction
	if ((*cptr)=='E'/*DNT*/ || (*cptr)=='e'/*DNT*/) goto exponentnode;	// exponent now
	return RTERROR;											// else failure

fracnode:													// process the fractional part
	cptr++;													// skip the decimal point
	if ((*cptr)==0) goto successnode;						// ending with decimal point is ok, done
	if ((*cptr)=='E'/*DNT*/ || (*cptr)=='e'/*DNT*/) goto exponentnode;	// exponent immediately after decimal is ok too
	if (!isdigit((unsigned char) *cptr)) return RTERROR;					// otherwise we need a digit

//gatherfrac:
	gathervalue(&cptr,&fracpart,&digitsread);				// gather the fractional part
	while (digitsread--) fracpart/=10.0;
	if ((*cptr)==0) goto successnode;						// no more, done
	if ((*cptr)!='E'/*DNT*/ && (*cptr)!='e'/*DNT*/) return RTERROR;		// only further acceptable input is exponent

exponentnode:												// process exponent
	cptr++;													// jump over the 'E' or 'e'
	if (isdigit((unsigned char) *cptr)) goto gatherexponent;
	if ((*cptr)=='+'/*DNT*/)										// set the sign, if any, and
		cptr++;												// fall through to gatherexponent
	else if ((*cptr)=='-'/*DNT*/) {
		expsign=-1.0;
		cptr++;
	}

gatherexponent:
	gathervalue(&cptr,&exponent,&digitsread);
	if ((*cptr)!=0 || exponent>99) return RTREJ;

successnode:
	if (icadRealEqual(exponent,0.0)) *retval=intpart+fracpart;
	else *retval=(intpart+fracpart)*pow(10,expsign*exponent);
	return(RTNORM);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_distof(const char *szDistance, int nUnitType, double *pdDistance) {


	// Modified PK 17/05/2000
	char *stringtoparse, *parseptr, *orgstr; // Reason : Fix for bug no.57888
	struct resbuf rb;
	int positive; //,loopct;

	short success;

	if(szDistance==NULL) return(RTERROR);
	stringtoparse=parseptr= new char [ strlen(szDistance)+1];
	strcpy(stringtoparse,szDistance);

	// Modified PK 17/05/2000
	orgstr= new char [ strlen(szDistance)+1]; // Reason : Fix for bug no.57888
	memset(orgstr,0,strlen(szDistance)+1);

	ic_trim(stringtoparse,"b"/*DNT*/);	 // remove beginning and ending spaces
	ic_trim(stringtoparse,"e"/*DNT*/);	 // which ACAD ignores

	if(nUnitType<0) {
		if(SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) nUnitType=rb.resval.rint;
		else nUnitType=0;
	} else if(nUnitType>5) nUnitType=5; //*** uh huh-huh huh-huh huh-huh huh-huh huh-huh huh-huh huh-huh

	positive=1;
	if(*parseptr=='-'/*DNT*/) {
		positive = 0;
		parseptr++;
	}
	else if(*parseptr=='+'/*DNT*/)
		parseptr++;

	// Modified PK 17/05/2000 [
	// Reason : Fix for bug no. 57888
	// if we see an E or e, process as a decimal number
//	if (strchr(parseptr,'E'/*DNT*/)!=NULL || strchr(parseptr,'e'/*DNT*/)!=NULL) success=parse_as_decimal(parseptr,pdDistance);
	if (strchr(parseptr,'E'/*DNT*/)!=NULL || strchr(parseptr,'e'/*DNT*/)!=NULL)
		{
		strcpy(orgstr,parseptr);
		success=parse_as_decimal(parseptr,pdDistance);
		if(success == RTNORM)
			*pdDistance=atof(orgstr);
		}
	// Modified PK 17/05/2000 ]

	// otherwise, if the string contains a foot sign, a dash for a fraction, a slash for a fraction,
	// or an inch sign, we process it as architectural
	else if (strchr(parseptr,'\''/*DNT*/)!=NULL ||
		strchr(parseptr,'-'/*DNT*/)!=NULL ||
		strchr(parseptr,' '/*DNT*/)!=NULL ||
		strchr(parseptr,'/'/*DNT*/)!=NULL ||
		strchr(parseptr,'"'/*DNT*/)) {
		success=parse_as_architectural(parseptr,pdDistance);
	}
	// otherwise, back to decimal again
	else success=parse_as_decimal(parseptr,pdDistance);

	if (!positive) *pdDistance = -(*pdDistance);

	IC_FREE(stringtoparse);
	// Modified PK 17/05/2000
	IC_FREE(orgstr); // Reason : Fix for bug no. 57888

	return(success);
}

int cmd_rtos_dim(double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType, int iDimdsep)
{
	return(cmd_rtos_main(dNumber,nUnitType,nPrecision,szNumber,2, iDimType, iDimdsep));
}

int cmd_rtos_area(double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType)
{
	return(cmd_rtos_main(dNumber,nUnitType,nPrecision,szNumber,1, iDimType, 0));
}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- is this thread safe??
//
int
sdsengine_rtos(double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType)
	{


	return(cmd_rtos_main(dNumber,nUnitType,nPrecision,szNumber,0, iDimType, 0 ));
	}

// SystemMetrix(Hiro)-[FIX: fuzz of rtos
static
BOOL
ConvertStringWithDimzin(
  double    dNumber,
  const int nPrecision,
  const int mode,
  const int dimzin,
  char*     szNumber
)
{
  sprintf(szNumber, "%.*f"/*DNT*/, nPrecision, dNumber);

  if (dimzin <= 3)
    {
      return TRUE;
    }

  if (!(mode == 2 || mode == 0))
    {
      return TRUE;
    }

  char str[IC_ACADBUF];
  sprintf(str, "%0.*f"/*DNT*/, nPrecision, dNumber);
  strcpy(szNumber, str);

  char* p = str;

  // Deal with DIMZIN.
  if ((dimzin <=  7) || (12 <= dimzin))
    {
      // dimzin  4,  5,  6,  7
      // dimzin 12, 13, 14, 15
      if (p[0] == '-')
        {
          p++;
        }

      if (p[0] == '0')
        {
          p++;
          if (p[0] == '.')
            {
              strcpy(szNumber, p);
              strcpy(str, szNumber);
            }
        }
    }

  // SystemMetrix(Hiro)-[FIX: 20 -> 2
  //if (8 <= dimzin)
  char* pHasDot = strchr(str, '.');
  if (pHasDot == NULL)
    {
      // do nothing
    }
  else if (8 <= dimzin)
  // ]-SystemMetrix(Hiro) FIX: 20 -> 2
    {
      // dimzin  8,  9, 10, 11
      // dimzin 12, 13, 14, 15
      char* pos;

      // remove last '0'
      while (1)
        {
          p = strrchr(str, '0'/*DNT*/);
          if (p == NULL)
            {
              break;
            }
          pos = str + strlen(str) - 1;
          if (pos != p)
            {
              break;
            }
          *pos = '\0';
        }

      // remove '.'
      pos = str + strlen(str) - 1;
      if (*pos == '.')
        {
          *pos = '\0';
        }

      strcpy(szNumber, str);
	  
	  //Bugzilla No. 78183 ; 10-06-2002
	  if (icadRealEqual(dNumber,0.0) && strcmp(szNumber,"") == 0 )
		  strcpy(szNumber,"0");
		  
    }

  return TRUE;
}
// ]-SystemMetrix(Hiro) FIX: fuzz of rtos

int cmd_rtos_main(double dNumber, int nUnitType, int nPrecision, char *szNumber,int mode, int iDimType, int iDimdsep)
	{
/*
**	mode-> 0=standard 1=area 2=dimensions
**
**
**	We try to follow the SPIRIT of the real function.  To duplicate
**	it would be almost impossible, requiring LOTS of experimentation.
**	Not worth it.
**
**	Note that the real function uses ACAD system variables in certain
**	cases.	Ours doesn't (yet).
**********
*** NOTE: I found out that sprintf() (using Microsoft Visual C++ 2.1 compiler)
*** 	  rounds when a number is greater than 5,
*** 	  instead of greater than or equal to five like you may expect.
*** 	  (ex.	12.45 = 12.4 and 12.451 = 12.5)  - JJC
**********
*** NOTE: If UNITMODE==1, no spaces are printed for Engrg, Arch, or Fract
***		  modes.  We'll pad using '-'.
**********
*/

	// MTEXT markers for creating dimensions-with-superscript
	// EBATECH-[move to resource string because I want to use different string.
	//static const char *s_beginSSmarker = "{\\A0;\\H0.5x;\\S";
	//static const char *s_endSSmarker = "^ ;}";
	CString s_beginSSmarker = ResourceString(IDC_APP_DIMSS_MARKER_BEGIN,"{\\A0;\\H0.5x;\\S");
	CString s_endSSmarker	= ResourceString(IDC_APP_DIMSS_MARKER_END,"^ ;}");
	// ]-EBATECH

	char  fs1[IC_ACADBUF],
		 *fcp1;
	int fi1,dimzin,unitmode;
	sds_real absval,fr1,frint,feet,frac1,fpart,inches,round,numerator;
	struct resbuf rb;
	int nMaxDigits = 16;

	if(szNumber==NULL) return(RTERROR);

	*szNumber=0; absval=(dNumber<0.0) ? -dNumber : dNumber;
	//*** Get the linear units system variable if needed.
	if(nUnitType<1) {
		SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		nUnitType=rb.resval.rint;
	} else if (nUnitType>5 && mode!=2) nUnitType=5;
	//*** Get the linear units precision system variable if needed.
	if(nPrecision<0) {
		SDS_getvar(NULL,DB_QLUPREC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		nPrecision=rb.resval.rint;
	}
	//*** Get the DIMZIN system variable for leading and trailing zeros.
	// EBATECH(CNBR) -[ Bugzilla#78443 2003/2/4 - Existing dimension uses its properties.
	switch (iDimType) {
	case DIMTYPEMAIN:
		SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		break;
	case DIMTYPETOL:
		SDS_getvar(NULL,DB_QDIMTZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		break;
	case DIMTYPEALT:
		SDS_getvar(NULL,DB_QDIMALTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		break;
	case DIMTYPEALTTOL:
		SDS_getvar(NULL,DB_QDIMALTTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		break;
	default:
		//SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		rb.resval.rint = iDimType;
		break;
	}
	//EBATECH(CNBR) ]-
	if(nUnitType>0 && nUnitType<9) dimzin=rb.resval.rint;
	else dimzin=0;

	unitmode=(SDS_getvar(NULL,DB_QUNITMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? rb.resval.rint : 0;

	// Set these here until we do the stacked or not stacked fractions.
	if (mode==2) {
		if (nUnitType==6) nUnitType=4;
		if (nUnitType==7) nUnitType=5;
		//if (nUnitType==8) nUnitType=2; EBATECH(CNBR) Add Windows NLS Support
	}

	switch(nUnitType) {
		case 1:  /* Scientific */
			if (nPrecision>15) nPrecision=15;
			sprintf(szNumber,"%0.*E"/*DNT*/,nPrecision,dNumber);
			// Move the last three characters toward the front of the string one char.
			// This is to mimic acad's +00 instead of the default +000.
			memmove(szNumber + _tcslen(szNumber) - 3, szNumber + _tcslen(szNumber) - 2, 3);
			break;
		case 2:  /* Decimal */
		case 8:  /* Windows Desktop EBATECH(CNBR) */
			if ((absval>1.0E+15 || absval<1.0E-15) && !icadRealEqual(absval,0.0)) {
				if (nPrecision>15) nPrecision=15;
				sprintf(szNumber,"%0.*E"/*DNT*/,nPrecision,dNumber);
				// Move the last three characters toward the front of the string one char.
				// This is to mimic acad's +00 instead of the default +000.
				memmove(szNumber + _tcslen(szNumber) - 3, szNumber + _tcslen(szNumber) - 2, 3);
			break;
			} else {
				fi1=(absval<1.0) ? 30 : 15; if (nPrecision>fi1) nPrecision=fi1;

				// SystemMetrix(Hiro)-[FIX: fuzz of rtos
//				sprintf(fs1,"%.*f"/*DNT*/,nPrecision,dNumber);
//				// DP: sprintf() rounds dNumber and really we obtain another number, so ...
//				dNumber = atof(fs1);
//				fr1 = modf(dNumber, &frint);
//				//*** Deal with DIMZIN.
//				if (dNumber>0.0 && dimzin>3 && (mode == 2 || mode == 0)) {
//					if (dimzin > 3 && dimzin < 8) {
//						if (frint==0.0) {
//							if ((fcp1=strchr(fs1,'.'))!=NULL)
//								strcpy(szNumber,fcp1);
//						} else
//							strcpy(szNumber,fs1);
//					} else if (dimzin >= 8 && dimzin < 12) {
//						sprintf(szNumber,"%.*g"/*DNT*/,nMaxDigits,dNumber);
//					} else if (dimzin >= 12) {
//						sprintf(fs1,"%.*g"/*DNT*/,nMaxDigits,dNumber);
//						if (frint==0.0) {
//							if ((fcp1=strchr(fs1,'.'/*DNT*/))!=NULL) strcpy(szNumber,fcp1);
//						} else strcpy(szNumber,fs1);
//					} else {
//						sprintf(szNumber,"%0.*f"/*DNT*/,nPrecision,dNumber);
//					}
//				} else {
//					sprintf(szNumber,"%0.*f"/*DNT*/,nPrecision,dNumber);
//				}
				ConvertStringWithDimzin(dNumber, nPrecision, mode, dimzin, szNumber);
				// ]-SystemMetrix(Hiro) FIX: fuzz of rtos

				// Windows Desktop Setting EBATECH(CNBR)
				if( nUnitType == 8 ){
					NUMBERFMT	nfBuf;
					int nChar;
					// Get number of decimals.
					nfBuf.NumDigits = ((fcp1 = _tcschr( szNumber, _T('.'))) ? ( _tcslen(fcp1) - 1 ): 0 );
					// Get suppress leading zeros from DIMZIN system variable.
					nfBuf.LeadingZero = (( dimzin & 4 ) != 0 );
					// Get grouping string from Windows Desktop setting.
					// switch from LOCALE_SYSTEM_DEFAULT to LOCALE_USER_DEFAULT
					if( GetLocaleInfo( LOCALE_USER_DEFAULT,LOCALE_SGROUPING,&(fs1[0]),8)){
						nfBuf.Grouping = ( fs1[2] == '0' ) ? atoi(&(fs1[0])) : 32;
					}else{
						nfBuf.Grouping = 3;
					}
					// Get decimal point string from Windows Desktop setting.
					nfBuf.lpDecimalSep = &(fs1[0]);
					if( GetLocaleInfo( LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,&(fs1[0]),8)==0){
						fs1[0] = _T('.');
						fs1[1] = _T('\0');
					}
					// Get thousand separator from Windows Desktop setting.
					nfBuf.lpThousandSep = &(fs1[8]);
					if( GetLocaleInfo( LOCALE_USER_DEFAULT,LOCALE_STHOUSAND,&(fs1[8]),8)==0){
						fs1[8] = _T(',');
						fs1[9] = _T('\0');
					}
					// Get negative number format from Windows Desktop setting.
					if( GetLocaleInfo( LOCALE_USER_DEFAULT,LOCALE_INEGNUMBER,&(fs1[16]),8)){
						nfBuf.NegativeOrder = atoi( &(fs1[16]) );
					}else{
						nfBuf.NegativeOrder = 1;
					}
					// Then, reform number string using GetNumberFormat NLS API.
					if(( nChar = GetNumberFormat( LOCALE_USER_DEFAULT, 0, szNumber, &nfBuf, &fs1[16], IC_ACADBUF-16)) > 0 ){
						_tcscpy(szNumber,&fs1[16]);
					}
				}// EBATECH(CNBR) END
				// The following section was added especially for implementation of dimensions
				// with superscript
				if( mode == 2 && !(iDimType == DIMTYPEALTTOL || iDimType == DIMTYPETOL) ) // case of dimension
				{
					struct resbuf rb;
					if( sds_getvar( "DIMSS", &rb ) == RTNORM )
					{
						if( rb.resval.rint )
						{
							if( sds_getvar( "DIMSSPOS",&rb ) == RTNORM )
							{
								int pos = rb.resval.rint;
								if( pos < nPrecision ) // only if precision of the number is greater than pos of superscripting
								{
									char *dot =  strrchr( szNumber, '.' );
									if( dot )
									{

										if( strlen( dot+1 ) > pos )// there is a fractional part of the string
										{
											dot = dot + 1 + pos; // now dot points to the first char of the part to be superscripted
											int ssLen = strlen( dot );
											//Trim trailing zeroes in superscript. German rule.
											//And you don't want a zero that looks like a degree symbol
											while (ssLen > 0 && dot[ssLen-1]=='0')
												ssLen-=1;

											if(ssLen)
											{
												char *ssString = new char[ssLen + 1];
												strncpy( ssString, dot,ssLen);

												// Then we have to add formatting string before the superscript part
												strcpy( dot, s_beginSSmarker );
												dot += strlen( s_beginSSmarker );
												strcpy( dot, ssString );
												dot += ssLen;
												strcpy( dot, s_endSSmarker );
												delete [] ssString;
											}
											else
											{
												*dot=0;
											}
										}
									}
								}
							}
						}
					}
				}
				// End of dimension-with-superscript section
				// EBATECH(CNBR) -[ 2003/3/31 Support DIMDSEP
				if( nUnitType == 2 && iDimdsep != 0 && iDimdsep != '.')
				{
					char *pstr;
					if( pstr = strchr(szNumber, '.'))
						*pstr = (char)iDimdsep;
				}
				// EBATECH(CNBR) ]-
			}
			break;
		case 3:  //*** Engineering
			*szNumber=0;
			if(absval!=dNumber) strcpy(szNumber,"-"/*DNT*/);
			//*** Convert from decimal to feet.
			absval=((mode==1)?(absval/144.0):(absval/12.0));
			//*** Split whole number and fractional amount apart.
			fr1=modf(absval,&frint);
			//*** Convert fractional amount into inches.
			fr1=((mode==1)?(fr1*144.0):(fr1*12.0));
			//*** Round the number of inches to correct precision.
			sprintf(fs1,"%0.*f\""/*DNT*/,nPrecision,fr1);
			fr1=atof(fs1);
			//*** Deal with DIMZIN.
			if(icadRealEqual(frint,0.0)) {  //*** If zero feet.
				//*** Don't suppress zero feet.
				if(fr1>0.0 && (dimzin==1 || dimzin==2 || dimzin==5 || dimzin==6 || dimzin==9 || dimzin==10 || dimzin==13 || dimzin==14)){
					if(unitmode)
						strcat(szNumber,"0'"/*DNT*/);
					else
						strcat(szNumber,"0'-"/*DNT*/);
				}
			} else {
				if(fr1>0.0){
					if(unitmode)
						sprintf(fs1,"%0.0f'"/*DNT*/,frint);
					else
						sprintf(fs1,"%0.0f'-"/*DNT*/,frint);
				} else { //*** If zero inches
					if(dimzin==1 || dimzin==3){
						if(unitmode)
							sprintf(fs1,"%0.0f'"/*DNT*/,frint);
						else
							sprintf(fs1,"%0.0f'-"/*DNT*/,frint);
					} else {
						sprintf(fs1,"%0.0f'"/*DNT*/,frint);
						strcat(szNumber,fs1);
						break;
					}
				}
				strcat(szNumber,fs1);
			}
			if (dimzin >= 8 && dimzin <=15) {
				sprintf(fs1,"%0.*g\""/*DNT*/ ,nPrecision,fr1);
				if (dimzin >= 12 && fr1 < 1.0) {
					if ((fcp1=strchr(fs1,'.'/*DNT*/))!=NULL) {
						char tmp[IC_ACADBUF];
						strcpy(tmp,fcp1);
						strcpy(fs1, tmp);
					}
				}
			} else
				sprintf(fs1,"%0.*f\""/*DNT*/,nPrecision,fr1);

			// Modified PK 17/05/2000 [
			// Reason : fix for bug no. 52604
			if(fr1>0.0)
				strcat(szNumber,fs1);
			else
				strcpy (szNumber, fs1);
			// Modified PK 17/05/2000 ]

			if (icadRealEqual(frint,0.0) && (dimzin==4 || dimzin==7 || dimzin==12)) {
				strcpy(fs1,szNumber);
				modf(fr1,&frint);
				if (icadRealEqual(frint,0.0)) {
					fcp1=strchr(fs1,'.');
					if (fcp1 != NULL)
						strcpy(szNumber,fcp1);
					else
						szNumber = NULL;
					}
			}
			break;
		case 4:  //*** Architectural
			*szNumber=0;
			fi1=nPrecision;
			if (fi1<0 || fi1>30) fi1=30;
			nPrecision=1<<fi1;//convert mode into denominator
			fr1=(1.0+CMD_FUZZ)*0.5/(((1==mode)?144.0:12.0)*nPrecision);//let fr1 represent fuzz for this precision

			//We have to worry about successive 'roundoff creep'.  The fraction handler
			//	will always do roundup/rounddown, but we have to do it 'by hand' for the
			//	feet & inches portion.	Actually, all we have to worry about is rounding
			//	up because the left over fraction will automatically round itself down
			//*** If negitive remember the sign
			if(absval!=dNumber) strcpy(szNumber,"-"/*DNT*/);
			//*** Convert from decimal inches to feet.
			absval=((mode==1)?(absval/144.0):(absval/12.0));
			//*** GET feet.
			frac1=modf(absval,&feet);
			//check for roundoff error
			if((1.0-frac1)<=fr1){
				feet++;
				frac1=0.0;
			}
			//*** Convert fractional feet amount into inches.
			frac1=((mode==1)?(frac1*144.0):(frac1*12.0));
			//*** Convert roundup from feet to inches
			fr1*=(1==mode)?144.0:12.0;
			//*** Get inches
			fpart=modf(frac1,&inches);
			//check for roundoff again...
			if((1.0-fpart)<=fr1){
				inches++;
				if(inches>=12.0*(1.0-CMD_FUZZ)){
					feet++;
					inches=0.0;
				}
				fpart=0.0;
			}
			//*** Convert fraction inches amount into numerator
			fpart*=nPrecision;
			//*** Get numerator
			round=modf(fpart,&numerator);
			//*** Check if the numerator needs to be rounded
			if(round>=0.5*(1.0-CMD_FUZZ)){
				numerator+=1.0;
			}
			//*** If the fraction can be reduced... Do it.
			while(icadRealEqual(fmod(numerator,2),0.0) && !icadRealEqual(numerator,0.0)){
				numerator/=2.0;
				nPrecision/=2;
			}
			//*** Deal with DIMZIN.
			if (mode==2) {
				if (*szNumber)
				{
					sprintf(fs1, "%0.0f'"/*DNT*/,feet);
					strcat (szNumber, fs1);
				}
				else
					sprintf(szNumber,"%0.0f'"/*DNT*/,feet);

				if (feet <= 0.0 && (dimzin == 0 || dimzin == 4 || dimzin == 8 || dimzin == 12 || dimzin == 3 || dimzin == 7 || dimzin == 11 || dimzin == 15)) *szNumber = 0;


				*fs1=0;
				if (inches <= 0.0 && (dimzin == 0 || dimzin == 4 || dimzin == 8 || dimzin == 12 || dimzin == 2 || dimzin == 6 || dimzin == 10 || dimzin == 14)) {
					if (icadRealEqual(numerator,0.0))
						*fs1 = '\0'/*DNT*/;
					else {
						if (dimzin == 0|| dimzin == 4 || dimzin == 8 || dimzin == 12 && feet <= 0)
							sprintf(fs1,"%0.0f/%d\""/*DNT*/,numerator,nPrecision);
						else
							sprintf(fs1,"0%0.0f/%d\""/*DNT*/,numerator,nPrecision);
					}
				} else if (inches <= 0.0 && (dimzin == 1 || dimzin == 5 || dimzin == 9 || dimzin == 13 || dimzin == 3 || dimzin == 7 || dimzin == 11 || dimzin == 15)) {
					if (icadRealEqual(numerator,0.0))
						sprintf(fs1, "0\""/*DNT*/);
					else {
						if (feet <= 0 && (dimzin == 3 || dimzin == 7 || dimzin == 11 || dimzin == 15))
							sprintf(fs1,"%0.0f/%d\""/*DNT*/,numerator,nPrecision);
						else
							sprintf(fs1,"0%0.0f/%d\""/*DNT*/,numerator,nPrecision);
					}
					} else if (numerator <= 0.0) {
						sprintf(fs1,"%0.0f\""/*DNT*/,inches);
					} else
						sprintf(fs1,"%0.0f %0.0f/%d\""/*DNT*/,inches,numerator,nPrecision);

				if (*szNumber) {
					if (*fs1) {
						strcat(szNumber,"-"/*DNT*/);
						strcat(szNumber,fs1);
					}
				} else if (!(*szNumber) && (icadRealEqual(dNumber,0.0) || dNumber < 1.0/nPrecision)) {
					// I'll have to revisit this later.
					strcpy(szNumber, (nUnitType > 3 && nUnitType < 8) ? "0\""/*DNT*/ : "0.0000"/*DNT*/);
				} else
					strcpy(szNumber,fs1);
			} else {
				if(icadRealEqual(feet,0.0)) {  //*** If zero feet.
					//*** Don't suppress zero feet.
					if(inches>0.0){
						if(unitmode)
							strcat(szNumber,"0'"/*DNT*/);
						else
							strcat(szNumber,"0'-"/*DNT*/);
					}
				}else{
					if(inches>0.0){
						if(unitmode)
							sprintf(fs1,"%0.0f'"/*DNT*/,feet);
						else
							sprintf(fs1,"%0.0f'-"/*DNT*/,feet);
					}else{ //*** If zero inches
						if(0==unitmode){
							sprintf(fs1,"%0.0f'-"/*DNT*/,feet);
						}else{
							sprintf(fs1,"%0.0f'"/*DNT*/,feet);
						}
					}
					strcat(szNumber,fs1);
				}
				if(nPrecision==1 || icadRealEqual(numerator,0.0) || nPrecision==numerator){
					sprintf(fs1,"%0.0f\""/*DNT*/,inches);
					if(icadRealEqual(feet,0.0)){
						if (*szNumber)
							strcat(szNumber,fs1);
						else
							strcpy(szNumber,fs1);
					}else{
						strcat(szNumber,fs1);
					}
				}else{
					if(unitmode)
						sprintf(fs1,"%0.0f-%0.0f/%d\""/*DNT*/,inches,numerator,nPrecision);
					else
						sprintf(fs1,"%0.0f %0.0f/%d\""/*DNT*/,inches,numerator,nPrecision);
					strcat(szNumber,fs1);
				}
			}
			break;
		case 5:  //*** Fractional
			*szNumber=0;
			fr1=absval;
			fi1=nPrecision;
			if (fi1<0 || fi1>30) fi1=30;
			nPrecision=1<<fi1;

			//*** If negitive remember the sign
			if(absval!=dNumber) strcpy(szNumber,"-"/*DNT*/);
			//print whole integer portion
			frac1=modf(fr1,&fpart);
			if(fpart>0.0){
				sprintf(fs1,"%li"/*DNT*/,(long)fpart);
				strcat(szNumber,fs1);
			}
			//get the numerator for the fraction
			frac1*=nPrecision;
			round=modf(frac1,&numerator);
			if(round>=(1.0-CMD_FUZZ)*0.5)numerator+=1.0;
			if((int)nPrecision==numerator){//if rounding up gets us to a new integer...
				fpart+=1.0;
				sprintf(fs1,"%li"/*DNT*/,(long)fpart);
				strcpy(szNumber,fs1);
				break;
			}
			//*** If the fraction can be reduced... Do it.
			if(!icadRealEqual(numerator,0.0)){
				while(icadRealEqual(fmod(numerator,2),0.0) && !icadRealEqual(numerator,0.0)){
					numerator/=2.0;
					nPrecision/=2;
				}
				if(fpart>0.0){
					if(unitmode)
						strcat(szNumber,"-"/*DNT*/);	//add dash between integer & fraction
					else
						strcat(szNumber," "/*DNT*/);	//add space between integer & fraction
				}
				sprintf(fs1,"%i/%i"/*DNT*/,(int)numerator,nPrecision);
				strcat(szNumber,fs1);
			}else{
				if(icadRealEqual(fpart,0.0))strcpy(szNumber,"0"/*DNT*/);
			}


			break;
		case 6:  /* Mapping */
			if (nPrecision>15) nPrecision=15;
			sprintf(szNumber,"%.*f"/*DNT*/,nPrecision,dNumber);
			break;
	}

	return(RTNORM);
}

