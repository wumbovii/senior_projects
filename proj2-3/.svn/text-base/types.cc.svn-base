/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* types.cc: AST subclasses related to types. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

/*****    AST     *****/

AST_Ptr
AST::joinTypes (AST_Map& typeEnv)
{
    for_each_child_var (c, this) {
        c = c->joinTypes (typeEnv);
    } end_for;
    return this;
}

/*****    TYPE    *****/

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
    assert (_binding == NULL);
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

void Type::restrict (int allowed) {
}

int Type::getAllowed () {
    return USER_CLASS;
}

bool Type::isTypeVariable () {
    return false;
}

Type::TypeCategory Type::getCategory () {
    return USER_CLASS;
}

void Type::setTypeCategory (Type::TypeCategory category) {
	cout << "Setting type cat of type superclass! Shouldn't be doing this!";
}

void Type::setComplete(bool complete) {

}

bool Type::isComplete() {
	return true;
}

/** Default implementation of checkBinding for non-function types. */
bool
Type::checkBinding (UnwindStack& old)
{
    Type_Ptr target = binding ();

    if (getCategory () != target->getCategory ())
        return false;
    if (arity () != target->arity ())
        return false;
    if (child (0)->getDecl () != target->child (0)->getDecl ())
        return false;
    for_each_child (c, child(1)) {
        if (! c->asType ()
            ->unify (target->child (1)->child (c_i_)->asType (), old))
            return false;
    } end_for;
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
        type_args[c_i_] = c->asType ()->freshen ();
        if (c != type_args[c_i_])
            modified = true;
    } end_for;

    if (!modified) {
        delete [] type_args;
        return this;
    }

    AST_Ptr kids[2] = {
        child (0), AST::make_tree (TYPE_LIST, type_args, type_args + type_arity)
    };
    _forward = AST::make_tree (oper ()->syntax (), kids, kids + 2)->asType ();
    delete [] type_args;
    return _forward;
}


Type_Ptr Type::replaceBindings (Type_Ptr type) {
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


/*****  TYPE VARIABLES *****/

class TypeVar_AST : public Type {
protected:
    
    NODE_CONSTRUCTORS (TypeVar_AST, Type);

    TypeCategory getCategory () {
        return NONE;
    }

    bool isTypeVariable () {
        return true;
    }

    bool bind (Type_Ptr target, UnwindStack& old) {
        if ((target->getAllowed () & allowed) == 0)
            return false;
        old.push (UnwindData (target, target->getAllowed ()));
        target->restrict (allowed);
        return Type::bind (target, old);
    }
    
    void restrict (int allowed) {
        this->allowed &= allowed;
    }

    void unwind (int allowed) {
        _binding = NULL;
        this->allowed = allowed;
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

    NODE_CONSTRUCTORS (FunctionType_AST, Type);

    TypeCategory getCategory () {
        return FUNCTION;
    }

    /** Implementation of checkBinding for function types. */
    bool checkBinding (UnwindStack& old) {
        Type_Ptr target = binding ();

        if (getCategory () != target->getCategory ())
            return false;
        if (child(1)->arity () != target->child(1)->arity ())
            return false;
        if (!child (0)->asType()->unify(target->child(0)->asType()))
            return false;
        for_each_child (c, child(1)) {
            // EDITED BELOW to remove "old" argument to unify.
            if (! c->asType ()
                ->unify (target->child (1)->child (c_i_)->asType ()))
                return false;
        } end_for;
        return true;
    }

    void setComplete(bool complete) {
    	typingComplete = complete;
    }

    bool isComplete() {
    	return typingComplete;
    }

private:
    bool typingComplete;
};

NODE_FACTORY (FunctionType_AST, FUNCTION_TYPE);


/***** CLASSES *****/

class ClassType_AST: public Type {
protected:

    NODE_CONSTRUCTORS (ClassType_AST, Type);

    TypeCategory getCategory () {
        if (myCategory == NULL)
            return NONE;
        else
            return myCategory;
    }

    int getAllowed () {
        if (myCategory == NULL)
            return USER_CLASS;
        else
            return myCategory;
    }

    void setTypeCategory (TypeCategory category) {
    	myCategory = category;
    }

    void setDecl (Decl* decl) {
    	myDecl = decl;
    }

    Decl* getDecl() {
    	return myDecl;
    }

private:
    TypeCategory myCategory;
    Decl* myDecl;
};

NODE_FACTORY (ClassType_AST, TYPE);
