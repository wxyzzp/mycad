#ifndef __cmdsInetH__
#define __cmdsInetH__

/*-------------------------------------------------------------------------*//**
Open a URL in the default browser.

@author	Martin Waardenburg (Bricsnet) & Denis Petrov
@return	RTNORM for success
*//*--------------------------------------------------------------------------*/
short cmd_url();
short cmd_browser();
/*-------------------------------------------------------------------------*//**
Upload files to ProjectCenter

@author	Martin Waardenburg (Bricsnet)
@return	RTNORM for success
*//*--------------------------------------------------------------------------*/
short cmd_projectCenter();

#define _SUPPORT_HYPERLINKS_
/*-------------------------------------------------------------------------*//**
Implementation of HYPERLINK command

@author Denis Petrov
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_hyperlink();

/*-------------------------------------------------------------------------*//**
Implementation of -HYPERLINK command

@author Denis Petrov
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_hyperlink_();

/*-------------------------------------------------------------------------*//**
Implementation of HYPERLINKOPTIONS command

@author Denis Petrov
@return RTNORM for success, RTERROR for error
*//*------------------------------------------------------------------------*/
short cmd_hyperlinkoptions();

/*-------------------------------------------------------------------------*//**
Determines whether a hyperlink is relative

@author Denis Petrov
@param pURL => hyperlink string
@return 1 if the hyperlink is relative and 0 else
*//*------------------------------------------------------------------------*/
int cmd_isRelativeHyperlink(const char* pURL);

/*-------------------------------------------------------------------------*//**
Gets hyperlink base to use

@author Denis Petrov
@param pName <= the pointer of buffer to put hyperlink base into 
@return 1 for success, 0 for error
*//*------------------------------------------------------------------------*/
int cmd_getHyperlinkBase(char* pBase);

/*-------------------------------------------------------------------------*//**
Gets hyperlink data of a given entity

@author Denis Petrov
@param entity => SDS name of entity 
@param pURL <= the pointer of buffer to put hyperlink into
@param pDescription <= the pointer of buffer to put hyperlink description into
@param pLocation <= the pointer of buffer to put named location
@return 1 if the entity has hyperlink, 0 else or for error
*//*------------------------------------------------------------------------*/
int cmd_getEntityHL(sds_name entity, char* pURL = NULL, char* pDescription = NULL, char* pLocation = NULL);

/*-------------------------------------------------------------------------*//**
Gets hyperlink data of a given selection set. 
If no common hyperlink exists "Varies" is returned

@author Denis Petrov
@param entities => SDS name of selection set 
@param pURL <= the pointer of buffer to put hyperlink into 
@param pDescription <= the pointer of buffer to put hyperlink description into
@param pLocation <= the pointer of buffer to put named location
@return 1 if the entities has hyperlink, 0 else or for error
*//*------------------------------------------------------------------------*/
int cmd_getEntitiesHL(sds_name entities, char* pURL, char* pDescription, char* pLocation);

/*-------------------------------------------------------------------------*//**
Modifies hyperlink data of a given entity

@author Denis Petrov
@param entity => SDS name of entity 
@param pURL => the pointer of buffer which contains new hyperlink
@param pDescription => the pointer of buffer which contains new description
@param pLocation => the pointer of buffer which contains new named location
@return 1 for success, 0 for error
*//*------------------------------------------------------------------------*/
int cmd_modifyEntityHL(sds_name entity, const char* pURL, const char* pDescription, const char* pLocation);

/*-------------------------------------------------------------------------*//**
Removes hyperlink data from EED of a given entity

@author Denis Petrov
@param entity => SDS name of entity 
@return 1 for success, 0 for error
*//*------------------------------------------------------------------------*/
int cmd_removeEntityHL(sds_name entity);

#endif
