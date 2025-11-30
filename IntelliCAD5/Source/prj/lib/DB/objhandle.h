/* C:\ICAD\PRJ\LIB\DB\OBJHANDLE.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
#ifndef INC_OBJHANDLE_H
#define INC_OBJHANDLE_H

// this is in here so that we can make objhandle.h self-contained
#ifndef DB_CLASS
	#if defined( DBDLL )
		#define	DB_CLASS __declspec( dllexport)
	#else
		#if defined( DBSTATIC )
			#define DB_CLASS
		#else
			#define	DB_CLASS __declspec( dllimport)
		#endif // DBSTATIC
	#endif // DBDLL
#endif // #ifndef DB_CLASS


class DB_CLASS db_objhandle
	{
friend int ObjhandleCompare(db_objhandle *a, db_objhandle *b);  // for qsort, bsearch, etc.

public:
	db_objhandle(void) { handle[0]=handle[1]=0L; }
	db_objhandle(const int num)  // to allow comparisons to NULL -- value is not interpreted
		{
		handle[0]=handle[1]=0L;
		}


	db_objhandle(const db_objhandle & source)
		{
		handle[0] = source.handle[0];
		handle[1] = source.handle[1];
		}

	db_objhandle(const db_objhandle & source, int i)
		{
		handle[0] = source.handle[0] + i;
		handle[1] = source.handle[1];
		}

	
	db_objhandle(const char *ahan);
	
	db_objhandle(unsigned char odhan[8])
		{
		short i;
		unsigned char *hptr=(unsigned char *)&handle[0];

		for (i=7; i>=0; i--)  // reverse it into our structure
			{
			*hptr++=odhan[i];
			}
		}

	~db_objhandle(void) {};

	void operator=(const db_objhandle& ohandle) 
		{ if (this == &ohandle) return;
		  handle[0]=ohandle.handle[0]; 
	      handle[1]=ohandle.handle[1]; } ;

	bool operator == (const db_objhandle& ohandle) const
		{ 
		if (handle[0]==ohandle.handle[0] && handle[1]==ohandle.handle[1])
			return true;
		return false;
		}

	bool operator != (const db_objhandle& ohandle) const
		{ 
		if (handle[0]!=ohandle.handle[0] || handle[1]!=ohandle.handle[1])
			return true;
		return false;
		}

	bool operator < (const db_objhandle& ohandle) const
		{
		if (handle[1]!=ohandle.handle[1]) 
			{
			if (handle[1]<ohandle.handle[1])
				return true;
			return false;
			}
		else 
			{
			if (handle[0]<ohandle.handle[0])
				return true;
			return false;
			}
		}

	bool operator > (const db_objhandle& ohandle) const
		{
		if (handle[1]!=ohandle.handle[1]) 
			{
			if (handle[1]>ohandle.handle[1])
				return true;
			return false;
			}
		else 
			{
			if (handle[0]>ohandle.handle[0])
				return true;
			return false;
			}
		}

	operator ++ (void)
		{
		handle[0]++;
		if (handle[0]==0L) handle[1]++;
		}


	void ToAscii(char *ahan);

	__int64 ToHugeInt();

	void ToODHandle(unsigned char odhan[8])
		{
		short i;
		unsigned char *hptr=(unsigned char *)&handle[0];

		for (i=7; i>=0; i--)  // reverse it out of our structure
			{
			odhan[i]=*hptr++;
			}
		}
	void Nullify(void)
		{
		handle[0]=handle[1]=0L;
		}

private:

	DWORD handle[2];
	} ;

#endif
