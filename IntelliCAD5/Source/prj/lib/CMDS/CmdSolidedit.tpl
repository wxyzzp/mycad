// File  :
// Author: Alexey Malov
template <typename handle>
CCmdQuery* TCmdSubentsProcess<handle>::query() throw(exception)
{
    run();
    if (m_rc != RTNORM &&
        m_rc != RTCAN)
        throw exception();
    return next();
}

template <typename handle>
void TCmdSubentsProcess<handle>::init(bool bRemoving)
{
    m_pSelectSubents->init(bRemoving);
}

template <typename handle>
int TCmdSubentsProcess<handle>::undraw()
{
    sds_name ename;
    entname(ename);
    
    return sds_redraw(ename, IC_REDRAW_UNDRAW) == RTNORM;
}

template <typename handle>
int TCmdSubentsProcess<handle>::update()
{
    if (m_obj.getSat().empty())
        return 0;

    sds_name ename;
    entname(ename);
    undo(m_iSubent)->modify(ename, obj());

	if (CCmdSolidProcess::m_InitProcess) { // need for UNDO command to undo changes made by SOLIDEDIT
		resbuf* p = sds_entget(ename);
		ic_assoc(p, 1);
		char*& dxf1 = ic_rbassoc->resval.rstring;
		delete [] dxf1;
		dxf1 = new char [m_obj.getSat().size()+1];
		strcpy(dxf1, m_obj.getSat().c_str());
		sds_entmod(p);
		CCmdSolidProcess::m_InitProcess = false;
	}
	else {
	    entity()->setData(m_obj);
		sds_entupd(ename);
	}

    return 1;
}

template <typename handle>
int TCmdSubentsProcess<handle>::redraw()
{
    if (entity())
    {
        CModeler::get()->unhighlight(obj(), 0);

        sds_name ename;
        entname(ename);
        sds_entupd(ename);
    }
    return 1;
}

