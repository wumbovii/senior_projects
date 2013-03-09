/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* modules.cc: Describes programs or libraries.  Contains top-level routines. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include <queue>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

const Environ* outer_environ;
const Environ* builtin_environ;

/*****   MODULE    *****/

static const char* HEADER[] = {
    "#include \"runtime.h\"",
    "#include \"stdio.h\"",
    ""
};

/** A module, representing a complete source file. */
class Module_AST : public AST_Tree {
protected:

    int lineNumber () {
	return 0;
    }

    AST_Ptr flatten () {
        for_each_child_var (c, this) {
            c = c->flatten ();
        } end_for;
        return this;
    }

    Decl* getDecl () {
        return _me;
    }

    AST_Ptr rewrite_lambdas (AST_Ptr, int&, int) {
        AST_Ptr newDefs = AST::make_list ();
        int counter;
        counter = 1;
        AST::rewrite_lambdas (newDefs, counter, DEF);
        insert (0, newDefs);
        return this;
    }

    /** Top-level semantic processing for the program. */
    AST_Ptr semantics () {
        initBuiltin ();
        _me = makeModuleDecl ("__main__", builtin_environ);
        flatten();
        int dummy;
        rewrite_lambdas (NULL, dummy, 0);

        outer_environ = _me->get_environ ();
        for_each_child_var (c, this) {
            c->collectDecls (_me);
            c->resolveSimpleIds (outer_environ);
            c = c->resolveStaticSelections (outer_environ);
            c = c->resolveAllocators (outer_environ);
            bool unresolved, changed;
            do {
                changed = unresolved = false;
                c->doTyping (_me, changed, unresolved);
            } while (unresolved && changed);
            if (unresolved)
                c->reportUnresolved ();
            c = c->rewriteMethodCalls ();
            c->checkNoBoundMethodValues (false);
            c->freezeDecls (false);
        } end_for;
        return this;
    }

    void codeGen (ostream& out, Decl*) {
    	/** Step 1: Output the header. */
        for (int i = 0; HEADER[i] != NULL; i += 1)
            cout << HEADER[i] << endl;


    	/** Step 2: Generate frames for all functions. */
    	Decl_Vector decls = outer_environ->get_members();

    	for (size_t i = 0; i < decls.size(); i++) {
    		decls[i]->genFrames(out);
    	}

        while (true) {
        	/* Generate frames for all nested functions. */
        	Decl* next = popFrameQueue();
        	if (next != NULL) {
        		next->genFrames(out);
        	} else break;
        }


    	/** Step 3: Generate forward declarations of global variables. */
    	for (size_t i = 0; i < decls.size(); i++) {
    		decls[i]->genForwards(out);
    	}
    	out << "\n";


    	/** Step 4: Generate code for functions and classes. */
		for (int i = 0; i < (int) arity(); i++) {
			Decl* d = child(i)->getDecl();
			if (d) {
				DeclCategory category = d->getDeclCategory();
				if (category == CLASS_DECL || category == FUNC_DECL) {
					child(i)->codeGen(out, NULL);
				}
			}
		}

		/** Since we need to break out all nested function definitions, we use a queue of code
         * that still needs to be parsed after we construct the main method. */
		nestedMode = false;

        while (true) {
        	AST_Ptr next = popQueue();
        	if (next != NULL) {
        		next->codeGen(out, _me);
        	} else break;
        }


    	/** Step 5: Generate main. */
    	declareMode = false;
        out << "int main(int argc, char* argv[]) {" << endl;
        indent(2);

		for (int i = 0; i < (int) arity(); i++) {
			Decl* d = child(i)->getDecl();
			if (d && (d->getDeclCategory() == CLASS_DECL || d->getDeclCategory() == FUNC_DECL)) continue;

			child(i)->codeGen(out, NULL);
		}

		out << indent() << "return 0;\n";
		out << "}" << endl;
    }

    NODE_CONSTRUCTORS_INIT (Module_AST, AST_Tree, _me (NULL));

private:

    Decl* _me;

};

NODE_FACTORY (Module_AST, MODULE);

