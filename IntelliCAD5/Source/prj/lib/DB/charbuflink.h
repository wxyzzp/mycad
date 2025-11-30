/* C:\ICAD\PRJ\LIB\DB\CHARBUFLINK.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

/*
**  A char buffer and its allocated size.  Used for system variables
**  buffers (and anything else needing a char buffer).
*/

#ifndef _INC_CHARBUFLINK
#define _INC_CHARBUFLINK

class DB_CLASS db_charbuflink {

  public:

    db_charbuflink(void);
    db_charbuflink(int sz, int clearit);
   ~db_charbuflink(void);

    void newbuf(int sz, int clearit) {
        delete [] buf; delete [] bufcopy; buf=bufcopy=NULL;
        size=seekof0=0; next=NULL;
        if (sz<1) return;
        size=sz; buf=new char[size];
        if (clearit) memset(buf,0,size);
    }

    void copybuf(void) {
        if (buf==NULL || size<1) return;
        if (bufcopy==NULL) bufcopy=new char[size];
        memcpy(bufcopy,buf,size);
    }
    void restorebuf(void) {
        if (bufcopy!=NULL && buf!=NULL && size>0) memcpy(buf,bufcopy,size);
        delete [] bufcopy; bufcopy=NULL;
    }
    void freebufcopy(void) { delete [] bufcopy; bufcopy=NULL; }

    char *buf;      /* The buffer */
    char *bufcopy;  /* For the save/restore "snap-shot" features. */
    int   size;     /* The allocated size */
    int   seekof0;  /* For file-reading buffers, seek position in the file */
                    /*   that is associated with byte 0 in the buffer */
                    /*   (the file position at the time of the fread() that */
                    /*   filled buf).  (Relates buf idx to file position.) */
    db_charbuflink *next;
};

#endif


