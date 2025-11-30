#include "Icad.h"

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// EntityMatchesFilterList code
// These following functions add up to parse the ssget "X" filter lists
//
// TODO Add more error checking for well-formed filters
// TODO Move this code to it's own file
//

// *********************************************
// HELPER FUNCTION for EntityMatchesFilterList
//
//  This simply matches a single node of the filter list
// against the entity.  If we come across a case we never
// even thought of, we return the passed in bDefaultRetval
// so that the caller can "pretend" it got what it wants to
// continue on it's search
//
//
static bool
entityMatchesFilterNode( const struct resbuf *filter, db_handitem *elp, db_drawing *flp, bool bDefaultRetval )
	{
	bool bRetval = false;
	ASSERT( filter != NULL );
	char szTempBuffer[IC_ACADNMLEN];

	//Some strings are coming in as constants. strupr does not work with constants, so make a copy of 
	//the string before it is used.
	CString locString = filter->resval.rstring;

	switch( filter->restype )
		{
		case 0:	// Entity type
			locString.MakeUpper();
			strcpy(szTempBuffer,db_hitype2str(elp->ret_type()));
			bRetval = ( sds_wcmatch( szTempBuffer,locString) == RTNORM );
			break;
		case 1: // Text Values
			if((elp->ret_type()==DB_MTEXT || 
				elp->ret_type()==DB_TEXT || 
				elp->ret_type()==DB_ATTRIB || 
				elp->ret_type()==DB_ATTDEF))
				{
				char *fcp1;
				locString.MakeUpper();
				elp->get_1(&fcp1);  
				strncpy(szTempBuffer,fcp1,sizeof(szTempBuffer)-1);  
				strupr(szTempBuffer);
				bRetval = ( sds_wcmatch(szTempBuffer,locString) == RTNORM );
				}
			break;
		case 2:	 // Block Names
				 //Attribute Tag string    EBATECH(FUTA) 2001-8-10
			if((elp->ret_type()==DB_INSERT) ||
			   (elp->ret_type()==DB_ATTRIB) ||
			   (elp->ret_type()==DB_ATTDEF))
				{
				char *fcp1;
				locString.MakeUpper();
				elp->get_2(&fcp1);  
				strncpy(szTempBuffer,fcp1,sizeof(szTempBuffer)-1);  
				strupr(szTempBuffer);
				bRetval = ( sds_wcmatch(szTempBuffer,locString) == RTNORM );
				}
			break;
		case 3: // ATTDEF Prompt & Dimension style name    EBATECH(FUTA) 2001-8-10
			if((elp->ret_type()==DB_DIMENSION) ||
			   (elp->ret_type()==DB_ATTDEF))
				{
				char *fcp1;
				locString.MakeUpper();
				elp->get_3(&fcp1);  
				strncpy(szTempBuffer,fcp1,sizeof(szTempBuffer)-1);  
				strupr(szTempBuffer);
				bRetval = ( sds_wcmatch(szTempBuffer,locString) == RTNORM );
				}
			break;
		case 7: // Text style name    EBATECH(FUTA) 2001-8-10
			{ 
			char *fcp1;
			locString.MakeUpper();
			if( elp->get_7(&fcp1) )
				{
				strncpy(szTempBuffer,fcp1,sizeof(szTempBuffer)-1);  
				strupr(szTempBuffer);
				bRetval = ( sds_wcmatch(szTempBuffer,locString) == RTNORM );
				}
			}
			break;
		case 10:
			{
			sds_point point;
			if ( elp->get_10( point ) )
				{
				if (icadPointEqual(point,filter->resval.rpoint))
					{
					bRetval = true;
					}
				}
			}
			break;
		case 11:
			{
			sds_point point;
			if ( elp->get_11( point ) )
				{
				if (icadPointEqual(point,filter->resval.rpoint))
					{
					bRetval = true;
					}
				}
			}
			break;
		case 12:
			{
			sds_point point;
			if ( elp->get_12( point ) )
				{
				if (icadPointEqual(point,filter->resval.rpoint))
					{
					bRetval = true;
					}
				}
			}
			break;
		case 13:
			{
			sds_point point;
			if ( elp->get_13( point ) )
				{
				if (icadPointEqual(point,filter->resval.rpoint))
					{
					bRetval = true;
					}
				}
			}
			break;
		case 14:
			{
			sds_point point;
			if ( elp->get_14( point ) )
				{
				if (icadPointEqual(point,filter->resval.rpoint))
					{
					bRetval = true;
					}
				}
			}
			break;
		case 40: // Polyline width
			//if(elp->ret_type()==DB_POLYLINE) 
			//	{
			//	double fr1;
			//	elp->get_40(&fr1);
			// Text height , Radius    EBATECH(FUTA) 2001-8-10 [
			{
			double fr1;
			if ( elp->get_40(&fr1) )
				{
				bRetval = (fabs(filter->resval.rreal - fr1) < IC_ZRO); // ]
				}
			}
			break;
		case 66: // Block has attibutes
			if(elp->ret_type()==DB_INSERT) 
				{
				int fi1;
				elp->get_66(&fi1);
				bRetval = (filter->resval.rint == fi1);
				}
			break;
		case 67:  // MODEL SPACE    EBATECH(FUTA) 2001-8-10
			{
			int fi1;
			if ( elp->get_67(&fi1) )
				{
				bRetval = (filter->resval.rint == fi1 );
				}
			}
			break;
		case 8:		// LAYER NAME
			{
			char *fcp1;
			locString.MakeUpper();
			elp->get_8(&fcp1);  
			strncpy(szTempBuffer,fcp1,sizeof(szTempBuffer)-1);  
			strupr(szTempBuffer);
			bRetval = ( sds_wcmatch(szTempBuffer,locString) == RTNORM );
			}
			break;
		case 6:		// LINETYPE
			{  
			char *fcp1;
			locString.MakeUpper();
			elp->get_6(&fcp1);
			strncpy(szTempBuffer,fcp1,sizeof(szTempBuffer)-1);
			strupr(szTempBuffer);
			bRetval = ( sds_wcmatch(szTempBuffer,locString) == RTNORM );
			}
			break;
		case 60:	// Object visibility    EBATECH(FUTA) 2001-8-10
			{	 
			int fi1;
			elp->get_60(&fi1);
			bRetval = (filter->resval.rint == fi1 );
			}
			break;
		case 62:	// COLOR
			{	 
			int fi1;
			elp->get_62(&fi1);
			bRetval = (filter->resval.rint == fi1 );
			}
			break;
		case 69:	// various flags depending on entity type
			{
			int fi1;
			if ( elp->get_69(&fi1) )
				{
				bRetval = (filter->resval.rint == fi1 );
				}
			}
			break;
		case 39:   // THICKNESS
			{	 
			double fr1;
			elp->get_39(&fr1);
			bRetval = ( filter->resval.rreal == fr1 );
			}
			break;
		case 48:	// Linetype Scale
			{
			double fr1;
			elp->get_48(&fr1);
			bRetval = ( filter->resval.rreal == fr1 );
			}
			break;
		case -3: // Has any EED
			{
			struct resbuf *trb=sds_buildlist(RTSTR,"*"/*DNT*/,0);
			struct resbuf *eed=elp->get_eed(trb,NULL);
			sds_relrb(trb);
			if(eed!=NULL)
				{  
				bRetval = true;
				sds_relrb(eed);	
				eed=NULL;
				}
			}
			break;
		case 1001:  // EED for spacific app.
			{
			struct resbuf *trb=sds_buildlist(RTSTR,filter->resval.rstring,0);
			struct resbuf *eed=elp->get_eed(trb,NULL);
			sds_relrb(trb);
			if(eed!=NULL)
				{
				bRetval = true;
				sds_relrb(eed);	
				eed=NULL;
				}
			}
			break;
		default:

			// The bDefaultRetval is set so that the calling function can ignore
			// this node
			//
			bRetval = bDefaultRetval;
			break;

		}

	return bRetval;
	}

// **************************
// These are the various 
//
enum EntityFilterBooleanMode
	{
	MODE_NONE = 0,
	MODE_AND,
	MODE_OR,
	MODE_NAND,
	MODE_XOR
	};


// ****************************************************
// HELPER FUNCTION for EntityMatchesFilterList
//
// We may just want to skip a node because it's a type we don't know about.
//  In that case, we need search mode.  This truth table will keep 
// the sub-filter going for whatever mode is passed in.
//
static bool
defaultFilterResult( EntityFilterBooleanMode mode )
	{
	bool bRetval = false;
	switch( mode )
		{
		case MODE_AND:
			bRetval = true;
			break;
		case MODE_OR:
			bRetval = false;
			break;
		case MODE_NAND:
			bRetval = false;
			break;
		case MODE_XOR:
			bRetval = false;
			break;
		default:
			ASSERT( false );
			break;
		}

	return bRetval;
	}


// *****************************************
// HELPER FUNCTION for EntityMatchesFilterList
//
// From where we are, find the end of the sub-filter
//
// This will be the overall end of the expression, or the place
// where a closing "BOOL>" node matches the depth we start at
//
static const struct resbuf *
findExpressionEnd( const struct resbuf *filter )
	{

	const struct resbuf *pTemp = filter;

	int iNestingDepth = 1;
	while( pTemp != NULL )
		{
		if(pTemp->restype==-4) 
			{
			if(strisame("AND>"/*DNT*/,pTemp->resval.rstring) || 		
			   strisame("OR>"/*DNT*/,pTemp->resval.rstring) || 	
			   strisame("NOT>"/*DNT*/,pTemp->resval.rstring) || 
			   strisame("XOR>"/*DNT*/,pTemp->resval.rstring) ) 
				{
				iNestingDepth--;
				if ( iNestingDepth == 0 )
					{
					break;
					}
				}
  			if(strisame("<AND"/*DNT*/,pTemp->resval.rstring) || 		
			   strisame("<OR"/*DNT*/,pTemp->resval.rstring) || 	
			   strisame("<NOT"/*DNT*/,pTemp->resval.rstring) || 
			   strisame("<XOR"/*DNT*/,pTemp->resval.rstring) ) 
				{
				iNestingDepth++;
				}
			}
		pTemp = pTemp->rbnext;
		}

		return pTemp;
	}


// *************
// Forward decl so processFilterNode can recurse
// See below
//
static bool
walkFilterList(const struct resbuf *filter, 
			   db_handitem *elp, 
			   db_drawing *flp, 
			   EntityFilterBooleanMode mode,
			   struct resbuf **pProcessNext 
			   );

// *******************************************
// HELPER FUNCTION for EntityMatchesFilterList 
//
// If we are going a level deeper into booleans, recurse
// on walkFilterList, otherwise simply check if this node
// matches the entity
//
static bool
processFilterNode( const struct resbuf *filter,
					db_handitem *elp,
				   db_drawing *flp, 
				   EntityFilterBooleanMode mode,
				   struct resbuf **pProcessNext )
	{
	ASSERT( filter != NULL );
	ASSERT( pProcessNext != NULL );


	if ( filter == NULL )
		{
		return false;
		}

	bool bRetval;

	if ( filter->restype == -4 )
		{
		EntityFilterBooleanMode newmode = MODE_NONE;
		if(strisame("<AND"/*DNT*/,filter->resval.rstring)) 		
			{
			newmode = MODE_AND;
			}
		else if(strisame("<OR"/*DNT*/,filter->resval.rstring)) 	
			{
			newmode = MODE_OR;
			}
		else if(strisame("<NOT"/*DNT*/,filter->resval.rstring)) 
			{
			newmode = MODE_NAND;
			}
		else if(strisame("<XOR"/*DNT*/,filter->resval.rstring)) 
			{
			newmode = MODE_XOR;
			}
		else
			{
			// malformed list
			ASSERT( false );  // or the parser's wrong
			return false;
			}
		bRetval = walkFilterList( filter->rbnext, elp, flp, newmode, pProcessNext );
		}
	else
		{
		// Just check if we match the current node.  One trick -- we may just want to 
		// skip this node because it's a type we don't know about.  In that case, we need
		// to return different values depending on the mode
		// See defaultFilterResult()
		//
	   	bRetval = entityMatchesFilterNode( filter, elp, flp, defaultFilterResult( mode ) );

		// We've just processed this one, so move to next one
		//
		*pProcessNext = filter->rbnext;
		}

	return bRetval;
	}

// ****************************************************
// HELPER FUNCTION
//
// walkFilterList is a helper for EntityMatchesFilterList.
// It recursively walks down into the list, processing the various
// boolean commands and checking each node
//
// pProcessNext is an OUT param, and represents where the caller
// should continue processing the filter list because the callee has processed
// everything up to that
//
static bool
walkFilterList( const struct resbuf *filter, 
			   db_handitem *elp, 
			   db_drawing *flp, 
			   EntityFilterBooleanMode mode,
			   struct resbuf **pProcessNext 
			   )
	{
	ASSERT( filter != NULL );
	ASSERT( pProcessNext != NULL );

	// Let's find the end of the current sub-filter	so we know where to stop
	//
	struct resbuf *pExpressionEnd = (struct resbuf *)findExpressionEnd( filter );

	// We're going to want the next level up to start processing again right after
	// the end of this sub-filter
	//
	if ( pExpressionEnd != NULL )
		{
		*pProcessNext = pExpressionEnd->rbnext;
		}
	else
		{
		*pProcessNext = NULL;
		}

	// Initialize return value based on the mode.  I worked these out
	// by a truth table. 
	//
	bool bRetval;
	switch( mode )
		{
		case MODE_AND:
			bRetval = true;
			break;
		case MODE_OR:
			bRetval = false;
			break;
		case MODE_NAND:
			bRetval = true;
			break;
		case MODE_XOR:
			bRetval = false;
			break;
		default:
			ASSERT( false );
			break;
		}

	// Start processing with the beginning of this sub-filter.
	// 
	struct resbuf* pCurrent = (struct resbuf *)filter;
	while( (pCurrent != NULL) &&
		   (pCurrent != pExpressionEnd) )
		{
		bool bLatestResult = processFilterNode( pCurrent, elp, flp, mode, &pCurrent );

		// Now based on the mode, we can bail out whenever we have resolved the expression
		// We already saved the place for the callee to carry on processing above
		//
		switch( mode )
			{
			case MODE_AND:
				if (!bLatestResult )
					{
                    return false;
					}
				break;
			case MODE_OR:
				if ( bLatestResult )
					{
					return true;
					}
				break;
			case MODE_NAND:
				if ( bLatestResult )
					{
					return false;
					}
				break;
			case MODE_XOR:
				if ( bLatestResult && bRetval )
					{
					return false;
					}
				if ( bLatestResult )
					{
					bRetval = true;
					}
				break;
			}
		}

	return bRetval;
	}

// ************************************************
// PUBLIC FUNCTION
//
// This is the top-level function for checking if an entity
// matches an ssget "X" filter list
//  It starts off a recursive parsing of the filter list by 
// calling walkFilterList with a ParseDepth of 0 (top-level)
//
bool 
EntityMatchesFilterList( const struct resbuf *filter, db_handitem *elp, db_drawing *flp ) 
	{
	if(filter==NULL || elp==NULL || flp==NULL) 
		{
		return false;
		}
	
	//Modified SAU  14/06/2000   [
	//Reason:(ssget) returns incorrect results 
	//		 with empty Boolean operators
	struct resbuf *pcNext=NULL;
	struct resbuf *pcPrev=NULL;
	
	int ftFlag=0;
	 
	for(pcPrev=pcNext=(struct resbuf *)filter;pcNext!=NULL;pcNext=pcNext->rbnext,ftFlag++){
		
		if((pcNext->restype == -4) && (pcPrev->restype == -4)){
			
			if(strisame("AND>",pcNext->resval.rstring) 
				&& strisame("<AND",pcPrev->resval.rstring))
				return false;
			else if(strisame("<AND",pcPrev->resval.rstring) &&
				(pcNext->rbnext!=NULL) && 
				((pcNext->rbnext)->restype == -4) &&
				strisame("AND>",(pcNext->rbnext)->resval.rstring))
				return false;							
			
			if(strisame("OR>",pcNext->resval.rstring) 
				&& strisame("<OR",pcPrev->resval.rstring))
				return false;
			else if(strisame("<OR",pcPrev->resval.rstring) &&
				(pcNext->rbnext!=NULL) && 
				((pcNext->rbnext)->restype == -4) &&
				strisame("OR>",(pcNext->rbnext)->resval.rstring))
				return false;
			
			if(strisame("NOT>",pcNext->resval.rstring) 
				&& strisame("<NOT",pcPrev->resval.rstring))
				return false;
			else if(strisame("<NOT",pcPrev->resval.rstring) &&
				(pcNext->rbnext!=NULL) && 
				((pcNext->rbnext)->restype == -4) &&
				strisame("NOT>",(pcNext->rbnext)->resval.rstring))
				return false;							
			
			if(strisame("XOR>",pcNext->resval.rstring) 
				&& strisame("<XOR",pcPrev->resval.rstring))
				return false;
			
		}
		if((pcPrev->restype == -4) &&  
			strisame("<XOR",pcPrev->resval.rstring) && 
			(pcNext->rbnext!=NULL)	&& 
			((pcNext->rbnext)->restype == -4) && 
			strisame("XOR>",(pcNext->rbnext)->resval.rstring))
			return false;							
		
		if((pcPrev->restype == -4) &&  
			strisame("<XOR",pcPrev->resval.rstring) && 
			(pcNext->restype != -4) &&
			((pcNext->rbnext)!=NULL)  && 
			((pcNext->rbnext)->restype != -4) && 
			(((pcNext->rbnext)->rbnext)!=NULL)	&& 
			((((pcNext->rbnext)->rbnext)->restype != -4) ||
			!strisame("XOR>",((pcNext->rbnext)->rbnext)->resval.rstring)))
			return false;							
		
		if((pcPrev->restype == -4) && ftFlag &&
			strisame("<NOT",pcPrev->resval.rstring) &&
			(pcNext->rbnext!=NULL) &&	
			(((pcNext->rbnext)->restype != -4) || 
			!strisame("NOT>",(pcNext->rbnext)->resval.rstring)) 
			){
			if(pcNext->restype != -4)
				return false;	 
		}
		if((pcPrev->restype == -4) && (!ftFlag)  && 
			strisame("<NOT",pcPrev->resval.rstring) && 
			((pcNext->rbnext)->rbnext!=NULL) && 
			((((pcNext->rbnext)->rbnext)->restype != -4) || 
			!strisame("NOT>",((pcNext->rbnext)->rbnext)->resval.rstring)) 
			){
			if((pcNext->rbnext)->restype!= -4)
				return false;	 
		}
		
		
		
		pcPrev=pcNext;
	}
    //Modified SAU  14/06/2000   ]

	struct resbuf *pNext;

	

	bool bRet = walkFilterList( filter, 
								elp, 
								flp, 
								MODE_AND, // default is AND items together
								&pNext );

	// This meant the filter list was malformed ...
	// we really should check these things
	//
	if ( pNext != NULL )
		{
		return false;
		}
	return bRet;
	}

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************









 
 
