/* COMMANDATOM.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Defines the commandAtomObj class
 * 
 */ 

 #ifndef _COMMANDATOM_H
 #define _COMMANDATOM_H
 
//  The following class is for commandAtomList of defined functions and
//  variables (commandAtomList).  It contains a super resbuf -- a resbuf
//  forced to store types that ADS never intended.  Don't sds_relrb()
//  this resbuf.  See the notes at the top of lisp.c in icadlib.
//
class commandAtomObj : public CObject
	{

	// CTORS and DTORS
	//
public:	
    commandAtomObj()  
    	{ 
    	LName     = NULL;
		GName     = NULL;
        m_funcptr  = NULL;
        ThreadID = 0;
        Thread   = 0;
        id       = 0;
		parensReqd = TRUE;
        rb.restype = RTNIL;
        rb.rbnext= NULL;
		m_bVbaFunction = FALSE;
		m_szHelpFile = NULL;
		m_szContextID = NULL;
		}
        
    ~commandAtomObj() 
    	{ 
    	void lsp_freesplrb(struct resbuf *rbp);
    	
        if (LName != NULL) 
        	{
        	delete(LName); 
        	LName = NULL;
        	}
		if (GName != NULL) 
			{
			delete(GName); 
			GName = NULL;
			}
		lsp_freesplrb(&rb);
		delete[]m_szHelpFile;
		delete[]m_szContextID;
		}

	// ---------------------------------------	
	// OPERATIONS
	//	
public:                    
    bool HasFuncPtr( void )
	    {  
	    return (m_funcptr != NULL);
	    }
	
	bool CallFunction( void );

	void SetFuncPtr( int (*fptr)(void) )
		{
		m_funcptr = fptr;
		}    

	bool IsVbaFunction( void )
		{
		return m_bVbaFunction;
		}

	void SetVbaFunction( void )
		{
		m_bVbaFunction = TRUE;
		}

	// ---------------------------	
	// PUBLIC DATA
	// This used to be a struct
	//	
public:
    TCHAR *LName;      // Localized translated name if no translation == NULL.
    TCHAR *GName;      // Global Function name or var name.
    DWORD ThreadID;    // The thread ID that registered this function.
    HANDLE Thread;     // The thread that registered this function.
    int id;            // User-defined function id number (>0)
                       //   or -1 for predefined fn or var
                       //   (0 for user-defined vars).
	bool parensReqd;   // There are so many cases slipping through,I have
					   // add this as a failsafe. If true, parenthisis are req'd
					   // Set based on if the function name is defun'd with C: or
					   // not.
    struct resbuf rb;  // The .restype is the type for this whole link.
                       // For a list, it's RTLB, and the list continues
                       // from the .rbnext.
                       // THIS IS A SUPER RESBUF.  SEE NOTE AT THE
                       // TOP OF LISP.C IN ICADLIB.

	// --------------------------
	// Internal data
	//		
private:                    
    int (*m_funcptr)(void);
	bool m_bVbaFunction;	//This indicates whether the function has been defined in a VBA dll.
public:                    
    
	// Introduced By Sachin Dange to store the help reference for the command
	// This is also related to Bugzilla 78663 ...........[
	char *m_szHelpFile,*m_szContextID;
	int m_nMapNumber;
	// ................] Modifications end here.
                      
	};
 
 
 #endif // _COMMANDATOM_H


