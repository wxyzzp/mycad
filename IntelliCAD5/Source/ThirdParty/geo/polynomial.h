/*	POLYNOMIAL.H - Defines the class of polynimals
 *
 *
 * ABSTRACT:
 *
 * CPolynomial captures the following polynormial
 * 
 *             Y = coef0 + coef1 * x + coef2 * X^2 + coef3 * X ^ 3 + .....
 *
 *
 * Author:			Stephen Hou
 *
 * Created Date:	1/27/2003
 *
 * Modified Date:
 *
 */

#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <vector>


using std::vector;



class CPolynomial
{
public:

	CPolynomial(int degree = 2);
	CPolynomial(double coef0, double coef1);
	CPolynomial(double coef0, double coef1, double coef2);
	CPolynomial(double coef0, double coef1, double coef2, double coef3);
	CPolynomial(double coef0, double coef1, double coef2, double coef3, double coef4);
	CPolynomial(double coef0, double coef1, double coef2, double coef3, double coef4, double coef5);
	CPolynomial(const vector<double> &coefficients);
	CPolynomial(const CPolynomial &polynomial) { *this = polynomial; }
	virtual ~CPolynomial();

	void					setCoefficient(int index, double coefficient);
	double					getCoefficient(int index) const;
	int						getNumOfCoefficients() const { return m_Num; }

	virtual	double			evaluate(double variable) const;

	virtual	void			setDegree(int degree);
	virtual	int				getDegree() const { return (getNumOfCoefficients() - 1); }
	virtual CPolynomial*	getDerivative() const; 

	// find the root(s) of polynomial with degree between 1 - 4. It returns true if 
	// succeeded and false otherwise.
	//
	virtual bool			getRoots(vector<double> &roots) const;

	// find the roots of polynomial within the closed interval [min, max]. It returns 
	// true if succeeded and false otherwise.
	//
	virtual bool			getRootsInInterval(vector<double> &roots,
		                                       double min, double max) const;

	virtual bool			operator==(const CPolynomial &polynomial) const;
	virtual bool			operator!=(const CPolynomial &polynomial) const;

	virtual CPolynomial&	operator=(const CPolynomial &polynomial);
	virtual CPolynomial&	operator*(const CPolynomial &polynomial);
	virtual CPolynomial&	operator*(double scale);

private:

	void					init();
	void					simplify();

	bool					getLinearRoot(double &root) const;
	bool					getQuadraticRoots(vector<double> &roots) const;
	bool					getCubicRoots(vector<double> &roots) const;
	bool					getQuarticRoots(vector<double> &roots) const;

	bool					bisection(double &root, double min, double max) const;

	double*					m_pCoefficients;
	int						m_Num;
};


inline CPolynomial::~CPolynomial()
{ if (m_pCoefficients) delete [] m_pCoefficients; }

inline bool CPolynomial::operator!=(const CPolynomial &polynomial) const
{ return !(*this == polynomial); }

inline void CPolynomial::init()
{	
	for (register i = m_Num - 1; i >= 0; i--) m_pCoefficients[i] = 0.0;
}


#endif // POLYNOMIAL_H