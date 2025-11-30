#include "stdafx.h"
#include "menuitem.h"
#include "menupage.h"

CMenuPage::CMenuPage()
{
}

CMenuPage::~CMenuPage()
{
}

void CMenuPage::Set(short idx, CMenuItem *pItem)
{
	if (idx < 0 || idx > 19)
		return;
	m_items[idx] = *pItem;
}

CMenuItem * CMenuPage::Get(short idx)
{
	if (idx < 0 || idx > 19)
		return NULL;
	return &m_items[idx];
}

void CMenuPage::Reset()
{
	for (short idx = 0; idx < 19; idx++)
		m_items[idx].Reset();
}
