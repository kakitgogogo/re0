#include "regex.h"
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	string post;
	State* start;

	if(argc < 3)
	{
		fprintf(stderr, "usage: %s regexp string...\n", argv[0]);
		return 1;
	}

	post = re2post(argv[1]);
	cout << "postfix expression: " << post << endl;

	start = post2nfa(post);
	if(start == NULL)
	{
		cerr << "error in post2nfa " << post << endl;
		return 1;
	}

	nfa_dot(start, argv[1]);

	for(int i = 2; i < argc; i++)
	{
		if(nfa_match(start, argv[i]))
			printf("nfa_match: %s\n", argv[i]);
	}
}