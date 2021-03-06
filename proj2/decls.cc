/* Implementations of declaration and type-related classes */

/* Authors:  YOUR NAMES HERE */
/* From a skeleton by Paul N. Hilfinger */

#include <stdexcept>
#include <iostream>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"
#include <string>
#include <vector>

using namespace std;
extern void AST::print(AST_Ptr tree, std::ostream& out, int indent);


/* Print THIS on the standard output. */
void Decl::print (ostream& out) {
	char *indexlist;
	Decl::set_printable();
	out << _dast_rep << "\n";
	AST::print(_type, out, 4);
	indexlist = get_indexlist();
	out << indexlist;
	delete indexlist;
}

char* Decl::get_indexlist(){
	char *buf = new char[256];
	for (size_t i = 0; i < _members.size(); i++) {
		int index = _members[i]->get_index();
		if (i == 0 && i == _members.size() - 1) {
			std::sprintf(buf, "\n(index_list %d)", index);
		} else {
			if(i == 0){
				std::sprintf(buf, "\n(index_list %d ", index);
			} else if (i == _members.size() - 1) {

				char ascii_index[32];
				std::sprintf(ascii_index, "%d))", index);
				std::strcat(buf, ascii_index);

			} else {
				char ascii_index[32];
				std::sprintf(ascii_index, "%d ", index);
				std::strcat(buf, ascii_index);
			}
		}
	}
	return buf;
}

void Decl::set_printable(){
	char *buf = new char[256];
	char *indexlist = get_indexlist();
	switch(_dast_type){
	case LOCALD:{
		std::sprintf(buf, "(localdecl %d %s %d", _index, _name.c_str(),
				_container->get_index(), _type->chars_dup());
	} break;
	case GLOBALD:{
		std::sprintf(buf, "(globaldecl %d %s %d", _index, _name.c_str(),
						_container->get_index(), _type->chars_dup ());
	} break;
	case PARAMD:{
		std::sprintf(buf, "(paramdecl %d %s %d %d", _index, _name.c_str(),
						_container->get_index(), _param_pos, _type->chars_dup());
	} break;
	case CONSTD:{
		std::sprintf(buf, "(constdecl %d %s %d", _index, _name.c_str(),
						_container->get_index(), _type->chars_dup());
	} break;
	case INSTANCED:{
		std::sprintf(buf, "(instancedecl %d %s %d", _index, _name.c_str(),
						_container->get_index(), _type->chars_dup());
	} break;
	case FUNCD:{
		std::sprintf(buf, "(funcdecl %d %s %d %s %s", _index, _name.c_str(),
						_container->get_index(), _type->chars_dup(),
						indexlist);
	} break;
	case CLASSD:{
		std::sprintf(buf, "(classdecl %d %s %d %s", _index, _name.c_str(),
						_container->get_index(), indexlist);
	} break;
	case MODULED:{
		std::sprintf(buf, "(moduledecl %d %s %s", _index, _name.c_str(), indexlist);
	} break;
	}

	_dast_rep = buf;

}

void Decl::set_dast_type(int t){
	_dast_type = t;
}

int Decl::get_dast_type() {
	return _dast_type;
}

std::string Decl::get_name(){
	return _name;
}

void Decl::set_index(int i){
	_index = i;
}

int Decl::get_index(){
	return _index;
}

void Decl::set_cont_type(int cont_type){
	_cont_type = cont_type;
}

int Decl::get_cont_type(){
	return _cont_type;
}


AST_Ptr Decl::get_type () {
	return _type;
}

void Decl::set_type (AST_Ptr type) {
	_type = type;
}

void Decl::set_position (int k) {
    _param_pos = k;
}

int Decl::get_position () {
    return _param_pos;
}

const vector<Decl*>& Decl::get_members () {
	return _members;
}

bool Decl::contains_member(Decl* decl) {
	int size = _members.size();
	for (int i = 0; i < size; i++) {
		if (decl->get_name() == _members[i]->get_name() && decl->get_dast_type() == _members[i]->get_dast_type()) {
			return true;
		}
	}
	return false;
}

bool Decl::contains_member(std::string name, int dast_type) {
	int size = _members.size();
	for (int i = 0; i < size; i++) {
		if (name == _members[i]->get_name() && dast_type == _members[i]->get_dast_type()) {
			return true;
		}
	}
	return false;
}


void Decl::add_member (Decl* new_member) {
	_members.push_back(new_member);
}

Type::TypeCategory Decl::getCategory () {
    return Type::NONE;
}

Decl::~Decl () {
}

Decl* makeLocalDecl (const string& name, Decl* func, AST_Ptr type) {

	Decl* new_decl = new Decl(0, name, func);

    func->add_member(new_decl);
    new_decl->set_type(type);

    new_decl->set_dast_type(LOCALD);

	return new_decl;
}


Decl*
makeGlobalDecl (const string& name, Decl* module, AST_Ptr type)
{
	Decl* new_decl = new Decl(0, name, module);

	module->add_member(new_decl);
	new_decl->set_type(type);

	new_decl->set_dast_type(GLOBALD);

	return new_decl;
}


Decl*
makeParamDecl (const string& name, Decl* func, int k, AST_Ptr type)
{
	Decl* new_decl = new Decl(0, name, func);

	func->add_member(new_decl);
	new_decl->set_type(type);
	new_decl->set_position(k);

	new_decl->set_dast_type(PARAMD);

	return new_decl;
}


Decl*
makeConstDecl (const string& name, Decl* module, AST_Ptr type)
{
	Decl* new_decl = new Decl(0, name, module);

	module->add_member(new_decl);
	new_decl->set_type(type);

	new_decl->set_dast_type(CONSTD);

	return new_decl;
}


Decl*
makeInstanceDecl (const string& name, Decl* cls, AST_Ptr type)
{
	Decl* new_decl = new Decl(0, name, cls);

	cls->add_member(new_decl);
	new_decl->set_type(type);

	new_decl->set_dast_type(INSTANCED);

	return new_decl;
}


Decl*
makeFuncDecl (const string& name, Decl* container, AST_Ptr type)
{
	Decl* new_decl = new Decl(0, name, container);

	container->add_member(new_decl);
	new_decl->set_type(type);
	new_decl->set_cont_type(DEF);

	new_decl->set_dast_type(FUNCD);

	return new_decl;
}


Decl*
makeMethodDecl (const string& name, Decl* cls, AST_Ptr type)
{
	Decl* new_decl = new Decl(0, name, cls);

	cls->add_member(new_decl);
	new_decl->set_type(type);
	new_decl->set_cont_type(DEF);

	new_decl->set_dast_type(FUNCD);

	return new_decl;
}


Decl*
makeClassDecl (const string& name, Decl* module, Decl* supertype)
{
	Decl* new_decl = new Decl(0, name, module);

	module->add_member(new_decl);

	new_decl->set_cont_type(CLASS_BLOCK);
	new_decl->set_dast_type(CLASSD);

	return new_decl;
}


Decl*
makeModuleDecl (const string& name)
{
	Decl* new_decl = new Decl(0, name, NULL);

	new_decl->set_cont_type(MODULE);

	new_decl->set_dast_type(MODULED);

	return new_decl;
}

void
output_decls (ostream& out, std::vector<Decl*> decl_set)
{
    for(size_t i = 0; i < decl_set.size(); i++){
    	(decl_set.at(i))->print(out);
    }
}
