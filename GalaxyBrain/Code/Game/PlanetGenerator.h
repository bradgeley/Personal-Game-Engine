// Bradley Christensen - 2023
#pragma once



struct EntityDef;



//----------------------------------------------------------------------------------------------------------------------
// PlanetGenerator
// 
// Dependencies: All (creates entities)
//
class PlanetGenerator
{
public:

	static EntityDef const* Generate(int seed);
};