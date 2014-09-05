/****************************************************************
 * File: cyke.h
 * Call CYK algorithm for both CFGs and contectual CFGs
 * David Peatman - Updated 9/03/14
 ****************************************************************/
#ifndef _CYK_
#define _CYK_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "types.h"

using namespace::std;

unordered_map<string, bool> buildNullable(const CFG &G);
unordered_map<string, unordered_set<string>> buildChains(const CFG &G, unordered_map<string, bool> nullable);

bool accepts(const vector<string> w, const CFG &G);
bool accepts(const vector<string> w, const CFG &G, unordered_map<string, unordered_set<string>> chain);

// Overload for CFGCs
bool accepts(const vector<string> w, const CFGC &G);

// A class to record what calls have been made
class History{
public:
	int checkHistory(const vector<string> w);
	void add(string s, bool b);
	int size(){ return map.size(); }
private:
	unordered_map<string, bool> map;
};

// History classes for both CFG and CFGC oracles made to be accessed anywhere
extern History history;
extern History historyC;

void checkSamples(const CFG &G);

#endif