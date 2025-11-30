/* PRJ\LIB\ICADLIB\RESBUF.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * Utility functions for dealing with resbufs.
 * 
 */ 

#include "icadlib.h"
#include "Resbuf.h"/*DNT*/
#include <string.h>


void FreeResbufIfNotNull(resbuf **rb)
	{
	if (*rb!=NULL)
		sds_relrb(*rb);
	*rb=NULL;
	}


/******************************************************************************
*+ appendRbLists - appends one resbuf list to another
*
*  Appends list2 to list1 and sets list2 to NULL.
*/

void appendRbLists(
	resbuf **list1,			// i/o - list that is added to
	resbuf **list2)			// i/o - list that is added; is set to NULL by function
	{

	if (NULL == *list1)
		{
		*list1 = *list2;
		return;
		}

	// find the end of the first list
	resbuf *tmpRb = *list1;
	while (tmpRb->rbnext != NULL)
		tmpRb = tmpRb->rbnext;

	tmpRb->rbnext = *list2;
	*list2 = NULL;
	}

resbuf *BuildDataList(
	resbuf **rbb,
	int type,
	const sds_point value)
	{
	return(DataList(&(*rbb), type, (sds_point *) value));
	}

resbuf *BuildDataList(
	resbuf **rbb,
	int type,
	const sds_real value)
	{
	return(DataList(&(*rbb), type, (sds_real *) &value));
	}

resbuf *BuildDataList(
	resbuf **rbb,
	int type,
	const int value)
	{
	return(DataList(&(*rbb), type, (int *) &value));
	}

resbuf *BuildDataList(
	resbuf **rbb,
	int type,
	const long value)
	{
	return(DataList(&(*rbb), type, (long *) &value));
	}

resbuf *BuildDataList(
	resbuf **rbb,
	int type,
	const char *value)
	{
	return(DataList(&(*rbb), type, (char *) value));
	}

resbuf *DataList(
	resbuf **rbb,
	int type,
	const void *value)
	{
	resbuf *rbt;

	if ( type EQ -3 )
		(*rbb) = sds_newrb(type);
	else if ( type != RTDXF0 )
		{
		(*rbb)->rbnext = sds_newrb(type);
		(*rbb)=(*rbb)->rbnext;
		}

	switch(type)
		{
		case RTDXF0:
			if (((*rbb) = sds_buildlist(RTDXF0, (const char *) value, 0)) == NULL)
				return(NULL);
			else
				break;

		case      6:
		case      4: // EBATECH(CNBR) Bigfont filename
		case      3:
		case      2:
		case   1000:
		case   1001:
			(*rbb)->resval.rstring = new char [strlen((const char *) value) + 1];
			strcpy((*rbb)->resval.rstring, (const char *) value);
			break;

		case     71:
		case     62:
		case     70:
		case    290: // EBATECH(CNBR) Layer|Plotable 
		case    370: // EBATECH(CNBR) Layer|Lineweight
			(*rbb)->resval.rint = *(const int *) value;
			break;

		case     50:
		case     51:
		case     40:
		case     41:
		case     42:
		case     43:
		case     44:
			(*rbb)->resval.rreal = *(const sds_real *) value;
			break;

		case     10:
		case     11:
		case     12:
			(*rbb)->resval.rpoint[0] = *((const double *) value + 0);
			(*rbb)->resval.rpoint[1] = *((const double *) value + 1);
			(*rbb)->resval.rpoint[2] = *((const double *) value + 2);
			break;

		case   1071:
			(*rbb)->resval.rlong = *(const long *) value;
			break;

		case     -3:	// here for completeness; but nothing to do
			break;
		}

	rbt = *rbb;
	return rbt;
	}





