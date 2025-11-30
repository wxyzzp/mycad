#ifndef _DWGCNVTDEF_H
#define _DWGCNVTDEF_H

//set up constant for code which differs between icad and visio dwgcnvt
#ifdef DWGCNVT
	const int DWGCNVTLIB = 1;
#else
	const int DWGCNVTLIB = 0;
#endif

#endif

