#include <string>
#include <iostream>
#include <ctime>
#include "regex.h"
using namespace std;

#define EXPECT_EQ_BASE(equality, expect, actual) \
do {\
	test_count++;\
	if (equality)\
		test_pass++;\
	else {\
		cerr << __FILE__ << ":" << __LINE__ << ": expect: " << expect << " actual: " << actual << endl;\
	}\
} while(0)

#define TEST_RE2POST(input, actual) \
	do {\
		string expect = re2post(input);\
		EXPECT_EQ_BASE(expect == actual, expect, actual); \
	} while(0)

#define TEST_NFA_MATCH(re, str) \
	do {\
		string post = re2post(re);\
		State* start = post2nfa(post);\
		int expect = nfa_match(start, str); \
		EXPECT_EQ_BASE(expect, "unmatch", "match"); \
	} while(0)

#define TEST_NFA_UNMATCH(re, str) \
	do {\
		string post = re2post(re);\
		State* start = post2nfa(post);\
		int expect = nfa_match(start, str); \
		EXPECT_EQ_BASE(!expect, "match", "unmatch"); \
	} while(0)

void test_re2post()
{
	int test_pass = 0, test_count = 0;
	TEST_RE2POST("abc", "ab.c.");
	TEST_RE2POST("a(b|c)", "abc|.");
	TEST_RE2POST("a(bc|de)f*", "abc.de.|.f*.");
	TEST_RE2POST("a(a(a(a|b)b)b)b", "aaaab|.b..b..b.");
	TEST_RE2POST("abc|(abc|123)", "ab.c.ab.c.12.3.||");

	printf("test_re2post: %d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
}

void test_nfa_match()
{
	int test_pass = 0, test_count = 0;
	TEST_NFA_MATCH("abc", "abc");
	TEST_NFA_MATCH("a(b|c)", "ab");
	TEST_NFA_MATCH("a(b|c)", "ac");
	TEST_NFA_MATCH("a(bc|de)f*", "abcf");
	TEST_NFA_MATCH("a(bc|de)f*", "adeff");
	TEST_NFA_MATCH("a(bc|de)f*", "abc");
	TEST_NFA_MATCH("a(a(a(a|b)b)b)b", "aaaabbb");
	TEST_NFA_MATCH("a(a(a(a|b)b)b)b", "aaabbbb");
	TEST_NFA_MATCH("abc|(abc|123)", "abc");
	TEST_NFA_MATCH("abc|(abc|123)", "123");

	TEST_NFA_UNMATCH("a(bc|de)f*", "abf");
	TEST_NFA_UNMATCH("a(bc|de)f*", "deff");
	TEST_NFA_UNMATCH("a(bc|de)f+", "abc");

	for(int i = 0; i < 10000; ++i)
	{
		TEST_NFA_MATCH("((1|2|3|4|5|6|7|8|9)+(0|1|2|3|4|5|6|7|8|9)*)|0", to_string(i));
	}

	printf("test_re2post: %d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
}

int main(int argc, char* argv[])
{
	test_re2post();
	test_nfa_match();
}