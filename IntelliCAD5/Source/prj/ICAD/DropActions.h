/*
**
**  Purpose    : DropActions.h: interface for the CDropActions class.
**  Description: 
**
*/
 

#ifndef DROPACTIONS_H
#define DROPACTIONS_H

#include "icad.h"
#import <msxml.dll> named_guids raw_interfaces_only 


class CDropActions  
{
public:
   CDropActions();
   virtual ~CDropActions();

   void XmlUrl(LPCTSTR pXmlUrl) { m_csXmlUrl = pXmlUrl; };
   bool LoadXml();
   DROPEFFECT DropIDrop(COleDataObject* pDataObject);

   bool Execute();
   bool SendCommandToCAD(const char *cs, bool bCancel);
   bool DownloadFile(const CString& csUrl, const CString& csFileName);
   bool GetLocalFile(const CString& csDataFile );

private:

   bool    GetDataFileURL(const CString& csXmlURL, const CString& csDataFile, 
                          CString& csDataFileUrl, DWORD& dwServiceType);
   bool    GetIdropParams( CString& csDataFile );
   bool    GetAttribute(CComPtr<MSXML::IXMLDOMNode>& pNode, const CString& csSubNode,
                        const CString& csAttribName, CString& csAttribValue);
   bool    GetElementText(CComPtr<MSXML::IXMLDOMNode>& pNode, const CString& csSubNode, CString& csText);


   CComPtr<MSXML::IXMLDOMDocument>    m_pXmlDoc;
   CString                            m_csXmlUrl;
   CString                            m_csBlockName;
   CString                            m_csFileName;

};


#endif 
