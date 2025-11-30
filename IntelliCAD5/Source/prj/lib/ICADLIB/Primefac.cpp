#include "icadlib.h"/*DNT*/

short ic_primefac(long num, unsigned char *pfbuf, long pfbufsz, long *pfa) {
/*
**  Prime factors num and puts the results in the pfa array.
**      num<2L is invalid and returns 0.
**      1 is never included in pfa.
**      If num is prime, num is placed in pfa[0] and 1 is returned.
**  The pfa array of longs must have enough elements to hold the
**      largest possible number of prime factors (the base-2 log of num).
**      For example, 31 is enough for any signed long.
**  pfbuf must point to a "prime flags" array, such as that prepared
**      by ic_mkprimes().
**  pfbufsz must give the size of the "prime flags" array (in bytes)
**      (which is returned by ic_mkprimes()).
**
**  The following will allocate a prime-flags array just big enough
**      to prime factor the largest signed long (making it perfect
**      for ic_primefac() and ic_gcd()):
**
**          unsigned char *pfbuf;
**          long pfbufsz;
**          pfbufsz=ic_mkprimes(46341L,&pfbuf);
**
**      (Remember the caller has to free it!)
**      Should alloc 5793 bytes.
**
**  Returns:
**      -1 : pfbuf wasn't long enough to factor num (see above).
**       0 : A calling error.
**       1 : num is prime (pfa[0] is set to num).
**      >1 : The number of prime factors stored in pfa.
*/
    short npfs,do2224,c4,worked;
    long tryit,max,byt;
    unsigned char mask[]= { 1,2,4,8,16,32,64,128 };


    npfs=0;

    if (num<1L || pfbuf==NULL || pfbufsz<1L || pfa==NULL) goto out;

    /* Never have to try greater than sqrt: */
    max=(long)sqrt(((double)num)+0.1);

    tryit=2L; byt=tryit/8L; do2224=0; c4=0;
    for (;;) {
        /* See if tryit has passed the max: */
        if (tryit>max) {
            if (num>1L) pfa[npfs++]=num;  /* Take whatever's left */
                                          /* (unless it's 1). */
            break;
        }

        /* Have we walked beyond the end of the primes table? */
        if (byt>=pfbufsz) { npfs=-1; goto out; }

        /* Take "tryit" as many times as it works: */
        do {
            worked=0;
            if (!(num%tryit)) {
                worked=1;
                pfa[npfs++]=tryit;
                num/=tryit;  /* Set num to quotient (what's left to factor) */
                max=(long)sqrt(((double)num)+0.1);  /* Set new max */
            }
        } while (worked);

        /* Step "try" to next prime: */
        do {
            if (do2224) { tryit+=(c4==3) ? 4L : 2L; if (++c4>3) c4=0; }
            else if (++tryit==7) do2224=1;
            byt=tryit/8L;
        } while (byt<pfbufsz && !(pfbuf[byt]&mask[tryit%8L]));
    }

out:
    return npfs;
}
