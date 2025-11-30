/* LIB\SDS\SDS_IMAGE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 *   Image functions
 *
 */ 

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "objects.h"

int SDS_entmakeImageCopy(struct sds_resbuf* oldImage)
{
    struct resbuf* tmp;
    int result = -1, undoarea=-1;
    db_handitem *bhip,*ehip;
    db_handitem *newhip = NULL;
    sds_name imageDef, imageReactName, imageName;

    imageDef[0] = imageDef[1] = imageReactName[0] = imageReactName[1] = 0;

    db_drawing* flp = (db_drawing*)SDS_CURDWG;

    flp->get_entllends(&bhip,&ehip);

    if(flp->entmake(oldImage,&newhip) || newhip==NULL) 
	{
        struct sds_resbuf rb;
		rb.restype=RTSHORT;
		rb.resval.rint=db_ret_lasterror();
		SDS_setvar(NULL,DB_QERRNO,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
		return(RTREJ);
	} 

	if(newhip) 
		SDS_LastHandItem=newhip;
    imageName[0] = (long)newhip;
    imageName[1] = (long)SDS_CURDWG;

    // Now get at the data of the oldImage's imagedef data.
    for (tmp = oldImage; tmp != NULL, tmp->restype != 340; tmp = tmp->rbnext) ;
    ASSERT(tmp != NULL);
    sds_name_set(tmp->resval.rlname, imageDef);

    struct sds_resbuf* headOfEnt = sds_entget(imageName);

    ASSERT(headOfEnt != NULL);

    for (tmp = headOfEnt; tmp; tmp = tmp->rbnext) 
	{
        if (tmp->restype == 360) 
		{
            // Create a new imagedef_reactor for this image
            struct sds_resbuf* imageReactor = sds_buildlist(
                        RTDXF0,"IMAGEDEF_REACTOR"/*DNT*/,
                        90,2,           // Class version.  (This time TWO means R14!) 
                        330,imageName,  
                        0);
            ASSERT(imageReactor != NULL);
            if (imageReactor == NULL)
                return(RTREJ);

            result = sds_entmakex(imageReactor,imageReactName);
            sds_relrb(imageReactor);
            ASSERT(result != RTERROR);
            if (result != RTNORM)
                return(RTREJ);

            // Now set this images reference to its IMAGEDEF-REACTOR to this
            // newly created one.
            sds_name_set(imageReactName, tmp->resval.rlname);

            // Now update the image.                            
            result = sds_entmod(headOfEnt);
            ASSERT(result != RTERROR);
            sds_relrb(headOfEnt);
            if (result != RTNORM)
                return (RTREJ);
            break;
        }
    }

    // If we don't have an imageDef, we are in a hurt and should
    // bail.
    ASSERT(imageDef[0] != 0 && imageDef[1] != 0);
    if (imageDef[0] == 0 || imageDef[1] == 0) 
	{
        struct sds_resbuf rb;
        rb.restype=RTSHORT;
		rb.resval.rint=OL_EMKINVCOMP;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		return (RTERROR);
    }

    struct resbuf* tmpReact = sds_entget(imageReactName);
    // If we have not created a imagedef_reactor, then simply update the entity,
    // because we are not copying an image rather creating one from scratch.
    if (tmpReact != NULL) 
	{
        // Now create a resbuf of type 330, with the reference to this new IMAGEDEF_REACTOR. 
        struct resbuf* new330 = sds_newrb(330);

        struct resbuf* reactorName;
        for (reactorName = tmpReact; reactorName; reactorName = reactorName->rbnext)
            if (reactorName->restype == -1)  // reactor name
                break;
        ASSERT(reactorName != NULL);
        sds_name_set(reactorName->resval.rlname, new330->resval.rlname);

        // Now find the spot in the IMAGEDEF resbuf chain to splice in the new
        // IMAGEDEF_REACTOR for this image.
        struct resbuf* tmpDef = sds_entget(imageDef);
        ASSERT(tmpDef != NULL);

        BOOL firstBrace = FALSE;
        for (tmp = tmpDef; tmp; tmp = tmp->rbnext) 
		{
            if (tmp->rbnext->restype == 102) {  // Open brace surrounding the IMAGEDEF_REACTOR references
                if (!firstBrace) 
				{              // We are looking for the closing one, so we'll skip the
                    firstBrace = TRUE;          // first one. Then add this new IMAGEDEF_REACTOR node just
                    continue;                   // before the closing brace.
                }
                new330->rbnext = tmp->rbnext;
                tmp->rbnext = new330;
                break;
            }
        }                    
        result = sds_entmod(tmpDef);
        ASSERT(result != RTERROR);

        sds_relrb(tmpReact); 

        // This will take care of the new330 resbuf too.
        sds_relrb(tmpDef);
        new330 = NULL;
    } 
    SDS_SetUndo(IC_UNDO_NEW_ENT_TAB_CLS,(void *)ehip,(void *)newhip,(void *)undoarea,flp);

    return(RTNORM);
}

int SDS_getaNewImageDef(sds_name &imagedefName, 
						sds_name imageName, 
                        sds_name imageDictionary,
						sds_name imageReactor, 
						db_drawing *flp)
{
    // At this point, the image still has the old imagedef on it so we will use it's info
    // to create a new one.
    struct sds_resbuf *tmp, *imageDefData, *imageData;
	//use handitem->entgetx instead of sds_entget, because if the image has been cut (therefore deleted) from the
	//original drawing, sds_entget will return a NULL.
	imageData = ((db_handitem *)imageName[0])->entgetx(NULL,(db_drawing *)imageName[1]);

    // Retrieve the old image def's data form the image.
    for (tmp = imageData; tmp != NULL, tmp->restype != 340; tmp = tmp->rbnext) ;
    ASSERT(tmp != NULL);
    if (tmp == NULL)
        return(RTERROR);

    int classVer;
    char imageFileName[_MAX_DIR];
    sds_point group10;
    sds_point group11;
    group10[2] =  group11[2] = 0.0;

    imageDefData = sds_entget(tmp->resval.rlname);
    for (tmp = imageDefData; tmp != NULL; tmp = tmp->rbnext) 
	{
        switch(tmp->restype) 
		{
        case 1:
            strcpy(imageFileName, tmp->resval.rstring);
        break;
        case 10:
            // 10x is the image width. // 20x is pixel width
            group10[SDS_X] = tmp->resval.rpoint[SDS_X];// 10 DXF codes
            group10[SDS_Y] = tmp->resval.rpoint[SDS_Y];// 20 DXF codes
            break;
        case 11:
            // 10y is the image height. // 20y is pixel height
            group11[SDS_X] = tmp->resval.rpoint[SDS_X];// 11
            group11[SDS_Y] = tmp->resval.rpoint[SDS_Y];// 21
            break;
        case 90:
            classVer = tmp->resval.rint;
            break;
        default:
            continue;
        }
    }

    // Build a list of resbufs for new image def.
    struct sds_resbuf *imageDef = sds_buildlist(
                                                RTDXF0,"IMAGEDEF"/*DNT*/,
                                                102, "{ACAD_REACTORS"/*DNT*/,
                                                330, imageDictionary,
                                                330, imageReactor,
                                                102, "}"/*DNT*/,
                                                100, "AcDbRasterImageDef"/*DNT*/,
                                                90,  classVer,
                                                1,   imageFileName,
                                                10,  group10,
                                                11,  group11,
                                                280, 1,
                                                281, 0,
                                                0);
    ASSERT(imageDef != NULL);
    if (imageDef == NULL)
        return(RTREJ);

	int result = flp->entmake(imageDef, (db_handitem **)(&imagedefName[0]));
    ASSERT(result == 0);
	if (result == 0) 
		result = RTNORM;
    sds_relrb(imageDef);

    if (result != RTNORM)
        return(RTREJ);
    imagedefName[1]=(long)flp;

    // Don't need these anymore.
    sds_relrb(imageData);
    sds_relrb(imageDefData);

    // Add a record to the image dictionary. 
    // The name is usually the filename, but doesn't have to be.
    char drive[_MAX_DIR], dir[_MAX_DIR], fname[_MAX_DIR],  ext[_MAX_DIR];
    _splitpath(imageFileName, drive, dir, fname, ext );

    if (flp->dictadd((db_handitem*)imageDictionary[0],fname,(db_handitem*)imagedefName[0])!=0)
        return(RTERROR);
    return(result);
}

// This function returns an integer of either RTNORM, RTERROR, or
// RTNONE. If RTNORM is returned, or if create is TRUE, then dictName
// will be filled in with the ename of the image dictionary. Its up
// to the user to check return values.
// 
int SDS_ret_imagedict(sds_name& dictName, db_drawing *argflp, bool create, bool& existed) 
{   
    int retVal = RTNORM;
    sds_name namedObjDict, imageDictName;
    struct sds_resbuf* dataBuf = NULL, *trbp1 = NULL;
	db_drawing *flp = (argflp)? argflp : SDS_CURDWG;
	if (flp==NULL) 
		return (RTERROR);

    dictName[0] = dictName[1] = 0;
    existed = false;

    // Retrieve the named object dictionary.
    namedObjDict[0]=(long)flp->namedobjdict();
    namedObjDict[1]=(long)flp;
    ASSERT(namedObjDict[0] != 0 && namedObjDict[1] != 0);
	dictName[1]=(long)flp;

    // Test to see if this drawing already have an image dictionary.
    db_handitem *hip=flp->dictsearch(((db_handitem *)namedObjDict[0]),"ACAD_IMAGE_DICT"/*DNT*/,0);
   if(hip!=NULL)    
	   // Yes an image dictionary exists. This drawing already has images in it.
       // Now get the data resbuf.
       dataBuf = hip->entgetx(NULL,flp);

   if (dataBuf != NULL) 
   {   
	    // We have an image dictionary in the drawing.
        // Get its ename and fill in the return ename.
        for (trbp1=dataBuf; trbp1 != NULL, trbp1->restype != -1; trbp1 = trbp1->rbnext);
        ASSERT(trbp1 != NULL);
        if (trbp1 == NULL) 
            return(RTERROR);

        sds_name_set(trbp1->resval.rlname, dictName);
        existed = true;

        // Release dictionary buffer.
        sds_relrb(dataBuf);

        return(RTNORM);
    } 
	else
        retVal = RTNONE;    // No image dictionary in this drawing.
    
    if (create == TRUE) 
	{   // No image dictionary exists 
        // in this drawing so we'll
        // make one and get its ename.
        // Build a dataBuf for an empty image dictionary object.
        if (dataBuf != NULL) 
		{
            sds_relrb(dataBuf); 
            dataBuf=NULL; 
        }
        dataBuf=sds_buildlist(  RTDXF0, "DICTIONARY"/*DNT*/,
                                102,    "{ACAD_REACTORS"/*DNT*/,
                                330,    namedObjDict,// Point back to parent.
                                102,    "}"/*DNT*/,
                                0);
        ASSERT(dataBuf != NULL);
        if (dataBuf == NULL) 
            return(RTERROR);

        // Make the image dictionary and and get its ename.
		if (flp->entmake(dataBuf,(db_handitem **)(&(imageDictName[0])))!=0) 
		{
            sds_relrb(dataBuf);
            return(RTERROR);
        }
        sds_relrb(dataBuf);  
        
        // Add the image dictionary record to the named objects dictionary, 
        retVal = flp->dictadd((db_handitem*)namedObjDict[0],"ACAD_IMAGE_DICT"/*DNT*/,(db_handitem*)imageDictName[0]);
        // Can't happen (i hope).
        ASSERT(retVal == 0);
		retVal = RTNORM;
        // Send back the dictName.
		imageDictName[1]=(long)flp;
        sds_name_set(imageDictName, dictName);
    }
    return(retVal);
}

int SDS_entmakeImageInsert(struct sds_resbuf* elist, db_drawing *argflp)
{
    int result = -1, undoarea=-1;
    db_handitem *bhip,*ehip;
    bool alreadyThere = false, createIt = true, foundExistingImageDef = false;
    struct sds_resbuf  *idef  = NULL, *tmpdef = NULL;
    sds_name ename = {0L, 0L}, imageDictionary = {0L, 0L}, imageReactor = {0L, 0L}, imagedefName = {0L, 0L};
    db_drawing* flp = (argflp) ? argflp : SDS_CURDWG;

    flp->get_entllends(&bhip,&ehip);

    // First get the name of the imageDictionary. Note we will create it if it 
    // doesn't already exist. The alreadyThere flage lets me know if I need to
    // check for the imagedef (which may also already exist.
    if (SDS_ret_imagedict(imageDictionary, flp, createIt, alreadyThere) == RTERROR)
        return(RTERROR);
    ASSERT(imageDictionary[0] != 0 && imageDictionary[1] != 0);

    //---------------------------------------------------------------------------
    // Ultimately we will change the res_buf here and entmod it after connecting 
    // all the goodies up correctly.
    //---------------------------------------------------------------------------

    // Create a new reactor for this image
    // First build a list of resbufs to create an image reactor from.
    tmpdef = sds_buildlist(RTDXF0, "IMAGEDEF_REACTOR"/*DNT*/,
                90, 2, // Class version.  (This time TWO means R14!) 
                0);
    ASSERT(tmpdef != NULL);

    // Make it and add it to the database.
    db_handitem* newhip = NULL;
    if(flp->entmake(tmpdef, &newhip) || newhip == NULL) 
	{
        struct sds_resbuf rb;
		rb.restype = RTSHORT;
		rb.resval.rint = db_ret_lasterror();
		SDS_setvar(NULL, DB_QERRNO, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		return(RTREJ);
	} 
    sds_relrb(tmpdef);
	imageReactor[0] = (long)newhip;
	imageReactor[1] = (long)flp;
	newhip = NULL;

    if (alreadyThere) 
	{   // The image dictionary was already in the drawing
        // so we don't need to creat one, just add this new
        // image to it. This could happen if there is already
        // an image in the drawing.
        // Need to see if the imagedef is already in the dictionary. So we will
        // save of the file name for the image.
        char* imageFileName = NULL;
        for (idef = elist; idef != NULL; idef = idef->rbnext) 
		{
            if (idef->restype == 340) 
			{
                struct sds_resbuf* oldIdef = sds_entget(idef->resval.rlname);
                ASSERT(oldIdef != NULL);
                for(idef = oldIdef; idef, idef->restype != 1; idef = idef->rbnext) ;
                ASSERT(idef != NULL);
                imageFileName = new char [strlen(idef->resval.rstring) + 1];
                strcpy(imageFileName, idef->resval.rstring);
                sds_relrb(oldIdef);
                break;
            }
        }
        // Now we have the image's image file name.
        ASSERT(imageFileName != NULL);

        // Now look through the imagedef dictionary looking for an imagedef with 
        // an image file name (group code 1) with a name that matches the one comming
        // in. If it does, then we need to simply hook it up instead of creating a new
        // one.
        struct sds_resbuf* imageDictionaryInfo = sds_entget(imageDictionary);
        struct sds_resbuf* tmpInfo = NULL;
        for (idef = imageDictionaryInfo; idef != NULL; idef = idef->rbnext) 
		{
            if (idef->restype == 350 && !foundExistingImageDef) 
			{
                sds_name_set(idef->resval.rlname, imagedefName);
                tmpInfo = sds_entget(imagedefName);
                for(tmpdef = tmpInfo; tmpdef, tmpdef->restype != 1; tmpdef = tmpdef->rbnext) ;
                ASSERT(tmpdef != NULL);
                if (strsame(imageFileName, tmpdef->resval.rstring)) 
				{
                    foundExistingImageDef = TRUE;
                    // No need to look any further
                    idef = NULL;
                    break;
                }
            }

        }
        if (tmpInfo != NULL)
            sds_relrb(tmpInfo);
        // Don't need the dictionary info anymore.
        if (imageDictionaryInfo != NULL)
            sds_relrb(imageDictionaryInfo);
        // Don't need the image's file name anymore either.
        if(imageFileName != NULL)
            IC_FREE(imageFileName);
       
        if (foundExistingImageDef) 
		{
            // So we found an existing imagedef in the imagedef dictionary that matches
            // the image being inserted. So we will add the imagedef_reactor we just created
            // to it. (Note the imagedef_reactor already knows it image)
            idef = sds_entget(imagedefName);
            ASSERT(idef != NULL);
            
            // Now create a resbuf link with the info on our new imagedef_reactor.
            struct resbuf* new330 = sds_newrb(330);
            sds_name_set(imageReactor, new330->resval.rlname);

            BOOL firstBrace = FALSE;
            for (tmpdef = idef; tmpdef != NULL ; tmpdef = tmpdef->rbnext) 
			{
                if (tmpdef->rbnext->restype == 102) 
				{   // Open brace surrounding the IMAGEDEF_REACTOR references
					// We are looking for the closing one, so we'll skip the
					// first one. Then add this new IMAGEDEF_REACTOR node just
					// before the closing brace.
                    if (!firstBrace) 
					{                  
                        firstBrace = TRUE;              
                        continue;                       
                    }
                    new330->rbnext = tmpdef->rbnext;
                    tmpdef->rbnext = new330;
                    break;
                }
            }  
            // Now actually update the image def, and release the buffer.
            int result = sds_entmod(idef);
            ASSERT(result != RTERROR);
            sds_relrb(idef);
        } 
    } 

    if (!foundExistingImageDef) 
	{
        // This not only creates an image def, it hooks it up to the dictionary, and adds the
        // new reactor to it too.
        if(SDS_getaNewImageDef(imagedefName, elist->resval.rlname, imageDictionary, imageReactor, flp) != RTNORM)
            return(RTERROR);
    }

    // Make sure the image has the correct imagedef in it's 340 group, and the new reactor 
    // name as it's 360.
    for(tmpdef = elist; tmpdef != NULL; tmpdef = tmpdef->rbnext) 
	{
        if (tmpdef->restype == 340)
            sds_name_set(imagedefName, tmpdef->resval.rlname);
        if (tmpdef->restype == 360) 
		{
            sds_name_set(imageReactor, tmpdef->resval.rlname);
            break;
        }
    }

    // First get a new hip based on the data in the old image's resbuf chain.
    if(flp->entmake(elist, &newhip) || newhip == NULL) 
	{
        struct sds_resbuf rb;
		rb.restype = RTSHORT;
		rb.resval.rint = db_ret_lasterror();
		SDS_setvar(NULL, DB_QERRNO, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
		return(RTREJ);
	} 
	if(newhip) 
		SDS_LastHandItem=newhip;
    // Get the images data.
    elist = newhip->entgetx(NULL, flp);             
    ASSERT(elist != NULL);
    if (elist == NULL) 
        // Probably should get rid of the imagedef dictionary here.
        return(RTERROR);

    // Now that the image has been created, we need to go back and update
    // the imagedef_reactor to have it's 330 group point to this new images
    // entity name.

	// 11/9/98.  Note that I have removed the method for adding this 330 group that was
	// using resbufs, and gone directly to the hips.  We know that this will probably not undo
	// properly, but as the Undo system does not work properly anyway, I think it is better
	// to make sure that we do not crash while doing this wblock.  The problem was that the
	// entgetx() on the imagedef reactor would FAIL because the imagedef reactor is originally
	// created with a 330 group with a blank handle.  This causes the entget to crash.
	
	// delete existing hireflinks
    ((db_imagedef_reactor *)imageReactor[0])->delhirefll();

	// add the new one
	db_hireflink *thirp1=new db_hireflink(DB_SOFT_POINTER,newhip->RetHandle(),newhip);
	((db_imagedef_reactor *)imageReactor[0])->addhiref(thirp1);

//	struct sds_resbuf* tmpBuf, *tmpReact = ((db_handitem *)imageReactor[0])->entgetx(NULL,flp);
//	for(tmpBuf = tmpReact; tmpBuf != NULL, tmpBuf->restype != 330; tmpBuf = tmpBuf->rbnext);
//	ASSERT(tmpBuf != NULL);
//	sds_name_set(elist->resval.rlname, tmpBuf->resval.rlname);
//	((db_imagedef_reactor *)imageReactor[0])->entmod(tmpReact,flp);

    SDS_SetUndo(IC_UNDO_NEW_ENT_TAB_CLS,(void *)ehip,(void *)newhip,(void *)undoarea,flp);

    return(RTNORM);
}



