// File  :
// Author: Alexey Malov
#include "IcadView.h"
#include <algorithm>
using std::find;
#include <stack>
using std::stack;
#include <vector>
using std::vector;
#include "cmds.h" //getInternalpoint()
#include "transf.h"

// Class TCmdQuerySwitch

template <degree N>
TCmdQuerySwitch<N>::TCmdQuerySwitch()
:
CCmdQueryEx(),
TCmdQueryLink<N>(),
m_default(0),
m_selection(-1)
{}

template <degree N>
void TCmdQuerySwitch<N>::init
(
const char* szPrompt,
const char* szKeys,
#if 0
std::string asKeys[N],
#endif
int accept,
int _default
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);

	m_default = _default;

	//recieve separate keys (m_asKeys) from one string (zKeys)
	int i = 0;
	for (const char* c = szKeys; *c != '\x0'; ++i)
	{
		for (; *c != '\x0'; ++c)
		{
			if (*c != ' ')
				break;
		}
		
		if (*c == '\x0')
			break;

		for (;*c != '\x0' && *c != ' '; ++c)
		{
			m_asKeys[i] += *c;
		}
	}

	//form prompt string from header and keys
    for (i = 0; i < N; ++i)
	{
		char delimeter = i == 0? ' ': '/';
		m_sPrompt += delimeter;

		bool bDefault = !(m_accept & RSG_NONULL) && i == m_default;

		if (bDefault)
			m_sPrompt += '<';
		
		m_sPrompt += m_asKeys[i];

		if (bDefault)
			m_sPrompt += '>';
	}
	m_sPrompt += ':';
	m_sPrompt += ' ';
}

template <degree N>
CCmdQuery* TCmdQuerySwitch<N>::query() throw (std::exception)
{
    char answer[IC_ACADBUF];
	m_selection = -1;
#if 1
    do
    {
	    m_rc = sds_initget(m_accept, m_sKeys.c_str());
        if (m_rc != RTNORM)
            throw std::exception();

        m_rc = sds_getkword(m_sPrompt.c_str(), answer);
        if (m_rc == RTERROR)
            continue;
		
		if (!(m_accept & RSG_NONULL) && (m_rc == RTNONE))
			return next(m_default);

	    if (m_rc != RTNORM)
            throw std::exception();
    }
    while (m_rc != RTNORM);
#else
    do
    {
        db::point tmp;
        m_rc = internalGetpoint(0, m_sPrompt.c_str(), tmp);
	    if (m_rc == RTCAN ||
            m_rc == RTERROR ||
            m_rc == RTFAIL)
            throw std::exception();
    }
    while (m_rc != RTKWORD);

    sds_getinput(answer);
//	SDS_SetCursor(IDC_ICAD_CROSS);
#endif

    for (int i = 0; i < N; i++)
    {
        if (strisame(m_asKeys[i].c_str(), answer))
            break;
    }
	m_selection = i;

    return next(i);
}

template <degree N>
int TCmdQuerySwitch<N>::getDefault() const
{
	return m_default;
}

template <degree N>
void TCmdQuerySwitch<N>::setDefault(int _def)
{
	m_default = _def;
}

template <degree N>
int TCmdQuerySwitch<N>::getSelection() const
{
	return m_selection;
}

// Class TSetWithUndo

template <typename type>
const vector<type>& TSetWithUndo<type>::set()
{
    return m_objects;
}

template <typename type>
int TSetWithUndo<type>::add(const type& t)
{
    assert(m_mode == eAdd);
    
    if (std::find(m_objects.begin(), m_objects.end(), t) == m_objects.end())
    {
        m_objects.push_back(t);
        return 1;
    }
    else
    {
        return 0;
    }
}

template <typename type>
int TSetWithUndo<type>::add
(
std::vector<type>::iterator first,
std::vector<type>::iterator last,
int n
)
{
    assert(m_mode == eAdd);
    assert(n > 0);

    for (; n > 0 && first != last; ++first)
    {
        add(*first) && (--n);
    }
    return 1;
}

template <typename type>
type TSetWithUndo<type>::added(int i)
{
    return m_objects.at(m_objects.size()-i-1);
}

template <typename type>
int TSetWithUndo<type>::remove(const type& t)
{
    assert(m_mode == eRemove);

    iterator i = std::find(m_objects.begin(), m_objects.end(), t);
    if (i != m_objects.end())
    {
        m_removed.push_back(*i);
        m_objects.erase(i);

        return 1;
    }
    return 0;
}

template <typename type>
int TSetWithUndo<type>::remove
(
std::vector<type>::iterator first,
std::vector<type>::iterator last,
int n
)
{
    assert(m_mode == eRemove);
    assert(n > 0);

    for (; n > 0 && first != last; ++first)
    {
		remove(*first) && (--n);
    }

    return 1;
}

template <typename type>
type TSetWithUndo<type>::removed(int i)
{
    return m_removed.at(m_removed.size()-i-1);
}

template <typename type>
void TSetWithUndo<type>::clear()
{
    m_objects.clear();
    m_removed.clear();
}

template <typename type>
type TSetWithUndo<type>::undo()
{
    if (m_mode == eAdd)
    {
        if (!m_objects.empty())
        {
            type u = m_objects.back();
            m_objects.pop_back();
            return u;
        }
    }
    else if (m_mode == eRemove)
    {
        if (!m_removed.empty())
        {
            type u = m_removed.back();
            m_objects.push_back(u);
            m_removed.pop_back();
            return u;
        }
    }
    return 0;
}

template <typename type>
void TSetWithUndo<type>::mode(int i)
{
    assert(i >= 0 && i < eCount);
    m_mode = i;
}

// Class TCmdQuerySubents
template <typename handle>
TCmdQuerySubents<handle>::TCmdQuerySubents()
:
CCmdQueryEx(),
m_dbitem(0),
m_mode(eAdd)
{
    m_asKeys[0].assign(ResourceString(IDC_QUERYTOOLS_REMOVE, "Remove"));
    m_asKeys[1].assign(ResourceString(IDC_QUERYTOOLS_ADD, "Add"));
    m_asKeys[2].assign(ResourceString(IDC_QUERYTOOLS_UNDO, "Undo"));
    m_asKeys[3].assign(ResourceString(IDC_QUERYTOOLS_ALL, "ALL"));
        
    m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDOALL, "Remove Undo ALL"));

    m_ename[0] = m_ename[1] = 0;
}

template <typename handle>
void TCmdQuerySubents<handle>::init(bool bRemoving)
{
    m_last.clear();
    m_objects.clear();
	icl::string s;
	m_obj.setSat(s);
	m_ename[0] = m_ename[1] = 0;
	m_dbitem = 0;

	if (bRemoving)
		remove();
	else
		add();
}

template <typename handle>
int TCmdQuerySubents<handle>::select(bool bAll) throw (exception)
{
	updatePromptAndKeys();
    m_rc = sds_initget(0, m_sKeys.c_str());
    if (m_rc != RTNORM)
        throw exception();

    {
        SDS_SetCursor(SDS_GetPickCursor());
        m_rc = SDS_AskForPoint(0, m_sPrompt.c_str(), m_pt);

        icl::transf t;
        if (!ucs2wcs(t))
            throw exception();

        m_pt *= t;
    }

    switch (m_rc)
    {
    case RTCAN:
        // command canceled
		unhighlight(0);
		regen();
        throw exception();
    case RTERROR:
        // error 
        m_rc = RTPICKS;  // set to RTPICKS to ask users to select again
        return 0;
    case RTNONE:
        // input finished
		selectNone();
        return 1;
    case RTKWORD:
        // key word selected
        return parse();
    case RTNORM:
        // point picked
		{
			sds_name ss, ename;
			ads_point point;
			point[0] = m_pt.x(); point[1] = m_pt.y(); point[2] = m_pt.z(); 
				
		    resbuf filter;
		    filter.restype = 0;
		    filter.resval.rstring = getFillet();
		    filter.rbnext = 0;
			if ((m_rc = sds_ssget(NULL, point, NULL, &filter, ss)) != RTNORM) {
                if (m_rc == RTERROR)
                    m_rc = RTVOID; 
				return 0;
            }
			m_rc = sds_ssname(ss, 0, ename);
			sds_ssfree(ss);
			if (m_rc != RTNORM)
                throw exception();

			CDbAcisEntity* pAcisEnt = reinterpret_cast<CDbAcisEntity*>(ename[0]);
			if (m_dbitem == NULL) {
				m_dbitem = pAcisEnt;
				m_obj = m_dbitem->getData();
				m_ename[0] = ename[0];
				m_ename[1] = ename[1];
			}
			else {
				if (pAcisEnt != m_dbitem) {
					cmd_ErrorPrompt(selectOnWrongEntityErrorIndex(), 0);
                    m_rc = RTPICKS; // set to RTPICKS to ask users to select again;
					return 0; 
				}
			}
			return pick(bAll);
		}
    }
    throw exception();
}

template <typename handle>
int TCmdQuerySubents<handle>::pick(bool bAll) throw (exception)
{
    icl::gvector dir;
    {
        resbuf rb;
        sds_getvar("VIEWDIR"/*DNT*/, &rb);
        dir = rb.resval.rpoint;

        icl::transf mtx;
        if (!ucs2wcs(mtx))
            throw exception();

        dir *= mtx;
    }

    int pickbox;
    {
        resbuf rb;
		sds_getvar("PICKBOX"/*DNT*/, &rb);
		if ((pickbox = rb.resval.rint) == 0) 
            pickbox = 5;
    }

    gr_view* pView = SDS_CURGRVW;
    double prec = fabs(pView->GetPixelHeight())*pickbox*2;

    m_last.resize(0);
    if (pickSubent(dir, prec))
    {
        update(bAll);
		return 1;
    }

    return 0;
}

template <typename handle>
int TCmdQuerySubents<handle>::update(bool bAll) throw (exception)
{
    if (m_last.empty())
        return 1;

    int nObj = bAll? m_last.size(): 1;

    // add/remove objects to/from object set
    if (m_mode == eAdd)
    {
        if (!m_objects.add(m_last.begin(), m_last.end(), nObj))
            throw exception();

        if (bAll) {
            for (std::vector<handle>::iterator first = m_last.begin(); first != m_last.end(); ++first)
                highlight(*first);
        }
        else
            highlight(m_objects.added(0));
    }
    else 
    {
        if (!m_objects.remove(m_last.begin(), m_last.end(), nObj))
            throw exception();

        if (bAll) {
            for (std::vector<handle>::iterator first = m_last.begin(); first != m_last.end(); ++first)
                unhighlight(*first);
        }
        else
            unhighlight(m_objects.removed(0));
    }

    // regen object
    regen();

    return 1;
}

template <typename handle>
int TCmdQuerySubents<handle>::highlight(handle hobj) throw (exception)
{
    if (!CModeler::get()->highlight(m_obj, hobj))
    {
        m_objects.undo();
        throw exception();
    }
    return 1;
}

template <typename handle>
int TCmdQuerySubents<handle>::unhighlight(handle hobj) throw (exception)
{
    if (!CModeler::get()->unhighlight(m_obj, hobj))
    {
        m_objects.undo();
        throw exception();
    }
    return 1;
}



template <typename handle>
int TCmdQuerySubents<handle>::regen()
{
    gr_displayobject* dobj = reinterpret_cast<gr_displayobject*>(m_dbitem->ret_disp());
    gr_freedisplayobjectll(dobj);
    m_dbitem->set_disp(0);

    sds_redraw(m_ename, IC_REDRAW_DRAW);

    return 1;
}

template <typename handle>
int TCmdQuerySubents<handle>::parse() throw (exception)
{
    char input[512];
    m_rc = sds_getinput(input);
    if (m_rc != RTNORM)
        throw exception();

    for (int i = 0; i < eNMode; i++)
    {
        if (!strcmp(input, m_asKeys[i].c_str()))
            break;
    }
    if (i == eNMode)
        throw exception();

    switch (i)
    {
    case eRemove:
		remove();
        break;
    case eAdd:
		add();
        break;
    case eUndo:
		undo();
        break;
    case eAll:
		all();
        break;
    }

    return 1;
}

template <typename handle>
void TCmdQuerySubents<handle>::initEntity()
{
	m_dbitem = NULL;
	m_ename[0] = m_ename[1] = 0L;
}


template <typename handle>
void TCmdQuerySubents<handle>::add()
{
    m_mode = eAdd;
    m_objects.mode(TSetWithUndo<handle>::eAdd);
}

template <typename handle>
void TCmdQuerySubents<handle>::remove()
{
    m_mode = eRemove;
    m_objects.mode(TSetWithUndo<handle>::eRemove);
}

template <typename handle>
void TCmdQuerySubents<handle>::undo()
{
    handle h = m_objects.undo();
    if (h)
    {
        if (m_mode == eAdd)
            unhighlight(h);
        else if (m_mode == eRemove)
            highlight(h);

        regen();
    }
}

template <typename handle>
void TCmdQuerySubents<handle>::all()
{
    m_last.resize(0);
    if (getAllSubents())
    {
		update(1);
    }
}

template <typename handle>
CCmdQuery* TCmdQuerySubents<handle>::query() throw (exception)
{
    initEntity();

	if (m_mode == eRemove)
	{
		// add(); all(); 
		remove();
	}

    while (true)
    {
        select();

        if (m_rc == RTNONE)
            return next();
    }
}

template <typename handle>
bool TCmdQuerySubents<handle>::setAcisEntity(sds_name ename)
{
	m_dbitem = (CDbAcisEntity*)ename[0];
	if (m_dbitem) {
		m_obj = m_dbitem->getData();
		m_ename[0] = ename[0]; 
		m_ename[1] = ename[1];
		return true;
	}
	else 
		return false;
}

template <typename handle>
const CDbAcisEntityData& TCmdQuerySubents<handle>::getAcisEntityData()
{
    return m_obj;
}

template <typename handle>
const vector<handle>& TCmdQuerySubents<handle>::getObjects()
{
    return m_objects.set();
}

template <typename handle>
CDbAcisEntity* TCmdQuerySubents<handle>::getEntity()
{
    return m_dbitem;
}

template <typename handle>
void TCmdQuerySubents<handle>::getEntityName(sds_name ename)
{
    ename[0] = m_ename[0];
    ename[1] = m_ename[1];
}

template <int N>
void TCmdQueryPointOrKeyWord<N>::init
(
const char* szPrompt,
const char* szKeys,
int accept
)
{
    CCmdQueryEx::init(szPrompt, szKeys, accept);

	//recieve separate keys (m_asKeys) from one string (zKeys)
	int i = 0;
	for (const char* c = szKeys; *c != '\x0'; ++i)
	{
		for (; *c != '\x0'; ++c)
		{
			if (*c != ' ')
				break;
		}
		
		if (*c == '\x0')
			break;

		for (;*c != '\x0' && *c != ' '; ++c)
		{
			m_asKeys[i] += *c;
		}
	}
	assert(i == N-1);

	//form prompt string from header and keys
    for (i = 0; i < N-1; ++i)
	{
		char delimeter = i == 0? ' ': '/';
		m_sPrompt += delimeter;

		m_sPrompt += m_asKeys[i];
	}
	m_sPrompt += ':';
	m_sPrompt += ' ';
}

template <int N>
CCmdQuery* TCmdQueryPointOrKeyWord<N>::query() throw (exception)
{
	m_rc = sds_initget(m_accept|RSG_NONULL, m_sKeys.c_str());
	if (m_rc != RTNORM)
		throw exception();

	m_selection = N-1;

	m_rc = sds_getpoint(0, m_sPrompt.c_str(), m_point);
	if (m_rc == RTKWORD)
	{
		char answer[IC_ACADBUF];
		m_rc = sds_getinput(answer);
		if (m_rc != RTNORM)
			throw exception();

		int i = 0;
		for (; i < N-1; ++i)
		{
			if (strisame(m_asKeys[i].c_str(), answer))
				break;
		}
		m_selection = i;
	}
	else if (m_rc != RTNORM)
		throw exception();

	return next(m_selection);
}

template <int N>
int TCmdQueryPointOrKeyWord<N>::getSelection() const
{
	return m_selection;
}

template <int N>
icl::point TCmdQueryPointOrKeyWord<N>::getPoint() const
{
	return m_point;
}
