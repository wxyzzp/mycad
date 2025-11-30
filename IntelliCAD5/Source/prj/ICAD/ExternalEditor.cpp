// ExternalEditor.cpp: CExternalEditor Class
//		by EBATECH(CNBR)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Icad.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "ExternalEditor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
BOOL CExternalEditor::RunEditor()
{
	return IsExternalEditor() && SearchEditor() && GetTempFilename() && WriteTempFile();
}

BOOL CExternalEditor::IsExternalEditor()
{
	return (( m_MtextedValue.CompareNoCase( _T("")) == 0 ||
		m_MtextedValue.CompareNoCase( _T(".")) == 0 ||
		m_MtextedValue.CompareNoCase( _T("Internal")) == 0 ) ? FALSE : TRUE );
}
BOOL CExternalEditor::SearchEditor()
{
	CString Result;
	TCHAR* pFilePart;
	DWORD  nCnt;
	nCnt = SearchPath( NULL, m_MtextedValue, _T(".EXE"), 
		_MAX_PATH, Result.GetBuffer( _MAX_PATH ), &pFilePart );
	if( nCnt > 0 ){
		Result.ReleaseBuffer(nCnt);
		m_MtextedValue = Result;
		return TRUE;
	}else{
		Result.ReleaseBuffer(0);
		return FALSE;
	}
}

BOOL CExternalEditor::GetTempFilename()
{
	CString aPath;
	DWORD nCnt;

	nCnt = GetTempPath( _MAX_PATH, aPath.GetBuffer( _MAX_PATH ));
	if( nCnt > 0 ){
		aPath.ReleaseBuffer( nCnt );
	}else{
		aPath.ReleaseBuffer();
		return FALSE;
	}

	nCnt = GetTempFileName( aPath, _T("CAD"), 0, m_TempFile.GetBuffer( _MAX_PATH ));
	m_TempFile.ReleaseBuffer();
	if( nCnt != 0 ){
		return TRUE;
	}else{
		return FALSE;
	}
}

BOOL CExternalEditor::WriteTempFile()
{
	CStdioFile	tempFile;
	if( !tempFile.Open( m_TempFile, 
		CFile::modeCreate | CFile::modeWrite | CFile::typeText ) ) {
		return FALSE;
	}
	// VT. 4-4-02. Fix of possible writing beyond allocated block.
	char	mbs[ BREAK_LIMIT + 3 ];
	int cnt, idx;
	for( cnt = idx = 0 ; idx < m_TextString.GetLength() ; ){
		if( IsDBCSLeadByte( m_TextString[idx] ) ){
			mbs[cnt++] = m_TextString[idx++];
			mbs[cnt++] = m_TextString[idx++];
			if( cnt > BREAK_OK ){
				mbs[cnt++] = '\\';
				mbs[cnt++] = '\n';
				mbs[cnt] = '\0';
				tempFile.WriteString(mbs);
				cnt = 0;
			}
		}else{
			CString fHead = m_TextString.Mid( idx, 2 );
			if( fHead.CompareNoCase( _T("\\P")) == 0 ){
				mbs[cnt++] = '\n';
				mbs[cnt] = '\0';
				idx+=2;
				tempFile.WriteString(mbs);
				cnt = 0;
			} else if (
				fHead.CompareNoCase( _T("\\F")) == 0 ||
				fHead.CompareNoCase( _T("\\S")) == 0 ){
				while( idx < m_TextString.GetLength()){ 
					mbs[cnt++] = m_TextString[idx++];
					if( cnt >= BREAK_LIMIT ){
						mbs[cnt++] = '\\';
						mbs[cnt++] = '\n';
						mbs[cnt] = '\0';
						tempFile.WriteString(mbs);
						cnt = 0;
					}
					// VT. 4-4-02. Fix. This (vv) check should be after (^^).
					if( mbs[cnt-1] == ';' )
						break;
				}
			}else if ( fHead.CompareNoCase( _T("\x0D\x0A")) == 0){
				mbs[cnt++] = '\n';
				mbs[cnt] = '\0';
				idx += 2;
				tempFile.WriteString(mbs);
				cnt = 0;
			}else{
				mbs[cnt++] = m_TextString[idx++];
				if( mbs[cnt-1] == ' ' && cnt > BREAK_OK ){
					mbs[cnt++] = '\n';
					mbs[cnt] = '\0';
					tempFile.WriteString(mbs);
					cnt = 0;
				}
				else if( cnt >= BREAK_LIMIT ){
					mbs[cnt++] = '\\';
					mbs[cnt++] = '\n';
					mbs[cnt] = '\0';
					tempFile.WriteString(mbs);
					cnt = 0;
				}
			}
		}
	}
	if( cnt > 0 ){
		mbs[cnt] = '\0';
		tempFile.WriteString(mbs);
	}

	tempFile.Close();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CExternalEditor::Wait()
{
	STARTUPINFO	si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	CString cmdLine;
	HANDLE hProcess;
	DWORD ExitCode;
	BOOL sts;

	si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW; 
    si.wShowWindow = SW_SHOWNORMAL;
	// cmdLine:= ""C:\WINDOW\NOTEPAD.EXE" "D:\DATA\SAMPLE\CAD0284.$$$""
	cmdLine.Format( "\"%s\" \"%s\"", m_MtextedValue, m_TempFile );
	// CreateProcess
	if( CreateProcess(NULL,
		const_cast<LPTSTR>((LPCTSTR)cmdLine),
		NULL,NULL,FALSE,(DWORD)NORMAL_PRIORITY_CLASS,
		NULL,NULL,&si,&pi)){
		// Suceed, wait its termination
		while( hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,TRUE,pi.dwProcessId)){
			sts = GetExitCodeProcess( hProcess, &ExitCode);
			CloseHandle(hProcess);
			if( sts && ExitCode == STILL_ACTIVE ){
				Sleep(500);
			}else{
				break;
			}
		}
		// Release resorces
		if( pi.hProcess && pi.hProcess != INVALID_HANDLE_VALUE){
			CloseHandle(pi.hProcess);
		}
		if( pi.hThread && pi.hThread != INVALID_HANDLE_VALUE){
			CloseHandle(pi.hThread);
		}
		// Update m_TextString
		ReadTempFile();
		DeleteFile(m_TempFile);
		m_TempFile = _T("");
		return TRUE;
	}
	return FALSE;
}
BOOL CExternalEditor::ReadTempFile()
{
	CStdioFile	tempFile;
	CString	tempLine;
	int	nLen;
	if( !tempFile.Open( m_TempFile, CFile::modeRead | CFile::typeText ) ) {
		return FALSE;
	}
	m_TextString = _T("");
	while( tempFile.ReadString( tempLine )){
		nLen = tempLine.GetLength();
		char *dupp;
		dupp = NULL;
		db_astrncpy(&dupp, (LPCTSTR)tempLine, -1);
		if( dupp ){
			// Replace '\t' to ' '
			for(int i=0;dupp[i]!='\0';i++){ 
				if( dupp[i] == '\t' ){
					dupp[i] = ' '; 
				}
			}
			// Check the end of paragraphs.
			_mbsrev( (unsigned char*)dupp );
			if( _mbsnicmp( 
				(const unsigned char*)dupp, 
				(const unsigned char*)" ", 1 ) == 0 ){
				m_TextString += tempLine;
			}else if(  _mbsnicmp( 
				(const unsigned char*)dupp, 
				(const unsigned char*)"\\", 1 ) == 0 ){
				m_TextString += tempLine.Left( nLen-1 );
			}else if( _mbsnicmp( 
				(const unsigned char*)dupp, 
				(const unsigned char*)"P\\", 2 ) == 0 ){
				m_TextString += tempLine;
			}else{
				tempLine += _T( "\\P" );
				m_TextString += tempLine;
			}
			IC_FREE(dupp);
		}
	}
	tempFile.Close();
	nLen = m_TextString.GetLength();
	char *dupp;
	dupp = NULL;
	db_astrncpy(&dupp, (LPCTSTR)m_TextString, -1);
	if( dupp ){
		_mbsrev( (unsigned char*)dupp );
		if( _mbsnicmp( 
			(const unsigned char*)dupp, 
			(const unsigned char*)"P\\", 2 ) == 0 ){
			m_TextString = m_TextString.Left( nLen-2 );
		}
		IC_FREE(dupp);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void LaunchMTEXTEditor( CString& TextString, BOOL bNew )
{
	CExternalEditor Ed;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QMTEXTED,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if( rb.resval.rstring ){
		Ed.SetMTextedValue( CString( rb.resval.rstring ));
		Ed.SetTextString( TextString );
		IC_FREE( rb.resval.rstring );

		if( Ed.RunEditor() == TRUE ){
			if( Ed.Wait() == TRUE ){
				if( TextString.Compare( Ed.GetTextString()) != 0){
					TextString = Ed.GetTextString();
					if( bNew ){
						CreateMTEXTObject();
					}
				}
			}
			return;
		}
	}
// EBATECH(watanabe)-[get mtext info only
//	ExecuteUIAction(ICAD_WNDACTION_MTEXT);
	UINT nId = (bNew) ? ICAD_WNDACTION_MTEXT : ICAD_WNDACTION_MTEXT2;
	ExecuteUIAction(nId);
//]-EBATECH(watanabe)
}

int CreateMTEXTObject( void )
{
	extern struct SDS_mTextData  SDS_mTextDataPtr;
	struct resbuf rbf, rbt;
	struct resbuf *rbb = 0;
	sds_point pt210, pt1;
	int ret = -1;

	SDS_getvar(NULL,DB_QUCSXDIR,&rbf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbt,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    //cross the two to get extrusion direction for new ent
	ic_crossproduct(rbf.resval.rpoint,rbt.resval.rpoint,pt210);

    rbf.restype=RTSHORT;
    rbf.resval.rint=1;
    rbt.restype=RTSHORT;
    rbt.resval.rint=0;
    //take the lesser of the X's
    if(SDS_mTextDataPtr.ptInsertion[0]>SDS_mTextDataPtr.ptOppCorner[0]){
        pt1[0]=SDS_mTextDataPtr.ptOppCorner[0];
    }else{
        pt1[0]=SDS_mTextDataPtr.ptInsertion[0];
    }
    //take the greater of the Y's
    if(SDS_mTextDataPtr.ptInsertion[1]>SDS_mTextDataPtr.ptOppCorner[1]){
        pt1[1]=SDS_mTextDataPtr.ptInsertion[1];
    }else{
        pt1[1]=SDS_mTextDataPtr.ptOppCorner[1];
    }
    //take any Z
    pt1[2]=SDS_mTextDataPtr.ptOppCorner[2];
	//adjust alignment point horizontally
	switch (SDS_mTextDataPtr.nAlignment){
		//center
		case 2:
		case 5:
		case 8:
			pt1[0]+=0.5*SDS_mTextDataPtr.rBoxWidth;
			break;
			//right
		case 3:
		case 6:
		case 9:
			pt1[0]+=SDS_mTextDataPtr.rBoxWidth;
			break;
		default:
			break;
	}
	//adjust alignment point vertically
	switch (SDS_mTextDataPtr.nAlignment){
		//middle
		case 4:
		case 5:
		case 6:
			pt1[1]-=0.5*fabs(SDS_mTextDataPtr.ptInsertion[1]-SDS_mTextDataPtr.ptOppCorner[1]);
			break;
			//bottom
		case 7:
		case 8:
		case 9:
			pt1[1]-=fabs(SDS_mTextDataPtr.ptInsertion[1]-SDS_mTextDataPtr.ptOppCorner[1]);
			break;
		default:
			break;
	}

    sds_trans(pt1,&rbf,&rbt,0,pt1);
    sds_entmake(NULL);
    rbb=sds_buildlist(RTDXF0,"MTEXT"/*DNT*/,
		 1,SDS_mTextDataPtr.text.GetBuffer(SDS_mTextDataPtr.text.GetLength()),
        10,pt1,
        40,SDS_mTextDataPtr.rTextHeight,
        41,SDS_mTextDataPtr.rBoxWidth,
        71,SDS_mTextDataPtr.nAlignment,
        72,SDS_mTextDataPtr.nDirection,
         7,SDS_mTextDataPtr.szTextStyle,
       210,pt210,
        50,SDS_mTextDataPtr.rRotAngle,
         0);
    if(rbb) {
        ret=sds_entmake(rbb);
	}
	IC_RELRB(rbb);
	return ret;
}
