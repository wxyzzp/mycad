/* PRJ\ICAD\TEXTSTYLETABLEINFO.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * $Revision: 1.2.6.1 $ $Date: 2003/05/15 20:17:35 $
 *
 * Abstract
 *
 * Support class for dealing with text styles.
 *
 */

#if !defined(_TEXTSTYLETABLEINFO_H)
#define _TEXTSTYLETABLEINFO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "icad.h"	/*DNT*/


CDC *GetIcadTargetDeviceCDC(void);


class db_drawing;

class CTextStyleTableInfo
	{
public:
	// Construction/Destruction
	CTextStyleTableInfo();
	virtual ~CTextStyleTableInfo();

	// Attributes
	inline db_drawing * GetDrawing(void);
	inline void			SetDrawing(db_drawing *drawing);

	inline sds_real		GetHeight(void);
	inline void			SetHeight(const sds_real value);

	inline sds_real		GetWidth(void);
	inline void			SetWidth(const sds_real value);

	inline sds_real		GetObliqueAngle(void);
	inline void			SetObliqueAngle(const sds_real value);

	inline BOOL			GetBackwards(void);
	inline void			SetBackwards(BOOL value);

	inline BOOL			GetUpsideDown(void);
	inline void			SetUpsideDown(BOOL value);

	inline BOOL			GetVertically(void);
	inline void			SetVertically(BOOL value);

	inline CString&		GetStyleName(void);
	inline void			SetStyleName(const CString& value);

	inline CString		GetFontFilename(const bool getFullPath);
	inline void			SetFontFilename(const CString& value);

	inline CString		GetBigFontFilename(const bool getFullPath); /*CNBR*/
	inline void			SetBigFontFilename(const CString& value);	/*CNBR*/

	inline CString&		GetFontFamily(void);
	inline void			SetFontFamily(const CString& value);

	inline long			GetFontStyle(void);
	inline void			SetFontStyle(const long value);

	inline bool			GetHasTrueTypeFont(void);
	inline void			SetHasTrueTypeFont(const bool value);

private:
	// Data members
	db_drawing *m_drawing;

	sds_real	m_height;
	sds_real	m_width;
	sds_real	m_obliqueAngle;

	BOOL		m_backwards;
	BOOL		m_upsideDown;
	BOOL		m_vertically;

	CString		m_styleName;
	CString		m_fontFilename;
	CString		m_fontFamily;
	CString		m_bigfontFilename;	// CNBR

	long		m_fontStyle;

	bool		m_hasTrueTypeFont;
	};


/////////////////////////////////// INLINE FUNCTIONS ///////////////////////////////////
//

inline db_drawing * CTextStyleTableInfo::GetDrawing(
	void)
	{
	return m_drawing;
	}

inline void CTextStyleTableInfo::SetDrawing(
	db_drawing *drawing)
	{
	m_drawing = drawing;
	}

inline sds_real	CTextStyleTableInfo::GetHeight(
	void)
	{
	return m_height;
	}

inline void CTextStyleTableInfo::SetHeight(
	const sds_real value)
	{
	m_height = value;
	}

inline sds_real	CTextStyleTableInfo::GetWidth(
	void)
	{
	return m_width;
	}

inline void CTextStyleTableInfo::SetWidth(
	const sds_real value)
	{
	m_width = value;
	}

inline sds_real	CTextStyleTableInfo::GetObliqueAngle(
	void)
	{
	return m_obliqueAngle;
	}

inline void CTextStyleTableInfo::SetObliqueAngle(
	const sds_real value)
	{
	m_obliqueAngle = value;
	}

inline BOOL CTextStyleTableInfo::GetBackwards(
	void)
	{
	return m_backwards;
	}

inline void CTextStyleTableInfo::SetBackwards(
	BOOL value)
	{
	m_backwards = value;
	}

inline BOOL CTextStyleTableInfo::GetUpsideDown(
	void)
	{
	return m_upsideDown;
	}

inline void CTextStyleTableInfo::SetUpsideDown(
	BOOL value)
	{
	m_upsideDown = value;
	}

inline BOOL CTextStyleTableInfo::GetVertically(
	void)
	{
	// 2003-5-5 Bugzilla#78571 Ebatech(CNBR) Reset verticaly flag when user choice truetype font.
	return ( m_hasTrueTypeFont == true ? false :  m_vertically );
	//return m_vertically;
	}

inline void CTextStyleTableInfo::SetVertically(
	BOOL value)
	{
	m_vertically = value;
	}

inline CString&	CTextStyleTableInfo::GetStyleName(
	void)
	{
	return m_styleName;
	}

inline void CTextStyleTableInfo::SetStyleName(
	const CString& value)
	{
	m_styleName = value;
	}

inline CString CTextStyleTableInfo::GetFontFilename(
	const bool getFullPath)
	{
	if (getFullPath)
		return m_fontFilename;
	else
		{
		return (strrchr(m_fontFilename, '\\')) ? strrchr(m_fontFilename, '\\') + 1 : m_fontFilename;
		}
	}

inline void CTextStyleTableInfo::SetFontFilename(
	const CString& value)
	{
	m_fontFilename = value;
	}


// EBATECH(CNBR) -[ Add Bigfont stuff
inline void CTextStyleTableInfo::SetBigFontFilename(
	const CString& value)
	{
	m_bigfontFilename = value;
	}

inline CString CTextStyleTableInfo::GetBigFontFilename(
	const bool getFullPath)
	{
	if (getFullPath)
		return m_bigfontFilename;
	else
		{
		return (strrchr(m_bigfontFilename, '\\')) ? strrchr(m_bigfontFilename, '\\') + 1 : m_bigfontFilename;
		}
	}
// EBATECH(CNBR) ]-

inline CString&	CTextStyleTableInfo::GetFontFamily(
	void)
	{
	return m_fontFamily;
	}

inline void CTextStyleTableInfo::SetFontFamily(
	const CString& value)
	{
	m_fontFamily = value;
	}

inline long CTextStyleTableInfo::GetFontStyle(
	void)
	{
	return m_fontStyle;
	}

inline void CTextStyleTableInfo::SetFontStyle(
	const long value)
	{
	m_fontStyle = value;
	}

inline bool CTextStyleTableInfo::GetHasTrueTypeFont(
	void)
	{
	return m_hasTrueTypeFont;
	}

inline void CTextStyleTableInfo::SetHasTrueTypeFont(
	const bool value)
	{
	m_hasTrueTypeFont = value;
	}


#endif // !defined(_TEXTSTYLETABLEINFO_H)



