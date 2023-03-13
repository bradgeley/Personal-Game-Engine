// Bradley Christensen - 2022-2023
#include "SudokuRuleSet.h"



SudokuRuleSet::~SudokuRuleSet()
{
	for (auto& rule : m_rules)
	{
		delete rule;
		rule = nullptr;
	}
}



SudokuRuleSet SudokuRuleSet::GetStandardSudokuRules()
{
	SudokuRuleSet standardRules;
	standardRules.m_rules.push_back(new SudokuRuleRowOnceEach());
	standardRules.m_rules.push_back(new SudokuRuleColumnOnceEach());
	standardRules.m_rules.push_back(new SudokuRuleBoxOnceEach());
	return standardRules;
}



bool SudokuRuleRowOnceEach::Evaluate(SudokuGrid const& grid) const
{
	return false;
}



bool SudokuRuleColumnOnceEach::Evaluate(SudokuGrid const& grid) const
{
	return false;
}



bool SudokuRuleBoxOnceEach::Evaluate(SudokuGrid const& grid) const
{
	return false;
}
