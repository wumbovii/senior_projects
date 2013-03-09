/* Implementations of declaration and type-related classes */

/* Authors:  Dmitry Kislyuk */
/* From a skeleton by Paul N. Hilfinger */

#include <iostream>
#include <stdexcept>
#include "apyc.h"
#include "apyc-parser.hh"

using namespace std;

static vector<Decl*> all_decls;

Decl::Decl (const std::string& name, Decl* container, Environ* members)
    : _frozen (true), _name (name), _container (container),
      _members (members) {
    _index = all_decls.size ();
    all_decls.push_back (this);
    /* Allow name.c_str() to be used in the future without reallocation. */
    name.c_str ();
}

DeclCategory Decl::getDeclCategory() {
	return LOCAL_DECL;
}

/** Code to generate frames for functions.
 * Author: Dmitry Kislyuk*/
void Decl::genFrames(ostream& out) {
	if (_members == NULL) return;

	Decl_Vector decls = get_environ()->get_members();

	if (getDeclCategory() == FUNC_DECL) {
		string return_name = get_type()->returnType()->get_code_name();
		out << return_name;
		if (return_name != "void") out << "*";

		out << " " << get_name() << "(";

		Decl_Vector decls = get_environ()->get_members();
		for (size_t i = 0; i < decls.size(); i++) {
			Decl* d = decls[i];
			if (d->getDeclCategory() == PARAM_DECL) {
				out << d->get_type()->get_code_name() << "* " << d->get_name() << ", ";
			}
		}

		out << "void* parent = NULL);\n";
		out << "struct " << get_name() << "_frame {\n";

		Decl* parent = get_container();
		if (parent != NULL && parent->getDeclCategory() == FUNC_DECL) {
			out << "  " << parent->get_name() << "_frame* parent;\n";
		}

		for (size_t i = 0; i < decls.size(); i++) {
			Decl* d = decls[i];
			DeclCategory category = d->getDeclCategory();

			if (category == FUNC_DECL) {
				pushFrameQueue(d);
			} else {
				out << "  " << d->get_type()->get_code_name() << "* " << d->get_name() << ";\n";
			}
		}

		out << "};\n\n";
	} else {
		for (size_t i = 0; i < decls.size(); i++) {
			decls[i]->genFrames(out);
		}
	}
}

void Decl::genForwards(ostream& out) {
	if (getDeclCategory() == GLOBAL_DECL)
		out << get_type()->get_code_name() << "* " << get_name() << ";\n";

	if (getDeclCategory() == CLASS_DECL)
		out << "class " << get_name() << ";\n";
}
        
/* Print THIS on the standard output. */
void 
Decl::print () const
{
    printf ("(%s %d %s ", decl_type_name (),
            get_index (), get_name().c_str ());
    print_container ();
    print_position ();
    print_type ();
    print_index_list ();
    printf (")");
    fflush (stdout);
}

void
Decl::print_container () const
{
    if (get_container () != NULL) 
        printf ("%d ", get_container ()->get_index ());    
    else
        printf ("-1 ");
}

void
Decl:: print_position () const {
}

void
Decl::print_type () const {
}

void
Decl::print_index_list () const {
    if (_members != NULL) {
	printf (" (index_list");
	const vector<Decl*>& members = get_environ ()->get_members ();
	for (size_t i = 0; i < members.size (); i += 1)
	    printf (" %d", members[i]->get_index ());
	printf (")");
    }
}

Type_Ptr 
Decl::get_type () const
{
    return NULL;
}

void
Decl::set_type (Type_Ptr type)
{
}

Type_Ptr
Decl::get_selected_type () const
{
    return get_type ();
}

int
Decl::get_position () const
{
    return -1;
}

bool
Decl::is_type () const
{
    return false;
}

bool
Decl::is_method () const
{
    return false;
}

Type_Ptr
Decl::as_type (int, Type_Ptr, Type_Ptr) const
{
    UNIMPLEMENTED (as_type);
}

Type_Ptr
Decl::as_type (int arity, Type_Ptr*) const
{
    UNIMPLEMENTED (as_type);
}

int
Decl::get_type_arity () const
{
    return 0;
}

const Environ*
Decl::get_environ () const
{
    if (_members == NULL)
	UNIMPLEMENTED (get_members);
    return _members;
}

void
Decl::add_member (Decl* new_member)
{
    if (_members == NULL)
	UNIMPLEMENTED (add_member);
    if (_members->find_immediate (new_member->get_name ()) == NULL)
	_members->define (new_member);
}

Decl*
Decl::add_var_decl (AST_Ptr) {
    UNIMPLEMENTED (add_var_decl);
}

Decl*
Decl::add_def_decl (AST_Ptr) {
    UNIMPLEMENTED (add_def_decl);
}

Type::TypeCategory
Decl::getCategory () const
{
    return Type::NONE;
}

bool
Decl::assignable () const
{
    return false;
}

bool
Decl::is_frozen () const
{
    return _frozen;
}

void
Decl::set_frozen (bool freeze)
{
}

Decl::~Decl ()
{
}

const char*
Decl::decl_type_name () const
{
    return "?";
}

/** The superclass of declarations that describe an entity with a type. */
class TypedDecl : public Decl {
protected:
    TypedDecl (const string& name, Decl* container, AST_Ptr type,
	       Environ* members = NULL) 
        :  Decl (name, container, members),
           _type (type == NULL ? NULL : type->asType ()) {
    }

public:

    Type_Ptr get_type () const {
	if (is_frozen () || _type == NULL)
	    return _type;
	else
	    return _type->freshen ();
    }

    void set_type (Type_Ptr type) {
        _type = type;
    }

protected:

    void print_type () const {
        printf (" ");
        if (_type != NULL) {
            _type->binding ()->print (cout, 0);
        } else
            printf ("?");
    }

private:
    Type_Ptr _type;
};

class LocalDecl : public TypedDecl {
public:

    LocalDecl (const string& name, Decl* container, AST_Ptr type)
        :  TypedDecl (name, container, type) {
    }

    DeclCategory getDeclCategory() {
    	return LOCAL_DECL;
    }

protected:

    const char* decl_type_name () const {
        return "localdecl";
    }

    bool assignable () const {
	return true;
    }

};

Decl*
makeLocalDecl (const string& name, Decl* func, AST_Ptr type)
{
    return new LocalDecl (name, func, type);
}


class GlobalDecl : public TypedDecl {
public:

    GlobalDecl (const string& name, Decl* container, AST_Ptr type)
        :  TypedDecl (name, container, type) {
    }

    DeclCategory getDeclCategory() {
    	return GLOBAL_DECL;
    }

protected:

    const char* decl_type_name () const {
        return "globaldecl";
    }

    bool assignable () const {
	return true;
    }

};

Decl*
makeGlobalDecl (const string& name, Decl* module, AST_Ptr type)
{
    return new GlobalDecl (name, module, type);
}


class ParamDecl : public TypedDecl {
public:

    ParamDecl (const string& name, Decl* func, int k,
              AST_Ptr type)
        :  TypedDecl (name, func, type), _posn (k) {
    }

    int get_position () const {
        return _posn;
    }

    DeclCategory getDeclCategory() {
    	return PARAM_DECL;
    }

protected:

    const char* decl_type_name () const {
        return "paramdecl";
    }

    void print_position () const {
        printf (" %d", get_position ());
    }

    bool assignable () const {
	return true;
    }

private:

    int _posn;

};

Decl*
makeParamDecl (const string& name, Decl* func, int k, AST_Ptr type)
{
    return new ParamDecl (name, func, k, type);
}


class ConstDecl : public TypedDecl {
public:

    ConstDecl (const string& name, Decl* module, AST_Ptr type)
        :  TypedDecl (name, module, type) {
        _frozen = false;
    }

protected:

    const char* decl_type_name () const {
        return "constdecl";
    }

};

Decl*
makeConstDecl (const string& name, Decl* module, AST_Ptr type)
{
    return new ConstDecl (name, module, type);
}

class InstanceDecl : public TypedDecl {
public:

    InstanceDecl (const string& name, Decl* cls, AST_Ptr type)
        :  TypedDecl (name, cls, type) {
    }

protected:

    /* FIXME: This is unsound.  We should fix it some day. */
    void set_frozen (bool freeze) {
        _frozen = freeze;
    }

    const char* decl_type_name () const {
        return "instancedecl";
    }

};

Decl*
makeInstanceDecl (const string& name, Decl* cls, AST_Ptr type)
{
    return new InstanceDecl (name, cls, type);
}

class FuncDecl : public TypedDecl {
public:

    FuncDecl (const string& name, Decl* container, AST_Ptr type,
	Environ* env)
        :  TypedDecl (name, container, type, env) {
    }

    DeclCategory getDeclCategory() {
    	return FUNC_DECL;
    }

protected:

    const char* decl_type_name () const {
        return "funcdecl";
    }

    void set_frozen (bool freeze) {
        _frozen = freeze;
    }

    Decl* add_var_decl (AST_Ptr id) {
	Decl* decl = makeLocalDecl (id->as_string (), this, Type::makeVar ());
	add_member (decl);
	return decl;
    }

    Decl* add_def_decl (AST_Ptr id) {
	Decl* decl = makeFuncDecl (id->as_string (), this, NULL);
	add_member (decl);
	return decl;
    }
};

Decl*
makeFuncDecl (const string& name, Decl* container, AST_Ptr type)
{
    return new FuncDecl (name, container, type,
			 new Environ (container->get_environ ()));
}

class MethodDecl : public FuncDecl {
public:

    MethodDecl (const string& name, Decl* container, AST_Ptr type)
        :  FuncDecl (name, container, type,
		     new Environ (container->get_environ ()->get_enclosure ())) {
    }

    DeclCategory getDeclCategory() {
    	return FUNC_DECL;
    }

protected:

    /* FIXME: This is unsound (as is get_type).  We should fix it some day. */
    Type_Ptr get_selected_type () const {
	if (_selectedType == NULL) {
	    Type_Ptr fullType = get_type ();
	    AST_Ptr newParams = consTree (TYPE_LIST);
	    for (int i = 1; i < fullType->numParams (); i += 1)
		newParams->append (fullType->paramType (i));
	    _selectedType = consTree (FUNCTION_TYPE,
				      fullType->returnType (),
				      newParams)->asType ();
	}

	if (is_frozen ())
	    return _selectedType;
	else
	    return _selectedType->freshen ();
    }

    bool is_method () const {
	return true;
    }

private:

    mutable Type_Ptr _selectedType;

};

Decl*
makeMethodDecl (const string& name, Decl* cls, AST_Ptr type)
{
    return new MethodDecl (name, cls, type);
}

class ClassDecl : public Decl {
public:

    ClassDecl (const string& name, Decl* module)
        : Decl (name, module, new Environ (module->get_environ ())) {
    }

    DeclCategory getDeclCategory() {
    	return CLASS_DECL;
    }

protected:

    Type::TypeCategory getCategory () const {
	return Type::USER_CLASS;
    }

    bool is_type () const {
	return true;
    }

    const char* decl_type_name () const {
        return "classdecl";
    }

    Type_Ptr as_type (int arity, Type_Ptr* params0) const {
        AST_Ptr* params = (AST_Ptr*) params0;
        if (get_type_arity () != -1 && get_type_arity () != arity) {
	    throw range_error ("wrong number of type parameters");
        }
        for (int i = 0; i < arity; i += 1)
            if (params[i] == NULL)
                throw domain_error ("attempt to pass null type parameter");

	AST_Ptr id = make_id (get_name ().c_str (), NULL);
	id->setDecl (const_cast<ClassDecl*> (this));

        return consTree (TYPE, id,
			 AST::make_tree (TYPE_LIST, params, params+arity))
            ->asType ();
    }

    Type_Ptr as_type (int arity, Type_Ptr t0, Type_Ptr t1) const {
        Type_Ptr params[] = { t0, t1 };
        return as_type (arity, params);
    }

    Decl* add_var_decl (AST_Ptr id) {
	Decl* decl = makeInstanceDecl (id->as_string (), this, Type::makeVar ());
	add_member (decl);
	return decl;
    }

    Decl* add_def_decl (AST_Ptr id) {
	Decl* decl = makeMethodDecl (id->as_string (), this, NULL);
	add_member (decl);
	return decl;
    }


};

Decl*
makeClassDecl (const string& name, Decl* module)
{
    return new ClassDecl (name, module);
}

class BuiltinClassDecl : public ClassDecl {
public:

    BuiltinClassDecl (const string& name, Decl* module,
		      Type::TypeCategory category, int arity)
	: ClassDecl (name, module), _category (category), _arity (arity) {
    }

    int get_type_arity () const {
	return _arity;
    }

    Type::TypeCategory getCategory () const {
	return _category;
    }

private:
    Type::TypeCategory _category;
    int _arity;
};

Decl*
makeBuiltinClassDecl (const string& name, Decl* module,
		      Type::TypeCategory category, int arity)
{
    return new BuiltinClassDecl (name, module, category, arity);
}


class ModuleDecl : public Decl {
public:

    ModuleDecl (const string& name, const Environ* enclosing)
        :  Decl (name, NULL, new Environ (enclosing)) {
    }

protected:

    const char* decl_type_name () const {
        return "moduledecl";
    }

    void print_container () const {
    }

    Decl* add_var_decl (AST_Ptr id) {
	Decl* decl = makeGlobalDecl (id->as_string (), this, Type::makeVar ());
	add_member (decl);
	return decl;
    }

    Decl* add_def_decl (AST_Ptr id) {
	Decl* decl = makeFuncDecl (id->as_string (), this, NULL);
	add_member (decl);
	return decl;
    }

};

Decl*
makeModuleDecl (const string& name, const Environ* enclosing)
{
    return new ModuleDecl (name, enclosing);
}

bool
undefinable (const string& name)
{
    return name == "None" || name == "True" || name == "False";
}

void
output_decls ()
{
    for (size_t i = 0; i < all_decls.size (); i += 1) {
	all_decls[i]->print ();
	printf ("\n");
    }
}

