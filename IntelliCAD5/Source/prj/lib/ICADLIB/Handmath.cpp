
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/


#define MAXNC 16  /* Max number of chars long the string version of a */
                  /*   handle can be. */

char * ic_handleStringToUpperCase( char *string )
	{
	char *cp;      
    for (cp=string; *cp; ++cp)
		{
		if ('a'/*DNT*/ <= *cp && *cp <= 'z'/*DNT*/)
			{
			*cp += 'A'/*DNT*/ - 'a'/*DNT*/;
			}
		} 
	return(string);
	}


