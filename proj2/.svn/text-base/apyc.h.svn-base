/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* Interfaces for apyc. */

#ifndef _APYC_H_
#define _APYC_H_

#include <cstdio>
#include <string>
#include <stdexcept>
#include "ast.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>


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

/* Defined by Horn framework. */
extern std::string yyprinted_location (const char* loc);

extern AST_Ptr parse (FILE*, const std::string&);

/** True iff parser should produce debugging output. */
extern bool debugParser;

/** The --phase argument. */
extern int maxPhase;

/** DAST decl type */
enum{ LOCALD, GLOBALD, PARAMD, CONSTD, INSTANCED, FUNCD, CLASSD, MODULED };

/* Decls */

/** A declaration node, containing semantic information about a 
 *  program entity.  Subtypes of Decl refer to local variables,
 *  parameters, global variables, defined functions, methods, constants,
 *  modules, and classes. */
class Decl {
public:
    Decl (int index, const std::string& name, Decl* container) 
        : _index (index), _name (name), _container (container) { }
    int _cont_type;				//containers type - module/class/def, DO NOT CONFUSE with variable type
    int get_index () const { return _index; }
    const std::string& get_name () const { return _name; }



    /** Print THIS on the standard output. */
    virtual void print (std::ostream& out);

    /** gets the index list of a class/module/def */
    char* get_indexlist();

    /** get the name */
    virtual std::string get_name();

    /** sets the DAST decl type */
    void set_dast_type(int t);

    /** get dast decl type */
    virtual int get_dast_type();

    /** Set the Decorated AST string represenation */
    virtual void set_printable();

    /** set the unique index for a decl */
    virtual void set_index(int i);

    /** get the unique index for decl */
    virtual int get_index();

    /**  set the container type for decl   */
    virtual void set_cont_type(int cont_type);

    /**  set the container type for decl   */
    virtual int get_cont_type();

    /** Get my container (null if none). */
    virtual Decl* get_container () { return _container; }

    /** Get my type. */
    virtual AST_Ptr get_type ();

    /** Set my type to TYPE. */
    virtual void set_type (AST_Ptr type);

    /** set param position */
    void set_position (int k);

    /** Get my position (in a parameter list, indexed from 0). */
    virtual int get_position ();

    /** A view of all member Decls I contain (modules, classes). */
    virtual const std::vector<Decl*>& get_members ();

    /** check if a given Decl* is part of members */
    virtual bool contains_member(Decl* decl);
    virtual bool contains_member(std::string name, int dast_type);

    /** Add DECL to the end of my list of member Decls. */
    virtual void add_member (Decl* new_member);

    /** If I represent a type, return what category of type, and
     *  otherwise the category NONE. */
    virtual Type::TypeCategory getCategory ();

    virtual ~Decl ();

private:
    int _index;					//unique identification index
    const std::string _name;	//the name of the variable (an ID)
    Decl* _container;			//the module/class/def 'this' is contained in

    AST_Ptr _type;				//semantic type
    int _dast_type;			//DAST type ie: localdecl, globaldecl, classdecl etc
    std::string _dast_rep; 		//DAST representation of this
    int _param_pos;				//the parameter pos if it exists

    std::vector<Decl*> _members; //all 'children' or other decs contained within 'this'



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
extern Decl* makeClassDecl (const std::string& name,
                            Decl* module, Decl* supertype);
extern Decl* makeModuleDecl (const std::string& name);


typedef std::vector<Decl*> Decl_Vector;

/** Output all Decl nodes to standard output. */
extern void output_decls (std::ostream& out, std::vector<Decl*> decl_set);


class Environ {
public:

    /** An initialiy empty environment enclosed in ENCLOSING. */
    Environ (const Environ* enclosing);

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

/** The current outer (__main__) environment */
extern const Environ* outer_environ;

/** The environment of __builtin__. */
extern const Environ* builtin_environ;




#endif

