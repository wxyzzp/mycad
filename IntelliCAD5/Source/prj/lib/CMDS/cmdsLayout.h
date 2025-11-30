#ifndef __cmdsLayoutH__
#define __cmdsLayoutH__

/*-------------------------------------------------------------------------*//**
Implementation of LAYOUT command

@author Denis Petrov
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_layout();

/*-------------------------------------------------------------------------*//**
Implementation of PAGESETUP command

@author Denis Petrov
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_pagesetup();

/*-------------------------------------------------------------------------*//**
Implementation of LayoutWizard command

@author Vishwanath Burkule
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_layoutwizard();

/*-------------------------------------------------------------------------*//**
Gets currnet LAYOUT 

@author Denis Petrov
@param layout <= SDS name of the current LAYOUT 
@return 1 for success, 0 for error
*//*------------------------------------------------------------------------*/
int cmd_getCurrentLayout(sds_name layout);

/*-------------------------------------------------------------------------*//**
Sets up value of CTAB system variable if it's supported 

@author Denis Petrov
@return 1 for success, 0 for error
*//*------------------------------------------------------------------------*/
int cmd_setUpCTAB();

class CDbLayout;
class db_drawing;
class db_dictionary;
class db_dictionaryrec;
class db_blocktabrec;

/*-----------------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Class for database-specific routines that manipulate and access CDbLayout objects.
*//*------------------------------------------------------------------------------------------*/
class CLayoutManager
{
public:
	CLayoutManager(db_drawing* pDrawing = SDS_CURDWG);

	void resetIterator();
	bool next(const char*& pName, CDbLayout** pLayout = NULL, db_blocktabrec** pBlockRec = NULL, bool bAllowModel = true);

	int createLayout(const char* pName, CDbLayout** pLayout = NULL);
	int copyLayout(const char* pName, const char* pNameNew);
	int renameLayout(const char* pNameOld, const char* pNameNew);
	int deleteLayout(const char* pName);
	int setCurrentLayout(const char* pName);

	const char* findActiveLayout(bool bAllowModel, CDbLayout** pLayout = NULL) const;
	CDbLayout* findLayoutNamed(const char* pName) const;

	int importLayout(sds_name sdsLayoutName);
	const char* defaultName();
	static long getLayoutEntities(sds_name layoutSS, sds_name sdsLayoutName);
private:
	static const char s_model[6];
	static char s_defaultName[IC_ACADBUF];
	db_drawing* m_pDrawing;
	db_dictionary* m_pDict;
	db_dictionaryrec* m_pDictIt;

	db_blocktabrec* createLayoutBlock();
	int addToDictionary(const char* pName, CDbLayout* pLayout);
	int setCurrentLayout(CDbLayout* pOldLayout, CDbLayout* pNewLayout);
};

#endif
