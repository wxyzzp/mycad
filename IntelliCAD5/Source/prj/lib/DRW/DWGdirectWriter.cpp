// DWGdirectWriter.cpp: implementation of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////

#include "Db.h"
#include "../../ICAD/res/icadrc2.h"

#include "DWGdirectWriter.h"
#include "DWGdirectConverter.h"
#include "Utilities.h"
#include "vxtabrec.h"

#include "DWGdirectReader.h" // For Audit.

// ICAD headers.
#include "styletabrec.h"
#include "ltypetabrec.h"
#include "layertabrec.h"
#include "appid.h"
#include "viewtabrec.h"
#include "ucstabrec.h"
#include "dimstyletabrec.h"
#include "vporttabrec.h"
#include "viewport.h"
#include "blocktabrec.h"
#include "..\\..\\prj\\lib\\db\\objects.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\Objects.h
#include "..\\..\\prj\\lib\\db\\DbLayout.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h 

// DWGdirect headers
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbTextStyleTable.h"
#include "DbTextStyleTableRecord.h"
#include "DbRegAppTable.h"
#include "DbRegAppTableRecord.h"
#include "DbLinetypeTable.h"
#include "DbLinetypeTableRecord.h"
#include "DbDimStyleTable.h"
#include "DbDimStyleTableRecord.h"
#include "DbViewportTable.h"
#include "DbViewportTableRecord.h"
#include "DbUCSTable.h"
#include "DbUCSTableRecord.h"
#include "DbViewTable.h"
#include "DbViewTableRecord.h"
#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "DbSymUtl.h"
#include "OdFileBuf.h"
#include "DbAudit.h"
#include "summinfo.h"
#include "DbDictionary.h"
#include "DbFiler.h"
#include "OdCharMapper.h"
#include "Gs.h"
#include "OdaDefs.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbGroup.h"
#include "DbBlockTableRecord.h"
#include "DbBlockBegin.h"
#include "DbBlockEnd.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h"
#include "DbMlineStyle.h"
#include "DbRasterImageDef.h"
#include "DbDictionaryVar.h"
#include "DbViewport.h"
#include "XRefMan.h"
#include "DbPlaceHolder.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbSortentsTable.h"

#include "CmColor.h"

// Compiling password protected r18 drawings loading support
#include "DWGdirectWriterCrypt.cpp"

#include "DWGdirectFiler.h"
#include "DbProxyObject.h"

#include "RxDynamicModule.h"

// Svg support stuff.
#include "SvgExport.h"
#include "..\..\..\Licensed\DWGdirect\Exports\SvgExport\Source\stdafx.h"
// Dwf support stuff.
#include "ColorMapping.h"
#include "DwfExport.h"
#include "RxDynamicModule.h"

//////////////////////////////////////////////////////////////////////////////
/// Utility class to get db_drawing data from IntelliCAD's technology and to
/// populate DWGdirect's OdDbDatabase structure and write it to disk..
//////////////////////////////////////////////////////////////////////////////
extern char *drw_str_msblockname;
extern char *drw_str_psblockname;

void _rollBackFile(OdString& tmpFileName, const char* fileName);
void _renameTmpFile(OdString& tmpFileName, const char* fileName);
//-----------------------------------------------------------------------------
DWGdirectWriter::DWGdirectWriter(OdRxObjectImpl<DWGdirectWriterServices>& exServices)
{
	try
	{
		m_pICdwg = NULL;
		m_pExServices = &exServices;
		odInitialize(m_pExServices);

		odrxInitWriterCrypt();
	}
	catch (OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		ASSERT(!"DWGdirectWriter initialzation error: ");
#endif
	}
}

//-----------------------------------------------------------------------------
DWGdirectWriter::~DWGdirectWriter()
{
	try
	{
		//odrxUninitModelerGeometry();
		odrxUninitWriterCrypt();
		odUninitialize();
	}
	catch (OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		ASSERT(!"DWGdirectWriter uninitialzation error: ");
#endif
	}
}

//-----------------------------------------------------------------------------
bool DWGdirectWriter::writeDiskFile(char* fileName, db_drawing* drawing, short fileType, int version, bool bUsePasswd, bool bSavePreview)
{
	ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModelerModule);
	ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(WinGDIModule);

	ODRX_BEGIN_STATIC_MODULE_MAP()
		ODRX_DEFINE_STATIC_APPLICATION("ModelerGeometry"/*DNT*/,  ModelerModule)
		ODRX_DEFINE_STATIC_APPLICATION("WinGDI"/*DNT*/,           WinGDIModule)
	ODRX_END_STATIC_MODULE_MAP()

	ODRX_INIT_STATIC_MODULE_MAP();

	bool retVal = false;
	OdDb::DwgVersion saveVersion = OdDb::vAC18; 

	int extended_type = 0;
	OdDb::SaveType saveType = OdDb::kDwg;
	switch (fileType)
	{
	case 0:
		saveType = OdDb::kDwg;
		break;
	case 1:
		saveType = OdDb::kDxf;
		break;
	case 2:
		saveType = OdDb::kDxb;
		break;
	case 3:
		saveType = OdDb::kDwg;	// yes dwg.
		break;
	case 4:
		extended_type = DWGdirectWriter::SaveType::kSvg;
		break;
	case 5:
		extended_type = DWGdirectWriter::SaveType::kDwf;
		break;
	}


	// TODO - MHB IntelliCAD version integers don't align with DWGdirect's
	// OdDb::DwgVersion enums. ICAD needs to be extended anyway.
	if (saveType == OdDb::kDwg && version < 4)
		version = 8;

	if (extended_type == 0)
	{
		switch(version)
		{
		case 0:
			saveVersion = OdDb::vAC025;
			break;
		case 1:
			saveVersion = OdDb::vAC026;
			break;
		case 2:
			saveVersion = OdDb::vAC09;
			break;
		case 3:
			saveVersion = OdDb::vAC10;
			break;
		case 4:
			saveVersion = OdDb::vAC12;
			break;
		case 5:
			saveVersion = OdDb::vAC13;
			break;
		case 6:
			saveVersion = OdDb::vAC14;
			break;
		case 7:
			saveVersion = OdDb::vAC15;
			break;
		case 8:
			saveVersion = OdDb::vAC18;
			break;
		default: 
			saveVersion = OdDb::vAC18;
			break;
		}
	}

	OdDbDatabasePtr pDDdwg;
	try
	{
		m_pICdwg = drawing;

		pDDdwg = m_pExServices->createDatabase(true);

		ASSERT(m_pICdwg != NULL);
		ASSERT(!pDDdwg.isNull());

	}
	catch (OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		ASSERT(!"DWGdirectWriter create file error: ");
#endif
		db_progressstop();
	}
    catch (...)
    {
#ifdef _DEBUG_MHB
		ASSERT(!"DWGdirectWriter file create error: unknown");
#endif
		db_progressstop();
	}

	if (m_pICdwg != NULL && !pDDdwg.isNull())
	{
		try
		{
			DWGdirectConverter tmpConverter(m_pICdwg, pDDdwg, saveVersion, saveType, *m_pExServices);
			m_Converter = &tmpConverter;

			// This will set the handseed high enough where there shouldn't
			// be any collisions with the things we will be stuffing in.
			struct sds_resbuf resBuf;
			resBuf.restype=RTSTR;
			resBuf.rbnext = NULL;
			db_getvar(NULL,DB_QHANDSEED, &resBuf,m_pICdwg,NULL,NULL);
			OdDbHandle handle(resBuf.resval.rstring);
			IC_FREE(resBuf.resval.rstring);
			pDDdwg->getOdDbObjectId(handle, true); 

			retVal = populateDatabase(pDDdwg);
			ASSERT(retVal == true);

			wrapUp(pDDdwg);
		}
		catch (OdError& err)
		{
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
#ifdef _DEBUG_MHB
			ASSERT(!"DWGdirectWriter error populating OdDbDatabase: ");
#endif
			db_progressstop();
		}

		try
		{
			OdString tmpFileName(fileName);
			tmpFileName = tmpFileName.left(tmpFileName.getLength() - 4);
			tmpFileName += EXTTMP;

			switch(extended_type)
			{
			case 0:
				break;
			case DWGdirectWriter::SaveType::kSvg:	// saveas svg;
				{
					bool retVal = false;
					try
					{
						retVal = saveAsSVG(pDDdwg, (char*)tmpFileName.c_str());
					}
					catch(...)
					{
						// We've had a problem.
						_rollBackFile(tmpFileName, fileName);

					}
					if (retVal == true)
					{
						// rename xxx.$$$ to xxx.dwg/.dxf
						_renameTmpFile(tmpFileName, fileName);
					}
					return retVal;
				}
			case DWGdirectWriter::SaveType::kDwf:	// saveas dwf;
				{
					try
					{
						//retVal = saveAsDWF(pDDdwg, fileName, version);
						retVal = saveAsDWF(pDDdwg, (char*)tmpFileName.c_str(), version);
					}
					catch(...)
					{
						// We've had a problem.
						_rollBackFile(tmpFileName, fileName);
					}
					if (retVal == true)
					{
						// rename xxx.$$$ to xxx.dwg/.dxf
						_renameTmpFile(tmpFileName, fileName);
					}
					return retVal;
				}
			}

			if (bUsePasswd == true)
			{
				OdPassword password;
				m_pExServices->getPassword(OdString(fileName), false, password);

				OdSecurityParams tmp;
				tmp.password = password;
				tmp.nFlags = SECURITYPARAMS_ENCRYPT_DATA;
				tmp.nProvType = 0xd;
				// "Microsoft Base DSS and Diffie-Hellman Cryptographic Provider"
				static wchar_t pn[] =
				{
					0x4D, 0x69, 0x63, 0x72, 0x6F,
					0x73, 0x6F, 0x66, 0x74, 0x20,
					0x42, 0x61, 0x73, 0x65, 0x20,
					0x44, 0x53, 0x53, 0x20, 0x61,
					0x6E, 0x64, 0x20, 0x44, 0x69,
					0x66, 0x66, 0x69, 0x65, 0x2D,
					0x48, 0x65, 0x6C, 0x6C, 0x6D,
					0x61, 0x6E, 0x20, 0x43, 0x72,
					0x79, 0x70, 0x74, 0x6F, 0x67,
					0x72, 0x61, 0x70, 0x68, 0x69,
					0x63, 0x20, 0x50, 0x72, 0x6F,
					0x76, 0x69, 0x64, 0x65, 0x72,
					0x00				
				};
				tmp.provName = pn;
				pDDdwg->setSecurityParams(tmp);
			}

			// Fix any errors present
			try
			{
				m_pExServices->setFixErrors(true);
				pDDdwg->auditDatabase(m_pExServices);
			}
			catch (OdError& err)
			{
				OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
				ASSERT(!"auditDatabase() error writing OdDbDatabase: ");
			}

			try
			{
				OdWrFileBuf pFileBuff(tmpFileName);
				pDDdwg->writeFile(&pFileBuff, saveType, saveVersion, bSavePreview);	// Saveas dwg or dxf

				m_pICdwg->init_FileVersion(version);
			}
			catch (OdError& err)
			{
				char buf[IC_ACADBUF];
				char errorBuffer[IC_ACADBUF] = {"\0"};
				OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
				if (err.code() == 32)
				{
					
					sprintf(errorBuffer, ResourceString(CMD_ERR_ERRORWITHSTRING, "Error: %s.\n\n"), msg.c_str());
					sprintf(buf, ResourceString(CMD_ERR_ERRORWRITINGFILE, "Your file could not be saved:\n%s.\n\n"),fileName);
					strcat(errorBuffer, buf);
					strcat(errorBuffer, ResourceString(CMD_ERR_ERRORWRITINGFILEMSG, "There may not be enough disk space.\nTry saving your file in a new location.\n"));
				}
				else
				{
					sprintf(errorBuffer, ResourceString(CMD_ERR_ERRORWITHSTRING, "Error: %s.\n\n"), msg.c_str());
					sprintf(buf, ResourceString(CMD_ERR_ERRORWRITINGFILE, "Your file could not be saved:\n%s.\n\n"),fileName);
					strcat(errorBuffer, buf);
				}

				sds_resbuf resBuf;
				IcadSharedGlobals::CallGetVar(DB_QEXPERT,&resBuf);
				if (resBuf.resval.rint < 5 && errorBuffer[0] != '\0')
				{
					MessageBox(GetActiveWindow(), errorBuffer, ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
				}
				else
				{
					// Echo to the command line instead.
					IcadSharedGlobals::CallCmdMessage(errorBuffer);
				}
			}
			catch(...)
			{
//				// We've had a problem.
//				DWORD error = GetLastError();
//				LPVOID lpMsgBuf;
//				FormatMessage( 
//					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//					FORMAT_MESSAGE_FROM_SYSTEM | 
//					FORMAT_MESSAGE_IGNORE_INSERTS,
//					NULL,
//					GetLastError(),
//					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//					(LPTSTR) &lpMsgBuf,
//					0,
//					NULL 
//				);
//				// Process any inserts in lpMsgBuf.
//				// ...
//				// Display the string.
//				MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
//				// Free the buffer.
//				LocalFree( lpMsgBuf );
 
				_rollBackFile(tmpFileName, fileName);

				char buf[IC_ACADBUF];
				char errorBuffer[IC_ACADBUF] = {"\0"};
				sprintf(errorBuffer, ResourceString(CMD_ERR_ERRORWITHSTRING, "Error: %s.\n\n"), ResourceString(IDC_ICADAPI_UNKNOWN_PROBLEM_40, "Unknown problem."));
				sprintf(buf, ResourceString(CMD_ERR_ERRORWRITINGFILE, "Your file could not be saved:\n%s.\n\n"),fileName);
				strcat(errorBuffer, buf);

				sds_resbuf resBuf;
				IcadSharedGlobals::CallGetVar(DB_QEXPERT,&resBuf);
				if (resBuf.resval.rint < 5 && errorBuffer[0] != '\0')
				{
					MessageBox(GetActiveWindow(), errorBuffer, ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
				}
				else
				{
					// Echo to the command line instead.
					IcadSharedGlobals::CallCmdMessage(errorBuffer);
				}			
			}

			if (retVal == true)
			{
				// rename xxx.$$$ to xxx.dwg/.dxf
				_renameTmpFile(tmpFileName, fileName);
			}
		}
		catch (OdError& err)
		{
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectWriter error writing OdDbDatabase: ");
			db_progressstop();
		}
	}

	return retVal;
}

db_drawing* DWGdirectWriter::auditFile(db_drawing* pDwg, bool fixErrors, short writeLogFile)
{
	ASSERT(pDwg != NULL);

	ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModelerModule);
	ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(WinGDIModule);

	ODRX_BEGIN_STATIC_MODULE_MAP()
		ODRX_DEFINE_STATIC_APPLICATION("ModelerGeometry"/*DNT*/,  ModelerModule)
		ODRX_DEFINE_STATIC_APPLICATION("WinGDI"/*DNT*/,           WinGDIModule)
	ODRX_END_STATIC_MODULE_MAP()

	ODRX_INIT_STATIC_MODULE_MAP();

	OdDbDatabasePtr pDDdwg;
	try
	{
		m_pICdwg = pDwg;

		pDDdwg = m_pExServices->createDatabase(true);
		ASSERT(!pDDdwg.isNull());
	}
	catch (OdError& err)
	{
		OdError err1 = err; // Shut the compiler up.
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
#ifdef _DEBUG_MHB
		ASSERT(!"DWGdirectWriter audit create file error: ");
#endif
		db_progressstop();
	}
    catch (...)
    {
#ifdef _DEBUG_MHB
		ASSERT(!"DWGdirectWriter file write error: unknown");
#endif
		db_progressstop();
	}

	if (pDwg != NULL && !pDDdwg.isNull())
	{
		bool retVal = false;
		try
		{
			OdDb::SaveType saveType = OdDb::kDwg;
			OdDb::DwgVersion saveVersion = OdDb::vAC18; 
			DWGdirectConverter tmpConverter(pDwg, pDDdwg, saveVersion, saveType, *m_pExServices);
			m_Converter = &tmpConverter;

			// This will set the handseed high enough where there shouldn't
			// be any collisions with the things we will be stuffing in.
			struct sds_resbuf resBuf;
			resBuf.restype=RTSTR;
			resBuf.rbnext = NULL;
			db_getvar(NULL,DB_QHANDSEED, &resBuf,m_pICdwg,NULL,NULL);
			OdDbHandle handle(resBuf.resval.rstring);
			IC_FREE(resBuf.resval.rstring);
			pDDdwg->getOdDbObjectId(handle, true); 

			retVal = populateDatabase(pDDdwg);
			ASSERT(retVal == true);

			wrapUp(pDDdwg);
		}
		catch (OdError& err)
		{
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
#ifdef _DEBUG_MHB
			ASSERT(!"DWGdirectWriter error audit populating OdDbDatabase: ");
#endif
			db_progressstop();
		}

		if (retVal == true)
		{
			int numErrors = 0;
			bool auditOK = false;
			// Fix any errors present
			try
			{
				m_pExServices->setFixErrors(fixErrors);
				pDDdwg->auditDatabase(m_pExServices);
				numErrors = ReadWriteUtils::reportAuditInfo(m_pICdwg, m_pExServices->summaryList, 
															m_pExServices->errorList,
															writeLogFile);

				auditOK = true;
			}
			catch (OdError& err)
			{
				OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
				ASSERT(!"auditDatabase() error auditing OdDbDatabase: ");
			}

			if (auditOK == true)
			{
				if (numErrors == 0 || fixErrors == false)
					return pDwg;

				// Need to create a new db_drawing and repopulate it from
				// the audited OdDbDatabase and pass it back.
				db_drawing *dp=new db_drawing(1);
				dp->SetPathName(m_pICdwg->ret_pn());
	
				OdRxObjectImpl<DWGdirectReaderServices> exServices;

				DWGdirectReader DWGReader(exServices);
				bool result = DWGReader.readMemoryFile(dp, pDDdwg);

				if (result == false)
				{
					delete dp;
					dp = NULL;
					return pDwg;
				}
				else
					return dp;
			}
			else
			{
				return pDwg;
			}
		}
	}

	// On any problems return same dwg passed in.
	return pDwg;
}

// Note the order of these calls is important.
bool DWGdirectWriter::populateDatabase(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	// Registered applications
	try
	{
		TransferRegAppsToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferRegAppsToDWGdirect error populating OdDbDatabase: ");
	}

	// Styles
	try
	{
		TransferTextStylesToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferTextStylesToDWGdirect error populating OdDbDatabase: ");
	}

	// Linetypes
	try
	{
		TransferLineTypesToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferLineTypesToDWGdirect error populating OdDbDatabase: ");
	}

	// Layers
	try
	{
		TransferLayersToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferLayersToDWGdirect error populating OdDbDatabase: ");
	}

	// UCSs
	try
	{
		TransferUCSsToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferUCSsToDWGdirect error populating OdDbDatabase: ");
	}

	// Viewports
	try
	{
		TransferViewportsToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferViewportsToDWGdirect error populating OdDbDatabase: ");
	}

	// Views
	try
	{
		TransferViewsToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferViewsToDWGdirect error populating OdDbDatabase: ");
	}

	// Blocks.
	try
	{
		TransferBlocksToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferBlocksToDWGdirect error populating OdDbDatabase: ");
	}

	// Dimension styles.
	try
	{
		TransferDimStylesToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferDimStylesToDWGdirect error populating OdDbDatabase: ");
	}

	// Entities paper & model
	try
	{
		TransferEntitiesToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferEntitiesToDWGdirect error populating OdDbDatabase: ");
	}

	// Objects
	try
	{
		TransferObjectsToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferObjectsToDWGdirect error populating OdDbDatabase: ");
	}

	// System Variables
	try
	{
		TransferSysVarsToDWGdirect(pDDdwg);
	}
	catch (OdError& err)
	{
		OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
		OutputDebugString("TransferSysVarsToDWGdirect error populating OdDbDatabase: ");
	}

	return true;
}

void DWGdirectWriter::TransferTextStylesToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	char styleName[DB_ACADBUF];
	OdDbTextStyleTableRecordPtr pStyle;
	db_styletabrec* styleRecord = NULL;

	OdDbTextStyleTablePtr pTable= pDDdwg->getTextStyleTableId().safeOpenObject(OdDb::kForWrite);
	for (styleRecord=(db_styletabrec*)m_pICdwg->tblnext(DB_STYLETABREC,1); styleRecord!=NULL; styleRecord=(db_styletabrec*)m_pICdwg->tblnext(DB_STYLETABREC,0)) 
	{
		int flags;
		styleRecord->get_70(&flags);
		if ((flags & BIT10) && (flags & BIT20))
			continue;	// Belongs to an xref and is created in TransferRemainingBlocksToDWGdirect()

		styleRecord->get_2(styleName,DB_ACADBUF - 1);
		if (stricmp(styleName, OdDbSymUtil::textStyleStandardName()) == 0)
		{
			pStyle = pDDdwg->getTextStyleStandardId().safeOpenObject(OdDb::kForWrite);
			OdDbObjectId odStyleAfterId = m_Converter->updateOdDbObject(pDDdwg->getTextStyleTableId(), (db_handitem*)styleRecord, pStyle);
		}
		else 
		{
			OdDbObjectId dimStyleId = m_Converter->createOdDbObject(pDDdwg->getTextStyleTableId(), (db_handitem*)styleRecord, OdString());
			pStyle = dimStyleId.safeOpenObject(OdDb::kForWrite);

			pStyle->setName(OdString(styleName));
			pTable->add(pStyle);
		}
	}
	return;
}

void DWGdirectWriter::TransferRegAppsToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	char appName[DB_ACADBUF];
	db_appidtabrec* appRecord = NULL;

	OdDbRegAppTablePtr pTable = pDDdwg->getRegAppTableId().safeOpenObject(OdDb::kForWrite);
	for (appRecord=(db_appidtabrec*)m_pICdwg->tblnext(DB_APPIDTAB,1); appRecord!=NULL; appRecord=(db_appidtabrec*)m_pICdwg->tblnext(DB_APPIDTAB,0)) 
	{
		appRecord->get_2(appName, DB_ACADBUF - 1);
		if (stricmp(appName, ACAD) == 0)
		{
			continue;
		}
		
		OdDbRegAppTableRecordPtr regAppPtr = OdDbRegAppTableRecord::createObject();
		// TODO MHB handle ??
		// Note currently, the call above also commits it to the database,
		// so I don't get a chance to set it's handle first.
		regAppPtr->setName(OdString(appName));
		pTable->add(regAppPtr);
	}

	return;
}

void DWGdirectWriter::TransferLineTypesToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	int flags;
	char linetypeName[DB_ACADBUF];
	OdDbLinetypeTableRecordPtr pLinetype;
	db_ltypetabrec* linetypeRecord = NULL;

	OdDbLinetypeTablePtr pTable = pDDdwg->getLinetypeTableId().safeOpenObject(OdDb::kForWrite);
	for (linetypeRecord=(db_ltypetabrec*)m_pICdwg->tblnext(DB_LTYPETABREC,1); linetypeRecord!=NULL; linetypeRecord=(db_ltypetabrec*)m_pICdwg->tblnext(DB_LTYPETABREC,0)) 
	{
		linetypeRecord->get_70(&flags);
		if ((flags & BIT10) && (flags & BIT20))
			continue;	// Belongs to an xref and is created in TransferRemainingBlocksToDWGdirect()

		bool bAddLineType = true;
		linetypeRecord->get_2(linetypeName, DB_ACADBUF - 1);

		if (stricmp(linetypeName, OdDbSymUtil::linetypeByBlockName()) == 0)
		{
			pLinetype = pDDdwg->getLinetypeByBlockId().safeOpenObject(OdDb::kForWrite);
			bAddLineType = false;
		}
		else if (stricmp(linetypeName, OdDbSymUtil::linetypeByLayerName()) == 0)
		{
			pLinetype = pDDdwg->getLinetypeByLayerId().safeOpenObject(OdDb::kForWrite);
			bAddLineType = false;
		}
		else if (stricmp(linetypeName, OdDbSymUtil::linetypeContinuousName()) == 0 )
		{

			pLinetype = pDDdwg->getLinetypeContinuousId().safeOpenObject(OdDb::kForWrite);
			bAddLineType = false;
		}
		else
		{
			pLinetype = OdDbLinetypeTableRecord::createObject();
			// TODO MHB handle ??
			bAddLineType = true;
		}

		setLineTypeProps(pDDdwg, pLinetype, linetypeRecord);

		if (bAddLineType == true)
		{
			pLinetype->setName(OdString(linetypeName));
			pTable->add(pLinetype);
		}
	}

	return;
}

void DWGdirectWriter::TransferLayersToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	char layerName[DB_ACADBUF];
	db_layertabrec* layerRecord = NULL;

	OdDbObjectId layerTableId = pDDdwg->getLayerTableId();
	OdDbLayerTablePtr pTable = layerTableId.safeOpenObject(OdDb::kForWrite);
	for (layerRecord=(db_layertabrec*)m_pICdwg->tblnext(DB_LAYERTABREC,1); layerRecord!=NULL; layerRecord=(db_layertabrec*)m_pICdwg->tblnext(DB_LAYERTABREC,0)) 
	{
		int flags;
		layerRecord->get_70(&flags);
		if ((flags & BIT10) && (flags & BIT20))
			continue;	// Belongs to an xref and is created in TransferRemainingBlocksToDWGdirect()

		layerRecord->get_2(layerName, DB_ACADBUF - 1);

		bool bAddLayer = true;
		OdDbLayerTableRecordPtr pLayer;

		if (stricmp(layerName, OdDbSymUtil::layerZeroName()) == 0)
		{
			pLayer = pDDdwg->getLayerZeroId().safeOpenObject(OdDb::kForWrite);
			ASSERT(!pLayer.isNull());

			bAddLayer = false;
		}
		else
		{
			pLayer = OdDbLayerTableRecord::createObject();
			ASSERT(!pLayer.isNull());
		}

		setLayerProps(pDDdwg, pLayer, layerRecord);

		if (bAddLayer == true)
		{
			pLayer->setName(OdString(layerName));

			ReadWriteUtils::setHandle(pDDdwg, layerRecord, layerTableId, pLayer);

			pTable->add(pLayer);
		}
	}

	return;
}

void DWGdirectWriter::TransferDimStylesToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	char styleName[DB_ACADBUF];
	db_dimstyletabrec* pDimStyleRec = NULL;

	OdDbObjectId dimStyleTableId = pDDdwg->getDimStyleTableId();
	OdDbDimStyleTablePtr pTable = dimStyleTableId.safeOpenObject(OdDb::kForWrite);

	for (pDimStyleRec=(db_dimstyletabrec*)m_pICdwg->tblnext(DB_DIMSTYLETAB,1); pDimStyleRec!=NULL; pDimStyleRec=(db_dimstyletabrec*)m_pICdwg->tblnext(DB_DIMSTYLETAB,0)) 
	{
		OdDbDimStyleTableRecordPtr pStyle;

		pDimStyleRec->get_2(styleName, DB_ACADBUF - 1);

		// If this is the standard dimension style, we don't need to add it to the 
		// database, simply alter it to match the one we're exporting.
		if (stricmp(styleName, OdDbSymUtil::textStyleStandardName()) == 0)
		{
			pStyle = pDDdwg->getDimStyleStandardId().safeOpenObject(OdDb::kForWrite);
			ASSERT(!pStyle.isNull());

			OdDbObjectId odDimStyleAfterId = m_Converter->updateOdDbObject(dimStyleTableId, (db_handitem*)pDimStyleRec, pStyle);
			continue;
		}
		else
		{
			OdDbObjectId dimStyleId = m_Converter->createOdDbObject(dimStyleTableId, (db_handitem*)pDimStyleRec, OdString());

			pStyle = dimStyleId.safeOpenObject(OdDb::kForWrite);
			ASSERT(!pStyle.isNull());

			pStyle->setName(OdString(styleName));
			ReadWriteUtils::setHandle(pDDdwg, pDimStyleRec, dimStyleTableId, pStyle);
			pTable->add(pStyle);
		}
	}
	
	return;
}

void DWGdirectWriter::TransferViewportsToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	db_vporttabrec* pViewportRecord = NULL;

	OdDbObjectId viewportTableId = pDDdwg->getViewportTableId();
	OdDbViewportTablePtr pTable = viewportTableId.safeOpenObject(OdDb::kForWrite);
	OdDbObjectId  activeVPId = pTable->getActiveViewportId();
	
	int vpCount = 0;
	bool bAddViewport = true;
	OdDbViewportTableRecordPtr pViewportRec;
	for (pViewportRecord=(db_vporttabrec*)m_pICdwg->tblnext(DB_VPORTTAB,1); pViewportRecord!=NULL; pViewportRecord=(db_vporttabrec*)m_pICdwg->tblnext(DB_VPORTTAB,0)) 
	{
		if (!vpCount)
		{
			// Active viewport handling.
			pViewportRec = activeVPId.safeOpenObject(OdDb::kForWrite);
			bAddViewport = false;
			vpCount++;
		}
		else
		{
			pViewportRec = OdDbViewportTableRecord::createObject();
			bAddViewport = true;
		}

		char* name;
		pViewportRecord->get_2(&name);
		pViewportRec->setName(OdString(name));

		int flags70;
		pViewportRecord->get_70(&flags70);
		// TODO MHB?
		// Break up flags into separate odt calls.

		sds_point point;
		pViewportRecord->get_10(point);
		pViewportRec->setLowerLeftCorner(OdGePoint2d(point[0], point[1]));

		pViewportRecord->get_11(point);
		pViewportRec->setUpperRightCorner(OdGePoint2d(point[0], point[1]));

		pViewportRecord->get_12(point);
		pViewportRec->setCenterPoint(OdGePoint2d(point[0], point[1]));

		pViewportRecord->get_13(point);
		pViewportRec->setSnapBase(OdGePoint2d(point[0], point[1]));

		pViewportRecord->get_14(point);
		pViewportRec->setSnapIncrements(OdGePoint2d(point[0], point[1]));

		pViewportRecord->get_15(point);
		pViewportRec->setGridIncrements(OdGePoint2d(point[0], point[1]));

		pViewportRecord->get_16(point);
		pViewportRec->setViewDirection(OdGeVector3d(point[0], point[1], point[2]));

		pViewportRecord->get_17(point);
		pViewportRec->setTarget(OdGePoint3d(point[0], point[1], point[2]));

		double doubleVal;
		pViewportRecord->get_40(&doubleVal);
		pViewportRec->setHeight(doubleVal);
		
		double widthRatio;
		pViewportRecord->get_41(&widthRatio);
		pViewportRec->setWidth(widthRatio * doubleVal);
		
		pViewportRecord->get_42(&doubleVal);
		pViewportRec->setLensLength(doubleVal);
		
		pViewportRecord->get_43(&doubleVal);
		pViewportRec->setFrontClipDistance(doubleVal);
	
		pViewportRecord->get_44(&doubleVal);
		pViewportRec->setBackClipDistance(doubleVal);

		pViewportRecord->get_50(&doubleVal);
		pViewportRec->setSnapAngle(doubleVal);

		pViewportRecord->get_51(&doubleVal);
		pViewportRec->setViewTwist(doubleVal);

		int flag90;
		pViewportRecord->get_90(&flag90);
		pViewportRec->setPerspectiveEnabled	((flag90 & BIT1) == BIT1);
		pViewportRec->setFrontClipEnabled	((flag90 & BIT2) == BIT2);
		pViewportRec->setBackClipEnabled	((flag90 & BIT4) == BIT4);
		pViewportRec->setUcsFollowMode		((flag90 & BIT8) == BIT8);
		pViewportRec->setFrontClipAtEye		((flag90 & BIT10) == BIT10);

		int intVal;
		pViewportRecord->get_72(&intVal);
		pViewportRec->setCircleSides(intVal);

		pViewportRecord->get_73(&intVal);
		pViewportRec->setFastZoomsEnabled((intVal == 1) ? true : false);

		pViewportRecord->get_74(&intVal);
		pViewportRec->setIconEnabled(((intVal & BIT1) == BIT1) ? true : false);
		pViewportRec->setIconAtOrigin(((intVal & BIT2) == BIT2) ? true : false);

		pViewportRecord->get_75(&intVal);
		pViewportRec->setSnapEnabled((intVal == 1) ? true : false);

		pViewportRecord->get_76(&intVal);
		pViewportRec->setGridEnabled((intVal == 1) ? true : false);

		pViewportRecord->get_77(&intVal);
		// TODO MHB ? pViewportRec->setSnapStyle((intVal == 1) ? true : false);

		pViewportRecord->get_78(&intVal);
		pViewportRec->setSnapPair(intVal);

		pViewportRecord->get_281(&intVal);
		pViewportRec->setRenderMode((OdDb::RenderMode)intVal);

		// TODO MHB ? pViewportRecord->get_65(&intVal);

		pViewportRecord->get_146(&doubleVal);
		pViewportRec->setElevation(doubleVal);

		db_handitem* ucs;
		pViewportRecord->get_345(&ucs);
		pViewportRec->setUcs(getUCSIdFromHandItem(pDDdwg, ucs));

		if (bAddViewport)
		{
			ReadWriteUtils::setHandle(pDDdwg, pViewportRecord, viewportTableId, pViewportRec);

			pTable->add(pViewportRec);
		}
	}
	
	return;
}

OdDbObjectId DWGdirectWriter::getUCSIdFromHandItem(OdDbDatabasePtr& pDDdwg, db_handitem* pUcs)
{
	OdDbObjectId ucsID;
	if (pUcs == NULL)
		return ucsID;

	ASSERT(pUcs->ret_type() == DB_UCSTABREC);
	if (pUcs->ret_type() != DB_UCSTABREC)
		return ucsID;

	char ucsName[DB_ACADBUF], searchName[DB_ACADBUF];
	pUcs->get_2(searchName, DB_ACADBUF - 1);

	OdDbUCSTablePtr pTable = pDDdwg->getUCSTableId().safeOpenObject(OdDb::kForWrite);
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();

	for (pTableIter->start(); !pTableIter->done(); pTableIter->step()) 
	{
		OdDbUCSTableRecordPtr pUCSRec = pTableIter->getRecordId().safeOpenObject();
		if (pUCSRec->getName() == OdString(ucsName))
			return pTableIter->getRecordId();
	}
	return ucsID;
}

void DWGdirectWriter::TransferUCSsToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	char ucsName[DB_ACADBUF];
	db_ucstabrec* pUCSRecord = NULL;

	OdDbObjectId ucsTableId = pDDdwg->getUCSTableId();
	OdDbUCSTablePtr pTable = ucsTableId.safeOpenObject(OdDb::kForWrite);

	for (pUCSRecord=(db_ucstabrec*)m_pICdwg->tblnext(DB_UCSTAB,1); pUCSRecord!=NULL; pUCSRecord=(db_ucstabrec*)m_pICdwg->tblnext(DB_UCSTAB,0)) 
	{
		OdDbUCSTableRecordPtr pUCS = OdDbUCSTableRecord::createObject();

		pUCSRecord->get_2(ucsName, DB_ACADBUF - 1);
		pUCS->setName(OdString(ucsName));

		sds_point point;
		pUCSRecord->get_10(point);
		pUCS->setOrigin(OdGePoint3d(point[0], point[1], point[2]));

		pUCSRecord->get_11(point);
		pUCS->setXAxis(OdGeVector3d(point[0], point[1], point[2]));

		pUCSRecord->get_12(point);
		pUCS->setYAxis(OdGeVector3d(point[0], point[1], point[2]));

		short oViewType;
		pUCSRecord->get_71(&oViewType);
		pUCSRecord->get_13(point);
		pUCS->setUcsBaseOrigin(OdGePoint3d(point[0], point[1], point[2]), (OdDb::OrthographicView)oViewType);

		ReadWriteUtils::setHandle(pDDdwg, pUCSRecord, ucsTableId, pUCS);

		pTable->add(pUCS);
	}

	return;
}

void DWGdirectWriter::TransferViewsToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	int flags;
	int flags1;
	char viewName[DB_ACADBUF];
	db_viewtabrec* pViewRecord = NULL;

	OdDbObjectId viewTableId = pDDdwg->getViewTableId();
	OdDbViewTablePtr pTable = viewTableId.safeOpenObject(OdDb::kForWrite);

	for (pViewRecord=(db_viewtabrec*)m_pICdwg->tblnext(DB_VIEWTAB,1); pViewRecord!=NULL; pViewRecord=(db_viewtabrec*)m_pICdwg->tblnext(DB_VIEWTAB,0)) 
	{
		OdDbViewTableRecordPtr pView = OdDbViewTableRecord::createObject();

		pViewRecord->get_2(viewName, DB_ACADBUF - 1);
		pView->setName(OdString(viewName));

		pViewRecord->get_70(&flags);
		pViewRecord->get_71(&flags1);
		pView->setIsPaperspaceView((flags & BIT1) == BIT1);

		double doubleVal;
		pViewRecord->get_43(&doubleVal);
		pView->setFrontClipDistance(doubleVal);

		pViewRecord->get_44(&doubleVal);
		pView->setBackClipDistance(doubleVal);

		pView->setBackClipEnabled((flags1 & BIT4) == BIT4);
		pView->setFrontClipEnabled((flags1 & BIT2) == BIT2);
		pView->setFrontClipAtEye((flags1 & BIT10) == BIT10);
		pView->setPerspectiveEnabled((flags1 & BIT1) == BIT1);

		sds_point point;
		pViewRecord->get_10(point);
		pView->setCenterPoint(OdGePoint2d(point[0], point[1]));

		pViewRecord->get_146(&doubleVal);
		pView->setElevation(doubleVal);

		pViewRecord->get_40(&doubleVal);
		pView->setHeight(doubleVal);

		pViewRecord->get_41(&doubleVal);
		pView->setWidth(doubleVal);

		pViewRecord->get_42(&doubleVal);
		pView->setLensLength(doubleVal);

		pViewRecord->get_50(&doubleVal);
		pView->setViewTwist(doubleVal);

		int integerVal;
		pViewRecord->get_281(&integerVal);
		pView->setRenderMode((OdDb::RenderMode)integerVal);

		pViewRecord->get_12(point);
		pView->setTarget(OdGePoint3d(point[0], point[1], point[2]));

		pViewRecord->get_11(point);
		pView->setViewDirection(OdGeVector3d(point[0], point[1], point[2]));

		// TODO - MHB
		db_handitem* ucs = NULL;
		pViewRecord->get_345(&ucs);
		if (ucs != NULL)
		{
			char ucsName[DB_ACADBUF];
			ucs->get_2(ucsName, DB_ACADBUF - 1);
				
			// TODO - MHB this seems to be failing. I have an email in to 
			// the ODT guys, and will follow up.
			pView->setUcs(getUcsIDByName(pDDdwg, OdString(ucsName)));
		}

		ReadWriteUtils::setHandle(pDDdwg, pViewRecord, viewTableId, pView);

		pTable->add(pView);
	}
	return;
}

void DWGdirectWriter::TransferObjectsToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	db_handitem* head = NULL;
	db_handitem* tail = NULL;

	m_pICdwg->get_objllends(&head, &tail);
	ASSERT(head != NULL);

	if (head == NULL)
		return;	// serious problem

	// First link in head, is the Named object dictionary. First take all the
	// records in it and transfer them to the ouput drawings named object dictionary.
	db_dictionary* pNamedObjDict = (db_dictionary*)head;
	ASSERT (pNamedObjDict != NULL);

	OdDbObjectId nodId = pDDdwg->getNamedObjectsDictionaryId();
	OdDbDictionaryPtr pNamedObjectDictionary = nodId.safeOpenObject(OdDb::kForWrite);

	if (pNamedObjDict != NULL)
	{
		db_dictionaryrec *beginDictRecord= NULL;
		db_dictionaryrec *endDictRecord = NULL;
		((db_dictionary*)head)->get_recllends(&beginDictRecord, &endDictRecord);

		db_dictionaryrec *walkingDict = beginDictRecord;
		while (walkingDict != NULL)
		{
			db_hireflink* recordReference = walkingDict->hiref;
			while (recordReference != NULL)
			{
#ifdef _DEBUG_MHB
		OdString tmp;
		tmp.format("Writinging - %s\n", walkingDict->name);
		OutputDebugString(tmp.c_str());
#endif
				processDictionaryRecord(pDDdwg, OdString(walkingDict->name), recordReference, pNamedObjectDictionary);

				recordReference = recordReference->next;
			}

			walkingDict = walkingDict->next;
		}
	}

	return;
}

void DWGdirectWriter::TransferBlocksToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	// For several reasons including handling its deletion, I need to get rid of
	// *Paper_Space0 in this new database, because it is there by default in DWGdirect.
	// If later it comes in from the Icad database, we'll add it back in. To do that
	// we need to get rid of its layout viewport, then its layout, then it.
	OdDbBlockTablePtr pBlockTable = pDDdwg->getBlockTableId().safeOpenObject(OdDb::kForWrite);
	OdDbBlockTableRecordPtr pOdDbBlock= pBlockTable->getAt(OdString("*Paper_Space0"/*DNT*/), OdDb::kForWrite);
	if (!pOdDbBlock.isNull())
	{
		OdDbObjectId layoutDictId = pDDdwg->getLayoutDictionaryId();
		OdDbDictionaryPtr pOdLayoutDictionary = layoutDictId.safeOpenObject(OdDb::kForWrite);

		OdDbLayoutPtr pOdLayout = pOdLayoutDictionary->getAt(LAYOUT2, OdDb::kForWrite);
		ASSERT(!pOdLayout.isNull());
		if (!pOdLayout.isNull())
		{
			OdDbObjectId pLayoutViewportId = pOdLayout->overallVportId();
			if (!pLayoutViewportId.isNull())
			{
				OdDbViewportPtr pViewport = pLayoutViewportId.safeOpenObject(OdDb::kForWrite);
				if (!pViewport.isNull())
				{
					pViewport->erase();
				}
			}
			pOdLayout->erase();
		}
		pOdDbBlock->erase();
	}

	db_handitem* pBlockRec = NULL;
    for (pBlockRec = m_pICdwg->tblnext(DB_BLOCKTABREC,1); pBlockRec!=NULL; pBlockRec = m_pICdwg->tblnext(DB_BLOCKTABREC,0)) 
	{
		if (pBlockRec->ret_deleted())
			continue;

		int flags;
		pBlockRec->get_70(&flags);
		if ((flags & BIT4) == BIT4)		// Use XRef manager if we need to.
		{
			OdDbBlockTablePtr pBlockTable = pDDdwg->getBlockTableId().safeOpenObject(OdDb::kForWrite);

			OdDbObjectId xrefId = TransferXRefToDWGdirect(pDDdwg, (db_blocktabrec*)pBlockRec);
			continue;
		}

		char blkNm[128];
		pBlockRec->get_2(blkNm, 128);
		OdString blockName = blkNm;
		OdString left6 = blockName.left(6);
		left6.makeUpper();
		
		if (left6.iCompare(MODEL) == 0 || left6.iCompare(PAPER) == 0)
		{
			ProcessBlockWithLayout(pDDdwg, (db_blocktabrec*)pBlockRec, blockName);
		}
		else
		{
			ProcessBlockWithoutLayout(pDDdwg, (db_blocktabrec*)pBlockRec, blockName);
		}
	}
}

void DWGdirectWriter::ProcessBlockWithLayout(OdDbDatabasePtr& pDDdwg, db_blocktabrec* pBlock, OdString blockName)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());
	bool deleteLayout1 = false;
	bool deleteLayout2 = false;

	OdDbBlockTablePtr pBlockTable = pDDdwg->getBlockTableId().safeOpenObject(OdDb::kForWrite);

	OdDbObjectId layoutDictId = pDDdwg->getLayoutDictionaryId();
	OdDbDictionaryPtr pOdLayoutDictionary = layoutDictId.safeOpenObject(OdDb::kForWrite);

	OdDbObjectId layoutId;
	OdDbLayoutPtr pOdLayout;
	OdDbBlockTableRecordPtr pOdDbBlock;

	if (blockName.iCompare(MSPACE) == 0)
	{
		// *Model_Space block ---------------------------------------------------
		pOdDbBlock= pBlockTable->getAt(OdDbSymUtil::blockModelSpaceName(), OdDb::kForWrite);
		ASSERT(!pOdDbBlock.isNull());
		if (!pOdDbBlock.isNull())
		{
			db_objhandle layoutHandle = m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT);
			db_handitem* layoutItem = m_pICdwg->handent(layoutHandle);
			if (layoutItem != NULL && layoutItem->ret_type() == DB_LAYOUT)
			{
				pOdLayout = pOdLayoutDictionary->getAt("Model"/*DNT*/, OdDb::kForWrite);
				ASSERT(!pOdLayout.isNull());
				if (!pOdLayout.isNull())
				{
					layoutId = pOdLayout->objectId();
					ASSERT(pOdLayout->isKindOf(OdDbLayout::desc()));
					OdDbObjectId afterId = m_Converter->updateOdDbObject(layoutDictId, layoutItem, pOdLayout);
					ASSERT(afterId == layoutId);

					pOdDbBlock->setLayoutId(layoutId);
					pOdLayout->setBlockTableRecordId(pOdDbBlock->objectId());
				}
			}

		}
		// Entities for this block reside in the ENTITIES section of the drawing
		// and are added to the output drawing in TransferEntitiesToDWGdirect()
	} 
	else if (blockName.iCompare(PSPACE) == 0)
	{
		// *Paper_Space		Always has current layout. -------------------------
		db_handitem* pCurrentLayout;
		bool ret = getCurrentPSLayout(&pCurrentLayout);
		ASSERT(ret == true);

		// Get the name of the layout that ICAD thinks was the last paperspace layout.
		char* layoutName = (char*)((CDbLayout*)pCurrentLayout)->name(m_pICdwg);

		if (pCurrentLayout->RetHandle() == db_objhandle(m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT), 1))
		{
			OdString defaultName(LAYOUT1);
			if(defaultName.iCompare(layoutName))
				pDDdwg->renameLayout(defaultName, layoutName);
		}

		if (pCurrentLayout->RetHandle() == db_objhandle(m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT), 2))
		{
			OdString defaultName(LAYOUT2);
			if(defaultName.iCompare(layoutName))
				pDDdwg->renameLayout(defaultName, layoutName);
		}

		// Now look that layout up by name in the DWGdirect database, and assign it to 
		// this block, and this block to it.
		pOdLayout = pOdLayoutDictionary->getAt(layoutName, OdDb::kForWrite);
		if (!pOdLayout.isNull())
		{
			layoutId = pOdLayout->objectId();
			ASSERT(pOdLayout->isKindOf(OdDbLayout::desc()));
			OdDbObjectId afterId = m_Converter->updateOdDbObject(layoutDictId, pCurrentLayout, pOdLayout);
			ASSERT(afterId == layoutId);
		}
		else
		{
			layoutId= m_Converter->createOdDbObject(layoutDictId, pCurrentLayout, OdString());
			ASSERT(!layoutId.isNull());
			if (!layoutId.isNull())
			{
				pOdLayout = layoutId.safeOpenObject(OdDb::kForWrite);

				OdDbObjectId odLayoutAfterId = pOdLayoutDictionary->setAt(layoutName, pOdLayout);
				ASSERT(layoutId == odLayoutAfterId);
			}
		}
		ASSERT(!pOdLayout.isNull());

		// Get pointer to the *Paper_Space block
		pOdDbBlock = pBlockTable->getAt(OdDbSymUtil::blockPaperSpaceName(), OdDb::kForWrite);
		ASSERT(!pOdDbBlock.isNull());

		// Hook up the layout to this block, and this block to this layout.
		if (!pOdLayout.isNull() && !pOdDbBlock.isNull())
		{
			ASSERT(pOdLayout->isKindOf(OdDbLayout::desc()));

			pOdDbBlock->setLayoutId(layoutId);
			pOdLayout->setBlockTableRecordId(pOdDbBlock->id()); 

			//pDDdwg->setCurrentLayout(layoutId); // Causes creation of a viewport if none exist.
		}
		// Entities for this block reside in the ENTITIES section of the drawing
		// and are added to the output drawing in TransferEntitiesToDWGdirect()
	}
	else
	{
		// Now the rest of them, i.e. *Paper_Space0, *Paper_Space1 etc.

		// *Paper_Space?	-----------------------------------------------------------
		
		// Ok, we know the name will be *Paper_Space? where ? is an index number
		// supplied by DWGdirect.
		if (pBlock->ret_deleted())
		{
			db_objhandle layoutHandle;
			pBlock->get_layoutobjhandle(&layoutHandle);
			if (layoutHandle == NULL)
				return;

			if (layoutHandle == db_objhandle(m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT), 1))
			{
				deleteLayout1 = true;
			}
			if (layoutHandle == db_objhandle(m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT), 2))
			{
				deleteLayout2 = true;
			}

			// Go ahead and process it as if it were still valid.
			// This prevents the index numbers at the end of the paper space block
			// from getting out of sync. Delete it when all *Paper_Space? blocks
			// have been processed.
			// continue;
		}

		pOdDbBlock = pBlockTable->getAt(blockName, OdDb::kForWrite);
		if (pOdDbBlock.isNull())
		{
			pOdDbBlock = OdDbBlockTableRecord::createObject();
			pOdDbBlock->setName(PSPACE);	// Let DWGdirect add the index.

			OdDbObjectId idBefore = ReadWriteUtils::setHandle(pDDdwg, pBlock, pBlockTable->objectId(), pOdDbBlock);

			OdDbObjectId idAfter  = pBlockTable->add(pOdDbBlock);
			ASSERT(idBefore == idAfter);

			OdString tmp = ((OdDbBlockTableRecordPtr)idAfter.safeOpenObject(OdDb::kForRead))->getName();

		}
		ASSERT(!pOdDbBlock.isNull());

		if (!pOdDbBlock.isNull())
		{
			// Now the layout.
			db_objhandle layoutHandle;
			pBlock->get_layoutobjhandle(&layoutHandle);
			if (layoutHandle == NULL)
				return;

			db_handitem* pCurrentLayout = m_pICdwg->handent(layoutHandle);
			char* layoutName = (char*)((CDbLayout*)pCurrentLayout)->name(m_pICdwg);

			if (pCurrentLayout->RetHandle() == db_objhandle(m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT), 1))
			{
				OdString defaultName(LAYOUT1);
				if(defaultName.iCompare(layoutName))
					pDDdwg->renameLayout(defaultName, layoutName);
			}

			if (pCurrentLayout->RetHandle() == db_objhandle(m_pICdwg->ret_stockhand(DB_SHI_MSLAYOUT), 2))
			{
				OdString defaultName(LAYOUT2);
				if(defaultName.iCompare(layoutName))
					pDDdwg->renameLayout(defaultName, layoutName);
			}

			pOdLayout = pOdLayoutDictionary->getAt(layoutName, OdDb::kForWrite);
			if (!pOdLayout.isNull())
			{
				layoutId = pOdLayout->objectId();
				ASSERT(pOdLayout->isKindOf(OdDbLayout::desc()));
				OdDbObjectId afterId = m_Converter->updateOdDbObject(layoutDictId, pCurrentLayout, pOdLayout);
				ASSERT(afterId == layoutId);
			}
			else
			{
				layoutId = m_Converter->createOdDbObject(layoutDictId, pCurrentLayout, OdString());
				pOdLayout = layoutId.safeOpenObject(OdDb::kForWrite);

				OdDbObjectId odLayoutAfterId = pOdLayoutDictionary->setAt(layoutName, pOdLayout);
				ASSERT(layoutId == odLayoutAfterId);
			}
			ASSERT(!pOdLayout.isNull());

			// Hook up the layout to this block, and this block to this layout.
			if (!pOdLayout.isNull() && !pOdDbBlock.isNull())
			{
				ASSERT(pOdLayout->isKindOf(OdDbLayout::desc()));

				pOdDbBlock->setLayoutId(layoutId);
				pOdLayout->setBlockTableRecordId(pOdDbBlock->id()); 
			}

			ReadWriteUtils::addEntitiesToBlocks(pDDdwg, m_pICdwg, m_Converter, pBlock, pOdDbBlock);
		}
	}

	return;
}

void DWGdirectWriter::ProcessBlockWithoutLayout(OdDbDatabasePtr& pDDdwg, db_blocktabrec* pBlock, OdString blockName)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull() && pBlock != NULL);

	OdDbObjectId blockTableId = pDDdwg->getBlockTableId();
	OdDbBlockTablePtr blockTablePtr = blockTableId.safeOpenObject(OdDb::kForWrite);

	char hand[17];
	bool handleConflict = false;
	pBlock->RetHandlePtr()->ToAscii(hand);
	OdDbObjectId existingId = pDDdwg->getOdDbObjectId(hand); 

	if (!existingId.isNull())
	{
		handleConflict = true;
	}

	OdString newBlockName;
	if (blockName[0] == '*' && isdigit(blockName[2]))
	{
		blockName = blockName.left(2);
		newBlockName = blockName;
	}
	else
	{
		newBlockName = blockName;
	}

	OdDbBlockTableRecordPtr pOdDbBlock = OdDbBlockTableRecord::createObject();
	pOdDbBlock->setName(newBlockName);

	// I have seen this call ASSERT() when doing *Paper_Space1 block. This causes the handle to
	// change, but seems ok. // TODO MHB see if there is a way to avoid this.
	OdDbObjectId idBefore = ReadWriteUtils::setHandle(pDDdwg, pBlock, blockTableId, pOdDbBlock);

	OdDbObjectId idAfter  = blockTablePtr->add(pOdDbBlock);
	ASSERT(idBefore == idAfter);

	if (handleConflict == true)
	{
		OdString newHandle = idAfter.getHandle().ascii();
		OdString newName = ((OdDbBlockTableRecordPtr)idAfter.safeOpenObject())->getName();

		m_Converter->addBlockHandleMap(OdString(hand), handleName(newHandle, newName));
	}
	ReadWriteUtils::addEntitiesToBlocks(pDDdwg, m_pICdwg, m_Converter, pBlock, pOdDbBlock);

	ASSERT(!pOdDbBlock.isNull());

	return;
}

bool DWGdirectWriter::getCurrentPSLayout(db_handitem** ppLayout)
{
	db_dictionary* pDictLayout = (db_dictionary*)m_pICdwg->dictsearch(m_pICdwg->namedobjdict(), "ACAD_LAYOUT"/*DNT*/, 0);
	db_dictionaryrec* pCurRec;
	pDictLayout->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		if(!static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pICdwg))->ret_deleted())
		{
			// paper space layout is active
			if(static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pICdwg))->isCurrent())
				break;
		}

		pCurRec = pCurRec->next;
	}
	ASSERT(pCurRec != NULL);
	*ppLayout = pCurRec->hiref->ret_hip(m_pICdwg);

	return (*ppLayout != NULL);
}

void DWGdirectWriter::TransferSysVarsToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	sds_resbuf resBuf;
	resBuf.rbnext = NULL;

	resBuf.restype = RTREAL;//-------------------------------------

	OdDbDate date;
	double fracpart, intpart;
	db_getvar(NULL, DB_QTDCREATE, &resBuf, m_pICdwg, NULL, NULL);
	fracpart = modf(resBuf.resval.rreal,&intpart);
	date.setJulianDate((OdUInt32)intpart, (OdUInt32)(intpart * TIMEFACTOR));
	odDbSetTDCREATE(*(pDDdwg), date);

	db_getvar(NULL, DB_QTDINDWG, &resBuf, m_pICdwg, NULL, NULL);
	fracpart = modf(resBuf.resval.rreal,&intpart);
	date.setJulianDate((OdUInt32)intpart, (OdUInt32)(intpart * TIMEFACTOR));
	odDbSetTDINDWG(*(pDDdwg), date);

	db_getvar(NULL, DB_QTDUPDATE, &resBuf, m_pICdwg, NULL, NULL);
	fracpart = modf(resBuf.resval.rreal,&intpart);
	date.setJulianDate((OdUInt32)intpart, (OdUInt32)(intpart * TIMEFACTOR));
	odDbSetTDUPDATE(*(pDDdwg), date);

	db_getvar(NULL, DB_QTDUUPDATE, &resBuf, m_pICdwg, NULL, NULL);
	fracpart = modf(resBuf.resval.rreal,&intpart);
	date.setJulianDate((OdUInt32)intpart, (OdUInt32)(intpart * TIMEFACTOR));
	odDbSetTDUUPDATE(*(pDDdwg), date);

	db_getvar(NULL, DB_QTDUSRTIMER, &resBuf, m_pICdwg, NULL, NULL);
	fracpart = modf(resBuf.resval.rreal,&intpart);
	date.setJulianDate((OdUInt32)intpart, (OdUInt32)(intpart * TIMEFACTOR));
	odDbSetTDUSRTIMER(*(pDDdwg), date);

	resBuf.restype = RTSTR;//-------------------------------------

	db_getvar(NULL, DB_QDIMAPOST, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimapost(resBuf.resval.rstring);
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QDIMBLK, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimblk(ReadWriteUtils::getBlockIDByName(pDDdwg, m_pICdwg, m_Converter, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);
	
	db_getvar(NULL, DB_QDIMBLK1, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimblk1(ReadWriteUtils::getBlockIDByName(pDDdwg, m_pICdwg, m_Converter, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QDIMBLK2, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimblk2(ReadWriteUtils::getBlockIDByName(pDDdwg, m_pICdwg, m_Converter, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QDIMLDRBLK, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimldrblk(ReadWriteUtils::getBlockIDByName(pDDdwg, m_pICdwg, m_Converter, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QDIMPOST, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimpost(resBuf.resval.rstring);
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QUCSNAME, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUCSNAME(getUcsIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	OdDbDatabaseSummaryInfoPtr pSI = oddbGetSummaryInfo(pDDdwg);
	ASSERT(!pSI.isNull());

	db_getvar(NULL, DB_QHYPERLINKBASE, &resBuf, m_pICdwg, NULL, NULL);
	pSI->setHyperlinkBase(OdString(resBuf.resval.rstring));
	IC_FREE(resBuf.resval.rstring);

	OdCodePageId cpID;
	db_getvar(NULL, DB_QDWGCODEPAGE, &resBuf, m_pICdwg, NULL, NULL);
	OdCharMapper::codepageDescToId(OdString(resBuf.resval.rstring), cpID);
	odDbSetDWGCODEPAGE(*(pDDdwg), cpID);
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QCELTYPE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCELTYPE(ReadWriteUtils::getLinetypeIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QCLAYER, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCLAYER(ReadWriteUtils::getLayerIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QCMLSTYLE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCMLSTYLE(ReadWriteUtils::getMLineStyleIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QDIMSTYLE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimstyleData(ReadWriteUtils::getDimStyleIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QDIMTXSTY, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtxsty(ReadWriteUtils::getTextStyleIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QP_UCSNAME, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUCSNAME(getUcsIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	db_getvar(NULL, DB_QTEXTSTYLE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTEXTSTYLE(ReadWriteUtils::getTextStyleIDByName(pDDdwg, OdString(resBuf.resval.rstring)));
	IC_FREE(resBuf.resval.rstring);

	OdCmColor color;
	db_getvar(NULL, DB_QCECOLOR, &resBuf, m_pICdwg, NULL, NULL);
	ASSERT(resBuf.resval.rstring[0] != '\0');

	int icColor = OdCmEntityColor::kForeground;
	if (strisame(resBuf.resval.rstring, OdDbSymUtil::linetypeByLayerName()))
		icColor = OdCmEntityColor::kByLayer;
	else if (strisame(resBuf.resval.rstring, OdDbSymUtil::linetypeByBlockName()))
		icColor = OdCmEntityColor::kByBlock;
	else
		icColor = atoi(resBuf.resval.rstring);
	IC_FREE(resBuf.resval.rstring);

	color.setColorMethod(OdCmEntityColor::kByACI);
	color.setColorIndex(icColor);
	pDDdwg->setCECOLOR(color);

	db_getvar(NULL, DB_QDIMDSEP, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimdsep((short)(resBuf.resval.rstring[0]));
	IC_FREE(resBuf.resval.rstring);

	resBuf.restype = RTSHORT;//-------------------------------------

	db_getvar(NULL, DB_QDIMALT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimalt((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMALTD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimaltd(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMALTTZ, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimalttz(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMATFIT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimatfit(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMFIT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimfit(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMTMOVE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtmove(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMJUST, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimjust(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMLIM, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimlim((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTIH, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtih((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMSAH, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimsah((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMSD1, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimsd1((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMSD2, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimsd2((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMSHO, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDIMSHO((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMSOXD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimsoxd((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTAD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtad((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTIX, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtix((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTIX, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtix((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTOFL, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtofl((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTOH, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtoh((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTOL, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtol((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTOLJ, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtolj((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMTZIN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtzin(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMUPT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimupt((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QDIMZIN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimzin(resBuf.resval.rint);

	db_getvar(NULL, DB_QDISPSILH, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDISPSILH((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPELLIPSE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPELLIPSE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPICKSTYLE, &resBuf, m_pICdwg, NULL, NULL);
	m_pExServices->setPICKSTYLE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPROXYGRAPHICS, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPROXYGRAPHICS((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QMIRRTEXT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setMIRRTEXT((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QORTHOMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setORTHOMODE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPDMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPDMODE(resBuf.resval.rint);

	db_getvar(NULL, DB_QP_LIMCHECK, &resBuf, m_pICdwg, NULL, NULL);
//2002?	pDDdwg->setPLIMCHECK((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPLINEGEN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPLINEGEN((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPSLTSCALE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPSLTSCALE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QQTEXTMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setQTEXTMODE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QREGENMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setREGENMODE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QSHADEDGE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSHADEDGE(resBuf.resval.rint);

	db_getvar(NULL, DB_QSHADEDIF, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSHADEDIF(resBuf.resval.rint);

	db_getvar(NULL, DB_QSKPOLY, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSKPOLY((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QSPLFRAME, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSPLFRAME((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QSPLINESEGS, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSPLINESEGS(resBuf.resval.rint);

	db_getvar(NULL, DB_QSURFTAB1, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSURFTAB1(resBuf.resval.rint);

	db_getvar(NULL, DB_QSURFTAB2, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSURFTAB2(resBuf.resval.rint);

	db_getvar(NULL, DB_QSURFTYPE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSURFTYPE(resBuf.resval.rint);

	db_getvar(NULL, DB_QSURFU, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSURFU(resBuf.resval.rint);

	db_getvar(NULL, DB_QSURFV, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSURFV(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMUNIT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimunit(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMTDEC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtdec(resBuf.resval.rint);

	db_getvar(NULL, DB_QANGDIR, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setANGDIR((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QATTDIA, &resBuf, m_pICdwg, NULL, NULL);
	m_pExServices->setATTDIA((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QATTMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setATTMODE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QAUNITS, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setAUNITS((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QAUPREC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setAUPREC(resBuf.resval.rint);

	OdResBufPtr odResBuf;

	db_getvar(NULL, DB_QAXISMODE, &resBuf, m_pICdwg, NULL, NULL);
//	odResBuf = OdResBuf::newRb(OdResBuf::kDxfXXXInt16, (OdInt16)resBuf.resval.rint);
//	pDDdwg->setAXISMODE(resBuf.resval.rint);

	db_getvar(NULL, DB_QCMLJUST, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCMLJUST(resBuf.resval.rint);

	db_getvar(NULL, DB_QCYCLECURR, &resBuf, m_pICdwg, NULL, NULL);
//	m_pExServices->setCYCLECURR(resBuf.resval.rint);
//	odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//	pDDdwg->setSysVar("CYCLECURR", odResBuf);

	db_getvar(NULL, DB_QCYCLEKEEP, &resBuf, m_pICdwg, NULL, NULL);
//	odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//	pDDdwg->setSysVar("CYCLEKEEP", odResBuf);

	db_getvar(NULL, DB_QDIMALTTD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimalttd(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMALTZ, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimaltz(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMALTU, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimaltu(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMAUNIT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimaunit(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMCLRD, &resBuf, m_pICdwg, NULL, NULL);
	color.setColorIndex(resBuf.resval.rint);
	pDDdwg->setDimclrd(color);

	db_getvar(NULL, DB_QDIMCLRE, &resBuf, m_pICdwg, NULL, NULL);
	color.setColorIndex(resBuf.resval.rint);
	pDDdwg->setDimclre(color);

	db_getvar(NULL, DB_QDIMCLRT, &resBuf, m_pICdwg, NULL, NULL);
	color.setColorIndex(resBuf.resval.rint);
	pDDdwg->setDimclrt(color);

	db_getvar(NULL, DB_QDIMDEC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimdec(resBuf.resval.rint);

	db_getvar(NULL, DB_QFILLMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setFILLMODE((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QISOLINES, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setISOLINES(resBuf.resval.rint);

	db_getvar(NULL, DB_QLIMCHECK, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLIMCHECK((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QLUNITS, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLUNITS(resBuf.resval.rint);

	db_getvar(NULL, DB_QLUPREC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLUPREC(resBuf.resval.rint);

	db_getvar(NULL, DB_QMAXACTVP, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setMAXACTVP(resBuf.resval.rint);

//	bool setviewvars = false;
	db_getvar(NULL, DB_QTILEMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTILEMODE((resBuf.resval.rint == 1) ? true : false);
//	setviewvars = (resBuf.resval.rint == 1) ? false : true;

	db_getvar(NULL, DB_QTEXTQLTY, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTEXTQLTY(resBuf.resval.rint);

	db_getvar(NULL, DB_QTREEDEPTH, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTREEDEPTH(resBuf.resval.rint);

	db_getvar(NULL, DB_QUCSFOLLOW, &resBuf, m_pICdwg, NULL, NULL);
//	odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//	pDDdwg->setSysVar("UCSFOLLOW", odResBuf);

	db_getvar(NULL, DB_QUNITMODE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUNITMODE(resBuf.resval.rint);

	db_getvar(NULL, DB_QUSRTIMER, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSRTIMER((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QVISRETAIN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setVISRETAIN((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QWORLDVIEW, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setWORLDVIEW((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QUSERI1, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERI1(resBuf.resval.rint);

	db_getvar(NULL, DB_QUSERI2, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERI2(resBuf.resval.rint);

	db_getvar(NULL, DB_QUSERI3, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERI3(resBuf.resval.rint);

	db_getvar(NULL, DB_QUSERI4, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERI4(resBuf.resval.rint);

	db_getvar(NULL, DB_QUSERI5, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERI5(resBuf.resval.rint);

	db_getvar(NULL, DB_QMEASUREMENT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setMEASUREMENT((OdDb::MeasurementValue)resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMADEC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimadec(resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMAZIN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimazin(resBuf.resval.rint);

	db_getvar(NULL, DB_QCELWEIGHT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCELWEIGHT((OdDb::LineWeight)resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMLWD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimlwd((OdDb::LineWeight)resBuf.resval.rint);

	db_getvar(NULL, DB_QDIMLWE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimlwe((OdDb::LineWeight)resBuf.resval.rint);

	db_getvar(NULL, DB_QLWDISPLAY, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLWDISPLAY((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QPSTYLEMODE, &resBuf, m_pICdwg, NULL, NULL);
	odDbSetPSTYLEMODE(*(pDDdwg), (resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QEXTNAMES, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setEXTNAMES((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QINSUNITS, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setINSUNITS((OdDb::UnitsValue)resBuf.resval.rint);

	db_getvar(NULL, DB_QXEDIT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setXEDIT((resBuf.resval.rint == 1) ? true : false);

	db_getvar(NULL, DB_QTSTACKSIZE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTSTACKSIZE(resBuf.resval.rint);

	db_getvar(NULL, DB_QTSTACKALIGN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTSTACKALIGN(resBuf.resval.rint);

	// TODO MHB
	// drw_set_cplotstyle( adhd, m_pICdwg );

//	if (setviewvars == true)
//	{
		// UCSICON not defined in SysVarDefs.h.
		//db_getvar(NULL, DB_QUCSICON, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//		pDDdwg->setSysVar("UCSICON", odResBuf);

		// FASTZOOM not defined in SysVarDefs.h.
		//db_getvar(NULL, DB_QFASTZOOM, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//		pDDdwg->setSysVar("FASTZOOM", odResBuf);

		// GRIDMODE commented out in SysVarDefs.h.
	//	db_getvar(NULL, DB_QGRIDMODE, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//		pDDdwg->setSysVar("GRIDMODE", odResBuf);

		// SNAPISOPAIR commented out in SysVarDefs.h.
		//db_getvar(NULL, DB_QSNAPISOPAIR, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//		pDDdwg->setSysVar("SNAPISOPAIR", odResBuf);

		// SNAPMODE commented out in SysVarDefs.h.
		//db_getvar(NULL, DB_QSNAPMODE, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//		pDDdwg->setSysVar("SNAPMODE", odResBuf);

		// SNAPSTYLE commented out in SysVarDefs.h.
		//db_getvar(NULL, DB_QSNAPSTYL, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setInt16((OdInt16)resBuf.resval.rint);
//		pDDdwg->setSysVar("SNAPSTYLE", odResBuf);

//	}

    resBuf.restype=RTREAL;//-------------------------------------

	db_getvar(NULL, DB_QANGBASE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setANGBASE(resBuf.resval.rreal);

	db_getvar(NULL, DB_QCELTSCALE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCELTSCALE(resBuf.resval.rreal);

	db_getvar(NULL, DB_QCHAMFERA, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCHAMFERA(resBuf.resval.rreal);

	db_getvar(NULL, DB_QCHAMFERB, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCHAMFERB(resBuf.resval.rreal);

	db_getvar(NULL, DB_QCHAMFERC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCHAMFERC(resBuf.resval.rreal);

	db_getvar(NULL, DB_QCHAMFERD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCHAMFERD(resBuf.resval.rreal);

	db_getvar(NULL, DB_QCMLSCALE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setCMLSCALE(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMALTF, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimaltf(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMASZ, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimasz(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMCEN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimcen(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMDLE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimdle(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMDLI, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimdli(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMEXE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimexe(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMEXO, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimexo(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMGAP, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimgap(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMLFAC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimlfac(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMRND, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimrnd(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMSCALE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimscale(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMTFAC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtfac(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMTM, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtm(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMTP, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtp(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMTSZ, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtsz(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMTVP, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtvp(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMTXT, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimtxt(resBuf.resval.rreal);

	db_getvar(NULL, DB_QFACETRES, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setFACETRES(resBuf.resval.rreal);

	db_getvar(NULL, DB_QFILLETRAD, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setFILLETRAD(resBuf.resval.rreal);

	db_getvar(NULL, DB_QLTSCALE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLTSCALE(resBuf.resval.rreal);

	db_getvar(NULL, DB_QPDSIZE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPDSIZE(resBuf.resval.rreal);

	db_getvar(NULL, DB_QPLINEWID, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPLINEWID(resBuf.resval.rreal);

	db_getvar(NULL, DB_QSKETCHINC, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setSKETCHINC(resBuf.resval.rreal);

	db_getvar(NULL, DB_QTEXTSIZE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTEXTSIZE(resBuf.resval.rreal);

	db_getvar(NULL, DB_QTHICKNESS, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTHICKNESS(resBuf.resval.rreal);

	db_getvar(NULL, DB_QTRACEWID, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setTRACEWID(resBuf.resval.rreal);

	db_getvar(NULL, DB_QUSERR1, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERR1(resBuf.resval.rreal);

	db_getvar(NULL, DB_QUSERR2, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERR2(resBuf.resval.rreal);

	db_getvar(NULL, DB_QUSERR3, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERR3(resBuf.resval.rreal);

	db_getvar(NULL, DB_QUSERR4, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERR4(resBuf.resval.rreal);

	db_getvar(NULL, DB_QUSERR5, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setUSERR5(resBuf.resval.rreal);

	db_getvar(NULL, DB_QP_ELEVATION, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPELEVATION(resBuf.resval.rreal);

	db_getvar(NULL, DB_QDIMALTRND, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setDimaltrnd(resBuf.resval.rreal);

	db_getvar(NULL, DB_QPSVPSCALE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPSVPSCALE(resBuf.resval.rreal);

//    if(setviewvars == true) 
//	{
//		odResBuf->setRestype(OdResBuf::kDxfReal);

		// VIEWTWIST not defined in SysVarDefs.h.
		//db_getvar(NULL, DB_QVIEWTWIST, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setDouble(resBuf.resval.rreal);
//		pDDdwg->setSysVar("VIEWTWIST", odResBuf);

		// BACKZ not defined in SysVarDefs.h.
		//db_getvar(NULL, DB_QBACKZ, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setDouble(resBuf.resval.rreal);
//		pDDdwg->setSysVar("BACKZ", odResBuf);

		// BACKZ not defined in SysVarDefs.h.
		//db_getvar(NULL, LENSLENGTH, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setDouble(resBuf.resval.rreal);
//		pDDdwg->setSysVar("LENSLENGTH", odResBuf);

		// BACKZ not defined in SysVarDefs.h.
		//db_getvar(NULL, DB_QFRONTZ, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setDouble(resBuf.resval.rreal);
//		pDDdwg->setSysVar("FRONTZ", odResBuf);

		// VIEWSIZE not defined in SysVarDefs.h.
		//db_getvar(NULL, DB_QVIEWSIZE, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setDouble(resBuf.resval.rreal);
//		pDDdwg->setSysVar("VIEWSIZE", odResBuf);

		// VIEWSIZE is commeted out in SysVarDefs.h.
		// db_getvar(NULL, DB_QSNAPANG, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setDouble(resBuf.resval.rreal);
//		pDDdwg->setSysVar("SNAPANG", odResBuf);

//    }

    resBuf.restype=RTPOINT;//-------------------------------------

	db_getvar(NULL, DB_QLIMMIN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLIMMIN(OdGePoint2d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1]));

	db_getvar(NULL, DB_QLIMMAX, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setLIMMAX(OdGePoint2d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1]));

	db_getvar(NULL, DB_QP_LIMMIN, &resBuf, m_pICdwg, NULL, NULL);
//2002?	pDDdwg->setPLIMMIN(OdGePoint2d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1]));

	db_getvar(NULL, DB_QP_LIMMAX, &resBuf, m_pICdwg, NULL, NULL);
//2002?	pDDdwg->setPLIMMAX(OdGePoint2d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1]));

//    if(setviewvars == true) {
//		odResBuf->setRestype(OdResBuf::kRtPoint3d);

		// GRIDUNIT is commeted out in SysVarDefs.h.
		//db_getvar(NULL, DB_QGRIDUNIT, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setPoint3d(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));
//		pDDdwg->setSysVar("GRIDUNIT", odResBuf);

		// SNAPBASE is commeted out in SysVarDefs.h.
		//db_getvar(NULL, DB_QSNAPBASE, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setPoint3d(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));
//		pDDdwg->setSysVar("SNAPBASE", odResBuf);

		// SNAPUNIT is commeted out in SysVarDefs.h.
		//db_getvar(NULL, DB_QSNAPUNIT, &resBuf, m_pICdwg, NULL, NULL);
//		odResBuf->setPoint3d(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));
//		pDDdwg->setSysVar("SNAPUNIT", odResBuf);
//    }

    resBuf.restype=RT3DPOINT;//-------------------------------------

	db_getvar(NULL, DB_QEXTMIN, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setEXTMIN(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QEXTMAX, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setEXTMAX(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QINSBASE, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setINSBASE(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QP_EXTMIN, &resBuf, m_pICdwg, NULL, NULL);
//2002?	pDDdwg->setPEXTMIN(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QP_EXTMAX, &resBuf, m_pICdwg, NULL, NULL);
//2002?	pDDdwg->setPEXTMAX(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QP_INSBASE, &resBuf, m_pICdwg, NULL, NULL);
//2002?	pDDdwg->setPINSBASE(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QP_UCSORG, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPUCSORG(OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QP_UCSXDIR, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPUCSXDIR(OdGeVector3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QP_UCSYDIR, &resBuf, m_pICdwg, NULL, NULL);
	pDDdwg->setPUCSYDIR(OdGeVector3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QUCSORG, &resBuf, m_pICdwg, NULL, NULL);
	odDbSetUCSORG(*(pDDdwg), OdGePoint3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QUCSXDIR, &resBuf, m_pICdwg, NULL, NULL);
	odDbSetUCSXDIR(*(pDDdwg), OdGeVector3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

	db_getvar(NULL, DB_QUCSYDIR, &resBuf, m_pICdwg, NULL, NULL);
	odDbSetUCSYDIR(*(pDDdwg), OdGeVector3d(resBuf.resval.rpoint[0], resBuf.resval.rpoint[1], resBuf.resval.rpoint[2]));

//	if(setviewvars == true) 
//	{
//		resBufPtr = pDDdwg->getSysVar("VIEWCTR");
//		point3D = resBufPtr->getPoint3d();  
//		resBuf.resval.rpoint[0] = point3D.x;
//		resBuf.resval.rpoint[1] = point3D.y;
//		resBuf.resval.rpoint[2] = point3D.z;
//		db_setvar(NULL, DB_QVIEWCTR, &resBuf, m_pICdwg, NULL, NULL, 0);
//
//		resBufPtr = pDDdwg->getSysVar("VIEWDIR");
//		point3D = resBufPtr->getPoint3d();  
//		resBuf.resval.rpoint[0] = point3D.x;
//		resBuf.resval.rpoint[1] = point3D.y;
//		resBuf.resval.rpoint[2] = point3D.z;
//		db_setvar(NULL, DB_QVIEWDIR, &resBuf, m_pICdwg, NULL, NULL, 0);
//
//		resBufPtr = pDDdwg->getSysVar("TARGET");
//		point3D = resBufPtr->getPoint3d();  
//		resBuf.resval.rpoint[0] = point3D.x;
//		resBuf.resval.rpoint[1] = point3D.y;
//		resBuf.resval.rpoint[2] = point3D.z;
//		db_setvar(NULL, DB_QTARGET, &resBuf, m_pICdwg, NULL, NULL, 0);
//	}
//
		sds_point point;
		pDDdwg->setDimfrac(m_pICdwg->ret_dimfrac());
		pDDdwg->setDimtmove(m_pICdwg->ret_dimtmove());
		pDDdwg->setDimtmove(m_pICdwg->ret_dimtmove());
		pDDdwg->setENDCAPS((OdDb::EndCaps)m_pICdwg->ret_endcaps());
		pDDdwg->setJOINSTYLE((OdDb::JoinStyle)m_pICdwg->ret_joinstyle());

		char *chBlkName = new char[256];
		m_pICdwg->get_dimldrblk(chBlkName);
		if (chBlkName[0] != '\0')
		{
			pDDdwg->setDimldrblk(ReadWriteUtils::getBlockIDByName(pDDdwg, m_pICdwg, m_Converter, OdString(chBlkName)));
		}
		delete [] chBlkName;

		char charBuf[128];
		
		m_pICdwg->get_fingerprintguid(charBuf);
		pDDdwg->setFINGERPRINTGUID(OdString(charBuf));

		m_pICdwg->get_versionguid(charBuf);
		pDDdwg->setVERSIONGUID(OdString(charBuf));

//		// TODO MHB
//		// m_pICdwg->set_pucsbase(adhd->curpucsbaseobjhandle);
//		// m_pICdwg->set_pucsorthoref(adhd->cuansferelrpucsorthorefobjhandle);
//		// m_pICdwg->set_ucsbase(adhd->curucsbaseobjhandle);
//		// m_pICdwg->set_ucsorthoref(adhd->curucsorthorefobjhandle);
//
		m_pICdwg->get_pucsorgback(point);
		pDDdwg->setPUCSORGBACK(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_pucsorgbottom(point);
		pDDdwg->setPUCSORGBOTTOM(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_pucsorgfront(point);
		pDDdwg->setPUCSORGFRONT(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_pucsorgleft(point);
		pDDdwg->setPUCSORGLEFT(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_pucsorgright(point);
		pDDdwg->setPUCSORGRIGHT(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_pucsorgtop(point);
		pDDdwg->setPUCSORGTOP(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_ucsorgback(point);
		pDDdwg->setUCSORGBACK(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_ucsorgbottom(point);
		pDDdwg->setUCSORGBOTTOM(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_ucsorgfront(point);
		pDDdwg->setUCSORGFRONT(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_ucsorgleft(point);
		pDDdwg->setUCSORGLEFT(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_ucsorgright(point);
		pDDdwg->setUCSORGRIGHT(OdGePoint3d(point[0], point[1], point[2]));
		m_pICdwg->get_ucsorgtop(point);
		pDDdwg->setUCSORGTOP(OdGePoint3d(point[0], point[1], point[2]));

		// TODO MHB Need to set 
		// db_setvar(NULL,DB_QCPLOTSTYLE,&rb,dp,NULL,NULL,0);

	return;
}

void DWGdirectWriter::TransferEntitiesToDWGdirect(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	db_handitem* pEntity = NULL;

	OdDbObjectId modelspaceId = pDDdwg->getModelSpaceId();
	OdDbObjectId paperspaceId = pDDdwg->getPaperSpaceId();

	for(pEntity = m_pICdwg->entnext_noxref(NULL); pEntity != NULL; pEntity = m_pICdwg->entnext_noxref(pEntity))
	{
		int pPSpace = pEntity->ret_pspace();

		try
		{
			OdDbObjectId id = m_Converter->createOdDbEntity((pPSpace == 0) ? modelspaceId : paperspaceId, pEntity);	

#ifdef _DEBUG_MHB
			// This is not really an error, because vertices for example are created
			// during polyline creation. or sequence ends are not created. Inserts are delayed
			// until the block associated with it is converted.
			if (id.isNull() && pEntity->ret_type() != DB_INSERT && 
							   pEntity->ret_type() != DB_DIMENSION &&
							   pEntity->ret_type() != DB_ATTRIB &&
							   pEntity->ret_type() != DB_SEQEND)
			{
				char tmp[100];
				sprintf(tmp, "Space Entity of type %d was NOT created\n", pEntity->ret_type());
				OutputDebugString(tmp);
			}
#endif
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			// TODO - MHB For some reason dimensions are seen as duplicate records. This doesn't
			// seem to affect the conversion at all, it just fires this exception. I have turned
			// it off for now and will follow up later.
			if (pEntity->ret_type() == DB_DIMENSION && msg.iCompare("Duplicate record name") == 0)
				; // do nothing - skip the assert.
			else
				ASSERT(!"DWGdirectWriter entity creation error: ");
#endif
		}

	}

	// Now we can process groups too.
	processGroupDictionary(pDDdwg);

	return;
}

void DWGdirectWriter::processDictionaryRecord(OdDbDatabasePtr& pDDdwg, OdString& recordName, db_hireflink* recordReference, OdDbDictionaryPtr& pOwnerDictionary)
{
	ASSERT(recordReference != NULL);
	if (recordReference == NULL)
		return;

	int refType;	// DB_SOFT_POINTER 1, DB_HARD_POINTER 2, DB_SOFT_OWNER 3, DB_HARD_OWNER 4
	db_objhandle refHandle;
	db_handitem* pRefHanditem;

	recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);

	int recordType = pRefHanditem->ret_type();

	if (recordType == DB_DICTIONARY)
	{
#ifdef _DEBUG_MHB
		OdString tmp;
		tmp.format("Processing dictionary - %s\n", recordName.c_str());
		OutputDebugString(tmp.c_str());
#endif
		// Already processed.
		if (!recordName.iCompare("ACAD_LAYOUT"/*DNT*/))
			return;

		processDictionary(pDDdwg, pOwnerDictionary->objectId(), pRefHanditem, recordName);
	}
	else
	{
		int entType = pRefHanditem->ret_type();
		if (db_is_objecttype(entType))
		{
#ifdef _DEBUG_MHB
			OdString tmp;
			tmp.format("Processing object - %s\n", recordName.c_str());
			OutputDebugString(tmp.c_str());
#endif
			OdDbObjectId objId = m_Converter->createOdDbObject(pOwnerDictionary->objectId(), pRefHanditem, OdString());
			if (!objId.isNull())
			{
				OdDbObjectPtr pObject = objId.safeOpenObject(OdDb::kForWrite);
				OdDbObjectId objAfterId = pOwnerDictionary->setAt(recordName, pObject);
				ASSERT(objId == objAfterId);
			}

			db_acad_proxy_object* pProxyRef = (db_acad_proxy_object*)pRefHanditem;

			db_hireflink* first;
			db_hireflink* last;
			pProxyRef->get_hirefllends(&first, &last);

//			while(first != NULL)
//			{
//				int type;
//				db_objhandle hand;
//				db_handitem* handItem;
//
//				((db_hireflink*)first)->get_data(&type, &hand, &handItem, m_pICdwg);
//				if (handItem != NULL)
//				{
//					int entType = handItem->ret_type();
//
//					if (db_is_objecttype(entType))
//					{
//						OdDbObjectId newObjId = m_Converter->createOdDbObject(objId, handItem, OdString());
//						if (!objId.isNull())
//						{
//							OdDbObjectPtr pObject = objId.safeOpenObject(OdDb::kForWrite);
//							OdDbObjectId objAfterId = pOwnerDictionary->setAt(recordName, pObject);
//							ASSERT(objId == objAfterId);
//						}
//					}
//					else if (db_is_entitytype(entType))
//					{
//						int pPSpace = handItem->ret_pspace();
//						OdDbObjectId newObjId = m_Converter->createOdDbEntity(objId, handItem);
//					}
//					else
//					{
//						ASSERT(!"Unknown");
//					}
//				}
//
//				first = first->next;
//			}
		}
		else if (db_is_entitytype(entType))
		{
#ifdef _DEBUG_MHB
		OdString tmp;
		tmp.format("Processing entity - %s\n", recordName.c_str());
		OutputDebugString(tmp.c_str());
#endif
		ASSERT(!"Haven't been here before?");
		}
	}
	
	return;
}

void DWGdirectWriter::processDictionary(OdDbDatabasePtr& pDDdwg, OdDbObjectId& ownerDictId, db_handitem* pRefHanditem, OdString& recordName)
{
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	if (recordName.iCompare("ACAD_GROUP"/*DNT*/) == 0)
	{
		// Because of the sequence of events, creating the 
		// group dictionary entries must wait until all the 
		// entities in blocks have been created - like *Paper_Space1
		// for instance. So I have created a seperate function;
		// processGroupDictionary() called from the bottom of 
		// TransferBlocksToDWGdirect()

		//processGroupDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACAD_LAYOUT"/*DNT*/) == 0)
	{
		// Because *Blocks for model/paper space have associated layouts, they
		// are processed prior to general objects. see TransferLayouts()
	}
	else if (recordName.iCompare("ACAD_MLINESTYLE"/*DNT*/) == 0)
	{
		processMlineStylesDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACAD_PLOTSETTINGS"/*DNT*/) == 0)
	{
		processPlotSettingsDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACAD_IMAGE_DICT"/*DNT*/) == 0)
	{
		processImageDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACAD_COLOR"/*DNT*/) == 0)
	{
		processColorDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACAD_MATERIAL"/*DNT*/) == 0)
	{
		processMaterialDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACAD_PLOTSTYLENAME"/*DNT*/) == 0)
	{
		processPlotStyleNameDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else if (recordName.iCompare("ACDBVARIABLEDICTIONARY"/*DNT*/) == 0)
	{
		processAcdbVariableDictionary(pDDdwg, pRefHanditem, recordName);
	}
	else
	{
		processUnknownDictionary(pDDdwg, ownerDictId, pRefHanditem, recordName);
#ifdef _DEBUG_MHB
		char tmp[500];
		sprintf(tmp, "%s was created, but not thouroughly tested.\n", recordName.c_str());
		OutputDebugString(tmp);
#endif
	}

	return;
}

void DWGdirectWriter::processUnknownDictionary(OdDbDatabasePtr& pDDdwg, OdDbObjectId& ownerDictId, db_handitem* pRefHanditem, OdString& dictionaryName)
{
#ifdef _DEBUG_MHB
		OutputDebugString("DWGdirectWriter::processUnknownDictionary() needs testing\n");
#endif
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId unknownDictId = createDictionary(pDDdwg, ownerDictId, pRefHanditem, dictionaryName);
	ASSERT(!unknownDictId.isNull());
	if (unknownDictId.isNull())
		return;
	OdDbDictionaryPtr pUnknownDictionary = unknownDictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pUnknownDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString  recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			db_handitem* pSubRefHanditem = recordReference->ret_hip(m_pICdwg);

			if (pSubRefHanditem != NULL)
			{
				if(pSubRefHanditem->ret_type() == DB_DICTIONARY)
				{
					OdDbObjectId subDictId = createDictionary(pDDdwg, unknownDictId, pSubRefHanditem, recordName);
					ASSERT(!subDictId.isNull());
					if (subDictId.isNull())
					{
						walkingDict = walkingDict->next;
						continue;
					}
					OdDbDictionaryPtr pSubDictionary = subDictId.safeOpenObject(OdDb::kForWrite);
					ASSERT(!pSubDictionary.isNull());

					// Now the records.
					db_dictionaryrec *beginDictRecord= NULL;
					db_dictionaryrec *endDictRecord = NULL;
					((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

					db_dictionaryrec *walkingSubDict = beginDictRecord;
					while (walkingSubDict != NULL)
					{
						db_hireflink* recordReference = walkingSubDict->hiref;
						while (recordReference != NULL)
						{
							processDictionaryRecord(pDDdwg, OdString(walkingSubDict->name), recordReference, pSubDictionary);

							recordReference = recordReference->next;
						}

						walkingSubDict = walkingSubDict->next;
					}
				}
				else
				{
					OdDbObjectId objId = m_Converter->createOdDbObject(unknownDictId, pRefHanditem, recordName);
					if (!objId.isNull())
					{
						OdDbObjectPtr pObject = objId.safeOpenObject(OdDb::kForWrite);
						OdDbObjectId objAfterId = pUnknownDictionary->setAt(recordName, pObject);
						ASSERT(objId == objAfterId);
					}
				}
			}
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}

	return;
}

void DWGdirectWriter::processSortentsTable(OdDbDatabasePtr& pDDdwg, db_hireflink *recordReference, OdDbDictionaryPtr& pOdExtensionDictionary)
{
	ASSERT(!pDDdwg.isNull());

	if (recordReference == NULL)
		return;	// No sortents on this block table record (either *Model_Space or *Paper_Space).

	int refType = 0;
	db_objhandle refHandle;
	db_handitem* pRefHanditem = NULL;

	ASSERT(recordReference->ret_hip(m_pICdwg)->ret_type() == DB_DICTIONARY);

	if (recordReference->ret_hip(m_pICdwg)->ret_type() != DB_DICTIONARY)
		return;
	else
	{
		// Now the records.
		db_dictionaryrec *beginDictRecord= NULL;
		db_dictionaryrec *endDictRecord = NULL;
		((db_dictionary*)recordReference->ret_hip(m_pICdwg))->get_recllends(&beginDictRecord, &endDictRecord);

		db_dictionaryrec *walkingSubDict = beginDictRecord;
		while (walkingSubDict != NULL)
		{
			db_hireflink* recordReference = walkingSubDict->hiref;
			while (recordReference != NULL)
			{
				processDictionaryRecord(pDDdwg, OdString(walkingSubDict->name), recordReference, pOdExtensionDictionary);

				recordReference = recordReference->next;
			}

			walkingSubDict = walkingSubDict->next;
		}

	}

	return;
}

void DWGdirectWriter::processGroupDictionary(OdDbDatabasePtr& pDDdwg)
{
	ASSERT(m_pICdwg != NULL && !pDDdwg.isNull());

	db_dictionary* pNamedObjDict = static_cast<db_dictionary*>(m_pICdwg->namedobjdict());
	if (pNamedObjDict == NULL)
		return;	// serious problem

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	pNamedObjDict->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		if (recordName.iCompare("ACAD_GROUP"/*DNT*/) == 0)
		{
			db_hireflink* recordReference = walkingDict->hiref;

			int refType;	// DB_SOFT_POINTER 1, DB_HARD_POINTER 2, DB_SOFT_OWNER 3, DB_HARD_OWNER 4
			db_objhandle refHandle;
			db_handitem* pRefHanditem;

			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);

			OdDbObjectId groupDictId = pDDdwg->getGroupDictionaryId();
			OdDbDictionaryPtr pOdGroupDictionary = groupDictId.safeOpenObject(OdDb::kForWrite);

			((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);
			db_dictionaryrec *walkingSubDict = beginDictRecord;
			while (walkingSubDict != NULL)
			{
				OdString recordName = OdString(walkingSubDict->name);

				db_hireflink* recordReference = walkingSubDict->hiref;
				while (recordReference != NULL)
				{
					recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);

					ASSERT(pRefHanditem->ret_type() == DB_GROUP);

					if (pRefHanditem->ret_type() == DB_GROUP)
					{
						OdDbObjectId odGroupID = m_Converter->createOdDbObject(groupDictId, pRefHanditem, OdString());
						if (!odGroupID.isNull())
						{
							OdDbGroupPtr pOdGroup = odGroupID.safeOpenObject(OdDb::kForWrite);

							OdDbObjectId odGroupAfterId = pOdGroupDictionary->setAt(recordName, pOdGroup);
							ASSERT(odGroupID == odGroupAfterId);
						}
					}
					recordReference = recordReference->next;
				}
				walkingSubDict = walkingSubDict->next;
			}

			break;
		}

		walkingDict = walkingDict->next;
	}

	return;
}

void DWGdirectWriter::processPlotSettingsDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId plotSettingsDictId = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!plotSettingsDictId.isNull());
	if (plotSettingsDictId.isNull())
		return;
	OdDbDictionaryPtr pOdPlotSettingsDictionary = plotSettingsDictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pOdPlotSettingsDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			ASSERT(pRefHanditem->ret_type() == DB_PLOTSETTINGS);

			OdDbObjectId pOdSettingsID;
			OdDbPlotSettingsPtr pOdSettings;

			db_objhandle plotSettingsHandle = pRefHanditem->RetHandle();
			db_handitem* plotSettingItem = m_pICdwg->handent(plotSettingsHandle);
			if (plotSettingItem != NULL && plotSettingItem->ret_type() == DB_PLOTSETTINGS)
			{
				char* plotSettingName = (char*)((CDbPlotSettings*)m_pICdwg->handent(plotSettingsHandle))->name(m_pICdwg);

				pOdSettings = pOdPlotSettingsDictionary->getAt(plotSettingName, OdDb::kForWrite);
				if (!pOdSettings.isNull())
				{
					ASSERT(pOdSettings->isKindOf(OdDbPlotSettings::desc()));
					pOdSettingsID = pOdSettings->id();
				}
			}

			if (!pOdSettingsID.isNull())
			{
				ASSERT(!pOdSettings.isNull());
				OdDbObjectId odSettingsAfterId = m_Converter->updateOdDbObject(plotSettingsDictId, pRefHanditem, pOdSettings);
				ASSERT(pOdSettingsID == odSettingsAfterId);

				recordReference = recordReference->next;
				continue;
			}

			if (pRefHanditem->ret_type() == DB_PLOTSETTINGS)
			{
				pOdSettingsID = m_Converter->createOdDbObject(plotSettingsDictId, pRefHanditem, OdString());
				pOdSettings = pOdSettingsID.safeOpenObject(OdDb::kForWrite);

				// Extended Entity data (All Apps)
				struct resbuf* extData = pRefHanditem->get_eed(NULL, NULL);
				if (extData != NULL)
				{
					pOdSettings->setXData(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(pDDdwg, extData));
					sds_relrb(extData);
				}

				OdDbObjectId odSettingsAfterId = pOdPlotSettingsDictionary->setAt(recordName, pOdSettings);
				ASSERT(pOdSettingsID == odSettingsAfterId);
			}
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}
	return;
}

void DWGdirectWriter::processPlotStyleNameDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId plotStyleNameDictId = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!plotStyleNameDictId.isNull());
	if (plotStyleNameDictId.isNull())
		return;
	OdDbDictionaryPtr pOdPlotStyleNameDictionary = plotStyleNameDictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pOdPlotStyleNameDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;

	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			ASSERT(pRefHanditem->ret_type() == DB_PLACEHOLDER);

			OdDbObjectId pOdPlaceholderID;
			OdDbPlaceHolderPtr pOdPlaceholder;

			db_objhandle placeholderHandle = pRefHanditem->RetHandle();
			db_handitem* placeholderItem = m_pICdwg->handent(placeholderHandle);
			if (placeholderItem != NULL && placeholderItem->ret_type() == DB_PLACEHOLDER)
			{
				pOdPlaceholder = pOdPlotStyleNameDictionary->getAt(recordName, OdDb::kForWrite);
				if (!pOdPlaceholder.isNull())
				{
					ASSERT(pOdPlaceholder->isKindOf(OdDbPlaceHolder::desc()));
					pOdPlaceholderID = pOdPlaceholder->id();
				}
			}

			if (!pOdPlaceholderID.isNull())
			{
				recordReference = recordReference->next;
				continue;
			}

			if (pRefHanditem->ret_type() == DB_PLACEHOLDER)
			{
				pOdPlaceholderID = m_Converter->createOdDbObject(plotStyleNameDictId, pRefHanditem, OdString());
				pOdPlaceholder = pOdPlaceholderID.safeOpenObject(OdDb::kForWrite);

				OdDbObjectId odPlaceholderAfterId = pOdPlotStyleNameDictionary->setAt(recordName, pOdPlaceholder);
				ASSERT(pOdPlaceholderID == odPlaceholderAfterId);
			}
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}
	
	return;
}

void DWGdirectWriter::processAcdbVariableDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId dictId = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!dictId.isNull());
	if (dictId.isNull())
		return;
	OdDbDictionaryPtr pVariablesDictionary = dictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pVariablesDictionary.isNull());

	// Now we need to add the entries.
	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			if (pRefHanditem->ret_type() == DB_DICTIONARYVAR)
			{
				OdDbObjectId dictVarID = m_Converter->createOdDbObject(pDDdwg->id(), pRefHanditem, OdString());
				OdDbDictionaryVarPtr pOdDictVar = dictVarID.safeOpenObject(OdDb::kForWrite);

				OdDbObjectId odVarDictAfterId = pVariablesDictionary->setAt(recordName, pOdDictVar);
				ASSERT(dictVarID == odVarDictAfterId);

				pOdDictVar->setOwnerId(pVariablesDictionary->objectId());
			}	
			else
			{
				ASSERT(!"What is this?");
			}

			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}

}

void DWGdirectWriter::processColorDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
//#pragma message("DWGdirectWriter::processColorDictionary() needs testing");
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId colorDictId = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!colorDictId.isNull());
	if (colorDictId.isNull())
		return;
	OdDbDictionaryPtr pOdColorDictionary = colorDictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pOdColorDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			ASSERT(!"NEEDS completion");
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			// TODO MHB complete this when you have a sample.

//			ASSERT(pRefHanditem->ret_type() == DB_PLOTSETTINGS);
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}
	return;

}

void DWGdirectWriter::processMaterialDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
//#pragma message("DWGdirectWriter::processMaterialDictionary() needs testing");
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId materialDictId = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!materialDictId.isNull());
	if (materialDictId.isNull())
		return;
	OdDbDictionaryPtr pOdMaterialDictionary = materialDictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pOdMaterialDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			ASSERT(!"NEEDS completion");
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			// TODO MHB complete this when you have a sample.

//			ASSERT(pRefHanditem->ret_type() == DB_PLOTSETTINGS);
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}
	return;

}

void DWGdirectWriter::processMlineStylesDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId mlStyleDictId = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!mlStyleDictId.isNull());
	if (mlStyleDictId.isNull())
		return;
	OdDbDictionaryPtr pOdMlStyleDictionary = mlStyleDictId.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pOdMlStyleDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	char hand[17];
	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			ASSERT(pRefHanditem->ret_type() == DB_MLINESTYLE);

			refHandle.ToAscii(hand);

			OdDbMlineStylePtr pOdMlStyle = pOdMlStyleDictionary->getAt(recordName, OdDb::kForWrite);
			OdDbObjectId odMlStyleID;
			if (!pOdMlStyle.isNull())
			{
				ASSERT(pOdMlStyle->isKindOf(OdDbMlineStyle::desc()));
				odMlStyleID = pOdMlStyle->id();
			}

			if (!odMlStyleID.isNull())
			{
				ASSERT(!pOdMlStyle.isNull());
				OdDbObjectId odMlStyleAfterId = m_Converter->updateOdDbObject(mlStyleDictId, pRefHanditem, pOdMlStyle);
				ASSERT(odMlStyleID == odMlStyleAfterId);

				recordReference = recordReference->next;
				continue;
			}

			if (pRefHanditem->ret_type() == DB_MLINESTYLE)
			{
				odMlStyleID = m_Converter->createOdDbObject(mlStyleDictId, pRefHanditem, OdString());
				pOdMlStyle = odMlStyleID.safeOpenObject(OdDb::kForWrite);

				OdDbObjectId odMlStyleAfterId = pOdMlStyleDictionary->setAt(recordName, pOdMlStyle);
				ASSERT(odMlStyleID == odMlStyleAfterId);
			}
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}

	return;
}

void DWGdirectWriter::processImageDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName)
{
	ASSERT(pRefHanditem != NULL);
	if (pRefHanditem == NULL)
		return;

	ASSERT(pRefHanditem->ret_type() == DB_DICTIONARY);

	OdDbObjectId imageDefDictionaryID = createDictionary(pDDdwg, pDDdwg->getNamedObjectsDictionaryId(), pRefHanditem, recordName);
	ASSERT(!imageDefDictionaryID.isNull());
	if (imageDefDictionaryID.isNull())
		return;
	OdDbDictionaryPtr pOdDbImageDictionary = imageDefDictionaryID.safeOpenObject(OdDb::kForWrite);
	ASSERT(!pOdDbImageDictionary.isNull());

	int refType;
	db_objhandle refHandle;

	db_dictionaryrec *beginDictRecord= NULL;
	db_dictionaryrec *endDictRecord = NULL;
	((db_dictionary*)pRefHanditem)->get_recllends(&beginDictRecord, &endDictRecord);

	db_dictionaryrec *walkingDict = beginDictRecord;
	while (walkingDict != NULL)
	{
		OdString recordName = OdString(walkingDict->name);
		db_hireflink* recordReference = walkingDict->hiref;
		while (recordReference != NULL)
		{
			recordReference->get_data(&refType, &refHandle, &pRefHanditem, m_pICdwg);
			ASSERT(pRefHanditem->ret_type() == DB_IMAGEDEF);

			OdDbObjectId odImageDefID;
			OdDbRasterImageDefPtr pOdImageDef;
			if (pRefHanditem->ret_type() == DB_IMAGEDEF)
			{
				odImageDefID = m_Converter->createOdDbObject(imageDefDictionaryID, pRefHanditem, OdString());
				pOdImageDef = odImageDefID.safeOpenObject(OdDb::kForWrite);

				OdDbObjectId odImageDefAfterId = pOdDbImageDictionary->setAt(recordName, pOdImageDef);
				ASSERT(odImageDefID == odImageDefAfterId);
			}
			recordReference = recordReference->next;
		}
		walkingDict = walkingDict->next;
	}

	return;
}

OdDbObjectId DWGdirectWriter::getUcsIDByName(OdDbDatabasePtr& pDDdwg, OdString& name)
{
	ASSERT(!pDDdwg.isNull());

	OdDbUCSTablePtr pTable = pDDdwg->getUCSTableId().safeOpenObject();

	if (!name.isEmpty())
	{
		OdDbObjectId id = pTable->getAt(name);
		if (!id.isNull())
		{
			return id;
		}
	}
	return OdDbObjectId();
}

OdDbObjectId DWGdirectWriter::createDictionary(OdDbDatabasePtr& pDDdwg, OdDbObjectId& ownerDictionaryId, db_handitem* pDictionary, OdString& name)
{
	ASSERT(!pDDdwg.isNull());
	ASSERT(!ownerDictionaryId.isNull());
	ASSERT(pDictionary != NULL && pDictionary->ret_type() == DB_DICTIONARY);
	ASSERT(!name.isEmpty());

	OdDbDictionaryPtr pRootDict = ownerDictionaryId.safeOpenObject(OdDb::kForWrite);
	OdDbObjectId dictId = pRootDict->getAt(name);
	if (dictId.isNull())
	{
		OdDbObjectPtr pDict = OdDbDictionary::createObject();
		OdDbObjectId idBefore = ReadWriteUtils::setHandle(pDDdwg, pDictionary, pRootDict->id(), pDict);

		int ownerFlag;
		((db_dictionary*)pDictionary)->get_280(&ownerFlag);
		((OdDbDictionaryPtr)pDict)->setTreatElementsAsHard((ownerFlag == 1) ? true : false);

		int mergeStyle;
		((db_dictionary*)pDictionary)->get_281(&mergeStyle);
		((OdDbDictionaryPtr)pDict)->setMergeStyle((OdDb::DuplicateRecordCloning)mergeStyle);

		dictId = pRootDict->setAt(name, pDict);

		ASSERT(idBefore == dictId);
	}

	return dictId;
}

bool DWGdirectWriter::saveAsSVG(OdDbDatabasePtr& pDDdwg, char* fileName)
{
	bool retVal = false;

	OdGsDevicePtr dev = createSvgDevice()->create();

    if ( !dev.isNull() )
    {
		OdStreamBufPtr file = odSystemServices()->createFile(fileName, Oda::kFileWrite);
  
		// Setup output stream
		dev->properties()->putAt("Output"/*DNT*/, file.get());
		// size of pixel in device units
		dev->properties()->putAt("LineWeightScale"/*DNT*/, OdRxVariantValue(3.0));
		// prefix to prepend to image name
		dev->properties()->putAt("ImageUrl"/*DNT*/, OdRxVariantValue( OdString("./")));
		// Set active palette
		dev->setLogicalPalette(odcmAcadLightPalette(), 256);

		// Prepare database context for device
		OdGiContextForDbDatabasePtr pDwgContext = OdGiContextForDbDatabase::createObject();
		pDwgContext->setDatabase(pDDdwg);

		// do not render paper
		pDwgContext->setPlotGeneration(true);

		// Prepare the device to render the active layout in this database.
		OdGsDevicePtr wrapper = OdDbGsManager::setupActiveLayoutViews( dev, pDwgContext );

		// Setup device coordinate space
		wrapper->onSize( OdGsDCRect( 0, 1024, 768, 0 ) );

		// Initiate rendering.
		wrapper->update( 0 );

		retVal = true;
	}

	return retVal;
}

bool DWGdirectWriter::saveAsDWF(OdDbDatabasePtr& pDDdwg, char* fileName, int version)
{
	ASSERT(fileName != NULL);

	bool exportALLlayouts = (version & 0x40) == 0x40;


	DwExportParams params;

	// Version
	if (version & 0x1)
		params.Version = nDwf_v42;
	else if (version & 0x2)
		params.Version = nDwf_v55;
	else
		params.Version = nDwf_v60;

	// Format
	if (version & 0x8)
		params.Format = DW_COMPRESSED_BINARY;
	else if (version & 0x10)
		params.Format = DW_UNCOMPRESSED_BINARY;
	else
		params.Format = DW_ASCII;

	if (exportALLlayouts)
	{
		params.Format = DW_ASCII;
		initPageData(pDDdwg, fileName, params);
	}
	else
	{
		DwfPageData pageData;
		pageData.sLayout = pDDdwg->findActiveLayout(true);
		params.arPageData.append(pageData);
	}

	params.pDb = pDDdwg;
	params.sDwfFileName = fileName;
	params.background = ODRGB(255, 255, 255);
	params.pPalette = odcmAcadLightPalette();
	exportDwf(params);

	return true;
}

void DWGdirectWriter::initPageData(OdDbDatabasePtr& pDDdwg, char* fileName, DwExportParams& params)
{
	OdString tmpFName = fileName;
	int iLastBackslash = tmpFName.reverseFind('\\');
	if(iLastBackslash)
		iLastBackslash++; // step past the back slash.
	OdString strSeed = tmpFName.right(tmpFName.getLength() - iLastBackslash);
	strSeed.deleteChars(strSeed.find('.'), 4);
		

	// Model layout;
	OdDbObjectId idMsLayout = OdDbBlockTableRecordPtr(pDDdwg->getModelSpaceId().safeOpenObject())->getLayoutId();

	OdDbLayoutPtr pMsLayout = idMsLayout.safeOpenObject();
	{
		DwfPageData pageData;
		OdString str;

		OdString strLayoutName = pMsLayout->getLayoutName();
		pageData.sLayout = strLayoutName;

		// init default sheet name
		OdString strSheetName = strSeed + '-' + strLayoutName;
		pageData.sPageTitle = strSheetName;

		params.arPageData.append(pageData);
	}

	// Paper space layouts
	OdDbDictionaryPtr         pLayoutDict = pDDdwg->getLayoutDictionaryId().safeOpenObject();
	OdDbDictionaryIteratorPtr pLayoutIter = pLayoutDict->newIterator();

	for (; !pLayoutIter->done(); pLayoutIter->next())
	{
		OdDbObjectId idCur = pLayoutIter->objectId();
		if (idCur == idMsLayout)
		{
		  continue;
		}

		DwfPageData pageData;
		OdString str;

		OdString strLayoutName = pLayoutIter->name();

		pageData.sLayout = strLayoutName;

		// init default sheet name
		OdString strSheetName = strSeed + '-' + strLayoutName;
		pageData.sPageTitle = strSheetName;

		params.arPageData.append(pageData);
	}

	return;
}

OdDbObjectId DWGdirectWriter::TransferXRefToDWGdirect(OdDbDatabasePtr& pDDdwg, db_blocktabrec* pBlock)
{
	char* blkName;
	char *xrefPath;
	pBlock->get_1(&xrefPath);
	pBlock->get_2(&blkName);

	char hand[17];
	OdPassword passwd;	// TODO MHB deal with password protected files.
	pBlock->RetHandlePtr()->ToAscii(hand);
	OdDbBlockTableRecordPtr pXRefBlock = OdDbXRefManExt::addNewXRefDefBlock(pDDdwg, xrefPath, blkName, false, passwd, OdDbHandle(hand));

	pXRefBlock->setPathName(OdString(xrefPath));

	char szRecordName[DB_ACADBUF];
	OdString xrefBlockName(blkName);

	// Transfere layers for this xref.
	db_layertabrec* layerRecord = NULL;
	for (layerRecord=(db_layertabrec*)m_pICdwg->tblnext(DB_LAYERTABREC,1); layerRecord!=NULL; layerRecord=(db_layertabrec*)m_pICdwg->tblnext(DB_LAYERTABREC,0)) 
	{
		int flags;
		layerRecord->get_70(&flags);

		if ((flags & BIT10) && (flags & BIT20))	// Belongs to an xref
		{
			layerRecord->get_2(szRecordName, DB_ACADBUF - 1);
			OdString layerName(szRecordName);
			
			OdString blkName = layerName.left(layerName.find('|'));
			OdString layName = layerName.right(layerName.getLength() - layerName.find('|') - 1);
			
			if (xrefBlockName.iCompare(blkName) == 0)
			{
				if (layName.iCompare("0"/*DNT*/))
				{
					OdDbLayerTableRecordPtr pLayer = OdDbXRefManExt::addNewXRefDependentLayer(pXRefBlock, layName);

					setLayerProps(pDDdwg, pLayer, layerRecord);
				}
			}
		}
	}

	// Transfer textstyles for this xref.
	db_styletabrec* textStyleRecord = NULL;
	for (textStyleRecord=(db_styletabrec*)m_pICdwg->tblnext(DB_STYLETABREC,1); textStyleRecord!=NULL; textStyleRecord=(db_styletabrec*)m_pICdwg->tblnext(DB_STYLETABREC,0)) 
	{
		int flags;
		textStyleRecord->get_70(&flags);

		if ((flags & BIT10) && (flags & BIT20))	// Belongs to an xref
		{
			textStyleRecord->get_2(szRecordName, DB_ACADBUF - 1);
			OdString styleName(szRecordName);
			
			OdString blkName = styleName.left(styleName.find('|'));
			OdString stlName = styleName.right(styleName.getLength() - styleName.find('|') - 1);
			
			if (xrefBlockName.iCompare(blkName) == 0)
			{
				if (stlName.iCompare("Standard"/*DNT*/))
				{
					OdDbTextStyleTableRecordPtr pTextStyle = OdDbXRefManExt::addNewXRefDependentTextStyle(pXRefBlock, stlName);
					OdDbObjectId odStyleAfterId = m_Converter->updateOdDbObject(pDDdwg->getTextStyleTableId(), (db_handitem*)textStyleRecord, pTextStyle);
					ASSERT(pTextStyle->objectId() == odStyleAfterId);
				}
			}
		}
	}


	// Transfer linetypes for this xref.
	db_ltypetabrec* linetypeRecord = NULL;
	for (linetypeRecord=(db_ltypetabrec*)m_pICdwg->tblnext(DB_LTYPETABREC,1); linetypeRecord!=NULL; linetypeRecord=(db_ltypetabrec*)m_pICdwg->tblnext(DB_LTYPETABREC,0)) 
	{
		int flags;
		linetypeRecord->get_70(&flags);

		if ((flags & BIT10) && (flags & BIT20))	// Belongs to an xref
		{
			linetypeRecord->get_2(szRecordName, DB_ACADBUF - 1);
			OdString linetypeName(szRecordName);
			
			OdString blkName = linetypeName.left(linetypeName.find('|'));
			OdString ltName = linetypeName.right(linetypeName.getLength() - linetypeName.find('|') - 1);
			
			if (xrefBlockName.iCompare(blkName) == 0)
			{
				if (ltName.iCompare("Standard"/*DNT*/))
				{
					OdDbLinetypeTableRecordPtr pLineType = OdDbXRefManExt::addNewXRefDependentLinetype(pXRefBlock, ltName);

					setLineTypeProps(pDDdwg, pLineType, linetypeRecord);
				}
			}
		}
	}

	return pXRefBlock->objectId();
}

void DWGdirectWriter::setLineTypeProps(OdDbDatabasePtr& pDDdwg, OdDbLinetypeTableRecordPtr& pLineType, db_ltypetabrec* pLineTypeRec)
{
	ASSERT(pLineTypeRec != NULL);
	ASSERT(!pDDdwg.isNull());
	ASSERT(!pLineType.isNull());

	char* str;
	pLineTypeRec->get_3(&str);
	pLineType->setComments(OdString(str));

	int intVal;

	pLineTypeRec->get_72(&intVal);
	pLineType->setIsScaledToFit((intVal != 65) ? true : false);

	int numDashes;
	pLineTypeRec->get_73(&numDashes);
	// must be more that just 1. If 1 change to zero.
	numDashes = (numDashes == 1) ? 0 : numDashes;
	pLineType->setNumDashes(numDashes);

	int flags;
	pLineTypeRec->get_70(&flags);

	char* text;
	int type, shapeNum;
	db_objhandle styleHandle;
	double xOffset, yOffset, scale, length, rotation;
	int patternLen = 0;
	for (int i = 0; i < numDashes; i++)
	{
		pLineTypeRec->get_dashdata(i, &text, &xOffset, &yOffset, &scale, &length, &rotation, &type, &shapeNum, &styleHandle);

		pLineType->setDashLengthAt(i, length);
		pLineType->setShapeIsUcsOrientedAt(i, ((flags & BIT1) == BIT1));
		pLineType->setShapeNumberAt(i, shapeNum);
		pLineType->setShapeOffsetAt(i, OdGeVector2d(xOffset, yOffset));
		pLineType->setShapeRotationAt(i, rotation);
		pLineType->setShapeScaleAt(i, scale);

		pLineType->setTextAt(i, text);
		patternLen += (length < 0.0) ? (length * -1) : length;

		if (styleHandle == db_objhandle(NULL))
			continue;

		OdDbObjectId styleId = ReadWriteUtils::getTextStyleIDByName(pDDdwg, ReadWriteUtils::getStyleNameByHandle(m_pICdwg, styleHandle));
		pLineType->setShapeStyleAt(i, styleId);

	}
	pLineType->setPatternLength(patternLen);

	return;
}

void DWGdirectWriter::setLayerProps(OdDbDatabasePtr& pDDdwg, OdDbLayerTableRecordPtr& pLayer, db_layertabrec* pLayerRec)
{
	ASSERT(pLayerRec != NULL);
	ASSERT(!pDDdwg.isNull());
	ASSERT(!pLayer.isNull());

	int intVal, iColor;
	pLayerRec->get_70(&intVal);

	pLayer->setIsFrozen((bool)((intVal & BIT1) == BIT1));
	pLayer->setVPDFLT((bool)((intVal & BIT2) == BIT2));
	pLayer->setIsLocked((bool)((intVal & BIT4) == BIT4));

	pLayerRec->get_62(&iColor);
	bool bIsOff = iColor < 0;
	iColor = (iColor < 0) ? iColor * -1: iColor;

	pLayer->setColorIndex(iColor);
	pLayer->setIsOff(bIsOff);

	pLayerRec->get_290(&intVal);
	pLayer->setIsPlottable((bool)(intVal != 0));

	pLayerRec->get_370(&intVal);
	pLayer->setLineWeight((OdDb::LineWeight)intVal);

	char* lineTypeName;
	pLayerRec->get_6(&lineTypeName);
	pLayer->setLinetypeObjectId(ReadWriteUtils::getLinetypeIDByName(pDDdwg, OdString(lineTypeName)));

	db_handitem* plotStyle = NULL;
	pLayerRec->get_390(&plotStyle);
	char hand[17];
	if (plotStyle != NULL)
	{
		plotStyle->RetHandlePtr()->ToAscii(hand);
		OdDbHandle handle(hand);
		OdDbObjectId plotStyleId = pDDdwg->getOdDbObjectId(handle); 
		if (!plotStyleId.isNull())
			pLayer->setPlotStyleName(plotStyleId);
	}

	return;
}

void _rollBackFile(OdString& tmpFileName, const char* fileName)
{
	// remove the temporary file.
	remove(tmpFileName);
	// Rename the .bak file to the orginal fileName.
	OdString tmpBAKFile(fileName);
	tmpBAKFile = tmpBAKFile.left(tmpBAKFile.getLength() - 4);
	tmpBAKFile += EXTBAK;

	int val = rename(tmpBAKFile, fileName);
	ASSERT(val == 0);

	return;
}

void _renameTmpFile(OdString& tmpFileName, const char* fileName)
{
	// rename xxx.$$$ to xxx.dwg/.dxf
	int val = rename(tmpFileName, fileName);
	if (val == EACCES)	// fileName already exists, so remove it first and try again.
	{
		remove(fileName);
		val = rename(tmpFileName, fileName);
		ASSERT(val == 0);
	}
	remove(tmpFileName);
	return;
}

void DWGdirectWriter::wrapUp(OdDbDatabasePtr& pDDdwg)
{
	m_Converter->processDelayedEntities();
	m_Converter->processPersistantReactors();
	m_Converter->processProxyReferences();

	// Check for sortents tables on *Model_Space block
	if (m_pICdwg->ret_xdict(0) != NULL)
	{
		pDDdwg->getModelSpaceId().safeOpenObject(OdDb::kForWrite)->createExtensionDictionary();
		OdDbDictionaryPtr pOdExtensionDictionary = pDDdwg->getModelSpaceId().safeOpenObject(OdDb::kForRead)->extensionDictionary().safeOpenObject(OdDb::kForWrite);
		processSortentsTable(pDDdwg, m_pICdwg->ret_xdict(0), pOdExtensionDictionary);
	}

	// Check for sortents tables on *Paper_Space block
	if (m_pICdwg->ret_xdict(1) != NULL)
	{
		pDDdwg->getPaperSpaceId().safeOpenObject(OdDb::kForWrite)->createExtensionDictionary();
		OdDbDictionaryPtr pOdExtensionDictionary = pDDdwg->getPaperSpaceId().safeOpenObject(OdDb::kForRead)->extensionDictionary().safeOpenObject(OdDb::kForWrite);
		processSortentsTable(pDDdwg, m_pICdwg->ret_xdict(1), pOdExtensionDictionary);
	}

	return;
}

