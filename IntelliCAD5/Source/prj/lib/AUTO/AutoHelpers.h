/* INTELLICAD\PRJ\LIB\AUTO\AUTOHELPERS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Helper functions used by automation
 * 
 */ 


HRESULT ConvertAutoSelectionSetToSDSsset (IIcadSelectionSet *SelectionSet, sds_name sset, CIcadDoc *pDoc);
HRESULT ConvertSDSssetToAutoSelectionSet (sds_name sset, IIcadSelectionSet **SelectionSet, CIcadDoc *pDoc);
void ConvertSdsMatrixToIIcadMatrix (sds_matrix sdsmatrix, IIcadMatrix *Matrix);
void ConvertIIcadMatrixToSdsMatrix (sds_matrix sdsmatrix, IIcadMatrix *Matrix);
HRESULT CreateEntityWrapper (CIcadDoc *pDoc, IDispatch *pOwner, db_handitem* pDbHandItem, IIcadEntity **ppEntity);
BOOL UnsupportedEntity (db_handitem *pDbHandItem);
BOOL NotACountableEntity (db_handitem *pDbHandItem);
BOOL DeletedEntity (db_handitem *pDbHandItem);
BOOL PaperSpaceEntity (db_handitem *pDbHandItem);
BOOL ModelSpaceEntity (db_handitem *pDbHandItem);
BOOL UnimplementedEntity (db_handitem *pDbHandItem);
db_handitem *CopyHip (db_handitem *hip);
db_handitem *CopyComplexHip (db_handitem *hip);
bool AddToDrawing (CIcadDoc *pDoc, db_handitem *pDbHandItem);

int ic_pt_equal(sds_point p1, sds_point p2) ;
BOOL FileExists (char *path);


