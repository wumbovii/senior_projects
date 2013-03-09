/* -*- mode: C++; c-file-style: "stroustrup"; -*- */

/* environ.cc: Environments. */

/* Author:  Paul N. Hilfinger */

#include <cassert>
#include "apyc.h"

using namespace std;

Environ::Environ (const Environ* enclosing)
    : enclosure (enclosing) 
{
}

Environ::Environ (const Environ* environ0, const Environ* enclosing) 
    : members (environ0->members), enclosure (enclosing)
{
}

Decl* 
Environ::find_immediate (const string& name) const
{
    for (Decl_Vector::const_iterator i = members.begin (); 
	 i != members.end (); 
	 i++) 
    {
	if (name == (*i)->get_name ())
	    return *i;
    }
    return NULL;
}

Decl*
Environ::find (const string& name) const
{
    Decl* d = find_immediate (name);
    if (d != NULL)
        return d;
    else if (enclosure == NULL)
	return NULL;
    else
        return enclosure->find (name);
}

void 
Environ::define (Decl* decl)
{
    assert (find_immediate (decl->get_name ()) == NULL);
    members.push_back (decl);
}

const Environ* 
Environ::get_enclosure () const
{
    return enclosure;
}

const Decl_Vector&
Environ::get_members () const
{
    return members;
}
