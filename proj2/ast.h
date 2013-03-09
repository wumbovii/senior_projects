/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* ast.h: Top-level definitions for abstract syntax trees. */

/* Authors: */

#ifndef _AST_H_
#define _AST_H_

#include <stack>
#include <map>
#include <string>
#include <vector>
#include "horn-common.h"

class AST;
class AST_Token;
class AST_Tree;
class Type;
class Decl;

//enum for target assignment
enum TargetEnum {
	IS_TARGET,
	IS_NOT_TARGET
};

/* The Horn framework uses reference-counting to reclaim space.  The
 * type Pointer<T> is a reference-counted pointer to type T.  It acts
 * like a T* (so -> and * work), and there is an implicit coercion
 * from T* to Pointer<T>.  */

typedef Pointer<AST> AST_Ptr;
typedef Pointer<Type> Type_Ptr;
YYDEFINE_LIST_TYPE(List_Ptr, AST_Ptr);

/* General Design Note:  We have only a few base types defined in .h
 * files.  Concrete types are typically defined in .cc files because
 * in general, their type names are not expected to be used directly
 * elsewhere in the program.  Instead, we rely on having these
 * concrete classes override virtual methods as appropriate, and use
 * factory methods to find and instantiate instances of these classes
 * where needed.  Thus AST and Type declare a common behavior for all
 * their subtypes, and it is that common behavior that the rest of the
 * program relies on exclusively, minimizing inquiries into the
 * specific types of child nodes.  */


/** The base type of all nodes in the abstract syntax tree. */
class AST : public CommonNode<AST, AST_Token, AST_Tree> {
public:
    typedef std::map<std::string, AST_Ptr> AST_Map;

    virtual int lineNumber ();

    /* Project two methods */
    virtual void declare(AST_Ptr root, Decl* decl_region, TargetEnum target_enum);
    //virtual void decorate(AST_Ptr root, Decl* decl_region, std::vector<Decl*> global_decls, TargetEnum target_enum);
    virtual void decorate(AST_Ptr root, Decl* decl_region, AST_Ptr parent, int child_number);
    virtual void rewriteLambda(AST_Ptr root, bool scope_region, std::vector<AST_Ptr>* additions);
    virtual void rewriteCalls(AST_Ptr root);
    virtual Type_Ptr typeInference(AST_Ptr root);

    /** Print my representation of as an AST on OUT.  Use INDENT as the
     *  indentation for subsequent lines if my representation takes up
     *  multiple lines. */
    void print (std::ostream& out, int indent);

    /** For nodes that represent types, return the node with a static
     *  type that reveals its Type operations.  An error on node types
     *  that do not represent types. */
    virtual Type_Ptr asType ();

    /** Do semantic processing on THIS, returning modified tree. */
    virtual AST_Ptr semantics ();

    /** For nodes that represent named entities, the declaration of
     *  that entity, or NULL if not available. */
    virtual Decl* getDecl ();

    /** Set getDecl () to DECL, if allowed. */
    virtual void setDecl (Decl* d);

    /** True for a missing node. */
    virtual bool isMissing ();

    /** Return a version of me in which type variables that are
     *  supposed to denote the same variable are represented by a
     *  single node, thus transforming the tree into a directed
     *  acyclic graph.  TYPEENV maps type names in the most recent
     *  enclosing type context to type variables; update to contain
     *  any new variables in me that I define. Assumes that I am
     *  initially a tree all of whose type objects are unbound
     *  (intended to be called on a freshly constructed AST). */
    virtual AST_Ptr joinTypes (AST_Map& typeEnv);

protected:

    AST ();

    /** It is possible to have "trees" that are actually DAGS or even
     *  circular graphs (because of recursive types, for example).  We
     *  use the following trick to avoid visiting a node multiple
     *  times: At the beginning of each potentially circular
     *  traversal, increment current_mark.  Critical nodes contain a
     *  mark value (initially 0), that the traversal sets to
     *  current_mark, so that when the same node is encountered a
     *  second time during the same traversal, its mark will equal
     *  current_mark.  Alternatively, by bracketing the body of a
     *  traversal method with mark() and unmark(), we can avoid
     *  traversing cycles infinitely. */
    static int current_mark;

    /** Mark THIS with the current traversal number, and return
     *  true if this is the first visit to THIS. */
    bool mark ();

    /** Undo the mark on THIS. */
    void unmark ();

    /** Print my as an AST on OUT.  Use INDENT as the indentation for 
     *  subsequent lines if my representation takes up multiple lines.
     *  This method is intended to be called by other print methods
     *  during a traversal, whereas the public print method begins a
     *  traversal.  Nodes already printed during the current traversal
     *  are not followed. */
    virtual void _print (std::ostream& out, int indent) = 0;

    /** Print TREE on OUT at given INDENT level, being sure not to
     *  traverse circular structures.  This is the intended way that
     *  overridings of _print should recursively print their children.
     */
    static void print (AST_Ptr tree, std::ostream& out, int indent);

private:
    int _mark;
    Decl* _decl;

};

class AST_Token : public CommonToken<AST, AST_Token, AST_Tree> {
public:
    AST_Token (int syntax, const char* text, size_t len, bool owner = false)
	: CommonToken<AST, AST_Token, AST_Tree> (syntax, text, len, owner) { }

    AST_Token (int syntax, const std::string& text, bool owner)
        : CommonToken<AST, AST_Token, AST_Tree> (syntax, text, owner) { }

protected:

    /** Used to produce factory objects. */
    AST_Token (int syntax) 
        : CommonToken<AST, AST_Token, AST_Tree>(syntax) {
    }

    void _print (std::ostream& out, int indent);
};
    
class AST_Tree : public CommonTree<AST, AST_Token, AST_Tree> {
    typedef CommonTree<AST, AST_Token, AST_Tree> Parent;

public:

    NODE_BASE_CONSTRUCTORS (AST_Tree, Parent);

protected:

    /** Overrides AST::print.  Default definition of printing on
     *  trees: prints (<OP> <LINE> <CHILD0> ...), where <OP> is the 
     *  external operator name and line is the source line number. */
    void _print (std::ostream& out, int indent);

    /** External name of this type of node, for printing purposes.
     *  This is NOT intended as a way of determine the type of a
     *  node, and is valid only until the next call on externalName. */
    virtual const char* externalName ();

};

/** The supertype of all tree nodes that represent types.  We could
 *  instead define type-specific operations in AST, but that clutters
 *  ASTs with operations that apply only to certain subtypes.  This
 *  class is intended to factor out the operations specific to types. */
class Type : public AST_Tree {
    friend class TypeVar_AST;
    friend class ClassType_AST;
public:

    /** Overrides AST::asType. Effectively reveals the Type-specific
     *  operations on this node.  */
    Type_Ptr asType () {
        return this;
    }

    /** My current binding.  Initially THIS, and then changed by
     *  unification. */
    Type_Ptr binding ();

    /** Unify THIS with TYPE, returning true iff successful, and
     *  otherwise restoring all types to their original binding state
     *  and returning false. */
    bool unify (Type_Ptr type);

    /** True iff THIS would unify with TYPE.  Does not change
     *  bindings. */
    virtual bool unifies (Type_Ptr type);

    /** Return a copy of me with all unbound type variables replaced
     *  by fresh unbound variables. */
    Type_Ptr freshen ();

    /** Represents categories of type.  Used for constrained type
     *  variables. */
    enum TypeCategory { 
        NONE = 0,
        LIST = 1, 
        TUPLE = 2, 
        INT = 4,
        STR = 8,
        DICT = 16, 
        /** Denotes anything defined by "class" in the main program. */
        USER_CLASS = 32,
        FUNCTION = 64,
        UNRESTRICTED = (FUNCTION << 1) - 1
    };

    /** My category.  Returns 0 for a type variable. */
    virtual TypeCategory getCategory ();

    /** A new, unbound type variable, constrained to bind to one of
     *  the categories in ALLOWED, a bitwise "or" of TypeCategory
     *  values. */
    static Type_Ptr makeVar (int allowed = UNRESTRICTED);

    static Type_Ptr makeClassType (int allowed);

    /** The current allowed categories of binding to THIS.  The result is
     *  as for the arguments of makeVar(int). */
    virtual int getAllowed ();

    /** True iff I am a type variable. */
    virtual bool isTypeVariable ();

    /** Return a type equivalent to TYPE in which all nodes are replaced
     *  by their bindings. */
    static Type_Ptr replaceBindings (Type_Ptr type);

    NODE_BASE_CONSTRUCTORS_INIT (Type, AST_Tree, _binding (NULL));

protected:

    /** Data needed to unwind a binding. */
    struct UnwindData {
        /** The object that was bound. */
        Type_Ptr boundObject;
        /** (For type variables) previous set of allowed bindings (see
         *  the makeVar(int) method.) */
        int previousRestriction;

        void unwind () {
            boundObject->unbind (previousRestriction);
        }

        UnwindData (Type_Ptr boundObject, int previousRestriction)
            : boundObject (boundObject), 
              previousRestriction (previousRestriction) {
        }

    };

    typedef std::stack<UnwindData> UnwindStack;

    /** Unify THIS with TYPE, returning true iff successful, and
     *  otherwise false.  Pushes pairs of pointers (type object,
     *  former binding in that order) on OLD for each type object
     *  that it binds in the process, and the previous value of its
     *  binding. */
    virtual bool unify (Type_Ptr type, UnwindStack& old);

    /** Bind me to TARGET.  Returns true and pushes the old binding on
     *  OLD if successful, and returns false (and does nothing) otherwise.
     *  Requires that I be unbound. */
    virtual bool bind (Type_Ptr target, UnwindStack& old);

    /** Undo my binding, and (if applicable), return my set of
     *  allowed bindings to ALLOWED. */
    virtual void unbind (int allowed);

    /** Intersect my allowed bindings with ALLOWED, if applicable. */
    virtual void restrict (int allowed);

    /** Check that my current binding is appropriate: that I am a type
     *  variable or of the same category as my binding and that our
     *  children unify.  Add any resulting bindings to OLD. Returns
     *  true if the binding succeeds, and otherwise false. Assumes I
     *  am bound. */
    virtual bool checkBinding (UnwindStack& old);

    /** Used internally by replaceBindings to do most of the work,
     *  avoiding circular traversals. */
    Type_Ptr replaceBindings ();

    /** Used internally by freshen to perform the traversal. */
    virtual Type_Ptr _freshen ();

private:
    Type_Ptr _binding;
    Type_Ptr _forward;
};

/** Control structure: 
 *      For each child, VAR, of AST_Node* NODE, ...
 *  Usage:
 *      for_each_child (C, aTree) {
 *           <operations involving C (an AST_Node*)>
 *      } end_for;
 *  Inside the body, C_i_ is the index of C within aTree.
 */
#define for_each_child(Var, Node)                                            \
    do {                                                                     \
       AST_Ptr Var ## _n_ = Node;                                            \
       for (int Var ## _i_ = 0, Var ## _e_ = Var ## _n_->arity ();           \
            Var ## _i_ < Var ## _e_; Var ## _i_ += 1) {                      \
           const AST_Ptr Var  = Var ## _n_->child (Var ## _i_);

/** Control structure: 
 *      For each child, VAR, of AST_Node* NODE in reverse order, ...
 *  Usage:
 *      for_each_child (C, aTree) {
 *           <operations involving C (an AST_Node*)>
 *      } end_for;
 *  Inside the body, C_i_ is the index of C within aTree.
 */
#define for_each_child_reverse(Var, Node)                                    \
    do {                                                                     \
       AST_Ptr Var ## _n_ = Node;                                            \
       for (int Var ## _i_ = Var ## _n_->arity () - 1;                       \
            Var ## _i_ >= 0; Var ## _i_ -= 1) {                              \
           const AST_Ptr Var  = Var ## _n_->child (Var ## _i_);

/** Control structure: 
 *      For each child, VAR, of AST_Node* NODE, ...
 *  replacing the child with the value of VAR at the end of each iteration.
 *    
 *  Usage:
 *      for_each_child_var (C, aTree) {
 *           <operations involving C (an AST_Node*)>
 *      } end_for;
 */
#define for_each_child_var(Var, Node)                                        \
    do {                                                                     \
       AST_Ptr Var ## _n_ = Node;                                            \
       AST_Ptr Var = NULL;                                                   \
       for (int Var ## _i_ = 0;                                              \
            Var ## _i_ < (int) Var ## _n_->arity ();                         \
            _replace_and_incr (Var ## _n_, Var ## _i_, Var)) {               \
           Var = Var ## _n_->child (Var ## _i_);

#define end_for } } while (0)

/** Auxiliary function used by the for_each_child_var */
static inline int
_replace_and_incr (AST_Ptr& node, int& k, AST_Ptr& new_child)
{
    int n = new_child == NULL ? 0 
        : new_child->is_list () ? new_child->arity () 
        : 1;
    node->replace (k, new_child);
    return k += n;
}


/** Create an "id" node for the identifier TEXT, giving LOC as its
 *  location. TEXT must be a permanent string (that is, it must not
 *  get deallocated while the resulting node is in use.) */
extern AST_Ptr make_id (const char* text, const char* loc);

#endif

