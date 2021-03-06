#include <iostream>
#include <cstdio>
#include <cstdlib>


YYNAMESPACE_CLOSE

using namespace std;

int code;

int
main (int argc, char* argv[])
{
#if defined(DEBUGGING) && DEBUGGING > 0
    yydebug = 1;
#endif
    if (argc > 1) {
	FILE* inp = fopen (argv[1], "r");
	if (inp == NULL) {
	    fprintf (stderr, "could not open %s\n", argv[1]);
	    exit (1);
	}
	YYNAMESPACE::yypush_lexer (inp, argv[1]);
    } else
	YYNAMESPACE::yypush_lexer (stdin, "<stdin>");
    code = 0;
    code |= YYNAMESPACE::yyparse ();
    if (YYNAMESPACE::theTree != NULL)
	YYNAMESPACE::theTree->dump (cout, 0);
    cout << endl;
    exit (code);
}
YYNAMESPACE_OPEN
