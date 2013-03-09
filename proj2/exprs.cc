/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* exprs.cc: AST subclasses related to expressions. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

/*****   EXPR_LIST    *****/

/** A list of expressions. */
class Expr_List_AST : public AST_Tree {
protected:

    NODE_CONSTRUCTORS (Expr_List_AST, AST_Tree);

};

NODE_FACTORY (Expr_List_AST, EXPR_LIST);

/** A string literal. */
class String_Literal_AST : public AST_Tree {
protected:

    NODE_CONSTRUCTORS (String_Literal_AST, AST_Tree);

    void _print (ostream& out, int indent) {
        out << "(string_literal " << lineNumber () << " \"";
        for_each_child (c, this) {
            print (c, out, indent);
        } end_for;
        out << "\")";
    }
};

NODE_FACTORY (String_Literal_AST, STRING_LITERAL);

    
