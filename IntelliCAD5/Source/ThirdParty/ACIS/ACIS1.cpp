//*********************************************************************
//*  ACIS1.cpp                                                        *
//*  Copyright (C) 1994,1995,1996,1997,1998 by Visio Corporation.     *
//*                                                                   *
//*********************************************************************

//** Includes
#include <stdio.h>
#include <stdlib.h>

#include "logical.h"
#include "acis.hxx"
#include "kernapi/api/api.hxx"
#include "kernapi/api/apimsc.hxx"
#include "kernapi/api/kernapi.hxx"
#include "kernapi/error/api.err"
#include "kerndata/top/body.hxx"
#include "kerndata/top/alltop.hxx"
#include "kerndata/lists/lists.hxx"
#include "kerndata/geometry/getbox.hxx"
#include "kernutil/vector/param.hxx"
#include "kernutil/vector/position.hxx"
#include "kernutil/vector/transf.hxx"
#include "kernutil/vector/unitvec.hxx"
#include "kernutil/vector/matrix.hxx"
#include "kernutil/box/box.hxx"
#include "geomhusk/getowner.hxx"
#include "geomhusk/geom_api.hxx"
#include "sg_husk/api/sgapi.hxx"
#include "faceter/acisintf/af_types.h"
#include "faceter/api/af_api.hxx"

#include "ACIS.h"

#if defined(SDS_NEWDB)
	#define gra_view        gr_view
	#define gra_freeviewll	gr_freeviewll
	#define gra_initview	gr_initview  
	#define gra_rp2pix		gr_rp2pix    
	#define gra_pix2rp		gr_pix2rp    
	#define gra_rp2ucs		gr_rp2ucs    
	#define gra_ucs2rp		gr_ucs2rp 
	#define gra_vect_rp2pix gr_vect_rp2pix  
	#define dwg_filelink	db_drawing    
	#define dwg_entlink		db_handitem  
	#define gra_displayobject gr_displayobject  
	#define gra_getdispobjs gr_getdispobjs
	#define gra_viewvars    gr_viewvars
	#define gra_initselector gr_initselector
	#define gra_freeselector gr_freeselector
#endif

#undef new

//** Global variables
bool ACIS_bInited;

//** Class definition
class ACIS_MESH_MANAGER : public MESH_MANAGER {

private:
	// All nodes are kept in a contiguous array.
	// The array is allocated in announce_counts(..)
	position *mesh_node;
	int num_mesh_nodes;

	// private utility to insert a position into mesh_node[..] with 
	// bounds checking.
	void *addnode(int node_index,
				  ENTITY *entity,
				  const position &node_position);

public:

	ACIS_MESH_MANAGER() { 
		mesh_node = 0;
		num_mesh_nodes = 0;

		// test output
		outfile = stdout; 
	}

	logical need_global_indexed_polygons() {
		return TRUE;
	}

	logical need_precount_of_global_indexed_polygons() {
		return TRUE;
	}

	virtual void announce_counts(
		int npoly,		// Number of polygons to follow.
		int nnode,		// Number of nodes to follow.
		int npolynode	// Number of nodes when counted each time
						// they are used by a polygon.
	);

	void begin_global_mesh_output( ENTITY*);

	void end_global_mesh_output( ENTITY*);

	virtual void *announce_global_node(
		int node_index,
		VERTEX *vertex,
		const position &node_position);

	virtual void *announce_global_node(
		int node_index,
		EDGE *edge,
		const position &node_position,
		double t);

	virtual void *announce_global_node(
		int node_index,
		FACE *face,
		const position &node_position,
		const par_pos &uv);

	virtual void start_indexed_polygon(
		int ipoly,		// 0-based polygon index
		int npolynode	// Number of nodes around the polygon
	);

	virtual void announce_indexed_polynode(
		int ipoly,	// 0-based polygon index. This is the
					// same as the immediately preceding call to
					// start_indexed_polygon().
		int i,		// 0-based counter within the polygon.  This
					// increments sequentially on successive calls.
		void *pnode	// Node identifer as previously received from
					// announce_indexed_node
	);

	virtual void end_indexed_polygon(
		int ipoly	// 0-based polygon index.  This matches the
					// immediately preceding call to 
					// start_indexed_polygon(..)
					// and the (multiple) calls to 
					// announce_indexed_polynode(..)
	);

	virtual void *null_node_id() {
		return (void *) -1;
	}

	// test stuff to add it to a file
	FILE *outfile;

	void set_output_file( FILE *fp = stdout) {
		outfile = fp;
	}

	void flush_file() {
		fflush( outfile);
	}

} *ACIS_mesh_manager;


struct gra_displayobject *ACIS_begDO=NULL,*ACIS_endDO=NULL;
struct dwg_entlink       *ACIS_elpDO=NULL;
struct dwg_filelink      *ACIS_flpDO;
struct gra_view          *ACIS_viewpDO;
int                       ACIS_modeDO;
int						  ACIS_CurVertex;

//** Functions
void ACIS_MESH_MANAGER::begin_global_mesh_output( ENTITY *)
{
}

void ACIS_MESH_MANAGER::end_global_mesh_output( ENTITY *)
{
	delete [] mesh_node;
	mesh_node = NULL;
	num_mesh_nodes = 0;
}

void ACIS_MESH_MANAGER::announce_counts(
	int npolys,
	int nnode,
	int npolynodes)
{
	num_mesh_nodes = nnode;
	mesh_node = new position[nnode];
}

void* ACIS_MESH_MANAGER::announce_global_node(
	int node_index,
	VERTEX *vertex,
	const position &node_position) 
{ 
	return addnode(node_index, (ENTITY *)vertex, node_position);
} 

void* ACIS_MESH_MANAGER::announce_global_node(
	int node_index, 
	EDGE *edge, 
	const position &node_position,
	double)
{ 
	return addnode(node_index, (ENTITY *)edge, node_position);
}

void* ACIS_MESH_MANAGER::announce_global_node(
	int node_index,
	FACE *face, 
	const position &node_position,
	const par_pos &)
{
	return addnode(node_index, (ENTITY *)face, node_position);
}

void* ACIS_MESH_MANAGER::addnode(
	int node_index,
	ENTITY *entity,
	const position &node_position)
{

	if (node_index < num_mesh_nodes && 0 <= node_index) {
		// first, transform the node
		position transf_position = node_position;

		if (entity)	{
			transf body_transf = get_owner_transf(entity);
			transf_position *= body_transf;
		}

		// now, record the node position
		mesh_node[node_index] = transf_position;
	} else {
		fprintf(stderr," Unexpected node id %d",node_index);
	}
	return (void*)node_index;
}

void ACIS_MESH_MANAGER::start_indexed_polygon( 
	int ipoly, 
	int npolynode)
{
	ACIS_CurVertex=0;
}


void ACIS_MESH_MANAGER::announce_indexed_polynode( 
	int ipoly,
	int ipnode,
	void *idptr)
{
	int node_index = (int)idptr;
	struct dwg_3dfaparlink *par=(struct dwg_3dfaparlink *)ACIS_elpDO->spec;

	if(ipnode<0 || ipnode>3) return;

	par->pt[ipnode][0]=mesh_node[node_index].x();
	par->pt[ipnode][1]=mesh_node[node_index].y();
	par->pt[ipnode][2]=mesh_node[node_index].z();

	if(ipnode==2) { // Just incase we get a triangle instead of a quad.
		par->pt[3][0]=mesh_node[node_index].x();
		par->pt[3][1]=mesh_node[node_index].y();
		par->pt[3][2]=mesh_node[node_index].z();
	}
}

void ACIS_MESH_MANAGER::end_indexed_polygon( int)
{
	struct gra_displayobject *beg=NULL,*end=NULL;
	struct dwg_entlink *elp=ACIS_elpDO;
	if(gra_getdispobjs(&ACIS_elpDO,&beg,&end,ACIS_flpDO,ACIS_viewpDO,ACIS_modeDO)) return;
	ACIS_elpDO=elp;

	if(ACIS_begDO==NULL) {
		ACIS_begDO=beg;
		ACIS_endDO=end;
	} else {
		ACIS_endDO->next=beg;
		ACIS_endDO=end;
	}
	return;
}



int ACIS_Init() {

	if(ACIS_bInited) return(RTNORM);

	// Initialization of the modeller must be done before any other calls.
	outcome result = api_start_modeller( 0 );
	if (!result.ok()) return(RTERROR);

	// Initialize the faceter
	result = api_initialise_faceter( NULL );
	if (!result.ok()) return(RTERROR);

	// Set up a refinement
	REFINEMENT *body_refinement;
	result = api_create_refinement(body_refinement);
	if (!result.ok()) return(RTERROR);

	// Set any refinement properties here
	body_refinement->set_triang_mode(AF_TRIANG_NONE);
	body_refinement->set_grid_mode(AF_GRID_TO_EDGES);
	body_refinement->set_adjust_mode(AF_ADJUST_NONE);
	body_refinement->set_normal_tol(16);

	// Set the refinement as default in the faceter.
	result = api_set_default_refinement(body_refinement);
	if (!result.ok()) return(RTERROR);

	// Create a mesh manager and set it
	ACIS_mesh_manager = new ACIS_MESH_MANAGER();
	result = api_set_mesh_manager(ACIS_mesh_manager);
	if (!result.ok()) return(RTERROR);

	ACIS_bInited=TRUE;
	return(RTNORM);
}

int ACIS_Exit() {

	if(ACIS_mesh_manager) delete ACIS_mesh_manager;

	outcome	result = api_terminate_faceter();
	if (!result.ok()) return(RTERROR);

	result = api_stop_modeller();
	if (!result.ok()) return(RTERROR);

	return(RTNORM);
}

int ACIS_BoolObjects(struct dwg_entlink *Blank, struct dwg_entlink *Tool,char Mode) {

	if(RTNORM!=ACIS_Init()) return(RTERROR);
	
	if(!Blank || !Tool) return(RTERROR);

	BODY *eBlank=(class BODY *)Blank->spec;
	BODY *eTool=(class BODY *)Tool->spec;

	outcome result;

	switch(toupper(Mode)) {
		case 'S':
			result = api_boolean(eTool, eBlank, SUBTRACTION);
			break;
		case 'U':
			result = api_boolean(eTool, eBlank, UNION);
			break;
		case 'I':
			result = api_boolean(eTool, eBlank, INTERSECTION);
			break;
		default:
			return(RTERROR);
	}

	if (!result.ok()) return RTERROR;

	// Because the ACIS object is now not valid!!
	Tool->spec=NULL;

	return(RTNORM);
}

int ACIS_CopyObjects(struct dwg_entlink *Source, struct dwg_entlink *Dest) {

	if(RTNORM!=ACIS_Init()) return(RTERROR);
	
	if(!Source || !Dest) return(RTERROR);

	BODY *eSource=(class BODY *)Source->spec;
	BODY *eDest=NULL;

	outcome result = api_copy_body(eSource, eDest);

	if (!result.ok()) return RTERROR;

	Dest->spec=eDest;

	return(RTNORM);
}

int ACIS_XformObject(struct dwg_entlink *elp, sds_matrix genmat) {

	if(RTNORM!=ACIS_Init()) return(RTERROR);
	
	if(!elp) return(RTERROR);

	ENTITY *entity=(class ENTITY *)elp->spec;

	// first, create our affine matrix
	vector x(genmat[0][0],genmat[0][1],genmat[0][2]);
	vector y(genmat[1][0],genmat[1][1],genmat[1][2]);
	vector z(genmat[2][0],genmat[2][1],genmat[2][2]);
	matrix affine_matrix(x, y, z);

	// now, create our translation vector
	vector translation(genmat[0][3],
					   genmat[1][3],
					   genmat[2][3]);

	// check to see if we have rotation
	logical rotation_flag = ((affine_matrix.element(0, 0) != 1.0) ||
							 (affine_matrix.element(1, 1) != 1.0) ||
							 (affine_matrix.element(2, 2) != 1.0));


	// get our scaling
	double scale_factor = genmat[3][3];

	// use the affine matrix and translation vector to
	// create an ACIS transf
	transf tform = restore_transf(affine_matrix,	// rotate/scale
								  translation,		// translate
								  scale_factor,		// scaling factor
								  rotation_flag,	// rotation flag
								  FALSE,			// reflection flag
								  FALSE);			// shear flag


	// now tranform the entity
	outcome result;
	result = api_transform_entity(entity, tform);
	if (!result.ok()) return RTERROR;

	return RTNORM;
}

int ACIS_getdispobjs(struct dwg_entlink       **p_elpp,
					 struct gra_displayobject **p_begdopp,
                     struct gra_displayobject **p_enddopp,
                     struct dwg_filelink       *p_flp,
                     struct gra_view           *p_viewp,
                     short                      p_mode) {

	ACIS_flpDO=p_flp;
	ACIS_viewpDO=p_viewp;
	ACIS_modeDO=p_mode;

	int ret=0;
	sds_point pt;
	pt[0]=pt[1]=pt[2]=0.0;

	ACIS_begDO=NULL; ACIS_endDO=NULL;
	dwg_build3dface(NULL,NULL,0.0,-1,1.0,0,0,0L,pt,pt,pt,pt,0,NULL,p_flp,&ACIS_elpDO,1);

	// Facet the body.
	BODY* b1=(BODY *)(*p_elpp)->spec;

	if ((b1)->identity() == BODY_TYPE) {
		outcome result = api_facet_entity(b1);
		if (!result.ok()) return(RTERROR);
	}

	*p_begdopp=ACIS_begDO;
	*p_enddopp=ACIS_endDO;
	if(ACIS_elpDO) dwg_freeentll(ACIS_elpDO); ACIS_elpDO=NULL;

	return(ret);
}

int ACIS_FreeObject(struct dwg_entlink *elp) {

	if(RTNORM!=ACIS_Init()) return(RTERROR);
	
	if(!elp) return(RTERROR);

	api_del_entity((class ENTITY *)elp->spec);

	return(RTNORM);
}

int ACIS_GetSAT(struct dwg_entlink *elp,char **SatBack) {

	if(RTNORM!=ACIS_Init()) return(RTERROR);
	
	if(!elp) return(RTERROR);

	/*
	// Open the output file
	FILE* fp = fopen("example.sat", "w");
	if (!fp) {
		printf("unable to open output file");
		exit(1);
	}
	*/

	// Create an entity list and add the body to it
	//ENTITY_LIST save_list;
	//save_list.add((class ENTITY *)elp->spec); 

	/*
	// Save the entity list to the file
	result = api_save_entity_list(fp, TRUE, save_list);
	if (!result.ok()) {
		printf("error saving solid\n");
		exit(1);
	}

	fclose(fp);
	*/
			   
	*SatBack="ACIS Model description";

	return(RTNORM);
}

int ACIS_CreateBlock(sds_point pt1,sds_point pt2, sds_real high,struct dwg_filelink *flp) {	

	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a block
	BODY* b1;
	position	p1(pt1[0], pt1[1], pt1[2]),
				p2(pt2[0], pt2[1], pt2[2]+high);
	outcome result = api_solid_block( p1, p2, b1 );
	if (!result.ok()) return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateSphere(sds_point Cent,double Rad,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a block
	BODY* b1;
	position	p1(Cent[0], Cent[1], Cent[2]);
	outcome result = api_solid_sphere(p1, Rad, b1);
	if (!result.ok()) return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateTorus(sds_point Cent,double Tordia, double Tubdia,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a torus
	BODY* b1;
	position	p1(Cent[0], Cent[1], Cent[2]);
	outcome result = api_solid_torus(p1,(Tordia/2.0)+(Tubdia/2.0),(Tordia/2.0)-(Tubdia/2.0),b1);
	if (!result.ok()) return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateWedge(sds_point pt1,sds_point pt2,sds_real high,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a wedge
	BODY* b1;
	//position	p1(Cent[0], Cent[1], Cent[2]);
	//???????outcome result = api_solid_sphere(p1, Rad, b1);
	//if (!result.ok()) return(RTERROR);

	return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateCone(sds_point Cent,sds_real bdia,sds_real tdia,sds_real high,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a Cone
	BODY* b1;
	position	p1(Cent[0], Cent[1], Cent[2]);
	vector		z(0, 0, high);
    double major_radius, minor_radius, top_radius;

    major_radius = minor_radius = bdia / 2.0;
    top_radius = tdia / 2.0;
	outcome result = api_solid_cylinder_cone(p1, p1 + z, major_radius, minor_radius, top_radius, NULL, b1);
	if (!result.ok()) return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateDish(sds_point Cent,sds_real Rad,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a Dish
	BODY* b1=NULL;
	FACE *face;
	position	 p1(Cent[0], Cent[1], Cent[2]);
	unit_vector	 lat(Cent[0], Cent[1]+Rad, Cent[2]);
	unit_vector	 lon(Cent[0], Cent[1], Cent[2]+Rad);

	outcome result;
	result = api_make_spface(p1, Rad, lat, lon, 0, 2.0*IC_PI, 0, IC_PI, face);
	if (!result.ok()) return(RTERROR);

	result = api_mk_by_faces(NULL,1,&face,b1);
	if (!result.ok()) return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateDome(sds_point Cent,sds_real Rad,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a Dome
	BODY* b1=NULL;
	FACE *face;
	position	 p1(Cent[0], Cent[1], Cent[2]);
	unit_vector	 lat(Cent[0], Cent[1]+Rad, Cent[2]);
	unit_vector	 lon(Cent[0], Cent[1], Cent[2]-Rad);

	outcome result;
	result = api_make_spface(p1, Rad, lat, lon, 0, 2.0*IC_PI, 0, IC_PI, face);
	if (!result.ok()) return(RTERROR);

	result = api_mk_by_faces(NULL,1,&face,b1);
	if (!result.ok()) return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreatePyramid(sds_point bpt1,sds_point bpt2,sds_point bpt3,sds_point bpt4,sds_point tpt1,sds_point tpt2,sds_point tpt3,sds_point tpt4,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a Pyramid
	BODY* b1;
	//position	p1(Cent[0], Cent[1], Cent[2]);
	//???????outcome result = api_make_pyramid(p1, Rad, b1);
	//if (!result.ok()) return(RTERROR);
	return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}

int ACIS_CreateMesh(sds_point pt1,sds_point pt2,sds_point pt3,sds_point pt4,int msize,int nsize,struct dwg_filelink *flp) {	
	
	if(RTNORM!=ACIS_Init()) return(RTERROR);

	// Create a Pyramid
	BODY* b1;
	//position	p1(Cent[0], Cent[1], Cent[2]);
	//???????outcome result = api_solid_sphere(p1, Rad, b1);
	//if (!result.ok()) return(RTERROR);
	return(RTERROR);

    return(dwg_CreateAcisObj((void *)b1,0,flp));
}



