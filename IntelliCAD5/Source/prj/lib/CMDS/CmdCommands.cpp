// File  :
// Author: Alexey Malov
#pragma warning (disable : 4786)
#include "afx.h" //CObject
#include "tchar.h" //TCHAR
#include "CmdQueryTools.h"
#include "CmdCommand.h"
#include "CmdOut.h"
#include "lispvars.h"//comamndAtomList
#include "commandatom.h"
#include <vector>
#include <algorithm>
#include <set>
using namespace std;

//Class implements command "COMMANDS"
//"COMMANDS" displays Icad commands in 3 modes:
//	- external commands (registered by extensions)
//	- internal commands
//	- both
class CCmdCommands: public CCmdCommand
{
	enum
	{
		eInternal,
		eExternal,
		eBoth,
		eNCount
	};
	TCmdQuerySwitch<eNCount> m_getOption;
public:
	CCmdCommands();
	virtual int run();
};

CCmdCommands::CCmdCommands()
{
	m_pStart = &m_getOption;
	m_getOption.init(ResourceString(IDC_COMMANDS_PROMPT, "List commands:"),
					 ResourceString(IDC_COMMANDS_VARIANTS, "Internal External Both"));
}

//Class is used for index sorting of strings
class CIndexedSort
{
	TCHAR** m_s1;
	TCHAR** m_s2;
public:
	CIndexedSort(TCHAR** s1, TCHAR** s2)
		:m_s1(s1), m_s2(s2)
	{}

	bool operator () (int i1, int i2) const
	{
		if (_tcscmp(m_s1[i1], m_s1[i2]) < 0)
			return true;
		else
			return false;
	}
};

int CCmdCommands::run()
{
	if (CCmdCommand::run())
	{
		int opt = m_getOption.getSelection();

		CCmdOut out;
		if (opt == eExternal)
			out << ResourceString(IDC_COMMANDS_TITLE_EXTERNAL, "\n\nIntelliCAD external commands:\n");
		else if (opt == eInternal)
			out << ResourceString(IDC_COMMANDS_TITLE_INTERNAL, "\n\nInteliCAD internal commands:\n");
		else
			out << ResourceString(IDC_COMMANDS_TITLE_BOTH, "\n\nIntelliCAD internal and external commands:\n");

		vector<TCHAR*> asGlobalNames;
		vector<TCHAR*> asLocalNames;
		vector<int> aiIndices;
		set<commandAtomObj*> atoms;
		int i = 0;
		CString sKey;
		POSITION p = commandAtomList->GetStartPosition();
		while (p)
		{
			commandAtomObj* pAtom;
			commandAtomList->GetNextAssoc(p, sKey, (CObject*&)pAtom);

			if (atoms.insert(pAtom).second)
			{
				if (pAtom->GName && pAtom->LName)
				{
					if (((opt == eExternal) && (pAtom->id <= 0)) ||
						((opt == eInternal) && (pAtom->id > 0)))
						continue;

					asGlobalNames.push_back(pAtom->GName);
					asLocalNames.push_back(pAtom->LName);
					aiIndices.push_back(i++);
				}
			}
		}

		int n = aiIndices.size();
		if (n > 0)
		{
			CIndexedSort indexedSort(asGlobalNames.begin(), asLocalNames.begin());
			sort(aiIndices.begin(), aiIndices.end(), indexedSort);

			out.openPromptWindow();

			for (i = 0; i < n && out.good(); i++)
			{
				out << asGlobalNames[aiIndices[i]] << ", " << asLocalNames[aiIndices[i]] << endl;
			}
		}

		if (out.good())
			out << ResourceString(IDC_COMMANDS_END, "End of List.\n");
	}
	return m_rc;
}


/*-------------------------------------------------------------------------*//**
Implemntation of comand COMMANDS. It prints names of all registered commands.
@author Alexey Malov
@return RTNORM
*//*--------------------------------------------------------------------------*/
extern "C"
{
short cmd_commands()
{
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	CCmdCommands cmd;
	return cmd.run();
}
}//"C"

#pragma warning (default : 4786)
