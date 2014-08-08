#ifndef _CYK_
#define _CYK_

#include <string>
#include <unordered_map>
#include <vector>

#include "types.h"
#include "grammars.h"

class CFG;
class CFGOracle{
public:
	bool accepts(const vector<string> w, vector<PLRule> PL, vector<PRule> P, string start);
	unordered_map<string, bool> history;
	int checkHistory(vector<string> w);
private:
	void initializeChart(vector<string> w, const vector<PLRule> PL, vector<string>** chart);
	void closeChart(const vector<PRule> P, vector<string>** chart, unsigned int n);
};

#endif