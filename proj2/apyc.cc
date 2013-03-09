AST_Ptr	left_op1 = root->child(0);
			AST_Ptr right_op2 = root->child(2);

			Type_Ptr op1 = typeInference(left_op1, env);
			Type_Ptr op2 = typeInference(right_op2, env);

			string op = root->child(1)->as_string();

			if (op == "notin" || op == "in") {
				// op1:$a in op2: list($a)|dict($a $c)
				Type_Ptr rule = Type::makeVar(Type::LIST | Type::DICT);
				result &= rule->unify(op2->binding());
				if(!result){
					//op2 must be of type dict or list
					errorCall("NOTIN/IN: second operand is not of type LIST or DICT");
				}

				//check to see if the list(type) or dict(type:value) match
				if(op2->getCategory() == Type::DICT){
					//if dictionary type, check to see if key/op1 are same type
					//comparison->(id expr)+->dict_display
					if(right_op2->arity() == 0){
						//No pairs exists, this means the dict is empty and => unbound
						rule = Type::makeVar(Type::NONE);
						result &= op1->unify(rule);
						my_type = op1->binding();
					} else {
						//at least one pair, find the key
						AST_Ptr pair = right_op2->child(0);
						Type_Ptr pair_type = typeInference(pair, env);
						if(pair_type->unify(op1->binding()) ||
									op1->unify(pair_type->binding())){
							//unify succeeds
							my_type = op1->binding();
							break;
						} else {
							//unify fails
							errorCall("Type mismatch error in Dict");
						}
					}
				} else {
					//else is list, check to see if list(elem)/op1 are same type

				}


			} else {
				//< <= >= > == != is isnot <>
				//ops must be of part str int bool
				Type_Ptr rule = Type::makeVar(Type::INT | Type::STR | Type::NONE);
			}
			/** Now check that the types are the same */
			if (!op1->unifies(op2)) {
				errorCall(root->loc(), "Type mismatch error on a binop");
			}
			break;
/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* apyc: A PYthon Compiler. */

/* Authors:  YOUR NAMES HERE */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdarg>
#include "apyc.h"
//#include "modules.cc"

using namespace std;

int errCount;
static string fileName;
int maxPhase;
extern std::vector<Decl*> decl_set;
extern void output_decls (std::ostream& out, std::vector<Decl*> decl_set);

void
Error (const char* loc, const char* format, ...)
{
    va_list ap;
    va_start (ap, format);
    fprintf (stderr, "%s: ", yyprinted_location (loc).c_str ());
    vfprintf (stderr, format, ap);
    va_end (ap);
    fprintf (stderr, "\n");
    errCount += 1;
}

void
ErrorNoFile (const char* format, ...) 
{
    va_list ap;
    va_start (ap, format);
    vfprintf (stderr, format, ap);
    va_end (ap);
    fprintf (stderr, "\n");
    errCount += 1;
}

static void
Usage ()
{
    fprintf (stderr, "\
Usage: apyc --phase=(1 | 2) [ -dp ] [ -o OUTFILE ] DIR/BASE.py\n");
    exit (1);
}

static AST_Ptr
compile (const string& input)
{
    fileName = input;
    FILE* inFile = fopen (input.c_str (), "r");
    if (inFile == NULL) {
	ErrorNoFile ("Could not open %s", input.c_str ());
	return NULL;	
    }
    AST_Ptr tree = parse (inFile, input);
    fclose (inFile);
    return tree;
}

static void
printDecl (string& output, std::vector<Decl*> decl_set)
{
    if (freopen (output.c_str (), "a", stdout) == NULL) {
	ErrorNoFile ("Could not open %s", output.c_str ());
	return;
    }
    cout << "\n";
    output_decls (cout, decl_set);
}


static void
printTree (AST_Ptr tree, string& output)
{
    if (freopen (output.c_str (), "w", stdout) == NULL) {
	ErrorNoFile ("Could not open %s", output.c_str ());
	return;	
    }
    tree->print (cout, 0);
}

int
main (int argc, char* argv[])
{
    int i;
    char* outfile;
    maxPhase = -1;
    outfile = NULL;
    for (i = 1; i < argc; i += 1) {
	string opt = argv[i];
	if (opt == "-o" && i < argc-1) {
	    outfile = argv[i+1];
	    i += 1;
	} else if (opt.compare (0, 8, "--phase=") == 0)
	    maxPhase = atoi(opt.c_str () + 8);
	else if (opt == "-dp") 
	    debugParser = true;
	else if (opt.size () == 0 || opt[0] == '-')
	    Usage();
	else
	    break;
    }
    if (i != argc-1 || maxPhase < 1 || maxPhase > 2)
	Usage ();
    errCount = 0;
    string infile = argv[i];

    if (infile.size () < 4 
        || infile.compare (infile.size ()-3, 3, ".py") != 0) {
        ErrorNoFile ("Unknown file type: %s\n", argv[i]);
        exit (1);
    }

    string outfileStr;
    if (outfile == NULL)
        outfileStr = string(infile, 0, infile.size ()-3) 
            + (maxPhase == 1 ? ".ast" : ".dast");
    else
        outfileStr = outfile;

    AST_Ptr tree = compile (infile);

    if (maxPhase == 2 && tree != NULL && errCount == 0)
        tree = tree->semantics ();
    if (errCount == 0) {
        printTree (tree, outfileStr);
        printDecl(outfileStr, decl_set);
    }

    exit (errCount == 0 ? 0 : 1);
}

/* Debugging routines.  These are never called in the skeleton.  They
 * are intended to be called from GDB, as in 
 *     (gdb) call DB(aTree)
 */

void
DB(const AST_Ptr& x)
{
    AST::dump(x, cerr);
    cerr << endl;
}
void
DB(const Type_Ptr& x)
{
    AST::dump(x, cerr);
    cerr << endl;
}
void
DB(AST* x)
{
    AST::dump(x, cerr);
    cerr << endl;
}
void
DB(Type* x)
{
    AST::dump(x, cerr);
    cerr << endl;
}
