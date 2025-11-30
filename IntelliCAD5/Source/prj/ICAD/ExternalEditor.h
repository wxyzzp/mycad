// ExternalEditor.h: CExternalEditor Class
//		by EBATECH(CNBR)
//////////////////////////////////////////////////////////////////////

#define BREAK_OK      72
#define BREAK_LIMIT  256

class CExternalEditor  
{
public:
	CExternalEditor(){
	};
	virtual ~CExternalEditor(){
	};
	BOOL RunEditor();
	BOOL Wait();

	void SetTextString(CString TextString){
		m_TextString = TextString;
	};
	void SetMTextedValue(CString MTextedValue){
		m_MtextedValue = MTextedValue;
	};
	CString& GetTextString( void ){
		return m_TextString;
	};
	CString& GetMTextedValue( void ){
		return m_MtextedValue;
	};
private:
	BOOL GetTempFilename();
	CString m_TempFile;
	BOOL ReadTempFile();
	BOOL WriteTempFile();
	BOOL SearchEditor();
	BOOL IsExternalEditor();

	CString m_MtextedValue;
	CString m_TextString;
};

int CreateMTEXTObject( void ); // Create MTEXT Object
