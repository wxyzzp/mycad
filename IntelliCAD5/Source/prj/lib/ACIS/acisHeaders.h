// File  : <DevDir>\source\prj\lib\acis\acisHeaders.h
// Author: Dmitry Gavrilov

#pragma once

#ifndef _ACISHEADERS_H_
#define _ACISHEADERS_H_


//#define MMGR_DISABLED
#define ACIS_DLL
#define NT

#pragma warning (disable : 4251)	// due to the following declaration: "class /*DECL_RB*/ rgb_color"
									// in the rbase\rnd_husk\rgbcolor.hxx file

// Header files grouped by component: (for names of all components, see page 4-2 "Getting Started Guide")

/* Base (BASE) */
#ifndef __acisHXX__
#include "acis.hxx"
#define __acisHXX_
#endif

#if !defined( BASE_HEADER_INCLUDED )
#include "base.hxx"
#endif

#if !defined( logical )
#include "logical.h"
#endif

#ifndef __freelistHXX__
#include "freelist.hxx"
#define __freelistHXX__
#endif

#ifndef acistol_hxx
#include "acistol.hxx"
#define acistol_hxx
#endif

#ifndef api_err
#include "api.err"
#define api_err
#endif

#if !defined( INTERVAL_CLASS )
#include "interval.hxx"
#endif

#if !defined( MATRIX_CLASS )
#include "matrix.hxx"
#endif

#if !defined( POSITION_CLASS )
#include "position.hxx"
#endif

#if !defined( TRANSF_CLASS )
#include "transf.hxx"
#endif

#if !defined( UNITVEC_CLASS )
#include "unitvec.hxx"
#endif

#if !defined( VECTOR_CLASS )
#include "vector.hxx"
#endif

/* Kernel (KERN) */
#ifndef VERSION_HDR_DEF
#include "version.hxx"
#define VERSION_HDR_DEF
#endif

#ifndef acistype_hxx
#include "acistype.hxx"
#endif

#ifndef bnd_arc_hxx
#include "bnd_arc.hxx"
#endif

#ifndef bnd_crv_hxx
#include "bnd_crv.hxx"
#endif

#ifndef bnd_line_hxx
#include "bnd_line.hxx"
#endif

#ifndef bnd_point_hxx
#include "bnd_pnt.hxx"
#endif

#ifndef cpoyent_hxx
#include "copyent.hxx"
#endif

#ifndef entwray_hxx
#include "entwray.hxx"
#endif

#ifndef geom_utl_hxx
#include "geom_utl.hxx"
#endif

#ifndef getowner_hxx
#include "getowner.hxx"
#endif

#ifndef pick_ray_hxx
#include "pick_ray.hxx"
#endif

#ifndef position_array_hxx
#include "posarray.hxx"
#endif

#ifndef text_hxx
#include "text.hxx"
#endif

#ifndef wcs_hxx
#include "wcs.hxx"
#endif

#ifndef wire_qry_hxx
#include "wire_qry.hxx"
#endif

#ifndef curveq_hxx
#include "curveq.hxx"
#endif

#if !defined( API_HEADER )
#include "api.hxx"
#endif

#ifndef KERNAPI_HXX
#include "kernapi.hxx"
#endif

#if !defined( ATTRIB_CLASS )
#include "attrib.hxx"
#endif

#if !defined( ENTITY_MISC )
#include "datamsc.hxx"
#endif

#if !defined( DEBUG_ENTITY )
#include "debug.hxx"
#endif

#if !defined( ENTITY_CLASS )
#include "entity.hxx"
#endif

#if !defined( ALLSURF_HDR_DEF )
#include "allsurf.hxx"
#endif

#if !defined( CNSTRUCT_HDR_DEF )
#include "cnstruct.hxx"
#endif

#if !defined( CURVE_CLASS )
#include "curve.hxx"
#endif

#if !defined( DEGENERATE_CLASS )
#include "degenerate.hxx"
#endif

#if !defined( ELLIPSE_CLASS )
#include "ellipse.hxx"
#endif

#if !defined( INTCURVE_CLASS )
#include "intcurve.hxx"
#endif

#if !defined( PCURVE_CLASS )
#include "pcurve.hxx"
#endif

#if !defined( APOINT_CLASS )
#include "point.hxx"
#endif

#if !defined( STRAIGHT_CLASS )
#include "straight.hxx"
#endif

#if !defined( TRANSFORM_CLASS )
#include "transfrm.hxx"
#endif

#if !defined( UNDEFC_CLASS )
#include "undefc.hxx"
#endif

#if !defined( GETBOX_ROUTINES )
#include "getbox.hxx"
#endif

#if !defined( LISTS_HXX )
#include "lists.hxx"
#endif

#if !defined( FILEINFO_CLASS )  // include guard name used in Acis
#include "fileinfo.hxx"
#endif

#if !defined( BODY_CLASS )
#include "body.hxx"
#endif

#if !defined( COEDGE_CLASS )
#include "coedge.hxx"
#endif

#if !defined( EDGE_CLASS )
#include "edge.hxx"
#endif

#if !defined( FACE_CLASS )
#include "face.hxx"
#endif

#if !defined( LOOP_CLASS )
#include "loop.hxx"
#endif

#if !defined( LUMP_CLASS )
#include "lump.hxx"
#endif

#if !defined( SHELL_CLASS )
#include "shell.hxx"
#endif

#if !defined( SUBSHELL_CLASS )
#include "subshell.hxx"
#endif

#if !defined( VERTEX_CLASS )
#include "vertex.hxx"
#endif

#if !defined( WIRE_CLASS )
#include "wire.hxx"
#endif

#if !defined( curve_CLASS )
#include "curdef.hxx"
#endif

#ifndef ACIS_LAW_HXX
#include "law.hxx"
#endif

#if !defined( TENSOR_CLASS )
#include "tensor.hxx"
#endif

#if !defined( SATFILE_H )
#include "satfile.hxx"
#endif

#if !defined( TAGDATA_H )
#include "tagdata.hxx"
#endif

#if !defined( TESTWIRE_HXX )
#include "testwire.hxx"
#endif

#if !defined( COMPCURV_CLASS )
#include "compcurv.hxx"
#endif

#if !defined( ADPCUR_HDR_DEF )
#include "add_pcu.hxx"
#endif

#if !defined( sweep_spl_sur_CLASS )
#include "swp_spl.hxx"
#endif

#ifndef SG_FACE
#include "faceutil.hxx"
#endif

#ifndef Q_VERT_HXX
#include "q_vert.hxx"
#endif

/* Spline (AG) */
#if !defined( SPLINE_API_ROUTINES )
#include "spl_api.hxx"
#endif

#if !defined( SG_BS3_CURVE_ROUTINES )	// include guard name used in Acis
#include "sps3crtn.hxx"
#endif

#if !defined ( CHK_STAT_HXX )
#include "chk_stat.hxx"
#endif

/* Intersectors (INTR) */
#if !defined( INTRAPI_HXX )
#include "intrapi.hxx"
#endif

#if !defined( RAY_TEST )
#include "raytest.hxx"
#endif

/* Faceter (FCT) */
#ifndef FACETER_API_HEADER
#include "af_api.hxx"
#endif

#ifndef _PPM
#include "ppm.hxx"
#endif

#ifndef _MESHMG
#include "meshmg.hxx"
#endif

/* Rendering Base (RBASE) */
#if !defined( RENDERER_API_HEADER_RB )
#include "rnd_api.hxx"
#endif

#ifndef rgb_attr_hxx
#include "rgb_attr.hxx"
#endif

#ifndef col_attr_hxx
#include "col_attr.hxx"
#endif

#include "../../icad/configure.h"
#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT

/* Constructors (CSTR) */
#if !defined( CSTRAPI_HXX )     // include guard name used in Acis
#include "cstrapi.hxx"
#endif

/* Sweeping (SWP) */
#if !defined( SWEEPAPI_HXX )    // include guard name used in Acis
#include "sweepapi.hxx"
#endif

#if !defined( SWEEP_OPTIONS_CLASS ) // include guard name used in Acis
#include "swp_opts.hxx"
#endif

/* Skinning (SKIN) */
#if !defined( SKINAPI_HXX )		// include guard name used in Acis
#include "skinapi.hxx"
#endif

/* Covering (COVR) */
#if !defined( COVERAPI_HXX )	// include guard name used in Acis
#include "coverapi.hxx"
#endif

/* Boolean (BOOL) */
#if !defined( BOOLAPI_HXX )		// include guard name used in Acis
#include "boolapi.hxx"
#endif

/* Local Ops (LOP) */
#if !defined( LOP_HUSK_API_ROUTINES )	// include guard name used in Acis
#include "lop_api.hxx"
#endif

/* Remove Faces (REM) */
#if !defined( REM_HUSK_API_ROUTINES )	// include guard name used in Acis
#include "rem_api.hxx"
#endif __rem_apiHXX__

/* Euler Operations (EULR) */
#if !defined( EULERAPI_HXX )	// include guard name used in Acis
#include "eulerapi.hxx"
#endif

/* Shelling (SHL) */
#if !defined( SHL_HUSK_API_ROUTINES )	// include guard name used in Acis
#include "shl_api.hxx"
#endif

/* Generic Attributes (GA) */
#if !defined( GA_HUSK_API_ROUTINES ) // include guard name used in Acis
#include "ga_api.hxx"
#endif

/* Blending (BLND) */
#if !defined( BLENDAPI_HXX )
#include "blendapi.hxx"
#endif

#if !defined( BLEND_ATTRIB )
#include "bl_att.hxx"
#endif

#ifndef ATTRIB_GEN_POINTER_CLASS // include guard name used in Acis
#include "at_ptr.hxx"
#endif

#endif //#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT

#endif	// _ACISHEADERS_H_
