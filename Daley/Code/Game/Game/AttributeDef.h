// Bradley Christensen - 2022-2026
#pragma once
#include "AttributeDisplayInfo.h"
#include "Engine/Core/Name.h"
#include <cstdint>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
enum class AttributeTag : uint8_t
{
	None = 0,
	Persistent = 1 << 0, // This attribute is saved to the save file and persists across runs
	State = 1 << 1,      // This attribute is part of the run state and is reset when a new run starts
};



//----------------------------------------------------------------------------------------------------------------------
struct AttributeDef
{
public:

	virtual ~AttributeDef() = default;
	explicit AttributeDef(void const* xmlElement);

public:

	static void LoadFromXML(Name filepath);
	static void Shutdown();
	static AttributeDef const* GetAttributeDef(uint8_t id);
	static AttributeDef const* GetAttributeDef(Name name);
	static int GetAttributeDefID(Name name);
	static std::vector<AttributeDef const*> const& GetAllAttributeDefs();

	bool GetIsPersistent() const;
	bool GetIsState() const;
	void SetIsPersistent(bool isPersistent);
	void SetIsState(bool isState);

private:

	static std::vector<AttributeDef const*> s_attributeDefs;

public:

	Name m_name = Name::Invalid;
	uint8_t m_tags = 0;
	AttributeDisplayInfo m_displayInfo;
};