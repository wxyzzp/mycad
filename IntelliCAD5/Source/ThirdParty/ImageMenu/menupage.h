class CMenuPage
{
// Construction
public:
	CMenuPage();
	virtual ~CMenuPage();

// Attributes
private:
	CMenuItem m_items[20];

// Operations
public:
	void Set(short idx, CMenuItem *pItem);
	CMenuItem *Get(short idx);
	void Reset();

private:
  
};
