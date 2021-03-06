/****************************************************************
 * File: cyke.h
 * Call CYK algorithm for both CFGs and contectual CFGs
 * David Peatman - Updated 9/23/14
 ****************************************************************/
#ifndef _CYK_
#define _CYK_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "types.h"

using namespace::std;

// A class to record what calls have been made
class History{
public:
	int checkHistory(const vector<string> w);
	void add(string s, bool b);
	int size(){ return map.size(); }
private:
	unordered_map<string, bool> map;
};

// History classes for CFG to be accessed anywhere (CFGC histories are instantiated in each CFGC)
extern History historyG; // History for target grammar G


unordered_map<string, bool> buildNullable(const CFG &G);
unordered_map<string, unordered_set<string>> buildChains(const CFG &G, unordered_map<string, bool> nullable);

bool accepts(const vector<string> &w, const CFG &G, History &history);
bool accepts(const vector<string> &w, const CFG &G, const unordered_map<string, unordered_set<string>> &chain, History &history);

void checkSamples(const CFG &G);
void checkLearner(const CFG &G, History h, const vector<vector<string>> &samples);

#endif