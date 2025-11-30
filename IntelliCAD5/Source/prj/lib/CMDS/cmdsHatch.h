#ifndef __cmdsHatchH__
#define __cmdsHatchH__

bool cmd_SetHatchFromDef(int numlines, cmd_hpdefll *currentDef, db_hatch *hatch);
bool cmd_FindPatternDef(CString patName, int *numlines, cmd_hpdefll **patternDef);
bool cmd_ApplyHatchToBoundryObjects(HanditemCollection *collection, db_hatch *hatch);

#endif
