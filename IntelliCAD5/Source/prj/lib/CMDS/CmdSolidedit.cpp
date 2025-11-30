// File  :
// Author: Alexey Malov
#pragma warning (disable : 4786)
#include "CmdSolidedit.h"
#include "Gr.h"
#include "IcadView.h"
#include "Icad.h"
#include "CmdCurveConstructor.h"
#include "Geodata.h" //geodata::curve
using geodata::curve;
#include "Modeler.h"
#include <exception>
using std::exception;
#include <string>
using std::string;
#include "transf.h"
using icl::transf;
using icl::translation;
#include "gvector.h"
using icl::point;
using icl::gvector;
#include <memory>
using std::auto_ptr;

#ifdef _DEBUG
#define inline
#include "CmdSolidedit.inl"
#undef inline
#endif

bool CCmdSolidProcess::m_InitProcess = true;

// Implementation of command SOLIDEDIT
extern "C"
{
short cmd_solidedit(void)
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	CCmdSolidProcess::m_InitProcess = true;

    CCmdSolidedit cmd;

	SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"SolidEdit"/*DNT*/,NULL,NULL,SDS_CURDWG);
    short result = cmd.run();
	SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"SolidEdit"/*DNT*/,NULL,NULL,SDS_CURDWG);
	return result;
}
}

enum
{
    eEdgeUndo,
    eFaceUndo,
    eBodyUndo
};
// statics
static int s_iLastUndoSubent = -1;
static CCmdUndoProcess* s_pEdgeUndo = 0;
static CCmdUndoProcess* s_pFaceUndo = 0;
static CCmdUndoProcess* s_pBodyUndo = 0;
static
CCmdUndoProcess* undo(int);


// Implemenation

// Queries

CCmdQuery* CCmdQueryDistanceOrPath::query() throw (exception)
{
    try
    {
        m_getDist.init(ResourceString(IDC_SOLIDEDIT_SPECIFYHEIGHTOFEXTRUSION, "\nSpecify height of extrusion or Path: "),
                       ResourceString(IDC_SOLIDEDIT_PATH, "Path"),
                       RSG_NONULL|RSG_NOZERO);
        m_getDist.query();
    }
    catch(const exception& e)
    {
        if (m_getDist.rc() != RTKWORD)
            throw e;
    }

    if (m_getDist.rc() == RTKWORD)
    {
        // Select path
        m_getPath.init(ResourceString(IDC_SOLIDEDIT_SELECTEXTRUSIONPATH, "\nSelect extrusion path: "));
        m_getPath.query();

        if (!geodata::extract(*m_getPath.getEntity(), m_pPath))
            throw exception();
    }
    else
    {
        // Query taper angle
        try
        {
            m_getTaperAngle.init(ResourceString(IDC_SOLIDEDIT_SPECIFYANGLEOFTAPER, "\nSpecify angle of taper for extrusion <0>: "));
            m_getTaperAngle.query();
        }
        catch(const exception& e)
        {
            if (m_getTaperAngle.rc() != RTNONE)
                throw e;
        }
    }
    return next();
}

// Runs

int CCmdSolidCheck::run()
{
    CModeler::get()->lockDatabase();
    {
        if (CCmdCommand::run())
        {
            xstd<icl::string>::vector messages;
            if (CModeler::get()->check(m_get3dSolid.getAcisEntityData(), messages))
            {
                sds_printf("\n");
                for (int i = 0, n = messages.size(); i < n; ++i)
                {
                    sds_printf(messages[i].c_str());
                }
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be 
				// terminated based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}


void CCmdSolidShellQueryFaces::initEntity()
{
    // Override the same function of parrent to not reset m_dbitem
}

void CCmdSolidShellQueryFaces::all()
{
    m_last.resize(0);
    if (getAllSubents())
    {
		m_mode = eAdd;		// Don't move this line to after update()!!!
		m_objects.mode(TSetWithUndo<HFACE>::eAdd);
		update(1);
		m_rc = RTNONE;
    }
}

void CCmdSolidShellQueryFaces::selectNone()
{
    if (!m_last.size())
        all();
}


int CCmdSolidShell::run()
{
    CModeler::get()->lockDatabase();
    {
		init(true); //initialize selector for removing subentities

        int rc;
        try {
            m_get3dSolid.query();
            rc = m_get3dSolid.rc();
        }
        catch (const exception&) {
            m_rc = m_get3dSolid.rc();
            return m_rc;
        }
        if (rc == RTNORM) {
            sds_name ename;
            m_get3dSolid.getEntityName(ename);
            if (m_pSelectSubents->setAcisEntity(ename)) {
                // Run command
                if (CCmdCommand::run())
                {
                    // Perform action
                    if (rc = CModeler::get()->shell(m_get3dSolid.getAcisEntityData(), m_obj, objects(), m_getShellOffsetDist.getValue()))
                    {
                        update();
                    }
			        else {
				        // We don't want to exit from the SOLIDEDIT command immediately if 
                        // the modeler is unable to performance an operation in certain case. 
				        // Note: Setting m_rc to RTERROR value will cause the command to be 
				        // terminated based on the current implementation.-- SWH.
				        //
				        cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			        }
                }
            }
            else 
                cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
        }
        
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdSolidClean::run()
{
	if (CCmdCommand::run())
	{
        int rc;
		if (rc = CModeler::get()->clean(m_get3dSolid.getAcisEntityData(), m_obj))
		{
			update();
		}
        m_rc = rc? RTNORM: RTERROR;
	}

	return m_rc;
}

int CCmdSolidSeparate::run()
{
	if (CCmdCommand::run())
	{
        int rc;
		if (rc = CModeler::get()->separate(m_get3dSolid.getAcisEntityData(), m_objs))
		{
            update();
		}
		else {
			// We don't to exit from the SOLIDEDIT command immediately when in the 
			// certain case the modeler is unable to performance an operation. 
			// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
			// based on the current implementation.-- SWH.
			//
			cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
		}
	}

	return m_rc;
}


int CCmdQueryImprinter::getCurves(std::vector<geodata::curve*>& curves) const
{
	if (!isCurve())
		return 0;

    db_handitem* item = getEntity();
    geodata::curve* crv;
    if (!geodata::extract(*item, crv)) {
        for (int i = 0, n = curves.size(); i < n; ++i)
            delete curves[i];

        return 0;
    }
    curves.push_back(crv);

    return 1;
}

int CCmdSolidImprint::run()
{
    try
    {
        m_get3dSolid.query();
        m_rc = m_get3dSolid.rc();
    }
    catch (const exception&)
    {
        m_rc = m_get3dSolid.rc();
        return m_rc;
    }

	try
	{
        m_pStart = &m_getImprinter;
		while (CCmdCommand::run())
		{
			int oc;
			if (m_getImprinter.is3DSolid()) {
				oc = CModeler::get()->imprint(m_get3dSolid.getAcisEntityData(),
										      m_getImprinter.getAcisEntityData(),
										      m_obj,
										      m_deleteSource.value());
			}
			else {
				std::vector<geodata::curve*> curves;
				if (!m_getImprinter.getCurves(curves))
					continue;

				oc = CModeler::get()->imprint(m_get3dSolid.getAcisEntityData(), curves, m_obj);
			}
			if (oc) {
				update();

				sds_name ename;
				m_get3dSolid.getEntityName(ename);
				sds_redraw(ename, IC_REDRAW_DRAW);
			}
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
		}
	}
	catch (const exception&)
	{
		if (m_rc == RTNONE)
			return RTNORM;
	}

	return m_rc;
}

int CCmdEdgesColor::run()
{
    CModeler::get()->lockDatabase();
    {
        init();
        if (CCmdCommand::run())
        {
            int rc;
            if (rc = CModeler::get()->colorEdges(obj(), //=> old data
												 m_obj, //<= new data
                                                 objects(),
                                                 m_queryColor.value()))
            {
                undraw();
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be 
				// terminated based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdEdgesCopy::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        if (CCmdCommand::run())
        {
            transf tr = translation(m_queryDisplacement.getValue());
            wcs2ucs2wcs(tr);

            redraw();

            int rc;
            xstd<geodata::curve*>::vector curves;
            if (rc = CModeler::get()->copyEdges(obj(),
                                            objects(),
                                            curves,
                                            tr))
            {
                xstd<geodata::curve*>::vector::iterator icrv = curves.begin();
                for (; icrv != curves.end(); ++icrv)
                {
                    dbcstr::create(**icrv);
                    delete *icrv;

                    sds_name ename;
                    sds_entlast(ename);
                    undo(eEdgeUndo)->erase(ename);
                }
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be 
				// terminated based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
            redraw();
        }
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesTaper::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        // Run command
        if (CCmdCommand::run())
        {
            // Undraw entity
            undraw();

            transf u2w = ucs2wcs();
            point base = m_queryAxis.getRoot();
            base *= u2w;
            gvector dir = m_queryAxis.getDirection();
            dir *= u2w;

            int rc;
            // Perform action
            if (rc = CModeler::get()->taperFaces(obj(),
												 m_obj,
	                                             objects(),
		                                         base,
			                                     dir,
				                                 m_queryAngle.getValue()))
            {
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        // Update entity
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesExtrude::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        // Run command
        if (CCmdCommand::run())
        {
            // We need to call redraw() rather than undraw() to unhighlight
			// the selected face - SWH
			//
            redraw();

            int rc;
            // Perform action
            if (m_queryDistOrPath.getPath())
                rc = CModeler::get()->extrudeFacesAlongPath(obj(), m_obj,
															objects(),
															*m_queryDistOrPath.getPath());
            else
                rc = CModeler::get()->extrudeFaces(obj(), m_obj,
													objects(),
													m_queryDistOrPath.getDistance(),
													m_queryDistOrPath.getTaperAngle());
            if (rc)
            {
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        // Update entity
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesMove::run()
{
    CModeler::get()->lockDatabase();
    {
        init();
        if (CCmdCommand::run())
        {
            undraw();

            transf mtx = translation(m_queryDisplacement.getValue());
            wcs2ucs2wcs(mtx);

            int rc;
            if (rc = CModeler::get()->moveFaces(obj(), m_obj, objects(), mtx))
            {
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesOffset::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        if (CCmdCommand::run())
        {
            // We need to call redraw() rather than undraw() to unhighlight
			// the selected face - SWH
			//
            redraw();

            int rc;
            if (rc = CModeler::get()->offsetFaces(obj(), m_obj,
                                        objects(),
                                       (m_queryOffsetDistance.getValue())))
            {
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesRotate::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        if (CCmdCommand::run())
        {
            undraw();
#if 0
            transf mtx;
            {
                mtx.rotate(m_getAngle.value(), m_getRotationAxis.getRoot(),
                                             m_getRotationAxis.getDirection());
                wcs2ucs2wcs(mtx);
            }
            int rc;
            if (rc = CModeler::get()->moveFaces(obj() m_obj, objects(), mtx))
#else
            transf u2w = ucs2wcs();
            point root = m_getRotationAxis.getRoot();
            root *= u2w;
            gvector axis = m_getRotationAxis.getDirection();
            axis *= u2w;
            int rc;
            if (rc = CModeler::get()->rotateFaces(obj(),
                                        m_obj,
                                        objects(),
                                        root,
                                        axis,
                                        m_getAngle.getValue()))
#endif
            {
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Note: Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesDelete::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        if (CCmdCommand::run())
        {
            undraw();
            int rc;
            if (rc = CModeler::get()->deleteFaces(obj(), m_obj, objects()))
                update();
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesCopy::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        if (CCmdCommand::run())
        {
            transf mtx = translation(m_queryDisplacement.getValue());
            wcs2ucs2wcs(mtx);

            int rc = CModeler::get()->copyFaces(obj(), objects(), m_objs, mtx);
            if (rc)
            {
                update();
            }
			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

int CCmdFacesColor::run()
{
    CModeler::get()->lockDatabase();
    {
		init();
        if (CCmdCommand::run())
        {
            int rc;
            if (rc = CModeler::get()->colorFaces(obj(), m_obj, objects(), m_queryColor.value()))
            {
                undraw();
                update();
            }
 			else {
				// We don't to exit from the SOLIDEDIT command immediately when in the 
				// certain case the modeler is unable to performance an operation. 
				// Setting m_rc to RTERROR value will cause the command to be terminated 
				// based on the current implementation.-- SWH.
				//
				cmd_ErrorPrompt(CMD_ERR_UNABLE,0);
			}
        }
        redraw();
    }
    CModeler::get()->unlockDatabase();

    return m_rc;
}

// Misc

CCmdQueryDistanceOrPath::~CCmdQueryDistanceOrPath()
{
    delete m_pPath;
}

CCmdEdgesProcess::CCmdEdgesProcess()
:
TCmdSubentsProcess<HEDGE>()
{
    m_pSelectSubents = new CCmdQueryEdges;
    m_iSubent = eEdgeUndo;
}

CCmdFacesProcess::CCmdFacesProcess()
:
TCmdSubentsProcess<HFACE>()
{
    m_pSelectSubents = new CCmdQueryFaces;
    m_iSubent = eFaceUndo;
}

CCmdSolidShellFacesProcess::CCmdSolidShellFacesProcess()
:
TCmdSubentsProcess<HFACE>()
{
    m_pSelectSubents = new CCmdSolidShellQueryFaces;
    m_iSubent = eFaceUndo;
}


CCmdSolidShell::CCmdSolidShell()
:
CCmdSolidShellFacesProcess()
{
    m_pStart = m_pSelectSubents;
    m_pSelectSubents->set(0, &m_getShellOffsetDist);

    m_getShellOffsetDist.init(ResourceString(IDC_SOLIDEDIT_ENTERSHELLOFFSETDIST, "\nEnter the shell offset distance: "));

	m_iSubent = eBodyUndo;
}

// Undo

CCmdSolidEditUndo::CCmdSolidEditUndo()
{
    s_iLastUndoSubent = -1;
    s_pEdgeUndo = new CCmdUndoProcess;
    s_pFaceUndo = new CCmdUndoProcess;
    s_pBodyUndo = new CCmdUndoProcess;
}

CCmdSolidEditUndo::~CCmdSolidEditUndo()
{
    delete s_pEdgeUndo;
    delete s_pFaceUndo;
    delete s_pBodyUndo;
}

static
CCmdUndoProcess* undo(int i)
{
    switch (i)
    {
    case eEdgeUndo:
        s_iLastUndoSubent = eEdgeUndo;
        return s_pEdgeUndo;
    case eFaceUndo:
        s_iLastUndoSubent = eFaceUndo;
        return s_pFaceUndo;
    case eBodyUndo:
        s_iLastUndoSubent = eBodyUndo;
        return s_pBodyUndo;
    }
    return 0;
}

CCmdQuery* CCmdSolidEditUndo::query()
{
    switch (s_iLastUndoSubent)
    {
    case eEdgeUndo:
        s_pEdgeUndo->query();
        break;
    case eFaceUndo:
        s_pFaceUndo->query();
        break;
    case eBodyUndo:
        s_pBodyUndo->query();
        break;
    }
    return next();
}

void CCmdUndoProcess::pop(CCmdUndoProcess::CUndoAtom& atom)
{
    if (m_undo.empty())
    {
        sds_printf(ResourceString(IDC_SOLIDEDIT_NOTHINGTOUNDO, "\nThere is nothing to undo."));
        atom.m_name[0] = atom.m_name[1] = 0;
        atom.m_mode = -1;
    }
    else
    {
        atom = m_undo.top();
        m_undo.pop();
    }
    return;
}

CCmdQuery* CCmdUndoProcess::query()
{
    CUndoAtom atom;
    pop(atom);

    switch (atom.m_mode)
    {
    case CUndoAtom::eModify:
        {
            assert(atom.m_name[0] != 0);
            assert(!atom.m_obj.getSat().empty());

            CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(atom.m_name[0]);

            //it's possible; CDbAcisEntity copies the string data into its std::string
            pObj->setData(atom.m_obj);
            sds_entupd(atom.m_name);
        }
        break;
    case CUndoAtom::eCreate:
        {
            assert(!atom.m_obj.getSat().empty());

            resbuf* elist = sds_buildlist(RTDXF0, "3DSOLID", 1, atom.m_obj.getSat().c_str(), 0);
            sds_entmake(elist);
            sds_relrb(elist);
        }
        break;
    case CUndoAtom::eErase:
        assert(atom.m_name[0] != 0);

        sds_entdel(atom.m_name);
        break;
    }

    return next();
}


int CCmdSolidProcess::update()
{
    if (m_obj.getSat().empty())
        return 0;

	sds_name ename;
    m_get3dSolid.getEntityName(ename);

    // store old data
    undo(eBodyUndo)->modify(ename, m_get3dSolid.getAcisEntityData());

	if (m_InitProcess) {
		resbuf* p = sds_entget(ename);
		ic_assoc(p, 1);
		char*& dxf1 = ic_rbassoc->resval.rstring;
		IC_FREE(dxf1);		
		dxf1 = new char [m_obj.getSat().size()+1];
		strcpy(dxf1, m_obj.getSat().c_str());
		sds_entmod(p);
		m_InitProcess = false;
	}
	else {
		CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(m_get3dSolid.getEntity());
		pObj->setData(m_obj);
		sds_entupd(ename);
	}

    return 1;
}

int CCmdSolidSeparate::update()
{
    if (m_objs.empty())
        return 0;
    m_obj = m_objs.front();

    if (!CCmdSolidProcess::update())
        return 0;

    for (int i = 1, n = m_objs.size(); i < n; ++i)
    {
        sds_name ename;
        create(m_objs[i], ename);

        undo(eBodyUndo)->erase(ename);
    }

    return 1;
}

int CCmdSolidImprint::update()
{
	CCmdSolidProcess::update();

	if (m_deleteSource.value())
	{
		// save curves in undo stack and then delete curves
		sds_name ename;
		m_getImprinter.getEntityName(ename);

		// save curve in the undo stack

		// delete the curve
        sds_entdel(ename);
	}

	return 1;
}

int CCmdFacesCopy::update()
{
    for (int i = 0, n = m_objs.size(); i < n; ++i)
    {
        sds_name ename;
        create(m_objs[i], ename);

        undo(eFaceUndo)->erase(ename);
    }
    return 1;
}

void CCmdSolidedit::init()
{
    m_selectSubentity.init(ResourceString(IDC_SOLIDEDIT_ENTERSOLIDEDITINGOPTION, "\nEnter a solids editing option:"),
		ResourceString(IDC_SOLIDEDIT_SOLIDOPTIONS, "Face Edge Body Undo eXit"), 0, 4);

    m_editFaces.init(ResourceString(IDC_SOLIDEDIT_ENTERFACEEDITINGOPTION,"\nEnter a face editing option:"),
		ResourceString(IDC_SOLIDEDIT_FACEOPTIONS, "Extrude Move Rotate Offset Taper Delete Copy coLor Undo eXit"), 0, 9);

    m_editEdges.init(ResourceString(IDC_SOLIDEDIT_ENTEREDGEEDITINGOPTION,"\nEnter an edge editing option:"),
		ResourceString(IDC_SOLIDEDIT_EDGEOPTIONS, "Copy coLor Undo eXit"), 0, 3);

	m_editBody.init(ResourceString(IDC_SOLIDEDIT_ENTERBODYEDITINGOPTION,"\nEnter a body editing option"),
		ResourceString(IDC_SOLIDEDIT_BODYOPTIONS, "Imprint seParate Shell cLean Check Undo eXit"), 0, 6);

    initScenario();
}

void CCmdSolidedit::initScenario()
{
    m_pStart = &m_selectSubentity;

    m_selectSubentity.  set(ObjectSelectionOptions::eFace, &m_editFaces);
    m_selectSubentity.  set(ObjectSelectionOptions::eEdge, &m_editEdges);
    m_selectSubentity.  set(ObjectSelectionOptions::eBody, &m_editBody);
    m_selectSubentity.  set(ObjectSelectionOptions::eExit, 0);
    m_selectSubentity.  set(ObjectSelectionOptions::eUndo, &m_undo);
    m_undo.             set(0,                             &m_selectSubentity);

    m_editFaces.        set(FaceEditOptions::eExtrude,  &m_extrudeFaces);
    m_extrudeFaces. set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eMove,     &m_moveFaces);
    m_moveFaces.    set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eRotate,   &m_rotateFaces);
    m_rotateFaces.  set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eOffset,   &m_offsetFaces);
    m_offsetFaces.  set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eTaper,    &m_taperFaces);
    m_taperFaces.   set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eDelete,   &m_deleteFaces);
    m_deleteFaces.  set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eCopy,     &m_copyFaces);
    m_copyFaces.    set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eColor,    &m_colorFaces);
    m_colorFaces.   set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eUndo,     s_pFaceUndo);
    s_pFaceUndo->   set(0,                          &m_editFaces);
    m_editFaces.    set(FaceEditOptions::eExit,     &m_selectSubentity);

    m_editEdges.    set(EdgeEditOptions::eCopy,     &m_copyEdges);
    m_copyEdges.    set(0,                          &m_editEdges);
    m_editEdges.    set(EdgeEditOptions::eColor,    &m_colorEdges);
    m_colorEdges.   set(0,                          &m_editEdges);
    m_editEdges.    set(EdgeEditOptions::eUndo,     s_pEdgeUndo);
    s_pEdgeUndo->   set(0,                          &m_editEdges);
    m_editEdges.    set(EdgeEditOptions::eExit,     &m_selectSubentity);

    m_editBody.     set(BodyEditOptions::eImprint,  &m_imprintBody);
    m_imprintBody.  set(0,                          &m_editBody);
    m_editBody.     set(BodyEditOptions::eSeparate, &m_separateBody);
    m_separateBody. set(0,                          &m_editBody);
    m_editBody.     set(BodyEditOptions::eShell,    &m_shellBody);
    m_shellBody.    set(0,                          &m_editBody);
    m_editBody.     set(BodyEditOptions::eClean,    &m_cleanBody);
    m_cleanBody.    set(0,                          &m_editBody);
    m_editBody.     set(BodyEditOptions::eCheck,    &m_checkBody);
    m_checkBody.    set(0,                          &m_editBody);
    m_editBody.     set(BodyEditOptions::eUndo,     s_pBodyUndo);
    s_pBodyUndo->   set(0,                          &m_editBody);
    m_editBody.     set(BodyEditOptions::eExit,     &m_selectSubentity);
}

