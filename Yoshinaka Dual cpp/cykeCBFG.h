/****************************************************************
* File: cykeCBFG.h
* Definitions for an Oracle for CBFGs using the CYK algorithm
* with empty categories permitted
* David Peatman - Updated 8/27/14
****************************************************************
* Notes:
* - a CBFGOracle class contains all the necessary functions for
*   the CYK algorithm
* - it is intended to be used as a member of a CBFG class
****************************************************************/
#ifndef _CYKCBFG_
#define _CYKCBFG_

#include <string>
#include <vector>

#include "grammars.h"

class CBFG;
class CBFGOracle{
public:
	CBFGOracle(CBFG* G);
	bool accepts(const vector<string> w,
				 const vector<PLC> vplc,
				 const vector<P2C> vp2c);
private:
	CBFG* parent;
	void initializeChart(const vector<string> w,
						 const vector<PLC> vplc,
						 vector<vector<context>>** chart);
	void closeChart(const vector<P2C> vp2c,
					vector<vector<context>>** chart,
					unsigned int n);
};

#endif