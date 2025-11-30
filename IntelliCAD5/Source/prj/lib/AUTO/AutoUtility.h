/* INTELLICAD\PRJ\LIB\AUTO\AUTOUTILITY.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * $Revision: 1.4 $ $Date: 2001/08/23 03:09:54 $
 *
 * Abstract
 *
 * Declaration of the CAutoUtility
 *
 */


#ifndef __AUTOUTILITY_H_
#define __AUTOUTILITY_H_

#define ICAD_E_USER_ENTERED_KEYWORD MAKE_HRESULT(SEVERITY_ERROR, 33, 32)


/////////////////////////////////////////////////////////////////////////////
// CAutoUtility
class ATL_NO_VTABLE CAutoUtility :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CAutoUtility, &CLSID_Utility>,
    public ISupportErrorInfo,
    public IDispatchImpl<IIcadUtility, &IID_IIcadUtility, &LIBID_IntelliCAD>
{
private:
    CIcadDoc *m_pDoc;

public:
    CAutoUtility()
    {
    }

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CAutoUtility)
        COM_INTERFACE_ENTRY(IIcadUtility)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
        COM_INTERFACE_ENTRY2(IDispatch, IIcadUtility)
    END_COM_MAP()

    HRESULT Init (CIcadDoc *pDoc);

public:
    // IIcadUtility
    STDMETHOD(get_Application)(IIcadApplication * * Application);
    STDMETHOD(get_Parent)(IIcadDocument * * Document);
    STDMETHOD(Alert)(BSTR message);
    STDMETHOD(AngleFromXAxis)(IIcadPoint * Point1, IIcadPoint * Point2, double * pAngle);
    STDMETHOD(AngleToReal)(BSTR Angle, AngleUnits Unit, double * pAngle);
    STDMETHOD(DistanceToReal)(BSTR Distance, DistanceUnits Unit, double * pDistance);
    STDMETHOD(RealToString)(double Value, DistanceUnits Unit, long Precision, BSTR * Distance);
    STDMETHOD(AngleToString)(double Angle, AngleUnits Unit, long Precision, BSTR * str);
    STDMETHOD(Prompt)(BSTR TextString);
    STDMETHOD(ConvertUnit)(double Value, BSTR From, BSTR To, double * ConvertedValue);
    STDMETHOD(EntitySnap)(IIcadPoint * InputPoint, ObjectSnapMode SnapMode, IIcadPoint * * ppPoint);
    STDMETHOD(FindFile)(BSTR Name, BSTR * FullPath);
    STDMETHOD(GetAngle)(IIcadPoint * Point, BSTR Prompt, double * pAngle);
    STDMETHOD(GetCorner)(IIcadPoint * Point, BSTR Prompt, IIcadPoint * * ppCorner);
    STDMETHOD(GetDistance)(IIcadPoint * Point, BSTR Prompt, double * pDistance);
    STDMETHOD(GetEntity)(IDispatch * * Object, IIcadPoint * * PickedPoint, BSTR Prompt);
    STDMETHOD(GetSubEntity)(IDispatch * * Object, IIcadPoint * * PickedPoint, IIcadMatrix * * TransMatrix, VARIANT * ContextData, BSTR Prompt);
    STDMETHOD(GetInput)(BSTR * InputString);
    STDMETHOD(GetInteger)(BSTR Prompt, long * Value);
    STDMETHOD(GetOrientation)(IIcadPoint * Point, BSTR Prompt, double * pOrientation);
    STDMETHOD(GetPoint)(IIcadPoint * Point, BSTR Prompt, IIcadPoint * * ppPoint);
    STDMETHOD(GetReal)(BSTR Prompt, double * Value);
    STDMETHOD(GetString)(VARIANT_BOOL HasSpaces, BSTR Prompt, BSTR * String);
    STDMETHOD(TranslateCoordinates)(IIcadPoint * OriginPoint, CoordinateSystemTypes From, CoordinateSystemTypes To, VARIANT_BOOL Disp, IIcadPoint * * ppPoint);
    STDMETHOD(InitializeUserInput)(UserInputType Bits, VARIANT Keywords);
    STDMETHOD(GetKeyword)(BSTR Prompt, BSTR *String);
    STDMETHOD(PolarPoint)(IIcadPoint * Point, double Angle, double Distance, IIcadPoint * * ppPoint);
    STDMETHOD(PaintLine)(IIcadPoint * StartPoint, IIcadPoint * EndPoint, Colors Color, VARIANT_BOOL HighlightFlag);
    STDMETHOD(ShowGraphicScreen)();
    STDMETHOD(ShowTextScreen)();
    STDMETHOD(WildCardMatch)(BSTR Pattern, BSTR String, VARIANT_BOOL * Match);

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
    {
        static const IID* arr[] =
        {
            &IID_IIcadUtility,
        };
        for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
        {
            if (IsEqualGUID(*arr[i], riid)) //  MDTJULY1001 - for mssdk january 2001 version
                return S_OK;
        }
        return S_FALSE;
    }

};

#endif //__AUTOUTILITY_H_


