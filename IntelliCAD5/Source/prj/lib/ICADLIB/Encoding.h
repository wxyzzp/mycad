#ifndef _ENCODING_H_
#define _ENCODING_H_

// Developer: VT

#ifdef _DEBUG
#include "assert.h"
#define ASSERT(A) assert(A)
#else
#define ASSERT(A)
#endif

#include "TCHAR.h"

#ifndef __fstream__
#include <fstream>
#define __fstream__
#endif

class Encoding
{
public:
	static Encoding* getEncoder(const TCHAR* encoding);
	static Encoding* getEncoderByXMLFile(FILE* pFile);

	int getch();
	virtual int putch(int ch) = 0;
	virtual bool setFile(FILE* pFile);
	virtual bool setFile(Encoding* pEncoder);
	virtual void getFilePos(int& iLine, int& iSymbol) const;
	virtual ~Encoding();
	virtual void ungetch(int ch);

protected:
	Encoding(): m_pFile(0), m_FileType(eFT_NONE), m_iLine(1), m_iSymbolInLine(1),
		m_bUngetchHere(false), m_bUngetchSource(false) {}
	Encoding(Encoding* pEncoder): m_pEncoder(pEncoder), m_FileType(eFT_ENCODER),
		m_iLine(1), m_iSymbolInLine(1), m_bUngetchHere(false), m_bUngetchSource(false) {}

	typedef enum
	{ eFT_NONE = 0
	, eFT_FILE
	, eFT_ENCODER
	, eFT_STREAM
	, eFT_CFILE
	, eFT_MAX
	} PFileType;

	int getchSource();
	virtual int getchTranslate() = 0;
	virtual void ungetchSource(int ch);

	int putchSource(int ch);

	Encoding& encoder() const { ASSERT(m_FileType == eFT_ENCODER); return *m_pEncoder; }

private:
	PFileType m_FileType;
	union {
		FILE* m_pFile;
//		CFile* m_pCFile;
		Encoding* m_pEncoder;
	};
	int m_iLine;		// # current line (from 1)
	int m_iSymbolInLine;// # of current symbol in line (from 1)
	int m_UngetchSymbol;// ungetch() symbol
	bool m_bUngetchHere;// true if ungetch symbol 
	bool m_bUngetchSource;// true if ungetch symbol for getchSource function
	int m_UngetchSource;// ungetch symbol for getchSource function
};

#endif // _ENCODING_H_
