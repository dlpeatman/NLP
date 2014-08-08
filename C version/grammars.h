#ifndef _GRAMMARS_
#define _GRAMMARS_

#include <iostream>
#include <string>
#include <vector>

#include "cyk.h"
#include "cykCBFG.h"

using namespace::std;

class CFGOracle;

class CFG{
public:
	CFG(string s, CFGRules r, vector<vector<string>> sam);
	void print();
	void checkSamples();
	bool accepts(vector<string> w);
	const string start;
	const CFGRules rules;
	const vector<vector<string>> samples;
	int queries;
	CFGOracle* oracle;
};

class CBFGOracle;

class CBFG{
public:
	CBFG(CBFGRules r);
	void print();
	void checkSamples(vector<vector<string>> s);
	bool accepts(vector<string> w);
	CBFGRules rules;
	CBFGOracle* oracle;
};

#endif