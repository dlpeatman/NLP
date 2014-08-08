#ifndef _TYPES_
#define _TYPES_

#include <string>
#include <vector>

using namespace::std;

// PL CFG Rule
typedef struct{
	string left;
	string right;
} PLRule;

// P CFG Rule
typedef struct{
	string left;
	string one;
	string two;
} PRule;

// Set of CFG Rules
struct CFGRules{
	vector<PLRule> PL;
	vector<PRule> P;
};

// A single context
typedef struct{
	vector<string> lhs;
	vector<string> rhs;
} context;

// PL Contextual (CBFG) Rule
typedef struct{
	vector<context> c;
	string s;
} PLCRule;

// P Contextual (CBFG) Rule
typedef struct{
	vector<context> lhs;
	vector<context> rhs1;
	vector<context> rhs2;
} PCRule;

// Set of CBFG Rules
struct CBFGRules{
	vector<PLCRule> PL;
	vector<PCRule> P;
};

//////////////////////////////
/* Equality helper funcions */
//////////////////////////////

// Check if two contexts are equal
bool equal(context a, context);

// Check if two string vectors are equal
bool equal(vector<string> a, vector<string> b);

// Check if two context vectors are equal
bool equal(vector<context> a, vector<context> b);

// Check if two PLCRules are equal
bool equal(PLCRule a, PLCRule b);

// Check if two PCRules are equal
bool equal(PCRule a, PCRule b);

/////////////////////////////////////
/* Vector search utility functions */
/////////////////////////////////////

// Search for a string w in vector v
bool search(vector<string> v, string w);

// Search for a context c in vector v
bool search(vector<context> v, context c);

// Search for a context in a 2d vector of contexts
bool search(vector<vector<context>> v2, context c);

// Search for a subset string in a list of subsets
bool search(vector<vector<string>> v, vector<string> s);

// Search for a PLC rule in a list of PLC rules
bool search(vector<PLCRule> rules, PLCRule r);

// Search for a PC rule in a list of PC Rules
bool search(vector<PCRule> rules, PCRule r);

//////////////////////////////
/* Subset utility functions */
//////////////////////////////

// Check if a set of contexts c1 is a subset of c2
bool subset(vector<context> c1, vector<context> c2);

// Check if a set of contexts c1 is contained within cl
bool subset(vector<context> c1, vector<vector<context>> cl);

#endif