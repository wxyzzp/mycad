#pragma once
#include <functional>

template <class _A>
class vfunc1_t: std::unary_function<_A, void>
{
	void(*_Op)(_A);
public:
	vfunc1_t(void(*Op)(_A)):
	  _Op(Op)
	{}

	void operator ()(_A Arg)
	{
		(*_Op)(Arg);
	}
};

template <typename _Ty, typename _R>
class rmem0_t
{
	_R(_Ty::* _Op)();
	_Ty* _P;

public:
	rmem0_t(_Ty* P, _R(_Ty::* Op)()):
	  _P(P),_Op(Op)
	{}
	_R operator ()()
	{
		return ((_P->*_Op)());
	}
};

template <typename _Ty, typename _A>
class vmem1_t: std::unary_function<_A, void>
{
	void(_Ty::* _Op)(_A);
	_Ty* _P;

public:
	vmem1_t(_Ty* P,void(_Ty::*Op)(_A)):
	  _Op(Op),_P(P)
	{}

	void operator () (_A Arg)
	{
		(_P->*_Op)(Arg);
	}
};

template <typename _R, typename _Ty, typename _A>
class rmem1_t: std::unary_function<_A, _R>
{
	_R(_Ty::* _Op)(_A);
	_Ty* _P;

public:
	rmem1_t(_Ty* P,_R(_Ty::*Op)(_A)):
	  _Op(Op),_P(P)
	{}

	_R operator () (_A Arg)
	{
		return (_P->*_Op)(Arg);
	}
};

template <class _A> inline
vfunc1_t<_A> ptr_vfun1(void(*_X)(_A))
{
	return vfunc1_t<_A>(_X);
}

template <typename _Ty, typename _R> inline
rmem0_t<_Ty,_R> ptr_rmem0(_Ty* obj, _R(_Ty::* mem)())
{
	return rmem0_t<_Ty,_R>(obj, mem);
}

template <typename _Ty, typename _A> inline 
vmem1_t<_Ty,_A> ptr_vmem1(_Ty*_P,void(_Ty::*_Op)(_A))
{
	return vmem1_t<_Ty,_A>(_P,_Op);
}

template <typename _R, typename _Ty, typename _A> inline 
rmem1_t<_R,_Ty,_A> ptr_rmem1(_Ty*_P,_R(_Ty::*_Op)(_A))
{
	return rmem1_t<_R,_Ty,_A>(_P,_Op);
}

template <typename _Op>
class action_on_destruct
{
	_Op m_action;

public:
	action_on_destruct(_Op action)
		:m_action(action)
	{}
	
	~action_on_destruct()
	{
		m_action();
	}
};

template <typename _Op1, typename _Op2>
class guard_t
{
	_Op1 _in;
	_Op2 _out;
public:
	guard_t(_Op1 in, _Op2 out)
		:_in(in), _out(out)
	{
		_in();
	}
	~guard_t()
	{
		_out();
	}

};
