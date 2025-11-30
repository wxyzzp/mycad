/*=====================================================================
	sds_xref.cpp 

		Contains sds-functions for work with xrefs.
=====================================================================*/

#include "Icad.h"/*DNT*/
#include "sds.h"
#include "IcadDoc.h"
#include "IcadView.h"
#include "commandqueue.h" /*DNT*/
#include "cmds.h"/*DNT*/
#include "insert.h"/*DNT*/

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif

//** prototypes
void cmd_OnOpenOK(HWND hDlg);

/*********** Auxiliary functions **************/

inline void init(sds_blockTree *pNode) {
	pNode->p_next=pNode->p_contents=NULL;
	pNode->p_hook=NULL;
	pNode->name=pNode->path=NULL;
	pNode->entity_name[0]=pNode->entity_name[1]=pNode->tblrec_name[0]=pNode->tblrec_name[1]=0;	
	pNode->type=0;
	pNode->bitmap=pNode->wmf=NULL;
	pNode->bmpsz=pNode->wmfsz=0;
}


sds_blockTree *scan_drawing(db_drawing *db,const int type_mask){
	sds_blockTree *top=NULL; 
	sds_blockTree *curr=NULL;
	for(db_handitem *hip=db->entnext_noxref(NULL); 	hip!=NULL;	hip=db->entnext_noxref(hip)) // loop for the entities from a drawing database
		if (hip->ret_type()==DB_INSERT) { // hip is a entity which have the record in a block table
			int type;

			(((db_insert*)hip)->ret_bthip())->get_70(&type);//hip->get_70(&curr->type);
			
			db_blocktabrec * p_block_tbl_rec=((db_insert*)hip)->ret_bthip();

			if (type & type_mask/*masking is OK*/) {
				if (curr)	{
					init(curr=curr->p_next=new sds_blockTree);
				}
				else{
					init(top=curr=new sds_blockTree);
				}				
				
				curr->type=type;


				p_block_tbl_rec->get_2(&(curr->name));
				//p_block_tbl_rec->get_1(&(curr->path));

				curr->bitmap=db->ret_imagebufbmp();
				curr->bmpsz	=db->ret_imagebufbmpsz();
				curr->wmf	=db->ret_imagebufwmf();
				curr->wmfsz	=db->ret_imagebufwmfsz();

				curr->entity_name[0]=(long)hip;
				curr->entity_name[1]=curr->tblrec_name[1]=(long)SDS_CURDWG;
				curr->tblrec_name[0]=(long)p_block_tbl_rec;	

			} //if masking ok bracket

			db_drawing *sub_db=p_block_tbl_rec->ret_xrefdp();
			if (sub_db) curr->path=sub_db->ret_pn();
			if (sub_db) curr->p_contents=scan_drawing(sub_db,type_mask);		
		} // of  for( hip ) if  (DB_INSERT)

return top;
}





/*********** SDS export functions ***************/

sds_blockTree *sds_buildBlockTree(int type_mask){
	if (SDS_CURDOC==NULL) return NULL;
	sds_blockTree *pTop=new sds_blockTree;
	init(pTop);
	pTop->type=0xFF; // top-level drawing
	pTop->p_contents=scan_drawing(SDS_CURDOC->m_dbDrawing,type_mask);
	pTop->name="\0"; 
	pTop->bitmap=SDS_CURDOC->m_dbDrawing->ret_imagebufbmp();
	pTop->bmpsz	=SDS_CURDOC->m_dbDrawing->ret_imagebufbmpsz();
	pTop->wmf	=SDS_CURDOC->m_dbDrawing->ret_imagebufwmf();
	pTop->wmfsz	=SDS_CURDOC->m_dbDrawing->ret_imagebufwmfsz();
	pTop->path	=SDS_CURDOC->m_dbDrawing->ret_pn(); //It's drawing file name

return pTop;
}

void sds_relBlockTree(sds_blockTree *pTree){
	if (pTree==NULL) return;
	sds_relBlockTree( pTree->p_next     );
	sds_relBlockTree( pTree->p_contents );
//	delete pTree->p_next;			/*D.G.*/
//	delete pTree->p_contents;		//
	delete pTree;					// Use this line instead of two previous ones for avoiding memory leaks.
}

int rename_xref(char *p_oldName, char* p_newName){
	ASSERT(p_oldName);	ASSERT(p_newName);
    if (!(p_oldName && p_newName))
		return RTERROR; // wrong parameter

	char name[IC_ACADBUF]; // buffer for get_2
	db_handitem *hip;
	for (hip = SDS_CURDWG->tblnext(DB_BLOCKTABREC, 1); hip != NULL; hip = hip->next){
		if (hip->ret_deleted())
			continue;
		int flag;
		hip->get_70(&flag);
		if (!(flag & IC_BLOCK_XREF))
			continue;

		hip->get_2(name,sizeof(name)-1);

		if (0==stricmp(name,p_oldName)){ /* the xref block db item is found */
			hip->set_2(p_newName,SDS_CURDWG);
			return RTNORM;
		}
	}
return RTERROR; // xref isn't found
}

int sds_xref(char action,resbuf* param){
#define SDS_XREF_ACTION_MASK	0x0f
/*=======================================================================
	sds_xref(action,param) 
	actions:
	-----------------
	SDS_XREF_FIND		Inquiry to the user for xref search. 
						On input :
							Can be combined with "SDS_XREF_OVERLAY" flag
							(by default will be request 
								for the attachment file)
							Can be combined with "SDS_XREF_USERDEF" flag.
							In a this case if 'param->restype' is "RTSTR"
							then dialog title will present as 
							'param->resval.rstring' string.
						On exit: 
						In param->resval.rstring will be returned path.
	---------------------------------------------------------------------
	SDS_XREF_ATTACH		
					On input:
						Can be combined with the flags "SDS_XREF_OVERLAY" 
						(In this case the file will be attached as overlay)
						and "SDS_XREF_CROP_PATH" (the xref will be kept 
						with file name only, without full path)					
						
					Remark:
						In this action "param" contains those parameters 
							(for example rotate angle, original point etc.), 
						 which will not be requested from the user.
							The DXF-codes may be next:
						 10 - insertion point,	41,42,43 - x,y,z scales 
						 50 - rotation,			1 - path,	2 - name
	---------------------------------------------------------------------	
	SDS_XREF_INSERT		In this actions "param" contains xref names list
	SDS_XREF_RELOAD					(DXF code 2 or SDS code RTSTR)
	SDS_XREF_UNLOAD			
	SDS_XREF_DETACH
					Remark:
						SDS_XREF_INSERT can be combined  
						with "SDS_XREF_BIND"
	---------------------------------------------------------------------
	SDS_XREF_EDIT_PATH	 
						On input:
							Can be combined with "SDS_XREF_CROP_PATH" flag
							(will be kept the file name only,
								without fully path)
						Remark:
							In this action "param" contains  
							xref name pattern	(DXF code 2) 
							and new path		(DXF code 1)
	---------------------------------------------------------------------
	SDS_XREF_RENAME
						On input:
							For this action "param" contains  
								xref name 			(DXF code 2) 
								and xref new name	(DXF code RTSTR)
========================================================================*/
  SDSAPI_Entry();	// Switch for IntelliCAD main thread etc..
					// In this case it's does restore, in particular, 
					// resource handle for correctly work the macros "MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW)".
  if (SDS_CURDOC==NULL) return RTERROR;

  int mode; // for call cmd_xref_uncouple()
  bool isRetain=true/*DNT*/;	//for case	SDS_XREF_EDIT_PATH

  switch (action & SDS_XREF_ACTION_MASK){
  case SDS_XREF_FIND	: {
		struct resbuf rb;
		if (RTNORM != SDS_getvar(NULL, DB_QFILEDIA, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES)) 
			return RTERROR;

		CString str_title;	/* Title for file select dialog */
		if (action & SDS_XREF_USERDEF && param->restype==RTSTR) /* User defined title */
			str_title=param->resval.rstring;
		else
		
			str_title=action & SDS_XREF_OVERLAY
				?ResourceString(IDC_CMDS3_SELECT_FILE_TO_O_491, "Select File to Overlay")/*  Standard dialog title for overlay  */
				:ResourceString(IDC_CMDS3_SELECT_FILE_TO_A_493, "Select File to Attach"); /* Standard dialog title for attachment */		

		if (rb.resval.rint 
			&&	GetActiveCommandQueue()->IsEmpty() 
			&&	GetMenuQueue()->IsEmpty() 
			&& 	!lsp_cmdhasmore) {					
		dia:	// doing the dialog thing
			static char		*SavePath;
			if (RTNORM != SDS_GetFiled(
					 str_title
					,SavePath
					,ResourceString(IDC_CMDS3_STANDARD_DRAWING_492, "Standard Drawing File|dwg" )
					, 512 + 1024 /*D.Gavrilov*/// 1024 added to tell xref-file from ordinary dwg-file in SDS_GetFiled.
					, param
					, MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW)
					,cmd_OnOpenOK))	return RTCAN;
			if (param->restype==RTSTR){
				return RTNORM;
			}
			else	goto typeit; // When it occurs? 2002/4/30 EBATECH(CNBR) Now Type button available.
		} 
		else {
		typeit:	// doing the command line thing	
		char path[IC_ACADBUF];
		sprintf(path,"\n%s: ", (LPCTSTR)str_title); // EBATECH(CNBR) 2002/4/27 Make command line prompt.
		if (RTNORM!=sds_getstring(
				1
				,path
				,path)) return RTCAN;

			ic_trim(path,"be");
			if (*path=='~') goto dia; // if doing call sds_sendmessage("xref ~") for example
			ic_setext(path,".DWG"/*DNT*/); // adds ".DWG" to the name selected

			param->restype=1; // xref path
			//Bugzilla No. 78104 ; 15-04-2002 
			param->resval.rstring= new char [strlen(path)+1];
			strcpy(param->resval.rstring,path);
		}
		ChangeSlashToBackslashAndRemovePipe(param->resval.rstring);
		return RTNORM; 
	}  
  case SDS_XREF_ATTACH	: 
	return xref_add_new(INT2BOOL(action & SDS_XREF_OVERLAY),param,INT2BOOL(action & SDS_XREF_CROP_PATH));
  case SDS_XREF_DETACH	:	mode=IC_UNCOUPLE_DETACH;	goto uncouple;
  case SDS_XREF_INSERT	:	mode=(action & SDS_XREF_BIND)?IC_UNCOUPLE_BIND:IC_UNCOUPLE_INSERT; 
	uncouple:{ // now "param" should be names list
		for (resbuf *p=param; p!=NULL; p=p->rbnext)
			if (2==p->restype || RTSTR==p->restype) {
				cmd_xref_uncouple(p->resval.rstring,mode,true/*displ. mess.*/);
			}
	return RTNORM;
	}
  case SDS_XREF_RELOAD		:	return xref_reload(param);
  case SDS_XREF_UNLOAD		:	return xref_unload(param);
  case SDS_XREF_EDIT_PATH	:{
	  char *name,*path;
	  name=path=NULL;
	  for (resbuf *prb=param; prb!=NULL; prb=prb->rbnext){
		  if (prb->restype==1) path=prb->resval.rstring;
		  if (prb->restype==2) name=prb->resval.rstring;
	  }
	  if (name && path) return cmd_xref_editpath(name,path,INT2BOOL(action & SDS_XREF_CROP_PATH),TRUE/*single xref*/);
	}
  case SDS_XREF_RENAME: 
		/*	ON INPUT:
				dxf code 2		define old xref name
				dxf code RTSTR	define new xref name
			ON OUTPUT:
				none									  */
		char *p_oldName=NULL,*p_newName=NULL;
		/* parse input chain */
		for (resbuf *p=param; p!=NULL; p=p->rbnext){
			if (2==p->restype)
				p_oldName=p->resval.rstring;
			if (RTSTR==p->restype) 
				p_newName=p->resval.rstring;
		} 
		return rename_xref(p_oldName,p_newName);
  }// switch action
return RTERROR;  
}

