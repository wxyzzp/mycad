/* F:\DEV\PRJ\LIB\ICADLIB\NORMANG.C
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icadlib.h"/*DNT*/
#include <float.h>

int
ic_isvalid_real(sds_real rInput)
{

#ifdef OLDWAY

	int	fpClass = _fpclass(rInput);
	switch(fpClass)
	{
		case _FPCLASS_SNAN: /* signaling NaN */
		case _FPCLASS_QNAN: /* quiet NaN */
		case _FPCLASS_NINF: /* negative infinity */
		case _FPCLASS_ND:   /* negative denormal */
		case _FPCLASS_PD:   /* positive denormal */
		case _FPCLASS_PINF:	/* positive infinity */
			return 0;
		case _FPCLASS_PN:  /* positive normal */
		case _FPCLASS_NN:  /* negative normal */
		case _FPCLASS_NZ:  /* -0 */
		case _FPCLASS_PZ:  /* +0 */
			return 1;
	}
	return 1;

#else // OLDWAY

	unsigned char*	cptr = (unsigned char*)&rInput;
	// for bigendian, make the [7] [0] and the [6] [1]
	// basically, if the exponent is all 0's, it's unnormalized (denormal), if the exponent is all 1's it's
	// either NaN or one of the infinities
	// test for 0.0 first

	/*D.G.*/// Replacing memcmp with the direct comparison decreased function's time by 97%.
	/*if(!memcmp(cptr, "\0\0\0\0\0\0\0", 7) && !(cptr[7] & 0x7F))
		return 1;	// 0.0, pos and neg*/
	if( !cptr[0] && !cptr[1] && !cptr[2] && !cptr[3] && !cptr[4] && !cptr[5] && !cptr[6] &&
		!(cptr[7] & 0x7F) )
		return 1;		// 0.0, pos and neg

	if( ((cptr[7] & 0x7F) == 0x7F && (cptr[6] & 0xF0) == 0xF0) ||	// all 1's exponent
		(!(cptr[7] & 0x7F) && !(cptr[6] & 0xF0)) )					// all 0's exponent that is not 0.0 itself
		return 0;
	else
		return 1;

#endif // OLDWAY

}

/******************************************************************************
*+ ic_roundReal - rounds a real number up or down
*
*  Note that fractions .5 or greater are rounded up and fractions -.5 and
*  smaller are rounded down.
*
*/

void ic_roundReal(
	sds_real& value)
	{
	bool valueIsPositive = (value >= 0.0) ? true : false;

	sds_real fabsValue = fabs(value);
	long tmpLong = (long) fabsValue;
	sds_real tmpFraction = fabsValue - tmpLong;
	if (tmpFraction >= 0.5)
		tmpLong++;

	value = valueIsPositive ? (sds_real) tmpLong : (sds_real) -tmpLong;
	}

void ic_normang(sds_real *a1, sds_real *a2) 
	{
/*
**  Normalizes 1 or 2 angles for CCW arc work.  *a1 is set to
**  0.0 <= *a1 < IC_TWOPI.  If a2!=NULL, *a2 is similarly adjusted -- but
**  then further adjusted so that *a2>=*a1 (so that an ending angle is
**  always greater than or equal to a starting angle).
*   Function altered 12/10/97 to accept much larger values
*/
    sds_real ar1;


//	ASSERT( ic_isvalid_real( *a1 ) );
//	ASSERT( (a2 == NULL) || 
//		    (ic_isvalid_real( *a2) ));
//	ASSERT( fabs(*a1) < 1.0e6);
//	ASSERT( (a2 == NULL) || 
//		    (fabs(*a2) < 1.0e6));


	// Bad input
	if((!ic_isvalid_real(*a1)) || 
		(fabs(*a1) > 1.0e6))
		{
		*a1 = 0.0;
		return;
		}
	//work incrementally to move the value into an acceptable range

    /* Avoid seemingly endless loop for vals like 1.0E+5: */
	ar1=1.0e5*IC_TWOPI;
    while(*a1 > ar1) 
		{
		*a1-=ar1;
		}
	while(*a1 < -ar1)
		{
		*a1+=ar1;
		}

	ar1=1.0e3*IC_TWOPI;
    while(*a1 > ar1) 
		{
		*a1-=ar1;
		}
	while(*a1 < -ar1)
		{
		*a1+=ar1;
		}

    if (*a1 + IC_ZRO < 0.0) 
		{
        do 
			{ 
			*a1+=IC_TWOPI; 
			} while (*a1 + IC_ZRO < 0.0);
		} 
	else if (*a1 - IC_ZRO >= IC_TWOPI) 
		{
        do 
			{ 
			*a1-=IC_TWOPI; 
			} while (*a1 - IC_ZRO >= IC_TWOPI);
	    }

    if (a2!=NULL) 
		{

		// Bad input
		if((!ic_isvalid_real(*a2)) || 
			(fabs(*a2) > 1.0e6))
			{
			*a2 = 0.0;
			return;
			}

		ar1=1.0e5*IC_TWOPI;
		while(*a2 > ar1) 
			{
			*a2-=ar1;
			}
		while(*a2 < -ar1)
			{
			*a2+=ar1;
			}

		ar1=1.0e3*IC_TWOPI;
		while(*a2 > ar1) 
			{
			*a2-=ar1;
			}
		while(*a2 < -ar1)
			{
			*a2+=ar1;
			}

        if (*a2 + IC_ZRO < 0.0) 
			{
            do 
				{ 
				*a2+=IC_TWOPI; 
				} while (*a2 + IC_ZRO < 0.0);
	        } 
		else if (*a2 - IC_ZRO >= IC_TWOPI) 
			{
            do 
				{ 
				*a2-=IC_TWOPI; 
				} while (*a2 - IC_ZRO >= IC_TWOPI);
	        }
        if (*a2<*a1) 
			{
			*a2+=IC_TWOPI;  /* Make sure *a2>=*a1 */
			}
	    }
	}

/******************************************************************************
*+ ic_condenseAngle - converts angles 360 degrees and larger to less than 360
*
*/

void ic_condenseAngle(
	sds_real& value)
	{
	bool isNegative = (value < 0.0) ? true : false;

	sds_real tmpReal = fabs(value);
	while (tmpReal > 360.0)
		tmpReal -= 360.0;

	value = isNegative ? -tmpReal : tmpReal;
	}



