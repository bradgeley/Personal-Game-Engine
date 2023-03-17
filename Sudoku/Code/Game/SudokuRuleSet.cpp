// Bradley Christensen - 2022-2023
#include "SudokuRuleSet.h"

#include "Engine/Core/EngineCommon.h"



//----------------------------------------------------------------------------------------------------------------------
SudokuRuleSet::SudokuRuleSet(SudokuRuleSet const& copy)
{
	for (auto& rule : copy.m_rules)
	{
		m_rules.emplace_back(rule->Clone());
	}
}



//----------------------------------------------------------------------------------------------------------------------
SudokuRuleSet::~SudokuRuleSet()
{
	for (auto& rule : m_rules)
	{
		delete rule;
		rule = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------------
bool SudokuRuleRowOnceEach::Evaluate(SudokuGrid const& grid) const
{
	UNUSED(grid)
	return false;
}



SudokuRule* SudokuRuleRowOnceEach::Clone() const
{
	return new SudokuRuleRowOnceEach();
}



//----------------------------------------------------------------------------------------------------------------------
bool SudokuRuleColumnOnceEach::Evaluate(SudokuGrid const& grid) const
{
	UNUSED(grid)
	return false;
}



SudokuRule* SudokuRuleColumnOnceEach::Clone() const
{
	return new SudokuRuleColumnOnceEach();
}



//----------------------------------------------------------------------------------------------------------------------
bool SudokuRuleBoxOnceEach::Evaluate(SudokuGrid const& grid) const
{
	UNUSED(grid)
	return false;
}



SudokuRule* SudokuRuleBoxOnceEach::Clone() const
{
	return new SudokuRuleBoxOnceEach();
}
