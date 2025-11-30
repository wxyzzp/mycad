class CReadMenu
{
// Construction
public:
	CReadMenu();
	virtual ~CReadMenu();

// Attributes
private:
	short m_page;

// Operations
public:
	short Do(const char *psMenu, const char *psSection);
	short InitDialog(CStdioFile *pfmenu, sds_hdlg handle);

private:
  
};

void dlg_callback(sds_callback_packet *cpkt);
void acceptCB(sds_callback_packet *cpkt);
void rejectCB(sds_callback_packet *cpkt);
void nextCB(sds_callback_packet *cpkt);
void prevCB(sds_callback_packet *cpkt);
void listCB(sds_callback_packet *cpkt);
