
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/
#include <locale.h>
#include "win32misc.h"/*DNT*/


// these really, really shouldn't be in here.  It was in cmdstend.c, in which all functions were
// unused, so I took it out, but we need these definitions, so here they are.

//I changed the call to sds_isalnum to call isalnum directly to remove the sds dependency.

char ic_bylayer[IC_ACADNMLEN]="BYLAYER"/*DNT*/;
char ic_byblock[IC_ACADNMLEN]="BYBLOCK"/*DNT*/;
char ic_bycolor[IC_ACADNMLEN]="BYCOLOR"/*DNT*/;
// EBATECH-[
//short
//ic_isvalidname(char *pName)
//{
////	Checks pName for a valid ACAD name: 1 - (IC_ACADNMLEN - 1) chars, alphanumeric and $-_ .
////	Returns: 1 = ok; 0 = bad
//
//	char*	pChar = pName;
//	int		length = 0;
//
//	/*D.G.*/// Enable non-English single-byte characters names.
//	setlocale(LC_CTYPE, "");
//
//	for( ; length < IC_ACADNMLEN && *pChar && (_istalnum(_tcsnextc(pChar)) || strchr("$-_"/*DNT*/, *pChar));
//		pChar = _tcsinc(pChar), length += _tclen(pChar))
//		;
//
//	ic_setlocale();
//
//	return length && !*pChar;
//}
short ic_isvalidname(char *pName) {
    char *fcp1;
    short fi1;

	setlocale(LC_CTYPE, "");
	short fi2=0;
	for (fcp1=pName,fi1=0;*fcp1 && fi2<IC_ACADNMLEN;fcp1++,fi1++,fi2++){
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*fcp1,0)) &&
			(_MBC_TRAIL==_mbbtype((unsigned char)*(fcp1+1),1)) ){
			fcp1++;
			fi1++;
			continue;
		}
		
		// \U+xxxx
		if( (*fcp1 == '\\') && (toupper(fcp1[1]) == 'U') &&
			(fcp1[2] == '+') &&
			isxdigit((unsigned char) fcp1[3]) && isxdigit((unsigned char) fcp1[4]) &&
			isxdigit((unsigned char) fcp1[5]) && isxdigit((unsigned char) fcp1[6]) ){
				fcp1 += 6;
				fi1 += 6;
				continue;
		}
		// \M+nxxxx
		if( (*fcp1 == '\\') && (toupper(fcp1[1]) == 'M') &&
			(fcp1[2] == '+') && isdigit((unsigned char) fcp1[3]) &&
			isxdigit((unsigned char) fcp1[4]) && isxdigit((unsigned char) fcp1[5]) &&
			isxdigit((unsigned char) fcp1[6]) && isxdigit((unsigned char) fcp1[7]) ){
				fcp1 += 7;
				fi1 += 7;
				continue;
		}
		// " * , / : ; > = < ? \ ` can't use charactar.
		//Modified Cybage AJK 17/12/2001 [
		//Reason- Fix for bug no 77948 from Bugzilla
		//if (!(isalnum((unsigned char)*fcp1) || strchr(" !#$%&'()+-.@[]^_{|}~",*fcp1))) break;
		if (!(isalnum((unsigned char)*fcp1) || strchr(" !#$%&'()+-.@[]^_{}~",*fcp1))) break;
		//Modified Cybage AJK 17/12/2001 DD/MM/YYYY ]
	}
	ic_setlocale();

    return (fi1 && !*fcp1);
}
// ]-EBATECH
