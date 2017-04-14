CC		= g++
CFLAGS	= -std=c++11

.PHONY: all clean test match

all: test match

test: test.cpp regex.cpp
	$(CC) $(CFLAGS) $(CFLAGS) -o $@ $^

match: match.cpp regex.cpp
	$(CC) $(CFLAGS) $(CFLAGS) -o $@ $^

nfa: nfa.cpp regex.cpp
	$(CC) $(CFLAGS) $(CFLAGS) -o $@ $^

clean:
	rm test