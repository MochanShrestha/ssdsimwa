#include "CBRules.h"

#include <iostream>
#include <sstream>
#include <string>
using namespace std;


CBRules::CBRules(string rules)
{
	parseRules (rules);
}


CBRules::~CBRules(void)
{
	delete[] cbrules;
}

void CBRules::parseRules(string rules)
{
	int nCommas = 0;

	// Find the size of the array we have to create by counting commas
	for (unsigned int i=0; i<rules.length(); i++)
	{
		if ( rules[i] == ',' )
		{
			nCommas++;
		}
	}

	// Now allocate the array of rules
	nRules = nCommas + 3;
	nCBblocks = nRules - 1;
	cbrules = new int[nRules];
	
	// Fill out the array
	cbrules[0] = 0;
	stringstream rulestream (rules);
	string num;
	for (int i=0; i<=nCommas; i++)
	{
		getline (rulestream, num, ',');
		stringstream nums (num);
		nums >> cbrules[i+1];
	}
	cbrules[nRules-1] = INT_MAX;
}

int CBRules::getCBblock (int c)
{
	// check the array and find out where it should go
	for (int i=0; i<nCBblocks; i++)
	{
		if ( c >= cbrules[i] && c < cbrules[i+1] )
		{
			return i;
		}
	}

	// Should never get here but to shut up the warnings
	return nCBblocks;
}

ostream &operator<< (ostream &stream, CBRules& r)
{
	for (int i=0; i<r.nCBblocks; i++)
	{
		stream << "[" << r.cbrules[i] << ", ";
		if ( r.cbrules[i+1] == INT_MAX)
		{
			cout << "oo";
		}
		else
		{
			cout << r.cbrules[i+1];
		}
		cout << ") | ";
	}
	cout << endl;
	return stream;
}