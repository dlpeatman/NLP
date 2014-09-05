#include <iostream>
#include <iomanip>

#include "cykeCBFG.h"

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

void CBFGOracle::initializeChart(const vector<string> w, const vector<PLC> vplc, vector<vector<context>>** chart){
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j = 0; j < vplc.size(); j++){
			if (w[i] == vplc[j].rhs)
				chart[i][i + 1].push_back(vplc[j].lhs);
		}
	}
}

void CBFGOracle::closeChart(const vector<P2C> vp2c, vector<vector<context>>** chart, unsigned int n){
	for (unsigned int width = 1; width <= n; width++){
		for (unsigned int start = 0; start <= n - width; start++){
			unsigned int end = start + width;
			for (unsigned int mid = start + 1; mid < end; mid++){
				for (unsigned int i = 0; i < vp2c.size(); i++){
					if (subset(vp2c[i].rhs1, chart[start][mid])
						&& subset(vp2c[i].rhs2, chart[mid][end])
						&& !subset(vp2c[i].lhs, chart[start][end])) // Stops redundacies
						chart[start][end].push_back(vp2c[i].lhs);
				}
			}
		}
	}
}

bool CBFGOracle::accepts(const vector<string> w, const vector<PLC> vplc, const vector<P2C> vp2c){
	unsigned int n = w.size();
	vector<vector<context>>** chart = new vector<vector<context>>*[n + 1];	// Chart is dynamically allocated 2D array
	for (unsigned int i = 0; i <= n; ++i)
		chart[i] = new vector<vector<context>>[n + 1];

	// printChart(chart, n);
	initializeChart(w, vplc, chart);
	// printChart(chart, n);
	closeChart(vp2c, chart, n);
	// printChart(chart, n);

	// Is the top left cell the start symbol?
	context c;
	bool success = search(chart[0][n], c) ? true : false;

	for (unsigned int i = 0; i <= n; ++i)	// Delete the dynamically allocated array from memory
		delete[] chart[i];
	delete[] chart;

	return success;
}