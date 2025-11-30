/* INTELLICAD\PRJ\ICAD\ICADVBAMACRO.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Intellicad VBA Macro Recorder class.  This provides the macro dialog functionality
 * as well as recording capabilities.  Source is similar to Summit's Scribble Macro
 * recorder example with a few changes.
 * 
 */ 


#ifndef __VBAMACRO_H__
#define __VBAMACRO_H__

#define Icad_E_NOTRECORDING 0x80040500L

template<class Owner>
class CIcadRecorderMember : public CComPtr<IApcRecorder>
{
	public:
		CIcadRecorderMember(Owner* pOwner) : m_pOwner(pOwner), m_macroCt(1)
		{
			ASSERT(m_pOwner);
		}

		IApcRecorder* GetApcRecorder() { return *this; }

		template <class ProjectOwner>
		HRESULT Create(ProjectOwner* pProjectOwner, const char* name)
		{
			ApcEntryHR(CIcadRecorderMember::Create);

			//
			// reset Dim dictionary
			//
			DimmedVar(NULL);

			//
			// start recording
			//
			CString txt;
			txt.Format("%s%d", name, m_macroCt++);

			ApcCallHR(GetApp()->ApcHost->APC_GET(Recorder)(&(*this)));
			ApcCallHR(GetApcRecorder()->APC_RAW(Start)(pProjectOwner->ApcProjectItem, CComBSTR(txt)));

			ApcReturnHR(S_OK);
		}

		template <class ProjectOwner>
		HRESULT Run(ProjectOwner* pProjectOwner, const char* title)
		{
			ApcEntryHR(CIcadRecorderMember::Run);

			CComPtr<IApcScopes> spScopes;
			//ApcCallHR(pProjectOwner->ApcProject->APC_RAW(CreateScopes)(VARIANT_TRUE, &spScopes));
			ApcCallHR(GetApp()->ApcHost->APC_RAW(CreateScopes)(VARIANT_TRUE, &spScopes));

			CComPtr<IApcMacrosDialog> spMacro;
			ApcCallHR(spScopes->APC_GET(MacrosDialog)(&spMacro));
			VERIFY(SUCCEEDED(spMacro->APC_PUT(DialogTitle)(CComBSTR(title))));
			VERIFY(SUCCEEDED(spMacro->APC_PUT(StartUpPosition)(axPositionCenterOwner)));
			VERIFY(SUCCEEDED(spMacro->APC_RAW(Show)()));
			//
			// IApcMacrosDialog::Show will complete in the 
			// ApcMacrosDialog_Create event if Create is selected
			//
			ApcReturnHR(S_OK);
		}

		HRESULT RunMacro (BSTR MacroName)
		{
			ApcEntryHR(CIcadRecorderMember::Run);

			CComPtr<IApcScopes> spScopes;
			ApcCallHR(GetApp()->ApcHost->APC_RAW(CreateScopes)(VARIANT_TRUE, &spScopes));

			VERIFY(SUCCEEDED(spScopes->APC_RAW(CallMacro)(MacroName)));

			ApcReturnHR(S_OK);
		}

		BOOL DimmedVar(const char* var)
		{
			if (var == NULL)
			{
				m_dimmed.RemoveAll();
			}
			else
			{
				for(int i = 0; i < m_dimmed.GetSize(); i++)
				{
					if (m_dimmed.GetAt(i) == var)
						return TRUE;
				}
				m_dimmed.Add(var);
			}
			return FALSE;
		}

	protected:
		CStringArray m_dimmed;
		Owner* m_pOwner;
		long m_macroCt;
};

class CIcadRecorder
{
	public:
		CIcadRecorderMember<CIcadRecorder> ApcRecorder;

		CIcadRecorder() : ApcRecorder(this)
		{
		}
};

/* to be used later, when we can actually record macros
template<class T>
class CIcadMacro 
{
	public:
		CIcadMacro(T* pT) : m_pOwner(pT)
		{
			ASSERT(m_pOwner);
			m_indent = 0;
		}

		void Indent(int levels = 1)
		{
			m_indent += levels;
			if (m_indent < 0)
			{
				ASSERT(0);
				m_indent = 0;
			}
		}

		void Unindent(int levels = 1)
		{
			m_indent -= levels;
			if (m_indent < 0)
			{
				ASSERT(0);
				m_indent = 0;
			}
		}

		BOOL Recording()
		{
			BOOL recording = FALSE;
			if (m_pOwner->ApcRecorder)
				m_pOwner->ApcRecorder->APC_GET(Recording)(ApcInOutVARIANT_BOOL(&recording));
			return recording;
		}

		void Dim(const char* var, const char* type)
		{
			if (m_pOwner->ApcRecorder.DimmedVar(var))
			{
				return;
			}

			CString txt;
			txt.Format("Dim %s As %s", var, type);
			RecordLine(txt);
		}

		HRESULT Record(const char* text)
		{
			ASSERT(text);
			if (m_pOwner->ApcRecorder)
			{
				CString txt = IndentText(text);
				return m_pOwner->ApcRecorder->APC_RAW(Record)(CComBSTR(txt));
			}
			else
				return Icad_E_NOTRECORDING;
		}
		HRESULT RecordLine(const char* text = 0)
		{
			if (m_pOwner->ApcRecorder)
			{
				if (text == 0)
					text = "";
				CString txt = IndentText(text);
				return m_pOwner->ApcRecorder->APC_RAW(RecordLine)(CComBSTR(txt));
			}
			else
				return Icad_E_NOTRECORDING;
		}

	protected:
		CString IndentText(const char* text)
		{
			ASSERT(text);
			CString ret;
			for(int i = 0; i < m_indent; i++)
				ret += "\t";
			ret += text;
			return ret;
		}

		T* m_pOwner;
		int m_indent;
};
*/

#endif



