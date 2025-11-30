
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/


int
ic_isnumstr(char *ss, double *retval)
	{

/*
**  Returns 1 if ss appears to represent a number, 0 otherwise.
**  Function strtod() is used to find the 1st char that can't be
**  converted.  ss is considered non-numeric if it's NULL, or
**  "", or any of the chars from the 1st ususable char on are
**  non-white-space.
*/
	// New Notes:  changed to return the value in retval.  Value is only
	// guaranteed to be good if ic_isnumstr returns 1.
    char *fcp1;

    if (ss==NULL || !*ss)
		{
		return 0;
		}


    *retval = strtod(ss,&fcp1);

    if (fcp1==NULL) 
		return 1;  /* Should never */
    if (fcp1==ss) 
		return 0;
    while (*fcp1 && isspace((unsigned char) *fcp1)) 
		{
		fcp1++;
		}

    return (*fcp1==0);

	}


/******************************************************************************
*+ ic_strIsValidReal - sees if the specified string is a valid real number
*
*  Checks to see if the specified string is a valid real number.
*
*  If allowNegative is true, - is allowed; but the function will return false
*  if there is more than one - or if the - isn't the first character.
*
*  If chewWhiteSpace is true, all spaces will be removed from the string.
*
*  If realVal is passed in, either 0.0 or the actual atof() value will be passed
*  back.  However, realVal is only valid if the function returns true.
*
*  WARNING!!!!!!!  The function may modify the string; so if you want to keep
*                  the original string, make a copy of it before passing it into
*                  this function.
*
*  WARNING!!!!!!!  This function currently does not support values entered in
*				   scientific notation.  Such values will be considered invalid.
*/

bool ic_strIsValidReal(
	char *strVal,
	const bool allowNegative,
	const bool chewWhiteSpace,
	sds_real *realVal)
	{

	if (realVal)
		*realVal = 0.0;

	// no string
	if (NULL == strVal)
		return false;

	// remove spaces
	if (chewWhiteSpace)
		ic_trim(strVal, "bme"/*DNT*/);

	// walk down the string to make sure it contains
	// the proper characters
	int decimalCount = 0;
	int negativeCount = 0;
	int length = strlen(strVal);
	for (int index = 0; index < length; index++)
		{
		if (strVal[index] == ' '/*DNT*/)				// shouldn't have spaces
			return false;
		else if (strVal[index] == '.'/*DNT*/)			// should only have 1 decimal point
			{
			decimalCount++;
			if (decimalCount > 1)
				return false;
			}
		else if (strVal[index] == '-'/*DNT*/)			// should have 0 or 1 negative
			{
			if (!allowNegative)
				return false;
			else
				{
				negativeCount++;
				if (negativeCount > 1)
					return false;

				if (index > 0)							// negative should be first character
					return false;
				}
			}
		else if (!isdigit((unsigned char) strVal[index]))				// must be 0 to 9
			return false;
		}

	if (0 == index)										// empty string
		return false;

	if (realVal)
		 *realVal = atof(strVal);

	return true;
	}
