#pragma once
#ifndef _CMDSOLIDEDIT_H_
#define _CMDSOLIDEDIT_H_

#include "CmdCommand.h"
#include <afx.h>	/*DG - 1.3.2002*/// hack: include MFC before inclusion of windows.h in subsequential includes from CmdQueryTools.h
#include "CmdQueryTools.h"
#include "DbAcisEntityData.h"
#include <stack>
#include "Modeler.h"

extern "C" short cmd_solidedit(void);

// forward declarations
namespace geodata
{
    struct curve;
}

/*-------------------------------------------------------------------------*//**
Helper class, providing selection of 3dsolid.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdQuery3dSolid: public CCmdQueryEntity
{
public:
    CCmdQuery3dSolid()
    :CCmdQueryEntity()
    {
        init("\nSelect 3d solid: ");

        // To accept keyword - SWH, 24/8/2004
        m_accept = RSG_OTHER;
    }

    // Checks selected object (3dsolid or not)
    virtual bool isAppropriate() const;
    // Returns sat-data of the selected 3dsolid
	const CDbAcisEntityData& getAcisEntityData() const;
};


/*-------------------------------------------------------------------------*//**
Base classes for comands in Solidedit/Body, Slidedit/Face, Solidedit/Edge directries.
*//*--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*//**
Base class for comands in Solidedit/Body directory.
Provides selection of 3dsolid, and tools to update entity data.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidProcess: public CCmdCommand, public TCmdQueryLink<1>
{
protected:
    // Query, providing selection of 3dsolid
    CCmdQuery3dSolid m_get3dSolid;
    // modified sat data, it's passed in modeler call and is used by update().
	CDbAcisEntityData m_obj;

    // update sat-data of the solid, free modeler memory (m_sat),
    // call sds_entupd, register modification at undo stack
	int update();

public:
    // ctor
    CCmdSolidProcess()
    {
        m_pStart = &m_get3dSolid;
    }
    // dtor
    virtual ~CCmdSolidProcess(){}

    // default implementation for inheritors
    virtual CCmdQuery* query() throw(exception);

	static bool	m_InitProcess;
};

/*-------------------------------------------------------------------------*//**
Common class for subentities (faces/edges) processing.
Provides selection of faces/edges, and tools to update entity data.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template <typename handle>
class TCmdSubentsProcess: public CCmdCommand, public TCmdQueryLink<1>
{
protected:
    // Query, providing selection of faces/edegs
    TCmdQuerySubents<handle>* m_pSelectSubents;

	// initialize mode and data before running
	// bRemoving tells what mode should be used as default
	void init
	(
	bool bRemoving = false
	);

    // modified object, it's passed in modeler call and is used by update().
    CDbAcisEntityData m_obj;

    // undraws whole entity
    int undraw();
    // update sat-data of the solid,
    // register modification at undo stack
    int update();
    // redraw the whole entity
    int redraw();

    // helper methods
    CDbAcisEntity* entity()
    {
        return m_pSelectSubents->getEntity();
    }
    void entname(sds_name ename)
    {
        m_pSelectSubents->getEntityName(ename);
    }
    const CDbAcisEntityData& obj()
    {
        return m_pSelectSubents->getAcisEntityData();
    }
    const vector<HFACE>& objects()
    {
        return m_pSelectSubents->getObjects();
    }

    // it's initialized by inheritors
    // means index of corresponding undo "manager"
    int m_iSubent;
    // corresponding undo "manager"
    class CCmdUndoProcess;
    CCmdUndoProcess* m_pUndo;

    // protected ctor
    TCmdSubentsProcess()
    :
    m_iSubent(-1),
    m_pUndo(0)
    {};
public:
    // dtor
    virtual ~TCmdSubentsProcess()
    {
        delete m_pSelectSubents;
    };

    // default implentation of the query (main functionality is in virtual run())
    virtual CCmdQuery* query() throw (exception);
};

/*-------------------------------------------------------------------------*//**
Base class for comands in Solidedit/Edge directory.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdEdgesProcess: public TCmdSubentsProcess<HEDGE>
{
public:
    // default ctor
    CCmdEdgesProcess();
};

/*-------------------------------------------------------------------------*//**
Base class for comands in Solidedit/Face directory.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesProcess: public TCmdSubentsProcess<HFACE>
{
public:
    // default ctor
    CCmdFacesProcess();
};



/*-------------------------------------------------------------------------*//**
                            Solid processing
*//*--------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*//**
The class for Shell command option of Solidedit/Body directory
@author Stephen Hou
*//*--------------------------------------------------------------------------*/
class CCmdSolidShellFacesProcess: public TCmdSubentsProcess<HFACE>
{
public:
    // default ctor
    CCmdSolidShellFacesProcess();
};


/*-------------------------------------------------------------------------*//**
The face query class for Shell command option of Solidedit/Body directory
@author Stephen Hou
*//*--------------------------------------------------------------------------*/
class CCmdSolidShellQueryFaces : public CCmdQueryFaces
{
public:
    CCmdSolidShellQueryFaces() {}

    virtual void initEntity();

    virtual void selectNone();

    virtual void all();
};


/*-------------------------------------------------------------------------*//**
Transform solid, into "thick" shell. Give the set of faces, unite them into sheet,
thick the sheet into solid (it is thick shell).
It's part of subcommand 'solidedit/body'
@author Alexey Malov
*//*--------------------------------------------------------------------------*/

class CCmdSolidShell: public CCmdSolidShellFacesProcess
{
    CCmdQuery3dSolid m_get3dSolid;
    CCmdQueryDist m_getShellOffsetDist;

	void initShelling();

public:
    CCmdSolidShell();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Helper class, providing selection of the tool to imprint.
The profit is the class check:
    is the selected entity acis entity and
    is this different from blank object
@author Alexey Malov
@param
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
class CCmdQueryImprinter: public CCmdQueryEntity 
{
    // Query, used for blank selection
	const CCmdQuery3dSolid* m_pGet3dSolid;

public:
	CCmdQueryImprinter(const CCmdQuery3dSolid* pGet3dSolid)
	:
	CCmdQueryEntity(),
	m_pGet3dSolid(pGet3dSolid)
	{
		init("\nSelect an object to imprint: ");
	}

	bool	isCurve() const;
	bool	is3DSolid() const;

	int		getCurves(std::vector<geodata::curve*>& curves) const;

	const CDbAcisEntityData& getAcisEntityData() const;

	virtual bool isAppropriate() const;
};


/*-------------------------------------------------------------------------*//**
Imprints an acis object at 3dsolid.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidImprint: public CCmdSolidProcess
{
    // Query tool
	CCmdQueryImprinter m_getImprinter;
    // Ask, should imprinter be deleted or should not
	CCmdQueryYesOrNo m_deleteSource;

protected:
	int update();

public:
    CCmdSolidImprint();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Separates multiply-component solid.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidSeparate: public CCmdSolidProcess
{
protected:
    // Sat-data of new one-component objects
    xstd<CDbAcisEntityData>::vector m_objs;
    // Update entity, create other objects
    int update();

public:
    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Cleans solid from faces/edges, that are unnecessarily to maintain topology of the solid.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidClean: public CCmdSolidProcess
{
public:
    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Checks solid for geometru/topology errors.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidCheck: public CCmdSolidProcess
{
public:
    virtual int run();
};









/*-------------------------------------------------------------------------*//**
                            Face processing
*//*--------------------------------------------------------------------------*/








/*-------------------------------------------------------------------------*//**
Taper faces.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesTaper: public CCmdFacesProcess
{
protected:
    // Query taper axis
    CCmdQueryAxis m_queryAxis;
    // Query taper angle
    CCmdQueryAngle m_queryAngle;

public:
    CCmdFacesTaper();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Helper class, providing selection of extrusion path or inputing extrusion distance.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdQueryDistanceOrPath: public CCmdQueryEx, TCmdQueryLink<1>
{
    // Query extrusion distance
    CCmdQueryDist m_getDist;
    // Query extrusion path
    CCmdQueryEntity m_getPath;
    geodata::curve* m_pPath;
    // Query taper angle for extrusion
    CCmdQueryAngle m_getTaperAngle;

public:
    CCmdQueryDistanceOrPath()
        :
    CCmdQueryEx(),
    TCmdQueryLink<1>(),
    m_pPath(0)
    {}

    ~CCmdQueryDistanceOrPath();

    const geodata::curve* getPath() const;
    double getDistance() const;
    double getTaperAngle() const;

    virtual CCmdQuery* query() throw (exception);
};

/*-------------------------------------------------------------------------*//**
Extrude planar face along path or over given distance
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesExtrude: public CCmdFacesProcess
{
protected:
    // Query distance or path for extrusion
    CCmdQueryDistanceOrPath m_queryDistOrPath;

public:
    CCmdFacesExtrude();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Move faces
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesMove: public CCmdFacesProcess
{
protected:
    // Query translation vector
    CCmdQueryVector m_queryDisplacement;

public:
    CCmdFacesMove();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Offset faces
@author Alexey Malov
*//*--------------------------------------------------------------------------*/

class CCmdFacesOffset: public CCmdFacesProcess
{
protected:
    // Query offset distance
    CCmdQueryDist m_queryOffsetDistance;

public:
    CCmdFacesOffset();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Rotate faces.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesRotate: public CCmdFacesProcess
{
protected:
    // Query rotation axis
    CCmdQueryAxis m_getRotationAxis;
    // Query angle of rotation
    CCmdQueryAngle m_getAngle;

public:
    CCmdFacesRotate();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Delete faces.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesDelete: public CCmdFacesProcess
{
public:
    CCmdFacesDelete();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Copy faces
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesCopy: public CCmdFacesProcess
{
protected:
    // Query translation vector
    CCmdQueryVector m_queryDisplacement;
    // sat-data of copied faces
    xstd<CDbAcisEntityData>::vector m_objs;

    // create entities, registers at undo stack
    int update();

public:
    CCmdFacesCopy();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Color faces
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdFacesColor: public CCmdFacesProcess
{
protected:
    // Query new face color
    CCmdQueryColor m_queryColor;

public:
    CCmdFacesColor();

    virtual int run();
};









/*-------------------------------------------------------------------------*//**
                            Edge processing
*//*--------------------------------------------------------------------------*/









/*-------------------------------------------------------------------------*//**
Copy edges
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdEdgesCopy: public CCmdEdgesProcess
{
protected:
    // Query translation vector
    CCmdQueryVector m_queryDisplacement;

public:
    CCmdEdgesCopy();

    virtual int run();
};

/*-------------------------------------------------------------------------*//**
Color edges.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdEdgesColor: public CCmdEdgesProcess
{
protected:
    // Query new edge color
    CCmdQueryColor m_queryColor;

public:
    CCmdEdgesColor();

    virtual int run();
};









/*-------------------------------------------------------------------------*//**
Simple undo manager, especialy for Solidedit.
NOTE: it can became problem, to use parts command Solidedit separately.
The classes (FacesMove, EdgeCopy,...) provide full functionality and
it partialy possible to use them separately, but they use undo managers,
which initialized by CCmdSolidEditUndo. So to use any of subcommand, it's need
to create only copy of CCmdSolidEditUndo.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidEditUndo: public CCmdQuery, public TCmdQueryLink<1>
{
    friend class CCmdSolidedit;

protected:
    CCmdSolidEditUndo();
    ~CCmdSolidEditUndo();

public:

    virtual CCmdQuery* query();
};


/*-------------------------------------------------------------------------*//**
Implementation of command SOLIDEDIT
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdSolidedit: public CCmdCommand
{
protected: //data
    struct ObjectSelectionOptions
    {
        enum
        {
            eFace,
            eEdge,
            eBody,
            eUndo,
            eExit,
            eCount
        };
    };
    TCmdQuerySwitch<ObjectSelectionOptions::eCount> m_selectSubentity;

    struct FaceEditOptions
    {
        enum
        {
            eExtrude,
            eMove,
            eRotate,
            eOffset,
            eTaper,
            eDelete,
            eCopy,
            eColor,
            eUndo,
            eExit,
            eCount
        };
    };
    TCmdQuerySwitch<FaceEditOptions::eCount> m_editFaces;

    CCmdFacesExtrude m_extrudeFaces;
    CCmdFacesMove m_moveFaces;
    CCmdFacesRotate m_rotateFaces;
    CCmdFacesOffset m_offsetFaces;
    CCmdFacesTaper m_taperFaces;
    CCmdFacesDelete m_deleteFaces;
    CCmdFacesCopy m_copyFaces;
    CCmdFacesColor m_colorFaces;

    struct EdgeEditOptions
    {
        enum
        {
            eCopy,
            eColor,
            eUndo,
            eExit,
            eCount
        };
    };
    TCmdQuerySwitch<EdgeEditOptions::eCount> m_editEdges;

    CCmdEdgesCopy m_copyEdges;
    CCmdEdgesColor m_colorEdges;

    struct BodyEditOptions
    {
        enum
        {
            eImprint,
            eSeparate,
            eShell,
            eClean,
            eCheck,
            eUndo,
            eExit,
            eCount
        };
    };
    TCmdQuerySwitch<BodyEditOptions::eCount> m_editBody;

    CCmdSolidImprint m_imprintBody;
    CCmdSolidSeparate m_separateBody;
    CCmdSolidShell m_shellBody;
    CCmdSolidClean m_cleanBody;
    CCmdSolidCheck m_checkBody;

    CCmdSolidEditUndo m_undo;

    void initScenario();

public:
    CCmdSolidedit()
    {
        init();
    }
    void init();
};

/*-------------------------------------------------------------------------*//**
Simple undo manager.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdUndoProcess: public CCmdQuery, public TCmdQueryLink<1>
{
    friend class CCmdSolidEditUndo;

protected:
    CCmdUndoProcess(){};

public:
    void modify(sds_name, const CDbAcisEntityData&);
    void create(const CDbAcisEntityData&);
    void erase(sds_name);

    virtual CCmdQuery* query();

protected:
    // Describes elementary modeling operation: entity creation,
    // deleting, modification
    struct CUndoAtom
    {
        // entity name
        sds_name m_name;
        // Acis object
        CDbAcisEntityData m_obj;
        // undo action
        enum EUndoMode
        {
            eModify,
            eCreate,
            eErase
        };
        int m_mode;

        // ctors
        // default
        CUndoAtom()
            :
        m_mode(-1)
        {
            m_name[0] = m_name[1] = 0;
        }
        // copy
        CUndoAtom(const CUndoAtom& atom)
            :
        m_obj(atom.m_obj),
        m_mode(atom.m_mode)
        {
            m_name[0] = atom.m_name[0];
            m_name[1] = atom.m_name[1];
        }
        // from data
        CUndoAtom(sds_name name, const CDbAcisEntityData& obj, int mode)
            :
        m_mode(mode)
        {
            m_obj = obj;
            m_name[0] = name[0];
            m_name[1] = name[1];
        }
    };

    std::stack<CUndoAtom> m_undo;

    void push(sds_name, const CDbAcisEntityData&, int);
    void pop(CUndoAtom& atom);
};


#ifdef NDEBUG
#include "CmdSolidedit.inl"
#endif

#include "CmdSolidedit.tpl"

#endif
