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

Type_Ptr Type::makeVar (int allowed) {
    AST_Ptr dummy[1];

    Type_Ptr result = AST::make_tree (TYPE_VAR, dummy, dummy)->asType ();
    result->restrict (allowed);
    return result;
}

/** move this logic to decls once decls done*/
Type_Ptr Type::makeClassType(int allowed) {
    AST_Ptr dummy[1];

    Type_Ptr result = AST::make_tree (TYPE, dummy, dummy)->asType ();
    //result->restrict (allowed);
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

bool Type::unify (Type_Ptr type) {
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

bool Type::unify (Type_Ptr type, UnwindStack& s) {
    Type_Ptr me = binding ();
    type = type->binding ();
    cout << "SHOULD BE INT: " << me->getAllowed() << "\n";
    cout << "SHOULD BE UNBOUND: " << type->getAllowed() << "\n";

    if (me == type)
    	return true;

    if (me->isTypeVariable ())
    	cout << "type variable binding \n";
    	return me->bind (type, s);
    if (!type->bind (me, s))
    	return false;
    return type->checkBinding (s);
}

bool Type::bind (Type_Ptr target, UnwindStack& old) {
    assert (_binding == NULL);
    old.push (UnwindData (this, 0));
    _binding = target;

    cout << _binding->getAllowed() << "bound \n";
    return true;
}

void Type::unbind (int allowed) {
    assert (_binding != NULL);
    _binding = NULL;
}

void Type::restrict (int allowed) {
	cout << "TRYING TO RESTRICT TO: " << allowed << "\n";
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

bool Type::checkBinding (UnwindStack& old) {
    Type_Ptr target = binding ();

    if (getCategory () != target->getCategory ())
        return false;
    if (arity () != target->arity ())
        return false;

    for_each_child (c, this->child(1)) {
        if (! c->asType ()->unify (target->child(1)->child (c_i_)->asType (), old))
            return false;
    } end_for;
    return true;
}

Type_Ptr Type::freshen () {
    current_mark += 1;
    return _freshen ();
}

Type_Ptr Type::_freshen () {
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
    AST_Ptr* kids = new AST_Ptr[arity ()];
    for_each_child (c, this) {
        kids[c_i_] = c->asType ()->freshen ();
        if (c != kids[c_i_])
            modified = true;
    } end_for;

    if (!modified)
        return this;

    _forward = AST::make_tree (oper ()->syntax (),
                               kids, kids + arity ())->asType ();
    delete [] kids;
    return _forward;
}

Type_Ptr Type::replaceBindings (Type_Ptr type) {
    current_mark += 1;
    return type->replaceBindings ();
}

Type_Ptr Type::replaceBindings () {
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

        cout << "  > restricting to: " << allowed << "\n";
        target->restrict (allowed);
        return Type::bind (target, old);
    }
    
    void restrict (int allowed) {
    	cout << "restricting shiznit" << allowed << "\n";
        this->allowed &= allowed;
        cout << "             after " << this->allowed << "\n";
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


};

NODE_FACTORY (FunctionType_AST, FUNCTION_TYPE);


/***** User-Defined CLASSES *****/

class ClassType_AST: public Type {
protected:

    NODE_CONSTRUCTORS (ClassType_AST, Type);

    TypeCategory getCategory () {
    	return INT;
//        if (getDecl () == NULL)
//            return NONE;
//        else
//            return getDecl ()->getCategory ();
    }


//    void restrict (int allowed) {
//    	this->allowed &= allowed;
//    }

    Decl* getDecl () {
        return child (0)->getDecl ();
    }

    void setDecl (Decl* decl) {
        child (0)->setDecl (decl);
    }

//    int getAllowed () {
//        return INT;
//    }

private:
    int allowed;
};

NODE_FACTORY (ClassType_AST, TYPE);

