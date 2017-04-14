#ifndef MY_REGEX_H
#define MY_REGEX_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>
using namespace std;

struct State
{
	int in;
	int id;
	State* out1;
	State* out2;
	static int nstate;

	State(int c, State* o1, State* o2):in(c), id(nstate++), out1(o1), out2(o2) {}
};

enum 
{
	Match = 256,
	Split = 257
};

class Block
{
public:
	Block():start(NULL) {}
	Block(State* s, State** outp):start(s)
	{
		outlist.push_back(outp);
	}
	Block(State* s, vector<State**>& o):start(s), outlist(o) {}

	Block(State* s, Block& b, State** outp):start(s)			// used by '?'
	{
		outlist = b.outlist;
		outlist.push_back(outp);
	}
	Block(State* s, Block& b1, Block& b2):start(s)				// used by '|'
	{
		outlist = b1.outlist;
		outlist.insert(outlist.end(), b2.outlist.begin(), b2.outlist.end());
	}

	Block(Block& b1, Block& b2):start(b1.start), outlist(b2.outlist)		// used by '.'
	{
		connect(b1, b2.start);
	}

	static void connect(Block& b, State* s)
	{
		for(int i = 0; i < b.outlist.size(); ++i)
		{
			*b.outlist[i] = s;
		}
	}

	State* start;
	vector<State**> outlist;
};

string re2post(string re);
State* post2nfa(string postfix);
int nfa_match(State* start, string s);
void nfa_dot(State* start, char* re);

#endif