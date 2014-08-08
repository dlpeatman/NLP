#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "cyk.h"
#include "cykCBFG.h"
#include "grammars.h"

using namespace::std;

////////////////////////////////////////////////////////////////
/* Utility / Print Functions *//////////////////////////////////
////////////////////////////////////////////////////////////////

// Takes the input file and creates an CFG object for the target grammar
CFG* extract(char* file){
	if (file == NULL){
		cout << "No input file given!" << endl;
		exit(1);
	}

	ifstream input(file);
	string line;

	string start;
	CFGRules rules;
	vector<vector<string>> samples;

	if (input.is_open())
	{
		enum test_data_types{ starter, lexical, nonlexical, sample} type;
		type = starter;

		while (getline(input, line)){
			if (line == "start:"){
				type = starter;
				continue;
			}
			else if (line == "lexical:"){
				type = lexical;
				continue;
			}
			else if (line == "nonlexical:"){
				type = nonlexical;
				continue;
			}
			else if (line == "samples:"){
				type = sample;
				continue;
			}

			PLRule PL;
			PRule P;
			string temp = "";
			int num = 0;

			switch (type){
			case starter:
				start = line;
				break;
			case lexical:
				unsigned int i;
				for (i = 0; i < line.length() && line[i] != ','; i++);
				PL.left = line.substr(0, i);
				PL.right = line.substr(i+1, line.length() - i - 1);
				rules.PL.push_back(PL);
				break;
			case nonlexical:
				for (unsigned int i = 0; i < line.length(); i++){
					if (line[i] != ',')
						temp += line[i];
					else{
						switch (num){
						case 0:
							P.left = temp;
							break;
						case 1:
							P.one = temp;
							break;
						}
						num++;
						temp = "";
					}
				}
				P.two = temp;
				rules.P.push_back(P);
				break;
			case sample:
				vector<string> tempsample;
				for (unsigned int i = 0; i < line.length(); i++){
					if (line[i] != ' ')
						temp += line[i];
					else{
						tempsample.push_back(temp);
						temp = "";
					}
				}
				tempsample.push_back(temp);
				samples.push_back(tempsample);
			}
		}

		input.close();

		CFG* G = new CFG(start, rules, samples);
		return G;
	}
	else {
		cout << "Unable to open file" << endl;
		return NULL;
	}
}

/* Printing functions */
// Print at the beginning of each new sample
void printProcessing(vector<string> w){
	cout << "Processing input: ";
	for (unsigned int i = 0; i < w.size(); i++)
		cout << w[i] + " ";
	cout << endl;
}

// Print the contents of D
void printD(vector<vector<string>> D){
	string s = "D: ";
	for (unsigned int i = 0; i < D.size(); i++){
		s.append("\"");
		for (unsigned int j = 0; j < D[i].size(); j++)
			s.append(D[i][j] + " ");
		s.pop_back();
		s.append("\", ");
	}
	s.pop_back();
	s.pop_back();
	s.append("\n");
	cout << s;
}

// Print a vector of contexts
void printContextVector(vector<context> v){
	cout << "Contexts: ";
	for (unsigned int i = 0; i < v.size(); i++){
		cout << "(";
		for (unsigned int j = 0; j < v[i].lhs.size(); j++){
			cout << v[i].lhs[j];
			if (j < v[i].lhs.size() - 1)
				cout << " ";
		}
		cout << ", ";
		for (unsigned int j = 0; j < v[i].rhs.size(); j++){
			cout << v[i].rhs[j];
			if (j < v[i].rhs.size() - 1)
				cout << " ";
		}
		cout << ") ";
	}
	cout << endl;
}

// Print a vector of substrings
void printSubstringVector(vector<vector<string>> s){
	cout << "Substrings: ";
	for (unsigned int i = 0; i < s.size(); i++){
		cout << "(";
		for (unsigned int j = 0; j < s[i].size(); j++){
			cout << s[i][j];
			if (j < s[i].size() - 1)
				cout << " ";
		}
		cout << ") ";
	}
	cout << endl;
}

////////////////////////////////////////////////////////////////
/* Main part of algorithm */////////////////////////////////////
////////////////////////////////////////////////////////////////

// Just like python version
void addNEsubstrings(vector<vector<string>> &sofar, vector<string> w){
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j = i; j <= w.size(); j++){
			vector<string> temp;
			for (unsigned int k = i; k < j; k++) // Python: s=w[i:j]
				temp.push_back(w[k]);

			if (!search(sofar, temp)) // If temp is not already in sofar, add it
				sofar.push_back(temp);
		}
	}
}

// Just like python vesion
void addContexts(vector<context> &sofar, vector<string> w){
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j = i + 1; j < w.size() + 1; j++){
			context c;
			for (unsigned int k = 0; k < i; k++)
				c.lhs.push_back(w[k]);
			for (unsigned int k = j; k < w.size(); k++)
				c.rhs.push_back(w[k]);

			if (!search(sofar, c))	// If c is not already in sofar, add it
				sofar.push_back(c);
		}
	}
}

// Mostly like python version
vector<context> FL(vector<context> F, vector<string> w, CFG* G){
	vector<context> features;
	for (unsigned int i = 0; i < F.size(); i++){
		// odot operation:
		vector<string> lur;
		// Add left side of the context
		for (unsigned int j = 0; j < F[i].lhs.size(); j++)
			lur.push_back(F[i].lhs[j]);
		// Add string
		for (unsigned int j = 0; j < w.size(); j++)
			lur.push_back(w[j]);
		// Add right side of context
		for (unsigned int j = 0; j < F[i].rhs.size(); j++)
			lur.push_back(F[i].rhs[j]);

		// Test if lur is in language
		if ((G->oracle->checkHistory(lur) == -1 && G->accepts(lur))
			|| G->oracle->checkHistory(lur) == 1)
			features.push_back(F[i]);
	}
	return features;
}

// Also mostly like python version
CBFG g(vector<vector<string>> K, vector<context> F, CFG* target){
	vector<PLCRule> PL;
	vector<PCRule> P;

	for (unsigned int i = 0; i < K.size(); i++){
		vector<string> w = K[i];
		vector<context> lhs = FL(F, w, target); // All the valid contexts of w
		if (w.size() == 1){	// Lexical rule
			string rhs = w[0];
			PLCRule rule;
			rule.c = lhs;
			rule.s = rhs;
			if (!search(PL, rule))	// Prevent redundancies
				PL.push_back(rule);
		}
		else {	// Nonlexical rule
			for (unsigned int j = 1; j < w.size(); j++){
				vector<string> wa;
				vector<string> wb;
				for (unsigned int k = 0; k < j; k++)
					wa.push_back(w[k]);
				for (unsigned int k = j; k < w.size(); k++)
					wb.push_back(w[k]);
				vector<context> fl1 = FL(F, wa, target);
				vector<context> fl2 = FL(F, wb, target);
				PCRule rule;
				rule.lhs = lhs;
				rule.rhs1 = fl1;
				rule.rhs2 = fl2;
				if (!search(P, rule))	// Prevent redundancies
					P.push_back(rule);
			}
		}
	}

	CBFGRules rules;
	rules.PL = PL;
	rules.P = P;
	CBFG Ghat = CBFG(rules);
	return Ghat;
}

// Just like python version
bool notDinLG(vector<vector<string>> D, CBFG G){
	for (unsigned int i = 0; i < D.size(); i++)
		if (!G.accepts(D[i]))
			return true;
	return false;
}

// Just like python version
bool reallyLongCond(vector<vector<string>> SubD, vector<vector<string>> K,
	vector<context> ConD, vector<context> F, CFG* G){
	for (unsigned int i = 0; i < SubD.size(); i++){
		vector<context> FLi = FL(F, SubD[i], G);
		for (unsigned int j = 0; j < K.size(); j++){
			vector<context> FLj = (FL(F, K[i], G));
			if (subset(FLj, FLi))
				for (unsigned int k = 0; k < ConD.size(); k++){
				// The following 10 lines are the same as the 2 lines in python (stupid c++)
				// We just Odot (insert) the given string from K with the context from ConD
					vector<string> lur;
					// Add left side of the context
					for (unsigned int l = 0; l < ConD[k].lhs.size(); l++)
						lur.push_back(ConD[k].lhs[l]);
					// Add string
					for (unsigned int l = 0; l < K[j].size(); l++)
						lur.push_back(K[j][l]);
					// Add right side of context
					for (unsigned int l = 0; l < ConD[k].rhs.size(); l++)
						lur.push_back(ConD[k].rhs[l]);
					// Test if lur is in language

					if ((G->oracle->checkHistory(lur) == -1 && G->accepts(lur))
						|| G->oracle->checkHistory(lur) == 1)
						return true;
				}
		}
	}
	return false;
}

// Main Algorithm function
CBFG IIL(CFG* target){
	clock_t t0 = clock();
	vector<vector<string>> K;
	vector<vector<string>> D;
	vector<context> F;
	vector<context> ConD;
	vector<vector<string>> SubD;

	CBFG Ghat = g(K, F, target);

	for (unsigned int i = 0; i < target->samples.size(); i++){
		vector<string> w = target->samples[i];
		printProcessing(w);
		cout << (float)(clock() - t0) / CLOCKS_PER_SEC << " seconds" << endl;
		D.push_back(w);
		// printD(D);
		addContexts(ConD, w);
		// printContextVector(ConD);
		addNEsubstrings(SubD, w);
		// printSubstringVector(SubD);
		if (notDinLG(D, Ghat)){
			K = SubD;
			F = ConD;
		}
		else if (reallyLongCond(SubD, K, ConD, F, target))
			F = ConD;

		Ghat = g(K, F, target);
		// Ghat.print();
	}

	cout << endl << (float)(clock() - t0) / CLOCKS_PER_SEC << " seconds" << endl << endl;

	return Ghat;
}

int main(int argc, char* argv[]){
	CFG* target = extract(argv[1]);
	target->print();
	target->checkSamples();
	CBFG Ghat = IIL(target);
	Ghat.print();
	cout << endl;
	Ghat.checkSamples(target->samples);
	cout << endl << target->queries << " queries to oracle" << endl;
}