
/*	POLYNOMIAL.CPP - Defines the class of polynimals
 *
 *
 * ABSTRACT:
 *
 * CPolynomial captures the following polynormial
 * 
 *      y = coef0 + coef1 * x + coef2 * x^2 + coef3 * x ^ 3 + .... + coefn * x ^ n
 *
 *
 * Author:			Stephen Hou
 *
 * Created Date:	1/27/2003
 *
 * Modified Date:
 *
 */

#include <memory.h> 
#include <assert.h>
#include <math.h>

#include "constnts.h"
#include "macros.h"
#include "polynomial.h"



#define POLYNOMIAL_ESILON		1.0e-6
#define POLYNOMIAL_ACCURACY		6



CPolynomial::CPolynomial(int degree) : m_Num(degree + 1)
{
	if (m_Num) {
		m_pCoefficients = new double[m_Num];
		for (register i = degree; i >= 0; i--)
			m_pCoefficients[i] = 0.0;
	}
	else
		m_pCoefficients = NULL;
}
	
    
CPolynomial::CPolynomial(double coef0, double coef1) : m_Num(2)
{
	m_pCoefficients = new double[2];
	m_pCoefficients[0] = coef0;
	m_pCoefficients[1] = coef1;
}

	
CPolynomial::CPolynomial(double coef0, double coef1, double coef2) : m_Num(3)
{
	m_pCoefficients = new double[3];
	m_pCoefficients[0] = coef0;
	m_pCoefficients[1] = coef1;
	m_pCoefficients[2] = coef2;
}
	
    
CPolynomial::CPolynomial(double coef0, double coef1, double coef2, double coef3) 
			: m_Num(4)
{
	m_pCoefficients = new double[4];
	m_pCoefficients[0] = coef0;
	m_pCoefficients[1] = coef1;
	m_pCoefficients[2] = coef2;
	m_pCoefficients[3] = coef3;
}


CPolynomial::CPolynomial(double coef0, double coef1, double coef2, double coef3, double coef4)
			: m_Num(5)
{
	m_pCoefficients = new double[5];
	m_pCoefficients[0] = coef0;
	m_pCoefficients[1] = coef1;
	m_pCoefficients[2] = coef2;
	m_pCoefficients[3] = coef3;
	m_pCoefficients[4] = coef4;
}


CPolynomial::CPolynomial(double coef0, double coef1, double coef2, double coef3, double coef4, double coef5)
			: m_Num(6)
{
	m_pCoefficients = new double[6];
	m_pCoefficients[0] = coef0;
	m_pCoefficients[1] = coef1;
	m_pCoefficients[2] = coef2;
	m_pCoefficients[3] = coef3;
	m_pCoefficients[4] = coef4;
	m_pCoefficients[5] = coef5;
}

	
CPolynomial::CPolynomial(const vector<double> &coefficients) : m_Num(coefficients.size())
{
	if (m_Num) {
		m_pCoefficients = new double[m_Num];
		for (register i = 0; i < m_Num; i++) 
			m_pCoefficients[i] = coefficients[i];
	}
	else
		m_pCoefficients = NULL;
}



void CPolynomial::setCoefficient(int index, double coefficient)
{
	assert(index >= 0 && index < m_Num);
	m_pCoefficients[index] = coefficient;
}


double CPolynomial::getCoefficient(int index) const
{
	assert(index >= 0 && index < m_Num);
	return m_pCoefficients[index];
}


double CPolynomial::evaluate(double x) const
{
    double result = 0.0;
    for (register i = getDegree(); i >= 0; i--)
        result = result * x + m_pCoefficients[i];

	return result;
}


void CPolynomial::setDegree(int degree)
{ 
	assert(degree > 0);

	int num = degree + 1;
	if (degree < getDegree()) {
		for (register i = getDegree(); i > degree; i--) 
			m_pCoefficients[i] = 0.0;
	}
	else {
		if (num > sizeof(m_pCoefficients) / sizeof(double)) {
			double* pArray = new double[num];

			// copy original coefficients to new array
			// and set the value of rest elements to 0.0
			//
			degree = getDegree();
			for (register i = 0; i < num - 1; i++) {
				if (i <= degree)
					pArray[i] = m_pCoefficients[i];
				else
					pArray[i] = 0.0;
			}
			delete [] m_pCoefficients;
			m_pCoefficients = pArray;
		}
	}
	m_Num = num;
}


CPolynomial* CPolynomial::getDerivative() const
{
    CPolynomial* pDerivative = new CPolynomial(getDegree() - 1);
    for (register i = 1; i < m_Num; i++) 
        pDerivative->m_pCoefficients[i - 1] = ((double)i) * m_pCoefficients[i];

    return pDerivative;
}


bool CPolynomial::getRoots(vector<double> &roots) const
{
	roots.clear();
	const_cast<CPolynomial*>(this)->simplify();
	switch(getDegree()) {
	case 0:
		return false;
	case 1: 
		{
			double root;
			if (getLinearRoot(root)) {
				roots.push_back(root);
				return true;
			}
			return false;
		}
	case 2:
		return getQuadraticRoots(roots);
	case 3:
		return getCubicRoots(roots);
	case 4:
		return getQuarticRoots(roots);
	default:
		return false;
	}
	return true;
}


bool CPolynomial::getRootsInInterval(vector<double> &roots,			// output
									 double min, double max) const	// input: clased interval
{
    double root;
    if (getDegree() == 1) {
		if (bisection(root, min, max)) {
			roots.push_back(root);
			return true;
		}
		else
			return false;
    } 
	else { 
		vector<double> dRoots;
        CPolynomial* pDerivative = getDerivative();
        if (pDerivative->getRootsInInterval(dRoots, min, max)) { // get roots of derivative
            if (bisection(root, min, dRoots[0]))
				roots.push_back(root);

            // find root on [dRoots[i], dRoots[i+1]] for 0 <= i <= count-2
			//
            for (register i = 0; i <= dRoots.size() - 2; i++) {
                if (bisection(root, dRoots[i], dRoots[i + 1]))
					roots.push_back(root);
            }

            if (bisection(root, dRoots[dRoots.size() - 1], max))
				roots.push_back(root);
        } 
		else { // polynomial is monotone on [min,max], has at most one root

            if (bisection(root, min, max))
				roots.push_back(root);
        }
		delete pDerivative;
    }
	return true;
}


bool CPolynomial::operator==(const CPolynomial &polynomial) const
{
	int num = getNumOfCoefficients();
	if (num == polynomial.getNumOfCoefficients()) {
		for (register i = num - 1; i >= 0; i--) {
			if (m_pCoefficients[i] != polynomial.m_pCoefficients[i])
				return false;
		}
	}
	return false;
}


CPolynomial& CPolynomial::operator=(const CPolynomial &polynomial)
{
	int num = polynomial.getNumOfCoefficients();
	if (num != getNumOfCoefficients()) {
		if (m_pCoefficients) {
			delete [] m_pCoefficients;
			m_pCoefficients = NULL;
		}
		if (num) {
			m_pCoefficients = new double[num];
			for (register i = num - 1; i >= 0; i--)
				m_pCoefficients[i] = polynomial.m_pCoefficients[i];
		}
	}
	return *this;
}


CPolynomial& CPolynomial::operator*(const CPolynomial &polynomial)
{
	double* pNewOne = NULL;
	if (!getNumOfCoefficients() || !polynomial.getNumOfCoefficients()) {
		int degree1 = getDegree();
		int degree2 = polynomial.getDegree();

		int newDegree = degree1 + degree2;
		pNewOne = new double[newDegree];
		for (register i = newDegree - 1; i >= 0; i--)
			pNewOne[i] = 0.0;

		for (register j = 0; j <= degree1; j++) 
			for (register k = 0; k <= degree2; k++) 
				pNewOne[j + k] += m_pCoefficients[j] * polynomial.m_pCoefficients[k];
	}
	delete [] m_pCoefficients;
	m_pCoefficients = pNewOne;

	return *this;
}


CPolynomial& CPolynomial::operator*(double scale)
{
	if (m_pCoefficients) {
		int num = getNumOfCoefficients();
		for (register i = num - 1; i >= 0; i--)
			m_pCoefficients[i] *= scale;
	}
	return *this;
}


void CPolynomial::simplify()
{
    for (register i = getDegree(); i >= 0; i--) {
        if (fabs(m_pCoefficients[i]) <= POLYNOMIAL_ESILON)
            m_Num--;
        else
            break;
    }
}

	
bool CPolynomial::getLinearRoot(double &root) const
{  
    if (fabs(m_pCoefficients[1]) > POLYNOMIAL_ESILON)
        root = m_pCoefficients[0] / m_pCoefficients[1];

	return false;
}


bool CPolynomial::getQuadraticRoots(vector<double> &roots) const
{
    if (getDegree() == 2) {
        double a = m_pCoefficients[2];
        double b = m_pCoefficients[1];
        double c = m_pCoefficients[0];

		if (FZERO(a, POLYNOMIAL_ESILON)) {
			if (!FZERO(b, POLYNOMIAL_ESILON)) 
				roots.push_back(c / b);
		}
		else {
			double d = b * b - 4.0 * a * c;
			if (fabs(d) < POLYNOMIAL_ESILON)
				d = 0.0;

			a *= 2.0;
			if (d > 0.0) {
				double e = sqrt(d);
						
				roots.push_back((-b + e) / a);
				roots.push_back((-b - e) / a);
			} 
			else if (d == 0.0)	// really two roots with same value, but we only return one
				roots.push_back(-b / a);
		}
    }

    return (roots.size() > 0);
}


bool CPolynomial::getCubicRoots(vector<double> &roots) const
{
    if (getDegree() == 3) {
        double c0 = m_pCoefficients[0];
		double c1 = m_pCoefficients[1];
		double c2 = m_pCoefficients[2];
		double c3 = m_pCoefficients[3];

		if (FZERO(c0, POLYNOMIAL_ESILON)) {
			double x = 0.0;			// one of solutions

			// compute cubic equation
			//
			CPolynomial polynomial(c1, c2, c3);
			if (!polynomial.getQuadraticRoots(roots))
				return false;

			// add solution 0.0 to the list iff it is not in on the list
			//
			bool found = false;
			for (register i = 0; i < roots.size(); i++) {
				if (FZERO(roots[i], POLYNOMIAL_ESILON)) {
					found = true;
					break;
				}
			}
			if (!found) 
				roots.push_back(x);
		}
		else {
			if (!FZERO(c3, POLYNOMIAL_ESILON)) {
				c2 /= c3;
				c1 /= c3;
				c0 /= c3;
			}

			double a       = (3.0 * c1 - c2 * c2) / 3.0;
			double b       = (2.0 * c2 * c2 * c2 - 9.0 * c1 * c2 + 27.0 * c0) / 27.0;
			double offset  = c2 / 3.0;
			double discrim = b * b / 4.0 + a * a * a / 27.0;
			double halfB   = b / 2.0;

			if (fabs(discrim) <= POLYNOMIAL_ESILON) 
				discrim = 0.0;
        
			if (discrim > 0.0) {
				double e = sqrt(discrim);
				double tmp;
				double root;

				tmp = -halfB + e;
				if (tmp >= 0.0)
					root = pow(tmp, 1.0 / 3.0);
				else
					root = -pow(-tmp, 1.0 / 3.0);

				tmp = -halfB - e;
				if (tmp >= 0.0)
					root += pow(tmp, 1.0 / 3.0);
				else
					root -= pow(-tmp, 1.0 / 3.0);

				roots.push_back(root - offset);
			} 
			else if (discrim < 0.0) {
				double distance = sqrt(-a/3.0);
				double angle    = atan2(sqrt(-discrim), -halfB) / 3.0;
				double cosAngle = cos(angle);
				double sinAngle = sin(angle);
				double sqrt3    = sqrt(3.0);

				roots.push_back(2.0 * distance * cosAngle - offset);
				roots.push_back(-distance * (cosAngle + sqrt3 * sinAngle) - offset);
				roots.push_back(-distance * (cosAngle - sqrt3 * sinAngle) - offset);
			} 
			else {
				double tmp;

				if (halfB >= 0.0)
					tmp = -pow(halfB, 1.0 / 3.0);
				else
					tmp = pow(-halfB, 1.0 / 3.0);

				roots.push_back(2.0 * tmp - offset);
				// really should return next root twice, but we return only one
				roots.push_back(-tmp - offset);
			}
		}
    }
	return (roots.size() > 0);
}


bool CPolynomial::getQuarticRoots(vector<double> &roots) const
{
    if (getDegree() == 4) {
		double c0 = m_pCoefficients[0];
		double c1 = m_pCoefficients[1];
		double c2 = m_pCoefficients[2];
		double c3 = m_pCoefficients[3];
		double c4 = m_pCoefficients[4];

		if (FZERO(c0, POLYNOMIAL_ESILON)) {
			double x = 0.0;			// one of solutions

			// compute cubic equation
			//
			CPolynomial polynomial(c1, c2, c3, c4);
			if (!polynomial.getCubicRoots(roots))
				return false;

			// add solution 0.0 to the list iff it is not in on the list
			//
			bool found = false;
			for (register i = 0; i < roots.size(); i++) {
				if (FZERO(roots[i], POLYNOMIAL_ESILON)) {
					found = true;
					break;
				}				
			}
			if (!found) 
				roots.push_back(x);
		}
		else {
			if (!FZERO(c4, POLYNOMIAL_ESILON)) {
				c3 /= c4;
				c2 /= c4;
				c1 /= c4;
				c0 /= c4;
			}

			vector<double> resolveRoots;
			CPolynomial polynomial(-c3 * c3 * c0 + 4.0 * c2 * c0 - c1 * c1, c3 * c1 - 4.0 * c0, -c2, 1.0);

			if (!polynomial.getCubicRoots(resolveRoots))
				return false;

			double y       = resolveRoots[0];
			double discrim = c3 * c3 / 4.0 - c2 + y;

			if (fabs(discrim) <= POLYNOMIAL_ESILON) 
				discrim = 0.0;

			if (discrim > 0.0) {
				double e     = sqrt(discrim);
				double t1    = 3.0 * c3 * c3 / 4.0 - e * e - 2.0 * c2;
				double t2    = (4.0 * c3 * c2 - 8.0 * c1 - c3 * c3 * c3) / (4.0 * e);
				double plus  = t1 + t2;
				double minus = t1 - t2;

				if (fabs(plus)  <= POLYNOMIAL_ESILON) 
					plus  = 0.0;

				if (fabs(minus) <= POLYNOMIAL_ESILON) 
					minus = 0.0;

				if (plus >= 0.0) {
					double f = sqrt(plus);

					roots.push_back( -c3 / 4.0 + (e + f) / 2.0);
					roots.push_back( -c3 / 4.0 + (e - f) / 2.0);
				}
				if (minus >= 0.0) {
					double f = sqrt(minus);

					roots.push_back( -c3 / 4.0 + (f - e) / 2.0);
					roots.push_back( -c3 / 4.0 - (f + e) / 2.0);
				}
			} 
			else if (discrim < 0.0) { // no roots
			} 
			else {
				double t2 = y * y - 4.0 * c0;

				if (t2 >= -POLYNOMIAL_ESILON) {
					if (t2 < 0.0) 
						t2 = 0.0;

					t2 = 2.0 * sqrt(t2);
					double t1 = 3.0 * c3 * c3 / 4.0 - 2.0 * c2;
					if (t1 + t2 >= - POLYNOMIAL_ESILON) {
						double d = sqrt(t1 + t2);
						if (d != 0.0) {
							roots.push_back(- c3 / 4.0 + d / 2.0);
							roots.push_back(- c3 / 4.0 - d / 2.0);
						}
						else
							roots.push_back(- c3 / 4.0);
					}
					double tDiff = t1 - t2;
					if (tDiff >= - POLYNOMIAL_ESILON) {
						double d = sqrt(tDiff);
						if (d != 0.0) {
							roots.push_back(- c3 / 4.0 + d / 2.0);
							roots.push_back(- c3 / 4.0 - d / 2.0);
						}
						else
							roots.push_back(- c3 / 4.0);
					}
				}
			}
		}
    }
	return (roots.size() > 0);
}



bool CPolynomial::bisection(double &root, double min, double max) const
{
    double minValue = evaluate(min);
    double maxValue = evaluate(max);
 
    if (fabs(minValue) <= POLYNOMIAL_ESILON)
        root = min;
    else if (fabs(maxValue) <= POLYNOMIAL_ESILON)
        root = max;
    else if (minValue * maxValue <= 0.0) {
        double tmp1  = log(max - min);
        double tmp2  = log(10.0) * (double)POLYNOMIAL_ACCURACY;
        double iters = ceil((tmp1 + tmp2) / log(2.0));

        for (register i = 0; i < iters; i++ ) {
            root = 0.5 * (min + max);
            double value = evaluate(root);

            if (fabs(value) <= POLYNOMIAL_ESILON) 
                break;

            if (value * minValue < 0) {
                max = root;
                maxValue = value;
            } else {
                min = root;
                minValue = value;
            }
        }
    }

    return true;
}



