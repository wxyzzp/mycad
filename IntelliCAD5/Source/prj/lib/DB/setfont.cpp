/* D:\ICADDEV\PRJ\LIB\DB\SETFONT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"/*DNT*/
#include "dbdll.h"
#include "SmartPointer.h"
#include "BinaryResource.h"
#include <tchar.h>
#include <iostream.h>
#include <fstream.h>
#include <strstrea.h>


	// helper functions for EOF checks
inline bool
countedStreamRead( istream& stream, TCHAR *dest, int count)
	{
	stream.read( dest, count);
	return (stream.gcount() EQ count );
	}


inline bool
safeStreamGetc( istream& stream, TCHAR& outChar)
	{
	bool	result = countedStreamRead( stream, &outChar, sizeof( outChar));
	
	if ( !result )
		outChar = EOF;
	
	return result;
	}


inline bool
safeStreamSeek( istream& stream, int position)
	{
	stream.seekg( position);
	return ( stream.tellg() EQ position );
	}

static int
findfont( const char *szLookFor, char *szPathFound, DWORD bufSize )
{
	char szPath[ MAX_PATH ];
	szPath[0] = 0;
	if( GetModuleFileName( ModuleInstance, szPath, MAX_PATH ) )
	{
		char* fchar = strrchr( szPath, '\\' );
		if( fchar )
		{
			strcpy( fchar+1, "Fonts" );
		}
	}
	return ic_findinpath(  szPath, szLookFor, szPathFound, bufSize );
}


/*static int
findfont( const char *szLookFor, char *szPathFound )
	{
	char szPath[ MAX_PATH ];
	const char *pSharedPath = ic_getvisiosharedpath();
	szPath[0] = 0;
	if ( pSharedPath )
		{
		strcpy( szPath, pSharedPath );
		strcat( szPath, "\\Fonts"  );
		}
	return ic_findinpath(  szPath, szLookFor, szPathFound );
	}
*/

DB_API int db_setfont(
    char         *fontname,
    char         *foundpn,
    db_fontlink **fontpp,
    db_drawing   *dp) {
/*
**  NOTE: This function does NOT use the fontmap.  See the wrapper
**  function, db_setfontusingmap().
**
**  Sets *fontpp to the link in dp->fontll that corresponds to
**  fontname.  If the font is not already present in dp->fontll,
**  this function reads the file and adds the db_fontlink to dp_fontll.
**
**  fontname can be be anything from the extensionless font name
**  to the full pathname.
**
**  NOTE:
**
**      Calling this function with all NULLs (db_setfont(NULL,NULL,NULL,NULL))
**      sets the static flag st_cantfindtol to 0 and static pointer
**      st_tolfont to NULL, which allows this function to search for
**      the TOLERANCE font again.
**
**  If foundpn!=NULL and the search is successful, the file specifier
**  that worked is copied to foundpn.  (THE CALLER HAS TO MAKE SURE
**  THAT foundpn CAN HOLD THE FULL PATHNAME -- UP TO 512 CHARS.)
**
**  WARNING: THE CALLER SHOULD NOT FREE *fontpp; it points into
**  dp->fontll.  Use it to set the font pointers in the STYLE
**  table records.
**
**  Even if fontpp==NULL, this function still creates a db_fontlink
**  if it reads the file (to confirm that it IS a valid font file),
**  but the data is freed before exit.  foundpn is set, however.
**
**  The search order is as follows:
**
**      fontname as passed
**      fontname without the path
**      fontname without the path + .SHX
**      fontname without the path + .SHP
**      db_icfontfn (icad.fnt)
**
**  File types are identified by contents -- not extension.
**
**  This file was derived from dwg_readfont(), which used to
**  reset all of the affected font and bigfont ptrs in the STYLE
**  table record.  db_setfont() doesn't do this.
**
**  NOTE:
**
**    Most of the code in here is the file-reading stuff.
**    The looping (file-extension-changing) code is relatively small --
**    but INCREDIBLY UGLY;  I don't like it at all.  For now, it stays.
**    BUT READ IT CAREFULLY BEFORE MESSING WITH IT!
**
**  Returns:
**      +1 : Took the default (icad.fnt)
**       0 : Okay.
**      -1 : fontname is NULL (or contains no filename).
**      -2 : Can't find the link in dp->fontll or read a recognizable
**             font file.  (*fontpp is set to NULL.)
**      -3 : No memory.
**      -4 : Found a line that's too long (SHP files have a max of 128;
**             we allow DB_ACADBUF-1).
*/
    TCHAR *ln,*lp,*fcp1,*filenm,*fullpn,fc1;
    short lidx,lnsz_1,pnsz,thiscode,hiidx,cidx,didx,nditems,pass,useff;
    short change1st,ftype,togo,shprev,readit,madeflink,added2fontll;
    short shapedef,lastditch,trying4tol,rc,fi1,fi2,fi3,fi4;
    TCHAR fint1;
    long nchrsinfile,def1seek,thisseek,nextseek;
    db_fontlink *flink,*tfp1;

//    static int st_cantfindtol;
//    static db_fontlink *st_tolfont;

    /* Array of code/length data for "shapes 1.0" or "shapes 1.1" SHX file: */
    struct lendatalink { short code,nbytes; } *lendata;

	istream		*stream = NULL;		// for reading files


    rc=0; 
	ln=filenm=fullpn=NULL; 
	flink=NULL; 
	lendata=NULL;
    lnsz_1=DB_ACADBUF-1; madeflink=added2fontll=0; lastditch=trying4tol=0;
    pnsz=DB_ACADBUF+10;


    /* If all NULL pass params, reset the static tolerance font search flag: */
    if (fontname==NULL &&
        foundpn ==NULL &&
        fontpp  ==NULL &&
        dp      ==NULL) { /*st_cantfindtol=0; st_tolfont=NULL; */goto out; }

    if (*fontpp!=NULL) *fontpp=NULL;


    if ((filenm= new char [pnsz])==NULL ||
        (fullpn= new char [pnsz])==NULL ||
        (ln    = new char [lnsz_1+1])   ==NULL) { rc=-3; goto out; }


    if (foundpn!=NULL) *foundpn=0;

    if (fontname==NULL) { rc=-1; goto out; }

    while (*fontname && isspace((unsigned char) *fontname)) fontname++;

    {
        /*
        **  The TOLERANCE font (db_tolfontnm or db_ourtolfontnm) is special.
        **  We can encounter the "change font to gtd" command several times
        **  within one TOLERANCE, and we don't want to keep loading
        **  a "new" definition (when we can find the file) or keep
        **  doing findfile when we can't.
        **
        **  So, we're implementing two things here:
        **
        **      (1) Bailing early if static st_cantfindtol tells us
        **          we've looked before and can't find it.
        **
        **      (2) Searching dp->fontll to see if it's already loaded
        **          and using that one, instead of redefining it in
        **          dp->fontll as usual. (I think that's what usually happens.)
        */

        /* Isolate the font name and see if we're looking for the TOLERANCE */
        /* font: */
        for (fi1=0; fontname[fi1]; fi1++);
        for (fi2=fi1; fi1>-1 && fontname[fi1]!=IC_SLASH; fi1--)
            if (fontname[fi1]=='.'/*DNT*/) fi2=fi1;
        fi1++;
        fc1=fontname[fi2]; 
		fontname[fi2]=0;
        trying4tol=(strisame(fontname+fi1,db_tolfontnm) ||
                    strisame(fontname+fi1,db_ourtolfontnm));
        fontname[fi2]=fc1;

        if (trying4tol) {
			db_fontlink *st_tolfont;
			st_tolfont = NULL;
            /* Already know we can't find it? */
            if (dp->ret_cantfindtolfont()) { rc=-2; goto out; }

            /* If we don't already have a pointer to it, see if it's */
            /* already in dp_fontll: */
            if (dp->ret_tolfontptr()==NULL && dp!=NULL) {
                for (st_tolfont=dp->ret_fontll(); st_tolfont!=NULL; st_tolfont=st_tolfont->next) {
                    if (st_tolfont->pn==NULL || !*st_tolfont->pn) continue;

                    /* Isolate the font name and see if it's the TOLERANCE font: */
                    for (fi1=0; st_tolfont->pn[fi1]; fi1++);
                    for (fi2=fi1; fi1>-1 && st_tolfont->pn[fi1]!=IC_SLASH; fi1--)
                        if (st_tolfont->pn[fi1]=='.'/*DNT*/) fi2=fi1;
                    fi1++;
                    fc1=st_tolfont->pn[fi2]; st_tolfont->pn[fi2]=0;
                    fi3=(strisame(st_tolfont->pn+fi1,db_tolfontnm) ||
                         strisame(st_tolfont->pn+fi1,db_ourtolfontnm));
                    st_tolfont->pn[fi2]=fc1;
                    if (fi3) break;  /* Got it. */
                }
            }

            if (st_tolfont!=NULL) {  /* Found it. */
				dp->set_tolfontptr(st_tolfont);
                flink=st_tolfont;
                madeflink=0;  /* Making SURE */
                if (foundpn!=NULL && flink->pn!=NULL)
                    strcpy(foundpn,flink->pn);
                goto out;
            }
        }

        /* Otherwise, try to load it like any other font. */
    }

	// if this is a TrueType font, use an existing db_fontlink if there
	// is one, otherwise create a db_fontlink for it; there's no need
	// to read the actual font file
	if (0 == ic_chkext(fontname, "ttf"/*DNT*/) || 
		0 == ic_chkext(fontname, "ttc"/*Win2000*/) )		
	{ // TrueType font
		// set up filenm and fullpn
		strncpy(fullpn, fontname, pnsz-10);
		fullpn[pnsz - 10] = 0;
		ic_trim(fullpn, "be"/*DNT*/);
		fi1 = 0;
		while (fullpn[fi1])
			fi1++;
		
		while (fi1 > -1 && fullpn[fi1] != IC_SLASH)
			fi1--;
		
		fi1++;
		strcpy(filenm, fullpn + fi1);
		if (!*fullpn || !*filenm)
			{
			rc = -1;
			goto out;
			}

		// look for an existing db_fontlink
		tfp1 = NULL;
		if (NULL != dp)
			{
			for (tfp1 = dp->ret_fontll();
				 tfp1 != NULL && stricmp(fullpn, tfp1->pn);
				 tfp1 = tfp1->next);
			
			if (tfp1 != NULL)
				{
				flink = tfp1;
				madeflink = 0;						// make sure this is set properly
				if (foundpn != NULL)
					strcpy(foundpn, fullpn);
				
				goto out;
				}
			}

		// didn't find an existing db_fontlink, so create one
		delete flink;								// make sure not pointing to something
		flink = new db_fontlink;
		madeflink = 1;
		ASSERT(flink);

		flink->pn = new char[strlen(fullpn) + 1];
		ASSERT(flink->pn);
		strcpy(flink->pn, fullpn);

		flink->desc = new char[LF_FULLFACESIZE];
		ASSERT(flink->desc);
		strcpy(flink->desc, "TrueType Font"/*DNT*/);

		flink->format = db_fontlink::TRUETYPE;
	}
	else											// shx/shp/or something else
	{
		for (readit=(dp==NULL); readit<2; readit++) {
			/* 0 = Search dp->fontll. */
			/* 1 = Try to read it from file. */

		  /* Set up filenm and fullpn: */
		  strncpy(fullpn,fontname,pnsz-10);
		  fullpn[pnsz-10]=0; ic_trim(fullpn,"be"/*DNT*/);
		  fi1=0; while (fullpn[fi1]) fi1++;
		  while (fi1>-1 && fullpn[fi1]!=IC_SLASH) fi1--;
		  fi1++;
		  strcpy(filenm,fullpn+fi1);
		  if (!*fullpn || !*filenm) { rc=-1; goto out; }

		  useff=0;  /* Try it first without findfile. */
		  change1st=0;  /* A flag determining when to change extension */

		  do {  /* Main file-determining loop. */

			  ftype=0; /* 0 : Unidentified */
					   /* 1 : SHP */
					   /* 2 : shape   SHX (1.0 or 1.1) */
					   /* 3 : unifont SHX */
					   /* 4 : bigfont SHX */

			  def1seek=0L;  /* Precaution */

			  if (useff) {
				  for (;;) {
					  if (change1st) change1st=0;
					  else {
						if (findfont(filenm,fullpn, (DWORD) pnsz)==RTNORM) {
						  if (readit) {
							break;
						  } else {  /* Look in dp->fontll */
							/* If found, take data from fontll and goto out: */
							/* (Duplicate code; see below.) */
							if (dp!=NULL) {
							  for (tfp1=dp->ret_fontll(); tfp1!=NULL &&
								  stricmp(fullpn,tfp1->pn); tfp1=tfp1->next);
							  if (tfp1!=NULL) {
								  flink=tfp1;
								  madeflink=0;  /* Making SURE */
								  if (foundpn!=NULL) strcpy(foundpn,fullpn);
								  goto out;
							  }
							}
						  }
						}
					  }

					  /* Change to the next ext (or to db_icfontfn): */
					  if (strisame(filenm,db_icfontfn)) {
						if (readit) 
							goto readfont; 
						else 
							goto nextreadit;

						/* "else" should never execute, but leave it for safety. */
					  }
					  fi2=0; while (filenm[fi2]) fi2++;
					  fi1=fi2-1;
					  while (fi1>-1 && filenm[fi1]!='.') fi1--;
					  if (fi1>-1) fi2=fi1;
					  if (filenm[fi2]) {  /* Has ext */
						  if        (strisame(filenm+fi2+1,"SHX"/*DNT*/)) {
							  filenm[fi2+3]='P'/*DNT*/;
						  } else if (strisame(filenm+fi2+1,"SHP"/*DNT*/)) {
							  /*
							  ** Last chance; change to default --
							  ** but not if we're searching dp->fontll
							  ** (readit==0).  Just start the reading
							  ** phase in that case.  We don't want to
							  ** latch onto icad.fnt in dp->fontll just
							  ** because the font we want hasn't been read
							  ** in yet.
							  */

							  if (!readit) goto nextreadit;

							  strcpy(filenm,db_icfontfn);
							  fi2=0;  /* An idx that's NOT the 0-terminator. */
							  lastditch=1;
						  } else filenm[fi2]=0;  /* Original ext; try SHX. */
					  }
					  if (!filenm[fi2]) {  /* NOT AN else! */
						  filenm[fi2++]='.'/*DNT*/;
						  filenm[fi2++]='S'/*DNT*/;
						  filenm[fi2++]='H'/*DNT*/;
						  filenm[fi2++]='X'/*DNT*/;
						  filenm[fi2]=0;
					  }
				  }

			  } else if (!readit) {

				/* If found, take data from fontll and goto out. */
				/* (Duplicate code; see above.) */
				if (dp!=NULL) {
				  for (tfp1=dp->ret_fontll(); tfp1!=NULL &&
					  stricmp(fullpn,tfp1->pn); tfp1=tfp1->next);
				  if (tfp1!=NULL) {
					  flink=tfp1;
					  madeflink=0;  /* Making SURE */
					  if (foundpn!=NULL) strcpy(foundpn,fullpn);
					  goto out;
				  }
				}

			  }

	readfont:
			  SmartBuffer	fileBuffer( 0x2000);	// 8192
			  SmartBuffer	readBuffer(0);
				
			  delete stream;			// also deleted at function exit
			  stream = NULL;
			  if (readit) 
			  {
				// locate the default font resource
				if(strisame(filenm, db_icfontfn))
				{
					stream = BinaryResource(IDR_ICADFNT).NewIstream();
					// DP: findfont() in statement above can't find this file, so ...(see next line)
					strcpy(fullpn, db_icfontfn);
				}
				else 
					// locate the default tolerance font
					// DP: if tolerance font has extension in his name then check with string is failed, use simple checking
					if(trying4tol /*strisame(filenm, db_ourtolfontnm)*/)
					{
						stream = BinaryResource(IDR_GDTSHX).NewIstream();
						// DP: findfont() in statement above can't find this file, so ...(see next line)
						strcpy(fullpn, db_ourtolfontnm);
					}
					// read a font file
					else
					{
					ifstream	*fileStream = new ifstream();
					fileStream->setbuf( 
								fileBuffer.Memcpy( NULL, fileBuffer.Size()), 
								fileBuffer.Size()
								);
					fileStream->open( fullpn, ios::nocreate | ios::binary);
					if ( fileStream && fileStream->is_open() )
						{
						size_t		size;
									
									// get size of file
						fileStream->seekg( 0, ios::end);
						size = fileStream->tellg();
						fileStream->seekg( 0, ios::beg);

									// if under threshhold, use a memory stream
						if ( size < 40000 && (readBuffer = SmartBuffer( size)) )
							{
							char	*buffer = readBuffer.Memcpy( NULL, size);
							if ( countedStreamRead( *fileStream, buffer, size) )
								stream = new istrstream( buffer, size);
							}

									// if no memory stream, use file
						if ( !stream )
							stream = fileStream;
						}

									// get rid of file if memory read
					if ( stream != fileStream )
						delete fileStream;
					}

										// NOTE: if stream is NULL, fontStream won't be used
				istream&	fontStream = *stream;

				if (!stream) { // mark inability to load
	//				dp->failedfontlist.AddTail((CString)filenm);
				}
				
				if ( stream ) {
					/* Alloc flink (and free old if necessary): */
					delete flink; flink=new db_fontlink; madeflink=1;

					/* Set flink->pn: */
					flink->pn=new char[strlen(fullpn)+1];
					strcpy(flink->pn,fullpn);

					/* Read beginning and try to determine file type and get */
					/* header data.  Set ftype after a successful header read. */
					const int FONTHEADER = 40;

					ASSERT( FONTHEADER < lnsz_1 );
					if ( !countedStreamRead( fontStream, ln, FONTHEADER ) )
						goto db_rf_unrec;

					if (strnsame(ln,"AutoCAD-86 shapes 1."/*DNT*/,20)) {

						/* Get the SHP revision #: */
						if      (ln[20]=='0'/*DNT*/) shprev=10;  /* 1.0 */
						else if (ln[20]=='1'/*DNT*/) shprev=11;  /* 1.1 */
						else  goto db_rf_unrec;

						/* Finish checking the ID string: */
						if (strncmp(ln+21,"\r\n\x1A"/*DNT*/,3)) goto db_rf_unrec;

						/* Skip ID and low/hi shape #s:*/
						if ( !safeStreamSeek( fontStream, 28L )	)
							goto db_rf_unrec;

						/* Get nchrsinfile: */

						if ( !countedStreamRead( fontStream, ln, 2 ) )
							goto db_rf_unrec;

						memcpy(&fi1,ln,sizeof(fi1));
						nchrsinfile=fi1;
						/* Actually, there should never be more than 259 chrs */
						/* in this type of file (counting 0 and the special */
						/* 256-258 chrs), but use 300 for safety: */
                        /* Lowest shape count is 1 for shape shx file. 2003/11/21 EBATECH(CNBR) */
						if (nchrsinfile<1 /*2L*/ || nchrsinfile>300) goto db_rf_unrec;

						def1seek=30L+nchrsinfile*4;  /* Seek pos of 1st def. */

						/* Alloc the array and read the code/length data */
						/* (free old 1st, if there is one): */
						if (lendata!=NULL) delete [] lendata;
						if ((lendata= new struct lendatalink [nchrsinfile] )==NULL) 
							{ rc=-3; goto out; }
						memset(lendata,0,sizeof(struct lendatalink)*nchrsinfile);
						flink->nchrs=256;  /* Determine flink->nchrs as we go. */
						shapedef=1;  /* We'll 0 this if we find a shape code of 0 */
									 /* (the special one that fonts have). */
						for (fi1=0; fi1<nchrsinfile; fi1++) {
							if ( !countedStreamRead( fontStream, ln, 4 ) )
								goto db_rf_unrec;

							memcpy(&lendata[fi1].code  ,ln  ,sizeof(short));
							memcpy(&lendata[fi1].nbytes,ln+2,sizeof(short));
							if (lendata[fi1].nbytes<2) goto db_rf_unrec;
							if (!fi1 && !lendata[fi1].code) shapedef=0;  /* Font */
							if (lendata[fi1].code<0 || lendata[fi1].code>255)
								flink->nchrs++;
						}

						if (shapedef) {
							flink->desc=new char[17];
							strcpy(flink->desc,"Shape Definition"/*DNT*/);
							flink->above=1;  /* So that SHAPEs scale properly. */
						} else {
							/* Alloc, read, and store the desc: */
							if ( !safeStreamSeek( fontStream, def1seek ) )
								goto db_rf_unrec;

							fi1=lendata[0].nbytes-4;  /* Len of desc (counting the 0). */
							if (fi1<1 || fi1>lnsz_1) goto db_rf_unrec;
							if ( !countedStreamRead( fontStream, ln, fi1) )
								goto db_rf_unrec;

							fi1--;  /* Expected strlen of desc. */
							fi2=0; while (fi2<fi1 && ln[fi2]) fi2++;
							/* fi2 is actual strlen of desc. */
							ln[fi2]=0;
							flink->desc=new char[fi2+1];
							strcpy(flink->desc,ln);

							/* Get the header bytes: */
							if ( !countedStreamRead( fontStream, ln, 4) )
								goto db_rf_unrec;

							flink->above=ln[0];
							flink->below=ln[1];
							flink->modes=ln[2];
						}

						ftype=2; 

						if (shprev==10)
							flink->format = db_fontlink::SHAPE_1_0;

						else
							flink->format = db_fontlink::SHAPE_1_1;


					} else if (strnsame(ln,"AutoCAD-86 unifont 1.0\r\n\x1A"/*DNT*/,25)) {

						/* Skip ID: */
						if ( !safeStreamSeek( fontStream, 25L ) )
							goto db_rf_unrec;

						/* Get nchrsinfile and offset to 1st chr def: */
						fi1=sizeof(long)+sizeof(short);
						if ( !countedStreamRead( fontStream, ln, fi1) )
							goto db_rf_unrec;

						memcpy(&nchrsinfile,ln,sizeof(nchrsinfile));
						/* Offset to 1st chr def: */
						memcpy(&fi1,ln+sizeof(long),sizeof(fi1));
						if (nchrsinfile<1L || nchrsinfile>32767L || fi1<7)
							goto db_rf_unrec;
						def1seek=fontStream.tellg()+fi1;  /* Seek pos of 1st def. */

						/* Alloc, read, and store the desc: */
						fi1-=6;  /* Length of desc (counting the 0). */
						if (fi1<1 || fi1>lnsz_1) goto db_rf_unrec;
						if ( !countedStreamRead( fontStream, ln, fi1) )
							goto db_rf_unrec;

						fi1--;  /* Expected strlen of desc. */
						fi2=0; while (fi2<fi1 && ln[fi2]) fi2++;
						/* fi2 is actual strlen of desc. */
						ln[fi2]=0;
						flink->desc=new char[fi2+1];
						strcpy(flink->desc,ln);

						/* Get the header bytes: */
						if ( !countedStreamRead( fontStream, ln, 6) )
							goto db_rf_unrec;

						flink->above   =ln[0];
						flink->below   =ln[1];
						flink->modes   =ln[2];
						flink->encoding=ln[3];
						flink->type    =ln[4];

						ftype=3; 
						
						flink->format = db_fontlink::UNIFONT;

					} else if (strnsame(ln,"AutoCAD-86 bigfont 1.0\r\n\x1A"/*DNT*/,25)) {

						/* Skip ID and unknown short: */
						if ( !safeStreamSeek( fontStream, 27L ) )
							goto db_rf_unrec;

						/* Get nchrsinfile and # of esc code ranges (nesc) */
						/* (nchrsinfile is actually # of locator recs here -- */
						/* some of which may be blank): */
						fi1=sizeof(short)+sizeof(short);
						if ( !countedStreamRead( fontStream, ln, fi1) )
							goto db_rf_unrec;

						memcpy(&fi1,ln,sizeof(fi1));
						nchrsinfile=fi1;
						memcpy(&fi1,ln+sizeof(short),sizeof(fi1));
						flink->nesc=(char)fi1;
						if (nchrsinfile<1L || nchrsinfile>32767L ||
							flink->nesc<1) goto db_rf_unrec;

						/* Alloc and read the escape code ranges. */
						/* Note that for each range, there are 2 flink->ecs chars */
						/* and 4 bytes in the file (since the beg/end specifiers */
						/* are shorts in the file): */
						fi1=2*flink->nesc;
						flink->esc=new char[fi1];
						fi1*=2;  /* They are shorts in the file. */
						if ( !countedStreamRead( fontStream, ln, fi1) )
							goto db_rf_unrec;

						for (fi1=fi2=fi3=0; fi1<flink->nesc; fi1++,fi2+=2,fi3+=4) {
							memcpy(&fi4,ln+fi3,sizeof(fi4));
							flink->esc[fi2]=(char)fi4;
							memcpy(&fi4,ln+fi3+2,sizeof(fi4));
							flink->esc[fi2+1]=(char)fi4;
						}

						/* We should be at the locator record for chr 0 -- */
						/* the locator for the header data: */
						def1seek=fontStream.tellg();  /* The 0 (header) locator */
						fi1=8;
						if ( !countedStreamRead( fontStream, ln, fi1) )
							goto db_rf_unrec;

						/* The chr code (better be 0): */
						if (*ln || *(ln+1)) goto db_rf_unrec;
						/* Get the size of the header data: */
						memcpy(&fi1,ln+sizeof(short),sizeof(fi1));
						if (fi1<5) goto db_rf_unrec;
						/* Get the seeker to the header data: */
						memcpy(&thisseek,ln+4,sizeof(thisseek));

						/* Seek and read the header data: */
						if (thisseek<fontStream.tellg() ||
							!safeStreamSeek(fontStream,thisseek) ||
							!countedStreamRead( fontStream, ln, fi1) ) goto db_rf_unrec;

						/* Alloc and capture the desc, and find out how many */
						/* header bytes there are (how many bytes left over): */
						fi2=0; while (fi2<fi1 && ln[fi2]) fi2++;
						fi2++;  /* strlen + 1 of desc (1 for the 0-terminator). */
						fi3=fi1-fi2;  /* # of header bytes */
						if (fi3<4 || fi3>5) goto db_rf_unrec;
						flink->desc=new char[fi2];
						strcpy(flink->desc,ln);

						/* Get the header bytes: */
						flink->above   =ln[fi2];
						flink->below   =ln[fi2+1];
						flink->modes   =ln[fi2+2];
						if (fi3>4) flink->width=ln[fi2+3];  /* Extended bigfont */

						ftype=4; 
						
						flink->format = db_fontlink::BIGFONT;

					} else {  /* See if it's an SHP file. */



		//  Set flink->form in here once the type is known.


						safeStreamSeek( fontStream, 0L);  /* Start over. */

						/* Read lines; 1st non-comment one should start with '*': */
						while (!ftype) {  /* Read lines. */

							/* Read 1 text line: */
							lidx=0;
							while (lidx<lnsz_1 && safeStreamGetc( fontStream, fint1) &&
								fint1 && fint1!='\n'/*DNT*/)
									if (fint1!='\r'/*DNT*/) ln[lidx++]=(char)fint1;
							ln[lidx]=0;
							if (fint1==EOF || lidx>=lnsz_1) goto db_rf_unrec;

							/* Chop comment: */
							fcp1=ln; while (*fcp1 && *fcp1!=';'/*DNT*/) fcp1++;
							*fcp1=0;

							/* Compress out all parentheses: */
							for (fi1=fi2=0; ln[fi2]; fi2++) {
								if (ln[fi2]=='('/*DNT*/ || ln[fi2]==')'/*DNT*/) continue;
								if (fi1!=fi2) ln[fi1]=ln[fi2];
								fi1++;
							}
							ln[fi1]=0;

							/* Start lp at 1st non-space char: */
							lp=ln; while (*lp && isspace((unsigned char) *lp)) lp++;
							if (!*lp) continue;  /* Blank line. */

							if (flink->desc==NULL) {  /* Get desc. */
								if (*lp!='*'/*DNT*/) goto db_rf_unrec;
								while (*lp && *lp!=','/*DNT*/) lp++;
								if (*lp) {
									lp++;
									while (*lp && *lp!=','/*DNT*/) lp++;
									if (*lp) {
										lp++;
										flink->desc=new char[strlen(lp)+1];
										strcpy(flink->desc,lp);
									}
								}
								if (flink->desc==NULL) goto db_rf_unrec;
							} else {  /* Get the header data. */
								/* Require at least 2 commas: */
								for (fcp1=lp,fi1=0; *fcp1 && fi1<2; fcp1++)
									if (*fcp1==','/*DNT*/) fi1++;
								if (fi1<2) goto db_rf_unrec;

								for (fi1=0; *lp && fi1<5; fi1++) {
									fc1=(char)((unsigned char)db_getfontval(&lp));
									switch (fi1) {
										case 0: flink->above   =fc1; break;
										case 1: flink->below   =fc1; break;
										case 2: flink->modes   =fc1; break;
										case 3: flink->encoding=fc1; break;
										case 4: flink->type    =fc1; break;
									}
								}

								if (flink->above<1 ||
									flink->below<0 ||
									(flink->modes && flink->modes!=2))
										goto db_rf_unrec;

								/* Seek pos is ok for reading defs: */
								def1seek=fontStream.tellg();

								ftype=1;
							}

						}  /* End while !ftype */

					}  /* End else trying for SHP file. */

				}  /* End if can fopen the file */

				if (stream && ftype) {

					/* OK so far.  Got header data.  Now read the defs. */
					/* If we fail to recognize the file, set ftype to 0 */
					/* and goto db_rf_unrec. */

					/*
					**  Do each specific file type.
					**  In this switch, if we hit something unrecognized,
					**  set ftype to 0 and goto db_rf_unrec.
					*/
					switch (ftype) {

						case 1:  /* SHP */

		//  Add code to interpret as we read a def (for subshape refs).


							for (pass=0; pass<2; pass++) {
								/*
								**  0 : Determine how big to make the chr array
								**        (256 + 1 element for each chr
								**        whose code is > 255).
								**
								**  1 : Alloc the chr array and fill in the data.
								*/

								/*
								**  Init chr idx for the chrs with codes above 255.
								**  Technically, this could be done just above
								**  the switch (since only pass==1 uses it),
								**  but this is safer: it should not continue
								**  to increment from a previous pass.
								*/
								hiidx=256;


								if (fontStream.eof())
									fontStream.clear();

								/* Start at 1st def: */
								if ( !safeStreamSeek( fontStream,def1seek) )
									{ ftype=0; goto db_rf_unrec; }

								if (pass) {  /* Alloc the chr array. */
									flink->chr=new db_fontchrlink[flink->nchrs];
								} else {
									flink->nchrs=256;
								}

								thiscode=0;  /* Current chr code being processed. */
								nditems=0;   /* # chr of definition items. */
								cidx=0;      /* flink->chr[] idx. */
								didx=0;      /* flink->chr[cidx].def[] idx */
								for (;;) {  /* Read lines. */

									/* Read 1 text line: */
									lidx=0;
									while (lidx<lnsz_1 && safeStreamGetc( fontStream, fint1) &&
										fint1 && fint1!='\n'/*DNT*/)
											if (fint1!='\r'/*DNT*/) ln[lidx++]=(char)fint1;
									ln[lidx]=0;
									if (fint1==EOF) break;
									if (lidx>=lnsz_1) { ftype=0; goto db_rf_unrec; }

									/* Chop comment: */
									fcp1=ln; while (*fcp1 && *fcp1!=';'/*DNT*/) fcp1++;
									*fcp1=0;

									/* Compress out all parentheses: */
									for (fi1=fi2=0; ln[fi2]; fi2++) {
										if (ln[fi2]=='('/*DNT*/ || ln[fi2]==')'/*DNT*/) continue;
										if (fi1!=fi2) ln[fi1]=ln[fi2];
										fi1++;
									}
									ln[fi1]=0;

									/* Start lp at 1st non-space char: */
									lp=ln; while (*lp && isspace((unsigned char) *lp)) lp++;
									if (!*lp) continue;  /* Blank line. */

									if (*lp=='*'/*DNT*/) {  /* Start a new chr. */

										lp++; while (*lp && isspace((unsigned char) *lp)) lp++;
										if (!*lp) { ftype=0; goto db_rf_unrec; }
										if (*lp>='0'/*DNT*/ && *lp<='9'/*DNT*/) {  /* Numeric */
											thiscode=db_getfontval(&lp);
										} else {  /* Non-numeric */
											/* Try for one of the non-UNICODE */
											/* symbolic identifiers: */
											thiscode=0;  /* Ignore this chr if not found. */
											if      (strnisame(lp,"DEGREE_SIGN"/*DNT*/,11))
												thiscode=0x00B0;
											else if (strnisame(lp,"PLUS_OR_MINUS_SIGN"/*DNT*/,18))
												thiscode=0x00B1;
											else if (strnisame(lp,"DIAMETER_SYMBOL"/*DNT*/,15))
												thiscode=0x2205;
											/* Step past next comma: */
											while (*lp && *lp!=',') lp++;
											if (*lp) lp++;
										}
										if (!thiscode) continue;  /* 0 reserved for header line */
										if (!*lp) { ftype=0; goto db_rf_unrec; }
										nditems=db_getfontval(&lp);  /* # of data items. */
										if (nditems<1) { ftype=0; goto db_rf_unrec; }
										didx=0;  /* Init data item idx */

										fi1=(thiscode<0 || thiscode>255);
										if (!pass) {
											if (fi1) flink->nchrs++;
											continue;
										}

										/* Get the chr idx to use: */
										cidx=(fi1) ? hiidx++ : thiscode;

										/* Set the code and alloc the def array: */
										flink->chr[cidx].code=thiscode;
										flink->chr[cidx].defsz=nditems;
										flink->chr[cidx].def=
											new char[flink->chr[cidx].defsz];
										memset(flink->chr[cidx].def,0,
											flink->chr[cidx].defsz);

									} else {  /* Gather data for thiscode. */

										if (!pass || !thiscode || cidx<1 ||
											cidx>=flink->nchrs)
												{ thiscode=0; continue; }
										while (*lp && didx<flink->chr[cidx].defsz)
											flink->chr[cidx].def[didx++]=
												(char)((unsigned char)db_getfontval(&lp));
										if (didx>=flink->chr[cidx].defsz) thiscode=0;

									}

								}  /* End for each line */

							}  /* End for pass */

							break;

						case 2:  /* shape SHX */

							/*
							**  Init chr idx for the chrs with codes above 255.
							*/
							hiidx=256;

							/* Alloc the flink->chr array. */
							/* (flink->nchrs was set above as lendata[] was being set.) */
							flink->chr=new db_fontchrlink[flink->nchrs];

							/* Make sure we're at the 1st def: */

							if (fontStream.eof())
								fontStream.clear();

							if ( !safeStreamSeek( fontStream,def1seek) )
								{ ftype=0; goto db_rf_unrec; }

							for (fi1=0; fi1<nchrsinfile; fi1++) {
								/* Skip code 0 (header data): */
								if (!lendata[fi1].code) {
									int	newPosition = fontStream.tellg() + lendata[fi1].nbytes;
									if ( !safeStreamSeek( fontStream,newPosition) )
										{ ftype=0; goto db_rf_unrec; }
									continue;
								}

								// added by Denis Petrov for support of complex linetypes
								char symbolName[255];
								// added by Denis Petrov for support of complex linetypes
								int symbolNameLen = 0;

								togo=lendata[fi1].nbytes;
								/* Eat the name: */
								while (togo>0) {
									safeStreamGetc( fontStream, fint1);
									// added by Denis Petrov for support of complex linetypes
									symbolName[symbolNameLen] = (char)fint1;
									// added by Denis Petrov for support of complex linetypes
									++symbolNameLen;
									togo--;
									if (fint1==0 || fint1==EOF) break;
								}
								// added by Denis Petrov for support of complex linetypes
								symbolName[symbolNameLen] = 0;
								// added by Denis Petrov for support of complex linetypes
								++symbolNameLen;

								if (fint1==EOF) { ftype=0; goto db_rf_unrec; }
								if (togo>0) {
									cidx=(lendata[fi1].code<0 ||
										  lendata[fi1].code>255) ?
											hiidx++ : lendata[fi1].code;
									flink->chr[cidx].code=lendata[fi1].code;
									flink->chr[cidx].defsz=togo;

									// added by Denis Petrov for support of complex linetypes
									delete[] flink->chr[cidx].symbolName;
									// added by Denis Petrov for support of complex linetypes
									flink->chr[cidx].symbolName = new char[symbolNameLen];
									// added by Denis Petrov for support of complex linetypes
									strcpy(flink->chr[cidx].symbolName, symbolName);

									/* Delete any old def (precaution against */
									/* memory leaks caused by duplicates): */
									delete [] flink->chr[cidx].def;
									flink->chr[cidx].def=new char[flink->chr[cidx].defsz];
									memset(flink->chr[cidx].def,0,flink->chr[cidx].defsz);
									for (didx=0; didx<flink->chr[cidx].defsz &&
										safeStreamGetc( fontStream, fint1); didx++)
											flink->chr[cidx].def[didx]=(char)fint1;
									if (fint1==EOF) { ftype=0; goto db_rf_unrec; }
								}
							}

							break;

						case 3:  /* unifont SHX */

							for (pass=0; pass<2; pass++) {
								/*
								**  0 : Determine how big to make the chr array
								**        (256 + 1 element for each chr
								**        whose code is > 255).
								**
								**  1 : Alloc the chr array and fill in the data.
								*/

								/*
								**  Init chr idx for the chrs with codes above 255.
								**  Technically, this could be done just above
								**  the switch (since only pass==1 uses it),
								**  but this is safer: it should not continue
								**  to increment from a previous pass.
								*/
								hiidx=256;

								if (pass) {  /* Alloc the chr array. */
									flink->chr=new db_fontchrlink[flink->nchrs];
								} else {
									flink->nchrs=256;
								}

								/* Read each def: */
								if (fontStream.eof())
									fontStream.clear();

								for (
										thisseek=def1seek; 
										safeStreamSeek( fontStream,thisseek); 
										thisseek=nextseek
										) {

									/* Don't "continue" until we set nextseek. */

									/* Read chr code and jumper: */
									if ( !countedStreamRead( fontStream, ln, 4 ) )
										break;  /* EOF?  Should have caught during seek. */

									memcpy(&thiscode,ln,sizeof(short));
									memcpy(&fi1,ln+sizeof(short),sizeof(short));
									if (fi1<1) { ftype=0; goto db_rf_unrec; }
									nextseek=thisseek+fi1+4;  /* Add 4 for the code and */
															  /* jumper bytes themselves. */

									fi1=(thiscode<0 || thiscode>255);
									if (!pass) {
										if (fi1) flink->nchrs++;
										continue;
									}

									/* Get the chr idx to use: */
									cidx=(fi1) ? hiidx++ : thiscode;

									/* Eat the chr desc: */
									while ( safeStreamGetc( fontStream, fint1) && fint1 != 0 )
										;

									if (fint1==EOF) { ftype=0; goto db_rf_unrec; }
									nditems=(short)(nextseek-fontStream.tellg());  /* # of data items. */
									if (nditems<1) { ftype=0; goto db_rf_unrec; }

									db_fontchrlink *fclptr=&flink->chr[cidx];
									/* Set the code and alloc the def array: */
									fclptr->code=thiscode;
									fclptr->defsz=nditems;

									/* Delete any old def (precaution against */
									/* memory leaks caused by duplicates): */
									delete [] fclptr->def;
									fclptr->def=new char[fclptr->defsz];
	//                                memset(flink->chr[cidx].def,0,flink->chr[cidx].defsz);
									bool ret=countedStreamRead( fontStream, fclptr->def, nditems);

									/* Read the def bytes: */
	//                                for (didx=0; didx<nditems &&
	//                                    safeStreamGetc( fontStream, fint1); didx++)
	//                                        flink->chr[cidx].def[didx]=(char)fint1;
									if (ret==false) { ftype=0; goto db_rf_unrec; }

								}  /* End for each def */



							}  /* End for pass */

							break;

						case 4:  /* bigfont SHX */

						  {
							short locidx,recsz;
							long savlocpos;

							for (pass=0; pass<2; pass++) {
								/*
								**  0 : Determine how big to make the chr array
								**        (256 + 1 element for each chr
								**        whose code is > 255).
								**
								**  1 : Alloc the chr array and fill in the data.
								*/

								/*
								**  Init chr idx for the chrs with codes above 255.
								**  Technically, this could be done just above
								**  the switch (since only pass==1 uses it),
								**  but this is safer: it should not continue
								**  to increment from a previous pass.
								*/
								hiidx=256;

								if (pass) {  /* Alloc the chr array. */
									flink->chr=new db_fontchrlink[flink->nchrs];
								} else {
									flink->nchrs=256;
								}

								/* Read each def.  (This time, def1seek refers */
								/* to the locators -- not the actual definitions.) */

								if (fontStream.eof())
									fontStream.clear();

								if ( !safeStreamSeek( fontStream, def1seek) )
									{ ftype=0; goto db_rf_unrec; }

								for (locidx=0; locidx<nchrsinfile; locidx++) {

									/* Read a locator rec: */
									if ( !countedStreamRead( fontStream, ln, 8 ) )
										{ ftype=0; goto db_rf_unrec; }

									/* Get chr code.  If it's 0, it's either the */
									/* header or a blank record.  Either way, cont: */
									memcpy(&thiscode,ln,sizeof(short));
									if (!thiscode) continue;

									/* Get the record size and seeker: */
									memcpy(&recsz,ln+2,sizeof(short));
									memcpy(&thisseek,ln+4,sizeof(thisseek));
									if (recsz<2 || thisseek<0L)
										{ ftype=0; goto db_rf_unrec; }

									fi1=(thiscode<0 || thiscode>255);
									if (!pass) {
										if (fi1) flink->nchrs++;
										continue;
									}

									/* Get the chr idx to use: */
									cidx=(fi1) ? hiidx++ : thiscode;

									/* Save our place in the locator recs: */
									savlocpos=fontStream.tellg();

									/* Seek the name and def data: */
									if (!safeStreamSeek( fontStream, thisseek) )
										{ ftype=0; goto db_rf_unrec; }

									/* Eat the chr desc: */
									for (fi1=1; fi1<recsz &&
										safeStreamGetc( fontStream, fint1) && fint1 != 0; fi1++);
									if (fint1==EOF) { ftype=0; goto db_rf_unrec; }
									nditems=recsz-fi1;  /* # of data items. */
									if (nditems<1) { ftype=0; goto db_rf_unrec; }

									/* Set the code and alloc the def array: */
									flink->chr[cidx].code=thiscode;
									flink->chr[cidx].defsz=nditems;
									/* Delete any old def (precaution against */
									/* memory leaks caused by duplicates): */
									delete [] flink->chr[cidx].def;
									flink->chr[cidx].def=new char[flink->chr[cidx].defsz];
									memset(flink->chr[cidx].def,0,flink->chr[cidx].defsz);

									/* Read the def bytes: */
									for (didx=0; didx<nditems &&
										safeStreamGetc( fontStream, fint1); didx++)
											flink->chr[cidx].def[didx]=(char)fint1;
									if (fint1==EOF) { ftype=0; goto db_rf_unrec; }

									/* Back to the locator recs: */
									safeStreamSeek( fontStream, savlocpos);

								}  /* End for each def */

							}  /* End for pass */

							break;

						  }

					}  /* End switch(ftype) */

				}  /* End if ok to read defs */

			  }  /* End if readit */


	db_rf_unrec:  /* A quick exit from above when file not recognized. */

			  if (useff) {
				  change1st=1;  /* Try next extension. */
			  } else {  /* 1st pass only */
				  useff=1;      /* Begin doing the findfiling. */
				  change1st=0;  /* Don't change the extension yet. */
			  }

		  } while (!ftype);  /* End main file-determining loop. */

		  nextreadit: ;

		}  /* End for readit */
	} // end else

    if (foundpn!=NULL) strcpy(foundpn,fullpn);

    if (fontpp==NULL) goto out;  /* flink will be freed in "out". */

    /* Set flink->lffact and flink->spfact: */
    db_getfontspacing(flink,&flink->lffact,&flink->spfact);

    /*
    **  If we made *flink and read the data from file, add it to
    **  dp->fontll.  First, make sure it isn't already there.
    **  (This is primarily to avoid adding icad.fnt multiple times,
    **  but it's a good idea anyway.  I think the logic has an
    **  inefficiency in that it reads icad.fnt every time it can't
    **  find the requested font.  Shouldn't be a problem unless
    **  there are LOTS of unlocatable fonts for some reason).
    */
    if (madeflink && flink!=NULL && flink->pn!=NULL &&
        *flink->pn && dp!=NULL) {

        for (tfp1=dp->ret_fontll(); tfp1!=NULL &&
            stricmp(flink->pn,tfp1->pn); tfp1=tfp1->next);
        if (tfp1==NULL) {  /* Not found; add it. */
            tfp1=dp->ret_fontll(); flink->next=tfp1; dp->set_fontll(flink);
        } else {  /* Already there; free flink and point it at the existing link. */
            delete flink; flink=tfp1;
        }
        added2fontll=1;  /* In EITHER case, so we don't free it. */
    }


out:
    if (trying4tol && !dp->ret_cantfindtolfont()) {
        if (flink==NULL || lastditch) dp->set_cantfindtolfont(true);
        else dp->set_tolfontptr(flink);
    }

    delete stream;
    if (ln     !=NULL) delete [] ln;
    if (filenm !=NULL) delete [] filenm;
    if (fullpn !=NULL) delete [] fullpn;
    if (lendata!=NULL) delete [] lendata;
    if (madeflink && !added2fontll) { delete flink; flink=NULL; }
    if (fontpp!=NULL) *fontpp=(trying4tol && (dp->ret_cantfindtolfont() || lastditch)) ?
        NULL : flink;

    return (!rc && lastditch) ? +1 : rc;
}


/******************* LOCAL DEFINITIONS ****************/

struct db_fontmaplink 
	{
						db_fontmaplink( TCHAR *old, TCHAR *mapped);
						~db_fontmaplink();
	const LPCTSTR		getOldname() {return oldname;}
	const LPCTSTR		getNewname() {return newname;}

    db_fontmaplink *next;

private:
    LPTSTR	oldname;
    LPTSTR	newname;
	};


static db_fontmaplink *db_fontmap=NULL, *lastlink=NULL;
void (*db_fontsubstfn)(char *looked4, char *found);

	// create map entry from old/new names

db_fontmaplink:: db_fontmaplink( TCHAR *old, TCHAR *mapped)
			: next( NULL)
	{
	// If the old name has an extension, it shouldn't, so discard it
	TCHAR	fname[ _MAX_FNAME];
	_tsplitpath( old, NULL, NULL, fname, NULL);
	
	oldname = new char[ _tcslen( fname)+1];
	newname = new char[ _tcslen( mapped)+1];
	
	if ( oldname && newname )
		{
		_tcscpy( oldname, fname);
		_tcscpy( newname, mapped);
		}
	else
		{
		delete [] oldname;
		delete [] newname;
		oldname = newname = NULL;
		}

	        
	if (db_fontmap==NULL) 
		db_fontmap = this;
	else
		lastlink->next = this;

    lastlink = this;
	}


db_fontmaplink:: ~db_fontmaplink()
	{
	delete [] oldname;
	delete [] newname;
	}



/*F*/
DB_API int db_setfontmap(char *pn) 
	{
/*
**  Creates db_fontmap from the file specified by pn, replacing
**  the old one.
**
**  Returns:
**       0 : OK
**      -1 : Calling error (pn==NULL)
**      -2 : Can't find/open pn
**      -3 : Doesn't appear to be a fontmap file
**      -4 : No memory
*/
    char ln[IC_ACADBUF],*scp;
    int ok2free,rc;
    db_fontmaplink *newlink;

    rc=0;  newlink=NULL; ok2free=0;


	istream *stream = NULL;

	// locate the default font resource
	if ( pn == NULL )
		{
		stream = BinaryResource( IDR_ICADFMP ).NewIstream();
		}
	else
		{
		ifstream	*fileStream = new ifstream();
		fileStream->open( pn, ios::nocreate | ios::binary );	// Yes, open as binary so it is the same form as the resource
		if ( fileStream && fileStream->is_open() )
			{
			stream = fileStream;
			}
		}
	if ( stream == NULL )
		{
		rc=-2; 
		goto out; 
		}

    db_freefontmap();  /* Free the old one. */

    ok2free=1;

    while (!stream->eof())
		{
		stream->get(ln,sizeof(ln)-1,'\r');	// read to the line terminator

		// stream is binary, must get rid of CRLF combo (2 chars)
#ifdef _DEBUG
		if (!stream->eof())
			{
			int c = stream->get(); 
			ASSERT( c == '\r');
			c = stream->get(); 
			ASSERT( c == '\n');
			}
#else
		if ( !stream->eof())
			stream->ignore( 2 );
#endif

        if (strlen(ln)>=sizeof(ln)-2) 
			{ 
			rc=-3; 
			goto out; 
			}

        ic_trim(ln,"be"/*DNT*/);
        if (!*ln || *ln==';') continue;  /* Semicolon marks a comment line */
                                         /* only if it is the first non-space */
                                         /* character. */
        /* Now REQUIRE a semicolon: the separator: */
        for (scp=ln; *scp && *scp!=';'; scp++);
        if (!scp[0] || !scp[1]) 
			{ 
			continue;
			}
        *scp=0;  /* Break the string at the semicolon. */

		if ( !(newlink = new db_fontmaplink( ln, scp+1)) )
            { 
			rc=-4; 
			goto out; 
			}
    }
    if (db_fontmap==NULL) 
		{ 
		rc=-3; 
		goto out; 
		}

out:
	if ( stream != NULL )
		{
		delete stream;
		}
    if (rc && ok2free) 
		{
		db_freefontmap();
		}

    return rc;
	}



/*F*/
DB_API void db_freefontmap(void) {
    db_fontmaplink *tp;

    while (db_fontmap!=NULL) {
        tp=db_fontmap->next;
        delete db_fontmap;
        db_fontmap=tp;
    }
}


/*F*/
DB_API int db_setfontusingmap(
    char         *fontname,
    char         *foundpn,
    db_fontlink **fontpp,
    db_fontsubstmode fontsubstmode,
	db_drawing   *dp) {
/*
**  db_setfont() is so complex already that I decided to write this
**  function to use the fontmap logic, rather than trying to put
**  another loop around everything in db_setfont().  (I started to
**  do that, but got WAY confused about how to do it.)  This function
**  calls db_setfont(), trying first the name from the fontmap, then,
**  if that fails, the original fontname.  It's basically a wrapper
**  for db_setfont().
**
**  Returns the same codes as db_setfont().
*/
    char *frommap;
    char *npx[2];  /* Name Plus Extension (no path): 0 : Sought; */
                   /*   1 : the one we're using. */
    int rc,fi1,fi2;


    rc=0; frommap=npx[0]=npx[1]=NULL;


    if ( fontpp != NULL)
		{
		*fontpp=NULL;
		}

    if (foundpn!=NULL) 
		*foundpn=0;

    if (fontname==NULL) { rc=-1; goto out; }

	frommap = db_usefontmap(fontname);
	if ( frommap == NULL )
		{
		rc = db_setfont( fontname, foundpn, fontpp, dp );
		}
	else
		{
		rc = db_setfont( frommap, foundpn, fontpp, dp );
		if ( rc != 0 )
			{
			rc = db_setfont( fontname, foundpn, fontpp, dp );
			}
		}

	// If we still didn't get it, try one more thing.  Tack on the
	// path of the drawing
	//
	if ( (rc != 0) &&
		 (dp->ret_pn() != NULL))
		{
		char fontwithdrawingpath[ _MAX_PATH ];
		strncpy( fontwithdrawingpath, fontname, _MAX_PATH );
		if ( ic_setpath( fontwithdrawingpath, dp->ret_pn() ) )
			{
			ic_setext( fontwithdrawingpath, "shx"/*DNT*/);
			rc = db_setfont( fontwithdrawingpath, foundpn, fontpp, dp );
			}
		}

    /* Get the filename we sought (npx[0]): */
    for (fi1=0; fontname[fi1]; fi1++);
    while (fi1>-1 && fontname[fi1]!=IC_SLASH) fi1--;
    fi1++;
    if ((npx[0]= new char [strlen(fontname+fi1)+1])==NULL)
        { rc=-3; goto out; }
    strcpy(npx[0],fontname+fi1); ic_trim(npx[0],"be"/*DNT*/);

    /* Get the filename we ended up using (npx[1]): */
    if (*fontpp!=NULL && (*fontpp)->pn!=NULL) {
        for (fi1=0; (*fontpp)->pn[fi1]; fi1++);
        while (fi1>-1 && (*fontpp)->pn[fi1]!=IC_SLASH) fi1--;
        fi1++;
        if ((npx[1]= new char [strlen((*fontpp)->pn+fi1)+1])==NULL)
            { rc=-3; goto out; }
        strcpy(npx[1],(*fontpp)->pn+fi1); ic_trim(npx[1],"be"/*DNT*/);
    }

	// If we still have nothing, add a map entry for the font to ICAD.FNT
	// so we don't look for it again
	if ( rc EQ 1 && fontsubstmode==IC_ALLOW_ICAD_FNT)
		{
								// new link is automatically added to chain
		db_fontmaplink	*newlink = new db_fontmaplink( fontname, (*fontpp)->pn + fi1);
		}

    /* If we had something to look for and found nothing or used */
    /* something different -- and the call-back function is non-NULL -- */
    /* use the call-back to tell about the problem: */
    if (npx[0]!=NULL && *npx[0] && (npx[1]==NULL || stricmp(npx[0],npx[1])) &&
        db_fontsubstfn!=NULL) {
        /*
        **  Further refinements:
        **
        **  If all we did is add an extension to a filename that
        **  didn't have one, don't report it.
        **
        **  If it's the TOLERANCE font, only report when we've substituted
        **  an entirely different font (like icad.fnt).
        */
        int reportit,dotpos[2],samefn,itstol[2];

        reportit=1; dotpos[0]=dotpos[1]=-1;

        if (npx[1]!=NULL) {
            for (fi1=0; fi1<2; fi1++) {
                for (fi2=0; npx[fi1][fi2]; fi2++);
                for (fi2--; fi2>-1 && dotpos[fi1]<0; fi2--) {
                    if (npx[fi1][fi2]=='.')
                        { dotpos[fi1]=fi2; npx[fi1][dotpos[fi1]]=0; }
                }
            }
            samefn=strisame(npx[0],npx[1]);
            itstol[0]=(strisame(npx[0],db_tolfontnm) ||
                    strisame(npx[0],db_ourtolfontnm));
            itstol[1]=(strisame(npx[1],db_tolfontnm) ||
                    strisame(npx[1],db_ourtolfontnm));
            if (dotpos[0]>-1) npx[0][dotpos[0]]='.';
            if (dotpos[1]>-1) npx[1][dotpos[1]]='.';
            if ((itstol[0] && itstol[1]) || (samefn && dotpos[0]<0)) reportit=0;
        }

        if (reportit && (!rc || fontsubstmode==IC_ALLOW_ICAD_FNT)) (*db_fontsubstfn)(
            (npx[0]!=NULL) ? npx[0] : db_str_quotequote,
            (npx[1]!=NULL) ? npx[1] : db_str_quotequote);
    }

out:
    if (frommap!=NULL) delete [] frommap;
    if (npx[0]!=NULL)  delete [] npx[0];
    if (npx[1]!=NULL)  delete [] npx[1];

    return rc;
}


/*F*/
DB_API char *db_usefontmap(char *oldpn) {
/*
**  Given a font file pathname (pn), this function uses the current
**  fontmap (db_fontmap) to replace the old FILEname with
**  the new one leaving the path the same.  In finding the old one
**  in the fontmap, only the extensionless fontname is used.
**  (ACAD fontmap lines seem to have the format "old;new.ext",
**  where "old" has no extension and "new" does.)
**
**  Returns a pointer to an allocated string holding the result --
**  or NULL if no match is found for any reason.  (THE CALLER
**  MUST FREE IT (using free()).
*/
    char *rs,*woldpn;
    int widx1;
    db_fontmaplink *tfmp1;


    rs=woldpn=NULL;


    if (oldpn==NULL || (woldpn= new char [strlen(oldpn)+1])==NULL) goto out;
    strcpy(woldpn,oldpn); ic_trim(woldpn,"be"/*DNT*/);
    if (!*woldpn) goto out;

    /* Set widx1 to the 1st char of the filename and chop at the */
    /* extension dot: */
    for (widx1=0; woldpn[widx1]; widx1++);
    for (; widx1>-1 && woldpn[widx1]!=IC_SLASH; widx1--)
        if (woldpn[widx1]=='.') woldpn[widx1]=0;
    widx1++;

    /* Look for the replacement value: */
    for (tfmp1=db_fontmap; tfmp1!=NULL && (tfmp1->getOldname()==NULL ||
        stricmp(tfmp1->getOldname(),woldpn+widx1)); tfmp1=tfmp1->next);

    if (tfmp1==NULL) goto out;

    /* Build the new pathname string: */
    woldpn[widx1]=0;
    rs= new char [strlen(woldpn)+strlen(tfmp1->getNewname())+1];
    strcat(strcpy(rs,woldpn),tfmp1->getNewname());

out:
    if (woldpn!=NULL) delete [] woldpn;
    return rs;
}


/*F*/
DB_API void db_setfontsubstfn(void (*fn)(char *looked4, char *found)) {
    /* To set up a call-back so that db_setfontusingmap() can call it */
    /* whenever a font is substituted for another that was */
    /* originally requested. */

    db_fontsubstfn=fn;
}


/*F*/
void db_getfontspacing(
    db_fontlink *font,
    sds_real    *lffactp,
    sds_real    *spfactp) {
/*
**  Given a font ptr, sets *spfactp to the delta-X for a space chr
**  and *lffactp to the delta-Y for a line-feed (newline) chr.  Both
**  are expressed as POSITIVE fractions of text height; multiply by text
**  height to get the true spacings for that text height.
**
**  Any of the pass parameters can be NULL if they aren't needed.
**
**  If font==NULL or there's a problem reading the font data,
**  TXT's values are used.
**
**  NOTE: The font def reader in here is very crude.  It stops at the
**  first 14 (process next command only if vertical) and understands
**  only penup/down, vector, displacement, and scaling commands.
*/
    int fidx,cidx,didx,vlen,done,err,fi1;
    sds_real fact[2],offset[2],scl,dx,dy;


    fact[0]=fact[1]=0.0;  /* Will trigger defaults if never changed. */

    if (font==NULL || font->chr==NULL || font->nchrs<33 ||
        font->above<1) goto out;

    for (fidx=0,cidx=10; fidx<2; fidx++,cidx=32) {

        offset[0]=offset[1]=0.0; scl=1.0; done=err=0;

        if (font->chr[cidx].def==NULL || font->chr[cidx].defsz<1) continue;
        for (didx=0; didx<font->chr[cidx].defsz; didx++) {
            dx=dy=0.0;
            if ((vlen=((unsigned char)(font->chr[cidx].def[didx]))>>4)>0) {
                /* Vector of length vlen. */
                switch (font->chr[cidx].def[didx]&'\x0F'/*DNT*/) {
                    case  0: dx= 1.0; dy= 0.0; break;
                    case  1: dx= 1.0; dy= 0.5; break;
                    case  2: dx= 1.0; dy= 1.0; break;
                    case  3: dx= 0.5; dy= 1.0; break;
                    case  4: dx= 0.0; dy= 1.0; break;
                    case  5: dx=-0.5; dy= 1.0; break;
                    case  6: dx=-1.0; dy= 1.0; break;
                    case  7: dx=-1.0; dy= 0.5; break;
                    case  8: dx=-1.0; dy= 0.0; break;
                    case  9: dx=-1.0; dy=-0.5; break;
                    case 10: dx=-1.0; dy=-1.0; break;
                    case 11: dx=-0.5; dy=-1.0; break;
                    case 12: dx= 0.0; dy=-1.0; break;
                    case 13: dx= 0.5; dy=-1.0; break;
                    case 14: dx= 1.0; dy=-1.0; break;
                    case 15: dx= 1.0; dy=-0.5; break;
                }
                dx*=vlen; dy*=vlen;
            } else {
                switch (font->chr[cidx].def[didx]) {
                    case  0:    /* End */
                        done=1;
                        break;
                    case  1:    /* Pen down */
                        /* Ignore */
                        break;
                    case  2:    /* Pen up */
                        /* Ignore */
                        break;
                    case  3:    /* Div by */
                        fi1=((unsigned char)(font->chr[cidx].def[++didx]));
                        if (fi1) scl/=fi1;
                        break;
                    case  4:    /* Mult by */
                        fi1=((unsigned char)(font->chr[cidx].def[++didx]));
                        if (fi1) scl*=fi1;
                        break;
                    case  8:    /* XY displacement */
                        dx=font->chr[cidx].def[++didx];
                        dy=font->chr[cidx].def[++didx];
                        break;
						// DP: special code 9 also often occurs in definition of these symbols 
					case  9:
						while(font->chr[cidx].def[didx + 1] != 0 || font->chr[cidx].def[didx + 2] != 0)
						{
	                        dx += font->chr[cidx].def[++didx];
		                    dy += font->chr[cidx].def[++didx];
						}
						break;
                    case 14:    /* Next cmd only if vertical */
                        done=1;
                        break;
                    default:    /* One we're not handling here. */
                        err=done=1;
                        break;
                }
            }
            if (done) break;
            offset[0]+=scl*dx; offset[1]+=scl*dy;
        }

        if (err) break;
        fact[fidx]=fabs(offset[!fidx]/font->above);
    }

out:
    /* Use TXT's values if something's seriously wrong: */
    if (err || fact[0]<0.01 || fact[0]>100.0) fact[0]=5.0/3.0;
    if (err || fact[1]<0.01 || fact[1]>100.0) fact[1]=1.0;

    if (lffactp!=NULL) *lffactp=fact[0];
    if (spfactp!=NULL) *spfactp=fact[1];
}


/*F*/
short db_getfontval(char **lpp) {
/*
**  This function is an extension of db_setfont() (for the SHP reading).
**
**  Reads a hex or decimal number from a char buffer, starting
**  at *lpp and then advances *lpp until it points to the first
**  non-space char after a comma (or until it points to the 0
**  string terminator).  Expects the string to be free of comments
**  and parentheses by this time.  (Font SHP files can have
**  parentheses for clarification.)
**
**  Hex numbers are indicated by an initial '0' (after the optional sign).
**  A maximum of 4 hex digits are taken; the rest are ignored.
**
**  Signs:
**
**      The number may begin with a '+' or '-' sign.  (ACAD uses a '-'
**      in places (the arc specifiers); we're adding '+' as a "just in
**      case".  For neg decimal values, we just negate the atoi()
**      of what follows the '-' (see the logic). The interesting case is
**      a neg HEX specifier.  The ACAD SHX compiler seems to turn on the
**      high bit in the char that follows -- not negate what follows.
**      Example: "-032" generates 0xB2 -- not 0xCE:
**
**          "032" is                      00110010 (0x32, c  50, s  50)
**          The negative is               11001110 (0xCE, c -50, s 206)
**          "032" with the high bit on is 10110010 (0xB2, c -78, s 178)
**
**      ACAD generates that final case (just turns on the high bit),
**      so that's what we have to do here.
**
**      Now another twist: this function has to handle shorts -- not
**      just chars (chr codes like *02205 for the diameter symbol,
**      for example).  Therefore, to try to be flexible, this function
**      sets bit 7 if the negative hex code has 2 or less hex chars
**      and bit 15 if the negative hex code has 3 or 4 more hex chars.
**      That is, "-032" will return 0000 0000 1011 0010, but "-0432"
**      will return 1000 0100 0011 0010 (as shorts, of course).
**
**      IF THE CALLER STUFFS THE RETURN VALUE INTO A char (WHICH IS WHAT
**      WE WANT IN THE CHR DEFINITION AREA), IT SHOULD CAST THE RETURN
**      VALUE TO AN unsigned char FIRST TO CHOP THE BITS AND AVOID SIGN
**      EXTENSION PROBLEMS.
**
**  Returns:
**      The number read from the string (or 0 if any problems occur).
*/
    char *lp;
    short rc,neg,fi1,fi2;


    rc=neg=0;


    if (lpp==NULL) goto out;

    lp=*lpp;  /* Convenience */

    while (*lp && isspace((unsigned char) *lp)) lp++;
    if (*lp=='+') lp++; else if (*lp=='-') { neg=1; lp++; }
    if (!*lp) goto out;
    if (*lp=='0'/*DNT*/) {  /* Hex */
        lp++;
        for (fi1=0; fi1<4; fi1++,lp++) {
            *lp=toupper(*lp);
            if (*lp>='A'/*DNT*/ && *lp<='F'/*DNT*/) {
                fi2=*lp-'A'/*DNT*/+10;
            } else if (*lp>='0'/*DNT*/ && *lp<='9'/*DNT*/) {
                fi2=*lp-'0'/*DNT*/;
            } else break;  /* Not a hex digit */
            rc<<=4; rc|=fi2;
        }
        if (neg) rc|=((fi1<3) ? 0x0080 : 0x8000);
    } else {  /* Decimal */
        rc=atoi(lp); if (neg) rc=-rc;
    }

    while (*lp && *lp!=',') lp++;
    if (*lp) { lp++; while (*lp && isspace((unsigned char) *lp)) lp++; }


out:
    if (lpp!=NULL) *lpp=lp;
    return rc;
}

int db_findCodeByName(const db_fontlink* pFont, const char* pName)
{
	assert(pFont != NULL);
	assert(pName != NULL);

	for(int i = 0; i < pFont->nchrs; ++i)
		if(pFont->chr[i].symbolName && strnicmp(pFont->chr[i].symbolName, pName, IC_ACADBUF) == 0)
			return pFont->chr[i].code;
	return 0;
}

int db_findNameByCode(char* pName, int code, const db_fontlink* pFont)
{
	assert(pFont != NULL);
	assert(pName != NULL);

	for(int i = 0; i < pFont->nchrs; ++i)
		if(pFont->chr[i].code == code && pFont->chr[i].symbolName)
		{
			strcpy(pName, pFont->chr[i].symbolName);
			return 1;
		}
	return 0;
}



