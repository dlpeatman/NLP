#include <iostream>
#include <iomanip>

#include "cyk.h"

// Prints the cyk chart for debugging purposes
void printChart(vector<string>** chart, unsigned int size){
	for (unsigned int i = 0; i <= size; i++){
		for (unsigned int x = 0; x < (i)* 6; x++)
			cout << " ";
		for (unsigned int j = i + 1; j <= size; j++){
			string temp;
			if (chart[i][j].size() != 0)
				temp = chart[i][j][0];
			for (unsigned int k = 1; k < chart[i][j].size(); k++){
				temp += ",";
				temp += chart[i][j][k];
			}
			cout << setw(4) << temp << ": ";
		}
		cout << endl;
	}
}

// Just like python version
void CFGOracle::initializeChart(vector<string> w, const vector<PLRule> PL, vector<string>** chart){
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j = 0; j < PL.size(); j++){
			if (w[i] == PL[j].right)
				chart[i][i + 1].push_back(PL[j].left);
		}
	}
}

// Just like python version
void CFGOracle::closeChart(const vector<PRule> P, vector<string>** chart, unsigned int n){
	for (unsigned int width = 1; width <= n; width++){
		for (unsigned int start = 0; start <= n-width; start++){
			unsigned int end = start + width;
			for (unsigned int mid = start + 1; mid < end; mid++){
				for (unsigned int i = 0; i < P.size(); i++){
					if (search(chart[start][mid], P[i].one)
						&& search(chart[mid][end], P[i].two)
						&& !search(chart[start][end], P[i].left)) // Stops redundacies
						chart[start][end].push_back(P[i].left);
				}
			}
		}
	}
}

bool CFGOracle::accepts(const vector<string> w, vector<PLRule> PL, vector<PRule> P, string start){
	unsigned int n = w.size();
	vector<string>** chart = new vector<string>*[n+1];	// Chart is dynamically allocated 2D array
	for (unsigned int i = 0; i <= n; ++i)
		chart[i] = new vector<string>[n + 1];

	initializeChart(w, PL, chart);
	closeChart(P, chart, n);

	// printChart(chart, n);

	// Is the top left cell the start symbol?
	bool success = search(chart[0][n], start) ? true : false;

	for (unsigned int i = 0; i <= n; ++i)	// Delete the dynamically allocated array from memory
		delete [] chart[i];
	delete [] chart;

	// add the string to the oracle's call history
	string s;
	for (unsigned int i = 0; i < w.size(); i++)
		s += w[i];
	history.emplace(s, success);

	return success;
}

// Returns -1 if w has not been called
// If w has been called, it returns its value (true/false)
int CFGOracle::checkHistory(vector<string> w){
	// string vectors have to be converted into a single, concatenated string
	string s;
	for (unsigned int i = 0; i < w.size(); i++)
		s += w[i];


	unordered_map<string, bool>::const_iterator it = history.find(s);
	if (it == history.end())
		return -1;
	else
		return it->second;
}