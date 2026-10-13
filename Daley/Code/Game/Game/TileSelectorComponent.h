// Bradley Christensen - 2022-2026
#pragma once
#include "Engine/Math/IntVec2.h"
#include <functional>
#include <vector>



class MapGenerator;
class SCWorld;
struct TileSelectorComponentDef;
struct NoiseRangeSelectorComponentDef;
struct NoisePeakSelectorComponentDef;
struct DistanceFieldSelectorComponentDef;



//----------------------------------------------------------------------------------------------------------------------
class TileSelectorComponent
{
public:

	TileSelectorComponent(TileSelectorComponentDef const& def);
	virtual ~TileSelectorComponent() = default;

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) = 0;

public:

	TileSelectorComponentDef const* m_def = nullptr;
};



//----------------------------------------------------------------------------------------------------------------------
class NoiseRangeSelectorComponent : public TileSelectorComponent
{
public:

	NoiseRangeSelectorComponent(NoiseRangeSelectorComponentDef const& def);

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) override final;

	NoiseRangeSelectorComponentDef const& GetDef() const;

public:

};



//----------------------------------------------------------------------------------------------------------------------
class NoisePeakSelectorComponent : public TileSelectorComponent
{
public:

	NoisePeakSelectorComponent(NoisePeakSelectorComponentDef const& def);

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) override final;

	NoisePeakSelectorComponentDef const& GetDef() const;

public:

	std::vector<float> m_cachedNoiseValues;
};



//----------------------------------------------------------------------------------------------------------------------
class DistanceFieldSelectorComponent : public TileSelectorComponent
{
public:

	DistanceFieldSelectorComponent(DistanceFieldSelectorComponentDef const& def);

	virtual void ForEachSelectedTile(MapGenerator& generator, SCWorld& world, std::function<bool(IntVec2 const&)> const& func) override final;

	DistanceFieldSelectorComponentDef const& GetDef() const;

public:

	std::vector<int> m_distanceFieldValues;
};