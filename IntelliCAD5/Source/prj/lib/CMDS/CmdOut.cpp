// File  :
// Author: Alexey Malov
#include "cmds.h"
#include "CmdOut.h"/*DNT*/
#include "sds.h"/*DNT*/ //resbuf
#undef T
#undef X
#undef Y
#undef Z
#include "Icad.h"/*DNT*/
#include "commandqueue.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/
using std::string;
#include <utility>
using std::_cpp_min;
#include "gvector.h"
using namespace icl;

CCmdOut::CCmdOut(bool bScroll)
:
m_bScroll(bScroll),
m_iCurrPos(0),
m_state(eGood)
{
	resbuf rb;
	SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	m_nMaxTrackedHistory = rb.resval.rint;
}

bool CCmdOut::isInteractive()
{
	return !GetActiveCommandQueue()->IsNotEmpty() && 
		   !GetMenuQueue()->IsNotEmpty();
}

void CCmdOut::flush()
{
	if (!isInteractive())
		return;
	if (m_bScroll)
		return;

	m_nStringsPerPage = _cpp_min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->
							  m_pPromptWndList->m_nLinesPerPage, m_nMaxTrackedHistory);

	if (m_iCurrPos >= m_nStringsPerPage)
	{
		char tmp[IC_ACADNMLEN];
		sds_initget(0, 0);
		if (RTNORM != sds_getstring(0, ResourceString(IDC_CMDS9__NPRESS_ENTER_TO_440, "\nPress ENTER to continue..." ), tmp))
			m_state |= eBad;
		
		m_iCurrPos = 0;
	}
}

void CCmdOut::openPromptWindow()
{
	m_bScroll = false;
	sds_textscr();
}

void CCmdOut::closePromptWindow()
{
	m_bScroll = true;
	sds_graphscr();
}

CCmdOut& operator << 
(
CCmdOut& o,
const string& s
)
{
	o.flush();
	sds_printf("  %s", s.c_str());

	return o;
}

CCmdOut& operator << 
(
CCmdOut& o,
const char* sz
)
{
	o.flush();
	sds_printf("  %s", sz);

	return o;
}

