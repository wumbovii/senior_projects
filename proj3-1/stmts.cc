/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* stmts.cc: AST subclasses related to statements and modules. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

/*****   BLOCK   *****/

/** Statements forming a declarative region */
class Block_AST : public AST_Tree {
protected:

    NODE_CONSTRUCTORS (Block_AST, AST_Tree);

};

NODE_FACTORY (Block_AST, BLOCK);



/*****   BREAK   *****/

/** break */
class Break_AST : public AST_Tree {
protected:

	void codeGen (ostream& out, Decl* decl) {
		out << "break;\n";
	}

    NODE_CONSTRUCTORS (Break_AST, AST_Tree);

};

NODE_FACTORY (Break_AST, BREAK);



/*****   CLASS   *****/

/** class ID(...): */
class Class_AST : public AST_Tree {
protected:
	void codeGen (ostream& out, Decl* decl) {
		classMode = true;
		out << "class " << child(0)->as_string() << " : public PyClass {\n";
		out << "public:\n";

		indent(2);
        while (true) {
        	/* Generate all subfunctions */
        	AST_Ptr next = popQueue();
        	if (next != NULL) {
        		next->codeGen(out, decl);
        	} else break;
        }

        dedent();
		out << "};\n";
		classMode = false;
	}

    Decl* getDecl () {
        return child (0)->getDecl ();
    }

    AST_Ptr rewrite_lambdas (AST_Ptr, int& id, int) {
        AST_Ptr newDefs = AST::make_list ();
        child (2)->rewrite_lambdas (newDefs, id, METHOD);
        child (2)->insert (0, newDefs);
        return this;
    }

    void collectDecls (Decl* enclosing) {
        AST_Ptr id = child (0);
        string name = id->as_string ();

        if (undefinable (name)) {
            Error (this, "attempt to redefine %s", name.c_str ());
            return;
        }

        Decl* old = enclosing->get_environ ()->find_immediate (name);
        if (old != NULL) {
            Error (this, "attempt to redefine %s as class", name.c_str ());
        }

        Decl* decl = makeClassDecl (name, enclosing);
        enclosing->add_member (decl);
        id->setDecl (decl);
    }
    
    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        child (2)->doTyping (context, changed, unresolved);
    }

    void resolveSimpleIds (const Environ* env) {
        AST_Ptr id = child (0);
        Decl* me = id->getDecl ();
        if (me == NULL)
            return;
        AST_Ptr body = child (2);
        body->collectDecls (me);
        body->resolveSimpleIds (me->get_environ ());
    }

    NODE_CONSTRUCTORS (Class_AST, AST_Tree);

};

NODE_FACTORY (Class_AST, CLASS);



/*****   CONTINUE   *****/

/** continue */
class Continue_AST : public AST_Tree {
protected:

	void codeGen (ostream& out, Decl* decl) {
		out << "continue;\n";
	}
    NODE_CONSTRUCTORS (Continue_AST, AST_Tree);

};

NODE_FACTORY (Continue_AST, CONTINUE);


/*****   DEF   *****/

/** def ID(...): */
class Def_AST : public AST_Tree {
protected:

	void codeGen (ostream& out, Decl* decl) {
		if (nestedMode) {
			/* This means we this is a nested function, codeGen later. */
			pushQueue(this);
		} else {
			nestedMode = true;

			Decl* me = getDecl();
			Type_Ptr return_type = me->get_type()->returnType();
			Decl_Vector decls = me->get_environ()->get_members();
			string return_name = return_type->get_code_name();

			out << indent() << return_name;
			if (return_name != "void") out << "*";

			out << " " << me->get_name() << "(";

			for (size_t i = 0; i < decls.size(); i++) {
				Decl* d = decls[i];
				if (d->getDeclCategory() == PARAM_DECL) {
					out << d->get_type()->get_code_name() << "* " << d->get_name() << ", ";
				}
			}

			out << "void* parent) {\n";
			indent(2);

			/* Begin function body. */
			out << indent() << getDecl()->get_name() << "_frame* _f = new " << getDecl()->get_name() << "_frame;\n";

			if (decl) {
				/* This means we are in a nested function, thus we have a parent pointer. */
				out << indent() << "_f->parent = (" << me->get_container()->get_name() << "_frame*) parent;\n";
			}

			for (size_t i = 0; i < decls.size(); i++) {
				Decl* d = decls[i];
				if (d->getDeclCategory() == PARAM_DECL) {
					out << indent() << "_f->" << d->get_name() << " = " << d->get_name() << ";\n";
				}
			}

			child(3)->codeGen(out, me);

			dedent();
			/* End function body. */
			out << indent() << "};\n\n";
		}
	}

    AST_Ptr rewrite_lambdas (AST_Ptr, int& id, int) {
        AST_Ptr newDefs = AST::make_list ();
        child (3)->rewrite_lambdas (newDefs, id, DEF);
        child (3)->insert (0, newDefs);
        return this;
    }

    void collectDecls (Decl* enclosing) {
        AST_Ptr id = child (0);
        string name = id->as_string ();

        if (undefinable (name)) {
            Error (this, "attempt to redefine %s", name.c_str ());
            return;
        }

        Decl* old = enclosing->get_environ ()->find_immediate (name);
        if (old != NULL) {
            Error (this, "attempt to redefine %s with def", name.c_str ());
        }

        Decl* me = enclosing->add_def_decl (id);
        id->setDecl (me);
    }
    
    void resolveSimpleIds (const Environ* env) {
        AST_Ptr id = child (0);
        AST_Ptr returnType = child (2);
           
        Decl* me = id->getDecl ();
        if (me == NULL)
            return;
        AST_Ptr body = child (3);
        processParams (me, env);
        body->collectDecls (me);
        returnType->resolveSimpleIds (env);
        me->set_type (computeDefType ());
        body->resolveSimpleIds (me->get_environ ());        
    }

    Decl* getDecl () {
        return child (0)->getDecl ();
    }

    void freezeDecls (bool frozen) {
        AST::freezeDecls (frozen);
        Decl* me = getDecl ();
        if (me != NULL)
            me->set_frozen (frozen);
    }

    /** Compute my type from that of my return type and parameter
     *  types. */
    virtual Type_Ptr computeDefType () {
        Type_Ptr returnType;
        if (child (2)->isMissing ())
            returnType = Type::makeVar ();
        else
            returnType = child (2)->asType ();
        AST_Ptr typeParams = consTree (TYPE_LIST);
        AST_Ptr params = child (1);
        for_each_child (p, params) {
            typeParams->append (p->getDecl ()->get_type ());
        } end_for;
        return consTree (FUNCTION_TYPE, returnType, typeParams)->asType ();
    }

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        AST_Tree::doTyping (getDecl (), changed, unresolved);
    }

    NODE_CONSTRUCTORS (Def_AST, AST_Tree);

private:

    void processParams (Decl* me, const Environ* outer_env) {
        AST_Ptr params = child (1);
        const Environ* env = me->get_environ ();
        for_each_child (c, params) {
            c->resolveSimpleTypeIds (outer_env);
            AST_Ptr id = c->getId ();
            if (id->getDecl () != NULL)
                continue;
            string name = id->as_string ();
            Decl* cDecl = makeParamDecl (name, me, c_i_, Type::makeVar ());
            id->setDecl (cDecl);
            if (undefinable (name)) {
                Error (c, "attempt to redefine %s", name.c_str ());
            } else if (env->find_immediate (name) != NULL) {
                Error (c, "parameter name %s previously defined",
                       name.c_str ());
            } else {
                me->add_member (cDecl);
            }
        } end_for;
    }
            
};

NODE_FACTORY (Def_AST, DEF);

/*****   METHOD   *****/

/** def ID(...): */
class Method_AST : public Def_AST {
protected:

    const char* externalName () {
        return "def";
    }

    Type_Ptr computeDefType () {
        Type_Ptr type = Def_AST::computeDefType ();
        Decl* me = getDecl ();
        if (me == NULL)
            return type;
        Type_Ptr classType = me->get_container ()->as_type ();
        if (type->numParams () == 0)
            Error (this, "method with no parameters");
        else if (! classType->unify (type->paramType (0)))
            throw logic_error ("could not set type of method's first parameter");
        return type;
    }

    NODE_CONSTRUCTORS (Method_AST, Def_AST);

};

NODE_FACTORY (Method_AST, METHOD);



/*****   FOR   *****/

/** for T in E: ... */
class For_AST : public AST_Tree {
protected:

	void codeGen (ostream& out, Decl* decl) {
		string var = "__tmp";
		for (int i = 0; i < tempVarCount; i++) {
			var += "_";
		}
		tempVarCount += 1;

		statementMode = true;
		out << "std::vector<PyObject*> " << var << " = (";
		child(1)->codeGen(out, decl);
		out << ")->value();\n";
		out << "for (size_t j = 0; j < " << var << ".size(); j++){\n " <<child(0)->getDecl()->get_type()->get_code_name() << "* ";
		child(0)->codeGen(out, decl);
		out << " = (" << child(0)->getDecl()->get_type()->get_code_name() << "*) " << var << "[j];\n";
		statementMode = false;
		child(2)->codeGen(out, decl);
		child(3)->codeGen(out, decl);

		out << "}\n";
	}

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        AST_Tree::doTyping (context, changed, unresolved);
        Type_Ptr listType = child (1)->getType ();
        if (! _init) {
            if (!listType->resolveBinding () 
                && !listType->unify (Type::makeVar 
                                     (Type::LIST|Type::XRANGE|Type::STR)))
                Error (this, "can only iterate over lists, ranges, and strings");
            _init = changed = true;
        }

        if (listType->resolveBinding ()) {
            Type_Ptr targetType = child (0)->getType ();
            if (_unresolved) {
                _unresolved = false;
                changed = true;
                switch (listType->getAllowed ()) {
                case Type::LIST:
                    if (!listType->typeParam (0)->unify (targetType))
                        Error (this,
                               "for-statement target inconsistent with "
                               "element type");
                    break;
                case Type::STR:
                    if (!listType->unify (targetType))
                        Error (this, "for-statement target must be a string");
                    break;
                case Type::XRANGE:
                    if (!intDecl->as_type ()->unify (targetType))
                        Error (this, "for-statement target must be an int");
                    break;
                default:
                    Error (this, "can only iterate over lists, ranges, and "
                           "strings");
                    break;
                }
            }
        } else
            unresolved = true;
    }    

    void collectDecls (Decl* enclosing) {
        child (0)->addTargetDecls (enclosing);
    }

    void reportUnresolved () {
        if (_unresolved)
            Error (this, "type of source in 'for' is unresolved");
    }

    NODE_CONSTRUCTORS_INIT (For_AST, AST_Tree,
                            _unresolved (true), _init (false));

private:

    bool _unresolved, _init;

};

NODE_FACTORY (For_AST, FOR);



/*****   FORMALS_LIST   *****/

/** Formal parameters */
class Formals_List_AST : public AST_Tree {
protected:

    void resolveSimpleIds (const Environ* env) {
    }        

    NODE_CONSTRUCTORS (Formals_List_AST, AST_Tree);

};

NODE_FACTORY (Formals_List_AST, FORMALS_LIST);



/*****   IF   *****/

/** if E: ... */
class If_AST : public AST_Tree {
protected:
	void codeGen (ostream& out, Decl* decl) {
		statementMode = true;
		out << "if (";
		this->child(0)->codeGen(out, decl);
		out << ") {\n";
		statementMode = false;

		this->child(1)->codeGen(out, decl);
		out << "}\n";
		this->child(2)->codeGen(out, decl);
	}

    NODE_CONSTRUCTORS (If_AST, AST_Tree);

};

NODE_FACTORY (If_AST, IF);



/*****   PRINT   *****/

/** print ... */
class Print_AST : public AST_Tree {
protected:

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        AST_Tree::doTyping (context, changed, unresolved);
        if (! child (0)->isMissing ()
            && ! child (0)->getType ()->unify (fileDecl->as_type ()))
            Error (this, "file argument to print has improper type");
    }

    NODE_CONSTRUCTORS (Print_AST, AST_Tree);

};

NODE_FACTORY (Print_AST, PRINT);

/*****   PRINTLN   *****/

/** A print statement without trailing comma. */
class Println_AST : public Print_AST {
protected:
	void codeGen(ostream& out, Decl* decl) {
		/** TODO: handle file output here. */
		AST_Ptr file_out = this->child(0);

		statementMode = true;
		out << indent();
	    for_each_child (c, this->child(1)) {
	        c->codeGen (out, decl);
	        out << "->print();" << endl;
	        out << indent();
	        out << "printf(\" \");" << endl;
	    } end_for;

	    out << indent();
	    out << "printf(\"\\n\");" << endl;
	    statementMode = false;
	}

    const char* externalName () {
    	return "println";
    }

    NODE_CONSTRUCTORS (Println_AST, Print_AST);

};

NODE_FACTORY (Println_AST, PRINTLN);


/*****   RETURN   *****/

/** return E */
class Return_AST : public AST_Tree {
protected:
	void codeGen (ostream& out, Decl* decl) {
		statementMode = true;
		out << indent() << "return ";
		child(0)->codeGen(out, decl);
		out << ";\n";
		statementMode = false;
	}

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        AST_Tree::doTyping (context, changed, unresolved);
        if (!child (0)->isMissing ()) {
            Type_Ptr returnType = child (0)->getType ();
            if (! returnType->unify (context->get_type ()->returnType ()))
                Error (this, "type of returned value inconsistent");
        }
    }

    NODE_CONSTRUCTORS (Return_AST, AST_Tree);

};

NODE_FACTORY (Return_AST, RETURN);



/*****   STMT_LIST   *****/

/** S1; S2 */
class Stmt_List_AST : public AST_Tree {
protected:

    AST_Ptr flatten () {
        vector<AST_Ptr> kids;
        for_each_child (c, this) {
            kids.push_back (c->flatten ());
        } end_for;
        return make_list (kids.begin (), kids.end ());
    }        

    NODE_CONSTRUCTORS (Stmt_List_AST, AST_Tree);

};

NODE_FACTORY (Stmt_List_AST, STMT_LIST);



/*****   WHILE   *****/

/** while E: ... */
class While_AST : public AST_Tree {
protected:

	void codeGen (ostream& out, Decl* decl) {
		out << "while (";
		child(0)->codeGen(out, decl);
		out << ") {\n";
		indent(2);
		child(1)->codeGen(out, decl);
		child(2)->codeGen(out, decl);
		dedent();
		out << "}\n";
	}

    NODE_CONSTRUCTORS (While_AST, AST_Tree);

};

NODE_FACTORY (While_AST, WHILE);
