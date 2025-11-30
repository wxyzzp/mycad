// Utilities.cpp: implementation of the ReadWriteUtils class.
//
//////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include <ctype.h>
// Icad
#include "styletabrec.h"
#include "viewport.h"
#include "..\\..\\prj\\lib\\db\\DbLayout.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h 
#include "..\\..\\prj\\lib\\db\\objects.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\Objects.h

// DWGdirect
#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "DbTextStyleTable.h"
#include "DbLinetypeTable.h"
#include "DbLayerTable.h"
#include "DbDictionary.h"
#include "DbSymUtl.h"
#include "DbDimStyleTable.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h 
#include "DbBlockBegin.h"
#include "DbBlockEnd.h"
#include "DbViewport.h"
#include "DbFiler.h"
#include "GeExtents3d.h"
#include "DbBlockReference.h"

// OdGe
#include "DbPoint.h"

//#define _DEBUG_MHB

#ifdef _DEBUG_MHB
void _floatToString(double value, char *psBuffer, short prec);
#ifdef _DEBUG_MHB
void _fixZeroRealDWGdirect(OdGeVector3d& pD);
void _fixZeroRealDWGdirect(OdGePoint3d& pD);
#endif

#endif

#define ICAD_WNDACTION_PRINTF 5104

OdDbObjectId ReadWriteUtils::createBlock(OdDbDatabasePtr& pDDdwg, db_drawing* pDwg, DWGdirectConverter* converter, OdString blockName) 
{
	OdDbObjectId blockId;

	OdDbObjectId blockTableId = pDDdwg->getBlockTableId();
	OdDbBlockTablePtr blockTablePtr = blockTableId.safeOpenObject(OdDb::kForWrite);

	OdDbObjectId existingId = blockTablePtr->getAt(blockName);
	if (!existingId.isNull())
		return existingId;

	db_blocktabrec* pBlockRec = (db_blocktabrec*)pDwg->findtabrec(DB_BLOCKTAB, blockName.c_str(),1);
	ASSERT(pBlockRec != NULL);
	if (pBlockRec == NULL)
		return blockId; // blockId.isNull()

	OdDbBlockTableRecordPtr pOdDbBlock = OdDbBlockTableRecord::createObject();
	// If its an anonymous block, look it up by its handle.
	if (blockName.getAt(0) == '*' && (blockName.getLength() == 2 || isdigit(blockName.getAt(2))))
	{
		db_blocktabrec* pBlockRec = (db_blocktabrec*)pDwg->findtabrec(DB_BLOCKTAB, blockName.c_str(),1);
		if (pBlockRec != NULL)
		{
			return getBlockIDByHandle(pDDdwg, pBlockRec->RetHandle());
		}
		blockName.deleteChars(2, (blockName.getLength() - 2));
	}

	pOdDbBlock->setName(blockName);

	OdDbObjectId idBefore = setHandle(pDDdwg, pBlockRec, blockTableId, pOdDbBlock);

	blockId  = blockTablePtr->add(pOdDbBlock);
	ASSERT(idBefore == blockId);

	addEntitiesToBlocks(pDDdwg, pDwg, converter, pBlockRec, pOdDbBlock);

	return blockId;
}

OdString ReadWriteUtils::getStyleNameByHandle(db_drawing* pDwg, db_objhandle& styleHandle)
{
	db_styletabrec* pTabRec = (db_styletabrec*)pDwg->handent(styleHandle);
	ASSERT(pTabRec != NULL);
	if (pTabRec == NULL)
		return OdString("");

	char styleName[DB_ACADBUF];
	pTabRec->get_2(styleName, DB_ACADBUF - 1);

	return OdString(styleName);
}

OdDbObjectId ReadWriteUtils::getBlockIDByHandle(OdDbDatabasePtr& pDDdwg, db_objhandle& blockRecordHandle)
{
	ASSERT(!pDDdwg.isNull());
	ASSERT(blockRecordHandle != NULL);

	OdDbObjectId blockRecId;
	if (!pDDdwg.isNull() && blockRecordHandle != NULL)
	{
		char hand[17];
		blockRecordHandle.ToAscii(hand);
		blockRecId = pDDdwg->getOdDbObjectId(OdDbHandle(hand));
	}
	// This may not be what you think it is.
	return blockRecId;
}

OdDbObjectId ReadWriteUtils::getTextStyleIDByName(OdDbDatabasePtr& pDDdwg, OdString& name)
{
	ASSERT(!pDDdwg.isNull());

	OdDbTextStyleTablePtr pTable = pDDdwg->getTextStyleTableId().safeOpenObject();

	if (!name.isEmpty())
	{
		OdDbObjectId id = pTable->getAt(name);
		if (!id.isNull())
		{
			return id;
		}
	}
	return pDDdwg->getTextStyleStandardId();
}

OdDbObjectId ReadWriteUtils::getLinetypeIDByName(OdDbDatabasePtr& pDDdwg, OdString& name)
{
	ASSERT(!pDDdwg.isNull());

	OdDbLinetypeTablePtr pTable = pDDdwg->getLinetypeTableId().safeOpenObject();

	if (!name.isEmpty())
	{
		OdDbObjectId id = pTable->getAt(name);
		if (!id.isNull())
		{
			return id;
		}
	}
	return pDDdwg->getLinetypeContinuousId();
}

OdDbObjectId ReadWriteUtils::getLayerIDByName(OdDbDatabasePtr& pDDdwg, OdString& name)
{
	ASSERT(!pDDdwg.isNull());

	OdDbLayerTablePtr pTable = pDDdwg->getLayerTableId().safeOpenObject();

	if (!name.isEmpty())
	{
		OdDbObjectId id = pTable->getAt(name);
		if (!id.isNull())
		{
			return id;
		}
	}
	return pDDdwg->getLayerZeroId();
}

OdDbObjectId ReadWriteUtils::getMLineStyleIDByName(OdDbDatabasePtr& pDDdwg, OdString& name)
{
	OdDbDictionaryPtr pMLStyleDictionary = pDDdwg->getMLStyleDictionaryId(false).safeOpenObject();
	OdDbDictionaryIteratorPtr  pMLStyleDictionaryIter = pMLStyleDictionary->newIterator();

	OdDbObjectId standardID;
	for(; !pMLStyleDictionaryIter->done(); pMLStyleDictionaryIter->next())
	{
		OdString objName = pMLStyleDictionaryIter->name();
		if (objName.iCompare(OdDbSymUtil::textStyleStandardName()) == 0)
			standardID = pMLStyleDictionaryIter->objectId();

		if (objName == name)
		{
			return pMLStyleDictionaryIter->objectId();
		}
	}
	
	return standardID;
}

OdDbObjectId ReadWriteUtils::getBlockIDByName(OdDbDatabasePtr& pDDdwg, db_drawing* pDwg, DWGdirectConverter* converter, OdString& name)
{
	ASSERT(!pDDdwg.isNull());
	ASSERT(pDwg != NULL);
	ASSERT(converter != NULL);

	OdDbObjectId retId;

	// There were a whole group of drawings that have had their block table records
	// handles changed because they were very low hex numbers. In these cases, looking
	// them up by handle would fail. So if we get back something other than an
	// OdDbBlockTableRecord, for this handle, look it up by its name instead.
	OdDbBlockTablePtr pTable = pDDdwg->getBlockTableId().safeOpenObject();

	db_blocktabrec* pBlockRec = (db_blocktabrec*)pDwg->findtabrec(DB_BLOCKTAB, name.c_str(),1);
	
	if (pBlockRec != NULL)	// Could have been a NULL name. DIMBLK for instance.
	{
		retId = getBlockIDByHandle(pDDdwg, pBlockRec->RetHandle());
		if (retId.isErased())
		{
			if (!name.isEmpty())
			{
				OdDbObjectId id = pTable->getAt(name);
				if (!id.isNull())
				{
					ASSERT(!id.isErased());
					return id;
				}
			}
			else
				return OdDbObjectId(); // We have a problem here.
		}

		OdDbObjectPtr pObject = retId.safeOpenObject(OdDb::kForRead, true);
		if (!(pObject->isKindOf(OdDbBlockTableRecord::desc())))
		{
			if (!name.isEmpty())
			{
				OdDbObjectId id = pTable->getAt(name);
				if (!id.isNull())
				{
					return id;
				}
			}
			else
				return OdDbObjectId(); // We have a problem here.
		}
		else
		{
			// This is even stranger - here I found drawings that the block
			// being returned was *Model_Space, but the name was *X2. Here,
			// I'll compare the first two characters if they match then return
			// the id. If not look it up by name.
			OdString left2 = ((OdDbBlockTableRecordPtr)pObject)->getName().left(2);
			if (left2 != name.left(2))
			{
				if (!name.isEmpty())
				{
					OdDbObjectId id = pTable->getAt(name);
					if (!id.isNull())
					{
						return id;
					}
				}
				else
					return OdDbObjectId(); // We have a problem here.
			}
			else
			{
				return retId;
			}
		}
	}

	return OdDbObjectId();
}

OdDbObjectId ReadWriteUtils::getDimStyleIDByName(OdDbDatabasePtr& pDDdwg, OdString& name)
{
	ASSERT(!pDDdwg.isNull());

	OdDbDimStyleTablePtr pTable = pDDdwg->getDimStyleTableId().safeOpenObject();

	if (!name.isEmpty())
	{
		OdDbObjectId id = pTable->getAt(name);
		if (!id.isNull())
		{
			return id;
		}
	}
	return pDDdwg->getDimStyleStandardId();
}

void ReadWriteUtils::addEntitiesToBlocks(OdDbDatabasePtr& pDDdwg, db_drawing* pDwg, DWGdirectConverter* converter, db_blocktabrec* pBlock, OdDbBlockTableRecord* pOdDbBlock, bool bResolveLayout)
{
	ASSERT(pBlock != NULL);
	ASSERT(pOdDbBlock != NULL);

	db_handitem *entsBegin = NULL;
	db_handitem *entsEnd = NULL;
	pBlock->get_blockllends(&entsBegin,&entsEnd);
	ASSERT(entsBegin != NULL && entsEnd != NULL);

	// We need to update is block table record's layout ID,
	// And we need to update the blocks layout's block table record ID also.
	OdDbLayoutPtr layoutPtr;
	OdDbObjectId layoutId;
	db_objhandle layoutHandle;
	pBlock->get_layoutobjhandle(&layoutHandle);

	db_handitem* layoutItem = NULL;
	if (layoutHandle != NULL && bResolveLayout == true)
	{
		layoutItem = pDwg->handent(layoutHandle);
		if (layoutItem != NULL && layoutItem->ret_type() == DB_LAYOUT)
		{
			char* layoutName = (char*)((CDbLayout*)pDwg->handent(layoutHandle))->name(pDwg);

			OdDbDictionaryPtr pOdLayoutDictionary = pDDdwg->getLayoutDictionaryId().safeOpenObject(OdDb::kForRead);
			layoutPtr = pOdLayoutDictionary->getAt(layoutName, OdDb::kForWrite);
			if (!layoutPtr.isNull())
			{
				ASSERT(layoutPtr->isKindOf(OdDbLayout::desc()));

				pOdDbBlock->setLayoutId(layoutPtr->id());
				layoutPtr->setBlockTableRecordId(pOdDbBlock->id()); 
			}
		}
	}

	char desc[1024]; // hard wired in blocktabrec.h
	pBlock->get_description(desc);
	pOdDbBlock->setComments(OdString(desc));

	char* pathName;
	pBlock->get_1(&pathName);
	pOdDbBlock->setPathName(OdString(pathName));

	int intVal;
	pBlock->get_70(&intVal);
	// TODO MHB Can't set these??

	pBlock->get_71(&intVal);
	// TODO MHB I don't think this is being preserved in db_blocktabrec
	// need to add it for round tripping. defaults to in db_handitem 0.
	pOdDbBlock->setIsUnloaded((intVal == 1) ? true : false);

	sds_point point;
	pBlock->get_10(point);
	pOdDbBlock->setOrigin(OdGePoint3d(point[0], point[1], point[2]));

	for( ; entsBegin != NULL; entsBegin = entsBegin->next)
	{
		if (entsBegin->ret_deleted())
			continue;
		if (entsBegin->ret_type() == DB_BLOCK)
		{
			char* layerName;
			entsBegin->get_8(&layerName);
			pOdDbBlock->openBlockBegin(OdDb::kForWrite)->setLayer(OdString(layerName));
			continue;
		}
		if (entsBegin->ret_type() == DB_ENDBLK)
		{
			char* layerName;
			entsBegin->get_8(&layerName);
			pOdDbBlock->openBlockEnd(OdDb::kForWrite)->setLayer(OdString(layerName));
			continue;
		}
		try
		{
			OdDbObjectId id = converter->createOdDbEntity(pOdDbBlock->id(), entsBegin);

#ifdef _DEBUG_MHB
			// This is not really an error, because vertices for example are created
			// during polyline creation. or sequence ends are not created.
			if (id.isNull() && entsBegin->ret_type() != DB_INSERT && 
							   entsBegin->ret_type() != DB_DIMENSION &&
							   entsBegin->ret_type() != DB_ATTRIB &&
							   entsBegin->ret_type() != DB_SEQEND)
			{
				char tmp[100];
				sprintf(tmp, "Block Entity of type %d was NOT created\n", entsBegin->ret_type());
				OutputDebugString(tmp);
			}
#endif
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			ASSERT(!"DWGdirectWriter block sub-entity creation error: ");
#endif
		}
	}
		
		return;
}

OdDbObjectId ReadWriteUtils::setHandle(OdDbDatabasePtr& pDDdwg, db_handitem* pHandItem, OdDbObjectId pOwnerId, OdDbObject* pOdDbObject)
{
	ASSERT(!pDDdwg.isNull());
	ASSERT(pOdDbObject != NULL);
	ASSERT(pHandItem != NULL);

	char hand[17] = {0};
	pHandItem->RetHandlePtr()->ToAscii(hand);

	OdDbHandle odHandle(hand);

	OdDbObjectId newId;
	try
	{
		OdDbObjectId tmp = pDDdwg->getOdDbObjectId(OdDbHandle(hand));
#ifdef _DEBUG_MHB
		if (!tmp.isNull())
		{
			static bool turnOff = false;
			if (turnOff == false)
			{
				OdDbObjectPtr pTmp = tmp.safeOpenObject();
				OdString stringHandle = pTmp->objectId().getHandle().ascii();
				ASSERT(!"DWGdirectWriter::setHandle() Encountered an existing handle!");
			}
		}
#endif
		if (tmp.isNull())
		{
			return pDDdwg->addOdDbObject(pOdDbObject, pOwnerId, odHandle);
		}
		else
		{
			newId = pDDdwg->addOdDbObject(pOdDbObject, pOwnerId);
		}
	}
	catch(OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
		ASSERT(!"DWGdirectWriter error writing OdDbDatabase: DWGdirectWriter::setHandle()");
#endif
		newId = pDDdwg->addOdDbObject(pOdDbObject, pOwnerId);	
	}

	return newId;
}

int ReadWriteUtils::countLayouts(db_drawing* pDwg)
{
	int count = 0;
	db_dictionary* pLayoutDict = (db_dictionary*)pDwg->dictsearch(pDwg->namedobjdict(), "ACAD_LAYOUT", 0);
	ASSERT(pLayoutDict != NULL);
	if (pLayoutDict != NULL)
	{
		db_dictionaryrec *beginDictRecord= NULL;
		db_dictionaryrec *endDictRecord = NULL;
		pLayoutDict->get_recllends(&beginDictRecord, &endDictRecord);

		db_handitem* pHandItem = NULL;
		while (beginDictRecord != NULL)
		{
#ifdef _DEBUG_MHB
			OdString recordName = OdString(beginDictRecord->name);
#endif
			pHandItem = beginDictRecord->hiref->ret_hip(pDwg);
			ASSERT(pHandItem->ret_type() == DB_LAYOUT);
			if (!pHandItem->ret_deleted())
			{
				count++;
			}
			beginDictRecord = beginDictRecord->next;
		}
	}

	ASSERT(count > 1);
	return count;
}

OdResBufPtr ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(OdDbDatabasePtr& pDDdwg, struct resbuf* extData)
{
	OdResBufPtr retBuf;
	if (extData == NULL)
		return retBuf;

	if (extData->restype == -3)
		extData = extData->rbnext;
#ifdef _DEBUG_MHB
	char sBuf[20];
	char psBuffer[100];
	OutputDebugString("//////////////////////////////////////////////////////////////////////////////\n");
	int count = 0;
#endif
	int type = 0;
	OdResBufPtr pWalker;
	retBuf = OdResBuf::newRb(extData->restype);
	pWalker = retBuf;
	while(extData != NULL)
	{
#ifdef _DEBUG_MHB
		count++;
#endif
		// See top of resbuf.h for complete list most of which
		// is here.
		switch(extData->restype)
		{
		case OdResBuf::kDxfLayoutName:			// String data
		case OdResBuf::kDxfComment:				// 999
		case OdResBuf::kDxfXdAsciiString:		// 1000
		case OdResBuf::kDxfRegAppName:			// 1001
		case OdResBuf::kDxfXdControlString:		// 1002
		case OdResBuf::kDxfXdLayerName:			// 1003
		//case 1004:
		case 1006:
		case 1007:
		case 1008:
		case 1009:
		case OdResBuf::kRtString:				// 5005
		case OdResBuf::kDxfText:				// 1
		case OdResBuf::kDxfBlockName:			// 2
		case OdResBuf::kDxfDescription:			// 3
		case OdResBuf::kDxfSymTableRecComments:	// 4
		case OdResBuf::kDxfLinetypeName:		// 6
		case OdResBuf::kDxfTextStyleName:		// 7
		case OdResBuf::kDxfLayerName:			// 8
		case OdResBuf::kDxfCLShapeText:			// 9
		case OdResBuf::kDxfSubclass:			// 100
		case OdResBuf::kDxfControlString:		// 102
		case OdResBuf::kDxfXTextString:			// 300
		case 301:
		case 302:
		case 303:
		case 304:
		case 305:
		case 306:
		case 307:
		case 308:
		case 309:
		case 430:
		case 431:
		case 432:
		case 433:
		case 434:
		case 435:
		case 436:
		case 437:
		case 438:
		case 439:
		case 470:
		case 471:
		case 472:
		case 473:
		case 474:
		case 475:
		case 476:
		case 477:
		case 478:
		case 479:
		case OdResBuf::kDxfPlotStyleNameId:		// 390
				pWalker->setString(OdString(extData->resval.rstring));
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, extData->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			break;
		case OdResBuf::kDxfXdBinaryChunk:		// 1004	Binary chunck data
		case OdResBuf::kDxfBinaryChunk:			// 310	
		case 311:
		case 312:
		case 313:
		case 314:
		case 315:
		case 316:
		case 317:
		case 318:
		case 319:
			{
				OdBinaryData data;
				data.resize(extData->resval.rbinary.clen);
				memcpy((void*)data.asArrayPtr(), extData->resval.rbinary.buf, extData->resval.rbinary.clen);
				pWalker->setBinaryChunk(data);
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				itoa(extData->resval.rbinary.clen, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer,  ")");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			}
			break;
		case OdResBuf::kDxfXdHandle:			// 1005
		case OdResBuf::kDxfHandle:				// 5
		case OdResBuf::kDxfArbHandle:			// 320
		case 321:
		case 322:
		case 323:
		case 324:
		case 325:
		case 326:
		case 327:
		case 328:
		case 329:
		case OdResBuf::kDxfDimVarHandle:		// 105
				pWalker->setHandle(OdDbHandle(extData->resval.rstring));
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, extData->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			break;
		case OdResBuf::kDxfXdXCoord:			// 1010	Point data
		case OdResBuf::kDxfXdWorldXCoord:		// 1011
		case OdResBuf::kDxfXdWorldXDisp:		// 1012
		case OdResBuf::kDxfXdWorldXDir:			// 1013
		case OdResBuf::kRtOrientation:			// 5008  ??
		case OdResBuf::kRtPoint3d:				// 5009
		case OdResBuf::kDxfXCoord:				// 10
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		//case 38:
		//case 39:
		case OdResBuf::kDxfUCSOrg:				// 110
		case OdResBuf::kDxfUCSOriX:				// 111
		case OdResBuf::kDxfUCSOriY:				// 112
		case OdResBuf::kDxfNormalX:				// 210
		case 211:
		case 212:
		case 213:
		case 214:
		case 215:
		case 216:
		case 217:
		case 218:
		case 219:
		case 220:
		case 221:
		case 222:
		case 223:
		case 224:
		case 225:
		case 226:
		case 227:
		case 228:
		case 229:
		case 230:
		case 231:
		case 232:
		case 233:
		case 234:
		case 235:
		case 236:
		case 237:
		case 238:
		case 239:
				pWalker->setPoint3d(OdGePoint3d(extData->resval.rpoint[0],extData->resval.rpoint[1],extData->resval.rpoint[2]));
#ifdef _DEBUG_MHB
			{
			strcpy(psBuffer, "( ");
			itoa(extData->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, "  ");

			double tmp = fabs(extData->resval.rpoint[0])>1.0e-11 ? extData->resval.rpoint[0] : 0.0;
			_floatToString(tmp, sBuf, 4);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, "  ");

			tmp = fabs(extData->resval.rpoint[1])>1.0e-11 ? extData->resval.rpoint[1] : 0.0;
			_floatToString(tmp, sBuf, 4);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, "  ");

			tmp = fabs(extData->resval.rpoint[2])>1.0e-11 ? extData->resval.rpoint[2] : 0.0;
			_floatToString(tmp, sBuf, 4);        
			strcat(psBuffer, sBuf);
			strcat(psBuffer, ")");
			}
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");
#endif
			break;
		case OdResBuf::kRtPoint2d:				// 5002	Point data 2D
				pWalker->setPoint3d(OdGePoint3d(extData->resval.rpoint[0],extData->resval.rpoint[1],0.0));
			break;
		case OdResBuf::kDxfXdReal:				// 1040	Real data
		case OdResBuf::kDxfXdDist:				// 1041
		case OdResBuf::kDxfXdScale:				// 1042
		case 1043:
		case 1044:
		case 1045:
		case 1046:
		case 1047:
		case 1048:
		case 1049:
		case OdResBuf::kRtReal:					// 5001
		case OdResBuf::kRtAngle:				// 5004
		case OdResBuf::kDxfElevation:			// 38
		case OdResBuf::kDxfThickness:			// 39
		case OdResBuf::kDxfReal:				// 40
		case OdResBuf::kDxfTxtStyleXScale:		// 41
		case OdResBuf::kDxfTxtStylePSize:		// 42
		case OdResBuf::kDxfViewFrontClip:		// 43
		case OdResBuf::kDxfViewBackClip:		// 44
		case OdResBuf::kDxfViewHeight:			// 45
		case OdResBuf::kDxfShapeScale:			// 46
		case OdResBuf::kDxfPixelScale:			// 47
		case OdResBuf::kDxfLinetypeScale:		// 48
		case OdResBuf::kDxfLinetypeElement:		// 49
		case OdResBuf::kDxfAngle:				// 50
		case OdResBuf::kDxfViewportTwist:		// 51
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 113:
		case 114:
		case 115:
		case 116:
		case 117:
		case 118:
		case 119:
		case 120:
		case 121:
		case 122:
		case 123:
		case 124:
		case 125:
		case 126:
		case 127:
		case 128:
		case 129:
		case 130:
		case 131:
		case 132:
		case 133:
		case 134:
		case 135:
		case 136:
		case 137:
		case 138:
		case 139:
		case OdResBuf::kDxfXReal:				// 140
		case 141:
		case 142:
		case 143:
		case 144:
		case 145:
		case 146:
		case 147:
		case 148:
		case 149:
		case 460:
		case 461:
		case 462:
		case 463:
		case 464:
		case 465:
		case 466:
		case 467:
		case 468:
		case 469:
		//case 1010:
		//case 1011:
		//case 1012:
		//case 1013:
		case 1014:
		case 1015:
		case 1016:
		case 1017:
		case 1018:
		case 1019:
		case 1020:
		case 1021:
		case 1022:
		case 1023:
		case 1024:
		case 1025:
		case 1026:
		case 1027:
		case 1028:
		case 1029:
		case 1030:
		case 1031:
		case 1032:
		case 1033:
		case 1034:
		case 1035:
		case 1036:
		case 1037:
		case 1038:
		case 1039:
		case 1050:
		case 1051:
		case 1052:
		case 1053:
		case 1054:
		case 1055:
		case 1056:
		case 1057:
		case 1058:
		case 1059:
				pWalker->setDouble(extData->resval.rreal);
#ifdef _DEBUG_MHB
			{
				strcpy(psBuffer, "( " );
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");

				double tmp = fabs(extData->resval.rreal)>1.0e-11 ? extData->resval.rreal : 0.0;
				_floatToString(tmp, sBuf, 4);

				strcat(psBuffer, sBuf);
				strcat(psBuffer, " )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");

			}
#endif
			break;
		case OdResBuf::kDxfXdInteger16:			// 1070	16 bit integer data signed or unsigned
		case OdResBuf::kRtShort:				// 5003
		case OdResBuf::kDxfVisibility:			// 60		
		case OdResBuf::kDxfLayerLinetype:		// 61		
		case OdResBuf::kDxfColor:				// 62
		case 63:
		case 64:
		case 65:
		case OdResBuf::kDxfHasSubentities:		// 66		
		case OdResBuf::kDxfViewportVisibility:	// 67		
		case OdResBuf::kDxfViewportActive:		// 68		
		case OdResBuf::kDxfViewportNumber:		// 69		
		case OdResBuf::kDxfInt16:				// 70		
		case OdResBuf::kDxfViewMode:			// 71		
		case OdResBuf::kDxfCircleSides:			// 72		
		case OdResBuf::kDxfViewportZoom:		// 73		
		case OdResBuf::kDxfViewportIcon:		// 74		
		case OdResBuf::kDxfViewportSnap:		// 75		
		case OdResBuf::kDxfViewportGrid:		// 76		
		case OdResBuf::kDxfViewportSnapStyle:	// 77		
		case OdResBuf::kDxfViewportSnapPair:	// 78
		case 79:
		case OdResBuf::kDxfXInt16:				// 170	
		case 171:
		case 172:
		case 173:
		case 174:
		case 175:
		case 176:
		case 177:
		case 178:
		case 179:
		case OdResBuf::kDxfXXInt16:				// 270	
		case 271:
		case 272:
		case 273:
		case 274:
		case 275:
		case 276:
		case 277:
		case 278:
		case 279:
		case OdResBuf::kDxfLineWeight:			// 370	
		case 371:
		case 372:
		case 373:
		case 374:
		case 375:
		case 376:
		case 377:
		case 378:
		case 379:
		case OdResBuf::kDxfPlotStyleNameType:	// 380	
		case 381:
		case 382:
		case 383:
		case 384:
		case 385:
		case 386:
		case 387:
		case 388:
		case 389:
		case OdResBuf::kDxfXXXInt16:			// 400	
		case 401:
		case 402:
		case 403:
		case 404:
		case 405:
		case 406:
		case 407:
		case 408:
		case 409:
		case 1060:
		case 1061:
		case 1062:
		case 1063:
		case 1064:
		case 1065:
		case 1066:
		case 1067:
		case 1068:
		case 1069:
				pWalker->setInt16(extData->resval.rint);
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(extData->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(extData->resval.rint, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			break;
		case OdResBuf::kDxfXdInteger32:			// 1071 32 bit signed (long) integer data
		case OdResBuf::kRtLong:					// 5010
		case OdResBuf::kDxfInt32:				// 90	
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
		case 99:
		case 420:
		case 421:
		case 422:
		case 423:
		case 424:
		case 425:
		case 426:
		case 427:
		case 428:
		case 429:
		case 440:
		case 441:
		case 442:
		case 443:
		case 444:
		case 445:
		case 446:
		case 447:
		case 448:
		case 449:
		case 450:
		case 451:
		case 452:
		case 453:
		case 454:
		case 455:
		case 456:
		case 457:
		case 458:
		case 459:
				pWalker->setInt32(extData->resval.rlong);
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(extData->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(extData->resval.rlong, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			break;
		case OdResBuf::kDxfInt8:				// 280	
		case 281:
		case 282:
		case 283:
		case 284:
		case 285:
		case 286:
		case 287:
		case 288:
		case 289:
				pWalker->setInt8(extData->resval.rint);
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(extData->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(extData->resval.rint, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			break;
		case OdResBuf::kDxfBool:				// 290	
		case 291:
		case 292:
		case 293:
		case 294:
		case 295:
		case 296:
		case 297:
		case 298:
		case 299:
				pWalker->setBool((extData->resval.rint == 1) ? true : false);
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(extData->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			strcat(psBuffer, extData->resval.rint == 1 ? "true" : "false");
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			break;
		case OdResBuf::kDxfSoftPointerId:		// 330			
		case 331:				
		case 332:				
		case 333:				
		case 334:				
		case 335:					
		case 336:				
		case 337:				
		case 338:				
		case 339:
			{
				ASSERT(extData->rbnext != NULL);
				ASSERT(extData->resval.rlname[0] == extData->restype && extData->resval.rlname[1] == 0L);
			
				extData = extData->rbnext;
				OdDbObjectId objId = pDDdwg->getOdDbObjectId(OdDbHandle(extData->resval.rstring));
				pWalker->setObjectId(objId);
			}
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, extData->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			break;
		case OdResBuf::kDxfHardPointerId:		// 340			
		case 341:				
		case 342:				
		case 343:				
		case 344:				
		case 345:					
		case 346:				
		case 347:				
		case 348:				
		case 349:				
			{
				ASSERT(extData->rbnext != NULL);
				ASSERT(extData->resval.rlname[0] == extData->restype && extData->resval.rlname[1] == 0L);
			
				extData = extData->rbnext;
				OdDbObjectId objId = pDDdwg->getOdDbObjectId(OdDbHandle(extData->resval.rstring));
				pWalker->setObjectId(objId);
			}
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, extData->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			break;
		case OdResBuf::kDxfSoftOwnershipId:		// 350			
		case 351:				
		case 352:				
		case 353:				
		case 354:				
		case 355:				
		case 356:				
		case 357:				
		case 358:				
		case 359:				
			{
				ASSERT(extData->rbnext != NULL);
				ASSERT(extData->resval.rlname[0] == extData->restype && extData->resval.rlname[1] == 0L);
			
				extData = extData->rbnext;
				OdDbObjectId objId = pDDdwg->getOdDbObjectId(OdDbHandle(extData->resval.rstring));
				pWalker->setObjectId(objId);
			}
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, extData->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			break;
		case OdResBuf::kDxfHardOwnershipId:		// 360			
		case 361:				
		case 362:				
		case 363:				
		case 364:				
		case 365:				
		case 366:				
		case 367:				
		case 368:				
		case 369:				
		//case 390:			
		case 391:				
		case 392:				
		case 393:				
		case 394:				
		case 395:					
		case 396:				
		case 397:				
		case 398:				
		case 399:				
			{
				ASSERT(extData->rbnext != NULL);
				ASSERT(extData->resval.rlname[0] == extData->restype && extData->resval.rlname[1] == 0L);
			
				extData = extData->rbnext;
				OdDbObjectId objId = pDDdwg->getOdDbObjectId(OdDbHandle(extData->resval.rstring));
				pWalker->setObjectId(objId);
			}
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(extData->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, extData->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			break;
		default:
				ASSERT(!"Invalid resbuf data type");
			break;
		}
		if (extData->rbnext != NULL)
		{
			pWalker->setNext(OdResBuf::newRb(extData->rbnext->restype));
			pWalker = pWalker->next();
		}

		extData = extData->rbnext;
	}

#ifdef _DEBUG_MHB
	sprintf(psBuffer, "%d links created.\n", count);
	OutputDebugString(psBuffer);
#endif
	return retBuf;

/*  TODO MHB - Not quite sure what to do with these.
	kRtEntityName   = 5006,  Entity name 
	kRtPics         = 5007,  Pick set 
	kRtVoid         = 5014,  Blank symbol 
	kRtListBeg      = 5016,  list begin 
	kRtListEnd      = 5017,  list end 
	kRtDottedPair   = 5018,  dotted pair 
	kRtNil          = 5019,  nil 
	kRtDxf0         = 5020,  DXF code 0 
	kRtT            = 5021,  T atom 
	kRtResBuf       = 5023,  resbuf 

	kDxfInvalid        = -9999,
	kDxfXDictionary     = -6,
	kDxfPReactors       = -5,
	kDxfUndo            = -4,
	kDxfXDataStart      = -3,
	kDxfHeaderId        = -2,
	kDxfFirstEntId      = -2,
	kDxfEnd             = -1,
	kDxfStart           = 0,
*/
}

struct sds_resbuf* ReadWriteUtils::getSdsResBufPtrFromOdResBuf(OdResBuf* pResChain, OdDbDatabase* pDDdwg)
{
	ASSERT(pResChain != NULL);

#ifdef _DEBUG_MHB
	char sBuf[20];
	char psBuffer[100];
	OutputDebugString("/////////////////////////////////////////////////////////////////////////\n");
	int count = 0;
#endif

	struct resbuf* tmpResBuf = sds_newrb(0);
	OdResBufPtr tmpResChain = pResChain;

	struct resbuf* retBuf = tmpResBuf;
	while (!tmpResChain.isNull())
	{
#ifdef _DEBUG_MHB
		count++;
#endif
		int type = tmpResChain->restype();
		switch(OdDxfCode::_getType(type))
		{
		case OdDxfCode::Unknown:
			ASSERT(!"Don't know what this is!");
			break;
		case OdDxfCode::Name:
		case OdDxfCode::String:
		case OdDxfCode::LayerName:
			{
				OdString str = tmpResChain->getString();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				int strLen = str.getLength();
				tmpResBuf->resval.rstring = new char[strLen + 1];
				memset(tmpResBuf->resval.rstring, 0, strLen + 1);
				strncpy(tmpResBuf->resval.rstring, (char*)str.c_str(), strLen);
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(tmpResBuf->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, tmpResBuf->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			}
			break;
		case OdDxfCode::Bool:
			{
				bool val = tmpResChain->getBool();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				tmpResBuf->resval.rint = (val = true) ? 1 : 0;
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(tmpResBuf->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(tmpResBuf->resval.rint, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			}
			break;
		case OdDxfCode::Integer8:
			{
				short val = tmpResChain->getInt8();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				tmpResBuf->resval.rint = (int)val;
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(tmpResBuf->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(tmpResBuf->resval.rint, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			}
			break;
		case OdDxfCode::Integer16:
			{
				short val = tmpResChain->getInt16();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				tmpResBuf->resval.rint = (int)val;
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(tmpResBuf->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(tmpResBuf->resval.rint, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");

#endif
			}
			break;
		case OdDxfCode::Integer32:
			{
				long val = tmpResChain->getInt32();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				tmpResBuf->resval.rlong = (long)val;
#ifdef _DEBUG_MHB
			strcpy(psBuffer, "( ");
			itoa(tmpResBuf->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, " . ");

			itoa(tmpResBuf->resval.rlong, sBuf, 10);

			strcat(psBuffer, sBuf);
			strcat(psBuffer, " )");
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");
#endif
			}
			break;
		case OdDxfCode::Double:
		case OdDxfCode::Angle:
			{
				double val = tmpResChain->getDouble();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				tmpResBuf->resval.rreal = val;
			}
#ifdef _DEBUG_MHB
			{
				strcpy(psBuffer, "( " );
				itoa(tmpResBuf->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");

				double tmp = fabs(tmpResBuf->resval.rreal)>1.0e-11 ? tmpResBuf->resval.rreal : 0.0;
				_floatToString(tmp, sBuf, 4);

				strcat(psBuffer, sBuf);
				strcat(psBuffer, " )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");

			}
#endif
			break;
		case OdDxfCode::Point:
			{
				OdGePoint3d point3D = tmpResChain->getPoint3d();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				_fixZeroRealDWGdirect(point3D);
				tmpResBuf->resval.rpoint[0] = point3D.x;
				tmpResBuf->resval.rpoint[1] = point3D.y;
				tmpResBuf->resval.rpoint[2] = point3D.z;
#ifdef _DEBUG_MHB
			{
			strcpy(psBuffer, "( ");
			itoa(tmpResBuf->restype, sBuf, 10);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, "  ");

			double tmp = fabs(tmpResBuf->resval.rpoint[0])>1.0e-11 ? tmpResBuf->resval.rpoint[0] : 0.0;
			_floatToString(tmp, sBuf, 4);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, "  ");

			tmp = fabs(tmpResBuf->resval.rpoint[1])>1.0e-11 ? tmpResBuf->resval.rpoint[1] : 0.0;
			_floatToString(tmp, sBuf, 4);
			strcat(psBuffer, sBuf);
			strcat(psBuffer, "  ");

			tmp = fabs(tmpResBuf->resval.rpoint[2])>1.0e-11 ? tmpResBuf->resval.rpoint[2] : 0.0;
			_floatToString(tmp, sBuf, 4);        
			strcat(psBuffer, sBuf);
			strcat(psBuffer, ")");
			}
			OutputDebugString("\t");
			OutputDebugString(psBuffer);
			OutputDebugString("\n");
#endif
			}
			break;
		case OdDxfCode::BinaryChunk:
			{
				OdBinaryData binaryData = tmpResChain->getBinaryChunk();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				int dataSize = binaryData.length();
				tmpResBuf->resval.rbinary.clen = dataSize;
				tmpResBuf->resval.rbinary.buf = new char[dataSize];

				memcpy(tmpResBuf->resval.rbinary.buf, (void*)binaryData.asArrayPtr(), tmpResBuf->resval.rbinary.clen);
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(tmpResBuf->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				itoa(tmpResBuf->resval.rbinary.clen, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer,  ")");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			}
			break;
		case OdDxfCode::Handle:
			{
				OdDbHandle hand = tmpResChain->getObjectId(pDDdwg).getHandle();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;

				OdString str = hand.ascii();
				int strLen = str.getLength();
				tmpResBuf->resval.rstring = new char[strLen + 1];
				memset(tmpResBuf->resval.rstring, 0, strLen + 1);
				strncpy(tmpResBuf->resval.rstring, (char*)str.c_str(), strLen);
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(tmpResBuf->restype, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, tmpResBuf->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			}
			break;
		case OdDxfCode::ObjectId:
		case OdDxfCode::SoftPointerId:
		case OdDxfCode::HardPointerId:
		case OdDxfCode::SoftOwnershipId:
		case OdDxfCode::HardOwnershipId:
			{
				OdDbHandle hand = tmpResChain->getObjectId(pDDdwg).getHandle();
				tmpResChain = tmpResChain->next();

				tmpResBuf->rbnext = sds_newrb(type);
				tmpResBuf = tmpResBuf->rbnext;
				tmpResBuf->resval.rlname[0] = type;	// Flag to let us know the next resbuf in the chain
				tmpResBuf->resval.rlname[1] = 0L;	// is the handle for this id.

				tmpResBuf->rbnext = sds_newrb(5);
				tmpResBuf = tmpResBuf->rbnext;

				OdString str = hand.ascii();
				int strLen = str.getLength();
				tmpResBuf->resval.rstring = new char[strLen + 1];
				memset(tmpResBuf->resval.rstring, 0, strLen + 1);
				strncpy(tmpResBuf->resval.rstring, (char*)str.c_str(), strLen);
#ifdef _DEBUG_MHB
				strcpy(psBuffer, "( ");
				itoa(type, sBuf, 10);
				strcat(psBuffer, sBuf);
				strcat(psBuffer, " . ");
				strcat(psBuffer, "\"");
				strcat(psBuffer, tmpResBuf->resval.rstring);
				strcat(psBuffer, "\" )");
				OutputDebugString("\t");
				OutputDebugString(psBuffer);
				OutputDebugString("\n");
#endif
			}
			break;

		default:
			tmpResChain = tmpResChain->next();
			break;
		}		
	}

	tmpResBuf = retBuf;
	retBuf = retBuf->rbnext;
	
	tmpResBuf->rbnext = NULL;
	sds_relrb(tmpResBuf);	

#ifdef _DEBUG_MHB
	sprintf(psBuffer, "%d links created.\n", count);
	OutputDebugString(psBuffer);
#endif
	return retBuf;
}

//-----------------------------------------------------------------------------
//
int ReadWriteUtils::reportAuditInfo(db_drawing* pDwg, 
								stringArray& summaryList, 
								stringArray& errorList, short flags)
{
	ASSERT(pDwg != NULL);

	OdString formatedStr;

	OdString fname(pDwg->ret_pn());
	fname = fname.left(fname.getLength() - 4);
	fname += ".adt"/*DNT*/;
	
	FILE* pAuditFile = NULL;
	int count = errorList.length();
	
	if (flags & 0x02 && count)	// Save to file flag. If no errors, don't create the file.
	{
		pAuditFile = fopen(fname.c_str(), "w");
		ASSERT(pAuditFile != NULL);
	}

	// Use system date as the Date: field.
	time_t aclock;
	struct tm *newtime;
	time(&aclock);
	newtime = localtime(&aclock);  

	if (count)
	{
		for (int i = 0; i < count; i++)
		{
			formatedStr.format("\n%s", errorList[i]);

			::SendMessage(IcadSharedGlobals::GetMainHWND(), 
							ICAD_WNDACTION_PRINTF, 
								NULL, 
									(LPARAM)formatedStr.c_str() );
//			OutputDebugString(formatedStr.c_str());
//			OutputDebugString("\n");
		}
	}

	count = summaryList.length();
	if (count)
	{
		for (int i = 0; i < count; i++)
		{
			if (i == 0)
				formatedStr.format("\n\n%s   %s   %s", summaryList[i],pDwg->ret_pn(),asctime(newtime));
			else
				formatedStr.format("%s\n", summaryList[i]);

			::SendMessage(IcadSharedGlobals::GetMainHWND(), 
							ICAD_WNDACTION_PRINTF, 
								NULL, 
									(LPARAM)formatedStr.c_str() );

//			OutputDebugString(formatedStr.c_str());
//			OutputDebugString("\n");
		}
	}

	if (pAuditFile == NULL)
	{
		errorList.clear();
		summaryList.clear();
		return 0;
	}

	// We'll put the summary at the top of the output file.
	if (count)
	{
		for (int i = 0; i < count; i++)
		{
			formatedStr.format("%s", summaryList[i]);
			fwrite(formatedStr.c_str(), sizeof(char), strlen(formatedStr.c_str()), pAuditFile);

			if (i == 0)
			{
				fwrite("   ", sizeof(char), strlen("   "), pAuditFile);
				fwrite(pDwg->ret_pn(), sizeof(char), strlen(pDwg->ret_pn()), pAuditFile);
				fwrite("   ", sizeof(char), strlen("   "), pAuditFile);

				fwrite(asctime(newtime), sizeof(char), strlen(asctime(newtime)), pAuditFile);

			}
			fwrite("\n", sizeof(char), strlen("\n"), pAuditFile);
		}
	}


	count = errorList.length();
	if (count)
	{
		for (int i = 0; i < count; i++)
		{
			formatedStr.format("\n%s", errorList[i]);
			fwrite(formatedStr.c_str(), sizeof(char), strlen(formatedStr.c_str()), pAuditFile);
		}
	}


	errorList.clear();
	summaryList.clear();

	if (pAuditFile != NULL)
		fclose(pAuditFile);

	return count;
}

//-----------------------------------------------------------------------------
//
void ReadWriteUtils::_fixZeroRealDWGdirect(OdGeVector3d& pD)
{
	if(fabs(pD.x) < IC_ZRO)
		pD.x = 0.0;
	if(fabs(pD.y) < IC_ZRO)
		pD.y = 0.0;
	if(fabs(pD.z) < IC_ZRO)
		pD.z = 0.0;
}

//-----------------------------------------------------------------------------
//
void ReadWriteUtils::_fixZeroRealDWGdirect(OdGePoint3d& pD)
{
	if(fabs(pD.x) < IC_ZRO)
		pD.x = 0.0;
	if(fabs(pD.y) < IC_ZRO)
		pD.y = 0.0;
	if(fabs(pD.z) < IC_ZRO)
		pD.z = 0.0;
}

#ifdef _DEBUG_MHB
void _floatToString(double value, char *psBuffer, short prec)
{
	int dec, sign;
	short place = 0;
	char *psTemp = _fcvt(value, prec, &dec, &sign);
	memset(psBuffer, '\0', prec + 4);
	if (sign) {
		strcpy(psBuffer, "-");
		place++;
	}
    dec = (dec > 4) ? 4 : (dec < -4) ? -4 : dec;
	if (dec <= 0) {
		if (prec) {
			strcat(psBuffer, "0.");
			place += 2;
		} else {
			strcat(psBuffer, "0");
			place++;
		}
		memset(psBuffer + place, '0', abs(dec));
		strcat(psBuffer, psTemp);
	} else {
		strncat(psBuffer, psTemp, dec);
		if (prec)
			strcat(psBuffer, ".");
		strcat(psBuffer, psTemp + dec);
	}
}


#endif