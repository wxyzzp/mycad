#include "icadlib.h"/*DNT*/

void ic_rpad(char *ss, short len) {   /* pad str on the right with spaces */
  int fi1;

  if (len<1) return;
  fi1=strlen(ss)-1; while (++fi1<len) ss[fi1]=' '/*DNT*/; ss[len]=0;
}
