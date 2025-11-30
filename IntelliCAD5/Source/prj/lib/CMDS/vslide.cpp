// **********************************************
// VSLIDE.CPP
//
// Contains functions for reading/display AutoDesk's SLD format
//
//  This code was originally in two places -- DCL3.CPP and CMDS6.CPP.  Someone had copy/pasted the code
// and each had bug-fixes that weren't reflected in other copy.  The DCL code did not display solids and 
// got colors badly wrong.  The CMDS code didn't scale properly and messed up solids if not in plan view.
// By combining the code in one place I got one copy where everything appears to work.	 The old functions
// now are just wrappers that call this IC_DrawSlideFile in this module
//
//
//
//
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include <assert.h>
#include "IcadApi.h"
#include "DrawDevice.h"


#define SLIDE_MAXSLDRLEN      31
#define SLIDE_TEST_NUMBER     0x1234
#define SLIDE_ACADSLIDE       "AutoCAD Slide"/*DNT*/
#define SLIDE_OFFSETVECTOR    '\xFB'
#define SLIDE_ENDOFFILE       '\xFC'
#define SLIDE_SOLIDFILL       '\xFD'
#define SLIDE_COMMONENDVECTOR '\xFE'
#define SLIDE_NEWCOLOR        '\xFF'
#define SLIDE_LIBEXT          ".slb"/*DNT*/


// ***********************************************
// class SlideRenderer
//
// This local class encapsulates the stuff needed to display
// a slide.
//
//  Mostly it keeps track of GDI objects and insures the HDC
// gets returned to its original state
//
class SlideRenderer 
	{
	// ctors and dtors
	//
	public:
		SlideRenderer( HDC hDC );
		~SlideRenderer( void );

	public:
		bool DrawSlideFile( char *szSlideFileName, RECT *pRect );

	private:
		bool setColor( int nColor );
		bool drawVector(int nXVectStart, int nYVectStart, int nXVectEnd, int nYVectEnd, int nColor);
		bool drawPolygon(POINT *points, int nColor, short vertices);

	// *************
	// Data members
	//
	private:
		// The DC we're drawing to
		//
		HDC		m_hDC;			

		// Keep track of the original state of the HDC
		//
		HPEN	m_hOriginalPen;
		HBRUSH	m_hOriginalBrush;

		int		m_nCurrentColor;
		// As we draw, we'll create and destroy other pens and brushes
		//
		HPEN	m_hCurrentPen;
		HBRUSH	m_hCurrentBrush;
	};


// ****************************
// ctor -- class SlideRenderer
//
// Save state of the HDC
//
SlideRenderer::SlideRenderer( HDC hDC )
	{
	ASSERT( hDC != NULL );

	m_hDC = hDC;
	m_hOriginalPen = (HPEN)::GetCurrentObject( hDC, OBJ_PEN );
	m_hOriginalBrush = (HBRUSH)::GetCurrentObject( hDC, OBJ_BRUSH );
	m_nCurrentColor = -1;			// -1 is never used
	m_hCurrentPen = NULL;
	m_hCurrentBrush = NULL;
	}

// *****************************
// 
// Restore state of the HDC
// also get rid of any pens or brushes we created
//
SlideRenderer::~SlideRenderer( void )
	{

	::SelectObject( m_hDC, m_hOriginalPen );
	::SelectObject( m_hDC, m_hOriginalBrush );

	if ( m_hCurrentPen != NULL )
		{
		::DeleteObject( m_hCurrentPen );
		m_hCurrentPen = NULL;
		}

	if ( m_hCurrentBrush != NULL )
		{
		::DeleteObject( m_hCurrentBrush );
		m_hCurrentBrush = NULL;
		}

	m_hDC = NULL;
	m_hOriginalPen = NULL;
	m_hOriginalBrush = NULL;
	}

bool
SlideRenderer::setColor( int nColor )
	{
	// If the color is not already set to this
	//
	if ( nColor != m_nCurrentColor )
		{
		m_nCurrentColor = nColor;

		// Convert to AutoCAD color
		//
		HBRUSH hBrush =	::CreateSolidBrush( ::SDS_BrushColorFromACADColor( nColor ) );
		HPEN hPen = ::CreatePen(PS_SOLID,1, ::SDS_PenColorFromACADColor( nColor ) );
	
		// Select in the new objects
		//
		::SelectObject(m_hDC,hBrush);
		::SelectObject(m_hDC,hPen);

		// Delete the old objects and save the newly created ones
		//  I deleted them after Select'ing the new one, just
		// to be nice to the DC, rather than Deleting ones it is holding onto
		//
		if ( m_hCurrentPen != NULL )
			{
			::DeleteObject( m_hCurrentPen );
			}
		if ( m_hCurrentBrush != NULL )
			{
			::DeleteObject( m_hCurrentBrush );
			}
		m_hCurrentPen = hPen;
		m_hCurrentBrush = hBrush;

		}
		return true;
	}

bool 
SlideRenderer::drawVector(int nXVectStart, int nYVectStart, int nXVectEnd, int nYVectEnd, int nColor) 
	{

	this->setColor( nColor );

    ::MoveToEx(m_hDC,(int)nXVectStart,(int)nYVectStart,NULL);
    ::LineTo(m_hDC,(int)nXVectEnd,(int)nYVectEnd);
    ::LineTo(m_hDC,(int)nXVectEnd,(int)nYVectEnd+1);

	return true;
	}



bool
SlideRenderer::drawPolygon(POINT *points,int nColor,short vertices) 
	{

	this->setColor( nColor );

	::Polygon(m_hDC,points,vertices); 

	return true;
	}



// *********************************
// DrawSlideFile
//
// this is the core method
//
// Most of this is copy/paste'd from the old DCL3.CPP, with fixes applied from the old CMDS6.CPP
// code.  The combined version now appears to work in all cases, but I didn't clean up everything, so
// some of it is still a little repulsive
//
//
bool
SlideRenderer::DrawSlideFile( char *szSlideFileName, RECT *pRect )
	{
    FILE     *fptr=NULL;      
    char   *szSlName=NULL,
		     *szLibName=NULL;
    sds_real  fr1;
	char     *slnamstr=NULL,
		     *fptr1=NULL,
		     *fptr2=NULL,
              tmpfname[IC_ACADBUF],
              fs1[IC_ACADBUF],
              cvalue[1];
    sds_real  pt[20],
              lst[2];
    short     tvalue[1],
              nverts[1];
    char      String[SLIDE_MAXSLDRLEN*2+1],
             *cp1;
    short     fi1,
              color;
    sds_point ptt[2]={{0.0,0.0,0.0},{0.0,0.0,0.0}};
	short     pix[3][2][2];
    double    asp[2],
    		  ar1,
    		  ar2;
	long      aspect;
	POINT    *points=NULL;
    BOOL      testflag;

	if ((slnamstr= new char [strlen(szSlideFileName)+1] )==NULL) 
		{
		goto err;
		}
	strcpy(slnamstr,szSlideFileName);

	if ((fptr1=strchr(slnamstr,'(')) && (fptr2=strchr(slnamstr,')'))) {
		*fptr1=0;
        //*** Get the pathname for the slide library
        strncpy(tmpfname,slnamstr,sizeof(tmpfname)-1);
		strcat(tmpfname,SLIDE_LIBEXT);
    	if((sds_findfile(tmpfname,fs1))!=SDS_RTNORM) goto err;

		if ((szLibName= new char [strlen(fs1)+1] )==NULL) goto err;
		strncpy(szLibName,fs1,strlen(fs1)+1);

		*fptr2=0;
		fptr1=fptr1+1;
		if (fptr1) {
			if ((szSlName= new char [strlen(fptr1)+1] )==NULL) goto err;
			strncpy(szSlName,fptr1,strlen(fptr1)+1);
		} else goto err;

    	// Open slide file.
	   	if ((fptr=fopen(szLibName,"rb"/*DNT*/))==NULL) goto err;

		fseek(fptr,32L,SEEK_SET);
		while (aspect=fread(String,1,36,fptr)) {
			if (aspect<36 && !feof(fptr)) goto err;
			if (strisame(String,szSlName)) {
				memcpy(&aspect,String+32,4);
				break;
			}
		}
		if (feof(fptr)) goto err;
		fseek(fptr,aspect,SEEK_SET);
	} else {
	   	if ((szSlName= new char [ strlen(slnamstr)+1] )==NULL) goto err;
    	strncpy(szSlName,slnamstr,strlen(slnamstr)+1);

		if(ic_chkext(szSlName,"sld"/*DNT*/)) {
			if ((szSlName=ic_realloc_char(szSlName,
				(strlen(szSlName))+(strlen(".sld"/*DNT*/))+1))==NULL) goto err;
			strcat(szSlName,".sld"/*DNT*/);
		}

		// Open slide file.
		//2001-8-5 EBATECH(FUTA) search path -[
		//if ((fptr=fopen(szSlName,"rb"/*DNT*/))==NULL) goto err;
		*tmpfname=0;
		if((sds_findfile(szSlName,tmpfname))!=SDS_RTNORM) goto err;
		if ((fptr=fopen(tmpfname,"rb"/*DNT*/))==NULL) goto err;
		// ]-
	
	}

	if (fread(String,1,SLIDE_MAXSLDRLEN,fptr)!=SLIDE_MAXSLDRLEN  || strnicmp(String,SLIDE_ACADSLIDE,strlen(SLIDE_ACADSLIDE)) || String[18]<1 || String[18]>2) {
	    goto err;
    }

    // Get the test number.
    cp1=(char *)(&fi1); *(cp1)=*((char *)(String+29));  *(cp1+1)=*((char *)(String+30));
    testflag=(SLIDE_TEST_NUMBER==fi1)?FALSE:TRUE;

    if (testflag) {
        cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+19));  *(cp1)=*((char *)(String+20));
    } else {
    	fi1=*((short *)(String+19));
    }
    ar1=fi1;
	// get the height in pix.
    if (testflag) {
        cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+21));  *(cp1)=*((char *)(String+22));
    } else {
	    fi1=*((short *)(String+21));
    }
    ar2=fi1;
	// get the aspect ratio. always written with the least significant byte first.
  	aspect=*((long *)(String+23));

    /*
    **  Set up these source and dest screen parameters:
    **
    **   (Meaning: pix[source/dest/working][LL/UR][X/Y])
    **
    **   pix[0][0][0] : Lower left  X for source window (pixels)
    **   pix[0][0][1] : Lower left  Y for source window (pixels)
    **   pix[0][1][0] : Upper right X for source window (pixels)
    **   pix[0][1][1] : Upper right Y for source window (pixels)
    **   pix[1][0][0] : Lower left  X for dest   window (pixels)
    **   pix[1][0][1] : Lower left  Y for dest   window (pixels)
    **   pix[1][1][0] : Upper right X for dest   window (pixels)
    **   pix[1][1][1] : Upper right Y for dest   window (pixels)
    **  asp[0] : Aspect ratio  for source (>0.0)
    **  asp[1] : Aspect ratio  for dest   (>0.0)
    */

	pix[0][0][0]=0;
	pix[0][0][1]=0;
	pix[0][1][0]=(short)ar1;
	pix[0][1][1]=(short)ar2;
	pix[1][0][0]=(short)pRect->left;
	pix[1][0][1]=(short)pRect->bottom;
	pix[1][1][0]=(short)pRect->right;
	pix[1][1][1]=(short)pRect->top;

    // Calculate the aspect ratio.
    asp[0]=ar1/ar2;
	asp[1]=(double)pRect->right/(double)pRect->bottom;

    //  Determine pixel coords of the lower left and upper right
    //  corners of the target rectangle in the dest window:
    //
    if (asp[1]>asp[0]) {  // Top and bottom will touch
        fr1=(0.5*(1.0-asp[0]/asp[1])*((double)(pix[1][1][0]-pix[1][0][0])));
        pix[2][0][0]=pix[1][0][0]+(short)fr1; pix[2][0][1]=pix[1][0][1];
        pix[2][1][0]=pix[1][1][0]-(short)fr1; pix[2][1][1]=pix[1][1][1];
    } else {  // Sides will touch 
        fr1=(0.5*(1.0-asp[1]/asp[0])*((double)(pix[1][1][1]-pix[1][0][1])));
        pix[2][0][0]=pix[1][0][0]; pix[2][0][1]=pix[1][0][1]+(short)fr1;
        pix[2][1][0]=pix[1][1][0]; pix[2][1][1]=pix[1][1][1]-(short)fr1;
    }

    // Set the interpolation factors:
    ar1=((double)(pix[2][1][0]-pix[2][0][0]))/((double)(pix[0][1][0]-pix[0][0][0]));
    ar2=((double)(pix[2][1][1]-pix[2][0][1]))/((double)(pix[0][1][1]-pix[0][0][1]));

    for(;;) {
        // Read (2) bytes in.
        if (fread(String,1,2,fptr)!=2) goto err;
        switch(String[(testflag)?0:1]) {
            case SLIDE_OFFSETVECTOR:
                if (fread(String+2,1,3,fptr)!=3) goto err;
                memcpy(cvalue,String+((testflag)?1:0),sizeof(char));
                pt[0]=lst[0]+(sds_real)cvalue[0];
                memcpy(cvalue,String+2,sizeof(char));
                pt[1]=lst[1]+(sds_real)cvalue[0];
                memcpy(cvalue,String+3,sizeof(char));
                pt[2]=lst[0]+(sds_real)cvalue[0];
                memcpy(cvalue,String+4,sizeof(char));
                pt[3]=lst[1]+(sds_real)cvalue[0];

				// save last point
                lst[0]=pt[0]; lst[1]=pt[1];

			    pt[0]=(((double)pix[2][0][0])+ar1*((double)(pt[0]-pix[0][0][0])));  /* X */
			    pt[1]=(((double)pix[2][0][1])+ar2*((double)(pt[1]-pix[0][0][1])));  /* Y */
			    pt[2]=(((double)pix[2][0][0])+ar1*((double)(pt[2]-pix[0][0][0])));  /* X */
			    pt[3]=(((double)pix[2][0][1])+ar2*((double)(pt[3]-pix[0][0][1])));  /* Y */

				this->drawVector((short)pt[0],(short)pt[1],(short)pt[2],(short)pt[3],color);

                break;
            case SLIDE_ENDOFFILE:
                fclose(fptr);
                goto err;
            case SLIDE_SOLIDFILL:
                if (fread(String,1,4,fptr)!=4)
					{
					ASSERT(false);
					goto err;
					}

                if (testflag) {
                    cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+1)); *(cp1)=*((char *)(String));
                    nverts[0]=(short)cp1;
                } else {
                    memcpy(nverts,String,sizeof(short)); 
                }

                if (testflag) {
                    cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+3)); *(cp1)=*((char *)(String+2));
                    tvalue[0]=(short)cp1;
                } else {
                    memcpy(tvalue,String+2,sizeof(short));
                }

                 if (nverts[0]>1 && (2*nverts[0])<=sizeof(pt)/sizeof(pt[0]) && tvalue[0]<0) {
                     if ((points= new POINT [nverts[0]] )==NULL) 
						 {
						 ASSERT(false);
						 goto err;
						 }
					 memset(points,0,sizeof(POINT)*nverts[0]);
                     for(fi1=0;fi1<nverts[0];++fi1) {
                        if (fread(String,1,2,fptr)!=2 || String[(testflag)?0:1]!=SLIDE_SOLIDFILL || fread(String,1,4,fptr)!=4) goto err;

                        if (testflag) {
                            cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+1)); *(cp1)=*((char *)(String));
                            tvalue[0]=(short)cp1;
                        } else {
                            memcpy(tvalue,String,sizeof(short));
                        }
                        pt[fi1*2]=tvalue[0];

                        if (testflag) {
                            cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+1)); *(cp1)=*((char *)(String));
                            tvalue[0]=(short)cp1;
                        } else {
                            memcpy(tvalue,String+2,sizeof(short));
                        }
                        pt[fi1*2+1]=tvalue[0];

                        if (pt[fi1*2+1]<0) 
							{
								// negative y-coordinate should only be at the end of the flood sequence
							    if (fi1 != nverts[0] - 1) 
									{
									ASSERT(false);
									goto err;
									}
							    // ignore this last point
							    nverts[0]--;
                            }


			  		     pt[fi1*2]  =(((double)pix[2][0][0])+ar1*((double)(pt[fi1*2]-pix[0][0][0])));    // X 
			 		     pt[fi1*2+1]=(((double)pix[2][0][1])+ar2*((double)(pt[fi1*2+1]-pix[0][0][1])));  // Y 
			 			 points[fi1].x=(long)pt[fi1*2];
			 			 points[fi1].y=(long)pt[fi1*2+1];
                     }


                 	 this->drawPolygon(points,color,(short)*nverts);
    			     IC_FREE(points); points=NULL;
                 }
                 break;
            case SLIDE_COMMONENDVECTOR:
                if (fread(String+2,1,1,fptr)!=1) goto err;
                pt[0]=lst[0]; pt[1]=lst[1];

				// save last point.
                memcpy(cvalue,String+((testflag)?1:0),sizeof(char));
				lst[0]=pt[2]=pt[0]+(sds_real)cvalue[0];
                memcpy(cvalue,String+2,sizeof(char));
				lst[1]=pt[3]=pt[1]+(sds_real)cvalue[0];

			    pt[0]=(((double)pix[2][0][0])+ar1*((double)(pt[0]-pix[0][0][0])));  /* X */
			    pt[1]=(((double)pix[2][0][1])+ar2*((double)(pt[1]-pix[0][0][1])));  /* Y */
			    pt[2]=(((double)pix[2][0][0])+ar1*((double)(pt[2]-pix[0][0][0])));  /* X */
			    pt[3]=(((double)pix[2][0][1])+ar2*((double)(pt[3]-pix[0][0][1])));  /* Y */

                this->drawVector((short)pt[0],(short)pt[1],(short)pt[2],(short)pt[3],color);
                break;
            case SLIDE_NEWCOLOR:
                // this is the color stuff.
                color=String[(testflag)?1:0];
                if (color<0)
					{
					color=256+color;
					}
                if (color>255 || color<0) 
					{
					color=7;
					}
                break;
            default:
                if (String[(testflag)?0:1]<0) goto err;

                if (testflag) {
                    memcpy((char *)tvalue,String+1,sizeof(char));
                    memcpy((char *)tvalue+1,String,sizeof(char));
                } else {
                    memcpy(tvalue,String,sizeof(short));
                }

                pt[0]=(sds_real)tvalue[0];
                for(fi1=1;fi1<4;++fi1) {
                    if (fread(String,1,2,fptr)!=2) goto err;

                    if (testflag) {
                        memcpy((char *)tvalue,String+1,sizeof(char));
                        memcpy((char *)tvalue+1,String,sizeof(char));
                    } else {
                        memcpy(tvalue,String,sizeof(short));
                    }

                    pt[fi1]=(sds_real)tvalue[0];
                }

                // save last point.
                lst[0]=pt[0]; lst[1]=pt[1];

			    pt[0]=(((double)pix[2][0][0])+ar1*((double)(pt[0]-pix[0][0][0])));  /* X */
			    pt[1]=(((double)pix[2][0][1])+ar2*((double)(pt[1]-pix[0][0][1])));  /* Y */
			    pt[2]=(((double)pix[2][0][0])+ar1*((double)(pt[2]-pix[0][0][0])));  /* X */
			    pt[3]=(((double)pix[2][0][1])+ar2*((double)(pt[3]-pix[0][0][1])));  /* Y */

                ptt[0][0]=pt[0]; ptt[0][1]=pt[1];
                ptt[1][0]=pt[2]; ptt[1][1]=pt[3];

                this->drawVector((short)pt[0],(short)pt[1],(short)pt[2],(short)pt[3],color);
                break;
        }
    }


	if (slnamstr)   IC_FREE(slnamstr);
	if (szSlName)  IC_FREE(szSlName);
	if (szLibName) IC_FREE(szLibName);
    if (fptr)       fclose(fptr);
	if (points)     IC_FREE(points);
	return true;
	err:

		if (slnamstr)   IC_FREE(slnamstr);
		if (szSlName)  IC_FREE(szSlName);
		if (szLibName) IC_FREE(szLibName);		
		if (fptr)       fclose(fptr);
		if (points)     IC_FREE(points);
	return false;
}



// ****************************************************************
// PUBLIC FUNCTION 
//
// IC_DrawSlideFile
//
// This is the entry point for this entire module
//
//
int
IC_DrawSlideFile( char *szSlideFileName, HDC hDC, RECT *pRect )
	{
	RT rc = RTERROR;

	SlideRenderer theRenderer( hDC );

	if (theRenderer.DrawSlideFile( szSlideFileName, pRect ))
		{
		rc = RTNORM;
		}

	return rc;
	}

int cmds_drawSlideFile(char *szSlideFileName, CDrawDevice* pDrawDevice, RECT* pRect)
{
    FILE     *fptr=NULL;      
    char   *szSlName=NULL,
		     *szLibName=NULL;
    sds_real  fr1;
	char     *slnamstr=NULL,
		     *fptr1=NULL,
		     *fptr2=NULL,
              tmpfname[IC_ACADBUF],
              fs1[IC_ACADBUF],
              cvalue[1];
    sds_real  pt[20],
              lst[2];
    short     tvalue[1],
              nverts[1];
    char      String[SLIDE_MAXSLDRLEN*2+1],
             *cp1;
    short     fi1,
              color;
    sds_point ptt[2]={{0.0,0.0,0.0},{0.0,0.0,0.0}};
	short     pix[3][2][2];
    double    asp[2],
    		  ar1,
    		  ar2;
	long      aspect;
	POINT    *points=NULL, line[2];
    BOOL      testflag;

	if ((slnamstr= new char [strlen(szSlideFileName)+1] )==NULL) 
		goto err;
	strcpy(slnamstr,szSlideFileName);

	if ((fptr1=strchr(slnamstr,'(')) && (fptr2=strchr(slnamstr,')')))
	{
		*fptr1=0;
        //*** Get the pathname for the slide library
        strncpy(tmpfname,slnamstr,sizeof(tmpfname)-1);
		strcat(tmpfname,SLIDE_LIBEXT);
    	if((sds_findfile(tmpfname,fs1))!=SDS_RTNORM)
			goto err;

		if ((szLibName= new char [strlen(fs1)+1] )==NULL)
			goto err;
		strncpy(szLibName,fs1,strlen(fs1)+1);

		*fptr2=0;
		fptr1=fptr1+1;
		if (fptr1)
		{
			if ((szSlName= new char [strlen(fptr1)+1] )==NULL)
				goto err;
			strncpy(szSlName,fptr1,strlen(fptr1)+1);
		} 
		else
			goto err;

    	// Open slide file.
	   	if ((fptr=fopen(szLibName,"rb"/*DNT*/))==NULL)
			goto err;

		fseek(fptr,32L,SEEK_SET);
		while (aspect=fread(String,1,36,fptr))
		{
			if (aspect<36 && !feof(fptr))
				goto err;
			if (strisame(String,szSlName))
			{
				memcpy(&aspect,String+32,4);
				break;
			}
		}
		if (feof(fptr))
			goto err;
		fseek(fptr,aspect,SEEK_SET);
	}
	else
	{
	   	if ((szSlName= new char [strlen(slnamstr)+1] )==NULL)
			goto err;
    	strncpy(szSlName,slnamstr,strlen(slnamstr)+1);

		if(ic_chkext(szSlName,"sld"/*DNT*/))
		{
			if ((szSlName=ic_realloc_char(szSlName,
				(strlen(szSlName))+(strlen(".sld"/*DNT*/))+1))==NULL)
				goto err;
			strcat(szSlName,".sld"/*DNT*/);
		}

		// Open slide file.
		//2001-8-5 EBATECH(FUTA) search path -[
		//if ((fptr=fopen(szSlName,"rb"/*DNT*/))==NULL) goto err;
		*tmpfname=0;
		if((sds_findfile(szSlName,tmpfname))!=SDS_RTNORM)
			goto err;
		if ((fptr=fopen(tmpfname,"rb"/*DNT*/))==NULL)
			goto err;
		// ]-
	
	}

	if (fread(String,1,SLIDE_MAXSLDRLEN,fptr)!=SLIDE_MAXSLDRLEN  || strnicmp(String,SLIDE_ACADSLIDE,strlen(SLIDE_ACADSLIDE)) || String[18]<1 || String[18]>2)
	    goto err;

    // Get the test number.
    cp1=(char *)(&fi1);
	*(cp1)=*((char *)(String+29));
	*(cp1+1)=*((char *)(String+30));
    testflag=(SLIDE_TEST_NUMBER==fi1)?FALSE:TRUE;

    if (testflag)
	{
        cp1=(char *)(&fi1);
		*(cp1+1)=*((char *)(String+19));
		*(cp1)=*((char *)(String+20));
    }
	else
	{
    	fi1=*((short *)(String+19));
    }
    ar1=fi1;
	// get the height in pix.
    if (testflag)
	{
        cp1=(char *)(&fi1);
		*(cp1+1)=*((char *)(String+21));
		*(cp1)=*((char *)(String+22));
    }
	else
	{
	    fi1=*((short *)(String+21));
    }
    ar2=fi1;
	// get the aspect ratio. always written with the least significant byte first.
  	aspect=*((long *)(String+23));

    /*
    **  Set up these source and dest screen parameters:
    **
    **   (Meaning: pix[source/dest/working][LL/UR][X/Y])
    **
    **   pix[0][0][0] : Lower left  X for source window (pixels)
    **   pix[0][0][1] : Lower left  Y for source window (pixels)
    **   pix[0][1][0] : Upper right X for source window (pixels)
    **   pix[0][1][1] : Upper right Y for source window (pixels)
    **   pix[1][0][0] : Lower left  X for dest   window (pixels)
    **   pix[1][0][1] : Lower left  Y for dest   window (pixels)
    **   pix[1][1][0] : Upper right X for dest   window (pixels)
    **   pix[1][1][1] : Upper right Y for dest   window (pixels)
    **  asp[0] : Aspect ratio  for source (>0.0)
    **  asp[1] : Aspect ratio  for dest   (>0.0)
    */

	pix[0][0][0]=0;
	pix[0][0][1]=0;
	pix[0][1][0]=(short)ar1;
	pix[0][1][1]=(short)ar2;
	pix[1][0][0]=(short)pRect->left;
	pix[1][0][1]=(short)pRect->bottom;
	pix[1][1][0]=(short)pRect->right;
	pix[1][1][1]=(short)pRect->top;

    // Calculate the aspect ratio.
    asp[0]=ar1/ar2;
	asp[1]=(double)pRect->right/(double)pRect->bottom;

    //  Determine pixel coords of the lower left and upper right
    //  corners of the target rectangle in the dest window:
    //
    if (asp[1]>asp[0])
	{  // Top and bottom will touch
        fr1=(0.5*(1.0-asp[0]/asp[1])*((double)(pix[1][1][0]-pix[1][0][0])));
        pix[2][0][0]=pix[1][0][0]+(short)fr1; pix[2][0][1]=pix[1][0][1];
        pix[2][1][0]=pix[1][1][0]-(short)fr1; pix[2][1][1]=pix[1][1][1];
    }
	else
	{  // Sides will touch 
        fr1=(0.5*(1.0-asp[1]/asp[0])*((double)(pix[1][1][1]-pix[1][0][1])));
        pix[2][0][0]=pix[1][0][0]; pix[2][0][1]=pix[1][0][1]+(short)fr1;
        pix[2][1][0]=pix[1][1][0]; pix[2][1][1]=pix[1][1][1]-(short)fr1;
    }

    // Set the interpolation factors:
    ar1=((double)(pix[2][1][0]-pix[2][0][0]))/((double)(pix[0][1][0]-pix[0][0][0]));
    ar2=((double)(pix[2][1][1]-pix[2][0][1]))/((double)(pix[0][1][1]-pix[0][0][1]));

    for(;;)
	{
        // Read (2) bytes in.
        if(fread(String,1,2,fptr)!=2)
			goto err;
        switch(String[(testflag)?0:1])
		{
            case SLIDE_OFFSETVECTOR:
                if (fread(String+2,1,3,fptr)!=3)
					goto err;
                memcpy(cvalue,String+((testflag)?1:0),sizeof(char));
                pt[0]=lst[0]+(sds_real)cvalue[0];
                memcpy(cvalue,String+2,sizeof(char));
                pt[1]=lst[1]+(sds_real)cvalue[0];
                memcpy(cvalue,String+3,sizeof(char));
                pt[2]=lst[0]+(sds_real)cvalue[0];
                memcpy(cvalue,String+4,sizeof(char));
                pt[3]=lst[1]+(sds_real)cvalue[0];

				// save last point
                lst[0]=pt[0]; lst[1]=pt[1];

			    pt[0]=(((double)pix[2][0][0])+ar1*((double)(pt[0]-pix[0][0][0])));  /* X */
			    pt[1]=(((double)pix[2][0][1])+ar2*((double)(pt[1]-pix[0][0][1])));  /* Y */
			    pt[2]=(((double)pix[2][0][0])+ar1*((double)(pt[2]-pix[0][0][0])));  /* X */
			    pt[3]=(((double)pix[2][0][1])+ar2*((double)(pt[3]-pix[0][0][1])));  /* Y */

				//this->drawVector((short)pt[0],(short)pt[1],(short)pt[2],(short)pt[3],color);
				line[0].x = (LONG)pt[0];
				line[0].y = (LONG)pt[1];
				line[1].x = (LONG)pt[2];
				line[1].y = (LONG)pt[3];
				pDrawDevice->Polyline(line, 2);
                break;
            case SLIDE_ENDOFFILE:
                fclose(fptr);
                goto err;
            case SLIDE_SOLIDFILL:
                if (fread(String,1,4,fptr)!=4)
				{
					ASSERT(false);
					goto err;
				}

                if (testflag)
				{
                    cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+1)); *(cp1)=*((char *)(String));
                    nverts[0]=(short)cp1;
                }
				else
				{
                    memcpy(nverts,String,sizeof(short)); 
                }

                if (testflag)
				{
                    cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+3)); *(cp1)=*((char *)(String+2));
                    tvalue[0]=(short)cp1;
                }
				else
				{
                    memcpy(tvalue,String+2,sizeof(short));
                }

                if(nverts[0]>1 && (2*nverts[0])<=sizeof(pt)/sizeof(pt[0]) && tvalue[0]<0)
				{
					if ((points= new POINT [nverts[0]] )==NULL)
					{
						ASSERT(false);
						goto err;
					}
					memset(points,0,sizeof(POINT)*nverts[0]);
                    for(fi1=0;fi1<nverts[0];++fi1)
					{
                        if (fread(String,1,2,fptr)!=2 || String[(testflag)?0:1]!=SLIDE_SOLIDFILL || fread(String,1,4,fptr)!=4)
							goto err;

                        if (testflag)
						{
                            cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+1)); *(cp1)=*((char *)(String));
                            tvalue[0]=(short)cp1;
                        }
						else
						{
                            memcpy(tvalue,String,sizeof(short));
                        }
                        pt[fi1*2]=tvalue[0];

                        if (testflag)
						{
                            cp1=(char *)(&fi1); *(cp1+1)=*((char *)(String+1)); *(cp1)=*((char *)(String));
                            tvalue[0]=(short)cp1;
                        }
						else
						{
                            memcpy(tvalue,String+2,sizeof(short));
                        }
                        pt[fi1*2+1]=tvalue[0];

                        if(pt[fi1*2+1]<0) 
						{
							// negative y-coordinate should only be at the end of the flood sequence
							if (fi1 != nverts[0] - 1) 
							{
								ASSERT(false);
								goto err;
							}
							// ignore this last point
							nverts[0]--;
                        }


			  		    pt[fi1*2]  =(((double)pix[2][0][0])+ar1*((double)(pt[fi1*2]-pix[0][0][0])));    // X 
			 		    pt[fi1*2+1]=(((double)pix[2][0][1])+ar2*((double)(pt[fi1*2+1]-pix[0][0][1])));  // Y 
			 			points[fi1].x=(long)pt[fi1*2];
			 			points[fi1].y=(long)pt[fi1*2+1];
                    }


                 	//this->drawPolygon(points,color,(short)*nverts);
					pDrawDevice->Polygon(points, *nverts, true);
	   			    IC_FREE(points);
					points=NULL;
                }
                break;
            case SLIDE_COMMONENDVECTOR:
                if (fread(String+2,1,1,fptr)!=1)
					goto err;
                pt[0]=lst[0];
				pt[1]=lst[1];

				// save last point.
                memcpy(cvalue,String+((testflag)?1:0),sizeof(char));
				lst[0]=pt[2]=pt[0]+(sds_real)cvalue[0];
                memcpy(cvalue,String+2,sizeof(char));
				lst[1]=pt[3]=pt[1]+(sds_real)cvalue[0];

			    pt[0]=(((double)pix[2][0][0])+ar1*((double)(pt[0]-pix[0][0][0])));  /* X */
			    pt[1]=(((double)pix[2][0][1])+ar2*((double)(pt[1]-pix[0][0][1])));  /* Y */
			    pt[2]=(((double)pix[2][0][0])+ar1*((double)(pt[2]-pix[0][0][0])));  /* X */
			    pt[3]=(((double)pix[2][0][1])+ar2*((double)(pt[3]-pix[0][0][1])));  /* Y */

                //this->drawVector((short)pt[0],(short)pt[1],(short)pt[2],(short)pt[3],color);
				line[0].x = (LONG)pt[0];
				line[0].y = (LONG)pt[1];
				line[1].x = (LONG)pt[2];
				line[1].y = (LONG)pt[3];
				pDrawDevice->Polyline(line, 2);
                break;
            case SLIDE_NEWCOLOR:
                // this is the color stuff.
                color=String[(testflag)?1:0];
                if (color<0)
				{
					color=256+color;
				}
                if (color>255 || color<0) 
				{
					color=7;
				}
				pDrawDevice->SetLineColorACAD(color);
				pDrawDevice->SetFillColorACAD(color);
                break;
            default:
                if (String[(testflag)?0:1]<0)
					goto err;

                if (testflag)
				{
                    memcpy((char *)tvalue,String+1,sizeof(char));
                    memcpy((char *)tvalue+1,String,sizeof(char));
                }
				else
				{
                    memcpy(tvalue,String,sizeof(short));
                }

                pt[0]=(sds_real)tvalue[0];
                for(fi1=1;fi1<4;++fi1)
				{
                    if (fread(String,1,2,fptr)!=2)
						goto err;

                    if (testflag)
					{
                        memcpy((char *)tvalue,String+1,sizeof(char));
                        memcpy((char *)tvalue+1,String,sizeof(char));
                    }
					else
					{
                        memcpy(tvalue,String,sizeof(short));
                    }

                    pt[fi1]=(sds_real)tvalue[0];
                }

                // save last point.
                lst[0]=pt[0];
				lst[1]=pt[1];

			    pt[0]=(((double)pix[2][0][0])+ar1*((double)(pt[0]-pix[0][0][0])));  /* X */
			    pt[1]=(((double)pix[2][0][1])+ar2*((double)(pt[1]-pix[0][0][1])));  /* Y */
			    pt[2]=(((double)pix[2][0][0])+ar1*((double)(pt[2]-pix[0][0][0])));  /* X */
			    pt[3]=(((double)pix[2][0][1])+ar2*((double)(pt[3]-pix[0][0][1])));  /* Y */

                ptt[0][0]=pt[0]; ptt[0][1]=pt[1];
                ptt[1][0]=pt[2]; ptt[1][1]=pt[3];

                //this->drawVector((short)pt[0],(short)pt[1],(short)pt[2],(short)pt[3],color);
				line[0].x = (LONG)pt[0];
				line[0].y = (LONG)pt[1];
				line[1].x = (LONG)pt[2];
				line[1].y = (LONG)pt[3];
				pDrawDevice->Polyline(line, 2);
                break;
        }
    }


	if (slnamstr)
		IC_FREE(slnamstr);
	if (szSlName)
		IC_FREE(szSlName);
	if (szLibName)
		IC_FREE(szLibName);
    if (fptr)
		fclose(fptr);
	if (points)
		IC_FREE(points);
	return true;
err:
	if (slnamstr)
		IC_FREE(slnamstr);
	if (szSlName)
		IC_FREE(szSlName);
	if (szLibName)
		IC_FREE(szLibName);		
	if (fptr)
		fclose(fptr);
	if (points)
		IC_FREE(points);
	return false;
}



