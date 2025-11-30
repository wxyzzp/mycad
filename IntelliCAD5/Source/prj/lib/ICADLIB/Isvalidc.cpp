#include "icadlib.h"

short ic_isvalidcol(char *col) {
    int fi1;
    char tmpColstr[20], *color = NULL;
    _tcscpy(tmpColstr, col);
    color = tmpColstr[0] == '_'/*DNT*/ ? &tmpColstr[1] : tmpColstr;

    _tcsupr(col);
    if((strisame(color,"BYLAYER"/*DNT*/))||
       (strisame(color,"BYBLOCK"/*DNT*/))||
       (strisame(color,"RED"/*DNT*/))||
       (strisame(color,"YELLOW"/*DNT*/))||
       (strisame(color,"GREEN"/*DNT*/))||
       (strisame(color,"CYAN"/*DNT*/))||
       (strisame(color,"BLUE"/*DNT*/))||
       (strisame(color,"MAGENTA"/*DNT*/))||
       (strisame(color,"WHITE"/*DNT*/))) {
        return(1);
    }
    if(*color>='0'/*DNT*/ && *color<='9'/*DNT*/){
        fi1=atoi(color);
        if(fi1>=0 && fi1<=256) return(1);
    }
    return(0);
}
