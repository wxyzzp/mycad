#pragma once
#ifndef _CMDQUERYTOOLS_H_
#define _CMDQUERYTOOLS_H_
#include "CmdCommand.h"
#include "db.h"
#include "res/icadrc2.h"
#include "Modeler.h"
#include <string>
#include <exception>
#include <vector>
#include "xstd.h"
#include "gvector.h"
#include "DbAcisEntity.h"

//forward declaration
namespace icl
{
    class transf;
};

/*-------------------------------------------------------------------------*//**
class CCmdQueryInt - quering for int value
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdQueryInt: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    int m_iAnswer;

public:
    CCmdQueryInt();
    void init
    (
    const char* szPrompt,
    const char* szKeys = 0,
    int accept = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    int getValue() const;
	void setValue(int);
};

/*-------------------------------------------------------------------------*//**
class CCmdQueryDist - quering for distance
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdQueryDist: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    double m_dAnswer;

public:
    CCmdQueryDist();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    double getValue() const;
	void setValue(double);
};

class CCmdQueryEntity: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    // selecetd entity
    sds_name m_entity;
    // picked point
    icl::point m_pt;

public:
    CCmdQueryEntity();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

	virtual bool isAppropriate() const {return true;}

    db_handitem* getEntity() const;
    void getEntityName(sds_name);
};

/*-------------------------------------------------------------------------*//**
class CCmdQuerySelectionSet - quering for selection set
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CCmdQuerySelectionSet: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    // selected entities
    sds_name m_ssEntities;

	// string with types of object to select
	std::string m_sTypes;
    // entity filter
    resbuf m_filter;

public:
    CCmdQuerySelectionSet();
    virtual ~CCmdQuerySelectionSet();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    void getEntities(sds_name);

	const std::string& getFilter();
	void setFilter(const char*);
};

/*-------------------------------------------------------------------------*//**
Class CCmdQuerySwitch - implementation of switch in command scenario
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
typedef int degree;
template <degree N>
class TCmdQuerySwitch: public CCmdQueryEx, public TCmdQueryLink<N>
{
protected:
    // variants of selection
    std::string m_asKeys[N];
    // default selection
    int m_default;
	// selected item
	int m_selection;

public:
    TCmdQuerySwitch();

    void init
    (
    const char* szPrompt,
    const char* szKeys,
    int accept = 0,
    int _default = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

	int getDefault() const;
	void setDefault(int);

	int getSelection() const;
};

class CCmdQueryPoint: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    icl::point m_point;
    icl::point m_base;
    bool m_bUseBase;

public:
    CCmdQueryPoint();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    void useBasePoint(const icl::point*);

    icl::point getValue() const;
	void setValue(const icl::point&);
};

class CCmdQueryVector: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    icl::gvector m_vector;

    enum
    {
        eVector,
        eBasePoint,
        eCount
    };
    std::string m_asKeys[eCount];

public:
    CCmdQueryVector();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    icl::gvector getValue() const;
	void setValue(const icl::gvector&);
};

class CCmdQueryAxis: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:

    CCmdQueryPoint m_getPoint;

    icl::point m_root;
    icl::gvector m_dir;

public:
    CCmdQueryAxis();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    icl::point getRoot() const;
	void setRoot(const icl::point&);
    icl::gvector getDirection() const;
    void setDirection(const icl::gvector&);
};

class CCmdQueryAngle: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
    double m_angle;

public:
    CCmdQueryAngle();
    void init
    (
    const char*,
    const char* = 0,
    int = 0
    );

    virtual CCmdQuery* query() throw (std::exception);

    double getValue() const;
    void setValue(double);
};

class CCmdQueryAcisEntity: public CCmdQueryEntity
{
    friend class CCmdSolidedit;

public:
    CCmdQueryAcisEntity()
    :CCmdQueryEntity()
    {
        init("\nSelect Acis entity: ");
    }

    virtual bool isAppropriate() const;

	const CDbAcisEntityData& getAcisEntityData() const;
	CDbAcisEntity* getEntity() const;
};

template <typename type>
class TSetWithUndo
{
protected:
    std::vector<type> m_objects;
    std::vector<type> m_removed;

    int m_mode;

    typedef std::vector<type>::iterator iterator;

public:
    TSetWithUndo()
        :m_mode(eAdd)
    {}

    const std::vector<type>& set();

    int add(const type& t);
    int add
    (
    std::vector<type>::iterator first,
    std::vector<type>::iterator last,
    int n = 1
    );
    type added(int i);

    int remove(const type& t);
    int remove
    (
    std::vector<type>::iterator first,
    std::vector<type>::iterator last,
    int n = 1
    );
    type removed(int i);

    void clear();

    type undo();

    enum
    {
        eRemove,
        eAdd,
		eCount
    };
    void mode(int i);
};

template <typename handle>
class TCmdQuerySubents: public CCmdQueryEx, public TCmdQueryLink<1>
{
    friend class CCmdSolidedit;

protected:
//    CCmdQueryAcisEntity m_selectAcisEntity;

    CDbAcisEntity* m_dbitem;
    sds_name m_ename;
    CDbAcisEntityData m_obj;

    TSetWithUndo<handle> m_objects;
    // it's array of items selected(unselected) during last pick
    // items are added in acis.dll, so we forced to use
    // safe memory allocating/deallocating array
    xstd<handle>::vector m_last;

    std::string m_sKeys;
    enum
    {
        eRemove,
        eAdd,
        eUndo,
        eAll,
        eNMode
    };
    int m_mode;
    std::string m_asKeys[eNMode];

    icl::point m_pt;

    virtual void initEntity();
    virtual int pick(bool bAll = false) throw (std::exception);
    virtual int parse() throw (std::exception);
    virtual int update(bool bAll = false) throw (std::exception);
    virtual int regen();

	// select sub-entity on the picked point and return 0 with "m_rc == RTVOID"
	// if there is not any sub-entity on the point. If there are more than one
	// sub-entities are on the pick point, they will all be selected if the 
	// "bAll" is true. Otherwise, only one of them will be seleted.
	//
    virtual int select(bool bAll = false) throw (std::exception);
    virtual void selectNone() {}

    int unhighlight(handle) throw (std::exception);
    int highlight(handle) throw (std::exception);

	virtual char* getFillet() { return "3DSOLID,BODY,REGION"/*DNT*/; }

    virtual int pickSubent(icl::gvector&, double) = 0;
	virtual int selectOnWrongEntityErrorIndex() const = 0;
    virtual int getAllSubents() = 0;

	virtual void updatePromptAndKeys() = 0;

public:
    TCmdQuerySubents();

    // clear arrays of selected/unselected objects,
	// set default mode
    void init(bool bRemoving = false);

    // set on add mode
    virtual void add();
    // set on remove mode
    virtual void remove();
    // perform undo
    virtual void undo();
    // select/uselect all subents
    virtual void all();

    // get selected subent
    const CDbAcisEntityData& getAcisEntityData();
    // get selected objects
    const std::vector<handle>& getObjects();
    // get acis entity
    CDbAcisEntity* getEntity();

    bool setAcisEntity(sds_name entity);
    // get sds_name of acis entity
    void getEntityName(sds_name);

    // performs query
    virtual CCmdQuery* query() throw (std::exception);
};

class CCmdQueryFaces: public TCmdQuerySubents<HFACE>
{
    friend class CCmdSolidedit;

protected:

    virtual int select(bool bAll) throw (std::exception);
    virtual int pickSubent(icl::gvector&, double);
    virtual int getAllSubents();

	virtual void updatePromptAndKeys()
	{
        if (m_mode == eRemove) {
            if (!m_dbitem) {
		        m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDO, "Remove Undo"));
			    m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_REMOVEFACES_ADDUNDO,"\nRemove faces: Add/Undo: "));
            }
            else {
                m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDOALL, "Remove Undo ALL"));
                m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_REMOVEFACES_ADDUNDOALL,"\nRemove faces: Add/Undo/ALL: "));
            }
        }
        else {
            if (!m_dbitem) {
		        m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDO, "Remove Undo"));
			    m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SELECTFACES_REMOVEUNDO,"\nSelect faces: Remove/Undo: "));
            }
            else {
                m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDOALL, "Remove Undo ALL"));
                m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SELECTFACES_REMOVEUNDOALL,"\nSelect faces: Remove/Undo/ALL: "));
            }
        }
	}

	virtual int selectOnWrongEntityErrorIndex() const { return CMD_ERR_SEL_FACE_ON_WRONG_SOLID; }


public:
    CCmdQueryFaces()
        :TCmdQuerySubents<HFACE>()
    {
        m_sPrompt.assign((!m_dbitem) ? 
            ResourceString(IDC_QUERYTOOLS_SELECTFACES_REMOVEUNDO,"\nSelect faces: Remove/Undo: ") :
            ResourceString(IDC_QUERYTOOLS_SELECTFACES_REMOVEUNDOALL,"\nSelect faces: Remove/Undo/ALL: "));
    }
};

class CCmdQueryEdges: public TCmdQuerySubents<HEDGE>
{
    friend class CCmdSolidedit;

protected:

    virtual int pickSubent(icl::gvector&, double);
    virtual int getAllSubents();
	virtual void updatePromptAndKeys()
	{
        if (m_mode == eRemove) {
            if (!m_dbitem) {
                m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDO, "Remove Undo"));
			    m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_REMOVEEDGES_ADDUNDO,"\nRemove edges: Add/Undo: "));
            }
            else {
                m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDOALL, "Remove Undo ALL"));
                m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_REMOVEEDGES_ADDUNDOALL,"\nRemove edges: Add/Undo/ALL: "));
            }
        }
        else {
            if (!m_dbitem) {
                m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDO, "Remove Undo"));
			    m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SELECTEDGES_REMOVEUNDO,"\nSelect edges: Remove/Undo: "));
            }
            else {
                m_sKeys.assign(ResourceString(IDC_QUERYTOOLS_REMOVEUNDOALL, "Remove Undo ALL"));
                m_sPrompt.assign(ResourceString(IDC_QUERYTOOLS_SELECTEDGES_REMOVEUNDOALL,"\nSelect edges: Remove/Undo/ALL: "));
            }
        }
	}

	virtual int selectOnWrongEntityErrorIndex() const { return CMD_ERR_SEL_EDGE_ON_WRONG_SOLID; }

public:
    CCmdQueryEdges()
        :TCmdQuerySubents<HEDGE>()
    {
        m_sPrompt.assign((!m_dbitem) ? 
                         ResourceString(IDC_QUERYTOOLS_SELECTEDGES_REMOVEUNDO,"\nSelect edges: Remove/Undo: ") : 
                         ResourceString(IDC_QUERYTOOLS_SELECTEDGES_REMOVEUNDOALL,"\nSelect edges: Remove/Undo/ALL: "));
    }
};

class CCmdQueryCurves: public CCmdQuerySelectionSet
{
public:
    CCmdQueryCurves();

    int getCurves(std::vector<geodata::curve*>& curves);
};

class CCmdQueryColor: public CCmdQueryEx, public TCmdQueryLink<1>
{
    int m_color;

public:
    CCmdQueryColor()
        :
    CCmdQueryEx(),
    TCmdQueryLink<1>(),
    m_color(0)
    {}

    virtual CCmdQuery* query() throw (std::exception);

    int value()
    {
        return m_color;
    }
};

class CCmdQueryYesOrNo: public CCmdQueryEx, public TCmdQueryLink<1>
{
protected:
	bool m_bAnswer;

public:
    CCmdQueryYesOrNo();

	CCmdQueryYesOrNo
	(
	const char*,
	int
    );

	virtual CCmdQuery* query() throw (exception);
    bool value() {return m_bAnswer;}
};

template <int N>
class TCmdQueryPointOrKeyWord: public CCmdQueryEx, public TCmdQueryLink<N>
{
	// array of key words
	std::string m_asKeys[N-1];
	// selected item
	int m_selection;
	// inputed point
	icl::point m_point;

public:
	void init(const char*, const char* = 0, int = 0);

    virtual CCmdQuery* query() throw (exception);

	int getSelection() const;
	icl::point getPoint() const;
};

class CCmdQueryCommandCallback: public CCmdQueryEx
{
public:
	typedef CCmdQuery*(CCmdCommand::*method_t)();

protected:
	CCmdCommand* m_pCommand;

	method_t m_callback;

public:
	CCmdQueryCommandCallback();
	CCmdQueryCommandCallback(CCmdCommand*, method_t);

	void init(CCmdCommand*, method_t);

	virtual CCmdQuery* query() throw (exception);
};

/*-------------------------------------------------------------------------*//**
Check type of a given entity, by comparision with other parameters;
last parameter should be -1.
@author Alexey Malov
@param => entity, to clarify its type
@param => int value - db type, e.g. DB_3DSOLID, DB_LINE,...
@return true - if type of the entity is one of passed; 0 - otherwise
*//*--------------------------------------------------------------------------*/
bool isKindOf(const db_handitem* pe, ...);

/*-------------------------------------------------------------------------*//**
Creates acis entity of specyfied type, from sat-data, return entity name.
@author Alexey Malov
@param => acis object
@param <= new entity's name
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int create
(
const CDbAcisEntityData&,
sds_name
);


/*-------------------------------------------------------------------------*//**
Creates acis entity of specyfied type, from sat-data, return entity name.
@author Stephen Hou
@param <= new entity's name
@param => acis object
@param => color
@param => layer
@param => linetype
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int create
(
sds_name ename,					// output: new entity name
const CDbAcisEntityData& obj,	// input:  aics object
int color,						// input:  color
const char* szLayer = NULL,		// optional input: layer name
const char* szLinetype = NULL	// optional input:linetype
);

/*-------------------------------------------------------------------------*//**
Creates acis entity of specyfied type, from sat-data.
@author Alexey Malov
@param => acis object
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int create
(
const CDbAcisEntityData&
);

// transformations into standard cordinate systems
/*-------------------------------------------------------------------------*//**
Creates transformation from UCS to WCS
@author Alexey Malov
@param <= ucs->wcs transformation
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int ucs2wcs
(
icl::transf&
);

/*-------------------------------------------------------------------------*//**
Creates transformation from UCS to WCS
@author Alexey Malov
@return proper transformation - for success; null transformation - for error
*//*--------------------------------------------------------------------------*/
icl::transf ucs2wcs();

/*-------------------------------------------------------------------------*//**
Creates transformation from WCS to UCS
@author Alexey Malov
@param <= wcs->ucs transformation
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int wcs2ucs
(
icl::transf&
);

/*-------------------------------------------------------------------------*//**
Creates transformation from WCS to UCS
@author Alexey Malov
@return proper transformation - for success; null transformation - for error
*//*--------------------------------------------------------------------------*/
icl::transf wcs2ucs();

/*-------------------------------------------------------------------------*//**
Considering given transf, as UCS->UCS', modify it in the WCS->UCS->UCS'->WCS
@author Alexey Malov
@param <=> transformation
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int wcs2ucs2wcs
(
icl::transf&
);

/*-------------------------------------------------------------------------*//**
Creates transformation from coordinate system, defined by zaxis, to WCS
@author Alexey Malov
@param => z axis of source coordinate system
@return proper transformation for success; null transformation for error
*//*--------------------------------------------------------------------------*/
icl::transf ocs2wcs(const icl::gvector&);

/*-------------------------------------------------------------------------*//**
Creates transformation from WCS to OCS.
@author Alexey Malov
@param => z axis of destination coordinate system
@return proper transformation for success; null transformation for error
*//*--------------------------------------------------------------------------*/
icl::transf wcs2ocs(const icl::gvector&);

/*-------------------------------------------------------------------------*//**
If 'rc' doesn't match neither 'success_value', function throws exception int(rc)
@author Alexey Malov
@param rc => checked value
@param success_value => values, describing successfull result
@return rc
*//*--------------------------------------------------------------------------*/
int checkSuccess(int rc, int success_value, ...) throw (int);

/*-------------------------------------------------------------------------*//**
If 'rc' does match some 'fail_value', function throws exception int(rc)
@author Alexey Malov
@param rc => checked value
@param fail_value => values, describing failure
@return rc
*//*--------------------------------------------------------------------------*/
int checkNoFailure(int rc, int fail_value, ...) throw (int);

/*-------------------------------------------------------------------------*//**
Print ACIS error message
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
void printAcisError();

/*-------------------------------------------------------------------------*//**
Function object, describing unary function with void return value
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template <class _A>
class vfun1_t: std::unary_function<_A, void>
{
	void(*_Op)(_A);
public:
	vfun1_t(void(*Op)(_A)):
	  _Op(Op){}

	void operator ()(_A Arg)
	{
		(*_Op)(Arg);
	}
};
template <class _A> inline
vfun1_t<_A> ptr_vfun1(void(*_X)(_A))
{
	return vfun1_t<_A>(_X);
}

/*-------------------------------------------------------------------------*//**
Auto-destroyer of std::vector of pointers,
destroy objects pointed by vector items
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
template <typename _Ty>
struct ptr_vector_holder
{
	std::vector<_Ty*>& m_vector;

	ptr_vector_holder(std::vector<_Ty*>& v)
		:m_vector(v)
	{}

	~ptr_vector_holder()
	{
		for_each(m_vector.begin(), m_vector.end(), ptr_vfun1(::operator delete));
	}
};

#include "CmdQueryTools.tpl"

#endif
