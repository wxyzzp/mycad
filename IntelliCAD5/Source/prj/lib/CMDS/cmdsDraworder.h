#ifndef __cmdsDraworderH__
#define __cmdsDraworderH__

/*-------------------------------------------------------------------------*//**
Implementation of DRAWORDER command.

@author Denis Petrov
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_draworder();

/*-------------------------------------------------------------------------*//**
Returns SDS name of SORTENTSTABLE object for given block.

@author Denis Petrov
@param sdsTableName <= name of SORTENTSTABLE object
@param ownerBlock => handle of owner block for SORTENTSTABLE object 
@param bCreate => true if SORTENTSTABLE object should be created when not exist 
@param pTableList <= return list of groups 5 and 331 for newly created SORTENTSTABLE object 
@return 1 if operation was successful, 0 else or for error
*//*------------------------------------------------------------------------*/
int cmd_getSortEntsTable(sds_name sdsTableName, 
						 /*const*/ db_objhandle& ownerBlock, 
						 bool bCreate = false, 
						 struct resbuf** pTableList = NULL);

#endif