
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

char *ic_endquote(const char *str, short nchars) {
/*
**  Locates the true terminal quote of a string literal, ignoring
**  backslash-escaped quotes.  Pointer str must point to the 1st char
**  AFTER the initial quote that starts a string literal.  Short nchars
**  should specify the max number of chars to check before giving up the
**  search (if the end of the string is not found first).  Use a
**  negative nchars (like -1) if you want it to continue looking until the
**  end of the string.
**
**  Returns a pointer to the terminating quote, or NULL if none was
**  found within nchars or before the end of the string was encountered.
**
**  Here's an example where the string literal is inside a bigger
**  string (like an expression to be parsed):
**
**      ..."String literal\nwith \"escape\" codes." )...
**          |                                     |
**          str                                   returned pointer
*/
    short idx;

    if (str==NULL) return NULL;

    for (idx=0; (nchars<0 || idx<nchars) && *str; idx++,str++) {
        if (*str=='\"'/*DNT*/) return (char *)str;
		//2000-06-18 EBATECH(FUTA) for MBCS String [
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*str,0)) &&
			  (_MBC_TRAIL==_mbbtype((unsigned char)*(str+1),1))){
			idx++,str++;
			continue;
		}
		//2000-06-18 EBATECH(FUTA) ]
        if (*str!='\\'/*DNT*/) continue;  /* Don't use IC_SLASH! */
        /* Jump over escaped char (should work for octal, too). */
        /* (Needs its own end-check before the "for" steps again.): */
        str++; idx++; if (!((nchars<0 || idx<nchars) && *str)) break;
		// Restore from comment 2000-06-25 EBATECH(ARA) Some Lisp is failed.

		//2000-06-25 EBATECH(ARA) for MBCS String [
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*str,0)) &&
			  (_MBC_TRAIL==_mbbtype((unsigned char)*(str+1),1))){
			idx++,str++;
		}
		//2000-06-25 EBATECH(ARA) ]
    }
    return NULL;
}
