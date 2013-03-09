/* runtime.h: */

/* Purpose: A PYthon Compiler uses runtime.h/runtime.cc
 * as a guideline for specific instantiations for the C++
 * immediate language for the python code
 *
 * Contains:
 * 	"Interfaces" for runtime.cc
 *
 * 	 */

/* Authors:  Wiser */
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <stdarg.h>
using namespace std;

/* PyObject */
class PyObject;

/* Data Structures */
class PyDict;
class PyKey;
class PyList;
class PyXrange;
class PyTuple;
class PyFile;
class PyFunc;
class PyClass;

/* Primitives */
class PyStr;
class PyBool;
class PyInt;
class PyNone;

/* All PyObjects have one of the following types */
enum PyType {
	PYDICT,
	PYKEY,
	PYLIST,
	PYXRANGE,
	PYTUPLE,
	PYFILE,
	PYFUNC,
	PYCLASS,
	PYSTR,
	PYBOOL,
	PYINT,
	PYNONE
};

/*
 *
 * Top level class to emulate something like Java's Object class */
class PyObject {

public:
	/* Returns the type of 'this' */
	PyType get_myType(){ return myType; };

//	void print(ostream& out = cout) {
//		return;
//	}

	virtual void print(ostream& out = cout)=0;

//	virtual PyObject* get(PyObject& o)=0;
//	virtual PyObject* set(PyObject& index, PyObject& o)=0;
	PyObject* logical(const char* op, PyObject* target);

protected:
	PyType 	myType;			//ENUM type of 'this' ie: DICT or INT etc.

};

class Gettable {

public:
	virtual PyObject* get(PyObject* o)=0;
};

class Settable {

public:
	virtual PyObject* set(PyObject* index, PyObject* o)=0;
};



/*
 *
 * Integer Object class*/
class PyInt : public PyObject {

public:
	/* Creates a PyInt object with its number being num */
	PyInt(int num) {
		myNum = num;
		myType = PYINT;
	};

	/* Prints out 'this' to the Intermediate Language "object" file
	 * which is in the form of: [compileTarget].[targetSuffix]
	 */
	void print(ostream& out = cout);

	int value() { return myNum; };

	/* returns myNum */
	int get_myNum() { return myNum; };

	/* sets myNum */
	void set_myNum(int num) { myNum = num; };

	/* returns the resulting PyInt object */
	PyInt* binop(const char* op, PyObject* target);

	/* returns resulting PyInt object for a unop (-, +*/
	PyObject* unop(const char* op);

	/* Basic arithmetic comparison for the wrapper, returns true/false */
	PyBool* compare(const char* op, PyObject* target);


private:
	int				myNum;		//the number 'this'

};



/*
 *
 * String Object class*/
class PyStr : public PyObject, public Gettable {

public:
	/* Creates a PyStr object with its number being str */
	PyStr(const std::string& str) {
		myStr = str;
		myLen = str.size();
		myType = PYSTR;
	};

	/* Prints out 'this' to the Intermediate Language "object" file
	 * which is in the form of: [compileTarget].[targetSuffix]
	 */
	void print(ostream& out = cout);

	std::string& value() { return myStr; };

	/* returns myStr */
	const std::string& get_myStr() { return myStr; };

	/* sets myStr */
	void set_myStr(const std::string& str) { myStr = str; };

	/* returns the resulting PyStr object
	 * This would be equivalent to concatenation if "+"
	 * """"""""""""""""""""""""""""multi append  if "*" */
	/* binop operator for strings */
	/* target is either an PyInt or PyStr depending on op */
	PyStr* binop(const char op[], PyObject* target);

	/* return a vector of the underlying characters to serve as an iterator */
	std::vector<char> iterator();

	/* returns the character at i */
	char charAt(int i) { return myStr.at(i); };

	/* returns the character at index i (same as charAt) */
	PyObject* get(PyObject* o);

	/* check if given index is in the range of the internal string */
	bool inRange(int index);

	/* returns string size/length */
	PyInt* len() { return (new PyInt(myLen)); };

	/* returns a slice of the string , start indexed at 0 */
	PyStr* slice(PyInt* start, PyInt* stop);

	/* Basic arithmetic comparison for the wrapper, returns true/false */
	PyBool* compare(const char* op,  PyObject* target);


private:
	std::string	myStr;		//the string of 'this'
	int			myLen;		//length of 'this'

};




/*
 *
 * Bool Object class*/
class PyBool : public PyObject {

public:
	/* Creates a PyBool object with its number being boolean */
	PyBool(bool boolean) {
		myBool = boolean;
		myType = PYBOOL;
	};

	/* Prints out 'this' to the Intermediate Language "object" file
	 * which is in the form of: [compileTarget].[targetSuffix]
	 */
	void print(ostream& out = cout);

	bool value() { return myBool; };


	/* Basic arithmetic comparison for the wrapper, returns true/false */
	PyBool* compare(const char* op, PyBool* target);

	/* logical algebra and returns resulting Pybool */
	PyBool* logical(const char* op, PyBool* target) ;

	/* unop NOT, returns resulting pybool */
	PyBool* unop(const char* op);

private:
	bool	myBool;		//the boolean of  'this'
};

/*
 * List Object class
 */
class PyList : public PyObject, public Gettable, public Settable {

public:
	PyList(std::vector<PyObject*> list) {
		myList = list;
		myType = PYLIST;
	}

	PyList(int count, ...) {
		myType = PYLIST;

		va_list pyObs;
		va_start(pyObs, count);
		for(int i = 0; i < count; i++) {
			myList.push_back(va_arg(pyObs, PyObject*));
		}
		va_end(pyObs);
	}

	std::vector<PyObject*> value() { return myList; };
	std::vector<PyObject*>* valuep() { return &myList; };

	/* check if given index is in the range of the internal vector */
	bool inRange(int index);

	/* get a slice of the list */
	PyList* slice(PyInt* start, PyInt* stop);

	/* return a vector that serves as an iterator of the list */
	virtual std::vector<PyObject*> iterator();

	/* append object to the end of the list */
	bool add(PyObject* o);

	/* Inserts the specified element at the specified position in this list */
	void add(int index, PyObject* o);

	/* Returns true if this list contains the specified element. */
	bool contains(PyObject* o);

	/* Returns the element at the specified position in this list. */
	PyObject* get(PyObject* o);

	/* Replaces the element at the specified position in this list with the specified element */
	PyObject* set(PyObject* index, PyObject* o);

	/* Removes the element at the specified position in this list */
	PyObject* remove(int index);

	/* Retrieve underlying vector */
	std::vector<PyObject*> getMyList() { return myList; }

	/* Returns the number of elements in this list. */
	PyInt* len() { return (new PyInt(myList.size())); };

	/* Returns new PyList after binop operation */
	PyList* binop(const char[], PyList* target);

//	/* comparison for [not] in comparisons */
//	PyBool* PyList::compare(const char* op, PyObject& target);

	void print(ostream& out = cout);

private:
	std::vector<PyObject*> myList;

};

class PyXrange : public PyObject {

public:

	PyXrange(std::vector<PyObject*> xrange) {
		myXrange = xrange;
		myType = PYXRANGE;
	}
	void print(ostream& out = cout);
	/* return a vector that serves as an iterator of the xrange */
	std::vector<PyObject*> iterator() { return myXrange; } ;

private:

	std::vector<PyObject*> myXrange;
};

class PyTuple : public PyObject, public Gettable {

public:

	/* constructor */
	PyTuple(std::vector<PyObject*> tuple) {
		myTuple = tuple;
		myType = PYTUPLE;
	}

	PyTuple(int count, ...) {
		myType = PYTUPLE;
		std::vector<PyObject*> tuple;

		va_list pyObs;
		va_start(pyObs, count);
		for(int i = 0; i < count; i++) {
			tuple.push_back(va_arg(pyObs, PyObject*));
		}
		va_end(pyObs);

		myTuple = tuple;
	}

	std::vector<PyObject*> value() { return myTuple; };

	/* check if given index is in the range of the internal vector */
	bool inRange(int index);

	/* Returns true if this tuple contains the specified element. */
	bool contains(PyObject* o);

	/* get a slice of the tuple */
	PyTuple* slice(PyInt* start, PyInt* stop);

	/* Returns the element at the specified position in this tuple. */
	PyObject* get(PyObject* o);

	/* return a vector that serves as an iterator of the list */
	std::vector<PyObject*> iterator();

	/* Returns the number of elements in this tuple. */
	PyInt* len() { return (new PyInt(myTuple.size())); };

	void print(ostream& out = cout);

private:
	std::vector<PyObject*> myTuple;

};

/*
 *
 * Dict Object class*/
class PyDict : public PyObject, public Gettable, public Settable {

public:

	PyDict(std::map<PyKey, PyObject*> map) {
		myDict = map;
		myType = PYDICT;
	}

	/* vararg constructor */
	PyDict(int count, ...);

	/* return underlying dict */
	std::map<PyKey, PyObject*> value() {
		return myDict;
	}

	/* is k in this */
	bool containsKey(PyObject* k);

	/* returns a list of items */
	PyList* items();

	/* returns a list of values */
	PyList* values();

	/* returns a list of keys */
	PyList* keys();

	/* gets element at k if it exists */
	PyObject* get(PyObject* k);

	/* put element v at key k */
	PyObject* put(PyObject* k, PyObject* v);

	/* set the element at given key to given value */
	PyObject* set(PyObject* key, PyObject* value);

	/* returns the length */
	PyInt* len();

	void print(ostream& out = cout);


private:
	std::map<PyKey, PyObject*> myDict;
};

/*
 * PyKey, for keys in PyDicts
 */

class PyKey : public PyObject {

public:

	/* constructor */
	PyKey(PyObject* o);

	/* get the underlying value of this key as a pyobject pointer */
	PyObject* value();


	/* comparison operators overloading */
	bool operator == (const PyKey& pyKey) const {
		if  (this->getKeyType() != pyKey.getKeyType()) return false;
		else {
			switch (this->getKeyType()) {
			case PYBOOL:
				return boolKey == pyKey.boolKey;
			case PYINT:
				return intKey == pyKey.intKey;
			case PYSTR:
				return strKey == pyKey.strKey;
			default:
				cerr << "bad key type";
			}
		}
	}

	/* comparison operators overloading */
	bool operator < (const PyKey& pyKey) const {
		if  (this->getKeyType() != pyKey.getKeyType()) return (this->getKeyType() < pyKey.getKeyType());
		else {
			switch (this->getKeyType()) {
			case PYBOOL:
				return boolKey < pyKey.boolKey;
			case PYINT:
				return intKey < pyKey.intKey;
			case PYSTR:
				return strKey < pyKey.strKey;
			default:
				cerr << "bad key type";
			}
		}
	}

	/* get key type */
	PyType getKeyType() const { return myKeyType; }

	void print(ostream& out);

private:
	bool boolKey;
	int intKey;
	std::string strKey;
	PyType myKeyType;

};
/*
 * Wrapper for python Files
 * contains a simple FILE *
 */
class PyFile : public PyObject{

public:
	PyFile(){};
	PyFile(const char * file_name, const char * access, FILE* stream) {
		myFile = freopen(file_name, access, stream);
		myType = PYFILE;
		myStream = stream;
	}

	PyFile(const char * file_name, const char * access) {
			myFile = fopen(file_name, access);
			myType = PYFILE;
			myStream = NULL;
		}

	PyFile(FILE* file) {
			myFile = file;
			myType = PYFILE;
			myStream = NULL;
	}
	FILE* value() {
		if(myStream != NULL){
			return myStream;
		} else {
			return myFile;
		}
	}



	void setFile(FILE* file){ myFile = file; } ;
	/* get the FILE* underneath wrapper */
	FILE* getFile() { return myFile; };

	PyNone* closeFile() { fclose(myFile); } ;

	void print(ostream& out = cout);

private:
	FILE* myFile;
	FILE* myStream;
};

/*
 * Wrapper for None
 * contains 0
 */
class PyNone : public PyObject {

public:

	PyNone() {
		myType = PYNONE;
	};
	int value() { return 0; };

	void print(ostream& out = cout);

private:
	int myValue;
};

/**
 * Wrapper for a function.
 * */
class PyFunc : public PyObject {
public:

	PyFunc(void* function, void* static_link) {
		func = function;
		sl = static_link;
		myType = PYFUNC;
	};

	int value() { return 1; };

	void print(ostream& out = cout) { out << "__function__"; };

	/** Pointer to code. */
	void* func;

	/** Static Link */
	void* sl;
};

/* Built in Constants */

extern PyBool* True;
extern PyBool* False;
extern PyNone* None;
extern PyFile* pystdout;
extern PyFile* pystderr;
extern PyFile* pystdin;
extern PyList* pyargv;

/* Built in Functions */
extern PyFile* open(PyObject* fileName, PyObject* mode, PyObject* useless);
extern PyNone* close(PyObject* file, PyObject* useless); //PyObject returned should be none?

extern void append(PyList* target, PyObject* addition, PyObject* useless);

/* Built in Function len returns the len of a list/dict/Str */
extern PyInt* len(PyObject* target, PyObject* useless);

/* xrange returns a PyList of the given range */
extern PyXrange* xrange(PyInt* start, PyInt* stop, PyObject* useless);
