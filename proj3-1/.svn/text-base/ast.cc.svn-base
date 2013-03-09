/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* ast.cc: Basic AST subclasses. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

/* Definitions of methods in base class AST. */

int AST::current_mark = 0;

AST::AST ()
    : _mark (0), _erroneous (false)
{
}

bool
AST::mark ()
{
    if (_mark == current_mark)
        return false;
    _mark = current_mark;
    return true;
}

void
AST::unmark ()
{
    _mark = 0;
}

void
AST::print (ostream& out, int indent)
{
    current_mark += 1;
    print(this, out, indent);
}

void
AST::print (AST_Ptr tree, std::ostream& out, int indent)
{
    if (tree->mark ())
        tree->_print (out, indent);
    else
        out << "...";
    tree->unmark ();
}

bool
AST::isErroneous ()
{
    return _erroneous;
}

void
AST::markErroneous ()
{
    _erroneous = true;
}

Type_Ptr
AST::asType ()
{
    throw logic_error ("node does not represent a type");
}

AST_Ptr
AST::getId ()
{
    UNIMPLEMENTED (getId);
}

Decl*
AST::getDecl ()
{
    return NULL;
}

void
AST::setDecl (Decl*)
{
    throw logic_error ("node does not represent a named entity");
}

Type_Ptr
AST::getType ()
{
    throw logic_error ("node does not represent a typed entity");
}

bool
AST::isMissing ()
{
    return false;
}

/* Definitions of methods in base class AST_Tree. */

void
AST_Tree::_print (ostream& out, int indent)
{
    out << "(" << externalName () << " " << lineNumber ();
    for_each_child (c, this) {
        out << endl << setw (indent + 4) << "";
        print (c, out, indent + 4);
    } end_for;
    out << ")";
}

const char*
AST_Tree::externalName ()
{
    static string result;

    int syntax = oper ()->syntax ();
    const char* name;
    name = yyexternal_token_name (syntax);
    if (name == NULL)
        return "?";
    if (name[0] != '"')
        return name;
    name += 1;
    while (*name == '@')
        name += 1;
    
    if (name[0] == '\0')
        return "?";

    result = string (name, strlen (name)-1);
    return result.c_str();
}

AST_Ptr
AST::flatten ()
{
    return this;
}

AST_Ptr
AST::semantics ()
{
    return this;
}

AST_Ptr
AST::rewrite_lambdas (AST_Ptr receiver, int& id, int syntax)
{
    for_each_child_var (c, this) {
        c = c->rewrite_lambdas (receiver, id, syntax);
    } end_for;
    return this;
}

void
AST::collectDecls (Decl* enclosing)
{
    for_each_child (c, this) {
        c->collectDecls (enclosing);
    } end_for;
}

void
AST::addTargetDecls (Decl* enclosing)
{
}

void
AST::resolveSimpleIds (const Environ* env)
{
    for_each_child (c, this) {
        c->resolveSimpleIds (env);
    } end_for;
}

void
AST::resolveSimpleTypeIds (const Environ* env)
{
}

AST_Ptr
AST::resolveAllocators (const Environ* env)
{
    for_each_child_var (c, this) {
        c = c->resolveAllocators (env);
    } end_for;
    return this;
}

AST_Ptr
AST::resolveStaticSelections (const Environ* env)
{
    for_each_child_var (c, this) {
        c = c->resolveStaticSelections (env);
    } end_for;
    return this;
}

AST_Ptr
AST::rewriteMethodCalls ()
{
    for_each_child_var (c, this) {
        c = c->rewriteMethodCalls ();
    } end_for;
    return this;
}

AST_Ptr
AST::rewriteMethodCall (AST_Ptr parentCall)
{
    return parentCall;
}

void
AST::freezeDecls (bool frozen)
{
    for_each_child (c, this) {
        c->freezeDecls (frozen);
    } end_for;
}    

void
AST::doTyping (Decl* context, bool& changed, bool& unresolved)
{
    for_each_child (c, this) {
        c->doTyping (context, changed, unresolved);
    } end_for;
}

void
AST::checkNoBoundMethodValues (bool) {
    for_each_child (c, this) {
        c->checkNoBoundMethodValues (false);
    } end_for;
}

void
AST::reportUnresolved ()
{
    for_each_child (c, this) {
        c->reportUnresolved ();
    } end_for;
}

void AST::codeGen(ostream& out, Decl* decl) {
	int syntax = oper()->syntax();

	switch (syntax) {
		case ID: {
			if (callMode && getDecl() && !getDecl()->get_type()->isFunction()) {
				/* We're calling a variable which is not actually a function! So we cast. */
				functionCastMode = true;
				functionCastName = as_string();
				Type_Ptr func_type = getDecl()->get_type();
				string return_type = func_type->returnType()->get_code_name();

				out << "((" << return_type;
				if (return_type != "void") out << "*";
			    out << " (*)(";
			    for (int i = 0; i < func_type->numParams(); i++) {
			    	out << func_type->paramType(i)->get_code_name() << "*, ";
			    }

			    out << "void*)) " << as_string() << "->func)";
			}
			else if (!callMode && getDecl() && getDecl()->get_type()->isFunction()) {
				out << "(new PyFunc((void *) " << as_string() << ", _f))";
			} else {
				if (getDecl() && getDecl()->getDeclCategory() != GLOBAL_DECL && decl) {
					Decl* me = decl->get_environ()->find_immediate(as_string());

					if (me == NULL) {
						/* This means this variable is not local, so we must traverse frames to find it. */
						const Environ* env = decl->get_environ();

						out << "_f->parent->";
						while (true) {
							env = env->get_enclosure();
							me = env->find_immediate(as_string());
							if (me == NULL) {
								out << "parent->";
							} else break;
						}
					}
				}

				out << as_string();
			}
		} break;
		case INT_LITERAL: {
			out << "(new PyInt(" << as_string() << "))";
		} break;
		case STRING: {
			out << "(new PyStr(\"" << as_string() << "\"))";
		} break;
		default: {
		    for_each_child (c, this) {
		        c->codeGen(out, decl);
		    } end_for;
		} break;
	}
}

AST_Ptr
make_id (const char* text, const char* loc) 
{
    AST_Ptr result = AST::make_token (ID, strlen (text), text, false);
    result->set_loc (loc);
    return result;
}

AST_Ptr
consTree (int syntax)
{
    AST_Ptr args[1];
    return AST::make_tree (syntax, args, args);
}

AST_Ptr
consTree (int syntax, const AST_Ptr& c0)
{
    AST_Ptr args[] = { c0 };
    return AST::make_tree (syntax,
                           args, args + sizeof (args) / sizeof(args[0]));
}

AST_Ptr
consTree (int syntax, const AST_Ptr& c0, const AST_Ptr& c1)
{
    AST_Ptr args[] = { c0, c1 };
    return AST::make_tree (syntax,
                           args, args + sizeof (args) / sizeof(args[0]));
}

AST_Ptr
consTree (int syntax, const AST_Ptr& c0, const AST_Ptr& c1,
          const AST_Ptr& c2)
{
    AST_Ptr args[] = { c0, c1, c2 };
    return AST::make_tree (syntax,
                           args, args + sizeof (args) / sizeof(args[0]));
}

AST_Ptr
consTree (int syntax, const AST_Ptr& c0, const AST_Ptr& c1,
          const AST_Ptr& c2, const AST_Ptr& c3)
{
    AST_Ptr args[] = { c0, c1, c2, c3 };
    return AST::make_tree (syntax,
                           args, args + sizeof (args) / sizeof(args[0]));
}

/* The Horn framework has a provision (the NODE_FACTORY macro) that
 * allows AST subclasses to "register" themselves with the parser and
 * associate themselves with a particular syntactic category so that
 * the parser will create instances of the appropriate subclass when
 * asked to produce an AST node with a particular operator.  The
 * NODE_..._CONSTRUCTOR... macros set up the necessary constructors
 * and factory declarations in any given class.  To associate a
 * class with a given operator token, pass the (int) value of that
 * token's syntactic category to the NODE_FACTORY macro (for
 * one-character tokens, the character itself is usually the syntactic
 * category; for other tokens, use the name declared in the Horn
 * source file (those definitions appear in apyc-parser.hh, included above).
 */

/** A node representing a missing element in the AST (e.g., a missing 
 *  >>FILE argument in a print node.) */
class Empty_AST : public AST_Tree {

protected:

    /** Override AST_Tree::_print */
    void _print (ostream& out, int indent) {
        out << "()";
    }

    bool isMissing () {
        return true;
    }

    NODE_CONSTRUCTORS (Empty_AST, AST_Tree);

};

NODE_FACTORY (Empty_AST, EMPTY);
