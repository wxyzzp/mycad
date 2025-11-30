#ifndef __DoFontH__
#define __DoFontH__

class db_entity;
class db_drawing;
class CDC;

GR_CLASS BYTE GetDWGCharSet(db_drawing* pDrw);

class GR_CLASS CDoFont: public CFont
{
public:
	enum EFontModifiers
	{
		eNONE		= 0x0000,
		eUNDERLINED	= 0x0001,
		eBOLD		= 0x0002,
		eITALIC		= 0x0004,
	};

/********************************************************************************
 * Author:	Dmitry Gavrilov
 * Purpose:	Allows creation of objects across DLL boundaries without defeating
 *			DELAYLOAD (DELAYLOAD can't be used if vtable is linked).
 * Returns:	Created TT font or NULL in failure.
 ********************************************************************************/
	static CDoFont * Factory(db_entity*  pTextEntity,
								 db_drawing* pDrw,
								 CDC*        pDC);

	CDoFont();
	~CDoFont();
	CDoFont* backup() { return m_pBackup; }
	bool createBackup(UINT code, CDC* pDC);
private:
	CDoFont* m_pBackup;
/********************************************************************************
 * Author:	Dmitry Gavrilov
 * Purpose:	Create a TrueType font.
 * Returns:	True in success and false in failure.
 ********************************************************************************/
	bool createTrueTypeFont(db_entity*	pTextEntity,
							db_drawing*	pDrw,
							CDC*		pDC);
};

#endif
