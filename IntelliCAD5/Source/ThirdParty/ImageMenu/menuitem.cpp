#include "stdafx.h"
#include "sds.h"
#include "menuitem.h"

CMenuItem::CMenuItem()
{
	strcpy(m_sSlide, "");
	strcpy(m_sItem, "");
	strcpy(m_sCmd, "");
	m_ShwSldFlg = FALSE;
	m_AddLstFlg = FALSE;
}

CMenuItem::~CMenuItem()
{
}

CMenuItem &CMenuItem::operator=(const CMenuItem &other)
{
	if (&other == this)
		return *this;
	m_ShwSldFlg = other.m_ShwSldFlg;
	m_AddLstFlg = other.m_AddLstFlg;
	strcpy(m_sSlide, other.m_sSlide);
	strcpy(m_sItem, other.m_sItem);
	strcpy(m_sCmd, other.m_sCmd);
	return *this;
}

void CMenuItem::Parse(const char *psLine)
{
	// Start with [
	if (psLine[0] != '[')
		return;
	// Contains a ]
	if (!strchr(psLine, ']'))
		return;

	char sBuffer[256];
	char sPar[100] = "";
	char *psTemp;

	// Strip preceding [
	strcpy(sBuffer, psLine + 1);

	// Pull out Cmd portion and remove trailing ]
	psTemp = strchr(sBuffer, ']');
	strcpy(m_sCmd, psTemp + 1);
	m_sCmd[strlen(m_sCmd) - 1] = '\0'; // remove return/line feed
	short slen = strlen(sBuffer);
	sBuffer[slen - strlen(psTemp)] = '\0';
	slen = strlen(sBuffer);

	// Pull out parenthesis
	psTemp = strchr(sBuffer, '(');
	if (psTemp) {
		strcpy(sPar, psTemp);
		sBuffer[slen - strlen(psTemp)] = '\0';
	}

	if (sBuffer[0] == ' ') { // List Item only
		m_ShwSldFlg = FALSE;
		m_AddLstFlg = TRUE;
		strcpy(m_sSlide, "");
		strcpy(m_sItem, sBuffer + 1);
		return;
	}
	if (!strlen(sPar)) {
		psTemp = strchr(sBuffer, ',');
		if (psTemp) { // Slide & List
			strcpy(m_sSlide, sBuffer);
			m_sSlide[strlen(sBuffer) - strlen(psTemp)] = '\0';
			m_ShwSldFlg = TRUE;
			strcpy(m_sItem, psTemp + 1);
			m_AddLstFlg = TRUE;
		} else { // Slide Only
			strcpy(m_sSlide, sBuffer);
		}
	} else {
		psTemp = strchr(sPar, ',');
		if (psTemp) { // Pull out List
			strcpy(m_sItem, psTemp + 1);
			m_sItem[strlen(m_sItem) - 1] = '\0';
			m_AddLstFlg = TRUE;
			strcpy(psTemp, ")");
		} else {
			strcpy(m_sItem, sPar + 1);
			m_sItem[strlen(m_sItem) - 1] = '\0';
			m_AddLstFlg = TRUE;
		}
		strcpy(m_sSlide, sBuffer);
		strcat(m_sSlide, sPar);
		m_ShwSldFlg = TRUE;
	}
}

void CMenuItem::Reset()
{
	strcpy(m_sSlide, "");
	strcpy(m_sItem, "");
	strcpy(m_sCmd, "");
	m_ShwSldFlg = FALSE;
	m_AddLstFlg = FALSE;
}

char * CMenuItem::Item()
{
	if (m_AddLstFlg)
		return m_sItem;
	return NULL;
}

char * CMenuItem::Slide()
{
	if (m_ShwSldFlg)
		return m_sSlide;
	return NULL;
}

char * CMenuItem::Cmd()
{
	return m_sCmd;
}
