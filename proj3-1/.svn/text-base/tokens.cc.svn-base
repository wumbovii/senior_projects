/* -*- mode: C++; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */

/* tokens.cc: Definitions related to AST_Token and its subclasses. */

/* Authors:  YOUR NAMES HERE */

#include <iostream>
#include <cerrno>
#include "apyc.h"
#include "ast.h"
#include "apyc-parser.hh"

using namespace std;

/** Default print for tokens. */
void
AST_Token::_print (ostream& out, int indent)
{
    out << "(<Token>)";
}

/** The supertype of tokens with a type. */
class Typed_Token : public AST_Token {
public:

    Type_Ptr getType () {
        if (_type == NULL)
            _type = computeType ();
        return _type;
    }

protected:

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        if (_type == NULL) {
            _type = computeType ();
            changed = true;
        }
    }

    /** Computes my type, which is then cached by getType(). */
    virtual Type_Ptr computeType () {
        return NULL;
    }

    Type_Ptr _type;

    TOKEN_BASE_CONSTRUCTORS (Typed_Token, AST_Token);

};    

/** Represents an integer literal. */
class Int_Token : public Typed_Token {
protected:

    void _print (ostream& out, int indent) {
	out << "(int_literal " << lineNumber () << " " << value
	    << ")";
    }

    Int_Token* post_make () {
        string text = as_string ();
        errno = 0;
        value = strtol (text.c_str (), (char**) NULL, 0);
        if (errno != 0 || value > (1L<<30)) {
            Error (loc (), "literal value out of range: %s",
                   text.c_str ());
            value = 0;
        }
        return this;
    }

    Type_Ptr computeType () {
        return intDecl->as_type ();
    }

    TOKEN_CONSTRUCTORS(Int_Token, Typed_Token);

    long value;
};

TOKEN_FACTORY(Int_Token, INT_LITERAL);

/** Represents an identifier. */
class Id_Token : public Typed_Token {
protected:

    void _print (ostream& out, int indent) {
	out << "(id " << lineNumber () << " " << as_string ();
        if (getDecl () != NULL)
            out << " " << getDecl ()->get_index ();
        out << ")";
    }

    AST_Ptr getId () {
        return this;
    }

    Decl* getDecl () {
        return _me;
    }

    void setDecl (Decl* decl) {
        _me = decl;
        if (decl != NULL) {
            Type_Ptr type = decl->get_type ();
            if (type != NULL && _type != NULL &&
                !_type->unify (decl->get_type ()))
                Error (this, "inconsistent types implied for %s",
                       as_string ().c_str ());
        }
    }

    Type_Ptr getType () {
        return computeType ();
    }

    Type_Ptr computeType () {
        if (_me == NULL)
            return Type::makeVar ();
        else if (_me->is_type ()) {
            Error (this, "attempt to use a type as a value");
            return Type::makeVar ();
        } else
            return _me->get_type ();
    }

    void addTargetDecls (Decl* enclosing) {
        string name = as_string ();
        if (undefinable (name)) {
            Error (this, "attempt to redefine %s", name.c_str ());
            return;
        }

        Decl* old = enclosing->get_environ ()->find_immediate (as_string ());

        if (old == NULL)
            enclosing->add_var_decl (this);
        else if (!old->assignable ()) {
            Error (this, "invalid assignment to %s", name.c_str ());
        }
    }
        
    void resolveSimpleIds (const Environ* env) {
        if (_me == NULL) {
            string name = as_string ();
            _me = env->find (name);
            if (_me == NULL)
                Error (this, "undefined name: %s", name.c_str ());
        }
    }

    void doTyping (Decl* context, bool& changed, bool& unresolved) {
        Typed_Token::doTyping (context, changed, unresolved);
        unresolved |= (getDecl () == NULL);
    }

    TOKEN_CONSTRUCTORS_INIT(Id_Token, Typed_Token, _me (NULL));

private:

    Decl* _me;

};

TOKEN_FACTORY(Id_Token, ID);

/** Represents a string. */
class String_Token : public AST_Token {
private:
    
    String_Token* post_make () {
        if (syntax () == RAWSTRING) {
            literal_text = string (as_chars (), text_size ());
        } else {
            int v;
            const char* s = as_chars ();
            size_t i;
            i = 0;
            literal_text.clear ();
            while (i < text_size ()) {
                i += 1;
                if (s[i-1] == '\\') {
                    i += 1;
                    switch (s[i-1]) {
                    default: literal_text += '\\'; v = s[i-1]; break;
                    case '\n': continue;
                    case 'a': v = '\007'; break;
                    case 'b': v = '\b'; break;
                    case 'f': v = '\f'; break;
                    case 'n': v = '\n'; break;
                    case 'r': v = '\r'; break;
                    case 't': v = '\t'; break;
                    case 'v': v = '\v'; break;
                    case '\'': v = '\''; break;
                    case '"': case '\\': v = s[i-1]; break;
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': 
                    { 
                        v = s[i-1] - '0';
                        for (int j = 0; j < 2; j += 1) {
                            if ('0' > s[i] || s[i] > '7')
                                break;
                            v = v*8 + (s[i] - '0');
                            i += 1;
                        }
                        break;
                    }
                    case 'x': {
                        if (i+2 > text_size () || 
                            !isxdigit (s[i]) || !isxdigit (s[i+1])) {
                            Error (s, "bad hexadecimal escape sequence");
                            break;
                        }
                        sscanf (s+i, "%2x", &v);
                        i += 2;
                        break;
                    }
                    }
                } else
                    v = s[i-1];
                literal_text += (char) v;        
            }
        }
        return this;
    }

    void _print (ostream& out, int indent) {
        for (size_t i = 0; i < literal_text.size (); i += 1) {
            char c = literal_text[i];
            if (c < 32 || c == '\\' || c == '"') {
                out << "\\" << oct << setw (3) << setfill('0') << (int) c
                    << setfill (' ') << dec;
            } else
                out << c;
        }
    }

    TOKEN_CONSTRUCTORS(String_Token, AST_Token);
    static const String_Token raw_factory;

    string literal_text;
};

TOKEN_FACTORY(String_Token, STRING);
const String_Token String_Token::raw_factory (RAWSTRING);

