#include "db.h"
#include "DbMText.h"

CDbMText::CDbMText(): db_entity_with_extrusion(DB_MTEXT)
{
    m_pTextStyle = NULL;
	m_insertionPoint[0] = m_insertionPoint[1] = m_insertionPoint[2] = 0.0;
    m_ox[0] = 1.0; m_ox[1] = m_ox[2] = 0.0;
    db_qgetvar(DB_QTEXTSIZE, NULL, &m_initialHeight, DB_DOUBLE, 0);
    m_initialHeight = 0.0;
    m_initialWidth = 1.0;
	m_attachTo = eTOP_LEFT;
	m_direction = eLEFT_RIGHT;
	m_LineFeedRatio = 1.0;
	m_LineFeedType = eVARIABLE;

    m_pDOs = NULL;
	memset(m_extents, 0, sizeof(m_extents));
}

CDbMText::CDbMText(db_drawing* pDrawing): db_entity_with_extrusion(DB_MTEXT, pDrawing)
{
    char* pBuf;
    pBuf = (pDrawing != NULL) ? pDrawing->ret_headerbuf() : NULL;
    db_qgetvar(DB_QTEXTSIZE, pBuf, &m_initialHeight, DB_DOUBLE, 0);

    m_pTextStyle = (pDrawing != NULL) ? pDrawing->ret_currenttabrec(DB_QTEXTSTYLE) : NULL;
    m_insertionPoint[0] = m_insertionPoint[1] = m_insertionPoint[2] = 0.0;
    m_ox[0] = 1.0; m_ox[1] = m_ox[2] = 0.0;
    m_initialHeight = 0.0;
    m_initialWidth = 1.0;
	m_attachTo = eTOP_LEFT;
	m_direction = eLEFT_RIGHT;
	m_LineFeedRatio = 1.0;
	m_LineFeedType = eVARIABLE;

    m_pDOs=NULL;
	memset(m_extents, 0, sizeof(m_extents));
}

CDbMText::CDbMText(const CDbMText& entity): db_entity_with_extrusion(entity)
{
    m_text = entity.m_text;
    memcpy(m_insertionPoint, entity.m_insertionPoint, sizeof(sds_point));
	memcpy(m_ox, entity.m_ox, sizeof(sds_point));
    m_pTextStyle = entity.m_pTextStyle;
	m_initialHeight = entity.m_initialHeight;
	m_initialWidth = entity.m_initialWidth;
    m_attachTo = entity.m_attachTo;
	m_direction = entity.m_direction;
	m_LineFeedRatio = entity.m_LineFeedRatio;
	m_LineFeedType = entity.m_LineFeedType;

    memcpy(m_extents[0], entity.m_extents[0], sizeof(sds_point));
	memcpy(m_extents[1], entity.m_extents[1], sizeof(sds_point));
	// Don't copy display objects.
    m_pDOs = NULL;
}

CDbMText::~CDbMText()
{
	if(db_grafreedofp != NULL)
		db_grafreedofp(m_pDOs);
}

void CDbMText::get_extent(sds_point ll, sds_point ur)
{
	memcpy(ll, m_extents[0], sizeof(sds_point));
	memcpy(ur, m_extents[1], sizeof(sds_point));
}

void CDbMText::set_extent(sds_point ll, sds_point ur)
{
	memcpy(m_extents[0], ll, sizeof(sds_point));
	memcpy(m_extents[1], ur, sizeof(sds_point));
}

bool CDbMText::get_1(char* p, int maxlen)
{
	if(m_text.empty())
		*p =0;
	else
	{
		strncpy(p, m_text.c_str(), maxlen);
		p[maxlen] = 0;
	}
	return true;
}

bool CDbMText::get_7(char *p, int maxlen)
{
	if(m_pTextStyle == NULL)
		*p = 0;
	else
		m_pTextStyle->get_2(p, maxlen);
	return true;
}

bool CDbMText::get_7(char** p)
{
	if(m_pTextStyle != NULL)
		m_pTextStyle->get_2(p);
	else
		*p = db_str_quotequote;
	return true;
}

bool CDbMText::get_50(sds_real *p, db_drawing *pDrawing)
{  /* Calculated from m_ox. */
	sds_point extrusion;
	get_210(extrusion);
	mtextrot(m_ox, extrusion, p, 1, pDrawing);
	return true;
}

bool CDbMText::set_7(char *p, db_drawing *pDrawing)
{
	db_handitem *pNewStyle;
    if(p == NULL || !*p || pDrawing==NULL ||
		(pNewStyle = pDrawing->findtabrec(DB_STYLETAB, p, 1)) == NULL)
	{
		db_lasterror = OL_ESNVALID;
		return false;
	}
	m_pTextStyle = pNewStyle;
	return true;
}

bool CDbMText::set_11(sds_point p)
{
	if(icadRealEqual(p[0],0.0) && icadRealEqual(p[1],0.0) && icadRealEqual(p[2],0.0))
	{
		m_ox[0] = 1.0;
		m_ox[1] = m_ox[2] = 0.0;
	}
	else
		memcpy(m_ox, p, sizeof(sds_point));
	return true;
}

bool CDbMText::set_50(sds_real p, db_drawing *pDrawing)
{
	// Sets m_ox
	sds_point axisOX;
	sds_point extrusion;
	get_210(extrusion);

	mtextrot(axisOX, extrusion, &p, 0, pDrawing);

	// Avoid value "creep" from repeated get_50()/set_50() calls.
	// Only change m_ox if it changes significantly.
	// It's a unit vector, so using absolute IC_ZRO is okay.
	if (fabs(m_ox[0] - axisOX[0]) > IC_ZRO ||
		fabs(m_ox[1] - axisOX[1]) > IC_ZRO ||
		fabs(m_ox[2] - axisOX[2]) > IC_ZRO)
		memcpy(m_ox, axisOX, sizeof(sds_point));

	return true;
}

int CDbMText::entgetspecific(struct resbuf** begpp, struct resbuf** endpp, db_drawing* pDrawing)
{
	// Specific entgetter.  Returns 0 (OK) or -1 (no memory).
	assert(begpp != NULL);
	assert(endpp != NULL);

    short modifier;
    sds_real angle;
    int result = 0;
	char* pString;
	CDbString fragment;
    int i, j, l, group;

    *begpp = NULL;
	*endpp = NULL;

    // 10 (m_insertionPoint) */
    if(((*begpp) =(*endpp) = db_newrb(10, 'P', m_insertionPoint)) == NULL)
	{
		result = -1;
		goto out;
	}
    // 40 (m_initialHeight) */
    if(((*endpp)->rbnext = db_newrb(40, 'R', &m_initialHeight)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 41 (m_initialWidth) */
    if(((*endpp)->rbnext = db_newrb(41, 'R', &m_initialWidth)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 71 (m_attachTo) */
    modifier = (short)m_attachTo;
    if(((*endpp)->rbnext = db_newrb(71, 'H', &modifier)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 72 (m_direction) */
    modifier = (short)m_direction;
    if(((*endpp)->rbnext = db_newrb(72, 'H', &modifier)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 3 / 1 (m_text)
	// EBATECH(CNBR) -[ Fixed: Duplicate DXF=1, MBCS was splitted at 250th char.
	if(m_text.empty())
	{
		(*endpp)->rbnext= db_newrb(1, 'S', db_str_quotequote);
        if((*endpp)->rbnext == NULL)
		{
			result = -1;
			goto out;
		}
        (*endpp) = (*endpp)->rbnext;
	}
	else
	{
		i = 0;
		l = m_text.length();
		while(i < l)
		{
			if(( l - i ) <= 255 )
			{
				group = 1;
				j = ( l - i );
			}
			else
			{
				group = 3;
				if((_MBC_LEAD ==_mbbtype((unsigned char)m_text[249],0)) &&
					(_MBC_TRAIL==_mbbtype((unsigned char)m_text[250],1)))
				{
					j = 249;
				}
				else
				{
					j = 250;
				}
			}
			fragment = m_text.substr(i, j);
			if(((*endpp)->rbnext= db_newrb(group, 'S', (char*)fragment.c_str())) == NULL)
			{
				result = -1;
				goto out;
			}
			(*endpp) = (*endpp)->rbnext;
			i += j;
		}
	}
	// -- old wrong code --
	//l = m_text.length();
	//if(l > 250)
	//{
	//	i = 0;
	//	while(i < l)
	//	{
	//		if(i + 250 >= l || i == 0)
	//			group = 1;
	//		else
	//			group = 3;
	//		fragment = m_text.substr(i, 250);
	//		if(((*endpp)->rbnext= db_newrb(group, 'S', (char*)fragment.c_str())) == NULL)
	//		{
	//			result = -1;
	//			goto out;
	//		}
	//		(*endpp) = (*endpp)->rbnext;
	//		i += 250;
	//	}
	//}
	//else
	//{
	//	if(m_text.empty())
	//        (*endpp)->rbnext= db_newrb(1, 'S', db_str_quotequote);
	//	else
	//        (*endpp)->rbnext= db_newrb(1, 'S', (char*)m_text.c_str());
    //    if((*endpp)->rbnext == NULL)
	//	{
	//		result = -1;
	//		goto out;
	//	}
    //    (*endpp) = (*endpp)->rbnext;
	//}
	// EBATECH(CNBR) ]-

    // 7 (text style name)
    pString = NULL;
    if(m_pTextStyle != NULL)
		m_pTextStyle->get_2(&pString);
    if(((*endpp)->rbnext = db_newrb(7, 'S', (pString != NULL) ? pString : db_str_quotequote)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 210 (extrusion)
	sds_point extrusion;
	get_210(extrusion);
    if(((*endpp)->rbnext = db_newrb(210, 'P', extrusion)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 11 (m_ox)
    if(((*endpp)->rbnext = db_newrb(11, 'P', m_ox)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 42 (Horizontal width of characters.) 2002/6/10 get actual width via get_42()
    {
    sds_real width;
    get_42(&width);
    if(((*endpp)->rbnext = db_newrb(42, 'R', &width)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;
	}
    // 43 (Vertical height of char.) 2002/6/10 get actual height via get_43()
    {
    sds_real height;
    get_43(&height);
    if(((*endpp)->rbnext = db_newrb(43, 'R', &height)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;
	}
    // 50 rotation (calculated)
    get_50(&angle, pDrawing);
    if(((*endpp)->rbnext = db_newrb(50, 'R', &angle)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 73 m_LineFeedType */
    modifier = (short)m_LineFeedType;
    if(((*endpp)->rbnext = db_newrb(73, 'H', &modifier)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

    // 44 m_LineFeedRatio
    if(((*endpp)->rbnext = db_newrb(44, 'R', &m_LineFeedRatio)) == NULL)
	{
		result = -1;
		goto out;
	}
    (*endpp) = (*endpp)->rbnext;

out:
	if (result && (*begpp) != NULL)
	{
		sds_relrb((*begpp));
		(*begpp) = (*endpp) = NULL;
	}
    return result;
}

int CDbMText::entmod(struct resbuf *modll, db_drawing *pDrawing)
{
	// Entity/table-record specific.  Uses the setters for range-correction.
	// Returns: See db_handitem::entmod().
    int result = 0;
    db_lasterror = 0;
    if(modll == NULL || pDrawing == NULL)
		return -1;
    del_xdict();

	bool bClearText = true;
    struct resbuf* pCurPair = modll;
    // If modll has been built properly, the first few groups
    // are things we don't allow to be modified (ename, type, handle),
    // so let's walk over these right now:
    while (pCurPair!=NULL &&
        (pCurPair->restype==-1 ||
         pCurPair->restype== 0 ||
         pCurPair->restype== 5))
		 pCurPair=pCurPair->rbnext;

    // Walk; stop at the end or the EED sentinel (-3):
    for (; !result && pCurPair != NULL && pCurPair->restype != -3; pCurPair = pCurPair->rbnext)
	{
        // Call the correct setter: */
        switch (pCurPair->restype)
		{
            case 1:
			case 3:
				if(bClearText)
				{
					m_text.erase(m_text.begin(), m_text.end());
					bClearText = false;
				}
                if(pCurPair->resval.rstring == NULL || !*pCurPair->resval.rstring)
                    break;
				m_text += pCurPair->resval.rstring;
                break;
            case   7:
				set_7(pCurPair->resval.rstring, pDrawing);
				break;
            case  10:
				set_10(pCurPair->resval.rpoint);
				break;
            case  11:
				set_11(pCurPair->resval.rpoint);
				break;
            case  40:
				set_40(pCurPair->resval.rreal);
				break;
            case  41:
				set_41(pCurPair->resval.rreal);
				break;
            case  50:
				set_50(pCurPair->resval.rreal,pDrawing);
				break;
            case  71:
				set_71(pCurPair->resval.rint);
				break;
            case  72:
				set_72(pCurPair->resval.rint);
				break;
            case 210:
				set_210(pCurPair->resval.rpoint);
				break;
            case  44:
				set_44(pCurPair->resval.rreal);
				break;
            case  73:
				set_73(pCurPair->resval.rint);
				break;

            default:  /* The common entity or table record groups. */
                result = entmod1common(&pCurPair, pDrawing);
                break;
        }
    }

    if(db_lasterror)
		return -6;
    // Do EED:
    if(!result && pCurPair != NULL)
		result = set_eed(pCurPair, pDrawing);

    return result;
}

int CDbMText::mtextrot(
    sds_point   axisOX,
    sds_point   mtextru,
    sds_real   *mtrotp,
    int         xdir2rot,
    db_drawing *pDrawing)
{
/*
**  This function deals with the MTEXT X-direction/rotation problem.
**  Given the MTEXT's extrusion vector, it produces the rotation
**  (*mtrotp) for a given X-direction (axisOX), or the X-direction for a
**  given rotation, depending on xdir2rot:
**
**      xdir2rot!=0 : Uses axisOX to set *mtrotp.
**      xdir2rot==0 : Uses *mtrotp to set axisOX.
**
**  The conversion is UCS-dependent.  (Yes, ACAD actually gives different
**  50-group values in entgets for different UCSs!  I thought entgets
**  were supposed to be immune to UCS changes.)  This function
**  determines the current UCS for pDrawing.
**
**  Here's the trick: MTEXT rotation (the 50-group) is the angle of the
**  MTEXT in it's ECS plus the angle offset of "the ECS defined by the
**  current UCS" from the current UCS.
**
**  Note that if axisOX is not normal to mtextru, it is adjusted so that
**  it is (but not for the caller).
**
**  Returns:
**    Always sets results (unless the receiver is NULL), but the
**    return code is bit-coded as follows:
**
**      ----DREX
**      76543210
**
**      D : pDrawing is NULL.  Using UCS=WCS.
**      R : mtrotp is NULL.  Using rotation 0.0 (xdir2rot==0).
**      E : mtextru is NULL or (0,0,0).  Using (0,0,1).
**      X : axisOX is NULL, (0,0,0), or parallel to mtextru.
**            Using the MTEXT's ECS's X-axis (xdir2rot!=0).
*/
    int rc;
    sds_point wxdir,mtecs[3],ucsaxis[3],ecs4ucs[3],ap1;
    sds_real wrot,anginecs,prot,ar1;


    rc=0;


    /* Get the UCS axes: */
    if (pDrawing==NULL) {
        memset(ucsaxis,0,sizeof(ucsaxis));
        ucsaxis[0][0]=ucsaxis[1][1]=ucsaxis[2][2]=1.0;
        rc|=8;
    } else {
        pDrawing->getucs(NULL,ucsaxis[0],ucsaxis[1],ucsaxis[2]);
    }

    /* Get the ECS defined by the current UCS's Z-axis: */
    DB_PTCPY(ecs4ucs[2],ucsaxis[2]);
    ic_arbaxis(ecs4ucs[2],ecs4ucs[0],ecs4ucs[1],ecs4ucs[2]);

    /*
    **  Get prot -- the angular offset of "the ECS defined by the
    **  current UCS" from the current UCS.  ecs2ucs[0] is WCS and so
    **  are the UCS axes.  See what ecs4ucs[0] looks like to the UCS
    **  system and use atan2() to get the position angle of ecs4ucs[0]
    **  in that system (prot):
    */
    prot=atan2(ecs4ucs[0][0]*ucsaxis[1][0]+
               ecs4ucs[0][1]*ucsaxis[1][1]+
               ecs4ucs[0][2]*ucsaxis[1][2],
               ecs4ucs[0][0]*ucsaxis[0][0]+
               ecs4ucs[0][1]*ucsaxis[0][1]+
               ecs4ucs[0][2]*ucsaxis[0][2]);

    /* Get the MTEXT's ECS: */
    if (mtextru==NULL || (icadRealEqual(mtextru[0],0.0) && icadRealEqual(mtextru[1],0.0) && icadRealEqual(mtextru[2],0.0))) {
        mtecs[2][0]=mtecs[2][1]=0.0; mtecs[2][2]=1.0;
        rc|=2;
    } else DB_PTCPY(mtecs[2],mtextru);
    ic_arbaxis(mtecs[2],mtecs[0],mtecs[1],mtecs[2]);

    if (xdir2rot) {

        if (axisOX==NULL || (icadRealEqual(axisOX[0],0.0) && icadRealEqual(axisOX[1],0.0) && icadRealEqual(axisOX[2],0.0))) {
            DB_PTCPY(wxdir,mtecs[0]);
            rc|=1;
        } else {
            /* axisOX SHOULD be unit and normal to mtecs[2].  We have to */
            /* make SURE by getting ydir and crossing it with mtecs[2] */
            /* to correct axisOX: */

            /* Get "ydir": */
			ic_crossproduct(mtecs[2],axisOX,ap1);
            if (icadRealEqual((ar1=sqrt(ap1[0]*ap1[0]+ap1[1]*ap1[1]+ap1[2]*ap1[2])),0.0)) {
                DB_PTCPY(wxdir,mtecs[0]); rc|=1;
            } else {
                if (ar1!=1.0) { ap1[0]/=ar1; ap1[1]/=ar1; ap1[2]/=ar1; }

                /* Get wxdir from "ydir" X mtecs[2]: */
				ic_crossproduct(ap1,mtecs[2],wxdir);
            }
        }

        /*
        **  Get anginecs -- the angle of the MTEXT in its ECS.
        **  wxdir is WCS and so are the mtecs axes.  See what wxdir
        **  looks like to the MTEXT ECS system and use atan2() to get
        **  the position angle of wxdir in that system (anginecs):
        */
        anginecs=atan2(wxdir[0]*mtecs[1][0]+
                       wxdir[1]*mtecs[1][1]+
                       wxdir[2]*mtecs[1][2],
                       wxdir[0]*mtecs[0][0]+
                       wxdir[1]*mtecs[0][1]+
                       wxdir[2]*mtecs[0][2]);

        /* Now just add prot and fix up the result: */
        wrot=prot+anginecs; ic_normang(&wrot,NULL);
        if (fabs(IC_TWOPI-wrot)<IC_ZRO) wrot=0.0;

    } else {  /* rot to axisOX. */

        if (mtrotp==NULL) { wrot=0.0; rc|=4; } else wrot=*mtrotp;

        /* Do the logic in reverse.  What's anginecs? */
        anginecs=wrot-prot;

        /* What unit vector in the MTEXT ECS is defined by that */
        /* position angle? */
        ap1[0]=cos(anginecs); ap1[1]=sin(anginecs);  /* Z-component is 0.0. */

        /* What's it look like to the WCS? */
        wxdir[0]=ap1[0]*mtecs[0][0]+ap1[1]*mtecs[1][0];
        wxdir[1]=ap1[0]*mtecs[0][1]+ap1[1]*mtecs[1][1];
        wxdir[2]=ap1[0]*mtecs[0][2]+ap1[1]*mtecs[1][2];

    }

    if (xdir2rot) {
        if (mtrotp!=NULL) *mtrotp=wrot;
    } else {
        if (axisOX!=NULL) DB_PTCPY(axisOX,wxdir);
    }

    return rc;
}

