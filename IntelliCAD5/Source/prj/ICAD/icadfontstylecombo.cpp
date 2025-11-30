// IcadFontStyleComboBox.cpp : implementation file
//

#include "icad.h"
#include "IcadFontStyleCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadFontStyleComboBox construction/destruction

/******************************************************************************
*+ CIcadFontStyleComboBox::CIcadFontStyleComboBox - constructor
*
*/

CIcadFontStyleComboBox::CIcadFontStyleComboBox()
: CComboBox()
	{
	}

/******************************************************************************
*+ CIcadFontStyleComboBox::~CIcadFontStyleComboBox - destructor
*
*/

CIcadFontStyleComboBox::~CIcadFontStyleComboBox()
	{
	}

/******************************************************************************
*+ CIcadFontStyleComboBox::~CIcadFontStyleComboBox - destructor
*
*/

void CIcadFontStyleComboBox::DeleteStyleItems(
	void)
	{
	int count;
	StyleItem *styleItem;

	// delete the styleItem attached to each entry
	count = GetCount();
	ASSERT(CB_ERR != count);
	if (CB_ERR != count)
		{
		for (int index = 0; index < count; index++)
			{
			styleItem = (StyleItem *) GetItemDataPtr(index);
			ASSERT(-1 != (int) styleItem);
			if (-1 != (int) styleItem)
				{
				delete styleItem;
				styleItem = NULL;
                SetItemDataPtr(index, NULL);    //Spirin Vitaly
				}
			}
		}
	}



/////////////////////////////////////////////////////////////////////////////
// CIcadFontStyleComboBox operations

/******************************************************************************
*+ CIcadFontStyleComboBox::FillTrueTypeStyles
*
*/

void CIcadFontStyleComboBox::FillTrueTypeStyles(
	const char *fontName,
	const long fontStyle)
	{
	LOGFONT logFont;

	// clear the current list
	DeleteStyleItems();					// delete dynamic memory before resetting contents
	ResetContent();

	ASSERT(fontName);
	if (NULL == fontName)
		strcpy(logFont.lfFaceName, "Arial"/*DNT*/);	// should be safe; ran into problems just returning
	else
		{
		// we had a problem with empty font names;
		// though they should never occur, we'll trap
		// for them

		CString tmpFontName = fontName;
		tmpFontName.TrimLeft();
		if (tmpFontName.IsEmpty())
			strcpy(logFont.lfFaceName, "Arial"/*DNT*/);	// should be safe; ran into problems just returning
		else
			strcpy(logFont.lfFaceName, fontName);
		}

	// enumerate all of the specified font's related fonts
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfPitchAndFamily = 0;

	CClientDC dc(NULL);
	::EnumFontFamiliesEx(dc.GetSafeHdc(),
						 &logFont,
						 (FONTENUMPROC)CIcadFontStyleComboBox::EnumFontFamExProcStyle,
						 (LPARAM) this,
						 0);

	int tmpInt = GetCount();

	StyleItem* styles[4] = {NULL, NULL, NULL, NULL};
	CStringArray names;

	if( !tmpInt )
	{
		// If no fonts found, then just fill ComboBox with warning message
		styles[0] = new StyleItem;
		if( styles[0] )
		{

			strncpy(styles[0]->fullName, fontName, LF_FULLFACESIZE - 1 );
			strcpy(styles[0]->styleName, "Font not found" );
			int idx = AddString( (LPCTSTR) styles[0]->styleName );
			if( idx < 0 )
			{
				delete styles[0];
			}
			else
			{
				if( CB_ERR == SetItemDataPtr( idx, (void*)styles[0] ) )
				{
					DeleteString( idx );
					delete styles[0];
				}
				else
				{
					SetCurSel(0);
				}

			}
		}
		return;
	}

	names.SetSize(tmpInt);

	int done = 0, done2nd = 0, donelast = 0;	//indicates which data has already been used.

	//store required info in the arrays...
	for (int i = 0; i < tmpInt; i++)
		{
		StyleItem* p_ItemData = (StyleItem*) GetItemDataPtr(i);
		styles[i] = new StyleItem;
		*styles[i] = *p_ItemData;
		names[i] = p_ItemData->styleName;
		delete p_ItemData;
		}

	//then delete all content in the box...
	ResetContent();

	//and reinsert them in the correct order.
	int j = 0;
	bool test = false;	//have we found a match?

	//this for loop is executed for all comboboxes
	for (j; j < i; j++)
		{

		//conditions: non-bold, non-italic
		if (styles[j]->weight < 600 && styles[j]->italic == 0)
			{
			AddString(names[j]);
			SetItemDataPtr(0, (void*)styles[j]);
			done = j;
			j = 4;
			test = true;
			}
		}
	j = 0;

	//failed!  Ditch the bold condition, find a non-italic font.  There might be only one
	//available selection, or there might be two, both of them bold.
	if(!test)
		{
		for (j; j < i; j++)
			{
			if (styles[j]->italic == 0)
				{
				AddString(names[j]);
				SetItemDataPtr(0, (void*)styles[j]);
				done = j;
				j = 4;
				test = true;
				}
			}
		}

	//default - insert first selection.  This will get called in many instances when there is only
	//one available selection.
	if (!test)
		{
		AddString(names[0]);
		SetItemDataPtr(0, (void*)styles[0]);
		done = 0;
		}
	j = 0;
	test = false;

	//for all comboboxes with 2 or more selections.
	if (i > 1)
		{
		for (j; j < i; j++)
			{

			//conditions: non-bold, italics
			if (styles[j]->weight < 600 && styles[j]->italic == 255 && j != done)
				{
				AddString(names[j]);
				SetItemDataPtr(1, (void*)styles[j]);
				done2nd = j;
				j = 4;
				test = true;
				}
			}

		//default - take first available selection.  This will get called in many cases when there
		//are only two available selections.
		if (!test)
			{
			if (done != 0)
				{
				AddString(names[0]);
				SetItemDataPtr(1, (void*)styles[0]);
				done2nd = 0;
				}
			else
				{
				AddString(names[1]);
				SetItemDataPtr(1, (void*)styles[1]);
				done2nd = 1;
				}
			}
		}
	j = 0;
	test = false;

	//for all comboboxes with three or more selections.
	if (i > 2)
		{
		for (j; j < i; j++)
			{

			//conditions: bold, non-italic
			if (styles[j]->weight >= 600 && styles[j]->italic == 0 && done != j && done2nd != j)
				{
				AddString(names[j]);
				SetItemDataPtr(2, (void*)styles[j]);
				donelast = j;
				j = 4;
				test = true;
				}
			}

		//failed!  Take first available selection.
		if (!test)
			{
			if (done != 0 && done2nd != 0)
				{
				AddString(names[0]);
				SetItemDataPtr(2, (void*)styles[0]);
				donelast = 0;
				}
			else if (done != 1 && done2nd != 1)
				{
				AddString(names[1]);
				SetItemDataPtr(2, (void*)styles[1]);
				donelast = 1;
				}
			else
				{
				AddString(names[2]);
				SetItemDataPtr(2, (void*)styles[2]);
				donelast = 2;
				}
			}
		}
	j = 0;
	test = false;

	//for comboboxes with 4 selections, insert the remaining data.
	if (i > 3)
		{
		int check = done + done2nd + donelast;
		if (check == 6)
			{
			AddString(names[0]);
			SetItemDataPtr(3, (void*)styles[0]);
			}
		else if (check == 5)
			{
			AddString(names[1]);
			SetItemDataPtr(3, (void*)styles[1]);
			}
		else if (check == 4)
			{
			AddString(names[2]);
			SetItemDataPtr(3, (void*)styles[2]);
			}
		else
			{
			AddString(names[3]);
			SetItemDataPtr(3, (void*)styles[3]);
			}
		}

// set to the current style or else to Regular or else to the first style
// TO DO - redo this to find the closest match by weight/italic

	if (CB_ERR == SetCurSel(0))
		EnableWindow(FALSE);

//	int index = FindStringExact(-1, styleName);
//	if (CB_ERR == index)
//		{
//		;	// to do - may have to find out if the current style's font is in the list; if it isn't, we probably
			//         have to add it so that it can be displayed and selected - preview should say "Font not found"
//		if (CB_ERR == SetCurSel(0))		// remove this when the above is done
//			EnableWindow(FALSE);
//		}
//	else
//		{
//		if (CB_ERR == SetCurSel(index))
//			EnableWindow(FALSE);
//		}
	}

/******************************************************************************
*+ CIcadFontStyleComboBox::EnumFontFamExProcStyle
*
*/

int CALLBACK CIcadFontStyleComboBox::EnumFontFamExProcStyle(
	ENUMLOGFONTEX *enumLogFontEx,
	NEWTEXTMETRICEX *newTextMetricEx,
	int fontType,
	LPARAM lParam)
	{
	if (TRUETYPE_FONTTYPE & fontType)
		{
		((CIcadFontStyleComboBox*)lParam)->AddStyle((LPCTSTR) enumLogFontEx->elfStyle,
													(LPCTSTR) enumLogFontEx->elfFullName,
													enumLogFontEx->elfLogFont.lfWeight,
													enumLogFontEx->elfLogFont.lfItalic,
													enumLogFontEx->elfLogFont.lfPitchAndFamily,
													enumLogFontEx->elfLogFont.lfCharSet); // EBATECH(CNBR)
		}

	return 1;
	}

/******************************************************************************
*+ CIcadFontStyleComboBox::AddStyle
*
*/

int CIcadFontStyleComboBox::AddStyle(
	const char *styleName,		// i - always used
	const char *fullName,		// i - always used		to do - make sure this fits in LF_FACESIZE, including the NULL terminator
	const int weight,			// i - always used
	const int italic,			// i - always used
	const int pitchAndFamily,	// i - always used
	const int charSet)			// i - always used
	{
	int index;
	StyleItem *styleItem = NULL;

	ASSERT(styleName && fullName);

	// make sure the style isn't already in the combobox;
	// we want each style listed only once
	if (CB_ERR != FindStringExact(-1, (LPCTSTR) styleName))
		return CB_ERR;

	// set up the styleItem
	styleItem = new StyleItem;
	ASSERT(styleItem);
	if (!styleItem)
		return CB_ERR;

	ASSERT(strlen(styleName) < LF_FACESIZE);
	strncpy(styleItem->styleName, styleName, LF_FACESIZE - 1);	// this has to be no more than LF_FACESIZE, including the NULL
	styleItem->styleName[LF_FACESIZE - 1] = '\0'/*DNT*/;

	ASSERT(strlen(fullName) < LF_FULLFACESIZE);
	strncpy(styleItem->fullName, fullName, LF_FULLFACESIZE - 1);	// this has to be no more than LF_FACESIZE, including the NULL
	styleItem->fullName[LF_FULLFACESIZE - 1] = '\0'/*DNT*/;

	styleItem->weight			= weight;
	styleItem->italic			= italic;
	styleItem->pitchAndFamily	= pitchAndFamily;
	styleItem->charSet			= charSet;		// EBATECH(CNBR)

	// insert the style name and set the styleItem
	index = AddString((LPCTSTR) styleItem->styleName);
	if (index < 0)	// failed to add
		{
		delete styleItem;
		return CB_ERR;
		}
	else			// successfully added
		{
		if (CB_ERR == SetItemDataPtr(index, (void *) styleItem))
			{
			DeleteString(index);	// remove the style name if we can't store its data
			delete styleItem;
			return CB_ERR;
			}
		}

	return index;
	}




BEGIN_MESSAGE_MAP(CIcadFontStyleComboBox, CComboBox)
	//{{AFX_MSG_MAP(CIcadFontStyleComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadFontStyleComboBox message handlers

void CIcadFontStyleComboBox::OnSelChange()
{
	// update preview and other controls
	GetParent()->SendMessage(WM_FONT_STYLE_CHANGED);
}

void CIcadFontStyleComboBox::OnDestroy()
{
	DeleteStyleItems();
	ResetContent();
	CComboBox::OnDestroy();
}
