#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <string>
using namespace std;

class RegEx {
private:
	// 规则
	string rule_;
	// 规则长度
	int len_;
	// NFA网络
	vector<int>* nfa_;
	// 节点标记
	bool* marked_;
public:
	RegEx(const string& rule = "");
	~RegEx();

	// 设置规则
	void SetRule(const string& rule = "");
	// 获得规则
	string GetRule()const;

	// 匹配
	bool Match(const string& text);
private:
	// 深度优先遍历
	void Dfs(int s);
	// 建立NFA
	void BuildNfa();
};

RegEx::RegEx(const string& rule) {
	nfa_ = nullptr;
	marked_ = nullptr;
	SetRule(rule);
}

RegEx::~RegEx() {
	if (nfa_) {
		delete[]nfa_;
		delete[]marked_;
	}
}

void RegEx::SetRule(const string& rule) {
	rule_ = rule;
	len_ = rule_.size();
	if (nfa_) {
		delete[]nfa_;
		delete[]marked_;
	}
	nfa_ = new vector<int>[len_ + 1];
	marked_ = new bool[len_ + 1];
	BuildNfa();
}

string RegEx::GetRule()const {
	return rule_;
}

bool RegEx::Match(const string& text) {
	if (rule_ == "") {
		if (text == "")
			return true;
		else
			return false;
	}
	set<int> closure;
	//初始状态集
	fill(marked_, marked_ + len_ + 1, false);
	Dfs(0);
	for (int v = 0; v <= len_; v++) {
		if (marked_[v]) {
			closure.insert(v);
		}
	}
	//状态转移
	for (int i = 0; i < text.size(); ++i) {
		//更新下一步状态集
		set<int> match;
		for (int v : closure) {
			if (v < len_) {
				if (rule_[v] == text[i] || rule_[v] == '?') {
					match.insert(v + 1);
				}
			}
		}
		closure.clear();
		fill(marked_, marked_ + len_ + 1, false);
		for (int v : match) {
			if (!marked_[v]) {
				Dfs(v);
			}
		}
		for (int v = 0; v <= len_; ++v) {
			if (marked_[v]) {
				closure.insert(v);
			}
		}
	}

	for (int v : closure) {
		if (v == len_) {
			return true;
		}
	}
	return false;
}

void RegEx::Dfs(int k) {
	marked_[k] = true;
	for (int i : nfa_[k])
		if (!marked_[i])
			Dfs(i);
}

void RegEx::BuildNfa() {
	//左括号和或的位置
	stack<int> op_stack;
	//或的位置
	vector<int> or_vector;
	for (int i = 0; i < len_; i++) {
		int lp = i;						//i或左括号的位置
		if (rule_[i] == '(' || rule_[i] == '|') {
			op_stack.push(i);
		}
		else if (rule_[i] == ')') {
			// "|" -> ")"
			int k = op_stack.top();		// 或的位置
			op_stack.pop();
			while (rule_[k] == '|') {
				nfa_[k].push_back(i);
				or_vector.push_back(k);
				lp = op_stack.top();
				op_stack.pop();
				k = lp;
			}
			// "(" -> "|"+1
			while (!or_vector.empty()) {
				k = or_vector.back();
				or_vector.pop_back();
				nfa_[lp].push_back(k + 1);
			}
		}
		if (i < len_ - 1 && rule_[i + 1] == '*') {
			nfa_[lp].push_back(i + 1);
			nfa_[i + 1].push_back(lp);
		}
		if (rule_[i] == '(' || rule_[i] == '*' || rule_[i] == ')') {
			nfa_[i].push_back(i + 1);
		}
	}
	if (!op_stack.empty()) {
		int i;
		do {
			i = op_stack.top();
			op_stack.pop();
			nfa_[0].push_back(i + 1);
			nfa_[i].push_back(len_);
		} while (!op_stack.empty());
	}

	return;
}
int main() {
	RegEx e;
	string rule = "(|+|-)((0|1|2|3|4|5|6|7|8|9)*(.(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*)|(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*(|.))(|((e|E)(|+|-)(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*))";
	string str = "3.1e-1";
	e.SetRule(rule);
	cout << e.Match(str);
}