#include "grammars.h"

//////////////////////////////
/* CFG Class emplimentation */
//////////////////////////////

CFG::CFG(string s, CFGRules r, vector<vector<string>> sam)
	:start(s), rules(r), samples(sam), queries(0), oracle(new CFGOracle()) {}

// Prints the target CFG grammar in a readable format
void CFG::print(){
	cout << "Target Grammar:" << endl;
	cout << "  Start symbol: " << start << endl;
	cout << "  Lexical Rules:" << endl;
	for (unsigned int i = 0; i < rules.PL.size(); i++)
		cout << "    " << rules.PL[i].left << " -> " << rules.PL[i].right << endl;
	cout << "  Nonlexical Rules:" << endl;
	for (unsigned int i = 0; i < rules.P.size(); i++){
		cout << "    " << rules.P[i].left << " ->";
		cout << " " << rules.P[i].one << "," << rules.P[i].two << endl;
	}
	cout << "  Samples:" << endl;
	for (unsigned int i = 0; i < samples.size(); i++){
		cout << "    ";
		for (unsigned int j = 0; j < samples[i].size(); j++)
			cout << samples[i][j] << " ";
		cout << endl;
	}
	cout << endl;
		
}

// Checks the inupt samples to make sure they are all accepted by target grammar
void CFG::checkSamples(){
	for (unsigned int i = 0; i < samples.size(); i++){
		bool accepted = accepts(samples[i]);
		if (accepted){
			cout << "Accepted by target grammar:";
			for (unsigned int j = 0; j < samples[i].size(); j++)
				cout << " " << samples[i][j];
			cout << endl;
		}
		else{
			cout << "Rejected by target grammar:";
			for (unsigned int j = 0; j < samples[i].size(); j++)
				cout << " " << samples[i][j];
			cout << endl;
			exit(1);
		}
	}
}

bool CFG::accepts(vector<string> w){
	queries++;
	return oracle->accepts(w, rules.PL, rules.P, start);
}

///////////////////////////////
/* CBFG Class Implimentation */
///////////////////////////////

CBFG::CBFG(CBFGRules r)
	:rules(r), oracle(new CBFGOracle(this)) {}

// Auxiliary function to make function calls in main look nicer
bool CBFG::accepts(vector<string> w){
	return oracle->accepts(w, rules.PL, rules.P);
}

// Print out a PLC rule in a readable format
void printPLCRule(PLCRule PL){
	for (unsigned int i = 0; i < PL.c.size(); i++){
		cout << "(";
		for (unsigned int j = 0; j < PL.c[i].lhs.size(); j++){
			cout << PL.c[i].lhs[j];
			if (j < PL.c[i].lhs.size() - 1)
				cout << " ";
		}
		cout << ",";
		for (unsigned int j = 0; j < PL.c[i].rhs.size(); j++){
			cout << PL.c[i].rhs[j];
			if (j < PL.c[i].rhs.size() - 1)
				cout << " ";
		}
		cout << ")";
		if (i < PL.c.size() - 1)
			cout << ",";
	}
	cout << "  ->  " << PL.s << endl;
}

// Print out a PC rule in a readable format
void printPCRule(PCRule P){
	for (unsigned int i = 0; i < P.lhs.size(); i++){
		cout << "(";
		for (unsigned int j = 0; j < P.lhs[i].lhs.size(); j++){
			cout << P.lhs[i].lhs[j];
			if (j < P.lhs[i].lhs.size() - 1)
				cout << " ";
		}
		cout << ",";
		for (unsigned int j = 0; j < P.lhs[i].rhs.size(); j++){
			cout << P.lhs[i].rhs[j];
			if (j < P.lhs[i].rhs.size() - 1)
				cout << " ";
		}
		cout << ")";
		if (i < P.lhs.size() - 1)
			cout << ",";
	}
	cout << "  ->  ";
	for (unsigned int i = 0; i < P.rhs1.size(); i++){
		cout << "(";
		for (unsigned int j = 0; j < P.rhs1[i].lhs.size(); j++){
			cout << P.rhs1[i].lhs[j];
			if (j < P.rhs1[i].lhs.size() - 1)
				cout << " ";
		}
		cout << ",";
		for (unsigned int j = 0; j < P.rhs1[i].rhs.size(); j++){
			cout << P.rhs1[i].rhs[j];
			if (j < P.rhs1[i].rhs.size() - 1)
				cout << " ";
		}
		cout << ")";
		if (i < P.rhs1.size() - 1)
			cout << ",";
	}
	cout << "     ";
	for (unsigned int i = 0; i < P.rhs2.size(); i++){
		cout << "(";
		for (unsigned int j = 0; j < P.rhs2[i].lhs.size(); j++){
			cout << P.rhs2[i].lhs[j];
			if (j < P.rhs2[i].lhs.size() - 1)
				cout << " ";
		}
		cout << ",";
		for (unsigned int j = 0; j < P.rhs2[i].rhs.size(); j++){
			cout << P.rhs2[i].rhs[j];
			if (j < P.rhs2[i].rhs.size() - 1)
				cout << " ";
		}
		cout << ")";
		if (i < P.rhs2.size() - 1)
			cout << ",";
	}
	cout << endl;
}

// Print out a whole CBFG in a readable format
void CBFG::print(){
	cout << "CBFG Grammar:" << endl;
	cout << "  Lexical rules:" << endl;
	for (unsigned int i = 0; i < rules.PL.size(); i++){
		cout << "    ";
		printPLCRule(rules.PL[i]);
	}
	cout << "  Nonlexical rules:" << endl;
	for (unsigned int i = 0; i < rules.P.size(); i++){
		cout << "    ";
		printPCRule(rules.P[i]);
	}
	cout << endl << rules.P.size() + rules.PL.size() << " rules in grammar" << endl;
}

// Check all of the input samples to make sure they are accepted by grammar
void CBFG::checkSamples(vector<vector<string>> s){
	for (unsigned int i = 0; i < s.size(); i++){
		bool accepted = accepts(s[i]);
		if (accepted){
			cout << "Accepted by leaner's grammar:";
			for (unsigned int j = 0; j < s[i].size(); j++)
				cout << " " << s[i][j];
			cout << endl;
		}
		else{
			cout << "Rejected by learner's grammar:";
			for (unsigned int j = 0; j < s[i].size(); j++)
				cout << " " << s[i][j];
			cout << endl;
		}
	}
}