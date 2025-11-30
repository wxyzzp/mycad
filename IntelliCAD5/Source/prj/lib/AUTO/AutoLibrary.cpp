/* INTELLICAD\PRJ\LIB\AUTO\AUTOLIBRARY.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoLibrary - the Automation object Library
 * 
 */ 

#include "StdAfx.h"
#include "AutoLibrary.h"
#include "AutoPoints.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoMatrix.h"
#include "AutoHelpers.h"
 
/////////////////////////////////////////////////////////////////////////////
// CAutoLibrary

STDMETHODIMP CAutoLibrary::get_Application (IIcadApplication * * Application)
	{
	if (Application == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (Application);
	}

STDMETHODIMP CAutoLibrary::get_Parent (IIcadApplication * * Application)
	{
	if (Application == NULL)
		return E_POINTER;
	
	return get_Application (Application);
	}

STDMETHODIMP CAutoLibrary::CreatePoint (double X1, double Y1, double Z1, IIcadPoint * * ppPoint)
	{
	if (ppPoint == NULL)
		return E_POINTER;
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(this, NULL, X1, Y1, Z1);
	
	return pPoint->QueryInterface (IID_IIcadPoint, (void **)ppPoint);
	}

STDMETHODIMP CAutoLibrary::CreatePoints (IIcadPoints * * ppPoints)
	{
	if (ppPoints == NULL)
		return E_POINTER;
	
	CComObject<CAutoPoints> *pPoints;
	CComObject<CAutoPoints>::CreateInstance(&pPoints);
	if (!pPoints)
		return E_FAIL;
	
	pPoints->Init(this);
	
	return pPoints->QueryInterface (IID_IIcadPoints, (void **)ppPoints);
	}


STDMETHODIMP CAutoLibrary::CalculateDistance (IIcadPoint * Point1, IIcadPoint * Point2, double * pDistance)
	{
	if (pDistance == NULL)
		return E_POINTER;

	if ((Point1 == NULL) || (Point2 == NULL))
		return E_INVALIDARG;
	
	double X2, Y2, Z2, X1, Y1, Z1;
	Point1->get_x (&X1);
	Point1->get_y (&Y1);
	Point1->get_z (&Z1);
	Point2->get_x (&X2);
	Point2->get_y (&Y2);
	Point2->get_z (&Z2);

	*pDistance = sqrt ((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) + (Z2-Z1)*(Z2-Z1));

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::CreateVector (double X2, double Y2, double Z2, double X1, double Y1, double Z1, IIcadVector * * ppVector)
	{
	if (ppVector == NULL)
		return E_POINTER;
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, (X2 - X1), (Y2 - Y1), (Z2 -Z1));
	
	return pVector->QueryInterface (IID_IIcadVector, (void **)ppVector);
	}

STDMETHODIMP CAutoLibrary::AddVectors (IIcadVector * Vector1, IIcadVector * Vector2, IIcadVector * * ppVector)
	{
	if (ppVector == NULL)
		return E_POINTER;
	
	double X2, Y2, Z2, X1, Y1, Z1;
	Vector1->get_x (&X1);
	Vector1->get_y (&Y1);
	Vector1->get_z (&Z1);
	Vector2->get_x (&X2);
	Vector2->get_y (&Y2);
	Vector2->get_z (&Z2);

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, (X2 + X1), (Y2 + Y1), (Z2 + Z1));
	
	return pVector->QueryInterface (IID_IIcadVector, (void **)ppVector);
	}

STDMETHODIMP CAutoLibrary::SubtractVectors (IIcadVector * Vector1, IIcadVector * Vector2, IIcadVector * * ppVector)
	{
	if (ppVector == NULL)
		return E_POINTER;
	
	double X2, Y2, Z2, X1, Y1, Z1;
	Vector1->get_x (&X1);
	Vector1->get_y (&Y1);
	Vector1->get_z (&Z1);
	Vector2->get_x (&X2);
	Vector2->get_y (&Y2);
	Vector2->get_z (&Z2);

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, (X2 - X1), (Y2 - Y1), (Z2 - Z1));
	
	return pVector->QueryInterface (IID_IIcadVector, (void **)ppVector);
	}

STDMETHODIMP CAutoLibrary::VectorDotProduct (IIcadVector * Vector1, IIcadVector * Vector2, double * pVal)
	{
	if (pVal == NULL)
		return E_POINTER;
	
	double X2, Y2, Z2, X1, Y1, Z1;
	Vector1->get_x (&X1);
	Vector1->get_y (&Y1);
	Vector1->get_z (&Z1);
	Vector2->get_x (&X2);
	Vector2->get_y (&Y2);
	Vector2->get_z (&Z2);

	*pVal = (X2 * X1) + (Y2 * Y1) + (Z2 * Z1);
	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::VectorCrossProduct (IIcadVector * Vector1, IIcadVector * Vector2, IIcadVector * * ppVector)
	{
	if (ppVector == NULL)
		return E_POINTER;
	
	double X2, Y2, Z2, X1, Y1, Z1;
	Vector1->get_x (&X1);
	Vector1->get_y (&Y1);
	Vector1->get_z (&Z1);
	Vector2->get_x (&X2);
	Vector2->get_y (&Y2);
	Vector2->get_z (&Z2);

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, (Y1*Z2 - Y2*Z1), (Z1*X2 - X1*Z2), (X1*Y2 - Y1*X2));
	
	return pVector->QueryInterface (IID_IIcadVector, (void **)ppVector);
	}

STDMETHODIMP CAutoLibrary::VectorScalarMultiply (IIcadVector * Vector, double Scale, IIcadVector * * ppVector)
	{
	if (ppVector == NULL)
		return E_POINTER;
	
	double X1, Y1, Z1;
	Vector->get_x (&X1);
	Vector->get_y (&Y1);
	Vector->get_z (&Z1);

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, X1*Scale, Y1*Scale, Z1*Scale);
	
	return pVector->QueryInterface (IID_IIcadVector, (void **)ppVector);
	}

STDMETHODIMP CAutoLibrary::VectorUnitize (IIcadVector * Vector, IIcadVector * * ppVector)
	{
	if (ppVector == NULL)
		return E_POINTER;

	double X1, Y1, Z1;
	double Length;
	VectorLength (Vector, &Length);
	if (icadRealEqual(Length,0.0))
		return E_FAIL;	//to avoid divide by 0

	Vector->get_x (&X1);
	Vector->get_y (&Y1);
	Vector->get_z (&Z1);

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, X1/Length, Y1/Length, Z1/Length);
	
	return pVector->QueryInterface (IID_IIcadVector, (void **)ppVector);
	}

STDMETHODIMP CAutoLibrary::VectorLength (IIcadVector * Vector, double * pLength)
	{
	if (pLength == NULL)
		return E_POINTER;
	
	double X1, Y1, Z1;
	Vector->get_x (&X1);
	Vector->get_y (&Y1);
	Vector->get_z (&Z1);

	*pLength = sqrt (X1*X1 + Y1*Y1 + Z1*Z1);

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::DistanceBetweenVectors (IIcadVector * Vector1, IIcadVector * Vector2, double * pVal)
	{
	if (pVal == NULL)
		return E_POINTER;
	
	sds_point p1, p2, p3;
	Vector1->get_x (&p1[0]);
	Vector1->get_y (&p1[1]);
	Vector1->get_z (&p1[2]);
	Vector2->get_x (&p2[0]);
	Vector2->get_y (&p2[1]);
	Vector2->get_z (&p2[2]);

	ic_crossproduct(p1, p2, p3);
	p3[0] = p3[0] * (p2[0] - p1[0]);
	p3[1] = p3[1] * (p2[1] - p1[1]);
	p3[2] = p3[2] * (p2[2] - p1[2]);

	*pVal = sqrt (p3[0]*p3[0] + p3[1]*p3[1] + p3[2]*p3[2]);

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::CreateIdentityMatrix (IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;
	
	CComObject<CAutoMatrix> *pMatrix;
	CComObject<CAutoMatrix>::CreateInstance(&pMatrix);
	if (!pMatrix)
		return E_FAIL;
	
	CComPtr<IIcadLibrary> pLib;
	QueryInterface(IID_IIcadLibrary, (void **)&pLib);
	pMatrix->Init(pLib);
	
	short row,column;
	for (row = 0; row < 4; row++)
		{
		for (column = 0; column < 4; column++)
			{
			pMatrix->SetValue (row, column, 0.0);
			}
		}
	pMatrix->SetValue (0, 0, 1.0);
	pMatrix->SetValue (1, 1, 1.0);
	pMatrix->SetValue (2, 2, 1.0);
	pMatrix->SetValue (3, 3, 1.0);
	pMatrix->SetValue (3, 0, 0.0);
	pMatrix->SetValue (3, 1, 0.0);
	pMatrix->SetValue (3, 2, 0.0);
	
	return pMatrix->QueryInterface (ppMatrix);
	}

STDMETHODIMP CAutoLibrary::CreateRotationMatrix (double Angle, IIcadPoint * AxisEnd, IIcadPoint * AxisStart, IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;

	if (AxisEnd == NULL)
		return E_INVALIDARG;
	

	sds_point pt1, pt2;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	if (AxisStart)		//this is an optional argument
		{
		AxisStart->get_x (&pt1[0]);
		AxisStart->get_y (&pt1[1]);
		AxisStart->get_z (&pt1[2]);
		}
	AxisEnd->get_x (&pt2[0]);
	AxisEnd->get_y (&pt2[1]);
	AxisEnd->get_z (&pt2[2]);

	sds_point extrusionvec;
	ic_sub (pt1, pt2, extrusionvec);
	if (db_unitizevector(extrusionvec) != 0)
		return E_INVALIDARG;

	sds_matrix matrix;
	ic_rotxform (pt1, extrusionvec, Angle, matrix);

	CComObject<CAutoMatrix> *pMatrix;
	CComObject<CAutoMatrix>::CreateInstance(&pMatrix);
	if (!pMatrix)
		return E_FAIL;
	
	pMatrix->Init(this);
	if (FAILED( pMatrix->QueryInterface (ppMatrix)))
		return E_FAIL;

	ConvertSdsMatrixToIIcadMatrix (matrix, *ppMatrix);

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::CreateTranslationMatrix (double x, double y, double z, IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;
	
	CComPtr<IIcadMatrix> pMatrix;
	if (FAILED (CreateIdentityMatrix (&pMatrix)))
		return E_FAIL;

	pMatrix->SetValue (0, 3, x);
	pMatrix->SetValue (1, 3, y);
	pMatrix->SetValue (2, 3, z);

	return pMatrix->QueryInterface (ppMatrix);
	}

STDMETHODIMP CAutoLibrary::CreateScaleMatrix (double x, double y, double z, IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;

	if ((x <= 0.0) || (y <= 0.0) || (z <= 0.0))
		return E_INVALIDARG;
	
	CComPtr<IIcadMatrix> pMatrix;
	if (FAILED (CreateIdentityMatrix (&pMatrix)))
		return E_FAIL;

	pMatrix->SetValue (0, 0, x);
	pMatrix->SetValue (1, 1, y);
	pMatrix->SetValue (2, 2, z);

	return pMatrix->QueryInterface (ppMatrix);
	}

STDMETHODIMP CAutoLibrary::CreateMirrorMatrix (VARIANT_BOOL x, VARIANT_BOOL y, VARIANT_BOOL z, IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;
	
	CComPtr<IIcadMatrix> pMatrix;
	if (FAILED (CreateIdentityMatrix (&pMatrix)))
		return E_FAIL;

	if (x == VARIANT_TRUE)
		pMatrix->SetValue (0, 0, -1.0);
	if (y == VARIANT_TRUE)
		pMatrix->SetValue (1, 1, -1.0);
	if (z == VARIANT_TRUE)
		pMatrix->SetValue (2, 2, -1.0);

	return pMatrix->QueryInterface (ppMatrix);
	}

STDMETHODIMP CAutoLibrary::ComposeMatrix (IIcadMatrix * Matrix1, IIcadMatrix * Matrix2, IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;
	
	CComPtr<IIcadMatrix> pMatrix;
	if (FAILED (CreateIdentityMatrix (&pMatrix)))
		return E_FAIL;
	
	double total, a, b;
	short i,row,column;
	for (row = 0; row < 4; row++)
		{
		for (column = 0; column < 4; column++)
			{
			total = 0.0;
			for (i = 0; i < 4; i++)
				{
				Matrix1->GetValue (row, i, &a);
				Matrix2->GetValue (i, column, &b);
				total += a * b;
				}
			pMatrix->SetValue (row, column, total);
			}
		}
	pMatrix->SetValue (3, 0, 0.0);
	pMatrix->SetValue (3, 1, 0.0);
	pMatrix->SetValue (3, 2, 0.0);
	pMatrix->SetValue (3, 3, 1.0);

	return pMatrix->QueryInterface (ppMatrix);
	}

STDMETHODIMP CAutoLibrary::ApplyMatrix (IIcadMatrix * Matrix, IIcadPoint * Point, IIcadPoint * * ppPoint)
	{
	if (ppPoint == NULL)
		return E_POINTER;
		
	double a, b, c, d;
	double X2, Y2, Z2, X1, Y1, Z1;
	
	Point->get_x (&X1);
	Point->get_y (&Y1);
	Point->get_z (&Z1);

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	Matrix->GetValue(0, 0, &a);
	Matrix->GetValue(0, 1, &b);
	Matrix->GetValue(0, 2, &c);
	Matrix->GetValue(0, 3, &d);
	X2 = X1*a + Y1*b + Z1*c + d;

	Matrix->GetValue(1, 0, &a);
	Matrix->GetValue(1, 1, &b);
	Matrix->GetValue(1, 2, &c);
	Matrix->GetValue(1, 3, &d);
	Y2 = X1*a + Y1*b + Z1*c + d;

	Matrix->GetValue(2, 0, &a);
	Matrix->GetValue(2, 1, &b);
	Matrix->GetValue(2, 2, &c);
	Matrix->GetValue(2, 3, &d);
	Z2 = X1*a + Y1*b + Z1*c + d;

	pPoint->Init(this, NULL, X2, Y2, Z2);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoLibrary::InvertMatrix (IIcadMatrix * Matrix, IIcadMatrix * * ppMatrix)
	{
	if (ppMatrix == NULL)
		return E_POINTER;

	if (Matrix == NULL)
		return E_INVALIDARG;

	sds_matrix mtx;
	ConvertIIcadMatrixToSdsMatrix (mtx, Matrix);

	bool invertible = false;
	sds_matrix inv;
	ic_invertmatrix (mtx, invertible, inv);
	
	if (!invertible)
		return E_FAIL;

	CComObject<CAutoMatrix> *pMatrix;
	CComObject<CAutoMatrix>::CreateInstance(&pMatrix);
	if (!pMatrix)
		return E_FAIL;
	
	pMatrix->Init(this);
	if (FAILED( pMatrix->QueryInterface (ppMatrix)))
		return E_FAIL;

	ConvertSdsMatrixToIIcadMatrix (inv, *ppMatrix);

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::ConvertArrayToPoint (VARIANT ArrayOfDoubles, VARIANT_BOOL Is3DPoint, IIcadPoint * * Point)
	{
	if (Point == NULL)
		return E_POINTER;
	
 	//get the array
	SAFEARRAY FAR* array;
	if (ArrayOfDoubles.vt == (VT_ARRAY | VT_R8 |VT_BYREF))
		array = *(ArrayOfDoubles.pparray);
	else if (ArrayOfDoubles.vt == (VT_ARRAY | VT_R8))
		array = ArrayOfDoubles.parray;
	else
		return E_INVALIDARG;

	BOOL bIs3D = (Is3DPoint == VARIANT_TRUE) ? TRUE : FALSE;

	long LBound, UBound;
	HRESULT hr;
	hr = SafeArrayGetLBound(array, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(array, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	if (bIs3D && (UBound - LBound) != 2)
		return E_INVALIDARG;

	if (!bIs3D && (UBound - LBound) != 1)
		return E_INVALIDARG;

	long i, j;
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	for (i = LBound, j = 0; i <= UBound; i++, j++)
		{
		if (j > 2) 
			break;
		if ((j == 2) && !bIs3D)
			break;

		if (FAILED(SafeArrayGetElement(array, &i, &pt[j])))
			continue;
		}

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(this, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (Point);
	}

STDMETHODIMP CAutoLibrary::ConvertArrayToPoints (VARIANT ArrayOfDoubles, VARIANT_BOOL Is3DPoint, IIcadPoints * * Points)
	{
	if (Points == NULL)
		return E_POINTER;
	
 	//get the array
	SAFEARRAY FAR* array;
	if (ArrayOfDoubles.vt == (VT_ARRAY | VT_R8 |VT_BYREF))
		array = *(ArrayOfDoubles.pparray);
	else if (ArrayOfDoubles.vt == (VT_ARRAY | VT_R8))
		array = ArrayOfDoubles.parray;
	else
		return E_INVALIDARG;

	BOOL bIs3D = (Is3DPoint == VARIANT_TRUE) ? TRUE : FALSE;

	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(array, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(array, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	if (bIs3D && ((UBound - LBound + 1)%3))
		return E_INVALIDARG;

	if (!bIs3D && ((UBound - LBound + 1)%2))
		return E_INVALIDARG;

	CComObject<CAutoPoints> *pPoints;
	CComObject<CAutoPoints>::CreateInstance(&pPoints);
	if (!pPoints)
		return E_FAIL;
	
	pPoints->Init(this);
	
	long i, j;
	j = 0;
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	for (i = LBound; i <= UBound; i++)
		{
		if (j > 2) 
			return E_INVALIDARG;
		if ((j == 2) && !bIs3D)
			return E_INVALIDARG;

		hr = SafeArrayGetElement(array, &i, &pt[j]);
		if (FAILED(hr))
			continue;

		if ( ((j == 2) && bIs3D) || ((j == 1) && !bIs3D) )
			{
			CComPtr<IIcadPoint> pIIcadPoint = NULL;
			pPoints->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
			if (pIIcadPoint == NULL)
				return E_OUTOFMEMORY;

			pIIcadPoint = NULL;
			j = -1;
			pt[0] = pt[1] = pt[2] = 0.0;
			}
		j++;
		}

	return pPoints->QueryInterface (Points);
	}

STDMETHODIMP CAutoLibrary::ConvertArrayToVector (VARIANT ArrayOfDoubles, VARIANT_BOOL Is3DVector, IIcadVector * * Vector)
	{
	if (Vector == NULL)
		return E_POINTER;
	
 	//get the array
	SAFEARRAY FAR* array;
	if (ArrayOfDoubles.vt == (VT_ARRAY | VT_R8 |VT_BYREF))
		array = *(ArrayOfDoubles.pparray);
	else if (ArrayOfDoubles.vt == (VT_ARRAY | VT_R8))
		array = ArrayOfDoubles.parray;
	else
		return E_INVALIDARG;

	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(array, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(array, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;

	BOOL bIs3D = (Is3DVector == VARIANT_TRUE) ? TRUE : FALSE;

	if (bIs3D && (UBound - LBound) != 2)
		return E_INVALIDARG;

	if (!bIs3D && (UBound - LBound) != 1)
		return E_INVALIDARG;

	long i, j;
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	for (i = LBound, j = 0; i <= UBound; i++, j++)
		{
		if (j > 2) 
			break;
		if ((j == 2) && !bIs3D)
			break;

		hr = SafeArrayGetElement(array, &i, &pt[j]);
		if (FAILED(hr))
			continue;
		}

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(this, pt[0], pt[1], pt[2]);
	
	return pVector->QueryInterface (Vector);
	}

STDMETHODIMP CAutoLibrary::ConvertArrayToMatrix (VARIANT ArrayOf16Doubles, IIcadMatrix * * Matrix)
	{
	if (Matrix == NULL)
		return E_POINTER;

	//get the array
	SAFEARRAY FAR* array;
	if (ArrayOf16Doubles.vt == (VT_ARRAY | VT_R8 |VT_BYREF))
		array = *(ArrayOf16Doubles.pparray);
	else if (ArrayOf16Doubles.vt == (VT_ARRAY | VT_R8))
		array = ArrayOf16Doubles.parray;
	else
		return E_INVALIDARG;
	
	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(array, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(array, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;

	if ((UBound - LBound) != 15)
		return E_INVALIDARG;

	long i, j;
	double values[16];
	for (i = LBound, j = 0; i <= UBound; i++, j++)
		{
		hr = SafeArrayGetElement(array, &i, &values[j]);
		if (FAILED(hr))
			continue;
		}

	CComObject<CAutoMatrix> *pMatrix;
	CComObject<CAutoMatrix>::CreateInstance(&pMatrix);
	if (!pMatrix)
		return E_FAIL;
	
	pMatrix->Init(this);
	
	short row,column;
	j = 0;
	for (row = 0; row < 4; row++)
		{
		for (column = 0; column < 4; column++)
			{
			pMatrix->SetValue (row, column, values[j]);
			j++;
			}
		}
	
	return pMatrix->QueryInterface (Matrix);
	}

STDMETHODIMP CAutoLibrary::ConvertPointToArray (IIcadPoint * Point, VARIANT_BOOL Is3DPoint, VARIANT * ArrayOfDoubles)
	{
	if (ArrayOfDoubles == NULL)
		return E_POINTER;
	
	if (Point == NULL)
		return E_INVALIDARG;

	sds_point pt;
	Point->get_x (&pt[0]);
	Point->get_y (&pt[1]);
	Point->get_z (&pt[2]);

	long nCount = (Is3DPoint == VARIANT_TRUE) ? 3 : 2;

	SAFEARRAY FAR* Values;
	SAFEARRAYBOUND Bound[1];
	Bound[0].lLbound = 0;
	Bound[0].cElements = nCount;
	Values = SafeArrayCreate(VT_R8, 1, Bound);

	long i;
	for (i = 0; i < nCount; i++)
		{
		SafeArrayPutElement(Values, &i, &pt[i]);
		}

	(*ArrayOfDoubles).vt = VT_ARRAY | VT_R8;
	(*ArrayOfDoubles).parray = Values;

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::ConvertPointsToArray (IIcadPoints * Points, VARIANT_BOOL Is3DPoint, VARIANT * ArrayOfDoubles)
	{
	if (ArrayOfDoubles == NULL)
		return E_POINTER;
	
	if (Points == NULL)
		return E_INVALIDARG;

	long nCount, nElements;
	Points->get_Count (&nCount);

	nElements = (Is3DPoint == VARIANT_TRUE) ? 3 * nCount : 2 * nCount;

	SAFEARRAY FAR* Values;
	SAFEARRAYBOUND Bound[1];
	Bound[0].lLbound = 0;
	Bound[0].cElements = nElements;
	Values = SafeArrayCreate(VT_R8, 1, Bound);

	long i, j = 0;
	sds_point pt;
	HRESULT hr;
	CComPtr<IIcadPoint> pIIcadPoint = NULL;

	for (i = 0; i < nCount; i++)
		{
		pt[0] = pt[1] = pt[2] = 0.0;
		hr = Points->Item(i+1, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		
		SafeArrayPutElement(Values, &j, &pt[0]);
		j++;
		SafeArrayPutElement(Values, &j, &pt[1]);
		j++;
		if (Is3DPoint == VARIANT_TRUE)
			{
			SafeArrayPutElement(Values, &j, &pt[2]);
			j++;
			}
		}

	ASSERT (j == nElements);

	(*ArrayOfDoubles).vt = VT_ARRAY | VT_R8;
	(*ArrayOfDoubles).parray = Values;

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::ConvertVectorToArray (IIcadVector * Vector, VARIANT_BOOL Is3DVector, VARIANT * ArrayOfDoubles)
	{
	if (ArrayOfDoubles == NULL)
		return E_POINTER;
	
	if (Vector == NULL)
		return E_INVALIDARG;

	sds_point pt;
	Vector->get_x (&pt[0]);
	Vector->get_y (&pt[1]);
	Vector->get_z (&pt[2]);

	long nCount = (Is3DVector == VARIANT_TRUE) ? 3 : 2;

	SAFEARRAY FAR* Values;
	SAFEARRAYBOUND Bound[1];
	Bound[0].lLbound = 0;
	Bound[0].cElements = nCount;
	Values = SafeArrayCreate(VT_R8, 1, Bound);

	long i;
	for (i = 0; i < nCount; i++)
		{
		SafeArrayPutElement(Values, &i, &pt[i]);
		}

	(*ArrayOfDoubles).vt = VT_ARRAY | VT_R8;
	(*ArrayOfDoubles).parray = Values;

	return NOERROR;
	}

STDMETHODIMP CAutoLibrary::ConvertMatrixToArray (IIcadMatrix * Matrix, VARIANT * ArrayOf16Doubles)
	{
	if (ArrayOf16Doubles == NULL)
		return E_POINTER;
	
	if (Matrix == NULL)
		return E_INVALIDARG;

	double matvals[16];
	short j,row,column;
	j = 0;
	for (row = 0; row < 4; row++)
		for (column = 0; column < 4; column++)
			{
			Matrix->GetValue (row, column, &matvals[j]);
			j++;
			}

	SAFEARRAY FAR* Values;
	SAFEARRAYBOUND Bound[1];
	Bound[0].lLbound = 0;
	Bound[0].cElements = 16;
	Values = SafeArrayCreate(VT_R8, 1, Bound);

	long i;
	for (i = 0; i < 16; i++)
		{
		SafeArrayPutElement(Values, &i, &matvals[i]);
		}

	(*ArrayOf16Doubles).vt = VT_ARRAY | VT_R8;
	(*ArrayOf16Doubles).parray = Values;

	return NOERROR;
	}



