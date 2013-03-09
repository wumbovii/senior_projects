/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* apyc: A PYthon Compiler. */

/* Authors:  YOUR NAMES HERE */

#include <cstdio>
#include <cstdlib>
#include <queue>
#include <string>
#include <cstring>
#include <cstdarg>
#include <sstream>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include "apyc.h"

using namespace std;

/** Extension of source files produced by phases 3-5 of the
 *  compiler. Change if target language changes from C++. */
const string TARGET_SUFFIX = ".cc";

/** Prefix of command used to compile or link.  Chnage if target
 *  language changes from C++. */
const string COMMAND_PREFIX = "g++";

/** Runtime directory suffix. */
const string RUNTIME_DIR = "/lib/apyc/";

/** Base name of runtime library source file. */
const string RUNTIME_LIBRARY = "runtime.cc";

/** Number of times Error or ErrorNoFile have been called. */
int errCount;
/** Name of input file (for error messages). */
static string fileName;
/** Value specified by --phase or other command-line arguments. */
int maxPhase;

/** Directory containing runtime support library. */
string libDir;

/** True if extra output requested. */
bool verbose;

queue<AST_Ptr> codegen_q;
queue<Decl*> framegen_q;

Decl* popFrameQueue() {
	if (!framegen_q.empty()) {
		Decl* next = framegen_q.front();
		framegen_q.pop();
		return next;
	}
	else return NULL;
}

void pushFrameQueue(Decl* ptr) {
	framegen_q.push(ptr);
}


/** This is our codegen queue. */
AST_Ptr popQueue() {
	if (!codegen_q.empty()) {
		AST_Ptr next = codegen_q.front();
		codegen_q.pop();
		return next;
	}
	else return NULL;
}

void pushQueue(AST_Ptr ptr) {
	codegen_q.push(ptr);
}

bool declareMode = true;
bool nestedMode = false;
bool classMode = false;
bool callMode = false;
bool functionCastMode = false;
string functionCastName = "";
bool statementMode = false;

bool assignSubMode = false;
string assignSubName;

int indent_count = 0;
int tempVarCount = 0;

string indent(int i) {
	string ret = "";
	indent_count += i;
	for (int j = 0; j < indent_count; j++) {
		ret += " ";
	}
	return ret;
}

void dedent() {
	indent_count -= 2;
}


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
Error (const AST_Ptr& locAST, const char* format, ...)
{
    va_list ap;

    errCount += 1;

    if (locAST != NULL) {
        if (locAST->isErroneous ())
            return;
        locAST->markErroneous ();
    }

    va_start (ap, format);
    fprintf (stderr, "%s: ", yyprinted_location (locAST->loc ()).c_str ());
    vfprintf (stderr, format, ap);
    va_end (ap);
    fprintf (stderr, "\n");
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
Usage: apyc [--phase=(1 | 2 | 3 | 4 | 5)] [ -dp ] [-c] [-S] \\\n\
            [ -o OUTFILE ] [ -v | --verbose ] DIR/BASE.py\n");
    exit (1);
}

/** As for the 'basename' command: NAME with any leading directory
 *  components removed.  */
static string
basename (const string& name)
{
    size_t start = name.rfind ('/');
    if (start == string::npos)
        return name;
    else
        return name.substr (start + 1);
}

/** NAME with any non-directory suffix removed, or "." NAME contains no
 *  directory prefix. */
static string
dirname (const string& name)
{
    size_t start = name.rfind ('/');
    if (start == string::npos)
        return ".";
    else
        return name.substr (0, start);
}


/** Assuming PROGRAMNAME is the pathname of this executable, and that
 *  the runtime library directory is in the same directory, returns
 *  the path to the directory containing the runtime support library. */
static string
defaultLibDir (const string& programName)
{
    return dirname (programName) + RUNTIME_DIR;
}

/** Returns the name of a temporary file ending in SUFFIX. */
static string
getTemp (const string& suffix)
{
    struct stat info;
    for (int i = 0; i < 1024; i += 1) {
        stringstream name;
        name << "/tmp/apyc" << i << suffix;
        if (stat (name.str ().c_str (), &info) != 0 
            && errno == ENOENT)
            return name.str ().c_str ();
    }
    ErrorNoFile ("fatal: could not get temporary file");
    exit (1);
}
        
/** Parse file named INPUT, returning the resulting AST. */
static AST_Ptr
parseToAST (const string& input)
{
    fileName = input;
    FILE* inFile = fopen (input.c_str (), "r");
    if (inFile == NULL) {
	ErrorNoFile ("Could not open %s", input.c_str ());
	return NULL;	
    }
    if (verbose)
        cerr << "Parsing..." << endl;
    AST_Ptr tree = parse (inFile, input);
    fclose (inFile);
    return tree;
}

/** Print external representation of TREE on file named OUTPUT. */
static void
printTree (AST_Ptr tree, const string& output)
{
    if (freopen (output.c_str (), "w", stdout) == NULL) {
	ErrorNoFile ("Could not open %s", output.c_str ());
	return;	
    }
    tree->print (cout, 0);
    cout << endl;
    if (maxPhase >= 2)
        output_decls ();
    cout.flush ();
    fclose (stdout);
}

/** Generate code from TREE (processed by phase 2) to the file named
 *  OUTPUT. */
static void
codeGen (AST_Ptr tree, const string& output)
{

    if (freopen (output.c_str (), "w", stdout) == NULL) {
        ErrorNoFile ("Could not write to %s", output.c_str ());
        return;
    }
    tree->codeGen (cout, NULL);

    cout.flush ();
    fclose (stdout);
}

/** Compile the C/C++ file INPUT to object file OUTPUT.  Uses the
 *  runtime library directory for include files. */
static void
compileToObject (const string& input, const string& output)
{
    stringstream command;
    command << COMMAND_PREFIX << " -g -c -I" << libDir 
            << " -o " << output << " " << input;
    if (verbose)
        cerr << command.str () << endl;
    int code = system (command.str ().c_str ());
    if (code != 0)
        ErrorNoFile ("C/C++ compilation failed.");
}

/** Compile the C/C++ file INPUT and the runtime support library to 
 *  executable file OUTPUT.  Also uses the runtime library directory
 *  for include files. */
static void
compileToExecutable (const string& input, const string& output)
{
    stringstream command;
    command << COMMAND_PREFIX << " -g -I" << libDir 
            << " -o " << output << " " << input 
            << " " << libDir << RUNTIME_LIBRARY;
    if (verbose)
        cerr << command.str () << endl;
    int code = system (command.str ().c_str ());
    if (code != 0)
        ErrorNoFile ("C/C++ compilation failed.");
}

int
main (int argc, char* argv[])
{
    int i;
    char* outfile;
    maxPhase = 5;
    outfile = NULL;
    verbose = false;

    libDir = defaultLibDir (argv[0]);

    for (i = 1; i < argc; i += 1) {
	string opt = argv[i];
	if (opt == "-o" && i < argc-1) {
	    outfile = argv[i+1];
	    i += 1;
	} else if (opt.compare (0, 8, "--phase=") == 0)
	    maxPhase = atoi(opt.c_str () + 8);
	else if (opt == "-dp") 
	    debugParser = true;
        else if (opt == "-S")
            maxPhase = 3;
        else if (opt == "-c")
            maxPhase = 4;
        else if (opt == "-v" || opt == "--verbose")
            verbose = true;
	else if (opt.size () == 0 || opt[0] == '-')
	    Usage();
	else
	    break;
    }
    if (i != argc-1 || maxPhase < 1 || maxPhase > 5)
	Usage ();
    errCount = 0;
    string infile = argv[i];

    if (verbose)
        cerr << "apyc version 0.1" << endl;

    if (infile.size () < 4 
        || infile.compare (infile.size ()-3, 3, ".py") != 0) {
        ErrorNoFile ("Unknown file type: %s\n", argv[i]);
        exit (1);
    }

    string outfileStr, compilerOutStr;
    if (outfile == NULL) {
        outfileStr = string(infile, 0, infile.size ()-3);
        switch (maxPhase) {
        case 1:
            outfileStr += ".ast";
            break;
        case 2:
            outfileStr += ".dast";
            break;
        case 3:
            outfileStr = basename (outfileStr) + TARGET_SUFFIX;
            break;
        case 4:
            outfileStr = basename (outfileStr) + ".o";
            break;
        case 5:
            outfileStr = "a.out";
        }
    } else
        outfileStr = outfile;

    switch (maxPhase) {
    case 1: case 2: case 3:
        compilerOutStr = outfileStr;
        break;
    case 4:
        compilerOutStr = getTemp (TARGET_SUFFIX);
        break;
    case 5:
        compilerOutStr = getTemp (TARGET_SUFFIX);
        break;
    }

    AST_Ptr tree = parseToAST (infile);

    if (maxPhase >= 2 && tree != NULL && errCount == 0) {
        if (verbose)
            cerr << "Semantic processing ..." << endl;
        tree = tree->semantics ();
    }
    if (maxPhase <= 2 && errCount == 0)
        printTree (tree, outfileStr);

    if (maxPhase > 2 && errCount == 0) {
        if (verbose)
            cerr << "Code generation ..." << endl;
        codeGen (tree, compilerOutStr);
    }
    
    if (maxPhase == 4 && errCount == 0) {
        compileToObject (compilerOutStr, outfileStr);
    } else if (maxPhase == 5 && errCount == 0) {
        compileToExecutable (compilerOutStr, outfileStr);
    }

    if (compilerOutStr != outfileStr)
        remove (compilerOutStr.c_str ());

    exit (errCount == 0 ? 0 : 1);
}

/* Debugging routines.  These are never called in the skeleton.  They
 * are intended to be called from GDB, as in 
 *     (gdb) call DB(aTree)
 */

/** Dump X on standard error. */
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

/** Dump binding of X on standard error. */
void
DBB(Type* x)
{
    if (x != NULL)
        x = x->binding ().data ();
    DB(x);
}

void
DBB(const Type_Ptr& x)
{
    if (x != NULL)
        DB (x->binding ());
    else
        DB(x);
}

