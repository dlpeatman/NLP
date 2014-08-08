#ifndef _CYKCBFG_
#define _CYKCBFG_

#include <string>
#include <vector>

#include "grammars.h"

class CBFG;
class CBFGOracle{
public:
	CBFGOracle(CBFG* G);
	bool accepts(const vector<string> w, const vector<PLCRule> PL, const vector<PCRule> P);
private:
	CBFG* parent;
	void initializeChart(vector<string> w, vector<PLCRule> PL, vector<vector<context>>** chart);
	void closeChart(vector<PCRule> P, vector<vector<context>>** chart, unsigned int n);

};

#endif