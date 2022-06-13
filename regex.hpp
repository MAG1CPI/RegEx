#pragma once
#include <string>
#include <vector>
#include <stack>
#include <set>

class RegEx {
private:
	// 规则
	std::string rule_;
	// 规则长度
	int rule_len_;
	// 转义符位置表
	bool* rule_esc_;
	// NFA网络
	std::vector<int>* nfa_;
	// 节点标记
	bool* marked_;
public:
	RegEx(const std::string& rule = "");
	~RegEx();

	// 设置规则
	bool SetRule(const std::string& rule = "");
	// 获得规则
	std::string GetRule()const;

	// 匹配
	bool Match(const std::string& text);
private:
	// 转义字符判断
	bool IsEscCh(const char& ch)const;
	// 输入合法性判断
	bool IsLegal(const std::string& rule)const;
	// 深度优先遍历
	void Dfs(int s);
	// 建立NFA
	void BuildNfa();
};