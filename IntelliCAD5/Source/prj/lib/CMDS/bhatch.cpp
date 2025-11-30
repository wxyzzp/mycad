/* ..\SOURCE\PRJ\LIB\CMDS\BHATCH.CPP
 *
 * Author	: Albert.Isaev.ITC[31.7.2002/15:12:17]
 *
 * Abstract	: -BHATCH command implementation
 *
 * Notes	: The "Select", "Remove Islands" and "Rays casting" options aren't implemented
 *			  The resource strings aren't updated
 *
 * THIS FILE HAS BEEN GLOBALIZED!
 */

#include "cmds.h"/*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"

#include "Geometry.h"/*DNT*/
#include "Extents.h"/*DNT*/
#include "Region.h"/*DNT*/
#include "Boolean.h"/*DNT*/
#include "BhatchDialog.h"/*DNT*/
#include "BPolyBuilder.h"/*DNT*/
#include "BHatchBuilder.h"
#include "GeoConvert.h"/*DNT*/
#include "bhatch.h"/*DNT*/
#include "cmdsHatch.h"

/*M---------------------------------------------------------------------------*/
/*	  Name: 	 CALL_GOTO_EXIT 											  */
/*	  Purpose:	 Macro checks result of function and if it not equal to 	  */
/*				 expected then set specified returned result and go to exit   */
/*				 label														  */
/*	  Context:																  */
/*	  Parameters:															  */
/*				call - function call										  */
/*				exp  - expected result of function							  */
/*				ret  - real returned result in error case					  */
/*				res  - result variable										  */
/*				exit - exit label											  */
/*	  Returns:	None														  */
/*	  Notes:																  */
/*M---------------------------------------------------------------------------*/
#define CALL_GOTO_EXIT( call, exp, ret, res, exit )\
	if( call != exp ){ res = ret; goto exit; }

/*M---------------------------------------------------------------------------*/
/*	  Name: 	 COND_GOTO_EXIT 											  */
/*	  Purpose:	 Macro checks condition for FALSE and if it is FALSE		  */
/*				 then set specified returned result and go to exit label	  */
/*	  Context:																  */
/*	  Parameters:															  */
/*				condition - condition which checked for FALSE, must be TRUE   */
/*				ret 	  - real returned result in error case				  */
/*				res 	  - result variable 								  */
/*				exit	  - exit label										  */
/*	  Returns:	None														  */
/*	  Notes:																  */
/*M---------------------------------------------------------------------------*/
#define COND_GOTO_EXIT( condition, ret, res, exit )\
	if( !(condition)){ res = ret; goto exit; }

/*F---------------------------------------------------------------------------*/
/*	  Name: 	 cmd_bhatch_cmdline 										  */
/*	  Purpose:	 Function performs command line input of parameters of bhatch */
/*				 dialog for hatch object									  */
/*	  Context:																  */
/*	  Parameters:															  */
/*	  Returns:	 RTERROR - Any error										  */
/*				 RTNORM  - Success											  */
/*	  Notes:																  */
/*F---------------------------------------------------------------------------*/
short cmd_bhatch_cmdline( void )
{
	cmd_HatchedOK = false;

	if( cmd_iswindowopen() == RTERROR ) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if( cmd_istransparent() == RTERROR ) return RTERROR;

	RC	 lRC = 0L;					//Result of operations
	long lResult = 0L;				//Result of functions

	char chHPName[IC_ACADNMLEN];		//Char buffer for hatch pattern name
	char chHPFileName[IC_ACADNMLEN];	//Char buffer for name of hatch patterns file
	char chBuffer1[IC_ACADBUF];			//Char buffer for user input strings
	char chBuffer2[IC_ACADBUF];			//Char buffer for user input strings
	char chBuffer3[IC_ACADBUF];			//Char buffer for user input strings

	struct resbuf SetGetRB;			//Buffer for set/get operations

	sds_real sdsrReal = 0;			//Temporary variable for set/get operations
	sds_real sdsrHPScale = 0;		//Current hatch pattern scale
	sds_real sdsrHPAngle = 0;		//Current hatch pattern angle (in rads)
	sds_real sdsrHPSpace = 0;		//Current hatch pattern lines space
	sds_real sdsrRCAngle = 0;		//Current ray casting angle

	sds_point sdsPoint;				//User click point

	bool bLines = false;			//Flag for user defined hatch pattern
	bool bSolid = false;			//Flag for solid hatch pattern
	bool bProceed = false;			//Flag for proceed default operations
	bool bIslandDetection = true;	//Flag for perform island detection
	bool bFirstPickPoints = true;	//Flag for accepting new items to hatch
	bool bIsAdvBoundaryNew = true;	//Flag for boundary set option

	RayCastingOption eRayCastingOption = RCO_NEAREST;	//Ray casting method

// From BHatchBoundaryTab
	HanditemCollection	BoundarySet;									//Current boundary set
	IslandOption		eIslandOption = Nested_Islands;					//Current island detection method
	int					iBoundarySet = BHatchBoundaryTab::ALL_OBJECTS;	//Flag of objects' set to hatch
	bool				bPickPointsCalled = false;						//Flag of points pick operation
	bool				bRetainBoundary = false;						//Flag for retain boundary operation

// From BHatchPatPropTab
	int		iPatType = BHatchPatPropTab::PREDEFINED;	//Current hatch pattern type
	bool	bCrossHatch = false;						//Flag for crossing user defined lines
	bool	bAssociative = true;						//Flag for associativity of hatch
	//CString	sIsoPenWidth = _T("");

// From BHatchPatternTab
	cmd_hpdefll	*pPatternDef = NULL;	//Current hatch pattern definition
	int			iNumPatLines = 0;		//Number of pattern lines

//From BHatchDialog
	// Original system variables.
	// Need to save the original system variables so that they can be restored when the user cancels
	IslandOption	eOrigIslandOption;			//Saver for original value of island edtection method
	bool			bOrigCrossHatch;			//Saver for original value of crossing user defined lines flag
	sds_real		sdsrOrigHPScale;				//Saver for original value of hatch pattern scale
	sds_real		sdsrOrigHPSpace;				//Saver for original value of user defined lines space
	sds_real		sdsrOrigHPAngle;				//Saver for original value of hatch pattern angle
	char			sOrigName[IC_ACADNMLEN];	//Saver for original hatch pattern name

	C3Point ptNormal;		//Normal point

	//***Get normal coordinates
	GetUcsZ( ptNormal );

	//***Creating a new hatch entity
	db_hatch *pHatch = new db_hatch( SDS_CURDWG );	//Pointer to boundary hatch entity
	ASSERT( pHatch );

	//***Creating a hatch builder object
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBHatchBuilder HatchBuilder( ptNormal, lResult );	//Boundary hatch builder object
#else
	CBHatchBuilder HatchBuilder( HATCH_TOLERANCE, ptNormal, lResult );	//Boundary hatch builder object
#endif
	
	COND_GOTO_EXIT( SUCCEEDED( lResult ), (-__LINE__), lRC, bail );

	//***Set hatch to hatch builder
	COND_GOTO_EXIT( HatchBuilder.set_hatch( pHatch, true ), (-__LINE__), lRC, bail );

	//***Set pattern file name
	//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
	SDS_getvar(NULL,DB_QMEASUREMENT,&SetGetRB,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy( chHPFileName, ( SetGetRB.resval.rint == DB_ENGLISH_MEASUREINIT ?
		SDS_PATTERNFILE : SDS_ISOPATTERNFILE));
	//strcpy( chHPFileName, SDS_PATTERNFILE );
	//EBATECH(CNBR)]-

	//***Get the default hatch properties
	COND_GOTO_EXIT( InitBHatchCMD( pHatch, &pPatternDef, sdsrHPScale, sdsrHPAngle, sdsrHPSpace,
								   bCrossHatch, iPatType, bSolid, iNumPatLines, eIslandOption,
								   bAssociative, (char*)chHPName, (char*)chHPFileName ),
					(-__LINE__), lRC, bail );

	//***Save original hatch properties
	eOrigIslandOption = eIslandOption;
	sdsrOrigHPScale = sdsrHPScale;
	sdsrOrigHPAngle = sdsrHPAngle;
	sdsrOrigHPSpace = sdsrHPSpace;
	bOrigCrossHatch = bCrossHatch;
	strcpy( sOrigName, chHPName );

	//***Get pointer to hatch
	COND_GOTO_EXIT( HatchBuilder.get_hatch( &pHatch ), (-__LINE__), lRC, bail );
	ASSERT( pHatch != NULL );

	//***Pattern recalculation with obtained parameters
	COND_GOTO_EXIT( RecalcPatternBHatchCMD( pHatch, (char*)chHPName, &pPatternDef,
											iPatType, iNumPatLines, bCrossHatch ),
					(-__LINE__), lRC, bail );

	//***Set user defined pattern flag
	bLines = strisame( chHPName, USER );

	//***Print current hatch pattern name

	CALL_GOTO_EXIT( sds_printf( ResourceString( IDC_BHATCH_CURRENT_STYLE, "\nCurrent hatch pattern: %s"), chHPName ), RTNORM, (-__LINE__), lRC, bail );

	/*B---------------------------------------------------------------------------*/
	/*	  Code Block:		BIG LOOP! BIG LOOP! BIG LOOP!						  */
	/*						Getting hatch parameters and preview				  */
	/*B---------------------------------------------------------------------------*/

	for( ; ; )
	{
		HatchBuilder.m_updateSeeds = true;
		HatchBuilder.m_storeSeeds = false;

		/* Save current OSMODE */
		CALL_GOTO_EXIT( sds_getvar( "OSMODE"/*DNT*/, &SetGetRB ), RTNORM, (-__LINE__), lRC, bail );
		int iPrevMode = SetGetRB.resval.rint;
		SetGetRB.resval.rint = 0;
		CALL_GOTO_EXIT( sds_setvar( "OSMODE"/*DNT*/, &SetGetRB ), RTNORM, (-__LINE__), lRC, bail );

		//***Boundary set can only be set before first pick points is called.
		if( !bPickPointsCalled || BoundarySet.IsEmpty())
		{
			iBoundarySet = BHatchBoundaryTab::ALL_OBJECTS;

			CALL_GOTO_EXIT( cmd_getAllEntities( BoundarySet ), RTNORM, (-__LINE__), lRC, bail );

			bPickPointsCalled = true;
		}

	//	From PickPoints( hatchDia, HatchBuilder, bFirstPickPoints, ptNormal );
		//***Only add boundary set the first time pick points is selected
		if( bFirstPickPoints )
		{
			//***Use the normal vector calculated from the beginning.
			COND_GOTO_EXIT( HatchBuilder.AcceptHanditemColl( SDS_CURDWG, BoundarySet, ptNormal ), (-__LINE__), lRC, bail );

			bFirstPickPoints = false;
		}

		// Set island detection
		HatchBuilder.SetIslandDetection( eIslandOption );

		//CALL_GOTO_EXIT( sds_initget( RSG_OTHER, "~Properties ~Select ~Remove ~Islands ~Advanced ~_Properties ~_Select ~_Remove ~_Islands ~_Advanced" ),
		CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString( IDC_BHATCH_POPMENU_1, "Hatch_properties|Properties Select_objects|Select Remove_islands|Remove ~Islands Advanced_options|Advanced ~_Properties ~_Select ~_Remove ~_Islands ~_Advanced" )),
						RTNORM, (-__LINE__), lRC, bail );
		strcpy( chBuffer2, ResourceString( IDC_BHATCH_PROMPT_1, "\nSpecify internal point or [Properties/Select/Remove islands/Advanced]: " ));

		//***Get user's "pick point" or keyword
		while(( lResult = internalGetpoint( NULL, chBuffer2, sdsPoint )) == RTNORM )
		{
			//***Processing new point and check result
			if( cmd_boundaryPointSelection( sdsPoint, HatchBuilder, BoundarySet ) != RTNORM )
			{
				//***If point is not valid then promt and continue
			}

			CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString( IDC_BHATCH_POPMENU_1, "Hatch_properties|Properties Select_objects|Select Remove_islands|Remove ~Islands Advanced_options|Advanced ~_Properties ~_Select ~_Remove ~_Islands ~_Advanced" )),
							RTNORM, (-__LINE__), lRC, bail );
		}

		/* Restore current OSMODE */
		SetGetRB.resval.rint = iPrevMode;
		CALL_GOTO_EXIT( sds_setvar( "OSMODE", &SetGetRB ), RTNORM, (-__LINE__), lRC, bail );

		//***Check result
		COND_GOTO_EXIT(( lResult != RTERROR ), (-__LINE__), lRC, bail );
		COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

		//***Check result for keyword
		if( lResult == RTKWORD )
		{
			//If a keyword is entered then get it to char buffer
			CALL_GOTO_EXIT( sds_getinput( chBuffer1 ), RTNORM, (-__LINE__), lRC, bail );

			//Uppercase the keyword for comparing
			strupr( chBuffer1 );

			//Check the keyword
			if( strisame( chBuffer1, "PROPERTIES"/*DNT*/ ))
			{
				bool bAngleEntered = false;
				bool bScSpcEntered = false;
				bool bCrossEntered = false;

				if( *chHPName )
				{
					if( strisame( chHPName, USER ))
					{ /* chHPName == "_USER" */
						CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_POPMENU_2, "~? List_patterns|List ~ Solid_fill|Solid User_defined|User ~ LINES|Lines ~Line ~& ~_? ~_List ~_ ~_Solid ~_User ~_ ~_Lines ~_Line ~_&" )),
										RTNORM, (-__LINE__), lRC, bail );
						strncpy( chBuffer2, ResourceString(IDC_BHATCH_PROMPT_2, "\nEnter a pattern name or: ? to list patterns/Solid/User defined/<USER> : "), sizeof( chBuffer2 ) - 1 );
					}
					else
					{ /* chHPName != "_USER" */
						// 1-57915
						sprintf( chBuffer2, ResourceString(IDC_BHATCH_POPMENU_3, "~? List_patterns|List ~ Solid_fill|Solid User_defined|User ~ LINES|Lines ~Line ~& ~User ~%s ~_? ~_List ~_ ~_Solid ~_User ~_ ~_Lines ~_Line ~_& ~%s"), chHPName, chHPName );
						CALL_GOTO_EXIT( sds_initget( RSG_OTHER, chBuffer2 ), RTNORM, (-__LINE__), lRC, bail );
						sprintf( chBuffer2, ResourceString(IDC_BHATCH_PROMPT_3, "\nEnter a pattern name or: ? to list patterns/Solid/User defined/<%s> : "), chHPName );
					}
				}
				else
				{ /* chHPName == "" */
					CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_POPMENU_4, "~? List_patterns|List ~ Solid_fill|Solid User_defined|User ~ LINES|Lines ~Line ~& ~_? ~_List ~_ ~_Solid ~_User ~_ ~_Lines ~_Line ~_&" )),
									RTNORM, (-__LINE__), lRC, bail );
					strncpy( chBuffer2, ResourceString(IDC_BHATCH_PROMPT_4, "\nEnter a pattern name or: ? to list patterns/Solid/User defined : "), sizeof( chBuffer2 ) - 1 );
				}

				//***(NAME[,ANGLE[,SPACE[,DOUBLE(0,1)]]]) - for use defined hatch pattern
				//***(NAME[,SCALE[,ANGLE]]) - for predefined pattern
				COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );
				COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

				// If user hits ENTER to accept the default...
				if( !*chBuffer3 )
				{
					// Check pattern name
					if( !*chHPName )
					{
						CALL_GOTO_EXIT( sds_printf( ResourceString(IDC_BHATCH_NOTDEFINED, "\nPattern name is not defined")), RTNORM, (-__LINE__), lRC, bail );
						continue;
					}

					if( strisame( chHPName, USER ))
					{	// If USER pattern was the default, do same as if they entered "USER".
						bLines = true;
						bSolid = false;
						iPatType = BHatchPatPropTab::USER_DEFINED;
						sdsrHPScale = 1.0;
						iNumPatLines = 1;
						COND_GOTO_EXIT( pHatch->set_76( iPatType ), (-__LINE__), lRC, bail );
					}
					else
					{
						// Otherwise use the existing chHPName as if the user entered it.
						strcpy( chBuffer1, chHPName );
					}
				} /* if( !*chBuffer3 ) */
				else
				{ /* Keyword is entered */

					//Uppercase the keyword
					strupr( chBuffer3 );

					if( strisame( chBuffer3, "?"/*DNT*/ ) || strisame( chBuffer3, "_?"/*DNT*/ ) || strisame( chBuffer3, "LIST"/*DNT*/ ))
					{
						CALL_GOTO_EXIT( cmd_hatch_list(), RTNORM, (-__LINE__), lRC, bail );
						CALL_GOTO_EXIT( sds_printf( "\n"/*DNT*/ ), RTNORM, (-__LINE__), lRC, bail );
						continue;
					}
					else
					{ /* Keyword is pattern name */
						sds_real	sdsRA[3];	//Real array for command line parameters
						long		lIndex = 0;
						char		chTmpBuff[IC_ACADNMLEN];
						char		*pChar = NULL;
						char		*pNextChar = NULL;

						//***Copying entered string to temporary buffer
						strncpy( chTmpBuff, chBuffer3, sizeof( chTmpBuff ) - 1 );
						pChar = chTmpBuff;

						//***Parse string for hatch pattern parameters
						// 3 - maximum parameters quantity, followed after pattern name
						while((( pChar = strchr( pChar, ','/*DNT*/ )) != NULL ) && ( lIndex <= 3 ))
						{
							pChar ++;
							pNextChar = NULL;

							//***Get parameter value
							if(( pNextChar = strchr( pChar, ','/*DNT*/ )) != NULL )
							{
								*pNextChar = 0;
								sdsRA[lIndex] = atof( pChar );
								*pNextChar = ','/*DNT*/;
								lIndex ++;
							} /* if(( pNextChar = strchr( pChar, ',' )) != NULL ) */
							else
							{ /* Get last parameter value */
								if( pChar != NULL )
								{
									sdsRA[lIndex] = atof( pChar );
									lIndex ++;
								}
							} /* else */
						} //***End parse string

						//***Check for _USER pattern
						if((( !strncmp( chBuffer3, "&"/*DNT*/, 1 ) || !strncmp( chBuffer3, "U"/*DNT*/, 1 )) && ( chBuffer3[1] == 0 || chBuffer3[1] == ',' )) ||
						   (( !strncmp( chBuffer3, "USER"/*DNT*/, 4 ) || !strncmp( chBuffer3, "LINE"/*DNT*/, 4 )) && ( chBuffer3[4] == 0 || chBuffer3[4] == ',' )) ||
						   (( !strncmp( chBuffer3, "LINES"/*DNT*/, 5 ) || !strncmp( chBuffer3, USER, 5 )) && ( chBuffer3[5] == 0 || chBuffer3[5] == ',' )))
						{ /* If pattern is user defined */
							bLines = true;
							bSolid = false;
							sdsrHPScale = 1.0;
							iNumPatLines = 1;

							strcpy( chHPName, USER );
							iPatType = BHatchPatPropTab::USER_DEFINED;

							//***Set parameters for user defined pattern parsed above
							if( lIndex > 0 )
							{
								sdsrHPAngle = sdsRA[0] * IC_PI / 180;
								bAngleEntered = pHatch->set_52( sdsrHPAngle );
							}
							if( lIndex > 1 )
							{
								sdsrHPSpace = sdsRA[1];
								bScSpcEntered = pHatch->set_41( sdsrHPSpace );
							}
							if( lIndex > 2 )
							{
								bCrossHatch = sdsRA[2] == 1;
								bCrossEntered = pHatch->set_77( (int)bCrossHatch );
							}
						}
						else
						{ /* If pattern is predefined */
							bLines = false;

							//***Cut pattern name if needed
							if(( pChar = strchr( chBuffer3, ','/*DNT*/ )) != NULL )
							{
								*pChar = 0;
							}

							//***Copying pattern name
							strncpy( chHPName, chBuffer3, sizeof( chHPName ));
							iPatType = BHatchPatPropTab::PREDEFINED;

							//***Check for SOLID
							if(( bSolid = strisame( chHPName, "SOLID"/*DNT*/ )) == false )	// Don't verify for SOLID: it always exists.
							{
								//***Check pattern name
								if( cmd_hatch_valid( chHPName, chHPFileName) == RTERROR )
								{
									sds_printf( ResourceString(IDC_BHATCH_NOT_CORRECT, "\nPattern name \"%s\" is not correct"), chHPName );
									*chHPName = 0;
									continue;
								}
							}

							//***Set parameters for predefined pattern parsed above
							if( lIndex > 0 )
							{
								sdsrHPScale = sdsRA[0];
								bScSpcEntered = pHatch->set_41( sdsrHPScale );
							}
							if( lIndex > 1 )
							{
								sdsrHPAngle = sdsRA[1] * IC_PI / 180;
								bAngleEntered = pHatch->set_52( sdsrHPAngle );
							}
						}

						//***Set other pattern parameters
						COND_GOTO_EXIT( pHatch->set_76( iPatType ), (-__LINE__), lRC, bail );
						COND_GOTO_EXIT( pHatch->set_2( (char*)(LPCTSTR)chHPName ), (-__LINE__), lRC, bail );
						COND_GOTO_EXIT( pHatch->set_70( (int)bSolid ), (-__LINE__), lRC, bail );
					} /* Keyword is pattern name */
				} /* Keyword is entered */

				//***Ask for hatch pattern parameters
				if( !bSolid )
				{ /* If pattern is't SOLID then ask for hatch pattern parameters */

					//***Check for the pattern's type and whether scale is entered
					if( !bLines && !bScSpcEntered )
					{ /* If pattern is not user defined and if scale is not entered then ask for scale */

						//*** Get the hatch pattern scale
						CALL_GOTO_EXIT( sds_rtos( sdsrHPScale, -1, -1, chBuffer1 ), RTNORM, (-__LINE__), lRC, bail );

						sprintf( chBuffer2, ResourceString(IDC_BHATCH_SCALE, "\nProceed/Scale for pattern <%s>: "), chBuffer1 );

						CALL_GOTO_EXIT( sds_initget( RSG_NONEG + RSG_NOZERO, ResourceString(IDC_BHATCH_POPMENU_PROCEED, "Proceed ~_Proceed" )), RTNORM, (-__LINE__), lRC, bail );

						//Get scale value
						COND_GOTO_EXIT((( lResult = sds_getdist( NULL, chBuffer2, &sdsrReal )) != RTERROR ), (-__LINE__), lRC, bail );
						COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

						//Check for keyword
						if( lResult == RTKWORD )
						{
							CALL_GOTO_EXIT( sds_getinput( chBuffer3 ), RTNORM, (-__LINE__), lRC, bail );

							if( *chBuffer3 )
							{
								strupr( chBuffer3 );
								bProceed = strisame( chBuffer3, "PROCEED"/*DNT*/ );
							}
						}
						else if( lResult == RTNORM )
						{
							//***Set scale for predefined hatch pattern
							sdsrHPScale = sdsrReal;
							COND_GOTO_EXIT( pHatch->set_41( sdsrHPScale ), (-__LINE__), lRC, bail );
						}
					} /* if( !bLines ) */

					//***Check whether angle is entered
					if( !bAngleEntered )
					{ /* If angle is not entered then ask for angle */

						//*** Get the angle for the hatch pattern
						CALL_GOTO_EXIT( sds_angtos( sdsrHPAngle, 0/*in degrees*/, -1, chBuffer1 ), RTNORM, (-__LINE__), lRC, bail );

						//***Check for user defined pattern
						if( bLines )
						{ /* Ask for angle for user defined pattern */
							sprintf( chBuffer2, ResourceString(IDC_BHATCH_ANGLE, "\nProceed/Angle for lines <%s>: "), chBuffer1 );
							CALL_GOTO_EXIT( sds_initget( 0, ResourceString(IDC_BHATCH_POPMENU_PROCEED, "Proceed ~_Proceed" )), RTNORM, (-__LINE__), lRC, bail );
						} /* if( bLines ) */
						else
						{ /* If not proceed then ask for angle for predefined pattern */
							if( !bProceed )
							{
								sprintf( chBuffer2, ResourceString(IDC_BHATCH_ANGLE2, "\nAngle for pattern <%s>: "), chBuffer1 );
								CALL_GOTO_EXIT( sds_initget( 0, NULL ), RTNORM, (-__LINE__), lRC, bail );
							} /* if( !bProceed ) */
						} /* if( !bLines ) */

						//***Ask angle if not proceed
						if( !bProceed )
						{
							//Get angle value
							COND_GOTO_EXIT((( lResult = sds_getangle( NULL, chBuffer2, &sdsrReal )) != RTERROR ), (-__LINE__), lRC, bail );
							COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

							//Check for keyword
							if( lResult == RTKWORD )
							{
								CALL_GOTO_EXIT( sds_getinput( chBuffer3 ), RTNORM, (-__LINE__), lRC, bail );

								if( *chBuffer3 )
								{
									strupr( chBuffer3 );
									bProceed = strisame( chBuffer3, "PROCEED"/*DNT*/ );
								} /* if( !*chBuffer3 ) */
							} /* if( lResult == RTKWORD ) */
							else if( lResult == RTNORM )
							{
								//***Set angle for all patters types
								sdsrHPAngle = sdsrReal;
								COND_GOTO_EXIT( pHatch->set_52( sdsrHPAngle ), (-__LINE__), lRC, bail );
							}
						} /* if( !bProceed ) */
					}

					//***Check for the pattern's type and whether proceed is true
					if( !bProceed && bLines )
					{
						sdsrHPScale = 1.0;

						//***Check whether the space for user defined is entered
						if( !bScSpcEntered )
						{
							//*** Spacing between lines
							CALL_GOTO_EXIT( sds_rtos( sdsrHPSpace, -1, -1, chBuffer3 ), RTNORM, (-__LINE__), lRC, bail );

							sprintf( chBuffer2, ResourceString(IDC_BHATCH_SPACE, "\nSpace between standard pattern lines <%s>: "), chBuffer3 );

							CALL_GOTO_EXIT( sds_initget( RSG_NONEG + RSG_NOZERO, NULL ), RTNORM, (-__LINE__), lRC, bail );

							COND_GOTO_EXIT((( lResult = sds_getdist( NULL, chBuffer2, &sdsrReal )) != RTERROR ), (-__LINE__), lRC, bail );
							COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

							if( lResult == RTNORM )
							{
								//***Set spasing between user defined lines
								sdsrHPSpace = sdsrReal;
								COND_GOTO_EXIT( pHatch->set_41( sdsrHPSpace ), (-__LINE__), lRC, bail );
							}
						}

						//***Check whether the double for user defined is entered
						if( !bCrossEntered )
						{
							//*** Double hatching
							if( bCrossHatch )
							{
								_tcscpy( chBuffer3, ResourceString( IDC_HATCH_YES_34, "Yes" ));
							}
							else
							{
								_tcscpy( chBuffer3, ResourceString( IDC_HATCH_NO_35, "No" ));
							}

							sprintf( chBuffer2, ResourceString(IDC_BHATCH_CROSS, "\nCross-hatch area? Yes/No/<%s> "), chBuffer3 );

							CALL_GOTO_EXIT( sds_initget( 0, ResourceString(IDC_BHATCH_POPMENU_CROSS, "Yes-Cross_hatch|Yes No-One_direction|No ~_Yes ~_No" )), RTNORM, (-__LINE__), lRC, bail );

							COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );
							COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

							if(( lResult == RTNORM ) && ( *chBuffer3 ))
							{
								strupr( chBuffer3 );

								//***Set double for user defined lines
								bCrossHatch = strisame( chBuffer3, "YES"/*DNT*/ );
								COND_GOTO_EXIT( pHatch->set_77( (int)bCrossHatch ), (-__LINE__), lRC, bail );
							}
						}
					} /* if( !bProceed && bLines ) */
				} /* if( !bSolid ) */
			} /* if( strisame( chBuffer1, "PROPERTIES" )) */

			/*B---------------------------------------------------------------------------*/
			/*	  Code Block: "SELECT" method don't work properly !!!					  */
			/*B---------------------------------------------------------------------------*/

			else if( strisame( chBuffer1, "SELECT"/*DNT*/ ))
//			{
//				sds_printf( "\nNot implemented" );
//				continue;
//			}
			{
				HanditemCollection	SelectedSet;

				//***Get the selected entities
				COND_GOTO_EXIT((( lResult = cmd_getEntitySet( SelectedSet )) != RTERROR ), (-__LINE__), lRC, bail );
				COND_GOTO_EXIT(( lResult != RTCAN ), (-1L), lRC, bail );

				if( !SelectedSet.IsEmpty())
				{
					long lSize = 0;
					bool bSelected = false;
					CandidateList *pCandidates = NULL;

					HatchBuilder.m_updateSeeds = true;
					HatchBuilder.m_storeSeeds = false;

					bFirstPickPoints = false;
					iBoundarySet = BHatchBoundaryTab::CURRENT_SET;

					COND_GOTO_EXIT( HatchBuilder.AcceptHanditemColl( SDS_CURDWG, SelectedSet, ptNormal ),
									(-__LINE__), lRC, bail );

					HatchBuilder.SetIslandDetection( eIslandOption );

					//***Get intersections & split paths
					COND_GOTO_EXIT(( HatchBuilder.m_pFragment->Operate() == SUCCESS ), (-__LINE__), lRC, bail );

					//***Create closed boundaries
					COND_GOTO_EXIT(( HatchBuilder.m_pFragment->WrapUp() == SUCCESS ), (-__LINE__), lRC, bail );

					//***Get pointer to candidates list
					pCandidates = HatchBuilder.m_pCandidates;

					//***Fill m_Islands array for each candidate
					COND_GOTO_EXIT(( pCandidates->GetIslands() == SUCCESS ), (-__LINE__), lRC, bail );

					//***Get candidates quantity
					lSize = pCandidates->Size();
					if( lSize )
					{
						long lIndex = 0;
						// from CandidateList::ClickPoint for these items
						for( ; lIndex < lSize; lIndex ++ )
						{
							if( pCandidates->Item( lIndex )->GetContainer( pCandidates, false ) == -1 &&
								pCandidates->Item( lIndex )->IsOpen( pCandidates ) &&
								!pCandidates->Item( lIndex )->IsFinalist() &&
								!pCandidates->Item( lIndex )->IsHatchFinalist())
							{
								pCandidates->Item( lIndex )->SetFinalist( true, pCandidates->m_RecursionDepth );
								pCandidates->Item( lIndex )->SetHatchFinalist( true, MAX_ISLAND_DEPTH );
							}
						}

						// Add some other candidates to the finalists set - holes (not islands!) in non-finalists.
						for( lIndex = 0; lIndex < lSize; lIndex ++ )
						{
							Candidate*	AnItem = pCandidates->Item( lIndex );
							int	ItemContainerNum = AnItem->GetContainer( pCandidates, false );

							if( !AnItem->IsFinalist() && !AnItem->GetParent() && ItemContainerNum != -1 &&
								!pCandidates->Item( ItemContainerNum )->IsFinalist())
							{
								AnItem->SetFinalist( true, pCandidates->m_RecursionDepth );
								AnItem->SetHatchFinalist( true, MAX_ISLAND_DEPTH );
							}
						}
					} // if(Size)

					//***Persist most resently selected
					COND_GOTO_EXIT(( HatchBuilder.PersistMostRecentlySelected( bSelected ) == SUCCESS ), (-__LINE__), lRC, bail );

					//***Redraw entities
					{
						db_handitem *pEntity = NULL;
						db_handitem *pPosition = NULL;
						HanditemCollection *pSelectedEnts = NULL;

						if( !bSelected )
						{
							if( !BoundarySet.IsEmpty())
							{
								pEntity = BoundarySet.GetNext( NULL );

								while( pEntity )
								{
									pPosition = pEntity;

									cmd_redrawEntity( pEntity, IC_REDRAW_UNHILITE );

									pEntity = BoundarySet.GetNext( pPosition );
								}
							}

							HatchBuilder.Persist();
						}

						pSelectedEnts = HatchBuilder.getHanditemCollection();

						if( pSelectedEnts )
						{
							cmd_drawHICollection( pSelectedEnts, IC_REDRAW_HILITE );
						}
					}
				} /* if( !Selected.IsEmpty()) */
				else
				{
					CALL_GOTO_EXIT( sds_printf( ResourceString(IDC_BHATCH_NO_SELECTED, "\nThere are no selected objects" )), RTNORM, (-1L), lRC, bail );
					continue;
				}
			}

			/*B---------------------------------------------------------------------------*/
			/*	  Code Block: REMOVE option is not implemented							  */
			/*B---------------------------------------------------------------------------*/

			else if( strisame( chBuffer1, "REMOVE"/*DNT*/ ))
			{
				sds_printf( ResourceString(IDC_BHATCH_NOTIMPLEMENTED, "\nNot implemented") );
				continue;
			}
			else if( strisame( chBuffer1, "ADVANCED"/*DNT*/ ))
			{

				CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_POPMENU_5, "Boundary_set|Boundary Retain_boundary|Retain Island_detection|Island ~Detection Specify_style|Style Set_associativity|Associativity ~_Boundary ~_Retain ~_Island ~_Detection ~_Style ~_Associativity" )),
								RTNORM, (-__LINE__), lRC, bail );
				strncpy( chBuffer2, ResourceString(IDC_BHATCH_PROMPT_5, "\nEnter an option: Boundary set/Retain boundary/Island detection/Style/Associativity : "), sizeof( chBuffer2 ) - 1 );

				COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

				if( lResult == RTNORM )
				{
					strupr( chBuffer3 );

					if( strisame( chBuffer3, "BOUNDARY"/*DNT*/ ))
					{
						if( bIsAdvBoundaryNew )
						{
							strcpy( chBuffer2, ResourceString(IDC_BHATCH_BOUNDARY_NEW, "\nSpecify candidate set for boundary: Everything/<New>: " ));
						}
						else
						{
							strcpy( chBuffer2, ResourceString(IDC_BHATCH_BOUNDARY_EVERY, "\nSpecify candidate set for boundary: New/<Everything>: " ));
						}

						CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_BOUNDARY_POPMENU, "New_selection|New All_objects|Everything ~_New ~_Everything" )), RTNORM, (-__LINE__), lRC, bail );

						COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

						if( lResult == RTNORM || lResult == RTNONE )
						{
							strupr( chBuffer3 );
							bFirstPickPoints = false;

							//***Purge old selection data
							HatchBuilder.m_pFragment->Purge();

							if(( !*chBuffer3 && !bIsAdvBoundaryNew ) ||
							   ( *chBuffer3 && strisame( chBuffer3, "EVERYTHING"/*DNT*/ )))
							{
								COND_GOTO_EXIT((( lResult = cmd_getAllEntities( BoundarySet )) != RTERROR ), (-__LINE__), lRC, bail );

								if(( lResult == RTNORM ) && ( !BoundarySet.IsEmpty() ))
								{
									bIsAdvBoundaryNew = false;
									iBoundarySet = BHatchBoundaryTab::ALL_OBJECTS;
								}
							}
							if(( !*chBuffer3 && bIsAdvBoundaryNew ) ||
							   ( *chBuffer3 && strisame( chBuffer3, "NEW"/*DNT*/ )))
							{
								COND_GOTO_EXIT((( lResult = cmd_getEntitySet( BoundarySet )) != RTERROR ), (-__LINE__), lRC, bail );

								if(( lResult == RTNORM ) && ( !BoundarySet.IsEmpty() ))
								{
									bIsAdvBoundaryNew = true;
									iBoundarySet = BHatchBoundaryTab::CURRENT_SET;
								}
							}

							//***Accept the new selection
							COND_GOTO_EXIT( HatchBuilder.AcceptHanditemColl( SDS_CURDWG, BoundarySet, ptNormal ), (-__LINE__), lRC, bail );
						}
					}
					else if( strisame( chBuffer3, "RETAIN"/*DNT*/ ))
					{
						if( bRetainBoundary )
						{
							_tcscpy( chBuffer3, ResourceString( IDC_HATCH_YES_34, "Yes" ));
						}
						else
						{
							_tcscpy( chBuffer3, ResourceString( IDC_HATCH_NO_35, "No" ));
						}

						CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_RETAIN_POPMENU, "Yes-Retain_boundary|Yes No-Discard_boundary ~_Yes ~_No" )), RTNORM, (-__LINE__), lRC, bail );
						sprintf( chBuffer2, ResourceString(IDC_BHATCH_RETAIN_PROMPT, "\nRetain derived boundaries? Yes/No/<%s> : "), chBuffer3 );

						COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

						if(( lResult == RTNORM ) && ( *chBuffer3 ))
						{
							strupr( chBuffer3 );
							bRetainBoundary = strisame( chBuffer3, "YES"/*DNT*/ );
						}
					}
					else if( strisame( chBuffer3, "STYLE"/*DNT*/ ))
					{
						CALL_GOTO_EXIT( sds_initget( NULL, ResourceString(IDC_BHATCH_STYLE_POPMENU, "Normal ~Nested ~Standard Outer ~All Ignore ~_Normal ~_Nested ~_Standard ~_Outer ~_All ~_Ignore" )),
										RTNORM, (-__LINE__), lRC, bail );
						if( eIslandOption == Nested_Islands )
						{
							sprintf( chBuffer2, ResourceString(IDC_BHATCH_STYLE_NORMAL, "\nEnter hatching style: Ignore/Outer/Normal/<Normal>: " ));
						}
						else if( eIslandOption == Outer_Only )
						{
							sprintf( chBuffer2, ResourceString(IDC_BHATCH_STYLE_OUTER, "\nEnter hatching style: Ignore/Outer/Normal/<Outer>: " ));
						}
						else if( eIslandOption == Ignore_Islands )
						{
							sprintf( chBuffer2, ResourceString(IDC_BHATCH_STYLE_IGNORE, "\nEnter hatching style: Ignore/Outer/Normal/<Ignore>: " ));
						}
						else
						{
							ASSERT( false );
						}

						COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

						if( lResult == RTNORM )
						{
							if( strisame( chBuffer3, "NORMAL"/*DNT*/ ) || strisame( chBuffer3, "STANDARD"/*DNT*/ ) || strisame( chBuffer3, "NESTED"/*DNT*/ ))
							{
								SetGetRB.resval.rint = 0;
								eIslandOption = Nested_Islands;
							}
							else if( strisame( chBuffer3, "OUTER"/*DNT*/ ))
							{
								SetGetRB.resval.rint = 1;
								eIslandOption = Outer_Only;
							}
							else if( strisame( chBuffer3, "ALL"/*DNT*/ ) || strisame( chBuffer3, "IGNORE"/*DNT*/ ))
							{
								SetGetRB.resval.rint = 2;
								eIslandOption = Ignore_Islands;
							}
							else
							{
								ASSERT( false );
							}

							COND_GOTO_EXIT( pHatch->set_75( SetGetRB.resval.rint ), (-__LINE__), lRC, bail );
						}
					}
					else if( strisame( chBuffer3, "ISLAND"/*DNT*/ ) || strisame( chBuffer3, "DETECTION"/*DNT*/ ))
					{
						if( bIslandDetection )
						{
							_tcscpy( chBuffer3, ResourceString( IDC_HATCH_YES_34, "Yes" ));
						}
						else
						{
							_tcscpy( chBuffer3, ResourceString( IDC_HATCH_NO_35, "No" ));
						}

						CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_DETECT_POPMENU, "Yes-Detect_islands|Yes No-Set_ray_casting|No ~_Yes ~_No" )), RTNORM, (-__LINE__), lRC, bail );
						sprintf( chBuffer2, ResourceString(IDC_BHATCH_DETECT_PROMPT, "\nDo you want island detection? Yes/No/<%s> : "), chBuffer3 );

						COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

						if(( lResult == RTNORM ) && ( *chBuffer3 ))
						{
							strupr( chBuffer3 );

							bIslandDetection = strisame( chBuffer3, "YES"/*DNT*/ );
							if( !bIslandDetection )
							{
								switch( eRayCastingOption )
								{
									case RCO_NEAREST:
										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_NEAREST, "\nEnter type of ray casting: Nearest/+X/-X/+Y/-Y/Angle/<Nearest>: " ));
										break;
									case RCO_PLUSX:
										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_PLUS_X, "\nEnter type of ray casting: Nearest/+X/-X/+Y/-Y/Angle/<+X>: " ));
										break;
									case RCO_MINUSX:
										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_MINUS_X, "\nEnter type of ray casting: Nearest/+X/-X/+Y/-Y/Angle/<-X>: " ));
										break;
									case RCO_PLUSY:
										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_PLUS_Y, "\nEnter type of ray casting: Nearest/+X/-X/+Y/-Y/Angle/<+Y>: " ));
										break;
									case RCO_MINUSY:
										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_MINUS_Y, "\nEnter type of ray casting: Nearest/+X/-X/+Y/-Y/Angle/<-Y>: " ));
										break;
									case RCO_ANGLE:
										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_ANGLE, "\nEnter type of ray casting: Nearest/+X/-X/+Y/-Y/Angle/<Angle>: " ));
										break;
									default:
										ASSERT( false );
								} /* switch */

								CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_RAYCAST_POPMENU, "Nearest +X -X +Y -Y Angle ~_Nearest ~_+X ~_-X ~_+Y ~_-Y ~_Angle" )),
												RTNORM, (-__LINE__), lRC, bail );

								COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

								if( lResult == RTNORM )
								{
									strupr( chBuffer3 );
									if( *chBuffer3 )
									{
										if( strisame( chBuffer3, "NEAREST" ))
										{
											eRayCastingOption = RCO_NEAREST;
										}
										else if( strisame( chBuffer3, "+X" ))
										{
											eRayCastingOption = RCO_PLUSX;
										}
										else if( strisame( chBuffer3, "-X" ))
										{
											eRayCastingOption = RCO_MINUSX;
										}
										else if( strisame( chBuffer3, "+Y" ))
										{
											eRayCastingOption = RCO_PLUSY;
										}
										else if( strisame( chBuffer3, "-Y" ))
										{
											eRayCastingOption = RCO_MINUSY;
										}
									}

									//***If RCO_ANGLE then exclusive processing
									if(( !*chBuffer3 ) && ( eRayCastingOption == RCO_ANGLE ) ||
									   ( *chBuffer3 ) && strisame( chBuffer3, "ANGLE" ))
									{
										//*** Get the angle for the ray casting
										CALL_GOTO_EXIT( sds_angtos( sdsrRCAngle, 0/*in degrees*/, -1, chBuffer1 ), RTNORM, (-__LINE__), lRC, bail );

										sprintf( chBuffer2, ResourceString(IDC_BHATCH_RAYCAST_ANGLE_PROMPT, "\nSpecify ray casting angle <%s>: "), chBuffer1 );

										CALL_GOTO_EXIT( sds_initget( 0, NULL ), RTNORM, (-__LINE__), lRC, bail );

										COND_GOTO_EXIT((( lResult = sds_getangle( NULL, chBuffer2, &sdsrRCAngle )) != RTERROR ), (-__LINE__), lRC, bail );

										if( lResult == RTNORM )
										{
											eRayCastingOption = RCO_ANGLE;
										}
									} /* End RCO_ANGLE exclusive processing */
								}
							} /* if( !bIslandDetection ) */
						}
					}
					else if( strisame( chBuffer3, "ASSOCIATIVITY"/*DNT*/ ))
					{
						if( bAssociative )
						{
							_tcscpy( chBuffer3, ResourceString( IDC_HATCH_YES_34, "Yes" ));
						}
						else
						{
							_tcscpy( chBuffer3, ResourceString( IDC_HATCH_NO_35, "No" ));
						}

						CALL_GOTO_EXIT( sds_initget( RSG_OTHER, ResourceString(IDC_BHATCH_ASSOC_POPMENU, "Yes-Associative_hatch|Yes No-Nonassociative_hatch|No ~_Yes ~_No" )), RTNORM, (-__LINE__), lRC, bail );
						sprintf( chBuffer2, ResourceString(IDC_BHATCH_ASSOC_PROMPT, "\nAssociativity hatch? Yes/No/<%s> : "), chBuffer3 );

						COND_GOTO_EXIT((( lResult = sds_getkword( chBuffer2, chBuffer3 )) != RTERROR ), (-__LINE__), lRC, bail );

						if(( lResult == RTNORM ) && ( *chBuffer3 ))
						{
							strupr( chBuffer3 );

							//***Set associativity for hatch
							bAssociative = strisame( chBuffer3, "YES"/*DNT*/ );
							COND_GOTO_EXIT( pHatch->set_71( (int)bAssociative ), (-__LINE__), lRC, bail );
						}
					} /* else if( strisame( chBuffer3, "ASSOCIATIVITY" )) */
				}
			} /* else if( strisame( chBuffer1, "ADVANCED" )) */
		} /* else if( lResult == RTKWORD ) */
		else if( lResult == RTNONE )
		{
			if( bPickPointsCalled &&
				( HatchBuilder.m_pCandidates->Size() > 0 ) &&
				( HatchBuilder.m_pCandidates->NumHatchPaths() > 0 ))
			{
				HatchBuilder.m_updateSeeds = false;
				HatchBuilder.m_storeSeeds = true;

				if( bRetainBoundary )
				{
					if( !RetainBoundaryBHatchCMD( HatchBuilder, BoundarySet, ptNormal, eIslandOption ))
					{
						//***Accept boundary to hatch and add hatch object to the current drawing
						CALL_GOTO_EXIT( lResult = cmd_boundaryAccept( HatchBuilder ), RTNORM, (-__LINE__), lRC, bail );
					} /* if( !RetainBoundaryBHatchCMD( HatchBuilder, BoundarySet, ptNormal, eIslandOption )) */
				} /* if( bRetainBoundary ) */
				else
				{
					//***Accept boundary to hatch and add hatch object to the current drawing
					CALL_GOTO_EXIT( lResult = cmd_boundaryAccept( HatchBuilder ), RTNORM, (-__LINE__), lRC, bail );
				}

			} /* if( bPickPointsCalled && HatchBuilder.m_pCandidates->Size() > 0 ) */

			//***Redraw the current drawing
			CALL_GOTO_EXIT( SDS_RedrawDrawing( SDS_CURDWG, NULL, NULL, 1 ), RTNORM, (-__LINE__), lRC, bail );

			//***Get off the BIG LOOP
			goto bail;
		} /* else if( lResult == RTNONE ) */

		//***Set proceed flag to negative
		bProceed = false;

		//***Set system variables
		COND_GOTO_EXIT( SetSVarsBHatchCMD( false, chHPName, iPatType, sdsrHPScale, sdsrHPAngle, sdsrHPSpace, bCrossHatch, eIslandOption ),
						(-__LINE__), lRC, bail );

		//*** Get the default hatch properties
		COND_GOTO_EXIT( InitBHatchCMD( pHatch, &pPatternDef, sdsrHPScale, sdsrHPAngle, sdsrHPSpace,
									   bCrossHatch, iPatType, bSolid, iNumPatLines, eIslandOption,
									   bAssociative, (char*)chHPName, (char*)chHPFileName ),
						(-1L), lRC, bail );

		// From UpdatePatern
		//***Only draw if user has selected boundary path
		if( bPickPointsCalled )
		{
			COND_GOTO_EXIT( HatchBuilder.get_hatch( &pHatch ), (-__LINE__), lRC, bail );
			ASSERT( pHatch );

			cmd_redrawEntity( pHatch, IC_REDRAW_UNDRAW );
		}

		//***Recalculate hatch pattern
		COND_GOTO_EXIT( RecalcPatternBHatchCMD( pHatch, chHPName, &pPatternDef, iPatType, iNumPatLines, bCrossHatch ),
						(-__LINE__), lRC, bail );

		//***Only draw if user has selected boundary path
		if( bPickPointsCalled )
		{
			cmd_redrawEntity( pHatch, IC_REDRAW_HILITE );
		}

	} //End of getting hatch parameters and preview
	//***BIG LOOP! BIG LOOP! BIG LOOP!

//***Exit
bail:

	if( lRC < 0L )
	{
		HatchBuilder.m_updateSeeds = false;
		HatchBuilder.m_storeSeeds = false;

		cmd_boundaryCancel( HatchBuilder );

		//***Restore system variables
		if( !SetSVarsBHatchCMD( true, sOrigName, iPatType, sdsrOrigHPScale, sdsrOrigHPAngle,
								sdsrOrigHPSpace, bOrigCrossHatch, eOrigIslandOption ))
		{
			sds_printf( ResourceString(IDC_BHATCH_CANT_RESTORE, "\nOriginal system variables aren't restored" ));
		}
	}

	if( pPatternDef )
	{
		cmd_hatch_freehpdefll( &pPatternDef );
	}

	if( lRC == 0L )
	{
		cmd_HatchedOK = true;
		// EBATECH(CNBR) Regen Hatch display list.
		{
			sds_name ename;
			ename[0] = (long)( pHatch );
			ename[1] =	(long)( SDS_CURDWG );
			sds_entupd( ename );
		}
		//
		return RTNORM;
	}
	if( lRC == (-1L))
	{
		return RTCAN;
	}
	if( lRC < (-1L))
	{
		CMD_INTERNAL_MSG( lRC );
	}

	return RTERROR;
}

/*F---------------------------------------------------------------------------*/
/*	  Name: 	  InitBHatchCMD 											  */
/*	  Purpose:	  Internal initialization of bhatch object					  */
/*	  Context:																  */
/*	  Parameters:															  */
/*		  pHatch		 - Pointer to hatch object							  */
/*		  ppPatDef		 - Address of pointer to pattern definition 		  */
/*		  sdsrHPScale	 - Hatch pattern scale								  */
/*		  sdsrHPAngle	 - Hatch pattern angle (in rads)					  */
/*		  sdsrHPSpace	 - Hatch pattern space between lines				  */
/*		  bCrossHatch	 - Flag for crossing hatch							  */
/*		  iPatType		 - Hatch pattern type (PREDEFINED or USER_DEFINED)	  */
/*		  iNumPatLines	 - Hatch pattern lines number						  */
/*		  eIO			 - Hatch pattern islands detection option			  */
/*						   (Normal, Outer or Ignore)						  */
/*		  bAssociative	 - Flag for associativity							  */
/*		  pHPName		 - Hatch pattern name								  */
/*		  sPatFile		 - Hatch pattern file name							  */
/*	  Returns: true  - If succeed											  */
/*			   false - Otherwise											  */
/*	  Notes:																  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool InitBHatchCMD( db_hatch		*pHatch,
					cmd_hpdefll		**ppPatDef,
					sds_real		&sdsrHPScale,
					sds_real		&sdsrHPAngle,
					sds_real		&sdsrHPSpace,
					bool			&bCrossHatch,
					int				&iPatType,
					bool			&bSolid,
					int				&iNumPatLines,
					IslandOption	&eIO,
					bool			bAssociative,
					char			*pchHPName,
					char			*pchHPFileName )
{
	int iTemp = 0;
	resbuf SetGetRB;

	SetGetRB.resval.rstring = NULL;
	SetGetRB.rbnext = NULL;

	//*** Get the default hatch pattern parameters
	if( SDS_getvar( NULL, DB_QHPSTYLE, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;
	iTemp = SetGetRB.resval.rint;
	eIO = (IslandOption)( iTemp == 2 ? 0 : iTemp == 0 ? 2 : 1 );

	if( SDS_getvar( NULL, DB_QHPSCALE, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;
	sdsrHPScale = SetGetRB.resval.rreal;

	if( SDS_getvar( NULL, DB_QHPANG, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;
	sdsrHPAngle = SetGetRB.resval.rreal;

	if( SDS_getvar( NULL, DB_QHPSPACE, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;
	sdsrHPSpace = SetGetRB.resval.rreal;

	if( SDS_getvar( NULL, DB_QHPDOUBLE, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;
	bCrossHatch = SetGetRB.resval.rint ? true : false;

	//EBATECH(CNBR)-[ 2003/10/05 Default linetype file is dependent on $MEASUREMENT.
	bool bIsoHatch = false;
	if( SDS_getvar( NULL, DB_QMEASUREMENT, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;
	bIsoHatch = SetGetRB.resval.rint ? true : false;
	// EBATECH(CNBR) ]-

	if( SDS_getvar( NULL, DB_QHPNAME, &SetGetRB, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM ) return false;

	if( strisame( SetGetRB.resval.rstring, USER ))
	{
		iPatType = BHatchPatPropTab::USER_DEFINED;
		strcpy( pchHPName, SetGetRB.resval.rstring );
	}
	//EBATECH(CNBR)-[ 2003/10/05 Default linetype file is dependent on $MEASUREMENT.
	else if( strisame( SetGetRB.resval.rstring, "" ) && bIsoHatch == true )
	{
		iPatType = BHatchPatPropTab::USER_DEFINED;
		strcpy( pchHPName, USER );
	}
	// EBATECH(CNBR) ]-
	else
	{

		// if no pattern - default to ANSI31
		if( strisame( SetGetRB.resval.rstring, "" ))
		{
			strcpy( pchHPName, "ANSI31"/*DNT*/ );
		}
		else
		{
			strcpy( pchHPName, SetGetRB.resval.rstring );
		}

		iPatType = BHatchPatPropTab::PREDEFINED;

		// If predefined, initialize the file from the pattern name and the pattern def.
		iNumPatLines = cmd_hatch_loadpat( pchHPFileName, pchHPName, ppPatDef );

		// if error set pattern to user defined so that some pattern is displayed.
		if( iNumPatLines == RTERROR )
		{
			strcpy( pchHPName, USER/*DNT*/ );
			iPatType = BHatchPatPropTab::USER_DEFINED;
			iNumPatLines = 1;
		}
	}

	//freeing a string buffer if it was used
	IC_FREE( SetGetRB.resval.rstring );

	strupr( pchHPName );
	bSolid = (bool)strisame( pchHPName, "SOLID"/*DNT*/ );

// From InitializeHatch
	//set hatch properties
	pHatch->set_70( (int)bSolid ); // support from user interface
	pHatch->set_75( iTemp );
	pHatch->set_52( sdsrHPAngle );
	pHatch->set_71( bAssociative );
	//Set values needed for a specific pattern type
	//Note: need to map pattern type values from their combo box order to their dxf value
	switch( iPatType )
	{
		//set parameters for predefined pattern
		case BHatchPatPropTab::PREDEFINED:
		{
			pHatch->set_2( (char*)(LPCTSTR)pchHPName ); //set predefined pattern name
			pHatch->set_76( 1 ); //predefined pattern
			pHatch->set_41( sdsrHPScale ); //set scale for predefined pattern
			pHatch->set_77( 0 );
			break;
		}
		//set parameters for user defined pattern
		case BHatchPatPropTab::USER_DEFINED:
		{
			pHatch->set_2( (char*)(LPCTSTR)USER ); //set const user defined pattern name
			pHatch->set_76( 0 ); //user defined pattern
			pHatch->set_41( sdsrHPSpace ); //set line space for user defined pattern
			pHatch->set_77( (int)bCrossHatch );
			break;
		}
		//no other case
		default:
		{
			ASSERT( false );
		}
	}

	return true;
}

/*F---------------------------------------------------------------------------*/
/*	  Name: 	  RecalcPatternBHatchCMD									  */
/*	  Purpose:	  Function performs internal recalcuation of pattern data	  */
/*	  Context:																  */
/*	  Parameters:															  */
/*		pHatch		 - Poiter to hatch object								  */
/*		pHPName 	 - Hatch pattern name									  */
/*		ppPatternDef - Hatch pattern definition 							  */
/*		iParType	 - Hatch pattern type (PREDEFINED or USER_DEFINED)		  */
/*		iNumPatLines - Hatch pattern lines number							  */
/*		bCrossHatch  - Hatch pattern crossing flag							  */
/*	  Returns: true  - If succeed											  */
/*			   false - Otherwise											  */
/*	  Notes:																  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool RecalcPatternBHatchCMD( db_hatch		*pHatch,
							 char			*pHPName,
							 cmd_hpdefll	**ppPatternDef,
							 int			iPatType,
							 int			iNumPatLines,
							 bool			bCrossHatch )
{
	long		lNumLines = 0;
	cmd_hpdefll	*pCurrentHPDef = NULL;
	cmd_hpdefll	*pSaveHPDef = NULL;

	// load proper pattern def
	switch( iPatType )
	{
		case BHatchPatPropTab::PREDEFINED:
		{
			// set specific predefined hatch settings
			pHatch->set_2( (char*)(LPCTSTR)pHPName );
			lNumLines = iNumPatLines;
			pCurrentHPDef = *ppPatternDef;
			break;
		}
		case BHatchPatPropTab::USER_DEFINED:
		{
			// set specific user defined hatch settings
			pHatch->set_2( (char*)(LPCTSTR)USER );
			lNumLines = bCrossHatch ? 2 : 1;
			// Note: don't pass in space here because it is adjusted for space below
			if( !cmd_loadUserDefPattern( &pCurrentHPDef, (int)bCrossHatch ))
			{
				return false;
			}
			break;
		}
		default:
		{
			ASSERT( false );  // should never get here
		}
	}

	if( !cmd_SetHatchFromDef( lNumLines, pCurrentHPDef, pHatch ))
	{
		return false;
	}

	if( pSaveHPDef )	// For user-defined
	{
		pCurrentHPDef->next = pSaveHPDef;	// (see above).
	}

	// need to free hatch def if user defined.
	if ( iPatType == BHatchPatPropTab::USER_DEFINED )
	{
		cmd_hatch_freehpdefll( &pCurrentHPDef );
	}

	return true;
}

/*F---------------------------------------------------------------------------*/
/*	  Name: 	  SetSVarsBHatchCMD 										  */
/*	  Purpose:	  Function sets system variables for hatch object			  */
/*	  Context:																  */
/*	  Parameters:															  */
/*		  bIsOriginal - Boolean flag for restore original system variables	  */
/*		  pchNPName   - Hatch pattern name									  */
/*		  iPatType	  - Hatch pattern style (PREDEFINED or USER_DEFINED)	  */
/*		  sdsrHPScale - Hatch pattern scale 								  */
/*		  sdsrHPAngle - Hatch pattern angle (in rads)						  */
/*		  sdsrHPSpace - Hatch pattern lines space							  */
/*		  bCrossHatch - Hatch pattern crossing flag 						  */
/*		  eIO		  - Hatch pattern island detection option				  */
/*						(Normal, Outer or Ignore)							  */
/*	  Returns: true  - If succeed											  */
/*			   false - Otherwise											  */
/*	  Notes:																  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool SetSVarsBHatchCMD( bool			bIsOriginal,
						char*			pchHPName,
						int				iPatType,
						sds_real		sdsrHPScale,
						sds_real		sdsrHPAngle,
						sds_real		sdsrHPSpace,
						bool			bCrossHatch,
						IslandOption	eIO )
{
	struct resbuf SetGetRB;

	//***Init buffer
	SetGetRB.resval.rstring = NULL;
	SetGetRB.rbnext = NULL;

	//*** Save changed system variables
	SetGetRB.restype = RTSHORT;
	SetGetRB.resval.rint = ( (int)eIO == 2 ? 0 : (int)eIO == 0 ? 2 : 1 );	//For DXF format compatibility
	if( sds_setvar( "HPSTYLE", &SetGetRB ) != RTNORM ) return false;

	SetGetRB.restype = RTREAL;
	SetGetRB.resval.rreal = sdsrHPAngle;
	if( sds_setvar( "HPANG"/*DNT*/, &SetGetRB ) != RTNORM ) return false;

	if( *pchHPName )
	{
		SetGetRB.restype = RTSTR;
		SetGetRB.resval.rstring = pchHPName;
		if( sds_setvar( "HPNAME"/*DNT*/, &SetGetRB ) != RTNORM ) return false;
	}
	else
	{
		char chBuffer[6];
		SetGetRB.restype = RTSTR;
		// cannot pass buffer to const
		strcpy( chBuffer, USER );
		SetGetRB.resval.rstring = chBuffer;
		if( sds_setvar( "HPNAME"/*DNT*/, &SetGetRB ) != RTNORM ) return false;
	}

	if( bIsOriginal || ( iPatType == BHatchPatPropTab::PREDEFINED ))
	{
		SetGetRB.restype = RTREAL;
		SetGetRB.resval.rreal = sdsrHPScale;
		if( sds_setvar( "HPSCALE"/*DNT*/, &SetGetRB ) != RTNORM ) return false;
	}

	if( bIsOriginal || ( iPatType == BHatchPatPropTab::USER_DEFINED ))
	{
		SetGetRB.restype = RTREAL;
		SetGetRB.resval.rreal = sdsrHPSpace;
		if( sds_setvar( "HPSPACE"/*DNT*/, &SetGetRB ) != RTNORM ) return false;

		SetGetRB.restype = RTSHORT;
		SetGetRB.resval.rint = bCrossHatch;
		if( sds_setvar( "HPDOUBLE"/*DNT*/, &SetGetRB ) != RTNORM ) return false;
	}

	return true;
}

/*F---------------------------------------------------------------------------*/
/*	  Name: 	  RetainBoundaryBHatchCMD									  */
/*	  Purpose:	  Function retains the objects builded from hatch boundary	  */
/*	  Context:																  */
/*	  Parameters:															  */
/*		HatchBuilder  - Hatch builder object								  */
/*		BoundarySet   - Active boundary set to retain						  */
/*		ptNormal	  - Normal point										  */
/*		eIslandOption - Hatch pattern island detection option				  */
/*						(Normal, Outer or Ignore)							  */
/*	  Returns: true   - If succeed											  */
/*			   false  - Otherwise											  */
/*	  Notes:																  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool RetainBoundaryBHatchCMD( CBHatchBuilder		&HatchBuilder,
							  HanditemCollection	&BoundarySet,
							  C3Point				ptNormal,
							  IslandOption			eIslandOption )
{
	RC					lRC = 0;
	long				lResult = 0;
	long				lIndex = 0;
	int					iNumSeeds = 0;
	int					iAssociativity = 0;
	bool				bSelected = false;
	sds_point			sdsPoint;
	db_hatch			*pHatch = NULL;
	HanditemCollection	*pSelectedEnts = NULL;
	HanditemCollection	*pNewSelectedEnts = NULL;

	// Exit the function if no finalists are found
	if( HatchBuilder.Persist() == FAILURE )
	{
		return false;
	}

	pSelectedEnts = HatchBuilder.getHanditemCollection();

	if( pSelectedEnts )
	{
		db_handitem *pEntity = pSelectedEnts->GetNext( NULL );
		while( pEntity )
		{
			// ensure pEntity is not destroyed when list is destroyed since it is being
			// added to the drawing.
			pSelectedEnts->SetDestroyHI( pEntity, false );
			pEntity = pSelectedEnts->GetNext( pEntity );
		} /* while( pEntity ) */
	} /* if( pSelectedEnts ) */

	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBPolyBuilder BoundaryBuilder( ptNormal, lResult );
#else
	CBPolyBuilder BoundaryBuilder( HATCH_TOLERANCE, ptNormal, lResult );
#endif
	
	ASSERT( SUCCEEDED( lResult ));
	//Bugzilla No. 78414 ; 01-30-2003 
#ifdef VARYING_HATCH_TOLERANCE
	CBHatchBuilder NewHatchBuilder( ptNormal, lResult );
#else
	CBHatchBuilder NewHatchBuilder( HATCH_TOLERANCE, ptNormal, lResult );
#endif

	ASSERT( SUCCEEDED( lResult ));

	HatchBuilder.get_hatch( &pHatch );
	pHatch->get_71( &iAssociativity );

	if( iAssociativity )
	{
		pHatch->BreakAssociation( false );
		pHatch->set_71( iAssociativity );
	} /* if( iAssociativity ) */

	// From RetainBoundaryWithHatch
	// build pline
	BoundaryBuilder.AcceptHanditemColl( SDS_CURDWG, BoundarySet, ptNormal );
	BoundaryBuilder.SetIslandDetection( eIslandOption );

	pHatch->get_98( &iNumSeeds );

	for( lIndex = 0; lIndex < iNumSeeds; lIndex ++ )
	{
		pHatch->get_seed( sdsPoint, lIndex );
		BoundaryBuilder.ClickPoint( sdsPoint[0], sdsPoint[1] );
	} /* for( lIndex = 0; lIndex < iNumSeeds; lIndex ++ ) */

	if( BoundaryBuilder.PersistMostRecentlySelected( bSelected ) == FAILURE )
	{
		return false;
	} /* if( BoundaryBuilder.PersistMostRecentlySelected( bSelected ) == FAILURE ) */

	if( !bSelected )
	{
		BoundaryBuilder.Persist();
	} /* if( !bSelected ) */

	pNewSelectedEnts = BoundaryBuilder.getHanditemCollection();

	if( !pNewSelectedEnts || ( cmd_boundaryAccept( BoundaryBuilder ) != RTNORM ))
	{
		return false;
	}

	//***Build hatch
	// Set candidate list not to toggle since hatch has already been bSelected
	NewHatchBuilder.SetToggle( false );
	NewHatchBuilder.set_hatch( pHatch, true );
	NewHatchBuilder.AcceptHanditemColl( SDS_CURDWG, *pNewSelectedEnts, ptNormal );
	NewHatchBuilder.SetIslandDetection( eIslandOption );

	for( lIndex = 0; lIndex < iNumSeeds; lIndex ++ )
	{
		pHatch->get_seed( sdsPoint, lIndex );
		NewHatchBuilder.ClickPoint( sdsPoint[0], sdsPoint[1] );
	} /* for( lIndex = 0; lIndex < iNumSeeds; lIndex ++ ) */

	if( cmd_boundaryAccept( NewHatchBuilder ) != RTNORM )
	{
		return false;
	} /* if( cmd_boundaryAccept( NewHatchBuilder ) != RTNORM ) */

	return true;
}

/* Undefine macro CALL_GOTO_EXIT */
#ifdef CALL_GOTO_EXIT
#undef CALL_GOTO_EXIT
#endif /* #ifdef CALL_GOTO_EXIT */

/* Undefine macro COND_GOTO_EXIT */
#ifdef COND_GOTO_EXIT
#undef COND_GOTO_EXIT
#endif /* #ifdef COND_GOTO_EXIT */

/*F---------------------------------------------------------------------------*/
/*	  Name: 	 cmd_hatchedit		 										  */
/*	  Notes:																  */
/*F---------------------------------------------------------------------------*/
short cmd_hatchedit( void )
{
	int ret;
    long    slen;
	sds_name ename;
    sds_point point;
	db_hatch *hatchEntity;
	extern	void IcadHatchPropDia(db_hatch *hatch ,sds_name * hatches);

	if( cmd_iswindowopen() == RTERROR ) return RTERROR;
	if( cmd_istransparent() == RTERROR ) return RTERROR;

    for( slen = -1; ; slen = 0)
    {
        if( slen < 0 )
        {
            if( sds_sslength(SDS_CURDOC->m_pGripSelection,&slen) == RTNORM && slen > 0L)
            {
                sds_ssname(SDS_CURDOC->m_pGripSelection,0,ename);
            }
        }
        if( slen == 0 )
        {
    		ret = sds_entsel(ResourceString(IDC_HATCHEDIT_SELECT,"\nSelect associative hatch object:"/*DNT*/),
                ename, point);
	    	if( ret != RTNORM )
            {
                return ret;
            }
        }
		hatchEntity = (db_hatch*)ename[0];
		if( hatchEntity->ret_type() == DB_HATCH )
        {
            break;
        }
        sds_printf(ResourceString(IDC_HATCHEDIT_ERROR,"\nSelected object must be a hatch object or associative hatch block.\n\
HATCHEDIT does not support old-format non-associative hatch blocks."));
	}
    // Show Hatch edit dialog box
	IcadHatchPropDia(hatchEntity, (sds_name *)0 );
    sds_redraw(ename,IC_REDRAW_DRAW);
	return RTNORM;
}
