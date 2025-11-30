// Source/prj/lib/IcadUtils/TransformUtils.cpp

#include <memory.h>

#ifndef __TransformUtilsH__
#include "TransformUtils.h"
#endif


int wc2uc( const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = RTSHORT;
    from.resval.rint = 0; //wcs
    to.restype = RTSHORT;
    to.resval.rint = 1;   //ucs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int uc2wc( const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = RTSHORT;
    from.resval.rint = 1; //ucs
    to.restype = RTSHORT;
    to.resval.rint = 0;   //wcs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int ec2uc( ads_name ent, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = RTENAME;
    ads_name_set( ent, from.resval.rlname );
    to.restype = RTSHORT;
    to.resval.rint = 1; //ucs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int ec2wc( ads_name ent, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = RTENAME;
    ads_name_set( ent, from.resval.rlname );
    to.restype = RTSHORT;
    to.resval.rint = 0; //wcs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int uc2ec( ads_name ent, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    to.restype = RTENAME;
    ads_name_set( ent, to.resval.rlname );
    from.restype = RTSHORT;
    from.resval.rint = 1; //ucs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int wc2ec( ads_name ent, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    to.restype = RTENAME;
    ads_name_set( ent, to.resval.rlname );
    from.restype = RTSHORT;
    from.resval.rint = 0; //wcs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int ec2ec( ads_name ent1, ads_name ent2, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = to.restype = RTENAME;
    ads_name_set( ent1, from.resval.rlname );
    ads_name_set( ent2, to.resval.rlname );
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int ec2uc( const ads_point zaxis, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = RT3DPOINT;
    ads_point_set( zaxis, from.resval.rpoint );
    to.restype = RTSHORT;
    to.resval.rint = 1; //ucs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int ec2wc( const ads_point zaxis, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    from.restype = RT3DPOINT;
    ads_point_set( zaxis, from.resval.rpoint );
    to.restype = RTSHORT;
    to.resval.rint = 0; //wcs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int uc2ec( const ads_point zaxis, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    to.restype = RT3DPOINT;
    ads_point_set( zaxis, to.resval.rpoint );
    from.restype = RTSHORT;
    from.resval.rint = 1; //ucs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}

int wc2ec( const ads_point zaxis, const ads_point src, ads_point dst, int disp )
{
    resbuf from, to;

    to.restype = RT3DPOINT;
    ads_point_set( zaxis, to.resval.rpoint );
    from.restype = RTSHORT;
    from.resval.rint = 0; //wcs
    ads_point  tmp;
    ads_point_set( src, tmp );
    return( ads_trans( tmp, &from, &to, disp, dst ) == RTNORM );
}



void wc2ucMatrix( sds_matrix matrix )
{
    sds_point  axis = { 0, 0, 0 };
    sds_point  zero = { 0, 0, 0 };
    sds_point  newAxis;

    for( int i = 0; i < 3; i++ )
    {
        sds_point_set( zero, newAxis );
        axis[i] = 1.;
        wc2uc( axis, newAxis, 1 );
        matrix[0][i] = newAxis[0];
        matrix[1][i] = newAxis[1];
        matrix[2][i] = newAxis[2];
        axis[i] = 0.;
    }
    sds_point_set( zero, newAxis );
    wc2uc( zero, newAxis, 0 );
    matrix[3][0] = matrix[3][1] = matrix[3][2] = 0.;
    matrix[0][3] = newAxis[0];
    matrix[1][3] = newAxis[1];
    matrix[2][3] = newAxis[2];
    matrix[3][3] = 1.;
}

void uc2wcMatrix( sds_matrix matrix )
{
    sds_point  axis = { 0, 0, 0 };
    sds_point  zero = { 0, 0, 0 };
    sds_point  newAxis;

    for( int i = 0; i < 3; i++ )
    {
        sds_point_set( zero, newAxis );
        axis[i] = 1.;
        uc2wc( axis, newAxis, 1 );
        matrix[0][i] = newAxis[0];
        matrix[1][i] = newAxis[1];
        matrix[2][i] = newAxis[2];
        axis[i] = 0.;
    }
    sds_point_set( zero, newAxis );
    uc2wc( zero, newAxis, 0 );
    matrix[3][0] = matrix[3][1] = matrix[3][2] = 0.;
    matrix[0][3] = newAxis[0];
    matrix[1][3] = newAxis[1];
    matrix[2][3] = newAxis[2];
    matrix[3][3] = 1.;
}


// c = a*b
void multMtx2Mtx( const sds_matrix a, const sds_matrix b, sds_matrix c )
{
    sds_matrix result;
    double     dtmp;
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            dtmp = 0;
            for( int k = 0; k < 4; k++ )
            {
                dtmp += a[i][k] * b[k][j];
            }
            result[i][j] = dtmp;
        }
    }
	memcpy( c, result, sizeof(sds_matrix) );
}
