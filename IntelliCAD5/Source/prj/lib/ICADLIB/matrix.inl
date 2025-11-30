// File  :
// Author: Alexey Malov
#include <assert.h>

namespace icl
{
inline
double& matrix::operator [](int i)
{
    assert(i >= 0);
    assert(i < nelem);
    return m_v[i];
}
inline
double matrix::operator [](int i) const
{
    assert(i >= 0);
    assert(i < nelem);
    return m_v[i];
}
inline
double& matrix::operator ()(int i, int j)
{
    assert(i >= 0); assert(i < ndim);
    assert(j >= 0); assert(j < mdim);
    return m_m[i][j];
}
inline
double matrix::operator ()(int i, int j) const
{
    assert(i >= 0); assert(i < ndim);
    assert(j >= 0); assert(j < mdim);
    return m_m[i][j];
}
inline
bool matrix::operator != (const matrix& m) const
{
    return !(operator == (m));
}
}
