#include "regex.hpp"

// 构造函数
RegEx::RegEx(const std::string& rule) {
	rule_esc_ = nullptr;
	nfa_ = nullptr;
	marked_ = nullptr;
	// 设置规则
	if (IsLegal(rule)) {
		SetRule(rule);
	}
	else {
		SetRule();
		throw false;
	}
}

// 析构函数
RegEx::~RegEx() {
	if (nfa_) {
		delete[]rule_esc_;
		delete[]nfa_;
		delete[]marked_;
	}
}

// 设置规则
bool RegEx::SetRule(const std::string& rule) {
	if (IsLegal(rule)) {
		// 保存规则信息
		rule_ = rule;
		rule_len_ = rule_.size();
		// 预处理
		if (nfa_) {
			delete[]rule_esc_;
			rule_esc_ = nullptr;
			delete[]nfa_;
			nfa_ = nullptr;
			delete[]marked_;
			marked_ = nullptr;
		}
		if (rule_len_) {
			rule_esc_ = new bool[rule_len_] {false};
			nfa_ = new std::vector<int>[rule_len_ + 1];
			marked_ = new bool[rule_len_ + 1];
			// 构建NFA
			BuildNfa();
		}
		return true;
	}
	else {
		return false;
	}
}

// 获得规则
std::string RegEx::GetRule()const {
	return rule_;
}

// 匹配
bool RegEx::Match(const std::string& text) {
	// 特殊情况的快速判断
	if (rule_ == "") {
		if (text == "")
			return true;
		else
			return false;
	}
	//闭包
	std::set<int> closure;
	//初始状态集
	std::fill(marked_, marked_ + rule_len_ + 1, false);
	int rule_begin = rule_[0] == '\\' ? 1 : 0;
	Dfs(rule_begin);
	for (int v = 0; v <= rule_len_; v++) {
		if (marked_[v]) {
			closure.insert(v);
		}
	}
	//状态转移
	char temp_str;
	for (int i = 0; i < text.size(); ++i) {
		//更新下一步状态集
		std::set<int> match;
		for (int v : closure) {
			if (v < rule_len_) {
				if (rule_esc_[v]) {
					switch (rule_[v])
					{
					case 'n':
						temp_str = '\n';
						break;
					case 't':
						temp_str = '\t';
						break;
					default:
						temp_str = rule_[v];
						break;
					}
					if (temp_str == text[i]) {
						rule_[v + 1] == '\\' ? match.insert(v + 2)
							: match.insert(v + 1);
					}
				}
				else if (rule_[v] == text[i] || rule_[v] == '.') {
					rule_[v + 1] == '\\' ? match.insert(v + 2)
						: match.insert(v + 1);
				}
			}
		}
		closure.clear();
		std::fill(marked_, marked_ + rule_len_ + 1, false);
		for (int v : match) {
			if (!marked_[v]) {
				Dfs(v);
			}
		}
		for (int v = 0; v <= rule_len_; ++v) {
			if (marked_[v]) {
				closure.insert(v);
			}
		}
		if (closure.empty())
			return false;
	}

	for (int v : closure) {
		if (v == rule_len_) {
			return true;
		}
	}
	return false;
}

// 转义字符判断
bool RegEx::IsEscCh(const char& ch)const {
	return ch == 'n' || ch == 't' || ch == '(' || ch == ')' || ch == '|' || ch == '*' || ch == '.' || ch == '\\';
}

// 输入合法性判断
bool RegEx::IsLegal(const std::string& rule)const {
	int len = rule.length();
	int lp_num = 0;
	for (int i = 0; i < len; i++) {
		if (rule[i] == '(') {
			lp_num++;
		}
		else if (rule[i] == ')') {
			if (--lp_num < 0)
				return false;
		}
		else if (rule[i] == '\\') {
			i++;
			if (i == len || !IsEscCh(rule[i])) {
				return false;
			}
		}
	}
	if (lp_num == 0)
		return true;
	else
		return false;
}

// 以递归方式深度优先遍历
void RegEx::Dfs(int k) {
	marked_[k] = true;
	for (int i : nfa_[k])
		if (!marked_[i])
			Dfs(i);
}

// 建立NFA
void RegEx::BuildNfa() {
	// 运算符的位置：主要是"(","|"
	std::stack<int> op_stack;
	// 或的位置
	std::vector<int> or_vector;

	for (int i = 0; i < rule_len_; i++) {
		int lp;								// i或左括号的位置
		// 转义字符，只需可能增加 "*" 闭环
		if (rule_[i] == '\\') {
			rule_esc_[++i] = true;
			lp = i;
		}
		// 非转义字符，可能是符号
		else {
			lp = i;
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
					if (rule_[k + 1] == '\\') {
						nfa_[lp].push_back(k + 2);
					}
					else {
						nfa_[lp].push_back(k + 1);
					}
				}
			}
			// "(", ")", "*" 等层
			if (rule_[i] == '(' || rule_[i] == ')' || rule_[i] == '*') {
				if (rule_[i + 1] == '\\') {
					nfa_[i].push_back(i + 2);
				}
				else {
					nfa_[i].push_back(i + 1);
				}
			}
		}
		// "*" 闭环
		if (i < rule_len_ - 1 && rule_[i + 1] == '*') {
			nfa_[lp].push_back(i + 1);
			nfa_[i + 1].push_back(lp);
		}
	}
	// 清空op栈
	if (!op_stack.empty()) {
		int begin = 0;
		if (rule_[0] == '\\') {
			begin = 1;
		}
		int i;
		do {
			i = op_stack.top();
			op_stack.pop();
			nfa_[i].push_back(rule_len_);
			if (rule_[i + 1] == '\\') {
				nfa_[begin].push_back(i + 2);
			}
			else {
				nfa_[begin].push_back(i + 1);
			}
		} while (!op_stack.empty());
	}

	return;
}
