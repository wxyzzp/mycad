/* D:\ICADDEV\PRJ\LIB\CMDS\PASTE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "CloneHelper.h"
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"

BOOL cmd_PasteItem(void *memptr,db_drawing *To_dbDrawing) {
    long         totalcnt,
                 index;
    db_handitem *handItem;
    db_drawing  *dbDrawing;
    long         offptr = 0L;

	cmd_GaveACISMsg=FALSE;
	cmd_GaveProxyMsg=FALSE;
	cmd_GaveA2KEntMsg=FALSE;

	if(SDS_CURDOC) sds_ssfree(SDS_CURDOC->m_pPrevSelection);

	offptr+=sizeof(long); //*** Step over the hInstance
    totalcnt=(*(long *)(((char *)memptr)+offptr)); offptr+=sizeof(totalcnt);
    dbDrawing=(db_drawing  *) (*(long *)(((char *)memptr)+offptr)); 
	offptr+=sizeof(long); //*** Step over the drawing
	offptr+=sizeof(long); //*** Step over the Process id

		// track the 1005 handles in eed so that they
		// can be fixed up to be in sync later
	To_dbDrawing->Make1005FixupTables();

	sds_name ssPasteFrom, ssPasteTo, ename;
	ssPasteFrom[0]=ssPasteFrom[1]=ssPasteTo[0]=ssPasteTo[1]=0L;
    ename[1] = (long)dbDrawing;
	for( index=0;index<totalcnt;++index )
	{
        handItem =(db_handitem *) (*(long *)(((char *)memptr)+offptr)); offptr+=sizeof(long);
		ename[0] = (long)handItem;
		sds_ssadd( ename, ssPasteFrom, ssPasteFrom );
        cmd_ItemInsert(dbDrawing,To_dbDrawing,handItem,NULL);
	}

	if( SDS_CURDOC )
		SDS_sscpy( ssPasteTo, SDS_CURDOC->m_pPrevSelection );

	// Send notification with ss name of entities to be pasted
	SDS_CallUserCallbackFunc(SDS_CBENDCLONE,(void *)ssPasteFrom,ssPasteTo,(void*)0);
	sds_ssfree( ssPasteFrom );
	sds_ssfree( ssPasteTo );

	// make sure the 1005 handles are in sync in eed
	if (To_dbDrawing->FixingUp1005s())
		{
		To_dbDrawing->Apply1005Fixups();
		To_dbDrawing->Delete1005FixupTables();
		}

    return(TRUE);
}





