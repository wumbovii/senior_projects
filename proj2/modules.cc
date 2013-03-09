/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* modules.cc: Describes programs or libraries.  Contains top-level routines. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"
#include <set>

using namespace std;

/*****   MODULE    *****/


//current decl number
int decl_ind_tracker = 0;
//head decl
Decl* decl_head;
//current container
Decl* current_container;

//set of all declarations: index in vector is the declaration index
std::vector<Decl*> decl_set;

//set of decl names found so far in global level
std::vector<Decl*> global_decls_found;

extern Decl* makeModuleDecl (const string& name);
extern void declareBuiltIns(Decl* builtin_module);

/** A module, representing a complete source file. */
class Module_AST : public AST_Tree {
protected:
    int lineNumber () {
    	return 0;
    }

    /** Top-level semantic processing for the program. */
    AST_Ptr semantics () {
    	vector<AST_Ptr> additions;
    	this->rewriteLambda(this, true, &additions);

    	/** Uncomment when we finish declare & decorate */
    	/*initialize top level main and builtin module*/
    	decl_set.push_back(makeModuleDecl("__main__"));
    	decl_set.push_back(makeModuleDecl("__builtin__"));

    	Decl* decl_tree = decl_set[0];
    	//set the module level decl container to null


    	Decl* builtin_module = decl_set[1];
    	builtin_module->set_index(1);

    	//declare builtins
    	declareBuiltIns(builtin_module);

    	//declare AST
    	this->declare(this, decl_tree, IS_NOT_TARGET);

    	//debug print
    	int set_size = decl_set.size();
    	cout << "\ndecl_set contents: \n\n";
    	for (int i = 0; i < set_size; i++) {

    		//get dast type as string
    		string dast_type;
    		int dt = decl_set[i]->get_dast_type();
    		switch (dt) {
    		case LOCALD: {
    			dast_type = "LOCAL_VAR";
    		} break;
    		case GLOBALD: {
    			dast_type = "GLOBAL_VAR";
    		} break;
    		case PARAMD: {
    			dast_type = "PARAM_VAR";
    		} break;
    		case CONSTD: {
    			dast_type = "CONST_VAR";
    		} break;
    		case INSTANCED: {
    			dast_type = "INSTANCE_VAR";
    		} break;
    		case CLASSD: {
    			dast_type = "CLASS_DEC";
    		} break;
    		case FUNCD: {
    			dast_type = "FUNC_DEC";
    		} break;
    		case MODULED: {
    			dast_type = "MODULE_DEC";
    		} break;
    		}
    		if (i == 0 || i == 1) {
    			cout << decl_set[i]->get_name() << "\t" << dast_type << "\t-> " << decl_set[i]->get_index() << "\n";
    		} else {
    			cout << decl_set[i]->get_name() << "\t\t" << dast_type << "\t-> " << decl_set[i]->get_index() << "\n";
    		}
    	}
    	cout << "\nend decl_set contents\n\n";
    	//this->decorate(this, decl_tree, global_decls_found, IS_NOT_TARGET);
    	this->decorate(this, decl_set[0], NULL, 0);
    	output_decls(cout, decl_set);

    	//this->rewriteCalls(this);



    	cout << " --- BEGIN TYPE INFERENCE --- \n";
    	this->typeInference(this);

        return this;
    }

    NODE_CONSTRUCTORS (Module_AST, AST_Tree);
};

NODE_FACTORY (Module_AST, MODULE);

