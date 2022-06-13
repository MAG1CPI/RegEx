#include <iostream>
#include "regex.hpp"

using namespace std;

int main() {
	RegEx e;
	string rule = R"+*((|+|-)((0|1|2|3|4|5|6|7|8|9)*(\.(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*)|(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*(|\.))(|((e|E)(|+|-)(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*)))+*";
	//string rule = R"+*(\.(1|2)*)+*";
	string str = "1.2e-3";
	e.SetRule(rule);
	cout << e.Match(str);
}