/****************************************************************
* File: types.cpp
* Implementation for types.h
* David Peatman - Updated 9/23/14
****************************************************************/
#include "types.h"

#include <iostream>

////////////////////////////////////////////////////////////////
/* Equality helper funcions                                   */
////////////////////////////////////////////////////////////////

// Check if two string vectors are equal
bool equal(vector<string> a, vector<string> b){
	if (a.size() != b.size())
		return false;
	for (unsigned int i = 0; i < a.size(); i++)
		if (a[i] != b[i])
			return false;
	return true;
}

// Check if two context vectors are equal
bool equal(vector<context> a, vector<context> b){
	if (a.size() != b.size())
		return false;
	for (unsigned int i = 0; i < a.size(); i++){
		if (a[i].lhs.size() != b[i].lhs.size()
			|| a[i].rhs.size() != b[i].rhs.size())
			return false;
		for (unsigned int j = 0; j < a[i].lhs.size(); j++)
			if (a[i].lhs[j] != b[i].lhs[j])
				return false;
		for (unsigned int j = 0; j < a[i].rhs.size(); j++)
			if (a[i].rhs[j] != b[i].rhs[j])
				return false;
	}
	return true;
}

/////////////////////////////////////
/* Vector search utility functions */
/////////////////////////////////////

// Search for a string w in vector v
bool search(vector<string> v, string w){
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i] == w)
			return true;
	return false;
}

// Search for a context c in vector v
bool search(vector<context> v, context c){
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].lhs == c.lhs && v[i].rhs == c.rhs)
			return true;
	return false;
}

// Search for a context in a 2d vector of contexts
bool search(vector<vector<context>> v2, context c){
	for (unsigned int i = 0; i < v2.size(); i++)
		if (search(v2[i], c))
			return true;
	return false;
}

// Search for a subset string in a list of subsets
bool search(vector<vector<string>> v, vector<string> s){
	for (unsigned int i = 0; i < v.size(); i++)
		if (equal(v[i], s))
			return true;
	return false;
}

//////////////////////////////
/* Subset utility functions */
//////////////////////////////

// Check if a set of contexts c1 is a subset of c2
bool subset(vector<context> c1, vector<context> c2){
	for (unsigned int i = 0; i < c1.size(); i++)
		if (!search(c2, c1[i]))
			return false;
	return true;
}

// Check if a set of contexts c1 is contained within cl
bool subset(vector<context> c1, vector<vector<context>> cl){
	bool contained = false;
	for (unsigned int i = 0; i < c1.size(); i++){
		contained = false;
		for (unsigned int j = 0; j < cl.size(); j++){
			contained = search(cl[j], c1[i]);
			if (contained) break;
		}
		if (!contained) break;
	}
	return contained;
}

////////////////////////////////////////////////////////////////
/* Utility and Print Functions                                */
////////////////////////////////////////////////////////////////

// Takes the input file and creates an CFG object for the target grammar
CFG extractCFG(char* file){
	if (file == NULL){
		cout << "No input file given!" << endl;
		exit(1);
	}

	ifstream input(file);
	string line;

	CFG G;

	if (input.is_open())
	{
		string type;

		while (getline(input, line)){
			// Check for input commands (#)
			if (line[0] == '#') {
				string command;
				for (unsigned int i = 1; i < line.length(); i++){
					if (isalnum(line[i]))	// Ignore whitespace
						command += line[i];
				}
				// cout << command << endl;
				type = command;
				continue;
			}

			// Parse the line depending on what type it is
			if (type == "P0"){
				P0 p0;
				string temp;
				for (unsigned int i = 0; i < line.length(); i++)
					if (isalpha(line[i]))
						temp += line[i];
				p0.lhs = temp;
				G.vp0.push_back(p0);
			}
			else if (type == "P1"){
				P1 p1;
				string temp;
				for (unsigned int i = 0; i < line.length(); i++){
					if (isalpha(line[i]))
						temp += line[i];
					else if (line[i] == '-' && line[i + 1] == '>'){
						p1.lhs = temp;
						temp = "";
						i++;
					}
					p1.rhs = temp;
					G.vp1.push_back(p1);
				}
			}
			else if (type == "P2"){
				P2 p2;
				string temp;
				for (unsigned int i = 0; i < line.length(); i++){
					if (line[i] == ' ' || line[i] == '\t')
						continue;
					else if (line[i] == '-' && line[i + 1] == '>'){
						p2.lhs = temp;
						temp = "";
						i++;
						continue;
					}
					else if (line[i] == ','){
						p2.rhs1 = temp;
						temp = "";
						continue;
					}
					else
						temp.push_back(line[i]);
				}
				p2.rhs2 = temp;
				G.vp2.push_back(p2);
			}
			else if (type == "PL"){
				PL pl;
				string temp;
				for (unsigned int i = 0; i < line.length(); i++){
					if (line[i] == ' ' || line[i] == '\t')
						continue;
					else if (line[i] == '-' && line[i + 1] == '>'){
						pl.lhs = temp;
						temp = "";
						i++;
						continue;
					}
					else
						temp.push_back(line[i]);
				}
				pl.rhs = temp;
				G.vpl.push_back(pl);
			}
			else if (type == "Starts"){
				string temp;
				for (unsigned int i = 0; i < line.length(); i++)
					if (isalpha(line[i]))
						temp += line[i];
				G.starts.emplace(temp);
			}
			else if (type == "Samples"){
				string temp;
				vector<string> w;
				bool inword = false;
				for (unsigned int i = 0; i < line.length(); i++){
					if (line[i] == ' ' || line[i] == '\t'){
						if (inword){
							w.push_back(temp);
							temp = "";
							inword = false;
						}
						else
							continue;
					}
					else{
						inword = true;
						temp += line[i];
					}
				}
				if (inword)
					w.push_back(temp);
				G.samples.push_back(w);
			}
			else // The input file is improperly formatted
			{
				cout << "Input file improperly formatted!" << endl;
			}
		}

		input.close();

		
		return G;
	}
	else {
		cout << "Unable to open file" << endl;
		exit(1);
		return G;
	}
}

// prints the current runtime
void runtime(clock_t t0){
	cout << "Current time: " << ((float)(clock() - t0) / CLOCKS_PER_SEC) << " seconds" << endl;
}

// Converts a CFG with contextual rules into a CFG with short strings
CFG convertCFGC(const CFGC &H){
	CFG Hprime;
	unordered_map<contextSet, string> cmap;
	Hprime.starts.emplace("0");
	cmap[contextSet()] = "0";
	unsigned elements = 1;

	for (auto p0c : H.sp0c.set){ // For each P0C rule
		auto csp = cmap.find(p0c.lhs); // csp = pointer to contextSet
		if (csp == cmap.end()) // lhs has not been hashed yet
			cmap.emplace(p0c.lhs, to_string(elements++)); // add it to cmap and increment elements
		Hprime.vp0.push_back(P0(cmap[p0c.lhs])); // make a new p0 rule and add it to Hprime.vp0
		// If lhs of rule contains empty context, add sentence rule
		for (auto c : p0c.lhs.set){  // For each context in p0c.lhs
			if (c.lhs.size() == 0 && c.rhs.size() == 0)
				Hprime.vp0.push_back(P0("0"));
		}
	}
	for (auto p1c : H.sp1c.set){
		
		auto lhscsp = cmap.find(p1c.lhs);
		if (lhscsp == cmap.end())
			cmap.emplace(p1c.lhs, to_string(elements++));
		auto rhscsp = cmap.find(p1c.rhs);
		if (rhscsp == cmap.end())
			cmap.emplace(p1c.rhs, to_string(elements++));
		Hprime.vp1.push_back(P1(cmap[p1c.lhs], cmap[p1c.rhs])); // make a new P1 rule and add it to Hprime.vp1
		// If lhs of rule contains empty context, add sentence rule
		for (auto c : p1c.lhs.set){  // For each context in p0c.lhs
			if (c.lhs.size() == 0 && c.rhs.size() == 0)
				Hprime.vp1.push_back(P1("0", cmap[p1c.rhs]));
		}
	}
	for (auto p2c : H.sp2c.set){
		auto lhscsp = cmap.find(p2c.lhs);
		if (lhscsp == cmap.end())
			cmap.emplace(p2c.lhs, to_string(elements++));
		auto rhs1csp = cmap.find(p2c.rhs1);
		if (rhs1csp == cmap.end())
			cmap.emplace(p2c.rhs1, to_string(elements++));
		auto rhs2csp = cmap.find(p2c.rhs2);
		if (rhs2csp == cmap.end())
			cmap.emplace(p2c.rhs2, to_string(elements++));
		Hprime.vp2.push_back(P2(cmap[p2c.lhs], cmap[p2c.rhs1], cmap[p2c.rhs2]));
		// If lhs of rule contains empty context, add sentence rule
		for (auto c : p2c.lhs.set){  // For each context in p0c.lhs
			if (c.lhs.size() == 0 && c.rhs.size() == 0)
				Hprime.vp2.push_back(P2("0", cmap[p2c.rhs1], cmap[p2c.rhs2]));
		}
	}
	for (auto plc : H.splc.set){
		auto lhscsp = cmap.find(plc.lhs);
		if (lhscsp == cmap.end())
			cmap.emplace(plc.lhs, to_string(elements++));
		Hprime.vpl.push_back(PL(cmap[plc.lhs], plc.rhs));
		// If lhs of rule contains empty context, add sentence rule
		for (auto c : plc.lhs.set){  // For each context in p0c.lhs
			if (c.lhs.size() == 0 && c.rhs.size() == 0)
				Hprime.vpl.push_back(PL("0", plc.rhs));
		}
	}
	return Hprime;
}

////////////////////////////////////////////////////////////////
/* Printing functions                                         */
////////////////////////////////////////////////////////////////

// Print a context
void printContext(const context &c){
	cout << "(";
	for (unsigned int i = 0; i < c.lhs.size(); i++){
		cout << c.lhs[i];
		if (i < c.lhs.size() - 1)
			cout << " ";
	}
	cout << ", ";
	for (unsigned int i = 0; i < c.rhs.size(); i++){
		cout << c.rhs[i];
		if (i < c.rhs.size() - 1)
			cout << " ";
	}
	cout << ")";
}

// Print a set of contexts
void printContextSet(const unordered_set<context> &S){
	cout << "Contexts: ";
	for (auto c : S){
		printContext(c);
		cout << "  ";
	}
	cout << endl;
}

// Print a P0C rule
void printP0C(const P0C &p0c){
	cout << "  P0C: ";
	for (auto c : p0c.lhs.set){
		printContext(c);
		cout << " ";
	}
	cout <<  "->  ()" << endl;
}

// Print a P0CSet
void printP0CSet(const P0CSet &sp0c){
	cout << "P0C rules:" << endl;
	for (auto p0c : sp0c.set)
		printP0C(p0c);
}

// Print a P1C rule
void printP1C(const P1C &p1c){
	cout << "  P1C: ";
	for (auto c : p1c.lhs.set){
		printContext(c);
		cout << " ";
	}
	cout << " ->  ";
	for (auto c : p1c.rhs.set){
		printContext(c);
		cout << " ";
	}
	cout << endl;
}

// Print a P1CSet
void printP1CSet(const P1CSet &sp1c){
	cout << "P1C rules:" << endl;
	for (auto p1c : sp1c.set)
		printP1C(p1c);
}

// Print a P2C rule
void printP2C(const P2C &p2c){
	cout << "  P2C: ";
	for (auto c : p2c.lhs.set){
		printContext(c);
		cout << " ";
	}
	cout << " ->  ";
	for (auto c : p2c.rhs1.set){
		printContext(c);
		cout << " ";
	}
	cout << " + ";
	for (auto c : p2c.rhs2.set){
		printContext(c);
		cout << " ";
	}
	cout << endl;
}

// Print a P2CSet
void printP2CSet(const P2CSet &sp2c){
	cout << "P2C rules:" << endl;
	for (auto p2c : sp2c.set)
		printP2C(p2c);
}

// Print a PLC rule
void printPLC(const PLC &plc){
	cout << "  PLC: ";
	for (auto c : plc.lhs.set){
		printContext(c);
		cout << " ";
	}
	cout << " ->  " << plc.rhs << endl;
}

// Print a PLCSet
void printPLCSet(const PLCSet &splc){
	cout << "PLC rules:" << endl;
	for (auto plc : splc.set)
		printPLC(plc);
}

// Print at the beginning of each new sample
void printProcessing(const vector<string> &w){
	cout << endl << "Processing input: ";
	for (unsigned int i = 0; i < w.size(); i++)
		cout << w[i] + " ";
	cout << endl;
}

// Print the contents of D
void printD(const vector<vector<string>> &D){
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

// Print a vector of substrings
void printSubstringVector(const vector<vector<string>> &s){
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

// Print a CFG
void printCFG(const CFG &G){
	cout << "Target Grammar:" << endl;
	cout << "  Start symbols:" << endl;
	for (auto S : G.starts){
		cout << "    " << S << endl;
	}
	cout << "  P0 Rules:" << endl;
	for (unsigned int i = 0; i < G.vp0.size(); i++){
		cout << "    " << G.vp0[i].lhs << " -> e" << endl;
	}
	cout << "  P1 Rules:" << endl;
	for (unsigned int i = 0; i < G.vp1.size(); i++){
		cout << "    " << G.vp1[i].lhs << " -> " << G.vp1[i].rhs << endl;
	}
	cout << "  P2 Rules:" << endl;
	for (unsigned int i = 0; i < G.vp2.size(); i++){
		cout << "    " << G.vp2[i].lhs << " ->";
		cout << " " << G.vp2[i].rhs1 << "," << G.vp2[i].rhs2 << endl;
	}
	cout << "  PL Rules:" << endl;
	for (unsigned int i = 0; i < G.vpl.size(); i++)
		cout << "    " << G.vpl[i].lhs << " -> " << G.vpl[i].rhs << endl;
	cout << "  Samples:" << endl;
	for (auto S : G.samples){
		cout << "    ";
		for (unsigned int j = 0; j < S.size(); j++)
			cout << S[j] << " ";
		cout << endl;
	}
	cout << endl;
}

// Print a CFGC
void printCFGC(const CFGC &H){
	printP0CSet(H.sp0c);
	printP1CSet(H.sp1c);
	printP2CSet(H.sp2c);
	printPLCSet(H.splc);
}

// Print # of rules in CGFC
void printCFGCRules(const CFGC &H){
	cout << setw(4) << H.sp0c.set.size() << " P0C Rules" << endl;
	cout << setw(4) << H.sp1c.set.size() << " P1C Rules" << endl;
	cout << setw(4) << H.sp2c.set.size() << " P2C Rules" << endl;
	cout << setw(4) << H.splc.set.size() << " PLC Rules" << endl;
}