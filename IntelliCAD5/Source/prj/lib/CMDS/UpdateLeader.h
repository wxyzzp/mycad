#ifndef UPDATELEADER_H
#define UPDATELEADER_H

// EBATECH(watanabe)-[update leader according to modifying annotation
class CUpdateLeader
{
public:
    CUpdateLeader(db_leader*);
    virtual bool Redraw(db_handitem* p = NULL);
    virtual bool ModifyLeader(db_objhandle*);
	//Bugzilla No. 78013 ; 01-04-2002 
    //virtual bool GetDimgap(sds_real&, int&);
private:
    db_leader* m_leader;
};

class CUpdateLeaderFactory
{
public:
    virtual CUpdateLeader* CreateUpdateLeader(db_leader*);
};
// ]-EBATECH(watanabe)

#endif // UPDATELEADER_H
