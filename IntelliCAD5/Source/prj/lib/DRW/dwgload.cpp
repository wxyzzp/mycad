/* C:\DEV\LIB\DRW\DRW2.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 *
 */

/*DG - 28.5.2002*/// use ODT versions defines in odtver.h instead of one(s) in configure.h
#include "odtver.h"
/*
//COSMO (ShokuY) ->
// OpenDWG ODT 2.008 adaption switch(BUILD_WITH_ODT2008GLE) added 2002-04-01
#include "..\..\ICAD\Configure.h" // KANJI
//<-COSMO
*/

#include "Db.h"
#include "objects.h"
#include "Drw.h"
#include "drwprotos.h"
#include "dwgcnvtdef.h"
#include "fixuptable.h"
#include "appid.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "ucstabrec.h"
#include "viewtabrec.h"
#include "vporttabrec.h"
#include "viewport.h"
#include "..\..\ICAD\res\icadrc2.h"
#include "DbAcisEntity.h"
#include "DbGroup.h"
#include "DbPlotSettings.h"
#include "DbLayout.h"
#include "DbMText.h"
#include "DbSortentsTable.h"
#include "dwgcodepage.h"
#include "TTFInfoGetter.H"

#include "OdaCommon.h"
#include "DbDatabase.h"

#include "ExSystemServices.h"
#include "ExHostAppServices.h"
#include "CollectionClasses.h"

#include "DWGdirectReader.h"

#include <mbctype.h>

db_handitem *layhipcache;
db_handitem *ltypehipcache;

DB_API db_drawing *drw_readwrite::drw_DWGdirect_Open(db_drawing *argdp, char *p_pn, int *rcp, bool convertplines, short bIsRecovering) 
{
//  Note I have found no way for argdp to come into this function as anything but NULL.
	ASSERT(argdp == NULL);

    db_drawing *dp=new db_drawing(1);
	db_astrncpy(&dp->pn,(p_pn==NULL) ? db_str_quotequote : ((char *)p_pn),-1);
	
	OdRxObjectImpl<DWGdirectReaderServices> exServices;

	DWGdirectReader DWGReader(exServices);
	bool result = DWGReader.readDiskFile(p_pn, dp, bIsRecovering);
#ifdef _DEBUG_MHB 
	// MHB I'm disabling this assert, because it is mainly fired when an xref isn't found.
	// That situation is handled by other code.
	// ASSERT(result == true);
#endif 
	if (result == false)
	{
		delete dp;
		dp = NULL;
		return NULL;
	}

	// Code copied from drw_readwrite::drw_marcompopen()
    if (dp != NULL)
    {
        ASSERT(dp->fixupTable != NULL);
        dp->fixupTable->ApplyFixups(dp);
        delete dp->fixupTable;  // this is also destroyed in the db_drawing destructor, but
        dp->fixupTable = NULL;  // why not get rid of it now and return the space?

        if(m_bCheckOnLoad)
		{
            if(!dp->check())	// currently it can't fail so just assert
			{
                assert(false);
			}
		}
        dp->fixupDicVars(false, dp->ret_FileVersion());
    }

	return(dp);
}

short AddTopLevelXrefBlockIfNecessary(db_drawing *topdp, db_drawing *xrefdp,db_blocktabrec *xrefblkptr,db_blocktabrec **toplevelblockheader)
{
    char fullpath[4096];
    short ret=RTNORM;
    db_handitem *blkhip,*enthip;
    sds_real base[3];

    ASSERT(topdp!=NULL);
    ASSERT(xrefdp!=NULL);
    ASSERT(xrefblkptr!=NULL);

    if ((*toplevelblockheader=(db_blocktabrec *)topdp->findtabrec(DB_BLOCKTAB,xrefblkptr->retp_name(),0))) return RTNORM;  // already there

// construct and add block table entry and entities
    blkhip = new db_blocktabrec(xrefblkptr->retp_name(),topdp);
    blkhip->set_70(xrefblkptr->ret_flags());
    ((db_tablerecord *)blkhip)->set_xrefidx(1);
    topdp->addhanditem(blkhip);

    enthip = new db_block(topdp);
    xrefblkptr->get_1(fullpath,4095);
    ((db_block *)enthip)->set_1(fullpath);
    ((db_block *)enthip)->set_2(xrefblkptr->retp_name());
    xrefblkptr->get_10(base);
    ((db_block *)enthip)->set_10(base);
    enthip->assignhand(topdp);
    blkhip->addent(enthip,topdp);

    enthip = new db_endblk(topdp);
    enthip->assignhand(topdp);
    blkhip->addent(enthip,topdp);
    *toplevelblockheader=(db_blocktabrec *)blkhip;
    return RTNORM;
}


inline bool NameContainsPipeChar(char *str)
    {
    if (strchr(str,'|')!=NULL) return true;
    return false;
    }


short AddXrefAppidIfNecc(db_drawing *topdp, db_appidtabrec *xrefappidhip,db_drawing *xrefdp)
    {
// note -- APPIDs do not get the xref block name prepend
    char toplevelappidname[512];

    db_appidtabrec *appidhip;
    if (strsame(xrefappidhip->retp_name(),"ACAD"/*DNT*/)) return RTNORM; // don't add app "ACAD"
    if (NameContainsPipeChar(xrefappidhip->retp_name())) return RTNORM;  // if this appid is itself from an xref below, don't mess with it
    strcpy(toplevelappidname,xrefappidhip->retp_name());
    //Bugzilla No. 78130 ; 06-05-2002
    //if (appidhip=(db_appidtabrec *)topdp->findtabrec(DB_APPIDTAB,toplevelappidname,0))
    if (appidhip=(db_appidtabrec *)topdp->findtabrec(DB_APPIDTAB,toplevelappidname,1))
        {
        appidhip->set_70(appidhip->ret_flags() | IC_XREF_RESOLVED);
        return RTNORM;  // already in there
        }
    appidhip = new db_appidtabrec(*xrefappidhip);
    appidhip->SetHandle(NULL);  // copy constructor also copies handle; we need a new one.
                                // new one will be added automatically at addhanditem time.
    appidhip->set_2(toplevelappidname);  // prolly redundant since it is the same
    appidhip->set_xrefidx(1);
    appidhip->set_70(appidhip->ret_flags() | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);
    appidhip->RemovePersistentReactors();
    if (topdp->addhanditem(appidhip))
        return RTERROR;
    return RTNORM;
    }


short AddXrefDimstyleIfNecc(db_drawing *topdp, db_dimstyletabrec *xrefdimstylehip,db_blocktabrec *xrefblkptr,
                        db_drawing *xrefdp)
    {
// Load xref dimstyle. -- note, standard DOES get added to the top level
    db_dimstyletabrec *dimstylehip;
    char topleveldimstylename[512],toplevelstylename[512];
    db_styletabrec *stylehip;

    if (NameContainsPipeChar(xrefdimstylehip->retp_name())) return RTNORM;  // if this is itself from an xref below, don't mess with it
    sprintf(topleveldimstylename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),xrefdimstylehip->retp_name());
    //Bugzilla No. 78130 ; 06-05-2002
    //if (dimstylehip=(db_dimstyletabrec *)topdp->findtabrec(DB_DIMSTYLETAB,topleveldimstylename,0))
    if (dimstylehip=(db_dimstyletabrec *)topdp->findtabrec(DB_DIMSTYLETAB,topleveldimstylename,1))
        {
        dimstylehip->set_70(dimstylehip->ret_flags() | IC_XREF_RESOLVED);
        return RTNORM;  // already in there
        }
    dimstylehip = new db_dimstyletabrec(*xrefdimstylehip);
    dimstylehip->SetHandle(NULL);   // copy constructor also copies handle; we need a new one.
                                    // new one will be added automatically at addhanditem time.
    dimstylehip->set_xrefidx(1);
    dimstylehip->set_2(topleveldimstylename);
    dimstylehip->set_70(xrefdimstylehip->ret_flags() | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);

    stylehip=NULL;
    if (xrefdimstylehip->ret_txsty()!=NULL)
        {
        sprintf(toplevelstylename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_dimstyletabrec *)(xrefdimstylehip->ret_txsty()))->retp_name());
        //Bugzilla No. 78130 ; 06-05-2002
        //stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,0);
        stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,1);
        }
    dimstylehip->set_340(stylehip);
    dimstylehip->RemovePersistentReactors();
    if (topdp->addhanditem(dimstylehip))
        return RTERROR;
    return RTNORM;
    }


short AddXrefStyleIfNecc(db_drawing *topdp, db_styletabrec *xrefstylehip,db_blocktabrec *xrefblkptr,
                        db_drawing *xrefdp)
// load xref style.
    {
    db_styletabrec *stylehip;
    char toplevelstylename[512];

    if (strlen(((db_styletabrec *)xrefstylehip)->retp_name())==0 &&
        (((db_styletabrec *)xrefstylehip)->ret_flags() & IC_SHAPEFILE_SHAPES))  // shape file-type styles have no name, so construct a fake one
        {
        strcpy(toplevelstylename,"SHAPE|REF"/*DNT*/);
// start at the first one with that name, check any others also
// see if any reference the same font.  If so, just point to it.
        //Bugzilla No. 78130 ; 06-05-2002
        //for (stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,0); stylehip!=NULL; stylehip=(db_styletabrec *)stylehip->next)
        for (stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,1); stylehip!=NULL; stylehip=(db_styletabrec *)stylehip->next)
            {
            if (strsame(toplevelstylename,stylehip->retp_name()) &&
                strsame(stylehip->retp_fontpn(),((db_styletabrec *)xrefstylehip)->retp_fontpn()))
                {
                stylehip->set_70(stylehip->ret_flags() | IC_XREF_RESOLVED);
                return RTNORM;
                }
            }
        }
    else
        {
        sprintf(toplevelstylename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_styletabrec *)xrefstylehip)->retp_name());
        //Bugzilla No. 78130 ; 06-05-2002
        //if ((stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,0))!=NULL)
        if ((stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,1))!=NULL)
            {
            stylehip->set_70(stylehip->ret_flags() | IC_XREF_RESOLVED);
            return RTNORM;  // already in there
            }
        }
    stylehip=new db_styletabrec(*xrefstylehip);
    stylehip->SetHandle(NULL);  // copy constructor also copies handle; we need a new one.
                                // new one will be added automatically at addhanditem time.
    stylehip->set_2(toplevelstylename);
    stylehip->set_xrefblkhip(xrefblkptr);
    stylehip->set_xrefidx(1);
    stylehip->set_70(stylehip->ret_flags() | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);
    stylehip->set_3(xrefstylehip->retp_fontpn(),topdp);
    stylehip->set_4(xrefstylehip->retp_bigfontpn(),topdp);
    // EBATECH(CNBR) -[ Copy EED
    if( xrefstylehip->hasTrueTypeFont()){
        char family[256];
        long stylenumber;
        xrefstylehip->getFontFamily(family);
        xrefstylehip->getFontStyleNumber(&stylenumber);
        stylehip->setFontStyleNumber(family,stylenumber,topdp);
    }
    // EBATECH(CNBR) ]-
    stylehip->RemovePersistentReactors();
    if (topdp->addhanditem(stylehip))
        return RTERROR;
    return RTNORM;
    }


short AddXrefLtypeIfNecc(db_drawing *topdp,db_ltypetabrec *xrefltypehip,db_blocktabrec *xrefblkptr,
                        db_drawing *xrefdp)
// load xref linetype.  Load any required styles.
    {
    db_ltypetabrec *ltypehip;
    db_styletabrec *stylehip;
    char toplevelltypename[512],*ltypename,toplevelstylename[512];
    db_ltypedashlink *ltdash,*newdash;

    ltypename=xrefltypehip->retp_name();
    if (strisame(ltypename,db_str_bylayer) || strisame(ltypename,db_str_byblock) || strisame(ltypename,db_str_continuous))
        return RTNORM;

// if not any of those, add the linetype from the xref to the top level drawing
    sprintf(toplevelltypename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),ltypename);
    //Bugzilla No. 78130 ; 06-05-2002
    //if ((ltypehip=(db_ltypetabrec *)topdp->findtabrec(DB_LTYPETAB,toplevelltypename,0))!=NULL)
    if ((ltypehip=(db_ltypetabrec *)topdp->findtabrec(DB_LTYPETAB,toplevelltypename,1))!=NULL)
        {
        ltypehip->set_70(ltypehip->ret_flags() | IC_XREF_RESOLVED);
        return RTNORM;  // already in there
        }
    ltypehip=new db_ltypetabrec(*xrefltypehip);
    ltypehip->SetHandle(NULL);  // copy constructor also copies handle; we need a new one.
                                // new one will be added automatically at addhanditem time.
    ltypehip->set_2(toplevelltypename);
    ltypehip->set_70(xrefltypehip->ret_flags() | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);
    ltypehip->set_xrefidx(1);
    ltypehip->set_xrefblkhip(xrefblkptr);

    ltdash=xrefltypehip->retp_dash();
    if (ltdash!=NULL)
        {
        newdash=ltypehip->retp_dash();
        for(int i=0; i<xrefltypehip->ret_ndashes() && i<DB_MAXLTDASHES; i++,ltdash++,newdash++)
            {
            stylehip=NULL;
            if (ltdash->stylehip!=NULL)
            // EBATECH(CNBR) -[ 2002/4/28 shape style record has no unique name.
            {
                if(ltdash->type & 4) // Is shape style record
                {
                    char* fontname = ((db_styletabrec *)(ltdash->stylehip))->retp_fontpn();
                    strcpy(toplevelstylename,"SHAPE|REF"/*DNT*/);
                    for (stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,0); stylehip!=NULL; stylehip=(db_styletabrec *)stylehip->next)
                    {
                        if( strisame(stylehip->retp_fontpn(), fontname))
                        {
                            break;
                        }
                    }
                }
                else // Is text style record
                {
                sprintf(toplevelstylename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_styletabrec *)(ltdash->stylehip))->retp_name());
                //Bugzilla No. 78130 ; 06-05-2002
                //stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,0);
                stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,1);
                }
            }
            // elder code.
            //{
            //  sprintf(toplevelstylename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_styletabrec *)(ltdash->stylehip))->retp_name());
            //  stylehip=(db_styletabrec *)topdp->findtabrec(DB_STYLETAB,toplevelstylename,0);
            //}
            // EBATECH(CNBR) ]-
            newdash->stylehip=stylehip;
            }

        }
    ltypehip->RemovePersistentReactors();
    if (topdp->addhanditem(ltypehip))
        return RTERROR;
    return RTNORM;
    }


short AddDefpointsLayerIfNecc(db_drawing *topdp,db_layertabrec *xreflayhip,db_blocktabrec *xrefblkptr)
    {
    db_handitem *defpointslayhip,*ltypehip;
    char toplevelltypename[512];

//if the defpts layer exists in the xref dwg, skip it unless it doesn't
//exist in current drawing.  If not in current drawing, copy xref entry
    //Bugzilla No. 78130 ; 06-05-2002
    //if (defpointslayhip=(db_layertabrec *)topdp->findtabrec(DB_LAYERTAB,"DEFPOINTS"/*DNT*/,0))
    if (defpointslayhip=(db_layertabrec *)topdp->findtabrec(DB_LAYERTAB,"DEFPOINTS"/*DNT*/,1))
        return RTNORM;
    defpointslayhip = new db_layertabrec(*xreflayhip);
    defpointslayhip->SetHandle(NULL);
// need to set linetype, etc. also?  need to move ltype in from xref if necc?  requires some research.

    ltypehip=NULL;
    if (xreflayhip->ret_ltypehip()!=NULL)
        {
        sprintf(toplevelltypename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_ltypetabrec *)(xreflayhip->ret_ltypehip()))->retp_name());
        //Bugzilla No. 78130 ; 06-05-2002
        //ltypehip=(db_ltypetabrec *)topdp->findtabrec(DB_LTYPETAB,toplevelltypename,0);
        ltypehip=(db_ltypetabrec *)topdp->findtabrec(DB_LTYPETAB,toplevelltypename,1);
        }
    //Bugzilla No. 78057 ; 22-04-2002
    defpointslayhip->set_6(ltypehip);
    defpointslayhip->RemovePersistentReactors();
    if (topdp->addhanditem(defpointslayhip))
        return RTERROR;
    return RTNORM;
    }


short AddXrefLayerIfNecc(db_drawing *topdp, db_layertabrec *xreflayhip, db_blocktabrec *xrefblkptr,db_drawing *xrefdp)
// Load xref layer.  Load any required linetypes.
    {
    db_layertabrec *layhip;
    db_ltypetabrec *ltypehip;
    char toplevellayname[512],toplevelltypename[512];

    if (strsame(xreflayhip->retp_name(),"0"/*DNT*/)) return RTNORM; // don't add layer 0
    if (strisame(xreflayhip->retp_name(),"DEFPOINTS"/*DNT*/))
        {
        return(AddDefpointsLayerIfNecc(topdp,xreflayhip,xrefblkptr));
        }
    if (NameContainsPipeChar(xreflayhip->retp_name())) return RTNORM;  // if this layer is itself from an xref below, don't mess with it
    sprintf(toplevellayname,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),xreflayhip->retp_name());
    //Bugzilla No. 78130 ; 06-05-2002
    //if (layhip=(db_layertabrec *)topdp->findtabrec(DB_LAYERTAB,toplevellayname,0))
    if (layhip=(db_layertabrec *)topdp->findtabrec(DB_LAYERTAB,toplevellayname,1))
        {
        layhip->set_70(layhip->ret_flags() | IC_XREF_RESOLVED);
        return RTNORM;  // already in there
        }
    layhip = new db_layertabrec(*xreflayhip);
    layhip->SetHandle(NULL);    // copy constructor also copies handle; we need a new one.
                                // new one will be added automatically at addhanditem time.
    layhip->set_xrefidx(1);
    layhip->set_2(toplevellayname);
    layhip->set_xrefblkhip(xrefblkptr);
    layhip->set_70(xreflayhip->ret_flags() | IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED);

    ltypehip=NULL;
    if (xreflayhip->ret_ltypehip()!=NULL)
        {
        sprintf(toplevelltypename,"%s|%s"/*DNT*/,xrefblkptr->retp_name(),((db_ltypetabrec *)(xreflayhip->ret_ltypehip()))->retp_name());
        //Bugzilla No. 78130 ; 06-05-2002
        //ltypehip=(db_ltypetabrec *)topdp->findtabrec(DB_LTYPETAB,toplevelltypename,0);
        ltypehip=(db_ltypetabrec *)topdp->findtabrec(DB_LTYPETAB,toplevelltypename,1);
        }

    layhip->set_6(ltypehip);
    layhip->RemovePersistentReactors();
    if (topdp->addhanditem(layhip))
        return RTERROR;
    return RTNORM;
    }

short UpdateTopLevelTables(db_drawing *topdp, db_drawing *xrefdp,db_blocktabrec *xrefblkptr)
    {
    db_handitem *hip;

    ASSERT(topdp!=NULL);
    ASSERT(xrefdp!=NULL);
    ASSERT(xrefblkptr!=NULL);

// order important
    for (hip=xrefdp->tblnext(DB_APPIDTABREC,1); hip!=NULL; hip=xrefdp->tblnext(DB_APPIDTABREC,0))
        {
        if (AddXrefAppidIfNecc(topdp,(db_appidtabrec *)hip,xrefdp)!=RTNORM)
            return RTERROR;
        }
    for (hip=xrefdp->tblnext(DB_STYLETABREC,1); hip!=NULL; hip=xrefdp->tblnext(DB_STYLETABREC,0))
        {
        if (AddXrefStyleIfNecc(topdp,(db_styletabrec *)hip,xrefblkptr,xrefdp)!=RTNORM)
            return RTERROR;
        }
    for (hip=xrefdp->tblnext(DB_DIMSTYLETABREC,1); hip!=NULL; hip=xrefdp->tblnext(DB_DIMSTYLETABREC,0))
        {
        if (AddXrefDimstyleIfNecc(topdp,(db_dimstyletabrec *)hip,xrefblkptr,xrefdp)!=RTNORM)
            return RTERROR;
        }
    for (hip=xrefdp->tblnext(DB_LTYPETABREC,1); hip!=NULL; hip=xrefdp->tblnext(DB_LTYPETABREC,0))
        {
        if (AddXrefLtypeIfNecc(topdp,(db_ltypetabrec *)hip,xrefblkptr,xrefdp)!=RTNORM)
            return RTERROR;
        }
    for (hip=xrefdp->tblnext(DB_LAYERTABREC,1); hip!=NULL; hip=xrefdp->tblnext(DB_LAYERTABREC,0))
        {
        if (AddXrefLayerIfNecc(topdp,(db_layertabrec *)hip,xrefblkptr,xrefdp)!=RTNORM)
            return RTERROR;
        }
    return RTNORM;
    }

/*-------------------------------------------------------------------------*//**
    Function set db_handitem::m_parentBlock = bthip for every db_entity in xref 
    and for every block table records except for Xref-s 
@param bthip => block table record which keeps xref
@return RTNORM - siccess or RTERROR if bthip is wrong
*//*--------------------------------------------------------------------------*/
short setParentBlockInXref(db_blocktabrec* bthip)
{
    ASSERT(bthip);
    db_drawing* pXrefDb = NULL;

    if(bthip->ret_flags() & IC_BLOCK_XREF && (pXrefDb = bthip->ret_xrefdp()))
    {
        for(db_handitem* pEnt = pXrefDb->ret_firstent(); pEnt; pEnt = pEnt->next)
        {
            ASSERT(!pEnt->ret_deleted());
            if(pEnt->ret_layerhip())
            { 
                ASSERT(!pEnt->m_parentBlock);
                if(!pEnt->m_parentBlock)
                    pEnt->m_parentBlock = bthip;
            }
        }
        for(db_handitem* pTabRec = pXrefDb->tblnext(DB_BLOCKTABREC,1); pTabRec; 
            pTabRec = pXrefDb->tblnext(DB_BLOCKTABREC,0))
        {
            ASSERT(!pTabRec->ret_deleted());
            ASSERT(!pTabRec->m_parentBlock);
            if(!(((db_blocktabrec*)pTabRec)->ret_flags() & IC_BLOCK_XREF) && !pTabRec->m_parentBlock) 
                pTabRec->m_parentBlock = bthip;
        }
        return RTNORM;
    }
    return RTERROR;
}


short LoadXref(
    db_drawing *p_topdp,        // I: pointer to top level drawing
    db_blocktabrec *bthip,      // I: pointer to db_blocktabrec for this xref (not necc in top level)
    char *btxrefpn,             // I: the path for this xref
    int recurlevel              // I: recursion level
    )
    {
    char *pn,*ffpn;
    short pass;
    drw_readwrite *pFileIO;
    db_drawing *btxrefdp=NULL;
    short fi1,fi2;
    int btflags;

    bthip->get_70(&btflags);
    pn=ffpn=NULL;

    if ((pn= new char [strlen(btxrefpn)+5])==NULL ||
        (ffpn= new char [DB_PATHNAMESZ])==NULL)
        goto xrefout;
    /* DON'T LEAVE UNTIL FREED.  goto xrefout instead. */

    strcpy(pn,btxrefpn);
    fi1=0;
    while (pn[fi1])
        fi1++;

    fi2=fi1;  /* Where the dot is or will be. */
    while (fi1>-1 && pn[fi1]!='.' && pn[fi1]!=IC_SLASH)
        fi1--;

    if (fi1>-1 && pn[fi1]=='.')
        fi2=fi1;

    pFileIO=new drw_readwrite;
    for (pass=0; pass<3 && btxrefdp==NULL; pass++)
        {
        if (pass==1)
            strncpy(pn+fi2,IC_DWGEXT,4);
        else if (pass==2)
            strncpy(pn+fi2,DB_OURFILEEXT,4);
        pn[fi2+4]=0;

        // Modified CyberAge VSB 09/07/2001 [DD/MM/YY] [
        // Mail from Surya Surda dated 28/06/2001.
        // Xref's files not being searched in search paths[ tools->options].
        if ((btxrefdp=pFileIO->drw_openfile(NULL,pn,NULL,true))==NULL )
            {
                /*DG - 31.1.2002*/// Allow also relative paths. Old code commented out below.
                //if(sds_findfile(pn, ffpn) == RTNORM)
                if(ic_findinpath(NULL, pn, ffpn, DB_PATHNAMESZ) == RTNORM)
                    btxrefdp = pFileIO->drw_openfile(NULL, ffpn, NULL, true);
                if(!btxrefdp)
                {
                    TCHAR   szFileName[ MAX_PATH] = {0}, szFileExt[MAX_PATH] = {0};

                    _tsplitpath(pn, NULL, NULL, szFileName, szFileExt);
                    _tcscat(szFileName, szFileExt);
                //  if(sds_findfile(szFileName, ffpn) == RTNORM)
                    if(ic_findinpath(NULL, szFileName, ffpn, DB_PATHNAMESZ) == RTNORM)
                        btxrefdp = pFileIO->drw_openfile(NULL, ffpn, NULL, true);
                }
                /*
                if( -1==access(pn,00) )
                {
                    TCHAR szFileName[ MAX_PATH] = {0};
                    TCHAR szFileExt[MAX_PATH]   = {0};

                    _tsplitpath( pn, NULL, NULL, szFileName, szFileExt);
                    _tcscat(szFileName, szFileExt);
                    if ( sds_findfile(szFileName,ffpn)==RTNORM )
                        btxrefdp=pFileIO->drw_openfile(NULL,ffpn,NULL,true);
                }
                else
                {
                    if ( sds_findfile(pn,ffpn)==RTNORM )
                        btxrefdp=pFileIO->drw_openfile(NULL,ffpn,NULL,true);
                }
                */
            }
        // Modified CyberAge VSB 09/07/2001]
        }
    delete pFileIO;

    bthip->set_xrefdp(btxrefdp);
    bthip->set_looked4xref(1);
    if (btxrefdp!=NULL)
        {
// add a top level block def for this xref, if necessary
        db_blocktabrec *toplevelblockheader;
        AddTopLevelXrefBlockIfNecessary(p_topdp,btxrefdp,(db_blocktabrec *)bthip,&toplevelblockheader);

// update top level layer table with layers from this just-loaded file
        if (recurlevel<=1 || !(btflags & IC_BLOCK_XREF_OVERLAID))
            UpdateTopLevelTables(p_topdp,btxrefdp,toplevelblockheader);
        }

    setParentBlockInXref(bthip);

    xrefout:

    if (pn  !=NULL)
        delete [] pn;

    if (ffpn!=NULL)
        delete [] ffpn;

    return RTNORM;
    }

