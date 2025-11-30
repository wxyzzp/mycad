#ifndef __cmdsDimBlockH__
#define __cmdsDimBlockH__

#define DIM_BYLAYER	      "BYLAYER"/*DNT*/
#define DIM_BYBLOCK       "BYBLOCK"/*DNT*/
#define DIM_ADD_TO_BLOCK 0x0010
#define DIM_ADD_ASSOC 0x0020

// EBATECH(CNBR) 2003/3/31 Support lineweight
void	cmd_initialrbcs(struct resbuf *rbecs, struct resbuf *rbwcs, struct resbuf *rbucs, db_drawing *flp, sds_real* pExtrusion = NULL);
void setColor(short *sColor);
int		dwg_addblock(char *szLayer,short sColor,char *szBlockName,short flag70,sds_point pt10,char *szXrefPath,db_drawing *pFlp);
int		dwg_addendblk(db_drawing *pFlp);
int		cmd_dimaddPolyline(char *szLayer, short sColor,short sLweight,short flag66,sds_point pt10,short flag70,sds_real r40,sds_real r41,db_drawing *pFlp,short dragmode,db_handitem **elp);
int		cmd_dimaddVertex(char *szLayer, short sColor,short sLweight,sds_point pt10,sds_real r40,sds_real r41,sds_real r42,db_drawing *pFlp,short dragmode);
int		cmd_dimaddSeqend(char *szLayer, short sColor,short sLweight,db_drawing *pFlp,short dragmode);
int		cmd_dimaddCircle(char *szLayer, short sColor,short sLweight,sds_point pt10,sds_real r40,db_drawing *pFlp,short dragmode);
int		cmd_dimaddarc(char *szLayer, short sColor,short sLweight,sds_point pt10,sds_real r40,sds_real r50,sds_real r51,sds_point extrusion,db_drawing *pFlp,short dragmode);
int		cmd_dimaddmtext(char *szLayer, short sColor,short sLweight,sds_point pt10,sds_real r40,char *szText,sds_real r50,sds_real r41,char *szTextStyle,short flag71,sds_point extrusion,db_drawing *pFlp,short dragmode);
int		cmd_dimaddsolid(char *szLayer, short sColor,short sLweight,sds_point pt10,sds_point pt11,sds_point pt12,sds_point pt13,db_drawing *pFlp,short dragmode,short flags,sds_point init210);

int		cmd_dimaddtext (char *szLayer, short sColor,short sLweight,sds_point pt10,sds_real r40,char *szText,sds_real r50,sds_real r41,sds_real r51,char *szTextStyle,short flag72,short flag73,sds_point pt11,db_drawing *pFlp,short dragmode);


#endif
