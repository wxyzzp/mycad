
	/* C:\DEV\PRJ\ICAD\ICADDRAWAPI.CPP
	 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
	 *
	 * Abstract
	 *
	 * Moved out of ICADAPI.CPP functions having to do with drawing
	 * Also see ICADDRAW.CPP which contains entity drawing functions
	 *
	 */

	// ** Includes
#include "Icad.h"	/* DNT */
#include "IcadDoc.h"
#include "drawdevice.h"	/* DNT */
#include "vxtabrec.h"
#include "IcadView.h"
#include "IcadEntityRenderer.h"

	#ifndef _ICADPRINTDIA_H
		#include "IcadPrintDia.h"	/* DNT */
	#endif
	#include "lisp.h"			/* DNT */
	#include "cmds.h"			/* DNT */

	#include "SyncObject.h"

	#if defined(USE_SMARTHEAP)
		#include "smrtheap.h"	/* DNT */
	#endif


	extern bool SDS_DontBitBlt;


	#define MAKE556WORD(r,g,b) ((((WORD)(r) >> 3) << 11) | (((WORD)(g) >> 3) << 6)	| ((WORD)(b) >> 2));
	#define MAKE565WORD(r,g,b) ((((WORD)(r) >> 3) << 11) | (((WORD)(g) >> 2) << 5)	| ((WORD)(b) >> 3));
	#define MAKE555WORD(r,g,b) ((((WORD)(r) >> 3) << 10) | (((WORD)(g) >> 3) << 5)	| ((WORD)(b) >> 3));

	#define MAKE565RGB(w,r,g,b) (r = (w & 0xF800) >> 8, g = (w & 0x07E0) >> 3, b = (w & 0x001F) << 3);
	#define MAKE555RGB(w,r,g,b) (r = (w & 0x7C00) >> 7, g = (w & 0x03E0) >> 2, b = (w & 0x001F) << 3);

	/**************************************************************************/
	/****************** Color and Palette Functions ***************************/
	/**************************************************************************/

	#define  SDS_HLSMAX   255	/* H,L, and S vary over 0-HLSMAX */
	#define  SDS_RGBMAX   255	/* R,G, and B vary over 0-RGBMAX */

	WORD SDS_HueToRGB(WORD n1,WORD n2,WORD hue) {

	   /* range check: note values passed add/subtract thirds of range */
	   if(hue < 0)		hue+=SDS_HLSMAX;
	   if(hue > SDS_HLSMAX) hue-=SDS_HLSMAX;

	   /* return r,g, or b value from this tridrant */
	   if(hue<(SDS_HLSMAX/6))
		  return ( n1 + (((n2-n1)*hue+(SDS_HLSMAX/12))/(SDS_HLSMAX/6)));
	   if(hue<(SDS_HLSMAX/2))
		  return ( n2 );
	   if(hue<((SDS_HLSMAX*2)/3))
		  return (n1 + (((n2-n1)*(((SDS_HLSMAX*2)/3)-hue)+(SDS_HLSMAX/12))/(SDS_HLSMAX/6)));
	   else
		  return (n1);
	}

	DWORD SDS_HLStoRGB(WORD hue,WORD lum,WORD sat) {
		WORD R,G,B; 			   /* RGB component values */
		WORD  Magic1,Magic2;	   /* calculated magic numbers (really!) */

		if(sat==0) {			/* achromatic case */
			R=G=B=(lum*SDS_RGBMAX)/SDS_HLSMAX;
			if (hue!=(SDS_HLSMAX*2/3)) return(RGB(255,255,255));
		} else	{					 /* chromatic case */
			/* set up magic numbers */
			if (lum <= (SDS_HLSMAX/2))
				Magic2 = (lum*(SDS_HLSMAX + sat) + (SDS_HLSMAX/2))/SDS_HLSMAX;
			else
				Magic2 = lum + sat - ((lum*sat) + (SDS_HLSMAX/2))/SDS_HLSMAX;

			Magic1 = 2*lum-Magic2;

			/* get RGB, change units from SDS_HLSMAX to SDS_RGBMAX */
			R=(SDS_HueToRGB(Magic1,Magic2,hue+(SDS_HLSMAX/3))*SDS_RGBMAX +(SDS_HLSMAX/2))/SDS_HLSMAX;
			G=(SDS_HueToRGB(Magic1,Magic2,hue)*SDS_RGBMAX + (SDS_HLSMAX/2)) / SDS_HLSMAX;
			B=(SDS_HueToRGB(Magic1,Magic2,hue-(SDS_HLSMAX/3))*SDS_RGBMAX +(SDS_HLSMAX/2))/SDS_HLSMAX;
	   }
	   return(RGB(R,G,B));
	}




	RGBQUAD		   SDS_PaletteColors[256];

	static RGBQUAD basePaletteColors[256] =
					{
						{0x0,0x0,0xff},
						{0x0,0xff,0xff},
						{0x0,0xff,0x0},
						{0xff,0xff,0x0},
						{0xff,0x0,0x0},
						{0xff,0x0,0xff},
						{0xff,0xff,0xff},
						{0x80,0x80,0x80},
						{0xc0,0xc0,0xc0},
						{0x0,0x0,0xff},
						{0x7f,0x7f,0xff},
						{0x0,0x0,0xcc},
						{0x66,0x66,0xcc},
						{0x0,0x0,0x99},
						{0x4c,0x4c,0x99},
						{0x0,0x0,0x7f},
						{0x3f,0x3f,0x7f},
						{0x0,0x0,0x4c},
						{0x26,0x26,0x4c},
						{0x0,0x3f,0xff},
						{0x7f,0x9f,0xff},
						{0x0,0x33,0xcc},
						{0x66,0x7f,0xcc},
						{0x0,0x26,0x99},
						{0x4c,0x5f,0x99},
						{0x0,0x1f,0x7f},
						{0x3f,0x4f,0x7f},
						{0x0,0x13,0x4c},
						{0x26,0x2f,0x4c},
						{0x0,0x7f,0xff},
						{0x7f,0xbf,0xff},
						{0x0,0x66,0xcc},
						{0x66,0x99,0xcc},
						{0x0,0x4c,0x99},
						{0x4c,0x72,0x99},
						{0x0,0x3f,0x7f},
						{0x3f,0x5f,0x7f},
						{0x0,0x26,0x4c},
						{0x26,0x39,0x4c},
						{0x0,0xbf,0xff},
						{0x7f,0xdf,0xff},
						{0x0,0x99,0xcc},
						{0x66,0xb2,0xcc},
						{0x0,0x72,0x99},
						{0x4c,0x85,0x99},
						{0x0,0x5f,0x7f},
						{0x3f,0x6f,0x7f},
						{0x0,0x39,0x4c},
						{0x26,0x42,0x4c},
						{0x0,0xff,0xff},
						{0x7f,0xff,0xff},
						{0x0,0xcc,0xcc},
						{0x66,0xcc,0xcc},
						{0x0,0x99,0x99},
						{0x4c,0x99,0x99},
						{0x0,0x7f,0x7f},
						{0x3f,0x7f,0x7f},
						{0x0,0x4c,0x4c},
						{0x26,0x4c,0x4c},
						{0x0,0xff,0xbf},
						{0x7f,0xff,0xdf},
						{0x0,0xcc,0x99},
						{0x66,0xcc,0xb2},
						{0x0,0x99,0x72},
						{0x4c,0x99,0x85},
						{0x0,0x7f,0x5f},
						{0x3f,0x7f,0x6f},
						{0x0,0x4c,0x39},
						{0x26,0x4c,0x42},
						{0x0,0xff,0x7f},
						{0x7f,0xff,0xbf},
						{0x0,0xcc,0x66},
						{0x66,0xcc,0x99},
						{0x0,0x99,0x4c},
						{0x4c,0x99,0x72},
						{0x0,0x7f,0x3f},
						{0x3f,0x7f,0x5f},
						{0x0,0x4c,0x26},
						{0x26,0x4c,0x39},
						{0x0,0xff,0x3f},
						{0x7f,0xff,0x9f},
						{0x0,0xcc,0x33},
						{0x66,0xcc,0x7f},
						{0x0,0x99,0x26},
						{0x4c,0x99,0x5f},
						{0x0,0x7f,0x1f},
						{0x3f,0x7f,0x4f},
						{0x0,0x4c,0x13},
						{0x26,0x4c,0x2f},
						{0x0,0xff,0x0},
						{0x7f,0xff,0x7f},
						{0x0,0xcc,0x0},
						{0x66,0xcc,0x66},
						{0x0,0x99,0x0},
						{0x4c,0x99,0x4c},
						{0x0,0x7f,0x0},
						{0x3f,0x7f,0x3f},
						{0x0,0x4c,0x0},
						{0x26,0x4c,0x26},
						{0x3f,0xff,0x0},
						{0x9f,0xff,0x7f},
						{0x33,0xcc,0x0},
						{0x7f,0xcc,0x66},
						{0x26,0x99,0x0},
						{0x5f,0x99,0x4c},
						{0x1f,0x7f,0x0},
						{0x4f,0x7f,0x3f},
						{0x13,0x4c,0x0},
						{0x2f,0x4c,0x26},
						{0x7f,0xff,0x0},
						{0xbf,0xff,0x7f},
						{0x66,0xcc,0x0},
						{0x99,0xcc,0x66},
						{0x4c,0x99,0x0},
						{0x72,0x99,0x4c},
						{0x3f,0x7f,0x0},
						{0x5f,0x7f,0x3f},
						{0x26,0x4c,0x0},
						{0x39,0x4c,0x26},
						{0xbf,0xff,0x0},
						{0xdf,0xff,0x7f},
						{0x99,0xcc,0x0},
						{0xb2,0xcc,0x66},
						{0x72,0x99,0x0},
						{0x85,0x99,0x4c},
						{0x5f,0x7f,0x0},
						{0x6f,0x7f,0x3f},
						{0x39,0x4c,0x0},
						{0x42,0x4c,0x26},
						{0xff,0xff,0x0},
						{0xff,0xff,0x7f},
						{0xcc,0xcc,0x0},
						{0xcc,0xcc,0x66},
						{0x99,0x99,0x0},
						{0x99,0x99,0x4c},
						{0x7f,0x7f,0x0},
						{0x7f,0x7f,0x3f},
						{0x4c,0x4c,0x0},
						{0x4c,0x4c,0x26},
						{0xff,0xbf,0x0},
						{0xff,0xdf,0x7f},
						{0xcc,0x99,0x0},
						{0xcc,0xb2,0x66},
						{0x99,0x72,0x0},
						{0x99,0x85,0x4c},
						{0x7f,0x5f,0x0},
						{0x7f,0x6f,0x3f},
						{0x4c,0x39,0x0},
						{0x4c,0x42,0x26},
						{0xff,0x7f,0x0},
						{0xff,0xbf,0x7f},
						{0xcc,0x66,0x0},
						{0xcc,0x99,0x66},
						{0x99,0x4c,0x0},
						{0x99,0x72,0x4c},
						{0x7f,0x3f,0x0},
						{0x7f,0x5f,0x3f},
						{0x4c,0x26,0x0},
						{0x4c,0x39,0x26},
						{0xff,0x3f,0x0},
						{0xff,0x9f,0x7f},
						{0xcc,0x33,0x0},
						{0xcc,0x7f,0x66},
						{0x99,0x26,0x0},
						{0x99,0x5f,0x4c},
						{0x7f,0x1f,0x0},
						{0x7f,0x4f,0x3f},
						{0x4c,0x13,0x0},
						{0x4c,0x2f,0x26},
						{0xff,0x0,0x0},
						{0xff,0x7f,0x7f},
						{0xcc,0x0,0x0},
						{0xcc,0x66,0x66},
						{0x99,0x0,0x0},
						{0x99,0x4c,0x4c},
						{0x7f,0x0,0x0},
						{0x7f,0x3f,0x3f},
						{0x4c,0x0,0x0},
						{0x4c,0x26,0x26},
						{0xff,0x0,0x3f},
						{0xff,0x7f,0x9f},
						{0xcc,0x0,0x33},
						{0xcc,0x66,0x7f},
						{0x99,0x0,0x26},
						{0x99,0x4c,0x5f},
						{0x7f,0x0,0x1f},
						{0x7f,0x3f,0x4f},
						{0x4c,0x0,0x13},
						{0x4c,0x26,0x2f},
						{0xff,0x0,0x7f},
						{0xff,0x7f,0xbf},
						{0xcc,0x0,0x66},
						{0xcc,0x66,0x99},
						{0x99,0x0,0x4c},
						{0x99,0x4c,0x72},
						{0x7f,0x0,0x3f},
						{0x7f,0x3f,0x5f},
						{0x4c,0x0,0x26},
						{0x4c,0x26,0x39},
						{0xff,0x0,0xbf},
						{0xff,0x7f,0xdf},
						{0xcc,0x0,0x99},
						{0xcc,0x66,0xb2},
						{0x99,0x0,0x72},
						{0x99,0x4c,0x85},
						{0x7f,0x0,0x5f},
						{0x7f,0x3f,0x6f},
						{0x4c,0x0,0x39},
						{0x4c,0x26,0x42},
						{0xff,0x0,0xff},
						{0xff,0x7f,0xff},
						{0xcc,0x0,0xcc},
						{0xcc,0x66,0xcc},
						{0x99,0x0,0x99},
						{0x99,0x4c,0x99},
						{0x7f,0x0,0x7f},
						{0x7f,0x3f,0x7f},
						{0x4c,0x0,0x4c},
						{0x4c,0x26,0x4c},
						{0xbf,0x0,0xff},
						{0xdf,0x7f,0xff},
						{0x99,0x0,0xcc},
						{0xb2,0x66,0xcc},
						{0x72,0x0,0x99},
						{0x85,0x4c,0x99},
						{0x5f,0x0,0x7f},
						{0x6f,0x3f,0x7f},
						{0x39,0x0,0x4c},
						{0x42,0x26,0x4c},
						{0x7f,0x0,0xff},
						{0xbf,0x7f,0xff},
						{0x66,0x0,0xcc},
						{0x99,0x66,0xcc},
						{0x4c,0x0,0x99},
						{0x72,0x4c,0x99},
						{0x3f,0x0,0x7f},
						{0x5f,0x3f,0x7f},
						{0x26,0x0,0x4c},
						{0x39,0x26,0x4c},
						{0x3f,0x0,0xff},
						{0x9f,0x7f,0xff},
						{0x33,0x0,0xcc},
						{0x7f,0x66,0xcc},
						{0x26,0x0,0x99},
						{0x5f,0x4c,0x99},
						{0x1f,0x0,0x7f},
						{0x4f,0x3f,0x7f},
						{0x13,0x0,0x4c},
						{0x2f,0x26,0x4c},
						{0x33,0x33,0x33},
						{0x5b,0x5b,0x5b},
						{0x84,0x84,0x84},
						{0xad,0xad,0xad},
						{0xd6,0xd6,0xd6},
						{0xff,0xff,0xff}
					};
	int 		   SDS_XorColorIndex;

	// EBATEHC(CNBR) 2001-06-24 -[ OEM App.
	/*
	RGBQUAD
	SDS_GetPaletteColorsRGBQUAD( int color )
		{
		ASSERT( color > 0 );
		ASSERT( color <= 256 );

		return SDS_PaletteColors[ color-1 ];
		}
	*/
	RGBQUAD
	SDS_GetPaletteColorsRGBQUAD( int color )
		{
		ASSERT( color > 0 );
		ASSERT( color <= 256 );

		RGBQUAD* ary = SDS_CURPALLET;

		return ary[ color-1 ];
		}

	/*
	int
	SDS_GetPaletteColorsRGB( int color )
		{
		ASSERT( color > 0 );
		ASSERT( color <= 256 );

		return	RGB(SDS_PaletteColors[color-1].rgbRed,
					SDS_PaletteColors[color-1].rgbGreen,
					SDS_PaletteColors[color-1].rgbBlue);

		}
	*/
	int
	SDS_GetPaletteColorsRGB( int color )
		{
		ASSERT( color > 0 );
		ASSERT( color <= 256 );

		RGBQUAD* ary = SDS_CURPALLET;

		return	RGB(ary[color-1].rgbRed,
					ary[color-1].rgbGreen,
					ary[color-1].rgbBlue);

		}

	/*
	void SDS_CreateOurColorMap(void)
		{


		int i;
		for( i = 0; i < 256; i++ )
			{
			SDS_PaletteColors[i] = basePaletteColors[i];
			}

		// Now reset the background color to the correct color.
		struct resbuf rb;
		SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		if ( (rb.resval.rint < 1) ||
			 (rb.resval.rint > 256 ) )
			{
			rb.resval.rint = 256;
			}
		if(rb.resval.rint!=256)
			{
			SDS_PaletteColors[255].rgbRed=	SDS_PaletteColors[rb.resval.rint-1].rgbRed;
			SDS_PaletteColors[255].rgbGreen=SDS_PaletteColors[rb.resval.rint-1].rgbGreen;
			SDS_PaletteColors[255].rgbBlue= SDS_PaletteColors[rb.resval.rint-1].rgbBlue;

			if( rb.resval.rint == 8 ) // Special for grey
				{
				SDS_PaletteColors[rb.resval.rint-1].rgbRed	= 255;
				SDS_PaletteColors[rb.resval.rint-1].rgbGreen= 255;
				SDS_PaletteColors[rb.resval.rint-1].rgbBlue = 255;
				}
			else
				{
				int xorcol=~RGB(SDS_PaletteColors[rb.resval.rint-1].rgbRed,
								SDS_PaletteColors[rb.resval.rint-1].rgbGreen,
								SDS_PaletteColors[rb.resval.rint-1].rgbBlue);
				SDS_PaletteColors[rb.resval.rint-1].rgbRed	=GetRValue(xorcol);
				SDS_PaletteColors[rb.resval.rint-1].rgbGreen=GetGValue(xorcol);
				SDS_PaletteColors[rb.resval.rint-1].rgbBlue =GetBValue(xorcol);
				}
			}

		if ( SDS_CMainWindow->m_pPalette != NULL )
			{
			for (i=0; i<256; i++)
				{
				int rgb,fi1,fi2;
				fi1=RGB(SDS_PaletteColors[255].rgbRed,
						SDS_PaletteColors[255].rgbGreen,
						SDS_PaletteColors[255].rgbBlue);
				fi2=RGB(SDS_PaletteColors[i].rgbRed,
						SDS_PaletteColors[i].rgbGreen,
						SDS_PaletteColors[i].rgbBlue);
				rgb=fi1^fi2;
				rgb = SDS_CMainWindow->m_pPalette->GetNearestPaletteIndex(rgb);
				SDS_PaletteColors[i].rgbReserved=rgb;
				}
			}

		}
	*/
	void SDS_CreateOurColorMap(void)
		{

		RGBQUAD* ary = SDS_CURPALLET;

		int i;
		for( i = 0; i < 256; i++ )
			{
			ary[i] = basePaletteColors[i];
			}

		// Now reset the background color to the correct color.
		struct resbuf rb;
		SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		if ( (rb.resval.rint < 1) ||
			 (rb.resval.rint > 256 ) )
			{
			rb.resval.rint = 256;
			}
		if(rb.resval.rint!=256)
			{
			ary[255].rgbRed=  ary[rb.resval.rint-1].rgbRed;
			ary[255].rgbGreen=ary[rb.resval.rint-1].rgbGreen;
			ary[255].rgbBlue= ary[rb.resval.rint-1].rgbBlue;

			if( rb.resval.rint == 8 ) // Special for grey
				{
				ary[rb.resval.rint-1].rgbRed  = 255;
				ary[rb.resval.rint-1].rgbGreen= 255;
				ary[rb.resval.rint-1].rgbBlue = 255;
				}
			else
				{
				int xorcol=~RGB(ary[rb.resval.rint-1].rgbRed,
								ary[rb.resval.rint-1].rgbGreen,
								ary[rb.resval.rint-1].rgbBlue);
				ary[rb.resval.rint-1].rgbRed  =GetRValue(xorcol);
				ary[rb.resval.rint-1].rgbGreen=GetGValue(xorcol);
				ary[rb.resval.rint-1].rgbBlue =GetBValue(xorcol);
				}
			}

		if ( SDS_CMainWindow->m_pPalette != NULL )
			{
			for (i=0; i<256; i++)
				{
				int rgb,fi1,fi2;
				fi1=RGB(ary[255].rgbRed,
						ary[255].rgbGreen,
						ary[255].rgbBlue);
				fi2=RGB(ary[i].rgbRed,
						ary[i].rgbGreen,
						ary[i].rgbBlue);
				rgb=fi1^fi2;
				rgb = SDS_CMainWindow->m_pPalette->GetNearestPaletteIndex(rgb);
				ary[i].rgbReserved=rgb;
				}
			}

		}

	/*
	CPalette *SDS_GetNewPalette(void) {
		CPalette *OurPalette;

		LPLOGPALETTE lpLogPal;
		lpLogPal=(LPLOGPALETTE) malloc(sizeof(LOGPALETTE) + ((255) * sizeof(PALETTEENTRY)));
		lpLogPal->palVersion = 0x0300;
		lpLogPal->palNumEntries = 256;

		for (int i = 0; i < 256; i++) {
			lpLogPal->palPalEntry[i].peRed =   SDS_PaletteColors[i].rgbRed;
			lpLogPal->palPalEntry[i].peGreen = SDS_PaletteColors[i].rgbGreen;
			lpLogPal->palPalEntry[i].peBlue =  SDS_PaletteColors[i].rgbBlue;
			lpLogPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}

		OurPalette=new CPalette;
		VERIFY(OurPalette->CreatePalette(lpLogPal));
		free(lpLogPal);

		return(OurPalette);
	}
	*/
	CPalette *SDS_GetNewPalette(void) {
		CPalette *OurPalette;
		RGBQUAD*	ary = SDS_CURPALLET;

		LPLOGPALETTE lpLogPal;
		lpLogPal= new LOGPALETTE [sizeof(LOGPALETTE) + ((255) * sizeof(PALETTEENTRY))];
		lpLogPal->palVersion = 0x0300;
		lpLogPal->palNumEntries = 256;

		for (int i = 0; i < 256; i++) {
			lpLogPal->palPalEntry[i].peRed =   ary[i].rgbRed;
			lpLogPal->palPalEntry[i].peGreen = ary[i].rgbGreen;
			lpLogPal->palPalEntry[i].peBlue =  ary[i].rgbBlue;
			lpLogPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}

		OurPalette=new CPalette;
		VERIFY(OurPalette->CreatePalette(lpLogPal));
		delete lpLogPal;

		return(OurPalette);
	}

	HPALETTE SDS_GetHPalette(void) {
		CPalette *OurPalette=SDS_GetNewPalette();
		if(OurPalette==NULL) return(NULL);
		return((HPALETTE)OurPalette);
	}

	/*
	int SDS_RGBFromACADColor( int aColor )
		{
		if ( (aColor < 1) ||
			 (aColor > 256) )
			{
			return RGB( SDS_PaletteColors[SDS_XorColorIndex].rgbRed,
					   SDS_PaletteColors[SDS_XorColorIndex].rgbGreen,
					   SDS_PaletteColors[SDS_XorColorIndex].rgbBlue );
			}
		return(RGB(SDS_PaletteColors[aColor-1].rgbRed,
			   SDS_PaletteColors[aColor-1].rgbGreen,
			   SDS_PaletteColors[aColor-1].rgbBlue));
		}
	*/
	int SDS_RGBFromACADColor( int aColor )
		{
		RGBQUAD*	ary = SDS_CURPALLET;

		if ( (aColor < 1) ||
			 (aColor > 256) )
			{
			return RGB( ary[SDS_XorColorIndex].rgbRed,
					   ary[SDS_XorColorIndex].rgbGreen,
					   ary[SDS_XorColorIndex].rgbBlue );
			}
		return(RGB(ary[aColor-1].rgbRed,
			   ary[aColor-1].rgbGreen,
			   ary[aColor-1].rgbBlue));
		}

	/*
	int SDS_RGB16FromACADColor( int aColor )
		{
		if ( (aColor < 1) ||
			 (aColor > 256) )
			{
			return MAKE555WORD( SDS_PaletteColors[SDS_XorColorIndex].rgbRed,
								   SDS_PaletteColors[SDS_XorColorIndex].rgbGreen,
								   SDS_PaletteColors[SDS_XorColorIndex].rgbBlue );
			}

		return MAKE555WORD(SDS_PaletteColors[aColor-1].rgbRed,
							SDS_PaletteColors[aColor-1].rgbGreen,
							SDS_PaletteColors[aColor-1].rgbBlue);
		}
	*/
	int SDS_RGB16FromACADColor( int aColor )
		{
		RGBQUAD*	ary = SDS_CURPALLET;

		if ( (aColor < 1) ||
			 (aColor > 256) )
			{
			return MAKE555WORD( ary[SDS_XorColorIndex].rgbRed,
								   ary[SDS_XorColorIndex].rgbGreen,
								   ary[SDS_XorColorIndex].rgbBlue );
			}

		return MAKE555WORD(ary[aColor-1].rgbRed,
							ary[aColor-1].rgbGreen,
							ary[aColor-1].rgbBlue);
		}

	/*
	int SDS_PenColorFromACADColor( int aColor )
		{
		int col=aColor-1;
		if(col<0 || col>255)
			return(col=PALETTEINDEX(SDS_XorColorIndex));

		if(SDS_CMainWindow->m_bMapToRGBColor)
			return(RGB(SDS_PaletteColors[aColor-1].rgbRed,
				   SDS_PaletteColors[aColor-1].rgbGreen,
				   SDS_PaletteColors[aColor-1].rgbBlue));

	// added by Vitaly Spirin: not to draw white on white
		if( (SDS_CMainWindow->m_bPrintPreview || (SDS_CURVIEW && SDS_CURVIEW->IsPrinting())) &&
			aColor == 7 )
				return RGB(0, 0, 0);
		//AVH color 255 returns grey in preview but print white on white
		if(aColor == 255 && !(SDS_CURVIEW && SDS_CURVIEW->IsPrinting()))
				return RGB(200,200,200);
		return(col=PALETTEINDEX(col));
		}
	*/
	int SDS_PenColorFromACADColor( int aColor )
		{
		RGBQUAD*	ary = SDS_CURPALLET;
		int col=aColor-1;
		if(col<0 || col>255)
			return(col=PALETTEINDEX(SDS_XorColorIndex));

		if(SDS_CMainWindow->m_bMapToRGBColor)
			return(RGB(ary[aColor-1].rgbRed,
				   ary[aColor-1].rgbGreen,
				   ary[aColor-1].rgbBlue));

	// added by Vitaly Spirin: not to draw white on white
		if( (SDS_CMainWindow->m_bPrintPreview || (SDS_CURVIEW && SDS_CURVIEW->IsPrinting())) &&
			aColor == 7 )
				return RGB(0, 0, 0);
		//AVH color 255 returns grey in preview but print white on white
		if(aColor == 255 && !(SDS_CURVIEW && SDS_CURVIEW->IsPrinting()))
				return RGB(200,200,200);
		return(col=PALETTEINDEX(col));
		}

	int SDS_BrushColorFromACADColor( int aColor )
		{
		// same algorithm for Brush as Pen
		//
		return SDS_PenColorFromACADColor( aColor );
		}

	/*
	int
	SDS_XORPenColorFromACADColor( int color )
		{
		if ( (color < 0) || (color > 255) )
			{
			return RGB(SDS_PaletteColors[SDS_PaletteColors[SDS_XorColorIndex].rgbReserved].rgbRed,
						SDS_PaletteColors[SDS_PaletteColors[SDS_XorColorIndex].rgbReserved].rgbGreen,
						SDS_PaletteColors[SDS_PaletteColors[SDS_XorColorIndex].rgbReserved].rgbBlue);
			}
		else
			{
			return	RGB(SDS_PaletteColors[SDS_PaletteColors[color-1].rgbReserved].rgbRed,
						SDS_PaletteColors[SDS_PaletteColors[color-1].rgbReserved].rgbGreen,
						SDS_PaletteColors[SDS_PaletteColors[color-1].rgbReserved].rgbBlue);

			}
		}
	*/
	int
	SDS_XORPenColorFromACADColor( int color )
		{
		RGBQUAD*	ary = SDS_CURPALLET;

		if ( (color < 0) || (color > 255) )
			{
			return RGB(ary[ary[SDS_XorColorIndex].rgbReserved].rgbRed,
						ary[ary[SDS_XorColorIndex].rgbReserved].rgbGreen,
						ary[ary[SDS_XorColorIndex].rgbReserved].rgbBlue);
			}
		else
			{
			return	RGB(ary[ary[color-1].rgbReserved].rgbRed,
						ary[ary[color-1].rgbReserved].rgbGreen,
						ary[ary[color-1].rgbReserved].rgbBlue);

			}
		}

	/*
	int
	SDS_XORBrushColorFromACADColor( int color )
		{
		int retval;


		if ( (color < 0) || (color > 255) )
			{
			retval = SDS_PaletteColors[SDS_XorColorIndex].rgbReserved;
			}
		else
			{
			retval = SDS_PaletteColors[color - 1].rgbReserved;
			}

		if(SDS_CMainWindow->m_bMapToRGBColor)
			{
			retval = RGB(SDS_PaletteColors[ retval ].rgbRed,
						SDS_PaletteColors[ retval ].rgbGreen,
						SDS_PaletteColors[ retval ].rgbBlue);
			}

		return retval;
		}
	*/
	int
	SDS_XORBrushColorFromACADColor( int color )
		{
		int retval;
		RGBQUAD*	ary = SDS_CURPALLET;

		if ( (color < 0) || (color > 255) )
			{
			retval = ary[SDS_XorColorIndex].rgbReserved;
			}
		else
			{
			retval = ary[color - 1].rgbReserved;
			}

		if(SDS_CMainWindow->m_bMapToRGBColor)
			{
			retval = RGB(ary[ retval ].rgbRed,
						ary[ retval ].rgbGreen,
						ary[ retval ].rgbBlue);
			}

		return retval;
		}
	// EBATECH(CNBR) ]-

	//int SDS_ColDefCol,SDS_ColRetCol,SDS_ColMode,SDS_ColReturn;
	int SDS_GetColorDialog(int defcol, int *retcol,int mode)
		{
		ASSERT(IsWindow(IcadSharedGlobals::GetMainHWND()));

		WndActionEvent colorDialogEvent(FALSE, FALSE);

		colorDialogEvent.ResetEvent();

		SendNotifyMessage(IcadSharedGlobals::GetMainHWND(),
						  ICAD_WNDACTION_COLORDIA,
						  MAKEWPARAM(defcol, mode),
						  (LPARAM) &colorDialogEvent);
		
  		if (GetCurrentThreadId() != GetWindowThreadProcessId(IcadSharedGlobals::GetMainHWND(), NULL))
			colorDialogEvent.Lock();

		// EBATECH(CNBR) 2001-06-24 -[ unsigned short -> int
		if (retcol) *retcol = (int)(short)LOWORD(colorDialogEvent.GetReturnVal());

		return (int)(short)HIWORD(colorDialogEvent.GetReturnVal());
		// EBATECH(CNBR) ]-
		}

int SDS_BitBlt2Scr(int OnlyRect)
{
	CRect rect;
	CView* pView;
	CIcadDoc* pDoc = SDS_CURDOC;
	if(pDoc == NULL)
		return(RTERROR);
	
    POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
        pView = pDoc->GetNextView(pos);
		
		// Bug fix for images.  Print Preview was crashing when drawing Images.  
		// It was because a CIcadPreview object was in the View list, and
		// being cast to CIcadView, which is illegal.  I put in this check and added
		// ASSERTs elsewhere
		//
		if ( pView->IsKindOf(RUNTIME_CLASS(CIcadView)))
		{
			SDS_BitBlt2View(OnlyRect,(CIcadView *)pView);
		}
	}
	return(RTNORM);
}

int SDS_BitBlt2View(int OnlyRect, CIcadView* pView) 
{
#ifdef _USE_OPENGL_
	return(RTNORM);
#endif
	CRect rect;
	if(pView == NULL)
		return(RTERROR);
	if(OnlyRect && !pView->m_RectBitBlt)
		return(RTERROR);

	CDC* pDC = pView->GetDC();
	CPalette* oldpal = pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);
	pView->GetClientRect(&rect);
	CDrawDevice* pDrawDevice = pView->GetDrawDevice();
	pDrawDevice->beginDraw(pDC, pView->m_pCurDwgView);

	if(!OnlyRect) 
		pDrawDevice->copyToFront(rect.left, rect.top, rect.Width(), rect.Height());
	else 
	{
		if(pView->m_RectBitBltPt[0].x<0) pView->m_RectBitBltPt[0].x=0;
		if(pView->m_RectBitBltPt[0].y<0) pView->m_RectBitBltPt[0].y=0;
		if(pView->m_RectBitBltPt[1].x>rect.Width())
			pView->m_RectBitBltPt[1].x=rect.Width();
		if(pView->m_RectBitBltPt[1].y>rect.Height())
			pView->m_RectBitBltPt[1].y=rect.Height();

		pView->m_RectBitBlt=0;

		pDrawDevice->copyToFront(pView->m_RectBitBltPt[0].x, pView->m_RectBitBltPt[0].y,
					         pView->m_RectBitBltPt[1].x - pView->m_RectBitBltPt[0].x + 1, 
							 pView->m_RectBitBltPt[1].y - pView->m_RectBitBltPt[0].y + 1);
	}
	pDrawDevice->endDraw();
	pView->ReleaseDrawDevice(pDrawDevice);
	pView->ReleaseDC(pDC);
	return(RTNORM);
}


	// Used to control SDS_DrawLine etc. to only draw in specific window instead of all
	//
int SDS_DrawInOneWindow;
CIcadView* SDS_pViewToDrawIn;

//	 // Modified CyberAge VSB 20/05/2001 [
//	 extern SNAPSTRUCT SnapStructLastSnap;	//Stores the snapped points
//	 extern Snap_DrawInOneWindow;			//Stores wether to draw in all views
//	 extern int SDS_BoxModeCursor;	//declared in icadapi.cpp, used to where a "selecting rectangle box" is being drawn
//	 extern BOOL bGetPoint ;			// if waiting for point.
//	 extern int SDS_PointDisplayMode;// if waiting for point.
//	 // Modified CyberAge VSB 20/05/2001 ]
void SDS_DrawDragVectors(CIcadDoc* pDoc) 
{
	if(pDoc == NULL) 
		pDoc = SDS_CURDOC;
	if(pDoc == NULL || SDS_CMainWindow->m_pDragVarsCur == NULL)
		return;
	
	POSITION pos = pDoc->GetFirstViewPosition();
	SDS_DrawInOneWindow = TRUE;
	while(pos != NULL) 
	{
		SDS_pViewToDrawIn = (CIcadView *)pDoc->GetNextView(pos);
		ASSERT_KINDOF(CIcadView, SDS_pViewToDrawIn);
		
		SDS_CMainWindow->m_pDragVarsCur->vects = SDS_pViewToDrawIn->m_pDragVects;
#ifdef _SDS_GRVECS_
		sds_grvecs(SDS_CMainWindow->m_pDragVarsCur->vects, SDS_CMainWindow->m_pDragVarsCur->matx);
#else
		CIcadEntityRenderer renderer(pDoc, SDS_pViewToDrawIn);
		renderer.drawVecs(SDS_CMainWindow->m_pDragVarsCur->vects, SDS_CMainWindow->m_pDragVarsCur->matx);
#endif
	}
	SDS_DrawInOneWindow = FALSE;
	SDS_pViewToDrawIn = NULL;

//	 	// Modified CyberAge VSB 04/06/2001 [
//	 	// Checking whether to show snap or not.
//	 	// Bug 77792 from BugZilla
//	 	resbuf rb = { 0 };
//	 	sds_getvar( "OSMODE", &rb );
//	 	int osmode = rb.resval.rint;
//
//	 	sds_getvar( "AUTOSNAP", &rb ) ;
//	 	int autosnap	 = rb.resval.rint ;
//
//	 	if( ( ( bGetPoint || SDS_PointDisplayMode )&& !SDS_BoxModeCursor && !(osmode & IC_OSMODE_OFF ) && autosnap) )
//	 	{
//	 		//Modified CyberAge VSB 20/05/2001 [
//	 		// Draws snap while dragging.
//	 		CIcadView *pView = SDS_CURVIEW;
//	 		//CIcadDoc	*pDoc	 = SDS_CURDOC;
//	 		pDoc	 = SDS_CURDOC;
//	 		//POSITION pos=pDoc->GetFirstViewPosition();
//	 		pos=pDoc->GetFirstViewPosition();
//	 		while (pos!=NULL)
//	 		{
//	 			if(Snap_DrawInOneWindow)
//	 			{
//	 				pView=SDS_CURVIEW;
//	 			}
//	 			else
//	 			{
//	 				pView=(CIcadView *)pDoc->GetNextView(pos);
//	 			}
//
//	 			if(pView==NULL)
//	 			{
//	 				return ;
//	 			}
//
//
//	 			ASSERT_KINDOF(CIcadView, pView);
//
//	 			ASSERT( !pView->IsPrinting() );
//
//	 			pView->SnapObject.m_gr_view  =pView->m_pCurDwgView;
//	 			if ( SnapStructLastSnap.bValidSnap )
//	 			{
//	 				pView->m_iFlyOverSnapX= pView->SnapObject.m_LastSnapPtPixX;
//	 				pView->m_iFlyOverSnapY= pView->SnapObject.m_LastSnapPtPixY;
//
//	 			}
//	 			if ( SnapStructLastSnap.bValidSnap )
//	 			{
//	 				int nWidth = 30;
//	 				pView->m_RectBitBlt = 1;
//	 				pView->m_RectBitBltPt[0].x = pView->m_iFlyOverSnapX - nWidth;
//	 				pView->m_RectBitBltPt[0].y = pView->m_iFlyOverSnapY - nWidth;
//	 				pView->m_RectBitBltPt[1].x = pView->m_iFlyOverSnapX + nWidth;
//	 				pView->m_RectBitBltPt[1].y = pView->m_iFlyOverSnapY + nWidth;
//
//	 				pView->SnapObject.pDC =  pView->GetDC();
//	 				int	rop = ::SetROP2( pView->SnapObject.pDC->m_hDC,R2_XORPEN);
//	 				pView->SnapObject.DrawSnap(SnapStructLastSnap,1);
//	 				pView->m_nFlyOverSnapDrawn	= 1;
//	 				pView->m_iFlyOverSnapX = pView->SnapObject.m_LastSnapPtPixX;
//	 				pView->m_iFlyOverSnapY = pView->SnapObject.m_LastSnapPtPixY;
//	 				::SetROP2( pView->SnapObject.pDC->m_hDC,rop);
//	 				pView->ReleaseDC( pView->SnapObject.pDC );
//
//	 			}
//	 			else
//	 			{
//	 				if ( pView->m_nFlyOverSnapDrawn )
//	 				{
//	 					int nWidth = 30;
//	 					pView->m_RectBitBlt = 1;
//	 					pView->m_RectBitBltPt[0].x = pView->m_iFlyOverSnapX - nWidth;
//	 					pView->m_RectBitBltPt[0].y = pView->m_iFlyOverSnapY - nWidth;
//	 					pView->m_RectBitBltPt[1].x = pView->m_iFlyOverSnapX + nWidth;
//	 					pView->m_RectBitBltPt[1].y = pView->m_iFlyOverSnapY + nWidth;
//	 					pView->m_nFlyOverSnapDrawn = 0;
//	 				}
//
//	 			}
//
//	 			if(Snap_DrawInOneWindow)
//	 				break;
//	 		}
//	 		// Modified CyberAge VSB 20/05/2001 ]
//	 	}
//	 	// Modified CyberAge VSB 03/06/2001 ]
//
}


int SDS_DrawLine(const sds_point from, const sds_point to, int color, int highlight, int xor, db_drawing* flp, int BitBlt2Scr)
{
#ifdef _SDS_GRDRAW_
	int pix[4];
	sds_point pto, pfrom;
	CDC* pDC = NULL;
	CIcadView* pView = NULL;
	CIcadDoc* pDoc = SDS_CURDOC;
	struct gr_view* view = NULL;

	if(pDoc==NULL)
		return(RTERROR);

    POSITION pos = pDoc->GetFirstViewPosition();
	bool bDrawToScreen = (BitBlt2Scr && !SDS_DontBitBlt);

	while(pos != NULL)
	{
		if(SDS_DrawInOneWindow)
		{
			if(SDS_pViewToDrawIn)
				pView = SDS_pViewToDrawIn;
			else
				pView = SDS_CURVIEW;
		}
		else
		{
			pView = (CIcadView*)pDoc->GetNextView(pos);
		}

		if(pView == NULL)
			return(RTERROR);
		ASSERT_KINDOF(CIcadView, pView);
		ASSERT(!pView->IsPrinting());

		view = pView->m_pCurDwgView;
		if(view==NULL)
			return(RTERROR);

		CDrawDevice* pBufferDrawDevice = pView->GetDrawDevice();
		ASSERT(pBufferDrawDevice->IsValid());
		if(bDrawToScreen)
		{
			pDC = pView->GetDC();
			pBufferDrawDevice->beginDraw(pDC, NULL);

			// Is the following line really necessary???
			//  Shouldn't this have been set when the DC was created??
			//
			SelectPalette(pDC->GetSafeHdc(), (HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(), TRUE);
		}
		int rop;
		if(color == -1 || xor)
		{
			rop = R2_XORPEN;
			pBufferDrawDevice->SetDrawMode(RasterConstants::MODE_XOR);
			//if(bDrawToScreen)
			//	SetROP2(Wincdc->m_hDC, R2_XORPEN);
		}
		else
		{
			rop = R2_COPYPEN;
			pBufferDrawDevice->SetDrawMode(RasterConstants::MODE_COPY);
			//if(bDrawToScreen)
			//	SetROP2(Wincdc->m_hDC, R2_COPYPEN);
		}
		if(highlight)
		{
			if(SDS_PaletteColors[255].rgbRed == 255 && SDS_PaletteColors[255].rgbGreen == 255 && SDS_PaletteColors[255].rgbBlue == 255)
				pBufferDrawDevice->SetLineColorACAD(-1, 1, RasterConstants::LS_DOT);
			else
				pBufferDrawDevice->SetLineColorACAD(256, 1, RasterConstants::LS_DOT);
		}
		else 
			if(R2_XORPEN == rop)
				pBufferDrawDevice->SetLineColorXOR(color, 1);
			else
				pBufferDrawDevice->SetLineColorACAD(color, 1);
/*
		if(bDrawToScreen)
		{
			if(highlight)
			{
				if(SDS_PaletteColors[255].rgbRed==255 &&
					SDS_PaletteColors[255].rgbGreen==255 &&
					SDS_PaletteColors[255].rgbBlue==255)
				{
					pScreenDrawDevice->SetLineColorACAD( -1, 1, RasterConstants::LS_DOT );
				}
				else
				{
					pScreenDrawDevice->SetLineColorACAD( 256, 1, RasterConstants::LS_DOT );
				}
			}
			else
			{
				pScreenDrawDevice->SetLineColorACAD( color );
			}
		}
*/
		gr_ucs2rp((double *)to, pto, view);
		gr_ucs2rp((double *)from, pfrom, view);
		int rc = gr_vect_rp2pix(pto, pfrom, pix, view);
		if(rc==0 || rc==2)		/*D.Gavrilov*///draw etc. only in these cases
		{
			POINT pixpt[2];
			pixpt[0].x = pix[0];
			pixpt[0].y = pix[1];
			pixpt[1].x = pix[2];
			pixpt[1].y = pix[3];

			pBufferDrawDevice->Polyline(pixpt, 2);
			if(bDrawToScreen)
			{
				int drawBuffer = pBufferDrawDevice->setDrawBuffer(CDrawDevice::eFRONT);
				pBufferDrawDevice->Polyline(pixpt, 2);
				pBufferDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
				pBufferDrawDevice->endDraw();
			}

			// Update blit region
			//
			int fi1;
			for(fi1 = 0; fi1 < 3; fi1 += 2)
			{
				if(pView->m_RectBitBlt==0)
				{
					pView->m_RectBitBltPt[0].x = pix[fi1+0];
					pView->m_RectBitBltPt[0].y = pix[fi1+1];
					pView->m_RectBitBltPt[1].x = pix[fi1+0];
					pView->m_RectBitBltPt[1].y = pix[fi1+1];
					pView->m_RectBitBlt = 1;
				}
				else
				{
					if(pView->m_RectBitBltPt[0].x > pix[fi1+0])
						pView->m_RectBitBltPt[0].x = pix[fi1+0];
					if(pView->m_RectBitBltPt[0].y > pix[fi1+1])
						pView->m_RectBitBltPt[0].y = pix[fi1+1];
					if(pView->m_RectBitBltPt[1].x < pix[fi1+0])
						pView->m_RectBitBltPt[1].x = pix[fi1+0];
					if(pView->m_RectBitBltPt[1].y < pix[fi1+1])
						pView->m_RectBitBltPt[1].y = pix[fi1+1];
				}
			}
		}
		pView->ReleaseDrawDevice(pBufferDrawDevice);
		if(bDrawToScreen)
			pView->ReleaseDC(pDC);

		if(SDS_DrawInOneWindow)
			break;
	}	// end of while

    return(RTNORM);
#else
	ASSERT(SDS_CURDOC->m_dbDrawing == flp);
	if(SDS_DrawInOneWindow)
	{
		if(SDS_pViewToDrawIn)
		{
			ASSERT(SDS_CURDOC == SDS_pViewToDrawIn->m_pViewsDoc);
			CIcadEntityRenderer renderer(SDS_CURDOC, SDS_pViewToDrawIn);
			return renderer.drawLine(from, to, color, highlight, xor, !!BitBlt2Scr);
		}
		else
		{
			CIcadEntityRenderer renderer(SDS_CURDOC, SDS_CURVIEW);
			return renderer.drawLine(from, to, color, highlight, xor, !!BitBlt2Scr);
		}
	}
	else
	{
		CIcadEntityRenderer renderer(SDS_CURDOC);
		return renderer.drawLine(from, to, color, highlight, xor, !!BitBlt2Scr);
	}
#endif
}	// end of SDS_DrawLine

int SDS_DrawFill(const sds_point *pts, int npts, int color, int highlight, int xor, db_drawing *flp, int BitBlt2Scr)
{
	int fi1;
	sds_point pt1;
	CDC* pDC = NULL;
	CIcadView* pView = NULL;
	CIcadDoc* pDoc = SDS_CURDOC;
	CPalette* oldpal;
	struct gr_view* view;

	if(pts == NULL || npts <= 0 || pDoc == NULL)
		return(RTERROR);

    POSITION pos = pDoc->GetFirstViewPosition();
	bool bDrawToScreen = (BitBlt2Scr && !SDS_DontBitBlt);
	while(pos != NULL)
	{
		if(SDS_DrawInOneWindow)
			pView=SDS_CURVIEW;
		else
			pView=(CIcadView *)pDoc->GetNextView(pos);
		if(pView == NULL)
			return(RTERROR);
		ASSERT_KINDOF(CIcadView, pView);
		ASSERT(!pView->IsPrinting());

		view = pView->m_pCurDwgView;
		if(view == NULL)
			return(RTERROR);

		if(npts > pView->m_nPolyPts)
		{
			if(pView->m_pPolyPts==NULL)				
				pView->m_pPolyPts= new POINT[npts];
			else
			{
				POINT *temp;
				temp = new POINT[npts];
				memcpy(temp, pView->m_pPolyPts, sizeof(POINT) * pView->m_nPolyPts);
				delete [] pView->m_pPolyPts;
				pView->m_pPolyPts = temp;
			}
			pView->m_nPolyPts = npts;
		}

		if(pView->m_pPolyPts == NULL)
			return(RTERROR);

		for(fi1 = 0; fi1 < npts; fi1++)
		{
			gr_ucs2rp((double *)(pts[fi1]),pt1,view);
			gr_rp2pix(view,pt1[0],pt1[1],(int *)&pView->m_pPolyPts[fi1].x,(int *)&pView->m_pPolyPts[fi1].y);
		}

		CDrawDevice* pBufferDrawDevice = pView->GetDrawDevice();
		ASSERT(pBufferDrawDevice->IsValid());
		if(bDrawToScreen)
		{
			pDC = pView->GetDC();
			pBufferDrawDevice->beginDraw(pDC, NULL);
			oldpal = pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);
		}
		int rop;
		if(color == -1 || xor)
		{
			rop = R2_XORPEN;
			pBufferDrawDevice->SetDrawMode( RasterConstants::MODE_XOR );
			//if(bDrawToScreen)
			//	SetROP2(Wincdc->m_hDC, R2_XORPEN);
		}
		else
		{
			rop = R2_COPYPEN;
			pBufferDrawDevice->SetDrawMode( RasterConstants::MODE_COPY );
			//if(bDrawToScreen)
			//	SetROP2(Wincdc->m_hDC, R2_COPYPEN);
		}

		if(highlight)
		{
			if(SDS_PaletteColors[255].rgbRed == 255 && SDS_PaletteColors[255].rgbGreen == 255 && SDS_PaletteColors[255].rgbBlue == 255)
			{
				pBufferDrawDevice->SetLineColorACAD(-1, 1, RasterConstants::LS_DOT);
				pBufferDrawDevice->SetFillColorXOR(255);
			}
			else
			{
				pBufferDrawDevice->SetLineColorACAD(256, 1, RasterConstants::LS_DOT);
				pBufferDrawDevice->SetFillColorXOR(-1);
			}
		}
		else 
			if(R2_XORPEN == rop)
			{
				pBufferDrawDevice->SetLineColorXOR(color, 1);
				pBufferDrawDevice->SetFillColorXOR(color);
			}
			else
			{
				pBufferDrawDevice->SetLineColorACAD(color, 1);
				pBufferDrawDevice->SetFillColorACAD(color);
			}

/*
		if(bDrawToScreen)
		{
			if(highlight)
			{
				if(SDS_PaletteColors[255].rgbRed==255 &&
					SDS_PaletteColors[255].rgbGreen==255 &&
					SDS_PaletteColors[255].rgbBlue==255)
				{
					pScreenDrawDevice->SetLineColorACAD( -1, 1, RasterConstants::LS_DOT );
					pScreenDrawDevice->SetFillColorACAD( -1 );
				}
				else
				{
					pScreenDrawDevice->SetLineColorACAD( 256, 1, RasterConstants::LS_DOT );
					pScreenDrawDevice->SetFillColorACAD( 256 );
				}
			}
			else
			{
				if(R2_XORPEN==Wincdc->GetROP2())
				{
					pScreenDrawDevice->SetLineColorXOR( color );
					pScreenDrawDevice->SetFillColorXOR( color );
				}
				else
				{
					pScreenDrawDevice->SetLineColorACAD( color );
					pScreenDrawDevice->SetFillColorACAD( color );
				}
			}
		}
*/
		pBufferDrawDevice->Polygon(pView->m_pPolyPts, npts, false);
		if(bDrawToScreen)
		{
			int drawBuffer = pBufferDrawDevice->setDrawBuffer(CDrawDevice::eFRONT);
			pBufferDrawDevice->Polygon(pView->m_pPolyPts, npts, false);
			pBufferDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
			pBufferDrawDevice->endDraw();
			pView->ReleaseDC(pDC);
		}

		for(fi1 = 0; fi1 < npts; fi1++)
		{
			if(pView->m_RectBitBlt == 0)
			{
				pView->m_RectBitBltPt[0].x = pView->m_pPolyPts[fi1].x;
				pView->m_RectBitBltPt[0].y = pView->m_pPolyPts[fi1].y;
				pView->m_RectBitBltPt[1].x = pView->m_pPolyPts[fi1].x;
				pView->m_RectBitBltPt[1].y = pView->m_pPolyPts[fi1].y;
				pView->m_RectBitBlt = 1;
			}
			else
			{
				if(pView->m_RectBitBltPt[0].x > pView->m_pPolyPts[fi1].x)
					pView->m_RectBitBltPt[0].x = pView->m_pPolyPts[fi1].x;
				if(pView->m_RectBitBltPt[0].y > pView->m_pPolyPts[fi1].y)
					pView->m_RectBitBltPt[0].y = pView->m_pPolyPts[fi1].y;
				if(pView->m_RectBitBltPt[1].x < pView->m_pPolyPts[fi1].x)
					pView->m_RectBitBltPt[1].x = pView->m_pPolyPts[fi1].x;
				if(pView->m_RectBitBltPt[1].y < pView->m_pPolyPts[fi1].y)
					pView->m_RectBitBltPt[1].y = pView->m_pPolyPts[fi1].y;
			}
		}
		pView->ReleaseDrawDevice(pBufferDrawDevice);

		if(SDS_DrawInOneWindow)
			break;
	} // end of while

    return(RTNORM);
} // end of SDS_DrawFill

int SDS_SetClipInDC(CIcadView* pView, db_handitem* pVPort, bool bClearOnly) 
{
	if(pView == NULL || pVPort == NULL || pView->m_pViewsDoc == NULL) 
		return RTERROR;
	db_drawing* pDrawing = pView->m_pViewsDoc->m_dbDrawing;
	if(pDrawing == NULL)
		return RTERROR;
	
	int x1, x2, y1, y2, id, tilemode, cvport;
	struct resbuf rb;
	CRect rect;
	
	SDS_getvar(NULL, DB_QTILEMODE, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES);
	tilemode = rb.resval.rint;
	SDS_getvar(NULL, DB_QCVPORT, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES);
	cvport = rb.resval.rint;
	
	if(tilemode == 0 && cvport != 1 && (SDS_CMainWindow->m_bPrintPreview || pView->IsPrinting()))
		return RTNORM;
	
	pView->GetClientRect(&rect);
	CDC* pDC = pView->GetDC();
	pDC->SelectClipRgn(NULL);
	pView->GetTargetDeviceCDC()->SelectClipRgn(NULL);
	
	if(SDS_CMainWindow->m_bPrintPreview || pView->IsPrinting())
	{
		pDC->IntersectClipRect(pView->m_PrintClipRect);
		pView->GetTargetDeviceCDC()->IntersectClipRect(pView->m_PrintClipRect);
	}
	
	if(pView->m_pClipRectDC != NULL) 
	{ 
		delete pView->m_pClipRectDC; 
		pView->m_pClipRectDC = NULL; 
	}
	if(pView->m_pClipRectMemDC != NULL) 
	{ 
		delete pView->m_pClipRectMemDC; 
		pView->m_pClipRectMemDC=NULL; 
	}
	
	if(tilemode == 1 || pVPort->get_69(&id) == FALSE || id == 1 || bClearOnly) 
	{
		pView->ReleaseDC(pDC);
		return RTERROR;
	}
	SDS_GetRectForVP(pView, pVPort, &x1, &x2, &y1, &y2);
	
	// This sets up the windows clipping regen.
	pView->m_pClipRectDC = new CRect(x1 + 1, y1 + 1, x2, y2);
	pDC->IntersectClipRect(pView->m_pClipRectDC);
	pView->m_pClipRectMemDC = new CRect(x1 + 1, y1 + 1, x2, y2);
	pView->GetTargetDeviceCDC()->IntersectClipRect(pView->m_pClipRectMemDC);
	pView->ReleaseDC(pDC);
	return RTNORM;
}

int SDS_GetRectForVP(CIcadView *pView, db_handitem* pVPort, int *x1,int *x2,int *y1,int *y2) 
{
	if(pView == NULL)
		return RTERROR;
	db_drawing *pDrawing = pView->GetDrawing();
	ASSERT(pDrawing != NULL);
	
	if(pDrawing->tblnext(DB_VXTAB, 1) == NULL)
		return RTERROR;
	
	// Need to make sure we ALWAYS have the view vars set to the PS viewport.
	struct gr_view *gViewVP = NULL;
	struct gr_viewvars viewvarsp;
	db_handitem* pCurVPort = pView->m_pVportTabHip;
	pView->m_pVportTabHip = pDrawing->tblnext(DB_VXTAB, 1);
	pView->m_pVportTabHip = ((db_vxtabrec *)pView->m_pVportTabHip)->ret_vpehip();
	SDS_ViewToVars2Use(pView, pDrawing, &viewvarsp);
	pView->m_pVportTabHip = pCurVPort;
	gr_initview(pDrawing, &SDS_CURCFG, &gViewVP, &viewvarsp);
	
	double pt[2], height, width;
	sds_point center;
	pVPort->get_10(center);
	pVPort->get_40(&width);
	pVPort->get_41(&height);
	
	// center,ht & width is the paperspace screen data
	pt[0] = center[0] + (width / 2.0);
	pt[1] = center[1] - (height / 2.0);
	gr_rp2pix(gViewVP, pt[0], pt[1], x2, y2);
	pt[0] = center[0] - (width / 2.0);
	pt[1] = center[1] + (height / 2.0);
	gr_rp2pix(gViewVP, pt[0], pt[1], x1, y1);
	
	gr_freeviewll(gViewVP);	 
	gViewVP = NULL;
	return RTNORM;
}

int SDS_ClearViewPort(db_handitem* pVPort, CIcadView* pView) 
{
	extern bool cmd_DontClearVport;
	if(cmd_DontClearVport) 
		return RTNORM;

	int x1, x2, y1, y2;
	SDS_GetRectForVP(pView, pVPort, &x1, &x2, &y1, &y2);
	RECT rect;
	rect.left = x1 - 2;
	rect.top = y1 - 2;
	rect.right = x2 + 2;
	rect.bottom = y2 + 2;
	pView->GetFrameBufferDrawDevice()->fillRect(rect, 256);
	SDS_BitBlt2Scr(0);
	return RTNORM;
}




