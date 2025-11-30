
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

short ic_escconv(char *sour, char *dest, char escflag, LPCTSTR conv,
    short special, short mode) {
/*
**  This function deals with strings that use escape sequences to
**  represent special characters.
**
**  sour and dest:
**
**      String sour is copied to dest, converting in the process. Note
**      that when converting from true chars to escape sequences, the
**      string gets BIGGER.  Make sure dest can handle this.
**
**      It's an error for sour to be NULL, but if dest is NULL, the
**      function does everything EXCEPT write to dest -- so it returns
**      how long dest NEEDS TO BE.
**
**  escflag:
**
**      This char must specify the escape flag char to use (usually
**      backslash).
**
**  conv:
**
**      The string conv must consist of pairs of characters:
**          [code][val][code][val]...
**      where code is the escape sequence code char (like 'n' for
**      line-feed) and val is the true char val it represents
**      (like ASCII char 10 ('\n') for line-feed).  For example, to
**      convert quote, backslash, and line-feed, conv would be
**
**          "\"\"\\\\n\n"
**
**      That's [quote][quote][backslash][backslash][n][newline].
**
**  special:
**
**      Bit-mapped to specify special options that can't be handled via
**      conv, such as octal and hex escape sequences. Here are the
**      current options (which can be expanded in the future):
**
**          Bit 0: Flags whether or not to convert using octal escape
**                   sequences [escflag]ooo where the o's represent
**                   octal digits.
**
**          Bit 1: Flags whether or not to convert using hexadecimal
**                   escape sequences [escflag]xHH where the H's
**                   represent hex digits.
**
**  mode:
**
**      Conversion mode:
**
**          0 : Escape sequences to true chars
**
**              For octal, conversion stops after the 3rd octal digit,
**              or with the 1st char that is not an octal digit. For
**              hex, conversion stops after the 2nd hex digit, or with
**              the 1st char that is not a hex digit.
**
**              If an escape flag is found that is NOT convertible, that
**              escape flag is ignored (skipped) (and so is the "x" for
**              [escflag]x hex).
**
**          1 : True chars to escape sequences
**
**              For octal and hex, four characters are always produced:
**              [escflag]ooo or [escflag]xHH.
**
**              If "special" specifies both octal and hex, hex is used.
**
**              Chars less than 32 or greater than 126 are converted
**              to octal or hex escape sequences.  (The 126 limit comes
**              from AutoLISP; I would have thought 127 would be more
**              natural.)
**
**  Returns:
**      -1 : Calling error (sour==NULL)
**     >-1 : The length of dest (or the length it needs to be,
**              if dest==NULL)
*/
	/*DG - 28.1.2002*/// I removed the "126" upper limit for single-byte charsets
	// mentioned above because acad2000 hasn't it any more.

    char fc1;
    int clen_1,sidx,cidx,dooct,dohex;
	short didx,slen;

    if (sour==NULL) return -1;

    dooct=special&1; dohex=((special&2)!=0);
    slen=shortCast(strlen(sour)); clen_1=(conv==NULL) ? -1 : (strlen(conv)-1);
    sidx=cidx=didx=0;

    if (mode==0) {  /* Escape sequences to true chars. */

        for (;;) {
            if (sidx>=slen) break;
			//2000-06-14 EBATECH(FUTA) for MBCS String
			if ((_MBC_LEAD ==_mbbtype((unsigned char)sour[sidx],0)) &&
				(_MBC_TRAIL==_mbbtype((unsigned char)sour[sidx+1],1)) ){
				if(dest!=NULL){
					dest[didx]=sour[sidx];
					dest[didx+1]=sour[sidx+1];
				}
				didx+=2;
				sidx+=2;
				continue;
			}
			// } 2000-06-14 EBATECH(FUTA)
            if (sour[sidx]!=escflag) {
                if (dest!=NULL) dest[didx]=sour[sidx];
                didx++; sidx++; continue;
            }
            if (++sidx>=slen) break;
            if (dooct && (sour[sidx]>='0'/*DNT*/ && sour[sidx]<='7'/*DNT*/)) {
                if (dest!=NULL) dest[didx]=charCast(sour[sidx]-'0'/*DNT*/);
                if (++sidx<slen && sour[sidx]>='0'/*DNT*/ && sour[sidx]<='7'/*DNT*/) {
                    if (dest!=NULL) dest[didx]=charCast(8*dest[didx]+sour[sidx]-'0'/*DNT*/);
                    if (++sidx<slen && sour[sidx]>='0'/*DNT*/ && sour[sidx]<='7'/*DNT*/) {
                        if (dest!=NULL) dest[didx]=charCast(8*dest[didx]+sour[sidx]-'0'/*DNT*/);
                        sidx++;
                    }
                }
                didx++;
            } else if (dohex && toupper(sour[sidx])=='X'/*DNT*/) {
                if (++sidx<slen && isxdigit((unsigned char) sour[sidx])) {
                    fc1=(char)toupper(sour[sidx]);
                    if (dest!=NULL) dest[didx]=charCast((fc1>='0'/*DNT*/ && fc1<='9'/*DNT*/) ?
                        fc1-'0'/*DNT*/ : fc1-'A'/*DNT*/+10);
                    if (++sidx<slen && isxdigit((unsigned char) sour[sidx])) {
                        fc1=(char)toupper(sour[sidx]);
                        if (dest!=NULL) dest[didx]=charCast(16*dest[didx]+
                            ((fc1>='0'/*DNT*/ && fc1<='9'/*DNT*/) ? fc1-'0'/*DNT*/ : fc1-'A'/*DNT*/+10));
                        sidx++;
                    }
                    didx++;
                }
            } else if (conv!=NULL) {
                for (cidx=0; cidx<clen_1 && sour[sidx]!=conv[cidx]; cidx+=2);
                if (cidx<clen_1) {
                    if (dest!=NULL) dest[didx]=conv[cidx+1];
                    didx++; sidx++;
                }
            }
        }

    } else if (mode==1) {  /* True chars to escape sequences. */

        for (; sidx<slen; sidx++) {
			//
			//2000-06-14 EBATECH(FUTA) for MBCS String[
			if ((_MBC_LEAD ==_mbbtype((unsigned char)sour[sidx],0)) &&
			   (_MBC_TRAIL==_mbbtype((unsigned char)sour[sidx+1],1)) ){
				if(dest!=NULL){
					dest[didx]=sour[sidx];
					dest[didx+1]=sour[sidx+1];
				}
				didx+=2;
				sidx++;
				continue;
			}
			// ]
			//2000-07-06 EBATECH(FUTA) for 1byte kana string. Many case _ismbbkalnum() return null. [
			if (_ismbbkalnum((unsigned int)sour[sidx])) {
				if (dest!=NULL) dest[didx]=sour[sidx];
				didx++;
				continue;
			}
			// ]
            if (conv!=NULL)
                for (cidx=0; cidx<clen_1 &&
                    sour[sidx]!=conv[cidx+1]; cidx+=2);
            if (conv!=NULL && cidx<clen_1) {
                if (dest!=NULL) dest[didx]=escflag;
                didx++;
                if (dest!=NULL) dest[didx]=conv[cidx];
                didx++;
            } else if ((dooct || dohex) && (/*sour[sidx]<32 || sour[sidx]>126*/ (unsigned char)sour[sidx] < 32)) {	/*DG - 28.1.2002*/// See notes above about "126".
                if (dest!=NULL) sprintf(dest+didx,
                    (dohex) ? "%cx%02X"/*DNT*/ : "%c%03o"/*DNT*/,escflag,(unsigned char)sour[sidx]);
                didx+=4;
            } else {
                if (dest!=NULL) dest[didx]=sour[sidx];
                didx++;
            }
        }

    } else {  /* Invalid mode; just copy. */

        if (dest!=NULL) strcpy(dest,sour);
        didx=slen;

    }

    if (dest!=NULL) dest[didx]=0;

    return didx;
}
