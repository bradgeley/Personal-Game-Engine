// Bradley Christensen - 2022-2023
#pragma once
#include <vector>



class SudokuGrid;



//----------------------------------------------------------------------------------------------------------------------
struct SudokuRule
{
	virtual ~SudokuRule() = default;

	virtual bool Evaluate(SudokuGrid const& grid) const = 0;
	virtual SudokuRule* Clone() const = 0;

	bool m_enabled = true;
};



//----------------------------------------------------------------------------------------------------------------------
struct SudokuRuleRowOnceEach : SudokuRule
{
	virtual bool Evaluate(SudokuGrid const& grid) const override;
	virtual SudokuRule* Clone() const override;
};



//----------------------------------------------------------------------------------------------------------------------
struct SudokuRuleColumnOnceEach : SudokuRule
{
	virtual bool Evaluate(SudokuGrid const& grid) const override;
	virtual SudokuRule* Clone() const override;
};



//----------------------------------------------------------------------------------------------------------------------
struct SudokuRuleBoxOnceEach : SudokuRule
{
	virtual bool Evaluate(SudokuGrid const& grid) const override;
	virtual SudokuRule* Clone() const override;
};



//----------------------------------------------------------------------------------------------------------------------
struct SudokuRuleSet
{
	SudokuRuleSet() = default;
	SudokuRuleSet(SudokuRuleSet const& copy);
	~SudokuRuleSet();

	std::vector<SudokuRule*> m_rules;
};

