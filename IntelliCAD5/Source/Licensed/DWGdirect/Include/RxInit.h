#ifndef _RxInit_h_Included_
#define _RxInit_h_Included_

class OdDbSystemServices;

//DD:EXPORT_ON

FIRSTDLL_EXPORT void rxInitialize(OdDbSystemServices* pSystemServices);

FIRSTDLL_EXPORT void rxUnInitialize();

//DD:EXPORT_OFF

#endif
