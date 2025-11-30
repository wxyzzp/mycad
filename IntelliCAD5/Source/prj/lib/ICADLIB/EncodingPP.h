#ifndef _ENCODINGPP_H_
#define _ENCODINGPP_H_

// Developer: VT

#include "xml.h"
#include "Encoding.h"

// FORWARDS
class XMLString;

class EncodingPreProc: public Encoding
{
public:
	EncodingPreProc(Encoding* pEncoding, XMLMapStrStr* pMap);
	virtual int getchTranslate();
	virtual int putch(int ch);
	virtual ~EncodingPreProc();

protected:
	virtual void ungetchSource(int ch);

private:
	bool isEnable() const;
	void push(bool value);
	void toggleLast();
	void pop();
	void setError(const TCHAR* errDesc) {}
	bool procDefine();
	int getchNL();
	bool isNewLine() { return m_NewLine > 0; }
	int lexToken(XMLString& string);
	bool syntaxAnalize();
	bool getConcreteToken(int tokenExpected, XMLString& keyWord);
	bool getMacro(const XMLString& name, XMLString& result);
	XMLMapStrStr& mapDefines() { return *m_pMapDefines; }
	bool getTillNL(XMLString& string);
	void reportUnexpectedToken(int token, int tokenExpected);

private:
	int m_IfStack;
	int m_nIfStackUsed;
	int m_NewLine;
	XMLMapStrStr* m_pMapDefines;
};

#endif // _ENCODINGPP_H_
