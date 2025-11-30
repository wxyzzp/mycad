// File: <DevDir>/source/prj/lib/drw/odtver.h
//
// /*DG - 28.5.2002*/
// This file contain defines which are necessary for correct relations with
// various versions of the OpenDWG Toolkit.
// So, if you need conditional compiling depending on the version of ODT,
// then use these defines or create needed ones if they don't exist.
// Notes:
// - don't rely on any defs in ODT's ad2.h itself;
// - there was the BUILD_WITH_ODT2008GLE macro defined previously
//   in icad/configure.h, this decision (in odtver.h) replaces that one.

#pragma once

#define ODTVER_2_007
#define ODTVER_2_008
#define ODTVER_2_009
