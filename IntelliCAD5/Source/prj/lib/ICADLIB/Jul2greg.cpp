#include "icadlib.h"/*DNT*/

#define SDN_OFFSET         32045
#define DAYS_PER_5_MONTHS  153
#define DAYS_PER_4_YEARS   1461
#define DAYS_PER_400_YEARS 146097

void ic_jul2greg(double dJul,int *pYear,int *pMonth,int *pDay,int *pDow,int *pHour,int *pMin,double *pSec){
    int iCentury,iYear,iMonth,iDay,iDow;
    long lTemp,lSdn;
    int iDayOfYear;
    double dFrac,dSdn,dHour,dMin,dSec;

    dFrac=modf(dJul,&dSdn);
    lSdn=(long)dSdn;
    
    if(lSdn <= 0) {
	    *pYear=*pMonth=*pDay=*pDow=*pHour=*pMin=0;
        *pSec=0.0;
	    return;
    }

    lTemp = (lSdn + SDN_OFFSET) * 4 - 1;

    /* Calculate the century (year/100). */
    iCentury = lTemp / DAYS_PER_400_YEARS;

    /* Calculate the year and day of year (1 <= dayOfYear <= 366). */
    lTemp = ((lTemp % DAYS_PER_400_YEARS) / 4) * 4 + 3;
    iYear = (iCentury * 100) + (lTemp / DAYS_PER_4_YEARS);
    iDayOfYear = (lTemp % DAYS_PER_4_YEARS) / 4 + 1;

    /* Calculate the month and day of month. */
    lTemp = iDayOfYear * 5 - 3;
    iMonth = lTemp / DAYS_PER_5_MONTHS;
    iDay = (lTemp % DAYS_PER_5_MONTHS) / 5 + 1;

    /* Convert to the normal beginning of the year. */
    if(iMonth < 10) {
	    iMonth += 3;
    }else{
	    iYear += 1;
	    iMonth -= 9;
    }

    /* Adjust to the B.C./A.D. type numbering. */
    iYear -= 4800;
    if (iYear <= 0) iYear--;

    /*Calc Day of week */
    iDow=(int)(lSdn+1) % 7;
    if(iDow < 0) iDow=7;

    /*Calc Hour */
    dFrac*=24;
    dFrac=modf(dFrac,&dHour);
    
    /*Calc Min */
    dFrac*=60;
    dFrac=modf(dFrac,&dMin);
    
    /*Calc Seconds */
    dSec=dFrac*60;

    *pYear  = iYear;
    *pMonth = iMonth;
    *pDay   = iDay;
    *pDow   = iDow;
    *pHour  = (int)dHour;
    *pMin   = (int)dMin;
    *pSec   = dSec;
}
