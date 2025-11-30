#include "afxtempl.h"
//4M Item:25->
class CSdsName:public CObject{
private:
   sds_name ename;
public:
   CSdsName(){ename[0]=0;ename[1]=0;}
   CSdsName(const CSdsName& n){ic_encpy(ename,n.ename);}
   CSdsName(sds_name n){ic_encpy(ename,n);}
   CSdsName(const sds_name n){ic_encpy(ename,n);}
   CSdsName& operator=(const CSdsName& n){ic_encpy(ename,n.ename);return *this;}
   void Get(sds_name n){ic_encpy(n,ename);}
   BOOL AFXAPI operator==(const CSdsName& n) const{return (ename[0]==n.ename[0])&&(ename[1]==n.ename[1]);}
   friend UINT AFXAPI HashKey(CSdsName& key);
};

typedef CMap<CSdsName, CSdsName&, CSdsName, CSdsName&> CSdsNameMap;
//<-4M Item:25
