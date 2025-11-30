
#pragma once

#define	ICAD_ENDSPACERSTR "--SubMenuEndingSpacer--"

// This structure contains all necessary information needed by our menu items.
typedef struct tagICMENUITEM
{
	CString	itemName,			// Item name.
			tearOffName,		// Tear-off menu name.
			command,			// Command string to be sent to SDS Event
			helpString,			// Help string.
			chekVar,			// Controls the checked state of items based on the value of the setvar.
			grayVar,			// Controls whether the item is available based on the setvar.
			fileName;			// If this item was created with menuload, the file name gets stored here (for menuunload).
	long    lflgVisibility,		// Bit-set value for the visibility of this item.
			lflgEntVis;			// Bit-set value for the entity visibility of this item.
	short	nSubLevel;			// Number representing how many sublevels deep an item belongs to.
	BOOL	bAddSpacerBefore;	// Flag indicating that a spacer should be placed before the item.

	tagICMENUITEM() : lflgVisibility(0), lflgEntVis(0), nSubLevel(0), bAddSpacerBefore(FALSE)
	{}

} ICMENUITEM, FAR* LPICMENUITEM;

// This structure contains all necessary information needed to read an AutoCAD mnu file.
typedef struct tagICMNUINFO
{
	CString	title,
			tearOffName,		// Tear-off menu name.
			command,
			helpString,			// Help string.
			expression,			// AutoCAD DIESEL Expression.
			fileName;			// Saves the filename this item was created with (for menuunload).
	BOOL	bIsContextMenu;		// AutoCAD uses pop0 as a context menu.

	tagICMNUINFO	*pmiSub,
					*next;

	tagICMNUINFO() : pmiSub(NULL), next(NULL), bIsContextMenu(FALSE)
	{}

} ICMNUINFO, FAR* LPICMNUINFO;


/////////////////////////////////////////////////////////////////////////////
// CIcadMenu
/////////////////////////////////////////////////////////////////////////////

class CIcadApp;

class CIcadMenu
{
	friend CIcadMenuMain;

	// Inner class
private:
	class CIcadLoadedMenuInfo
		{
		private:
			CString	m_strMenuGroupName;
			CString m_strMenuFilePath;

			// ctors and dtors
		public:
			CIcadLoadedMenuInfo(CString strGroup, CString strPath)
			{
				m_strMenuGroupName = strGroup;
				m_strMenuFilePath = strPath;
			}
			virtual ~CIcadLoadedMenuInfo()
			{
				// nothing to do
			}
			bool MatchesGroup(CString strOtherGroup)
			{
				return !m_strMenuGroupName.CompareNoCase(strOtherGroup);
			}
			CString GetPathName()
			{
				return m_strMenuFilePath;
			}
			CString GetGroupName()
			{
				return m_strMenuGroupName;
			}
		};

// Member variables
public:
	LPICMENUITEM	m_pMenuArray;
	int				m_nMenuItems,
					m_ctRef;
	BOOL			m_bMenuModified,
					m_bDisplayedWarning;
	int				m_idxModifyMenu,		// Index of the Modify Menu in the menu array (for ICAD_WNDACTION_RCLICK).
					m_idxEsnapMenu;			// Index of the Entity Snap Menu in the menu array (for shift right-click).
	CString			m_strMenuFile;
	HMENU			m_hMenu;
	BOOL			m_bIsEmbedMenu;

private:
	CArray<CIcadLoadedMenuInfo*, CIcadLoadedMenuInfo*>	m_theLoadedMenuList;

protected:
	CIcadApp*		m_pApp;
	CMainWindow*	m_pMain;

// Member functions
public:
	CIcadMenu(CIcadApp* pApp, CMainWindow* pMain);
	~CIcadMenu();
	void	SaveToReg();
	void	EnableAllItems(BOOL bEnable = TRUE);
	void	UpdateCheckState(char* Setvar,CMenu* pMenu);
	HMENU	UpdateVisibility();
	void	DoMenuRef(LPCTSTR pszRef);
	HMENU	MenuCreator(long lflgVisibility);
	HMENU	SubMenuCreator(int* idxCurArrayItem, long lflgVisibility, long lflgEntVis);
	LPSTR	GetHelpString(int nID);
	LPSTR	GetCommandString(int nID);
	BOOL	LoadMenu(LPCTSTR pszMenuFile, BOOL bAppend);
	BOOL	CreateMenuArrayFromReg();
	void	CreateDefaultMenuArray();
	void	CreateDefaultSubMenuItems(long lMnuID, int* idxCurArrayItem, int* ctRecur);
	void	DeleteMenuArray();
	BOOL	LoadFromICM(LPCTSTR pszFileName);
	BOOL	SaveToICM(LPCTSTR pszFileName);
	// AutoCAD mnu reader functions
	LPICMNUINFO	MnuToMnuInfo(LPCTSTR pszFileName, BOOL bAppend);
	void	MnuInfoToMenuArray(LPICMNUINFO pMnuInfo, BOOL bAppend);
	void	FreeMnuInfo(LPICMNUINFO pmiBeg);
	int		MnuInfoCount(LPICMNUINFO pmiBeg);
	BOOL	UnloadMnuFromMenuArray(LPCTSTR pszFileName);
	void	CopyItem(LPICMENUITEM lpDest, LPICMENUITEM lpSrc);
	BOOL	SetMenu(BOOL bDestroyOld = TRUE);
	void	AddRef();
	BOOL	Release();

	bool	IsMenuGroupLoaded(CString strGroup);
	bool	FreeLoadedMenuList();
	bool	AddMenuToLoadedMenuList(CString strGroup, CString strPathName);
	bool	RemoveMenuFromLoadedMenuList(CString strGroup);
	bool	LoadMNLFilesFromLoadedMenuList();

protected:
	virtual bool DestroyIcadMenu(HMENU hMenu);
	void	SubMenuItemsFromMnuInfo(LPICMNUINFO pmiParent, int* idxCurArrayItem);
	char*	CreateMnuSubItems(LPCTSTR pszFileName, LPCTSTR pBuffer, LPICMNUINFO pmiSubBeg, int* ctRecur, LPCTSTR helpstrs, char** sortlist, int sortcount);
	int		UnloadSubItems(LPCTSTR pszFileName, int* idxCurItem, LPICMENUITEM lpNewArray, int* idxNewItem);
};


// Prototypes

LONG IcadRegDeleteKey(HKEY hKey, LPCTSTR lpSubKey);
LONG IcadCopyKeyAndValue(LPCTSTR lpFromKey, LPCTSTR lpToKey, BOOL bForceRemove);
BOOL IcadSetKeyAndValue(LPTSTR pszKey, LPTSTR pszSubkey, LPTSTR pszValue);
BOOL IcadSetKeyAndValue2(HKEY Root, LPTSTR pszKey, LPTSTR pszSubkey, LPTSTR pszValueName, DWORD ValueType, BYTE* Value);
int  IcadMenuPositionSort(const void* cmp1, const void* cmp2);
