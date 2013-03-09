/* runtime.c: */

/* Purpose: A PYthon Compiler uses runtime.h/runtime.cc
 * as a guideline for specific instantiations for the C++
 * immediate language for the python code
 *
 * Contains:
 * 	Definitions for data structures
 * 	 */

#include "runtime.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <cstdlib>
using namespace std;

PyBool* True = new PyBool(true);
PyBool* False = new PyBool(false);
PyNone* None = new PyNone();
PyFile* pystdout = new PyFile((const char*)"stdout", (const char*)"w+", stdout);
PyFile* pystderr = new PyFile((const char*)"stdout", (const char*)"w+", stderr);
PyFile* pystdin = new PyFile((const char*)"stdout", (const char*)"w+", stdin);
PyList* pyargv = new PyList(0);

PyFile* open(PyObject* f, PyObject* am, PyObject* useless){
	string& f2 = ((PyStr*)f)->value();
	string& am2 = ((PyStr*)am)->value();

	const char * filename = f2.c_str();
	const char * accessMode = am2.c_str();

	PyFile* pyfile = new PyFile(filename, accessMode);
	FILE* cfile = fopen(filename, accessMode);
	pyfile->setFile(cfile);
	return pyfile;
}
PyNone* close(PyObject* f, PyObject* useless){
	PyNone* none = ((PyFile*)f)->closeFile();
	return none;

}

/* Built in functions, see project 2 page 10 for more detail */
void append(PyList* target, PyObject* addition, PyObject* useless) {
	target->valuep()->push_back(addition);
}

/* Built in Function len returns the len of a list/dict/Str */
PyInt* len(PyObject* target, PyObject* useless) {
	PyType myType = target->get_myType();
	PyInt* result;

	if (myType == PYLIST) {
		result = ((PyList*) target)->len();
	} else if (myType == PYSTR) {
		result = ((PyStr*) target)->len();
	} else if (myType == PYDICT) {
		result = ((PyDict*) target)->len();
	} else {
		std::cout << "len()'s argument is not of type list/str/dict";
	}

	return result;
}

PyXrange* xrange(PyInt* start, PyInt* stop, PyObject* useless) {
	int begin = start->value();
	int end = stop->value();

	std::vector<PyObject*> v;
	for (int i = begin; i < end; i++) {
		PyInt* pyInt = new PyInt(i);
		v.push_back(pyInt);
	}

	PyXrange* p = new PyXrange(v);
	return p;
}

PyObject* PyObject::logical(const char* op, PyObject* target) {
	PyObject* result;
	PyType this_myType = this->get_myType();
	PyType targ_myType = target->get_myType();
	if (this_myType == PYINT && targ_myType  == PYINT) {
		int operand1 = ((PyInt*)this)->value();
		int operand2 = ((PyInt*)target)->value();

		if (!strcmp(op, "and")) {
			result = target;
			if(operand1 == 0 || operand2 == 0){
				result = new PyInt(0);
			}
		} else if (!strcmp(op, "or")) {
			result = this;
			if(operand1 == 1 || operand2 == 1){
				result = new PyInt(1);
			}
		} else {
			cout << "Error: Unrecognized logic operator";
		}
	} else if(this_myType == PYLIST && targ_myType == PYLIST) {
		int operand1 = ((PyList*) this)->value().size();
		int operand2 = ((PyList*) target)->value().size();

		if (!strcmp(op, "and")) {
			result = target;
			if (operand1 == 0 || operand2 == 0) {
				result = new PyList(0);
			}
		} else if (!strcmp(op, "or")) {
			result = this;
		} else {
			cout << "Error: Unrecognized logic operator";
		}
	} else {
		if (!strcmp(op, "and")) {
			result = target;
		} else if (!strcmp(op, "or")) {
			result = this;
		}  else {
			cout << "Error: Unrecognized logic operator";
		}
	}
	return result;
}

PyBool* PyBool::logical(const char* op, PyBool* target) {
	PyBool* result;
	if (!strcmp(op, "and")) {
		result = new PyBool(this->value() && target->value());
	} else if (!strcmp(op, "or")) {
		result = new PyBool(this->value() || target->value());
	} else if (!strcmp(op, "not")) {
		result = new PyBool(!target->value());
	} else {
		cout << "Error: Unrecognized logic operator";
	}
	return result;

}

PyObject* PyInt::unop(const char* op) {
	int result;
	if (!strcmp(op, "+")) {
		result = +(this->value());
	} else if (!strcmp(op, "-")) {
		result = -(this->value());
	} else if (!strcmp(op, "~")) {
		result = ~(this->value());
	} else if (!strcmp(op, "not")) {
		if (this->value() == 0){ return True; }
		if (this->value() == 1){ return False; }
	} else {
		cout << "Error: Unrecognized unary op";
	}
	return new PyInt(result);
}

PyBool* PyBool::unop(const char* op) {
	return new PyBool(!this->value());
}

PyInt* PyInt::binop(const char op[], PyObject* target) {
	int operand1 = this->value();
	int operand2 = ((PyInt*)target)->value();
	int result = 0;
	if (!strcmp(op, "+")) {
		result = operand1 + operand2;
	} else if (!strcmp(op, "*")) {
		result = operand1 * operand2;
	} else if (!strcmp(op, "/")) {
		result = operand1 / operand2;
	} else if (!strcmp(op, "-")) {
		result = operand1 - operand2;
	} else if (!strcmp(op, "//")) {
		result = operand1 / operand2;
	} else if (!strcmp(op, "%")) {
		result = operand1 % operand2;
	} else if (!strcmp(op, "<<")) {
		result = operand1 << operand2;
	} else if (!strcmp(op, ">>")) {
		result = operand1 >> operand2;
	} else if (!strcmp(op, "&")) {
		result = operand1 & operand2;
	} else if (!strcmp(op, "|")) {
		result = operand1 | operand2;
	} else if (!strcmp(op, "^")) {
		result = operand1 ^ operand2;
	} else {
		cout << "Error: Unrecognized binop";
	}
	return (new PyInt(result));
}

PyStr* PyStr::binop(const char op[], PyObject* target) {
	std::string result = string("");
	if (!strcmp(op, "+")) {
		std::string operand1 = this->value();
		std::string operand2 = ((PyStr*) target)->value();
		result = operand1;
		result.append(operand2);
	} else if (!strcmp(op, "*")) {
		std::string operand1 = this->value();
		int operand2 = ((PyInt*) target)->value();
		for (int i = 1; i <= operand2; i++) {
			result.append(operand1);
		}
	} else { cout << "Error: Unrecognized binop"; }
	return (new PyStr(result));
}

PyList* PyList::binop(const char op[], PyList* target) {
	std::vector<PyObject*> operand1 = this->value();
	std::vector<PyObject*> operand2 = target->value();
	if (!strcmp(op, "+")) {
		for (int i = 0; i < operand2.size(); i++) { operand1.push_back(operand2[i]); }
	} else { cout << "Error: Unrecognized binop operator"; }
	return new PyList(operand1);
}

PyBool* PyInt::compare(const char* op, PyObject* target) {
	int operand1 = this->value();
	bool result = false;
	if(target->get_myType() == PYLIST){
		std::vector<PyObject*> operand2 = ((PyList*)target)->value();
		if (!strcmp(op, "in")) {
			result = false;
			for (int i = 0; i < operand2.size(); i++)
				if (((PyInt*)operand2[i])->value() == operand1) result = true;
		} else if (!strcmp(op, "notin")) {
			result = true;
			for (int i = 0; i < operand2.size(); i++)
				if (((PyInt*)operand2[i])->value() == operand1) result = false;
		} else { cout << "Error: Unrecognized compare operator"; }
	} else {
		int operand2 = ((PyInt*)target)->value();
		if (!strcmp(op, "<")) {
			result = (operand1 < operand2);
		} else if (!strcmp(op, ">")) {
			result = (operand1 > operand2);
		} else if (!strcmp(op, "<=")) {
			result = (operand1 <= operand2);
		} else if (!strcmp(op, ">=")) {
			result = (operand1 >= operand2);
		} else if (!strcmp(op, "==")) {
			result = (operand1 == operand2);
		} else if (!strcmp(op, "<>")) {
			result = (operand1 != operand2);
		} else if (!strcmp(op, "!=")) {
			result = (operand1 != operand2);
		} else { cout << "Error: Unrecognized compare operator"; }
	} if (result) { return True;
	} else { return False; }
}

PyBool* PyStr::compare(const char* op, PyObject* target) {
	std::string operand1 = this->value();
	bool result = false;
	if (target->get_myType() == PYLIST) {
		std::vector<PyObject*> operand2 = ((PyList*) target)->value();
		if (!strcmp(op, "in")) {
			result = false;
			for (int i = 0; i < operand2.size(); i++) {
				if (((PyStr*) operand2[i])->value() == operand1) result = true;
			}
		} else if (!strcmp(op, "notin")) {
			result = true;
			for (int i = 0; i < operand2.size(); i++)
				if (((PyStr*) operand2[i])->value() == operand1) result = false;
		} else cout << "Error: Unrecognized compare operator";
	} else {
		std::string operand2 = ((PyStr*)target)->value();
		int compared = operand1.compare(operand2);
		if (!strcmp(op, "<")) {
			if (compared < 0) result = true;
			else result = false;
		} else if (!strcmp(op, ">")) {
			if (compared > 0) result = true;
			else result = false;
		} else if (!strcmp(op, "<=")) {
			if (compared <= 0) result = true;
			else result = false;
		} else if (!strcmp(op, ">=")) {
			if (compared >= 0) result = true;
			else result = false;
		} else if (!strcmp(op, "==")) {
			if (compared == 0) result = true;
			else result = false;
		} else if (!strcmp(op, "<>") || !strcmp(op, "!=")) {
			if (compared != 0) result = true;
			else result = false;
		} else cout << "Error: Unrecognized compare operator";
	}

	if (result) return True;
	else return False;
}

PyBool* PyBool::compare(const char* op, PyBool* target) {
	bool operand1 = this->value();
	bool operand2 = target->value();
	PyInt* int1 = new PyInt((int) operand1);
	PyInt* int2 = new PyInt((int) operand2);
	return int1->compare(op, int2);
}

bool PyStr::inRange(int index) {
	return (index < this->value().size() && index >= 0) ? true : false;
}

PyObject* PyStr::get(PyObject* o) {
	int index = ((PyInt*) o)->value();
	index = (index >= 0) ? index : this->value().size() + index;
	if (!this->inRange(index)) exit(1);
	const char meow = (const char) this->value()[index];
	return new PyStr(string(&meow));
}

PyStr* PyStr::slice(PyInt* start, PyInt* stop) {
	int begin = (start->value() >= 0) ? start->value() : this->value().size()
			+ start->value();
	int end = (stop->value() >= 0) ? stop->value() : this->value().size()
			+ stop->value();

	begin = (begin > 0) ? begin : 0;
	end = (this->value().size() < end) ? this->value().size() : end;
	std::string new_str = "";
	for (int i = begin; i < end; i++) new_str += this->value()[i];
	return new PyStr(new_str);
}

std::vector<char> PyStr::iterator() {
	std::string s = this->value();
	std::vector<char> char_vector;
	for (int i = 0; i < s.size(); i++) char_vector.push_back(s[i]);
	return char_vector;
}

bool PyList::inRange(int index) {
	return (index < myList.size() && index >= 0) ? true : false;
}

PyList* PyList::slice(PyInt* start, PyInt* stop) {
	std::vector<PyObject*> contents = this->value();
	int begin = (start->value() >= 0) ? start->value() : this->value().size()
			+ start->value();
	int end = (stop->value() >= 0) ? stop->value() : this->value().size()
			+ stop->value();
	begin = (begin > 0) ? begin : 0;
	end = (this->value().size() < end) ? this->value().size() : end;

	std::vector<PyObject*> new_vector;
	for (int i = begin; i < end; i++) new_vector.push_back(contents[i]);

	PyList* p = new PyList(new_vector);
	return p;
}

PyObject* PyList::get(PyObject* o) {
	int index = ((PyInt*) o)->value();
	index = (index >= 0) ? index : this->value().size() + index;
	if (!this->inRange(index)) exit(1);
	return this->value()[index];
}

PyObject* PyList::set(PyObject* index, PyObject* o) {
	int i = ((PyInt*) index)->value();
	i = (i >= 0) ? i : this->value().size() + i;
	if (!this->inRange(i)) exit(1);
	PyObject* prevOb = this->value()[i];
	myList[i] = o;
	return prevOb;
}

std::vector<PyObject*> PyList::iterator() {
	return this->value();
}

bool PyTuple::inRange(int index) {
	return (index < myTuple.size() && index >= 0) ? true : false;
}

PyTuple* PyTuple::slice(PyInt* start, PyInt* stop) {
	std::vector<PyObject*> contents = this->value();

	//handle negative indexing
	int begin = (start->value() >= 0) ? start->value() : this->value().size()
			+ start->value();
	int end = (stop->value() >= 0) ? stop->value() : this->value().size()
			+ stop->value();

	//handle indices that are out of range after negative index handling
	begin = (begin > 0) ? begin : 0;
	end = (this->value().size() < end) ? this->value().size() : end;

	std::vector<PyObject*> new_vector;
	for (int i = begin; i < end; i++) {
		new_vector.push_back(contents[i]);
	}

	PyTuple* p = new PyTuple(new_vector);
	return p;
}

PyObject* PyTuple::get(PyObject* o) {
	int index = ((PyInt*) o)->value();
	index = (index >= 0) ? index : this->value().size() + index;
	if (!this->inRange(index)) exit(1);
	return this->value()[index];
}

std::vector<PyObject*> PyTuple::iterator() {
	return this->value();
}


PyDict::PyDict(int count, ...) {
	//arguments should be (number of arguments, key1, value1, key2, value2...)
	myType = PYDICT;

	count*=2;
	if (count % 2 != 0) cerr << "bad kv pairs for PyDict constructor";

	va_list pairs;
	va_start(pairs, count);
	for (int i = 0; i < count; i += 2) {
		PyObject* key = va_arg(pairs, PyObject*);
		PyObject* value = va_arg(pairs, PyObject*);

		PyKey pyKey = *(new PyKey(key));
		if (myDict.count(pyKey) > 0) {
			//erase current kv mapping if we're overwriting with a new key value pair
			myDict.erase(pyKey);
		}
		myDict.insert(pair<PyKey, PyObject*> (pyKey, value));

	}
	va_end(pairs);
}

bool PyDict::containsKey(PyObject* k) {
	PyKey key = *(new PyKey(k));
	return (myDict.find(key) != myDict.end());
}

PyObject* PyDict::get(PyObject* k) {

	if (!this->containsKey(k)) {
		cerr << "access Key DNE\n";
		exit(1);
	}
	return myDict[new PyKey(k)];
}

PyObject* PyDict::set(PyObject* key, PyObject* value) {

	PyKey pyKey = *(new PyKey(key));
	if (myDict.count(pyKey) > 0) {
		myDict.erase(pyKey);
	}
	myDict.insert(pair<PyKey, PyObject*> (pyKey, value));
	return NULL;
}

PyInt* PyDict::len() {
	return new PyInt(this->value().size());
}

PyKey::PyKey(PyObject* o) {
	myType = PYKEY;

	switch (o->get_myType()) {
	case PYBOOL:
		boolKey = ((PyBool*) o)->value();
		myKeyType = PYBOOL;
		break;
	case PYINT:
		intKey = ((PyInt*) o)->value();
		myKeyType = PYINT;
		break;
	case PYSTR:
		strKey = ((PyStr*) o)->value();
		myKeyType = PYSTR;
		break;
	case PYKEY: {
		PyObject* val = ((PyKey*) o)->value();
		PyType kkType = ((PyKey*) o)->getKeyType();
		if (kkType == PYBOOL) {
			boolKey = ((PyBool*) val)->value();
			myKeyType = PYBOOL;
		} else if (kkType == PYINT) {
			intKey = ((PyInt*) val)->value();
			myKeyType = PYINT;
		} else if (kkType == PYSTR) {
			strKey = ((PyStr*) val)->value();
			myKeyType = PYSTR;
		} else cout << "WTF IS GOING ON!!!!\n";
		break;
	}
	default:
		cerr << "unknown key type for PyKey constructor";
	}
}

PyObject* PyKey::value() {
	switch (myKeyType) {
	case PYBOOL:
		return new PyBool(boolKey);
	case PYINT:
		return new PyInt(intKey);
	case PYSTR:
		return new PyStr(strKey);
	default:
		return NULL;
	}
}

void PyInt::print(ostream& out) {
	out << this->value();
}

void PyBool::print(ostream& out) {
	if (this->value()) out << "True";
	else out << "False";
}

void PyStr::print(ostream& out) {
	out << this->value();
}

void PyNone::print(ostream& out) {
	out << this->value();
}

void PyList::print(ostream& out) {
	out << "[";
	std::string delim = "";
	for (int i = 0; i < myList.size(); i++) {
		PyObject* element = myList[i];
		bool isStr = false;
		if (element->get_myType() == PYSTR) isStr = true;
		out << delim;
		delim = ", ";
		if (isStr) out << "'";

		element->print(out);
		if (isStr) out << "'";
	}
	out << "]";
}

void PyDict::print(ostream& out) {
	out << "{";
	std::map<PyKey, PyObject*> contents = this->value();
	std::map<PyKey, PyObject*>::iterator it;
	std::string delim = "";
	for (it = contents.begin(); it != contents.end(); it++) {
		PyKey key = (*it).first;
		PyObject* value = (*it).second;
		bool keyIsStr = false;
		bool valIsStr = false;
		out << delim;
		delim = ", ";
		if (key.getKeyType() == PYSTR) keyIsStr = true;
		if (keyIsStr) out << "'";
		key.print(out);
		if (keyIsStr) out << "'";
		out << ": ";
		if (value->get_myType() == PYSTR) valIsStr = true;
		if (valIsStr) out << "'";
		value->print(out);
		if (valIsStr) out << "'";
	}
	out << "}";
}

void PyKey::print(ostream& out) {
	switch (myKeyType) {
	case PYBOOL:
		out << boolKey;
		break;
	case PYINT:
		out << intKey;
		break;
	case PYSTR:
		out << strKey;
		break;
	default:
		return;
	}
}

void PyTuple::print(ostream& out) {
	out << "(";
	std::string delim = "";
	std::vector<PyObject*> contents = this->value();
	for (int i = 0; i < contents.size(); i++) {
		PyObject* element = contents[i];
		bool isStr = false;
		if (element->get_myType() == PYSTR) isStr = true;

		out << delim;
		delim = ", ";
		if (isStr) out << "'";
		element->print(out);
		if (isStr) out << "'";
	}
	out << ")";
}
void PyFile::print(ostream& out){}
void PyXrange::print(ostream& out) { }
