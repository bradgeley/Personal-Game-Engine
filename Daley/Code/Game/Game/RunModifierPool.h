// Bradley Christensen - 2022-2026
#pragma once
#include "RunModifierDef.h"
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
struct RunModifierPool
{
public:

	void LoadFromXml(const char* filePath);
	void Shutdown();

public:

	std::vector<RunModifierDef const*> m_runModifierDefs;
};