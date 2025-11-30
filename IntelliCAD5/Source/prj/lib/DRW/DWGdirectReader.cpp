// DWGdirectReader.cpp: implementation of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////

#include "Db.h"
#include "sds.h"			// for sds_alert()
#include "CollectionClasses.h"
#include "../../ICAD/res/icadrc2.h"

#include "OdaCommon.h"
#include "OdToolKit.h"
#include "OdString.h"
#include "DbDatabase.h"

#include "ExSystemServices.h"
#include "ExHostAppServices.h"
#include "ModelerGeometryImpl.h"
#include "CollectionClasses.h"
#include "DWGdirectReader.h"
#include "Utilities.h"

// OdDb headers
#include "DbObject.h"
#include "RxObjectImpl.h"
#include "DbMaterial.h"

#include "RxVariantValue.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbLinetypeTable.h"
#include "DbLinetypeTableRecord.h"
#include "DbRegAppTable.h"
#include "DbRegAppTableRecord.h"
#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "DbTextStyleTable.h"
#include "DbTextStyleTableRecord.h"
#include "DbDimStyleTable.h"
#include "DbDimStyleTableRecord.h"
#include "DbViewportTable.h"
#include "DbViewportTableRecord.h"
#include "DbViewTable.h"
#include "DbViewTableRecord.h"
#include "DbUCSTable.h"
#include "DbUCSTableRecord.h"
#include "DbBlockBegin.h"
#include "DbBlockEnd.h"
#include "DbSymbolTableRecord.h"
#include "DbObjectIterator.h"
#include "DbDictionary.h"
#include "RxDictionary.h"

#include "GiLinetype.h"
#include "GiLayer.h"

#include "DbXrecord.h"
#include "DbMlineStyle.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbGroup.h"	// File name clash with ...lib\db\dbgroup.h
#include "DbIdBuffer.h"
#include "DbDictionaryVar.h"
#include "DbRasterImageDef.h"
#include "DbRasterImage.h"
#include "DbRasterVariables.h"
#include "DbSpatialFilter.h"
#include "DbSpatialIndex.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbSortentsTable.h"
#include "DbLayerIndex.h"
#include "DbDictionaryWithDefault.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h" 	// File name clash with ...lib\db\DbLayout.h
#include "DbPlaceHolder.h"
#include "DbWipeoutVariables.h"
#include "DbVbaProject.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbPlotSettings.h" // File name clash with ...lib\db\DbPlotSettings.h
#include "DbDimAssoc.h"
#include "DbProxyExt.h"
#include "GiGeometry.h"
#include "summinfo.h"
#include "OdCharMapper.h"
#include "DbViewport.h"
#include "DbSymUtl.h"
#include "DbDimension.h"
#include "DbLeader.h"
#include "DbTableStyle.h"
#include "ThumbnailImage.h"

// ICAD headers.
#include "objects.h"
#include "appid.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "ucstabrec.h"
#include "viewtabrec.h"
#include "vporttabrec.h"
#include "viewport.h"
#include "DbGroup.h"
#include "DbLayout.h"
#include "DbSortentsTable.h"
#include "DbProxyObject.h"
#include "DbBlockReference.h"
#include "GiClipBoundary.h"

#include "TTFInfoGetter.h"

#include "RxDictionary.h"
#include "OdString.h"
#include "CAseDLPNTableRecord.h"

// Compiling password protected r18 drawings loading support
#include "DWGdirectReaderCrypt.cpp"

#include "DWGdirectFiler.h"

#include "RxDynamicModule.h"

#include "DwfImport.h"
#include "DbAudit.h"

#include <time.h>

TOOLKIT_EXPORT OdRxDictionaryIteratorPtr odDbGetDrawingClasses(OdDbDatabase& database);

#ifdef _DEBUG_MHB
extern void _floatToString(double value, char *psBuffer, short prec);
#endif

proxyRefHandles g_proxyObjectReferences;

//////////////////////////////////////////////////////////////////////////////
/// Utility class to get db_drawing data from DWGdirect technology and to
/// populate IntelliCAD's db_drawing structure.
//////////////////////////////////////////////////////////////////////////////

static int setviewvars = 1;

//-----------------------------------------------------------------------------
DWGdirectReader::DWGdirectReader(OdRxObjectImpl<DWGdirectReaderServices>& exServices)
{
	try
	{
		m_pICdwg = NULL;
		m_pExServices = &exServices;
		odInitialize(m_pExServices);
		
		odrxInitReaderCrypt();

		// Initialize the protocol extension.
		m_TheProtocolExtensions.initialize();

		g_proxyObjectReferences.clear();
	}
	catch (OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
		int error = err.code();
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		ASSERT(!"DWGdirectReader initialzation error: ");
#endif
	}
}

//-----------------------------------------------------------------------------
DWGdirectReader::~DWGdirectReader()
{
	try
	{
		m_TheProtocolExtensions.uninitialize();

		//odrxUninitModelerGeometry();
		odrxUninitReaderCrypt();
		odUninitialize();
	}
	catch (OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
		int error = err.code();
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		ASSERT(!"DWGdirectReader uninitialzation error: ");
#endif
	}
}

//-----------------------------------------------------------------------------
bool DWGdirectReader::readDiskFile(char* fileName, db_drawing* drawing, short bIsRecovering)
{
	bool retVal = false;

	OdString fileReadName;
	OdDbDatabasePtr pDDdwg;
	char qualifiedName[DB_PATHNAMESZ];
	char errorBuffer[IC_ACADBUF] = {""};
	bool errorUnsupportedVersion = false;

	try
	{
		ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModelerModule);
		ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdOleStorageModule);

		ODRX_BEGIN_STATIC_MODULE_MAP()
			ODRX_DEFINE_STATIC_APPLICATION("ModelerGeometry",  ModelerModule)
			ODRX_DEFINE_STATIC_APPLICATION("OdOleStorage",     OdOleStorageModule)
		ODRX_END_STATIC_MODULE_MAP()

		ODRX_INIT_STATIC_MODULE_MAP();

		m_pICdwg = drawing;

		// Mostly for xref's without fully qualified file names.
		bool bNeedsRecover = false;
		bool fileFound = ic_findinpath(NULL, fileName, qualifiedName, DB_PATHNAMESZ) == RTNORM;
		if(fileFound == true)
		{			
			OdString file(qualifiedName);
			file.makeUpper();

			fileReadName = qualifiedName;
			if (file.find(".DWF") == (file.getLength() -4))
			{		
				pDDdwg = m_pExServices->createDatabase();
				importDwf(pDDdwg, qualifiedName);

				if (bIsRecovering & 0x01)
				{
					try
					{
						m_pExServices->setFixErrors(true);
						pDDdwg->auditDatabase(m_pExServices);
						
						ReadWriteUtils::reportAuditInfo(m_pICdwg, m_pExServices->summaryList, 
																  m_pExServices->errorList,
																  bIsRecovering);
					}
					catch (OdError& err)
					{
						pDDdwg = OdDbDatabasePtr();
						OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
						sprintf(errorBuffer, "Error: %s\n\n", msg.c_str());
					}
					catch(...)
					{
						pDDdwg = OdDbDatabasePtr();
					}
				}
			}
			else
			{
				try
				{
					if (bIsRecovering & 0x01)
					{
						m_pExServices->setFixErrors(true);
						pDDdwg = m_pExServices->recoverFile(m_pExServices->createFile(qualifiedName), m_pExServices);
						
						ReadWriteUtils::reportAuditInfo(m_pICdwg, m_pExServices->summaryList, 
																  m_pExServices->errorList,
																  bIsRecovering);
					}
					else
					{
						pDDdwg = m_pExServices->readFile(qualifiedName, false, false, Oda::kShareDenyNo);

						//m_pExServices->setFixErrors(true);
						//pDDdwg->auditDatabase(m_pExServices);
						//ReadWriteUtils::reportAuditInfo(m_pICdwg, m_pExServices->summaryList, 
						//										  m_pExServices->errorList,
						//										  0);

						if (pDDdwg.isNull())
							bNeedsRecover = true;
					}
				}
				catch (OdError& err)
				{
					pDDdwg = OdDbDatabasePtr();
					OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();

					if (OdString("Invalid Dwg Version").iCompare(msg) == 0)
					{
						errorUnsupportedVersion = true;
					}
					sprintf(errorBuffer, ResourceString(CMD_ERR_ERRORWITHSTRING, "Error: %s.\n\n"), msg.c_str());
				}
				catch(...)
				{
					pDDdwg = OdDbDatabasePtr();
				}
			}
		}

		ASSERT(m_pICdwg != NULL);
		if (pDDdwg.isNull())
		{
			if (fileFound == true)
			{
				char buf[IC_ACADBUF];
				if (errorUnsupportedVersion == true)
				{
					sprintf(buf, ResourceString(CMD_ERR_CANTOPENFILE, "Unable to open the requested file: ""%s""."),fileName);
					strcat(errorBuffer, buf); 
					strcat(errorBuffer, ResourceString(CMD_ERR_INVALIDFILEVERSION, "Only drawing versions 2.5 and higher are supported.\n"));
				}
				else if (bIsRecovering & 0x01)
				{
					///"%s\n\nUnable to RECOVER this drawing.\nIt contains incorrect or corrupted information.\nand is too damaged for repair."
					sprintf(buf, ResourceString(IDC_OPENCLOSE_RECOVER_MESSAGE_FAIL, "%s\n\nUnable to open this drawing......."),fileName); 
					strcat(errorBuffer, buf);
				}
				else
				{
					///"%s\n\nUnable to open this drawing.\nIt contains incorrect or corrupted information.\n\nThe RECOVER command may be able to restore\nundamaged material from this drawing."
					sprintf(buf, ResourceString(IDC_OPENCLOSE_RECOVER_MESSAGE, "%s\n\nUnable to open this drawing......."),fileName); 
					strcat(errorBuffer, buf);
				}

				sds_resbuf resBuf;
				IcadSharedGlobals::CallGetVar(DB_QEXPERT,&resBuf);
				if (resBuf.resval.rint < 5)
				{
					MessageBox(GetActiveWindow(), errorBuffer, ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
				}
				else
				{
					// Echo to the command line instead.
					IcadSharedGlobals::CallCmdMessage(errorBuffer);
				}
			}

			db_progressstop();
			return false;
		}

		int ver;
		switch((int)pDDdwg->originalFileVersion())
		{
		case OdDb::vAC025:
			ver = IC_ACAD25;
			break;
		case OdDb::vAC026:
			ver = IC_ACAD26;
			break;
		case OdDb::vAC09:
			ver = IC_ACAD9;
			break;
		case OdDb::vAC10:
			ver = IC_ACAD10;
			break;
		case OdDb::vAC12:
			ver = IC_ACAD11;
			break;
		case OdDb::vAC13:
			ver = IC_ACAD13;
			break;
		case OdDb::vAC14:
			ver = IC_ACAD14;
			break;
		case OdDb::vAC15:
			ver = IC_ACAD2000;
			break;
		case OdDb::vAC18:
			ver = IC_ACAD2004;
			break;
		default: 
			ver = IC_ACAD2004;
			break;
		}
		ASSERT(ver >= 0 && ver <= 8);
		m_pICdwg->init_FileVersion(ver);

		m_pICdwg->SetFileLoadedFromA2K((bool)(ver == 7));
	}
	catch (OdError& err)
	{
		OdError err1 = err;
#ifdef _DEBUG_MHB
		int error = err.code();
		if (error == 30) // 'Can't open file'	// Probably a missing xref.
		{
			char sBuf[100];
			sprintf(sBuf, "Can't open file: %s\n", fileName);
			OutputDebugString(sBuf);
		}
		else if (error == 128)	// User hit cancel on password dialog.
		{
			;// Do nothing.
		}
		else
		{
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader file read error: ");
		}
#endif
			db_progressstop();
	}
    catch (...)
    {
#ifdef _DEBUG_MHB
		ASSERT(!"DWGdirectReader file read error: unknown");
#endif
		db_progressstop();
	}

	if (m_pICdwg != NULL && !pDDdwg.isNull())
	{
		try
		{
			retVal = populateIcadDWG(pDDdwg, fileReadName);
			processDelayedEntities();
			ProcessProxyObjectReferences();
			setActiveLayoutTab(pDDdwg);
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			int error = err.code();
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader error populating IntelliCAD drawing: ");
#endif
			db_progressstop();
		}
	}

	return retVal;
}

//-----------------------------------------------------------------------------
bool DWGdirectReader::readMemoryFile(db_drawing* drawing, OdDbDatabasePtr& pDDdwg)
{
	bool retVal = false;
	ASSERT(drawing != NULL);
	ASSERT(!pDDdwg.isNull());

	if (pDDdwg.isNull())
	{
		db_progressstop();
		return false;
	}

	ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModelerModule);
	ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdOleStorageModule);

	ODRX_BEGIN_STATIC_MODULE_MAP()
		ODRX_DEFINE_STATIC_APPLICATION("ModelerGeometry",  ModelerModule)
		ODRX_DEFINE_STATIC_APPLICATION("OdOleStorage",     OdOleStorageModule)
	ODRX_END_STATIC_MODULE_MAP()

	ODRX_INIT_STATIC_MODULE_MAP();

	if (drawing != NULL && !pDDdwg.isNull())
	{
		try
		{
			m_pICdwg = drawing;

			retVal = populateIcadDWG(pDDdwg, OdString(m_pICdwg->ret_pn()));
			processDelayedEntities();
			ProcessProxyObjectReferences();
			setActiveLayoutTab(pDDdwg);

			retVal = true;
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			int error = err.code();
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader audit error populating IntelliCAD drawing: ");
#endif
			db_progressstop();
		}
	}

	return retVal;
}
// Note the order of these calls is important.
bool DWGdirectReader::populateIcadDWG(OdDbDatabasePtr& pDDdwg, OdString& fileReadName)
{
	// Transfer the thumbnail bitmap image data if any.
	try
	{
		TransferThumbnailBmpFromDWGdirect(pDDdwg, fileReadName);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferThumbnailBmpFromDWGdirect error populating OdDbDatabase: ");
	}

	// Transfere all of the stock handles.
	try
	{
		TransferStockHandlesFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferStockHandlesFromDWGdirect error populating OdDbDatabase: ");
	}

	try
	{
		TransferStartHandseedFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferStartHandseedFromDWGdirect error populating OdDbDatabase: ");
	}

	// Registered Applications.
	try
	{
		TransferRegAppsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferRegAppsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Styles & shapes.
	try
	{
		TransferStylesFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferStylesFromDWGdirect error populating OdDbDatabase: ");
	}

	CTTFInfoGetter::get().resolveLocalNames();

	// Linetypes.
	try
	{
		TransferLineTypesFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferLineTypesFromDWGdirect error populating OdDbDatabase: ");
	}

	// Layers.
	try
	{
		TransferLayersFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferLayersFromDWGdirect error populating OdDbDatabase: ");
	}

	// Blocks.
	try
	{
		TransferBlocksFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferBlocksFromDWGdirect error populating OdDbDatabase: ");
	}

	// Viewports
	try
	{
		TransferViewportsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferViewportsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Objects - including dictionaries
	try
	{
		TransferObjectsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferObjectsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Dimension styles.
	try
	{
		TransferDimStylesFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferDimStylesFromDWGdirect error populating OdDbDatabase: ");
	}

	// TODO MHB this may not be necessary?.
	try
	{
		FixPreR14dimblkDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("FixPreR14dimblkDWGdirect error populating OdDbDatabase: ");
	}

	// UCSs
	try
	{
		TransferUCSsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferUCSsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Views
	try
	{
		TransferViewsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferViewsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Modelspace entities
	try
	{
		TransferModelspaceEntsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferModelspaceEntsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Paperspace entities
	try
	{
		TransferPaperspaceEntsFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferPaperspaceEntsFromDWGdirect error populating OdDbDatabase: ");
	}

	// Entities in blocks
	try
	{
		TransferEntitiesInBlocksFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferEntitiesInBlocksFromDWGdirect error populating OdDbDatabase: ");
	}

	// System Variables
	try
	{
		TransferSystemVariablesFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferSystemVariablesFromDWGdirect error populating OdDbDatabase: ");
	}

	// World UCS
	try
	{
		TransferWorldUCSFromDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferWorldUCSFromDWGdirect error populating OdDbDatabase: ");
	}

	return true;
}

//-----------------------------------------------------------------------------
// File open preview bitmap.
void DWGdirectReader::TransferThumbnailBmpFromDWGdirect(OdDbDatabasePtr& pDDdwg, OdString& fileReadName)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdThumbnailImage image;
	odDbGetPreviewBitmap(m_pExServices->createFile(fileReadName), &image);
    if (image.hasBmp() || image.hasWmf()) {
        OdBinaryData binaryData = image.hasBmp() ? image.bmp : image.wmf;
	    OdUInt32 nDataLength = binaryData.length();
	    void* bitmapInfo = (void*)binaryData.asArrayPtr(); 

        if (image.hasBmp())
	        m_pICdwg->set_imagebufbmpDWGdirect(bitmapInfo,nDataLength);
        else
            m_pICdwg->set_imagebufwmf((char*)bitmapInfo, nDataLength);
    }
	return;
}

//-----------------------------------------------------------------------------
// Fix up stock handle table.
void DWGdirectReader::TransferStockHandlesFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdString   stringHandle;

	stringHandle = pDDdwg->getBlockTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_BLKREC);
	stringHandle = pDDdwg->getLayerTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_LAYER);
	stringHandle = pDDdwg->getTextStyleTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_STYLE);
	stringHandle = pDDdwg->getLinetypeTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_LTYPE);
	stringHandle = pDDdwg->getViewTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_VIEW);
	stringHandle = pDDdwg->getUCSTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_UCS);
	stringHandle = pDDdwg->getViewportTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_VPORT);
	stringHandle = pDDdwg->getRegAppTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_APPID);
	stringHandle = pDDdwg->getDimStyleTableId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_DIMSTYLE);

	// The note in drawing.cpp around line 150, says this isn't used.
	// It is hard coded there to "B", so I'll do it here for consistancy.
	stringHandle = "B";
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_TAB_VX);

	// Modelspace block handles
	OdDbObjectId objId = pDDdwg->getModelSpaceId();
	OdDbBlockTableRecordPtr pMSBlock = objId.safeOpenObject();

	// Modelspace block header
	stringHandle = objId.getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_BLKREC_MSPACE);
	// Modelspace block entity
	stringHandle = pMSBlock->openBlockBegin()->objectId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_BLK_MSPACE);
	// Modelspace ENDBLK entity			
	stringHandle = pMSBlock->openBlockEnd()->objectId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_ENDBLK_MSPACE);

	// Paperspace handles
	objId = pDDdwg->getPaperSpaceId();
	OdDbBlockTableRecordPtr pPSBlock = objId.safeOpenObject();

	// Paperspace block header
	stringHandle = objId.getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_BLKREC_PSPACE);
	// Paperspace block entity
	stringHandle = pPSBlock->openBlockBegin()->objectId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_BLK_PSPACE);
	// Paperspace ENDBLK entity			
	stringHandle = pPSBlock->openBlockEnd()->objectId().getHandle().ascii();
	m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_ENDBLK_PSPACE);

	return;
}

//-----------------------------------------------------------------------------
// 
void DWGdirectReader::FixPreR14dimblkDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	int fileVersion = m_pICdwg->ret_FileVersion();

	OdResBufPtr dimBLK = pDDdwg->getSysVar("DIMBLK");
	OdDbObjectId id = dimBLK->getObjectId(pDDdwg);
	OdString dimBlk_Name = GetBlockNameFromDWGdirectId(id);

	OdResBufPtr dimBLK1 = pDDdwg->getSysVar("DIMBLK1");
	id = dimBLK1->getObjectId(pDDdwg);
	OdString dimBlk1_Name = GetBlockNameFromDWGdirectId(id);

	OdResBufPtr dimBLK2 = pDDdwg->getSysVar("DIMBLK2");
	id = dimBLK2->getObjectId(pDDdwg);
	OdString dimBlk2_Name = GetBlockNameFromDWGdirectId(id);

	OdResBufPtr dimLdrBLK = pDDdwg->getSysVar("DIMLDRBLK");
	id = dimLdrBLK->getObjectId(pDDdwg);
	OdString dimLdrBLK_Name = GetBlockNameFromDWGdirectId(id);

	struct sds_resbuf resBuf;
    resBuf.restype=RTSTR;

	if (fileVersion == OdDb::kDHL_1013)
	{
		resBuf.resval.rstring = (char*)dimBlk_Name.c_str();
		db_setvar(NULL,DB_QDIMBLK, &resBuf,m_pICdwg,NULL,NULL,0);

		resBuf.resval.rstring = (char*)dimBlk1_Name.c_str();
		db_setvar(NULL,DB_QDIMBLK1, &resBuf,m_pICdwg,NULL,NULL,0);

		resBuf.resval.rstring = (char*)dimBlk2_Name.c_str();
		db_setvar(NULL,DB_QDIMBLK2, &resBuf,m_pICdwg,NULL,NULL,0);
	}

	if (fileVersion >= OdDb::kDHL_1013)
	{
		resBuf.resval.rstring = (char*)dimLdrBLK_Name.c_str();
		db_setvar(NULL,DB_QDIMLDRBLK, &resBuf,m_pICdwg,NULL,NULL,0);
	}

	if (fileVersion < OdDb::kDHL_1013)
	{
        // before R14, DIMLDRBLK is decided by combination with
        // DIMBLK, DIMBLK1, and DIMSAH.
        //  Determine what kind of "arrow" we're going to draw (if everything
        //  works out).  Set blockname to NULL for SOLID, or to dimblk or
        //  dimblk1.  ACAD uses the following logic:
        //
        //  DIMBLK set  DIMBLK1 set  DIMSAH  Result
        //  ----------  -----------  ------  -------
        //       *           -          1    DIMBLK
        //       *           -          0    DIMBLK
        //       *           *          0    DIMBLK
        //       *           *          1    DIMBLK1
        //       -           *          1    DIMBLK1
        //       -           *          0    SOLID
        //       -           -          0    SOLID
        //       -           -          1    SOLID
		resBuf.resval.rstring = "";
		db_setvar(NULL,DB_QDIMLDRBLK, &resBuf,m_pICdwg,NULL,NULL,0);

		OdResBufPtr dimSAH  = pDDdwg->getSysVar("DIMSAH");
		OdInt32  dimSAH_val = dimSAH->getInt32();

		if (!dimBlk1_Name.isEmpty() && dimSAH_val == 1) 
		{
			resBuf.resval.rstring = (char*)dimBlk1_Name.c_str();
			db_setvar(NULL,DB_QDIMLDRBLK, &resBuf,m_pICdwg,NULL,NULL,0);
		}
		else if (!dimBlk_Name.isEmpty())
		{
			resBuf.resval.rstring = (char*)dimBlk_Name.c_str();
			db_setvar(NULL,DB_QDIMLDRBLK, &resBuf,m_pICdwg,NULL,NULL,0);
		}
	}

	return;
}

//-----------------------------------------------------------------------------
// Don't get the handseed from the db_drawing header it may not be correct because
// marcomp has generated new handles for the dictionary objects.
void DWGdirectReader::TransferStartHandseedFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	// Returns the next available unused handle value for pDp.
	OdDbHandle handle = pDDdwg->handseed();
	OdString stringHandle = handle.ascii();

	struct sds_resbuf resBuf;
    resBuf.resval.rstring = (char*)stringHandle.c_str();
    resBuf.restype=RTSTR;

    if(resBuf.resval.rstring) 
		db_setvar(NULL,DB_QHANDSEED, &resBuf,m_pICdwg,NULL,NULL,0);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferBlocksFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	// This call ensures that the Named object dictionary is the first
	// thing in the db_drawing's objll.
	GetNameObjectDictionaryFromDWGdirect(pDDdwg);

	OdDbBlockTablePtr pTable = pDDdwg->getBlockTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	OdDbObjectId idMsSpaceBlock = pDDdwg->getModelSpaceId();
	m_pICdwg->set_stockhand(db_objhandle(idMsSpaceBlock.getHandle().ascii()),DB_SHI_BLKREC_MSPACE);

	OdDbObjectId idPsSpaceBlock = pDDdwg->getPaperSpaceId();
	m_pICdwg->set_stockhand(db_objhandle(idPsSpaceBlock.getHandle().ascii()),DB_SHI_BLKREC_PSPACE);

	db_dictionary* pExtensionDictionary = NULL;
	for (pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		db_blocktabrec *pNewBlock = NULL;

		// Inerator skips deleted blocks by default.
		OdDbObjectId curBlockId = pTableIter->getRecordId();
		OdDbBlockTableRecordPtr pBlockRec = curBlockId.safeOpenObject();
		
		OdDbObjectId extDictId = pBlockRec->extensionDictionary();

		OdString blockName = pBlockRec->getName();

		// Don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_BLOCKTAB, blockName.c_str(), 0))
			continue;

		if (strisame(OdDbSymUtil::blockModelSpaceName(), blockName))
		{
			pNewBlock = new db_blocktabrec((char*)blockName.c_str(), m_pICdwg);

			OdDbObjectId msLayoutId = OdDbBlockTableRecordPtr(idMsSpaceBlock.safeOpenObject())->getLayoutId();
			ASSERT(!msLayoutId.isNull());
			if (!msLayoutId.isNull())
			{	
				m_pICdwg->set_stockhand(db_objhandle(msLayoutId.getHandle().ascii()),DB_SHI_MSLAYOUT);
			}

			if (!extDictId.isNull())
			{
				pExtensionDictionary = new db_dictionary();
				pExtensionDictionary->set_280(1);	
				pExtensionDictionary->set_281(1);
				m_pICdwg->addhanditem(pExtensionDictionary);

				m_pICdwg->set_xdict(0, DB_HARD_POINTER, pExtensionDictionary->RetHandle(), pExtensionDictionary);
			}
		}
		else if (strisame(OdDbSymUtil::blockPaperSpaceName(), blockName))
		{
			pNewBlock = new db_blocktabrec((char*)blockName.c_str(), m_pICdwg);

			OdDbObjectId psLayoutId = OdDbBlockTableRecordPtr(idPsSpaceBlock.safeOpenObject())->getLayoutId();
			ASSERT(!psLayoutId.isNull());
			if (!psLayoutId.isNull())
			{
				m_pICdwg->set_stockhand(db_objhandle(psLayoutId.getHandle().ascii()),DB_SHI_PSLAYOUT);
			}

			if (!extDictId.isNull())
			{
				pExtensionDictionary = new db_dictionary();
				pExtensionDictionary->set_280(1);	
				pExtensionDictionary->set_281(1);
				m_pICdwg->addhanditem(pExtensionDictionary);

				m_pICdwg->set_xdict(1, DB_HARD_POINTER, pExtensionDictionary->RetHandle(), pExtensionDictionary);
			}
		}
		else
		{
			pNewBlock = new db_blocktabrec((char*)blockName.c_str(), m_pICdwg);

			if (!extDictId.isNull())
			{
				pExtensionDictionary = new db_dictionary();
				pExtensionDictionary->set_280(1);	
				pExtensionDictionary->set_281(1);
				m_pICdwg->addhanditem(pExtensionDictionary);

				pNewBlock->set_xdict(DB_HARD_POINTER, pExtensionDictionary->RetHandle(), pExtensionDictionary);
			}

		}	

		ASSERT(pNewBlock != NULL);
		TransferHandleFromDWGdirect(pNewBlock, pBlockRec);

		sds_point base;
		CPYPT3D(base, pBlockRec->origin());
		pNewBlock->set_10(base);

		// Pretty ugly way to get flags!!!!
		int flags = 0;
		if (pBlockRec->isAnonymous())
			flags |= BIT1;
		if (pBlockRec->hasAttributeDefinitions())
			flags |= BIT2;
		if (pBlockRec->isFromExternalReference())
			flags |= BIT4;
		if (pBlockRec->isFromOverlayReference())
			flags |= BIT80;
		pNewBlock->set_70(flags);
		pNewBlock->set_71(pBlockRec->isUnloaded());

		OdString description = pBlockRec->comments();
        pNewBlock->set_description((char*)description.c_str());

		OdString pathName = pBlockRec->pathName();
		pNewBlock->set_1((char*)pathName.c_str());

		// Extension Dictionary
		if (!extDictId.isNull() && pExtensionDictionary != NULL)
		{
			OdDbDictionaryPtr pExtDictionary= extDictId.safeOpenObject();
			AddDictionaryRecords(pExtensionDictionary, pExtDictionary, DB_SOFT_OWNER);

			ProcessDictionaryRecords(pExtDictionary, m_pICdwg, pExtensionDictionary);
		}
		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewBlock, pBlockRec);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewBlock, pBlockRec, DB_SOFT_POINTER);

		// Layout Id.
		OdDbObjectId layoutId = pBlockRec->getLayoutId();
		if (!layoutId.isNull())
		{
#ifdef _DEBUG_MHB
			OdString layoutName = ((OdDbLayoutPtr)layoutId.safeOpenObject())->getLayoutName();
			OdString vp = ((OdDbLayoutPtr)layoutId.safeOpenObject())->activeViewportId().getHandle().ascii();
#endif
			OdString stringHandle = layoutId.getHandle().ascii();
			pNewBlock->set_layoutobjhandle(db_objhandle(stringHandle.c_str()));
		}

		pNewBlock->set_deleted(0);

        m_pICdwg->addhanditem(pNewBlock);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferRegAppsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbRegAppTablePtr pTable = pDDdwg->getRegAppTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for (pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbRegAppTableRecordPtr pAppRec = pTableIter->getRecordId().safeOpenObject();
		OdString appName = pAppRec->getName();

		if (appName.iCompare(db_str_acad) == 0)
		{
			OdString stringHandle = pAppRec->objectId().getHandle().ascii();
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_APPID_ACAD);
		}

		// Don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_APPIDTAB, appName.c_str(),0))
			continue;

		// If we got this far, we need to add the rest of the application table records
		// to the current db_drawing.
		db_handitem* pNewApp = new db_appidtabrec((char*)appName.c_str(),m_pICdwg);

		// TODO MHB more??
		int flags = 0;
		if (pAppRec->isDependent())
			flags |= BIT10;
		if (pAppRec->isResolved())
			flags |= BIT20;
		pNewApp->set_70(flags);

		// Handle
		TransferHandleFromDWGdirect(pNewApp, pAppRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewApp, pAppRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewApp, pAppRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewApp, pAppRec);

#ifdef _DEBUG_MHB
		OdString stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewApp);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferExtendedEntityDataFromDWGdirect(OdResBuf* eed, db_handitem* ent, db_drawing* pDwg, OdDbDatabase* pDDdwg)
{
	ASSERT(eed != NULL);
	ASSERT(pDwg != NULL);
	ASSERT(ent != NULL);
	ASSERT(pDDdwg != NULL);

    struct sds_resbuf *extendedData = sds_newrb(-3);
	extendedData->rbnext = ReadWriteUtils::getSdsResBufPtrFromOdResBuf(eed, pDDdwg);

    // If the type of drawing is R14, eed must change to 2000, From drwfns.cpp
    ChangeEedAfterLoading(pDwg, ent, extendedData);

    ent->set_eed(extendedData,pDwg);

    sds_relrb(extendedData);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferStylesFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());


    OdDbSymbolTablePtr pTable = pDDdwg->getTextStyleTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbTextStyleTableRecordPtr pStyleRec = pTableIter->getRecordId().safeOpenObject();
		OdString styleName = pStyleRec->getName();

		// If we are inserting into a drawing (DXFIN) don't overwrite existing tables.
		if (styleName.isEmpty() || m_pICdwg->findtabrec(DB_STYLETAB, styleName.c_str(),0))
			continue;

		if (styleName.iCompare(OdDbSymUtil::textStyleStandardName()) == 0)
		{
			OdString stringHandle = pStyleRec->objectId().getHandle().ascii();
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_STYLE_STD);
		}

		// If we got this far, we need to add the rest fo the style table records
		// to the current db_drawing.
		db_handitem* newStyle = new db_styletabrec((char*)styleName.c_str(),m_pICdwg);
		
		int flags = 0;
		// TODO MHB Are there more?
		if (pStyleRec->isShapeFile())
			flags |= BIT1;
		if (pStyleRec->isVertical())
			flags |= BIT4;
		if (pStyleRec->isDependent())
			flags |= BIT10;
		if (pStyleRec->isResolved())
			flags |= BIT20;
        newStyle->set_70(flags);

		OdString fontName;
		fontName = pStyleRec->fileName();
		if (fontName.isEmpty())
			fontName = "SIMPLEX";	// default.
        newStyle->set_3((char*)fontName.c_str(),m_pICdwg);
		// Bigfont name.
		fontName = pStyleRec->bigFontFileName();
		newStyle->set_4((char*)fontName.c_str(),m_pICdwg);
		
		newStyle->set_40(pStyleRec->textSize());
		newStyle->set_41(pStyleRec->xScale());
		newStyle->set_42(pStyleRec->priorSize());
		newStyle->set_50(pStyleRec->obliquingAngle());

		flags = 0;
		// TODO MHB Are there more?
		if (pStyleRec->isBackwards())
			flags |= BIT2;
		if (pStyleRec->isUpsideDown())
			flags |= BIT4;
		newStyle->set_71(flags);

		// Handle
		TransferHandleFromDWGdirect(newStyle, pStyleRec);

		// Ownership ptr to extension dictionary.
		TransferExtensionDictionariesFromDWGdirect(newStyle, pStyleRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, newStyle, pStyleRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, newStyle, pStyleRec);

		((db_styletabrec *)newStyle)->setFontFamilyEED();

#ifdef _DEBUG_MHB
		OdString stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(newStyle);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferLineTypesFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

    OdDbSymbolTablePtr pTable = pDDdwg->getLinetypeTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbLinetypeTableRecordPtr pLTypeRec = pTableIter->getRecordId().safeOpenObject();
		OdString lineTypeName = pLTypeRec->getName();

		// If we are inserting into a drawing (DXFIN) don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_LTYPETAB, lineTypeName.c_str(),0))
			continue;

		OdString stringHandle = pLTypeRec->objectId().getHandle().ascii();
		if (lineTypeName.iCompare(OdDbSymUtil::linetypeByBlockName()) == 0)
		{
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_LTYPE_BB);
		}
		else if (lineTypeName.iCompare(db_str_bylayer) == 0)
		{
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_LTYPE_BL);
		}
		else if (lineTypeName.iCompare(db_str_continuous) == 0)
		{
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_LTYPE_CONT);
		}

		// If we got this far, we need to add the rest fo the line type table records
		// to the current db_drawing.
		db_ltypetabrec* pNewLType = new db_ltypetabrec((char*)lineTypeName.c_str(),m_pICdwg);

		OdString lTypeDesc;
		lTypeDesc = pLTypeRec->comments();
		pNewLType->set_3((char*)lTypeDesc.c_str());

		pNewLType->set_40(pLTypeRec->patternLength());
		pNewLType->set_73(pLTypeRec->numDashes());
		pNewLType->set_72((pLTypeRec->isScaledToFit()) ? 'S' : 'A');

		int flags = 0;
//		if (pLTypeRec->isByBlock())
//			flags |= BIT1;
//		if (pLTypeRec->isByLayer())
//			flags |= BIT2;
		if (pLTypeRec->isDependent())
			flags |= BIT10;
		if (pLTypeRec->isResolved())
			flags |= BIT20;
		pNewLType->set_70(flags);

		for (int i = 0; i < pLTypeRec->numDashes(); i++)
		{
			int dashFlag = 0;
//			if (ltDash.isRotationAbsolute() == true)
//				dashFlag |= BIT1;
//			if (ltDash.isEmbeddedTextString() == true)
//				dashFlag |= BIT2;
//			if (ltDash.isEmbeddedShape() == true)
//				dashFlag |= BIT4;

			db_objhandle temphandle = db_objhandle(pLTypeRec->shapeStyleAt(i).getHandle().ascii().c_str());
			OdString text = pLTypeRec->textAt(i);
			OdGeVector2d XYOffset = pLTypeRec->shapeOffsetAt(i);
			
			((db_ltypetabrec *)pNewLType)->set_dashdata(i,
									(pLTypeRec->textAt(i).isEmpty()) ? NULL : (char*)pLTypeRec->textAt(i).c_str(),
									XYOffset.x,
									XYOffset.y,
									pLTypeRec->shapeScaleAt(i),
									pLTypeRec->dashLengthAt(i),
									pLTypeRec->shapeRotationAt(i),
									dashFlag,
									pLTypeRec->shapeNumberAt(i), 
									temphandle,
									m_pICdwg);
		}

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewLType, pLTypeRec);

		// Handle
		TransferHandleFromDWGdirect(pNewLType, pLTypeRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewLType, pLTypeRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewLType, pLTypeRec, DB_SOFT_POINTER);

#ifdef _DEBUG_MHB
		stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewLType);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferLayersFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());
	
	OdDbLayerTablePtr pTable = pDDdwg->getLayerTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbLayerTableRecordPtr pLayerRec = pTableIter->getRecordId().safeOpenObject();
		OdString layerName = pLayerRec->getName();

		// Don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_LAYERTAB, layerName.c_str(), 0))
			continue;

		if (layerName.iCompare(OdDbSymUtil::layerZeroName()) == 0)
		{
			OdString stringHandle = pLayerRec->objectId().getHandle().ascii();
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_LAYER_0);
		}

		// If we got this far, we need to add the rest fo the layer table records
		// to the current db_drawing.
		db_layertabrec* pNewLayer = new db_layertabrec((char*)layerName.c_str(),m_pICdwg);

		// Layer linetype.
		OdString lineTypeName = GetLinetypeNameFromDWGdirectId(pLayerRec->linetypeObjectId());
		pNewLayer->set_6((char*)lineTypeName.c_str(), m_pICdwg);

		// Color
		int color = pLayerRec->color().colorIndex();
		color *= (pLayerRec->isOff() == true) ? -1 : 1;
		pNewLayer->set_62(color);

		// TODO MHB more flags ????
		int flags = 0;
		if (pLayerRec->isFrozen())
			flags |= BIT1;
		if (pLayerRec->VPDFLT())
			flags |= BIT2;
		if (pLayerRec->isLocked())
			flags |= BIT4;
		if (pLayerRec->isDependent())
			flags |= BIT10;
		if (pLayerRec->isResolved())
			flags |= BIT20;
		pNewLayer->set_70(flags);

        pNewLayer->set_290((pLayerRec->isPlottable() == true) ? 1 : 0);

        pNewLayer->set_370(pLayerRec->lineWeight());

		OdString stringHandle = pLayerRec->plotStyleNameId().getHandle().ascii();
		if (stringHandle.isEmpty())
		{
			pNewLayer->set_390(NULL);
		}
		else
		{
			m_pICdwg->AddHandleToFixupTable(db_objhandle(stringHandle.c_str()), (void**)pNewLayer->retp_plotstylehip());
		}
		if (m_pICdwg->ret_stockhand(DB_SHI_DICWDFLT) == NULL)
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_PLACEHOLDER);

		// Handle
		TransferHandleFromDWGdirect(pNewLayer, pLayerRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewLayer, pLayerRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewLayer, pLayerRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewLayer, pLayerRec);

#ifdef _DEBUG_MHB
		OdString tmpHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(tmpHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewLayer);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferDimStylesFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbDimStyleTablePtr pTable = pDDdwg->getDimStyleTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbDimStyleTableRecordPtr pDimStyleRec = pTableIter->getRecordId().safeOpenObject();
		OdString dimStyleName = pDimStyleRec->getName();

		// Don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_DIMSTYLETAB, dimStyleName.c_str(), 1))
			continue;

		db_dimstyletabrec* pNewDimStyle = new db_dimstyletabrec((char*)dimStyleName.c_str(),m_pICdwg);

		if (dimStyleName.iCompare(OdDbSymUtil::textStyleStandardName()) == 0)
		{
			OdString stringHandle = pDimStyleRec->objectId().getHandle().ascii();
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_DIMSTYLE_STD);
		}

		// TODO MHB more flags ????
		int flags = 0;
		if (pDimStyleRec->isDependent())
			flags |= BIT10;
		if (pDimStyleRec->isResolved())
			flags |= BIT20;
		pNewDimStyle->set_70(flags);

		pNewDimStyle->set_3((char*)pDimStyleRec->dimpost().c_str());
		pNewDimStyle->set_4((char*)pDimStyleRec->dimapost().c_str());

		OdDbHardPointerId hardPointerId;

		hardPointerId = pDimStyleRec->dimblk();				
		pNewDimStyle->set_5((char*)GetBlockNameFromDWGdirectId(hardPointerId).c_str());
		hardPointerId = pDimStyleRec->dimblk1();			
		pNewDimStyle->set_6((char*)GetBlockNameFromDWGdirectId(hardPointerId).c_str());
		hardPointerId = pDimStyleRec->dimblk2();		
		pNewDimStyle->set_7((char*)GetBlockNameFromDWGdirectId(hardPointerId).c_str());
		hardPointerId = pDimStyleRec->dimldrblk();			
		pNewDimStyle->set_341byname((char*)GetBlockNameFromDWGdirectId(hardPointerId).c_str(), m_pICdwg);

        pNewDimStyle->set_40(pDimStyleRec->dimscale());
        pNewDimStyle->set_41(pDimStyleRec->dimasz());
        pNewDimStyle->set_42(pDimStyleRec->dimexo());
        pNewDimStyle->set_43(pDimStyleRec->dimdli());
        pNewDimStyle->set_44(pDimStyleRec->dimexe());
        pNewDimStyle->set_45(pDimStyleRec->dimrnd());
        pNewDimStyle->set_46(pDimStyleRec->dimdle());
        pNewDimStyle->set_47(pDimStyleRec->dimtp());
        pNewDimStyle->set_48(pDimStyleRec->dimtm());
        pNewDimStyle->set_71(pDimStyleRec->dimtol());
        pNewDimStyle->set_72(pDimStyleRec->dimlim());
        pNewDimStyle->set_73(pDimStyleRec->dimtih());
        pNewDimStyle->set_74(pDimStyleRec->dimtoh());
        pNewDimStyle->set_75(pDimStyleRec->dimse1());
        pNewDimStyle->set_76(pDimStyleRec->dimse2());
        pNewDimStyle->set_77(pDimStyleRec->dimtad());
        pNewDimStyle->set_78(pDimStyleRec->dimzin());
		pNewDimStyle->set_79(pDimStyleRec->dimazin());
        pNewDimStyle->set_140(pDimStyleRec->dimtxt());
        pNewDimStyle->set_141(pDimStyleRec->dimcen());
        pNewDimStyle->set_142(pDimStyleRec->dimtsz());
        pNewDimStyle->set_143(pDimStyleRec->dimaltf());
        pNewDimStyle->set_144(pDimStyleRec->dimlfac());
        pNewDimStyle->set_145(pDimStyleRec->dimtvp());
        pNewDimStyle->set_146(pDimStyleRec->dimtfac());
        pNewDimStyle->set_147(pDimStyleRec->dimgap());
		pNewDimStyle->set_148(pDimStyleRec->dimaltrnd());
        pNewDimStyle->set_170(pDimStyleRec->dimalt());
        pNewDimStyle->set_171(pDimStyleRec->dimaltd());
        pNewDimStyle->set_172(pDimStyleRec->dimtofl());
        pNewDimStyle->set_173(pDimStyleRec->dimsah());
        pNewDimStyle->set_174(pDimStyleRec->dimtix());
        pNewDimStyle->set_175(pDimStyleRec->dimsoxd());
        pNewDimStyle->set_176(pDimStyleRec->dimclrd().colorIndex());
        pNewDimStyle->set_177(pDimStyleRec->dimclre().colorIndex());
        pNewDimStyle->set_178(pDimStyleRec->dimclrt().colorIndex());
		pNewDimStyle->set_179(pDimStyleRec->dimadec());
		// Obsolete using dimlunits.
		pNewDimStyle->set_270(pDimStyleRec->dimlunit());
        pNewDimStyle->set_271(pDimStyleRec->dimdec());
        pNewDimStyle->set_272(pDimStyleRec->dimtdec());
        pNewDimStyle->set_273(pDimStyleRec->dimaltu());
        pNewDimStyle->set_274(pDimStyleRec->dimalttd());
        pNewDimStyle->set_275(pDimStyleRec->dimaunit());
		pNewDimStyle->set_276(pDimStyleRec->dimfrac());
		pNewDimStyle->set_277(pDimStyleRec->dimlunit());
		pNewDimStyle->set_278(pDimStyleRec->dimdsep());
        pNewDimStyle->set_280(pDimStyleRec->dimjust());
        pNewDimStyle->set_281(pDimStyleRec->dimsd1());
        pNewDimStyle->set_282(pDimStyleRec->dimsd2());
        pNewDimStyle->set_283(pDimStyleRec->dimtolj());
        pNewDimStyle->set_284(pDimStyleRec->dimtzin());
        pNewDimStyle->set_285(pDimStyleRec->dimaltz());
        pNewDimStyle->set_286(pDimStyleRec->dimalttz());
		// TODO MHB VERIFY
		pNewDimStyle->set_287(pDimStyleRec->dimatfit());

        pNewDimStyle->set_288(pDimStyleRec->dimupt());
		// TODO MHB VERIFY
		pNewDimStyle->set_289(pDimStyleRec->dimatfit());

		pNewDimStyle->set_279(pDimStyleRec->dimtmove());
        pNewDimStyle->set_371(pDimStyleRec->dimlwd());
        pNewDimStyle->set_372(pDimStyleRec->dimlwe());

		pNewDimStyle->set_340byname((char*)GetTextStyleNameFromDWGdirectId(pDDdwg->dimtxsty()).c_str(), m_pICdwg);

		// Handle
		TransferHandleFromDWGdirect(pNewDimStyle, pDimStyleRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewDimStyle, pDimStyleRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewDimStyle, pDimStyleRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewDimStyle, pDimStyleRec);

#ifdef _DEBUG_MHB
		OdString stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewDimStyle);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferViewportsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbViewportTablePtr pTable = pDDdwg->getViewportTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	OdGePoint2d  point2D;
	OdGePoint3d  point3D;
	OdGeVector3d vector3D;

	sds_resbuf resBuf;
	bool bGotActiveViewport = false;
	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbViewportTableRecordPtr pViewportRec = pTableIter->getRecordId().safeOpenObject();
		OdString vportName = pViewportRec->getName();

		if (vportName.iCompare(OdDbSymUtil::viewportActiveName()) == 0 && bGotActiveViewport == false)
		{
			setviewvars = 0;

			OdString stringHandle = pViewportRec->objectId().getHandle().ascii();
			m_pICdwg->set_stockhand(db_objhandle(stringHandle.c_str()),DB_SHI_VPORT_STARACTIVE);

			point2D = pViewportRec->centerPoint();
			resBuf.restype = RT3DPOINT;
			resBuf.resval.rpoint[0] = point2D.x;
			resBuf.resval.rpoint[1] = point2D.y;
			resBuf.resval.rpoint[2] = 0.0;
			db_setvar(NULL, DB_QVIEWCTR, &resBuf, m_pICdwg, NULL, NULL, 1);

			vector3D = pViewportRec->viewDirection();
			ReadWriteUtils::_fixZeroRealDWGdirect(vector3D);
			resBuf.resval.rpoint[0] = vector3D.x;
			resBuf.resval.rpoint[1] = vector3D.y;
			resBuf.resval.rpoint[2] = vector3D.z;
			db_setvar(NULL, DB_QVIEWDIR, &resBuf, m_pICdwg, NULL, NULL, 1);

			point3D = pViewportRec->target();
			ReadWriteUtils::_fixZeroRealDWGdirect(point3D);
			resBuf.resval.rpoint[0] = point3D.x;
			resBuf.resval.rpoint[1] = point3D.y;
			resBuf.resval.rpoint[2] = point3D.z;
			db_setvar(NULL, DB_QTARGET, &resBuf, m_pICdwg, NULL, NULL, 1);

			point2D = pViewportRec->gridIncrements();
			resBuf.resval.rpoint[0] = point2D.x;
			resBuf.resval.rpoint[1] = point2D.y;
			resBuf.resval.rpoint[2] = 0.0;
			db_setvar(NULL, DB_QGRIDUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);

			point2D = pViewportRec->snapBase();
			resBuf.resval.rpoint[0] = point2D.x;
			resBuf.resval.rpoint[1] = point2D.y;
			resBuf.resval.rpoint[2] = 0.0;
			db_setvar(NULL, DB_QSNAPBASE, &resBuf, m_pICdwg, NULL, NULL, 0);

			point2D = pViewportRec->snapIncrements();
			resBuf.resval.rpoint[0] = point2D.x;
			resBuf.resval.rpoint[1] = point2D.y;
			resBuf.resval.rpoint[2] = 0.0;			
			db_setvar(NULL, DB_QSNAPUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);

			resBuf.restype=RTREAL;

			resBuf.resval.rreal = pViewportRec->viewTwist() ;
			db_setvar(NULL, DB_QVIEWTWIST, &resBuf, m_pICdwg, NULL, NULL, 0);

			resBuf.resval.rreal = pViewportRec->backClipDistance() ;  
			db_setvar(NULL, DB_QBACKZ, &resBuf, m_pICdwg, NULL, NULL, 0);

			resBuf.resval.rreal = pViewportRec->lensLength() ;
			db_setvar(NULL, DB_QLENSLENGTH, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rreal = pViewportRec->frontClipDistance();   
			db_setvar(NULL, DB_QFRONTZ, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rreal = pViewportRec->height();
			db_setvar(NULL, DB_QVIEWSIZE, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rreal = pViewportRec->snapAngle();     
			db_setvar(NULL, DB_QSNAPANG, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.restype=RTSHORT;
            resBuf.resval.rint = (pViewportRec->iconEnabled()) ? 1 : 0;      
			db_setvar(NULL, DB_QUCSICON, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rint = (pViewportRec->fastZoomsEnabled()) ? 1 : 0;     
			db_setvar(NULL, DB_QFASTZOOM, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rint = pViewportRec->circleSides() ;     
			db_setvar(NULL, DB_QZOOMPERCENT, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rint = (pViewportRec->gridEnabled()) ? 1 : 0;     
			db_setvar(NULL, DB_QGRIDMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

            resBuf.resval.rint = pViewportRec->snapPair();  
			db_setvar(NULL, DB_QSNAPISOPAIR, &resBuf, m_pICdwg, NULL, NULL, 0);

			// TODO MHB
//            resBuf.resval.rint    
//			db_setvar(NULL, DB_QSNAPMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

//            resBuf.resval.rint   
//			db_setvar(NULL, DB_QSNAPSTYL, &resBuf, m_pICdwg, NULL, NULL, 0);


			bGotActiveViewport = true;
		}

		db_vporttabrec* pNewViewport = new  db_vporttabrec((char*)vportName.c_str(), m_pICdwg);

		// TODO MHB more flags ????
		int flags = 0;
		if (pViewportRec->isDependent())
			flags |= BIT10;
		if (pViewportRec->isResolved())
			flags |= BIT20;
		pNewViewport->set_70(flags);

		sds_point sdsPoint;
		sds_point sdsTmpPoint1;
		sds_point sdsTmpPoint2;

		point2D = pViewportRec->lowerLeftCorner();
		sdsTmpPoint1[0] = point2D.x;
		sdsTmpPoint1[1] = point2D.y;
		sdsTmpPoint1[2] = 0.0;
        pNewViewport->set_10(sdsTmpPoint1);

 		point2D = pViewportRec->upperRightCorner();
		sdsTmpPoint2[0] = point2D.x;
		sdsTmpPoint2[1] = point2D.y;
		sdsTmpPoint2[2] = 0.0;
		pNewViewport->set_11(sdsTmpPoint2);

		point2D = pViewportRec->centerPoint();
		sdsPoint[0] = point2D.x;
 		sdsPoint[1] = point2D.y;
 		sdsPoint[2] = 0.0;
        pNewViewport->set_12(sdsPoint);

		point2D = pViewportRec->snapBase();
		sdsPoint[0] = point2D[0];
 		sdsPoint[1] = point2D[1];
 		sdsPoint[2] = 0.0;
        pNewViewport->set_13(sdsPoint);

		point2D = pViewportRec->snapIncrements();
		sdsPoint[0] = point2D[0];
 		sdsPoint[1] = point2D[1];
 		sdsPoint[2] = 0.0;
        pNewViewport->set_14(sdsPoint);

		point2D = pViewportRec->gridIncrements();
		sdsPoint[0] = point2D[0];
 		sdsPoint[1] = point2D[1];
 		sdsPoint[2] = 0.0;
        pNewViewport->set_15(sdsPoint);

		vector3D = pViewportRec->viewDirection();
		ReadWriteUtils::_fixZeroRealDWGdirect(vector3D);
		sdsPoint[0] = vector3D.x;
		sdsPoint[1] = vector3D.y;
		sdsPoint[2] = vector3D.z;
        pNewViewport->set_16(sdsPoint);
 
		point3D = pViewportRec->target();
		ReadWriteUtils::_fixZeroRealDWGdirect(point3D);
		sdsPoint[0] = point3D.x;
		sdsPoint[1] = point3D.y;
		sdsPoint[2] = point3D.z;
        pNewViewport->set_17(sdsPoint);

        pNewViewport->set_40(pViewportRec->height());
        pNewViewport->set_41(pViewportRec->width() / pViewportRec->height());
        pNewViewport->set_42(pViewportRec->lensLength());
        pNewViewport->set_43(pViewportRec->frontClipDistance());
        pNewViewport->set_44(pViewportRec->backClipDistance());
        pNewViewport->set_50(pViewportRec->snapAngle());
        pNewViewport->set_51(pViewportRec->viewTwist());

		flags = 0;
		if (pViewportRec->perspectiveEnabled() == true)
			flags |= BIT1;
		if (pViewportRec->frontClipEnabled() == true)
			flags |= BIT2;
		if (pViewportRec->backClipEnabled() == true)
			flags |= BIT4;
		if (pViewportRec->ucsFollowMode() == true)
			flags |= BIT8;
		if (pViewportRec->frontClipAtEye() == true)
			flags |= BIT10;
        pNewViewport->set_71(flags);

        pNewViewport->set_72(pViewportRec->circleSides());
        pNewViewport->set_73((pViewportRec->fastZoomsEnabled()) ? 1 : 0);

		flags = 0;
		if (pViewportRec->iconEnabled() == true)
			flags |= BIT1;
		if (pViewportRec->iconAtOrigin() == true)
			flags |= BIT2;
        pNewViewport->set_74(flags);

        pNewViewport->set_75((pViewportRec->snapEnabled()) ? 1 : 0);
        pNewViewport->set_76((pViewportRec->gridEnabled()) ? 1 : 0);

		// TODO MHB
        // pNewViewport->set_77(adtb.vport.snapstyle);

        pNewViewport->set_78(pViewportRec->snapPair());

		// Handle
		TransferHandleFromDWGdirect(pNewViewport, pViewportRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewViewport, pViewportRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewViewport, pViewportRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewViewport, pViewportRec);

#ifdef _DEBUG_MHB
		OdString stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewViewport);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferViewsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbViewTablePtr pTable = pDDdwg->getViewTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	sds_point sdsPoint;
	OdGeVector3d vector3D;
	OdGePoint3d point3D;
	OdGePoint2d point2D;

	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbViewTableRecordPtr pViewRec = pTableIter->getRecordId().safeOpenObject();
		OdString viewName = pViewRec->getName();

		// Don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_VIEWTAB, viewName.c_str(), 0))
			continue;

		db_viewtabrec* pNewView = new db_viewtabrec((char*)viewName.c_str(),m_pICdwg);

		// TODO MHB more flags ????
		int flags = 0;
		if (pViewRec->isDependent())
			flags |= BIT10;
		if (pViewRec->isResolved())
			flags |= BIT20;
		pNewView->set_70(flags);

		point2D = pViewRec->centerPoint(); 
		sdsPoint[0] = point2D.x;
		sdsPoint[1] = point2D.y;
		sdsPoint[2] = 0.0;
        pNewView->set_10(sdsPoint);

		vector3D = pViewRec->viewDirection();
		ReadWriteUtils::_fixZeroRealDWGdirect(vector3D);
		sdsPoint[0] = vector3D.x;
		sdsPoint[1] = vector3D.y;
		sdsPoint[2] = vector3D.z;
        pNewView->set_11(sdsPoint);

		point3D = pViewRec->target();
		ReadWriteUtils::_fixZeroRealDWGdirect(point3D);
		sdsPoint[0] = point3D.x;
		sdsPoint[1] = point3D.y;
		sdsPoint[2] = point3D.z;
        pNewView->set_12(sdsPoint);

        pNewView->set_40(pViewRec->height());
        pNewView->set_41(pViewRec->width());
        pNewView->set_42(pViewRec->lensLength());
        pNewView->set_43(pViewRec->frontClipDistance());
        pNewView->set_44(pViewRec->backClipDistance());
        pNewView->set_50(pViewRec->viewTwist());

		// TODO MHB more flags ????
		flags = 0;
		if (pViewRec->perspectiveEnabled())
			flags |= BIT1;
		if (pViewRec->frontClipEnabled())
			flags |= BIT2;
		if (pViewRec->backClipEnabled())
			flags |= BIT4;
		if (pViewRec->frontClipAtEye())
			flags |= BIT10;
        pNewView->set_71(flags);

		OdDbObjectId ucsId = pViewRec->ucsName();
		if (ucsId.isNull())
			pNewView->set_345(NULL);
		else
		{
			OdDbUCSTableRecordPtr ucsPtr = pViewRec->ucsName().safeOpenObject();
			OdString ucsName = ucsPtr->getName();
			pNewView->set_345(m_pICdwg->findtabrec(DB_UCSTAB, ucsName.c_str(), 0));
			pNewView->set_72(1);
		}
			
		// Handle
		TransferHandleFromDWGdirect(pNewView, pViewRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewView, pViewRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewView, pViewRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewView, pViewRec);

#ifdef _DEBUG_MHB
		OdString stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewView);
	}
		return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferUCSsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbUCSTablePtr pTable = pDDdwg->getUCSTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	sds_point sdsPoint;
	OdGeVector3d vector3D;
	OdGePoint3d point3D;
	OdGePoint2d point2D;
	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbUCSTableRecordPtr pUCSRec = pTableIter->getRecordId().safeOpenObject();
		OdString ucsName = pUCSRec->getName();

		// If we are inserting into a drawing (DXFIN) don't overwrite existing tables.
		if (m_pICdwg->findtabrec(DB_UCSTAB, ucsName.c_str(),0))
			continue;

		db_ucstabrec* pNewUCS = new db_ucstabrec((char*)ucsName.c_str(),m_pICdwg);

		// TODO MHB more flags ????
		int flags = 0;
		if (pUCSRec->isDependent())
			flags |= BIT10;
		if (pUCSRec->isResolved())
			flags |= BIT20;
		pNewUCS->set_70(flags);

		point3D = pUCSRec->origin() ;
 		sdsPoint[0] = point3D.x;
		sdsPoint[1] = point3D.y;
		sdsPoint[2] = point3D.z;
		pNewUCS->set_10(sdsPoint);
        //pNewUCS->set_146(point3D.z);          ???? could work?

		vector3D = pUCSRec->xAxis();
		sdsPoint[0] = vector3D.x;
		sdsPoint[1] = vector3D.y;
		sdsPoint[2] = vector3D.z;
        pNewUCS->set_11(sdsPoint);

		vector3D = pUCSRec->yAxis();
		sdsPoint[0] = vector3D.x;
		sdsPoint[1] = vector3D.y;
		sdsPoint[2] = vector3D.z;
        pNewUCS->set_12(sdsPoint);

		// TODO MHB
        //pNewUCS->set_79(adtb.ucs.orthoviewtype);
        //pNewUCS->set_71(adtb.ucs.orthographictype);
        //pNewUCS->set_146(adtb.ucs.elevation);

		// TODO MHB What is this??
		//m_pICdwg->AddHandleToFixupTable(adtb.ucs.orthoviewrefobjhandle, (void**)pNewUCS->retp_baseucs());

		// Handle
		TransferHandleFromDWGdirect(pNewUCS, pUCSRec);

		// Ownership.
		TransferExtensionDictionariesFromDWGdirect(pNewUCS, pUCSRec, m_pICdwg);

		// Reactors that may be on the style record.
		TransferReactorsFromDWGdirect(m_pICdwg, pNewUCS, pUCSRec, DB_SOFT_POINTER);

		// Extended entity data.
		TransferEEDataFromDWGdirect(m_pICdwg, pNewUCS, pUCSRec);

#ifdef _DEBUG_MHB
		OdString stringHandle = pTableIter->getRecordId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);
#endif
		m_pICdwg->addhanditem(pNewUCS);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferObjectsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	db_dictionary* pNamedDictionary = GetNameObjectDictionaryFromDWGdirect(pDDdwg);
	ASSERT(pNamedDictionary != NULL);

	OdDbDictionaryPtr pNamedObjectDictionary = pDDdwg->getNamedObjectsDictionaryId().safeOpenObject();
	OdDbDictionaryIteratorPtr  pNamedObjectDictionaryIter = pNamedObjectDictionary->newIterator();

	for(; !pNamedObjectDictionaryIter->done(); pNamedObjectDictionaryIter->next())
	{
		OdString objectName = pNamedObjectDictionaryIter->name();
		OdDbObjectPtr pObject = pNamedObjectDictionaryIter->objectId().safeOpenObject();
		db_objhandle objectHandle = db_objhandle(pObject->objectId().getHandle().ascii());

#ifdef _DEBUG_MHB
		OdString tmp;
		tmp.format("Reading - %s\n", objectName.c_str());
		OutputDebugString(tmp.c_str());
#endif

		if (pObject->isKindOf(OdDbDictionary::desc()))
		{
			OdDbDictionaryPtr pDict = (OdDbDictionaryPtr)pObject;

			if (objectName.iCompare("ACAD_GROUP") == 0)
			{
				// Create the group dictionary first.
				db_dictionary* pGroupDictionary = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pGroupDictionary != NULL);

				m_pICdwg->set_stockhand(objectHandle,DB_SHI_DICT_GROUP);

				// Now create the group record, and add it to the group dictionary.
				db_dictionaryrec* pGroupDictionaryRec = new db_dictionaryrec("ACAD_GROUP", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pGroupDictionaryRec != NULL);
				pNamedDictionary->addrec(pGroupDictionaryRec);

				// Add andy records this object might have to the group dictionary
				AddDictionaryRecords(pGroupDictionary, pDict, DB_SOFT_OWNER);

				CreateAllGroups(pGroupDictionaryRec, pDict);
			}
			else if (objectName.iCompare("ACAD_IMAGE_DICT") == 0)
			{
				// Create the image dictionary first.
				db_dictionary* pImageDictionary = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pImageDictionary != NULL);

				// Now create the image record, and add it to the image dictionary.
				db_dictionaryrec* pImageDictionaryRec = new db_dictionaryrec("ACAD_IMAGE_DICT", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pImageDictionaryRec != NULL);
				pNamedDictionary->addrec(pImageDictionaryRec);
				
				AddDictionaryRecords(pImageDictionary, pDict, DB_SOFT_OWNER);

				CreateAllImageDefs(pImageDictionaryRec, pDict);
			}
			else if (objectName.iCompare("ACAD_MLINESTYLE") == 0)
			{
				// Create the mline styles dictionary first.
				db_dictionary* pMLineStyleDictionary = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pMLineStyleDictionary != NULL);

				m_pICdwg->set_stockhand(objectHandle,DB_SHI_DICT_MLS);

				// Now create the mline style record, and add it to the mline styles dictionary.
				db_dictionaryrec* pMLineStyleDictionaryRec = new db_dictionaryrec("ACAD_MLINESTYLE", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pMLineStyleDictionaryRec != NULL);
				pNamedDictionary->addrec(pMLineStyleDictionaryRec);
				
				AddDictionaryRecords(pMLineStyleDictionary, pDict, DB_SOFT_OWNER);

				CreateAllMlineStyles(pMLineStyleDictionaryRec, pDict);
			}
			else if (objectName.iCompare("ACAD_LAYOUT") == 0)
			{
				// Create the layout dictionary first.
				db_dictionary* pLayoutDictionary = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pLayoutDictionary != NULL);

				m_pICdwg->set_stockhand(objectHandle,DB_SHI_LAYOUTDIC);

				// Now create the layout record, and add it to the layout dictionary.
				db_dictionaryrec* pLayoutDictionaryRec = new db_dictionaryrec("ACAD_LAYOUT", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pLayoutDictionaryRec != NULL);
				pNamedDictionary->addrec(pLayoutDictionaryRec);

				AddDictionaryRecords(pLayoutDictionary, pDict, DB_SOFT_OWNER);

				CreateAllLayouts(pLayoutDictionaryRec, pDict);
			}
			else if (objectName.iCompare("ACAD_PLOTSTYLENAME") == 0)
			{
				// Create the plotstylename dictionary first.
				db_dictionary* pACDBDICTIONARYWDFLT = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pACDBDICTIONARYWDFLT != NULL);

				AddDictionaryRecords(pACDBDICTIONARYWDFLT, pDict, DB_SOFT_OWNER);

				// Now create the plotstylename record, and add it to the plotstylename dictionary.
				db_dictionaryrec* pPlotStyleNameRec = new db_dictionaryrec("ACAD_PLOTSTYLENAME", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pPlotStyleNameRec != NULL);
				pNamedDictionary->addrec(pPlotStyleNameRec);

				CreateAllPlaceHolders(pDict);

				// TODO MHB figure out what to set this to.
//				if (tmpPlaceHolder != NULL)
//				{
//					m_pICdwg->AddHandleToFixupTable(tmpPlaceHolder->RetHandle(),(void **)((db_dictionarywdflt*)pACDBDICTIONARYWDFLT)->retp_defaultobject());
//					m_pICdwg->set_stockhand(tmpPlaceHolder->RetHandle(),DB_SHI_PLACEHOLDER);
//				}
			}

			else if (objectName.iCompare("ACAD_PLOTSETTINGS") == 0)
			{
				// Create the plotssettings dictionary first.
				db_dictionary* pPlotSettingsDictionary = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pPlotSettingsDictionary != NULL);

				m_pICdwg->set_stockhand(objectHandle,DB_SHI_PLOTSETTINGS);

				// Now create the plotssettings record, and add it to the plotssettings dictionary.
				db_dictionaryrec* pPlotSettingsRec = new db_dictionaryrec("ACAD_PLOTSETTINGS", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pPlotSettingsRec != NULL);
				pNamedDictionary->addrec(pPlotSettingsRec);

				AddDictionaryRecords(pPlotSettingsDictionary, pDict, DB_SOFT_OWNER);

				CreateAllPlotSettings(pPlotSettingsRec, pDict);
			}

			else
			{
				// Create the dictionary first.
				db_dictionary* pNewDictionary = CreateDictionary(m_pICdwg, pObject);
				ASSERT(pNewDictionary != NULL);

				// Now create the dictionary record, and add it to the dictionary.
				db_dictionaryrec* pNewDictionaryRec = new db_dictionaryrec((char*)objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pNewDictionaryRec != NULL);
				pNamedDictionary->addrec(pNewDictionaryRec);

				AddDictionaryRecords(pNewDictionary, pDict, DB_SOFT_OWNER);

				// Create the entries.
				ProcessDictionaryRecords((OdDbDictionaryPtr)pObject, m_pICdwg, (db_handitem*)NULL);

#ifdef _DEBUG_MHB
				char sBuf[100];
				sprintf(sBuf, "Dictionary %s created.\n", (char*)objectName.c_str());
				OutputDebugString(sBuf);
#endif
			}
		}
		else // Other Objects
		{
			// RASTERVARIABLES ////////////////////////////////////////////////////////////
			if (pObject->isKindOf(OdDbRasterVariables::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateRasterVariables(pObject);
			}
			// XRECORD OBJECT /////////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbXrecord::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateXRecord(m_pICdwg, pObject);
			}
			// IDBUFFER OBJECT ////////////////////////////////////////////////////////////
			 else if (pObject->isKindOf(OdDbIdBuffer::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateIDBuffer(m_pICdwg, pObject);
			}
			// DICTIONARYVAR //////////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbDictionaryVar::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateDictionaryVar(m_pICdwg, pObject);
			}
			// SPATIAL_FILTER /////////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbSpatialFilter::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateSpatialFilter(m_pICdwg, pObject);
			}
			// SPATIAL_INDEX ///////////////////////////////////////////////////////////////]
			else if (pObject->isKindOf(OdDbSpatialIndex::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateSpatialIndex(m_pICdwg, pObject);
			}
			// LAYER_INDEX ////////////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbLayerIndex::desc()))
			{
				//db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				//pNamedDictionary->addrec(pObjRec);
				//CreateLayerIndex(m_pICdwg, pObject);
			}
			// WIPEOUTVARIABLES OBJECT (Express Tools)/////////////////////////////////////
			else if (pObject->isKindOf(OdDbWipeoutVariables::desc()))
			{
				m_pICdwg->set_stockhand(objectHandle,DB_SHI_WIPEOUTVARIABLES);

				// Now create the wipeout vars record, and add it to the named object dictionary.
				db_dictionaryrec* pWipeoutVarsRec = new db_dictionaryrec("ACAD_WIPEOUT_VARS", DB_HARD_POINTER, objectHandle,NULL);
				ASSERT(pWipeoutVarsRec != NULL);
				pNamedDictionary->addrec(pWipeoutVarsRec);

				CreateWipeoutVariables(pObject);
			}
			// VBA_PROJECT OBJECT /////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbVbaProject::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateVBAproject(m_pICdwg, pObject);
 			}
			// PROXY OBJECT ///////////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbProxyObject::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateProxyObject(m_pICdwg, pObject);
			}
			// PROXY ENTITY ///////////////////////////////////////////////////////////////
			else if (pObject->isKindOf(OdDbProxyEntity::desc()))
			{
				ASSERT(!"Should never hit this but....");
			}
			// OBJECTPTR //////////////////////////////////////////////////////////////////
			// TODO MHB I suspect this as being bogus??
			else if (pObject->isKindOf(OdDbObject::desc()))
			{
				db_dictionaryrec* pObjRec = new db_dictionaryrec(objectName.c_str(), DB_HARD_POINTER, objectHandle,NULL);
				pNamedDictionary->addrec(pObjRec);
				CreateObject(m_pICdwg, pObject);
			}
			else
			{
				ASSERT(!"DWGdirectReader::TransferObjectsFromDWGdirect():  Don't know what this is?");
			}
		}
	}


	return;
}

//-----------------------------------------------------------------------------

void DWGdirectReader::TransferEntitiesInBlocksFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbBlockTablePtr pTable = pDDdwg->getBlockTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	OdDbObjectId idMsSpaceBlock = pDDdwg->getModelSpaceId();
	OdDbObjectId idPsSpaceBlock = pDDdwg->getPaperSpaceId();

	// I want to delay the conversion of the entities in the *Model_Space,
	// *Paper_Space and any block with an assoceated layout. This will 
	// allow all the other blocks to be converted before these are. For
	// example if you have a dimension on layout1, this will allow that
	// dimension block to be covnerted prior to the *Paper_Space??? block
	// when contains it. So we'll save of the layout owning blocks and
	// convert them last.
	delayedConversionMap delayedBlocks;

	for (pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		// Inerator skips deleted blocks by default.
		OdDbObjectId curBlockId = pTableIter->getRecordId();
		OdDbBlockTableRecordPtr pBlockRec = curBlockId.safeOpenObject();

		OdString blockName = pBlockRec->getName();

		db_handitem* pThisBlockTableRecord = m_pICdwg->findtabrec(DB_BLOCKTABREC, blockName.c_str(), 0);
		if (pThisBlockTableRecord == NULL)
		{
			continue;
		}

		db_objhandle handle;
		((db_blocktabrec*)pThisBlockTableRecord)->get_layoutobjhandle(&handle);
		if (handle != NULL)
		{
			delayedBlocks.insert(delayedBlocks.end(), handItemObjectIDPair(pThisBlockTableRecord, curBlockId, blockName.iCompare("*Model_Space") != 0));
		}
		else
		{
			populateBlock((db_blocktabrec*)pThisBlockTableRecord, pBlockRec, false);
		}
	}

	int delayedCount = delayedBlocks.size();
	for (int i = 0; i < delayedCount; i++)
	{
		handItemObjectIDPair tmp = delayedBlocks.getAt(i);
		
		OdDbObjectId odBlockRecId = tmp.getObjectId();
		db_handitem* pBlockTable = tmp.getHandItem();
		int space = tmp.getSpace();

		populateBlock((db_blocktabrec*)pBlockTable, odBlockRecId.safeOpenObject(), space);
	}

	return;
}

//------------------------------------------------------------------------------
//
void DWGdirectReader::populateBlock(db_blocktabrec* pBlockRec, OdDbBlockTableRecordPtr pOdDbBlockRec, int isPaperSpace)
{
	ASSERT(pBlockRec != NULL);
	ASSERT(!pOdDbBlockRec.isNull());

	OdString blockName = pOdDbBlockRec->getName();

	// Create the actual block.
	db_block* pBlock = new db_block();
	pBlock->set_2((char*)blockName.c_str(), m_pICdwg);

	sds_point base;
	CPYPT3D(base, pOdDbBlockRec->origin());
	pBlock->set_10(base);

	TransferHandleFromDWGdirect(pBlock, pOdDbBlockRec->openBlockBegin());
	// First thing in record!
	pBlockRec->addent(pBlock, m_pICdwg);

	// Create the ENDBLK to tack on at the end. Last thing in record!
	db_endblk* endBlk = new db_endblk(m_pICdwg);
	ASSERT(endBlk != NULL);

	// Layers
	OdString layerName = pOdDbBlockRec->openBlockBegin()->layer();
	OdString lineTypeName = pOdDbBlockRec->openBlockBegin()->linetype();
	pBlock->set_6((char*)lineTypeName.c_str(), m_pICdwg);
	pBlock->set_8((char*)layerName.c_str(), m_pICdwg);

	layerName = pOdDbBlockRec->openBlockEnd()->layer();
	lineTypeName = pOdDbBlockRec->openBlockEnd()->linetype();
	endBlk->set_6((char*)lineTypeName.c_str(), m_pICdwg);
	endBlk->set_8((char*)layerName.c_str(), m_pICdwg);

	TransferHandleFromDWGdirect(endBlk, pOdDbBlockRec->openBlockEnd());

	// No entities in *Model_Space nor *Paper_Space. So tack on the endBlk and continue.
	if (blockName.iCompare(OdDbSymUtil::blockModelSpaceName()) == 0 || blockName.iCompare(OdDbSymUtil::blockPaperSpaceName()) == 0)
	{
		pBlockRec->addent(endBlk,m_pICdwg);
		return;
	}

	delayedConversionMap delayedEntities;

	int viewportIndex = 1;
	OdDbObjectIteratorPtr pEntityIter = pOdDbBlockRec->newIterator();
	for (; !pEntityIter->done(); pEntityIter->step())
	{
		OdDbObjectId entId = pEntityIter->entity()->id();
		OdDbEntityPtr pEntity = entId.safeOpenObject();
		
		db_handitem* pEnt = NULL;
		try
		{
		if (pEntity->isKindOf(OdDbDimension::desc()) ||
			pEntity->isKindOf(OdDbHatch::desc()) ||
			pEntity->isKindOf(OdDbBlockReference::desc()) ||
			pEntity->isKindOf(OdDbLeader::desc()) )
			{
				delayedEntities.insert(delayedEntities.end(), handItemObjectIDPair(pBlockRec, entId, isPaperSpace));
			}
			else
			{
				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pEntCreator = pEntity;
				pEnt = pEntCreator->createHanditem(pEntity, m_pICdwg, pBlockRec);
				if (isPaperSpace)
				{
					// Set sub-ents space flag too.
					db_handitem* tmp = pEnt;
					while(tmp)
					{
						tmp->set_67(1);
						tmp = tmp->next;
					}
				}
#ifdef _DEBUG_MHB
				ASSERT(pEnt != NULL);
#endif
				if (pEnt == NULL)
					continue;

				if (isPaperSpace && pEntity->isKindOf(OdDbViewport::desc()))
				{
					pEnt->set_69(viewportIndex++);
				}
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			int error = err.code();
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirect block sub-entity creation error: ");
#endif
		}
	}
	processDelayedEntities(&delayedEntities);

	// Finish up with the endBlk.
	pBlockRec->addent(endBlk,m_pICdwg);

	// Now set the 1 and 3 group of the BLOCK. Block must 
	// be complete prior to doing this.
	OdString path = pOdDbBlockRec->pathName();
    pBlockRec->set_1((char*)path.c_str());
    pBlockRec->set_3((char*)path.c_str());

	return;
}


//------------------------------------------------------------------------------
//
void DWGdirectReader::TransferSystemVariablesFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	sds_resbuf resBuf;
	resBuf.rbnext = NULL;

	resBuf.restype = RTREAL;//-------------------------------------

	OdResBufPtr resBufPtr = pDDdwg->getSysVar("TDCREATE");
	resBuf.resval.rreal = resBufPtr->getDouble();
	db_setvar(NULL, DB_QTDCREATE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("TDUCREATE");
	// TODO MHB need a long[2] here ???
	//m_pICdwg->set_tducreate(resBufPtr->getInt32());

	resBufPtr = pDDdwg->getSysVar("TDINDWG");
	resBuf.resval.rreal = resBufPtr->getDouble();	
	db_setvar(NULL, DB_QTDINDWG, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("TDUPDATE");
	resBuf.resval.rreal = resBufPtr->getDouble();	
	db_setvar(NULL, DB_QTDUPDATE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("TDUUPDATE");
	resBuf.resval.rreal = resBufPtr->getDouble();	
	db_setvar(NULL, DB_QTDUUPDATE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("TDUSRTIMER");
	resBuf.resval.rreal = resBufPtr->getDouble();	
	db_setvar(NULL, DB_QTDUSRTIMER, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.restype = RTSTR;//-------------------------------------

	resBufPtr = pDDdwg->getSysVar("DIMAPOST");
	resBuf.resval.rstring = (char*)resBufPtr->getString().c_str();  
	db_setvar(NULL, DB_QDIMAPOST, &resBuf, m_pICdwg, NULL, NULL, 0);


	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QDIMBLK, &resBuf, m_pICdwg, NULL, NULL, 0);
	OdDbHardPointerId id = pDDdwg->dimblk();
	if (id.isValid())
	{
		OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
		ASSERT(!pBlocks.isNull());
		OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QDIMBLK, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QDIMBLK1, &resBuf, m_pICdwg, NULL, NULL, 0);
	id = pDDdwg->dimblk1();
	if (id.isValid())
	{
		OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
		ASSERT(!pBlocks.isNull());
		OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QDIMBLK1, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QDIMBLK2, &resBuf, m_pICdwg, NULL, NULL, 0);
	id = pDDdwg->dimblk2();
	if (id.isValid())
	{
		OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
		ASSERT(!pBlocks.isNull());
		OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QDIMBLK2, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QDIMLDRBLK, &resBuf, m_pICdwg, NULL, NULL, 0);
	id = pDDdwg->dimldrblk();
	if (id.isValid())
	{
		OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
		ASSERT(!pBlocks.isNull());
		OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QDIMLDRBLK, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	resBufPtr = pDDdwg->getSysVar("DIMPOST");
	resBuf.resval.rstring = (char*)resBufPtr->getString().c_str();  
	db_setvar(NULL, DB_QDIMPOST, &resBuf, m_pICdwg, NULL, NULL, 0);

	// TODO MHB crashes
	//resBufPtr = pDDdwg->getSysVar("MENUNAME");
	//resBuf.resval.rstring = (char*)resBufPtr->getString().c_str();  
	//db_setvar(NULL, DB_QMENUNAME, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QUCSNAME, &resBuf, m_pICdwg, NULL, NULL, 0);
	id = pDDdwg->getUCSNAME();
	if (id.isValid())
	{
		OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
		ASSERT(!pBlocks.isNull());
		OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QUCSNAME, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	OdDbDatabaseSummaryInfoPtr pSI = oddbGetSummaryInfo(pDDdwg);
	ASSERT(!pSI.isNull());

	resBuf.resval.rstring = (char*)pSI->getHyperlinkBase().c_str();  
	db_setvar(NULL, DB_QHYPERLINKBASE, &resBuf, m_pICdwg, NULL, NULL, 0);

	OdString codePageStr;
	OdCharMapper::codepageIdToDesc(m_pExServices->systemCodePage(), codePageStr);
	resBuf.resval.rstring = (char*)codePageStr.c_str(); 
	db_setvar(NULL, DB_QDWGCODEPAGE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = (char*)GetLinetypeNameFromDWGdirectId(pDDdwg->getCELTYPE()).c_str();
	db_setvar(NULL, DB_QCELTYPE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = (char*)GetLayerNameFromDWGdirectId(pDDdwg->getCLAYER()).c_str();
	db_setvar(NULL, DB_QCLAYER, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = (char*)GetMlineStyleNameFromDWGdirectId(pDDdwg->getCMLSTYLE()).c_str();  
	db_setvar(NULL, DB_QCMLSTYLE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = (char*)GetDimStyleNameFromDWGdirectId(pDDdwg->getDIMSTYLE()).c_str();
	db_setvar(NULL, DB_QDIMSTYLE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QDIMTXSTY, &resBuf, m_pICdwg, NULL, NULL, 0);
	id = pDDdwg->dimtxsty();
	if (id.isValid())
	{
		OdDbTextStyleTablePtr pStyles = pDDdwg->getTextStyleTableId().safeOpenObject();
		ASSERT(!pStyles.isNull());
		OdDbSymbolTableIteratorPtr pIter = pStyles->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbTextStyleTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QDIMTXSTY, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	resBuf.resval.rstring = "";
	db_setvar(NULL, DB_QP_UCSNAME, &resBuf, m_pICdwg, NULL, NULL, 0);
	id = pDDdwg->getPUCSNAME();
	if (id.isValid())
	{
		OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
		ASSERT(!pBlocks.isNull());
		OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			if (pIter->getRecordId() == id)
			{
				resBuf.resval.rstring = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
				db_setvar(NULL, DB_QP_UCSNAME, &resBuf, m_pICdwg, NULL, NULL, 0);
			}
		}
	}

	resBuf.resval.rstring = (char*)GetTextStyleNameFromDWGdirectId(pDDdwg->getTEXTSTYLE()).c_str();
	db_setvar(NULL, DB_QTEXTSTYLE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rstring = (char*)pDDdwg->getCECOLOR().colorName().c_str();
	db_setvar(NULL, DB_QCECOLOR, &resBuf, m_pICdwg, NULL, NULL, 0);

	char tmp[2] = {pDDdwg->dimdsep(), 0};
	resBuf.resval.rstring = tmp;  
	db_setvar(NULL, DB_QDIMDSEP, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.restype = RTSHORT;//-------------------------------------
	
	// TODO MHB make sure 'getInt16()' is the correct call for RTSHORT
//	resBufPtr = pDDdwg->getSysVar("DIMASO");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//	db_setvar(NULL, DB_QDIMASO, &resBuf, m_pICdwg, NULL, NULL, 0);

//	resBufPtr = pDDdwg->getSysVar("DELOBJ");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//    db_setvar(NULL, DB_QDELOBJ, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimalt();
    db_setvar(NULL, DB_QDIMALT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimaltd();  
    db_setvar(NULL, DB_QDIMALTD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimalttz();
    db_setvar(NULL, DB_QDIMALTTZ, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimaltz();  
    db_setvar(NULL, DB_QDIMALTZ, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimatfit();  
	db_setvar(NULL, DB_QDIMATFIT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimfit();  
	db_setvar(NULL, DB_QDIMFIT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtmove();  
	db_setvar(NULL, DB_QDIMTMOVE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimjust();  
	db_setvar(NULL, DB_QDIMJUST, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimlim();  
	db_setvar(NULL, DB_QDIMLIM, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimsah();  
	db_setvar(NULL, DB_QDIMSAH, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimsd1();  
	db_setvar(NULL, DB_QDIMSD1, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimsd2();  
	db_setvar(NULL, DB_QDIMSD2, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getDIMSHO();  
	db_setvar(NULL, DB_QDIMSHO, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimsoxd();  
	db_setvar(NULL, DB_QDIMSOXD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtad();  
	db_setvar(NULL, DB_QDIMTAD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtih();  
	db_setvar(NULL, DB_QDIMTIH, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtix();  
	db_setvar(NULL, DB_QDIMTIX, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtofl();  
	db_setvar(NULL, DB_QDIMTOFL, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtoh();  
	db_setvar(NULL, DB_QDIMTOH, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtol();  
	db_setvar(NULL, DB_QDIMTOL, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtolj();  
	db_setvar(NULL, DB_QDIMTOLJ, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimtzin();  
	db_setvar(NULL, DB_QDIMTZIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimupt();  
	db_setvar(NULL, DB_QDIMUPT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimzin();  
	db_setvar(NULL, DB_QDIMZIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getDISPSILH();  
	db_setvar(NULL, DB_QDISPSILH, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getPELLIPSE();  
	db_setvar(NULL, DB_QPELLIPSE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = m_pExServices->getPICKSTYLE();  
	db_setvar(NULL, DB_QPICKSTYLE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getPROXYGRAPHICS();  
	db_setvar(NULL, DB_QPROXYGRAPHICS, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getMIRRTEXT();  
	db_setvar(NULL, DB_QMIRRTEXT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getORTHOMODE();  
	db_setvar(NULL, DB_QORTHOMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getPDMODE();  
	db_setvar(NULL, DB_QPDMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getPLIMCHECK();  
	db_setvar(NULL, DB_QP_LIMCHECK, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getPLINEGEN();  
	db_setvar(NULL, DB_QPLINEGEN, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getPSLTSCALE();  
	db_setvar(NULL, DB_QPSLTSCALE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getQTEXTMODE();  
	db_setvar(NULL, DB_QQTEXTMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getREGENMODE();  
	db_setvar(NULL, DB_QREGENMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSHADEDGE();  
	db_setvar(NULL, DB_QSHADEDGE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSHADEDIF();  
	db_setvar(NULL, DB_QSHADEDIF, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSKPOLY();  
	db_setvar(NULL, DB_QSKPOLY, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSPLFRAME();  
	db_setvar(NULL, DB_QSPLFRAME, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSPLINESEGS();  
	db_setvar(NULL, DB_QSPLINESEGS, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSPLINETYPE();  
	db_setvar(NULL, DB_QSPLINETYPE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSURFTAB1();  
	db_setvar(NULL, DB_QSURFTAB1, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSURFTAB2();  
	db_setvar(NULL, DB_QSURFTAB2, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSURFTYPE();  
	db_setvar(NULL, DB_QSURFTYPE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSURFU();  
	db_setvar(NULL, DB_QSURFU, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getSURFV();  
	db_setvar(NULL, DB_QSURFV, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint =  pDDdwg->dimunit();
	db_setvar(NULL, DB_QDIMUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimdec();  
	db_setvar(NULL, DB_QDIMTDEC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getANGDIR();  
	db_setvar(NULL, DB_QANGDIR, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = m_pExServices->getATTDIA();  
	db_setvar(NULL, DB_QATTDIA, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getATTMODE();  
	db_setvar(NULL, DB_QATTMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getAUNITS();  
	db_setvar(NULL, DB_QAUNITS, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getAUPREC();  
	db_setvar(NULL, DB_QAUPREC, &resBuf, m_pICdwg, NULL, NULL, 0);

//	resBufPtr = pDDdwg->getSysVar("AXISMODE");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//	db_setvar(NULL, DB_QAXISMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getCMLJUST();  
    db_setvar(NULL, DB_QCMLJUST, &resBuf, m_pICdwg, NULL, NULL, 0);

//	resBufPtr = pDDdwg->getSysVar("CYCLECURR");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//    db_setvar(NULL, DB_QCYCLECURR, &resBuf, m_pICdwg, NULL, NULL, 0);

//	resBufPtr = pDDdwg->getSysVar("CYCLEKEEP");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//    db_setvar(NULL, DB_QCYCLEKEEP, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimalttd();  
    db_setvar(NULL, DB_QDIMALTTD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimaltu();  
    db_setvar(NULL, DB_QDIMALTU, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimaunit();  
    db_setvar(NULL, DB_QDIMAUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimclrd().colorIndex();  
    db_setvar(NULL, DB_QDIMCLRD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimclre().colorIndex();  
    db_setvar(NULL, DB_QDIMCLRE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimclrt().colorIndex();  
    db_setvar(NULL, DB_QDIMCLRT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimdec();  
    db_setvar(NULL, DB_QDIMDEC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getFILLMODE();  
    db_setvar(NULL, DB_QFILLMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

//	resBufPtr = pDDdwg->getSysVar("FLATLAND");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//    db_setvar(NULL, DB_QFLATLAND, &resBuf, m_pICdwg, NULL, NULL, 0);

    resBuf.resval.rint = 1; // Handles are always on.
    db_setvar(NULL, DB_QHANDLES, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getISOLINES();  
    db_setvar(NULL, DB_QISOLINES, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getLIMCHECK();  
    db_setvar(NULL, DB_QLIMCHECK, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getLUNITS();  
    db_setvar(NULL, DB_QLUNITS, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getLUPREC();  
    db_setvar(NULL, DB_QLUPREC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getMAXACTVP();  
    db_setvar(NULL, DB_QMAXACTVP, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getTILEMODE();  
    db_setvar(NULL, DB_QTILEMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getTEXTQLTY();  
    db_setvar(NULL, DB_QTEXTQLTY, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getTREEDEPTH();  
    db_setvar(NULL, DB_QTREEDEPTH, &resBuf, m_pICdwg, NULL, NULL, 0);

//	resBufPtr = pDDdwg->getSysVar("UCSFOLLOW");
//	resBuf.resval.rint = resBufPtr->getInt16();  
//    db_setvar(NULL, DB_QUCSFOLLOW, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUNITMODE();  
    db_setvar(NULL, DB_QUNITMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUSRTIMER();  
    db_setvar(NULL, DB_QUSRTIMER, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getVISRETAIN();  
    db_setvar(NULL, DB_QVISRETAIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getWORLDVIEW();  
    db_setvar(NULL, DB_QWORLDVIEW, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUSERI1() ;  
    db_setvar(NULL, DB_QUSERI1, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUSERI2() ;  
    db_setvar(NULL, DB_QUSERI2, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUSERI3() ;  
    db_setvar(NULL, DB_QUSERI3, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUSERI4() ;  
    db_setvar(NULL, DB_QUSERI4, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getUSERI5() ;  
    db_setvar(NULL, DB_QUSERI5, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getMEASUREMENT();  
    db_setvar(NULL, DB_QMEASUREMENT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimadec();  
    db_setvar(NULL, DB_QDIMADEC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimazin();  
    db_setvar(NULL, DB_QDIMAZIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getCELWEIGHT();  
    db_setvar(NULL, DB_QCELWEIGHT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimlwd();  
    db_setvar(NULL, DB_QDIMLWD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->dimlwe();  
    db_setvar(NULL, DB_QDIMLWE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rint = pDDdwg->getLWDISPLAY();  
    db_setvar(NULL, DB_QLWDISPLAY, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("PSTYLEMODE");
	resBuf.resval.rint = resBufPtr->getInt16();  
    db_setvar(NULL, DB_QPSTYLEMODE, &resBuf, m_pICdwg, NULL, NULL, 0);
	//drw_set_cplotstyle( adhd, m_pICdwg );

	resBufPtr = pDDdwg->getSysVar("EXTNAMES");
	resBuf.resval.rint = resBufPtr->getInt16();  
    db_setvar(NULL, DB_QEXTNAMES, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("INSUNITS");
	resBuf.resval.rint = resBufPtr->getInt16();  
    db_setvar(NULL, DB_QINSUNITS, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("XEDIT");
	resBuf.resval.rint = resBufPtr->getInt16();  
    db_setvar(NULL, DB_QXEDIT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("TSTACKSIZE");
	resBuf.resval.rint = resBufPtr->getInt16();  
    db_setvar(NULL, DB_QTSTACKSIZE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBufPtr = pDDdwg->getSysVar("TSTACKALIGN");
	resBuf.resval.rint = resBufPtr->getInt16();  
    db_setvar(NULL, DB_QTSTACKALIGN, &resBuf, m_pICdwg, NULL, NULL, 0);

	// TODO MHB
	// drw_set_cplotstyle( adhd, m_pICdwg );

	if (setviewvars == 1)
	{
		resBufPtr = pDDdwg->getSysVar("UCSICON");
		resBuf.resval.rint = resBufPtr->getInt16();  
		db_setvar(NULL, DB_QUCSICON, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("FASTZOOM");
		resBuf.resval.rint = resBufPtr->getInt16();  
		db_setvar(NULL, DB_QFASTZOOM, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("GRIDMODE");
		resBuf.resval.rint = resBufPtr->getInt16();  
		db_setvar(NULL, DB_QGRIDMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("SNAPISOPAIR");
		resBuf.resval.rint = resBufPtr->getInt16();  
		db_setvar(NULL, DB_QSNAPISOPAIR, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("NAPMODE");
		resBuf.resval.rint = resBufPtr->getInt16();  
		db_setvar(NULL, DB_QSNAPMODE, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("SNAPSTYL");
		resBuf.resval.rint = resBufPtr->getInt16();  
		db_setvar(NULL, DB_QSNAPSTYL, &resBuf, m_pICdwg, NULL, NULL, 0);
	}

    resBuf.restype=RTREAL;//-------------------------------------

	resBuf.resval.rreal = pDDdwg->getANGBASE();  
    db_setvar(NULL, DB_QANGBASE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getCELTSCALE();  
    db_setvar(NULL, DB_QCELTSCALE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getCHAMFERA();  
    db_setvar(NULL, DB_QCHAMFERA, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getCHAMFERB();  
    db_setvar(NULL, DB_QCHAMFERB, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getCHAMFERC();  
    db_setvar(NULL, DB_QCHAMFERC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getCHAMFERD();  
    db_setvar(NULL, DB_QCHAMFERD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getCMLSCALE();  
    db_setvar(NULL, DB_QCMLSCALE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimaltf();  
    db_setvar(NULL, DB_QDIMALTF, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimasz();  
    db_setvar(NULL, DB_QDIMASZ, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimcen();  
    db_setvar(NULL, DB_QDIMCEN, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimdle();  
    db_setvar(NULL, DB_QDIMDLE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimdli();  
    db_setvar(NULL, DB_QDIMDLI, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimexe();  
    db_setvar(NULL, DB_QDIMEXE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimexo();  
    db_setvar(NULL, DB_QDIMEXO, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimgap();  
    db_setvar(NULL, DB_QDIMGAP, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimlfac();  
    db_setvar(NULL, DB_QDIMLFAC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimrnd();  
    db_setvar(NULL, DB_QDIMRND, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimscale();  
    db_setvar(NULL, DB_QDIMSCALE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimtfac();  
    db_setvar(NULL, DB_QDIMTFAC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimtm();  
    db_setvar(NULL, DB_QDIMTM , &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimtp();  
    db_setvar(NULL, DB_QDIMTP , &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimtsz();  
    db_setvar(NULL, DB_QDIMTSZ, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimtvp();  
    db_setvar(NULL, DB_QDIMTVP, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->dimtxt();  
    db_setvar(NULL, DB_QDIMTXT, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getFACETRES();  
    db_setvar(NULL, DB_QFACETRES, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getFILLETRAD();  
    db_setvar(NULL, DB_QFILLETRAD, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getLTSCALE();  
    db_setvar(NULL, DB_QLTSCALE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getPDSIZE() ;  
    db_setvar(NULL, DB_QPDSIZE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getPLINEWID();  
    db_setvar(NULL, DB_QPLINEWID, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getSKETCHINC();  
    db_setvar(NULL, DB_QSKETCHINC, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getTEXTSIZE();  
    db_setvar(NULL, DB_QTEXTSIZE, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getTHICKNESS();  
    db_setvar(NULL, DB_QTHICKNESS, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getTRACEWID();  
    db_setvar(NULL, DB_QTRACEWID, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getUSERR1();  
    db_setvar(NULL, DB_QUSERR1, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getUSERR2();  
    db_setvar(NULL, DB_QUSERR2, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getUSERR3();  
    db_setvar(NULL, DB_QUSERR3, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getUSERR4();  
    db_setvar(NULL, DB_QUSERR4, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getUSERR5();  
    db_setvar(NULL, DB_QUSERR5, &resBuf, m_pICdwg, NULL, NULL, 0);

	resBuf.resval.rreal = pDDdwg->getPELEVATION();  
    db_setvar(NULL, DB_QP_ELEVATION, &resBuf, m_pICdwg, NULL, NULL, 1);

	resBuf.resval.rreal = pDDdwg->getELEVATION();  
    db_setvar(NULL,DB_QELEVATION, &resBuf, m_pICdwg, NULL, NULL, 1);

	resBuf.resval.rreal = pDDdwg->dimaltrnd();  
    db_setvar(NULL,DB_QDIMALTRND, &resBuf, m_pICdwg, NULL, NULL, 1);

	resBufPtr = pDDdwg->getSysVar("PSVPSCALE");
	resBuf.resval.rreal = resBufPtr->getDouble();  
    db_setvar(NULL,DB_QPSVPSCALE, &resBuf, m_pICdwg, NULL, NULL, 1);

    if(setviewvars == 1) 
	{
		resBufPtr = pDDdwg->getSysVar("VIEWTWIST");
		resBuf.resval.rreal = resBufPtr->getDouble();  
		db_setvar(NULL, DB_QVIEWTWIST, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("BACKZ");
		resBuf.resval.rreal = resBufPtr->getDouble();  
		db_setvar(NULL, DB_QBACKZ, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("LENSLENGTH");
		resBuf.resval.rreal = resBufPtr->getDouble();  
		db_setvar(NULL, DB_QLENSLENGTH, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("FRONTZ");
		resBuf.resval.rreal = resBufPtr->getDouble();  
		db_setvar(NULL, DB_QFRONTZ, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("VIEWSIZE");
		resBuf.resval.rreal = resBufPtr->getDouble();  
		db_setvar(NULL, DB_QVIEWSIZE, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("SNAPANG");
		resBuf.resval.rreal = resBufPtr->getDouble();  
		db_setvar(NULL, DB_QSNAPANG, &resBuf, m_pICdwg, NULL, NULL, 0);
    }
 
    resBuf.restype=RTPOINT;//-------------------------------------

	OdGePoint3d point3D;

//	resBufPtr = pDDdwg->getSysVar("AXISUNIT");
//	point3D = resBufPtr->getPoint3d();  
//	resBuf.resval.rpoint[0] = point3D.x;
//	resBuf.resval.rpoint[1] = point3D.y;
//	resBuf.resval.rpoint[2] = point3D.z;
//    db_setvar(NULL, DB_QAXISUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);

	OdGePoint2d point2D;
	point2D = pDDdwg->getLIMMIN();  
	resBuf.resval.rpoint[0] = point2D.x;
	resBuf.resval.rpoint[1] = point2D.y;
	resBuf.resval.rpoint[2] = 0.0;
    db_setvar(NULL, DB_QLIMMIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	point2D = pDDdwg->getLIMMAX();  
	resBuf.resval.rpoint[0] = point2D.x;
	resBuf.resval.rpoint[1] = point2D.y;
	resBuf.resval.rpoint[2] = 0.0;
    db_setvar(NULL, DB_QLIMMAX , &resBuf, m_pICdwg, NULL, NULL, 0);

	point2D = pDDdwg->getPLIMMIN(); 
	resBuf.resval.rpoint[0] = point2D.x;
	resBuf.resval.rpoint[1] = point2D.y;
	resBuf.resval.rpoint[2] = 0.0;
    db_setvar(NULL, DB_QP_LIMMIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	point2D = pDDdwg->getPLIMMAX(); 
	resBuf.resval.rpoint[0] = point2D.x;
	resBuf.resval.rpoint[1] = point2D.y;
	resBuf.resval.rpoint[2] = 0.0;
    db_setvar(NULL, DB_QP_LIMMAX, &resBuf, m_pICdwg, NULL, NULL, 0);

    if(setviewvars) {

		resBufPtr = pDDdwg->getSysVar("GRIDUNIT");
		point3D = resBufPtr->getPoint3d();  
		resBuf.resval.rpoint[0] = point3D.x;
		resBuf.resval.rpoint[1] = point3D.y;
		resBuf.resval.rpoint[2] = point3D.z;
        db_setvar(NULL, DB_QGRIDUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("SNAPBASE");
		point3D = resBufPtr->getPoint3d();  
		resBuf.resval.rpoint[0] = point3D.x;
		resBuf.resval.rpoint[1] = point3D.y;
		resBuf.resval.rpoint[2] = point3D.z;
        db_setvar(NULL, DB_QSNAPBASE, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("SNAPUNIT");
		point3D = resBufPtr->getPoint3d();  
		resBuf.resval.rpoint[0] = point3D.x;
		resBuf.resval.rpoint[1] = point3D.y;
		resBuf.resval.rpoint[2] = point3D.z;
        db_setvar(NULL, DB_QSNAPUNIT, &resBuf, m_pICdwg, NULL, NULL, 0);
    }

    resBuf.restype=RT3DPOINT;//-------------------------------------

	point3D = pDDdwg->getEXTMIN(); 
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QEXTMIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	point3D = pDDdwg->getEXTMAX();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QEXTMAX , &resBuf, m_pICdwg, NULL, NULL, 0);

	point3D = pDDdwg->getINSBASE();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QINSBASE, &resBuf, m_pICdwg, NULL, NULL, 1);

	point3D = pDDdwg->getPEXTMIN();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QP_EXTMIN, &resBuf, m_pICdwg, NULL, NULL, 0);

	point3D = pDDdwg->getPEXTMAX();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QP_EXTMAX, &resBuf, m_pICdwg, NULL, NULL, 0);

	point3D = pDDdwg->getPINSBASE();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QP_INSBASE, &resBuf, m_pICdwg, NULL, NULL, 1);

	point3D = pDDdwg->getPUCSORG();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QP_UCSORG, &resBuf, m_pICdwg, NULL, NULL, 0);

	OdGeVector3d vector3D;

	vector3D = pDDdwg->getPUCSXDIR();
	resBuf.resval.rpoint[0] = vector3D.x;
	resBuf.resval.rpoint[1] = vector3D.y;
	resBuf.resval.rpoint[2] = vector3D.z;
    db_setvar(NULL, DB_QP_UCSXDIR, &resBuf, m_pICdwg, NULL, NULL, 0);

	vector3D = pDDdwg->getPUCSYDIR();
	resBuf.resval.rpoint[0] = vector3D.x;
	resBuf.resval.rpoint[1] = vector3D.y;
	resBuf.resval.rpoint[2] = vector3D.z;
    db_setvar(NULL, DB_QP_UCSYDIR, &resBuf, m_pICdwg, NULL, NULL, 0);

	point3D = pDDdwg->getUCSORG();
	resBuf.resval.rpoint[0] = point3D.x;
	resBuf.resval.rpoint[1] = point3D.y;
	resBuf.resval.rpoint[2] = point3D.z;
    db_setvar(NULL, DB_QUCSORG, &resBuf, m_pICdwg, NULL, NULL, 0);

	vector3D = pDDdwg->getUCSXDIR();
	resBuf.resval.rpoint[0] = vector3D.x;
	resBuf.resval.rpoint[1] = vector3D.y;
	resBuf.resval.rpoint[2] = vector3D.z;
    db_setvar(NULL, DB_QUCSXDIR, &resBuf, m_pICdwg, NULL, NULL, 0);

	vector3D = pDDdwg->getUCSYDIR();
	resBuf.resval.rpoint[0] = vector3D.x;
	resBuf.resval.rpoint[1] = vector3D.y;
	resBuf.resval.rpoint[2] = vector3D.z;
    db_setvar(NULL, DB_QUCSYDIR, &resBuf, m_pICdwg, NULL, NULL, 0);

	if(setviewvars) 
	{
		resBufPtr = pDDdwg->getSysVar("VIEWCTR");
		point3D = resBufPtr->getPoint3d();  
		resBuf.resval.rpoint[0] = point3D.x;
		resBuf.resval.rpoint[1] = point3D.y;
		resBuf.resval.rpoint[2] = point3D.z;
		db_setvar(NULL, DB_QVIEWCTR, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("VIEWDIR");
		point3D = resBufPtr->getPoint3d();  
		resBuf.resval.rpoint[0] = point3D.x;
		resBuf.resval.rpoint[1] = point3D.y;
		resBuf.resval.rpoint[2] = point3D.z;
		db_setvar(NULL, DB_QVIEWDIR, &resBuf, m_pICdwg, NULL, NULL, 0);

		resBufPtr = pDDdwg->getSysVar("TARGET");
		point3D = resBufPtr->getPoint3d();  
		resBuf.resval.rpoint[0] = point3D.x;
		resBuf.resval.rpoint[1] = point3D.y;
		resBuf.resval.rpoint[2] = point3D.z;
		db_setvar(NULL, DB_QTARGET, &resBuf, m_pICdwg, NULL, NULL, 0);
	}

		sds_point point;

		m_pICdwg->set_dimfrac(pDDdwg->dimfrac());
		m_pICdwg->set_dimtmove(pDDdwg->dimtmove());
		m_pICdwg->set_endcaps(pDDdwg->getENDCAPS());
		m_pICdwg->set_joinstyle(pDDdwg->getJOINSTYLE());


		id = pDDdwg->dimldrblk();
		if (id.isValid())
		{
			OdDbBlockTablePtr pBlocks = pDDdwg->getBlockTableId().safeOpenObject();
			ASSERT(!pBlocks.isNull());
			OdDbSymbolTableIteratorPtr pIter = pBlocks->newIterator();
			for (pIter->start(); !pIter->done(); pIter->step())
			{
				if (pIter->getRecordId() == id)
				{
					char* tmpLDRBlkName = (char*)((OdDbBlockTableRecordPtr)pIter->getRecordId().safeOpenObject())->getName().c_str(); 
					m_pICdwg->set_dimldrblk(tmpLDRBlkName);
				}
			}
		}
		
		m_pICdwg->set_fingerprintguid((char*)pDDdwg->getFINGERPRINTGUID().c_str());
		m_pICdwg->set_versionguid((char*)pDDdwg->getVERSIONGUID().c_str());

		// TODO MHB
		// m_pICdwg->set_pucsbase(adhd->curpucsbaseobjhandle);
		// m_pICdwg->set_pucsorthoref(adhd->cuansferelrpucsorthorefobjhandle);
		// m_pICdwg->set_ucsbase(adhd->curucsbaseobjhandle);
		// m_pICdwg->set_ucsorthoref(adhd->curucsorthorefobjhandle);

		CPYPT3D(point, pDDdwg->getPUCSORGBACK());
		m_pICdwg->set_pucsorgback(point);
        m_pICdwg->set_pucsorgbottom(point);
		CPYPT3D(point, pDDdwg->getPUCSORGFRONT());
		m_pICdwg->set_pucsorgfront(point);
		CPYPT3D(point, pDDdwg->getPUCSORGLEFT());
		m_pICdwg->set_pucsorgleft(point);
		CPYPT3D(point, pDDdwg->getPUCSORGRIGHT());
		m_pICdwg->set_pucsorgright(point);
 		CPYPT3D(point, pDDdwg->getPUCSORGTOP());
		m_pICdwg->set_pucsorgtop(point);
 		CPYPT3D(point, pDDdwg->getUCSORGBACK());
		m_pICdwg->set_ucsorgback(point);
 		CPYPT3D(point, pDDdwg->getUCSORGBOTTOM());
		m_pICdwg->set_ucsorgbottom(point);
 		CPYPT3D(point, pDDdwg->getUCSORGFRONT());
		m_pICdwg->set_ucsorgfront(point);
 		CPYPT3D(point, pDDdwg->getUCSORGLEFT());
		m_pICdwg->set_ucsorgleft(point);
 		CPYPT3D(point, pDDdwg->getUCSORGRIGHT());
		m_pICdwg->set_ucsorgright(point);
 		CPYPT3D(point, pDDdwg->getUCSORGTOP());
		m_pICdwg->set_ucsorgtop(point);

		// TODO MHB Need to set 
		// db_setvar(NULL,DB_QCPLOTSTYLE,&rb,dp,NULL,NULL,0);
	return;
}

//------------------------------------------------------------------------------
//
void DWGdirectReader::TransferWorldUCSFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	sds_resbuf resBuf;

	db_getvar(NULL, DB_QTILEMODE, &resBuf, m_pICdwg, NULL, NULL);
	int tilemode = resBuf.resval.rint;

	// Determaine World UCS - get CVPORT
	db_getvar(NULL, DB_QCVPORT, &resBuf, m_pICdwg, NULL, NULL);
	int cvport = resBuf.resval.rint;

	OdGePoint3d ucsOrigin;
	OdGeVector3d ucsXVector;
	OdGeVector3d ucsYVector;
	resBuf.resval.rint = 1;
	if (tilemode == 1 || cvport == 1)	// Model space
	{
		ucsOrigin = pDDdwg->getUCSORG();
		ucsXVector = pDDdwg->getUCSXDIR();
		ucsYVector = pDDdwg->getUCSYDIR();
	}
	else								// Paper space
	{
		ucsOrigin = pDDdwg->getPUCSORG();
		ucsXVector = pDDdwg->getPUCSXDIR();
		ucsYVector = pDDdwg->getPUCSYDIR();
	}

	if (!icadRealEqual(ucsOrigin.x,0.0)		||
		!icadRealEqual(ucsOrigin.y,0.0)		||
		!icadRealEqual(ucsOrigin.z,0.0)		||
		!icadRealEqual(ucsXVector.x,1.0)	||
		!icadRealEqual(ucsXVector.y,0.0)	||
		!icadRealEqual(ucsXVector.z,0.0)	||
		!icadRealEqual(ucsYVector.x,0.0)	||
		!icadRealEqual(ucsYVector.y,1.0)	||
		!icadRealEqual(ucsYVector.z,0.0)) 

		resBuf.resval.rint=0;

	db_setvar(NULL, DB_QWORLDUCS, &resBuf, m_pICdwg, NULL, NULL, 0);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferModelspaceEntsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbObjectId modelspaceID = pDDdwg->getModelSpaceId();
	ASSERT(!modelspaceID.isNull() && modelspaceID.isValid());

	OdDbBlockTableRecordPtr pMSBlock = modelspaceID.safeOpenObject();

	OdDbObjectIteratorPtr pMSSpaceIter = pMSBlock->newIterator();

	for (; !pMSSpaceIter->done(); pMSSpaceIter->step())
	{
		OdDbObjectId entId = pMSSpaceIter->entity()->id();
		OdDbEntityPtr pEntity = entId.safeOpenObject();
		
		try
		{
			if (pEntity->isKindOf(OdDbDimension::desc()) ||
				pEntity->isKindOf(OdDbHatch::desc()) ||
				pEntity->isKindOf(OdDbBlockReference::desc()) ||
				pEntity->isKindOf(OdDbLeader::desc()) )
			{
				m_delayedEntities.insert(m_delayedEntities.end(), handItemObjectIDPair(NULL, entId, 0));
			}
			else
			{
				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pEntCreator = pEntity;
				db_handitem* pEnt = pEntCreator->createHanditem(pEntity, m_pICdwg, NULL);
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			int error = err.code();
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader model space entity creation error: ");
#endif
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferPaperspaceEntsFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	OdDbObjectId paperspaceID = pDDdwg->getPaperSpaceId();
	ASSERT(!paperspaceID.isNull() && paperspaceID.isValid());

	OdDbBlockTableRecordPtr pPSBlock = paperspaceID.safeOpenObject();

	OdDbObjectIteratorPtr pPSSpaceIter = pPSBlock->newIterator();

	int viewportIndex = 1;
	for (; !pPSSpaceIter->done(); pPSSpaceIter->step())
	{
		OdDbObjectId entId = pPSSpaceIter->entity()->id();
		OdDbEntityPtr pEntity = entId.safeOpenObject();
		
		try
		{
			if (pEntity->isKindOf(OdDbDimension::desc()) ||
				pEntity->isKindOf(OdDbHatch::desc()) ||
				pEntity->isKindOf(OdDbBlockReference::desc()) ||
				pEntity->isKindOf(OdDbLeader::desc()) )
			{
				m_delayedEntities.insert(m_delayedEntities.end(), handItemObjectIDPair(NULL, entId, 1));
			}
			else
			{
				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pEntCreator = pEntity;
				db_handitem* pEnt = pEntCreator->createHanditem(pEntity, m_pICdwg, NULL);
#ifdef _DEBUG_MHB
				ASSERT(pEnt != NULL);
#endif
				if (pEnt != NULL) 
				{
					// Set sub-ents space flag too.
					db_handitem* tmp = pEnt;
					while(tmp)
					{
						tmp->set_67(1);
						tmp = tmp->next;
					}

					if (pEntity->isKindOf(OdDbViewport::desc()))
					{
						pEnt->set_68(viewportIndex);
						pEnt->set_69(viewportIndex++);
					}
				}
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			int error = err.code();
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader paper space entity creation error: ");
#endif
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferHandleFromDWGdirect(db_handitem* pItem, OdDbObject* DWGdirectObject)
{
	ASSERT(pItem != NULL);
	ASSERT(DWGdirectObject != NULL);

	OdString stringHandle = DWGdirectObject->objectId().getHandle().ascii();
	pItem->SetHandle(db_objhandle(stringHandle.c_str()));

#ifdef _DEBUG_MHB
	char sBuff[17];
	char sBuffer[256];
	db_objhandle hand = db_objhandle(stringHandle.c_str());
	hand.ToAscii(sBuff);
	
	strcpy(sBuffer, "New entity added with handle; - \"");
	strcat(sBuffer, sBuff);
	strcat(sBuffer, "\"\n");

	OutputDebugString(sBuffer);
#endif // _DEBUG

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferExtensionDictionariesFromDWGdirect(db_handitem* pItem, OdDbObject* DWGdirectObject, db_drawing* pDb)
{
	ASSERT(pItem != NULL);
	ASSERT(DWGdirectObject != NULL);

	OdDbObjectId extDictId = DWGdirectObject->extensionDictionary();

	if (!extDictId.isNull())
	{
		OdString stringHandle = extDictId.getHandle().ascii();

		pItem->set_xdict(DB_SOFT_POINTER,db_objhandle(stringHandle.c_str()),NULL);
		OdDbDictionaryPtr pExtDictionary= extDictId.safeOpenObject();

		db_dictionary* pExtensionDictionary = (db_dictionary*)pDb->handent(db_objhandle(stringHandle.c_str()));
		if (pExtensionDictionary == NULL)
		{
			pExtensionDictionary = CreateDictionary(pDb, pExtDictionary);
			AddDictionaryRecords(pExtensionDictionary, pExtDictionary, DB_SOFT_OWNER);
		}
		ASSERT(pExtensionDictionary != NULL);

		// Create the entries.
		ProcessDictionaryRecords(pExtDictionary, pDb, pItem);
	}
	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::ProcessDictionaryRecords(OdDbDictionary* pDict, db_drawing* pDb, db_handitem* pItem)
{
	ASSERT(pDict != NULL);

	OdDbDictionaryIteratorPtr pDictIter = pDict->newIterator();
	for(; !pDictIter->done(); pDictIter->next())
	{
		OdDbObjectPtr pObject = pDictIter->getObject();
		if (pObject.isNull())
			continue;

		if (pObject->isKindOf(OdDbXrecord::desc()))
		{
			CreateXRecord(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbDictionaryVar::desc()))
		{
			CreateDictionaryVar(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbSortentsTable::desc()))
		{
			CreateSortEntTable(pDict, pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbIdBuffer::desc()))
		{
			CreateIDBuffer(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbDimAssoc::desc()))
		{
			CreateDimAssoc(pDb, pItem, pObject);
		}
		else if (pObject->isKindOf(OdDbProxyObject::desc()))
		{
			CreateProxyObject(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbSpatialFilter::desc()))
		{
			CreateSpatialFilter(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbVbaProject::desc()))
		{
			CreateVBAproject(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbPlaceHolder::desc()))
		{
			CreatePlaceHolder(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbTableStyle::desc()))
		{
			;
#ifdef _DEBUG_MHB
			OutputDebugString("OdDbTableStyle not implemented\n");
#endif
		}
		else if (pObject->isKindOf(OdDbMaterial::desc()))
		{
			;
#ifdef _DEBUG_MHB
			OutputDebugString("OdDbMaterial not implemented\n");
#endif
		}
		else if (pObject->isKindOf(OdDbColor::desc()))
		{
			;
#ifdef _DEBUG_MHB
			OutputDebugString("OdDbColor not implemented\n");
#endif
		}
		else if (pObject->isKindOf(OdDbLayerIndex::desc()))
		{
			//OutputDebugString("OdDbLayerIndex not implemented\n");
			//CreateLayerIndex(pDb, pObject);
		}
		else if (pObject->isKindOf(CAseDLPNTableRecord::desc()))
		{
			// CreateDLPNTableRecord(pDb, pObject);
		}
		else if (pObject->isKindOf(OdDbDictionary::desc()))
		{
			db_dictionary* pDictionary = CreateDictionary(pDb, pObject);
			AddDictionaryRecords(pDictionary, (OdDbDictionaryPtr)pObject, DB_SOFT_OWNER);
			// Recursion here!
			ProcessDictionaryRecords((OdDbDictionaryPtr)pObject, pDb, pItem);
		}
		else
		{
			ASSERT(!"IGNORE - What is this thing: ProcessDictionaryRecords()");
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferReactorsFromDWGdirect(db_drawing* pDwg, db_handitem* pItem, OdDbObject* DWGdirectObject, int type)
{
	ASSERT(pDwg != NULL);
	ASSERT(pItem != NULL);
	ASSERT(DWGdirectObject != NULL);

	OdDbObjectIdArray reactorIds;
	DWGdirectObject->getPersistentReactors(reactorIds);
	if (!reactorIds.isEmpty())
	{
		int count = reactorIds.length();
		for (int i = 0; i < reactorIds.length(); i++)
		{
			OdDbObjectId id = reactorIds.getAt(i);
			OdString stringHandle = id.getHandle().ascii();
			pItem->addReactor(db_objhandle(stringHandle.c_str()), pDwg, (ReactorItem::Type)type);
		}
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::TransferEEDataFromDWGdirect(db_drawing* pDwg, db_handitem* pItem, OdDbObject* DWGdirectObject)
{
	ASSERT(pItem != NULL);
	ASSERT(DWGdirectObject != NULL);

	OdDbRegAppTablePtr pTable = DWGdirectObject->database()->getRegAppTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for (pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbRegAppTableRecordPtr pAppRec = pTableIter->getRecordId().safeOpenObject();
		OdString appName = pAppRec->getName();

		OdResBufPtr extendedData = DWGdirectObject->xData(appName);
		if (!extendedData.isNull())
		{
			TransferExtendedEntityDataFromDWGdirect(extendedData, pItem, pDwg, DWGdirectObject->database());
		}
	}
	return;
}

//-----------------------------------------------------------------------------
// There should be a better way of getting a block name besides having
// to iterate through the whole table with an ID looking of it. Bug I haven't
// found one.
OdString  DWGdirectReader::GetBlockNameFromDWGdirectId(OdDbObjectId& id)
{
	OdString retStr = "";

	if (id.isNull())
		return retStr;

	retStr = ((OdDbBlockTableRecordPtr)id.safeOpenObject())->getName();

	return retStr;
}

//-----------------------------------------------------------------------------
// There should be a better way of getting a layer name besides having
// to iterate through the whole table with an ID looking of it. Bug I haven't
// found one.
OdString DWGdirectReader::GetLayerNameFromDWGdirectId(OdDbObjectId& id)
{
	OdString retStr = OdDbSymUtil::layerZeroName();

	if (id.isNull())
		return retStr;

	retStr = ((OdDbLayerTableRecordPtr)id.safeOpenObject())->getName();

	return retStr;
}

//-----------------------------------------------------------------------------
// There should be a better way of getting a line type name besides having
// to iterate through the whole table with an ID looking of it. Bug I haven't
// found one.
OdString DWGdirectReader::GetLinetypeNameFromDWGdirectId(OdDbObjectId& id)
{
	OdString retStr = OdDbSymUtil::linetypeContinuousName();

	if (id.isNull())
		return retStr;

	retStr = ((OdDbLinetypeTableRecordPtr)id.safeOpenObject())->getName();

	return retStr;
}

//-----------------------------------------------------------------------------
// There should be a better way of getting a Dim style name besides having
// to iterate through the whole table with an ID looking of it. Bug I haven't
// found one.
OdString DWGdirectReader::GetDimStyleNameFromDWGdirectId(OdDbObjectId& id)
{
	OdString retStr = OdDbSymUtil::textStyleStandardName();

	if (id.isNull())
		return retStr;

	retStr = ((OdDbDimStyleTableRecordPtr)id.safeOpenObject())->getName();

	return retStr;
}

//-----------------------------------------------------------------------------
// There should be a better way of getting a mline style name besides having
// to iterate through the whole table with an ID looking of it. Bug I haven't
// found one.
OdString DWGdirectReader::GetMlineStyleNameFromDWGdirectId(OdDbObjectId& id)
{
	OdString retStr = OdDbSymUtil::textStyleStandardName();

	if (id.isNull())
		return retStr;

	retStr = ((OdDbMlineStylePtr)id.safeOpenObject())->name();

	return retStr;
}

//-----------------------------------------------------------------------------
// There should be a better way of getting a mline style name besides having
// to iterate through the whole table with an ID looking of it. Bug I haven't
// found one.
OdString DWGdirectReader::GetTextStyleNameFromDWGdirectId(OdDbObjectId& id)
{
	OdString retStr = OdDbSymUtil::textStyleStandardName();

	if (id.isNull())
		return retStr;

	retStr = ((OdDbTextStyleTableRecordPtr)id.safeOpenObject())->getName();

	return retStr;
}

//-----------------------------------------------------------------------------
// Gets or creates the Named object ditionary for this drawing.
db_dictionary* DWGdirectReader::GetNameObjectDictionaryFromDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL);
	ASSERT(!pDDdwg.isNull());

	db_dictionary*	pNamedObjectDictionary = NULL;
	// If it already exists, simply return it. If
	// not create it.
	if (m_pICdwg->ret_stockhand(DB_SHI_DICT_NO) != NULL)
	{
		pNamedObjectDictionary = (db_dictionary*)m_pICdwg->handent(m_pICdwg->ret_stockhand(DB_SHI_DICT_NO));
		return pNamedObjectDictionary;
	}

	pNamedObjectDictionary	= CreateDictionary(m_pICdwg, pDDdwg->getNamedObjectsDictionaryId().safeOpenObject());
	ASSERT(pNamedObjectDictionary	!= NULL);

	m_pICdwg->set_stockhand(pNamedObjectDictionary->RetHandle(), DB_SHI_DICT_NO);

	return pNamedObjectDictionary;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::AddDictionaryRecords(db_dictionary* pDictionary, OdDbDictionary* pObject, int ptrOwnerType)
{
	ASSERT(pDictionary != NULL);
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pObjectIter = pObject->newIterator();
	for(; !pObjectIter->done(); pObjectIter->next())
	{
		OdString objName  = pObjectIter->name();
		db_objhandle hand = db_objhandle(pObjectIter->objectId().getHandle().ascii());

		db_dictionaryrec *pNewDictRec=new db_dictionaryrec(objName,ptrOwnerType,hand,NULL);
		pDictionary->addrec(pNewDictRec);
	}
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateAllMlineStyles(db_dictionaryrec* pMLineStyleDictionary, OdDbDictionary* pObject)
{
	ASSERT(pMLineStyleDictionary != NULL);
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pObjectIter = pObject->newIterator();
	for(; !pObjectIter->done(); pObjectIter->next())
	{
		db_mlinestyle* pNewMlineStyle = new db_mlinestyle();
		ASSERT(pNewMlineStyle != NULL);

		OdString styleName = pObjectIter->name();
		db_objhandle objectHandle = db_objhandle(pObjectIter->objectId().getHandle().ascii());

		if (styleName.iCompare(OdDbSymUtil::textStyleStandardName()) == 0)
		{
			m_pICdwg->set_stockhand(objectHandle,DB_SHI_MLS_STD);
		}
		OdDbMlineStylePtr pMlineStyle = pObjectIter->getObject();
		TransferHandleFromDWGdirect((db_handitem*)pNewMlineStyle, pMlineStyle);

		TransferEEDataFromDWGdirect(m_pICdwg, (db_handitem*)pNewMlineStyle, pMlineStyle);
		TransferReactorsFromDWGdirect(m_pICdwg, (db_handitem*)pNewMlineStyle, pMlineStyle, DB_HARD_POINTER);

        pNewMlineStyle->set_2((char*)styleName.c_str());
        pNewMlineStyle->set_3((char*)pMlineStyle->description().c_str());
        pNewMlineStyle->set_51(pMlineStyle->startAngle());
        pNewMlineStyle->set_52(pMlineStyle->endAngle());
        pNewMlineStyle->set_62(pMlineStyle->fillColor().colorIndex());
		int flags = 0;
		if (pMlineStyle->filled())
			flags |= BIT1;
		if (pMlineStyle->showMiters())
			flags |= BIT2;
		if (pMlineStyle->startSquareCap())
			flags |= BIT10;
		if (pMlineStyle->startInnerArcs())
			flags |= BIT20;
		if (pMlineStyle->startRoundCap())
			flags |= BIT40;
		if (pMlineStyle->endSquareCap())
			flags |= BIT100;
		if (pMlineStyle->endInnerArcs())
			flags |= BIT200;
		if (pMlineStyle->endRoundCap())
			flags |= BIT400;
        pNewMlineStyle->set_70(flags);

		int linesInStyle = pMlineStyle->numElements();
        pNewMlineStyle->set_71(linesInStyle);

		int *pi;
		double *pd;
		char **pcp;

		pi=  new int [linesInStyle];
        pd=  new double [linesInStyle];
        pcp= new char*[linesInStyle];

		memset(pi,0,linesInStyle*sizeof(int) );
		memset(pd,0,linesInStyle*sizeof(double));
		memset(pcp,0,linesInStyle*sizeof(char*));
		
		OdCmColor color;
		OdDbObjectId linetypeId;
		for (int i = 0; i < linesInStyle; i++)
		{
			pMlineStyle->getElementAt(i, pd[i], color, linetypeId);
			pi[i] = color.colorIndex();

			OdDbLinetypeTableRecordPtr pLTypeRec = linetypeId.safeOpenObject();
			char *tmpStr= new char [pLTypeRec->getName().getLength()+1];
            strcpy(tmpStr,(char*)pLTypeRec->getName().c_str());
            pcp[i]=tmpStr;

		}

		pNewMlineStyle->set_6(pcp,linesInStyle,m_pICdwg);
        pNewMlineStyle->set_49(pd,linesInStyle);
        pNewMlineStyle->set_62(pi,linesInStyle);

        for (i = 0; i < linesInStyle; i++)
		{
			delete [] pcp[i];
		}
        delete [] pcp;
        delete [] pd;
        delete [] pi;

#ifdef _DEBUG_MHB
		ASSERT(m_pICdwg->handent(pNewMlineStyle->RetHandle()) == NULL);
#endif
		m_pICdwg->addhanditem(pNewMlineStyle);
	}

	return;	
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateAllGroups(db_dictionaryrec* pGroupDictionary, OdDbDictionary* pObject)
{
	ASSERT(pGroupDictionary != NULL);
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pObjectIter = pObject->newIterator();
	for(; !pObjectIter->done(); pObjectIter->next())
	{
		db_handitem* pNewGroup = new CDbGroup();
		ASSERT(pNewGroup != NULL);

		OdString styleName = pObjectIter->name();
		db_objhandle objectHandle = db_objhandle(pObjectIter->objectId().getHandle().ascii());

		OdDbGroupPtr pGroup = pObjectIter->getObject();

		TransferHandleFromDWGdirect((db_handitem*)pNewGroup, pGroup);
		TransferEEDataFromDWGdirect(m_pICdwg, (db_handitem*)pNewGroup, pGroup);
		TransferReactorsFromDWGdirect(m_pICdwg, (db_handitem*)pNewGroup, pGroup, DB_HARD_POINTER);

        pNewGroup->set_300((char*)pGroup->description().c_str());
        pNewGroup->set_70((pGroup->isAnonymous() == true) ? 1 : 0);
        pNewGroup->set_71((pGroup->isSelectable() == true) ? 1 : 0);

		db_objhandle hand;
		OdString stringHandle;
		OdDbObjectIdArray entIds;
		int entCount = pGroup->allEntityIds(entIds);
		if (entCount > 0)
		{
			db_hireflink *startList = NULL;
			db_hireflink *endList   = NULL;
			for (int i = 0; i < entCount; i++)
			{
				hand = db_objhandle(entIds.getAt(i).getHandle().ascii());
				db_hireflink* pNewHireflink = new db_hireflink(DB_HARD_POINTER,hand,NULL);
				ASSERT(pNewHireflink != NULL);
				if (startList ==  NULL)
				{
					startList = endList = pNewHireflink;
				}
				else
				{
					endList->next = pNewHireflink;
					endList = pNewHireflink;
				}
			}
			((CDbGroup *)pNewGroup)->setEntitiesList(startList, endList);
		}
#ifdef _DEBUG_MHB
		ASSERT(m_pICdwg->handent(pNewGroup->RetHandle()) == NULL);
#endif
		m_pICdwg->addhanditem(pNewGroup);
	}

	return;	
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateAllLayouts(db_dictionaryrec* pLayoutDictionary, OdDbDictionary* pObject)
{
	ASSERT(pLayoutDictionary != NULL);
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pObjectIter = pObject->newIterator();
	for(; !pObjectIter->done(); pObjectIter->next())
	{
		db_handitem* pNewLayout = new CDbLayout();
		ASSERT(pNewLayout != NULL);

		CDbLayout* pCNewLayout = (CDbLayout*)pNewLayout; // Not treated as a real db_handitem.

		OdString layoutName = pObjectIter->name();
		db_objhandle objectHandle = db_objhandle(pObjectIter->objectId().getHandle().ascii());

		OdDbLayoutPtr pLayout = pObjectIter->getObject();

		TransferHandleFromDWGdirect((db_handitem*)pNewLayout, pLayout);
		TransferEEDataFromDWGdirect(m_pICdwg, (db_handitem*)pNewLayout, pLayout);
		TransferReactorsFromDWGdirect(m_pICdwg, (db_handitem*)pNewLayout, pLayout, DB_SOFT_POINTER);

#ifdef _DEBUG_MHB
		ASSERT(m_pICdwg->handent(pNewLayout->RetHandle()) == NULL);
#endif
		m_pICdwg->addhanditem(pNewLayout);

		// setName() uses strncpy()
		pCNewLayout->setName((char*)layoutName.c_str(), m_pICdwg);
		
		pCNewLayout->set_2((char*)pLayout->getPlotCfgName().c_str());
        pCNewLayout->set_4((char*)pLayout->getCanonicalMediaName().c_str());
        pCNewLayout->set_6((char*)pLayout->getPlotViewName().c_str());
        pCNewLayout->set_7((char*)pLayout->getCurrentStyleSheet().c_str());

        pCNewLayout->set_40(pLayout->getLeftMargin());
        pCNewLayout->set_41(pLayout->getBottomMargin());
        pCNewLayout->set_42(pLayout->getRightMargin());
        pCNewLayout->set_43(pLayout->getTopMargin());

		double paperWidth, paperHeight;
		pLayout->getPlotPaperSize(paperWidth, paperHeight);
        pCNewLayout->set_44(paperWidth);
        pCNewLayout->set_45(paperHeight);

		double x, y, xmax, ymax;
		pLayout->getPlotOrigin(x, y);
        pCNewLayout->set_46(x);
        pCNewLayout->set_47(y);

		pLayout->getPlotWindowArea(x, y, xmax, ymax);
        pCNewLayout->set_48(x);
        pCNewLayout->set_49(y);
        pCNewLayout->set_140(xmax);
		pCNewLayout->set_141(ymax);

		pLayout->getCustomPrintScale(x, y);
        pCNewLayout->set_142(x);
        pCNewLayout->set_143(y);

		int flags = 0;
		if (pLayout->plotViewportBorders() == true)
			flags |= BIT1;
		if (pLayout->showPlotStyles() == true)
			flags |= BIT2;
		if (pLayout->plotCentered() == true)
			flags |= BIT4;
		if (pLayout->plotHidden() == true)
			flags |= BIT8;
		if (pLayout->useStandardScale() == true)
			flags |= BIT10;
		if (pLayout->plotPlotStyles() == true)
			flags |= BIT20;
		if (pLayout->scaleLineweights() == true)
			flags |= BIT40;
		if (pLayout->printLineweights() == true)
			flags |= BIT80;
		if (pLayout->drawViewportsFirst() == true)
			flags |= BIT200;
		if (pLayout->modelType() == true)
		{
			flags |= BIT400;

			// For Model Layout the "PrintLineweights" Bit should be set in order to
			// print the objects with lineweights by default.
			flags |= BIT80; 
		}

		// TODO MHB 
		//2048 = UpdatePaper 
		//4096 = ZoomToPaperOnUpdate 
		//8192 = Initializing 
		//16384 = PrevPlotInit
        pCNewLayout->set_70(flags);

        pCNewLayout->set_72(pLayout->plotPaperUnits());
        pCNewLayout->set_73(pLayout->plotRotation());
        pCNewLayout->set_74(pLayout->plotType());
        pCNewLayout->set_75(pLayout->stdScaleType());

		pLayout->getStdScale(x);
        pCNewLayout->set_147(x);

		OdGePoint2d point2D = pLayout->getPaperImageOrigin();
        pCNewLayout->set_148(point2D.x);
        pCNewLayout->set_149(point2D.y);

		flags = 0;
		if (pLayout->getPSLTSCALE() == true)
			flags |= BIT1;
		if (pLayout->getLIMCHECK() == true)
			flags |= BIT2;
		pCNewLayout->set_Layout70(flags);

		// TODO MHB
//        pCNewLayout->m_layoutFlag = adob.layout.flag;

        pCNewLayout->set_71(pLayout->getTabOrder());

		sds_point point;
		point2D = pLayout->getLIMMIN();
		CPYPT2D(point, point2D);
        pCNewLayout->set_10(point);

		point2D = pLayout->getLIMMAX();
		CPYPT2D(point, point2D);
        pCNewLayout->set_11(point);

		OdGePoint3d point3D = pLayout->getINSBASE();
		CPYPT3D(point, point3D);
        pCNewLayout->set_12(point);

		point3D = pLayout->getEXTMIN();
		CPYPT3D(point, point3D);
        pCNewLayout->set_14(point);

		point3D = pLayout->getEXTMAX();
		CPYPT3D(point, point3D);
        pCNewLayout->set_15(point);

		// TODO MHB
        //pCNewLayout->set_13(pLayout->	adob.layout.ucsorg);
        //pCNewLayout->set_16(pLayout->	adob.layout.ucsxdir);
        //pCNewLayout->set_17(pLayout->	adob.layout.ucsydir);
        //pCNewLayout->set_146(pLayout->	adob.layout.elev);
        //pCNewLayout->set_76(pLayout->	adob.layout.ucsorthoviewtype);

		if (objectHandle == m_pICdwg->ret_stockhand(DB_SHI_PSLAYOUT))
		{
			pCNewLayout->setBlock((db_handitem*)pCNewLayout->blockAddress());
		}
		else
		{
			if (objectHandle == m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT))
			{
				pCNewLayout->setBlock((db_handitem*)pCNewLayout);
			}
			else
			{
				OdDbObjectId psId = pLayout->getBlockTableRecordId();
				db_objhandle blkHandle = db_objhandle(psId.getHandle().ascii());
				db_handitem* blkRec = m_pICdwg->handent(blkHandle);

				if (blkRec != NULL)
				{
					ASSERT(blkRec->ret_type() == DB_BLOCKTABREC);
					pCNewLayout->setBlock(blkRec);
				}
				else
				{
					m_pICdwg->AddHandleToFixupTable(objectHandle, (void**)pCNewLayout->blockAddress());
				}
			}
		}

		OdDbObjectId vpId = pLayout->activeViewportId();
		db_objhandle vprtHandle = db_objhandle(vpId.getHandle().ascii());
		if (vprtHandle != NULL)
		{
			db_handitem* viewport = m_pICdwg->handent(vprtHandle);
			if (viewport != NULL)
			{
				ASSERT(viewport->ret_type() == DB_VIEWPORT);
				pCNewLayout->setLastViewport(viewport);
			}
			else
			{
				m_pICdwg->AddHandleToFixupTable(objectHandle, (void**)pCNewLayout->viewportAddress());
			}
		}
		else
		{
			m_pICdwg->AddHandleToFixupTable(objectHandle, (void**)pCNewLayout->viewportAddress());
		}
		// TODO MHB no access in pLayout ???
        //hand = adob.layout.ucsobjhandle;
        //m_pICdwg->AddHandleToFixupTable(hand, (void**)&pCNewLayout->m_pUCS);
        //hand = adob.layout.baseucsobjhandle;
        //m_pICdwg->AddHandleToFixupTable(hand, (void**)&pCNewLayout->m_pBaseUCS);
	}

	return;	
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateAllImageDefs(db_dictionaryrec* pImageDictionary, OdDbDictionary* pObject)
{
	ASSERT(pImageDictionary != NULL);
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pDictionaryIter = pObject->newIterator();
	for(; !pDictionaryIter->done(); pDictionaryIter->next())
	{
		try
		{
			OdDbObjectPtr pObject = pDictionaryIter->objectId().safeOpenObject();
			OdString imageDefStringHandle = pObject->objectId().getHandle().ascii();
			OdString imageDefName = pDictionaryIter->name();
			
			ASSERT(!pObject.isNull());
			if (pObject.isNull())
				continue;
			
			ASSERT(pObject->isKindOf(OdDbRasterImageDef::desc()));

			OdDbRasterImageDefPtr pImageDef = pObject;

			db_imagedef* pNewImageDef = new db_imagedef();
			ASSERT(pNewImageDef != NULL);
			if (pNewImageDef == NULL)
				continue;

			TransferHandleFromDWGdirect(pNewImageDef, pImageDef);
			TransferEEDataFromDWGdirect(m_pICdwg, pNewImageDef, pImageDef);
			TransferReactorsFromDWGdirect(m_pICdwg, pNewImageDef, pImageDef, DB_SOFT_POINTER);

			// TODO MHB
			//pNewImageDef->set_90(adob.imagedef.classversion);

			pNewImageDef->set_1((char*)pImageDef->sourceFileName().c_str());

			sds_point point;
			CPYPT2D(point, pImageDef->size());
			pNewImageDef->set_10(point);

			CPYPT2D(point, pImageDef->resolutionMMPerPixel());
			pNewImageDef->set_11(point);

			pNewImageDef->set_280((pImageDef->isLoaded()) ? 1 : 0);
			pNewImageDef->set_281(pImageDef->resolutionUnits());

#ifdef _DEBUG_MHB
		ASSERT(m_pICdwg->handent(pNewImageDef->RetHandle()) == NULL);
#endif
			m_pICdwg->addhanditem(pNewImageDef);
		}
		catch(OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			int error = err.code();
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectReader image definition creation error: ");
#endif
		}
	}
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateDictionaryVar(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbDictionaryVar::desc()));
	if (!(pObject->isKindOf(OdDbDictionaryVar::desc())))
		return;

	OdDbDictionaryVarPtr pDictionaryVar= pObject;

	db_dictionaryvar* pNewDictionaryVar = new db_dictionaryvar();
	ASSERT(pNewDictionaryVar != NULL);
	if (pNewDictionaryVar == NULL)
		return;

	pNewDictionaryVar->set_280(pDictionaryVar->getSchema());
	pNewDictionaryVar->set_1((char*)pDictionaryVar->value().c_str());

	TransferHandleFromDWGdirect(pNewDictionaryVar, pObject);
	TransferEEDataFromDWGdirect(pDwg, pNewDictionaryVar, pObject);	
	TransferReactorsFromDWGdirect(pDwg, pNewDictionaryVar, pObject, DB_SOFT_POINTER);

#ifdef _DEBUG_MHB
	OdString stringHandle = pObject->objectId().getHandle().ascii();
	ASSERT(pDwg->handent(db_objhandle(stringHandle.c_str())) == NULL);

	char sBuf[100];
	sprintf(sBuf, "DictionaryVar was created\n");
	OutputDebugString(sBuf);

#endif
	pDwg->addhanditem(pNewDictionaryVar);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateAllPlaceHolders(OdDbDictionary* pObject)
{
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pDictionaryIter = pObject->newIterator();
	for(; !pDictionaryIter->done(); pDictionaryIter->next())
	{
		OdDbObjectPtr pObject = pDictionaryIter->getObject();
		ASSERT(!pObject.isNull());
		if (pObject.isNull())
			continue;

		// Note to self, I found a situation where this is either a OdDbPlaceHolder or an OdDbProxyObject.
		ASSERT(pObject->isKindOf(OdDbPlaceHolder::desc()) == true || pObject->isKindOf(OdDbProxyObject::desc()) == true);

		CreatePlaceHolder(m_pICdwg, pObject);
	}

	return;
}

void DWGdirectReader::CreatePlaceHolder(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pDwg != NULL);
	ASSERT(pObject != NULL);
	ASSERT(pObject->isKindOf(OdDbPlaceHolder::desc()) == true || pObject->isKindOf(OdDbProxyObject::desc()) == true);

	if (pObject->isKindOf(OdDbPlaceHolder::desc()) == false && pObject->isKindOf(OdDbProxyObject::desc()) == false)
		return;

	db_placeholder* pNewPlaceHolder = NULL;

	pNewPlaceHolder = new db_placeholder();
	ASSERT(pNewPlaceHolder != NULL);
	if (pNewPlaceHolder == NULL)
		return;

	TransferHandleFromDWGdirect(pNewPlaceHolder, pObject);
	TransferEEDataFromDWGdirect(pDwg, pNewPlaceHolder, pObject);
	TransferReactorsFromDWGdirect(pDwg, pNewPlaceHolder, pObject, DB_HARD_POINTER);

	pDwg->addhanditem(pNewPlaceHolder);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateIDBuffer(db_drawing* pDb, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pDb != NULL);
	if (pDb == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbIdBuffer::desc()) == true);
	if (pObject->isKindOf(OdDbIdBuffer::desc()) == false)
		return;

	db_idbuffer* pNewIDBuffer = new db_idbuffer();
	ASSERT(pNewIDBuffer != NULL);
	if (pNewIDBuffer == NULL)
		return;

	TransferHandleFromDWGdirect(pNewIDBuffer, pObject);
	TransferEEDataFromDWGdirect(pDb, pNewIDBuffer, pObject);
	TransferReactorsFromDWGdirect(pDb, pNewIDBuffer, pObject, DB_HARD_POINTER);

	OdDbIdBufferPtr pIDBuffer = pObject;

	OdDbIdBufferIteratorPtr  pIDBuffIter = pIDBuffer->newIterator();
	for(pIDBuffIter->start(); !pIDBuffIter->done(); pIDBuffIter->next())
	{
		OdDbObjectId objId = pIDBuffIter->id();
		if (!objId.isNull())
		{
			OdString stringHandle = objId.getHandle().ascii();

			db_hireflink* pNewLink = new db_hireflink(DB_HARD_POINTER, db_objhandle(stringHandle.c_str()), NULL);
			pNewIDBuffer->addhiref(pNewLink);
		}
	}

#ifdef _DEBUG_MHB
	OdString stringHandle = pObject->objectId().getHandle().ascii();
	ASSERT(pDb->handent(db_objhandle(stringHandle.c_str())) == NULL);

	char sBuf[100];
	sprintf(sBuf, "IDBuffer was created\n");
	OutputDebugString(sBuf);

#endif

	pDb->addhanditem(pNewIDBuffer);
	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateDimAssoc(db_drawing* pDb, db_handitem* pDim, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pDb != NULL);
	if (pDb == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbDimAssoc::desc()) == true);
	if (pObject->isKindOf(OdDbDimAssoc::desc()) == false)
		return;

	db_dimassoc* pNewDimAssoc = new db_dimassoc();
	ASSERT(pNewDimAssoc != NULL);
	if (pNewDimAssoc == NULL)
		return;

	TransferHandleFromDWGdirect(pNewDimAssoc, pObject);
	TransferEEDataFromDWGdirect(pDb, pNewDimAssoc, pObject);
	TransferReactorsFromDWGdirect(pDb, pNewDimAssoc, pObject, DB_HARD_POINTER);

	OdDbDimAssocPtr pDimAssoc = pObject;

	pNewDimAssoc->set_70(pDimAssoc->transSpace());
	pNewDimAssoc->set_71(pDimAssoc->rotatedDimType());
	int flags = pDimAssoc->associativityFlag();
	pNewDimAssoc->set_90(flags);

	ASSERT(pDim != NULL);
	pNewDimAssoc->set_330(pDim);

	if (flags)
	{
		OdDbOsnapPointRefPtr osnapPointPtr;
		int osnapPtCount = 0;
		for (int i = 0; i < 4; i++) 
		{
			osnapPointPtr = pDimAssoc->osnapPointRef((OdDbDimAssoc::Associativity)i);

			if (!osnapPointPtr.isNull())
			{
				char clsName[] = "AcDbOsnapPointRef";
				pNewDimAssoc->set_1(clsName, osnapPtCount);

				pNewDimAssoc->set_72((int)osnapPointPtr->osnapMode(), osnapPtCount);
					
				// Main entity
				OdDbXrefFullSubentPath& mainEntity =  osnapPointPtr->mainEntity();
				OdDbObjectIdArray& objectIds = mainEntity.objectIds();
				int objCount = objectIds.size();
				if (objCount > 0)
				{
					// Main entity objects.
					for (int i = 0; i < objCount; i++)
					{
						OdDbObjectPtr objectPtr = objectIds.getAt(i).safeOpenObject();
						OdString objHandle = objectIds.getAt(i).getHandle().ascii();
						db_handitem* pObj = pDb->handent(db_objhandle(objHandle.c_str()));
						ASSERT(pObj != NULL);
						pNewDimAssoc->set_331(pObj, osnapPtCount);
					}

					// Main entitiy sub entities
					OdDbSubentId& subEntId = mainEntity.subentId();
					int subEntIndex = subEntId.index();
					OdDb::SubentType subType = subEntId.type();
					pNewDimAssoc->set_73((int)subType, osnapPtCount);
					pNewDimAssoc->set_91(subEntIndex, osnapPtCount);

					// Main entity xrefs
					OdHandleArray& handles = mainEntity.xrefObjHandles();
					int handleCount = handles.size();
					ASSERT(handleCount >= 0 && handleCount <= 8);
					if (handleCount >= 0 && handleCount <= 8)
					{					
						pNewDimAssoc->set_xrefHandleCount(osnapPtCount, (long)handles.size());
						for (int i = 0; i < handleCount; i++)
						{
							unsigned char hand[8];
							handles.getAt(i).getIntoAsciiBuffer((char*)(&hand[0]));
							pNewDimAssoc->set_xrefObjHandle(osnapPtCount, hand);
						}
					}
				}
				// Intersect entity
				OdDbXrefFullSubentPath& intEntity =  osnapPointPtr->intersectEntity();
				objectIds = intEntity.objectIds();
				objCount = objectIds.size();
				if (objCount > 0)
				{
					// Intersect entity objects.
					for (int i = 0; i < objCount; i++)
					{
						OdDbObjectPtr objectPtr = objectIds.getAt(i).safeOpenObject();
						OdString objHandle = objectIds.getAt(i).getHandle().ascii();
						db_handitem* pObj = pDb->handent(db_objhandle(objHandle.c_str()));
						ASSERT(pObj != NULL);
						pNewDimAssoc->set_332(pObj, osnapPtCount);
					}

					// Intersect entitiy sub entities
					OdDbSubentId& intEntId = intEntity.subentId();
					int subEntIndex = intEntId.index();
					OdDb::SubentType subType = intEntId.type();
					pNewDimAssoc->set_74((int)subType, osnapPtCount);
					pNewDimAssoc->set_92(subEntIndex, osnapPtCount);

					// Intersect entity xrefs
					OdHandleArray& handles = intEntity.xrefObjHandles();
					int handleCount = handles.size();
					ASSERT(handleCount >= 0 && handleCount <= 8);
					if (handleCount >= 0 && handleCount <= 8)
					{					
						//pNewDimAssoc->set_xrefHandleCount(osnapPtCount, (long)handles.size());
						for (int i = 0; i < handleCount; i++)
						{
							unsigned char hand[8];
							handles.getAt(i).getIntoAsciiBuffer((char*)(&hand[0]));
							pNewDimAssoc->set_intXrefObjHandle(osnapPtCount, hand);
						}
					}
				}
				pNewDimAssoc->set_40(osnapPointPtr->nearOsnap(), osnapPtCount);

				sds_point point;
				CPYPT3D(point, osnapPointPtr->osnapPoint())
				pNewDimAssoc->set_10(point, osnapPtCount);

				pNewDimAssoc->set_75((osnapPointPtr->lastPointRef() == NULL) ? 1 : 0, osnapPtCount);

				osnapPtCount++;
			}
		}
	}

#ifdef _DEBUG_MHB
	OdString stringHandle = pObject->objectId().getHandle().ascii();
	ASSERT(pDb->handent(db_objhandle(stringHandle.c_str())) == NULL);

	char sBuf[100];
	sprintf(sBuf, "DimAssoc was created\n");
	OutputDebugString(sBuf);

#endif

	pDb->addhanditem(pNewDimAssoc);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateAllPlotSettings(db_dictionaryrec* pSettingsDictionary, OdDbDictionary* pObject)
{
	ASSERT(pSettingsDictionary != NULL);
	ASSERT(pObject != NULL);

	OdDbDictionaryIteratorPtr  pObjectIter = pObject->newIterator();
	for(; !pObjectIter->done(); pObjectIter->next())
	{
		CDbPlotSettings* pNewSettings = new CDbPlotSettings();
		ASSERT(pNewSettings != NULL);

		OdString settingsName = pObjectIter->name();
		db_objhandle objectHandle = db_objhandle(pObjectIter->objectId().getHandle().ascii());

		OdDbPlotSettingsPtr pSettings = pObjectIter->getObject();

		TransferHandleFromDWGdirect((db_handitem*)pNewSettings, pSettings);
		TransferEEDataFromDWGdirect(m_pICdwg, (db_handitem*)pNewSettings, pSettings);
		TransferReactorsFromDWGdirect(m_pICdwg, (db_handitem*)pNewSettings, pSettings, DB_SOFT_POINTER);

#ifdef _DEBUG_MHB
		ASSERT(m_pICdwg->handent(pNewSettings->RetHandle()) == NULL);

		char sBuf[100];
		sprintf(sBuf, "PlotSettings was created\n");
		OutputDebugString(sBuf);

#endif
		m_pICdwg->addhanditem(pNewSettings);

//		pNewSettings->setName((char*)settingsName.c_str(), m_pICdwg);
		pNewSettings->set_2((char*)pSettings->getPlotCfgName().c_str());
		pNewSettings->set_4((char*)pSettings->getCanonicalMediaName().c_str());
		pNewSettings->set_6((char*)pSettings->getPlotViewName().c_str());
		pNewSettings->set_7((char*)pSettings->getCurrentStyleSheet().c_str());
		pNewSettings->set_40(pSettings->getLeftMargin());
		pNewSettings->set_41(pSettings->getBottomMargin());
		pNewSettings->set_42(pSettings->getRightMargin());
		pNewSettings->set_43(pSettings->getTopMargin());

		double xmin, ymin, xmax, ymax;

		pSettings->getPlotPaperSize(xmin, ymin);
		pNewSettings->set_44(xmin);
		pNewSettings->set_45(ymin);

		pSettings->getPlotOrigin(xmin, ymin);
		pNewSettings->set_46(xmin);
		pNewSettings->set_47(ymin);

		pSettings->getPlotWindowArea(xmin, ymin, xmax, ymax);
		pNewSettings->set_48(xmin);
		pNewSettings->set_49(ymin);
		pNewSettings->set_140(xmax);
		pNewSettings->set_141(ymax);

		pSettings->getCustomPrintScale(xmin, ymin);
		pNewSettings->set_142(xmin);
		pNewSettings->set_143(ymin);

		pSettings->getStdScale(xmin);
		pNewSettings->set_147(xmin);

		OdGePoint2d point2D = pSettings->getPaperImageOrigin();
		pNewSettings->set_148(point2D.x);
		pNewSettings->set_149(point2D.y);

		int flags = 0;
		if (pSettings->plotViewportBorders())
			flags |= BIT1;
		if (pSettings->showPlotStyles())
			flags |= BIT2;
		if (pSettings->plotCentered())
			flags |= BIT4;
		if (pSettings->plotHidden())
			flags |= BIT8;
		if (pSettings->useStandardScale())
			flags |= BIT10;
		if (pSettings->plotPlotStyles())
			flags |= BIT20;
		if (pSettings->scaleLineweights())
			flags |= BIT40;
		if (pSettings->printLineweights())
			flags |= BIT80;
		if (pSettings->drawViewportsFirst())
			flags |= BIT200;
		if (pSettings->modelType())
			flags |= BIT400;
		pNewSettings->set_70(flags);

		pNewSettings->set_72(pSettings->plotPaperUnits());
		pNewSettings->set_73(pSettings->plotRotation());
		pNewSettings->set_74(pSettings->plotType());
		pNewSettings->set_75(pSettings->stdScaleType());
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateWipeoutVariables(OdDbObject* pObject)
{
	ASSERT(pObject != NULL);

	OdDbWipeoutVariablesPtr pVars = pObject;
    db_wipeoutvariables *pNewVars = new db_wipeoutvariables();
	ASSERT(pNewVars != NULL);

	if (pNewVars != NULL)
	{
		TransferHandleFromDWGdirect(pNewVars, pVars);
		TransferEEDataFromDWGdirect(m_pICdwg, pNewVars, pVars);
		TransferReactorsFromDWGdirect(m_pICdwg, pNewVars, pVars, DB_HARD_POINTER);

		pNewVars->set_70(pVars->showFrame());

#ifdef _DEBUG_MHB
		OdString stringHandle = pVars->objectId().getHandle().ascii();
		ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);

		char sBuf[100];
		sprintf(sBuf, "WipeOut Vars was created\n");
		OutputDebugString(sBuf);
#endif
		m_pICdwg->addhanditem(pNewVars);
	}
	return;
}

//-----------------------------------------------------------------------------
//
db_dictionary* DWGdirectReader::CreateDictionary(db_drawing* pDwg, OdDbObject* DWGdirectObject)
{
	ASSERT(DWGdirectObject != NULL);
	if (DWGdirectObject == NULL)
		return NULL;

	ASSERT(DWGdirectObject->isKindOf(OdDbDictionary::desc()) == true);
	if (DWGdirectObject->isKindOf(OdDbDictionary::desc()) == false)
		return NULL;

	db_dictionary* pNewDictionary = new db_dictionary();
	ASSERT(pNewDictionary != NULL);
	if (pNewDictionary == NULL)
		return NULL;

	TransferHandleFromDWGdirect((db_handitem*)pNewDictionary, DWGdirectObject);
	TransferEEDataFromDWGdirect(pDwg, (db_handitem*)pNewDictionary, DWGdirectObject);
	TransferReactorsFromDWGdirect(pDwg, (db_handitem*)pNewDictionary, DWGdirectObject, DB_HARD_POINTER);

	pNewDictionary->set_280((((OdDbDictionaryPtr)DWGdirectObject)->isTreatElementsAsHard() == true) ? 1 : 0);
	pNewDictionary->set_281(((OdDbDictionaryPtr)DWGdirectObject)->mergeStyle());

	pDwg->addhanditem(pNewDictionary);

	return pNewDictionary;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateXRecord(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbXrecord::desc()));
	if (!pObject->isKindOf(OdDbXrecord::desc()))
		return;

	OdDbXrecordPtr pXRecord = pObject;
	ASSERT(!pXRecord.isNull());

	db_xrecord* pNewXRecord = new db_xrecord();
	ASSERT(pNewXRecord != NULL);

	if (pNewXRecord != NULL)
	{
		TransferHandleFromDWGdirect(pNewXRecord, pXRecord);
		TransferEEDataFromDWGdirect(pDwg, pNewXRecord, pXRecord);
		TransferReactorsFromDWGdirect(pDwg, pNewXRecord, pXRecord, DB_HARD_POINTER);

		pNewXRecord->set_280(pXRecord->mergeStyle());

		OdResBufPtr pResChain = pXRecord->rbChain(pObject->database());
		if (!pResChain.isNull())
		{
			pNewXRecord->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pResChain, pObject->database()));
		}
		pDwg->addhanditem(pNewXRecord);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateDLPNTableRecord(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbObject::desc()));
	if (!pObject->isKindOf(OdDbObject::desc()))
		return;

	CAseDLPNTableRecordPtr pObjectPtr = pObject;
	ASSERT(!pObjectPtr.isNull());
	OdString stringHandle = pObjectPtr->objectId().getHandle().ascii();

	db_handitem* existingItem = pDwg->handent(db_objhandle(stringHandle.c_str()));
	if (existingItem != NULL)
		return;

	db_object_ptr* pNewObjectPtr = new db_object_ptr();
	ASSERT(pNewObjectPtr != NULL);
	
	if (pNewObjectPtr != NULL)
	{
		TransferHandleFromDWGdirect(pNewObjectPtr, pObjectPtr);
		TransferEEDataFromDWGdirect(pDwg, pNewObjectPtr, pObjectPtr);
		// Extension Dictionary
		TransferExtensionDictionariesFromDWGdirect(pNewObjectPtr, pObjectPtr, pDwg);
		TransferReactorsFromDWGdirect(pDwg, pNewObjectPtr, pObjectPtr, DB_HARD_POINTER);

		CDxfFiler tmpFiler(pObject->database());
		pObjectPtr->dxfOutFields(&tmpFiler);
		OdResBufPtr pOdResBuf = tmpFiler.getFilerData();

		pNewObjectPtr->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pObject->database()));

		OdRxClassPtr pRegisteredClass = pObjectPtr->isA();
		ASSERT(!pRegisteredClass.isNull());
		if (!pRegisteredClass.isNull())
		{
			pNewObjectPtr->setOdDbClass((void*)pRegisteredClass.detach());
		}

		pDwg->addhanditem(pNewObjectPtr);
	}

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateRasterVariables(OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbRasterVariables::desc()));
	if (!(pObject->isKindOf(OdDbRasterVariables::desc())))
		return;

	OdDbRasterVariablesPtr pRasterVar = pObject;

	db_rastervariables* pNewRasterVars = new db_rastervariables();
	ASSERT(pNewRasterVars != NULL);
	if (pNewRasterVars == NULL)
		return;

	TransferHandleFromDWGdirect(pNewRasterVars, pObject);
	TransferEEDataFromDWGdirect(m_pICdwg, pNewRasterVars, pObject);
	TransferReactorsFromDWGdirect(m_pICdwg, pNewRasterVars, pObject, DB_HARD_POINTER);

	// TODO MHB 2 == R14
	pNewRasterVars->set_90(2/*adob.rastervariables.classversion*/);

	// TODO MHB frame can be 0,1,2 Test result of setting to 2 ??
	OdDbRasterVariables::FrameSettings frame = pRasterVar->imageFrame();
	ASSERT(frame >= 0);
	pNewRasterVars->set_70(frame);

	OdDbRasterVariables::ImageQuality quality = pRasterVar->imageQuality();
	ASSERT(quality >= 0);
	pNewRasterVars->set_71(quality);

	OdGiRasterImage::Units scale = pRasterVar->userScale();
	ASSERT(scale >= OdGiRasterImage::kNone && scale <= OdGiRasterImage::kMile);
	if (scale > OdGiRasterImage::kMile)
		scale = OdGiRasterImage::kNone;
	pNewRasterVars->set_72(scale);

#ifdef _DEBUG_MHB
	OdString stringHandle = pRasterVar->objectId().getHandle().ascii();
	ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);

	char sBuf[100];
	sprintf(sBuf, "RasterVars was created\n");
	OutputDebugString(sBuf);

#endif

	m_pICdwg->addhanditem(pNewRasterVars);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateSpatialFilter(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbSpatialFilter::desc()));
	if (!(pObject->isKindOf(OdDbSpatialFilter::desc())))
		return;

	OdDbSpatialFilterPtr pSpatialFilter = pObject;

	db_spatial_filter* pNewSpatialFilter = new db_spatial_filter();
	ASSERT(pNewSpatialFilter != NULL);
	if (pNewSpatialFilter == NULL)
		return;

	TransferHandleFromDWGdirect(pNewSpatialFilter, pObject);
	TransferEEDataFromDWGdirect(pDwg, pNewSpatialFilter, pObject);
	TransferReactorsFromDWGdirect(pDwg, pNewSpatialFilter, pObject, DB_HARD_POINTER);

	sds_point point;
	CPYPT3D(point, pSpatialFilter->normal());
	pNewSpatialFilter->set_extru(point);

	pNewSpatialFilter->set_frontclipon(pSpatialFilter->frontClipEnabled());
	pNewSpatialFilter->set_backclipon(pSpatialFilter->backClipDistEnabled());
	pNewSpatialFilter->set_frontclip(pSpatialFilter->frontClipDist());
	pNewSpatialFilter->set_backclip(pSpatialFilter->backClipDist());

	CPYPT3D(point, pSpatialFilter->origin());
	pNewSpatialFilter->set_11(point);

	pNewSpatialFilter->set_71(pSpatialFilter->isEnabled());
	{
		OdGePoint2dArray pts;
		pSpatialFilter->boundary(pts);
		int pointCount = pts.length();

		sds_point *icadPoints= new sds_point[pointCount];
		memset(icadPoints,0,pointCount * sizeof (sds_point));

		for (int i = 0; i < pointCount; i++) 
		{
			CPYPT2D(icadPoints[i], pts.getAt(i));
		}
		pNewSpatialFilter->set_ptarray(icadPoints,pointCount);
		delete [] icadPoints;
	}
	sds_point icadMatrix[4];
	OdGiClipBoundary boundaryInfo;
	pSpatialFilter->getDefinition(boundaryInfo);

	int j;
	OdGeMatrix3d tmpMat = boundaryInfo.m_xInverseBlockRefXForm;
	for(int i = 0; i < 4; ++i)
	{
		for(j = 0; j < 4; ++j)
		{
			icadMatrix[i][j] = tmpMat[j][i];
		}
	}
	pNewSpatialFilter->set_inverseblockmat(icadMatrix);


	tmpMat = boundaryInfo.m_xToClipSpace;
 	for(i = 0; i < 4; ++i)
	{
		for(j = 0; j < 4; ++j)
		{
			icadMatrix[i][j] = tmpMat[j][i];
		}
	}
	pNewSpatialFilter->set_clipboundmat(icadMatrix);

#ifdef _DEBUG_MHB
	OdString stringHandle = pObject->objectId().getHandle().ascii();
	ASSERT(m_pICdwg->handent(db_objhandle(stringHandle.c_str())) == NULL);

	char sBuf[100];
	sprintf(sBuf, "Spatial Filter was created\n");
	OutputDebugString(sBuf);

#endif
	pDwg->addhanditem(pNewSpatialFilter);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateSpatialIndex(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbSpatialIndex::desc()));
	if (!(pObject->isKindOf(OdDbSpatialIndex::desc())))
		return;

	OdDbSpatialIndexPtr pSpatialIndex = pObject;

	db_spatial_index* pNewSpatialIndex = new db_spatial_index();
	ASSERT(pNewSpatialIndex != NULL);
	if (pNewSpatialIndex == NULL)
		return;

	TransferHandleFromDWGdirect(pNewSpatialIndex, pObject);
	TransferEEDataFromDWGdirect(m_pICdwg, pNewSpatialIndex, pObject);
	TransferReactorsFromDWGdirect(m_pICdwg, pNewSpatialIndex, pObject, DB_HARD_POINTER);

	CDxfFiler tmpFiler(pObject->database());
	pSpatialIndex->dxfOutFields(&tmpFiler);
	OdResBufPtr pOdResBuf = tmpFiler.getFilerData();

	pNewSpatialIndex->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pObject->database()));

	m_pICdwg->addhanditem(pNewSpatialIndex);

	return;
}

//-----------------------------------------------------------------------------
// 
//void DWGdirectReader::CreateLayerIndex(db_drawing* pDwg, OdDbObject* pObject)
//{
//	ASSERT(pObject != NULL);
//	if (pObject == NULL)
//		return;
//
//	ASSERT(pObject->isKindOf(OdDbLayerIndex::desc()));
//	if (!(pObject->isKindOf(OdDbLayerIndex::desc())))
//		return;
//
//	OdDbLayerIndexPtr pLayerIndex = pObject;
//
//	db_layer_index* pNewLayerIndex = new db_layer_index();
//	ASSERT(pNewLayerIndex != NULL);
//	if (pNewLayerIndex == NULL)
//		return;
//
//	TransferHandleFromDWGdirect(pNewLayerIndex, pObject);
//	TransferEEDataFromDWGdirect(pDwg, pNewLayerIndex, pObject);
//	TransferReactorsFromDWGdirect(pDwg, pNewLayerIndex, pObject, DB_HARD_POINTER);
//
//	CDxfFiler tmpFiler(pObject->database());
//	pLayerIndex->dxfOutFields(&tmpFiler);
//	OdResBufPtr pOdResBuf = tmpFiler.getFilerData();
//
//	// TODO Note to self.
//	// Need to actually unwind the layerindex so I can create each
//	// IDBUFFER.
//	pNewLayerIndex->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pObject->database()));
//
//	OdRxClassPtr pRegisteredClass = pLayerIndex->isA();
//	ASSERT(!pRegisteredClass.isNull());
//	if (!pRegisteredClass.isNull())
//	{
//		pNewLayerIndex->setOdDbClass((void*)pRegisteredClass.detach());
//	}
//
//	pDwg->addhanditem(pNewLayerIndex);
//
//	return;
//}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateVBAproject(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbVbaProject::desc()));
	if (!(pObject->isKindOf(OdDbVbaProject::desc())))
		return;

	OdDbVbaProjectPtr pVBAproject = pObject;

	db_vbaproject* pNewVBAproject = new db_vbaproject();
	ASSERT(pNewVBAproject != NULL);
	if (pNewVBAproject == NULL)
		return;

	TransferHandleFromDWGdirect(pNewVBAproject, pObject);
	TransferEEDataFromDWGdirect(pDwg, pNewVBAproject, pObject);
	TransferReactorsFromDWGdirect(pDwg, pNewVBAproject, pObject, DB_HARD_POINTER);

	OdBinaryData odBinary;

	pVBAproject->getVbaProject(odBinary);
	pNewVBAproject->set_data(odBinary.length(), (char*)odBinary.asArrayPtr());

	pDwg->addhanditem(pNewVBAproject);

	return;
}

//-----------------------------------------------------------------------------
//
void DWGdirectReader::CreateObject(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(!"Should not have ended up here - don't know what this is??");

	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbObject::desc()));
	if (!(pObject->isKindOf(OdDbObject::desc())))
		return;

	db_object_ptr* pNewObject = new db_object_ptr();
	ASSERT(pNewObject != NULL);
	if (pNewObject == NULL)
		return;

	TransferHandleFromDWGdirect(pNewObject, pObject);
	TransferEEDataFromDWGdirect(m_pICdwg, pNewObject, pObject);	
	TransferReactorsFromDWGdirect(m_pICdwg, pNewObject, pObject, DB_SOFT_POINTER);

	CDxfFiler tmpFiler(pObject->database());
	pObject->dxfOutFields(&tmpFiler);
	OdResBufPtr pOdResBuf = tmpFiler.getFilerData();

	pNewObject->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pObject->database()));

	m_pICdwg->addhanditem(pNewObject);

	return;
}

//-----------------------------------------------------------------------------
//
db_handitem* DWGdirectReader::CreateProxyObject(db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pObject != NULL);
	if (pObject == NULL)
		return NULL;

	ASSERT(pObject->isKindOf(OdDbProxyObject::desc()));
	if (!(pObject->isKindOf(OdDbProxyObject::desc())))
		return NULL;

	OdDbProxyObjectPtr pProxy = pObject;
	OdString stringHandle = pObject->objectId().getHandle().ascii();

	db_handitem* existingItem = pDwg->handent(db_objhandle(stringHandle.c_str()));
	if (existingItem != NULL)
		return existingItem;

	db_acad_proxy_object* pNewProxy = new db_acad_proxy_object();
	ASSERT(pNewProxy != NULL);
	if (pNewProxy == NULL)
		return NULL;

	TransferHandleFromDWGdirect(pNewProxy, pObject);
	TransferEEDataFromDWGdirect(pDwg, pNewProxy, pObject);	
	// Extension Dictionary
	TransferExtensionDictionariesFromDWGdirect(pNewProxy, pProxy, pDwg);
	TransferReactorsFromDWGdirect(pDwg, pNewProxy, pObject, DB_SOFT_POINTER);

	CDxfFiler tmpFiler(pObject->database());
	pProxy->dxfOutFields(&tmpFiler);
	OdResBufPtr pOdResBuf = tmpFiler.getFilerData();

	pNewProxy->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pObject->database()));

	OdRxDictionaryIteratorPtr pIter = odDbGetDrawingClasses(*pObject->database());
	OdRxClassPtr pRegisteredClass;
	while(!pIter->done())
	{
		if (pIter->getKey() == pProxy->originalClassName())
		{
		  pRegisteredClass = pIter->object();
		  break;
		}
		pIter->next();
	}

	ASSERT(!pRegisteredClass.isNull());
	if (!pRegisteredClass.isNull())
	{
		pNewProxy->setOdDbClass((void*)pRegisteredClass.detach());
	}

	pDwg->addhanditem(pNewProxy);

	if (!(pObject->isKindOf(OdDbProxyObject::desc())))
		return pNewProxy;

	// Now handle references maintained by this proxy.
	OdTypedIdsArray references;
	((OdDbProxyObjectPtr)pObject)->getReferences(references);
	int count = references.size();

	proxyRef newProxyRefHandles(pNewProxy);

	for (int i = 0; i < count; i++)
	{
		OdDbTypedId typedId = references[i];
		OdDbObjectId objId = (OdDbObjectId)typedId;

		db_handitem* pEnt = NULL;
		if (!objId.isNull())
		{
			OdDbObjectPtr pObj = objId.safeOpenObject();
			OdString stringHandle = objId.getHandle().ascii();

			if (pObj->isKindOf(OdDbProxyObject::desc()))
			{
				pEnt = CreateProxyObject(pDwg, pObj);
				if (pEnt != NULL)
				{
					db_hireflink* hirLink = new db_hireflink(((((int)typedId.getRefType() + 3) * 10) + 300), db_objhandle(stringHandle), pEnt);
					pNewProxy->addhiref(hirLink);
				}

			}
			else if (pObj->isKindOf(OdDbEntity::desc()))
			{
				newProxyRefHandles.addReference(((((int)typedId.getRefType() + 3) * 10) + 300), stringHandle);
			}
			else
			{
				// This may in fact be valid. For instance a proxy can reference a block record.
				// In that case we wouldn't create it, we would let normal processing create it
				// when it is time.
				pEnt = NULL;
				pEnt = pDwg->handent(db_objhandle(stringHandle));
				db_hireflink * hirLink = new db_hireflink(((((int)typedId.getRefType() + 3) * 10) + 300), db_objhandle(stringHandle), pEnt);
				pNewProxy->addhiref(hirLink);
			}
		}
	}

	if (newProxyRefHandles.getRefHandles().size())
		g_proxyObjectReferences.insert(g_proxyObjectReferences.end(), newProxyRefHandles);

	return pNewProxy;
}

void DWGdirectReader::CreateSortEntTable(OdDbDictionary* pDict, db_drawing* pDwg, OdDbObject* pObject)
{
	ASSERT(pDwg != NULL);
	ASSERT(pObject != NULL);

	if (pObject == NULL)
		return;

	ASSERT(pObject->isKindOf(OdDbSortentsTable::desc()));
	if (!pObject->isKindOf(OdDbSortentsTable::desc()))
		return;

	OdDbSortentsTablePtr pSortTable = pObject;

	CDbSortentsTable* pNewSortTable = new CDbSortentsTable();
	ASSERT(pNewSortTable != NULL);
	if (pNewSortTable == NULL)
		return;

	TransferHandleFromDWGdirect(pNewSortTable, pObject);
	TransferEEDataFromDWGdirect(pDwg, pNewSortTable, pObject);	
	TransferReactorsFromDWGdirect(pDwg, pNewSortTable, pObject, DB_SOFT_POINTER);

#ifdef _DEBUG_MHB
	OdString stringHandle = pObject->objectId().getHandle().ascii();
	ASSERT(pDwg->handent(db_objhandle(stringHandle.c_str())) == NULL);

	char sBuf[100];
	sprintf(sBuf, "SortEnts was created\n");
	OutputDebugString(sBuf);
#endif

	// Space block handle.
	db_objhandle ownerHandle = db_objhandle(pDict->ownerId().getHandle().ascii());
	pNewSortTable->setBlock(ownerHandle);

	pDwg->addhanditem(pNewSortTable);

	if (ownerHandle == db_objhandle(pDict->database()->getModelSpaceId().getHandle().ascii()))
	{
		db_hireflink * msExtDictLink = pDwg->ret_xdict(0);
		db_dictionary* pExtDict = (db_dictionary*)msExtDictLink->ret_hip(pDwg);

		db_dictionaryrec* rec = new db_dictionaryrec("ACAD_SORTENTS", DB_HARD_POINTER, pNewSortTable->RetHandle(), pNewSortTable);

		pExtDict->addrec(rec);
	}

	if (ownerHandle == db_objhandle(pDict->database()->getPaperSpaceId().getHandle().ascii()))
	{
		db_hireflink * psExtDictLink = pDwg->ret_xdict(1);
		db_dictionary* pExtDict = (db_dictionary*)psExtDictLink->ret_hip(pDwg);

		db_dictionaryrec* rec = new db_dictionaryrec("ACAD_SORTENTS", DB_HARD_POINTER, pNewSortTable->RetHandle(), pNewSortTable);

		pExtDict->addrec(rec);
	}

	// This call gets the IDs in absolute (sorted) order.
	HandlePairsArray HandlePairs;
	pSortTable->getAbsoluteDrawOrder(HandlePairs);		// Absolute draw order

	// To get the IDs in the original unsorted order, you have to iterate
	// the block record to get the ids in the order they were added to the
	// table record.
	OdDbObjectId blockId = pSortTable->blockId();
	OdDbBlockTableRecordPtr pBlockRecord = pSortTable->blockId().safeOpenObject();
	OdDbObjectIteratorPtr pRecordIter = pBlockRecord->newIterator();

	int handleCount = HandlePairs.size();
	handleCount = 0;
	for (int i = 0; i < handleCount; i++)
	{
		HandlePair pairs = HandlePairs[i];
		OdDbHandle key = pairs.first;
		OdDbSoftPointerId value = pairs.second;
		
		db_objhandle orighandle   = db_objhandle(key.ascii());
		db_objhandle sorthandle   = db_objhandle(value.getHandle().ascii());
		pNewSortTable->insert(orighandle, sorthandle);
	}

	return;
}


void DWGdirectReader::processDelayedEntities(delayedConversionMap* pEnts)
{
	delayedConversionMap* delayedEntities;

	if (pEnts == NULL)
		delayedEntities = &m_delayedEntities;
	else
		delayedEntities = pEnts;

	int count = delayedEntities->size();

	if (count == 0)
		return;

	for (int i = 0; i < count; i++)
	{
		handItemObjectIDPair tmpPtrs = delayedEntities->getAt(i);

		db_handitem* pOwner = tmpPtrs.getHandItem();
		OdDbObjectId entityId = tmpPtrs.getObjectId();
		int space = tmpPtrs.getSpace();

		if (entityId.isNull())
			continue;

		db_handitem* pEnt = NULL;
		try
		{
			OdDbEntityPtr pEntity = entityId.safeOpenObject();
			// Use protocol extension to create a db version of this entity.
			OdSmartPtr<OdDbEntity_Creator> pEntCreator = pEntity;

			pEnt = pEntCreator->createHanditem(pEntity, m_pICdwg, (db_blocktabrec*)pOwner);
			if (space)
			{
				// Set sub-ents space flag too.
				db_handitem* tmp = pEnt;
				while(tmp)
				{
					tmp->set_67(1);
					tmp = tmp->next;
				}
			}
#ifdef _DEBUG_MHB
			ASSERT(pEnt != NULL);
#endif
		}
			catch (OdError& err)
			{
				OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
				int error = err.code();
				OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirectReader::processDelayedEntities() entity creation error: ");
#endif
			}
	}
	delayedEntities->clear();

	return;
}

void DWGdirectReader::ProcessProxyObjectReferences()
{
	int count = g_proxyObjectReferences.size();

	if (count == 0)
		return;

	for (int i = 0; i < count; i++)
	{
		proxyRef tmpRef = g_proxyObjectReferences.getAt(i);
		db_acad_proxy_object* tmpProxy = tmpRef.getProxy();

		proxyRefs tmpReferences = tmpRef.getRefHandles();

		int refCount = tmpReferences.size();

		for (int j = 0; j < refCount; j++)
		{
			int type = tmpReferences[j].m_Type;
			OdString hand = tmpReferences[j].m_handle;

			db_handitem* pEnt = m_pICdwg->handent(db_objhandle(hand));;
			ASSERT(pEnt != NULL);
			if (pEnt != NULL)
			{
				db_hireflink * hirLink = new db_hireflink(type, db_objhandle(hand), pEnt);
				tmpRef.getProxy()->addhiref(hirLink);
			}
		}
	}
	g_proxyObjectReferences.clear();

	return;
}

void DWGdirectReader::importDwf(OdDbDatabasePtr& pDDdwg, char* fileName)
{
	OdDwfImportPtr importer = createImporter();
	if ( !importer.isNull() )
	{
		importer->properties()->putAt( "Database", pDDdwg );
		importer->properties()->putAt( "DwfPath", OdRxVariantValue( OdString(fileName) ) );
		importer->properties()->putAt( "Password", OdRxVariantValue( OdString("") ) );
		importer->properties()->putAt( "PaperWidth", OdRxVariantValue( 297. ) );
		importer->properties()->putAt( "PaperHeight", OdRxVariantValue( 210. ) );
		importer->import();
	}
	return;
}

void DWGdirectReader::setActiveLayoutTab(OdDbDatabasePtr& pDDdwg)
{
	OdDbObjectId activeLayoutBTRId = pDDdwg->getActiveLayoutBTRId();
	OdDbObjectId activeLayoutId = OdDbBlockTableRecordPtr(activeLayoutBTRId.safeOpenObject())->getLayoutId();
	OdString stringHandle = activeLayoutId.getHandle().ascii();
	CDbLayout* pLayout = (CDbLayout*)m_pICdwg->handent(db_objhandle(stringHandle.c_str()));
	ASSERT(pLayout != NULL);

	if (pLayout != NULL)
	{
		sds_resbuf cTab;
		cTab.rbnext = NULL;
		cTab.restype = RTSTR;
		cTab.resval.rstring = (char*)pLayout->name(m_pICdwg);
		db_setvar(NULL,DB_QCTAB, &cTab,m_pICdwg,NULL,NULL,0);
	}
	return;
}
