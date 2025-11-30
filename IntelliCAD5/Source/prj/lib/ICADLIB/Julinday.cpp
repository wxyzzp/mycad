#include "icadlib.h"/*DNT*/

long ic_julianday(long mm, long dd, long yy) {
  return(367*yy-7*(yy+(mm+9)/12)/4-3*((yy+(mm-9)/7)/100+1)/4+275*mm/9+dd+1721029);
}
