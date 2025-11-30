#pragma once
#ifndef _AS_FUNCTIONAL_H_
#define _AS_FUNCTIONAL_H_
#include <functional>

template <typename _Op>
class destraction_t
{
	_Op m_action;

	destraction_t(const destraction_t&){}
public:
	destraction_t(_Op action)
		:m_action(action)
	{}
	
	virtual ~destraction_t()
	{
		m_action();
	}
};

template <typename _A>
class vfun10_t
{
	void(*_Op)(_A);
	_A _Arg;

public:
	vfun10_t(void(*Op)(_A), _A Arg):
	  _Op(Op),_Arg(Arg){}
	void operator () ()
	{(*_Op)(_Arg);}
};

template <typename _A> inline
vfun10_t<_A> ptr_vfun10(void(*Op)(_A), _A Arg)
{
	return vfun10_t<_A>(Op,Arg);
}

template <typename _A1, typename _A2, typename _A3, typename _R>
class fun30_t
{
	_R(*_Op)(_A1,_A2,_A3);
	_A1 _Arg1;
	_A2 _Arg2;
	_A3 _Arg3;

public:
	fun30_t(void(*Op)(_A1,_A2,_A3),_A1 Arg1,_A2 Arg2,_A3 Arg3):
	  _Op(Op),_Arg1(Arg1),_Arg2(Arg2),_Arg3(Arg3)
	  {}

	_R operator ()()
	{
		return (*_Op)(_Arg1,_Arg2,_Arg3);
	}
};

template <typename _A1, typename _A2, typename _A3, typename _R>
inline
fun30_t<_A1,_A2,_A3,_R> ptr_fun30
(
_R(*Op)(_A1,_A2,_A3),
_A1 Arg1,
_A2 Arg2,
_A3 Arg3
)
{
	return fun30_t<_A1,_A2,_A3,_R>(Op,Arg1,Arg2,Arg3);
}

template <class _A>
class vfun1_t: std::unary_function<_A, void>
{
	void(*_Op)(_A);
public:
	vfun1_t(void(*Op)(_A)):
	  _Op(Op)
	{}

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

template <typename _R>
class ident_t: std::unary_function<_R,_R>
{
public:
	_R operator () (_R _V)
	{
		return _V;
	}
};

template <typename _A, typename _R>
class fun1r_t: std::unary_function<_A,_R>
{
	typedef _A& arg_reference;
	typedef _A* arg_pointer;
	_R(*_Op)(_A&);

public:
	explicit fun1r_t(_R(Op)(_A&)):
	_Op(Op){}

	_R operator () (_A* Arg) throw(std::exception)
	{
		if (0 == Arg)
			throw std::exception();
		return ((*_Op)(*Arg));
	}
};

template <typename _A, typename _R> inline
fun1r_t<_A,_R> ptr_fun1r(_R(*Op)(const _A&))
{
	return fun1r_t<_A,_R>(Op);
}

#endif
