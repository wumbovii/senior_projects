/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* exprs.cc: AST subclasses related to expressions. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include <sstream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"
#include "lib/apyc/runtime.h"


using namespace std;
extern bool assignSubMode;
extern string assignSubName;
/*****   TYPED_EXPR *****/

/** The supertype of expressions with a type. */
class Typed_Expr_AST : public AST_Tree {
public:

    Type_Ptr getType () {
        if (_unresolved)
            _type = computeType ();
        return _type;
    }

protected:

    void updateResolution (bool& changed, bool& unresolved) {
        if (_unresolved) {
            if (_type == NULL)
                changed = true;
            getType ();
            changed |= !_unresolved;
            unresolved |= _unresolved;
        }
    }

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        AST::doTyping (context, changed, unresolved);
        updateResolution (changed, unresolved);
    }

    /** Computes my type, which is then cached by getType(). */
    virtual Type_Ptr computeType () {
        _unresolved = false;
        return Type::makeVar ();
    }

    void reportUnresolved () {
        if (_unresolved)
            Error (this, "unresolved type in expression");
    }

    Type_Ptr _type;
    bool _unresolved;

    NODE_BASE_CONSTRUCTORS_INIT (Typed_Expr_AST, AST_Tree,
                                 _unresolved (true));

};    


/*****   EXPR_LIST    *****/

/** A list of expressions. */
class Expr_List_AST : public AST_Tree {
protected:

    NODE_CONSTRUCTORS (Expr_List_AST, AST_Tree);

};

NODE_FACTORY (Expr_List_AST, EXPR_LIST);

/** A string literal. */
class String_Literal_AST : public Typed_Expr_AST {
protected:

    void _print (ostream& out, int indent) {
        out << "(string_literal " << lineNumber () << " \"";
        for_each_child (c, this) {
            print (c, out, indent);
        } end_for;
        out << "\")";
    }

    Type_Ptr computeType () {
        _unresolved = false;
        return strDecl->as_type ();
    }
        
    NODE_CONSTRUCTORS (String_Literal_AST, Typed_Expr_AST);

};

NODE_FACTORY (String_Literal_AST, STRING_LITERAL);

    
/*****   AND   *****/

/** E1 'and' E2 */
class And_AST : public Typed_Expr_AST {
protected:
	void codeGen (ostream& out, Decl* decl) {

		this->child(0)->codeGen(out, decl);
		out << "->logical(\"and\", ";
		this->child(1)->codeGen(out, decl);
		out << ")";

	}
    Type_Ptr computeType () {
        Type_Ptr 
            t0 = child (0)->getType (),
            t1 = child (1)->getType ();
        _unresolved = false;
        if (! t0->unify (t1)) {
            Error (this, "operands of 'and' have incompatible types");
            return Type::makeVar ();
        }
        return t0;
    }

    NODE_CONSTRUCTORS (And_AST, Typed_Expr_AST);

};

NODE_FACTORY (And_AST, AND);

/*****   ASSIGN   *****/

/** T = E */
class Assign_AST : public Typed_Expr_AST {
protected:
	void codeGen (ostream& out, Decl* decl) {
		/* Currently only works for simple 1 to 1 assignments */

		AST_Ptr var = child(0);
		if(var->getDecl() != NULL){
			//normal assign
			Type_Ptr varType = var->getDecl()->get_type();
			out << indent();
			if (declareMode) {
				out << varType->get_code_name();
				out << "* ";
			}
			out << var->getDecl()->get_name();
			out << " = ";
			child(1)->codeGen(out, decl);
			out << ";\n";
		} else {
			//selection assign
			assignSubMode = true;
			this->child(0)->codeGen(out, decl);

			this->child(1)->codeGen(out, decl);
			out << ");\n";


		}



	}

    void collectDecls (Decl* enclosing) {
        child (1)->collectDecls (enclosing);
        child (0)->addTargetDecls (enclosing);
    }

    Type_Ptr computeType () {
        Type_Ptr t0 = child (0)->getType (),
            t1 = child (1)->getType ();
        _unresolved = false;
        if (! t0->unify (t1)) {
            Error (this, "operands of 'assign' have incompatible types");
            return Type::makeVar ();
        }
        return t0;
    }

    NODE_CONSTRUCTORS (Assign_AST, Typed_Expr_AST);

};

NODE_FACTORY (Assign_AST, ASSIGN);


/*****   ATTRIBUTEREF   *****/

/** E.id */
class Attributeref_AST : public Typed_Expr_AST {
protected:

    Decl* getDecl () {
        return child (1)->getDecl ();
    }

    void resolveSimpleIds (const Environ* env) {
        child (0)->resolveSimpleIds (env);
    }

    AST_Ptr resolveStaticSelections (const Environ* env) {
        replace (0, child (0)->resolveStaticSelections (env));
        Decl* qualifier = child (0)->getDecl ();
        if (qualifier != NULL && qualifier->is_type ()) {
            AST_Ptr id = child (1);
            string name = id->as_string ();
            Decl* decl = 
                qualifier->get_environ ()->find_immediate (name);
            if (decl == NULL) {
                Error (id, "type has no member %s", name.c_str ());
                return this;
            }
            id->setDecl (decl);
            return id;
        } else
            return this;
    }

    AST_Ptr rewriteMethodCall (AST_Ptr parentCall) {
        assert (this == parentCall->child (0));
        Decl* me = getDecl ();
        if (me != NULL && me->is_method()) {
            AST_Ptr params = consTree (EXPR_LIST, child (0));
            for_each_child (c, parentCall->child (1)) {
                params->append (c);
            } end_for;
            return consTree (CALL, child (1), params);
        } else
            return parentCall;
    }        

    Type_Ptr computeType () {
        Type_Ptr objType = child (0)->getType ();
        AST_Ptr id = child (1);
        string name = id->as_string ();
        Type_Ptr result;
        if (_type == NULL)
            result = Type::makeVar ();
        else
            result = _type;

        if (objType->resolveBinding ()) {
            _unresolved = false;
            if (objType->getAllowed () != Type::USER_CLASS)
                Error (this, "can only select from instances of classes");
            else {
                Decl* objDecl = objType->getDecl ();
                if (objDecl == NULL)
                    return result;
                Decl* attrDecl = 
                    objType->getDecl ()->get_environ ()->find_immediate (name);
                if (attrDecl == NULL) {
                    Error (this, "%s is not an attribute of object",
                           name.c_str ());
                    return result;
                }
                else if (!result->unify (attrDecl->get_selected_type ()))
                    Error (this, "%s has incompatible type", name.c_str ());
                id->setDecl (attrDecl);
            }
        }
        
        return result;
    }

    void checkNoBoundMethodValues (bool called) {
        child (0)->checkNoBoundMethodValues (false);
        if (!called) {
            Decl* decl = getDecl ();
            if (decl != NULL && decl->is_method ()) 
                Error (this, "improper use of bound method value");
        }
    }

    NODE_CONSTRUCTORS (Attributeref_AST, Typed_Expr_AST);

};

NODE_FACTORY (Attributeref_AST, ATTRIBUTEREF);



/*****   BINOP   *****/

/** E1 op E2 */
class Binop_AST : public Typed_Expr_AST {
protected:
	void codeGen (ostream& out, Decl* decl) {
		this->child(0)->codeGen(out, decl);

		out << "->binop(\"" << this->child(1)->as_string() << "\", ";

		this->child(2)->codeGen(out, decl);

		out << ")";
	}

    const char* externalName () {
        return "binop";
    }

    void resolveSimpleIds (const Environ* env) {
        child (0)->resolveSimpleIds (env);
        child (2)->resolveSimpleIds (env);
    }

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        child (0)->doTyping (context, changed, unresolved);
        child (2)->doTyping (context, changed, unresolved);
        updateResolution (changed, unresolved);
    }

    /** Default checks operands are ints and returns type int. */
    Type_Ptr computeType () {
        Type_Ptr
            t0 = child (0)->getType (),
            t1 = child (2)->getType ();
        
        if (!(intDecl->as_type ()->unify (t0) && t0->unify (t1))) {
            Error (this, "operands of %s must be of type int", 
                   child (1)->as_string().c_str ());
        }
        _unresolved = false;
        return t0;
    }

    NODE_BASE_CONSTRUCTORS (Binop_AST, Typed_Expr_AST);

};

/*****   +   *****/

/** E0 + E1 */
class Add_AST : public Binop_AST {
protected:

    /** Default checks operands are ints and returns type int. */
    Type_Ptr computeType () {
        Type_Ptr
            t0 = child (0)->getType (),
            t1 = child (2)->getType ();
        
        Type_Ptr result;
        if (_type == NULL)
            result = Type::makeVar (Type::INT|Type::LIST|Type::STR);
        else
            result = _type;

        if (!result->unify (t0) || !t0->unify (t1)) {
            Error (this, "operands of %s must both be of type int, list or str", 
                   child (1)->as_string().c_str ());
        }
        
        _unresolved = !result->resolveBinding ();
        return result;
    }

    NODE_CONSTRUCTORS (Add_AST, Binop_AST);

};

NODE_FACTORY (Add_AST, '+');



/*****   -   *****/

/** E0 - E1 */
class Sub_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (Sub_AST, Binop_AST);

};

NODE_FACTORY (Sub_AST, '-');



/*****   *   *****/

/** E0 * E1 */
class Mult_AST : public Binop_AST {
protected:

    Type_Ptr computeType () {
        Type_Ptr
            t0 = child (0)->getType (),
            t1 = child (2)->getType ();
        
        _unresolved = false;
        if (!t1->unify(intDecl->as_type()))
            Error (this, "second operand of '*' must be int");
        else if (!t0->unify(Type::makeVar(Type::INT | Type::STR)))
            Error (this, "first operand of '*' must be int or str");
        else if (!t0->resolveBinding())
            _unresolved = true;

        return t0;
    }

    NODE_CONSTRUCTORS (Mult_AST, Binop_AST);

};

NODE_FACTORY (Mult_AST, '*');



/*****   /   *****/

/** E0 / E1 */
class Div_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (Div_AST, Binop_AST);

};

NODE_FACTORY (Div_AST, '/');



/*****   //   *****/

/** E0 // E1 */
class DivDiv_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (DivDiv_AST, Binop_AST);

};

NODE_FACTORY (DivDiv_AST, DIVDIV);



/*****   %   *****/

/** E0 % E1 */
class MOD_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (MOD_AST, Binop_AST);

};

NODE_FACTORY (MOD_AST, '%');



/*****   <<   *****/

/** E0 << E1 */
class Lshift_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (Lshift_AST, Binop_AST);

};

NODE_FACTORY (Lshift_AST, LSH);



/*****   >>   *****/

/** E0 >> E1 */
class Rshift_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (Rshift_AST, Binop_AST);

};

NODE_FACTORY (Rshift_AST, RSH);



/*****   &   *****/

/** E0 & E1 */
class BitAnd_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (BitAnd_AST, Binop_AST);

};

NODE_FACTORY (BitAnd_AST, '&');



/*****   |   *****/

/** E0 | E1 */
class BitOr_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (BitOr_AST, Binop_AST);

};

NODE_FACTORY (BitOr_AST, '|');



/*****   ^   *****/

/** E0 ^ E1 */
class BitXor_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (BitXor_AST, Binop_AST);

};

NODE_FACTORY (BitXor_AST, '^');



/*****   **   *****/

/** E0 ** E1 */
class Exp_AST : public Binop_AST {
protected:

    NODE_CONSTRUCTORS (Exp_AST, Binop_AST);

};

NODE_FACTORY (Exp_AST, POW);


/*****   CALL   *****/

/** E0(E1,...En) */
class Call_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {
		callMode = true;
		child(0)->codeGen(out, decl);
		out << "(";
		callMode = false;
	    for_each_child (c, child(1)) {
	        c->codeGen (out, decl);
	        out << ", ";
	    } end_for;

	    if (functionCastMode) {
	    	out << functionCastName << "->sl)";
	    } else out << "NULL)";

	    if (!statementMode) out << ";\n";


	    functionCastMode = false;
	}

    AST_Ptr resolveAllocators (const Environ* env) {
        AST::resolveAllocators (env);
        AST_Ptr callee = child (0);
        Decl* calleeDecl = callee->getDecl ();
        if (calleeDecl == NULL || !calleeDecl->is_type ())
            return this;
        Decl* init = calleeDecl->get_environ ()->find_immediate ("__init__");
        if (init == NULL)
            return consTree (NEW, callee);
        else {
            AST_Ptr init_id = make_id ("__init__", child (0)->loc ());
            init_id->setDecl (init);
            return consTree (CALL1, init_id, 
                             child (1)->insert (0, consTree (NEW, callee)));
        }
    }

    AST_Ptr rewriteMethodCalls () {
        Typed_Expr_AST::rewriteMethodCalls ();
        return child(0)->rewriteMethodCall (this);
    }

    Type_Ptr computeType () {
        _unresolved = false;
        Type_Ptr callerType = child (0)->getType ();
        AST_Ptr paramTypes = consTree (TYPE_LIST);
        for_each_child (c, child (1)) {
            paramTypes->append (c->getType ());
        } end_for;
        Type_Ptr returnType = Type::makeVar ();
        if (!callerType->unify (consTree (FUNCTION_TYPE, returnType, paramTypes)
                                ->asType ())) {
            if ((callerType->getAllowed () & Type::FUNCTION) == 0) 
                Error (this, "attempt to call a non-function");
            else
                Error (this, "wrong parameters for function");
        } else if (_type != NULL && !returnType->unify (_type))
            Error (this, "returned value incompatible with use");
        return returnType;
    }

    void checkNoBoundMethodValues (bool) {
        child (0)->checkNoBoundMethodValues (true);
        child (1)->checkNoBoundMethodValues (false);
    }

    NODE_CONSTRUCTORS (Call_AST, Typed_Expr_AST);

};

NODE_FACTORY (Call_AST, CALL);

/*****   CALL1   *****/
class Call1_AST : public Call_AST {
protected:
    
    Type_Ptr computeType () {
        _unresolved = false;
        return child (1)->child (0)->getType ();
    }

    NODE_CONSTRUCTORS (Call1_AST, Call_AST);

};

NODE_FACTORY (Call1_AST, CALL1);


/*****   COMPARISON   *****/

/** E0 op1 E1 op2 E2 op3 ... */
class Comparison_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {
			int parenFlag = 0;
			for (size_t numChildren = 2, op = 1, prevID = 0;
				numChildren	< this->arity();
				numChildren += 2, op += 2, prevID += 2) {
			//if there is more than 1 expr
			//op will always be odd child
			//additional exprs will be even

			this->child(prevID)->codeGen(out, decl);

			if (this->arity() > 1) {
				out << "->compare(\"";
				out << this->child(op)->as_string();
				out << "\", ";
				this->child(numChildren)->codeGen(out, decl);
				out << ")";
			}

			if (numChildren + 2 <= this->arity() && this->arity() > 3) {
				out << "->logical(\"and\", ";
				parenFlag++;
			}

		}
		while(parenFlag) {
			out << "  )";
			parenFlag--;
		}
	}

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        for (size_t i = 0; i < arity (); i += 2) {
            child (i)->doTyping (context, changed, unresolved);
        }
        updateResolution (changed, unresolved);
    }

    Type_Ptr computeType () {
        Type_Ptr left;
        left = child (0)->getType ();
        _unresolved = false;
        for (size_t i = 2; i < arity (); i += 2) {
            Type_Ptr right  = child (i)->getType ();
            if (!compareTypeCheck (child (i-1), left, right))
                Error (child (i-1), "improper or incompatible types for %s",
                       child (i-1)->as_string ().c_str ());
            left = right;
        }
        return boolDecl->as_type ();
    }

    void resolveSimpleIds (const Environ* env) {
        for (size_t i = 0; i < arity (); i += 2) {
            child (i)->resolveSimpleIds (env);
        }
    }

    /** Check that LEFT and RIGHT are proper types for OP,
     *  unifying as indicated, and returning true on success, or false
     *  if incompatible. */
    bool compareTypeCheck (AST_Ptr op, Type_Ptr left, Type_Ptr right) {
        string opName = op->as_string ();
        if (opName == "in" || opName == "notin") {
            Type_Ptr container = Type::makeVar (Type::LIST|Type::DICT);
            if (!container->unify (right))
                return false;
            else if (right->resolveBinding ()) {
                if (! right->typeParam (0)->unify (left))
                    return false;
            } else
                _unresolved = true;
        } else if (!left->unify (right))
            return false;
        else if (!left->unify(Type::makeVar(Type::INT|Type::STR|Type::BOOL))) 
            return false;
        else if (!left->resolveBinding())
            _unresolved = true;
        return true;
    }


    NODE_CONSTRUCTORS (Comparison_AST, Typed_Expr_AST);

};

NODE_FACTORY (Comparison_AST, COMPARISON);



/*****   DICT_DISPLAY   *****/

/** { ... } */
class Dict_Display_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {
		out << "(new PyDict(" << arity();
	    for_each_child (c, this) {
	        out << ", ";
	        //will do the codegen for pair, puts "item1.codegen, item2.codegen" on outstream
	        c->codeGen(out, decl);
	    } end_for;
	    out << "))";
	}

    Type_Ptr computeType () {
        Type_Ptr keyType = Type::makeVar ();
        Type_Ptr valueType = Type::makeVar ();
        for_each_child (c, this) {
            if (!keyType->unify (c->child (0)->getType ())) {
                Error (this, "key values have incompatible types");
                break;
            }
            if (!valueType->unify (c->child (1)->getType ())) {
                Error (this, "values have incompatible types");
                break;
            }
        } end_for;
        _unresolved = false;
        return dictDecl->as_type (2, keyType, valueType);
    }

    NODE_CONSTRUCTORS (Dict_Display_AST, Typed_Expr_AST);

};

NODE_FACTORY (Dict_Display_AST, DICT_DISPLAY);



/*****   ID_LIST   *****/

/** List of identifiers */
class Id_List_AST : public Typed_Expr_AST {
protected:

    NODE_CONSTRUCTORS (Id_List_AST, Typed_Expr_AST);

};

NODE_FACTORY (Id_List_AST, ID_LIST);



/*****   IF_EXPR   *****/

/** E1 if E0 else E2 */
class If_Expr_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {
		out << "if(";
		this->child(0)->codeGen(out, decl);
		out << ") { ";
		this->child(1)->codeGen(out, decl);
		out << "} else {";
		this->child(2)->codeGen(out, decl);
		out << "}\n";
	}

    Type_Ptr computeType () {
        Type_Ptr tc = child (0)->getType (),
            tthen = child (1)->getType (),
            telse = child (2)->getType ();
        if (! tthen->unify (telse)) 
            Error (this,
                   "branches of conditional expression have incompatible types");
        _unresolved = false;
        return tthen;
    }

    NODE_CONSTRUCTORS (If_Expr_AST, Typed_Expr_AST);

};

NODE_FACTORY (If_Expr_AST, IF_EXPR);



/*****   LAMBDA   *****/

/** lambda */
class Lambda_AST : public AST_Tree {
protected:

    AST_Ptr rewrite_lambdas (AST_Ptr receiver, int& count, int syntax) {
        AST_Ptr id = AST::make_token (ID, newLambdaId (count), false);
        AST_Ptr def = consTree (syntax, id, child (0), child (1),
                                consTree (BLOCK, 
                                          consTree (RETURN, child (2))));
        def->rewrite_lambdas (receiver, count, syntax);
        id->set_loc (loc ());
        def->set_loc (loc ());
        receiver->append (def);
        return id;
    }        

    NODE_CONSTRUCTORS (Lambda_AST, AST_Tree);

private:

    string newLambdaId (int& counter) {
        stringstream s;
        s << "__lambda" << counter << "__";
        counter += 1;
        return s.str ();
    }

};

NODE_FACTORY (Lambda_AST, LAMBDA);



/*****   LIST_DISPLAY   *****/

/** [ ... ] */
class List_Display_AST : public Typed_Expr_AST {
protected:
	void codeGen(ostream& out, Decl* decl) {


		out << "(new PyList(" << arity();
	    for_each_child (c, this) {
	        out << ", ";
	        c->codeGen(out, decl);
	    } end_for;

	    out << "))";
	}

    Type_Ptr computeType () {
        Type_Ptr elementType = Type::makeVar ();
        for_each_child (c, this) {
            if (!elementType->unify (c->getType ())) {
                Error (this, "list elements have incompatible types");
                break;
            }
        } end_for;
        _unresolved = false;
        return listDecl->as_type (1, elementType);
    }

    NODE_CONSTRUCTORS (List_Display_AST, Typed_Expr_AST);

};

NODE_FACTORY (List_Display_AST, LIST_DISPLAY);

/*****   NEW      *****/

/** CLASS (ARGS) */
class New_AST : public Typed_Expr_AST {
protected:

    Type_Ptr computeType () {
        _unresolved = false;
        return child (0)->getDecl ()-> as_type ();
    }

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        updateResolution (changed, unresolved);
    }

    NODE_CONSTRUCTORS (New_AST, Typed_Expr_AST);

};

NODE_FACTORY (New_AST, NEW);

/*****   OR   *****/

/** E1 or E2 */
class Or_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {

		this->child(0)->codeGen(out, decl);
		out << "->logical(\"or\", ";
		this->child(1)->codeGen(out, decl);
		out << ")";

	}

    Type_Ptr computeType () {
        Type_Ptr 
            t0 = child (0)->getType (),
            t1 = child (1)->getType ();
        _unresolved = false;
        if (! t0->unify (t1)) {
            Error (this, "operands of 'or' have incompatible types");
            return Type::makeVar ();
        }
        return t0;
    }

    NODE_CONSTRUCTORS (Or_AST, Typed_Expr_AST);

};

NODE_FACTORY (Or_AST, OR);



/*****   PAIR   *****/

/** KEY : VALUE */
class Pair_AST : public AST_Tree {
protected:

	void codeGen(ostream& out, Decl* decl) {
		this->child(0)->codeGen(out, decl);
		out << ", ";
		this->child(1)->codeGen(out, decl);

	}

    NODE_CONSTRUCTORS (Pair_AST, AST_Tree);

};

NODE_FACTORY (Pair_AST, PAIR);



/*****   SLICING   *****/

/** E0[E1:E2] */
class Slicing_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {

		this->child(0)->codeGen(out, decl);
		out << "->slice(";
		this->child(1)->codeGen(out, decl);
		out << ", ";
		this->child(2)->codeGen(out, decl);
		out << ")";
	}

    Type_Ptr computeType () {
        AST_Ptr obj = child (0);
        AST_Ptr lb = child (1);
        AST_Ptr ub = child (2);
        Type_Ptr intType = intDecl->as_type ();

        _unresolved = false;
        if ((!lb->isMissing () && !lb->getType ()->unify (intType))
            || (!ub->isMissing () && !ub->getType ()->unify (intType)))
            Error (this, "bounds of slice must be integers");
        
        Type_Ptr result = obj->getType ();
        if (_type != NULL && !result->unify (_type))
            Error (this, "value of slice invalid here");
        else if (!result->unify (Type::makeVar (Type::LIST|Type::STR)))
            Error (this, "can only slice strings and lists");
        else if (!result->resolveBinding ()) 
            _unresolved = true;
        return result;
    }

    NODE_CONSTRUCTORS (Slicing_AST, Typed_Expr_AST);

};

NODE_FACTORY (Slicing_AST, SLICING);



/*****   SUBSCRIPTION   *****/

/** E0[E1] */
class Subscription_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {

		if(assignSubMode){
			//subscription is targetlist
			assignSubMode = false;

			out << "((Settable*)";
			this->child(0)->codeGen(out, decl);
			out << ")->set(";
			child(1)->codeGen(out, decl);
			out << ", ";

		} else {
			//subscription is right side
			out << "((" << computeType()->get_code_name() << "* )((Gettable*)";
			this->child(0)->codeGen(out, decl);
			out << ")->get(";
			this->child(1)->codeGen(out, decl);
			out << "))";

		}

	}
    Type_Ptr computeType () {
        Type_Ptr t0 = child (0)->getType (),
            t1 = child (1)->getType ();
        bool resolved0 = t0->resolveBinding ();

        Type_Ptr result;
        if (_type == NULL)
            result = Type::makeVar ();
        else
            result = _type;

        int allowed0 = t0->getAllowed (),
            allowed1 = t1->getAllowed ();
                                       
        if ((result->getAllowed () && Type::STR) == 0 
            && (allowed0 & Type::STR)) {
            t0->unify (Type::makeVar (Type::UNRESTRICTED & ~Type::STR));
            resolved0 = t0->resolveBinding ();
            allowed0 = t0->getAllowed ();
        }

        _unresolved = false;
        if (allowed0 == Type::DICT || (allowed1 & Type::INT) == 0) {
            Type_Ptr t0a = dictDecl->as_type (2, t1, result);
            if (!t0->unify (t0a))
                Error (this, "improper types for subscription");
            return result;
        } else if ((allowed0 & Type::DICT) == 0 
                   && !t1->unify (intDecl->as_type ())) {
            Error (this, "subscript must be int");
            return result;
        } else if (allowed0 == Type::STR) {
            t1->unify (intDecl->as_type ());
            if (!result->unify (t0))
                Error (this, "string value invalid here");
        } else if (allowed0 == Type::LIST) {
            t1->unify (intDecl->as_type ());
            if (!result->unify (t0->typeParam (0)))
                Error (this, "value of list subscription invalid here");
        } else
            _unresolved = true;
        return result;
    }

    NODE_CONSTRUCTORS (Subscription_AST, Typed_Expr_AST);

};

NODE_FACTORY (Subscription_AST, SUBSCRIPTION);



/*****   TUPLE   *****/

/** (E0, ...) */
class Tuple_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {
		out << "(new PyTuple(" << arity();
	    for_each_child (c, this) {
	        out << ", ";
	        c->codeGen(out, decl);
	    } end_for;

	    out << "))";
	}

    Type_Ptr computeType () {
        _unresolved = false;
        Type_Ptr* elementTypes = new Type_Ptr[arity ()];
        for_each_child (c, this) {
            elementTypes[c_i_] = c->getType ();
        } end_for;
        Type_Ptr result = tupleDecl->as_type (arity (), elementTypes);
        delete [] elementTypes;
        return result;
    }

    NODE_CONSTRUCTORS (Tuple_AST, Typed_Expr_AST);

};

NODE_FACTORY (Tuple_AST, TUPLE);


/*****   TARGET_LIST   *****/

/** Left side of assignment or control variables in for. */
class Target_List_AST : public Tuple_AST {
protected:

    void addTargetDecls (Decl* enclosing) {
        for_each_child (c, this) {
            c->addTargetDecls (enclosing);
        } end_for;
    }

    NODE_CONSTRUCTORS (Target_List_AST, Tuple_AST);

};

NODE_FACTORY (Target_List_AST, TARGET_LIST);




/*****   TYPED_ID   *****/

/** ID :: TYPE */
class Typed_Id_AST : public Typed_Expr_AST {
protected:

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        updateResolution (changed, unresolved);
    }

    void addTargetDecls (Decl* enclosing) {
        child (0)->addTargetDecls (enclosing);
    }

    AST_Ptr getId () {
        return child (0);
    }

    Decl* getDecl () {
        return child (0)->getDecl ();
    }

    Type_Ptr computeType () {
        Type_Ptr type0 = child (0)->getType ();
        if (! type0->unify (child (1)->asType ())) {
            Error (this, "incompatible types implied for %s",
                   child (0)->as_string ().c_str ());
        }
        _unresolved = false;
        return type0;
    }

    void resolveSimpleTypeIds (const Environ* env) {
        child (1)->resolveSimpleIds (env);
    }

    NODE_CONSTRUCTORS (Typed_Id_AST, Typed_Expr_AST);

};

NODE_FACTORY (Typed_Id_AST, TYPED_ID);



/*****   UNOP   *****/

/** op E */
class Unop_AST : public Typed_Expr_AST {
protected:

	void codeGen(ostream& out, Decl* decl) {


		out << "((" << computeType()->get_code_name() << "* )";
		this->child(1)->codeGen(out, decl);
		out << "->unop(\"";
		this->child(0)->codeGen(out, decl);
		out << "\"))";

	}

    const char* externalName () {
        return "unop";
    }

    void resolveSimpleIds (const Environ* env) {
        child (1)->resolveSimpleIds (env);
    }

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        child (1)->doTyping (context, changed, unresolved);
        updateResolution (changed, unresolved);
    }

    Type_Ptr computeType () {
        _unresolved = false;
        Type_Ptr result = intDecl->as_type();
        if (!result->unify(child (1)->getType ())) {
            Error (this, "operand of %s must be int", 
                   child (0)->as_string ().c_str ());
        }
        return result;
    }

    NODE_BASE_CONSTRUCTORS (Unop_AST, Typed_Expr_AST);

};

/*****   +   *****/

/** + E1 */
class Unop_Plus_AST : public Unop_AST {
protected:

    NODE_CONSTRUCTORS (Unop_Plus_AST, Unop_AST);

};

NODE_FACTORY (Unop_Plus_AST, UNOP_PLUS);



/*****   -   *****/

/** - E1 */
class Neg_AST : public Unop_AST {
protected:

    NODE_CONSTRUCTORS (Neg_AST, Unop_AST);

};

NODE_FACTORY (Neg_AST, NEG);



/*****   ~   *****/

/** ~ E1 */
class Complement_AST : public Unop_AST {
protected:

    NODE_CONSTRUCTORS (Complement_AST, Unop_AST);

};

NODE_FACTORY (Complement_AST, '~');



/*****   not   *****/

/** not E1 */
class Not_AST : public Unop_AST {
protected:

    Type_Ptr computeType () {
        _unresolved = false;
        return boolDecl->as_type();
    }

    NODE_CONSTRUCTORS (Not_AST, Unop_AST);

};

NODE_FACTORY (Not_AST, NOT);
