/* C:\ICAD\PRJ\LIB\DB\ENTITYWITHEXTRUSION.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// **************************************
// class db_entity_with_extrusion
// Many entities share the 210 code for extrustion direction or ECS (entity coordinate system)
// or normal
//
//
#ifndef __entitywithextrusionH__
#define __entitywithextrusionH__

#include "..\..\icad\IcadUtil.h"

  
class DB_CLASS db_entity_with_extrusion : public db_entity
	{
    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

	// *****************
	// CTORS / DTORS
public:
    db_entity_with_extrusion(int ptype, db_drawing *dp=NULL) :
		db_entity( ptype, dp )
		{
		m_Extrusion = NULL;
		}

	db_entity_with_extrusion(const db_entity_with_extrusion &sour) :
		db_entity( sour )
		{
		m_Extrusion=NULL;  /* Don't use db_aextrucpy() here; wastes time unitizing. */
		if (sour.m_Extrusion!=NULL) 
			{ 
			m_Extrusion=new sds_real[3]; 
			DB_PTCPY(m_Extrusion,sour.m_Extrusion); 
			}
		}

	virtual ~db_entity_with_extrusion(void)
		{
		if ( m_Extrusion != NULL )
			{
			delete [] m_Extrusion;
			m_Extrusion = NULL;
			}
		}

	// ****************
	// ACCESSORS
	// overrides of virtuals in db_handitem and db_entity
	//
public:

    bool get_210(sds_point p) 
		{
        if (m_Extrusion==NULL) 
			{ 
			p[0]=p[1]=0.0; p[2]=1.0; 
			}
        else 
			{ p[0]=m_Extrusion[0]; p[1]=m_Extrusion[1]; p[2]=m_Extrusion[2]; 
			}
        return true;
	    }
    bool set_210(sds_point p) 
		{ 
		db_aextrucpy(&m_Extrusion,p); 
		return true; 
		}
	// This version is faster than set_210 because it does not use db_aextrucpy.  Use if you
	// know the vector is already normalized
	//
    bool init_210(sds_point newvalue) 
		{
			if ( m_Extrusion != NULL )
				{
				delete [] m_Extrusion;
				}
			// If we are passing in (0,0,1) 
			// m_Extrusion == NULL signifies (0,0,1)
			if (icadRealEqual(newvalue[2],1.0))
				{
				m_Extrusion = NULL;
				}
			else
				{ 
				m_Extrusion=new sds_real[3]; 
				DB_PTCPY(m_Extrusion,newvalue); 
				}
			return true;
		}

private:

    sds_real  *m_Extrusion;   /* 210 */

	};

#endif

