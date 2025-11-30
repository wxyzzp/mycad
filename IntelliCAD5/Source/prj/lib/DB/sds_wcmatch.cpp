/* D:\ICADDEV\PRJ\LIB\DB\SDS_WCMATCH.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

#include "db.h"/*DNT*/

static
char *SDS_wcstrtok(char *strToken,const char *strDelimit) {
    static BOOL  bDoneflag;
    static char *fcp1,
                *fcp2;

    if (!strToken && fcp1) {
        if (bDoneflag) return(NULL);
        fcp1=fcp2+1;
    } else {
        if (!strToken || !strDelimit) return(NULL);
		//Modified SAU 20/04/00 
		// (!*strToken || !*strDelimit) return(NULL);
        if (!*strDelimit) return(NULL);
        bDoneflag=FALSE;
        fcp1=strToken;
    }
// Modified Cybage PP 16/02/2001 [
// Reason: Fix for bug reported in Sander's mail dated 09/02/2001
start : 
	if (NULL!=(fcp2=strchr(fcp1,','/*DNT*/))) {
		if (!(fcp1-fcp2))
		{
			fcp1++ ;
			goto start;
		}
		while ( strlen(fcp2) && ( *(fcp2-1) =='`'/*DNT*/) ){			
			fcp2=fcp2+1;
			if (NULL!=(fcp2=strchr(fcp2,','/*DNT*/)))//MHB;
			{
				if ( strlen( fcp2 ) == 0)
					break;
			}
			else
				break;
		}
// Modified Cybage PP 16/02/2001 ]
        if (fcp2) {
            if (!(fcp2+1)) return(NULL);
            *fcp2=0;
        } else bDoneflag=TRUE;
    } else bDoneflag=TRUE;

    return(fcp1);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function SDS_WCMATCH
//
//  This function does something cool.
//
//  #       (pound) Mathces any numeric digit.
//  @       (at) Matches any alpha  character.
//  .       (period) Matches any nonalphanumeric character.
//  *       (asterisk) Matches any string, including the null string. It can be used anywhere in the search pattern:
//          at the beginnniong, middle, or end of the string.        
//  ?       (question mark) Matches any single character.
//  ~       (tilde) Matches any but the pattern.
//  [...]   Matches any one of the characters enclosed.
//  [~...]  Matches any character not enclosed.
//  -       (hyphen) Is used inside brackets to specify a range for a single character.
//  `       (apostrophe) Escapes special characters (reads next character literally).
//
//  returns : RTNORM if the character string pass the pattern(s) passed.
//          : RTERROR if not.
//
//	/*D.Gavrilov*/// I've added a recursive call of this function to fix the following bug (BRX 1085):
//	it doesn't match the strings "abcdeXfgX" & "*X" (or analogous) because it doesn't suppose that
//	the 2nd but not the 1st(!) 'X' in the Compare string matches to the 'X' of the Pattern string.
//	TODO: make it nonrecursive.
//
// *****************************************
// This is an SDS External API
//
int sds_wcmatch(const char *szCompareThis, const char *szToThis) {
	int   ret=RTERROR,       // Return code.
          iPindex,           // Pattern string index.
          iPlength,          // Pattern string length.
          iCindex,           // Character string index.
          iClength,          // Character string length
          iLastiPindex,      // The last saved index for the pattern string, This string is initaly set to zero
                             // and set every time an astris is hit.
          iBindex,           // The character after the opening bracket.
          iEindex,           // The character before the closeing bracket.
          iTindex;           // This is a temporary index.
    char *pat,               // The pattern string.
         *fcp1;              // A pointer to a single pattern.
    BOOL  bPatflag,          // The pattern flag is set when a pattern has matched.
          bLAstrisflg,       // This is set when the previous pattern character is an astris.
          bNegateflg,        // This is set when the negation symbol is read.
          bNegateBracketflg, // This is set when the negation symbol is read in a pair of brackets.
          bPAstrisflg,       // This is set when an astris had been read for the pattern string.
          bCompareflg;       // The character string.


	if(szCompareThis==NULL || szToThis==NULL) return(ret);

	pat = NULL;
	db_astrncpy(&pat, szToThis, -1);
    if(!pat)// VT. 26-3-02. return error if nomem.
        return ret;
	fcp1=SDS_wcstrtok(pat,","/*DNT*/);
    iClength=strlen(szCompareThis);

    // VT. 26-3-02. Handle szCompareThis = "" - initialize variables
    iCindex = 0;
    iLastiPindex = 1;

    // Loop through all the pattern strings.
	while(fcp1) {
        iPlength=strlen(fcp1);
        bLAstrisflg=bPAstrisflg=FALSE;
        bNegateflg=0;

        // If a character string with zero length is sent through, set to see if the pattern string is anything 
        // other then, astris if it is then return false if not return true.
        if (!iClength) {
            bPatflag=TRUE;
            for (iPindex=0;iPindex<iPlength;++iPindex) 
				{
				if (fcp1[iPindex]!='*'/*DNT*/) 
					bPatflag=FALSE; 
//				else 
//					{ 
//					bPatflag=TRUE; 
//					++iPindex; 
//					}
				}
        } else {
            // Go through each character in the patern, and compare to the string being compared too. to put it shortly.
            for(bPatflag=TRUE,iEindex=iLastiPindex=iCindex=iPindex=0;iPindex<iPlength && iCindex<iClength;) {
				//2000-6-28 EBATECH(FUTA) for MBCS String aware [
				if ((_MBC_LEAD ==_mbbtype((unsigned char)fcp1[iPindex],0)) &&
				    (_MBC_TRAIL==_mbbtype((unsigned char)fcp1[iPindex+1],1)) ){
					if (fcp1[iPindex]==szCompareThis[iCindex]) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
					else if (!bLAstrisflg) {
						iPindex=iLastiPindex;
						if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
					} else { ++iCindex; continue; }
					if (!bPatflag) break;

					if (fcp1[iPindex]==szCompareThis[iCindex]) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
					else if (!bLAstrisflg) {
						iPindex=iLastiPindex;
						if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
					} else { ++iCindex; continue; }
					if (!bPatflag) break;
					continue;
				}
				//2000-6-28 EBATECH(FUTA) ]
                // If the pattern index is the first character.
                if (!iPindex) {
                    // If the first character is a tilde then set the negation flag to TRUE and increment the 
                    // pattern index otherwise set the negation flag to FALSE.
                    if (*fcp1=='~'/*DNT*/) {
                        bNegateflg=TRUE;
                        ++iPindex;
                    } else bNegateflg=FALSE;
                }
                switch(fcp1[iPindex]) {
                    case '*'/*DNT*/:
                        // If the pattern is only an astris then any character string will match.
                        if (iPlength==1) { delete [] pat; return(RTNORM); }
                        bPAstrisflg=bLAstrisflg=TRUE;
                        ++iPindex;
                        iLastiPindex=iPindex;
                        break;
                    case '#'/*DNT*/:
                        if (isdigit((unsigned char) szCompareThis[iCindex])) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; } 
                        else if (!bLAstrisflg) {
                            iPindex=iLastiPindex;
                            if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                        } else { ++iCindex; continue; }
                        break;
                    case '@'/*DNT*/:
                        if (isalpha((unsigned char) szCompareThis[iCindex])) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; } 
                        else if (!bLAstrisflg) {
                            iPindex=iLastiPindex;
                            if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                        } else { ++iCindex; continue; }
                        break;
                    case '.'/*DNT*/:
                        if (!isalnum((unsigned char) szCompareThis[iCindex])) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
                        else if (!bLAstrisflg) {
                            iPindex=iLastiPindex;
                            if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                        } else { ++iCindex; continue; }
                        break;
                    case '?'/*DNT*/:
                        bPatflag=TRUE; ++iCindex; ++iPindex;
                        break;
                    case '`'/*DNT*/:
                        ++iPindex;
                        if (iPindex>=iPlength) { bPatflag=FALSE; break; }
                        if (fcp1[iPindex]==szCompareThis[iCindex]) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
                        else if (!bLAstrisflg) {
                            iPindex=iLastiPindex;
                            if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
				        //Modified SAU 22/04/00                
						//} else { ++iCindex;continue; }
                        } else { ++iCindex;--iPindex; continue; }
                        break;
                    case '['/*DNT*/:
                        bNegateBracketflg=FALSE;
                        // Set the begining index to the character after the first braket.
                        iBindex=iPindex+1;
                        if (iBindex>=iPlength) { bPatflag=FALSE; break; }
                        // Go to the closeing braket, if none stop at end of string.
                        // The closing bracket character ("]") is read literally if it is the first bracketed 
                        // character or follows a leading tilde (as in "[]ABC]" or "[~]ABC]").
                        for(iEindex=iBindex;fcp1[iEindex];++iEindex) {
                            if (fcp1[iEindex]==']'/*DNT*/) {
                                if (iEindex==iBindex) continue;
                                if (iEindex==(iBindex+1) && fcp1[iBindex]=='~'/*DNT*/) continue;
                                break;
                            }
                        }
                        // If the iEindex is greater then iPlength then there was no closeing bracket. no good.
                        if (iEindex>=iPlength) { bPatflag=FALSE; break; }
                        iPindex=iEindex;
                        if (iEindex) --iEindex;
                        break;
                    case ']'/*DNT*/:
                        // If iEindex has not be set then there was not an open bracket, so treat the close bracket as a literal.
                        if (!iEindex) {
                            if (fcp1[iPindex]==szCompareThis[iCindex]) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
                            else if (!bLAstrisflg) {
                                iPindex=iLastiPindex;
                                if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                            } else { ++iCindex; continue; }
                        } else {
                            bCompareflg=FALSE;
                            iTindex=iBindex;
                            // If a tilde is the first character in the brackets then set the negation flag.
                            if (fcp1[iBindex]=='~'/*DNT*/) { bNegateBracketflg=TRUE; ++iBindex; }
                            // loop through the characters in between the brackets.
                            while(iBindex<=iEindex) { 
                                // there is a hyphen, and the hyphen is not at the beginning or end, then check if character is within the limits.
                                if (fcp1[iBindex]=='-'/*DNT*/ && iBindex!=iTindex && iBindex!=iEindex) {
                                    if (fcp1[iBindex-1]>fcp1[iBindex+1]) {
                                        if (szCompareThis[iCindex]<fcp1[iBindex-1] && szCompareThis[iCindex]>fcp1[iBindex+1]) bCompareflg=TRUE;
                                    } else {
                                        if (szCompareThis[iCindex]>fcp1[iBindex-1] && szCompareThis[iCindex]<fcp1[iBindex+1]) bCompareflg=TRUE;
                                    }
                                    if (bCompareflg) break; else ++iBindex;
                                } else if (fcp1[iBindex]==szCompareThis[iCindex]) break; else ++iBindex; 
                            }
                            if (bNegateBracketflg) {
                                // This part was added to do the negation stuff for the bracket function.
                                if (iBindex>iEindex) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
                                else if (!bLAstrisflg) {
                                    iPindex=iLastiPindex;
                                    if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                                } else { ++iCindex; continue; }
                            } else {
                                if (iBindex<=iEindex) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
                                else if (!bLAstrisflg) {
                                    iPindex=iLastiPindex;
                                    if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                                    //Modified SAU 22/04/00                            
									//} else { ++iCindex; continue; }
									} else { ++iCindex;iBindex=iTindex; continue; }
                            }
                        }
                        break;
                    default:
                        if (fcp1[iPindex]==szCompareThis[iCindex]) { bLAstrisflg=FALSE; bPatflag=TRUE; ++iCindex; ++iPindex; }
                        else if (!bLAstrisflg) {
                            iPindex=iLastiPindex;
                            if (bPAstrisflg) ++iCindex; else bPatflag=FALSE;
                        } else { ++iCindex; continue; }
                        break;
                }
                // If the "bPatflag" is set to false then the current pattern being matched did not pass.
                if (!bPatflag) break;
            }
        }
        // If we hit the end of the pattern but not the compare string. and the last item in the pattern was not an astris then no good.
        if (iCindex<iClength && !bLAstrisflg)	/*D.Gavrilov*/// We'll go to recursion if we've had an '*'.
			if(!bPAstrisflg || sds_wcmatch(szCompareThis + iCindex, fcp1 + iLastiPindex - 1) != RTNORM)
				bPatflag=FALSE;
        // If we hit the end of the string but not the pattern, then no good.
        if (iPindex<iPlength) {
            for (;iPindex<iPlength;++iPindex) if (fcp1[iPindex]!='*'/*DNT*/) { bPatflag=FALSE; break; }
        }
        // If the tilde is the first character in the pattern list then negate the pattern match;
        if (bNegateflg) bPatflag=(!bPatflag)?TRUE:FALSE;
        // If "bPatflag" is set to true at the end of the for loop then the pattern matched so break;
        if (bPatflag) ret=RTNORM;
        // Go to the next pattern.
		fcp1=SDS_wcstrtok(NULL,","/*DNT*/);
	}
	delete [] pat;
    return(ret);
}


