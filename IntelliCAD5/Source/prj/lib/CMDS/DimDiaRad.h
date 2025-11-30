// header file for radial dimension


//////////////////////////////////////////////////////////////////////
// for dimension line in block
class CDimLine
{
public:
  // constructor
  CDimLine()
    {
      // line for DIMSD1
      showSd1 = TRUE;
      sd1S[0] = sd1S[1] = sd1S[2] = 0;
      sd1E[0] = sd1E[1] = sd1E[2] = 0;

      // line for DIMSD2
      showSd2 = TRUE;
      sd2S[0] = sd2S[1] = sd2S[2] = 0;
      sd2E[0] = sd2E[1] = sd2E[2] = 0;

      // line for outside
      showOut = FALSE;
      outS[0] = outS[1] = outS[2] = 0;
      outE[0] = outE[1] = outE[2] = 0;

      // line for leader
      showLdr = FALSE;
      ldrS[0] = ldrS[1] = ldrS[2] = 0;
      ldrE[0] = ldrE[1] = ldrE[2] = 0;

      // extnded line for outside
      showExt = FALSE;
      extS[0] = extS[1] = extS[2] = 0;
      extE[0] = extE[1] = extE[2] = 0;

      // center line
      showTofl = FALSE;
      toflS[0] = toflS[1] = toflS[2] = 0;
      toflE[0] = toflE[1] = toflE[2] = 0;

      // arrow position
      showArw1 = TRUE;
      arw1[0] = arw1[1] = arw1[2] = 0;
      showArw2 = TRUE;
      arw2[0] = arw2[1] = arw2[2] = 0;

      // text rotation
      rot = 0.0,

      // flag for arrow fit
      arrow = TRUE;

      // flag for text fit
      text  = TRUE;
    };

public:
  BOOL      showSd1;
  sds_point sd1S;
  sds_point sd1E;

  BOOL      showSd2;
  sds_point sd2S;
  sds_point sd2E;

  BOOL      showOut;
  sds_point outS;
  sds_point outE;

  BOOL      showLdr;
  sds_point ldrS;
  sds_point ldrE;

  BOOL      showExt;
  sds_point extS;
  sds_point extE;

  BOOL      showTofl;
  sds_point toflS;
  sds_point toflE;

  BOOL      showArw1;
  sds_point arw1;
  BOOL      showArw2;
  sds_point arw2;

  BOOL      arrow;
  BOOL      text;

  double    rot;
};

//////////////////////////////////////////////////////////////////////
// prototype

// If angle is left side
BOOL
IsLeftSide(
  double    angle
);

// If angle is over 15 degree, the leader has hook line.
BOOL
HasHook(
  double    angle
);

// Get text rotation
double
GetTextRotation(
  const int     iFlg,
  const double  angle
);

// move up text position
double
GetUpperDimgap(
);

// draw center mark or line according to DIMTOFL
BOOL
DrawCenterMark(
  CDimLine&     dl,
  db_drawing*   flp,
  const BOOL    bIsDia,
  short     dragmode
);

// if angle is horizontal
BOOL
IsHorizontal(
  const double  angle
);

// Get a near edge point of arc
BOOL
GetNearEdgePoint(
  double&   angle
);

BOOL
DrawOppositeDimRad(
  const double  angle,
  sds_point     txtbox[],
  CDimLine&     dl
);

BOOL
cmd_DimRadExLeader(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  const BOOL bIsDia,
  short     dragmode
);

void CreateDimBlock(
  db_drawing*   flp
);

//////////////////////////////////////////////////////////////////////
// Radial dimensions
BOOL
cmd_Diam_Radius_Rad(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
);

BOOL
cmd_DimRad0(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
);

BOOL
cmd_DimRad3(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
);

BOOL
cmd_DimRad4(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
);

BOOL
cmd_DimRad5(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
);
