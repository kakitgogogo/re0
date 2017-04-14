#include <iostream>
#include <set>
#include "regex.h"

void badregex(const string &re)
{
	std::cerr << "bad regexp " << re << std::endl;
	exit(1);
}

string re2post(string re)
{
	string buf;
	int nalt = 0, natom = 0;
	vector<pair<int, int> > parens;

	for(int i = 0; i < re.size(); ++i)
	{
		switch(re[i])
		{
			case '(':
				if(natom > 1)
				{
					--natom;
					buf += '.';
				}
				parens.push_back(make_pair(nalt, natom));
				nalt = 0;
				natom = 0;
				break;
			case '|':
				if(natom == 0)
					badregex(re);
				while(--natom > 0)
					buf += '.';
				++nalt;
				break;
			case ')':
				if(parens.empty())
					badregex(re);
				if(natom == 0)
					badregex(re);
				while(--natom > 0)
					buf += '.';
				while(nalt-- > 0)
					buf += '|';
				nalt = parens.back().first;
				natom = parens.back().second;
				parens.pop_back();
				++natom;
				break;
			case '*':
			case '+':
			case '?':
				if(natom == 0)
					badregex(re);
				buf += re[i];
				break;
			default:
				if(natom > 1)
				{
					--natom;
					buf += '.';
				}
				buf += re[i];
				++natom;
				break;
		}
	}
	if(!parens.empty())
		badregex(re);
	while(--natom > 0)
		buf += '.';
	while(nalt-- > 0)
		buf += '|';
	return buf;
}

int State::nstate = 0;
State* matchstate = new State(Match, NULL, NULL);

State* post2nfa(string postfix)
{
	Block stack[1000], *stackp, b1, b2, b;
	State* s;

	State::nstate = 1;

	if(postfix.empty())
		return NULL;

#define push(s) *stackp++ = s
#define pop() *--stackp

	stackp = stack;
	for(int i = 0; i < postfix.size(); ++i)
	{
		switch(postfix[i])
		{
			default:
				s = new State(postfix[i], NULL, NULL);
				push(Block(s, &s->out1));
				break;
			case '.':
				b2 = pop();
				b1 = pop();
				push(Block(b1, b2));
				break;
			case '|':
				b2 = pop();
				b1 = pop();
				s = new State(Split, b1.start, b2.start);
				push(Block(s, b1, b2));
				break;
			case '?':
				b = pop();
				s = new State(Split, b.start, NULL);
				push(Block(s, b, &s->out2));
				break;
			case '*':
				b = pop();
				s = new State(Split, b.start, NULL);
				Block::connect(b, s);
				push(Block(s, &s->out2));
				break;
			case '+':
				b = pop();
				s = new State(Split, b.start, NULL);
				Block::connect(b, s);
				push(Block(b.start, &s->out2));
				break;
		}
	}

	b = pop();
	if(stackp != stack)
		return NULL;

	Block::connect(b, matchstate);
	return b.start;

#undef pop
#undef push
}



void searchnext(State* s, int in, set<State*>& out2)
{
	if(s->in == Split)
	{
		searchnext(s->out1, in, out2);
		searchnext(s->out2, in, out2);
	}
	else if(s->in == in)
	{
		out2.insert(s->out1);
	}
	else if(s->in == Match)
	{
		out2.insert(s);
	}
}

int ismatch(set<State*>& out)
{
	for(auto outp:out)
	{
		set<State*> out2;
		searchnext(outp, Split, out2);
		for(auto outp2:out2)
		{
			if(outp2 == matchstate)
			{
				return 1;
			}
		}
	}
	return 0;
}

int nfa_match(State* start, string s)
{
	set<State*>out1, out2;
	out1.insert(start);
	for(int i = 0; i < s.size(); ++i)
	{
		for(auto outp:out1)
		{
			searchnext(outp, s[i], out2);
		}

		swap(out1, out2);
		out2.clear();
	}

	return ismatch(out1);
}


void nfa_dot(State* start, char* re)
{
	State* queue[1000];
	State* array[1000];
	State* cur;
	int front, back;
	int i, j;
	FILE* fout;

#define N 1000
#define push(x) queue[front=(front+1)%N] = x
#define pop() queue[back=(back+1)%N]
#define empty() (front == back)

	fout = fopen("nfa.dot", "w");
	fprintf(fout, "digraph G {\n");
	fprintf(fout, "\tlabel=\"%s\"\n", re);

	front = back =0;
	i = 0;
	push(start);
	while(!empty())
	{
		cur = pop();
		int isvisited = 0;
		for(j = 0; j < i; ++j)
		{
			if(array[j] == cur)
			{
				isvisited = 1;
			}
		}
		if(isvisited) continue;

		fprintf(fout, "\t%d[shape=circle]\n", cur->id);

		array[i++] = cur;

		if(cur->out1)
		{
			fprintf(fout, "\t%d->%d[label=\"%c\"]\n", cur->id, cur->out1->id, (cur->in < 256 ? cur->in : ' '));
			push(cur->out1);
		}
		if(cur->out2)
		{
			fprintf(fout, "\t%d->%d[label=\"%c\"]\n", cur->id, cur->out2->id, (cur->in < 256 ? cur->in : ' '));
			push(cur->out2);
		}
	}

	fprintf(fout, "\t%d[shape=doublecircle]\n", matchstate->id);
	fprintf(fout, "}\n");

#undef empty
#undef pop
#undef push	
}
