// Source/prj/lib/IcadUtils/TransformUtils.h
#ifndef __TransformUtilsH__
#define __TransformUtilsH__

#include "sds.h"

int wc2uc( const ads_point src, ads_point dst, int disp );
int uc2wc( const ads_point src, ads_point dst, int disp );
int ec2uc( ads_name ent, const ads_point src, ads_point dst, int disp );
int ec2wc( ads_name ent, const ads_point src, ads_point dst, int disp );
int uc2ec( ads_name ent, const ads_point src, ads_point dst, int disp );
int wc2ec( ads_name ent, const ads_point src, ads_point dst, int disp );
int ec2ec( ads_name ent1, ads_name ent2, const ads_point src, ads_point dst, int disp );
int ec2uc( const ads_point zaxis, const ads_point src, ads_point dst, int disp );
int ec2wc( const ads_point zaxis, const ads_point src, ads_point dst, int disp );
int uc2ec( const ads_point zaxis, const ads_point src, ads_point dst, int disp );
int wc2ec( const ads_point zaxis, const ads_point src, ads_point dst, int disp );
void wc2ucMatrix( sds_matrix matrix );
void uc2wcMatrix( sds_matrix matrix );
void multMtx2Mtx( const sds_matrix a, const sds_matrix b, sds_matrix c );

#endif  // __TransformUtilsH__
