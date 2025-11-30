/* C:\ICAD\PRJ\LIB\DB\OBJHANDLE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

inline short hexval(char c)
{
  if (c>='0' && c<='9') return(c-'0');
  if (c>='A' && c<='F') return(c-'A'+10);
  if (c>='a' && c<='f') return(c-'a'+10);
  return(0);
}

db_objhandle::db_objhandle(const char *ahan)
	{
	short len,val,charsleft;
	unsigned char *bincharloc;
	short numbinchars;

	handle[0]=handle[1]=0L;
	len=strlen(ahan);
	if (len!=0)
		{
		numbinchars=(len+1)/2;

		ASSERT(numbinchars<=8);
		if (numbinchars<=8)
			{

// point directly at the location in the two longs where we will put the binary data
			bincharloc=((unsigned char *)&handle[0])+numbinchars-1;

			if (len & 1) // odd length, process the first character separately
				{
				*bincharloc--=(char)hexval(*ahan++);
				len--;
				}

			charsleft=len/2;

			while (charsleft--)
				{
				val=hexval(*ahan++)*16;
				val+=hexval(*ahan++);
				*bincharloc-- = (char)(val & 0xFF);
				}
			}
		}
	}

__int64 db_objhandle::ToHugeInt()
{
	__int64 retVal;

	retVal = handle[0];
	retVal = retVal << 32;
	retVal += handle[1];
	return retVal;
}

void db_objhandle::ToAscii(char *ahan)
	{
	short i;
	unsigned char nextchar;
	unsigned short nibble;
	unsigned char *enthandle=((unsigned char *)(&handle))+7;

	i=7;
	while (i>=0 && !(*enthandle))
		{
		i--;
		enthandle--;
		}
	if (i<0)
		{
		ahan[0]='0'; ahan[1]=0;
		return;
		}

	nextchar=*enthandle--;
	i--;
	nibble=(nextchar & 0xF0);
	if (nibble!=0)   // DON'T add a leading zero
		{
		nibble>>=4;
		*ahan++ = ((nibble<=9) ? nibble+'0' : nibble + 'A' - 10);
		}
	nibble=(nextchar & 0x0F);
	*ahan++ = ((nibble<=9) ? nibble+'0' : nibble + 'A' - 10);

	while (i>=0) 
		{
		i--;
		nextchar=*enthandle--;
		nibble=((nextchar & 0xF0) >> 4);
		*ahan++ = ((nibble<=9) ? nibble+'0' : nibble + 'A' - 10);
		nibble=(nextchar & 0x0F);
		*ahan++ = ((nibble<=9) ? nibble+'0' : nibble + 'A' - 10);
		}

	*ahan=0;  /* terminate the string */
	}
