#include "icadlib.h"/*DNT*/

long ic_mkprimes(long through, unsigned char **pfbufpp) {
/*
**  Allocs an unsigned char array of "prime flags".
**  THE CALLER MUST FREE *pfbufpp IF THIS FUNCTION SETS IT TO NON-NULL!
**  Param "through" is the highest number you want to flag.
**
**  A "prime flags" array of unsigned chars uses bits to flag whether a
**    given number is prime or not.  For example, the first two bytes in
**    the file are 172 and 40
**
**       Byte               0         1
**       Bit positions  76543210  76543210
**       Bit values     10101100  00101000
**
**    showing that 2, 3, 5, 7, 11, and 13 are prime, while the others are
**    not. In general, if you want to know if T is prime, look at byte
**    T/8, bit T%8.
**
**  The calculations to create this array may take awhile if you choose
**    a large "through".
**
**  This function is used to set up a prime-flags array for ic_primefac()
**    and ic_gcd().  The following will allocate a prime-flags array
**    just big enough to prime factor the largest signed long (making
**    it perfect for ic_primefac() and ic_gcd()):
**
**      unsigned char *pfbuf;
**      long pfbufsz;
**      pfbufsz=ic_mkprimes(46341L,&pfbuf);
**
**    (Remember the caller has to free it!)
**    Should alloc 5793 bytes.
**
**  Returns:
**      -1 : Calling error (pfbufpp==NULL).
**      -2 : No memory.
**     >-1 : Size of the allocated array that *pfbufpp points to.
*/
    unsigned char *buf;
    short c4,cc4,do2224,ok;
    long bufsz,test,max,tryit;
    unsigned char mask[]= { 1,2,4,8,16,32,64,128 };

    buf=NULL;

    if (pfbufpp==NULL) return -1;
    *pfbufpp=NULL;

    if (through<0L) through=0L;

    bufsz=through/8L+1L; through=bufsz*8L-1L;
    if ((buf= new unsigned char [bufsz] )==NULL) return -2;
	memset(buf,0,sizeof(char)*bufsz);

    *buf=172;  /* Seed it with 10101100 */

    test=7L; c4=0;
    while (test<=through) {
        max=(long)sqrt(((double)test)+0.1);
        tryit=3L; cc4=0; do2224=0; ok=1;
        while (tryit<=max) {
            if ((buf[tryit/8L]&mask[tryit%8L]) && !(test%tryit)) { ok=0; break; }
            if (do2224) { tryit+=(cc4==3) ? 4L : 2L; if (++cc4>3) cc4=0;
            } else if (++tryit==7) do2224=1;
        }
        if (ok) buf[test/8L]|=mask[test%8L];
        test+=(c4==3) ? 4L : 2L; if (++c4>3) c4=0;
    }

    *pfbufpp=buf; return bufsz;
}
