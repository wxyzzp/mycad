class CMenuItem
{
// Construction
public:
	CMenuItem();
	virtual ~CMenuItem();

// Attributes
private:
	char m_sSlide[100];
	char m_sItem[100];
	char m_sCmd[100];
	short m_ShwSldFlg;
	short m_AddLstFlg;

// Operations
public:
	CMenuItem & operator=(const CMenuItem &other);
	void Parse(const char *psLine);
	void Reset();
	char * Item();
	char * Slide();
	char * Cmd();

private:
  
};
