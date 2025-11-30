/* D:\DEV\PRJ\ICAD\ICADBITMAPS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// *********************************************************************
// *  ICADBITMAPS.C                                                   *
// *  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
// *                                                                   *
// *********************************************************************

// ** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"

//////// Imaging stuff is below.

//*** Header information for placeable metafile format.
	#define ALDUS_ID	0x9AC6CDD7 
	#define APMSIZE 22 

	typedef struct 
	{ 
		DWORD key;
		WORD hmf;
		SMALL_RECT bbox;
		WORD inch;
		DWORD reserved;
		WORD checksum;
	} APMFILEHEADER;
	typedef APMFILEHEADER* PAPMFILEHEADER;
//***


BOOL SDS_ReadFileIntoBuffer(char *pszFile,char **ppBuffer, unsigned long *bytes)
{
	CFile fileMenu;
	CFileException fileException;
	DWORD dwFileLen;

	if(!fileMenu.Open(pszFile,CFile::modeRead,&fileException)) return(FALSE);
	fileMenu.SeekToBegin();
	dwFileLen=fileMenu.GetLength();
	*ppBuffer = new char[dwFileLen+1];
	if(*ppBuffer==NULL) return(FALSE);
	if(fileMenu.Read(*ppBuffer,dwFileLen)<dwFileLen) { IC_FREE(*ppBuffer); return(FALSE); }	
	else (*ppBuffer)[dwFileLen]=0;

	//*** Close the file.
	fileMenu.Close();
	*bytes=(long)dwFileLen;

	return TRUE;
}

CBitmap *SDS_GetBitmapFromBuffer(CDC *cdc, char *pBuffer, long bytes) {

    HBITMAP hBitmap;
    BITMAPFILEHEADER fHdr;          // BitmapFile header
    BITMAPINFOHEADER bmiHdr;        // BitmapInfo header

	// Make sure this is a bitmap.
	if(pBuffer==NULL || ((pBuffer[0]!='B' || pBuffer[1]!='M') && (pBuffer[0]!='M' || pBuffer[1]!='Z'))) 
		return(NULL);

	/* Read the Bitmap header from buffer. */
    memcpy((char *)&fHdr,pBuffer,sizeof(BITMAPFILEHEADER));

    /* Read the Bitmap Info Header. */
    memcpy((char *)&bmiHdr,pBuffer+sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER));

//  Need to step through the RGB Quads here to set the background color.
	int colct=0;
    if (bmiHdr.biBitCount==1) colct=1;
    else if(bmiHdr.biBitCount <= 4) colct=16;
	else colct=256;
	LPBITMAPINFO bmi=(LPBITMAPINFO)(pBuffer+sizeof(BITMAPFILEHEADER));

	RGBQUAD rqBackGroundColor = SDS_GetPaletteColorsRGBQUAD( 256 );
	int		iBackGroundColor = SDS_GetPaletteColorsRGB( 256 );

	for(colct; colct>=0; colct--) 
		{
		if(bmi->bmiColors[colct].rgbBlue==rqBackGroundColor.rgbBlue && 			
		   bmi->bmiColors[colct].rgbGreen==rqBackGroundColor.rgbGreen &&			
		   bmi->bmiColors[colct].rgbRed==rqBackGroundColor.rgbRed) 
			{			
			int xorcol=~iBackGroundColor;

			bmi->bmiColors[colct].rgbRed  =GetRValue(xorcol);
			bmi->bmiColors[colct].rgbGreen=GetGValue(xorcol);
			bmi->bmiColors[colct].rgbBlue =GetBValue(xorcol);
			}
		}

	bmi->bmiColors[0].rgbBlue=rqBackGroundColor.rgbRed;
	bmi->bmiColors[0].rgbGreen=rqBackGroundColor.rgbGreen;
	bmi->bmiColors[0].rgbRed=rqBackGroundColor.rgbBlue; 


    hBitmap=CreateDIBitmap(cdc->m_hDC,&bmiHdr,0L,NULL,NULL,0);

	SetDIBits(cdc->m_hDC,hBitmap,0,bmiHdr.biHeight,pBuffer+fHdr.bfOffBits,(LPBITMAPINFO)(pBuffer+sizeof(BITMAPFILEHEADER)),DIB_RGB_COLORS);

	CBitmap *bm=new CBitmap;
	bm->Attach(hBitmap);

    return(bm);
}

PBITMAPINFO SDS_CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp) { 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 
	int     nRGBquads;
 
    /* Retrieve the bitmap's color format, width, and height. */ 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) return(NULL);
 
    /* Convert the color format to a count of bits. */ 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
 
    if (cClrBits==1) {
        cClrBits=1; 
		nRGBquads=1;
    } else if (cClrBits <= 4) {
        cClrBits = 4; 
		nRGBquads=16;
	} else {
        cClrBits = 8; 
		nRGBquads=256;
		bmp.bmBitsPixel=8;
	}
 
    /* 
     * Allocate memory for the BITMAPINFO structure. (This structure 
     * contains a BITMAPINFOHEADER structure and an array of RGBQUAD data 
     * structures.) 
     */ 
    pbmi = new BITMAPINFO [sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nRGBquads]; 
 
    /* Initialize the fields in the BITMAPINFO structure. */ 
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    pbmi->bmiHeader.biClrUsed = nRGBquads; 
 
    /* If the bitmap is not compressed, set the BI_RGB flag. */ 
    pbmi->bmiHeader.biCompression = BI_RGB; 
 
    /* 
     * Compute the number of bytes in the array of color 
     * indices and store the result in biSizeImage. 
     */ 
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 
                                  * pbmi->bmiHeader.biHeight 
                                  * cClrBits; 
 
    /* 
     * Set biClrImportant to 0, indicating that all of the 
     * device colors are important. 
     */ 
    pbmi->bmiHeader.biClrImportant = 0; 
 
    return pbmi; 
} 
 
void SDS_PutBMPinBuffer(HWND hwnd, char **ppBuffer, long *bytes, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC,bool ChgBackgnd) { 
 
    BITMAPFILEHEADER hdr;       /* bitmap file-header */ 
    PBITMAPINFOHEADER pbih;     /* bitmap info-header */ 
    LPBYTE lpBits;              /* memory pointer */ 
  
    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    if (!lpBits) return;
 
    /* 
     * Retrieve the color table (RGBQUAD array) and the bits 
     * (array of palette indices) from the DIB. 
     */ 
 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, 
		lpBits, pbi, DIB_RGB_COLORS)) {
		return;
	}

	pbih->biClrUsed=256;
 
    /* Create the .BMP file. */ 
 
 
    hdr.bfType = 0x4d42;        /* 0x42 = "B" 0x4d = "M" */ 
 
    /* Compute the size of the entire file. */ 
 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
 
    /* Compute the offset to the array of color indices. */ 
 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 
 
    /* Copy the BITMAPFILEHEADER into the .BMP file. */ 
	(*bytes)=sizeof(BITMAPFILEHEADER)+(sizeof(BITMAPINFOHEADER)+pbih->biClrUsed*sizeof (RGBQUAD))+pbih->biSizeImage;
	(*ppBuffer)= new char [*bytes];
 
    memcpy(*ppBuffer, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER));
 
    /* Copy the BITMAPINFOHEADER and RGBQUAD array into the file. */ 
 
    memcpy((*ppBuffer)+sizeof(BITMAPFILEHEADER), (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD));

	if(ChgBackgnd) {
		//  Need to step through the RGB Quads here to set the background color.
		int colct=0;
		if (pbih->biBitCount==1) colct=1;
		else if(pbih->biBitCount <= 4) colct=16;
		else colct=256;
		LPBITMAPINFO bmi=(LPBITMAPINFO)((*ppBuffer)+sizeof(BITMAPFILEHEADER));

		struct resbuf rb;
		SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint!=7) {
			for(colct; colct>=0; colct--) {
				if(bmi->bmiColors[colct].rgbBlue==255 && 			
				   bmi->bmiColors[colct].rgbGreen==255 &&			
				   bmi->bmiColors[colct].rgbRed==255) {			
				int xorcol=~RGB(255,255,255);
					bmi->bmiColors[colct].rgbRed  =GetRValue(xorcol);
					bmi->bmiColors[colct].rgbGreen=GetGValue(xorcol);
					bmi->bmiColors[colct].rgbBlue =GetBValue(xorcol);
				}
			}
			bmi->bmiColors[0].rgbBlue=255;
			bmi->bmiColors[0].rgbGreen=255;
			bmi->bmiColors[0].rgbRed=255; 
		}
	}
 
    /* Copy the array of color indices into the .BMP file. */  
    memcpy((*ppBuffer)+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), (LPVOID) lpBits, pbih->biSizeImage); 
  
    /* Free memory. */

    GlobalFree((HGLOBAL)lpBits);
} 
 
int SDS_SaveBitmapToBuffer(CBitmap *bitmap, char **ppBuffer, long *bytes,bool ChgBackgnd) {

	if(bitmap==NULL || ppBuffer==NULL || bytes==NULL) return(-1);

    HBITMAP hBitmap=(HBITMAP)bitmap->GetSafeHandle();
//    if(NULL==hBitmap) hBitmap=(HBITMAP)bitmap;
    BITMAPINFO *bmiHdr;        // BitmapInfo header
	HDC hdc;

	bmiHdr=SDS_CreateBitmapInfoStruct(SDS_CMainWindow->m_hWnd,hBitmap); 
	if(NULL==bmiHdr) return(-1);

	SDS_PutBMPinBuffer(SDS_CMainWindow->m_hWnd,ppBuffer,bytes,bmiHdr,hBitmap,hdc=GetDC(SDS_CMainWindow->m_hWnd),ChgBackgnd);
	ReleaseDC(SDS_CMainWindow->m_hWnd,hdc);    
	delete bmiHdr;
	
    return(0);
}

int SDS_SaveBufferToFile(char *pBuffer,long len, LPTSTR pszFile) {
	CFile tfil;
	if(!tfil.Open(pszFile,CFile::modeCreate|CFile::modeWrite)) {
		return(RTERROR);
	}
	tfil.Write(pBuffer,len);	
	tfil.Close();
    return(RTNORM);
}

HMETAFILE SDS_ReadWMF(char* pszFileName)
{
	HANDLE           hData;
	LPSTR            lpData;
	DWORD            OffsetToMeta;
	METAHEADER       mfHeader;
	APMFILEHEADER    APMHeader;
	HFILE			 fh;
	HMETAFILE		 hMetaFile;

	if(NULL==pszFileName)
		return NULL;

	OffsetToMeta = APMSIZE;

	if(HFILE_ERROR==(fh=_lopen(pszFileName, OF_READ)))
		return NULL;
	// Seek to beginning of file and read APM header
	_llseek(fh, 0, 0);
	if(!_lread(fh, (LPSTR)&APMHeader, sizeof(APMFILEHEADER)))
	{
		// Error in reading the file
		_lclose(fh);
		return NULL;
	}

	// Return to read metafile header
	_llseek(fh, OffsetToMeta, 0);
	if(!_lread(fh, (LPSTR)&mfHeader, sizeof(METAHEADER)))
	{
		// Error in reading
		_lclose(fh);
		return NULL;
	}

	// Allocate memory for memory based metafile
	if(!(hData = GlobalAlloc(GHND, (mfHeader.mtSize * 2L))))
	{
		_lclose(fh);
		return NULL;
	}
	// Were we successful?
	if(!(lpData = (LPSTR)GlobalLock(hData)))
	{
		// Error in allocation
		GlobalFree(hData);
		_lclose(fh);
		return NULL;
	}

	// Read metafile bits
	_llseek(fh, OffsetToMeta, 0);
	if(!_lread(fh, lpData, (mfHeader.mtSize * 2L)))
	{
		// Error in reading
		GlobalUnlock(hData);
		GlobalFree(hData);
		_lclose(fh);
		return NULL;
	}

	// Create the METAFILE with the bits we read in.
	DWORD dwSize = GlobalSize(hData);
	if(!(hMetaFile = SetMetaFileBitsEx(dwSize,(CONST BYTE*)lpData)))
	{
		_lclose(fh);
		return NULL;
	}

	GlobalUnlock(hData);

	// Close the APM file
	_lclose(fh);
	return hMetaFile;
}

int SDS_WriteWMF(ICADWMFINFO* pWmfInfo, char* pszFileName)
{
//*** Write a windows metafile to a file.

	UINT uiSize = GetMetaFileBitsEx(pWmfInfo->hMetaFile,0,NULL); 

	DWORD dwHigh = 0; 
	DWORD dwLow = uiSize; 
	HANDLE hFile = NULL;
	if(INVALID_HANDLE_VALUE==(hFile=CreateFile(pszFileName,
		GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,NULL)))
	{ 
		return RTERROR;
	} 

	if(!uiSize)
	{
		CloseHandle(hFile);
		return RTERROR;
	}

	APMFILEHEADER AldHdr;

	AldHdr.key = ALDUS_ID;
	AldHdr.hmf = 0;									// Unused; must be zero 
	AldHdr.bbox.Left = 0;							// in metafile units 
	AldHdr.bbox.Top = 0; 
	AldHdr.inch = pWmfInfo->inch;
	AldHdr.bbox.Right  = (SHORT)pWmfInfo->rectWmf.right;
	AldHdr.bbox.Bottom = (SHORT)pWmfInfo->rectWmf.bottom;
	AldHdr.reserved = 0;
	AldHdr.checksum = 0;

	WORD* p;
	for(p=(WORD*)&AldHdr,AldHdr.checksum=0; p<(WORD*)&(AldHdr.checksum); ++p)
	{
		AldHdr.checksum ^= *p;
	}

	DWORD dwBufSize = (APMSIZE + uiSize);
	LPBYTE pBuffer = new BYTE[dwBufSize];
	if(NULL==pBuffer)
	{
		CloseHandle(hFile);
		return RTERROR;
	}
	//*** Write the placeable metafile data to the buffer.
	memcpy(pBuffer,&AldHdr,APMSIZE);
	//*** Write the metafile data to the buffer.
	GetMetaFileBitsEx(pWmfInfo->hMetaFile,uiSize,pBuffer+APMSIZE);
	//*** Write the buffer to the file.
	DWORD dwBytesWritten;
	WriteFile(hFile,pBuffer,dwBufSize,&dwBytesWritten,NULL);

	delete [] pBuffer;
	CloseHandle(hFile);

	return RTNORM;
}



