/* LIB\CMDS\TABLET.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * The Tablet command - for digitizer support.
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
//#define	__AFXTEMPL_H__			// suppress AFX templates
#include "Icad.h" /*DNT*/
#include <assert.h>

#include "IcadApi.h"/*DNT*/
#include "Icadtablet.h"/*DNT*/

// the following include statements are needed to support GEO
#define THREE_D
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Trnsform.h"/*DNT*/
#include "Projective.h"/*DNT*/
#include "LsqTrans.h"/*DNT*/
#include "LinSolver.h"/*DNT*/ // may not need this
#include "GeoConvert.h"/*DNT*/
// the preceding include statements are needed to support GEO

class CTablet
	{
	public:
		CTablet () { }; 
		~CTablet( void ) { };

	public:
		short	SetTabletOn ();
		short	SetTabletOff ();
		short	ToggleTablet ();
		short	cmd_calibrate ();
		short	cmd_configure ();

	};


short CTablet::SetTabletOn()
	{

	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		return RTERROR;

	if (SDS_CMainWindow->m_pTablet->IsCalibrated())
		{
		sds_matrix cal;
		SDS_CMainWindow->m_pTablet->GetCalibration (cal);
		SDS_CMainWindow->m_pTablet->SetCalibration (cal, XFORM_PROJECTIVE);
		SDS_CMainWindow->m_pTablet->EnableCal(TRUE);
		SDS_CMainWindow->m_pTablet->EnableCfg(FALSE);
		}
	else
		{
		struct resbuf rb;
		if ( (SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) || (rb.resval.rint==0) )
			{
			sds_printf("%s"/*DNT*/,ResourceString(IDC_CMDTABLET_CAL_19, "Please calibrate the Tablet before turning it on."));
			}
		else
			{
			MessageBox(IcadSharedGlobals::GetMainHWND(), 
					   ResourceString(IDC_CMDTABLET_CAL_19, "Please calibrate the Tablet before turning it on."),
					   ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OK|MB_ICONEXCLAMATION);
			}
		return RTNORM;
		}

	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint=1;
	sds_setvar("TABMODE"/*DNT*/,&rb);
	SDS_UpdateSetvar=(char *)"TABMODE"/*DNT*/;
	ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );

	return RTNORM;
	}

short CTablet::SetTabletOff()
	{
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		return RTERROR;

	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint=0;
	sds_setvar("TABMODE"/*DNT*/,&rb);
	SDS_UpdateSetvar=(char *)"TABMODE"/*DNT*/;
	ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );

	//ensure the calibrated contexts are turned off
	SDS_CMainWindow->m_pTablet->EnableCal(FALSE);
	if (SDS_CMainWindow->m_pTablet->IsConfigured())
		{
		//if it was previously configured, turn it on, else leave it off, 
		//and the default Wintab context takes over, and puts it in mouse mode.
		SDS_CMainWindow->m_pTablet->EnableCfg(TRUE);
		}
	return RTNORM;
	}

short CTablet::ToggleTablet()
	{
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		return RTERROR;

	struct resbuf rb;
	SDS_getvar(NULL,DB_QTABMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint)
		SetTabletOff();
	else
		SetTabletOn();
	return RTNORM;
	}




// This function converts an sds_point to a C3Point
void sds_point_to_C3Point(const sds_point & A,
						  C3Point *B)
{
	B->SetX(A[0]);
	B->SetY(A[1]);
	B->SetZ(A[2]);
}



// This function converts a CProjective object to an sds_matrix
// The CProjective object is a projective mapping from the plane to itself
void CProjective_to_sds_matrix(const CProjective & A,
							   sds_matrix B)
{
	// In the current implementation of CProjective, the defining data
	// is not hidden, and, in fact, the 3 x 3 matrix that represents the
	// CProjective transformation can be imbedded in the 4 x 4 sds_matrix
	// as follows:

	// If the CProjective matrix is as follows
	// 
	// | a b c |
	// | d e f |
	// | g h m |
    //
	// Then the sds_matrix will be as follows
	//
	// | a b 0 c |
	// | d e 0 f |
	// | 0 0 1 0 |
	// | g h 0 m |

	int i, j;
	// map the upper right hand 2 x 2 corner directly
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			B[i][j] = A.m_Matrix[i][j];
		}
	}

	// Now fill in the remaining values
    B[0][3] = A.m_Matrix[0][2];
	B[1][3] = A.m_Matrix[1][2];

	B[2][0] = 0.0;
	B[2][1] = 0.0;
	B[2][2] = 1.0;
	B[2][3] = 0.0;

	B[3][0] = A.m_Matrix[2][0];
	B[3][1] = A.m_Matrix[2][1];
	B[3][2] = 0.0;
	B[3][3] = A.m_Matrix[2][2];




}


// The preceding functions are used to support the calibration process

// The following function just copies one sds_matrix to another
void copy(sds_matrix a, // Input: The matrix to be copied
		  sds_matrix b)   // Output:  b has the same entries as a
{
	int i,j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			b[i][j] = a[i][j];
		}
	}
}


void calculate_CalibrationXForm 
(
 int n, 
	sds_point *ptDig, 
	sds_point *ptWorld, 
	sds_matrix OrthoTrans, double *Ortho_RMSerror, 
	sds_matrix AffineTrans, double *Affine_RMSerror,
	sds_matrix ProjTrans, double *Proj_RMSerror,
	int *BestFit  // 1, 2 or 3 indicating which of the three is the best one that you suggest
	// If there has been a failure, and no transformation has been 
	// constructed, we will return *BestFit = 0.
	)
{
	int i;
	
	// convert the input points into GEO classes so they can be used by TransNPt
	C3Point * P = new C3Point[n];
	C3Point * Q = new C3Point[n];
	
	for (i = 0; i < n; i++)
	{   // begin i-loop
		sds_point_to_C3Point(ptDig[i], &P[i]);
		sds_point_to_C3Point(ptWorld[i], &Q[i]);
	}   //   end i-loop
	

    CAffine T;
	CProjective S;
	RC rc, rcOrthogonal, rcAffine;

	switch (n)
	{   //  begin switch (n)
	case 2:
		rc = Orthogonal2Pt(P, Q, T);
		if (rc == SUCCESS)
		{
			CAffine_to_sds_matrix(T, OrthoTrans);
			*Ortho_RMSerror = 0.0;

			// In this case, the best affine and best projective transformations
			// are identical to the best orthogonal transformation; no fitting
			// has to be done in any case.

			copy(OrthoTrans, AffineTrans);
            *Affine_RMSerror = 0.0;

			copy(OrthoTrans, ProjTrans);
			*Proj_RMSerror = 0.0;

			// Since it is an exact orthogonal fit in this case, all errors are zero.




			*BestFit = 1; // We recommend that you choose orthogonal
		}
		else
		{
			*BestFit = 0; // unsuccessful attempt to build orthogonal
			// Let's set all errors to -1.0 to indicate this
			*Ortho_RMSerror = -1.0;
			*Affine_RMSerror = -1.0;
			*Proj_RMSerror = -1.0;
		}
		break;

	case 3:
		rc = Affine3Pt(P, Q, T);
		if (rc == SUCCESS)
		{
			CAffine_to_sds_matrix(T, AffineTrans);
			*Affine_RMSerror = 0.0;

			// In this case the best projective transformation
			// is equal to the best affine transformation; no fitting
			// has to be done in that case
			copy(AffineTrans, ProjTrans);
			*Proj_RMSerror = 0.0;

			// We now compute the best orthogonal transformation
			OrthogonalNPt(n, P, Q, T, Ortho_RMSerror, rcOrthogonal);

			if (rcOrthogonal == SUCCESS)
			{
				CAffine_to_sds_matrix(T, OrthoTrans);
			}



			*BestFit = 2; // We recommend that you choose affine
		}
		else
		{
			*BestFit = 0; // unsuccessful attempt to build affine
			// Let's set all errors to -1.0 to indicate this
			*Ortho_RMSerror = -1.0;
			*Affine_RMSerror = -1.0;
			*Proj_RMSerror = -1.0;
		}
		break;

	case 4:
		Projective4Pt(P, Q, S, rc);
        if (rc == SUCCESS)
		{
			CProjective_to_sds_matrix(S, ProjTrans);

			// We now compute the best orthogonal and affine transformations

			OrthogonalNPt(n, P, Q, T, Ortho_RMSerror, rcOrthogonal);

			if (rcOrthogonal == SUCCESS)
			{
				CAffine_to_sds_matrix(T, OrthoTrans);
			}

            AffineNPt(n, P, Q, T, Affine_RMSerror, rcAffine);

			if (rcAffine == SUCCESS)
			{
				CAffine_to_sds_matrix(T, AffineTrans);
			}


			*BestFit = 3; // We recommend that you choose projective
		}
		else
		{
			*BestFit = 0; // unsuccessful attempt to build projective
			// Let's set all errors to -1.0 to indicate this
			*Ortho_RMSerror = -1.0;
			*Affine_RMSerror = -1.0;
			*Proj_RMSerror = -1.0;
		}
		break;


	default:

		// We will compute the best orthogonal and best affine transformations
		// Best projective is not yet supported.
			OrthogonalNPt(n, P, Q, T, Ortho_RMSerror, rcOrthogonal);

			if (rcOrthogonal == SUCCESS)
			{
				CAffine_to_sds_matrix(T, OrthoTrans);
			}

            AffineNPt(n, P, Q, T, Affine_RMSerror, rcAffine);

			if (rcAffine == SUCCESS)
			{
				CAffine_to_sds_matrix(T, AffineTrans);
			}

			// TO DO: - ProjectiveNPt
			// for now, lets do the following
			*Proj_RMSerror = -1;

			if ((rcOrthogonal == SUCCESS) && (rcAffine == SUCCESS))
			{
				if (Affine_RMSerror < Ortho_RMSerror) // which it should be
					*BestFit = 2;
				else // this case is unlikely
					*BestFit = 1;
			}
			else
			{
				*BestFit = 0;
			    // Let's set all errors to -1.0 to indicate this
			    *Ortho_RMSerror = -1.0;
			    *Affine_RMSerror = -1.0;
			    *Proj_RMSerror = -1.0;
			}


		break;


	}   //    end switch (n)


	
	// clean up temporary memory
	delete [] P;
	delete [] Q;
}


short sds_gettabletinput (const char* Prompt, sds_point point)
	{
	SDS_CMainWindow->m_pTablet->m_RawInputEvent.ResetEvent();

	SDS_CMainWindow->m_pTablet->EnableRaw(TRUE);

	sds_printf("%s"/*DNT*/,Prompt);

	DWORD dwResult = WaitForSingleObject(SDS_CMainWindow->m_pTablet->m_RawInputEvent, INFINITE);

	short ret;
	if ( dwResult != WAIT_OBJECT_0 || SDS_CMainWindow->m_pTablet->m_bCancelled )
		ret = RTCAN;
	else if (SDS_CMainWindow->m_pTablet->m_bExit)
		ret = RTNONE;
	else
		{
		sds_point cpoint;
		SDS_CMainWindow->m_pTablet->m_pWinTabRaw->GetTabletPosition (cpoint);
		point[0] = cpoint[0];
		point[1] = cpoint[1];
		point[2] = 0;
		ret = RTNORM;
		}
	
	SDS_CMainWindow->m_pTablet->EnableRaw(FALSE);

	//sds_printf("%s"/*DNT*/," "/*DNT*/);

	return ret;
	}


short CTablet::cmd_calibrate()
	{
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		return RTERROR;

#define MAXPOINT 10
	int i;
	RT	ret;
	sds_point ptDig[MAXPOINT], ptWorld[MAXPOINT];
	sds_matrix OTrans, ATrans, PTrans;
	double Oerr = 0.0, Aerr = 0.0, Perr = 0.0;
	int BestFit = 0;
	char Prompt[IC_ACADBUF], fs1[IC_ACADBUF];

	// Prepare some of the strings that we will need after calibration is done

	const char * strResults = ResourceString(IDC_CMDTABLET_CAL_20,"Results:");
	const char * strRecommended = ResourceString(IDC_CMDTABLET_CAL_21,"Recommended");
	const char * strFeasible = ResourceString(IDC_CMDTABLET_CAL_22,"Feasible");
	const char * strNot_Applicable = ResourceString(IDC_CMDTABLET_CAL_23,"Not Applicable");
	char strOrthogonal[20];
	char strAffine[20];
    char strProjective[20];

	sds_initget(SDS_RSG_NOLIM,NULL);
    for(i=0;i<MAXPOINT;i++) 
		{
		do  {
			if (i<2)	sprintf (Prompt, ResourceString(IDC_CMDTABLET_CAL_1,"\nDigitize point #%d :"), i+1);
			else		sprintf (Prompt, ResourceString(IDC_CMDTABLET_CAL_2,"\nDigitize point #%d (or ENTER to end) :"), i+1);

			ret = sds_gettabletinput(Prompt, ptDig[i]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			if (ret==RTNONE && i>=2)
				{
				ret = RTNORM;
				Prompt[0] = '\0';
				sds_getstring(0,Prompt,fs1);  //swallow the ENTER key.
				goto calibrate;
				}
			} while (ret != RTNORM);

		do  {
			sprintf (Prompt, ResourceString(IDC_CMDTABLET_CAL_3,"\nEnter coordinates for point #%d :"), i+1);
			ret = sds_getpoint(NULL, Prompt, ptWorld[i]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			} while (ret != RTNORM);
		}

calibrate:
	calculate_CalibrationXForm (i, ptDig, ptWorld, OTrans, &Oerr, ATrans, &Aerr, PTrans, &Perr, &BestFit);

	ret = RTNORM;



	switch (BestFit)
		{
		case 0:
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_4,"\nNo valid transformation was found for this input."));
			ret = RTERROR;
			goto out;
		case 1:
			strcpy (fs1, "Orthogonal");
			break;
		case 2:
			if (Oerr >= 0.0)
				strcpy(strOrthogonal,strFeasible);
			else
				strcpy(strOrthogonal,strNot_Applicable);

			strcpy(strAffine,strRecommended);

			sds_textscr();
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_8,"\n\nTransformation Type : \t\tOrthogonal \tAffine"));
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_6,"\n--------------------------------------------------------------------"));
			sds_printf ("\n%s\t\t\t\t\t\t%s\t%s\n",strResults,strOrthogonal,strAffine);
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_9,"\nRMS Error           : \t\t%f\t\t%f"), Oerr, Aerr);

			sds_initget(0,ResourceString(IDC_CMDTABLET_CAL_12,"Orthogonal Affine ~_Orthogonal ~_Affine"));
			ret = sds_getkword(ResourceString(IDC_CMDTABLET_CAL_13,"\nSelect transformation type  Orthogonal/Affine :"),fs1 );
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			break;
		case 3:
		default:
            if (Oerr >= 0.0)
				strcpy(strOrthogonal,strFeasible);
			else
				strcpy(strOrthogonal,strNot_Applicable);

			if (Aerr >= 0.0)
				strcpy(strAffine,strFeasible);
			else
				strcpy(strAffine,strNot_Applicable);

			strcpy(strProjective,strRecommended);

			sds_textscr();
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_10,"\nTransformation Type : \t\tOrthogonal \tAffine \tProjective"));
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_6,"\n--------------------------------------------------------------------"));
			sds_printf ("\n%s\t\t\t\t\t\t%s\t%s\t%s\n",strResults,strOrthogonal,strAffine,strProjective);
			sds_printf (ResourceString(IDC_CMDTABLET_CAL_11,"\nRMS Error           : \t\t%f\t\t%f\t%f"), Oerr, Aerr, Perr);
			sds_initget(SDS_RSG_NONULL,ResourceString(IDC_CMDTABLET_CAL_14,"Orthogonal Affine Projective ~_Orthogonal ~_Affine ~_Projective"));
			ret = sds_getkword(ResourceString(IDC_CMDTABLET_CAL_15,"\n\nSelect transformation type  Orthogonal/Affine/Projective :"),fs1 );
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			break;
		}

out:
	if ((ret == RTERROR) || (ret == RTCAN))
		return ret;

	if (BestFit >= 1)
		{
		if (strisame(fs1,"ORTHOGONAL"/*DNT*/)) 
			{
			SDS_CMainWindow->m_pTablet->SetCalibration(OTrans, XFORM_ORTHOGONAL);
			}
		else if (strisame(fs1,"AFFINE"/*DNT*/)) 
			{
			SDS_CMainWindow->m_pTablet->SetCalibration(ATrans, XFORM_AFFINE);
			}
		else if (strisame(fs1,"PROJECTIVE"/*DNT*/)) 
			{
			SDS_CMainWindow->m_pTablet->SetCalibration(PTrans, XFORM_PROJECTIVE);
			}
		}


	return RTNORM;
	}


short CTablet::cmd_configure()
	{
	ASSERT (SDS_CMainWindow->m_pTablet);
	if (!SDS_CMainWindow->m_pTablet)
		return RTERROR;

	RT	ret;
	sds_point ptDig[3], ptScreen[2], ptWorld[3];
    char fs1[IC_ACADBUF];

top:
	if(sds_initget(0,ResourceString(IDC_CMDTABLET_YESNO,"Yes No ~_Yes ~_No"))!=RTNORM) 
		{
		return(RTERROR);
		}
	ret = sds_getkword(ResourceString(IDC_CMDTABLET_CFG_1,"\nDo you want to align the tablet overlay? Yes/No/<N> :"),fs1 );
	if ((ret == RTERROR) || (ret == RTCAN))
		goto out;
	if (ret == RTNONE)
		strcpy (fs1, "NO"/*DNT*/);

	if (strisame(fs1,"YES"/*DNT*/)) 
		{
		sds_initget(SDS_RSG_NOLIM,NULL);
		do 	{
			//ret = sds_getpoint(NULL,ResourceString(IDC_CMDTABLET_CFG_2,"\nDigitize upper left corner of the overlay :"),ptDig1);
			ret = sds_gettabletinput(ResourceString(IDC_CMDTABLET_CFG_2,"\nDigitize upper left corner of the overlay :"),ptDig[0]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			} while (ret != RTNORM);

		do 	{
			ret = sds_gettabletinput(ResourceString(IDC_CMDTABLET_CFG_3,"\nDigitize lower left corner of the overlay :"), ptDig[1]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			} while (ret != RTNORM);
		
		do 	{
			ret = sds_gettabletinput(ResourceString(IDC_CMDTABLET_CFG_4,"\nDigitize lower right corner of the overlay :"),ptDig[2]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			} while (ret != RTNORM);
		
/*		For now we do not allow variable size grids.  At a later point, we may want to turn these prompts on to allow
		the user to input #of rows and cols.  AM.  8/25/98
		int nCol, nRow;
		do 	{
			ret = sds_getint(ResourceString(IDC_CMDTABLET_CFG_5,"\nEnter the number of columns in the overlay <26>:"), &nCol);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			if (ret == RTNONE)
				{
				nCol = 26;
				break;
				}
			} while (ret != RTNORM);

		do 	{
			ret = sds_getint(ResourceString(IDC_CMDTABLET_CFG_6,"\nEnter the number of rows in the overlay <26>:"), &nRow);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			if (ret == RTNONE)
				{
				nRow = 26;
				break;
				}
			}while (ret != RTNORM);
		
		do	{
			sds_initget(0,ResourceString(IDC_CMDTABLET_YESNO,"Yes No ~_Yes ~_No"));
			ret = sds_getkword(ResourceString(IDC_CMDTABLET_CFG_7,"\nDo you want to specify the Screen Pointing area? Yes/No/<N> :"),fs1 );
			if ((ret == RTERROR) || (ret == RTCAN))
				break;
			if (ret == RTNONE)
				{
				strcpy (fs1, "No"*DNT*);
				goto out;
				}
			} while (ret != RTNORM);

		if (strisame(fs1,"YES"*DNT*)) 
			{
		sds_initget(SDS_RSG_NOLIM,NULL);
*/
		do {
			ret = sds_gettabletinput(ResourceString(IDC_CMDTABLET_CFG_8,"\nDigitize lower left corner of the Screen Pointing Area :"), ptScreen[0]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			} while (ret != RTNORM);

		do {
			ret = sds_gettabletinput(ResourceString(IDC_CMDTABLET_CFG_9,"\nDigitize upper right corner of the Screen Pointing Area :"),ptScreen[1]);
			if ((ret == RTERROR) || (ret == RTCAN))
				goto out;
			} while (ret != RTNORM);
		}
	else if (strisame(fs1,"NO"/*DNT*/)) 
		{
		//the default tablet coords go from bottom left (0,0) to top right (size.cx, size.cy)
		SDS_CMainWindow->m_pTablet->GetTabletSize (ptDig[0], ptDig[1], ptDig[2]);

		RECT rctSystem;
		SDS_CMainWindow->m_pTablet->GetScreenSize (&rctSystem);

		ptScreen[0][0] = rctSystem.left;
		ptScreen[0][1] = rctSystem.bottom;
		ptScreen[1][0] = rctSystem.right;
		ptScreen[1][1] = rctSystem.top;

		goto out;
		}
	else
		goto top;

out:
	sds_matrix OTrans, ATrans, PTrans;
	double Oerr, Aerr, Perr;
	int BestFit;
	int nRows, nCols;

	//********
	//Set up the Template area
	//Calculate a matrix that goes from Tablet Coordinates to a Grid coordiantes.  
	//So set up grid size (0,0 to nrows,ncols)
	SDS_CMainWindow->m_pTablet->GetNumberOfRows (&nRows);
	SDS_CMainWindow->m_pTablet->GetNumberOfCols (&nCols);
	(ptWorld[0])[0] = 0.0;
	(ptWorld[0])[1] = 0.0;
	(ptWorld[0])[2] = 0.0;
	(ptWorld[1])[0] = 0.0;
	(ptWorld[1])[1] = nRows;
	(ptWorld[1])[2] = 0.0;
	(ptWorld[2])[0] = nCols;
	(ptWorld[2])[1] = nRows;
	(ptWorld[2])[2] = 0.0;
	ptDig[0][2] = 0;
	ptDig[1][2] = 0;
	ptDig[2][2] = 0;
	calculate_CalibrationXForm (3, ptDig, ptWorld, OTrans, &Oerr, ATrans, &Aerr, PTrans, &Perr, &BestFit);

	SDS_CMainWindow->m_pTablet->m_pWinTabTab->SetTransformation(ATrans, XFORM_AFFINE);
	SDS_CMainWindow->m_pTablet->SetGridSize (ptDig[0], ptDig[1], ptDig[2]);

	//********
	//Set up the Screen area
	RECT inrect;
	inrect.left = (long)ptScreen[0][0];
	inrect.bottom = (long)ptScreen[1][1];
	inrect.right = (long)ptScreen[1][0];
	inrect.top = (long)ptScreen[0][1];
	SDS_CMainWindow->m_pTablet->m_pWinTabSys->SetInputArea(&inrect);
	SDS_CMainWindow->m_pTablet->SetScreenSize (0,0,0,0, ptScreen[0], ptScreen[1]);

	if (ret==RTNONE)
		ret = RTNORM;
	return ret;
	}


class CTablet g_Tab;

short cmd_tablet(void)
	{

	if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF];
    int ret;
	short rc = RTNORM;
	
    for(;;) 
		{
		if (sds_initget(SDS_RSG_OTHER|SDS_RSG_NOLIM, ResourceString(IDC_CMDTABLET_MENUPROMPTS, "On|ON Off|OFF ~ Calibrate|CALibrate Configure|ConFiGure ~|CFG ~|Toggle ~_ON ~_OFF ~_ ~_CALibrate ~_ConFiGure ~_ConFiGure ~_T")) != RTNORM)
			{
			rc = -2;
			break;
			}

		ret = sds_getkword( ResourceString(IDC_CMDTABLET_KEYWORDPROMPTS, "\nTablet:  ON/OFF/CALibrate/ConFiGure/<On>: "),fs1 );
        if(ret==RTERROR) 
			{
            rc = -2;
			break;
			}
		else if(ret==RTCAN) 
			{
            rc = -1;
			break;
			} 
		else if(ret==RTNONE) 
			{
			strcpy (fs1, "ON"/*DNT*/);
	        }

		//if the tablet has not yet been initialized, try once more
		if (!SDS_CMainWindow->m_pTablet)
			{
			//initialize the array with the default tablet menus
			SDS_CMainWindow->m_pTablet = new CIcadTablet();
			if (SDS_CMainWindow->m_pTablet == NULL)
				{
				sds_printf("%s"/*DNT*/,ResourceString(IDC_CMDTABLET_INIT_ERR, "\nTablet initialization failed."));
				return RTERROR;
				}

			char err[80];
			if (!SDS_CMainWindow->m_pTablet->Initialize(SDS_CMainWindow->GetSafeHwnd(), err, sizeof(err)))
				{
				sds_printf("%s"/*DNT*/,ResourceString(IDC_CMDTABLET_INIT_ERR, "\nTablet initialization failed."));
				delete SDS_CMainWindow->m_pTablet;
				SDS_CMainWindow->m_pTablet = NULL;
				return RTERROR;
				}
			}

		if (strisame(fs1,"ON"/*DNT*/)) 
			{
			ret = RTNORM;

			if (!SDS_CMainWindow->m_pTablet->IsCalibrated())
				ret = g_Tab.cmd_calibrate();

			if (ret == RTNORM)
				{
				SDS_CMainWindow->m_pTablet->SetCalibrated();
				ret = g_Tab.SetTabletOn();
				}
			}
		else if (strisame(fs1,"OFF"/*DNT*/)) 
			{
			ret = g_Tab.SetTabletOff();
			}
		else if (strisame(fs1,"T"/*DNT*/)) 
			{
			ret = g_Tab.ToggleTablet();
			}
		else if (strisame(fs1,"CALIBRATE"/*DNT*/)) 
			{
			//Turn off the tablet (and any calibrated context) in case it was on
			ret = g_Tab.SetTabletOff();

			//Turn off any previously configured contexts
			SDS_CMainWindow->m_pTablet->EnableCfg(FALSE);

			ret = g_Tab.cmd_calibrate();

			if (ret == RTNORM)
				{
				SDS_CMainWindow->m_pTablet->SetCalibrated();
				ret = g_Tab.SetTabletOn();
				}
			else if (ret == RTCAN)
				ret = RTNORM;  //reset the return value, so that we don't loop back into this command.
			}
		else if(strisame(fs1,"CONFIGURE"/*DNT*/))
			{
			//Turn off the tablet (and any calibrated context) in case it was on
			ret = g_Tab.SetTabletOff();

			//Turn off any previously configured contexts
			SDS_CMainWindow->m_pTablet->EnableCfg(FALSE);

			ret = g_Tab.cmd_configure();
			if (ret == RTNORM)
				{
				SDS_CMainWindow->m_pTablet->SetConfigured();
				}
			//this is important, since it will turn on the CFG context.  
			//If the cmd_configure failed or was cancelled, it will turn on the previous CFG if any.
			ret = g_Tab.SetTabletOff();
			}
		else
			continue;

        if(ret==RTERROR) 
			{
            rc = -2;
			break;
			} 
		else if(ret==RTCAN) 
			{
            continue;
	        }
		else if (ret==RTNORM)
			{
			rc = RTNORM;
			break;
			}
		}

	return rc;

}


