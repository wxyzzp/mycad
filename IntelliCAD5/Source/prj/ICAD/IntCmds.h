#ifndef __IntCmdsH__
#define __IntCmdsH__

#define SDS_CMDHIDE   1
#define SDS_CMDACAD   2
#define SDS_CMDBOTH   SDS_CMDHIDE|SDS_CMDACAD

struct SDS_InternalCmdList 
{
	char* globalName;
	void* funcPtr;
	short cmdFlags; // See SDS_CMDxxx above
	char* localName;
	int   cmdIndex;
};

extern struct SDS_InternalCmdList SDS_pInternalCmdList[];

int SDS_loadfuncs();
int SDS_Unloadfuncs();

#endif
