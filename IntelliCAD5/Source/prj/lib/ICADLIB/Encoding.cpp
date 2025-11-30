#include "Encoding.h"

#ifndef __fstream__
#include <fstream>
#define __fstream__
#endif

class EncASCII: public Encoding
{
protected:
	virtual int getchTranslate() { return getchSource(); }

public:
	virtual int putch(int ch) { return putchSource(ch); }
};

class EncUTF8: public Encoding
{
protected:
	virtual int getchTranslate();

public:
	virtual int putch(int ch);
};

int EncUTF8::getchTranslate()
{
	// bad symbols not handled!
	int ch = getchSource();

	if(ch < 0x80)
		return ch;

	int chN,
		bit = 1 << 6,
		result = 0,
		highN = 0;
	
	ASSERT((ch & 0xc0) == 0xc0);

	do
	{
		chN = getchSource();
		if(chN == -1)
			return -1;
		
		result <<= 6;
		result |= chN & 63;
		bit >>= 1;
		highN += 6;
	} while(ch & bit);
	
	return result | ((ch & (bit - 1)) << highN);
}

int EncUTF8::putch(int ch)
{
	ASSERT(ch >= 0);

#define BITS(src, bitFrom, bitsTake) ((src >> bitFrom) & ((1 << bitsTake) - 1))

	if(ch < 0x80)
		return putchSource(ch);

	if(ch < 0x7ff)
	{
		putchSource(0xc0 | (ch >> 6));
		return (putchSource(0x80 | (ch & 63)) > 0) ? ch:-1;
	}

	if(ch < 0x10000)
	{
		putchSource(0xe0 | (ch >> 12));
		putchSource(0x80 | BITS(ch, 6, 6));
		return (putchSource(0x80 | (ch & 63)) > 0) ? ch:-1;
	}

	if(ch < 0x200000)
	{
		putchSource(0xf0 | (ch >> 18));
		putchSource(0x80 | BITS(ch, 12, 6));
		putchSource(0x80 | BITS(ch, 6, 6));
		return (putchSource(0x80 | (ch & 63)) > 0) ? ch:-1;
	}

	if(ch < 0x4000000)
	{
		putchSource(0xf8 | (ch >> 24));
		putchSource(0x80 | BITS(ch, 18, 6));
		putchSource(0x80 | BITS(ch, 12, 6));
		putchSource(0x80 | BITS(ch, 6, 6));
		return (putchSource(0x80 | (ch & 63)) > 0) ? ch:-1;
	}

	putchSource(0xfc | (ch >> 30));
	putchSource(0x80 | BITS(ch, 24, 6));
	putchSource(0x80 | BITS(ch, 18, 6));
	putchSource(0x80 | BITS(ch, 12, 6));
	putchSource(0x80 | BITS(ch, 6, 6));
	return (putchSource(0x80 | (ch & 63)) > 0) ? ch:-1;
}

class EncUCS2: public Encoding
{
protected:
	virtual int getchTranslate();

public:
	virtual int putch(int ch);
};

int EncUCS2::getchTranslate()
{
	int ch1, ch2;
	
	return ((ch1 = getchSource()) == -1 || (ch2 = getchSource()) == -1) ? -1:ch2*256 + ch1;
}

int EncUCS2::putch(int ch)
{
	return (putchSource(ch & 255) >= 0 && putchSource(ch >> 8) >= 0) ? ch:-1;
}

static Encoding* createEncASCII()
{
	return new EncASCII();
}

static Encoding* createEncUTF8()
{
	return new EncUTF8;
}

static Encoding* createEncUCS2()
{
	return new EncUCS2;
}

int Encoding::getch()
{
	if(m_bUngetchHere)
	{
		m_bUngetchHere = false;
		return m_UngetchSymbol;
	}

	register int ch = getchTranslate();

	if(m_FileType != eFT_ENCODER)
	{
		if(ch == '\n')
		{
			++m_iLine;
			m_iSymbolInLine = 1;
		}
		else if(ch != -1)
			++m_iSymbolInLine;
	}

	return ch;
}

void Encoding::ungetch(int ch)
{
	ASSERT(m_bUngetchHere == false);

	m_bUngetchHere = true;
	m_UngetchSymbol = ch;
	if(m_FileType != eFT_ENCODER)
	{
		if(ch == '\n')
		{
			--m_iLine;
			m_iSymbolInLine = 0;// special value
		}
		else
			--m_iSymbolInLine;
	}
}

Encoding* Encoding::getEncoder(const TCHAR* encoding)
{
	static struct {
		TCHAR* pName;
		Encoding* (*pFuncCreate)();
	} encodingNames[] =
	{ "ASCII", createEncASCII
	, "UTF-8", createEncUTF8
	, "UCS-2", createEncUCS2
	, "Unicode", createEncUCS2
	, "UTF-16", createEncUCS2
	, "UTF-16LE", createEncUCS2
	, "UTF-16BE", createEncUCS2
	};

	for(int i = sizeof(encodingNames)/sizeof(encodingNames[0]); i--;)
		if(_tcsicmp(encoding, encodingNames[i].pName) == 0)
			return encodingNames[i].pFuncCreate();
	return createEncUTF8();
}

static struct {
	char signature[4];
	Encoding* (*pFuncCreate)();
} encodingSignatures[] =
{ // UCS-4 0x3c, 0x00, 0x00, 0x00,
	0x3c, 0x3f, 0x78, 0x6d, createEncUTF8
};

Encoding* Encoding::getEncoderByXMLFile(FILE* pFile)
{
	Encoding* pEncoder = NULL;
	long pos = ftell(pFile);
	char signature[4];
	int i;

	if(fseek(pFile, 0, SEEK_SET) != 0) // file is incapable of seeking
		return createEncUTF8();
	if(fread(signature, 4, 1, pFile) != 1)
		goto done;

	for(i = sizeof(encodingSignatures)/sizeof(encodingSignatures[0]); i--;)
	{
		if(memcmp(signature, encodingSignatures[i].signature, sizeof(signature)) == 0)
		{
			pEncoder = encodingSignatures[i].pFuncCreate();
			if(pEncoder)
				pEncoder->setFile(pFile);
			break;
		}
	}

done:
	fseek(pFile, pos, SEEK_SET);
	return (pEncoder) ? pEncoder:createEncUTF8();
}

void Encoding::getFilePos(int& iLine, int& iSymbol) const
{
	if(m_FileType == eFT_ENCODER)
		m_pEncoder->getFilePos(iLine, iSymbol);
	else
	{
		ASSERT(m_iSymbolInLine > 0);
		iLine = m_iLine;
		iSymbol = m_iSymbolInLine;
	}
}

int Encoding::getchSource()
{
	int ch;

	if(m_bUngetchSource)
	{
		m_bUngetchSource = false;
		return m_UngetchSource;
	}

	switch(m_FileType)
	{
	case eFT_NONE:  return -1;
	case eFT_FILE:  ch = getc(m_pFile); break;
//	case eFT_CFILE: { unsigned char c = 0; ch = (m_pCFile->Read(&c, 1) == 1) ? c : -1; break; }
	case eFT_ENCODER: return m_pEncoder->getch();
	default: ASSERT(false && "unknown file type"); return -1;
	}

	return ch;
}

void Encoding::ungetchSource(int ch)
{
	ASSERT(m_bUngetchSource == false);

	m_bUngetchSource = true;
	m_UngetchSource = ch;
}

bool Encoding::setFile(FILE* pFile)
{
	if(m_FileType == eFT_ENCODER)
		delete m_pEncoder;
	m_pFile = pFile;
	m_FileType = eFT_FILE;
	return true;
}

bool Encoding::setFile(Encoding* pEncoder)
{
	if(m_FileType == eFT_ENCODER)
		delete m_pEncoder;
	m_pEncoder = pEncoder;
	m_FileType = eFT_ENCODER;
	return true;
}

Encoding::~Encoding()
{
	if(m_FileType == eFT_ENCODER)
		delete m_pEncoder;
}

int Encoding::putchSource(int ch)
{
	switch(m_FileType)
	{
	case eFT_NONE:  return -1;
	case eFT_FILE:  return putc(ch, m_pFile);
//	case eFT_CFILE: { char c = ch; return (m_pCFile->Write(&c, 1) == 1) ? ch : -1; }
	case eFT_ENCODER: return m_pEncoder->putch(ch);
	default: ASSERT(false && "unknown file type"); return -1;
	}
}

