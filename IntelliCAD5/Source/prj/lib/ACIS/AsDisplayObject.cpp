// File  : 
// Author: Alexey Malov
#include "stdafx.h"
#include "AsDisplayObject.h"
#include "AsTools.h"
#include "sds.h"
#undef T
#include <stack>
#include <ostream.h>

#include "../../ICAD/Configure.h"

CAsDisplayObject::CAsDisplayObject()
{}

CAsDisplayObject::~CAsDisplayObject()
{
    clear();
}

void CAsDisplayObject::clear()
{
    dob_iterator inode = m_nodes.begin();
    for (; inode != m_nodes.end(); ++inode)
    {
        delete *inode;
    }
    m_nodes.clear();
}

				// jlkwejd   /**/

void CAsDisplayObject::copy(sds_dobjll*& doblist)
{
    doblist = 0;
#if 0
    dob_iterator inode = m_nodes.begin();
	for (; inode != m_nodes.end(); ++inode)
#else
	std::list<CNode*>::reverse_iterator inode = m_nodes.rbegin();
	for (; inode != m_nodes.rend(); ++inode)
#endif
    {
        if (!(*inode)->m_entity)
        {
            CChain* pChain = static_cast<CChain*>(*inode);
			int type = pChain->m_type;
			if (type & eHighlighted)
				type &= ~eInvisible;
			if (type & eInvisible)
				continue;

            sds_dobjll* dob = reinterpret_cast<sds_dobjll*>(new sds_dobjll );

            dob->type = type;
            dob->color = pChain->m_color;
#ifdef BUILD_WITH_NEW_SDS_DOBJLL
            dob->lweight = 0; // 2003/5/5 Ebatech(CNBR) Initialize lweight as zero.
            dob->npts = pChain->m_pts.size();
#else
			assert( pChain->m_pts.size() <= SHRT_MAX );
			dob->npts = ( pChain->m_pts.size() > SHRT_MAX ? SHRT_MAX :  pChain->m_pts.size());
#endif
            int size = sizeof(sds_point)*dob->npts;
            dob->chain = reinterpret_cast<sds_real*>(new sds_point [dob->npts]);
            memcpy(dob->chain, pChain->m_pts.begin(), size);

            dob->next = doblist;
			doblist = dob;
        }
    }
}

void CAsDisplayObject::highlight(int entity)
{
    bool bHighlight = false;
    if (!entity)
        // highlight all
        bHighlight = true;

    dob_iterator inode = m_nodes.begin();
    for (; inode != m_nodes.end(); inode++)
    {
        if (entity && (*inode)->m_entity == entity)
        {
            bHighlight = !bHighlight;
        }
        if (!(*inode)->m_entity && bHighlight)
        {
            CChain* pChain = static_cast<CChain*>(*inode);
            pChain->m_type |= eHighlighted;
        }
    }

    up(entity);
}

void CAsDisplayObject::up(int entity)
{
    if (!entity)
        return;

    std::stack<dob_iterator> nodes; nodes.push(0);
    dob_iterator inode = m_nodes.begin();
    dob_iterator isuper = 0;
    dob_iterator istart = 0;
    dob_iterator iend = 0;

    for (; inode != m_nodes.end(); inode++)
    {
        int ent = (*inode)->m_entity;

        if (!ent)
            continue;

        // store possible super node
        if ((nodes.top() == 0) ||
            (*nodes.top())->m_entity != ent)
        {
            // new entity node started
            if (nodes.top() != 0 &&
                ent == entity)
            {
                isuper = nodes.top();
                istart = inode;
            }

            nodes.push(inode);
        }
        else
        {
            // entity node finished
            if (ent == entity)
            {
                iend = ++inode; 
                inode--;
            }
            if (isuper != 0 &&
                (*isuper)->m_entity == ent)
            {
                if (istart != 0 && iend != 0)
                {
                    if (iend != inode)
                        m_nodes.splice(inode, m_nodes, istart, iend);
                }
                isuper = istart = iend = 0;
            }
            nodes.pop();
        }
    }
}

void CAsDisplayObject::unhighlight(int entity)
{
    bool bUnhighlight = false;
    if (!entity)
        // unhighlight all
        bUnhighlight = true;

    dob_iterator inode = m_nodes.begin();
    for (; inode != m_nodes.end(); inode++)
    {
        if (entity && (*inode)->m_entity == entity)
        {
            bUnhighlight = !bUnhighlight;
        }
        if (!(*inode)->m_entity && bUnhighlight)
        {
            CChain* pChain = static_cast<CChain*>(*inode);
            pChain->m_type &= ~eHighlighted;
        }
    }
    
	up(entity);
}

void CAsDisplayObject::transform(const SPAtransf& tr)
{
    dob_iterator inode = m_nodes.begin();
    for (; inode != m_nodes.end(); inode++)
    {
        if (!(*inode)->m_entity)
        {
            CChain* pChain = static_cast<CChain*>(*inode);
            std::vector<point>::iterator i = pChain->m_pts.begin();
            for (; i != pChain->m_pts.end(); i++)
            {
                (SPAposition&)(*i) *= tr;
            }
        }
    }
}

void CAsDisplayObject::start(int entity)
{
    m_nodes.push_back(new CNode);
    m_nodes.back()->m_entity = entity;
}

void CAsDisplayObject::end(int entity)
{
    m_nodes.push_back(new CNode);
    m_nodes.back()->m_entity = entity;
}

void CAsDisplayObject::add
(
point aPoint[], 
int nPoint, 
int type, 
int color
)
{
    if (nPoint < 1)
        return;

    CChain* pChain = new CChain;
    pChain->m_entity = 0;
    pChain->m_color = color;
    pChain->m_type = type;

    for (int i = 0; i < nPoint; i++)
    {
		point pt = aPoint[i];
        pChain->m_pts.push_back(pt);
    }

    m_nodes.push_back(pChain);
}


ostream& operator << (ostream &os, const CAsDisplayObject& asDO)
{
	os << "**** CAsDisplayObject: " << &asDO << " **** " << endl;
	os << "Total number of nodes: " << asDO.m_nodes.size() << endl;

	int i = 0;
	list<CAsDisplayObject::CNode*> nodes = const_cast<CAsDisplayObject*>(&asDO)->m_nodes;
    std::list<CAsDisplayObject::CNode*>::iterator inode = nodes.begin();
    for (; inode != nodes.end(); inode++) {
		if (!(*inode)->m_entity) {
			os << "*** Chain Node[" << i++ << "] Begin ***" << endl;
			CAsDisplayObject::CChain* pChain = static_cast<CAsDisplayObject::CChain*>(*inode);
			os << "Numebr of points: ";
			if (pChain) {
				int num = pChain->m_pts.size();
				os << num << endl;
				os << "Type: " << pChain->m_type << "; " << endl;
				os << "Color: " << pChain->m_color << "; " << endl;
				for (register j = 0; j < num; j++) {
					point pt = pChain->m_pts[j];
					os << "Point[" << j << "]: (" << pt.x() << ", " << pt.y() 
					   << ", " << pt.z() << "); " << endl;
				}
			}
			else 
				os << 0 << endl;
			os << "*** Chain Node[" << i++ << "] End ***" << endl;
		}
	}
	os << "**** CAsDisplayObject ****" << endl;
	return os;
}

