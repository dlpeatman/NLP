/****************************************************************
 * File: yoshinakadual.cpp
 * Implementation of the dual method of Yoshinaka'11, Algorithm 2
 * David Peatman - Updated 9/04/14
 ****************************************************************
 * Compile this file and run with properly formatted CFG file as
 * command line argument
 ****************************************************************/
#include <cctype>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "cyke.h"
#include "types.h"

using namespace::std;

////////////////////////////////////////////////////////////////
/* Yoshinaka algorithm                                        */
////////////////////////////////////////////////////////////////

// Just like python version
void addSub(vector<vector<string>> &SubD, const vector<string> w){
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j = i; j <= w.size(); j++){
			vector<string> temp;
			for (unsigned int k = i; k < j; k++) // Python: s=w[i:j]
				temp.push_back(w[k]);

			if (!search(SubD, temp)) // If temp is not already in ConD, add it
				SubD.push_back(temp);
		}
	}
}

// Just like python vesion
void addCon(contextSet &ConD, const vector<string> w){
	for (unsigned int i = 0; i <= w.size(); i++){
		for (unsigned int j = i; j <= w.size() + 1; j++){
			context c;
			for (unsigned int k = 0; k < i; k++)
				c.lhs.push_back(w[k]);
			for (unsigned int k = j; k < w.size(); k++)
				c.rhs.push_back(w[k]);
			ConD.set.emplace(c);
		}
	}
}

// CK - just like python
vector<vector<string>> CK(const contextSet C, const vector<vector<string>> K, const CFG &G){
	vector<vector<string>> ck;
	for (auto w : K){
		bool b = true;
		for (auto c : C.set){
			vector<string> lur;
			for (auto s : c.lhs)
				lur.push_back(s);
			for (auto s : w)
				lur.push_back(s);
			for (auto s : c.rhs)
				lur.push_back(s);
			if (!accepts(lur, G, G.chains, historyG)){
				b = false;
				break;
			}
		}
		if (b){
			ck.push_back(w);
		}
	}
	return ck;
}

void newP0C(const contextSet C, P0CSet &sp0c, const CFG &G){
	for (auto c : C.set){
		vector<string> lur;
		for (auto s : c.lhs)
			lur.push_back(s);
		for (auto s : c.rhs)
			lur.push_back(s);
		if (!accepts(lur, G, G.chains, historyG))
			return;
	}
	P0C p0c(C);
	sp0c.set.emplace(p0c);
}

void newP2C(const contextSet C, const unordered_set<contextSet> Vf,
	const vector<vector<string>> K, P2CSet &sp2c, const CFG &G){
	for (auto C1 : Vf){
		for (auto C2 : Vf){
			vector<vector<string>> ck1 = CK(C1, K, G);
			vector<vector<string>> ck2 = CK(C2, K, G);
			bool b = true;
			for (auto c : C.set){
				for (auto s1 : ck1){
					for (auto s2 : ck2){
						vector<string> lur;
						for (auto s : c.lhs)
							lur.push_back(s);
						for (auto s : s1)
							lur.push_back(s);
						for (auto s : s2)
							lur.push_back(s);
						for (auto s : c.rhs)
							lur.push_back(s);
						if (!accepts(lur, G, G.chains, historyG)){
							b = false;
							break;
						}
					}
					if (!b)
						break;
				}
				if (!b)
					break;
			}
			if (b){
				P2C p2c(C, C1, C2);
				sp2c.set.emplace(p2c);
			}
		}
	}
}

void newPLC(const contextSet C, PLCSet &splc, const CFG &G, unordered_set<string> sigma){
	for (auto x : sigma){
		bool b = true;
		for (auto c : C.set){
			vector<string> lur;
			for (auto s : c.lhs)
				lur.push_back(s);
			lur.push_back(x);
			for (auto s : c.rhs)
				lur.push_back(s);
			if (!accepts(lur, G, G.chains, historyG)){
				b = false;
				break;
			}
		}
		if (b){
			PLC plc(C, x);
			splc.set.emplace(plc);
		}
	}
}

void newP1C(const P0CSet &sp0c, P1CSet &sp1c, const P2CSet &sp2c,
	const PLCSet &splc, const CFG &G)
{
	context c = context();
	contextSet cs;
	cs.set.emplace(c);
	for (auto r : sp0c.set){ // For each P0C rule
		if (r.lhs.set.find(c) != r.lhs.set.end()){
			P1C p1c(cs, r.lhs);
			sp1c.set.emplace(p1c);
		}
	}
	for (auto r : sp2c.set){ // For each P2C rule
		if (r.lhs.set.find(c) != r.lhs.set.end()){
			P1C p1c(cs, r.lhs);
			sp1c.set.emplace(p1c);
		}
	}
	for (auto r : splc.set){ // For each PLC rule
		if (r.lhs.set.find(c) != r.lhs.set.end()){
			P1C p1c(cs, r.lhs);
			sp1c.set.emplace(p1c);
		}
	}
}

// Return the powerset of a set of contexts F
unordered_set<contextSet> powerSet(contextSet F, int f){
	unordered_set<contextSet> a;
	if (f > 0) // Recursive case
	{
		auto b = powerSet(F, f - 1);
		for (auto cset : b){
			for (auto c : F.set){
				cset.set.emplace(c);
				a.emplace(cset);
			}
		}
		for (auto c : F.set){
			contextSet cset;
			cset.set.emplace(c);
			a.emplace(cset);
		}
	}
	return a; // Base case or recursive step finished
}

// Create a Conditional CFG Grammar from F and K
CFGC Hf(const contextSet &F, const vector<vector<string>> &K,
	const CFG &G, const unordered_set<string> &sigma, const int f)
{
	printContextSet(F.set);
	// printD(K);
	CFGC H;
	unordered_set<contextSet> Vf = powerSet(F,f);
	//for (auto cs : Vf)
	//	printContextSet(cs.set);
	for (auto Cset : Vf){ // Cset is a set of contexts
		if (Cset.set.size() > 0){
			newP0C(Cset, H.sp0c, G);
			newP2C(Cset, Vf, K, H.sp2c, G);
			newPLC(Cset, H.splc, G, sigma);
		}
	}
	newP1C(H.sp0c, H.sp1c, H.sp2c, H.splc, G);
	// printCFGC(H);
	return H;
}


bool notInLhat(vector<vector<string>> D, CFG &Hprime){
	//if (Hprime.vp0.size() + Hprime.vp1.size() + Hprime.vp2.size()
	//	+ Hprime.vpl.size() == 0) // If Hprime is empty, just return true
	//	return true;
	for (auto s : D){
		// printCFG(Hprime);
		if (!accepts(s, Hprime, historyH)){
			cout << "Not in Lhat" << endl;
			return true;
		}
	}
	cout << "In Lhat" << endl;
	return false;
}

// Main Algorithm function
CFGC fFCP(const CFG &target, const int f){
	clock_t t0 = clock();

	vector<vector<string>> D;
	vector<vector<string>> SubD;
	vector<vector<string>> K;
	contextSet F;
	contextSet ConD;
	unordered_set<string> sigma;

	for (auto pl : target.vpl)
		sigma.emplace(pl.rhs);

	CFGC Hhat = Hf(F, K, target, sigma, f);
	CFG Hprime;

	for (unsigned int i = 0; i < target.samples.size(); i++){
		vector<string> w = target.samples[i];
		printProcessing(w);
		runtime(t0);
		D.push_back(w);
		// printD(D);
		addCon(ConD, w);
		// printContextSet(ConD.set);
		addSub(SubD, w);
		// printSubstringVector(SubD);
		K = SubD;
		Hhat = Hf(F, K, target, sigma, f);
		Hprime = convertCFGC(Hhat);
		if (notInLhat(D, Hprime)){
			for (auto c : ConD.set)
				F.set.emplace(c);
			Hhat = Hf(F, K, target, sigma, f);
			Hprime = convertCFGC(Hhat);
		}
		// printCFGC(Hhat);
		// printCFG(Hprime);
	}
	cout << "Done. Checking learner grammar..." << endl;
	runtime(t0);
	checkLearner(Hprime, target.samples);
	runtime(t0);

	return Hhat;
}

int main(int argc, char* argv[]){
	CFG target = extractCFG(argv[1]);
	printCFG(target);
	checkSamples(target);
	CFGC Hhat = fFCP(target, 1);
	cout << endl << "Learner's grammar:" << endl;
	// printCFGC(Hhat);
	printCFGCRules(Hhat);
}