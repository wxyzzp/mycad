/* ..\SOURCE\PRJ\LIB\CMDS\BHATCH.H
 * 
 * Author	: Albert.Isaev.ITC[31.7.2002/15:12:17]
 * 
 * Abstract	: -BHATCH functions and types definitions
 *
 */ 

#ifndef BHATCH_H
#define BHATCH_H

/*E---------------------------------------------------------------------------*/
/*    Name:       RayCastingOption                                            */
/*    Purpose:    Ray casting method enumeration for no island detection      */
/*                             NOT IMPLEMENTED!!!                             */
/*    Context:                                                                */
/*      RCO_NEAREST - Runs a line from the point you specify to the nearest   */
/*                    object and then traces the boundary in                  */
/*                    a counterclockwise direction.                           */
/*      RCO_PLUSX   - Runs a line in the positive X direction from the point  */
/*                    you specify to the first object encountered and then    */
/*                    traces the boundary in a counterclockwise direction.    */
/*      RCO_MINUSX  - Runs a line in the negative X direction from the point  */
/*                    you specify to the first object encountered and then    */
/*                    traces the boundary in a counterclockwise direction.    */
/*      RCO_PLUSY   - Runs a line in the positive Y direction from the point  */
/*                    you specify to the first object encountered and then    */
/*                    traces the boundary in a counterclockwise direction.    */
/*      RCO_MINUSY  - Runs a line in the negative Y direction from the point  */
/*                    you specify to the first object encountered and then    */
/*                    traces the boundary in a counterclockwise direction.    */
/*      RCO_ANGLE   - Runs a line at the specified angle from the point you   */
/*                    specify to the first object encountered and then traces */
/*                    the boundary in a counterclockwise direction.           */
/*    Parameters:                                                             */
/*    Returns:                                                                */
/*    Notes:                                                                  */
/*E---------------------------------------------------------------------------*/
enum RayCastingOption
{
	RCO_NEAREST,
	RCO_PLUSX,
	RCO_MINUSX,
	RCO_PLUSY,
	RCO_MINUSY,
	RCO_ANGLE
};

/*F---------------------------------------------------------------------------*/
/*    Name:       InitBHatchCMD                                               */
/*    Purpose:    Internal initialization of bhatch object                    */
/*    Context:                                                                */
/*    Parameters:                                                             */
/*        pHatch         - Pointer to hatch object                            */
/*        ppPatDef       - Address of pointer to pattern definition           */
/*        sdsrHPScale    - Hatch pattern scale                                */
/*        sdsrHPAngle    - Hatch pattern angle (in rads)                      */
/*        sdsrHPSpace    - Hatch pattern space between lines                  */
/*        bCrossHatch    - Flag for crossing hatch                            */
/*        iPatType       - Hatch pattern type (PREDEFINED or USER_DEFINED)    */
/*        iNumPatLines   - Hatch pattern lines number                         */
/*        eIO            - Hatch pattern islands detection option             */
/*                         (Normal, Outer or Ignore)                          */
/*        bAssociative   - Flag for associativity                             */
/*        pHPName        - Hatch pattern name                                 */
/*        sPatFile       - Hatch pattern file name                            */
/*    Returns: true  - If succeed                                             */
/*             false - Otherwise                                              */
/*    Notes:                                                                  */
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
					char			*pchHPFileName );

/*F---------------------------------------------------------------------------*/
/*    Name:       RecalcPatternBHatchCMD                                      */
/*    Purpose:    Function performs internal recalcuation of pattern data     */
/*    Context:                                                                */
/*    Parameters:                                                             */
/*      pHatch       - Poiter to hatch object                                 */
/*      pHPName      - Hatch pattern name                                     */
/*      ppPatternDef - Hatch pattern definition                               */
/*      iParType     - Hatch pattern type (PREDEFINED or USER_DEFINED)        */
/*      iNumPatLines - Hatch pattern lines number                             */
/*      bCrossHatch  - Hatch pattern crossing flag                            */
/*    Returns: true  - If succeed                                             */
/*             false - Otherwise                                              */
/*    Notes:                                                                  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool RecalcPatternBHatchCMD( db_hatch		*pHatch,
						     char			*pHPName,
							 cmd_hpdefll	**ppPatternDef,
							 int			iPatType,
							 int			iNumPatLines,
							 bool			bCrossHatch );

/*F---------------------------------------------------------------------------*/
/*    Name:       SetSVarsBHatchCMD                                           */
/*    Purpose:    Function sets system variables for hatch object             */
/*    Context:                                                                */
/*    Parameters:                                                             */
/*        bIsOriginal - Boolean flag for restore original system variables    */
/*        pchNPName   - Hatch pattern name                                    */
/*        iPatType    - Hatch pattern style (PREDEFINED or USER_DEFINED)      */
/*        sdsrHPScale - Hatch pattern scale                                   */
/*        sdsrHPAngle - Hatch pattern angle (in rads)                         */
/*        sdsrHPSpace - Hatch pattern lines space                             */
/*        bCrossHatch - Hatch pattern crossing flag                           */
/*        eIO         - Hatch pattern island detection option                 */
/*                      (Normal, Outer or Ignore)                             */
/*    Returns: true  - If succeed                                             */
/*             false - Otherwise                                              */
/*    Notes:                                                                  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool SetSVarsBHatchCMD( bool			bIsOriginal,
						char*			pchHPName,
					    int				iPatType,
						sds_real		sdsrHPScale,
						sds_real		sdsrHPAngle,
						sds_real		sdsrHPSpace,
						bool			bCrossHatch,
						IslandOption	eIO );

/*F---------------------------------------------------------------------------*/
/*    Name:       RetainBoundaryBHatchCMD                                     */
/*    Purpose:    Function retains the objects builded from hatch boundary    */
/*    Context:                                                                */
/*    Parameters:                                                             */
/*      HatchBuilder  - Hatch builder object                                  */
/*      BoundarySet   - Active boundary set to retain                         */
/*      ptNormal      - Normal point                                          */
/*      eIslandOption - Hatch pattern island detection option                 */
/*                      (Normal, Outer or Ignore)                             */
/*    Returns: true   - If succeed                                            */
/*             false  - Otherwise                                             */
/*    Notes:                                                                  */
/*F---------------------------------------------------------------------------*/
LOCAL
bool RetainBoundaryBHatchCMD( CBHatchBuilder		&HatchBuilder,
							  HanditemCollection	&BoundarySet,
							  C3Point				ptNormal,
							  IslandOption			eIslandOption );

#endif /* #ifndef BHATCH_H */
