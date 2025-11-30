/* C:\ICAD\PRJ\LIB\DB\LAYERTABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "layertabrec.h"

/************************** db_layertabrec **************************/

/*DG - 11.6.2002*/// It's needed for dxf290 of layers which is supported since dxf 2000.
char *db_layertabrec::defpoints_name = "DEFPOINTS";

/*F*/
db_layertabrec::db_layertabrec(void) :
    db_tablerecord(DB_LAYERTABREC,db_str_quotequote) {
    ltypehip=NULL;
	color=7;
	xrefblkhip=NULL;
/*DG - 11.6.2002*/// dxf290 and dxf370 of layers are supported since dxf 2000.
	lineweight=DB_DFWEIGHT;
	plottable=1;
	plotstylehip=NULL;
	}

db_layertabrec::db_layertabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_LAYERTABREC,pname) {
    ltypehip= ((dp!=NULL) ? dp->findtabrec(DB_LTYPETAB,db_str_continuous,1) : NULL);
    color=7;
	xrefblkhip=NULL;
/*DG - 11.6.2002*/// dxf290 and dxf370 of layers are supported since dxf 2000.
	lineweight=DB_DFWEIGHT;
	plottable= (strisame(pname,defpoints_name) ? 0 : 1);
	plotstylehip= db_ret_default_plotstylehip(dp);
	}
db_layertabrec::db_layertabrec(const db_layertabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */
    ltypehip=sour.ltypehip;
	color=sour.color;
	xrefblkhip=sour.xrefblkhip;
/*DG - 11.6.2002*/// dxf290 and dxf370 of layers are supported since dxf 2000.
	lineweight=sour.lineweight;
	plottable=sour.plottable;
	plotstylehip=sour.plotstylehip;
	}

/*DG - 11.6.2002*/// dxf290 of layers is supported since dxf 2000.
bool db_layertabrec::set_2(char* pName)
{
	if(strisame(pName,defpoints_name))
	{
		plottable = 0;
	}
	return 	db_tablerecord::set_2(pName);
}

bool db_layertabrec::set_290(int p)
{
	char* pName= db_tablerecord::retp_name();
	if(pName && stricmp(pName,defpoints_name))
	{
		plottable = ( p != 0 );
	}
	return true;
}

// ********************************************************************
// VPLAYER methods
//

DB_API
bool
db_layertabrec::isVpLayerFrozenByDefault( void )
	{
	int iFlags;
	this->get_70( &iFlags );

	return ((iFlags & IC_LAYER_VPFROZEN) != 0);
	}

DB_API
void
db_layertabrec::setVpLayerFrozenByDefault( bool bFrozen )
	{
	int iFlags;
	this->get_70( &iFlags );

	if ( bFrozen )
		{
		iFlags |= IC_LAYER_VPFROZEN;
		}
	else
		{
		iFlags &= ~IC_LAYER_VPFROZEN;
		}

	this->set_70( iFlags );
	}
