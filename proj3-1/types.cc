/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* types.cc: AST subclasses related to types. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

/*****    AST     *****/

/**
 * This is a method which returns the mapping of a type to our representation of
 * types as defined in runtime.h
 *
 * */

string Type::get_code_name() {
	string name;

	if (this->as_string() == "bool") {
		name = "PyBool";
		return name;
	}

	switch (getCategory()) {
		case INT:
			name = "PyInt";
			break;
		case STR:
			name = "PyStr";
			break;
		case TUPLE:
			name = "PyTuple";
			break;
		case LIST:
			name = "PyList";
			break;
		case DICT:
			name = "PyDict";
			break;
		case USER_CLASS:
			name = "PyClass";
			break;
		case FUNCTION:
			name = "PyFunc";
			break;
		case NONE:
			/** This is used for return types of functions. */
			name = "void";
			break;
		default:
			name = "PyObject";
			break;
	}
	return name;
}

AST_Ptr
AST::joinTypes (AST_Map& typeEnv)
{
    for_each_child_var (c, this) {
        c = c->joinTypes (typeEnv);
    } end_for;
    return this;
}

/*****    TYPE    *****/

int
Type::numParams ()
{
    return -1;
}

Type_Ptr
Type::returnType ()
{
    return NULL;
}

Type_Ptr
Type::paramType (int k)
{
    throw range_error ("function type has no parameters");
}

int
Type::numTypeParams ()
{
    return -1;
}

Type_Ptr
Type::typeParam (int k)
{
    throw range_error ("type has no type parameters");
}

Type_Ptr
Type::makeVar (int allowed)
{
    AST_Ptr dummy[1];

    Type_Ptr result = AST::make_tree (TYPE_VAR, dummy, dummy)->asType ();
    result->restrict (allowed);
    return result;
}

Type_Ptr
Type::binding ()
{
    Type_Ptr p;
    p = this;
    while (p->_binding != NULL)
	p = p->_binding;
    return p;
}

bool
Type::unify (Type_Ptr type)
{
    UnwindStack bindingStack;
    if (unify (type, bindingStack))
	return true;
    else {
	while (!bindingStack.empty ()) {
            bindingStack.top ().unwind ();
	    bindingStack.pop ();
	}
	return false;
    }
}

bool
Type::unify (Type_Ptr T0, Type_Ptr T1, UnwindStack& old)
{
    return T0->unify (T1, old);
}

bool
Type::unifyLists (AST_Ptr L1, AST_Ptr L2, UnwindStack& old)
{
    if (L1->arity () != L2->arity ())
        return false;
    for_each_child (c, L1) {
        if (!c->asType ()->unify (L2->child (c_i_)->asType (), old))
            return false;
    } end_for;
    return true;
}

bool
Type::unifies (Type_Ptr type)
{
    UnwindStack bindingStack;
    bool result = unify (type, bindingStack);
    while (!bindingStack.empty ()) {
        bindingStack.top ().unwind ();
        bindingStack.pop ();
    }
    return result;
}

bool
Type::unify (Type_Ptr type, UnwindStack& s)
{
    assert (type != NULL);
    Type_Ptr me = binding ();
    type = type->binding ();
    if (me == type)
	return true;
    if (me->isTypeVariable ())
	return me->bind (type, s);
    if (!type->bind (me, s))
	return false;
    return type->checkBinding (s);
}

bool
Type::bind (Type_Ptr target, UnwindStack& old)
{
    assert (_binding == NULL && target != NULL);
    old.push (UnwindData (this, 0));
    _binding = target;
    return true;
}

void
Type::unbind (int allowed)
{
    assert (_binding != NULL);
    _binding = NULL;
}

void
Type::restrict (int allowed)
{
}

void
Type::unrestrict (int allowed)
{
}

int
Type::getAllowed ()
{
    return USER_CLASS;
}

bool
Type::isTypeVariable ()
{
    return false;
}

bool
Type::resolveBinding ()
{
    return true;
}

Type::TypeCategory
Type::getCategory ()
{
    return USER_CLASS;
}

/** Default implementation of checkBinding for non-function types. */
bool
Type::checkBinding (UnwindStack& old)
{
    Type_Ptr target = binding ();

    if (getCategory () != target->getCategory ())
        return false;
    if (child (0)->getDecl () != target->child (0)->getDecl ())
        return false;
    if (!unifyLists (child (1), target->child (1), old))
        return false;
    return true;
}

Type_Ptr
Type::freshen ()
{
    current_mark += 1;
    return _freshen ();
}

Type_Ptr
Type::_freshen ()
{
    Type_Ptr me = binding ();
    if (me != this)
        return me->_freshen ();

    if (!mark ())
        return _forward;

    _forward = this;
    if (arity () == 0)
        return this;

    bool modified;
    modified = false;
    int type_arity = child (1)->arity ();
    AST_Ptr* type_args = new AST_Ptr[type_arity];
    for_each_child (c, child (1)) {
        type_args[c_i_] = c->asType ()->_freshen ();
        if (c != type_args[c_i_])
            modified = true;
    } end_for;

    if (!modified) {
        delete [] type_args;
        return this;
    }

    _forward = consTree (oper ()->syntax (), child (0),
                         AST::make_tree (TYPE_LIST, type_args,
                                         type_args + type_arity))->asType ();
    delete [] type_args;
    return _forward;
}

Type_Ptr
Type::replaceBindings (Type_Ptr type)
{
    current_mark += 1;
    return type->replaceBindings ();
}

Type_Ptr
Type::replaceBindings ()
{
    Type_Ptr me = binding ();
    if (me->mark ()) {
        for_each_child_var (c, this) {
            c = c->asType()->replaceBindings ();
        } end_for;
    }
    return me;
}

void
Type::doTyping (Decl*, bool&, bool&)
{
}

bool Type::isFunction() {
	return false;
}


/*****  TYPE VARIABLES *****/

class TypeVar_AST : public Type {
protected:
    
    TypeCategory getCategory () {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return NONE;
        else
            return b->getCategory ();
    }

    bool isTypeVariable () {
        return true;
    }

    Decl* getDecl () {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return NULL;
        else
            return b->getDecl ();
    }

    int numParams () {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return Type::numParams ();
        else
            return b->numParams ();
    }

    Type_Ptr paramType (int k) {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return Type::paramType (k);
        else
            return b->paramType (k);
    }

    Type_Ptr returnType () {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return Type::returnType ();
        else
            return b->returnType ();
    }

    int numTypeParams () {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return Type::numTypeParams ();
        else
            return b->numTypeParams ();
    }

    Type_Ptr typeParam (int k) {
        Type_Ptr b = binding ();
        if (b->isTypeVariable ())
            return Type::typeParam (k);
        else
            return b->typeParam (k);
    }

    int getAllowed () {
        Type_Ptr b = binding ();
        if (b == this) {
            return allowed;
        } else
            return b->getAllowed ();
    }

    bool bind (Type_Ptr target, UnwindStack& old) {
        assert (_binding == NULL && target != NULL);
        if ((target->getAllowed () & allowed) == 0)
            return false;
        old.push (UnwindData (this, target->getAllowed ()));
        target->restrict (allowed);
        _binding = target;
        return true;
    }
    
    bool resolveBinding () {
        Type_Ptr me = binding ();
        if (! me->isTypeVariable ())
            return true;
        switch (me->getAllowed ()) {
        default:
            return false;
        case INT:
            me->unify (intDecl->as_type ());
            return true;
        case STR:
            me->unify (strDecl->as_type ());
            return true;
        case XRANGE:
            me->unify (rangeDecl->as_type ());
            return true;
        case LIST:
            me->unify (listDecl->as_type (1, makeVar ()));
            return true;
        case DICT:
            me->unify (dictDecl->as_type (2, makeVar (), makeVar ()));
            return true;
        }
    }

    void restrict (int allowed) {
        this->allowed &= allowed;
    }

    void unrestrict (int allowed) {
        this->allowed = allowed;
    }

    void unwind (int allowed) {
        _binding->unrestrict (allowed);
        _binding = NULL;
    }

    Type_Ptr _freshen () {
        Type_Ptr me = binding ();
        if (mark ()) {
            if (me == this) {
                _forward = makeVar (allowed);
                return _forward;
            }
        } else if (me == this)
            return _forward;
        return me->_freshen ();
    }

    /** Since we will already have checked that we are bound to
     *  something proper in bind, this needn't do anything that return
     *  true. */
    bool checkBinding (UnwindStack&) {
        return true;
    }

    AST_Ptr joinTypes (map<string, AST_Ptr>& typeEnv) {
        pair<AST_Map::iterator, bool> place 
            = typeEnv.insert (make_pair (as_string(), this));
        if (place.second)
            return this;
        else
            return place.first->second;
    }

    void _print (ostream& out, int indent) {
        Type_Ptr me = binding ();
        if (me == this) {
            if (arity () == 1) {
                out << "(type_var " << lineNumber () << " ";
                print (child (0), out, indent);
                out << ")";
            } else
                out << "(type_var 0 (id 0 $_" << uid << "))";
        } else
            print (me, out, indent);
    }

    AST* post_make () {
        uid = next_uid;
        next_uid += 1;
        allowed = UNRESTRICTED;
        return this;
    }

    void dump (ostream& out, int indent = 0) {
        out << "(@type_var";
        Type_Ptr b = binding ();
        if (b->isTypeVariable ()) {
            if (b == this)
                out << " #" << uid;
            else
                b->dump (out, indent);
        } else {
            out << endl << setw (indent + 4) << "";
            b->dump (out, indent+4);
        }
        out << ")";
    }

    void resolveSimpleIds (const Environ* env) {
    }    

    NODE_CONSTRUCTORS (TypeVar_AST, Type);

private:

    int allowed;
    int uid;
    static int next_uid;
};

NODE_FACTORY (TypeVar_AST, TYPE_VAR);
int TypeVar_AST::next_uid = 0;


/***** FUNCTION TYPES *****/

class FunctionType_AST: public Type {
protected:
	bool isFunction() {
		return true;
	}

    int numParams () {
        return child (1)->arity ();
    }

    Type_Ptr paramType (int k) {
        return child (1)->child (k)->asType ();
    }

    Type_Ptr returnType () {
        return child (0)->asType ();
    }

    TypeCategory getCategory () {
        return FUNCTION;
    }

    int getAllowed () {
        return FUNCTION;
    }

    bool checkBinding (UnwindStack& old) {
        Type_Ptr target = binding ();

        if (getCategory () != target->getCategory ())
            return false;
        if (!unify (returnType (), target->returnType (), old))
            return false;
        if (!unifyLists (child (1), target->child (1), old))
            return false;
        return true;
    }

    Type_Ptr
    _freshen ()
    {
        bool modified;
        modified = false;

        Type_Ptr me = binding ();
        if (me != this)
            return me->_freshen ();

        if (!mark ())
            return _forward;

        _forward = this;

        Type_Ptr rtnType = returnType ()->_freshen ();
        modified |= rtnType != returnType ();

        int type_arity = child (1)->arity ();
        AST_Ptr* type_args = new AST_Ptr[type_arity];
        for_each_child (c, child (1)) {
            type_args[c_i_] = c->asType ()->_freshen ();
            if (c != type_args[c_i_])
                modified = true;
        } end_for;

        if (!modified) {
            delete [] type_args;
            return this;
        }

        _forward = consTree (oper ()->syntax (), rtnType,
                             AST::make_tree (TYPE_LIST, type_args,
                                             type_args + type_arity))->asType ();
        delete [] type_args;
        return _forward;
    }

    NODE_CONSTRUCTORS (FunctionType_AST, Type);

};

NODE_FACTORY (FunctionType_AST, FUNCTION_TYPE);


/***** CLASSES *****/

class ClassType_AST: public Type {
protected:

    TypeCategory getCategory () {
        if (getDecl () == NULL)
            return NONE;
        else
            return getDecl ()->getCategory ();
    }

    int numTypeParams () {
        return child (1)->arity ();
    }

    Type_Ptr typeParam (int k) {
        return child (1)->child (k)->asType ();
    }

    int getAllowed () {
        return getCategory ();
    }

    Decl* getDecl () {
        return child (0)->getDecl ();
    }

    void setDecl (Decl* decl) {
        child (0)->setDecl (decl);
    }

    NODE_CONSTRUCTORS (ClassType_AST, Type);

};

NODE_FACTORY (ClassType_AST, TYPE);


/*****   TYPE_LIST   *****/

/** List of types in parameterized type. */
class Type_List_AST : public AST_Tree {
protected:

    NODE_CONSTRUCTORS (Type_List_AST, AST_Tree);

};

NODE_FACTORY (Type_List_AST, TYPE_LIST);

