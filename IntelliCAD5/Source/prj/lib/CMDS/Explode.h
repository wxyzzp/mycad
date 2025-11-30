// EXPLODE command

BOOL
ExplodeLeader(
  sds_name          ename,
  struct resbuf*    elist,
  const int         psflag
);

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

