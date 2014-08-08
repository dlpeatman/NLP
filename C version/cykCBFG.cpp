#include <iostream>
#include <iomanip>

#include "cykCBFG.h"

// Prints the cykCBFG chart for debugging purposes
void printChart(vector<vector<context>>** chart, unsigned int size){
	for (unsigned int i = 0; i <= size; i++){
		for (unsigned int j = i; j <= size; j++){
			int x = chart[i][j].size();
			cout << x;
		}
		cout << endl;
	}
}

CBFGOracle::CBFGOracle(CBFG* G)
	: parent(G) {}

void CBFGOracle::initializeChart(vector<string> w, vector<PLCRule> PL, vector<vector<context>>** chart){
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j = 0; j < PL.size(); j++){
			if (w[i] == PL[j].s)
				chart[i][i + 1].push_back(PL[j].c);
		}
	}
}

void CBFGOracle::closeChart(vector<PCRule> P, vector<vector<context>>** chart, unsigned int n){
	for (unsigned int width = 1; width <= n; width++){
		for (unsigned int start = 0; start <= n - width; start++){
			unsigned int end = start + width;
			for (unsigned int mid = start + 1; mid < end; mid++){
				for (unsigned int i = 0; i < P.size(); i++){
					if (subset(P[i].rhs1, chart[start][mid])
						&& subset(P[i].rhs2, chart[mid][end])
						&& !subset(P[i].lhs, chart[start][end])) // Stops redundacies
						chart[start][end].push_back(P[i].lhs);
				}
			}
		}
	}
}

bool CBFGOracle::accepts(vector<string> w, const vector<PLCRule> PL, const vector<PCRule> P){
	unsigned int n = w.size();
	vector<vector<context>>** chart = new vector<vector<context>>*[n + 1];	// Chart is dynamically allocated 2D array
	for (unsigned int i = 0; i <= n; ++i)
		chart[i] = new vector<vector<context>>[n + 1];

	// printChart(chart, n);
	initializeChart(w, PL, chart);
	// printChart(chart, n);
	closeChart(P, chart, n);
	// printChart(chart, n);

	// Is the top left cell the start symbol?
	context c;
	bool success = search(chart[0][n], c) ? true : false;

	for (unsigned int i = 0; i <= n; ++i)	// Delete the dynamically allocated array from memory
		delete[] chart[i];
	delete[] chart;

	return success;
}