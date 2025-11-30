#include "OdPlatform.h"

#ifdef ODA_MAC_OSX    /////////////////////////////////// MAC

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#ifdef DD_USING_GLUT
#include <GLUT/glut.h>
#endif

#define DD_STD_CALL 

#elif defined ODA_SGI  ///////////////////////////////// SGI

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef DD_USING_GLUT
#include <GL/glut.h>
#endif

#define DD_STD_CALL 

#else                  ////////////////////////////////// Windows

#include <gl/gl.h>
#include <gl/glu.h>

#ifdef DD_USING_GLUT
#include <glut.h>
#endif

#define DD_STD_CALL __stdcall

#endif                 ////////////////////////////////// END





