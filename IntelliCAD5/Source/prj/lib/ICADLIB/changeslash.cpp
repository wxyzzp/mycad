
// THIS FILE HAS BEEN GLOBALIZED!

#include <icadlib.h>

void ChangeSlashToBackslashAndRemovePipe(char *str)
{
	//*** Convert any /'s to \'s that may be in the path
	char* cp1;
	for(cp1=str; *cp1!=0; cp1++)
	{
		if(*cp1=='/'/*DNT*/) *cp1='\\'/*DNT*/;
		if(*cp1=='|'/*DNT*/) *cp1=' '/*DNT*/;
		// 2000-6-16 EBATECH(ARA) for MBCS String [
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*cp1,0)) &&
			(_MBC_TRAIL==_mbbtype((unsigned char)*(cp1+1),1)))
			cp1 ++;
		// ]  2000-6-16 EBATECH(ARA)
	}

}

