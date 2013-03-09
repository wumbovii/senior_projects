/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* Interfaces for apyc. */

#ifndef _APYC_H_
#define _APYC_H_

#include <iostream>
#include <cstdio>
#include <string>
#include <stdexcept>
#include "ast.h"


class Decl;
class Environ;

class semantic_error : public std::runtime_error {
public:
    explicit semantic_error (const std::string& s) : std::runtime_error (s)
        { }
};

#define UNIMPLEMENTED(name) \
    throw logic_error ("unimplemented method: " #name)

/** Print an error message in standard Unix format:
 *      <filename>:<linenum>: <message>
 *  where <filename> and <linenum> denote the file and line number
 *  containing source location LOC, and <message> is composed from
 *  FORMAT and the trailing arguments as
 *  for the printf family. */
extern void Error (const char* loc, const char* format, ...);
/** Print an error message (without file or line number indications)
 *  composed from FORMAT and the trailing arguments as for the printf
 *  family. */
extern void ErrorNoFile (const char* format, ...);
/** As for Error(const char* loc,...) above, but get location from
 *  LOCAST. */
extern void Error (const AST_Ptr& locAST, const char* format, ...);

/* Defined by Horn framework. */
extern std::string yyprinted_location (const char* loc);

extern AST_Ptr parse (FILE*, const std::string&);


/* The following were added by our team for codeGen purposes */
extern AST_Ptr popQueue();
extern void pushQueue(AST_Ptr ptr);
extern Decl* popFrameQueue();
extern void pushFrameQueue(Decl* ptr);


extern bool declareMode;
extern int indent_count;
extern std::string indent(int i = 0);
extern void dedent();

extern bool nestedMode;
extern bool classMode;
extern bool callMode;
extern bool functionCastMode;
extern std::string functionCastName;
extern int tempVarCount;

extern bool statementMode;


/** True iff parser should produce debugging output. */
extern bool debugParser;

/** The --phase argument. */
extern int maxPhase;

/* Decls */

enum DeclCategory {
	FUNC_DECL,
	PARAM_DECL,
	CLASS_DECL,
	LOCAL_DECL,
	GLOBAL_DECL
};

/** A declaration node, containing semantic information about a 
 *  program entity.  Subtypes of Decl refer to local variables,
 *  parameters, global variables, defined functions, methods, constants,
 *  modules, and classes. */
class Decl {
public:

    Decl (const std::string& name, Decl* container, 
          Environ* members = NULL);
        
    int get_index () const { return _index; }

    const std::string& get_name () const { return _name; }

    /** Print THIS on the standard output. */
    virtual void print () const;

    /** Get my container (null if none). */
    virtual Decl* get_container () const { return _container; }

    /** Get the type of the entity I represent. */
    virtual Type_Ptr get_type () const;

    /** Get the type of the entity I represent when it is selected
     *  from an instance. */
    virtual Type_Ptr get_selected_type () const;

    /** Set my type to TYPE. */
    virtual void set_type (Type_Ptr type);

    /** True iff I represent a method. */
    virtual bool is_method () const;

    /** True iff I represent a type. */
    virtual bool is_type () const;

    /** A Type representing me with ARITY (0 <= ARITY <=2) type parameters
     *  taken from T0 and T1.  Requires is_type(). */
    virtual Type_Ptr as_type (int arity = 0,
                              Type_Ptr t0 = NULL, Type_Ptr t1 = NULL) const;

    /** A Type representing me with parameters PARAMS[0 .. ARITY-1]. */
    virtual Type_Ptr as_type (int arity, Type_Ptr* params) const;

    /** If I am type, returns the number of type arguments, or -1 for
     *  type constructors with variable arity. */
    virtual int get_type_arity () const;

    /** Get my position (in a parameter list, indexed from 0). */
    virtual int get_position () const;

    /** A view of all member Decls I contain (modules, classes). */
    virtual const Environ* get_environ () const;

    /** Add DECL to the end of my list of member Decls. */
    virtual void add_member (Decl* new_member);

    /** Create a declaration of ID appropriate for a variable declaration to
     *  my members.  There must not already be one. Returns the
     *  resulting declaration */
    virtual Decl* add_var_decl (AST_Ptr id);

    /** Create a declaration of ID appropriate for a 'def' to my
     *  members.  There must not already be one.  Returns the
     *  resulting declaration. */
    virtual Decl* add_def_decl (AST_Ptr id);

    /** If I represent a type, return what category of type, and
     *  otherwise the category NONE. */
    virtual Type::TypeCategory getCategory () const;

    /** True iff the entity I represent can be assigned to or defined
     *  by assignment. */
    virtual bool assignable () const;

    /** True iff my type is currently "frozen", meaning that get_type is
     *  not to freshen type variables in it. */
    virtual bool is_frozen () const;

    /** If my freezing status is mutable, set it to FROZEN.
     *  Otherwise, does nothing. */
    virtual void set_frozen (bool frozen);

    /** Methods for generating frames during codegen. */
    virtual DeclCategory getDeclCategory();
    virtual void genFrames(std::ostream& out);
    virtual void genForwards(std::ostream& out);


    virtual ~Decl ();

protected:

    /** The name of this type of Decl for external printing purposes. */
    virtual const char* decl_type_name () const;

    /** Print my container's index on the standard output, if
     *  applicable, and otherwise do nothing. */
    virtual void print_container () const;

    /** Print my position in my container, if applicable, on the
     *  standard output, and otherwise do nothing. */
    virtual void print_position () const;

    /** Print my type on the standard output, if applicable, and
     *  otherwise do nothing. */
    virtual void print_type () const;

    /** Print my index list on the standard output, if applicable, and
     *  otherwise do nothing. */
    virtual void print_index_list () const;

    /** Add me to the list of all Decls and set my index. */
    void register_decl ();

protected:
    bool _frozen;

private:
    int _index;
    const std::string _name;
    Decl* _container;
    Environ* _members;
};

extern Decl* makeLocalDecl (const std::string& name,
                            Decl* func, AST_Ptr type);
extern Decl* makeParamDecl (const std::string& name,
                            Decl* func, int k, AST_Ptr type);
extern Decl* makeGlobalDecl (const std::string& name,
                             Decl* module, AST_Ptr type);
extern Decl* makeConstDecl (const std::string& name,
                            Decl* module, AST_Ptr type);
extern Decl* makeInstanceDecl (const std::string& name, Decl* clas,
                               AST_Ptr type);
extern Decl* makeFuncDecl (const std::string& name,
                           Decl* container, AST_Ptr type);
extern Decl* makeClassDecl (const std::string& name, Decl* module);
extern Decl* makeBuiltinClassDecl (const std::string& name,
                                   Decl* module, Type::TypeCategory category,
                                   int arity);
extern Decl* makeModuleDecl (const std::string& name, const Environ* enclosing);

/** True iff NAME may not be defined or set by assignment. */
extern bool undefinable (const std::string& name);

/** Output all Decl nodes to standard output. */
extern void output_decls ();

/** Decls for built-in types. */
extern Decl* intDecl;
extern Decl* listDecl;
extern Decl* tupleDecl;
extern Decl* strDecl;
extern Decl* dictDecl;
extern Decl* boolDecl;
extern Decl* fileDecl;
extern Decl* rangeDecl;

typedef std::vector<Decl*> Decl_Vector;

class Environ {
public:

    /** An initialiy empty environment enclosed in ENCLOSING. */
    Environ (const Environ* enclosing = NULL);

    /** An environment initialized to the (immediate) contents of 
     *  ENVIRON0, enclosed in enclosing. */
    Environ (const Environ* environ0, const Environ* enclosing);

    /** My entry for NAME, or NULL if none.  Does not search enclosing
     *  Environs. */
    Decl* find_immediate (const std::string& name) const;

    /** The entry for NAME in me or enclosing Environs, or NULL if
     *  none. */
    Decl* find (const std::string& name) const;

    /** Define DECL in me by its name.  */
    void define (Decl* decl);

    /** The environment enclosing me (passed to my constructor), if
     *  any. */
    const Environ* get_enclosure () const;

    /** A view of my members (not including enclosing Environs). */
    const Decl_Vector& get_members () const;

private:

    Decl_Vector members;
    const Environ* enclosure;
};

/** Make sure builtin definitions, builtin module, and builtin_environ
 *  are initialized. */
extern void initBuiltin ();

/** The current outer (__main__) environment */
extern const Environ* outer_environ;

/** The environment of __builtin__. */
extern const Environ* builtin_environ;

#endif

