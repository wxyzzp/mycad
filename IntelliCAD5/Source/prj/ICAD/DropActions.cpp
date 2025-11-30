/*
**  © 2001 Stabiplan BV, all rights reserved
**
**  Name       : DropActions.cpp 
**  Programmer : HBL
**
**  Purpose    : Given an AutoCAD XMLDOM i-drop Package, do what is intended...
**  Description: 
**
*/

#include "Icad.h"/*DNT*/
#include "DropActions.h"
#include <atlconv.h>
#include <afxinet.h>

using namespace MSXML;

///////////////////////////////////////////////////////////////////
// HRESULT throwing inline routines, return routins
inline void
ThrowIfFail(HRESULT hr)
{
    if (FAILED(hr)) {
#ifdef DEBUG
        ASSERT(FALSE);
#endif // DEBUG
        throw hr;
    }
}

inline void
ThrowIfNotOK(HRESULT hr)
{
    if (hr != S_OK) {
#ifdef DEBUG
        ASSERT(FALSE);
#endif // DEBUG
        throw hr;
    }
}

inline void
ThrowIfNotTrue(int b)
{
    if (b != 1) {
#ifdef DEBUG
        ASSERT(FALSE);
#endif // DEBUG
        throw E_FAIL;
    }
}


// The class DummySite is just implemented here to solve a Microsoft bug
// see report Q235344. 

class DummySite : public IUnknown
{
public:
    DummySite() : _ulRefCount(1) {}

public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
    {
        if (!ppvObject)
            return E_POINTER;

        if (riid == IID_IUnknown)
        {
            this->AddRef();
            *ppvObject = (IUnknown*)this;
        }
        else
        {
            *ppvObject = NULL;
            return E_NOINTERFACE;
        }

        return S_OK;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void)
    {
        return ++_ulRefCount;
    }

    virtual ULONG STDMETHODCALLTYPE Release( void)
    {
        ULONG ul = --_ulRefCount;
        if (!ul)
            delete this;
        return ul;
    }

protected:
    ULONG _ulRefCount;
};

HRESULT XMLDummySecurity(IXMLDOMDocument * pXMLDocument)
{
    HRESULT hr;
    IObjectWithSite * pObjWithSite = NULL;
    if (SUCCEEDED(hr = pXMLDocument->QueryInterface(IID_IObjectWithSite, (void **)&pObjWithSite)))
    {
        DummySite * pObj = new DummySite();
        pObjWithSite->SetSite(pObj);
        pObjWithSite->Release();
        pObj->Release();
    }
    return hr;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDropActions::CDropActions()
{
}

CDropActions::~CDropActions()
{
}

/*
**  Function   : LoadXml
**  Description: Create our XMLDOM from this string
**                     
**  Arguments  : - 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::LoadXml()
{

   try 
   { 
      // Create our XMLDOM if needed
      if (m_pXmlDoc == NULL) 
      {
   		ThrowIfNotOK(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_ALL,
	   		           __uuidof(IXMLDOMDocument), (void**)&m_pXmlDoc));
      }


      XMLDummySecurity(m_pXmlDoc);

      VARIANT_BOOL vIsSuccessful = VARIANT_FALSE;

      CString csXmlUrl(m_csXmlUrl);
  	   csXmlUrl.Replace('\\', '/');   // paranoid

      CComVariant varXmlUrl(csXmlUrl.AllocSysString());

      // Populate the XML...
      // We use the URL of the XML file, not the xml string on the clipboard. 
      // We need to use the URL because else we don't know how to find the associated schema file.
      ThrowIfNotOK(m_pXmlDoc->put_async(VARIANT_FALSE));

      ThrowIfNotOK(m_pXmlDoc->load(varXmlUrl, &vIsSuccessful));

      ThrowIfNotTrue(vIsSuccessful == VARIANT_TRUE);

   } 
   catch (HRESULT hr) 
   {
      sds_printf("\nProblem 0x%X loading Xml %s", hr, m_csXmlUrl);

      CComPtr<IXMLDOMParseError> pIParseError;

      if(SUCCEEDED(m_pXmlDoc->get_parseError(&pIParseError)))
      {
         CComBSTR bstrReason;
         USES_CONVERSION;
         pIParseError->get_reason(&bstrReason);
         sds_printf("\n%s", W2A(bstrReason));
      }

      return false;
   }
   return true;
}


/*
**  Function   : GetLocalFile
**  Description: Build a full path local file name
**                     
**  Arguments  : const CString& csDataFile - name of the symbol
**               CString& csLocalFile  - Full path file name
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::GetLocalFile(const CString& csDataFile )
{
   int nIndex;

   CString csLocalFile = csDataFile;

   // strip fileName
   nIndex = csLocalFile.ReverseFind('/');
   if (nIndex > 0)
   {
      csLocalFile = csLocalFile.Mid(nIndex + 1);
   }

   m_csBlockName        = csLocalFile;

   // strip extension from blockname
   nIndex = m_csBlockName.ReverseFind('.');
   if (nIndex > 0)
   {
      m_csBlockName = m_csBlockName.Left( nIndex );
   }

   // get windows temp patch
	CString csTempPath;
	DWORD nCnt;
	nCnt = GetTempPath( _MAX_PATH, csTempPath.GetBuffer( _MAX_PATH ));
	if( nCnt > 0 ){
		csTempPath.ReleaseBuffer( nCnt );
	}else{
		csTempPath.ReleaseBuffer();
		return false;
	}

	CString csLongTempPath;
	nCnt = GetLongPathName(csTempPath, csLongTempPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	if( nCnt > 0 ){
		csLongTempPath.ReleaseBuffer( nCnt );
	}else{
		csLongTempPath.ReleaseBuffer();
		return false;
	}

   m_csFileName = csLongTempPath + csLocalFile;
   
   // convert backslashes to slash, the commandline evaluates backslash as an escape character
   m_csFileName.Replace('\\', '/');
   

   return true;
}

/*
**  Function   : Execute
**  Description: Download file and inserts the block !
**                     
**  Arguments  : - 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::Execute()
{
   CString       csDataFile, csDataFileUrl, csCommandLine;
   DWORD         dwServiceType;
   bool          bCopied = false;
   bool          bResult = false;
   bool          bOk = false;

   // get parameters for idrop 
   GetIdropParams( csDataFile );

   // get a temporary local file name to save the download
   GetLocalFile( csDataFile );

   // Download file from internet server
   if (GetDataFileURL(m_csXmlUrl, csDataFile, csDataFileUrl, dwServiceType))
   {
      if (dwServiceType == AFX_INET_SERVICE_HTTP)
        bCopied = DownloadFile(csDataFileUrl, m_csFileName);
      else 
        bCopied = false; // not supported
   }

   if (bCopied)
   {
      // The downloaded file is must now be inserted in current drawing
      // insertion is done by sending an insert command to the Icad Commandline.
      // directly calling sds_command("insert") couses Icad to hang maybe becouse we're in the wrong tread now?

      struct resbuf *pRb = NULL;

      pRb = sds_tblsearch( "BLOCK", m_csBlockName, 0);
      if ( pRb )
      {
        csCommandLine = "(command \"_.INSERT\" \"" + m_csBlockName + "\" Pause \"\" \"\" PAUSE)";
      } else
      {
        csCommandLine = "(command \"_.INSERT\" \"" + m_csFileName + "\" Pause \"\" \"\" PAUSE)";
      }
      SendCommandToCAD( csCommandLine, true );
     
      //TODO The downloaded file in tempfolder could be deleted now
   }

   return bResult;
}


/*
**  Function   : SendCommandToCAD
**  Description: Send a command to the command line
**                     
**  Arguments  : char *str,int cancel 
**  
**  Remarks    : 
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::SendCommandToCAD( const char *str, bool bCancel)
{
   char *buffer = NULL;

   if (bCancel) 
   {
      buffer = (char *) calloc(strlen(str) + 7, 1);
      strcpy(buffer, "^C^C^C");
      strcat(buffer, str);
   }
   else 
      buffer = (char*) str;         

   sds_sendmessage(buffer);

   if (bCancel)
      free(buffer);

   return true;
}


/*
**  Function   : GetAttribute
**  Description: 
**                     
**  Arguments  : CComPtr<IXMLDOMNode>& pNode,
**               const string& strSubNode,
**               const string& strAttrib 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::GetAttribute( CComPtr<IXMLDOMNode>& pNode,
                                 const CString& csSubNode,
                                 const CString& csAttribName,
                                 CString&       csAttribValue )
{
   CComPtr<IXMLDOMNode> pSubNode, pAttribNode;
   CComBSTR bstrAttribName(csAttribName);
   CComBSTR bstrSubNodeName(csSubNode);
   CComVariant vAttribValue;
   CComPtr<IXMLDOMNamedNodeMap> pAttribMap;
   CString csResult;
   bool bResult = false;

   csAttribValue = "";

   try
   {
      ThrowIfNotOK(pNode->selectSingleNode(bstrSubNodeName, &pSubNode));
      ThrowIfNotOK(pSubNode->get_attributes(&pAttribMap));
      ThrowIfNotOK(pAttribMap->getNamedItem(bstrAttribName, &pAttribNode));
      ThrowIfNotOK(pAttribNode->get_nodeValue(&vAttribValue));
      ThrowIfNotTrue(vAttribValue.vt != VT_EMPTY);

      csAttribValue = vAttribValue.bstrVal;
      bResult = true;
   }
   catch (HRESULT hr)
   {
      hr;
      csAttribValue = "";
      bResult = false;
   }

   return bResult;
}


/*
**  Function   : GetElementText
**  Description: 
**                     
**  Arguments  : CComPtr<IXMLDOMNode>& pNode,
**               const string& strSubNode,
**               CString& csText 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::GetElementText( CComPtr<IXMLDOMNode>& pNode,
                                   const CString& csSubNode,
                                   CString& csText)
{
   CComPtr<IXMLDOMNode> pSubNode;

   CComBSTR bstrSubNodeName(csSubNode);
   CComBSTR bstrText;
   bool     bResult = false;

   csText = "";

   try
   {
      ThrowIfNotOK(pNode->selectSingleNode(bstrSubNodeName, &pSubNode));
      ThrowIfNotOK(pSubNode->get_text(&bstrText));

      csText = bstrText;
      bResult = true;
   }
   catch (HRESULT hr)
   {
      hr;
      csText = "";
      bResult = false;
   }

   return bResult;
}


/*
**  Function   : GetDataFileURL
**  Description: 
**                     
**  Arguments  : CComPtr<IXMLDOMNode>& pNode,
**               const string& strSubNode,
**               CString& csText 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::GetDataFileURL( const CString& csXmlUrl,      
                                   const CString& csDataFile, 
                                   CString&       csDataFileUrl,
                                   DWORD&         dwServiceType)
{
    CString       csLocalFile, csRemoteFile, csServer, csObject;
    INTERNET_PORT nPort = 0;
    BOOL          bParsed = false;
    int           nIndex = 0;
    bool          bResult = false;

    // Check for an absolute URL
    bParsed = AfxParseURL(csDataFile, dwServiceType, csServer, csObject, nPort);
    if (bParsed && dwServiceType == AFX_INET_SERVICE_HTTP && !csServer.IsEmpty())
    {
       csDataFileUrl = csDataFile;
       bResult = true;
       // ready!
    }
    else
    {
       // It's a relative URL or a local file
       bParsed = AfxParseURL(csXmlUrl, dwServiceType, csServer, csObject, nPort);
       if (bParsed)
       {
          if (dwServiceType == AFX_INET_SERVICE_HTTP)
          {
             if (csDataFile[0] == '/')
             {
                csDataFileUrl = "http://" + csServer + csDataFile;
                bResult = true;
             }
             else
             {
                csDataFileUrl = csXmlUrl;
                csDataFileUrl.Replace('\\', '/');   // paranoid

                nIndex = csDataFileUrl.ReverseFind('/');
                if (nIndex == -1) 
                {
                   csDataFileUrl.Empty();
                   bResult = false;
                } 
                else
                { 
                   csDataFileUrl = csDataFileUrl.Left(nIndex + 1) + csDataFile;
                   bResult = true;
                }
  
             }
          }
          else if (dwServiceType == AFX_INET_SERVICE_FILE)
          {
             // local file
             csDataFileUrl = csObject;

             csDataFileUrl.Replace('\\', '/');   // paranoid
   
             nIndex = csDataFileUrl.ReverseFind('/');
             if (nIndex == -1) 
             {
                csDataFileUrl.Empty();
                bResult = false;
             } 
             else
             { 
                csDataFileUrl = csDataFileUrl.Left(nIndex + 1) + csDataFile;
                bResult = true;
             }
          }
       }
    }

    return bResult;
}

/*
**  Function   : GetIdropParams
**  Description: Iterate through the XMLDOM, and get a list of standard IDROP
**               parameters 
**               
**  Arguments  : cmdInsertParams& CmdInsertParams 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if found
*/
bool CDropActions::GetIdropParams( CString& csDataFile)
{
   bool bResult = false;
   HRESULT hr;

   CComQIPtr<IXMLDOMNode> pDocNode(m_pXmlDoc);

   CComPtr<IXMLDOMNode> pNode, pOurNode;
   CString csDataFileURL;
   CString csLocalFile;
   int     nIndex = -1;


   CComBSTR bstrNodeName1("package/dataset/datasrc[@clipformat=\"CF_IDROP.DWG\"]");
   CComBSTR bstrNodeName2("package/dataset/datasrc[@clipformat=\"CF_IDROP.dwg\"]");
   CComBSTR bstrNodeName3("package");

   try 
   {
       // Note that the following query is case-sensitive.
       // By convention, all clipboard formats are defined in uppercase.
       hr = pDocNode->selectSingleNode(bstrNodeName1, &pOurNode);
       if (hr != S_OK)
          hr = pDocNode->selectSingleNode(bstrNodeName2, &pOurNode);

       if (hr != S_OK )  
       {  // another XML schema, read "package/dataset/defaultsrc
          hr = pDocNode->selectSingleNode(bstrNodeName3, &pOurNode);
          ThrowIfNotOK(hr);
          ThrowIfNotTrue(GetAttribute(pOurNode, "dataset", "defaultsrc", csDataFile));
       } else
       {
          ThrowIfNotTrue(GetAttribute(pOurNode, "datafile", "src", csDataFile));
       }
       
       bResult = true;
   }
   catch (HRESULT hr)
   {
      hr;
      // unkown Idrop Format
      bResult = false;
   }

   return bResult;
}



/*
**  Function   : DropIDrop
**  Description: Create our XMLDOM from this package.
**                     
**  Arguments  : COleDataObject* pDataObject 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : DROPEFFECT
*/
DROPEFFECT CDropActions::DropIDrop(COleDataObject* pDataObject)
{

   DROPEFFECT result = DROPEFFECT_NONE;

   try 
   {
     // Save the URL of this package...
     HGLOBAL hGlobal = pDataObject->GetGlobalData(CF_TEXT);
     LPCSTR pText = (LPCSTR)GlobalLock(hGlobal);
     ThrowIfNotTrue(AfxIsValidString(pText));
     XmlUrl(pText);
     GlobalUnlock(hGlobal);
     GlobalFree(hGlobal);            // free data

     // The following code is used to check for an IDrop operation

     CLIPFORMAT cfPackageStream = RegisterClipboardFormat(/*MSG0*/"CF_IDROP_PKGSTM");
     FORMATETC idropfmt = { cfPackageStream, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM };

     // All packages should have this format...
     ThrowIfNotTrue(pDataObject->IsDataAvailable(cfPackageStream, &idropfmt));

     // Values used to calculate the size of the stream
     LARGE_INTEGER nPos;
     ULARGE_INTEGER nEnd;
     ZeroMemory(&nPos, sizeof(nPos));

     // Initialize stg contruct
     STGMEDIUM stg = {TYMED_ISTREAM, NULL, NULL };
     ThrowIfNotTrue(pDataObject->GetData(cfPackageStream, &stg, &idropfmt));

     // Calculates the number of bytes in the stream by pointing to the end.
     stg.pstm->Seek(nPos, STREAM_SEEK_END, &nEnd);
     // Now point to the beginning
     stg.pstm->Seek(nPos, STREAM_SEEK_SET, NULL);

     // IStream are potentially very large, and an unsigned large integer is 
     // used to retrieve their size, which is a 64 bits usigned integer.
     // In our case, the XML packages should remain reasonably
     // small (a few hundred bytes).  Therefore we will use only the lower 
     // part of the Large Integer which can be represented by a 16 bits 
     // usigned int. This sets a limit to the XML package size of 
     // 4,294,967,295 bytes. I think it should be plenty for now.
     // **js 04/07/00
     UINT size = nEnd.LowPart;

     // Too big for us to handle, we just bail out.
     ThrowIfNotTrue(nEnd.HighPart == 0);

     // Copy directly the stream into a CString buffer
     CString csXmlPackage;
     char* buffer = csXmlPackage.GetBuffer(size + 2);
     csXmlPackage.LockBuffer();
     HRESULT hr = stg.pstm->Read((void *)buffer, size, NULL);

     // We need to properly terminate the string for the DOM parser
     buffer[size] = '\0';
     buffer[size+1] = '\0';

     csXmlPackage.UnlockBuffer();
     ReleaseStgMedium(&stg);

     ThrowIfNotOK(hr);

     // The XML on the clipboard is not used, the XML is loaded 
     // from the URL (although we have the XML, we can not validate it because
     // the schema file is still on the internet server).

     // Load the XML in our XMLDOM object...
     ThrowIfNotTrue(LoadXml());

     // From now on, we know it's IDROP and we
     
     result = DROPEFFECT_COPY;

   } 
   catch (HRESULT hr) 
   {
     hr;
     result = DROPEFFECT_NONE;
   }

   return result;
}

/*
**  Function   : DownloadFile
**  Description: Download the DWG from http server
**               
**  Arguments  : const CString& csUrl
**               const CString& csFileName 
**  
**  Remarks    :
**  See also   :
**
**  Returns    : bool TRUE if success
*/
bool CDropActions::DownloadFile(const CString& csUrl, const CString& csFileName)
{
   const int HTTPBUFLEN = 512; // Size of HTTP Buffer...
   char    httpbuff[HTTPBUFLEN];
   bool    bResult = false;
   TCHAR   szCause[255];

   TRY
   {
       CInternetSession mysession;
       CStdioFile *pRemotefile = mysession.OpenURL(csUrl,1,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD);

       if (pRemotefile != NULL)
       {
          CFile myfile(csFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
          int numbytes = 0;

          while (numbytes = pRemotefile->Read(httpbuff, HTTPBUFLEN))
               myfile.Write(httpbuff, numbytes);

          myfile.Close();

          pRemotefile->Close();
          delete pRemotefile;
       }
    
       mysession.Close();

       bResult = true;
   }

   CATCH_ALL(error)
   {
     error->GetErrorMessage(szCause,254,NULL);
     bResult = false;
     AfxMessageBox(szCause, MB_OK);
   }
   END_CATCH_ALL;

   return bResult;
}

