/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* ast.cc: Basic AST subclasses. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/* Definitions of methods in base class AST. */

int AST::current_mark = 0;

//current lambda number
int lambda_marker = 0;

//set of all declarations
extern std::vector<Decl*> decl_set;
//declaration index
int decl_index = 2;

void addDecl(Decl* decl) {
	decl_set.push_back(decl);
	decl_index++;
}

//extern declarations for creating decls
extern Decl* makeLocalDecl(const string& name, Decl* func, AST_Ptr type);

extern Decl*
makeGlobalDecl(const string& name, Decl* module, AST_Ptr type);

extern Decl*
makeParamDecl(const string& name, Decl* func, int k, AST_Ptr type);

extern Decl*
makeConstDecl(const string& name, Decl* module, AST_Ptr type);

extern Decl*
makeInstanceDecl(const string& name, Decl* cls, AST_Ptr type);

extern Decl*
makeFuncDecl(const string& name, Decl* container, AST_Ptr type);

extern Decl*
makeMethodDecl(const string& name, Decl* cls, AST_Ptr type);

extern Decl*
makeClassDecl(const string& name, Decl* module, Decl* supertype);

extern Decl*
makeModuleDecl(const string& name);

AST::AST() :
	_mark(0) {
}

bool AST::mark() {
	if (_mark == current_mark)
		return false;
	_mark = current_mark;
	return true;
}

void AST::unmark() {
	_mark = 0;
}

void AST::print(ostream& out, int indent) {
	current_mark += 1;
	print(this, out, indent);
}

void AST::print(AST_Ptr tree, std::ostream& out, int indent) {
	if (tree->mark())
		tree->_print(out, indent);
	else
		out << "...";
	tree->unmark();
}

Type_Ptr AST::asType() {
	throw logic_error("node does not represent a type");
}

Decl*
AST::getDecl() {
	throw logic_error("node does not represent a named entity");
}

void AST::setDecl(Decl*) {
	throw logic_error("node does not represent a named entity");
}

bool AST::isMissing() {
	return false;
}

/* Definitions of methods in base class AST_Tree. */

void AST_Tree::_print(ostream& out, int indent) {
	out << "(" << externalName() << " " << lineNumber();
	for_each_child (c, this) {
		out << endl << setw (indent + 4) << "";
		print (c, out, indent + 4);
	}
	end_for;
	out << ")";
}

const char* AST_Tree::externalName() {
	static string result;

	int syntax = oper()->syntax();
	const char* name;
	name = yyexternal_token_name(syntax);
	if (name == NULL)
		return "?";
	if (name[0] != '"')
		return name;
	name += 1;
	while (*name == '@')
		name += 1;

	if (name[0] == '\0')
		return "?";

	result = string(name, strlen(name) - 1);
	return result.c_str();
}

/** This method needs to construct the decl tree and assign unique declaration IDs to all
 * variable assignments, classdefs, and function defs.
 * */
//TODO: Nov 5th 1300 - to melvin
/* 1 we have to to make sure that super class works
 *    --- ??? we're not doing inheritence, why do we need superclass?
 * 2 i implemented some decls methods - note that much of our pointer manipulation
 * 		and assignment can be consolidated into the actual make*yada*Decl in decls.cc
 * 		Please see proposed changes in "decorate" for declare and if you agree wiht them
 * 3 also under case ID: - I'm not sure if your done implementing this, but it maybe
 * 		just an id/typedid vs a target list, i think we went over this, just a reminder
 */
void declareBuiltIns(Decl* builtin_module) {
	//handle typing keywords
	const char* type_keywords[] = { "int", "bool", "file", "str", "range", "list", "tuple", "dict", };

	const Type::TypeCategory type_categories[] = { Type::INT, Type::NONE, Type::NONE, Type::STR, Type::NONE, Type::LIST, Type::TUPLE, Type::DICT, };

	int type_keywords_size = sizeof(type_keywords) / sizeof(char*);
	for (int i = 0; i < type_keywords_size; i++) {
		string decl_name = type_keywords[i];

		AST_Ptr dummy[1];
		AST_Ptr name = AST::make_token(ID, decl_name);
		AST_Ptr typelist = AST::make_tree(TYPE_LIST, dummy, dummy);
		AST_Ptr args[2] = { name, typelist };

		Decl* new_decl = makeClassDecl(decl_name, builtin_module, NULL);
		new_decl->set_index(decl_index);

		name->setDecl(new_decl);
		Type_Ptr class_type = AST::make_tree(TYPE, args, args + 2)->asType();

		class_type->setTypeCategory(type_categories[i]);
		class_type->setDecl(new_decl);

		new_decl->set_type(class_type);

		addDecl(new_decl);
	}

	//handling const keywords


	const char* type_names[] = { "None", "bool", "bool", "file", "file", "file", "file", "None", "None", "None", "None", "None", "None", "None", "None" };

	const char* const_keywords[] = { "None", "True", "False", "stdout", "stdin", "stderr", "argv", "for", "in", "while", "if", "elif", "else", "return",
			"print" };

	int const_keywords_size = sizeof(const_keywords) / sizeof(char*);
	for (int i = 0; i < const_keywords_size; i++) {
		AST_Ptr dummy[1];
		AST_Ptr name = AST::make_token(ID, type_names[i], false);
		AST_Ptr typelist = AST::make_tree(TYPE_LIST, dummy, dummy);
		AST_Ptr args[2] = { name, typelist };
		Type_Ptr class_type = AST::make_tree(TYPE, args, args + 2)->asType();

		class_type->setTypeCategory(Type::NONE);

		string decl_name = const_keywords[i];
		Decl* new_decl = makeConstDecl(decl_name, builtin_module, NULL);

		class_type->setDecl(new_decl);

		new_decl->set_type(class_type);
		new_decl->set_index(decl_index);
		addDecl(new_decl);
	}

	//handle function keywords
	const char * func_keywords[] = { "append", "len", "open", "close", "xrange" };
	int func_keywords_size = sizeof(func_keywords) / sizeof(char*);
	for (int i = 0; i < func_keywords_size; i++) {
		string decl_name = func_keywords[i];
		Decl* new_decl = makeFuncDecl(decl_name, builtin_module, Type::makeVar());
		new_decl->set_index(decl_index);
		addDecl(new_decl);
	}
}

bool strEquals(string s1, string s2) {
	return s1.compare(s2) == 0;
}

bool isOperator(string decl_name) {
	char firstChar = decl_name[0];
	bool isLetter = ((firstChar >= 65 && firstChar <= 90) || (firstChar >= 97 || firstChar <= 122));
	bool isOp = firstChar == '_' || isLetter;
	return !isOp;
}

bool isRestricted(int syntax, string decl_name, Decl* decl_region) {
	bool restricted = decl_name == "None" || decl_name == "True" || decl_name == "False";
	if (syntax == ID) {
		return restricted;
	} else if (syntax == 324 || syntax == DEF) {
		int dast_type = decl_region->get_dast_type();

		switch (dast_type) {
			case CLASSD: {
				restricted = restricted || decl_region->contains_member(decl_name, INSTANCED);
			}
				break;
			case FUNCD: {
				restricted = restricted || decl_region->contains_member(decl_name, LOCALD);
				if (syntax == DEF) {
					restricted = restricted || decl_region->contains_member(decl_name, PARAMD);
				}
			}
				break;
			case MODULED: {
				restricted = restricted || decl_region->contains_member(decl_name, GLOBALD);
			}
				break;
		}

		return restricted;
	} else {
		cout << "restriction error\n";
		return false;
	}
}

void AST::declare(AST_Ptr root, Decl* decl_region, TargetEnum target_enum) {
	/** pay particular attention to these sections in spec:
	 * 6.5
	 * 6.6
	 * 6.10
	 *
	 * need to throw errors if you catch these.
	 * to throw an error, use:     throw logic_error ("derp derp derp"); */
	int syntax = root->oper()->syntax();
	int num_children = root->arity();

	Decl* new_decl;
	switch (syntax) {
		case 324: {
			//create decl_node
			string decl_name = root->child(0)->as_string();

			//check for project restrictions
			bool restricted = isRestricted(syntax, decl_name, decl_region);

			if (restricted) {
				errorCall(root->loc(), "class declaration violates project spec restrictions in section 6\n");
			}
			new_decl = makeClassDecl(decl_name, decl_set[0], NULL);
			new_decl->set_index(decl_index);

			//add decl to global decl_set
			addDecl(new_decl);

			AST_Ptr dummy[1];
			Type_Ptr class_type = AST::make_tree(TYPE, dummy, dummy)->asType();
			class_type->setTypeCategory(Type::USER_CLASS);
			class_type->setDecl(new_decl);
			new_decl->set_type(class_type);

			//Create the 'self' instance variable
			string decl_self_name = "self";
			Decl* self = makeInstanceDecl(decl_self_name, new_decl, new_decl->get_type());
			self->set_index(decl_index);

			//add decl to global decl_set
			addDecl(self);

			//declare the appropriate child nodes (block structure)
			declare(root->child(2), new_decl, target_enum);
		}
			break;
		case DEF: {
			//create funcdecl node
			string decl_name = root->child(0)->as_string();

			//check for project restrictions
			bool restricted = isRestricted(syntax, decl_name, decl_region);

			if (restricted) {
				cout << "error in creating def declaration\n";
				errorCall(root->loc(), "def declaration violates project spec restrictions in section 6\n");
			}

			AST_Ptr decl_formals = root->child(1);
			new_decl = makeFuncDecl(decl_name, decl_region, Type::makeVar());
			new_decl->set_index(decl_index);

			AST_Ptr return_type = Type::makeVar();
			int num_formals = decl_formals->arity();

			AST_Ptr formal_types[num_formals];
			for (int i = 0; i < num_formals; i++) {
				formal_types[i] = Type::makeVar();
			}

			AST_Ptr typelist = AST::make_tree(TYPE_LIST, formal_types, formal_types + num_formals);

			AST_Ptr args[2] = { return_type, typelist };
			Type_Ptr def_type = AST::make_tree(FUNCTION_TYPE, args, args + 2)->asType();
			def_type->setComplete(false);

			new_decl->set_type(def_type);

			//add decl to parent_region and global decl_set
			addDecl(new_decl);
			decl_region = new_decl;

			/** TODO: fix how we set the types of the formals here.
			 * */

			//get parameters
			for (size_t j = 0; j < decl_formals->arity(); j++) {
				AST_Ptr formal = decl_formals->child(j);
				string formal_name;
				AST_Ptr formal_type = formal_types[j];

				if (formal->arity() == 0) {
					//ID
					formal_name = formal->as_string();
					//formal_type = Type::makeVar();
				} else {
					//typed ID
					AST_Ptr formal_typedid = formal;
					formal_name = formal_typedid->child(0)->as_string();
					//formal_type = formal_typedid->child(1);//not sure if correct on type
				}

				if (decl_region->get_container()->get_dast_type() == CLASSD && j == 0) {
					string self = "self";
					if (formal_name == "self" || decl_name.substr(0, 8) == "__lambda") {
						//the first param is self - great - move on
						//OR SUPER LAMBDA H4X
						continue;

					} else {
						//the first param is missing self, throw error
						cout << "Self is not first param: ERROR";

						errorCall(root->loc(), "Bogus Instance Definition: First parameter must be of \"self\"");

					}

				}

				//create param decl if it's not called self
				if (formal_name != "self") {
					new_decl = makeParamDecl(formal_name, decl_region, j, formal_type);
					new_decl->set_index(decl_index);
					addDecl(new_decl);
				}
			}
			//declare the appropriate child nodes (formals and block)
			declare(root->child(3), decl_region, target_enum);

		}
			break;
		case ASSIGN: {
			declare(root->child(0), decl_region, IS_TARGET);
		}
			break;
		case FOR: {
			declare(root->child(0), decl_region, IS_TARGET);
		}
			break;
		case TYPED_ID: {
			declare(root->child(0), decl_region, target_enum);
		}
			break;
		case ATTRIBUTEREF: {
			//should not create any declarations, unless left side is self ID
			string left_attr = root->child(0)->as_string();
			if (left_attr == "self") {
				Decl* container = decl_region;
				while (container->get_dast_type() != CLASSD) {
					container = container->get_container();
				}
				declare(root->child(1), container, target_enum);
			}
		}
			break;
		case ID: {
			if (target_enum == IS_TARGET) {
				//create decl based on container type
				string decl_name = root->as_string();

				//check for built in restrictions
				bool restricted = isRestricted(syntax, decl_name, decl_region);
				if (restricted) {
					errorCall(root->loc(), "variable declaration violates project spec restrictions in section 6\n");
				}

				bool valid = false;
				switch (decl_region->_cont_type) {
					case 324: {
						//if decl is in class and not previously declared => instance var
						if (!decl_region->contains_member(decl_name, INSTANCED)) {
							new_decl = makeInstanceDecl(decl_name, decl_region, Type::makeVar());
							valid = true;
						}

					}
						break;
					case MODULE: {
						//if decl is in module and not previously declared => global var
						if (!decl_region->contains_member(decl_name, GLOBALD)) {
							new_decl = makeGlobalDecl(decl_name, decl_region, Type::makeVar());
							valid = true;
						}
					}
						break;
					case DEF: {
						//if decl is in def and not previously declared => local var
						if (!decl_region->contains_member(decl_name, LOCALD) && !decl_region->contains_member(decl_name, PARAMD)) {
							new_decl = makeLocalDecl(decl_name, decl_region, Type::makeVar());
							valid = true;
						}
					}
						break;
				}

				if (valid) {
					new_decl->set_index(decl_index);

					//add decl to global decl_set
					addDecl(new_decl);
				}
			}
		}
			break;

		default: {
			for (int i = 0; i < num_children; i++) {
				declare(root->child(i), decl_region, target_enum);
			}

		}
	}
}

void AST::decorate(AST_Ptr root, Decl* decl_region, vector<Decl*>* global_decls, TargetEnum target_enum, Decl* builtin_decls) {

	bool foundDecl = false;

	string node_name;

	Decl* new_decl_region;
	Decl* container = decl_region->get_container();

	int i;
	int syntax = root->oper()->syntax();

	vector<Decl*> decl_members = decl_region->get_members();
	switch (syntax) {
		case BINOP: {
			//ignore the binary operator
			decorate(root->child(0), decl_region, global_decls, target_enum, builtin_decls);
			decorate(root->child(2), decl_region, global_decls, target_enum, builtin_decls);
			break;
		}
		case UNOP: {
			decorate(root->child(1), decl_region, global_decls, target_enum, builtin_decls);
			break;
		}
		case ASSIGN: {
			//decorate the left side as a target, the right side as not a target.
			decorate(root->child(0), decl_region, global_decls, IS_TARGET, builtin_decls);
			decorate(root->child(1), decl_region, global_decls, IS_NOT_TARGET, builtin_decls);
			break;
		}
		case ATTRIBUTEREF: {
			//child 0 gives us E1, which is the identifier for the containing class.
			new_decl_region = decl_region;
			Decl* parent_decl;
			AST_Ptr primary = root->child(0);
			//if the expr of the attributeref node isn't an ID, decorate that expr node.
			//scope should not have to change.
			if (primary->oper()->syntax() != ID) {
				cout << "decorating primary of attributeRef node with reference field: " << (root->child(1)->as_string()) << "\n";
				decorate(primary, decl_region, global_decls, target_enum, builtin_decls);

				bool foundAttribute = false;

				while (primary->oper()->syntax() != ID) {

					//if this is part of an attributeref chain, we just get the second child of the next attributeref
					//as our ID
					if (primary->oper()->syntax() == ATTRIBUTEREF) {
						parent_decl = primary->child(1)->getDecl()->get_container();
						foundAttribute = true;
						break;
					}
					primary = primary->child(0);
				}
				if (!foundAttribute) {
					parent_decl = primary->getDecl();
				}
			} else {
				decorate(primary, decl_region, global_decls, target_enum, builtin_decls);
				parent_decl = primary->getDecl();
			}

			//if we've found a class, we want to use that class as our scope for finding the attribute
			new_decl_region = parent_decl;
			if (parent_decl != NULL) {
				//find the id we referenced now.

				//Note that this enforces the restriction that we have to find this reference at the top level
				//of a class! i.e. section 6.6
				node_name = root->child(1)->as_string();
				vector<Decl*> parent_members = parent_decl->get_members();
				Decl* member;
				for (i = 0; i < (int) parent_decl->get_members().size(); i++) {
					member = parent_members[i];
					if (member->get_name().compare(node_name) == 0) {
						foundDecl = true;
						root->child(1)->setDecl(member);
					}
				}
			} else {
				errorCall(root->loc(), "Couldn't find class containing: " + (root->child(1)->as_string()));
			}

			break;
		}
		case 324: {
			node_name = root->child(0)->as_string();

			//check for project restrictions
			bool restricted = isRestricted(syntax, node_name, decl_region);

			if (restricted) {
				errorCall(root->loc(), "class declaration violates project spec restrictions in section 6\n");
			}
			//look for a CLASSDEF declaration in the region
			new_decl_region = decl_region->get_member(node_name, CLASSD);
			if (new_decl_region != NULL) {
				//decorate the funcdef with the current region since we found it,
				decorate(root->child(0), decl_region, global_decls, target_enum, builtin_decls);

				//then decorate the children with the new region since they belong to the function
				decorate(root->child(2), new_decl_region, global_decls, target_enum, builtin_decls);
			} else {
				errorCall(root->loc(), "Could not find class declaration in the proper scope");
			}
			break;
		}
		case DEF: {
			node_name = root->child(0)->as_string();

			//check for project restrictions
			bool restricted = isRestricted(syntax, node_name, decl_region);

			if (restricted) {
				errorCall(root->loc(), "function definition violates project spec restrictions in section 6\n");
			}

			new_decl_region = decl_region->get_member(node_name, FUNCD);
			if (new_decl_region != NULL) {
				//decorate the funcdef with the current region since we found it,
				decorate(root->child(0), decl_region, global_decls, target_enum, builtin_decls);
				//also decorate the formals list

				decorate(root->child(1), new_decl_region, global_decls, target_enum, builtin_decls);

				cout << "scoping in from def, new decl region is: " << (new_decl_region->get_name()) << "\n";
				//then decorate the children with the new region since they belong to the function
				decorate(root->child(3), new_decl_region, global_decls, target_enum, builtin_decls);

			} else {
				//throw an error, can't find the decl in this region
				errorCall(root->loc(), "Could not find funcdef declaration in the proper scope");
			}
			break;
		}
		case ID: {
			node_name = root->as_string();
			if (isOperator(node_name)) {
				break;
			}
			//check for project restrictions here
			bool restricted = isRestricted(syntax, node_name, decl_region);
			vector<Decl*> builtin_members = builtin_decls->get_members();

			if (restricted && (target_enum == IS_TARGET)) {
				errorCall(root->loc(), "identifier violates project spec restrictions in section 6\n");
			} else {
				for (i = 0; i < (int) builtin_members.size(); i++) {
					Decl* member = builtin_members[i];
					if (member->get_name().compare(node_name) == 0) {
						root->setDecl(member);
						foundDecl = true;
						break;
					}
				}
				if (foundDecl) {
					//leave the switch statement if we found something already
					break;
				}
			}

			cout << "looking for decl for " << (node_name) << "\n";
			while (!foundDecl) {
				for (i = 0; i < (int) decl_members.size(); i++) {
					Decl* member = decl_members[i];
					if (member->get_name().compare(node_name) == 0) {
						//if this is a global, we want to mark that we've seen it so far so we can use it later
						//also make sure that this is a left side declaration
						if (container == NULL && target_enum == IS_TARGET) {
							bool decl_exists = false;
							for (int i = 0; i < (int) (*global_decls).size(); i++) {
								if ((*global_decls)[i]->get_name().compare(node_name) == 0) {
									decl_exists = true;
									break;
								}
							}
							if (!decl_exists) {
								cout << "adding global var " << (member->get_name()) << "\n";
								(*global_decls).push_back(member);
							}
							for (int j = 0; j < (int) (*global_decls).size(); j++) {
								//we're at the module level.
								//we have to have seen this first to accept it, so only take it if we've found it
								//thus far
								Decl* d = (*global_decls)[j];
								if (d->get_name().compare(node_name) == 0) {
									foundDecl = true;
								}
							}
						} else {
							foundDecl = true;
						}
						if (foundDecl) {
							//actually set the declaration for the node if we have found it and it's valid.
							cout << "found decl for " << (root->as_string()) << "\n";
							root->setDecl(member);

							/* For ID's belonging to Defs and Class nodes, we already did set type. */
							if (member->get_type() == NULL) {
								member->set_type(Type::makeVar());
							}
						}
						break;
					}
				}
				if (foundDecl) {
					break;
				} else {
					//switch the decl_region, get the members if they're not null
					decl_region = container;
					if (container != NULL) {
						container = decl_region->get_container();
						decl_members = decl_region->get_members();
					} else
						break;
				}
			}

			if (!foundDecl) {
				errorCall(root->loc(), "Could not find declaration for: " + (root->as_string()));
			}
			break;
		}

		default: {
			for (i = 0; i < (int) root->arity(); i++) {
				decorate(root->child(i), decl_region, global_decls, target_enum, builtin_decls);
			}
		}
	}
}

void AST::rewriteLambda(AST_Ptr root, bool scope_region, vector<AST_Ptr>* additions) {
	if (root->arity() != 0) {
		int original_count = root->arity();
		for (int i = 0; i < original_count; i++) {
			AST_Ptr child = root->child(i);
			int syntax = child->oper()->syntax();

			switch (syntax) {
				case LAMBDA: {
					AST_Ptr formals = child->child(0);
					AST_Ptr type0 = child->child(1);
					AST_Ptr expr = child->child(2);

					AST_Ptr a[1] = { expr };
					AST_Ptr block = AST::make_tree(BLOCK, a, a + 1);

					//lambda naming
					char func_name[256];
					std::sprintf(func_name, "__lambda%d__", lambda_marker);
					string id = func_name;
					//increment current lambda number
					lambda_marker++;
					AST_Ptr name = AST::make_token(ID, id);
					AST_Ptr args[4] = { name, formals, type0, block };

					AST_Ptr def = AST::make_tree(DEF, args, args + 4);

					/* Now rewrite the new expression (might be nested lambdas) */
					vector<AST_Ptr> empty;
					rewriteLambda(def->child(3), true, &empty);

					root->replace(i, AST::make_token(ID, id));

					(*additions).push_back(def);
				}
					break;
				case 324: {
					cout << "begin\n";
					vector<AST_Ptr> empty;
					rewriteLambda(child->child(2), true, &empty);
					cout << "end\n";
				}
					break;
				case DEF: {
					vector<AST_Ptr> empty;
					rewriteLambda(child->child(3), true, &empty);
				}
					break;
				default:
					rewriteLambda(child, false, additions);
			}
		}

		if (scope_region) {
			for (size_t i = 0; i < (*additions).size(); i++) {
				root->insert(0, (*additions)[i]);
			}
		}
	}
}

void AST::rewriteCalls(AST_Ptr root) {
	/* iterate through children of the root
	 * looking for call nodes to rewrite.
	 */
	int num_children = root->arity();

	for (int i = 0; i < num_children; i++) {
		AST_Ptr child = root->child(i);
		int syntax = child->oper()->syntax();

		if (syntax == CALL) {

			AST_Ptr primary = child->child(0);
			AST_Ptr arg_list = child->child(1);
			string name = primary->as_string();
			int num_args = arg_list->arity();

			if (decl_set[0]->contains_member(name, CLASSD)) {
				//begin rewrite if child is a call node and is a call to a class
				Decl* class_decl = decl_set[0]->get_member(name, CLASSD);
				bool contains_init = class_decl->contains_member("__init__", FUNCD);

				if (num_args > 0 && !contains_init) {
					//can't pass in args without initializer defined
					errorCall(root->loc(), "object instantiation contains parameters without an __init__ definition. See 4.2 in specs.");

				} else if (num_args == 0 && !contains_init) {
					//replace call with a simple "new" node
					AST_Ptr new_children[1] = { primary };
					root->replace(i, AST::make_tree(NEW, new_children, new_children + 1));

					//recurse on replacement node
					rewriteCalls(root->child(i));
				} else if (contains_init && num_args >= 0) {
					//replace call with a call1 node
					AST_Ptr init_id = AST::make_token(ID, "__init__");
					child->replace(0, init_id);
					AST_Ptr call1_children[2] = { init_id, arg_list };
					root->replace(i, AST::make_tree(CALL1, call1_children, call1_children + 2)); /*line number issue here...*/

					//insert "new" node into expr_list
					AST_Ptr new_children[1] = { primary };
					AST_Ptr new_node = AST::make_tree(NEW, new_children, new_children + 1);
					arg_list->insert(0, new_node);

					//recurse on replacement node
					rewriteCalls(root->child(i));
				}
			}
		} else {
			//recurse on child node
			rewriteCalls(child);
		}
	}
}

void AST::rewriteMethods(AST_Ptr root) {
	/* iterate through children of the root
	 * and replace all method nodes with def nodes
	 */

	int num_children = root->arity();
	for (int i = 0; i < num_children; i++) {
		AST_Ptr child = root->child(i);
		int syntax = child->oper()->syntax();

		if (syntax == METHOD) {
			//rewrite method node
			AST_Ptr id = child->child(0);
			AST_Ptr formals = child->child(1);
			AST_Ptr type0 = child->child(2);
			AST_Ptr block = child->child(3);
			AST_Ptr def_children[4] = { id, formals, type0, block };
			root->replace(i, AST::make_tree(DEF, def_children, def_children + 4));

			//recurse on child node
			rewriteMethods(root->child(i));
		} else {
			//recurse on child
			rewriteMethods(child);
		}
	}
}

void AST::rewriteAttributes(AST_Ptr root) {
	int num_children = root->arity();

	for (int i = 0; i < num_children; i++) {
		AST_Ptr child = root->child(i);
		int syntax = child->oper()->syntax();

		if (syntax == ATTRIBUTEREF) {
			int replace_index = i;
			AST_Ptr attr_root = root;
			AST_Ptr temp_root = root;
			AST_Ptr primary = child->child(0);
			AST_Ptr id = child->child(1);

			//since we might have multiple attribute references,
			//we traverse our pointers until we get the deepest attribute ref
			while (primary->arity() > 0) {
				if (primary->oper()->syntax() == ATTRIBUTEREF) {
					attr_root = child;
				}
				temp_root = child;
				child = primary;
				id = primary->child(1);
				primary = primary->child(0);
				replace_index = 0; //if we have nested attrrefs, our replacement index is 0 rather than i
			}

			//there are no nested classes, so class can only be the leftmost ID
			//also, only rewrite if our last nesting is an attribute_ref
			if (primary->oper()->syntax() == ID && attr_root == temp_root) {
				string name = primary->as_string();
				if (decl_set[0]->contains_member(name, CLASSD)) {
					//replace attributeref node with the id node
					attr_root->replace(replace_index, id);
				}
			}
		} else {
			//recurse
			rewriteAttributes(child);
		}
	}
}

Type_Ptr AST::typeInference(AST_Ptr root, vector<Decl*> env) {
	cout << "top of type inference for " << root->as_string() << "\n";
	int syntax = root->oper()->syntax();
	int result = 1;
	Type_Ptr my_type = Type::makeVar();

	switch (syntax) {
		case ID: {
			my_type = root->getDecl()->get_type()->asType()->binding();
			cout << "ID allowed: " << my_type->binding()->getAllowed() << "\n";
			cout << "ID category: " << my_type->binding()->getCategory() << "\n";

			break;
		}
		case TYPED_ID: {
			root->child(0)->getDecl()->set_type(root->child(1));

			my_type = root->child(1)->asType();
			break;
		}
		case INT_LITERAL: {

			Decl* int_classdecl = env[1]->get_member("int", CLASSD);
			my_type = int_classdecl->get_type()->asType();

			cout << "INT allowed: " << my_type->getAllowed() << "\n";
			cout << "INT category: " << my_type->getCategory() << "\n";
			break;
		}
		case STRING_LITERAL: {
			Decl* str_classdecl = env[1]->get_member("str", CLASSD);
			my_type = str_classdecl->get_type()->asType();

			cout << "STR allowed: " << my_type->getAllowed() << "\n";
			cout << "STR category: " << my_type->getCategory() << "\n";
			break;
		}
		case AND: {
			Type_Ptr op1 = typeInference(root->child(0), env);
			Type_Ptr op2 = typeInference(root->child(1), env);
			/** Now check that the types are the same */
			if (op1->binding() != op2->binding()) {
				errorCall(root->loc(), "Type mismatch error on a AND/OR");
			}
			if (op2->unify(op1->binding()) || op1->unify(op2->binding())) {
				//unify succeeds
				my_type = op1->binding();
				break;
			} else {
				//unify fails
				errorCall(root->loc(), "Type mismatch error on an AND/OR");
			}

		}
		case OR: {
			Type_Ptr op1 = typeInference(root->child(0), env);
			Type_Ptr op2 = typeInference(root->child(1), env);
			/** Now check that the types are the same */
			if (op1->binding() != op2->binding()) {
				errorCall(root->loc(), "Type mismatch error on a AND/OR");
			}
			if (op2->unify(op1->binding()) || op1->unify(op2->binding())) {
				//unify succeeds
				my_type = op1->binding();
				break;
			} else {
				//unify fails
				errorCall(root->loc(), "Type mismatch error on an AND/OR");
			}
		}
		case COMPARISON: {

			Type_Ptr op1 = typeInference(root->child(0), env);
			Type_Ptr op2 = typeInference(root->child(1)->child(1), env);
			string op = root->child(1)->child(0)->as_string();

			if (op == "notin" || op == "in") {
				// op1:$a in op2: list($a)|dict($a $c)
				Type_Ptr rule = Type::makeVar(Type::LIST | Type::DICT);

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
		}
		case BINOP: {
			Type_Ptr op1 = typeInference(root->child(0), env);
			Type_Ptr op2 = typeInference(root->child(2), env);
			string op = root->child(1)->as_string();

			if (op == "+") {
				Type_Ptr rule = Type::makeVar(Type::INT | Type::STR | Type::LIST);

				/** First check that rule is satisfied */
				result &= rule->unify(op1->binding());
				result &= rule->unify(op2->binding());

				/** Now check that the types are the same */
				if (op1->binding() != op2->binding()) {
					errorCall(root->loc(), "Type mismatch error on a binop");
				}
			} else if (op == "*") {
				Type_Ptr rule_left = Type::makeVar(Type::INT | Type::STR);
				Type_Ptr rule_right = Type::makeVar(Type::INT);

				result &= rule_left->unify(op1->binding());
				result &= rule_right->unify(op2->binding());
			} else {

				Type_Ptr rule = Type::makeVar(Type::INT);

				result &= rule->unify(op1->binding());
				result &= rule->unify(op2->binding());
			}

			my_type = op1->binding();
			break;
		}

		case UNOP: {
			AST_Ptr op = root->child(0);
			string op_name = op->as_string();
			Type_Ptr op1 = typeInference(root->child(1), env);
			if (op_name == "NOT") {
				//Unary Negation
				Type_Ptr hasToBeBool = Type::makeVar(Type::NONE);
				hasToBeBool = op1->binding();
			} else {
				//Unary +/-
				Type_Ptr rule = Type::makeVar(Type::INT);
				result &= rule->unify(op1->binding());
				my_type = op1->binding();
			}

			break;
		}
		case CALL: {
			/* ID case, still need to handle attr case */
			string id_name = root->child(0)->as_string();

			if (strEquals(id_name, "append")) {

			}

			Type_Ptr func_type = root->child(0)->getDecl()->get_type()->asType();

			/* todo: handle formals */
			int num_formals = root->child(1)->arity();
			for (int i = 0; i < num_formals; i++) {
				/* The type of the param in the call statment */
				Type_Ptr param_type = typeInference(root->child(1)->child(i), env);
				Type_Ptr formal_type = func_type->child(1)->child(i)->asType();

				result &= formal_type->unify(param_type);
			}

			if (func_type->isComplete()) {
				func_type->freshen();
			}

			/* My type will be the binding of the return type of the function I'm calling. */
			my_type = func_type->child(0)->asType()->binding();

			break;
		}
		case TUPLE: {
			cout << "tuple statement start\n";
			AST_Ptr dummy[1];
			AST_Ptr name = AST::make_token(ID, "tuple");
			AST_Ptr typelist = AST::make_tree(TYPE_LIST, dummy, dummy);

			for (int i = 0; i < (int) root->arity(); i++) {
				typeInference(root->child(i), env);
				typelist->append(root->child(i));
			}

			AST_Ptr args[2] = { name, typelist };
			Type_Ptr class_type = AST::make_tree(TYPE, args, args + 2)->asType();
			class_type->setTypeCategory(Type::TUPLE);
			my_type = class_type;
		}
		case ASSIGN: {
			cout << " - assign statement start\n";
			/** Single target case: */

			Type_Ptr target = typeInference(root->child(0), env);
			Type_Ptr rightSide = typeInference(root->child(1), env);

			cout << "before: " << rightSide->binding()->getCategory() << "\n";
			cout << "before: " << target->binding()->getCategory() << "\n";

			result &= target->unify(rightSide->binding());

			cout << "after: " << rightSide->binding()->getCategory() << "\n";
			cout << "after: " << target->binding()->getCategory() << "\n";

			break;
		}
		case DEF: {
			/* This is now decl of the function we're in. */
			Decl* scope = root->child(0)->getDecl();
			env[2] = scope;

			/* Get the initialized type of this function. */
			Type_Ptr def_type = root->child(0)->getDecl()->get_type()->asType();

			/* Check if any of the parameters have types. */
			for (int i = 0; i < (int) root->child(1)->arity(); i++) {
				Type_Ptr param_type = typeInference(root->child(1)->child(i), env);
				Type_Ptr formal_type = def_type->child(1)->child(i)->asType();

				result &= formal_type->unify(param_type);
			}

			/* Check if there is a user-defined return type */
			if (root->child(2)->arity() != 0) {
				/* This means a user-defined return type was set. */
				Type_Ptr initial_type = def_type->child(0)->asType()->binding();
				Type_Ptr user_defined_return_type = typeInference(root->child(2), env);

				result &= initial_type->unify(user_defined_return_type);
			}

			/* Now do type inference on the def block. */
			for (int i = 0; i < (int) root->child(3)->arity(); i++) {
				typeInference(root->child(3)->child(i), env);
			}

			/* Set complete to true so we can freshen on subsequent calls. */
			def_type->setComplete(true);
			break;
		}
			/** Return node */
		case 333: {
			cout << " return node inference \n";

			/* Type of the return expression */
			Type_Ptr expr_type = typeInference(root->child(0), env);

			/* The current return type binding of the def we are in. */
			Type_Ptr return_type = env[2]->get_type()->child(0)->asType()->binding();

			/* Now bind the two. */
			result &= return_type->unify(expr_type);
			break;
		}
		case 324:
			/* Class block case, skip the inherited class */
			typeInference(root->child(0), env);
			typeInference(root->child(2), env);
			break;
		default: {
			/** Traverse block */
			for (int i = 0; i < (int) root->arity(); i++) {
				typeInference(root->child(i), env);
			}
		}
	}

	if (!result)
		errorCall(root->loc(), "Type inference error.");
	return my_type;
}

void AST::errorCall(Location_Type loc, string message) {
	Error(loc, message.c_str());
}

AST_Ptr AST::semantics() {
	cout << " semantics() called inside ast.cc";
	return this;
}

AST_Ptr make_id(const char* text, const char* loc) {
	AST_Ptr result = AST::make_token(ID, strlen(text), text, false);
	result->set_loc(loc);
	return result;
}

/* The Horn framework has a provision (the NODE_FACTORY macro) that
 * allows AST subclasses to "register" themselves with the parser and
 * associate themselves with a particular syntactic category so that
 * the parser will create instances of the appropriate subclass when
 * asked to produce an AST node with a particular operator.  The
 * NODE_..._CONSTRUCTOR... macros set up the necessary constructors
 * and factory declarations in any given class.  To associate a
 * class with a given operator token, pass the (int) value of that
 * token's syntactic category to the NODE_FACTORY macro (for
 * one-character tokens, the character itself is usually the syntactic
 * category; for other tokens, use the name declared in the Horn
 * source file (those definitions appear in apyc-parser.hh, included above).
 */

/** A node representing a missing element in the AST (e.g., a missing 
 *  >>FILE argument in a print node.) */
class Empty_AST: public AST_Tree {
protected:

	/** Override AST_Tree::_print */
	void _print(ostream& out, int indent) {
		out << "()";
	}

	bool isMissing() {
		return true;
	}

NODE_CONSTRUCTORS (Empty_AST, AST_Tree);

};

NODE_FACTORY (Empty_AST, EMPTY);
