// File  : <DevDir>\source\prj\icad\icadEntPropTabs.h

#include "icad.h"
#include "icadapi.h"
#include "cmds.h"
#include "paths.h"
#include "resource.hm"


#define ENT_CHECK   56
#define ENT_GREY    57
#define ENT_JUSTI   58
#define ENT_COL     59
#define ENT_VERTEX  60
#define ENT_210     61
#define PLINE_MESH  62
#define ENT_COMBO   63
#define RTANG_END   64
//Bugzilla No. 7219 ; 07-10-2002 [
#define DIM_MEASURE	65
#define DIME_TYPE	66
//Bugzilla No. 7219 ; 07-10-2002 ]
// EBATECH(CNBR) -[ 31-03-2003 Bugzilla#78471 Support Lineweight
#define ENT_LW      67
// EBATECH(CNBR) ]-

#define MY_DB_ACIS	111		/*D.G.*/// just a number is not in other DB_xxx values range

struct ent_EntCodes
{
	resbuf*			prbb; //Points to the resbuf
	ent_EntCodes*	next; //Points to itself
	ent_EntCodes*	prev;
};

struct ent_DlgItem
{
	int				ID,			//This is the resource ID
					DXFCode,	//This is the DXF Code
					ResType;	//This is specifies how to handle the data
	short			PtXYZ;		//This is used for x,y,z array indexes as well as check box values, negitive on 210 for WCS
	int				EntTyp;		//This specifies the dialog group
	BOOL			DoOnce,		//This is used to detect if the word "Varies" should be entered
					ReadOnly;	//This is used for speed to keep from setting variables unnecessarily
	int				CheckBit;	//This is the original value of a short as well as the value of the check box
	double			OrgValue;	//This is the original value
	char			*DspValue;	//This is the char array that will hold the value to be displayed
	int				SColor;		//This is for the color and lineweight.
	ent_EntCodes	*pRbll,		//Pointer to the struct that points to resbuf(s) of the display value
					*pCurll;	//Pointer used to step through the struct that points to the resbuf(s)
	short			howmany;	//Only for testing
};

//Protos
// DP: I had change request to display linetype scale independent on drawing units.
// DP: So, now we always form linetype scale with decimal units
// DP: and for correct spinning we need nUnitType parameter.
void ent_SpinMasterReal(HWND hWndItem,int increment,int nUnitType = -1);
void ent_SpinMasterInt(HWND hWndItem,int increment);
void ent_SpinMasterAngle(HWND hWndItem,int increment);
void ent_SpinMasterEndAngle(HWND hWndItem,int increment);


class ent_3DFACE_TAB : public CDialog
{
    public:
		BOOL	ent_Pick1, ent_Pick2, ent_Pick3, ent_Pick4;
		int		iTmp;
		CString	csTmp;

        ent_3DFACE_TAB() ;
        BOOL OnInitDialog();
    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void ent_OnPick3();
        afx_msg void ent_OnPick4();
        afx_msg void OnSpinPt1x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt1y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt1z(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2z(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3z(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4z(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_ARC_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick;

        ent_ARC_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinArcRad(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinArcSAng(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinArcEAng(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinArcCpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinArcCpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinArcCpz(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_ATTDEF_TAB : public CDialog
{
    public:
		int		iTmp, iTxtCurSel, iJust72, iJust74;
		CString	csTmp;
		BOOL	ent_Pick1, ent_Pick2;

        ent_ATTDEF_TAB() ;
        BOOL OnInitDialog();
        void ent_StyleChoice();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        void ent_Yes10();
        void ent_No10();
        void ent_Yes11();
        void ent_No11();
        void ent_Yes40();
        void ent_No40();
        void ent_Yes50();
        void ent_No50();
        afx_msg void OnSpinAttdSpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdSpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdSpz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdApx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdApy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdApz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdHth(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdRot(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdTck(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdScl(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttdObl(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        void ent_SetSel();
        void ent_SelChange();
        afx_msg void OnStyleChange();
        afx_msg void OnSelChange();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		void		 FillCombo();

    DECLARE_MESSAGE_MAP()
};


class ent_ATTRIB_TAB : public CDialog
{
    public:
		int		iTmp, iTxtCurSel, iJust72, iJust74;
		CString	csTmp;
		BOOL	ent_Pick1,ent_Pick2;

        ent_ATTRIB_TAB() ;
        BOOL OnInitDialog();
        void ent_StyleChoice();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        void ent_Yes10();
        void ent_No10();
        void ent_Yes11();
        void ent_No11();
        void ent_Yes40();
        void ent_No40();
        void ent_Yes50();
        void ent_No50();
        afx_msg void OnSpinAttrSpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrSpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrSpz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrApx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrApy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrApz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrHth(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrRot(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrTck(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrScl(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAttrObl(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void OnStyleChange();
        void ent_SetSel();
        void ent_SelChange();
        afx_msg void OnSelChange();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		void		 FillCombo();

    DECLARE_MESSAGE_MAP()
};


class ent_CIRCLE_TAB : public CDialog
{
    public:
		BOOL	ent_Pick;
		int		iTmp;
		CString	csTmp;

        ent_CIRCLE_TAB() ;
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinCircleRad(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCircleCpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCircleCpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCircleCpz(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_DIM_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		bool	ent_Pick;

        ent_DIM_TAB() ;

    protected:
        BOOL OnInitDialog();
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void ent_DimSettings();
        afx_msg void OnSpinDimIpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinDimIpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinDimIpz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinDimRot(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_ELP_TAB : public CDialog
{
    public:
		CString	csTmp;
		int		iTmp;
		BOOL	ent_Pick1, ent_Pick2;

        ent_ELP_TAB();
        BOOL OnInitDialog();

    protected:
        afx_msg void OnSpin10(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin20(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin30(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin11(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin21(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin31(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin40(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin41(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpin42(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_HATCH_TAB : public CDialog
{
public:
	ent_HATCH_TAB() : CDialog(ENT_HATCH_TAB)
	{}
	virtual ~ent_HATCH_TAB()
	{}

	BOOL OnInitDialog();

protected:
    virtual void OnOK();
    virtual void OnCancel();
	afx_msg void OnHatchEditButton();
    afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
    virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
};

//Modified Cybage MM 24-09-2001 DD-MM-YYYY [
//Reason : Bug No.- 77847 from Bugzilla
class ent_IMAGE_TAB : public CDialog
{
    public:
		BOOL	ent_Pick;
		int		iTmp;
		CString	csTmp;
		int validationFailure;
		ent_IMAGE_TAB() ;
		BOOL OnInitDialog();
		virtual void OnOK();
		BOOL validate(int value,int ID);

	protected:
		virtual void OnCancel();
        afx_msg void ent_OnPick();
		afx_msg void OnSpinImageOPx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinImageOPy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinImageOPz(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinImageBrightness(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinImageContrast(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinImageFade(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};
//Modified Cybage MM 24-09-2001 DD-MM-YYYY ]

class ent_INSERT_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick;

        ent_INSERT_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinCol(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRow(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRowSpace(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinColSpace(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinIpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinIpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinIpz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSfx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSfy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSfz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRot(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_LEADER_TAB : public CDialog
{
    public:
		ent_EntCodes	*pLedbb, *pLcur;
		int				iTmp, LedCurCnt, LedTot;
		CString			csTmp;
		BOOL			ent_Pick;

        ent_LEADER_TAB();
        BOOL OnInitDialog();

    protected:
        struct resbuf ucs,wcs;
        int ddxStart,ddxEnd;
                void LoadLed();
                void GreyLed();
        afx_msg void OnLedNext();
        afx_msg void OnLedPrev();
                void Count();
        virtual void OnOK();
        virtual void OnCancel();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg void OnSpinLed(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result);
		// Bugzilla No. 78001; 18/03/2002
		afx_msg void ent_DimSettings();
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_LINE_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick1, ent_Pick2;

        ent_LINE_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void OnSpinStx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSty(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinStz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdz(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_MLINE_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;

        ent_MLINE_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_MTEXT_TAB : public CDialog
{
    public:
		BOOL	ent_Pick;
		int		iTmp;
		CString	csTmp;

        ent_MTEXT_TAB();
        BOOL OnInitDialog();

   protected:
		afx_msg void OnEditMText();
        afx_msg void OnSpinX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinWid(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinHigh(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRot(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void ent_OnPick();
                void ent_StyleChoice();
        virtual void OnOK();
        virtual void OnCancel();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		void		 FillCombo();

    DECLARE_MESSAGE_MAP()
};


class ent_PLINE_TAB : public CDialog
{
    public:
		int			iTmp;
		double		dOrgX, dOrgY, dOrgZ, dOrg40, dOrg41, dOrg42;
		long		lVertNum, lVertTot;
		cmd_elistll *pCurV;
		resbuf		*pRb, rbent, rbucs;
		CString		csTmp;
		BOOL		ent_Pick;

        ent_PLINE_TAB() ;
        BOOL OnInitDialog();
        void DisableSomeThings();
        void LoadVertex();
        void GrabVert();
        void GrabElev();
        void CountVertex();
        void DisableVertex();
        void OnNextSub(int increment);
        void OnPrevSub(int increment);

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnNext();
        afx_msg void OnPrev();
        afx_msg void OnSpinPlineElev(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineST(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineED(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineBulge(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnVSpin(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


#include <vector>
using std::vector;

struct PlineVertex
{
	sds_point	vertex;
	sds_real	startWidth;
	sds_real	endWidth;
	sds_real	bulge;
};

class ent_LWPLINE_TAB : public CDialog
{
    public:
        BOOL ent_Pick;

        ent_LWPLINE_TAB();
        void GetVertex();
        void GrabElev();
		void SelectVertex( sds_point);


    protected:
		bool				vertexLoaded;
		vector<PlineVertex>	vertexArray;
        int				iTmp;
        double			dOrgX, dOrgY, dOrgZ, dOrg40, dOrg41, dOrg42;
        long			lVertNum, lVertTot;
        cmd_elistll*	pCurV;
        resbuf			*pRb, rbent, rbucs;
        CString			csTmp;

		void AssertValid() const;
        BOOL OnInitDialog();
        void DisableSomeThings();
		void GetVertexArray();
		void SaveVertexArray();
		void ChangeVertex( int n);
		void DisplayVertex();
        PlineVertex LoadVertex( sds_resbuf *&);
        void DisableVertex();
        void OnNextSub(int increment);
        void OnPrevSub(int increment);

        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnNext();
        afx_msg void OnPrev();
        afx_msg void OnSpinPlineElev(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineST(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineED(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPlineBulge(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnVSpin(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_POINT_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick;

        ent_POINT_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinPointAng(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCpz(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_RAY_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick1, ent_Pick2;

        ent_RAY_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void OnSpinStx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSty(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinStz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdz(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};

// EBATECH(CNBR) -[ 2002/4/24
class ent_SHAPE_TAB : public CDialog
{
    public:
		int		iTmp, iTxtCurSel;
		CString	csTmp;
		BOOL	ent_Pick;

        ent_SHAPE_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinHeight(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinRotation(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinWidth(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOblique(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOriginX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOriginY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOriginZ(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		void		 FillCombo();
		void		ent_NameChoice();

	DECLARE_MESSAGE_MAP()
};
// EBATECH(CNBR) ]-


class ent_SOLID_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick1, ent_Pick2, ent_Pick3, ent_Pick4;

        ent_SOLID_TAB();
        BOOL OnInitDialog();
        void CopyAllZs();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void ent_OnPick3();
        afx_msg void ent_OnPick4();
        afx_msg void OnSpinPt1x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt1y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4z(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_SPLINE_TAB : public CDialog
{
    public:
		ent_EntCodes	*pFitbb, *pCtlbb, *pFcur, *pCcur;
		int				iTmp, CtlCurCnt,FitCurCnt,CtlTot,FitTot;
		CString			csTmp;
		BOOL			ent_Pick;

        ent_SPLINE_TAB();
        BOOL OnInitDialog();

    protected:
        struct resbuf ucs,wcs;
                void LoadCtl();
                void LoadFit();
                void GreyCtl();
                void GreyFit();
        afx_msg void OnCtlNext();
        afx_msg void OnCtlPrev();
        afx_msg void OnFitNext();
        afx_msg void OnFitPrev();
                void Count();
        virtual void OnOK();
        virtual void OnCancel();
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_TEXT_TAB : public CDialog
{
    public:
		int		iTmp, iTxtCurSel, iJust72, iJust73;
		CString	csTmp;
		BOOL	ent_Pick,ent_Pick2;

        ent_TEXT_TAB();
        BOOL OnInitDialog();
        void ent_StyleChoice();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        void ent_Yes10();
        void ent_No10();
        void ent_Yes11();
        void ent_No11();
        void ent_Yes40();
        void ent_No40();
        void ent_Yes50();
        void ent_No50();
        afx_msg void ent_OnPick();
        afx_msg void ent_OnPick2();
        afx_msg void OnSpinTextHigh(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinTextRot(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinTextWide(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinTextQue(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOrgX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOrgY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinOrgZ(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAliX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAliY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinAliZ(NMHDR *pNotifyStruct,LRESULT *result);
        void ent_SetSel();
        virtual void DoDataExchange(CDataExchange *pDX);
        void ent_SelChange();
        afx_msg void OnJustChange();
        afx_msg void OnStyleChange();
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};


class ent_TOLER_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick;

        ent_TOLER_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinX(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinY(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinZ(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_TRACE_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick1, ent_Pick2, ent_Pick3, ent_Pick4;

        ent_TRACE_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void ent_OnPick3();
        afx_msg void ent_OnPick4();
        afx_msg void OnSpinPt1x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt1y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt1z(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt2z(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt3z(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4x(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4y(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinPt4z(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_VIEWP_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick;

        ent_VIEWP_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick();
        afx_msg void OnSpinCpx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCpy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinCpz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinHigh(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinWide(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};


class ent_XLINE_TAB : public CDialog
{
    public:
		int		iTmp;
		CString	csTmp;
		BOOL	ent_Pick1, ent_Pick2;

        ent_XLINE_TAB();
        BOOL OnInitDialog();

    protected:
        virtual void OnOK();
        virtual void OnCancel();
        afx_msg void ent_OnPick1();
        afx_msg void ent_OnPick2();
        afx_msg void OnSpinStx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinSty(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinStz(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdx(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdy(NMHDR *pNotifyStruct,LRESULT *result);
        afx_msg void OnSpinEdz(NMHDR *pNotifyStruct,LRESULT *result);
        virtual void DoDataExchange(CDataExchange *pDX);
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);

    DECLARE_MESSAGE_MAP()
};

class ent_3DSOLID_TAB : public CDialog
{
public:
	ent_3DSOLID_TAB() : CDialog(ENT_3DSOLID_TAB)
	{}
	virtual ~ent_3DSOLID_TAB()
	{}

	BOOL OnInitDialog();

protected:
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHel);
    virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

class ent_REGION_TAB : public CDialog
{
public:
	ent_REGION_TAB() : CDialog(ENT_REGION_TAB)
	{}
	virtual ~ent_REGION_TAB()
	{}

	BOOL OnInitDialog();

protected:
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHel);
    virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

class ent_BODY_TAB : public CDialog
{
public:
	ent_BODY_TAB() : CDialog(ENT_BODY_TAB)
	{}
	virtual ~ent_BODY_TAB()
	{}

	BOOL OnInitDialog();

protected:
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHel);
    virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
