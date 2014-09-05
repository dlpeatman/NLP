/****************************************************************
* File: cyke.cpp
* Implements ckye.h
* David Peatman - Updated 8/30/14
****************************************************************/
#include <iostream>
#include <iomanip>
#include <queue>

#include "cyke.h"

History history;
History historyC;

// Prints the CFG Matrix for debugging purposes
void printMatrix(unordered_set<string>** matrix, unsigned int size){
	for (unsigned int j = 0; j < size; j++){
		for (unsigned int i = 0; i < size; i++){
			if (j <= i){
				cout << "matrix[" << i << "][" << j << "]: ";
				for (auto s : matrix[i][j])
					cout << s << " ";
				cout << endl;
			}
		}
	}
}

// Prints a CGFC Matrix for debugging purposes
void printMatrix(vector<vector<context>>** matrix, unsigned int size){
	for (unsigned int i = 0; i <= size; i++){
		for (unsigned int j = i; j <= size; j++){
			int x = matrix[i][j].size();
			cout << x;
		}
		cout << endl;
	}
}

// Prints chain map
void printChains(unordered_map<string, unordered_set<string>> chains){
	for (auto x : chains){
		cout << x.first << ":";
		for (auto C : x.second)
			cout << " " << C;
		cout << endl;
	}
}

////////////////////////////////////////////////////////////////
/* CFG CYK algorithm                                          */
////////////////////////////////////////////////////////////////

unordered_map<string, bool> buildNullable(const CFG &G){
	unordered_map<string, bool> nullable;
	unordered_map<string, unordered_set<string>> occurs1;
	struct pair{
		string lhs;
		string rhs;
	};
	unordered_map<string, vector<pair>> occurs2;
	queue<string> todo;

	for (unsigned int i = 0; i < G.vp1.size(); i++){	// unary rules
		nullable.emplace(G.vp1[i].lhs, false);
		occurs1[G.vp1[i].rhs].emplace(G.vp1[i].lhs);
	}
	for (unsigned int i = 0; i < G.vp2.size(); i++){	// binary rules
		nullable.emplace(G.vp2[i].lhs, false);
		pair AC, AB;
		AC.lhs = G.vp2[i].lhs;
		AC.rhs = G.vp2[i].rhs2;
		occurs2[G.vp2[i].rhs1].push_back(AC);
		AB.lhs = G.vp2[i].lhs;
		AB.rhs = G.vp2[i].rhs1;
		occurs2[G.vp2[i].rhs2].push_back(AB);
	}
	for (unsigned int i = 0; i < G.vpl.size(); i++)		// lexical rules
		nullable.emplace(G.vpl[i].lhs, false);
	for (unsigned int i = 0; i < G.vp0.size(); i++){	// empty rules
		nullable.emplace(G.vp0[i].lhs, true);
		todo.push(G.vp0[i].lhs);
	}
	// todo loop
	while (todo.empty() == false){
		string B = todo.front();
		todo.pop();
		// Using an iterator, go through all elements in occurs1[B]
		for (auto& it : occurs1[B]){
			if (nullable[it] == false){
				nullable[it] = true;
				todo.push(it);
			}
		}
		// Using an iterator, go through all elements in occurs2[B]
		for (auto& it : occurs2[B]){
			if (nullable[it.lhs] == true && nullable[it.lhs] == false){
				nullable[it.lhs] = true;
				todo.push(it.lhs);
			}
		}
	}

	return nullable;
}

// chains maps each symbol x to {C| C =>* x} (key:string x, value:set of strings C1, C2, ...)
unordered_map<string, unordered_set<string>> buildChains(const CFG &G, unordered_map<string, bool> nullable){
	unordered_map<string, unordered_set<string>> chains;

	// Emplace is set insertion (unique elements only)
	// Also references existing element or creates new one automatically
	// Initialize with non-terminals
	for (auto& it : nullable)
		chains[it.first].emplace(it.first);
	// Lexical Rules
	for (auto& pl : G.vpl)
		chains[pl.rhs].emplace(pl.lhs);	
	// Unary rules
	for (auto& p1 : G.vp1)
		chains[p1.rhs].emplace(p1.lhs);
	// Binary rules
	for (auto& p2 : G.vp2){
		if (nullable[p2.rhs1])
			chains[p2.rhs2].emplace(p2.lhs);
		if (nullable[p2.rhs2])
			chains[p2.rhs1].emplace(p2.lhs);
	}
	// Compute transitive closure
	for (auto& i : chains)	// i,j,k are iterators to chains elements
		for (auto& j : chains)
			for (auto& k : chains)
				if (i.second.find(k.first) != i.second.end()	// k is in chains[i]
					&& k.second.find(j.first) != k.second.end())	// j is in chains[k]
					i.second.emplace(j.first);	// add j key to i set

	return chains;
}

void buildMatrix(const vector<string> &w, const CFG &G,
	unordered_map<string, unordered_set<string>> chains,
	unordered_set<string>** matrix, const unsigned int n)
{
	// Lexical initialization
	for (unsigned int i = 0; i < n; i++)
		for (auto C : chains[w[i]])	// For every {C| C =>* w[i]}
			matrix[i][i].emplace(C);	// Add C to the set at [i][i]

	// printMatrix(matrix, n);

	for (int j = 1; j < n; j++)
		for (int i = j; i >= 0; i--)
			for (int h = i; h < j; h++)
				for (auto p2 : G.vp2) // For every P2 in G (A -> yz)
					if (matrix[i][h].find(p2.rhs1) != matrix[i][h].end()
						&& matrix[h + 1][j].find(p2.rhs2) != matrix[h + 1][j].end())
						for (auto C : chains[p2.lhs]) // Add every C =>* A from chains
							matrix[i][j].emplace(C);
}

// Called with calculated chains/nullable maps
bool accepts(const vector<string> w, const CFG &G, unordered_map<string, unordered_set<string>> chains){
	// If this call has been made before, return check (previous result)
	int check = history.checkHistory(w);
	if (check == 0)
		return false;
	else if (check == 1)
		return true;

	// Matrix is dynamically allocated 2D array of sets of strings
	unsigned int n = w.size();
	unordered_set<string>** matrix = new unordered_set<string>*[n];
	for (unsigned int i = 0; i < n; ++i)
		matrix[i] = new unordered_set<string>[n];

	// printChains(chains);
	// Do all the CYK magic to the matrix
	buildMatrix(w, G, chains, matrix, n);

	// printMatrix(matrix, n);

	// Is the top left cell the start symbol?
	bool success = false;
	for (auto s : G.starts){	// Test for each start symbol
		if (matrix[0][n-1].find(s) != matrix[0][n-1].end()){
			success = true;
			break;
		}
	}

	for (unsigned int i = 0; i < n; ++i)	// Delete the dynamically allocated array from memory
		delete[] matrix[i];
	delete[] matrix;

	// add the string to the oracle's call history
	// note that unordered maps only accept standard types
	// to account for this, we take all elements of w and concatenate them into s
	string s;
	for (unsigned int i = 0; i < w.size(); i++)
		s += w[i];
	history.add(s, success);

	return success;
}

// Called if no chains/nullable maps are pre-calculated
bool accepts(const vector<string> w, const CFG &G){
	auto chains = buildChains(G, buildNullable(G));
	return accepts(w, G, chains);
}

////////////////////////////////////////////////////////////////
/* CFGC CYK algorithm overload                                */
////////////////////////////////////////////////////////////////

void buildMatrix(const vector<string> &w,
	const vector<P0C> &vp0c,
	const vector<P1C> &vp1c,
	const vector<P2C> &vp2c,
	const vector<PLC> &vplc,
	vector<contextSet>** matrix,
	unsigned int n)
{
	//for (unsigned int i = 0; i < w.size(); i++){
	//	for (unsigned int j = 0; j < vplc.size(); j++){
	//		if (w[i] == vplc[j].rhs)
	//			matrix[i][i + 1].push_back(vplc[j].lhs);
	//	}
	//}
	//for (unsigned int width = 1; width <= n; width++){
	//	for (unsigned int start = 0; start <= n - width; start++){
	//		unsigned int end = start + width;
	//		for (unsigned int mid = start + 1; mid < end; mid++){
	//			for (unsigned int i = 0; i < vp2c.size(); i++){
	//				if (subset(vp2c[i].rhs1, matrix[start][mid])
	//					&& subset(vp2c[i].rhs2, matrix[mid][end])
	//					&& !subset(vp2c[i].lhs, matrix[start][end])) // Stops redundacies
	//					matrix[start][end].push_back(vp2c[i].lhs);
	//			}
	//		}
	//	}
	//}
}

bool accepts(const vector<string> w, const CFGC &G)
{
	unsigned int n = w.size();
	vector<contextSet>** matrix = new vector<contextSet>*[n + 1];	// Matrix is dynamically allocated 2D array
	for (unsigned int i = 0; i <= n; ++i)
		matrix[i] = new vector<contextSet>[n + 1];

	// buildMatrix(w, G.vp0c, G.vp1c, G.vp2c, G.vplc, matrix, n);
	// printMatrix(matrix, n);

	// Is the top left cell the start symbol?
	context c;	// i.e. empty context
	//bool success = search(matrix[0][n], c) ? true : false;

	for (unsigned int i = 0; i <= n; ++i)	// Delete the dynamically allocated array from memory
		delete[] matrix[i];
	delete[] matrix;

	//string s;
	//for (unsigned int i = 0; i < w.size(); i++)
	//	s += w[i];
	//historyC.add(s, success);

	return true;
}

////////////////////////////////////////////////////////////////
/* Call History stuff                                         */
////////////////////////////////////////////////////////////////

// Returns -1 if w has not been called
// If w has been called, it returns its value (true/false)
int History::checkHistory(vector<string> w){
	// string vectors have to be converted into a single, concatenated string
	string s;
	for (unsigned int i = 0; i < w.size(); i++)
		s += w[i];


	unordered_map<string, bool>::const_iterator it = map.find(s);
	if (it == map.end())
		return -1;
	else
		return it->second;
}

void History::add(string s, bool b){
	map.emplace(s, b);
}

////////////////////////////////////////////////////////////////
/* Utility Functions                                          */
////////////////////////////////////////////////////////////////

// Checks the input samples for a grammar to make sure they're accepted
void checkSamples(const CFG &G){
	auto chains = buildChains(G, buildNullable(G));
	for (unsigned int i = 0; i < G.samples.size(); i++){
		bool accepted = accepts(G.samples[i], G, chains);
		if (accepted){
			cout << "Accepted by target grammar:";
			for (unsigned int j = 0; j < G.samples[i].size(); j++)
				cout << " " << G.samples[i][j];
			cout << endl;
		}
		else{
			cout << "Rejected by target grammar:";
			for (unsigned int j = 0; j < G.samples[i].size(); j++)
				cout << " " << G.samples[i][j];
			cout << endl;
			exit(1);
		}
	}
}