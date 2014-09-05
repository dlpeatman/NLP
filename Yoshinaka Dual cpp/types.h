/****************************************************************
 * File: types.h
 * Type definitions and utility functions for yoshiakadual
 * David Peatman - Updated 9/04/14
 ****************************************************************
 * Notes:
 * Types are defined in uppercase
 * Variables instantiations are written lowercase
 * 'v' is prepended to variable names when rule types are used
 *   as vectors. 's' for sets.
 * There are many struct abstractions (I call them links) where a
 *   struct consists of a single element.  This is because of
 *   horrible compile/linker problems.  As a result, there are
 *   custom structs for almost everything.  In link (always sets)
 *   usage, the linked element is access by ".set"
 ****************************************************************/
#ifndef _TYPES_
#define _TYPES_

#include <fstream>
#include <string>
#include <time.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace::std;
////////////////////////////////////////////////////////////////
/* CFG types                                                  */
////////////////////////////////////////////////////////////////

// P0 CFG Rule
typedef struct{
	string lhs;
} P0;

// P1 CFG Rule
typedef struct{
	string lhs;
	string rhs;
} P1;

// P2 CFG Rule
typedef struct{
	string lhs;
	string rhs1;
	string rhs2;
} P2;

// PL CFG Rule
typedef struct{
	string lhs;
	string rhs;
} PL;

// A CFG
struct CFG{
	vector<P0> vp0;
	vector<P1> vp1;
	vector<P2> vp2;
	vector<PL> vpl;
	unordered_set<string> starts;
	vector<vector<string>> samples;
	unordered_map<string, unordered_set<string>> chains;
};

////////////////////////////////////////////////////////////////
/* CFG (Contectual) types                                     */
////////////////////////////////////////////////////////////////
/* Types are defined as follows:
 * - Type
 * - Type hash function (for stl set)
 * - Struct of set<type> (necessary abstraction) */

// A single context
struct context{
	context(){ lhs.resize(0); rhs.resize(0); }
	vector<string> lhs;
	vector<string> rhs;
	bool operator==(const context &other) const {
		return (lhs == other.lhs && rhs == other.rhs);
	}
};

// context hash definition (for stl sets)
namespace std {
	template <>	struct hash <context>
	{
		size_t operator()(const context &c) const {
			string lhs, rhs;
			for (auto s : c.lhs)
				lhs += s;
			for (auto s : c.rhs)
				rhs += s;
			return (hash<string>()(lhs) ^ (hash<string>()(rhs)));
		}
	};
}

// context unordered_set (link struct)
struct contextSet{
	unordered_set<context> set;
	bool operator==(const contextSet &other) const {
		return set == other.set;
	}
};

// contextSet hash definition (for stl sets)
namespace std {
	template <>	struct hash <contextSet>
	{
		size_t operator()(const contextSet &s) const {
			string lhs, rhs;
			for (auto c : s.set){
				for (auto s : c.lhs)
					lhs += s;
				for (auto s : c.rhs)
					rhs += s;
			}
			return (hash<string>()(lhs) ^ (hash<string>()(rhs)));
		}
	};
}
////////////////////////////////////////////////////////////////
// P0C Rule
struct P0C{
	P0C(contextSet a)
		: lhs(a) {}
	contextSet lhs;
	bool operator==(const P0C &other) const {
		return (lhs == other.lhs);
	}
};

// P0C hash definition (for stl sets)
namespace std {
	template <>	struct hash <P0C>
	{
		size_t operator()(const P0C &p0c) const {
			string lhs, rhs;
			for (auto c : p0c.lhs.set){
				for (auto s : c.lhs)
					lhs += s;
				for (auto s : c.rhs)
					rhs += s;
			}
			return (hash<string>()(lhs) ^ (hash<string>()(rhs)));
		}
	};
}

// P0C unordered_set (link struct)
struct P0CSet{
	unordered_set<P0C> set;
	bool operator==(const P0CSet &other) const {
		return (set == other.set);
	}
};
////////////////////////////////////////////////////////////////
// P1C Rule
struct P1C{
	P1C(contextSet a, contextSet b)
		: lhs(a), rhs(b) {}
	contextSet lhs;
	contextSet rhs;
	bool operator==(const P1C &other) const {
		return (lhs == other.lhs && rhs == other.rhs);
	}
};

// P1C hash definition (for stl sets)
namespace std {
	template <>	struct hash <P1C>
	{
		size_t operator()(const P1C &p1c) const {
			string lhs, rhs;
			for (auto c : p1c.lhs.set){
				for (auto s : c.lhs)
					lhs += s;
				for (auto s : c.rhs)
					lhs += s;
			}
			for (auto c : p1c.rhs.set){
				for (auto s : c.lhs)
					rhs += s;
				for (auto s : c.rhs)
					rhs += s;
			}
			return (hash<string>()(lhs) ^ (hash<string>()(rhs)));
		}
	};
}

// P1C unordered_set (link struct)
struct P1CSet{
	unordered_set<P1C> set;
	bool operator==(const P1CSet &other) const {
		return (set == other.set);
	}
};
////////////////////////////////////////////////////////////////
// P2 Contextual Rule
struct P2C{
	P2C(contextSet a, contextSet b, contextSet c)
		:lhs(a), rhs1(b), rhs2(c) {}
	contextSet lhs;
	contextSet rhs1;
	contextSet rhs2;
	bool operator==(const P2C &other) const {
		return (lhs == other.lhs && rhs1 == other.rhs1 && rhs2 == other.rhs2);
	}
};

// P2C hash definition (for stl sets)
namespace std {
	template <>	struct hash <P2C>
	{
		size_t operator()(const P2C &p2c) const {
			string lhs, rhs;
			for (auto c : p2c.lhs.set){
				for (auto s : c.lhs)
					lhs += s;
				for (auto s : c.rhs)
					lhs += s;
			}
			for (auto c : p2c.rhs1.set){
				for (auto s : c.lhs)
					rhs += s;
				for (auto s : c.rhs)
					rhs += s;
			}
			return (hash<string>()(lhs) ^ (hash<string>()(rhs)));
		}
	};
}

// P2C unordered_set (link struct)
struct P2CSet{
	unordered_set<P2C> set;
	bool operator==(const P2CSet &other) const {
		return (set == other.set);
	}
};
////////////////////////////////////////////////////////////////
// PL Contextual Rule
struct PLC{
	PLC(contextSet a, string b)
		: lhs(a), rhs(b) {}
	contextSet lhs;
	string rhs;
	bool operator==(const PLC &other) const {
		return (lhs == other.lhs && rhs == other.rhs);
	}
};

// PLC hash definition (for stl sets)
namespace std {
	template <>	struct hash <PLC>
	{
		size_t operator()(const PLC &plc) const {
			string lhs;
			for (auto c : plc.lhs.set){
				for (auto s : c.lhs)
					lhs += s;
				for (auto s : c.rhs)
					lhs += s;
			}
			return (hash<string>()(lhs) ^ (hash<string>()(plc.rhs)));
		}
	};
}

// PLC unordered_set (link struct)
struct PLCSet{
	unordered_set<PLC> set;
	bool operator==(const PLCSet &other) const {
		return (set == other.set);
	}
};
////////////////////////////////////////////////////////////////
// A CFG composed of contextual rules
struct CFGC{
	P0CSet sp0c;
	P1CSet sp1c;
	P2CSet sp2c;
	PLCSet splc;
};

////////////////////////////////////////////////////////////////
/* Equality helper funcions                                   */
////////////////////////////////////////////////////////////////

// Check if two contexts are equal
bool equal(context a, context);

// Check if two string vectors are equal
bool equal(vector<string> a, vector<string> b);

// Check if two context vectors are equal
bool equal(vector<context> a, vector<context> b);

////////////////////////////////////////////////////////////////
/* Vector search utility functions                            */
////////////////////////////////////////////////////////////////

// Search for a string w in vector v
bool search(vector<string> v, string w);

// Search for a context c in vector v
bool search(vector<context> v, context c);

// Search for a context in a 2d vector of contexts
bool search(vector<vector<context>> v2, context c);

// Search for a subset string in a list of subsets
bool search(vector<vector<string>> v, vector<string> s);

////////////////////////////////////////////////////////////////
/* Subset utility functions                                   */
////////////////////////////////////////////////////////////////

// Check if a set of contexts c1 is a subset of c2
bool subset(vector<context> c1, vector<context> c2);

// Check if a set of contexts c1 is contained within cl
bool subset(vector<context> c1, vector<vector<context>> cl);

////////////////////////////////////////////////////////////////
/* Utility Functions                                          */
////////////////////////////////////////////////////////////////

// Takes the input file and creates an CFG object for the target grammar
CFG extractCFG(char* file);

void runtime(clock_t t0);

////////////////////////////////////////////////////////////////
/* Printing functions                                         */
////////////////////////////////////////////////////////////////

// Print a context
void printContext(const context &c);

// Print a set of contexts
void printContextSet(const unordered_set<context> &S);

// Print a P0C rule
void printP0C(const P0C &p0c);

// Print a P0CSet
void printP0CSet(const P0CSet &sp0c);

// Print a P1C rule
void printP1C(const P1C &p1c);

// Print a P1CSet
void printP1CSet(const P1CSet &sp1c);

// Print a P2C rule
void printP2C(const P2C &p2c);

// Print a P2CSet
void printP2CSet(const P2CSet &sp2c);

// Print a PLC rule
void printPLC(const PLC &plc);

// Print a PLCSet
void printPLCSet(const PLCSet &splc);

// Print at the beginning of each new sample
void printProcessing(const vector<string> &w);

// Print the contents of D
void printD(const vector<vector<string>> &D);

// Print a vector of substrings
void printSubstringVector(const vector<vector<string>> &s);

// Print a CFG
void printCFG(const CFG &G);

#endif