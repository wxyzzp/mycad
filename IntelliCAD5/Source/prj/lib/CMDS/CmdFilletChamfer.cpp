#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadView.h"
#include "IcadDoc.h"
#include "IcadTextScrn.h"
#include "CmdSolidEdit.h"
#include "gvector.h"
using namespace icl;
#include <vector>
#include <algorithm>
using namespace std;

int cmd_lwpline_dist
(
sds_name lwplename, 
sds_point pFrom, 
sds_point pTo, 
sds_real* dist
);

void LwplrbAddVerts
(
db_lwpolyline* lwpl,
resbuf** lwpladdrb,
int startvert,
int endvert,
bool circular
);

int cmd_champt_by_angle
(
sds_point startpt,
sds_point intpt,
sds_point segstart,
sds_point segend,
sds_real deflang,
sds_point pickpt2,
int onseg,
sds_point result
);

int cmd_modpts_4_chamfer
(
sds_point startpt,
sds_point endpt,
sds_point chamferpt,
sds_point inters,
sds_point pickpt
);

int cmd_do_fillet
(
sds_name ent1,
sds_point ptsel1,
sds_name ent2,
sds_point ptsel2,
int ptflag
);

int cmd_fillet_line2line
(
sds_point p1,
sds_point p2,
sds_point line1pt,
int *l1type,
sds_point p3,
sds_point p4,
sds_point line2pt,
int *l2type,
sds_point fp1,
sds_point fp2,
int *pts2move,
sds_point cc,
sds_real *sa,
sds_real *ea,
sds_point extru
);

int cmd_linept4fillet
(
sds_point p1,
sds_point p2,
sds_point p7,
sds_point ptsel1,
sds_point p5,
int *moveflag
);

int cmd_fillet_arc2line
(
sds_point p1,
sds_point p2,
sds_point linpt,
sds_point cc,
sds_real rr,
sds_point arcpt,
sds_point p3,
sds_point p4,
sds_point cc2,
int *cw
);

void cmd_arcang4fillet
(
sds_real filletang,
sds_real intang,
sds_real sa,
sds_real ea,
sds_real pickang,
int *dxf
);

int cmd_fillet_arc2arc
(
sds_point cc1,
sds_real rr1,
sds_point ptsel1,
sds_point cc2,
sds_real rr2,
sds_point ptsel2,
sds_point fp1,
sds_point fp2,
sds_point cc3,
int *cw
);

int cmd_segxpoly
(
sds_name polyent,
sds_point epoint,
sds_point p0,
sds_point p1,
sds_real bulge,
sds_point intpt,
sds_real *polydist,
sds_name vtxent
);

int cmd_segxlwpoly
(
sds_name lwplename,
sds_point epoint,
sds_point p0,
sds_point p1,
sds_real bulge,
sds_point intpt,
sds_real *polydist,
int *vertno
);

extern int svar_ChildDialog;

class CCmdChamferAcisEntity
{
public:
	CCmdChamferAcisEntity
	(
	sds_name ent,
	const point& pickpt
	);
	~CCmdChamferAcisEntity();

	int run();

private://methods

	int  pickFaces();
	int  selectFaces();
	void getChamferDistancies();
	void pickEdge();
	void selectEdges();
	void highlight(HSUBENT hsubent, bool bRegen);
	void unhighlight(HSUBENT hsubent, bool bRegen);
	void regen();

private://data
	// sat data
	CDbAcisEntityData m_oldData;
	CDbAcisEntityData m_newData;
	// selected ACIS entity
	sds_name m_ent;

	// edges for chamfering
	vector<HEDGE> m_aChamferEdges;
	// base surface chamfer distance
	double m_baseChamferDist;
	// other surface chamfer distance
	double m_otherChamferDist;

	// modeler
	CModeler* m_pModeler;

	// edge selection mode
	bool m_bSelectLoop;

	// last pick point
	point m_pt;

	// faces, adjacent to first picked edge
	HFACE m_aFaces[2];
	// index of base face
	int m_iSelectedFace;

	// edges of base face
	xstd<HEDGE>::vector m_aBaseEdges;
};

CCmdChamferAcisEntity::CCmdChamferAcisEntity
(
sds_name ent,
const point& pickpt
)
:m_baseChamferDist(0.), m_otherChamferDist(0.),
 m_bSelectLoop(false),
 m_pModeler(CModeler::get()),
 m_pt(pickpt),
 m_iSelectedFace(0)
{
	ic_encpy(m_ent, ent);

	CDbAcisEntity* pEnt = reinterpret_cast<CDbAcisEntity*>(ent[0]);
	m_oldData = pEnt->getData();
	
	m_pModeler->lockDatabase();
}

CCmdChamferAcisEntity::~CCmdChamferAcisEntity()
{
	m_pModeler->unlockDatabase();
}

int CCmdChamferAcisEntity::run()
{
	try
	{
		// select base face
		int rc = selectFaces();
		if (rc != RTNORM)
			return rc;

		// input chamfer distancies
		getChamferDistancies();

		// select edges
		selectEdges();

		// call modeler
		if (m_pModeler->chamfer(m_oldData, m_newData, 
								m_aChamferEdges,
								m_baseChamferDist, 
								m_otherChamferDist,
								getCurrColorIndex()))
		{
			// Update blank solid
			resbuf* p = sds_entget(m_ent);
			ic_assoc(p, 1);
			char*& dxf1 = ic_rbassoc->resval.rstring;
			IC_FREE(dxf1);			
			dxf1 = new char [m_newData.getSat().size()+1];
			strcpy(dxf1, m_newData.getSat().c_str());

			return sds_entmod(p);
		}
		else
		{
			// error of modeler
			printAcisError();
			unhighlight(0, true);
			return RTERROR;
		}
	}
	catch (int ec)
	{
		unhighlight(0, true);
		return ec;
	}
	return RTNORM;
}

int CCmdChamferAcisEntity::pickFaces()
{
    icl::gvector dir;
    {
        resbuf rb;
        sds_getvar("VIEWDIR"/*DNT*/, &rb);
        dir = rb.resval.rpoint;

        icl::transf t;
        if (!ucs2wcs(t))
            return RTERROR;

        dir *= t;
		m_pt *= t;
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

    xstd<HFACE>::vector selected;
	if (!m_pModeler->pickFace(m_oldData, m_pt, dir, prec, CModeler::eByEdge, selected))
    {
		printAcisError();
		return RTERROR;
	}

	if (selected.empty())
		return RTPICKS;

	assert(selected.size() == 2);

	m_aFaces[0] = selected[0];
	m_aFaces[1] = selected[1];

	m_iSelectedFace = 0;
	highlight(m_aFaces[0], true);
	return RTNORM;
}

int CCmdChamferAcisEntity::selectFaces()
{
	int rc = pickFaces();
	if (rc != RTNORM)
		return rc;

	sds_printf("Base surface selection...\n");

	do
	{
		char szKeys[] = "Next OK ~Next ~OK";
		checkSuccess(sds_initget(0, szKeys), RTNORM, 0);

		char szAnswer[IC_ACADBUF];
		char szPrompt[] = "Enter surface selection option: Next/<OK (current)>: ";
		checkSuccess(sds_getkword(szPrompt, szAnswer), RTNORM, RTNONE, RTKWORD, 0);

		if (strisame(szAnswer, "NEXT"))
		{
			unhighlight(m_aFaces[m_iSelectedFace], false);
			m_iSelectedFace = !m_iSelectedFace;
			highlight(m_aFaces[m_iSelectedFace], true);
		}
		else
			break;
	}
	// "endless" loop is broken, when user select OK
	while (true);

	// get edges of base face
	if (!m_pModeler->getEdges(m_aFaces[m_iSelectedFace], m_aBaseEdges))
	{
		printAcisError();
		return RTERROR;
	}

	unhighlight(0, true);
	return RTNORM;
}

void CCmdChamferAcisEntity::getChamferDistancies()
{
	resbuf rb;
	rb.rbnext = 0;
	rb.restype=RTREAL;

	char szChamferDist[IC_ACADBUF];
	char szPrompt[IC_ACADBUF];

	m_baseChamferDist = 0.;
	{
		checkSuccess(SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES),
					 RTNORM, 0);
		m_baseChamferDist = rb.resval.rreal;
	
		sds_rtos(m_baseChamferDist,-1,-1,szChamferDist);
		sprintf(szPrompt,ResourceString(IDC_CMDS4__NCHAMFER_DISTANC_49, "\nChamfer distance on first entity <%s>: " ),
				szChamferDist);

		checkSuccess(sds_initget(RSG_NONEG,NULL), RTNORM, 0);
		int rc = checkSuccess(sds_getdist(NULL,szPrompt,&m_baseChamferDist), RTNORM, RTNONE, 0);
		if (rc == RTNORM)
	   	{
			rb.resval.rreal = m_baseChamferDist;
			checkSuccess(sds_setvar("CHAMFERA"/*DNT*/,&rb), RTNORM, 0);
		}
	}

	m_otherChamferDist = m_baseChamferDist;
	{
		sds_rtos(m_otherChamferDist,-1,-1,szChamferDist);
		sprintf(szPrompt,ResourceString(IDC_CMDS4__NCHAMFER_DISTANC_51, "\nChamfer distance on second entity <%s>: " ),
				szChamferDist);

		checkSuccess(sds_initget(RSG_NONEG,NULL), RTNORM, 0);
		checkSuccess(sds_getdist(NULL,szPrompt,&m_otherChamferDist), RTNORM, RTNONE, 0);

		rb.resval.rreal = m_otherChamferDist;
		checkSuccess(sds_setvar("CHAMFERB"/*DNT*/,&rb), RTNORM, 0);
	}
}

void CCmdChamferAcisEntity::pickEdge()
{
    icl::gvector dir;
    {
        resbuf rb;
        sds_getvar("VIEWDIR"/*DNT*/, &rb);
        dir = rb.resval.rpoint;

        icl::transf t;
        if (!ucs2wcs(t))
            throw RTERROR;

        dir *= t;
		m_pt *= t;
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

    xstd<HEDGE>::vector selected;
	int rc;
	if (m_bSelectLoop)
	{
		rc = m_pModeler->pickLoop(m_oldData, m_pt, dir, prec, m_aFaces[m_iSelectedFace], selected);
		if (rc && selected.empty())
		{
			sds_printf("Edges must belong to base face.\n");
		}
	}
	else
		rc = m_pModeler->pickEdge(m_oldData, m_pt, dir, prec, CModeler::eOnly, selected);
	if (rc)
    {
		xstd<HEDGE>::vector::const_iterator iedge;
		for (iedge = selected.begin(); iedge != selected.end(); ++iedge)
		{
			// selected edge should belong base face
			if (m_aBaseEdges.end() ==
				std::find(m_aBaseEdges.begin(), m_aBaseEdges.end(), *iedge))
			{
				sds_printf("Edges must belong to base face.\n");
				selected.clear();
				break;
			}
		}

		for (iedge = selected.begin(); iedge != selected.end(); ++iedge)
		{
			if (std::find(m_aChamferEdges.begin(), m_aChamferEdges.end(),
					 *iedge) == m_aChamferEdges.end())
			{
				m_aChamferEdges.push_back(*iedge);
			}

			highlight(*iedge, true);
		}
    }
}

void CCmdChamferAcisEntity::selectEdges()
{
	do
	{
		const char* szVariants1 = "Loop ~Loop";
		const char* szVariants2 = "Edge ~Edge";
		const char* szVariants = m_bSelectLoop? szVariants2: szVariants1;
		checkSuccess(sds_initget(0, szVariants), RTNORM, 0);

		const char* szPrompt1 = "\nSelect an edge or /Loop: ";
		const char* szPrompt2 = "\nSelect an edge loop or /Edge: ";
		const char* szPrompt = m_bSelectLoop? szPrompt2: szPrompt1;

		SDS_SetCursor(SDS_GetPickCursor());
		int rc = checkSuccess(SDS_AskForPoint(0, szPrompt, m_pt), RTNORM, RTNONE, RTKWORD, 0);

		switch (rc)
		{
		case RTNORM:
			pickEdge();
			break;
		case RTKWORD:
			{
				char szAnswer[IC_ACADBUF];
				checkSuccess(sds_getinput(szAnswer), RTNORM, 0);

				if (strisame(szAnswer, "LOOP"/*DNT*/))
		   		{
					m_bSelectLoop = true;
				}
				else if (strisame(szAnswer, "EDGE"/*DNT*/))
				{
					m_bSelectLoop = false;
				}
			}
			break;
		case RTNONE:
			return;
		default:
			assert(false);
		}
	}
	while (true);
}

void CCmdChamferAcisEntity::regen()
{
	// redraw entity
	db_handitem* dbitem = reinterpret_cast<db_handitem*>(m_ent[0]);
	gr_displayobject* dobj = reinterpret_cast<gr_displayobject*>(dbitem->ret_disp());
	gr_freedisplayobjectll(dobj);
	dbitem->set_disp(0);

	sds_redraw(m_ent, IC_REDRAW_DRAW);
}

void CCmdChamferAcisEntity::unhighlight(HSUBENT hsubent, bool bRegen)
{
	if (m_pModeler->unhighlight(m_oldData, hsubent))
	{
		if (bRegen)
			regen();
	}
	else
	{
		// print error info
		printAcisError();
	}
}

void CCmdChamferAcisEntity::highlight(HSUBENT hsubent, bool bRegen)
{
	if (m_pModeler->highlight(m_oldData, hsubent))
	{
		if (bRegen)
			regen();
	}
	else
	{
		// print error info
		printAcisError();
	}
}

static
int cmd_do_chamfer_acis_entity(sds_name ename, sds_point pt)
{
	CCmdChamferAcisEntity cmd(ename, pt);
	return cmd.run();
}

//CMD_DO_CHAMFER is called from CMD_CHAMFER, which handles settings & input
int cmd_do_chamfer(sds_name ename1, sds_point pt1, sds_name ename2, sds_point pt2,int ptflag)
{

	//this command actually does the chamferring of two entities. You can only chamfer
	//	a pline to a line (not to an xline, ray, or another pline.  If ename1==ename2,
	//	we call chamfer routine	for polylines.
	//pt1 & pt2 are UCS points
	//pt1 and pt2 need not be on the entities - they can be passed from entsel
	//ptflag bit0 tells if pt1 is a valid pt.  if it's 0, just take the start of the ent
	//same for ptflag bit1 as indicator for pt2
	//if ptflag&8, this is a SPECIAL CASE - the user has turned on the "Directional" option
	//	and the first selected object was a CLOSED pline.

	char fs1[IC_ACADBUF], chamlayer[IC_ACADNMLEN], chamltype[IC_ACADNMLEN],tmpstr[IC_ACADNMLEN];
	int ret=0, fi1,fi2,fi3,chammode,trimmode,closure=0,chamcolor,l1type,l2type,l1maxdir,l2maxdir,makebit;
	sds_name etemp1, etemp2, etemp3, line_ent, plent;
	sds_real fr1,fr2,fr3,fr4,fr5,fr6;
	struct resbuf *rbp1, *rbp2, *rbp3, rb, *rbtemp,rbwcs,rbent;
	sds_point p1,p2,p3,p4,p5,p6,p7,p8,p9,p10;

	rbp1=rbp2=rbp3=NULL;
	SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	chammode=rb.resval.rint;
	SDS_getvar(NULL,DB_QTRIMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	trimmode=rb.resval.rint;
	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;

	makebit=0;//bit 0 tells if ename1 is an entmake (not entmod)
			  //bit 1 tells if ename2 is and entmake (not entmod)

	if (!trimmode)
	{
		if (chammode)
		{
			SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
			fr2=1.0;
		}
		else
		{
			SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
			SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}
		if ((fr1<CMD_FUZZ)||(fr2<CMD_FUZZ))
		{
			cmd_ErrorPrompt(CMD_ERR_NOEFFECT,0);
			goto exit;
		}
	}

	rbp3=sds_buildlist(RTSTR,"*"/*DNT*/,0);
	if ((rbp1=sds_entgetx(ename1,rbp3))==NULL)
	{
		ret=-1;goto exit;
	}
	if ((rbp2=sds_entgetx(ename2,rbp3))==NULL)
	{
		ret=-1;goto exit;
	}
	FreeResbufIfNotNull(&rbp3);

	//check to see that they're not the same entity and they're not both plines
	ic_assoc(rbp1,0);
	strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
	rbtemp=NULL;
	if (rbp2!=NULL)
	{
		ic_assoc(rbp2,-1);
		//if the entities are the same, make sure it's a pline
		if (ic_rbassoc->resval.rlname[0]==ename1[0] && ic_rbassoc->resval.rlname[1]==ename1[1])
		{
			if (!strsame(fs1,"POLYLINE"/*DNT*/ ) && !strsame(fs1,"LWPOLYLINE"/*DNT*/))
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMNONPOLY,0);
				ret=-1;
				goto exit;
			}
			else
			{
				sds_relrb(rbp2);
				rbp2=NULL;
				if (strsame(fs1,"POLYLINE"/*DNT*/))
				{
					ic_assoc(rbp1,70);
					if (ic_rbassoc->resval.rint & 88)
					{
						cmd_ErrorPrompt(CMD_ERR_CHAM3DPOLY,0);
						ret=(-1);
						goto exit;
					}
				}
				if (chammode)
				{
					SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr1=rb.resval.rreal;
					SDS_getvar(NULL,DB_QCHAMFERD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr2=rb.resval.rreal;
					if (trimmode)
						fi1=5;
					else
						fi1=6;
				}
				else
				{
					SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr1=rb.resval.rreal;
					SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fr2=rb.resval.rreal;
					if (trimmode)
						fi1=3;
					else
						fi1=4;
				}
				if (ptflag & 8)
					fi1|=8;
				ret=cmd_fillet_or_chamfer_pline(ename1,fi1,fr1,fr2,pt1,pt2);
				goto exit;
			}
		}
		//EBATECH(FUTA) 2001-8-11 "LWPOLYLINE" too -[
		//else if (strsame(fs1,"POLYLINE"/*DNT*/)) //if they're different, make sure both aren't polylines
		else if (strsame(fs1,"POLYLINE"/*DNT*/) || strsame(fs1,"LWPOLYLINE"/*DNT*/)) // ]-
		{
			ic_assoc(rbp1,70);
			if (ic_rbassoc->resval.rint & 88)
			{
				cmd_ErrorPrompt(CMD_ERR_CHAM3DPOLY,0);
				ret=(-1);
				goto exit;
			}
			ic_assoc(rbp2,0);
			//EBATECH(FUTA) 2001-8-11 -[
			//if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
			if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/) ||
				strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/)) // ]-
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMDIFFPOLY,0);
				ret=(-1);
				goto exit;
			}
			else if (!strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMNONLINEAR,0);
				ret=(-1);
				goto exit;
			}
		}
		else if (strsame(fs1,db_hitype2str(DB_LINE)))
		{
			ic_assoc(rbp2,0);
			if (sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,POLYLINE,RAY,XLINE"/*DNT*/)!=RTNORM)
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMNONLINEAR,0);
				ret=(-1);
				goto exit;
			}
			if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
			{
				ic_assoc(rbp2,70);
				if (ic_rbassoc->resval.rint & 88)
				{
					cmd_ErrorPrompt(CMD_ERR_CHAM3DPOLY,0);
					ret=(-1);
					goto exit;
				}
			}
		}
		ic_assoc(rbp2,0);
	}
	if (0==trimmode || (!strsame(fs1,"POLYLINE"/*DNT*/) && !strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)))
	{
		//get the layer to be used for new chamfer entities
		ic_assoc(rbp1,8);
		strncpy(chamlayer,ic_rbassoc->resval.rstring,sizeof(chamlayer)-1);
		ic_assoc(rbp2,8);
		if (strcmp(chamlayer,ic_rbassoc->resval.rstring))
		{
			SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			strncpy(chamlayer,rb.resval.rstring,sizeof(chamlayer)-1);
			IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL;
		}
		//get linetype
		if (ic_assoc(rbp1,6))
			strncpy(chamltype,"BYLAYER"/*DNT*/ ,sizeof(chamltype)-1);
		else
			strncpy(chamltype,ic_rbassoc->resval.rstring,sizeof(chamltype)-1);
		if (ic_assoc(rbp2,6))
			strncpy(tmpstr,"BYLAYER"/*DNT*/ ,sizeof(fs1)-1);
		else
			strncpy(tmpstr,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
		if (strcmp(chamltype,tmpstr))
		{
			SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			strncpy(chamltype,rb.resval.rstring,sizeof(chamltype)-1);
			IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL;
		}
		//get color
		if (ic_assoc(rbp1,62))
			chamcolor=256;
		else
			chamcolor=ic_rbassoc->resval.rint;
		if (ic_assoc(rbp2,62))
			fi2=256;
		else
			fi2=ic_rbassoc->resval.rint;
		if (chamcolor!=fi2)
		{
			SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			chamcolor=ic_colornum(rb.resval.rstring);
			IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL;
		}
	}
	//******************************************************
	//******************************************************
	/*in line-line & line-pline, the following conventions
		are used:

		p1=1st line start point			p5=1st line chamfer point (where it'll break)
		p2=1st line end pt				p6=2nd line/seg chamfer point
		p3=2nd line/seg start pt		p7=intersection of line w/ line/seg
		p4=2nd line/seg end pt			p8,p9,p10 temporary pts for calcs

		if it's a line & pline, line is always first entity (p1,p2,p5)

	  these conventions not followed for chamfering a pline w/itself

	*/
	if (rbp2!=NULL)
		ic_assoc(rbp2,0);//need this in etype if statement
	if (!strsame(fs1,"POLYLINE"/*DNT*/ ) && !strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ ))
	{
		//***************************************************************
		//******CHAMFER TWO LINE-TYPE ENTS TOGETHER**********************
		//***************************************************************
		if (strsame(fs1,db_hitype2str(DB_LINE)))
			l1type=0;
		else if (strsame(fs1,"RAY"/*DNT*/ ))
			l1type=1;
		else
			l1type=2;

		if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))
			l2type=0;
		else if (strsame(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ ))
			l2type=1;
		else
			l2type=2;

		for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
		}
		for (rbtemp=rbp2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==10)
				ic_ptcpy(p3,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p4,rbtemp->resval.rpoint);
		}

        if (l1type == 0) {
            if (icadPointEqual(p1,p2)) {
			    ret=(-1);
			    cmd_ErrorPrompt(CMD_ERR_CHAMNOLENGTH,0);
			    goto exit;
            }
		    p9[0]=p2[0]-p1[0];
		    p9[1]=p2[1]-p1[1];
		    p9[2]=p2[2]-p1[2];
		}
		else {
			if (fabs(p2[1])>fabs(p2[0]))
			{
				if (fabs(p2[2])>fabs(p2[1]))
					l1maxdir=2;
				else
					l1maxdir=1;
			}
			else if (fabs(p2[2])>fabs(p2[0]))
				l1maxdir=2;
			else
				l1maxdir=0;

            ic_ptcpy(p9, p2);

			p2[0]+=p1[0];
			p2[1]+=p1[1];
			p2[2]+=p1[2];
		}
		if (l2type == 0) {
            if (icadPointEqual(p3,p4)) {
			    ret=(-1);
			    cmd_ErrorPrompt(CMD_ERR_CHAMNOLENGTH,0);
			    goto exit;
            }
		    p10[0]=p4[0]-p3[0];
		    p10[1]=p4[1]-p3[1];
		    p10[2]=p4[2]-p3[2];
        }
        else {
			if (fabs(p4[1]) > fabs(p4[0]))
			{
				if (fabs(p4[2]) > fabs(p4[1]))
					l2maxdir=2;
				else
					l2maxdir=1;
			}
			else if (fabs(p4[2]) > fabs(p4[0]))
				l2maxdir=2;
			else
				l2maxdir=0;

            ic_ptcpy(p10, p4);

			p4[0]+=p3[0];
			p4[1]+=p3[1];
			p4[2]+=p3[2];
		}
		//translate all 4 points into the plane defined by the two lines
		ic_crossproduct(p9,p10,rbent.resval.rpoint);
		p5[0]=sqrt(rbent.resval.rpoint[0]*rbent.resval.rpoint[0]+rbent.resval.rpoint[1]*rbent.resval.rpoint[1]+rbent.resval.rpoint[2]*rbent.resval.rpoint[2]);
		rbent.resval.rpoint[0]/=p5[0];
		rbent.resval.rpoint[1]/=p5[0];
		rbent.resval.rpoint[2]/=p5[0];
		rbent.restype=RT3DPOINT;
		sds_trans(p1,&rbwcs,&rbent,0,p1);
		sds_trans(p2,&rbwcs,&rbent,0,p2);
		sds_trans(p3,&rbwcs,&rbent,0,p3);
		sds_trans(p4,&rbwcs,&rbent,0,p4);

		rbtemp=NULL;
		if ((ptflag&1)==0)
		{
			if (0==l1type)
			{
				pt1[0]=(p1[0]+p2[0])/2.0;
				pt1[1]=(p1[1]+p2[1])/2.0;
				pt1[2]=(p1[2]+p2[2])/2.0;
				ic_ptcpy(p5,pt1);
			}
			else
				ic_ptcpy(pt1,p1);
		}
		else
		{
			//move pt1 onto p1 p2 and call it p5
			rbwcs.resval.rint=1;//use as ucs
			sds_trans(pt1,&rbwcs,&rbent,0,p5);
			rbwcs.resval.rint=0;
			ic_ptlndist(p5,p1,p2,&fr1,p5);
		}
		if ((ptflag&2)==0)
		{
			if (0==l2type)
			{
				pt2[0]=(p3[0]+p4[0])/2.0;
				pt2[1]=(p3[1]+p4[1])/2.0;
				pt2[2]=(p3[2]+p4[2])/2.0;
				ic_ptcpy(p6,pt2);
			}
			else
				ic_ptcpy(pt2,p3);
		}
		else
		{
			//move pt2 onto p3 p4 and call it p6
			rbwcs.resval.rint=1;//use as ucs
			sds_trans(pt2,&rbwcs,&rbent,0,p6);
			rbwcs.resval.rint=0;
			ic_ptlndist(p6,p3,p4,&fr1,p6);
		}
		if (RTNORM!=sds_inters(p1,p2,p3,p4,0,p7))
		{
			cmd_ErrorPrompt(CMD_ERR_CHAMPARALLEL,0);
			ret=(-__LINE__);
			goto exit;
		}
		//handle boundary case first
		if (icadPointEqual(p7,p5))
		{
			if (icadPointEqual(p1,p5))
				ic_ptcpy(p5,p2);
			else if (icadPointEqual(p2,p5))
				ic_ptcpy(p5,p1);
			else if (0!=l1type || sds_distance(p7,p2)<=sds_distance(p7,p1))
				ic_ptcpy(p5,p1);
			else
				ic_ptcpy(p5,p1);
		}
		if (icadPointEqual(p7,p6))
		{
			if (icadPointEqual(p3,p6))
				ic_ptcpy(p6,p4);
			else if (icadPointEqual(p4,p6))
				ic_ptcpy(p6,p3);
			else if (0!=l2type || sds_distance(p7,p4)<=sds_distance(p7,p3))
				ic_ptcpy(p6,p3);
			else
				ic_ptcpy(p6,p4);
		}
		//get current vars
		if (chammode)
		{
			SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
			SDS_getvar(NULL,DB_QCHAMFERD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}
		else
		{
			SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
			SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}
		//set p10 to first chamfer point.
		sds_polar(p7,sds_angle(p7,p5),fr1,p10);
		//ic_ptcpy(p5,p10);
		//set p9 to second chamfer point
		if (!chammode)	//if we're using distance...
			sds_polar(p7,sds_angle(p7,p6),fr2,p9);
		else			//we need to get p6 in angle mode...
		{
            if (0>(ret=cmd_champt_by_angle(p10,p7,p3,p4,fr2,p6,(!l2type) ? trimmode : 0, p9)))
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMANGLE,0);
				goto exit;
			}
		}
		if (trimmode)
		{
			if (0==l1type && -1==cmd_modpts_4_chamfer(p1,p2,p10,p7,p5))
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMDISTLG1,0);
				ret=(-__LINE__);
				goto exit;
			}
			else if (1==l1type)	//a ray
			{
				if (((p2[l1maxdir]-p1[l1maxdir])*(p10[l1maxdir]-p1[l1maxdir]))>0.0 &&	//vector p1->p2 same dir as vector p1->p10
					((p1[l1maxdir]-p10[l1maxdir])*(p7[l1maxdir]-p10[l1maxdir]))<0.0)
				{	//vector p10->p1 not same dir as vector p10->p7
					//convert ray into a line
					ic_assoc(rbp1,0);
					if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL)
					{
						ret=(-__LINE__);
						goto exit;
					}
					strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));
					ic_ptcpy(p2,p10);
					makebit|=1;
					//p1 is ok as start of line,
				}
				else //it remains a ray, but we need to rework the start pt and vector
				{
					p2[0]-=p1[0];
					p2[1]-=p1[1];
					p2[2]-=p1[2];
					ic_ptcpy(p1,p10);
				}
			}
			else if (2==l1type)	//an xline, which will ALWAYS become a ray
			{
				makebit|=1;
				ic_assoc(rbp1,0);
				if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
				strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
				p2[0]-=p1[0];
				p2[1]-=p1[1];
				p2[2]-=p1[2];
				ic_ptcpy(p1,p10);
				if (p2[l1maxdir]*(p10[l1maxdir]-p7[l1maxdir])<0.0)
				{
					p2[0]=-p2[0];
					p2[1]=-p2[1];
					p2[2]=-p2[2];
				}
			}
			if (0==l2type && -1==cmd_modpts_4_chamfer(p3,p4,p9,p7,p6))
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMDISTLG2,0);
				ret=(-__LINE__);
				goto exit;
			}
			else if (1==l2type)	//a ray
			{
				if (((p4[l2maxdir]-p3[l2maxdir])*(p9[l2maxdir]-p3[l2maxdir]))>0.0 &&	//vector p3->p4 same dir as vector p3->p9
					((p3[l2maxdir]-p9[l2maxdir])*(p7[l2maxdir]-p9[l2maxdir]))<0.0)	//vector p9->p3 not same dir as vector p9->p7
				{
					//convert ray into a line
					ic_assoc(rbp2,0);
					if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL)
					{
						ret=(-__LINE__);
						goto exit;
					}
					strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));
					ic_ptcpy(p4,p9);
					makebit|=2;
					//p3 is ok as start of line,
				}
				else //it remains a ray, but we need to rework the start pt and vector
				{
					p4[0]-=p3[0];
					p4[1]-=p3[1];
					p4[2]-=p3[2];
					ic_ptcpy(p3,p9);
				}
			}
			else if (2==l2type)	//an xline, which will ALWAYS become a ray
			{
				makebit|=2;
				ic_assoc(rbp2,0);
				if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
				strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
				p4[0]-=p3[0];
				p4[1]-=p3[1];
				p4[2]-=p3[2];
				ic_ptcpy(p3,p9);
				if (p4[l2maxdir]*(p9[l2maxdir]-p7[l2maxdir])<0.0)
				{
					p4[0]=-p4[0];
					p4[1]=-p4[1];
					p4[2]=-p4[2];
				}
			}
			sds_trans(p1,&rbent,&rbwcs,0,p1);
			sds_trans(p2,&rbent,&rbwcs,(l1type==0)?0:1,p2);
			for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(rbtemp->resval.rpoint,p1);
				else if (rbtemp->restype==11)
					ic_ptcpy(rbtemp->resval.rpoint,p2);
			}
			sds_trans(p3,&rbent,&rbwcs,0,p3);
			sds_trans(p4,&rbent,&rbwcs,(l1type==0)?0:1,p4);
			for (rbtemp=rbp2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(rbtemp->resval.rpoint,p3);
				else if (rbtemp->restype==11)
					ic_ptcpy(rbtemp->resval.rpoint,p4);
			}
			//if we're okay to this point, go ahead and make/mod the ents...
			if (0==(makebit&1) && sds_entmod(rbp1)==RTNORM)
			{
				//don't do anything
			}
			else if ((makebit&1) && RTNORM==sds_entmake(rbp1))
			{
				sds_entdel(ename1);
				sds_entlast(ename1);
				sds_redraw(ename1,IC_REDRAW_DRAW);
			}
			else
			{
				ret=(-__LINE__);
				goto exit;
			}
			if (0==(makebit&2) && sds_entmod(rbp2)==RTNORM)
			{
				//don't do anything
			}
			else if ((makebit&2) && RTNORM==sds_entmake(rbp2))
			{
				sds_entdel(ename2);
				sds_entlast(ename2);
				sds_redraw(ename2,IC_REDRAW_DRAW);
			}
			else
			{
				ret=(-__LINE__);
				goto exit;
			}
		}
		if (!icadPointEqual(p9,p10))
		{
			sds_trans(p9,&rbent,&rbwcs,0,p9);
			sds_trans(p10,&rbent,&rbwcs,0,p10);
			rbp3=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,p10,11,p9,8,chamlayer,6,chamltype,62,chamcolor,0);
			if (sds_entmake(rbp3)!=RTNORM)
			{
				ret=(-__LINE__);
				goto exit;
			}
			sds_redraw(ename1,IC_REDRAW_DRAW);//new
			sds_redraw(ename2,IC_REDRAW_DRAW);//new
		}
	}
	else
	{
		//******************************************************
		//******CHAMFER A LINE TO A PLINE***********************
		//******************************************************
		//make sure rbp1 entity is the line, rbp2 entity is the pline
		if (!strsame(fs1,db_hitype2str(DB_LINE)))
		{
			ic_encpy(line_ent,ename2);
			ic_encpy(plent,ename1);
			rbp3=rbp1;
			rbp1=rbp2;
			rbp2=rbp3;
			rbp3=NULL;
			ic_ptcpy(p8,pt2);  //p8's a dummy point
			ic_ptcpy(p6,pt1);
			if (ptflag&2)fi1=1;
			else fi1=0;
			if (ptflag&1)fi1+=2;
			ptflag=fi1;
		}
		else
		{
			ic_encpy(plent,ename2);
			ic_encpy(line_ent,ename1);
			ic_ptcpy(p8,pt1);  //dummy
			ic_ptcpy(p6,pt2);
		}
		for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
		}
		if ((ptflag&1)==0)
		{
			pt1[0]=(p1[0]+p2[0])/2.0;
			pt1[1]=(p1[1]+p2[1])/2.0;
			pt1[2]=(p1[2]+p2[2])/2.0;
			ic_ptcpy(p5,pt1);
		}
		else
		{
			//get first estimate of p5 - move pt passed to f'n onto line
			ic_ptlndist(p8,p1,p2,&fr1,p5);
		}
		//find out which straight segment intersect w/line is closest to
		//	point selected on PLINE
		for (rbtemp=rbp2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==70)
				closure=rbtemp->resval.rint;
			else if (rbtemp->restype==210)
				ic_ptcpy(rbent.resval.rpoint,rbtemp->resval.rpoint);
		}
		rbent.restype=RT3DPOINT;
		//move all pts into ecs for pline
		sds_trans(p1,&rbwcs,&rbent,0,p1);
		sds_trans(p2,&rbwcs,&rbent,0,p2);
		if (!icadRealEqual(p1[2],p2[2]))
		{
			cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
			ret=RTERROR;
			goto exit;
		}
		sds_trans(p5,&rbwcs,&rbent,0,p5);

		FreeResbufIfNotNull(&rbp1);
		FreeResbufIfNotNull(&rbp2);
		sds_entnext_noxref(plent,etemp1);
		rbp1=sds_entget(etemp1);
		if ((ptflag&2)==0)
		{
			ic_assoc(rbp1,10);
			ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
			ic_ptcpy(p6,pt2);
		}
		else
		{
			rbwcs.resval.rint=1;
			sds_trans(pt2,&rbwcs,&rbent,0,pt2);
			rbwcs.resval.rint=0;
		}
		fi1=1;	//keep track of how many times we've stepped past plent
		fi2=-1;
		fr5=fr6=0;
		for (;;)
		{
			//fr1=min dist from pline pick pt to pline where
			// line crosses straight segment
			//fr2=min dist from pline pick pt to pline
			//fr3=bulge for current segment, p8=seg start, p9=seg end
			//fr4=current distance measuement for checking against fr1 & fr2
			//fi1=current vtx number
			//fi2=vtx number where fr1 is a minimum(vtx where line crosses pline)
			//fi3=vtx number where fr2 is a minimum(vtx where pline picked)
			//p6 was passed point to use for chamfer of pline
			//fr5=pline start width to use if making new pline from start
			//fr6=pline end width to use
			ic_assoc(rbp1,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
				break;
			sds_entnext_noxref(etemp1,etemp2);
			rbp2=sds_entget(etemp2);
			ic_assoc(rbp2,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
			{
				if (closure&1)
				{
					sds_entnext_noxref(plent,etemp2);
					sds_relrb(rbp2);
					rbp2=sds_entget(etemp2);
				}
				else
					break;
			}
			fr3=0.0;
			for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(p8,rbtemp->resval.rpoint);
				else if (rbtemp->restype==42)
					fr3=rbtemp->resval.rreal;
			}
			ic_assoc(rbp2,10);
			ic_ptcpy(p9,ic_rbassoc->resval.rpoint);
			//keep checking to see if this is the segment where p6 is closest to pline
			ic_ptsegdist(p6,p8,p9,fr3,&fr4);
			if ((fi1==1)||((fi1!=1)&&(fr4<=fr2)))  //needs <= for case where pl pick is on vertex pt
			{
				fr2=fr4;
				fi3=fi1;
			}
			//keep checking if this is the straight seg where intersect w/line closest to p6
			if (icadRealEqual(fr3,0.0))
			{
				if (ic_linexline(p1,p2,p8,p9,p10)>1)
				{
					fr4=sds_distance(p10,p6);
					if ((fi2==-1)||((fi2!=-1)&&(fr4<fr1)))
					{
						fr1=fr4;
						fi2=fi1;
						ic_ptcpy(p3,p8);	//start and endpoints of pline segment
						ic_ptcpy(p4,p9);
						ic_ptcpy(p7,p10);	//intersect point
						ic_encpy(etemp3,etemp1);
						fr5=fr6=0.0;
						if (trimmode)
						{
							for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
							{
								if (rbtemp->restype==40)
									fr5=rbtemp->resval.rreal;
								else if (rbtemp->restype==41)
									fr6=rbtemp->resval.rreal;
							}
						}
					}
				}
			}
			//NOTE:can't just move rbp1 to rbp2 - we may have skipped over a seqend marker
			FreeResbufIfNotNull(&rbp1);
			FreeResbufIfNotNull(&rbp2);
			sds_entnext_noxref(etemp1,etemp1);
			rbp1=sds_entget(etemp1);
			fi1++;
		}
		if (fi2==-1)
		{
			cmd_ErrorPrompt(CMD_ERR_CHAMPOLYARC,0);
//			sds_printf("\nNo intersection can be found between line and polyline line segments.");
			goto exit;
		}
		//we now have p1,p2,p3,p4,& p7.  We need to get chamfer pts
		if (fi2>fi3)
		{
			ic_ptcpy(p6,p3);
			fi1=0;		//build new pline from start
		}
		if (fi2<fi3)
		{
			ic_ptcpy(p6,p4);
			fi1=1;		//build new pline from end
		}
		if (fi2==fi3)
		{
			ic_ptlndist(p6,p3,p4,&fr4,p10);
			ic_ptcpy(p6,p10);
			if (sds_distance(p3,p7)>sds_distance(p3,p6))
			{
				fi1=0;	//build new pline from start
				//ic_ptcpy(p6,p3) not needed
			}
			else
			{
				fi1=1;	//build new pline from end
				//ic_ptcpy(p6,p4) not needed
			}
		}
		ic_encpy(etemp1,etemp3);	//copy name of vertex to work on to etemp1
		if (chammode)
		{
			SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
			SDS_getvar(NULL,DB_QCHAMFERD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}
		else
		{
			SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
			SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}

		if (trimmode)
		{
			//get the pline width at the chamfer point
			fr4=fr5+((fr6-fr5) * sds_distance(p3,p7) / sds_distance(p3,p4));
		}

		if (icadRealEqual(fr1,0.0))
		{
			//if chamfer dist is 0, p6 won't be needed.   Adjust line's end and start points.
			if (sds_distance(p5,p2)<sds_distance(p5,p1))
			{
				ic_ptcpy(p1,p5);
				ic_ptcpy(p5,p7);
			}
			else
			{
				ic_ptcpy(p2,p5);
				ic_ptcpy(p5,p7);
			}
		}
		else
		{
			if (strsame(fs1,db_hitype2str(DB_LINE)))	//if first entity passed was the line, chamfer line to pline
			//get the chamfer points, p10 & p6 - we'll later rename p10 to p5
			{
				sds_polar(p7,sds_angle(p7,p5),fr1,p10);
				ic_ptcpy(p5,p10);
				if (!chammode)
					sds_polar(p7,sds_angle(p7,p6),fr2,p10);
				else
				{
					if (0>(ret=cmd_champt_by_angle(p5,p7,p3,p4,fr2,p6,1,p10))) //int MUST be on pline
					{
						cmd_ErrorPrompt(CMD_ERR_CHAMANGLE,0);
						goto exit;
					}
				}
				ic_ptcpy(p6,p10);
			}
			else
			{
				sds_polar(p7,sds_angle(p7,p6),fr1,p10);
				ic_ptcpy(p6,p10);//don't reset p5 yet
				if (!chammode)
					sds_polar(p7,sds_angle(p7,p5),fr2,p10);
				else
				{
					if (0>(cmd_champt_by_angle(p6,p7,p1,p2,fr2,p5,trimmode,p10)))
					{
						cmd_ErrorPrompt(CMD_ERR_CHAMANGLE,0);
						goto exit;
					}
				}
				//switch the pts
				ic_ptcpy(p5,p10);
			}
			if ((fi1==0 && (sds_distance(p7,p3)<sds_distance(p7,p6)))||
				(fi1==1 && (sds_distance(p7,p4)<sds_distance(p7,p6))))
			{
				cmd_ErrorPrompt(CMD_ERR_CHAMTOOCLOSE,0);
				goto exit;
			}
			if (!trimmode)
			{
				if (rbp1!=NULL)
					sds_relrb(rbp1);
				sds_trans(p5,&rbent,&rbwcs,0,p9);
				sds_trans(p6,&rbent,&rbwcs,0,p10);
				if (NULL==(rbp1=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),8,chamlayer,6,chamltype,62,chamcolor,10,p9,11,p10,0)))
				{
					ret=(-__LINE__);
					goto exit;
				}
				ret=sds_entmake(rbp1);
				sds_redraw(ename1,IC_REDRAW_DRAW);//new
				sds_redraw(ename2,IC_REDRAW_DRAW);//new
				goto exit;
			}
			//adjust line's start and end points
			if (strsame(fs1,db_hitype2str(DB_LINE)))
			{
				if (-1==cmd_modpts_4_chamfer(p1,p2,p5,p7,pt1))
				{
					cmd_ErrorPrompt(CMD_ERR_CHAMDISTLG3,0);
					ret=RTERROR;
					goto exit;
				}
			}
			else
			{
				if (-1==cmd_modpts_4_chamfer(p1,p2,p5,p7,pt2))
				{
					cmd_ErrorPrompt(CMD_ERR_CHAMDISTLG3,0);
					ret=RTERROR;
					goto exit;
				}
			}
			//ic_ptcpy(p5,p10);
		}
		if (fi1==0)	 //if we're making new pline from start & the line will be at the end...
		{
			if (cmd_make_pline_start(plent,etemp1))	//make ent up to (but not including) etemp1
			{
				ret=(-__LINE__);
				goto exit;
			}
			FreeResbufIfNotNull(&rbp1);
			FreeResbufIfNotNull(&rbp2);
			rbp1=sds_entget(etemp1);
			if (!ic_assoc(rbp1,41))
				ic_rbassoc->resval.rreal=fr4;
			else
			{
				for (rbp2=rbp1; rbp2->rbnext!=NULL; rbp2=rbp2->rbnext);
				if ((rbp2->rbnext=sds_buildlist(41,fr4,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			if (sds_entmake(rbp1)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			FreeResbufIfNotNull(&rbp1);
			rbp2=NULL;
			if (icadRealEqual(fr1,0.0))
				ic_ptcpy(p6,p5);
			if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,40,fr4,41,fr4,42,0.0,0))==NULL)
			{
				ret=(-__LINE__);
				goto exit;
			}
			if (sds_entmake(rbp1)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			sds_relrb(rbp1);
			if (!icadRealEqual(fr1,0.0))
			{
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p5,40,fr4,41,fr4,42,0.0,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
				if (sds_entmake(rbp1)!=RTNORM)
				{
					ret=(-__LINE__);goto exit;
				}
				sds_relrb(rbp1); rbp1=NULL;
			}
			if (icadPointEqual(p1,p5))
			{
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,40,fr4,41,fr4,42,0.0,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			else
			{
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p1,40,fr4,41,fr4,42,0.0,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			if (sds_entmake(rbp1)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			FreeResbufIfNotNull(&rbp1);
			if ((rbp1=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
			{
				ret=(-__LINE__);
				goto exit;
			}
			if (sds_entmake(rbp1)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
		}
		else	//we're making the end of the pline - the line will be the start
		{
			//make header only
			sds_entnext_noxref(plent,etemp3);
			if (cmd_make_pline_start(plent,etemp3)!=0)
			{
				ret=(-__LINE__);
				goto exit;
			}
			if (icadPointEqual(p1,p5))
			{
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,42,0.0,40,fr4,41,fr4,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			else
			{
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p1,42,0.0,40,fr4,41,fr4,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			if (sds_entmake(rbp1)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			FreeResbufIfNotNull(&rbp1);
			if (icadRealEqual(fr1,0.0))  //if the next vtx will be the 1st one on the pline (
			{
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p5,42,0.0,40,fr4,41,fr6,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			else
			{
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p5,42,0.0,40,fr4,41,fr4,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			if (sds_entmake(rbp1)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			FreeResbufIfNotNull(&rbp1);
			if (!icadRealEqual(fr1,0.0))
			{
				if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,42,0.0,40,fr4,41,fr6,0))==NULL)
				{
					ret=(-__LINE__);
					goto exit;
				}
				if (sds_entmake(rbp1)!=RTNORM)
				{
					ret=(-__LINE__);goto exit;
				}
				FreeResbufIfNotNull(&rbp1);
			}
			rbp1=sds_entget(etemp3);
			ic_assoc(rbp1,10);
			cmd_make_pline_end(etemp1,closure&1,ic_rbassoc->resval.rpoint);
			FreeResbufIfNotNull(&rbp1);
		}
		sds_entdel(plent);
		sds_entdel(line_ent);
		sds_entlast(plent);
		sds_redraw(plent,IC_REDRAW_DRAW);
	}
	exit:
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	FreeResbufIfNotNull(&rbp3);
	return(ret);
}

short cmd_chamfer(void)
{

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR)	return RTERROR;

	int trimmode=0, anglemode=0;
	char fs1[IC_ACADBUF], fs2[IC_ACADBUF], fs3[IC_ACADBUF];
	int gotpoint,pickfirst=0,dirflag=-1;
	RT ret=0;
	long fi1,fi2;
	sds_name ename1,ename2,ename3;
	sds_point pt1,pt2;
	sds_real chamfera, chamferb, chamferang, chamferlen, fr1, fr2;
	struct resbuf rb, *rbp1, *rbp2;
	long vtxmode=0;

	ename1[0]=ename2[0]=ename3[0]=0L;

	rbp1=rbp2=NULL;

	ret=RTKWORD;
	while (ret==RTKWORD)
	{

		//get current chamferring vars
		if (SDS_getvar(NULL,DB_QTRIMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
			ret=-__LINE__;goto exit;
		}
		trimmode=rb.resval.rint;
		if (SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
			ret=-__LINE__;goto exit;
		}
		anglemode=rb.resval.rint;
		if (SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
			ret=-__LINE__;goto exit;
		}
		chamfera=rb.resval.rreal;
		if (SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
			ret=-__LINE__;goto exit;
		}
		chamferb=rb.resval.rreal;
		if (SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
			ret=-__LINE__;goto exit;
		}
		chamferlen=rb.resval.rreal;
		if (SDS_getvar(NULL,DB_QCHAMFERD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
			ret=-__LINE__;goto exit;
		}
		chamferang=rb.resval.rreal;	// in radians

		fs1[0]=0;
		//sds_initget(8,"~Angle ~Distance ~Method ~Trim Settings ~ Polyline");
		//if ((ret=(sds_entsel("\nChamfer:  Angle/Distance/Method/Trim/Last/Polyline/<Select first entity>: ",ename1,pt1))) == RTCAN) goto exit;
		if (rbp1!=NULL)
			sds_relrb(rbp1);
		rbp1=NULL;
		if ((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)
			goto exit;
		if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fi1) && fi1>0L)
			pickfirst=1;
		else
			pickfirst=0;

		if (anglemode)
		{
			sds_rtos(chamferlen,-1,-1,fs2);
			sds_angtos(chamferang,-1,-1,fs3);
			sprintf(fs1,ResourceString(IDC_CMDS4__NCHAMFER__LENGTH_42, "\nChamfer (length=%s, angle=%s):  Settings/Polyline/<Select first entity>: " ),fs2,fs3);
		}
		else
		{
			sds_rtos(chamfera,-1,-1,fs2);
			sds_rtos(chamferb,-1,-1,fs3);
			sprintf(fs1,ResourceString(IDC_CMDS4__NCHAMFER__DIST1__43, "\nChamfer (dist1=%s, dist2=%s):  Settings/Polyline/<Select first entity>: " ),fs2,fs3);
		}

		rb.rbnext=NULL;
		rb.restype=0;
		rb.resval.rstring="LINE,POLYLINE,LWPOLYLINE,RAY,XLINE,3DSOLID,BODY"/*DNT*/ ;
		IC_RELRB(rbp2);
		ret=sds_pmtssget(fs1,ResourceString(IDC_CMDS4__ANGLE__DISTANCE__44, "~Angle ~Distance ~Method ~Trim Chamfer_Settings...|Settings ~ Polyline ~_Angle ~_Distance ~_Method ~_Trim ~_Settings ~_ ~_Polyline " ),&rbp2,NULL,&rb,ename2,1);
		if (ret==RTCAN)
			goto exit;
		else if (ret==RTERROR)
		{
			if (rbp2==NULL)
				goto exit;
			else
			{
				sds_relrb(rbp2);
				rbp2=NULL;
				ret=RTKWORD;
				continue;
			}
		}

		if (ret==RTKWORD)
		{
			sds_getinput(fs1);
			if (strisame(fs1,"SETTINGS" ))
			{
				svar_ChildDialog=EM_CHAMFER;
				ExecuteUIAction( ICAD_WNDACTION_SETVAR );
			}
			else if (strisame(fs1,"POLYLINE" ))
			{
				for (;;)
				{
					ret=sds_entsel(ResourceString(IDC_CMDS4__NSELECT_2D_POLYL_47, "\nSelect 2D polyline to chamfer: " ),ename1,pt1);
					if (ret==RTCAN)
						return(ret);
					if (ret==RTERROR)
					{
						SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if (rb.resval.rint==OL_ENTSELPICK)
						{
							cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
							//continue;
						}
						else
							return(ret);
					}
					if (ret==RTNORM)
					{
						FreeResbufIfNotNull(&rbp1);
						if (NULL==(rbp1=sds_entget(ename1)))
							return(RTERROR);
						ic_assoc(rbp1,0);
						if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
						{
							ic_assoc(rbp1,70);
							if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
							{
								sds_relrb(rbp1); rbp1=NULL;
								cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
								//continue;
							}
							else
								break;
						}
						else if (!strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
						{
							sds_relrb(rbp1); rbp1=NULL;
							cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
							//continue;
						}
						else
							break;
					}
				}
				if (anglemode)
				{
					if (trimmode)
						ret=cmd_fillet_or_chamfer_pline(ename1,5,chamferlen,chamferang,NULL,NULL);
					else
						ret=cmd_fillet_or_chamfer_pline(ename1,6,chamferlen,chamferang,NULL,NULL);
				}
				else
				{
					if (trimmode)
						ret=cmd_fillet_or_chamfer_pline(ename1,3,chamfera,chamferb,NULL,NULL);
					else
						ret=cmd_fillet_or_chamfer_pline(ename1,4,chamfera,chamferb,NULL,NULL);
				}
				FreeResbufIfNotNull(&rbp1);
				return(ret);
			}
			else if (strisame(fs1,"DISTANCE" ))
			{
				rb.restype=RTREAL;
				rb.rbnext=NULL;
				sds_rtos(chamfera,-1,-1,fs1);
				sprintf(fs2,ResourceString(IDC_CMDS4__NCHAMFER_DISTANC_49, "\nChamfer distance on first entity <%s>: " ),fs1);
				sds_initget(RSG_NONEG,NULL);
				if (RTCAN==(ret=sds_getdist(NULL,fs2,&fr1)))
					goto exit;
				if (ret==RTNORM)
				{
					rb.resval.rreal=fr1;
					if (ret=(sds_setvar("CHAMFERA"/*DNT*/,&rb))!=RTNORM)
					{
						ret=-__LINE__;goto exit;
					}
					fr2=fr1;
				}
				else
					fr2=chamferb;
				sds_rtos(fr2,-1,-1,fs1);
				sds_initget(RSG_NONEG,NULL);
				sprintf(fs2,ResourceString(IDC_CMDS4__NCHAMFER_DISTANC_51, "\nChamfer distance on second entity <%s>: " ),fs1);
				if (RTCAN==(ret=sds_getdist(NULL,fs2,&fr1)))
					goto exit;
				if (ret==RTNORM)
					rb.resval.rreal=fr1;
				else  //RTNONE
					rb.resval.rreal=fr2;
				if (ret=(sds_setvar("CHAMFERB"/*DNT*/,&rb))!=RTNORM)
					goto exit;
				if (RTNORM==SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)&& rb.resval.rint==1)
				{
					rb.resval.rint=0;
					if (ret=(sds_setvar("CHAMMODE"/*DNT*/,&rb))!=RTNORM)
						goto exit;
					SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if (CMD_CMDECHO_PRINTF(rb.resval.rint)
						&& RTNORM==SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)
						&& rb.resval.rint<4)
						sds_printf(ResourceString(IDC_CMDS4__NCHAMFER_MODE_NO_54, "\nChamfer mode now set to Distance method." ));
				}
				goto exit;
			}
			else if (strisame(fs1,"ANGLE" ))
			{
				sds_rtos(chamferlen,-1,-1,fs1);
				rb.restype=RTREAL;
				rb.rbnext=NULL;
				sds_initget(RSG_NONEG,NULL);
				sprintf(fs2,ResourceString(IDC_CMDS4__NLENGTH_ON_FIRST_56, "\nLength on first line <%s>: " ),fs1);
				if (RTCAN==(ret=sds_getdist(NULL,fs2,&fr1))) goto exit;
				if (ret==RTNORM)
				{
					rb.resval.rreal=fr1;
					if (sds_setvar("CHAMFERC"/*DNT*/,&rb)!=RTNORM)
					{
						ret=-__LINE__;goto exit;
					}
				}
				SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				fi1=rb.resval.rint;
				SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (fi1==0)
					fr1=chamferang+rb.resval.rreal;
				else
					fr1=chamferang-rb.resval.rreal;
				if (fi1==1)
					fr1-=IC_TWOPI;
				for (;;)
				{
					sds_angtos(fr1,-1,-1,fs1);
					sds_initget(0,NULL);
					sprintf(fs2,ResourceString(IDC_CMDS4__NANGLE_RELATIVE__58, "\nAngle relative to first line <%s>: " ),fs1);
					if (RTCAN==(ret=sds_getorient(NULL,fs2,&rb.resval.rreal)))
						goto exit;
					if (ret==RTNORM)
					{
						if (rb.resval.rreal<0.0 || rb.resval.rreal>IC_PI)
						{
							cmd_ErrorPrompt(CMD_ERR_ANGLEPI,0);
							continue;
						}
						fr1=rb.resval.rreal;
						if (sds_setvar("CHAMFERD"/*DNT*/,&rb)!=RTNORM)
						{
							ret=-__LINE__;goto exit;
						}
					}
					else if (ret!=RTNONE)
						goto exit;
					break;
				}
				if (RTNORM==SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)&& rb.resval.rint==0)
				{
					rb.resval.rint=1;
					if (ret=(sds_setvar("CHAMMODE"/*DNT*/,&rb))!=RTNORM)
						goto exit;
					SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if (CMD_CMDECHO_PRINTF(rb.resval.rint)
						&& RTNORM==SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)
						&& rb.resval.rint<4)
						sds_printf(ResourceString(IDC_CMDS4__NCHAMFER_MODE_NO_60, "\nChamfer mode now set to Angle method." ));
				}
				goto exit;
			}
			else if (strisame(fs1,"TRIM" ))
			{
				sds_initget(8, ResourceString(IDC_CHAMFER_TRIM_INIT1,"Trim No_trim|No ~_TRIM ~_NO"));
				if (trimmode)
					sprintf(fs2,ResourceString(IDC_CMDS4__NNO_TRIM____TRIM_63, "\nNo trim/<Trim>: " ));
				else
					sprintf(fs2,ResourceString(IDC_CMDS4__NTRIM____NO_TRIM_64, "\nTrim/<No trim>: " ));
				if ((ret=sds_getkword(fs2,fs1))!=RTNORM && ret!=RTNONE)
					goto exit;
				if (ret==RTNORM)
				{
					if (strisame("TRIM" ,fs1))
						trimmode=1;
					else
						trimmode=0;
					rb.restype=RTSHORT;
					rb.rbnext=NULL;
					rb.resval.rint=trimmode;
					if (sds_setvar("TRIMMODE"/*DNT*/,&rb)!=RTNORM)
					{
						ret=-__LINE__;goto exit;
					}
				}
				ret=RTKWORD;
			}
			else if (strisame(fs1,"METHOD" ))
			{
				if (SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				{
					ret=-__LINE__;goto exit;
				}
				sds_initget(8, ResourceString(IDC_CHAMFER_METHOD_INIT1, "Angle Distance ~_ANGLE ~_DISTANCE"));
				if (rb.resval.rint)
					sprintf(fs2,ResourceString(IDC_CMDS4__NCHAMFER_USING___68, "\nChamfer using:  Distance/<Angle>: " ));
				else
					sprintf(fs2,ResourceString(IDC_CMDS4__NCHAMFER_USING___69, "\nChamfer using:  Angle/<Distance>: " ));
				if ((ret=sds_getkword(fs2,fs1))!=RTNORM && ret!=RTNONE)
					return(ret);
				if (ret==RTNORM)
				{
					if (strisame("ANGLE" ,fs1))
						rb.resval.rint=1;
					else
						rb.resval.rint=0;
					if (sds_setvar("CHAMMODE"/*DNT*/,&rb)!=RTNORM)
					{
						ret=-__LINE__;goto exit;
					}
				}
				ret=RTKWORD;
			}
			else if (strsame(fs1,"LAST" ))
			{
				sds_entlast(ename1);
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_entget(ename1))==NULL)
				{
					ret=-__LINE__;goto exit;
				}
				ic_assoc(rbp1,0);
				if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))
				{
					ic_assoc(rbp1,10);
					ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				}
				else if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
				{
					sds_entnext_noxref(ename1,ename2);
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_entget(ename2))==NULL)
					{
						ret=-__LINE__;goto exit;
					}
					ic_assoc(rbp1,10);
					ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				}
				else if (strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
				{
					((db_lwpolyline *)ename1[0])->get_10(pt1,0); // went straight to the hip -- faster
					// it'd be faster still if all of these did
				}
				FreeResbufIfNotNull(&rbp1);
				ret=RTNORM;
			}
		}
		if ((ret==RTNORM) && !strisame(fs1,"POLYLINE"/*DNT*/) && !strsame(fs1,"LWPOLYLINE"/*DNT*/))
		{
			//first, resolve what we got back from pmtssget...
			sds_sslength(ename2,&fi1);
			if (fi1==0L)
			{
				sds_ssfree(ename2);
				ename2[0]=0L;
				ret=RTKWORD;
				continue;
			}
			if (fi1>2L)
			{
				for (fi2=2;sds_ssname(ename2,fi2,ename1)==RTNORM;fi2++)
					sds_redraw(ename1,IC_REDRAW_UNHILITE);
			}
			if (fi1>=1L && sds_ssname(ename2,0L,ename1)!=RTNORM)
				return(-1);
			if (fi1>=2L && sds_ssname(ename2,1L,ename3)!=RTNORM)
				return(-1);
			sds_ssfree(ename2); ename2[0]=0L;
			if (rbp2!=NULL)
			{
				if (rbp2->restype==RTSTR && strsame(rbp2->resval.rstring,"S"/*DNT*/))
				{
					ic_ptcpy(pt1,rbp2->rbnext->resval.rpoint);
					gotpoint=1;
				}
				else
					//they picked using last, fence, etc w/o specifying a point
					gotpoint=0;
				FreeResbufIfNotNull(&rbp2);
			}
			else gotpoint=0;
			FreeResbufIfNotNull(&rbp1);

			// <alm> if selected entity is ACIS entity, process it in separate function
			{
				db_handitem* dbitem = reinterpret_cast<db_handitem*>(ename1[0]);
				if (dbitem->ret_type() == DB_3DSOLID || dbitem->ret_type() == DB_BODY)
				{
					// if cannot load acis modeler, select another object
					if (!CModeler::get()->checkFullModeler())
						continue;

					sds_redraw(ename1,IC_REDRAW_UNHILITE);
					int rc = cmd_do_chamfer_acis_entity(ename1, pt1);
					if (rc == RTPICKS) {
						ret=RTKWORD;
						continue;
					}
					else
						return rc;
				}
			}

			if ((rbp1=sds_entget(ename1))==NULL)
				return(RTERROR);
			ic_assoc(rbp1,0);
			if (RTNORM!=sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,POLYLINE,LWPOLYLINE,RAY,XLINE,CIRCLE"/*DNT*/))
			{
				sds_redraw(ename1,IC_REDRAW_DRAW);
				cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
				ret=RTKWORD;
				continue;
			}
			if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/) || strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
			{
				ic_assoc(rbp1,70);
				if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
				{
					sds_relrb(rbp1); rbp1=NULL;
					cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
					sds_redraw(ename1,IC_REDRAW_DRAW);
					ret=RTKWORD;
					continue;
				}
				else if (ic_rbassoc->resval.rint&1)
					dirflag=0;
			}

			if (1==fi1 && 1==pickfirst)
			{
				sds_redraw(ename1,IC_REDRAW_HILITE);
				ic_assoc(rbp1,0);
				if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ ) || strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
				{
					pickfirst=2;//we can chamfer this pline later
					ic_assoc(rbp1,70);
					if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
					{
						if (anglemode)
						{
							if (trimmode)
								ret=cmd_fillet_or_chamfer_pline(ename1,5,chamferlen,chamferang,NULL,NULL);
							else
								ret=cmd_fillet_or_chamfer_pline(ename1,6,chamferlen,chamferang,NULL,NULL);
						}
						else
						{
							if (trimmode)
								ret=cmd_fillet_or_chamfer_pline(ename1,3,chamfera,chamferb,NULL,NULL);
							else
								ret=cmd_fillet_or_chamfer_pline(ename1,4,chamfera,chamferb,NULL,NULL);
						}
						return(ret);
					}
					else pickfirst=2;//we can chamfer this pline later
				}
			}
			if (fi1<2L)
				ret=RTERROR;
			else
			{
				ret=RTNORM;
				ic_encpy(ename2,ename3);
				fi1=0;
			}
			while (ret!=RTNORM)
			{
				//if ((ret=sds_entsel("\nSelect second entity: ",ename2,pt2))!=RTNORM) return(ret);
				pick2:
				for (;;)
				{
					FreeResbufIfNotNull(&rbp2);
					if (2==pickfirst) //if pre-selected an open pline
						ret=sds_pmtssget(ResourceString(IDC_CMDS4__NPOLYLINE____SEL_73, "\nPolyline/<Select second entity>: " ),"Polyline ~_Polyline"/*DNT*/ ,&rbp2,NULL,NULL,ename3,1);
					else if (0==dirflag)	//if first ent a closed pline
						ret=sds_pmtssget(ResourceString(IDC_CMDS4__NDIRECTIONAL_____75, "\nDirectional/<Select second entity>: " ),"Directional ~_Directional"/*DNT*/ ,&rbp2,NULL,NULL,ename3,1);
					else
						ret=sds_pmtssget(ResourceString(IDC_CMDS4__NSELECT_SECOND_E_77, "\nSelect second entity: " ),""/*DNT*/,&rbp2,NULL,NULL,ename3,1);
					if (ret==RTCAN)
						goto exit;
					if (ret==RTERROR)
					{
						if (rbp2==NULL)
							goto exit;
						FreeResbufIfNotNull(&rbp2);	//else, set empty so continue;
					}
					else if (ret==RTKWORD)
					{
						sds_getinput(fs2);
						if (strisame(fs2,"DIRECTIONAL" ))
						{
							dirflag=1;
							continue;
						}
						//else "Polyline"...
						if (anglemode)
						{
							if (trimmode)
								ret=cmd_fillet_or_chamfer_pline(ename1,5,chamferlen,chamferang,NULL,NULL);
							else
								ret=cmd_fillet_or_chamfer_pline(ename1,6,chamferlen,chamferang,NULL,NULL);
						}
						else
						{
							if (trimmode)
								ret=cmd_fillet_or_chamfer_pline(ename1,3,chamfera,chamferb,NULL,NULL);
							else
								ret=cmd_fillet_or_chamfer_pline(ename1,4,chamfera,chamferb,NULL,NULL);
						}
						return(ret);
					}
					if ((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)
						goto exit;
					if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fi2) && fi2>1L)
					{
						sds_ssname(SDS_CURDOC->m_pGripSelection,1L,ename2);
						sds_relrb(rbp2); rbp2=NULL;
						gotpoint=0;
					}
					else
					{
						if (fi1>1L)
							for (fi1=1;sds_ssname(ename3,fi1,ename2)==RTNORM;fi1++)
								sds_redraw(ename2,IC_REDRAW_UNHILITE);
						if (sds_ssname(ename3,0L,ename2)!=RTNORM)
							return(-1);
					}
					sds_ssfree(ename3);
					if (rbp2!=NULL)
					{
						if (rbp2->restype==RTSTR && strsame(rbp2->resval.rstring,"S"/*DNT*/))
						{
							ic_ptcpy(pt2,rbp2->rbnext->resval.rpoint);
							fi1=gotpoint+2;			//both pts are valid
						}
						else
							fi1=gotpoint;			//2nd pt not valid
					}
					else
						fi1=gotpoint;//happens on previous picks w/grips
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_entget(ename2))==NULL)
						return(RTERROR);
					ic_assoc(rbp1,0);
					if (RTNORM!=sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,POLYLINE,LWPOLYLINE,RAY,XLINE,CIRCLE"/*DNT*/))
					{
						sds_redraw(ename2,IC_REDRAW_DRAW);
						cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
						continue;
					}
					if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
					{
						ic_assoc(rbp1,70);
						if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
						{
							FreeResbufIfNotNull(&rbp1);
							cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
							sds_redraw(ename2,IC_REDRAW_DRAW);
							continue;
						}
					}
					if (ret==RTNORM)
						break;
				}
			}  //end while for second entity selection
		}
	}  //end while for first entity & options selection
	if (1==dirflag)
		fi1|=8;
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	if (0>(ret=cmd_do_chamfer(ename1,pt1,ename2,pt2,fi1)))	// 0=repeats for plines, RTNORM=repeats for lines
	{
		sds_redraw(ename2,IC_REDRAW_DRAW);
		pickfirst=0;
		goto pick2;
	}

	exit:
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	rbp1=rbp2=NULL;
	if (ret==0)
		ret=RTNORM;
	if (ret==-1)
		ret=RTERROR;
	if (ret<-1 && ret>-5000)
	{
		CMD_INTERNAL_MSG(ret);
		return(RTERROR);
	}
	if (ret!=RTNORM)
	{
		if (ename1[0]!=0L)
			sds_redraw(ename1,IC_REDRAW_DRAW);
		if (ename2[0]!=0L)
			sds_redraw(ename2,IC_REDRAW_DRAW);
	}
	return(ret);
}

// Class implements filleting of ACIS entities
class CCmdFilletAcisEntity
{
public:
    /*-------------------------------------------------------------------------*//**
    Ctor
    @author Alexey Malov
	@param obj => selected ACIS entity
	@param pt => picked point
	@param bUsePoint => defines can we use passes picked ponit or not
    *//*--------------------------------------------------------------------------*/
	CCmdFilletAcisEntity
	(
	sds_name obj, 
	const point& pt, 
	bool bUsePoint
	);

    /*-------------------------------------------------------------------------*//**
    Dtor
    @author Alexey Malov
    *//*--------------------------------------------------------------------------*/
	~CCmdFilletAcisEntity();

    /*-------------------------------------------------------------------------*//**
    Run command
    @author Alexey Malov
	@return standard error code (RTNORM if command succesful)
    *//*--------------------------------------------------------------------------*/
	int run();

private:

    /*-------------------------------------------------------------------------*//**
    Select edges untill error or end of user input
    @author Alexey Malov
	@return void for end of user input, 
			in case of RTERROR, RTCAN exception will be generated
    *//*--------------------------------------------------------------------------*/
	void select();

    /*-------------------------------------------------------------------------*//**
    Call ACIS modeler to find selected edge
    @author Alexey Malov
	@return void,
			in case of error generate exception
    *//*--------------------------------------------------------------------------*/
	void pick();

    /*-------------------------------------------------------------------------*//**
    User interface for asking fillet radius
    @author Alexey Malov
	@return void,
			in case of error generate exception
    *//*--------------------------------------------------------------------------*/
	void getFilletRadius();

    /*-------------------------------------------------------------------------*//**
    Unhighlight all edges of given ACIS entity
    @author Alexey Malov
	@return void,
			in case of error generate exception
    *//*--------------------------------------------------------------------------*/
	void unhighlight(const CDbAcisEntityData&);

	// selected Acis entity
	sds_name m_ent;
	// old data of selected ACIS entity
	CDbAcisEntityData m_oldData;
	// new data of selected ACIS entity
	CDbAcisEntityData m_newData;
	
	// picked point
	point m_pt;
	// flag defining can we use pick point or not
	bool m_bUsePoint;

	// selected edges with associated fillet radius
	vector<pair<HEDGE, double> > m_aBlendEdges;

	// flag defining selection mode: only edge or edge chain
	bool m_bSelectChain;

	// current fillet radius
	double m_dRadius;

	// modeler
	CModeler* m_pModeler;
};

CCmdFilletAcisEntity::CCmdFilletAcisEntity(sds_name obj, const point& pt, bool bUsePoint)
:
m_pt(pt),
m_bUsePoint(bUsePoint),
m_bSelectChain(false),
m_dRadius(0.),
m_pModeler(CModeler::get())
{
	ic_encpy(m_ent, obj);

	CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(m_ent[0]);
	m_oldData = pObj->getData();

	m_pModeler->lockDatabase();
}

CCmdFilletAcisEntity::~CCmdFilletAcisEntity()
{
	m_pModeler->unlockDatabase();
}

int CCmdFilletAcisEntity::run()
{
	try
	{
		// Run commands while any error situation is not occured

		// Pick first edge
		if (m_bUsePoint)
		{
			pick();
		}

		// Get fillet radius for first edge
		getFilletRadius();

		// Update first edge record with proper radius value
		if (!m_aBlendEdges.empty())
		{
			m_aBlendEdges.front().second = m_dRadius;
		}

		// Select edges
		select();

		// Call modeler
		if (m_pModeler->fillet(m_oldData, m_newData, getCurrColorIndex(), m_aBlendEdges))
		{
			// Update blank solid
			resbuf* p = sds_entget(m_ent);
			ic_assoc(p, 1);
			char*& dxf1 = ic_rbassoc->resval.rstring;
			IC_FREE(dxf1);			
			dxf1 = new char [m_newData.getSat().size()+1];
			strcpy(dxf1, m_newData.getSat().c_str());

			sds_entmod(p);

			return RTNORM;
		}
		else
		{
			printAcisError();

			unhighlight(m_oldData);

			return RTERROR;
		}

	}
	catch (int rc)
	{
		unhighlight(m_oldData);
		return rc;
	}
	return RTNORM;
}

void CCmdFilletAcisEntity::select()
{
	do
	{
		const char* szVariants1 = "Chain Radius ~Chain ~Radius";
		const char* szVariants2 = "Edge Radius ~Edge ~Radius";
		const char* szVariants = m_bSelectChain? szVariants2: szVariants1;
		checkSuccess(sds_initget(0, szVariants), RTNORM, 0);

		const char* szPrompt1 = "\nSelect an edge: Chain/Radius: ";
		const char* szPrompt2 = "\nSelect an edge chain: Edge/Radius: ";
		const char* szPrompt = m_bSelectChain? szPrompt2: szPrompt1;

		SDS_SetCursor(SDS_GetPickCursor());
		int rc = checkSuccess(SDS_AskForPoint(0, szPrompt, m_pt), RTNORM, RTNONE, RTKWORD, 0);

		switch (rc)
		{
		case RTNORM:
			pick();
			break;
		case RTKWORD:
			{
				char szAnswer[IC_ACADBUF];
				checkSuccess(sds_getinput(szAnswer), RTNORM, 0);

				if (strisame(szAnswer, "RADIUS"/*DNT*/))
				{
					getFilletRadius();
				}
				else if (strisame(szAnswer, "CHAIN"/*DNT*/))
		   		{
					m_bSelectChain = true;
				}
				else if (strisame(szAnswer, "EDGE"/*DNT*/))
				{
					m_bSelectChain = false;
				}
			}
			break;
		case RTNONE:
			return;
		default:
			assert(false);
		}
	}
	while (true);
}

void CCmdFilletAcisEntity::pick()
{
    icl::gvector dir;
    {
        resbuf rb;
        sds_getvar("VIEWDIR"/*DNT*/, &rb);
        dir = rb.resval.rpoint;

        icl::transf t;
        if (!ucs2wcs(t))
            throw RTERROR;

        dir *= t;
		m_pt *= t;
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

	int mode = m_bSelectChain? CModeler::eChain: CModeler::eOnly;
    xstd<HEDGE>::vector selected;
	if (m_pModeler->pickEdge(m_oldData, m_pt, dir, prec, mode, selected))
    {
		if (!selected.empty())
		{
			xstd<HEDGE>::vector::const_iterator iedge;
			for (iedge = selected.begin(); iedge != selected.end(); ++iedge)
			{
				HEDGE hedge = *iedge;
				bool bGood;
				if (!m_pModeler->isEdgeAppropriateForBlending(m_oldData, hedge, bGood))
					throw RTERROR;
				if (bGood)
				{
					// check existence of the edge in vector
					std::vector<pair<HEDGE,double> >::iterator ibedge;
					for (ibedge = m_aBlendEdges.begin();
						 ibedge != m_aBlendEdges.end(); ++ibedge)
					{
						if (ibedge->first == hedge)
						{
							ibedge->second = m_dRadius;
							break;
						}
					}
					if (ibedge == m_aBlendEdges.end())
					{
						// strore selected edge into internal container
						pair<HEDGE, double> blendEdge(hedge, m_dRadius);
						m_aBlendEdges.push_back(blendEdge);
					}

					// highlight selected edge
					m_pModeler->highlight(m_oldData, hedge);

					// redraw entity
					db_handitem* dbitem = reinterpret_cast<db_handitem*>(m_ent[0]);
					gr_displayobject* dobj = reinterpret_cast<gr_displayobject*>(dbitem->ret_disp());
					gr_freedisplayobjectll(dobj);
					dbitem->set_disp(0);

					sds_redraw(m_ent, IC_REDRAW_DRAW);
				}
				else
				{
					sds_printf("\nCannot blend this edge.");
				}
			}
		}
    }
}

void CCmdFilletAcisEntity::getFilletRadius()
{
	// Input fillet radius

	// Get default value, from system variable
	resbuf rb;
	checkSuccess(SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES),
		RTNORM, 0);
	m_dRadius = rb.resval.rreal;

	char szDefaultRadius[IC_ACADBUF];
	char szPrompt[IC_ACADBUF];
	sds_rtos(rb.resval.rreal,-1,-1,szDefaultRadius);
	sprintf(szPrompt,
			ResourceString(IDC_CMDS4__NFILLET_RADIUS___84, "\nFillet radius <%s>: "),
			szDefaultRadius);

	sds_initget(4, 0);
	int rc = checkSuccess(sds_getdist(0, szPrompt, &m_dRadius), RTNORM, RTNONE, 0);
	
	if (rc == RTNORM)
	{
		rb.restype = RTREAL;
		rb.rbnext = 0;
		rb.resval.rreal = m_dRadius;
		checkSuccess(sds_setvar("FILLETRAD"/*DNT*/, &rb), RTNORM, 0);
	}
}

void CCmdFilletAcisEntity::unhighlight(const CDbAcisEntityData& obj)
{
	m_pModeler->unhighlight(obj, 0);

	// regenerate display object  
	db_handitem* dbitem = reinterpret_cast<db_handitem*>(m_ent[0]);
	gr_displayobject* dobj = reinterpret_cast<gr_displayobject*>(dbitem->ret_disp());
	gr_freedisplayobjectll(dobj);
	dbitem->set_disp(0);

	sds_redraw(m_ent, IC_REDRAW_DRAW);
}

short cmd_fillet_acis_entity(sds_name ent, const point& pickpt, int good_point)
{
	CModeler* pModeler = CModeler::get();
	if (!pModeler->checkFullModeler())
		return 0;
	
	sds_redraw(ent,IC_REDRAW_UNHILITE);

	pModeler->lockDatabase();

	CCmdFilletAcisEntity cmd(ent, pickpt, good_point == 1);
	cmd.run();

	pModeler->unlockDatabase();

	return 1;
}

short cmd_fillet(void)
{

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR)	return RTERROR;

	int trimmode,gotpoint;
	char fs1[IC_ACADBUF], fs2[IC_ACADBUF];
	long vtxmode=0,fl1,fl2;
	int pickfirst,dirflag=-1;
	RT  ret;
	sds_name ent1,ent2,ent3;
	sds_real fr1,filletrad;
	sds_point ptsel1,ptsel2;
	struct resbuf rb, *rbp1;

	rbp1=NULL;
	SDS_getvar(NULL,DB_QTRIMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	trimmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_rtos(rb.resval.rreal,-1,-1,fs1);
	filletrad=rb.resval.rreal;

	ret = RTNORM;
	while (ret!=RTERROR && ret!=RTCAN)
	{
		SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L)
			pickfirst=1;
		else
			pickfirst=0;
		FreeResbufIfNotNull(&rbp1);

		if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			return(RTERROR);
		sds_rtos(rb.resval.rreal,-1,-1,fs2);
		sprintf(fs1,ResourceString(IDC_CMDS4__NFILLET__RADIUS__80, "\nFillet (radius=%s):  Settings/Polyline/<Select first entity>: " ),fs2);
		if ((ret=sds_pmtssget(fs1,ResourceString(IDC_CMDS4__RADIUS__TRIM_FIL_81, "~Radius ~Trim Fillet_Settings...|Settings ~ Polyline ~_Radius ~_Trim ~_Settings ~_ ~_Polyline " ),&rbp1,NULL,NULL,ent2,1))==RTCAN)
			return(ret);
		if (ret==RTERROR)
		{
			if (rbp1==NULL)
				return(ret);	//right click
			else
			{						//empty sset
				FreeResbufIfNotNull(&rbp1);
				ret=RTNORM;
				continue;
			}
		}
		if (ret==RTKWORD)
		{
			sds_getinput(fs1);
			strupr(fs1);
			if (strisame(fs1,"SETTINGS" ))
			{
				svar_ChildDialog=EM_CHAMFER;
				ExecuteUIAction( ICAD_WNDACTION_SETVAR );
			}
			else if (strisame(fs1,"POLYLINE" ))
			{
				for (;;)
				{
					if ((ret=sds_pmtssget(ResourceString(IDC_CMDS4__NSELECT_2D_POLYL_82, "\nSelect 2D polyline to fillet: " ),"~ ~_"/*DNT*/,&rbp1,NULL,NULL,ent2,1))==RTCAN)
						return(ret);
					//ret=sds_entsel("\nSelect 2D polyline to fillet: ",ent1,ptsel1);
// I don't understand why we're NOT using entsel here, but in any event if we're going to do it this way
// we need to free the selection set, which I have added below
					if (ret==RTERROR)
					{
						if (NULL==rbp1)
							return(ret);	//right click
						FreeResbufIfNotNull(&rbp1);
						continue;
					}
					if (ret==RTNORM)
					{
						sds_sslength(ent2,&fl1);
						if (fl1!=1L)
							return(RTERROR);
						sds_ssname(ent2,0L,ent1);
						sds_ssfree(ent2);
						FreeResbufIfNotNull(&rbp1);
						if (NULL==(rbp1=sds_entget(ent1)))
							return(RTERROR);
						ic_assoc(rbp1,0);
						if (!strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ ) && !strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
						{
							FreeResbufIfNotNull(&rbp1);
							cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
							//continue;
						}
						else
						{
							ic_assoc(rbp1,70);
							if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
							{
								FreeResbufIfNotNull(&rbp1);
								cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
								//continue;
							}
							else
							{
								FreeResbufIfNotNull(&rbp1);
								break;
							}
						}
						FreeResbufIfNotNull(&rbp1);
					}
				}
				if (trimmode)
					ret=cmd_fillet_or_chamfer_pline(ent1,1,filletrad,0.0,NULL,NULL);
				else
					ret=cmd_fillet_or_chamfer_pline(ent1,2,filletrad,0.0,NULL,NULL);
				return(ret);
			}
			else if (strisame(fs1,"RADIUS" ))
			{
				if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
					return(RTERROR);
				sds_rtos(rb.resval.rreal,-1,-1,fs2);
				sprintf(fs1,ResourceString(IDC_CMDS4__NFILLET_RADIUS___84, "\nFillet radius <%s>: " ),fs2);
				sds_initget(4,NULL);
				if (RTCAN==(ret=sds_getdist(NULL,fs1,&fr1)))
					return(ret);
				if (ret==RTNORM)
				{
					rb.restype=RTREAL;
					rb.rbnext=NULL;
					rb.resval.rreal=fr1;
					if (sds_setvar("FILLETRAD"/*DNT*/,&rb)!=RTNORM)
						return(RTERROR);
				}
				return(RTNORM);
			}
			else if (strisame(fs1,"TRIM" ))
			{
				sds_initget(0, ResourceString (IDC_FILLET_TRIM_INIT1, "Trim No ~_TRIM ~_NO"));
				sprintf(fs2,ResourceString(IDC_CMDS4__NTRIM___NO_TRIM__87, "\nTrim/No trim <%s>: " ),trimmode ? ResourceString(IDC_CMDS4_TRIM_61, "Trim" ) : ResourceString(IDC_CMDS4_NO_TRIM_88, "No trim" ));
				if ((ret=sds_getkword(fs2,fs1))==RTCAN)
					return(ret);
				if (ret==RTNORM)
				{
					if (strisame("TRIM" ,fs1))
						trimmode=1;
					else
						trimmode=0;
					rb.restype=RTSHORT;
					rb.rbnext=NULL;
					rb.resval.rint=trimmode;
					if (sds_setvar("TRIMMODE"/*DNT*/,&rb)!=RTNORM)
						return(RTERROR);
					ret=RTKWORD;
				}
			}
		}
		if (ret==RTNORM)
		{
			//first, resolve what we got back from pmtssget...
			sds_sslength(ent2,&fl1);
			if (fl1==0L)
			{
				sds_ssfree(ent2);
				continue;
			}
			if (fl1>2L)
				for (fl2=2;sds_ssname(ent2,fl2,ent1)==RTNORM;fl2++)
					sds_redraw(ent1,IC_REDRAW_UNHILITE);
			if (sds_ssname(ent2,0L,ent1)!=RTNORM)
				return(-1);
			if (fl1>1L && sds_ssname(ent2,1L,ent3)!=RTNORM)
				return(-1);

			sds_ssfree(ent2);
			if (fl1>1L)
				ic_encpy(ent2,ent3);
			gotpoint=0;
			if (rbp1!=NULL)
			{
				if (rbp1->restype==RTSTR && strisame(rbp1->resval.rstring,"S"/*DNT*/))
				{
					ic_ptcpy(ptsel1,rbp1->rbnext->resval.rpoint);
					gotpoint=1;
				}
				FreeResbufIfNotNull(&rbp1);
			}
			FreeResbufIfNotNull(&rbp1);

			//<alm> if selected object is ACIS entity, process it in separate function
			{
				db_handitem* dbitem = reinterpret_cast<db_handitem*>(ent1[0]);
				if (dbitem->ret_type() == DB_3DSOLID || dbitem->ret_type() == DB_BODY)
				{
					// if cannot load modeler, select another entity
					if (!CModeler::get()->checkFullModeler())
						continue;

					point v = ptsel1;
					if (cmd_fillet_acis_entity(ent1, v, gotpoint))
						return RTNORM;
					else
						return RTERROR;
				}
			}
			//</alm>

			if ((rbp1=sds_entget(ent1))==NULL)
				return(RTERROR);
			ic_assoc(rbp1,0);
			if (RTNORM!=sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,POLYLINE,LWPOLYLINE,RAY,XLINE,CIRCLE"/*DNT*/))
			{
				FreeResbufIfNotNull(&rbp1);
				sds_redraw(ent1,IC_REDRAW_DRAW);
				cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
				continue;
			}
			if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/) || strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
			{
				ic_assoc(rbp1,70);
				if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
				{
					FreeResbufIfNotNull(&rbp1);
					cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
					sds_redraw(ent1,IC_REDRAW_DRAW);
					continue;
				}
				else if (ic_rbassoc->resval.rint&1)
					dirflag=0;
			}

			if (1==fl1 && 1==pickfirst)
			{
				sds_redraw(ent1,IC_REDRAW_HILITE);
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_entget(ent1))==NULL)
					return(RTERROR);
				ic_assoc(rbp1,0);
				if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ ) || strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
				{
					ic_assoc(rbp1,70);
					if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
					{
						FreeResbufIfNotNull(&rbp1);
						cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
						continue;
					}
					else if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
					{
						if (trimmode)
							ret=cmd_fillet_or_chamfer_pline(ent1,1,filletrad,0.0,NULL,NULL);
						else
							ret=cmd_fillet_or_chamfer_pline(ent1,2,filletrad,0.0,NULL,NULL);
						return(ret);
					}
					else pickfirst=2;
				}
			}

			if (fl1<2L)
			{
				for (;;)
				{
					FreeResbufIfNotNull(&rbp1);
					if (2==pickfirst)	//pre-selected an open pline
						ret=sds_pmtssget(ResourceString(IDC_CMDS4__NPOLYLINE___SELE_89, "\nPolyline/Select second entity: " ),"Polyline ~_Polyline"/*DNT*/,&rbp1,NULL,NULL,ent3,1);
					else if (0==dirflag)	//1st ent is a closed pline
						ret=sds_pmtssget(ResourceString(IDC_CMDS4__NDIRECTIONAL___S_90, "\nDirectional/Select second entity: " ),"Directional ~_Directional" ,&rbp1,NULL,NULL,ent3,1);
					else
						ret=sds_pmtssget(ResourceString(IDC_CMDS4__NSELECT_SECOND_E_77, "\nSelect second entity: " ),""/*DNT*/,&rbp1,NULL,NULL,ent3,1);
					if (ret==RTCAN)
					{
						sds_redraw(ent1,IC_REDRAW_UNHILITE);
						return(ret);
					}
					if (ret==RTERROR)
					{
						if (rbp1==NULL)
						{
							sds_redraw(ent1,IC_REDRAW_UNHILITE);
							return(ret);	//right click
						}
						FreeResbufIfNotNull(&rbp1);		//empty set of objects
						continue;
					}
					else if (ret==RTKWORD)
					{
						sds_getinput(fs2);
						strupr(fs2);
						if (strsame(fs2,"DIRECTIONAL" ))
						{
							dirflag=1;
							continue;
						}
						//else "Polyline"...
						if (trimmode)
							ret=cmd_fillet_or_chamfer_pline(ent1,1,filletrad,0.0,NULL,NULL);
						else
							ret=cmd_fillet_or_chamfer_pline(ent1,2,filletrad,0.0,NULL,NULL);
						return(ret);
					}
					if ((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)
					{
						FreeResbufIfNotNull(&rbp1);
						return(-1);
					}
					if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl2) && fl2>1L)
					{
						sds_ssname(SDS_CURDOC->m_pGripSelection,1L,ent2);
						FreeResbufIfNotNull(&rbp1);
						gotpoint=0;
					}
					else
					{
						if (fl1>1L)
							for (fl1=0; sds_ssname(ent3,fl1,ent2)==RTNORM; fl1++)
								sds_redraw(ent1,IC_REDRAW_UNHILITE);
						if (sds_ssname(ent3,0L,ent2)!=RTNORM)
						{
							sds_ssfree(ent3);
							continue;//nothing in ent3 selection set
						}
					}

					sds_ssfree(ent3);
					if (rbp1!=NULL)
					{
						if (rbp1->restype==RTSTR && strsame(rbp1->resval.rstring,"S"/*DNT*/))
						{
							ic_ptcpy(ptsel2,rbp1->rbnext->resval.rpoint);
							gotpoint+=2;
						}
						FreeResbufIfNotNull(&rbp1);
					}
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_entget(ent2))==NULL)
						return(RTERROR);
					ic_assoc(rbp1,0);
					if (RTNORM!=sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,POLYLINE,RAY,XLINE,CIRCLE,LWPOLYLINE"/*DNT*/))
					{
						FreeResbufIfNotNull(&rbp1);
						sds_redraw(ent2,IC_REDRAW_DRAW);
						cmd_ErrorPrompt(CMD_ERR_LINEARCCIRPLY,0);
						continue;
					}
					if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
					{
						ic_assoc(rbp1,70);
						if (ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
						{
							sds_relrb(rbp1); rbp1=NULL;
							cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
							sds_redraw(ent2,IC_REDRAW_DRAW);
							continue;
						}
					}
					if (ret==RTNORM)
						break;
				}
			}
			if (1==dirflag)
				gotpoint|=8;
			if (ret=cmd_do_fillet(ent1,ptsel1,ent2,ptsel2,gotpoint)!=0)
			{
				FreeResbufIfNotNull(&rbp1);
				sds_redraw(ent1,IC_REDRAW_DRAW);
				sds_redraw(ent2,IC_REDRAW_DRAW);
				return(RTERROR);
			}
			FreeResbufIfNotNull(&rbp1);
			return(RTNORM);
		}
	}
	return(RTNORM);
}



// Compute 2d point - line distance
// 
// Author:  Stephen W. Hou
// Date:	4/29/2003
//
sds_real cmdPointLineDistance(sds_point start, ads_point end, sds_point pt)
{
	sds_real yDiff = end[1] - start[1]; 
	sds_real xDiff = end[0] - start[0]; 

	ASSERT(!icadRealEqual(yDiff, 0.0) || !icadRealEqual(xDiff, 0.0));

	sds_real a = yDiff;
	sds_real b = - xDiff;
	sds_real c = xDiff * start[1] - yDiff * start[0];
	return fabs(a * pt[0] + b * pt[1] + c) / sqrt(a * a + b * b);
}

//  Compute 2d line and line intersection and return true the intersection is found, and
//  false otherwise.
//
//	Author:	Stephen W. Hou
//  Date:	4/29/2003
//
bool cmdCompute2dIntersection(sds_point xPoint,					// Out: Intersection point
							  sds_point start1, sds_point end1, // In:	The first line
							  sds_point start2, sds_point end2)	// In:	The second line
{
	sds_real xDiff1 = end1[0] - start1[0];
	sds_real yDiff1 = end1[1] - start1[1];

	sds_real xDiff2 = end2[0] - start2[0];
	sds_real yDiff2 = end2[1] - start2[1];

	if (icadRealEqual(xDiff1, 0.0)) {
		ASSERT(!icadRealEqual(xDiff2, 0.0));     

		sds_real k2 = yDiff2 / xDiff2;

		xPoint[0] = start1[0];
		xPoint[1] = k2 * xPoint[0] + start2[1] - k2 * start2[0];
		xPoint[2] = start1[2];
	}
	else if (icadRealEqual(xDiff2, 0.0)) {
		ASSERT(!icadRealEqual(xDiff1, 0.0));     

		sds_real k1 = yDiff1 / xDiff1;

		xPoint[0] = start2[0];
		xPoint[1] = k1 * xPoint[0] + start1[1] - k1 * start1[0];
		xPoint[2] = start2[2];
	}
	else {
		sds_real k1 = yDiff1 / xDiff1;
		sds_real k2 = yDiff2 / xDiff2;
		sds_real b1 = start1[1] - k1 * start1[0];
		sds_real b2 = start2[1] - k2 * start2[0];

		xPoint[0] = (b2 - b1) / (k1 - k2);
		xPoint[1] = k1 * xPoint[0] + b1;
		xPoint[2] = start1[2];
	}
	return true;
}


int cmd_fillet_or_chamfer_lwpline(sds_name pline,int mode,sds_real dist1,sds_real dist2,sds_point pickpt1,sds_point pickpt2)
{
	//NOTE: MAJOR DIFFERENCE FROM ACAD: We allow user to pick any two pts on the
	//	pline, and fillet/chamfer all vertices between the selected points.
	//******************************************************
	//If pickpt1==NULL, we'll fillet/chamfer the entire pline.  We'll
	//	set vtxnum[0] and vtxnum[1] to 0;
	//	If pickpt1 & 2 aren't NULL, we're going to fillet/chamfer only
	//	those vertices between those points. Fillet/chamfer between 2
	//	pts on closed pline requires that bit 8 on mode be OFF.  If it's
	//	not off and pline is closed and vertices are adjacent, we'll only
	//	chamfer between those vertices.  What a pain in the neck!!!
	//******************************************************
	//Filletrad/chamfer distance of 0.0 is equivalent to removing all
	//	non-consecutive curves from pline
	//pickpt1 & pickpt2 are ucs points - we'll transform them into ecs.

	//MODE: 1=perform fillet w/trimmode on (dist2 ignored)
	//		2=perform fillet w/trimmode off (dist2 ignored)
	//		3=perform chamfer w/trim on and dist1 & dist2 as the chamfer distances
	//					(chamferA and chamferB)
	//		4=perform chamfer w/trim off and dist1 & dist2 as the chamfer distances
	//		5=perform chamfer w/trim on and dist1 & dist2 as the chamfer dist & angle
	//					(chamferC and chamferD)
	//		6=perform chamfer w/trim off and dist1 & dist2 as the chamfer dist & angle
	//		8 (bit which requires mode 1-6 to also be set) = User opted for DIRECTIONAL mode
	//			on closed pline.  This is only valid if both pickpts are non-null and the
	//			pline is closed.
	typedef struct lwplvertdatastru
	{
		sds_point pt;
		sds_real bulge, stwid, endwid;
	} lwplvertdata;

	char chamltype[IC_ACADNMLEN],chamlayer[IC_ACADNMLEN];//fs1[IC_ACADBUF],
	int ret=0, fi1,fi2,fi4,nAdjacentIsArc,skip,closure=0,chamcolor,nParallelVerts,nSegsTooShort,nDivergentVerts,
	pass_go,exit_early,reverse_flag,vertno;//,rbchk
	int curvert,lastvert,nextvert, nextnextvert;
	sds_real fr1,lastvertbulge,curvertbulge,nextvertbulge,tempdist,bulge,stwid,endwid,endwid2;
	struct resbuf *rbp1, *rbtemp, rbecs,rbwcs;
	sds_point p1,p2,p3,p4,p5,p6,p7,p8,p9,p10, xPoint;
	long vtxnum[2];
	int exactvert;
	bool trimmode=false, skipSegment = false;
	bool directionalmode=false, hasCreated = false;
	db_lwpolyline *lwpl=new db_lwpolyline(*((db_lwpolyline *)(pline[0])));	// a copy for us to use to build the new one
	bool lvfound=false;
	lwplvertdata lv;
	resbuf *lwplrb,*lwpladdrb,*trb;
	sds_point firstlwpt, secondlwpt;
	sds_real firstlwbulge;
	resbuf *prexdata,*xdata;

	if (mode & 1)
		trimmode=true;
	if (mode & 8)
		directionalmode=true;

	rbecs.restype=RT3DPOINT;
	if ((ret=SDS_getvar(NULL,DB_QUCSXDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)
		return(RTERROR);
	ic_ptcpy(p1,rbwcs.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_crossproduct(p1,rbwcs.resval.rpoint,rbecs.resval.rpoint);


	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;

	if ((rbp1=sds_buildlist(RTSTR,"*",0))==NULL)
	{
		ret=-1;goto exit;
	}
	if ((lwplrb=sds_entgetx(pline,rbp1))==NULL)
	{
		ret=-1;goto exit;
	}
	closure=lwpl->ret_70();
	fr1=lwpl->ret_38();
	trb=lwplrb;	// grab the first two points and first bulge and hold them
	while (trb->restype!=10)
		trb=trb->rbnext;
	ic_ptcpy(firstlwpt,trb->resval.rpoint);
	while (trb->restype!=42)
		trb=trb->rbnext;
	firstlwbulge=trb->resval.rreal;
	while (trb->restype!=10)
		trb=trb->rbnext;
	ic_ptcpy(secondlwpt,trb->resval.rpoint);


	FreeResbufIfNotNull(&rbp1);

	reverse_flag=0;	//tells us if pline's closed and we're working around
	//the vertex where it starts...

	if (!trimmode)//if trimmode is OFF, we need to create separate entities
	{
		strncpy(chamltype,"BYLAYER"/*DNT*/ ,sizeof(chamltype)-1);
		chamcolor=256;
		lwpl->get_8(chamlayer,sizeof(chamlayer));
		lwpl->get_6(chamltype,sizeof(chamltype));
		lwpl->get_62(&chamcolor);
	}

	sds_entmake(NULL);	//make sure we're at start of entity

	// Set the Undo so we correctly redraw the entity Undo.
	SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)pline[0],NULL,NULL,SDS_CURDWG);

	// find and hold xdata
	prexdata=lwplrb;
	xdata=NULL;
	while (prexdata->rbnext!=NULL && prexdata->rbnext->restype!=-3)
		prexdata=prexdata->rbnext;
	if (prexdata->rbnext!=NULL)
	{
		xdata=prexdata->rbnext;
		prexdata->rbnext=NULL;
	}

	//set up new main ent
	if (trimmode)
	{
		LwplrbDeleteVertexData(lwplrb);
		lwpladdrb=lwplrb;
		while (lwpladdrb->rbnext!=NULL)
			lwpladdrb=lwpladdrb->rbnext;
	}

	//Now, step thru each vertex. Call current vertex curvert.  Previous vtx is lastvert and
	//succeeding vertices will be called nextvert and nextnextvert.
	//We'll chamfer between lastvert & curvert.
	//if the pline's closed, we will start at the LAST vertex of the pline - the one which closes it!
	//if the pline is closed, set lastvert to the NEXT-TO-LAST vertex in the pline - otherwise set it
	//to NULL.  The pass_go var is whether we've passed main(pline)ent on a closed pline or not
	//if pass_go=0, first vtx chamfer pt is at end of pline, 2nd is at beginning

	//nParallelVerts=number of parallel vertices we cannot chamfer
	//nSegsTooShort=number of vertices too short to chamfer
	//nDivergentVerts=number of divergent vertices we cannot chamfer
	//nAdjacentIsArc=number of vertices which weren't chamfered because adjacent segment was also arc.  This
	//	counter also incremented for vertices which were removed (arc removal).
	//NOTE:  Parallel and Divergent vertices only occur when removing a bulge betw 2 straight segs
	nParallelVerts=nSegsTooShort=nDivergentVerts=nAdjacentIsArc=0;

	lastvert=curvert=nextvert=nextnextvert=-1;
	pass_go=1;
	exit_early=0;
	if (closure & IC_PLINE_CLOSED)
	{
		//init vars
		//temporarily let fi4 count the number of vertices on a CLOSED pline
		fi4=lwpl->ret_90();
		pass_go=0;
		nextvert=0;
		curvert=fi4-1;
		lastvert=curvert-1;
	}
	else
	{
		curvert=-1;
	}
	//determine on which segment each point lies
	if (pickpt1!=NULL)
	{
		if ((fi1=cmd_lwpl_vtx_no(pline,pickpt1,1,&exactvert))<0)
		{
			ret=-1;
			goto exit;
		}
		fi2=cmd_lwpl_vtx_no(pline,pickpt2,1,&exactvert);
		double bulge1, bulge2;
		if (fi2 > fi1) {
			lwpl->get_42(&bulge1, fi1 + 1);
			lwpl->get_42(&bulge2, fi2 + 1);
		}
		else {
			lwpl->get_42(&bulge1, fi1);
			lwpl->get_42(&bulge2, fi2);
		}
		if (!icadRealEqual(bulge1, 0.0) || !icadRealEqual(bulge2, 0.0)) {
			// can not fillet or chamfer polyline arc.
			if (mode > 2) 
				sds_printf(ResourceString(CMD_ERR_CHAMFER_1, "Cannot chamfer polyline arc segments\n" ));
			else
				sds_printf(ResourceString(CMD_ERR_FILLET_1, "Cannot fillet polyline arc segments\n" ));

			sds_redraw(pline,IC_REDRAW_UNHILITE);
			ret=(-__LINE__);
			goto exit;
		}
		fi1++;
		fi2++;
		if (fi1==fi2)
		{
			cmd_ErrorPrompt(CMD_ERR_FILPOLYSEGS,0);
			ret=-1;
			goto exit;
		}
		if (!(closure & IC_PLINE_CLOSED))
		{
			vtxnum[0]=min(fi1,fi2);
			vtxnum[1]=max(fi1,fi2);
		}
		else
		{
			//if not directional mode && closed pline pick pts in
			//	backwards order separated by one vertex
			if (!directionalmode && 1==(fi1-fi2))
			{
				vtxnum[0]=fi2;	//reverse 'em
				vtxnum[1]=fi1;
			}
			else if (!directionalmode && 1==fi1 && fi4==fi2)
			{
				vtxnum[0]=fi2;	//reverse 'em
				vtxnum[1]=fi1;
			}
			else
			{
				//default to working between pts in direction of pline
				int diff = abs(fi2 - fi1);
				if (!directionalmode && diff > 2 && closure & IC_PLINE_CLOSED)
					diff = (fi2 < fi1) ? fi4 + fi2 - fi1 : fi4 + fi1 - fi2;

				if (diff > 2) {
					nDivergentVerts = 2;
					ret=(-__LINE__);
					goto exit;
				}
				else if (diff == 2) {
					// now we detect if selected line segments are parallel
					// or divergent
					int iStart1 = fi1 - 1;
					int iEnd1 = (fi1 != fi4) ? fi1 : 0;
					int iStart2 = fi2 - 1;
					int iEnd2 = (fi2 != fi4) ? fi2 : 0;
					if (directionalmode || cmd_lwpline_entnext(iEnd1, lwpl) == iStart2) {
						lwpl->get_10(p1, iStart1);
						lwpl->get_10(p2, iEnd1);
						lwpl->get_10(p3, iStart2);
						lwpl->get_10(p4, iEnd2);
						vtxnum[0] = fi1;
						vtxnum[1] = fi2;
					}
					else {
						lwpl->get_10(p1, iStart2);
						lwpl->get_10(p2, iEnd2);
						lwpl->get_10(p3, iStart1);
						lwpl->get_10(p4, iEnd1);
						vtxnum[0] = fi2;
						vtxnum[1] = fi1;
					}
					sds_real disDiff = cmdPointLineDistance(p1, p2, p3) - 
									   cmdPointLineDistance(p1, p2, p4);
					if (icadRealEqual(disDiff, 0.0)) {
						nParallelVerts = 2;
						ret=(-__LINE__);
						goto exit;
					}
					else if (disDiff > 0.0) {
						nDivergentVerts = 2;
						ret=(-__LINE__);
						goto exit;
					}
					else {
						cmdCompute2dIntersection(xPoint, p1, p2, p3, p4);
						skipSegment = true;
					}
				}
				else {
					vtxnum[0]=fi1;
					vtxnum[1]=fi2;
				}
			}
			if (vtxnum[0]>vtxnum[1])
				reverse_flag=1;
		}
	}
	else
		vtxnum[0]=vtxnum[1]=0;
	directionalmode=false;
	mode &= ~8;	// just to make sure

	//now we're set to start looping.  lastvertrb & lastvert define last vertex before closure or NULL & 0.0
	//if pline not closed.  curvertrb & curvertbulge are elist and bulge for current (first) vtx of pline.
	///****MAIN LOOP***
	if (closure & IC_PLINE_CLOSED)
		vertno=0;	   //vertno keeps track of which vtx we're on.
	//If closed, we're before start
	else
		vertno=1;

	skip=-1;					//if removing a bulged vtx, we must skip over vtx after bulge
	for (;;vertno++)			//vertno will count vertex number
	{
		//setup entities and check if we need to break
		if (vertno!=0)		//fetch next vtx & keep last vtx as lastvertrb & lastvert
		{
			lastvert=curvert;	//UNLESS pline's closed and we're on the 1st pass
			if (vertno<2)
				curvert=cmd_lwpline_entnext(curvert,lwpl);
			else
				curvert++;
		}
		if ((closure & IC_PLINE_CLOSED) && !exit_early)	//on closure, curvert will step on seqend to exit loop.  Non-closure means we
		{
			//can't chamfer end of last segment, so nextvert will test exit from loop
			if (curvert>=lwpl->ret_90())
				break;
		}
		if ((!exit_early) && (closure & IC_PLINE_CLOSED))	//was if (vertno!=0)
			//Now set nextvert to next pline vertex
			nextvert=cmd_lwpline_entnext(curvert,lwpl);
		else
			nextvert=curvert+1;

		if (!(closure & IC_PLINE_CLOSED) || exit_early)
		{
			if (nextvert>=lwpl->ret_90())
			{
				if (!exit_early)
				{
					if (trimmode)
					{
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					}
				}
				break;
			}
		}
		nextnextvert=cmd_lwpline_entnext(nextvert,lwpl);

		if (skip == vertno)
			continue;
		
		if (vertno==0)				 //first pass thru - we need to get pt from elist
		{
			lwpl->get_10(p1,0);
			lwpl->get_42(&lastvertbulge,0);
		}
		if (lastvert!=-1 && vertno!=0)
		{
			ic_ptcpy(p1,p2);	//when advancing entities we can't get p1 from elist
			lastvertbulge=curvertbulge;			//	- we may have moved p2
		}
		lwpl->get_10(p2,curvert);
		if (curvert!=-1)
			lwpl->get_42(&curvertbulge,curvert);

		if (nextvert>=1)
		{
			lwpl->get_10(p3,nextvert);
			if (nextvert!=-1)
				lwpl->get_42(&nextvertbulge,nextvert);
		}
		else // if back to the start (closed pline), we get the data from the rbs instead of the entity copy
		{
			// because the copy will have changed
			trb=lwplrb;					// nextvert can only be the 0th item
			ic_ptcpy(p3,firstlwpt);
			nextvertbulge=firstlwbulge;
		}

		if (nextnextvert!=-1)
		{
			if (nextnextvert>=2)
				lwpl->get_10(p4,nextnextvert);
			else						// same here -- get from the resbufs if we've looped around
			{
				// nextnextvert can be the 0th or first
				if (nextnextvert==0)
					ic_ptcpy(p4,firstlwpt);
				else
					ic_ptcpy(p4,secondlwpt);
			}
		}

		if (!(closure & IC_PLINE_CLOSED) && vertno==1)		//if no closure, no alteration necessary to 1st segment
		{
			if (trimmode)
			{
				LwplrbAddVert(lwpl, &lwpladdrb, curvert);
			}
			continue;
		}
		if (vertno==0 && icadRealEqual(curvertbulge,0.0))		//if last segment on closed pline is straight, continue
			continue;							//we'll build segment later (pass_go still zero)

		if (pickpt1!=NULL) {
			if (!reverse_flag) {
				if (hasCreated || vertno <= vtxnum[0] || vertno > vtxnum[1]) {
					//if we're chamfering specific segments & this isn't one...
					if (trimmode)
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					pass_go=1;
					nAdjacentIsArc++;
					continue;
				}
				else {
					if (skipSegment && vertno < vtxnum[1]) {
						ic_ptcpy(p2, xPoint);
						ic_ptcpy(p3, p4);
						skip = vertno + 1;		
					}
				}
			} 
			else {
				if (hasCreated || vertno>vtxnum[1] && vertno<=vtxnum[0]) {
					if (trimmode)
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					pass_go=1;
					nAdjacentIsArc++;
					continue;
				}
				else {
					if (skipSegment) {
						if (vertno == 1 && vertno == vtxnum[1]) {
							ic_ptcpy(p2, xPoint);
							lwpl->get_10(p1, lastvert - 1);
							skip = vertno - 1;
							if (skip == 0)
								skip = fi4;
						}
						else if (vertno < vtxnum[1]) {
							ic_ptcpy(p2, xPoint);
							ic_ptcpy(p3, p4);
							skip = vertno + 1;	
						}
					}
				}
			}
		}

		//**************************
		//adjust a straight segment
		//**************************
		if (icadRealEqual(curvertbulge,0.0))
		{
			if (!icadRealEqual(lastvertbulge,0.0))
			{
				//if we're on a straight segment after a curve, we can't make any adjustments
				//so just make the list.  we know that vertno>0, because if straight w/vertno==0 we skip
				if (trimmode)
				{
					LwplrbAddVert(lwpl, &lwpladdrb, curvert);
				}
				pass_go=1;
				nAdjacentIsArc++;
				continue;
			}
			if (mode>2 && (sds_distance(p2,p1)<=dist1))
			{
				//if we're chamferring and the segment length is too short...
				if (pass_go)
				{
					if (trimmode)
					{
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					}
					nSegsTooShort++;
				}
				continue;
			}
			if (mode>2)	//chamfer
			{
				if (icadRealEqual(dist1,0.0))
					ic_ptcpy(p5,p2);
				else
				{
					sds_polar(p2,sds_angle(p2,p1),dist1,p5);
				}
				if (icadRealEqual(dist2,0.0))							//if 2nd chamfer dist or angle is zero,
					ic_ptcpy(p6,p2);						//we make only one vertex so copy pts
				else
				{
					if (mode<5)	//dist-dist chamfer
					{
						if (sds_distance(p2,p3)<=dist2)
						{
							if (pass_go)
							{
								if (trimmode)
								{
									LwplrbAddVert(lwpl, &lwpladdrb, curvert);
								}
								nSegsTooShort++;
							}
							continue;
						}
						sds_real theangle;
						theangle=sds_angle(p2,p3);
						sds_polar(p2,theangle,dist2,p6);
					}
					else	//dist-angle chamfer
					{
						sds_polar(p5,(sds_angle(p1,p2)+dist2),1.0,p9);
						sds_polar(p5,(sds_angle(p1,p2)-dist2),1.0,p10);
						if (ic_linexline(p5,p9,p2,p3,p8)>1)
							ic_ptcpy(p6,p8);
						else if (ic_linexline(p5,p10,p2,p3,p8)>1)
							ic_ptcpy(p6,p8);
						else							//line too short to chamfer
						{
							if (pass_go)	//if we're not before beginning of a closed pline
							{
								if (mode==5)
								{
									LwplrbAddVert(lwpl, &lwpladdrb, curvert);
								}
								nSegsTooShort++;
							}
							pass_go=1;
							continue;
						}
					}
				}
				bulge=0.0;//new segment's bulge to be 0.0 because we're doing a chamfer
			}
			else//fillet
			{
				if (icadRealEqual(dist1,0.0))
				{
					ic_ptcpy(p5,p2);
					ic_ptcpy(p6,p2);
					bulge=0.0;
				}
				else
				{
					if (1==cmd_fillet_strseg2strseg(p1,p2,p3,dist1,&tempdist,&bulge) ||
						tempdist>=sds_distance(p1,p2) || tempdist>=sds_distance(p2,p3))
					{
						if (pass_go)
						{
							if (trimmode)
							{
								LwplrbAddVert(lwpl, &lwpladdrb, curvert);
							}
							nSegsTooShort++;
						}
						continue;
					}
					sds_polar(p2,sds_angle(p2,p1),tempdist,p5);
					sds_polar(p2,sds_angle(p2,p3),tempdist,p6);
				}
			}
			if (trimmode)	//if trimmode is on
			{
				if (pass_go)
				{
					lwpl->get_41(&stwid,  lastvert);
					lwpl->get_40(&endwid, curvert);
					lwpl->get_41(&endwid2,curvert);

					//set curvertrb's new point, bulge, & widths
					lwpl->set_10(p5,curvert);
					lwpl->set_40(stwid,curvert);
					lwpl->set_41(endwid,curvert);
					lwpl->set_42(bulge,curvert);
					LwplrbAddVert(lwpl, &lwpladdrb, curvert);

					lwpl->set_40(endwid,curvert);
					lwpl->set_41(endwid2,curvert);
					lwpl->set_42(0.0,curvert);
				}
				else						   //save chamfer pt if we're before the beginning
				{
					lvfound=true;
					ic_ptcpy(lv.pt,p5);
					lwpl->get_40(&lv.stwid,curvert);
					lwpl->get_41(&lv.endwid,curvert);
					lv.bulge=bulge;
					//reset start & end width for 1st chamfer vtx to ending width for lastvert
					lwpl->get_41(&tempdist,lastvert);
					stwid=lv.stwid;
					lv.stwid=tempdist;
					lv.endwid=stwid;
					pass_go=1;
				}
				if (!icadPointEqual(p5,p6))		//don't insert 2nd chamfer vtx if it's same as 1st
				{
					lwpl->set_10(p6,curvert);
					LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					if (pickpt1)
						hasCreated = true;
				}
			}
			else
			{	//don't make new pline - just make new lines.
				if (icadPointEqual(p5,p6))
					continue;
				//if (!pass_go)continue;
				resbuf *temprb;
				if (mode>2)
				{
					sds_trans(p5,&rbecs,&rbwcs,0,p9);
					sds_trans(p6,&rbecs,&rbwcs,0,p10);
					temprb=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),8,chamlayer,62,chamcolor,6,chamltype,10,p9,11,p10,210,rbecs.resval.rpoint,0);
				}
				else
				{
					sds_real stang,endang;
					ic_bulge2arc(p5,p6,bulge,p7,&tempdist,&stang,&endang);
					temprb=sds_buildlist(RTDXF0,"ARC"/*DNT*/,8,chamlayer,62,chamcolor,6,chamltype,10,p7,40,tempdist,50,stang,51,endang,210,rbecs.resval.rpoint,0);
				}
				if (sds_entmake(temprb)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
				if (pickpt1)
					hasCreated = true;
				FreeResbufIfNotNull(&temprb);
				pass_go=1;
			}
		}
		else
		{
			//***********************
			//adjust a bulged section
			if (!icadRealEqual(nextvertbulge,0.0) || !icadRealEqual(lastvertbulge,0.0))
			{
				if (pass_go)						//don't make vtx if we're before the beginning
					if (trimmode)
					{
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					}
				pass_go=1;
				nAdjacentIsArc++;
				continue;
			}
			if (ic_linexline(p1,p2,p3,p4,p7)==(-1))	   //parallel lines around arc
			{
				if (pass_go)  //don't make vtx if we're before the beginning
				{
					if (trimmode)
					{
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					}
					nParallelVerts++;
				}
				pass_go=1;
				continue;
			}
			if (!icadDirectionEqual(p1,p2,p1,p7))
			{
				//divergent lines
				if (pass_go)  //don't make vtx if we're before the beginning
				{
					if (trimmode)
					{
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
					}
					nDivergentVerts++;
				}
				pass_go=1;
				continue;
			}
			if (mode>2)
			{
				//ic_ptcpy(p2,p7);
				if (sds_distance(p7,p1)<=dist1)
				{
					if (pass_go)
					{
						LwplrbAddVert(lwpl, &lwpladdrb, curvert);
						nSegsTooShort++;
					}
					continue;
				}
				sds_polar(p7,sds_angle(p2,p1),dist1,p5);
				if (icadRealEqual(dist2,0.0))
					ic_ptcpy(p6,p5);
				else
				{
					if (mode<5)
					{
						if (sds_distance(p7,p4)<=dist2)
						{
							if (pass_go)
							{
								LwplrbAddVert(lwpl, &lwpladdrb, curvert);
								nSegsTooShort++;
							}
							continue;
						}
						sds_polar(p7,sds_angle(p3,p4),dist2,p6);
					}
					else
					{
						sds_polar(p5,(sds_angle(p1,p2)+dist2),1.0,p9);
						sds_polar(p5,(sds_angle(p1,p2)-dist2),1.0,p10);
						if (ic_linexline(p5,p9,p7,p4,p8)>1)
							ic_ptcpy(p6,p8);
						else if (ic_linexline(p5,p10,p7,p4,p8)>1)
							ic_ptcpy(p6,p8);
						else						   //line too short
						{
							if (pass_go)  //don't make vtx if we're before the beginning
							{
								if (trimmode)
								{
									LwplrbAddVert(lwpl, &lwpladdrb, curvert);
								}
								nSegsTooShort++;
							}
							continue;
						}
					}
				}
				bulge=0.0;//new bulge
			}
			else
			{
				if (icadRealEqual(dist1,0.0))
				{
					ic_ptcpy(p5,p7);
					ic_ptcpy(p6,p7);
					bulge=0.0;
				}
				else
				{
					if (1==cmd_fillet_strseg2strseg(p1,p7,p3,dist1,&tempdist,&bulge)||
						tempdist>=sds_distance(p1,p7) || tempdist>=sds_distance(p7,p4))
					{
						if (pass_go)
						{
							LwplrbAddVert(lwpl, &lwpladdrb, curvert);
							nSegsTooShort++;
						}
						continue;
					}
					sds_polar(p7,sds_angle(p7,p1),tempdist,p5);
					sds_polar(p7,sds_angle(p7,p4),tempdist,p6);
				}
			}
			if (trimmode)
			{
				curvertbulge=0.0;					//make sure we reset this to 0.0 so
				//that when we skip to next vtx we start
				//w/a straight segment...
				if (pass_go)				//don't make 1st chamfer pt if we're before the beginning
				{
					//of a closed pline
					lwpl->set_10(p5,curvert);
					lwpl->set_42(bulge,curvert);
					LwplrbAddVert(lwpl, &lwpladdrb, curvert);
				}
				else				//save 1st chamfer pt if we're before beginning
				{
					lvfound=true;
					ic_ptcpy(lv.pt,p5);
					lwpl->get_40(&lv.stwid,curvert);
					lwpl->get_41(&lv.endwid,curvert);
					lv.bulge=curvertbulge;

					rbtemp=NULL;
					pass_go=1;
					//exit early if bulged section at end of pline is being removed
					exit_early=1;
				}
				if (!icadPointEqual(p6,p5))		//don't insert 2nd chamfer vtx if it's same as 1st
				{
					lwpl->set_10(p6,nextvert);
					LwplrbAddVert(lwpl, &lwpladdrb, nextvert);
				}
			}
			else
			{	//don't make new pline - just make new lines.
				if (icadPointEqual(p5,p6))
					continue;
				if (!pass_go)
					continue;
				resbuf *temprb;
				if (mode>2)
				{
					sds_trans(p5,&rbecs,&rbwcs,0,p9);
					sds_trans(p6,&rbecs,&rbwcs,0,p10);
					temprb=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),8,chamlayer,62,chamcolor,6,chamltype,10,p9,11,p10,210,rbecs.resval.rpoint,0);
				}
				else
				{
					sds_real stang,endang;
					ic_bulge2arc(p5,p6,dist1,p7,&tempdist,&stang,&endang);
					temprb=sds_buildlist(RTDXF0,"ARC"/*DNT*/,8,chamlayer,62,chamcolor,6,chamltype,10,p7,40,tempdist,50,stang,51,endang,210,rbecs.resval.rpoint,0);
				}
				if (sds_entmake(temprb)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
				FreeResbufIfNotNull(&temprb);
				if (pass_go==0)
				{
					pass_go=1;
					exit_early=1;
				}
			}
			skip=vertno + 1;
			nAdjacentIsArc++;
		}
		ic_ptcpy(p2,p6);

	}
	if (trimmode)
	{
		if ((closure & IC_PLINE_CLOSED) && lvfound)
		{//make the vertex we saved at the beginning
			LwplrbAddVertFromData(&lwpladdrb, lv.pt, lv.stwid, lv.endwid, lv.bulge);
		}

		LwplrbSetNumVerts(lwplrb);
		if (xdata!=NULL)
		{
			lwpladdrb->rbnext=xdata;
			xdata=NULL;	 // avoid double free
		}

		/*BUGBEGIN 	#ITC_7168
		if (sds_entmake(lwplrb)!=RTNORM)
			{
			ret=(-__LINE__);
			}
		sds_entdel(pline);
		sds_entlast(pline);
		sds_redraw(pline,IC_REDRAW_DRAW);
		BUGEND   	#ITC_7168
		*/

		//#ITC_7168 FIXED
		//------------------------------------------------------------------------------------
		//WHY & HOW:   sds_entmake(lwplrb) - add to draw polyline created by pointer lwplrb
		//	(where keep filleted polyline); nSegsTooShort - quantity mistaked verticies
		//	for specified distance. If added new polyline is OK we delete old polyline
		//	(pline) and make redrawing.
		//------------------------------------------------------------------------------------
		//FIXED:

		if (sds_entmake(lwplrb) != RTNORM)
			ret=(-__LINE__);

		sds_entdel(pline);
		sds_entlast(pline);
		sds_redraw(pline,IC_REDRAW_DRAW);

		//END of FIXED 	#ITC_7168

	}
	vertno=vertno-nParallelVerts-nSegsTooShort-nDivergentVerts-nAdjacentIsArc-1;
	if (exit_early)
		vertno++;
	if (!(closure & IC_PLINE_CLOSED))
		vertno=vertno-1;   //can't chamfer starting or ending vertices....
	if (vertno>0)
	{
		sds_name elast;
		sds_entlast(elast);
		// Set the Undo so we corectly redraw the entity Redo.
		SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)elast[0],NULL,NULL,SDS_CURDWG);
//		sds_printf(ResourceString(IDC_CMDS4__N_I__S__S__N_92, "\n%i %s %s.\n" ),vertno,(vertno==1)?ResourceString(IDC_CMDS4_VERTEX_WAS_93, "vertex was" ):ResourceString(IDC_CMDS4_VERTICES_WERE_94, "vertices were" ),(mode>2)?ResourceString(IDC_CMDS4_CHAMFERED_95, "chamfered" ):ResourceString(IDC_CMDS4_FILLETED_96, "filleted" ));
	}
	else if (vertno <= 0 || nSegsTooShort || nParallelVerts || nDivergentVerts) {
		sds_redraw(pline,IC_REDRAW_UNHILITE);
		sds_printf(ResourceString(IDC_CMDS4__NUNABLE_TO__S_AN_97, "\nUnable to %s any polyline vertices.\n" ),(mode>2)?ResourceString(IDC_CMDS4_CHAMFER_98, "chamfer" ):ResourceString(IDC_CMDS4_FILLET_99, "fillet" ));
	}
#ifdef LOCALISE_ISSUE
// The following is a nono
#endif
	
exit:

	if (nParallelVerts)
		sds_printf(ResourceString(IDC_CMDS4__T_I__S_PARALLEL_100, "\t%i %s parallel" ),nParallelVerts,(nParallelVerts==1)?ResourceString(IDC_CMDS4_WAS_101, "was" ):ResourceString(IDC_CMDS4_WERE_102, "were" ));
	if (nSegsTooShort) 
		sds_printf(ResourceString(IDC_CMDS4__T_I__S_TOO_SHOR_103, "\t%i %s too short" ),nSegsTooShort,(nSegsTooShort==1)?ResourceString(IDC_CMDS4_WAS_101, "was" ):ResourceString(IDC_CMDS4_WERE_102, "were" ));
	if (nDivergentVerts)
		sds_printf(ResourceString(IDC_CMDS4__T_I__S_DIVERGEN_104, "\t%i %s divergent" ),nDivergentVerts,(nDivergentVerts==1)?ResourceString(IDC_CMDS4_WAS_101, "was" ):ResourceString(IDC_CMDS4_WERE_102, "were" ));

	delete lwpl;
	FreeResbufIfNotNull(&lwplrb);
	FreeResbufIfNotNull(&xdata);
	if (ret==0)
		return(RTNORM);
	if (ret==-1)
		return(RTERROR);
	if (ret<-1)
		CMD_INTERNAL_MSG(ret);
	return(ret);
}


int cmd_fillet_or_chamfer_pline(sds_name pline,int mode,sds_real dist1,sds_real dist2,sds_point pickpt1,sds_point pickpt2)
{
	//NOTE: MAJOR DIFFERENCE FROM ACAD: We allow user to pick any two pts on the
	//	pline, and fillet/chamfer all vertices between the selected points.
	//******************************************************
	//If pickpt1==NULL, we'll fillet/chamfer the entire pline.  We'll
	//	set vtxnum[0] and vtxnum[1] to 0;
	//	If pickpt1 & 2 aren't NULL, we're going to fillet/chamfer only
	//	those vertices between those points. Fillet/chamfer between 2
	//	pts on closed pline requires that bit 8 on mode be OFF.  If it's
	//	not off and pline is closed and vertices are adjacent, we'll only
	//	chamfer between those vertices.  What a pain in the neck!!!
	//******************************************************
	//Filletrad/chamfer distance of 0.0 is equivalent to removing all
	//	non-consecutive curves from pline
	//pickpt1 & pickpt2 are ucs points - we'll transform them into ecs.

	//MODE: 1=perform fillet w/trimmode on (dist2 ignored)
	//		2=perform fillet w/trimmode off (dist2 ignored)
	//		3=perform chamfer w/trim on and dist1 & dist2 as the chamfer distances
	//					(chamferA and chamferB)
	//		4=perform chamfer w/trim off and dist1 & dist2 as the chamfer distances
	//		5=perform chamfer w/trim on and dist1 & dist2 as the chamfer dist & angle
	//					(chamferC and chamferD)
	//		6=perform chamfer w/trim off and dist1 & dist2 as the chamfer dist & angle
	//		8 (bit which requires mode 1-6 to also be set) = User opted for DIRECTIONAL mode
	//			on closed pline.  This is only valid if both pickpts are non-null and the
	//			pline is closed.
	char chamltype[IC_ACADNMLEN],chamlayer[IC_ACADNMLEN];//fs1[IC_ACADBUF],
	int ret=0, fi1,fi2,fi4,nAdjacentIsArc,skip,closure=0,chamcolor,nParallelVerts,nSegsTooShort,nDivergentVerts,
	pass_go,exit_early,reverse_flag,vertno;//,rbchk
	sds_name curvert,lastvert,nextvert, nextnextvert;
	sds_real fr1,lastvertbulge,curvertbulge,nextvertbulge,tempdist,bulge,stwid,endwid,endwid2;
	struct resbuf *rbp1, *rbp2, *rbp5 = NULL, *rbtemp, rbecs,rbwcs,*curvertrb=NULL,*lastvertrb=NULL,*nextvertrb=NULL,*nextnextvertrb=NULL;
	sds_point p1,p2,p3,p4,p5,p6,p7,p8,p9,p10;
	long vtxnum[2];
	int exactvert;
	bool trimmode=false;
	bool directionalmode=false;

	// make sure pline[0] is not NULL. Added to fix the bug# 78118
	if (!pline[0])
		return RTERROR;

	if (IC_TYPE_FROM_ENAME(pline)==DB_LWPOLYLINE)
		return cmd_fillet_or_chamfer_lwpline(pline,mode,dist1,dist2,pickpt1,pickpt2);

	if (mode & 1)
		trimmode=true;
	if (mode & 8)
		directionalmode=true;

	rbecs.restype=RT3DPOINT;
	if ((ret=SDS_getvar(NULL,DB_QUCSXDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)
		return(RTERROR);
	ic_ptcpy(p1,rbwcs.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_crossproduct(p1,rbwcs.resval.rpoint,rbecs.resval.rpoint);


	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;

	if ((rbp2=sds_buildlist(RTSTR,"*",0))==NULL)
	{
		ret=-1;goto exit;
	}
	if ((rbp1=sds_entgetx(pline,rbp2))==NULL)
	{
		ret=-1;goto exit;
	}
	for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
	{
		if (rbtemp->restype==70)
			closure=rbtemp->resval.rint;
		else if (10==rbtemp->restype)
			fr1=rbtemp->resval.rpoint[2];//elevation
	}
	FreeResbufIfNotNull(&rbp2);

	reverse_flag=0;	//tells us if pline's closed and we're working around
	//the vertex where it starts...

	if (!trimmode)//if trimmode is OFF, we need to create separate entities
	{
		strncpy(chamltype,"BYLAYER"/*DNT*/ ,sizeof(chamltype)-1);
		chamcolor=256;
		for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==8)
				//get the layer to be used for new chamfer entities (if new lines to be drawn)
				strncpy(chamlayer,rbtemp->resval.rstring,sizeof(chamlayer)-1);
			else if (rbtemp->restype==6)
				//get linetype
				strncpy(chamltype,rbtemp->resval.rstring,sizeof(chamltype)-1);
			else if (rbtemp->restype==62)
				//get color
				chamcolor=rbtemp->resval.rint;
		}
	}

	sds_entmake(NULL);	//make sure we're at start of entity

	// Set the Undo so we correctly redraw the entity Undo.
	SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)pline[0],NULL,NULL,SDS_CURDWG);

	//make new main ent - can't use cmd_make_pline_start because it sets closure flag off
	if (trimmode)//trimmode
	{
		if (sds_entmake(rbp1)!=RTNORM)
		{
			ret=(-__LINE__);goto exit;
		}
	}

	FreeResbufIfNotNull(&rbp1);
	//Now, step thru each vertex. Call current vertex curvert.  Previous vtx is lastvert and
	//succeeding vertices will be called nextvert and nextnextvert.  Each entity will have a corres-
	//ponding elist lastvertrb thru nextnextvertrb.  We'll chamfer between lastvert & curvert.
	//if the pline's closed, we will start at the LAST vertex of the pline - the one which closes it!
	//if the pline is closed, set lastvert to the NEXT-TO-LAST vertex in the pline - otherwise set it
	//to NULL.  The pass_go var is whether we've passed main(pline)ent on a closed pline or not
	//if pass_go=0, first vtx chamfer pt is at end of pline, 2nd is at beginning
	lastvert[0]=lastvert[1]=curvert[0]=curvert[1]=nextvert[0]=nextvert[1]=0;
	pass_go=1;
	exit_early=0;
	if (closure & IC_PLINE_CLOSED)
	{
		//init vars
		//temporarily let fi4 count the number of vertices on a CLOSED pline
		fi4=0;
		pass_go=0;
		sds_entnext_noxref(pline,nextvert);
		do
		{
			ic_encpy(lastvert,curvert);
			ic_encpy(curvert,nextvert);
			FreeResbufIfNotNull(&lastvertrb);
			lastvertrb=curvertrb;
			curvertrb=nextvertrb;
			sds_entnext_noxref(nextvert,nextvert);
			if ((nextvertrb=sds_entget(nextvert))==NULL)
			{
				ret=(-__LINE__);
				goto exit;
			}
			if (closure & IC_PLINE_SPLINEFIT)
			{
				if ((0==ic_assoc(nextvertrb,70)) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
					break;
			}
			fi4++;
			ic_assoc(nextvertrb,0);
		}
		while (!strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ));
		FreeResbufIfNotNull(&nextvertrb);
	}
	else
	{
		ic_encpy(curvert,pline);
	}
	//determine on which segment each point lies
	if (pickpt1!=NULL)
	{
		if ((fi1=cmd_pl_vtx_no(pline,pickpt1,1,&exactvert))<=0)
		{
			ret=-1;goto exit;
		}
		fi2=cmd_pl_vtx_no(pline,pickpt2,1,&exactvert);
		if (fi1==fi2)
		{
			cmd_ErrorPrompt(CMD_ERR_FILPOLYSEGS,0);
			ret=-1;
			goto exit;
		}
		if (!(closure & IC_PLINE_CLOSED))
		{
			vtxnum[0]=min(fi1,fi2);
			vtxnum[1]=max(fi1,fi2);
		}
		else
		{
			//if not directional mode && closed pline pick pts in
			//	backwards order separated by one vertex
			if (!directionalmode && 1==(fi1-fi2))
			{
				vtxnum[0]=fi2;	//reverse 'em
				vtxnum[1]=fi1;
			}
			else if (!directionalmode && 1==fi1 && fi4==fi2)
			{
				vtxnum[0]=fi2;	//reverse 'em
				vtxnum[1]=fi1;
			}
			else
			{
				//default to working between pts in direction of pline
				vtxnum[0]=fi1;
				vtxnum[1]=fi2;
			}
			if (vtxnum[0]>vtxnum[1])
				reverse_flag=1;
		}
	}
	else
		vtxnum[0]=vtxnum[1]=0;
	directionalmode=false;
	mode &= ~8;	// just to make sure

	//nParallelVerts=number of parallel vertices we cannot chamfer
	//nSegsTooShort=number of vertices too short to chamfer
	//nDivergentVerts=number of divergent vertices we cannot chamfer
	//nAdjacentIsArc=number of vertices which weren't chamfered because adjacent segment was also arc.  This
	//	counter also incremented for vertices which were removed (arc removal).
	//NOTE:  Parallel and Divergent vertices only occur when removing a bulge betw 2 straight segs
	nParallelVerts=nSegsTooShort=nDivergentVerts=nAdjacentIsArc=0;

	//now we're set to start looping.  lastvertrb & lastvert define last vertex before closure or NULL & 0.0
	//if pline not closed.  curvertrb & curvertbulge are elist and bulge for current (first) vtx of pline.
	///****MAIN LOOP***
	if (closure & IC_PLINE_CLOSED)
		vertno=0;	   //vertno keeps track of which vtx we're on.
	//If closed, we're before start
	else
		vertno=1;

	skip=0;					//if removing a bulged vtx, we must skip over vtx after bulge
	for (;;vertno++)			//vertno will count vertex number
	{
		//setup entities and check if we need to break
		if (vertno!=0)		//fetch next vtx & keep last vtx as lastvertrb & lastvert
		{
			ic_encpy(lastvert,curvert);	//UNLESS pline's closed and we're on the 1st pass
			FreeResbufIfNotNull(&lastvertrb);
			lastvertrb=curvertrb;
			curvertrb=NULL;
			if (vertno<2)
				cmd_pline_entnext(curvert,curvert,closure);
			else
				sds_entnext_noxref(curvert,curvert);
			FreeResbufIfNotNull(&curvertrb);
			if ((curvertrb=sds_entget(curvert))==NULL)
			{
				ret=(-__LINE__);
				goto exit;
			}
		}
		if ((closure & IC_PLINE_CLOSED) && !exit_early)	//on closure, curvert will step on seqend to exit loop.  Non-closure means we
		{
			//can't chamfer end of last segment, so nextvert will test exit from loop
			ic_assoc(curvertrb,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
				break;
		}
		if ((!exit_early) && (closure & IC_PLINE_CLOSED))	//was if (vertno!=0)
		{
			//Now set nextvert to next pline vertex
			if (ret=cmd_pline_entnext(curvert,nextvert,closure))
				goto exit;
		}
		else
		{
			if (sds_entnext_noxref(curvert,nextvert)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
		}
		FreeResbufIfNotNull(&nextvertrb);
		if ((nextvertrb=sds_entget(nextvert))==NULL)
		{
			ret=(-__LINE__);goto exit;
		}
		if (!(closure & IC_PLINE_CLOSED) || exit_early)
		{
			ic_assoc(nextvertrb,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
			{
				if (!exit_early)
				{
					if (trimmode && (sds_entmake(curvertrb)!=RTNORM))
					{
						ret=(-__LINE__);goto exit;
					}
				}
				break;
			}
		}
		if (ret=cmd_pline_entnext(nextvert,nextnextvert,closure))
		{
			if (closure & IC_PLINE_CLOSED)
			{
				ret=(-__LINE__);goto exit;
			}
			nextnextvert[0]=nextnextvert[1]=0;
		}
		FreeResbufIfNotNull(&nextnextvertrb);
		if ((nextnextvert[0]!=0 && nextnextvert[1]!=0) && ((nextnextvertrb=sds_entget(nextnextvert))==NULL))
		{
			ret=(-__LINE__);
			goto exit;
		}
		if (closure & IC_PLINE_SPLINEFIT)
		{
			if ((0==ic_assoc(nextvertrb,70)) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
			{
				if (trimmode && (sds_entmake(curvertrb)!=RTNORM))
				{
					ret=(-__LINE__);goto exit;
				}
				break;
			}
		}
		if (skip)
		{
			skip=0;
			continue;
		}
		if (vertno==0)				 //first pass thru - we need to get pt from elist
		{
			for (rbtemp=lastvertrb; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(p1,rbtemp->resval.rpoint);
				else if (rbtemp->restype==42)
					lastvertbulge=rbtemp->resval.rreal;
			}
		}
		if (lastvertrb!=NULL && vertno!=0)
		{
			ic_ptcpy(p1,p2);	//when advancing entities we can't get p1 from elist
			lastvertbulge=curvertbulge;			//	- we may have moved p2
		}
		ic_assoc(curvertrb,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		ic_assoc(nextvertrb,10);
		ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
		if (nextnextvertrb!=NULL)
		{
			ic_assoc(nextnextvertrb,10);
			ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
		}

		if (curvertrb!=NULL && !ic_assoc(curvertrb,42))
			curvertbulge=ic_rbassoc->resval.rreal;
		if (nextvertrb!=NULL && !ic_assoc(nextvertrb,42))
			nextvertbulge=ic_rbassoc->resval.rreal;

		if (!(closure & IC_PLINE_CLOSED) && vertno==1)		//if no closure, no alteration necessary to 1st segment
		{
			if (trimmode && sds_entmake(curvertrb)!=RTNORM)
			{
				ret=(-__LINE__);
				goto exit;
			}
			continue;
		}
		if (vertno==0 && icadRealEqual(curvertbulge,0.0))		//if last segment on closed pline is straight, continue
			continue;							//we'll build segment later (pass_go still zero)

		if (pickpt1!=NULL && !reverse_flag && (vertno<=vtxnum[0] || vertno>vtxnum[1]))
		{
			//if we're chamfering specific segments & this isn't one...
			if (trimmode && sds_entmake(curvertrb)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			pass_go=1;
			nAdjacentIsArc++;
			continue;
		}
		else if (pickpt1!=NULL && reverse_flag && (vertno>vtxnum[1] && vertno<=vtxnum[0]))
		{
			if (trimmode && sds_entmake(curvertrb)!=RTNORM)
			{
				ret=(-__LINE__);goto exit;
			}
			pass_go=1;
			nAdjacentIsArc++;
			continue;
		}

		//**************************
		//adjust a straight segment
		//**************************
		if (icadRealEqual(curvertbulge,0.0))
		{
			if (!icadRealEqual(lastvertbulge,0.0))
			{
				//if we're on a straight segment after a curve, we can't make any adjustments
				//so just make the list.  we know that vertno>0, because if straight w/vertno==0 we skip
				if (trimmode && sds_entmake(curvertrb)!=RTNORM)
				{
					ret=(-__LINE__);goto exit;
				}
				pass_go=1;
				nAdjacentIsArc++;
				continue;
			}
			if (mode>2 && (sds_distance(p2,p1)<=dist1))
			{
				//if we're chamferring and the segment length is too short...
				if (pass_go)
				{
					if (trimmode && sds_entmake(curvertrb)!=RTNORM)
					{
						ret=(-__LINE__);goto exit;
					}
					nSegsTooShort++;
				}
				continue;
			}
			if (mode>2)	//chamfer
			{
				if (icadRealEqual(dist1,0.0))
					ic_ptcpy(p5,p2);
				else
					sds_polar(p2,sds_angle(p2,p1),dist1,p5);
				if (icadRealEqual(dist2,0.0))							//if 2nd chamfer dist or angle is zero,
					ic_ptcpy(p6,p2);						//we make only one vertex so copy pts
				else
				{
					if (mode<5)	//dist-dist chamfer
					{
						if (sds_distance(p2,p3)<=dist2)
						{
							if (pass_go)
							{
								if (trimmode && sds_entmake(curvertrb)!=RTNORM)
								{
									ret=(-__LINE__);goto exit;
								}
								nSegsTooShort++;
							}
							continue;
						}
						sds_polar(p2,sds_angle(p2,p3),dist2,p6);
					}
					else	//dist-angle chamfer
					{
						sds_polar(p5,(sds_angle(p1,p2)+dist2),1.0,p9);
						sds_polar(p5,(sds_angle(p1,p2)-dist2),1.0,p10);
						if (ic_linexline(p5,p9,p2,p3,p8)>1)
							ic_ptcpy(p6,p8);
						else if (ic_linexline(p5,p10,p2,p3,p8)>1)
							ic_ptcpy(p6,p8);
						else							//line too short to chamfer
						{
							if (pass_go)	//if we're not before beginning of a closed pline
							{
								if (mode==5 && sds_entmake(curvertrb)!=RTNORM)
								{
									ret=(-__LINE__);goto exit;
								}
								nSegsTooShort++;
							}
							pass_go=1;
							continue;
						}
					}
				}
				bulge=0.0;//new segment's bulge to be 0.0 because we're doing a chamfer
			}
			else//fillet
			{
				if (icadRealEqual(dist1,0.0))
				{
					ic_ptcpy(p5,p2);
					ic_ptcpy(p6,p2);
					bulge=0.0;
				}
				else
				{
					if (1==cmd_fillet_strseg2strseg(p1,p2,p3,dist1,&tempdist,&bulge) ||
						tempdist>=sds_distance(p1,p2) || tempdist>=sds_distance(p2,p3))
					{
						if (pass_go)
						{
							if (trimmode && sds_entmake(curvertrb)!=RTNORM)
							{
								ret=(-__LINE__);goto exit;
							}
							nSegsTooShort++;
						}
						continue;
					}
					sds_polar(p2,sds_angle(p2,p1),tempdist,p5);
					sds_polar(p2,sds_angle(p2,p3),tempdist,p6);
				}
			}
			if (trimmode)	//if trimmode is on
			{
				if (pass_go)
				{
					if (ic_assoc(lastvertrb,41))
						stwid=0.0;
					else
						stwid=ic_rbassoc->resval.rreal;
					if (ic_assoc(curvertrb,40))
					{
						for (rbtemp=curvertrb; rbtemp->rbnext!=NULL; rbtemp=rbtemp->rbnext);
						if ((rbtemp->rbnext=sds_buildlist(40,0.0,0))==NULL)
						{
							ret=(-__LINE__);goto exit;
						}
						ic_rbassoc=rbtemp->rbnext;
					}
					endwid=ic_rbassoc->resval.rreal;
					if (ic_assoc(curvertrb,41))
					{
						for (rbtemp=curvertrb; rbtemp->rbnext!=NULL; rbtemp=rbtemp->rbnext);
						if ((rbtemp->rbnext=sds_buildlist(41,0.0,0))==NULL)
						{
							ret=(-__LINE__);goto exit;
						}
						ic_rbassoc=rbtemp->rbnext;
					}
					endwid2=ic_rbassoc->resval.rreal;
					//set curvertrb's new point, bulge, & widths
					for (rbtemp=curvertrb; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
					{
						if (rbtemp->restype==10)
							ic_ptcpy(rbtemp->resval.rpoint,p5);
						else if (rbtemp->restype==42)
							rbtemp->resval.rreal=bulge;
						else if (rbtemp->restype==40)
							rbtemp->resval.rreal=stwid;
						else if (rbtemp->restype==41)
							rbtemp->resval.rreal=endwid;
					}
					if (sds_entmake(curvertrb)!=RTNORM)
					{
						ret=(-__LINE__);goto exit;
					}
					for (rbtemp=curvertrb; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
					{
						if (rbtemp->restype==40)
							rbtemp->resval.rreal=endwid;
						else if (rbtemp->restype==41)
							rbtemp->resval.rreal=endwid2;
						else if (rbtemp->restype==42)
							rbtemp->resval.rreal=0.0;
					}
				}
				else						   //save chamfer pt if we're before the beginning
				{
					rbp5=sds_entget(curvert);
					for (rbtemp=rbp5; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
					{
						if (rbtemp->restype==10)
							ic_ptcpy(rbtemp->resval.rpoint,p5);
						//else if (rbtemp->restype==42)curvertbulge=rbtemp->resval.rreal;
						else if (rbtemp->restype==42)
							rbtemp->resval.rreal=bulge;
					}
					//reset start & end width for 1st chamfer vtx to ending width for lastvertrb
					if (0!=ic_assoc(lastvertrb,41))
						tempdist=0.0;
					else
						tempdist=ic_rbassoc->resval.rreal;
					if (ic_assoc(rbp5,40))
					{
						for (rbtemp=rbp5; rbtemp->rbnext!=NULL; rbtemp=rbtemp->rbnext);
						if ((rbtemp->rbnext=sds_buildlist(40,0.0,0))==NULL)
						{
							ret=(-__LINE__);goto exit;
						}
						ic_rbassoc=rbtemp->rbnext;
						rbtemp=NULL;
					}
					stwid=ic_rbassoc->resval.rreal;
					ic_rbassoc->resval.rreal=tempdist;
					if (ic_assoc(rbp5,41))
					{
						for (rbtemp=rbp5; rbtemp->rbnext!=NULL; rbtemp=rbtemp->rbnext);
						if ((rbtemp->rbnext=sds_buildlist(41,0.0,0))==NULL)
						{
							ret=(-__LINE__);goto exit;
						}
						ic_rbassoc=rbtemp->rbnext;
						rbtemp=NULL;
					}
					ic_rbassoc->resval.rreal=stwid;
					pass_go=1;
				}
				if (!icadPointEqual(p5,p6))		//don't insert 2nd chamfer vtx if it's same as 1st
				{
					ic_assoc(curvertrb,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p6);
					if (sds_entmake(curvertrb)!=RTNORM)
					{
						ret=(-__LINE__);goto exit;
					}
				}
			}
			else
			{	//don't make new pline - just make new lines.
				if (icadPointEqual(p5,p6))
					continue;
				//if (!pass_go)continue;
				FreeResbufIfNotNull(&nextnextvertrb);
				if (mode>2)
				{
					sds_trans(p5,&rbecs,&rbwcs,0,p9);
					sds_trans(p6,&rbecs,&rbwcs,0,p10);
					nextnextvertrb=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),8,chamlayer,62,chamcolor,6,chamltype,10,p9,11,p10,210,rbecs.resval.rpoint,0);
				}
				else
				{
					ic_bulge2arc(p5,p6,bulge,p7,&tempdist,&stwid,&endwid);
					nextnextvertrb=sds_buildlist(RTDXF0,"ARC"/*DNT*/,8,chamlayer,62,chamcolor,6,chamltype,10,p7,40,tempdist,50,stwid,51,endwid,210,rbecs.resval.rpoint,0);
				}
				if (sds_entmake(nextnextvertrb)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
				FreeResbufIfNotNull(&nextnextvertrb);
				pass_go=1;
			}
		}
		else
		{
			//***********************
			//adjust a bulged section
			if (!icadRealEqual(nextvertbulge,0.0) || !icadRealEqual(lastvertbulge,0.0))
			{
				if (pass_go)						//don't make vtx if we're before the beginning
				{
					if (trimmode &&(sds_entmake(curvertrb)!=RTNORM))
					{
						ret=(-__LINE__);
						goto exit;
					}
				}
				pass_go=1;
				nAdjacentIsArc++;
				continue;
			}
			if (ic_linexline(p1,p2,p3,p4,p7)==(-1))	   //parallel lines around arc
			{
				if (pass_go)  //don't make vtx if we're before the beginning
				{
					if (trimmode && sds_entmake(curvertrb)!=RTNORM)
					{
						ret=(-__LINE__);
						goto exit;
					}
					nParallelVerts++;
				}
				pass_go=1;
				continue;
			}
			if (!icadDirectionEqual(p1,p2,p1,p7))
			{
				//divergent lines
				if (pass_go)  //don't make vtx if we're before the beginning
				{
					if (trimmode &&(sds_entmake(curvertrb)!=RTNORM))
					{
						ret=(-__LINE__);
						goto exit;
					}
					nDivergentVerts++;
				}
				pass_go=1;
				continue;
			}
			if (mode>2)
			{
				//ic_ptcpy(p2,p7);
				if (sds_distance(p7,p1)<=dist1)
				{
					if (pass_go)
					{
						if (sds_entmake(curvertrb)!=RTNORM)
						{
							ret=(-__LINE__);goto exit;
						}
						nSegsTooShort++;
					}
					continue;
				}
				sds_polar(p7,sds_angle(p2,p1),dist1,p5);
				if (icadRealEqual(dist2,0.0))
					ic_ptcpy(p6,p5);
				else
				{
					if (mode<5)
					{
						if (sds_distance(p7,p4)<=dist2)
						{
							if (pass_go)
							{
								if (sds_entmake(curvertrb)!=RTNORM)
								{
									ret=(-__LINE__);goto exit;
								}
								nSegsTooShort++;
							}
							continue;
						}
						sds_polar(p7,sds_angle(p3,p4),dist2,p6);
					}
					else
					{
						sds_polar(p5,(sds_angle(p1,p2)+dist2),1.0,p9);
						sds_polar(p5,(sds_angle(p1,p2)-dist2),1.0,p10);
						if (ic_linexline(p5,p9,p7,p4,p8)>1)
							ic_ptcpy(p6,p8);
						else if (ic_linexline(p5,p10,p7,p4,p8)>1)
							ic_ptcpy(p6,p8);
						else						   //line too short
						{
							if (pass_go)  //don't make vtx if we're before the beginning
							{
								if (trimmode &&(sds_entmake(curvertrb)!=RTNORM))
								{
									ret=(-__LINE__);
									goto exit;
								}
								nSegsTooShort++;
							}
							continue;
						}
					}
				}
				bulge=0.0;//new bulge
			}
			else
			{
				if (icadRealEqual(dist1,0.0))
				{
					ic_ptcpy(p5,p7);
					ic_ptcpy(p6,p7);
					bulge=0.0;
				}
				else
				{
					if (1==cmd_fillet_strseg2strseg(p1,p7,p3,dist1,&tempdist,&bulge)||
						tempdist>=sds_distance(p1,p7) || tempdist>=sds_distance(p7,p4))
					{
						if (pass_go)
						{
							if (sds_entmake(curvertrb)!=RTNORM)
							{
								ret=(-__LINE__);goto exit;
							}
							nSegsTooShort++;
						}
						continue;
					}
					sds_polar(p7,sds_angle(p7,p1),tempdist,p5);
					sds_polar(p7,sds_angle(p7,p4),tempdist,p6);
				}
			}
			if (trimmode)
			{
				curvertbulge=0.0;					//make sure we reset this to 0.0 so
				//that when we skip to next vtx we start
				//w/a straight segment...
				if (pass_go)				//don't make 1st chamfer pt if we're before the beginning
				{
					//of a closed pline

					for (rbtemp=curvertrb; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
					{
						if (rbtemp->restype==10)
							ic_ptcpy(rbtemp->resval.rpoint,p5);
						else if (rbtemp->restype==42)
							rbtemp->resval.rreal=bulge;
					}
					if (sds_entmake(curvertrb)!=RTNORM)
					{
						ret=(-__LINE__);goto exit;
					}

				}
				else				//save 1st chamfer pt if we're before beginning
				{
					rbp5=sds_entget(curvert);
					for (rbtemp=rbp5; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
					{
						if (rbtemp->restype==10)
							ic_ptcpy(rbtemp->resval.rpoint,p5);
						else if (rbtemp->restype==42)
							rbtemp->resval.rreal=curvertbulge;
					}
					rbtemp=NULL;
					pass_go=1;
					//exit early if bulged section at end of pline is being removed
					exit_early=1;
				}
				if (!icadPointEqual(p6,p5))		//don't insert 2nd chamfer vtx if it's same as 1st
				{
					ic_assoc(nextvertrb,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p6);
					if (sds_entmake(nextvertrb)!=RTNORM)
					{
						ret=(-__LINE__);
						goto exit;
					}
				}
			}
			else
			{	//don't make new pline - just make new lines.
				if (icadPointEqual(p5,p6))
					continue;
				if (!pass_go)
					continue;
				FreeResbufIfNotNull(&nextnextvertrb);
				if (mode>2)
				{
					sds_trans(p5,&rbecs,&rbwcs,0,p9);
					sds_trans(p6,&rbecs,&rbwcs,0,p10);
					nextnextvertrb=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),8,chamlayer,62,chamcolor,6,chamltype,10,p9,11,p10,210,rbecs.resval.rpoint,0);
				}
				else
				{
					ic_bulge2arc(p5,p6,dist1,p7,&tempdist,&stwid,&endwid);
					nextnextvertrb=sds_buildlist(RTDXF0,"ARC"/*DNT*/,8,chamlayer,62,chamcolor,6,chamltype,10,p7,40,tempdist,50,stwid,51,endwid,210,rbecs.resval.rpoint,0);
				}
				if (sds_entmake(nextnextvertrb)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
				FreeResbufIfNotNull(&nextnextvertrb);
				if (pass_go==0)
				{
					pass_go=1;
					exit_early=1;
				}
			}
			skip=1;
			nAdjacentIsArc++;
		}
		ic_ptcpy(p2,p6);

	}
	if (trimmode)
	{
		if ((closure & IC_PLINE_CLOSED) && (rbp5!=NULL))	//make the vertex we saved at the beginning
		{
			if (sds_entmake(rbp5)!=RTNORM)
			{
				ret=(-__LINE__);
				goto exit;
			}
		}


		FreeResbufIfNotNull(&lastvertrb);
		lastvertrb=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
		if (sds_entmake(lastvertrb)!=RTNORM)
		{
			ret=(-__LINE__);
		}
		sds_entdel(pline);
		sds_entlast(pline);
		sds_redraw(pline,IC_REDRAW_DRAW);
	}
	vertno=vertno-nParallelVerts-nSegsTooShort-nDivergentVerts-nAdjacentIsArc-1;
	if (exit_early)
		vertno++;
	if (!(closure & IC_PLINE_CLOSED))
		vertno=vertno-1;   //can't chamfer starting or ending vertices....
	if (vertno>0)
	{
		sds_name elast;
		sds_entlast(elast);
		// Set the Undo so we corectly redraw the entity Redo.
		SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)elast[0],NULL,NULL,SDS_CURDWG);
		sds_printf(ResourceString(IDC_CMDS4__N_I__S__S__N_92, "\n%i %s %s.\n" ),vertno,(vertno==1)?ResourceString(IDC_CMDS4_VERTEX_WAS_93, "vertex was" ):ResourceString(IDC_CMDS4_VERTICES_WERE_94, "vertices were" ),(mode>2)?ResourceString(IDC_CMDS4_CHAMFERED_95, "chamfered" ):ResourceString(IDC_CMDS4_FILLETED_96, "filleted" ));
	}
	else
		sds_printf(ResourceString(IDC_CMDS4__NUNABLE_TO__S_AN_97, "\nUnable to %s any polyline vertices.\n" ),(mode>2)?ResourceString(IDC_CMDS4_CHAMFER_98, "chamfer" ):ResourceString(IDC_CMDS4_FILLET_99, "fillet" ));
#ifdef LOCALISE_ISSUE
// The following is a nono
#endif
	if (nParallelVerts)
		sds_printf(ResourceString(IDC_CMDS4__T_I__S_PARALLEL_100, "\t%i %s parallel" ),nParallelVerts,(nParallelVerts==1)?ResourceString(IDC_CMDS4_WAS_101, "was" ):ResourceString(IDC_CMDS4_WERE_102, "were" ));
	if (nSegsTooShort)
		sds_printf(ResourceString(IDC_CMDS4__T_I__S_TOO_SHOR_103, "\t%i %s too short" ),nSegsTooShort,(nSegsTooShort==1)?ResourceString(IDC_CMDS4_WAS_101, "was" ):ResourceString(IDC_CMDS4_WERE_102, "were" ));
	if (nDivergentVerts)
		sds_printf(ResourceString(IDC_CMDS4__T_I__S_DIVERGEN_104, "\t%i %s divergent" ),nDivergentVerts,(nDivergentVerts==1)?ResourceString(IDC_CMDS4_WAS_101, "was" ):ResourceString(IDC_CMDS4_WERE_102, "were" ));
	goto exit;

	exit:


	FreeResbufIfNotNull(&lastvertrb);
	FreeResbufIfNotNull(&curvertrb);
	FreeResbufIfNotNull(&nextvertrb);
	FreeResbufIfNotNull(&nextnextvertrb);
	FreeResbufIfNotNull(&rbp5);
	if (ret==0)
		return(RTNORM);
	if (ret==-1)
		return(RTERROR);
	if (ret<-1)
		CMD_INTERNAL_MSG(ret);
	return(ret);
}


int cmd_fillet_strseg2strseg(sds_point p1,sds_point p2,sds_point p3,sds_real filletrad,sds_real *segdist,sds_real *newbulg)
{
	//given p1->p2->p3 which define a corner to be filleted, calcs new bulge and  distance from p2 for vertices!
	sds_real fr1,fr2,fr3;

	if (icadRealEqual(filletrad,0.0))
	{
		*newbulg=0.0;
		*segdist=0.0;
		return(0);
	}
	fr1=sds_angle(p1,p2);
	fr2=sds_angle(p2,p3);
	if (icadAngleEqual(fr1,fr2))return(1);
	if (icadAngleEqual(fabs(fr2-fr1),IC_PI))return(1);

	fr3=fr2;
	ic_normang(&fr1,&fr3);
	if ((fr3-fr1)>IC_PI)
	{
		ic_normang(&fr2,&fr1);
		(*segdist)=filletrad*tan((fr1-fr2)/2.0);
		(*newbulg)=-tan((fr1-fr2)/4.0);
	}
	else
	{
		(*segdist)=filletrad*tan((fr3-fr1)/2.0);
		(*newbulg)=tan((fr3-fr1)/4.0);
	}
	return(0);
}

int cmd_do_fillet(sds_name ent1,sds_point ptsel1,sds_name ent2,sds_point ptsel2,int ptflag)
{
	//command performs actual fillet between entities
	//both pick points are ucs as gathered from entsel
	//NOTE: don't use this function to fillet an entire pline
	//ptflag bit 0 flags if ptsel1 is valid (if 0, we calc it)
	//ptflag bit 1 flags if ptsel2 is valid
	//if ptflag&8, the user has specified Directional mode for chamfer on closed pline
	struct resbuf *elist1,*elist2,*rbp1,*rbp2,rb, *rbtemp,rbucs,rbwcs,rbent;
	struct gr_view *view=SDS_CURGRVW;
	int ret=0;
	int ltcolor,trimmode,closure=0,fi1,fi2,l1type,l2type;
	sds_point p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,closept,p210;
	sds_real fr1,fr2,fr3,fr4,fr5,fr6,fr7,fr8,fr9,fr10,fr11,ucsz;
	char etype1[IC_ACADNMLEN],etype2[IC_ACADNMLEN],filletlay[IC_ACADNMLEN],
	filletlt[IC_ACADNMLEN];
	sds_name etemp,etemp2,etemp3;
	resbuf *newlwpl,*lwpladdrb;
	db_lwpolyline *lwpl;
	int nextvertno,thevert;

	//NOTE: l1type and l2type are used for filleting between lines, xlines, & rays.
	//	also, if bit 2 (4) set in subroutine, then entity type must change

	p7[0]=p7[1]=p210[0]=p210[1]=0.0;
	p7[2]=p210[2]=1.0;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	elist1=elist2=rbp1=rbp2=NULL;

	SDS_getvar(NULL,DB_QTRIMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	trimmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p0,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ucsz=p0[0]*rb.resval.rpoint[1]-rb.resval.rpoint[0]*p0[1];

	if ((elist1=sds_entget(ent1))==NULL)
		return(-1);
	if ((elist2=sds_entget(ent2))==NULL)
		return(-1);
	ic_assoc(elist1,0);
	strncpy(etype1,ic_rbassoc->resval.rstring,sizeof(etype1)-1);
	ic_assoc(elist2,0);
	strncpy(etype2,ic_rbassoc->resval.rstring,sizeof(etype2)-1);

	if ((strsame(etype1,"POLYLINE"/*DNT*/ ))   && (strsame(etype2,"POLYLINE"/*DNT*/ )) ||
		(strsame(etype1,"LWPOLYLINE"/*DNT*/ )) && (strsame(etype2,"LWPOLYLINE"/*DNT*/ )))
	{
		if (ent1[0]==ent2[0] && ent1[1]==ent2[1])	//if they're 2 pts on
		{
			//the same polyline
			if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
				ret=-1;goto exit;
			}
			if (3==(ptflag&3))	//if we know 2 specific pts, fillet between them
			{
				if (trimmode)
					ret=cmd_fillet_or_chamfer_pline(ent1,(ptflag&8)?9:1,rb.resval.rreal,0.0,ptsel1,ptsel2);
				else
					ret=cmd_fillet_or_chamfer_pline(ent1,(ptflag&8)?10:2,rb.resval.rreal,0.0,ptsel1,ptsel2);
			}
			else
			{			//else just fillet the whole thing
				if (trimmode)
					ret=cmd_fillet_or_chamfer_pline(ent1,(ptflag&8)?9:1,rb.resval.rreal,0.0,NULL,NULL);
				else
					ret=cmd_fillet_or_chamfer_pline(ent1,(ptflag&8)?10:2,rb.resval.rreal,0.0,NULL,NULL);
			}
		}
		else
		{
			cmd_ErrorPrompt(CMD_ERR_FILDIFFPOLY,0);
			ret=-1;
		}
		goto exit;
	}
	if ((strsame(etype2,db_hitype2str(DB_LINE)) && !strsame(etype1,db_hitype2str(DB_LINE))) ||
		(strsame(etype1,"POLYLINE"/*DNT*/ ) || strsame(etype1,"LWPOLYLINE"/*DNT*/)))  //switch entities around...
	{
		rbp1=elist1;
		elist1=elist2;
		elist2=rbp1;
		rbp1=NULL;
		strncpy(filletlay,etype1,sizeof(filletlay)-1);
		strncpy(etype1,etype2,sizeof(etype1)-1);
		strncpy(etype2,filletlay,sizeof(etype2)-1);
		ic_encpy(etemp,ent1);
		ic_encpy(ent1,ent2);
		ic_encpy(ent2,etemp);
		ic_ptcpy(p0,ptsel1);
		ic_ptcpy(ptsel1,ptsel2);
		ic_ptcpy(ptsel2,p0);
		etemp[0]=etemp[1]=filletlay[0]=0;
		if (ptflag&1)fi1=2;
		else fi1=0;
		if (ptflag&2)fi1+=1;
		ptflag=fi1;
	}

	//***********************************************************
	//before anything else, we need layer, color, & lt of new arc
	//***********************************************************
	ic_assoc(elist1,8);
	strncpy(filletlay,ic_rbassoc->resval.rstring,sizeof(filletlay)-1);
	ic_assoc(elist2,8);
	if (!strsame(filletlay,ic_rbassoc->resval.rstring))
	{
		SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		strncpy(filletlay,rb.resval.rstring,sizeof(filletlay)-1);
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=NULL;
	}
	if (ic_assoc(elist1,6))
		strncpy(filletlt,"BYLAYER"/*DNT*/ ,sizeof(filletlt)-1);
	else
		strncpy(filletlt,ic_rbassoc->resval.rstring,sizeof(filletlt)-1);
	if (ic_assoc(elist2,6))
	{
		if (!strsame(filletlt,"BYLAYER"/*DNT*/ ))
		{
			SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			strncpy(filletlt,rb.resval.rstring,sizeof(filletlt)-1);
			IC_FREE(rb.resval.rstring);
			rb.resval.rstring=NULL;
		}
	}
	else if (!strsame(filletlt,ic_rbassoc->resval.rstring))
	{
		SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		strncpy(filletlt,rb.resval.rstring,sizeof(filletlt)-1);
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=NULL;
	}
	if (ic_assoc(elist1,62)!=0)
		ltcolor=256;
	else
		ltcolor=ic_rbassoc->resval.rint;
	if (ic_assoc(elist2,62)!=0)	//not found on 2nd entity
		fi1=256;
	else
		fi1=ic_rbassoc->resval.rint;
	if (fi1!=ltcolor)
	{
		SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ltcolor=ic_colornum(rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=NULL;
	}
	//***********************************************************
	//BEGIN ENTITY MODIFY****************************************
	//***********************************************************
	if (RTNORM==sds_wcmatch(etype1,"LINE,XLINE,RAY"/*DNT*/))
	{
		if (strsame(etype1,db_hitype2str(DB_LINE)))
			l1type=0;
		else if (strsame(etype1,"RAY"/*DNT*/))
			l1type=1;
		else
			l1type=2;
		for (rbtemp=elist1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
		}
		if (0!=l1type)
		{
			p2[0]+=p1[0];
			p2[1]+=p1[1];
			p2[2]+=p1[2];
		}
		if (ptflag&1)
		{
/*
			//Move selected point onto the line itself
			sds_trans(ptsel1,&rbucs,&rbwcs,0,ptsel1);
			if ((fi1=ic_ptlndist(ptsel1,p1,p2,&fr1,ptsel1))==-1)
			{
				ret=(-1);goto exit;
			}
			if (fi1==0 && 0==l1type)
			{
				if (sds_distance(ptsel1,p1)<sds_distance(ptsel1,p2))
					ic_ptcpy(ptsel1,p1);
				else
					ic_ptcpy(ptsel1,p2);
			}
*/
			// the code above only works when the line is lying on the XY plane of 
			// current UCS or in the case the viewing direction is the same as 
			// the Z directions of the UCS - Bug Fixed by SWH, 2/12/2004
			//

			// project the line and selected points to the real projection in UCS.
			//
			sds_point ptemp1, ptemp2;
			sds_trans(p1,&rbwcs,&rbucs,0,ptemp1);
			sds_trans(p2,&rbwcs,&rbucs,0,ptemp2);
			gr_ucs2rp(ptemp1,ptemp1,view);
			gr_ucs2rp(ptemp2,ptemp2,view);
			gr_ucs2rp(ptsel1,ptsel1,view);

			// find the nearest point on the projected line from the selected point
			//
			if ((fi1 = ic_ptlndist(ptsel1,ptemp1,ptemp2,&fr1,ptsel1))==-2) { // errors
				ret=(-1);
				goto exit;
			}
			else if (fi1 == -1) 	// the line has coincident endpts
				ic_ptcpy(ptsel1,p1);
			else if (fi1 == 0 &&	// the selected point is not on the line segment
				     l1type == 0) {	// the entity is line
				if (sds_distance(ptsel1, ptemp1) < sds_distance(ptsel1, ptemp2))
					ic_ptcpy(ptsel1, p1);
				else
					ic_ptcpy(ptsel1, p2);
			} 
			else {	// the entity is ray or xline, or the point is one the line segment

				// find the point on the line, which is corresponding to the seleted 
				// point in the viewing direction.
				//
				double param =	icadRealEqual(ptemp2[0], ptemp1[0]) ? 
								(ptsel1[1] - ptemp1[1]) / (ptemp2[1] - ptemp1[1]) :
								(ptsel1[0] - ptemp1[0]) / (ptemp2[0] - ptemp1[0]) ;
				for (register i = 0; i < 3; i++)
					ptsel1[i] = p1[i] + param * (p2[i] - p1[i]);
			}
		}
		else
		{
			if (0==l1type)
			{
				ptsel1[0]=(p1[0]+p2[0])/2.0;
				ptsel1[1]=(p1[1]+p2[1])/2.0;
				ptsel1[2]=(p1[2]+p2[2])/2.0;
			}
			else
				ic_ptcpy(ptsel1,p1);
		}
		if (RTNORM==sds_wcmatch(etype2,"LINE,XLINE,RAY"/*DNT*/))
		{
			if (strsame(etype2,db_hitype2str(DB_LINE)))
				l2type=0;
			else if (strsame(etype2,"RAY"/*DNT*/))
				l2type=1;
			else
				l2type=2;

			for (rbtemp=elist2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(p3,rbtemp->resval.rpoint);
				else if (rbtemp->restype==11)
					ic_ptcpy(p4,rbtemp->resval.rpoint);
			}
			if (0!=l2type)
			{
				p4[0]+=p3[0];
				p4[1]+=p3[1];
				p4[2]+=p3[2];
			}
			if (ptflag&2)
			{
				/*
				sds_trans(ptsel2,&rbucs,&rbwcs,0,ptsel2);
				if ((fi1=ic_ptlndist(ptsel2,p3,p4,&fr1,ptsel2))==-1)
				{
					ret=(-1);goto exit;
				}
				if (fi1==0 && 0==l2type)
				{
					if (sds_distance(ptsel2,p3)<sds_distance(ptsel2,p4))
						ic_ptcpy(ptsel2,p3);
					else
						ic_ptcpy(ptsel2,p4);
				}
				*/

				// the code above only works when the line is lying on the XY plane of 
				// current UCS or in the case the viewing direction is the same as 
				// the Z directions of the UCS - Bug Fixed by SWH, 2/12/2004
				//

				// project the line and selected points to the real projection in UCS.
				//
				sds_point ptemp3, ptemp4;
				sds_trans(p3,&rbwcs,&rbucs,0,ptemp3);
				sds_trans(p4,&rbwcs,&rbucs,0,ptemp4);
				gr_ucs2rp(ptemp3,ptemp3,view);
				gr_ucs2rp(ptemp4,ptemp4,view);
				gr_ucs2rp(ptsel2,ptsel2,view);

				// find the nearest point on the projected line from the selected point
				//
				if ((fi1 = ic_ptlndist(ptsel2,ptemp3,ptemp4,&fr1,ptsel2)) == -2) {  // errors
					ret=(-1);
					goto exit;
				}
				else if (fi1 == -1) {	// the line has coincident endpts
					ic_ptcpy(ptsel2, p3);
				}
				else if (fi1 == 0 &&	// the selected point is not on the line segment
						 l2type == 0) {	// the entity is line
					if (sds_distance(ptsel2, ptemp3) < sds_distance(ptsel2, ptemp4))
						ic_ptcpy(ptsel2, p3);
					else
						ic_ptcpy(ptsel2, p4);
				} 
				else {	// the entity is ray or xline, or the point is one the line segment

					// find the point on the line, which is corresponding to the seleted 
					// point in the viewing direction.
					//
					double param =	icadRealEqual(ptemp4[0], ptemp3[0]) ? 
									(ptsel2[1] - ptemp3[1]) / (ptemp4[1] - ptemp3[1]) :
									(ptsel2[0] - ptemp3[0]) / (ptemp4[0] - ptemp3[0]);
					for (register i = 0; i < 3; i++)
						ptsel2[i] = p3[i] + param * (p4[i] - p3[i]);
				}
			}
			else
			{
				if (0==l2type)
				{
					ptsel2[0]=(p3[0]+p4[0])/2.0;
					ptsel2[1]=(p3[1]+p4[1])/2.0;
					ptsel2[2]=(p3[2]+p4[2])/2.0;
				}
				else
					ic_ptcpy(ptsel2,p3);
			}
			p7[0]=p7[1]=p7[2]=0.0;
			fi1=cmd_fillet_line2line(p1,p2,ptsel1,&l1type,p3,p4,ptsel2,&l2type,p5,p6,&fi2,p0,&fr2,&fr3,p7);
			if (-1==fi1)
			{
				ret=-1;
				cmd_ErrorPrompt(CMD_ERR_FILNOTPOSS,0);
				goto exit;
			}
			else if (-2==fi1)
			{
				ret=-1;
				cmd_ErrorPrompt(CMD_ERR_FILLETCAP,0);
				goto exit;
			}
			else if (-3==fi1)
			{
				ret=-1;
				cmd_ErrorPrompt(CMD_ERR_NOINTERSECT,0);
				goto exit;
			}
			else if (1==fi1 && (5==l1type || 5==l2type))
			{
				//changing ray to line and fillet point isn't on original ray
				ret=-1;
				cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
				goto exit;
			}
			//now we have fillet pts and center pt.
			//p5 is on p1-p2, p6 is on p3-p4.

			//**********************************************************
			//MODIFY EXISTING LINES*************************************
			//**********************************************************
			//moveflag is bit-coded: bit0 for first ent, bit1 for second ent. 0=move start,1=move end
			//NOTES: fi2 flags line pt to move:
			//			bit 0 for ent1 and bit1 for ent2
			//			if bit2 set, we're doing an end cap fillet
			//		 For rays & xlines, if bit is set, reverse the direction vector
			//			because we always move the 10pt
			//		 if l1type&4, or l2type&4, entity type is changing: ray to line or xline to ray
			if (trimmode)
			{
				if (fi2<4)
				{
					if ((4+1)==l1type)	//converting a ray into a line...
					{
						ic_assoc(elist1,11);
						ic_ptcpy(ic_rbassoc->resval.rpoint,p5);
					}
					else
					{
						if (0!=l1type || 0==(fi2&1))	//always move 10pt on an xline
						{
							ic_assoc(elist1,10);
							ic_ptcpy(ic_rbassoc->resval.rpoint,p5);
						}
						if (1==(fi2&1))
						{
							ic_assoc(elist1,11);
							if (0==l1type)
								ic_ptcpy(ic_rbassoc->resval.rpoint,p5);
							else
							{
								ic_rbassoc->resval.rpoint[0]=-ic_rbassoc->resval.rpoint[0];
								ic_rbassoc->resval.rpoint[1]=-ic_rbassoc->resval.rpoint[1];
								ic_rbassoc->resval.rpoint[2]=-ic_rbassoc->resval.rpoint[2];
							}
						}
					}
				}
				if ((4+1)==l2type)	//converting a ray into a line
				{
					ic_assoc(elist2,11);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p6);
				}
				else
				{
					if (0!=l2type || 0==(fi2&2))
					{
						ic_assoc(elist2,10);
						ic_ptcpy(ic_rbassoc->resval.rpoint,p6);
					}
					if (fi2&2)
					{
						ic_assoc(elist2,11);
						if (0==l2type)
							ic_ptcpy(ic_rbassoc->resval.rpoint,p6);
						else
						{
							ic_rbassoc->resval.rpoint[0]=-ic_rbassoc->resval.rpoint[0];
							ic_rbassoc->resval.rpoint[1]=-ic_rbassoc->resval.rpoint[1];
							ic_rbassoc->resval.rpoint[2]=-ic_rbassoc->resval.rpoint[2];
						}
					}
				}
				if (l1type>4)
				{
					ic_assoc(elist1,0);
					if (l1type&1)//a ray
					{
						if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL)
						{
							ret=(-__LINE__);
							goto exit;
						}
						strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));
					}
					else if (l1type&2)
					{
						if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL)
						{
							ret=(-__LINE__);
							goto exit;
						}
						strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
					}
					ret=sds_entmake(elist1);
				}
				else
					ret=sds_entmod(elist1);
				if (ret!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
				if (l1type>4) //if we did an entmake...
				{
					sds_entdel(ent1);
					sds_entlast(ent1);
					sds_redraw(ent1,IC_REDRAW_DRAW);
				}

				if (l2type>4)
				{
					ic_assoc(elist2,0);
					if (l2type&1)//a ray
					{
						if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL)
						{
							ret=(-__LINE__);
							goto exit;
						}
						strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));
					}
					else if (l2type&2)
					{
						if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL)
						{
							ret=(-__LINE__);
							goto exit;
						}
						strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
					}
					ret=sds_entmake(elist2);
				}
				else
					ret=sds_entmod(elist2);
				if (ret!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
				if (l2type>4) //if we did an entmake...
				{
					sds_entdel(ent2);
					sds_entlast(ent2);
					sds_redraw(ent2,IC_REDRAW_DRAW);
				}

				/*
				//let p7 be intersection pt
				ic_linexline(p1,p2,p3,p4,p7);	//int ok if we got back fillet pts ok
				if ((ret=cmd_linept4fillet(p1,p2,p7,ptsel1,p5,&fi1))==-1){goto exit;}
				ic_assoc(elist1,fi1);
				ic_ptcpy(ic_rbassoc->resval.rpoint,p5);
				if ((ret=cmd_linept4fillet(p3,p4,p7,ptsel2,p6,&fi1))==-1){goto exit;}
				ic_assoc(elist2,fi1);
				ic_ptcpy(ic_rbassoc->resval.rpoint,p6);
				if ((sds_entmod(elist1)!=RTNORM)||(sds_entmod(elist2)!=RTNORM)){
					ret=(-__LINE__);
					goto exit;
				}	*/
			}
			if (fi2&4)
				rb.resval.rreal=0.5*sds_distance(p5,p6);
			else
				if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
				ret=-1;goto exit;
			}
			if (rb.resval.rreal>0.0)
			{
				//create new arc entity
				FreeResbufIfNotNull(&rbp1);
				if (p7[2]*ucsz<0.0)
				{
					//extru we got back is antiparallel to current, so switch stuff around
					p7[0]=-p7[0];
					p7[1]=-p7[1];
					p7[2]=-p7[2];
					p0[0]=-p0[0];
					p0[2]=-p0[2];
					fr8=(3.0*IC_PI)-fr3;
					fr3=(3.0*IC_PI)-fr2;
					fr2=fr8;
				}
				rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
								   10,p0,40,rb.resval.rreal,50,fr2,51,fr3,210,p7,0);

				if (rbp1==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=-1;
					goto exit;
				}
				if (sds_entmake(rbp1)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			sds_redraw(ent1,IC_REDRAW_DRAW);
			sds_redraw(ent2,IC_REDRAW_DRAW);
		}
		else if (sds_wcmatch(etype2,"ARC,CIRCLE"/*DNT*/)==RTNORM)
		{
			fr3=fr2=0.0;
			for (rbtemp=elist2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(p0,rbtemp->resval.rpoint);
				else if (rbtemp->restype==40)
					fr1=rbtemp->resval.rreal;
				else if (rbtemp->restype==50 && strsame(etype2,"ARC"/*DNT*/))
					fr2=rbtemp->resval.rreal;
				else if (rbtemp->restype==51 && strsame(etype2,"ARC"/*DNT*/))
					fr3=rbtemp->resval.rreal;
				else if (rbtemp->restype==210)
					ic_ptcpy(p210,rbtemp->resval.rpoint);
			}
			rbent.restype=RT3DPOINT;
			ic_ptcpy(rbent.resval.rpoint,p210);
			//calc point to use if user didn't pick one...
			if (strsame(etype2,"ARC"/*DNT*/))
			{
				ic_normang(&fr2,&fr3);
				if (0==(ptflag&2))
					sds_polar(p0,(fr2+fr3)/2.0,fr1,ptsel2);
			}
			else if (0==(ptflag&2))
			{
				if (SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				{
					ret=-1;goto exit;
				}
				sds_polar(p0,rb.resval.rreal,fr1,ptsel2);
			}
			//put all points into arc system
			sds_trans(p1,&rbwcs,&rbent,0,p1);
			sds_trans(p2,&rbwcs,&rbent,0,p2);
			sds_trans(ptsel1,&rbwcs,&rbent,0,p8);
			cmd_elev_correct(p0[2],p8,3,p210);
			if (cmd_fillet_arc2line(p1,p2,p8,p0,fr1,ptsel2,p3,p4,p5,&fi2)==-1)
			{
				//p3 is pt on line, p4 is pt on arc
				//NOTE:  FI2 will tell us whether new arc goes clockwise (not ccw)
				//DON'T ALTER FR2 IN TRIMMING F'N'S!!!
				ret=-1;
				goto exit;
			}
			if (trimmode)
			{
				//get the pt where line & arc cross - call it p6
				fi1=ic_linexarc(p1,p2,p0,fr1,fr2,fr3,p6,p7);
				if (fi1==-1)
					ic_ptlndist(p0,p1,p2,&fr1,p6);
				else if (sds_distance(p7,p3)<sds_distance(p6,p3))
					ic_ptcpy(p6,p7);
				sds_trans(ptsel1,&rbwcs,&rbent,0,p8);
				if (-1==cmd_linept4fillet(p1,p2,p6,p8,p3,&fi1))
				{
					ret=-1;
					goto exit;
				}
				ic_assoc(elist1,fi1);
				sds_trans(p3,&rbent,&rbwcs,0,ic_rbassoc->resval.rpoint);
				if (sds_entmod(elist1)!=RTNORM)
				{
					ret=-1;goto exit;
				}
				if (strsame(etype2,"ARC"/*DNT*/ ))	 //we never modify a circle, so modify start/end of arc
				{
					fr4=sds_angle(p0,p4);
					cmd_arcang4fillet(fr4,sds_angle(p0,p6),fr2,fr3,sds_angle(p0,ptsel2),&fi1);
					ic_assoc(elist2,fi1);
					ic_rbassoc->resval.rreal=fr4;
					if (sds_entmod(elist2)!=RTNORM)
					{
						ret=-1;goto exit;
					}
				}
			}
			if (!icadPointEqual(p3,p4))
			{
				//create new arc entity
				FreeResbufIfNotNull(&rbp1);
				if (fi2!=0)
				{
					rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
									   10,p5,40,sds_distance(p3,p5),50,sds_angle(p5,p4),51,sds_angle(p5,p3),210,p210,0);
				}
				else
					rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
									   10,p5,40,sds_distance(p3,p5),50,sds_angle(p5,p3),51,sds_angle(p5,p4),210,p210,0);
				if (rbp1==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=-1;
					goto exit;
				}
				if (sds_entmake(rbp1)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			sds_redraw(ent1,IC_REDRAW_DRAW);
			sds_redraw(ent2,IC_REDRAW_DRAW);
		}
		else if (strsame(etype2,"POLYLINE"/*DNT*/ ) || strsame(etype2,"LWPOLYLINE"/*DNT*/))
		{
			bool ispolyline=true; // lwpolyline otherwise
			db_polyline *thepline=NULL;
			db_lwpolyline *thelwpline=NULL;
			int lwplvertno=0,intvertno=0;

			if (strsame(etype2,"LWPOLYLINE"/*DNT*/))
			{
				ispolyline=false;
				thelwpline=(db_lwpolyline *)ent2[0];//etype2[0];
			}
			else
				thepline=(db_polyline *)ent2[0]; //etype2[0];

			if (strsame(etype1,"XLINE"/*DNT*/ ) || strsame(etype1,"RAY"/*DNT*/ ))
			{
				//can't fillet pline to ray or xline
				cmd_ErrorPrompt(CMD_ERR_FILLETPLINE,0);
				ret=RTERROR;
				goto exit;
			}
			if (ispolyline)
			{
				closure=thepline->ret_70();
				thepline->get_210(p210);
				p8[2]=thepline->ret_elev();
			}
			else
			{
				closure=thelwpline->ret_70();
				thelwpline->get_210(p210);
				p8[2]=thelwpline->ret_38();
			}

			rbent.restype=RT3DPOINT;
			ic_ptcpy(rbent.resval.rpoint,p210);
			if (ispolyline && (closure & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
			{
				cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
				ret=RTERROR;
				goto exit;
			}
			else
				closure	&= IC_PLINE_CLOSED;
// TODO: from here on probably needs rewrite for LWPOLYLINE -- next 500 lines or so

			// we track the location in the lwpline with lwplvertno (an index), in the polyline
			// we follow etemp (I guess; trying not to change that stuff)
			lwplvertno=0;
			if (closure || 0==(ptflag&2))
			{
				sds_point thept;

				if (ispolyline)
				{
					sds_entnext_noxref(ent2,etemp);
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_entget(etemp))==NULL)
					{
						ret=-1;goto exit;
					}
					ic_assoc(rbp1,10);
					ic_ptcpy(thept,ic_rbassoc->resval.rpoint);
				}
				else
					thelwpline->get_10(thept,0);
				if (closure)
					ic_ptcpy(closept,thept);
				if (0==(ptflag&2))
					ic_ptcpy(ptsel2,thept);
			}
			else
				sds_trans(ptsel2,&rbucs,&rbent,0,ptsel2);
			sds_trans(p1,&rbwcs,&rbent,0,p1);
			sds_trans(p2,&rbwcs,&rbent,0,p2);
			sds_trans(ptsel1,&rbwcs,&rbent,0,ptsel1);
			cmd_elev_correct(p8[2],ptsel1,3,p210);
			if (ispolyline)
				ret=cmd_segxpoly(ent2,ptsel2,p1,p2,0.0,p0,&fr1,etemp);
			else
				ret=cmd_segxlwpoly(ent2,ptsel2,p1,p2,0.0,p0,&fr1,&intvertno);
			if (ret!=1)
				goto exit;
			ret=0;
			FreeResbufIfNotNull(&rbp1);
			if (fr1<0.0)	//picked pt before crossing on pline, so
			{
				//build new pline from start...
				if (ispolyline)
				{
					sds_name holdlastvert,stepvert;
					if ((rbp1=sds_entget(etemp))==NULL)
					{
						ret=-1;goto exit;
					}
					ic_assoc(rbp1,10);
					ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
					while (icadPointEqual(p3,p0))	   //go back one vertex
					{
						ic_encpy(stepvert,ent2);
						do
						{
							ic_encpy(holdlastvert,stepvert);
							if (sds_entnext_noxref(stepvert,stepvert)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
						while (stepvert[0]!=etemp[0] || stepvert[1]!=etemp[1]);
						ic_encpy(etemp,holdlastvert);
						FreeResbufIfNotNull(&rbp1);
						if ((rbp1=sds_entget(etemp))==NULL)
						{
							ret=-1;goto exit;
						}
						ic_assoc(rbp1,10);
						ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
					}
					if (icadPointEqual(p3,p0))
					{
						ret=-1;goto exit;
					}
					//make beginning portion of pline....
					if (trimmode)
						cmd_make_pline_start(ent2,etemp);
					ic_assoc(rbp1,42);
					fr2=ic_rbassoc->resval.rreal;
					cmd_pline_entnext(etemp,etemp2,closure);
					FreeResbufIfNotNull(&rbp2);
					if ((rbp2=sds_entget(etemp2))==NULL)
					{
						ret=-1;goto exit;
					}
					ic_assoc(rbp2,10);
					ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
				}
				else  // lwpolyline
				{
					int stepvertno=0;
					nextvertno=0;
					thelwpline->get_10(p3,intvertno);
					thevert = intvertno;
					while (icadPointEqual(p3,p0))	   //go back one vertex
					{
						stepvertno=0;
						do
						{
							nextvertno=stepvertno;
							stepvertno++;
						}
						while (stepvertno!=intvertno);
						thevert=nextvertno;
						thelwpline->get_10(p3,thevert);
					}
					if (icadPointEqual(p3,p0))
					{
						ret=-1;goto exit;
					}
					//make beginning portion of lwpline....
					nextvertno=cmd_lwpline_entnext(thevert,thelwpline);
					if (trimmode)
					{
						newlwpl=sds_entgetx(ent2,&rb);	// create new lwpline resbuf to build on
						lwpl=new db_lwpolyline(*thelwpline);	// a copy for us to use to build the new one
						LwplrbDeleteVertexData(newlwpl);
						lwpladdrb=newlwpl;
						while (lwpladdrb->rbnext!=NULL)
							lwpladdrb=lwpladdrb->rbnext;
						LwplrbAddVerts(lwpl, &lwpladdrb,0,/*thevert */nextvertno,false);
					}
					if ((rbp1=sds_entget(ent2))==NULL)
					{
						ret=-1;goto exit;
					}
					ic_assoc(rbp1,42);
					fr2=ic_rbassoc->resval.rreal;
					thelwpline->get_10(p4,nextvertno);
				}

// From here no difference btwn pline and lwpline for a while
				if (!icadRealEqual(fr2,0.0))
				{
/*					ic_ptcpy(p8,p1);
					if (!icadRealEqual(sds_angle(p0,p1),sds_angle(p0,p2))){
						ic_ptlndist(ptsel1,p1,p2,&fr9,p8);
						if (icadRealEqual(sds_angle(p0,p8),sds_angle(p0,p2)))
							ic_ptcpy(p8,p2);
					}  */ //if pt outside arc, it does an outer fillet!!
					if (ic_bulge2arc(p3,p4,fr2,p5,&fr3,&fr4,&fr5)==-1)
					{
						ret=-1;goto exit;
					}
					if ((fi1=ic_ptsegdist(ptsel2,p3,p4,fr2,&fr9))==0)
					{
						//fi2=cmd_fillet_arc2line(p0,p8,ptsel1,p5,fr3,ptsel2,p6,p7,p8,&fi1);
						fi2=cmd_fillet_arc2line(p1,p2,ptsel1,p5,fr3,ptsel2,p6,p7,p8,&fi1);
					}
					else if (fi1==1)
					{
						//fi2=cmd_fillet_arc2line(p0,p8,ptsel1,p5,fr3,p3,p6,p7,p8,&fi1);
						fi2=cmd_fillet_arc2line(p1,p2,ptsel1,p5,fr3,p3,p6,p7,p8,&fi1);
					}
					if (fi2<0)
					{
						ret=-1;
						goto exit;
					}
					//p6 is on line, p7 on pline
					if (!cmd_ang_betw(sds_angle(p5,p7),fr4,fr5))
					{
						//they may have picked the wrong end of the line for fillet,
						//and we can't extend segment....
						if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						sds_polar(p0,sds_angle(p0,ptsel1),rb.resval.rreal,ptsel1);
						if (cmd_fillet_arc2line(p1,p2,ptsel1,p5,fr3,p3,p6,p7,p8,&fi1)<0)
						{
							ret=-1;
							goto exit;
						}
						if (!cmd_ang_betw(sds_angle(p5,p7),fr4,fr5))
						{
							cmd_ErrorPrompt(CMD_ERR_FILCLOSEFAR,0);
							goto exit;
						}
					}

					if (!trimmode)
					{
						//get extru for arc..
						ic_ptcpy(p9,rbent.resval.rpoint);
						if (0==fi1)
						{
							p9[0]=-p9[0];
							p9[1]=-p9[1];
							p9[2]=-p9[2];
						}
					}
					fr6=sds_angle(p5,p7);//new ending angle
					if (fr2>0.0) //ccw bulge
					{
						ic_normang(&fr4,&fr6);
						fr3=fr6-fr4;
						fr7=tan(fr3/4.0);
						ic_normang(&fr4,&fr5);
						fr3=(fr3/(fr5-fr4));	//new ending width factor
					}
					else		 //clockwise
					{
						ic_normang(&fr6,&fr5);
						fr3=fr5-fr6;
						fr7=-tan(fr3/4.0);
						ic_normang(&fr4,&fr5);
						fr3=(fr3/(fr5-fr4));   //new ending width factor
					}
					if (trimmode)
					{
						ic_assoc(rbp1,42);
						ic_rbassoc->resval.rreal=fr7;
					}
					fr5=sds_angle(p8,p6);
					fr4=sds_angle(p8,p7);
					if (fi1)
					{
						ic_normang(&fr4,&fr5);
						if (trimmode)
							fr5=atan((fr5-fr4)/4.0);//angle to bulge
					}
					else
					{
						ic_normang(&fr5,&fr4);
						if (trimmode)
							fr5=-atan((fr4-fr5)/4.0);//angle to bulge
						else
						{
							p8[0]=-p8[0];
							p8[2]=-p8[2];
							fr5=(3.0*IC_PI)-fr5;
							fr4=(3.0*IC_PI)-fr4;
						}
					}
				}
				else
				{
					ic_ptcpy(p9,rbent.resval.rpoint);
					fi2=cmd_fillet_line2line(p1,p2,ptsel1,NULL,p3,p4,p3,NULL,p6,p7,&fi1,p8,&fr4,&fr5,p9);
					//p6 is on line, p7 on pline
					if (fi2<0 || (fi1&4))
					{
						if (-1==fi2)
							cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
						else if (-2==fi2)
							cmd_ErrorPrompt(CMD_ERR_FILNOTPOSS,0);
						else if (-3==fi2)
							cmd_ErrorPrompt(CMD_ERR_NOINTERSECT,0);
						ret=-1;
						goto exit;
					}
					//make sure extrusions are equal or opposites
					if (!icadPointEqual(p9,p210))
					{
						if (!icadRealEqual(p210[0],-p9[0]) ||
							!icadRealEqual(p210[1],-p9[1]) ||
							!icadRealEqual(p210[2],-p9[2]))
						{
							cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
							goto exit;
						}
						fr11=-1.0;
					}
					else
						fr11=1.0;
					if (fi2==1)
					{
						//check if p7 is actually ON pline (don't care about p6
						if ((fabs(p4[0]-p3[0]))>(fabs(p4[1]-p3[1])))
						{
							if (fabs(p4[2]-p3[2])>fabs(p4[0]-p3[0]))
								fi2=2;
							else
								fi2=0;
						}
						else if (fabs(p4[2]-p3[2])>fabs(p4[1]-p3[1]))
							fi2=2;
						else
							fi2=1;
						if ((p4[fi2]-p3[fi2])*(p7[fi2]-p3[fi2])<0.0)
						{
							cmd_ErrorPrompt(CMD_ERR_FILTOOCLOSE,0);
							goto exit;
						}
					}
					fr3=(sds_distance(p3,p7)/sds_distance(p3,p4));
					ic_normang(&fr4,&fr5);
					if (trimmode)
					{
						//convert from angles to bulge
						fr5=tan((fr5-fr4)/4.0);
						fr5*=fr11;
					}
				}
// resume split btwn lwpline and pline
				if (ispolyline)
				{
					if (trimmode) //make new break-off vertex & fillet vertex
					{
						fr10=fr11=0.0;

						//NOTE:assume 40 group before 41 group
						for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
						{
							if (rbtemp->restype==40)
								fr10=rbtemp->resval.rreal;
							else if (rbtemp->restype==41)
							{
								fr11=rbtemp->resval.rreal;
								fr9=rbtemp->resval.rreal=fr10+((fr11-fr10)*fr3);
							}
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						/*
						fr3=sds_angle(p8,p6);
						fr4=sds_angle(p8,p7);
						if (fi1){
							ic_normang(&fr4,&fr3);	//going from pline to line
							fr5=tan((fr3-fr4)/4.0);
							//if (fr2>0.0) fr5=-fr5;
						}else{
							ic_normang(&fr3,&fr4);
							fr5=-tan((fr4-fr3)/4.0);
							//fr5=tan((fr4-fr3)/4.0);
							//if (fr2<=0.0)fr5=-fr5;
						}*/
						FreeResbufIfNotNull(&rbp1);
						if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p7,40,fr9,41,0.0,42,fr5,0))==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						FreeResbufIfNotNull(&rbp1);
						if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,40,0.0,41,0.0,42,0.0,0))==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						if (-1==cmd_linept4fillet(p1,p2,p0,ptsel1,p6,&fi1))
						{
							ret=-1;
							goto exit;
						}
						FreeResbufIfNotNull(&rbp1);
						if (0==(fi1&1))	//if we need to move startpt, endpt is ok
						{
							if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,40,0.0,41,0.0,42,0.0,0))==NULL)
							{
								ret=-1;goto exit;
							}
						}
						else
						{
							if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p1,40,0.0,41,0.0,42,0.0,0))==NULL)
							{
								ret=-1;goto exit;
							}
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						FreeResbufIfNotNull(&rbp1);
						if ((rbp1=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						if (!strsame(etype1,"CIRCLE"/*DNT*/ ))
							sds_entdel(ent1);
						sds_entdel(ent2);
						sds_entlast(ent1);
						ic_encpy(ent2,ent1);
						sds_redraw(ent1,IC_REDRAW_DRAW);
					}
					else
					{
						if (!icadPointEqual(p7,p6))
						{
							if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
							{
								ret=-1;goto exit;
							}
							FreeResbufIfNotNull(&rbp1);
							rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
											   10,p8,40,rb.resval.rreal,50,fr4,51,fr5,210,p9,0);
							if (rbp1==NULL)
							{
								ret=-1;
								goto exit;
							}
							if (sds_entmake(rbp1)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
					}
				}
				else // LWPOLYLINE
				{
					if (trimmode) //make new break-off vertex & fillet vertex
					{
						fr10=fr11=0.0;

						//NOTE:assume 40 group before 41 group
						lwpl->get_40(&fr10,thevert);
						lwpl->get_41(&fr11,thevert);
						fr9=fr10+((fr11-fr10)*fr3);
						lwpl->set_41(fr9,thevert);

						LwplrbAddVertFromData(&lwpladdrb,p7,fr9,0.0,fr5);
						LwplrbAddVertFromData(&lwpladdrb,p6,0.0,0.0,0.0);

						if (-1==cmd_linept4fillet(p1,p2,p0,ptsel1,p6,&fi1))
						{
							ret=-1;
							goto exit;
						}
						FreeResbufIfNotNull(&rbp1);
						if (0==(fi1&1))	//if we need to move startpt, endpt is ok
							LwplrbAddVertFromData(&lwpladdrb,p2,0.0,0.0,0.0);
						else
							LwplrbAddVertFromData(&lwpladdrb,p1,0.0,0.0,0.0);

						LwplrbSetNumVerts(newlwpl);
						if (sds_entmake(newlwpl)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						FreeResbufIfNotNull(&newlwpl);
						if (!strsame(etype1,"CIRCLE"/*DNT*/ ))
							sds_entdel(ent1);
						sds_entdel(ent2);
						sds_entlast(ent1);
						ic_encpy(ent2,ent1);
						sds_redraw(ent1,IC_REDRAW_DRAW);
						delete lwpl;
					}
					else
					{
						if (!icadPointEqual(p7,p6))
						{
							if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
							{
								ret=-1;goto exit;
							}
							FreeResbufIfNotNull(&rbp1);
							rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
											   10,p8,40,rb.resval.rreal,50,fr4,51,fr5,210,p9,0);
							if (rbp1==NULL)
							{
								ret=-1;
								goto exit;
							}
							if (sds_entmake(rbp1)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
					}
				}
				// END OF LWPOLYLINE
			}
			else	//picked pt after crossing, so build new pline from end...
			{
				if (sds_entnext_noxref(ent2,etemp2)!=RTNORM)
				{
					ret=-1;goto exit;
				}
				if (trimmode)
				{
					if (cmd_make_pline_start(ent2,etemp2)!=0)
					{
						ret=(-__LINE__);
						goto exit;
					}
				}
				get_etemp:
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_entget(etemp))==NULL)
				{
					ret=-1;goto exit;
				}
				for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
				{
					if (rbtemp->restype==10)
						ic_ptcpy(p3,rbtemp->resval.rpoint);
					else if (rbtemp->restype==42)
						fr2=rbtemp->resval.rreal;
				}
				cmd_pline_entnext(etemp,etemp2,closure);
				FreeResbufIfNotNull(&rbp2);
				if ((rbp2=sds_entget(etemp2))==NULL)
				{
					ret=-1;goto exit;
				}
				ic_assoc(rbp2,10);
				ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
				if (icadPointEqual(p4,p0))
				{
					//if the intersect pt is right at the start of the next
					//  vtx and pick pt was after the intersect, walk etemp
					//	down one more entity - a boundary condition
					ic_encpy(etemp,etemp2);
					goto get_etemp;
				}
				if (!icadRealEqual(fr2,0.0))
				{
					if (ic_bulge2arc(p3,p4,fr2,p5,&fr3,&fr4,&fr5)==-1)
					{
						ret=-1;goto exit;
					}
					if ((fi1=ic_ptsegdist(ptsel2,p3,p4,fr2,&fr9))==0)
					{
						fi2=cmd_fillet_arc2line(p1,p2,ptsel1,p5,fr3,ptsel2,p6,p7,p8,&fi1);
					}
					else if (fi1==1)
					{
						fi2=cmd_fillet_arc2line(p1,p2,ptsel1,p5,fr3,p4,p6,p7,p8,&fi1);
					}
					if (fi2<0)
					{
						ret=-1;
						goto exit;
					}
					//p6 is on line, p7 on pline
					if (!cmd_ang_betw(sds_angle(p5,p7),fr4,fr5))
					{
						//they may have picked the wrong end of the line for fillet,
						//and we can't extend segment....
						if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						sds_polar(p0,sds_angle(p0,ptsel1),rb.resval.rreal,ptsel1);
						if (cmd_fillet_arc2line(p1,p2,ptsel1,p5,fr3,p4,p6,p7,p8,&fi1)<0)
						{
							ret=-1;
							goto exit;
						}
						if (!cmd_ang_betw(sds_angle(p5,p7),fr4,fr5))
						{
							cmd_ErrorPrompt(CMD_ERR_FILCLOSEFAR,0);
							goto exit;
						}
					}
					if (!trimmode)
					{
						//get extru for arc..
						ic_ptcpy(p9,rbent.resval.rpoint);
						if (0==fi1)
						{
							p9[0]=-p9[0];
							p9[1]=-p9[1];
							p9[2]=-p9[2];
						}
					}
					fr6=sds_angle(p5,p7);//new ending angle
					if (fr2>0.0) //ccw bulge
					{
						ic_normang(&fr6,&fr5);
						fr3=fr5-fr6;
						fr7=tan(fr3/4.0);
						ic_normang(&fr4,&fr5);
						fr3=1.0-(fr3/(fr5-fr4));	//new ending width factor

					}
					else		 //clockwise
					{
						ic_normang(&fr4,&fr6);
						fr3=fr6-fr4;
						fr7=-tan(fr3/4.0);
						ic_normang(&fr4,&fr5);
						fr3=1.0-(fr3/(fr5-fr4));   //new ending width factor
					}
					if (trimmode)
					{
						ic_assoc(rbp1,42);
						ic_rbassoc->resval.rreal=fr7;
					}
					fr5=sds_angle(p8,p6);
					fr4=sds_angle(p8,p7);
					if (fi1)
					{
						ic_normang(&fr4,&fr5);
						if (trimmode)fr5=-atan((fr5-fr4)/4.0);
					}
					else
					{
						ic_normang(&fr5,&fr4);
						if (trimmode)
							fr5=atan((fr4-fr5)/4.0);
						else
						{
							p8[0]=-p8[0];
							p8[2]=-p8[2];
							fr5=(3.0*IC_PI)-fr5;
							fr4=(3.0*IC_PI)-fr4;
						}
					}

				}
				else
				{
					ic_ptcpy(p9,rbent.resval.rpoint);
					fi2=cmd_fillet_line2line(p1,p2,ptsel1,NULL,p3,p4,p4,NULL,p6,p7,&fi1,p8,&fr4,&fr5,p9);
					//p6 is on line, p7 on pline
					if (fi2==-1 || (fi1&4))
					{
						if (-1==fi2)
							cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
						else if (-2==fi2)
							cmd_ErrorPrompt(CMD_ERR_FILNOTPOSS,0);
						else if (-3==fi2)
							cmd_ErrorPrompt(CMD_ERR_NOINTERSECT,0);
						ret=-1;
						goto exit;
					}
					//make sure extrusions are equal or opposite
					if (!icadPointEqual(p9,p210))
					{
						if (!icadRealEqual(p210[0],-p9[0]) ||
							!icadRealEqual(p210[1],-p9[1]) ||
							!icadRealEqual(p210[2],-p9[2]))
						{
							cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
							goto exit;
						}
						fr11=1.0;
					}
					else
						fr11=-1.0;
					if (fi2==1)
					{
						//check if p7 is actually ON pline (don't care about p6
						if ((fabs(p4[0]-p3[0]))>(fabs(p4[1]-p3[1])))
						{
							if (fabs(p4[2]-p3[2])>fabs(p4[0]-p3[0]))
								fi2=2;
							else
								fi2=0;
						}
						else if (fabs(p4[2]-p3[2])>fabs(p4[1]-p3[1]))
							fi2=2;
						else
							fi2=1;
						if ((p4[fi2]-p3[fi2])*(p7[fi2]-p3[fi2])<0.0)
						{
							cmd_ErrorPrompt(CMD_ERR_FILTOOCLOSE,0);
							goto exit;
						}
					}
					ic_normang(&fr4,&fr5);
					if (trimmode)
					{
						fr5=tan((fr5-fr4)/4.0);//convert from ang to bulge
						fr5*=fr11;
					}
					fr3=(sds_distance(p3,p7)/sds_distance(p3,p4));
				}

				if (trimmode) //make new break-off vertex & fillet vertex
				{
					fr10=fr11=0.0;
					//note: assume 40 resbuf before 41
					for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
					{
						if (rbtemp->restype==10)
							ic_ptcpy(rbtemp->resval.rpoint,p7);
						else if (rbtemp->restype==40)
							fr10=rbtemp->resval.rreal;
						else if (rbtemp->restype==41)
						{
							fr11=rbtemp->resval.rreal;
							fr9=rbtemp->resval.rreal=fr10+((fr11-fr10)*fr3);
						}
					}
					//don't mess w/rbp1 - we need to make vertex at appropriate place
					//if (sds_entmake(rbp1)!=RTNORM){ret=-1;goto exit;}
					/*fr3=sds_angle(p8,p6);
					fr4=sds_angle(p8,p7);
					if (fi1){
						ic_normang(&fr4,&fr3);	//going from pline to line
						fr5=-tan((fr3-fr4)/4.0);
						//fr5=tan((fr3-fr4)/4.0);
						//if (fr2<=0.0) fr5=-fr5;
					}else{
						ic_normang(&fr3,&fr4);
						fr5=tan((fr4-fr3)/4.0);
						//if (fr2>0.0)fr5=-fr5;
					}
					if (-1==cmd_linept4fillet(p1,p2,p0,ptsel1,p6,&fi1)){
						ret=-1;
						goto exit;
					}*/
					cmd_linept4fillet(p1,p2,p0,ptsel1,p6,&fi1);
					FreeResbufIfNotNull(&rbp2);
					if (0==(fi1&1))	//if we need to move startpt, endpt is ok
					{
						if ((rbp2=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,40,0.0,41,0.0,42,0.0,0))==NULL)
						{
							ret=-1;goto exit;
						}
					}
					else
					{
						if ((rbp2=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p1,40,0.0,41,0.0,42,0.0,0))==NULL)
						{
							ret=-1;goto exit;
						}
					}
					if (sds_entmake(rbp2)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					FreeResbufIfNotNull(&rbp2);
					if ((rbp2=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,40,0.0,41,fr9,42,fr5,0))==NULL)
					{
						ret=-1;goto exit;
					}
					if (sds_entmake(rbp2)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					if (sds_entmake(rbp1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					if (cmd_make_pline_end(etemp,closure&1,closept)!=0)
					{
						ret=-1;goto exit;
					}
					if (!strsame(etype1,"CIRCLE"/*DNT*/ ))
						sds_entdel(ent1);
					sds_entdel(ent2);
					sds_entlast(ent1);
					ic_encpy(ent2,ent1);
					sds_redraw(ent1,IC_REDRAW_DRAW);
				}
				else
				{
					if (!icadPointEqual(p7,p6))
					{
						//just make an arc..
						if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						FreeResbufIfNotNull(&rbp1);
						rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
										   10,p8,40,rb.resval.rreal,50,fr4,51,fr5,210,p9,0);
						if (rbp1==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
					}
				}
			}
		}
	}
	else if (sds_wcmatch(etype1,"ARC,CIRCLE"/*DNT*/)==RTNORM)
	{
		//fillet an arc or circle with another object
		fr2=fr3=0.0;
		for (rbtemp=elist1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
		{
			if (rbtemp->restype==10)
				ic_ptcpy(p0,rbtemp->resval.rpoint);
			else if (rbtemp->restype==40)
				fr1=rbtemp->resval.rreal;
			else if (rbtemp->restype==50 && strsame(etype1,"ARC"/*DNT*/))
				fr2=rbtemp->resval.rreal;
			else if (rbtemp->restype==51 && strsame(etype1,"ARC"/*DNT*/))
				fr3=rbtemp->resval.rreal;
			else if (rbtemp->restype==210)
				ic_ptcpy(p210,rbtemp->resval.rpoint);
		}
		rbent.restype=RT3DPOINT;
		ic_ptcpy(rbent.resval.rpoint,p210);
		if (ptflag&1)
		{
			sds_trans(ptsel1,&rbucs,&rbent,0,ptsel1);
			cmd_elev_correct(p0[2],ptsel1,3,p210);
		}
		//calc point to use if user didn't pick one...
		if (strsame(etype1,"ARC"/*DNT*/ ))
		{
			ic_normang(&fr2,&fr3);
			if (0==(ptflag&1))
				sds_polar(p0,(fr2+fr3)/2.0,fr1,ptsel1);
		}
		else if (0==(ptflag&1))
		{
			if (SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
				ret=-1;goto exit;
			}
			sds_polar(p0,rb.resval.rreal,fr1,ptsel1);
		}


		if (strsame(etype2,db_hitype2str(DB_LINE)))
		{
			for (rbtemp=elist2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(p1,rbtemp->resval.rpoint);
				if (rbtemp->restype==11)
					ic_ptcpy(p2,rbtemp->resval.rpoint);
			}
			if (0==(ptflag&2))
			{
				fi1=1;
				ptsel2[0]=(p1[0]+p2[0])/2.0;
				ptsel2[1]=(p1[1]+p2[1])/2.0;
				ptsel2[2]=(p1[2]+p2[2])/2.0;
			}
			else
				sds_trans(ptsel2,&rbucs,&rbwcs,0,ptsel2);
			sds_trans(p1,&rbwcs,&rbent,0,p1);
			sds_trans(p2,&rbwcs,&rbent,0,p2);
			sds_trans(ptsel2,&rbwcs,&rbent,0,ptsel2);
			cmd_elev_correct(p0[2],ptsel2,3,p210);
			if ((fi1=ic_ptlndist(ptsel2,p1,p2,&fr4,ptsel2))==-1)
			{
				ret=(-1);
				goto exit;
			}

			if (fi1==0)
			{
				if (sds_distance(ptsel2,p1)<sds_distance(ptsel2,p2))
					ic_ptcpy(ptsel2,p1);
				else
					ic_ptcpy(ptsel2,p2);
			}
			if (cmd_fillet_arc2line(p1,p2,ptsel2,p0,fr1,ptsel1,p3,p4,p5,&fi2)==-1)
			{
				//p3 is pt on line, p4 is pt on arc
				//NOTE:  FI2 will tell us whether new arc goes clockwise (not ccw)
				//DON'T ALTER FR2 IN TRIMMING F'N'S!!!
				ret=-1;
				goto exit;
			}
			if (trimmode)
			{
				//get the pt where line & arc cross - call it p6
				fi1=ic_linexarc(p1,p2,p0,fr1,fr2,fr3,p6,p7);
				if (fi1==-1)
					ic_ptlndist(p0,p1,p2,&fr4,p6);
				else if (sds_distance(p7,p3)<sds_distance(p6,p3))
					ic_ptcpy(p6,p7);
				if (-1==cmd_linept4fillet(p1,p2,p6,ptsel2,p3,&fi1))
				{
					ret=-1;
					goto exit;
				}
				ic_assoc(elist2,fi1);
				ic_ptcpy(ic_rbassoc->resval.rpoint,p3);
				if (sds_entmod(elist2)!=RTNORM)
				{
					ret=-1;goto exit;
				}
				if (strsame(etype1,"ARC"/*DNT*/ ))	 //we never modify a circle, so modify start/end of arc
				{
					fr4=sds_angle(p0,p4);
					cmd_arcang4fillet(fr4,sds_angle(p0,p6),fr2,fr3,sds_angle(p0,ptsel1),&fi1);
					ic_assoc(elist1,fi1);
					ic_rbassoc->resval.rreal=fr4;
					if (sds_entmod(elist1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
				}
			}
			if (!icadPointEqual(p3,p4))
			{
				//create new arc entity
				FreeResbufIfNotNull(&rbp1);
				if (fi2!=0)
				{
					rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
									   10,p5,40,sds_distance(p3,p5),50,sds_angle(p5,p4),51,sds_angle(p5,p3),210,p210,0);
				}
				else
				{
					rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
									   10,p5,40,sds_distance(p3,p5),50,sds_angle(p5,p3),51,sds_angle(p5,p4),210,p210,0);
				}
				if (rbp1==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=-1;
					goto exit;
				}
				if (sds_entmake(rbp1)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			sds_redraw(ent1,IC_REDRAW_DRAW);
			sds_redraw(ent2,IC_REDRAW_DRAW);
		}
		else if (sds_wcmatch(etype2,"ARC,CIRCLE"/*DNT*/)==RTNORM)
		{
			fr5=fr6=0;
			for (rbtemp=elist2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					ic_ptcpy(p1,rbtemp->resval.rpoint);
				else if (rbtemp->restype==40)
					fr4=rbtemp->resval.rreal;
				else if (rbtemp->restype==50 && strsame(etype2,"ARC"/*DNT*/))
					fr5=rbtemp->resval.rreal;
				else if (rbtemp->restype==51 && strsame(etype2,"ARC"/*DNT*/))
					fr6=rbtemp->resval.rreal;
				else if (rbtemp->restype==210)
					ic_ptcpy(p210,rbtemp->resval.rpoint);
			}
			if (!icadPointEqual(p210,rbent.resval.rpoint))
			{
				p210[0]=-p210[0];
				p210[1]=-p210[1];
				p210[2]=-p210[2];
				if (!icadPointEqual(p210,rbent.resval.rpoint) ||
					!icadRealEqual(p0[2],-p1[2]))
				{
					cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
					ret=RTERROR;
					goto exit;
				}
				//if they're antiparallel...
				p1[0]=-p1[0];
				p1[2]=-p1[2];
				p8[0]=(3.0*IC_PI)-fr5;
				fr5=(3.0*IC_PI)-fr6;
				fr6=p8[0];
			}
			else if (!icadRealEqual(p0[2],p1[2]))
			{
				cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
				ret=RTERROR;
				goto exit;
			}
			if (ptflag&2)
				sds_trans(ptsel2,&rbucs,&rbent,0,ptsel2);
			cmd_elev_correct(p1[2],ptsel2,3,p210);
			//calc point to use if user didn't pick one...
			if (strsame(etype1,"ARC"/*DNT*/ ))
			{
				ic_normang(&fr5,&fr6);
				if (0==(ptflag&2))
					sds_polar(p1,(fr5+fr6)/2.0,fr4,ptsel2);
			}
			else if (0==(ptflag&2))
			{
				if (SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				{
					ret=-1;goto exit;
				}
				sds_polar(p1,rb.resval.rreal,fr4,ptsel2);
			}
			if (cmd_fillet_arc2arc(p0,fr1,ptsel1,p1,fr4,ptsel2,p2,p3,p4,&fi2)==-1)
			{
				ret=(-1);
				goto exit;
			}
			//DON'T USE FI2 - we need it for clockwise/ccw arc creation!
			if (trimmode)
			{
				//we'll need arc "intersection" points for trimming determinations
				//if they don't intersect, take closest point
				if ((strsame(etype1,"ARC"/*DNT*/ ))||(strsame(etype2,"ARC"/*DNT*/ )))
				{
					if (ic_arcxarc(p0,fr1,0.0,0.0,p1,fr4,0.0,0.0,p5,p6)==-1)
					{
						//either they're too far apart or one's inside the other
						if (sds_distance(p0,p1)<fr4)
							sds_polar(p0,sds_angle(p1,p0),fr1,p5);
						else
							sds_polar(p0,sds_angle(p0,p1),fr1,p5);
					}
					else if (sds_distance(p6,ptsel1)<sds_distance(p5,ptsel1))
						ic_ptcpy(p5,p6);
				}
				if (strsame(etype1,"ARC"/*DNT*/ ))	 //we never modify a circle, so modify start/end of arc
				{
					fr7=sds_angle(p0,p2);
					cmd_arcang4fillet(fr7,sds_angle(p0,p5),fr2,fr3,sds_angle(p0,ptsel1),&fi1);
					ic_assoc(elist1,fi1);
					ic_rbassoc->resval.rreal=fr7;
					if (sds_entmod(elist1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
				}
				if (strsame(etype2,"ARC"/*DNT*/ ))	 //we never modify a circle, so modify start/end of arc
				{
					fr7=sds_angle(p1,p3);
					cmd_arcang4fillet(fr7,sds_angle(p1,p5),fr5,fr6,sds_angle(p1,ptsel2),&fi1);
					ic_assoc(elist2,fi1);
					ic_rbassoc->resval.rreal=fr7;
					if (sds_entmod(elist2)!=RTNORM)
					{
						ret=-1;goto exit;
					}
				}
			}
			if (!icadPointEqual(p2,p3))
			{
				//create new arc entity
				FreeResbufIfNotNull(&rbp1);
				if (fi2!=0)
				{
					rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
									   10,p4,40,sds_distance(p3,p4),50,sds_angle(p4,p3),51,sds_angle(p4,p2),210,p210,0);
				}
				else
				{
					rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
									   10,p4,40,sds_distance(p3,p4),50,sds_angle(p4,p2),51,sds_angle(p4,p3),210,p210,0);
				}
				if (rbp1==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=-1;
					goto exit;
				}
				if (sds_entmake(rbp1)!=RTNORM)
				{
					ret=(-__LINE__);
					goto exit;
				}
			}
			sds_redraw(ent1,IC_REDRAW_DRAW);
			sds_redraw(ent2,IC_REDRAW_DRAW);

		}
		else if (strsame(etype2,"POLYLINE"/*DNT*/ ))
		{
			//arc crossing pline
			for (rbtemp=elist2; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
			{
				if (rbtemp->restype==10)
					p1[2]=rbtemp->resval.rpoint[2];
				else if (rbtemp->restype==70)
					closure=rbtemp->resval.rint;
				else if (rbtemp->restype==210)
					ic_ptcpy(p210,rbtemp->resval.rpoint);
			}
			if (closure & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
			{
				cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
				ret=RTERROR;
				goto exit;
			}
			else
				closure	&= IC_PLINE_CLOSED;
			if (!icadPointEqual(p210,rbent.resval.rpoint))
			{
				rbent.resval.rpoint[0]=-rbent.resval.rpoint[0];
				rbent.resval.rpoint[1]=-rbent.resval.rpoint[1];
				rbent.resval.rpoint[2]=-rbent.resval.rpoint[2];
				if (!icadPointEqual(p210,rbent.resval.rpoint) ||
					!icadRealEqual(-p0[2],p1[2]))
				{
					cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
					ret=RTERROR;
					goto exit;
				}
				//move arc's data into ucs of polyline
				p0[0]=-p0[0];
				p0[2]=-p0[2];
				ptsel1[0]=-ptsel1[0];
				ptsel1[2]=-ptsel1[2];
				p8[0]=(3.0*IC_PI)-fr2;
				fr2=(3.0*IC_PI)-fr3;
				fr3=fr2;

			}
			else if (!icadRealEqual(p0[2],p1[2]))
			{
				cmd_ErrorPrompt(CMD_ERR_COPLANAR,0);
				ret=RTERROR;
				goto exit;
			}

			if (closure || 0==(ptflag&2))
			{
				sds_entnext_noxref(ent2,etemp);
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_entget(etemp))==NULL)
				{
					ret=-1;goto exit;
				}
				ic_assoc(rbp1,10);
				if (closure)
					ic_ptcpy(closept,ic_rbassoc->resval.rpoint);
				if (0==(ptflag&2))
					ic_ptcpy(ptsel2,ic_rbassoc->resval.rpoint);
			}
			else
				sds_trans(ptsel2,&rbucs,&rbent,0,ptsel2);

			if (strsame(etype1,"ARC"/*DNT*/ ))
			{
				sds_polar(p0,fr2,fr1,p1);
				sds_polar(p0,fr3,fr1,p2);
				ic_normang(&fr2,&fr3);
				fr4=tan((fr3-fr2)/4.0);
				ret=cmd_segxpoly(ent2,ptsel2,p1,p2,fr4,p3,&fr5,etemp);
			}
			else
				ret=cmd_segxpoly(ent2,ptsel2,p0,p0,fr1,p3,&fr5,etemp);
			if (ret!=1)
				goto exit;
			ret=0;
			FreeResbufIfNotNull(&rbp1);
			if ((rbp1=sds_entget(etemp))==NULL)
			{
				ret=-1;goto exit;
			}
			if (fr5<0.0)  //picked pt before crossing on pline, so
			//build new pline from start...
			{
				ic_assoc(rbp1,10);
				ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
				while (icadPointEqual(p3,p1))	   //go back one vertex
				{
					ic_encpy(etemp3,ent2);
					do
					{
						ic_encpy(etemp2,etemp3);
						if (sds_entnext_noxref(etemp3,etemp3)!=RTNORM)
						{
							ret=-1;goto exit;
						}
					}
					while (etemp3[0]!=etemp[0] || etemp3[1]!=etemp[1]);
					ic_encpy(etemp,etemp2);
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_entget(etemp))==NULL)
					{
						ret=-1;goto exit;
					}
					ic_assoc(rbp1,10);
					ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
				}
				if (icadPointEqual(p3,p1))
				{
					ret=-1;
					goto exit;
				}
				//make beginning portion of pline....
				if (trimmode)
					cmd_make_pline_start(ent2,etemp);
				ic_assoc(rbp1,42);
				fr6=ic_rbassoc->resval.rreal;
				cmd_pline_entnext(etemp,etemp2,closure);
				FreeResbufIfNotNull(&rbp2);
				if ((rbp2=sds_entget(etemp2))==NULL)
				{
					ret=-1;goto exit;
				}
				ic_assoc(rbp2,10);
				ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
				if (!icadRealEqual(fr6,0.0))
				{
					if (ic_bulge2arc(p1,p2,fr6,p5,&fr7,&fr8,&fr9)==-1)
					{
						ret=-1;goto exit;
					}
					if (cmd_ang_betw(sds_angle(p5,ptsel2),fr8,fr9))
						fi2=cmd_fillet_arc2arc(p0,fr1,ptsel1,p5,fr7,ptsel2,p6,p7,p8,&fi1);
					else
						fi2=cmd_fillet_arc2arc(p0,fr1,ptsel1,p5,fr7,p1,p6,p7,p8,&fi1);
					if (fi2<0)
					{
						ret=-1;
						goto exit;
					}
					//p6 is on arc, p7 on pline
					if (!cmd_ang_betw(sds_angle(p5,p7),fr8,fr9))
					{
						//they may have picked the wrong end of the line for fillet,
						//and we can't extend segment....
						if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						sds_polar(p3,sds_angle(p3,ptsel1),rb.resval.rreal,ptsel1);
						if (cmd_fillet_arc2arc(p0,fr1,ptsel1,p5,fr7,p1,p6,p7,p8,&fi1)<0)
						{
							ret=-1;
							goto exit;
						}
						if (!cmd_ang_betw(sds_angle(p5,p7),fr8,fr9))
						{
							cmd_ErrorPrompt(CMD_ERR_FILCLOSEFAR,0);
							goto exit;
						}
					}
					fr4=sds_angle(p5,p7);//new starting angle
					if (fr6>0.0) //ccw bulge
					{
						ic_normang(&fr8,&fr4);
						fr11=fr4-fr8;
						fr10=tan(fr11/4.0);
						ic_normang(&fr8,&fr9);
						fr11=(fr11/(fr9-fr8));	//new ending width factor
					}
					else
					{		 //clockwise
						ic_normang(&fr4,&fr9);
						fr11=fr9-fr4;
						fr10=-tan(fr11/4.0);
						ic_normang(&fr8,&fr9);
						fr11=(fr11/(fr9-fr8));	 //new ending width factor
					}
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr10;
				}
				else
				{
					//fillet str seg on pline to crossing arc
					if (ic_ptlndist(ptsel2,p1,p2,&fr7,p8)==1)
						fi2=cmd_fillet_arc2line(p1,p2,ptsel2,p0,fr1,ptsel1,p7,p6,p8,&fi1);
					else
						fi2=cmd_fillet_arc2line(p1,p2,p1,p0,fr1,ptsel1,p7,p6,p8,&fi1);
					fi1=(fi1 ^ 1);
					//p7 is on pline, p6 on arc
					if (ic_ptlndist(p6,p1,p2,&fr10,p4)==0)
					{
						cmd_ErrorPrompt(CMD_ERR_FILTOOCLOSE,0);
						goto exit;
					}
					fr11=(sds_distance(p1,p7)/sds_distance(p1,p2));
				}
				fr4=fr5=0.0;
				for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
				{
					if (rbtemp->restype==40)fr4=rbtemp->resval.rreal;
					if (rbtemp->restype==41)
					{
						fr5=rbtemp->resval.rreal;
						rbtemp->resval.rreal=fr10=fr4+((fr5-fr4)*fr11);
					}
				}
				if (trimmode) //make new break-off vertex & fillet vertex
				{
					if (sds_entmake(rbp1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					fr4=sds_angle(p8,p6);
					fr5=sds_angle(p8,p7);
					if (fi1)
					{
						ic_normang(&fr5,&fr4);	//going from pline to arc
						fr11=tan((fr4-fr5)/4.0);
						//if (fr6>0.0) fr11=-fr11;
					}
					else
					{
						ic_normang(&fr4,&fr5);
						fr11=-tan((fr5-fr4)/4.0);
						//if (fr2<=0.0)fr11=-fr11;
					}
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p7,40,fr10,41,0.0,42,fr11,0))==NULL)
					{
						ret=-1;goto exit;
					}
					if (sds_entmake(rbp1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					FreeResbufIfNotNull(&rbp1);
					//make new vertex along arc at end of fillet
					fr11=sds_angle(p0,p6);
					if (strsame(etype1,"ARC"/*DNT*/ ))
					{
						cmd_arcang4fillet(fr11,sds_angle(p0,p3),fr2,fr3,sds_angle(p0,ptsel1),&fi2);
						if (fi2==50)	//if we're adjusting arc's start angle....
						{
							ic_normang(&fr11,&fr3);
							fr10=tan((fr3-fr11)/4.0);
							sds_polar(p0,fr3,fr1,p2);
						}
						else
						{
							ic_normang(&fr2,&fr11);
							fr10=-tan((fr11-fr2)/4.0);
							sds_polar(p0,fr2,fr1,p2);
						}
						if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,40,0.0,41,0.0,42,fr10,0))==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						FreeResbufIfNotNull(&rbp1);
						if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,40,0.0,41,0.0,42,0.0,0))==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
					}
					else
					{
						if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,40,0.0,41,0.0,42,0.0,0))==NULL)
						{
							ret=-1;goto exit;
						}
						if (sds_entmake(rbp1)!=RTNORM)
						{
							ret=-1;goto exit;
						}
					}
					FreeResbufIfNotNull(&rbp1);
					if ((rbp1=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
					{
						ret=-1;goto exit;
					}
					if (sds_entmake(rbp1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					if (!strsame(etype1,"CIRCLE"/*DNT*/ ))
						sds_entdel(ent1);
					sds_entdel(ent2);
					sds_entlast(ent1);
					ic_encpy(ent2,ent1);
					sds_redraw(ent1,IC_REDRAW_DRAW);
				}
				else
				{
					if (!icadPointEqual(p7,p6))
					{
						//just make an arc..
						FreeResbufIfNotNull(&rbp1);
						if (fi1)	//clockwise....
						{
							rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
											   10,p8,40,sds_distance(p8,p6),50,sds_angle(p8,p7),51,sds_angle(p8,p6),210,p210,0);
							if (rbp1==NULL)
							{
								ret=-1;goto exit;
							}
							if (sds_entmake(rbp1)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
						else
						{
							rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
											   10,p8,40,sds_distance(p8,p6),50,sds_angle(p8,p6),51,sds_angle(p8,p7),210,p210,0);
							if (rbp1==NULL)
							{
								ret=-1;goto exit;
							}
							if (sds_entmake(rbp1)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
					}
				}
			}
			else
			{	//picked pt after crossing, so build new pline from end...
				if (sds_entnext_noxref(ent2,etemp2)!=RTNORM)
				{
					ret=-1;goto exit;
				}
				if (trimmode)
				{
					if (cmd_make_pline_start(ent2,etemp2)!=0)
					{
						ret=(-__LINE__);
						goto exit;
					}
				}
				for (rbtemp=rbp1; rbtemp!=NULL; rbtemp=rbtemp->rbnext)
				{
					if (rbtemp->restype==10)
						ic_ptcpy(p1,rbtemp->resval.rpoint);
					if (rbtemp->restype==42)
						fr6=rbtemp->resval.rreal;
				}
				cmd_pline_entnext(etemp,etemp2,closure);
				FreeResbufIfNotNull(&rbp2);
				if ((rbp2=sds_entget(etemp2))==NULL)
				{
					ret=-1;goto exit;
				}
				ic_assoc(rbp2,10);
				ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
				if (!icadRealEqual(fr6,0.0))
				{
					if (ic_bulge2arc(p1,p2,fr6,p5,&fr7,&fr8,&fr9)==-1)
					{
						ret=-1;goto exit;
					}
					if (cmd_ang_betw(sds_angle(p5,ptsel2),fr8,fr9))
						fi2=cmd_fillet_arc2arc(p0,fr1,ptsel1,p5,fr7,ptsel2,p6,p7,p8,&fi1);
					else
						fi2=cmd_fillet_arc2arc(p0,fr1,ptsel1,p5,fr7,p2,p6,p7,p8,&fi1);
					if (fi2<0)
					{
						ret=-1;
						goto exit;
					}
					//p6 is on arc, p7 on pline
					if (!cmd_ang_betw(sds_angle(p5,p7),fr8,fr9))
					{
						//they may have picked way off the intersection & arc2arc is
						//picking a different intersection.  Calc new pt to try with....
						if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
						{
							ret=-1;goto exit;
						}
						sds_polar(p3,sds_angle(p3,ptsel1),rb.resval.rreal,p8);
						sds_polar(p0,sds_angle(p0,p8),fr1,ptsel1);
						if (cmd_fillet_arc2arc(p0,fr1,ptsel1,p5,fr7,p2,p6,p7,p8,&fi1)<0)
						{
							ret=-1;
							goto exit;
						}
						if (!cmd_ang_betw(sds_angle(p5,p7),fr8,fr9))
						{
							cmd_ErrorPrompt(CMD_ERR_FILCLOSEFAR,0);
							goto exit;
						}
					}
					fr4=sds_angle(p5,p7);//new ending angle
					if (fr6>0.0) //ccw bulge
					{
						ic_normang(&fr4,&fr9);
						fr11=fr9-fr4;
						fr10=tan(fr11/4.0);
						ic_normang(&fr8,&fr9);
						fr11=1.0-(fr11/(fr9-fr8));	//new starting width factor
					}
					else		 //clockwise
					{
						ic_normang(&fr8,&fr4);
						fr11=fr4-fr8;
						fr10=-tan(fr11/4.0);
						ic_normang(&fr8,&fr9);
						fr11=1.0-(fr11/(fr9-fr8));	 //new ending width factor
					}
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr10;
				}
				else
				{
					if (ic_ptlndist(ptsel2,p1,p2,&fr7,p8)==1)
						fi2=cmd_fillet_arc2line(p1,p2,ptsel2,p0,fr1,ptsel1,p7,p6,p8,&fi1);
					else
						fi2=cmd_fillet_arc2line(p1,p2,p2,p0,fr1,ptsel1,p7,p6,p8,&fi1);
					fi1=(fi1 ^ 1);
					//p7 is on pline, p6 on arc
					if (fi2==-1)
					{
						ret=-1;
						goto exit;
					}
					if (fi2==1)
					{
						cmd_ErrorPrompt(CMD_ERR_FILTOOCLOSE,0);
						goto exit;
					}
					fr11=(sds_distance(p6,p1)/sds_distance(p1,p2));
				}
				ic_assoc(rbp1,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,p7);
				if (!ic_assoc(rbp1,41))
					fr5=ic_rbassoc->resval.rreal;
				else
					fr5=0.0;
				if (!ic_assoc(rbp1,40))
					fr4=ic_rbassoc->resval.rreal;
				else
					fr4=0.0;
				fr9=ic_rbassoc->resval.rreal=fr4+((fr5-fr4)*fr11);
				//don't mess w/rbp1 - we need to make vertex at appropriate place
				if (trimmode) //make new break-off vertex & fillet vertex
				{
					fr11=sds_angle(p0,p6);
					if (strsame(etype1,"ARC"/*DNT*/ ))
					{
						cmd_arcang4fillet(fr11,sds_angle(p0,p3),fr2,fr3,sds_angle(p0,ptsel1),&fi2);
						if (fi2==50)	//if we're adjusting arc's start angle....
						{
							ic_normang(&fr11,&fr3);
							fr10=-tan((fr3-fr11)/4.0);
							sds_polar(p0,fr3,fr1,p2);
						}
						else
						{
							ic_normang(&fr2,&fr11);
							fr10=tan((fr11-fr2)/4.0);
							sds_polar(p0,fr2,fr1,p2);
						}
						FreeResbufIfNotNull(&rbp2);
						if (((rbp2=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,40,0.0,41,0.0,42,fr10,0))==NULL) ||
							(sds_entmake(rbp2)!=RTNORM))
						{
							ret=-1;
							goto exit;
						}
					}
					fr4=sds_angle(p8,p6);
					fr5=sds_angle(p8,p7);
					if (fi1)
					{
						ic_normang(&fr5,&fr4);	//going from pline to arc
						fr11=-tan((fr4-fr5)/4.0);
						//if (fr6>0.0) fr11=-fr11;
					}
					else
					{
						ic_normang(&fr4,&fr5);
						fr11=tan((fr5-fr4)/4.0);
						//if (fr2<=0.0)fr11=-fr11;
					}
					FreeResbufIfNotNull(&rbp2);
					if ((rbp2=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p6,40,0.0,41,fr9,42,fr11,0))==NULL)
					{
						ret=-1;goto exit;
					}
					if (sds_entmake(rbp2)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					if (sds_entmake(rbp1)!=RTNORM)
					{
						ret=-1;goto exit;
					}
					if (cmd_make_pline_end(etemp,closure&1,closept)!=0)
					{
						ret=-1;goto exit;
					}
					if (!strsame(etype1,"CIRCLE"/*DNT*/ ))
						sds_entdel(ent1);
					sds_entdel(ent2);
					sds_entlast(ent1);
					ic_encpy(ent2,ent1);
					sds_redraw(ent1,IC_REDRAW_DRAW);
				}
				else
				{
					if (!icadPointEqual(p7,p6))
					{
						//just make an arc..
						FreeResbufIfNotNull(&rbp1);
						if (fi1)	//clockwise....
						{
							rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
											   10,p8,40,sds_distance(p8,p6),50,sds_angle(p8,p7),51,sds_angle(p8,p6),210,p210,0);
							if (rbp1==NULL)
							{
								ret=-1;goto exit;
							}
							if (sds_entmake(rbp1)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
						else
						{
							rbp1=sds_buildlist(RTDXF0,"ARC"/*DNT*/,6,filletlt,8,filletlay,62,ltcolor,
											   10,p8,40,sds_distance(p8,p6),50,sds_angle(p8,p6),51,sds_angle(p8,p7),210,p210,0);
							if (rbp1==NULL)
							{
								ret=-1;goto exit;
							}
							if (sds_entmake(rbp1)!=RTNORM)
							{
								ret=-1;goto exit;
							}
						}
					}
				}
			}
		}
	}
	exit:
	if (elist1!=NULL)
	{
		sds_relrb(elist1); elist1=NULL;
	}
	if (elist2!=NULL)
	{
		sds_relrb(elist2); elist2=NULL;
	}
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	sds_entmake(NULL);
	return(ret);
}

int cmd_segxpoly(sds_name polyent,sds_point epoint, sds_point p0,sds_point p1,
				 sds_real bulge, sds_point intpt, sds_real *polydist, sds_name vtxent)
{
	//Returns 0-no crossing.  No successful measurement made
	//		 -1-error
	//		  1 successful measurement made
	//finds point nearest epoint on polyent where segment p0-p1 crosses pline.
	//*polydist will be set to distance along pline from intpt to epoint (>0 indicates
	//that intpt is before epoint on pline.
	//NOTE:circle can be tested by setting both p0 and p1 to same pt - circle ctr.
	// bulge then contains radius of circle

	sds_name etemp1,etemp2;
	sds_real fr1,fr2,fr3,fr4,fr5,rr,sa,ea;
	sds_point cc,p3,p4,p5,p6,p7;
	struct resbuf *rbp1,*rbp2;
	int closure=0,fi1,circle=0;
	int ret=0;

	rbp1=rbp2=NULL;
	ic_encpy(etemp1,polyent);
	if ((rbp1=sds_entget(etemp1))==NULL)
	{
		ret=-1; goto exit;
	}
	ic_assoc(rbp1,70);
	if (ic_rbassoc->resval.rint & 1)
		closure=1;
	else
		closure=0;

	if (icadPointEqual(p0,p1))	//circle
	{
		circle=1;
		if (icadRealEqual(bulge,0.0))
		{
			ret=-1; goto exit;
		}
		ic_ptcpy(cc,p0);
		rr=bulge;
		sa=0.0;
		ea=IC_PI;
		sds_polar(p0,0.0,rr,p1);
		sds_polar(p0,IC_PI,rr,p0);
	}
	else if (!icadRealEqual(bulge,0.0))	//arc
	{
		if (ic_bulge2arc(p0,p1,bulge,cc,&rr,&sa,&ea)==-1)
		{
			ret=-1; goto exit;
		}
	}
	for (;;)
	{
		if (sds_entnext_noxref(etemp1,etemp1)!=RTNORM)
		{
			ret=-1; goto exit;
		}
		FreeResbufIfNotNull(&rbp1);
		if ((rbp1=sds_entget(etemp1))==NULL)
		{
			ret=-1; goto exit;
		}
		ic_assoc(rbp1,0);
		if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ))
			break;
		if (sds_entnext_noxref(etemp1,etemp2)!=RTNORM)
		{
			ret=-1; goto exit;
		}
		FreeResbufIfNotNull(&rbp2);
		if ((rbp2=sds_entget(etemp2))==NULL)
		{
			ret=-1; goto exit;
		}
		ic_assoc(rbp2,0);
		if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ))
		{
			if (!closure)
				break;
			else
			{
				sds_relrb(rbp2);
				rbp2=NULL;
				cmd_pline_entnext(etemp1,etemp2,closure);
				if ((rbp2=sds_entget(etemp2))==NULL)
				{
					ret=-1; goto exit;
				}
			}
		}
		ic_assoc(rbp1,10);
		ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
		ic_assoc(rbp2,10);
		ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
		ic_assoc(rbp1,42);
		fr1=ic_rbassoc->resval.rreal;
		if (icadRealEqual(fr1,0.0))
		{
			//a straight segment on pline
			if (icadRealEqual(bulge,0.0))
			{
				fi1=ic_linexline(p0,p1,p3,p4,p5);
				if (fi1>=2)
				{
					cmd_pline_dist(polyent,p5,epoint,&fr2);
					if (!ret || (fabs(fr2)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p5);
						(*polydist)=fr2;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
			}
			else	//arc or circle crossing straight segment
			{
				fi1=ic_linexarc(p3,p4,cc,rr,sa,ea,p5,p6);
				if (fi1==-1)
					continue;
				if ((fi1 & 1)==1)
				{
					cmd_pline_dist(polyent,p5,epoint,&fr2);
					if (!ret || (fabs(fr2)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p5);
						(*polydist)=fr2;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
				if ((fi1 & 4)==4)
				{
					cmd_pline_dist(polyent,p6,epoint,&fr2);
					if (!ret ||(fabs(fr2)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p6);
						(*polydist)=fr2;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
			}
		}
		else
		{
			//curved section of pline
			ic_bulge2arc(p3,p4,fr1,p5,&fr2,&fr3,&fr4);
			if (icadRealEqual(bulge,0.0))
			{
				fi1=ic_linexarc(p0,p1,p5,fr2,fr3,fr4,p7,p6);
				if (fi1==-1)
					continue;
				if ((fi1 & 2)==2)
				{
					cmd_pline_dist(polyent,p7,epoint,&fr5);
					if (!ret|| (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p7);
						(*polydist)=fr5;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
				if ((fi1 & 8)==8)
				{
					cmd_pline_dist(polyent,p6,epoint,&fr5);
					if (!ret || (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p6);
						(*polydist)=fr5;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
			}
			else	//arc or circle crossing curved segment
			{
				fi1=ic_arcxarc(cc,rr,sa,ea,p5,fr2,fr3,fr4,p6,p7);
				if (fi1==-1)
					continue;
				if ((fi1 & 2)==2)
				{
					cmd_pline_dist(polyent,p6,epoint,&fr5);
					if (!ret || (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p6);
						(*polydist)=fr5;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
				if ((fi1 & 8)==8)
				{
					cmd_pline_dist(polyent,p7,epoint,&fr5);
					if (!ret || (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p7);
						(*polydist)=fr5;
						ic_encpy(vtxent,etemp1);
						ret=1;
					}
				}
			}
		}
	}

	exit:

	IC_RELRB(rbp1);
	IC_RELRB(rbp2);
	return(ret);
}

int cmd_segxlwpoly(sds_name lwplename, sds_point epoint, sds_point p0, sds_point p1,
				   sds_real bulge, sds_point intpt, sds_real *polydist, int *vertno)
{
	//Returns 0-no crossing.  No successful measurement made
	//		 -1-error
	//		  1 successful measurement made
	//finds point nearest epoint on polyent where segment p0-p1 crosses pline.
	//*polydist will be set to distance along pline from intpt to epoint (>0 indicates
	//that intpt is before epoint on pline.
	//NOTE:circle can be tested by setting both p0 and p1 to same pt - circle ctr.
	// bulge then contains radius of circle

	int etemp1,etemp2;
	sds_real fr1,fr2,fr3,fr4,fr5,rr,sa,ea;
	sds_point cc,p3,p4,p5,p6,p7;
	int closure=0,fi1,circle=0;
	int ret=0;
	db_lwpolyline *lwpl=(db_lwpolyline *)lwplename[0];

	if (lwpl->ret_70() & 1)
		closure=1;
	else
		closure=0;

	if (icadPointEqual(p0,p1))	//circle
	{
		circle=1;
		if (icadRealEqual(bulge,0.0))
		{
			ret=-1; goto exit;
		}
		ic_ptcpy(cc,p0);
		rr=bulge;
		sa=0.0;
		ea=IC_PI;
		sds_polar(p0,0.0,rr,p1);
		sds_polar(p0,IC_PI,rr,p0);
	}
	else if (!icadRealEqual(bulge,0.0))	//arc
	{
		if (ic_bulge2arc(p0,p1,bulge,cc,&rr,&sa,&ea)==-1)
		{
			ret=-1; goto exit;
		}
	}
	for (etemp1=0; etemp1<lwpl->ret_90()-1; etemp1++)
	{
		etemp2=cmd_lwpline_entnext(etemp1,lwpl);
		lwpl->get_10(p3,etemp1);
		lwpl->get_10(p4,etemp2);
		lwpl->get_42(&fr1,etemp1);
		if (icadRealEqual(fr1,0.0))
		{
			//a straight segment on pline
			if (icadRealEqual(bulge,0.0))
			{
				fi1=ic_linexline(p0,p1,p3,p4,p5);
				if (fi1>=2)
				{
					cmd_lwpline_dist(lwplename,p5,epoint,&fr2);
					if (!ret || (fabs(fr2)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p5);
						(*polydist)=fr2;
						*vertno=etemp1;
						ret=1;
					}
				}
			}
			else	//arc or circle crossing straight segment
			{
				fi1=ic_linexarc(p3,p4,cc,rr,sa,ea,p5,p6);
				if (fi1==-1)
					continue;
				if ((fi1 & 1)==1)
				{
					cmd_lwpline_dist(lwplename,p5,epoint,&fr2);
					if (!ret || (fabs(fr2)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p5);
						(*polydist)=fr2;
						*vertno=etemp1;
						ret=1;
					}
				}
				if ((fi1 & 4)==4)
				{
					cmd_lwpline_dist(lwplename,p6,epoint,&fr2);
					if (!ret ||(fabs(fr2)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p6);
						(*polydist)=fr2;
						*vertno=etemp1;
						ret=1;
					}
				}
			}
		}
		else
		{
			//curved section of pline
			ic_bulge2arc(p3,p4,fr1,p5,&fr2,&fr3,&fr4);
			if (icadRealEqual(bulge,0.0))
			{
				fi1=ic_linexarc(p0,p1,p5,fr2,fr3,fr4,p7,p6);
				if (fi1==-1)
					continue;
				if ((fi1 & 2)==2)
				{
					cmd_lwpline_dist(lwplename,p7,epoint,&fr5);
					if (!ret|| (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p7);
						(*polydist)=fr5;
						*vertno=etemp1;
						ret=1;
					}
				}
				if ((fi1 & 8)==8)
				{
					cmd_lwpline_dist(lwplename,p6,epoint,&fr5);
					if (!ret || (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p6);
						(*polydist)=fr5;
						*vertno=etemp1;
						ret=1;
					}
				}
			}
			else	//arc or circle crossing curved segment
			{
				fi1=ic_arcxarc(cc,rr,sa,ea,p5,fr2,fr3,fr4,p6,p7);
				if (fi1==-1)
					continue;
				if ((fi1 & 2)==2)
				{
					cmd_lwpline_dist(lwplename,p6,epoint,&fr5);
					if (!ret || (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p6);
						(*polydist)=fr5;
						*vertno=etemp1;
						ret=1;
					}
				}
				if ((fi1 & 8)==8)
				{
					cmd_lwpline_dist(lwplename,p7,epoint,&fr5);
					if (!ret || (fabs(fr5)<fabs(*polydist)))
					{
						ic_ptcpy(intpt,p7);
						(*polydist)=fr5;
						*vertno=etemp1;
						ret=1;
					}
				}
			}
		}
	}

	exit:

	return(ret);
}

int cmd_fillet_arc2arc(sds_point cc1,sds_real rr1,sds_point ptsel1,sds_point cc2,sds_real rr2,
					   sds_point ptsel2,sds_point fp1,sds_point fp2,sds_point cc3,int *cw)
{
	//calculates center and fillet points for intersection of two arcs/circles
	sds_real filletrad,fr1,fr2;
	sds_point p1,p2;
	int ret=0;
	struct resbuf rb;
	char fs1[IC_ACADBUF];

	if (icadRealEqual(rr1,0.0)||icadRealEqual(rr2,0.0))
	{
		cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
//		sds_printf("\nCannot fillet circle of zero radius.");
		return(-1);
	}

	if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(-1);
	filletrad=rb.resval.rreal;

	if (sds_distance(cc1,ptsel2)<rr1)
		fr1=rr1-filletrad;
	else
		fr1=rr1+filletrad;
	if (sds_distance(cc2,ptsel1)<rr2)
		fr2=rr2-filletrad;
	else
		fr2=rr2+filletrad;
	if (ic_arcxarc(cc1,fr1,0.0,0.0,cc2,fr2,0.0,0.0,p1,p2)==-1)
	{
		sds_rtos(filletrad,-1,-1,fs1);
		sds_printf(ResourceString(IDC_CMDS4__NUNABLE_TO_FILL_111, "\nUnable to fillet using radius %s." ),fs1);
		return(-1);
	}
	//point on circle/arc selected FIRST takes precedence
	if (icadRealEqual(sds_distance(ptsel1,p2),sds_distance(ptsel1,p1)))
	{
		if (sds_distance(ptsel2,p2)<sds_distance(ptsel2,p1))
			ic_ptcpy(cc3,p2);
		else
			ic_ptcpy(cc3,p1);
	}
	else if (sds_distance(ptsel1,p1)<sds_distance(ptsel1,p2))
		ic_ptcpy(cc3,p1);
	else
		ic_ptcpy(cc3,p2);
	if (fr1<rr1)
		fr1=sds_angle(cc1,cc3);
	else
		fr1=sds_angle(cc3,cc1);
	if (fr2<rr2)
		fr2=sds_angle(cc2,cc3);
	else
		fr2=sds_angle(cc3,cc2);
	sds_polar(cc3,fr1,filletrad,fp1);
	sds_polar(cc3,fr2,filletrad,fp2);
	ic_normang(&fr1,&fr2);
	if ((fr2-fr1)>IC_PI)
		(*cw)=1;
	else if ((fr2-fr1)<IC_PI)
		(*cw)=0;
	else
	{				//180 degree fillet
		fr2=sds_angle(cc1,ptsel1);
		ic_normang(&fr1,&fr2);
		if ((fr2-fr1)<IC_PI)
			(*cw=1);
		else
			(*cw=0);
	}

	return(0);
}


void cmd_arcang4fillet(sds_real filletang,sds_real intang,sds_real sa,
					   sds_real ea, sds_real pickang,int *dxf)
{
	//f'n figures out if we want to modify start or end angle on arc for fillet
	//dxf will be set to either 50 or 51
	sds_real fr1;

	if (!icadAngleEqual(filletang,intang))
	{
		fr1=filletang;
		ic_normang(&intang,&fr1);
		if ((fr1-intang)<IC_PI)
		{
			(*dxf)=50;
		}
		else if ((fr1-intang)>IC_PI)
		{
			(*dxf)=51;
		}
		else
		{  //fillet of 180 degrees....
			fr1=filletang;
			ic_normang(&pickang,&fr1);
			if ((fr1-pickang)<IC_PI)
				(*dxf)=51;
			else
				(*dxf)=50;
		}
	}
	else
	{
		if (cmd_ang_betw(intang,sa,ea))
		{
			if (cmd_ang_betw(pickang,sa,intang))
				(*dxf)=51;
			else
				(*dxf)=50;
		}
		else
		{
			ic_normang(&sa,&pickang);	// determine whether the pick angle is closer
			ic_normang(&sa,&ea);		// to the end or start, and use the right one
			if (fabs(sa-pickang)<fabs(ea-pickang))
				(*dxf)=50;
			else
				(*dxf)=51;
		}
	}
}

int cmd_fillet_arc2line(sds_point p1,sds_point p2,sds_point linpt,sds_point cc,sds_real rr,
						sds_point arcpt,sds_point p3,sds_point p4,sds_point cc2,int *cw)
{

	//given line p1-p2 picked at linpt and arc/circle at cc w/rad rr and start/end <'s sa & ea,
	//calculates fillet points p3 & p4 and fillet ctr cc2.  cw tells calling function if we're
	//filleting clockwise or ccw.  p3 is on line,p4 on arc.
	//fillets betw line & arc are either "inner" or "outer" - are you filleting
	//inside or outside the arc.  if user picked line pt inside circle/arc, then
	//we'll do an inner fillet, otherwise we'll do an outer fillet.

	char fs1[IC_ACADBUF];
	sds_point p5,p6,p7,p8;
	sds_real filletrad,fr1,fr2,fr3;
	struct resbuf rb;
	int fi1;

	if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(-1);
	filletrad=rb.resval.rreal;


	if (sds_distance(linpt,cc)<rr)
	{ //an inner fillet
		fr1=rr-filletrad;
	}
	else
	{
		fr1=rr+filletrad;
	}
	if (fr1<=0.0)
	{
		cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
		return(-1);
	}
	//offset line toward selected portion of arc
	if (ic_ptlndist(arcpt,p1,p2,&fr2,p5)==-1)
	{ //p5 is arc pick pt projected onto line
		cmd_ErrorPrompt(CMD_ERR_FILNOLENGTH,0);
//		sds_printf("\nLine has coincident entpoints.");
		return(-1);
	}
	fr2=sds_angle(p5,arcpt);	//Perpendicular to line toward arc
	sds_polar(p1,fr2,filletrad,p6);
	sds_polar(p2,fr2,filletrad,p7);
	if ((fi1=ic_linexarc(p6,p7,cc,fr1,0.0,0.0,p5,p8))==-1)
	{
		sds_rtos(filletrad,-1,-1,fs1);
		sds_printf(ResourceString(IDC_CMDS4__NNO_VALID_FILLE_112, "\nNo valid fillet for radius %s." ),fs1);
		return(-1);
	}
	//line selection pt takes precedence over arc selection pt....
	if (icadRealEqual(sds_distance(linpt,p5),sds_distance(linpt,p8)))
	{
		if (sds_distance(arcpt,p8)<sds_distance(arcpt,p5))
			ic_ptcpy(cc2,p8);
		else
			ic_ptcpy(cc2,p5);
	}
	else if (sds_distance(linpt,p8)<sds_distance(linpt,p5))
		ic_ptcpy(cc2,p8);
	else
		ic_ptcpy(cc2,p5);

	if (fr1<rr)		//on an inner fillet,we walk AWAY from ctr of old circle
		fr1=sds_angle(cc,cc2);
	else				//on outer fillet, walk TOWARD ctr of old circle
		fr1=sds_angle(cc2,cc);

	fr2=fr2+IC_PI;
	sds_polar(cc2,fr1,filletrad,p4);
	sds_polar(cc2,fr2,filletrad,p3);
	fr3=fr1;
	ic_normang(&fr2,&fr3);
	if ((fr3-fr2)<IC_PI)
		(*cw)=0;
	else
		(*cw)=1;
	return(0);
}


int cmd_linept4fillet(sds_point p1, sds_point p2,sds_point p7, sds_point ptsel1,sds_point p5, int *moveflag)
{
	//p1=line start,p2=line end,p7=inters,ptsel1=user-picked fillet pt,p5=point at which fillet to be made
	//modifies line pts for fillet.  moveflag coded 10=move start to filletpt, 11=move end
	sds_point ptemp;
	if (icadPointEqual(p7,p5))
		ic_ptcpy(ptemp,ptsel1);
	else
		ic_ptcpy(ptemp,p5);
	if (icadDirectionEqual(p7,p1,p7,p2) && icadDirectionEqual(p7,p1,p7,ptemp))
	{

		if (sds_distance(p7,p5)>max(sds_distance(p7,p1),sds_distance(p7,p2)))
		{
			cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
			return(-1);
		}
		if (sds_distance(p7,p1)<sds_distance(p7,p2))
			(*moveflag)=10;
		else
			(*moveflag)=11;
	}
	else if (icadDirectionEqual(p7,p1,p7,ptemp))
	{

		if (sds_distance(p7,p1)<sds_distance(p7,p5))
		{
			cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
//			sds_printf("\nRadius too large for selected side of entity.");
			return(-1);
		}
		(*moveflag)=11;
	}
	else
	{  // if (fr2==fr3)
		if (sds_distance(p7,p2)<sds_distance(p7,p5))
		{
			cmd_ErrorPrompt(CMD_ERR_FILRADIUSLG,0);
//			sds_printf("\nRadius too large for selected side of entity.");
			return(-1);
		}
		(*moveflag)=10;
	}
	return(0);
}

int cmd_fillet_line2line(sds_point p1,sds_point p2,sds_point line1pt,int *l1type,sds_point p3,sds_point p4,
						 sds_point line2pt,int *l2type,sds_point fp1,sds_point fp2,int *pts2move,
						 sds_point cc, sds_real *sa, sds_real *ea, sds_point extru)
{
	//given lines p1-p2 and p3-p4 calculate fillet pts fp1 & fp2 and center of fillet arc
	//l1type & l2type are as follows: 0=line, 1=ray, 2=xline;
	//if ray needs to turn into a line, l1type or l2type |=4	(entmake bit flag)
	//if xline needs to turn into a ray, l1type or l2type |=4 (entmake flag)
	//RETURNS:  0 if successful and fillet pts are on lines
	//			1 if successful and fillet pts not on lines
	//			-1 if error
	//			-2 if error & lines parallel
	//			-3 if no 3d intersection found
	//NOTE: pts line1pt & line2pt are points picked by user on the lines in case we need to know which
	//			direction to fillet along.  They need to be ON THE LINE SEGMENTS.
	//	p1 & p3 are 10pts, p2 & p4 are 11pts
	//  if (pts2move&1)==0, move the 10pt of 1st line.  if (pts2move&1)==1 move 11pt
	//  if (pts2move&2)==0, move the 10pt of 2nd line.  if (pts2move&2)==2 move 11pt
	//	if (pts2move&4)!=0, DON'T move anything on line1 AND caller must calc radius
	//			of arc (parallel line fillet)
	//  Extru is such that arc always goes from line2pt to line1pt ccw.
	//***NOTE: if point initially passed in extru is not {0,0,0}, then
	//	all points are in the ecs defined by this vector, not the wcs

	sds_point inters;
	sds_real ang1,fr1,fr2,fuzz;
	int ret=0,dim1,dim2,direction1,direction2,l1typ,l2typ;
	struct resbuf rb,rb2;
	sds_point p5,p6,p7;

	inters[0]=inters[1]=inters[2]=0.0;
	if (icadPointEqual(inters,extru))
	{
		rb2.restype=RTSHORT;
		rb2.resval.rint=0;
	}
	else
	{
		rb2.restype=RT3DPOINT;
		ic_ptcpy(rb2.resval.rpoint,extru);
	}
	*pts2move=0;
	if (l1type!=NULL)
		l1typ=*l1type;
	else
		l1typ=0;
	if (l2type!=NULL)
		l2typ=*l2type;
	else
		l2typ=0;

	//set bits for which direction of line has greatest magnitude
	if (fabs(p2[0]-p1[0])>fabs(p2[1]-p1[1]))
	{
		if (fabs(p2[0]-p1[0])>fabs(p2[2]-p1[2]))dim1=0;
		else dim1=2;
	}
	else
	{
		if (fabs(p2[1]-p1[1])>fabs(p2[2]-p1[2]))dim1=1;
		else dim1=2;
	}
	if (fabs(p4[0]-p3[0])>fabs(p4[1]-p3[1]))
	{
		if (fabs(p4[0]-p3[0])>fabs(p4[2]-p3[2]))dim2=0;
		else dim2=2;
	}
	else
	{
		if (fabs(p4[1]-p3[1])>fabs(p4[2]-p3[2]))dim2=1;
		else dim2=2;
	}
	fuzz=fabs(CMD_FUZZ*(p2[dim1]-p1[dim1]));
	if (RTNORM!=sds_inters(p1,p2,p3,p4,0,inters))
	{
		//no 3d intersection.  First, take unit vector dot product &
		//	see if they're parallel
		if (0==l1typ && ((fr1=sds_distance(p2,line1pt)) < (fr2=sds_distance(p1,line1pt))))
		{
			p5[0]=p1[0]-p2[0];
			p5[1]=p1[1]-p2[1];
			p5[2]=p1[2]-p2[2];
			ic_ptcpy(fp1,p2);
			*pts2move|=1;	//p2 will be filleted end
		}
		else
		{
			p5[0]=p2[0]-p1[0];
			p5[1]=p2[1]-p1[1];
			p5[2]=p2[2]-p1[2];
			ic_ptcpy(fp1,p1);
		}
		*pts2move|=4;
		p6[0]=p4[0]-p3[0];
		p6[1]=p4[1]-p3[1];
		p6[2]=p4[2]-p3[2];
		if (l1typ==0)
		{
			fr1=sqrt(p5[0]*p5[0]+p5[1]*p5[1]+p5[2]*p5[2]);
			p5[0]/=fr1;
			p5[1]/=fr1;
			p5[2]/=fr1;
		}
		if (0==l2typ)
		{
			fr2=sqrt(p6[0]*p6[0]+p6[1]*p6[1]+p6[2]*p6[2]);
			p6[0]/=fr2;
			p6[1]/=fr2;
			p6[2]/=fr2;
		}
		if (!icadRealEqual(1.0,fabs(p5[0]*p6[0]+p5[1]*p6[1]+p5[2]*p6[2])))return(-3);
		//the lines are parallel, so let's fillet them together at the first endpt selected
		if (l1typ==2)
		{
			return(-2);
		}
		if ((*pts2move)&1)
		{ //11pt is fillet end (LINE ent only)
			p6[0]=p3[0]-p2[0];
			p6[1]=p3[1]-p2[1];
			p6[2]=p3[2]-p2[2];
		}
		else
		{
			p6[0]=p3[0]-p1[0];
			p6[1]=p3[1]-p1[1];
			p6[2]=p3[2]-p1[2];
		}
		fr2=sqrt(p6[0]*p6[0]+p6[1]*p6[1]+p6[2]*p6[2]);
		p6[0]/=fr2;
		p6[1]/=fr2;
		p6[2]/=fr2;
		ic_crossproduct(p6,p5,extru);
		ang1=sqrt(extru[0]*extru[0]+extru[1]*extru[1]+extru[2]*extru[2]);
		//actually, ang1 is the sine of the angle between the vectors,
		//	which is what we'll need later
		if (icadAngleEqual(ang1,0.0))return(-1);  //lines are collinear
		extru[0]/=ang1;
		extru[1]/=ang1;
		extru[2]/=ang1;
		//now cross extru w/unit vector for line 1 (p5) to get unit vector normal toward p3-p4
		ic_crossproduct(p5,extru,p7);
		//go from fp1 to fp2
		fp2[0]=fp1[0]+p7[0]*fr2*ang1;
		fp2[1]=fp1[1]+p7[1]*fr2*ang1;
		fp2[2]=fp1[2]+p7[2]*fr2*ang1;
		//average the 2 points to get the center of the arc
		p7[0]=(fp1[0]+fp2[0])*0.5;
		p7[1]=(fp1[1]+fp2[1])*0.5;
		p7[2]=(fp1[2]+fp2[2])*0.5;
		//now, all we really need to know is which pt will move to fp2.
		if (((0==((*pts2move)&1)) && p2[dim1]>p1[dim1]) ||
			((1==((*pts2move)&1)) && p2[dim1]<p1[dim1]))
		{
			//if we're filleting at the end w/the smaller coordinate
			if (p3[dim1]>=fp2[dim1]-fuzz && p4[dim1]>=fp2[dim1]-fuzz)
			{
				if (p3[dim1]>p4[dim1])
				{
					*pts2move|=2;
					if (0!=l2typ)l2typ|=4;
				}
				else
				{
					if (2==l2typ)l2typ|=4;
				}
			}
			else if (p3[dim1]>fp2[dim1]-fuzz)
			{
				*pts2move|=2;
				if (0!=l2typ)l2typ|=4;
			}
			else if (p4[dim1]>fp2[dim1]-fuzz)
			{
				if (2==l2typ)l2typ|=4;
			}
			else
			{
				if (0==l2typ)return(-1);
				if (1==l2typ && p4[dim1]<p3[dim1])return(-1);
				if (p4[dim1]<p3[dim1])*pts2move|=2;
			}
		}
		else
		{
			//filleting at end w/larger coordinate
			if (p3[dim1]<=fp2[dim1]+fuzz && p4[dim1]<=fp2[dim1]+fuzz)
			{
				if (p3[dim1]<p4[dim1])
				{
					(*pts2move)|=2;
					if (0!=l2typ)l2typ|=4;
				}
				else
				{
					if (2==l2typ)l2typ|=4;
				}
			}
			else if (p3[dim1]<fp2[dim1]+fuzz)
			{
				*pts2move|=2;
				if (0!=l2typ)l2typ|=4;
			}
			else if (p4[dim1]<fp2[dim1]+fuzz)
			{
				if (2==l2typ)l2typ|=4;
			}
			else
			{
				if (0==l2typ)return(-1);
				if (1==l1typ && p4[dim1]>p3[dim1])return(-1);
				if (p4[dim1]>p3[dim1])*pts2move|=2;
			}
		}
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,extru);
		if (rb2.restype==RTSHORT)
		{
			//if points passed were wcs - convert to new ecs
			sds_trans(fp1,&rb2,&rb,0,p5);
			sds_trans(fp2,&rb2,&rb,0,p6);
			sds_trans(p7,&rb2,&rb,0,cc);
		}
		else
		{
			return(-1);
		}


		//because of the way we defined the ecs w/crossing the vectors, the arc will
		//	ALWAYS run clockwise from p6 to p5, so reverse sa&ea
		*ea=sds_angle(cc,p6);
		*sa=sds_angle(cc,p5);
		if (l1type!=NULL)*l1type=l1typ;
		if (l2type!=NULL)*l2type=l2typ;
		return(ret);
	}
	//***Begin work on non-parallel fillet***
	//Now find out where the inters is relative to pts on line segment 1
	if (0==l1typ)
	{
		if (inters[dim1]<=(max(p1[dim1],p2[dim1])+fuzz) && inters[dim1]>=(min(p1[dim1],p2[dim1])-fuzz))
		{
			//inters is on p1-p2
			if (p2[dim1]>p1[dim1])
			{
				if (inters[dim1]<line1pt[dim1] || icadRealEqual(p1[dim1],inters[dim1]))direction1=2;//go from pt2 toward inters
				else direction1=1;
			}
			else
			{
				if (inters[dim1]<line1pt[dim1] || icadRealEqual(p2[dim1],inters[dim1]))direction1=1;
				else direction1=2;
			}
		}
		else
		{
			if (fabs(inters[dim1]-p2[dim1])>fabs(inters[dim1]-p1[dim1])) direction1=2;
			else direction1=1;
		}
	}
	else
	{  //rays & xlines
		if (p2[dim1]>p1[dim1])
		{
			if ((inters[dim1]>(p1[dim1]+fuzz) && line1pt[dim1]<(inters[dim1]-fuzz))||
				(2==l1typ && line1pt[dim1]<(inters[dim1]-fuzz)))
			{
				direction1=2;
				l1typ|=4;
			}
			else
			{
				direction1=1;
				if (2==l1typ)l1typ|=4;
			}
		}
		else
		{
			if ((inters[dim1]<(p1[dim1]-fuzz) && line1pt[dim1]>(inters[dim1]+fuzz)) ||
				(2==l1typ && line1pt[dim1]>(inters[dim1]+fuzz)))
			{
				direction1=2;
				l1typ|=4;
			}
			else
			{
				direction1=1;
				if (2==l1typ)l1typ|=4;
			}
		}
	}

	//Now find out where the inters is relative to pts on line segment 2
	if (0==l2typ)
	{
		if (inters[dim2]<=(fuzz+max(p3[dim2],p4[dim2])) && inters[dim2]>=(min(p3[dim2],p4[dim2])-fuzz))
		{
			//inters is on p3-p4
			if (p4[dim2]>p3[dim2])
			{
				if (inters[dim2]<line2pt[dim2]+fuzz || icadRealEqual(p3[dim2],inters[dim2]))direction2=4;
				else direction2=3;
			}
			else
			{
				if (inters[dim2]<line2pt[dim2]+fuzz || icadRealEqual(p4[dim2],inters[dim2]))direction2=3;
				else direction2=4;
			}
		}
		else
		{
			if (fabs(inters[dim2]-p4[dim2])>fabs(inters[dim2]-p3[dim2])) direction2=4;
			else direction2=3;
		}
	}
	else
	{
		if (p4[dim2]>p3[dim2])
		{
			if ((inters[dim2]>(p3[dim2]+fuzz) && line2pt[dim2]<(inters[dim2]+fuzz))||
				(2==l2typ && line2pt[dim2]<(inters[dim2]-fuzz)))
			{
				direction2=4;
				l2typ|=4;
			}
			else
			{
				direction2=3;
				if (2==l2typ)l2typ|=4;
			}
		}
		else
		{
			if ((inters[dim2]<(p3[dim2]-fuzz) && line2pt[dim2]>(inters[dim2]-fuzz))||
				(2==l2typ && line2pt[dim2]>(inters[dim2]+fuzz)))
			{
				direction2=4;
				l2typ|=4;
			}
			else
			{
				direction2=3;
				if (2==l2typ)l2typ|=4;
			}
		}
	}
	//get cross product of two vectors...
	if (1==direction1)
	{
		p5[0]=p1[0]-p2[0];
		p5[1]=p1[1]-p2[1];
		p5[2]=p1[2]-p2[2];
	}
	else
	{
		p5[0]=p2[0]-p1[0];
		p5[1]=p2[1]-p1[1];
		p5[2]=p2[2]-p1[2];
	}
	if (3==direction2)
	{
		p6[0]=p3[0]-p4[0];
		p6[1]=p3[1]-p4[1];
		p6[2]=p3[2]-p4[2];
	}
	else
	{
		p6[0]=p4[0]-p3[0];
		p6[1]=p4[1]-p3[1];
		p6[2]=p4[2]-p3[2];
	}
	if (l1typ!=0)
	{
		p5[0]=-p5[0];
		p5[1]=-p5[1];
		p5[2]=-p5[2];
	}
	if (l2typ!=0)
	{
		p6[0]=-p6[0];
		p6[1]=-p6[1];
		p6[2]=-p6[2];
	}

	fr1=sqrt((p5[0]*p5[0])+(p5[1]*p5[1])+(p5[2]*p5[2]));
	fr2=sqrt((p6[0]*p6[0])+(p6[1]*p6[1])+(p6[2]*p6[2]));
	p5[0]/=fr1;
	p5[1]/=fr1;
	p5[2]/=fr1;
	p6[0]/=fr2;
	p6[1]/=fr2;
	p6[2]/=fr2;
	ic_crossproduct(p5,p6,extru);
	//extru is the unitized extrusion direction
	//get the angle between the two lines
	fr1=sqrt((extru[0]*extru[0])+(extru[1]*extru[1])+(extru[2]*extru[2]));
	if (fabs(fr1)>1.0)
	{
		fr1/=fabs(fr1);
	};//asin hurls badly if floating pt error
	//cross product of 2 normalized vectors has magnitude of sine of angle between 'em
	ang1=asin(fr1);
	//check if angle is obtuse using dot product
	if ((p6[0]*p5[0]+p6[1]*p5[1]+p6[2]*p5[2])<0.0)ang1=IC_PI-ang1;
	extru[0]/=fr1;
	extru[1]/=fr1;
	extru[2]/=fr1;


	if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(-1);
	if (0.0==rb.resval.rreal)
	{
		ic_ptcpy(cc,inters);
		ic_ptcpy(fp1,inters);
		ic_ptcpy(fp2,inters);
	}
	else
	{
		fr1=fabs(rb.resval.rreal/sin(ang1/2.0));		//distance from inters to cc
		fr2=fabs(rb.resval.rreal/tan(ang1/2.0));	//distance along the lines from intersection to fillet pts
		//bisect directional vectors to get vector toward cc
		p7[0]=(p5[0]+p6[0])/2.0;
		p7[1]=(p5[1]+p6[1])/2.0;
		p7[2]=(p5[2]+p6[2])/2.0;
		rb.resval.rreal=sqrt(p7[0]*p7[0]+p7[1]*p7[1]+p7[2]*p7[2]);
		if (icadRealEqual(rb.resval.rreal,0.0))return(-1);
		p7[0]/=rb.resval.rreal;
		p7[1]/=rb.resval.rreal;
		p7[2]/=rb.resval.rreal;

		p7[0]=inters[0]+fr1*p7[0];
		p7[1]=inters[1]+fr1*p7[1];
		p7[2]=inters[2]+fr1*p7[2];

		//now move p5 & p6 to the intersection pts where arc will start & end
		fp1[0]=inters[0]+fr2*p5[0];
		fp1[1]=inters[1]+fr2*p5[1];
		fp1[2]=inters[2]+fr2*p5[2];
		fp2[0]=inters[0]+fr2*p6[0];
		fp2[1]=inters[1]+fr2*p6[1];
		fp2[2]=inters[2]+fr2*p6[2];
		//we already know which point we're not moving, so if fp is beyond it (going away from int),
		//	then no fillet possible
		if (0==l1typ)
		{
			if (direction1==1)
			{
				if (fabs(fr2*p5[dim1])>(fabs(p1[dim1]-inters[dim1])+fuzz))return(-1);
			}
			else
			{
				if (fabs(fr2*p5[dim1])>(fabs(p2[dim1]-inters[dim1])+fuzz))return(-1);
			}
		}
		else if ((1&l1typ) && 2==direction1)
		{
			if (fabs(fr2*p5[dim1])>(fabs(p1[dim1]-inters[dim1])+fuzz))return(-1);
		}
		if (0==l2type)
		{
			if (direction2==3)
			{
				if (fabs(fr2*p6[dim2])>(fabs(p3[dim2]-inters[dim2])+fuzz))return(-1);
			}
			else
			{
				if (fabs(fr2*p6[dim2])>(fabs(p4[dim2]-inters[dim2])+fuzz))return(-1);
			}
		}
		else if ((1&l2typ) && 4==direction2)
		{
			if (fabs(fr2*p6[dim2])>(fabs(p3[dim2]-inters[dim2])+fuzz))return(-1);
		}
		if (0==l1typ && (fp1[dim1]>max(p1[dim1],p2[dim1])-fuzz ||
						 fp1[dim1]<min(p1[dim1],p2[dim1])+fuzz))ret=1;
		if (0==l2typ && (fp2[dim2]>max(p3[dim2],p4[dim2])-fuzz ||
						 fp2[dim2]<min(p3[dim2],p4[dim2])+fuzz))ret=1;

		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,extru);
		if (rb2.restype==RTSHORT)
		{
			//if points passed were wcs - convert to new ecs
			sds_trans(fp1,&rb2,&rb,0,p5);
			sds_trans(fp2,&rb2,&rb,0,p6);
			sds_trans(p7,&rb2,&rb,0,cc);
			//because of the way we defined the ecs w/crossing the vectors, the arc will
			//	ALWAYS run clockwise from p5 to p6, so reverse sa&ea
			*sa=sds_angle(cc,p6);
			*ea=sds_angle(cc,p5);
		}
		else if (extru[2]>0.0)
		{
			//pts were ecs, so all new pts ecs and extru is 0,0,1
			//	so set it back
			ic_ptcpy(extru,rb2.resval.rpoint);
			ic_ptcpy(cc,p7);
			*sa=sds_angle(cc,fp2);
			*ea=sds_angle(cc,fp1);
		}
		else
		{
			//all pts ecs and arc antiparallel to other stuff
			*sa=(3.0*IC_PI)-sds_angle(p7,fp2);
			*ea=(3.0*IC_PI)-sds_angle(p7,fp1);
			ic_ptcpy(extru,rb2.resval.rpoint);
			extru[0]=-extru[0];
			extru[1]=-extru[1];
			extru[2]=-extru[2];
			//Note: don't mess w/fp's.  they're on arc whose extru won't change
			/*fp1[0]=-fp1[0];
			fp1[2]=-fp1[2];
			fp2[0]=-fp2[0];
			fp2[2]=-fp2[2];*/
			cc[0]=-p7[0];
			cc[1]=p7[1];
			cc[2]=-p7[2];

		}
	}

	if (0!=l1typ)
	{
		if (2==direction1)*pts2move|=1;
	}
	else
	{
		if (1==direction1)*pts2move|=1;
	}
	if (0!=l2typ)
	{
		if (4==direction2)*pts2move|=2;
	}
	else
	{
		if (3==direction2)*pts2move|=2;
	}
	if (l1type!=NULL)*l1type=l1typ;
	if (l2type!=NULL)*l2type=l2typ;

	return(ret);
}

