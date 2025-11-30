/*
 * Grammer: P a r s e r  H e a d e r 
 *
 * Generated from: GrammerForDCL.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-1995
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * with AHPCRC, University of Minnesota
 * ANTLR Version 1.33
 */

#ifndef Grammer_h
#define Grammer_h
#include "AParser.h"


// Any definitions that you need in the generated files
#include "stdafx.h"
#include "tile.h"
#include "treenode.h" 
#include "fileinfo.h"  
#include "tree.h"
#include "Main.h"
#include "tileattributes.h"
#include "NonDefinedNode.h"  
#include "exceptionmanager.h"
#include "auditlevel.h"
#include "resource.h"
extern Main g_Main;            //global object used in the application

	
class Grammer : public ANTLRParser {
protected:
	static ANTLRChar *_token_tbl[];
private:
#line 281 "GrammerForDCL.g"

// Parser definitions go here

	public:

		 // Global variables while parsing

		 FileInfo *m_pFileInfo;                // Used to hold the info regarding which file is being parsed
TreeNode *m_pCurrentTreeNode;         // Used to keep track of nodes added during tree node
CString wszErrorInFunc ;              // Used for formatting error messages. 
CString wszErrorInWidget;             // Used for formatting error messages. 
CObList lstNodesWithErrors;           // Used for stacking nodes with redundant assignments(used for hinting in audit level 3).
CObList lstNonContianerNodes;         // Used to contain stack the nodes which cannot have children.

		 void init()
{   
	ANTLRParser::init(m_pFileInfo->get_DCLFileName());
	// Any specific initialization you need
	// (if none, don't override the init() method
}
	static SetWordType err1[16];
	static SetWordType err2[16];
	static SetWordType err3[16];
	static SetWordType setwd1[123];
	static SetWordType err4[16];
	static SetWordType TILES_set[16];
	static SetWordType BASE_BLOCKS_set[16];
	static SetWordType err7[16];
	static SetWordType setwd2[123];
	static SetWordType err8[16];
	static SetWordType err9[16];
	static SetWordType err10[16];
	static SetWordType err11[16];
	static SetWordType ATTRIBUTES_set[16];
	static SetWordType err13[16];
	static SetWordType setwd3[123];
	static SetWordType ATTRIB_VALUES_set[16];
	static SetWordType err15[16];
	static SetWordType err16[16];
	static SetWordType err17[16];
	static SetWordType setwd4[123];
private:
	void zzdflthandlers( int _signal, int *_retsignal );

public:
	Grammer(ANTLRTokenBuffer *input);
	void program(void);
	void includeStatementList(void);
	void includeStatement(void);
	void functionList(void);
	void function(void);
	void subBlockList(void);
	void subBlock(void);
	void specialBlockStatement(void);
	void statementList(void);
	void functionStartStatement(void);
	void referenceStatement(void);
	void assignmentStatement(void);
	void constantValue( CString& wszAttributeName );
	 TreeNode *   colonStatement( int nOccurance );
};

#endif /* Grammer_h */
