// File :
// Author: Alexey Malov
#pragma once
#ifndef _ASSNAPING_H_
#define _ASSNAPING_H_

#include "xstd.h"
#include "Modeler.h"//snaping

int getEndSnapPoints
(
BODY* pBody,
xstd<snaping>::vector& pnts
);

int getMidSnapPoints
(
EDGE* pEdge,
xstd<snaping>::vector& pnts
);

int getCenterSnapPoints
(
EDGE* pEdge,
xstd<snaping>::vector& pnts
);

int getQuadrantSnapPoints
(
EDGE* pEdge,
xstd<snaping>::vector& pnts
);

int getNearSnapPoints
(
bounded_curve* crv,
const SPAposition& pick,
const SPAunit_vector& dir,
xstd<snaping>::vector& pnts
);

int getPerpSnapPoints
(
bounded_curve* crv,
const SPAposition& base,
xstd<snaping>::vector& pnts
);

int getTangSnapPoints
(
EDGE* pEdge,
const SPAposition& base,
xstd<snaping>::vector& pnts
);

int getTangentPointsOnEllipse
(
EDGE* pEdge,
const SPAposition& base,
xstd<snaping>::vector& pnts
);

int getTangentPointsOnSpline
(
EDGE* pEdge,
const SPAposition& base,
xstd<snaping>::vector& pnts
);









#endif
