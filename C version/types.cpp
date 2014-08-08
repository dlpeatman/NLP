#include "types.h"

//////////////////////////////
/* Equality helper funcions */
//////////////////////////////

// Check if two contexts are equal
bool equal(context a, context b){
	if (a.lhs.size() != b.lhs.size()
		|| a.rhs.size() != b.rhs.size())
		return false;
	for (unsigned int i = 0; i < a.lhs.size(); i++)
		if (a.lhs[i] != b.lhs[i])
			return false;
	for (unsigned int i = 0; i < a.lhs.size(); i++)
		if (a.rhs[i] != b.rhs[i])
			return false;
	return true;
}

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

// Check if two PLCRules are equal
bool equal(PLCRule a, PLCRule b){
	return a.s == b.s && equal(a.c, b.c);
}

// Check if two PCRules are equal
bool equal(PCRule a, PCRule b){
	return equal(a.lhs, b.lhs) && equal(a.rhs1, b.rhs1) && equal(a.rhs2, b.rhs2);
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

// Search for a PLC rule in a list of PLC rules
bool search(vector<PLCRule> rules, PLCRule r){
	for (unsigned int i = 0; i < rules.size(); i++)
		if (equal(rules[i], r))
			return true;
	return false;
}

// Search for a PC rule in a list of PC Rules
bool search(vector<PCRule> rules, PCRule r){
	for (unsigned int i = 0; i < rules.size(); i++)
		if (equal(rules[i], r))
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