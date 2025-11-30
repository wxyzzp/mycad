/* F:\BLD\PRJ\ICAD\ICADTOLERANCEDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract 
 *
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"
// To Add DIMENSION XDATA
#include "dimensions.h"  
#include "cmdsdimvars.h"

#define AA      10
#define BB      11
#define CC      12
#define DD      13
#define EE      14
#define FF      15
#define GG      16
#define HH      17
#define II      18
#define JJ      19
#define KK      20
#define LL      21
#define MM      22
#define NN      23
#define PP      24
#define RR      25
#define SS      26
#define TT      27
#define UU      28
#define BLANK   29

//********************************************************************************
//********************************************************************************
//******************** This is for the material dialog ***************************
//********************************************************************************
//********************************************************************************

class CMaterial:public CDialog {
    public:
        int x,y;
        CMaterial();
        ~CMaterial();
        BOOL OnInitDialog();
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        HBITMAP hBitmap[4];
        afx_msg void OnM();
        afx_msg void OnL();
        afx_msg void OnS();
        afx_msg void OnBlank();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMaterial,CDialog)
    ON_BN_CLICKED(TOL_MC_M,OnM)
    ON_BN_CLICKED(TOL_MC_L,OnL)
    ON_BN_CLICKED(TOL_MC_S,OnS)
    ON_BN_CLICKED(TOL_MC_BLANK,OnBlank)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL CMaterial::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_TOLERANCE);
	return(true);
}

void CMaterial::OnM(){
    EndDialog(MM);
}
void CMaterial::OnL(){
    EndDialog(LL);
}
void CMaterial::OnS(){
    EndDialog(SS);
}
void CMaterial::OnBlank(){
    EndDialog(BLANK);
}

void CMaterial::DoDataExchange(CDataExchange *pDX) {
}

BOOL CMaterial::OnInitDialog(){
    RECT rect;
    HWND hWnd;

    CDialog::OnInitDialog();
    GetWindowRect(&rect);
    CMaterial::MoveWindow(x,y,rect.right-rect.left,rect.bottom-rect.top,true);

    hWnd=::GetDlgItem(m_hWnd,TOL_MC_M);
    hBitmap[0]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_M));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[0]);
    hWnd=::GetDlgItem(m_hWnd,TOL_MC_L);
    hBitmap[1]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_L));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[1]);
    hWnd=::GetDlgItem(m_hWnd,TOL_MC_S);
    hBitmap[2]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_S));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[2]);
    hWnd=::GetDlgItem(m_hWnd,TOL_MC_BLANK);
    hBitmap[3]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[3]);

    return(TRUE);
}

CMaterial::~CMaterial(){
	for(int i=0;i<4;i++) DeleteObject(hBitmap[i]);
}

CMaterial::CMaterial():CDialog(TOL_MC) {
}

//********************************************************************************
//********************************************************************************
//******************** This is for the symbol dialog *****************************
//********************************************************************************
//********************************************************************************

class CSymbol:public CDialog {
    public:
        int x,y;
        CSymbol();
        ~CSymbol();
        BOOL OnInitDialog();
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        HBITMAP hBitmap[15];
        afx_msg void OnA();
        afx_msg void OnB();
        afx_msg void OnC();
        afx_msg void OnD();
        afx_msg void OnE();
        afx_msg void OnF();
        afx_msg void OnG();
        afx_msg void OnH();
        afx_msg void OnI();
        afx_msg void OnJ();
        afx_msg void OnK();
        afx_msg void OnR();
        afx_msg void OnT();
        afx_msg void OnU();
        afx_msg void OnBlank();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CSymbol,CDialog)
    ON_BN_CLICKED(TOL_SUB_A,OnA)
    ON_BN_CLICKED(TOL_SUB_B,OnB)
    ON_BN_CLICKED(TOL_SUB_C,OnC)
    ON_BN_CLICKED(TOL_SUB_D,OnD)
    ON_BN_CLICKED(TOL_SUB_E,OnE)
    ON_BN_CLICKED(TOL_SUB_F,OnF)
    ON_BN_CLICKED(TOL_SUB_G,OnG)
    ON_BN_CLICKED(TOL_SUB_H,OnH)
    ON_BN_CLICKED(TOL_SUB_I,OnI)
    ON_BN_CLICKED(TOL_SUB_J,OnJ)
    ON_BN_CLICKED(TOL_SUB_K,OnK)
    ON_BN_CLICKED(TOL_SUB_R,OnR)
    ON_BN_CLICKED(TOL_SUB_T,OnT)
    ON_BN_CLICKED(TOL_SUB_U,OnU)
    ON_BN_CLICKED(TOL_SUB_BLANK,OnBlank)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL CSymbol::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_TOLERANCE);
	return(true);
}

void CSymbol::OnA(){
    EndDialog(AA);
}
void CSymbol::OnB(){
    EndDialog(BB);
}
void CSymbol::OnC(){
    EndDialog(CC);
}
void CSymbol::OnD(){
    EndDialog(DD);
}
void CSymbol::OnE(){
    EndDialog(EE);
}
void CSymbol::OnF(){
    EndDialog(FF);
}
void CSymbol::OnG(){
    EndDialog(GG);
}
void CSymbol::OnH(){
    EndDialog(HH);
}
void CSymbol::OnI(){
    EndDialog(II);
}
void CSymbol::OnJ(){
    EndDialog(JJ);
}
void CSymbol::OnK(){
    EndDialog(KK);
}
void CSymbol::OnR(){
    EndDialog(RR);
}
void CSymbol::OnT(){
    EndDialog(TT);
}
void CSymbol::OnU(){
    EndDialog(UU);
}
void CSymbol::OnBlank(){
    EndDialog(BLANK);
}

void CSymbol::DoDataExchange(CDataExchange *pDX) {
}

BOOL CSymbol::OnInitDialog(){
    RECT rect;
    HWND hWnd;

    CDialog::OnInitDialog();
    GetWindowRect(&rect);
    CSymbol::MoveWindow(x,y,rect.right-rect.left,rect.bottom-rect.top,true);

    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_A);
    hBitmap[0]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_A));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[0]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_B);
    hBitmap[1]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_B));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[1]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_C);
    hBitmap[2]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_C));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[2]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_D);
    hBitmap[3]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_D));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[3]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_E);
    hBitmap[4]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_E));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[4]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_F);
    hBitmap[5]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_F));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[5]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_G);
    hBitmap[6]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_G));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[6]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_H);
    hBitmap[7]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_H));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[7]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_I);
    hBitmap[8]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_I));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[8]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_J);
    hBitmap[9]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_J));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[9]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_K);
    hBitmap[10]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_K));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[10]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_R);
    hBitmap[11]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_R));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[11]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_T);
    hBitmap[12]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_T));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[12]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_U);
    hBitmap[13]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_U));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[13]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SUB_BLANK);
    hBitmap[14]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[14]);



    return(TRUE);
}

CSymbol::CSymbol():CDialog(TOL_SYMBOL) {
}

CSymbol::~CSymbol(){
	for(int i=0;i<15;i++) DeleteObject(hBitmap[i]);
}

//********************************************************************************
//********************************************************************************
//******************** This is for the main tolerance dialog *********************
//********************************************************************************
//********************************************************************************

class CTolerance:public IcadDialog {
    public:
        CString cs1Val1,cs1Val2,
                cs1Dat1,cs1Dat2,cs1Dat3,
                cs2Val1,cs2Val2,
                cs2Dat1,cs2Dat2,cs2Dat3,
                csString1,csString2,csHeight,csID;
        int iSym1,i1Mc1,i1Mc2,i1Mc3,i1Mc4,i1Mc5,
            iSym2,i2Mc1,i2Mc2,i2Mc3,i2Mc4,i2Mc5,i1Dia1,i1Dia2,i2Dia1,i2Dia2,iProt;
        RECT rect;
        CTolerance();
        ~CTolerance();
        BOOL OnInitDialog();
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void OnHelp();
        HBITMAP hBitmap[17];
        void SwitchBitMaps(int incc,int iMap,HWND hWnd);
        void PopUpSym(int *isym,int x,int y);
        void PopUpMC(int *imc,int x,int y);
        virtual void OnOK();
        afx_msg void OnSym1();
        afx_msg void OnSym2();
        afx_msg void On1Dia1();
        afx_msg void On1Dia2();
        afx_msg void On2Dia1();
        afx_msg void On2Dia2();
        afx_msg void On1MC1();
        afx_msg void On1MC2();
        afx_msg void On1MC3();
        afx_msg void On1MC4();
        afx_msg void On1MC5();
        afx_msg void On2MC1();
        afx_msg void On2MC2();
        afx_msg void On2MC3();
        afx_msg void On2MC4();
        afx_msg void On2MC5();
        afx_msg void OnProt();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CTolerance,CDialog)
    ON_BN_CLICKED(TOL_SYM1,OnSym1)
    ON_BN_CLICKED(TOL_SYM2,OnSym2)
    ON_BN_CLICKED(TOL_1DIA1,On1Dia1)
    ON_BN_CLICKED(TOL_1DIA2,On1Dia2)
    ON_BN_CLICKED(TOL_2DIA1,On2Dia1)
    ON_BN_CLICKED(TOL_2DIA2,On2Dia2)
    ON_BN_CLICKED(TOL_1MC1,On1MC1)
    ON_BN_CLICKED(TOL_1MC2,On1MC2)
    ON_BN_CLICKED(TOL_1MC3,On1MC3)
    ON_BN_CLICKED(TOL_1MC4,On1MC4)
    ON_BN_CLICKED(TOL_1MC5,On1MC5)
    ON_BN_CLICKED(TOL_2MC1,On2MC1)
    ON_BN_CLICKED(TOL_2MC2,On2MC2)
    ON_BN_CLICKED(TOL_2MC3,On2MC3)
    ON_BN_CLICKED(TOL_2MC4,On2MC4)
    ON_BN_CLICKED(TOL_2MC5,On2MC5)
    ON_BN_CLICKED(TOL_PROT,OnProt)
    ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL CTolerance::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void CTolerance::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_TOLERANCE);
}

void CTolerance::OnSym1(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_SYM1),&rect);
    CTolerance::PopUpSym(&iSym1,rect.right,rect.bottom);
    SwitchBitMaps(0,iSym1,::GetDlgItem(m_hWnd,TOL_SYM1));
}

void CTolerance::OnSym2(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_SYM2),&rect);
    CTolerance::PopUpSym(&iSym2,rect.right,rect.bottom);
    SwitchBitMaps(8,iSym2,::GetDlgItem(m_hWnd,TOL_SYM2));
}

void CTolerance::PopUpSym(int *ret,int x,int y){
    CSymbol m_Sym;
    m_Sym.x=x;
    m_Sym.y=y;
    *ret=m_Sym.DoModal();
}

void CTolerance::On1MC1(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_1MC1),&rect);
    CTolerance::PopUpMC(&i1Mc1,rect.right,rect.bottom);
    SwitchBitMaps(2,i1Mc1,::GetDlgItem(m_hWnd,TOL_1MC1));
}

void CTolerance::On1MC2(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_1MC2),&rect);
    CTolerance::PopUpMC(&i1Mc2,rect.right,rect.bottom);
    SwitchBitMaps(4,i1Mc2,::GetDlgItem(m_hWnd,TOL_1MC2));
}

void CTolerance::On1MC3(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_1MC3),&rect);
    CTolerance::PopUpMC(&i1Mc3,rect.right,rect.bottom);
    SwitchBitMaps(5,i1Mc3,::GetDlgItem(m_hWnd,TOL_1MC3));
}

void CTolerance::On1MC4(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_1MC4),&rect);
    CTolerance::PopUpMC(&i1Mc4,rect.right,rect.bottom);
    SwitchBitMaps(6,i1Mc4,::GetDlgItem(m_hWnd,TOL_1MC4));
}

void CTolerance::On1MC5(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_1MC5),&rect);
    CTolerance::PopUpMC(&i1Mc5,rect.right,rect.bottom);
    SwitchBitMaps(7,i1Mc5,::GetDlgItem(m_hWnd,TOL_1MC5));
}

void CTolerance::On2MC1(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_2MC1),&rect);
    CTolerance::PopUpMC(&i2Mc1,rect.right,rect.bottom);
    SwitchBitMaps(10,i2Mc1,::GetDlgItem(m_hWnd,TOL_2MC1));
}

void CTolerance::On2MC2(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_2MC2),&rect);
    CTolerance::PopUpMC(&i2Mc2,rect.right,rect.bottom);
    SwitchBitMaps(12,i2Mc2,::GetDlgItem(m_hWnd,TOL_2MC2));
}

void CTolerance::On2MC3(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_2MC3),&rect);
    CTolerance::PopUpMC(&i2Mc3,rect.right,rect.bottom);
    SwitchBitMaps(13,i2Mc3,::GetDlgItem(m_hWnd,TOL_2MC3));
}

void CTolerance::On2MC4(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_2MC4),&rect);
    CTolerance::PopUpMC(&i2Mc4,rect.right,rect.bottom);
    SwitchBitMaps(14,i2Mc4,::GetDlgItem(m_hWnd,TOL_2MC4));
}

void CTolerance::On2MC5(){
    ::GetWindowRect(::GetDlgItem(m_hWnd,TOL_2MC5),&rect);
    CTolerance::PopUpMC(&i2Mc5,rect.right,rect.bottom);
    SwitchBitMaps(15,i2Mc5,::GetDlgItem(m_hWnd,TOL_2MC5));
}

void CTolerance::PopUpMC(int *ret,int x,int y){
    CMaterial m_MC;
    m_MC.x=x;
    m_MC.y=y;
    *ret=m_MC.DoModal();
}

void CTolerance::On1Dia1(){
    if(i1Dia1==BLANK) {
        i1Dia1=NN;
    }else{
        i1Dia1=BLANK;
    }
    SwitchBitMaps(1,i1Dia1,::GetDlgItem(m_hWnd,TOL_1DIA1));
}

void CTolerance::On1Dia2(){
    if(i1Dia2==BLANK) {
        i1Dia2=NN;
    }else{
        i1Dia2=BLANK;
    }
    SwitchBitMaps(3,i1Dia2,::GetDlgItem(m_hWnd,TOL_1DIA2));
}

void CTolerance::On2Dia1(){
    if(i2Dia1==BLANK) {
        i2Dia1=NN;
    }else{
        i2Dia1=BLANK;
    }
    SwitchBitMaps(9,i2Dia1,::GetDlgItem(m_hWnd,TOL_2DIA1));
}

void CTolerance::On2Dia2(){
    if(i2Dia2==BLANK) {
        i2Dia2=NN;
    }else{
        i2Dia2=BLANK;
    }
    SwitchBitMaps(11,i2Dia2,::GetDlgItem(m_hWnd,TOL_2DIA2));
}

void CTolerance::OnProt(){
    if(iProt==BLANK) {
        iProt=PP;
    }else{
        iProt=BLANK;
    }
    SwitchBitMaps(16,iProt,::GetDlgItem(m_hWnd,TOL_PROT));
}

void CTolerance::SwitchBitMaps(int incc,int iMap,HWND hWnd){

    DeleteObject(hBitmap[incc]);
    switch(iMap){
        case AA:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_A));
            break;
        case BB:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_B));
            break;
        case CC:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_C));
            break;
        case DD:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_D));
            break;
        case EE:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_E));
            break;
        case FF:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_F));
            break;
        case GG:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_G));
            break;
        case HH:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_H));
            break;
        case II:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_I));
            break;
        case JJ:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_J));
            break;
        case KK:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_K));
            break;
        case LL:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_L));
            break;
        case MM:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_M));
            break;
        case NN:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_N));
            break;
        case PP:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_P));
            break;
        case RR:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_R));
            break;
        case SS:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_S));
            break;
        case TT:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_T));
            break;
        case UU:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_U));
            break;
        case BLANK:
        default:
            hBitmap[incc]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
            break;
    }
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[incc]);
}

void CTolerance::DoDataExchange(CDataExchange *pDX) {
    DDX_Text(pDX,TOL_1VAL1,cs1Val1);
    DDV_MaxChars(pDX,cs1Val1,127);
    DDX_Text(pDX,TOL_1VAL2,cs1Val2);
    DDV_MaxChars(pDX,cs1Val2,127);
    DDX_Text(pDX,TOL_1DAT1,cs1Dat1);
    DDV_MaxChars(pDX,cs1Dat1,3);
    DDX_Text(pDX,TOL_1DAT2,cs1Dat2);
    DDV_MaxChars(pDX,cs1Dat2,3);
    DDX_Text(pDX,TOL_1DAT3,cs1Dat3);
    DDV_MaxChars(pDX,cs1Dat3,3);
    DDX_Text(pDX,TOL_2VAL1,cs2Val1);
    DDV_MaxChars(pDX,cs2Val1,127);
    DDX_Text(pDX,TOL_2VAL2,cs2Val2);
    DDV_MaxChars(pDX,cs2Val2,127);
    DDX_Text(pDX,TOL_2DAT1,cs2Dat1);
    DDV_MaxChars(pDX,cs2Dat1,3);
    DDX_Text(pDX,TOL_2DAT2,cs2Dat2);
    DDV_MaxChars(pDX,cs2Dat2,3);
    DDX_Text(pDX,TOL_2DAT3,cs2Dat3);
    DDV_MaxChars(pDX,cs2Dat3,3);
    DDX_Text(pDX,TOL_HEIGHT,csHeight);
    DDV_MaxChars(pDX,csHeight,127);
    DDX_Text(pDX,TOL_ID,csID);
    DDV_MaxChars(pDX,csID,127);
}

BOOL CTolerance::OnInitDialog() {
    HWND hWnd;

    IcadDialog::OnInitDialog();
    hBitmap[ 0]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 1]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 2]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 3]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 4]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 5]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 6]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 7]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 8]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[ 9]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[10]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[11]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[12]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[13]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[14]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[15]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hBitmap[16]=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(TOLERANCE_BLANK));
    hWnd=::GetDlgItem(m_hWnd,TOL_SYM1);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[0]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1DIA1);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[1]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1MC1);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[2]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1DIA2);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[3]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1MC2);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[4]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1MC3);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[5]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1MC4);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[6]);
    hWnd=::GetDlgItem(m_hWnd,TOL_1MC5);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[7]);
    hWnd=::GetDlgItem(m_hWnd,TOL_SYM2);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[8]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2DIA1);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[9]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2MC1);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[10]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2DIA2);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[11]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2MC2);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[12]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2MC3);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[13]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2MC4);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[14]);
    hWnd=::GetDlgItem(m_hWnd,TOL_2MC5);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[15]);
    hWnd=::GetDlgItem(m_hWnd,TOL_PROT);
    ::SendMessage(hWnd,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBitmap[16]);

    return(TRUE);
}

void CTolerance::OnOK() {

//    CDialog::OnOK();

    UpdateData(TRUE);

    //first string
    //one
    switch(iSym1){
        case AA:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_2, "{\\Fgdt;a}%%v" );
            break;
        case BB:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_3, "{\\Fgdt;b}%%v" );
            break;
        case CC:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_4, "{\\Fgdt;c}%%v" );
            break;
        case DD:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_5, "{\\Fgdt;d}%%v" );
            break;
        case EE:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_6, "{\\Fgdt;e}%%v" );
            break;
        case FF:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_7, "{\\Fgdt;f}%%v" );
            break;
        case GG:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_8, "{\\Fgdt;g}%%v" );
            break;
        case HH:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_9, "{\\Fgdt;h}%%v" );
            break;
        case II:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGD_10, "{\\Fgdt;i}%%v" );
            break;
        case JJ:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGD_11, "{\\Fgdt;j}%%v" );
            break;
        case KK:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGD_12, "{\\Fgdt;k}%%v" );
            break;
        case RR:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGD_13, "{\\Fgdt;r}%%v" );
            break;
        case TT:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGD_14, "{\\Fgdt;t}%%v" );
            break;
        case UU:
            csString1=ResourceString(IDC_ICADTOLERANCEDIA____FGD_15, "{\\Fgdt;u}%%v" );
            break;
        case BLANK:
        default:
            csString1="%%v"/*DNT*/;
            break;
    }
    //two
    if(i1Dia1==NN) csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_16, "{\\Fgdt;n}" );
    csString1+=cs1Val1;
    switch(i1Mc1){
        case MM:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString1+="%%v"/*DNT*/;
            break;
    }
    //three
    if(i1Dia2==NN) csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_16, "{\\Fgdt;n}" );
    csString1+=cs1Val2;
    switch(i1Mc2){
        case MM:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString1+="%%v"/*DNT*/;
            break;
    }
    //four
    csString1+=cs1Dat1;
    switch(i1Mc3){
        case MM:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString1+="%%v"/*DNT*/;
            break;
    }
    //five
    csString1+=cs1Dat2;
    switch(i1Mc4){
        case MM:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString1+="%%v"/*DNT*/;
            break;
    }
    //six
    csString1+=cs1Dat3;
    switch(i1Mc5){
        case MM:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_20, "{\\Fgdt;m}" );
            break;
        case LL:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_21, "{\\Fgdt;l}" );
            break;
        case SS:
            csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_22, "{\\Fgdt;s}" );
            break;
        case BLANK:
        default:
            break;
    }
    //Second string
    //one
    switch(iSym2){
        case AA:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_2, "{\\Fgdt;a}%%v" );
            break;
        case BB:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_3, "{\\Fgdt;b}%%v" );
            break;
        case CC:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_4, "{\\Fgdt;c}%%v" );
            break;
        case DD:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_5, "{\\Fgdt;d}%%v" );
            break;
        case EE:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_6, "{\\Fgdt;e}%%v" );
            break;
        case FF:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_7, "{\\Fgdt;f}%%v" );
            break;
        case GG:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_8, "{\\Fgdt;g}%%v" );
            break;
        case HH:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGDT_9, "{\\Fgdt;h}%%v" );
            break;
        case II:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGD_10, "{\\Fgdt;i}%%v" );
            break;
        case JJ:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGD_11, "{\\Fgdt;j}%%v" );
            break;
        case KK:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGD_12, "{\\Fgdt;k}%%v" );
            break;
        case RR:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGD_13, "{\\Fgdt;r}%%v" );
            break;
        case TT:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGD_14, "{\\Fgdt;t}%%v" );
            break;
        case UU:
            csString2=ResourceString(IDC_ICADTOLERANCEDIA____FGD_15, "{\\Fgdt;u}%%v" );
            break;
        case BLANK:
        default:
            csString2="%%v"/*DNT*/;
            break;
    }
    //two
    if(i2Dia1==NN) csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_16, "{\\Fgdt;n}" );
    csString2+=cs2Val1;
    switch(i2Mc1){
        case MM:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString2+="%%v"/*DNT*/;
            break;
    }
    //three
    if(i2Dia2==NN) csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_16, "{\\Fgdt;n}" );
    csString2+=cs2Val2;
    switch(i2Mc2){
        case MM:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString2+="%%v"/*DNT*/;
            break;
    }
    //four
    csString2+=cs2Dat1;
    switch(i2Mc3){
        case MM:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString2+="%%v"/*DNT*/;
            break;
    }
    //five
    csString2+=cs2Dat2;
    switch(i2Mc4){
        case MM:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_17, "{\\Fgdt;m}%%v" );
            break;
        case LL:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_18, "{\\Fgdt;l}%%v" );
            break;
        case SS:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_19, "{\\Fgdt;s}%%v" );
            break;
        case BLANK:
        default:
            csString2+="%%v"/*DNT*/;
            break;
    }
    //six
    csString2+=cs2Dat3;
    switch(i2Mc5){
        case MM:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_20, "{\\Fgdt;m}" );
            break;
        case LL:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_21, "{\\Fgdt;l}" );
            break;
        case SS:
            csString2+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_22, "{\\Fgdt;s}" );
            break;
        case BLANK:
        default:
            break;
    }

    if(csString1.GetLength()>15){
        if(csString2.GetLength()>15){
            csString1+="\n"/*DNT*/;
            csString1+=csString2;
        }
        csString2.Empty();
    }else{
        if(csString2.GetLength()>15){
            csString1=csString2;
            csString2.Empty();
        }else{
            csString1.Empty();
            csString2.Empty();
        }
    }
    if(csString1.IsEmpty()){
        if(!csHeight.IsEmpty()) csString1=csHeight;
    }else{
        if(!csHeight.IsEmpty()) csString1+="\n"/*DNT*/+csHeight;
    }
    if(iProt==PP) csString1+=ResourceString(IDC_ICADTOLERANCEDIA____FGD_23, "{\\Fgdt;p}" );
    if(csString1.IsEmpty()){
        if(!csID.IsEmpty()) csString1=csID;
    }else{
        if(!csID.IsEmpty()) csString1+="\n"/*DNT*/+csID;
    }
    if(csString1.IsEmpty()){
        EndDialog(IDCANCEL);
    }else{
        EndDialog(IDOK);
    }
}

CTolerance::CTolerance():IcadDialog(TOL_MAIN) {
    iSym1=i1Mc1=i1Mc2=i1Mc3=i1Mc4=i1Mc5=BLANK;
    iSym2=i2Mc1=i2Mc2=i2Mc3=i2Mc4=i2Mc5=BLANK;
    i1Dia1=i1Dia2=i2Dia1=i2Dia2=iProt=BLANK;
}

CTolerance::~CTolerance(){
	for(int i=0;i<17;i++) DeleteObject(hBitmap[i]);
}


//***This is the main window
// EBATECH(watanabe)-[changed
//void CMainWindow::IcadToleranceDia(void) {
void CMainWindow::IcadToleranceDia(double* pt /* = NULL */) {
// ]-EBATECH(watanabe)
    int ret;
	sds_point pt1,pt2,pt210;
    struct resbuf *pRbb,rbf,rbt;

    sds_entmake(NULL);
    CTolerance m_Tol;
    ret=m_Tol.DoModal();
    switch(ret){
        case IDOK:
            SDS_getvar(NULL,DB_QUCSXDIR,&rbf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	        SDS_getvar(NULL,DB_QUCSYDIR,&rbt,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            ic_ptcpy(pt2,rbf.resval.rpoint);
            //cross the two to get extrusion direction for new ent
	        ic_crossproduct(rbf.resval.rpoint,rbt.resval.rpoint,pt210);
            //ucs to wcs
            rbf.restype=RTSHORT;
            rbf.resval.rint=1;
            rbt.restype=RTSHORT;
            rbt.resval.rint=0;

            // EBATECH(watanabe)-[changed
            //if((ret=internalGetpoint(NULL,ResourceString(IDC_ICADTOLERANCEDIA__NSELE_24, "\nSelect location for tolerance: " ),pt1))!=RTNORM) goto bail;
            //sds_trans(pt1,&rbf,&rbt,0,pt1);
            if (pt == NULL)
              {
                ret = internalGetpoint(NULL, ResourceString(IDC_ICADTOLERANCEDIA__NSELE_24, "\nSelect location for tolerance: " ), pt1);
                if (ret != RTNORM) goto bail;
                sds_trans(pt1, &rbf, &rbt, 0, pt1);
              }
            else
              {
                sds_point_set(pt, pt1);
              }
            // ]-EBATECH(watanabe)
            // EBATECHC(CNBR) -[ 2003/4/1 TOLERANCE has dimesntion system variables override.
			if( SDS_getvar(NULL,DB_QDIMSTYLE,&rbf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM )
			{
				cmd_initialize();
				cmd_setparlink(SDS_CURDWG);
				if((pRbb=sds_buildlist( RTDXF0,ResourceString(IDC_ICADTOLERANCEDIA_TOLERA_25, "TOLERANCE" ),
                                         1,m_Tol.csString1,
                                         3,rbf.resval.rstring,
                                        10,pt1,
                                        11,pt2,
                                       210,pt210,
                                           0))==NULL){
					ret=RTERROR;
				}else{
					cmd_buildEEdList(pRbb);
					sds_entmake(pRbb);
					IC_RELRB(pRbb);
					ret=RTNORM;
				}
				IC_FREE(rbf.resval.rstring);
				cmd_CleanUpDimVarStrings();
			}
			if( ret != RTNORM )
	  	        MessageBox(ResourceString(IDC_ICADTOLERANCEDIA_MORE_M_26, "More memory is needed to do this operation" ),NULL,MB_OK|MB_ICONSTOP);
            // EBATECHC(CNBR) ]-
            break;
    }
bail:
    return;
}

