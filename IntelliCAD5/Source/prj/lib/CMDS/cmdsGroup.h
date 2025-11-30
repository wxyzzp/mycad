#ifndef __cmdsGroupH__
#define __cmdsGroupH__

/*-------------------------------------------------------------------------*//**
@author Denis Petrov

@description functions to work with GROUPs
*//*------------------------------------------------------------------------*/
short cmd_group();
short cmd_group_();

int cmd_listGroups();
int cmd_getSSFromGroup(sds_name ss, sds_name sdsGroupName);
int cmd_removeSSFromGroup(sds_name ss, sds_name sdsGroupName);
int cmd_addSSToGroup(sds_name ss, sds_name sdsGroupName);
int cmd_findGroup(const char* pName, sds_name sdsGroupName = NULL);
int cmd_findGroup(sds_name entity, sds_name ssGroups);
int cmd_createGroup(const char* pName, const char* pDescription, int selectable, int unnamed, sds_name sdsGroupName);
int cmd_modifyGroup(const char* pNewName, const char* pNewDescription, int selectable, sds_name sdsGroupName);
int cmd_reverseGroupOrder(sds_name sdsGroupName);
int cmd_reorderGroupEntities(sds_name sdsGroupName, int oldPos, int newPos, int count);
int cmd_reverseSSOrder(sds_name ssOld, sds_name ssNew);
int cmd_redrawSS(sds_name ss, int mode);

#endif
