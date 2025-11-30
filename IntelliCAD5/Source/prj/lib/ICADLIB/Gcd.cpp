
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/

long ic_gcd(long n1, long n2, unsigned char *pfbuf, long pfbufsz) {
/*
**  Finds the Greatest Common Denominator (or Divisor) of n1 and n2.
**  Both numbers must be > 0L.
**
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
**      >0 : The GCD.
*/
    short npf,fi1;
    long pfa[32],res;

    if (n1< 1L || n2< 1L) return 0L;
    if (n1==1L || n2==1L) return 1L;

    /* Since we only have to prime factor n1, make it the smaller number: */
    if (n1>n2) { n1^=n2; n2^=n1; n1^=n2; }

    if ((npf=ic_primefac(n1,pfbuf,pfbufsz,pfa))<1) return (long)npf;

    /* Walk n1's prime factors and collect all that divide n2. */
    /* (When one DOES divide n2, remove it from n2.): */
    res=1L;
    for (fi1=0; fi1<32 && fi1<npf; fi1++) {
        if (n2%pfa[fi1]) continue;
        res*=pfa[fi1]; n2/=pfa[fi1];
    }

    return res;
}
