#include "StdAfx.h"
#include "DwfImportImpl.h"
#include "DwfCallbacks.h"
#include "summinfo.h"
// w2d metadata records are deprecated in 6.00
//

#define SUMMINFO_VALUE(value, BigValue, SummValue) \
WT_Result DwfCallbackManager::process_##value##(WT_##BigValue &v, WT_File&)\
{\
	if (_importer->_ignoreMetadata) return WT_Result::Success;\
	OdDbDatabaseSummaryInfoPtr summ = oddbGetSummaryInfo(_importer->database());\
	summ->set##SummValue##(OdString(v.string().ascii()));\
	return WT_Result::Success;\
}

SUMMINFO_VALUE(author, Author, Author);
SUMMINFO_VALUE(creator, Creator, LastSavedBy);
SUMMINFO_VALUE(keywords, Keywords, Keywords);
SUMMINFO_VALUE(subject, Subject, Subject);
SUMMINFO_VALUE(title, Title, Title);
SUMMINFO_VALUE(comments, Comments, Comments);

#define SUMMINFO_CUSTOM_VALUE(value, BigValue, SummValue)\
WT_Result DwfCallbackManager::process_##value##(WT_##BigValue &v, WT_File&)\
{\
	if (_importer->_ignoreMetadata) return WT_Result::Success;\
	OdDbDatabaseSummaryInfoPtr summ = oddbGetSummaryInfo(_importer->database());\
	summ->addCustomSummaryInfo(SummValue, OdString(v.string().ascii()));\
	return WT_Result::Success;\
}

SUMMINFO_CUSTOM_VALUE(modification_time, Modification_Time, "Modification time");
SUMMINFO_CUSTOM_VALUE(description, Description, "Description");
SUMMINFO_CUSTOM_VALUE(source_filename, Source_Filename, "Source file name");
SUMMINFO_CUSTOM_VALUE(source_creation_time, Source_Creation_Time, "Source creation time");
SUMMINFO_CUSTOM_VALUE(source_modification_time, Source_Modification_Time, "Source modification time");
SUMMINFO_CUSTOM_VALUE(copyright, Copyright, "Copyright");
SUMMINFO_CUSTOM_VALUE(creation_time, Creation_Time, "Creation time");

