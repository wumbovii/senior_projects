/* Builtin definitions */

/* Authors:  YOUR NAMES HERE */
/* From a skeleton by Paul N. Hilfinger */

#include <stdexcept>
#include "apyc.h"
#include "apyc-parser.hh"

using namespace std;

/* Modules */

static Decl* builtinModule = makeModuleDecl ("__builtin__", NULL);

/* Classes */

static Decl*
defineBuiltinClass (const string& name, Type::TypeCategory category, int arity) 
{
    Decl* decl = makeBuiltinClassDecl (name, builtinModule, category, arity);
    builtinModule->add_member (decl);
    return decl;
}


Decl* intDecl;
Decl* listDecl;
Decl* tupleDecl;
Decl* strDecl;
Decl* dictDecl;
Decl* boolDecl;
Decl* fileDecl;
Decl* rangeDecl;

/* Functions */

static Decl*
defineBuiltinDef (const string& name, Type_Ptr returnType,
                  int arity,
                  Type_Ptr param0 = NULL, Type_Ptr param1 = NULL) {
    AST_Ptr params[] = { param0, param1 };

    if (arity < 0 || arity > 2 || (arity > 0 && param0 == NULL)
        || (arity > 1 && param1 == NULL) || returnType == NULL)
        throw domain_error ("bad arguments to defineBuiltinDef");
    
    Type_Ptr type = 
        consTree (FUNCTION_TYPE, returnType,
                  AST::make_tree (TYPE_LIST, params, params+arity))->asType ();
    
    Decl* decl = makeFuncDecl (name, builtinModule, type);
    decl->set_frozen (false);
    builtinModule->add_member (decl);
    return decl;
}

Decl* appendDecl;
Decl* lenDecl;
Decl* openDecl;
Decl* closeDecl;
Decl* xrangeDecl;

/* Constants */

static Decl*
defineBuiltinConst (const string& name, Type_Ptr type) {
    Decl* decl = makeConstDecl (name, builtinModule, type);
    builtinModule->add_member (decl);
    return decl;
}

Decl* NoneDecl;
Decl* TrueDecl;
Decl* FalseDecl;
Decl* stdoutDecl;
Decl* stdinDecl;
Decl* stderrDecl;
Decl* argvDecl;

void
initBuiltin ()
{
    builtinModule = makeModuleDecl ("__builtin__", NULL);

    intDecl = defineBuiltinClass ("int", Type::INT, 0);
    listDecl = defineBuiltinClass ("list", Type::LIST, 1);
    tupleDecl = defineBuiltinClass ("tuple", Type::TUPLE, -1);
    strDecl = defineBuiltinClass ("str", Type::STR, 0);
    dictDecl = defineBuiltinClass ("dict", Type::DICT, 2);
    boolDecl = defineBuiltinClass ("bool", Type::BOOL, 0);
    fileDecl = defineBuiltinClass ("file", Type::USER_CLASS, 0);
    rangeDecl = defineBuiltinClass ("range", Type::XRANGE, 0);

    Type_Ptr append1 = Type::makeVar (),
        append2 = listDecl->as_type (1, append1);
    appendDecl = defineBuiltinDef ("append", append2, 2, append2, append1);

    Type_Ptr len1 = Type::makeVar (Type::LIST|Type::DICT|Type::STR);
    lenDecl = defineBuiltinDef ("len", intDecl->as_type (), 1, len1);

    openDecl = defineBuiltinDef ("open", fileDecl->as_type (), 2,
                                 strDecl->as_type (), strDecl->as_type ());

    closeDecl =
        defineBuiltinDef ("close", Type::makeVar (), 1, fileDecl->as_type ());

    xrangeDecl = 
        defineBuiltinDef ("xrange", rangeDecl->as_type (), 2,
                          intDecl->as_type (), intDecl->as_type ());

    NoneDecl = defineBuiltinConst ("None", Type::makeVar ());
    TrueDecl = defineBuiltinConst ("True", boolDecl->as_type ());
    FalseDecl = defineBuiltinConst ("False", boolDecl->as_type ());
    stdoutDecl = defineBuiltinConst ("stdout", fileDecl->as_type ());
    stdinDecl = defineBuiltinConst ("stdin", fileDecl->as_type ());
    stderrDecl = defineBuiltinConst ("stderr", fileDecl->as_type ());
    argvDecl = defineBuiltinConst ("argv", 
                                   listDecl->as_type (1, strDecl->as_type ()));

    builtin_environ = builtinModule->get_environ ();
}

