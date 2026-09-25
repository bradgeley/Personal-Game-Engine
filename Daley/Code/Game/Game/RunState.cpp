// Bradley Christensen - 2022-2026
#include "RunState.h"
#include "AbilityDef.h"
#include "BiomeDef.h"
#include "FlavorDef.h"
#include "TileDef.h"
#include "MapGeneratorDef.h"
#include "Engine/Assets/Font.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include "Engine/Core/NamedProperties.h"
#include "Engine/Debug/DevConsoleUtils.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Math/RandomNumberGenerator.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexUtils.h"
#include "Engine/Renderer/Vertex_PCU.h"
#include "Engine/Window/Window.h"



//----------------------------------------------------------------------------------------------------------------------
static constexpr char const* MAIN_MENU_FONT_NAME = "Data/Fonts/Gypsy.fnt";
const char* s_abilityDefsFilePath = "Data/Definitions/AbilityDefs.xml";
const char* s_flavorCombinationAbilityDefsFilePath = "Data/Definitions/FlavorCombinationAbilityDefs.xml";



//----------------------------------------------------------------------------------------------------------------------
RunState::RunState()
{
	m_name = "RunState";
}



//----------------------------------------------------------------------------------------------------------------------
void SetupJourney(RunData& runData)
{
	runData.m_mode = GameMode::Journey;

	// Todo: make a file for journey

	// Forest Biome first
	runData.m_missionGenData[0] = { "forest" };
	runData.m_missionGenData[1] = { "forestOpen" };
	runData.m_missionGenData[2] = { "forestHatch" };

	// Then Desert
	runData.m_missionGenData[3] = { "desert" };
	runData.m_missionGenData[4] = { "desertOpen" };
	runData.m_missionGenData[5] = { "desertHatch" };

	// Then River
	runData.m_missionGenData[6] = { "river" };
	runData.m_missionGenData[7] = { "riverDouble" };
	runData.m_missionGenData[8] = { "riverHatch" };
}



//----------------------------------------------------------------------------------------------------------------------
void SetupEndless(RunData& runData)
{
	MapGeneratorDef const* mapGeneratorDef = MapGeneratorDef::GetRandom(runData.m_seed);
	ASSERT_OR_DIE(mapGeneratorDef != nullptr, "SetupEndless() - No MapGeneratorDefs available for endless mode.");

	runData.m_mode = GameMode::Endless;

	//runData.m_missionGenData[0] = { mapGeneratorDef->m_name };
	runData.m_missionGenData[0] = { "forest" };
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Enter(NamedProperties const& props)
{
	GameState::Enter(props);

	AbilityDef::LoadFromXML(s_abilityDefsFilePath);
	AbilityDef::LoadFromXML(s_flavorCombinationAbilityDefsFilePath);
	FlavorDef::LoadFromXML();
	TileDef::LoadFromXML();
	BiomeDef::LoadFromXML();
	MapGeneratorDef::LoadFromXML();

	Name mode = props.Get<Name>("mode", Name::Invalid);

	g_eventSystem->SubscribeMethod("MissionOver", this, &RunState::MissionOver);
	DevConsoleUtils::AddDevConsoleCommand("GenerateFlavorCombinations", &RunState::GenerateFlavorCombinations, "filename", DevConsoleArgType::Name);
	DevConsoleUtils::AddDevConsoleCommand("SaveAbilityDefs", &RunState::SaveAbilityDefsToXML, "filename", DevConsoleArgType::Name);

	m_untexturedVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();
	m_textVerts = g_renderer->MakeVertexBuffer<Vertex_PCU>();

	IntVec2 windowDims = g_window->GetActualWindowResolution();
	m_camera.SetOrthoDims2D(Vec2(windowDims));

	VertexBuffer& untexturedVBO = *g_renderer->GetVertexBuffer(m_untexturedVerts);

	VertexUtils::AddVertsForAABB2(untexturedVBO, m_camera.GetOrthoBounds2D(), Rgba8::DarkGray);

	m_fontID = g_assetManager->LoadSynchronous<Font>(MAIN_MENU_FONT_NAME);
	Font const* font = g_assetManager->Get<Font>(m_fontID);
	ASSERT_OR_DIE(font != nullptr, StringUtils::StringF("Failed to load %s for RunState!", MAIN_MENU_FONT_NAME));

	m_runData.m_runModifierPool.LoadFromXml("Data/Definitions/RunModifierPool.xml");
	m_runData.m_numModifierChoicesRemaining = 1; // Give 1 mod choice at the start of the run.

	m_runData.m_seed = g_rng->Rand();

	if (mode == "Endless")
	{
		SetupEndless(m_runData);
	}
	else // Journey
	{
		SetupJourney(m_runData);
	}

	NamedProperties transitionProps;
	transitionProps.Set<Name>("state", "TowerDefense");
	transitionProps.Set<RunData*>("runData", &m_runData);
	g_eventSystem->FireEvent("PushState", transitionProps);
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Exit(NamedProperties const& props)
{
	MapGeneratorDef::Shutdown();
	BiomeDef::Shutdown();
	TileDef::Shutdown();
	FlavorDef::Shutdown();
	AbilityDef::Shutdown();

	g_eventSystem->UnsubscribeMethod("MissionOver", this, &RunState::MissionOver);
	DevConsoleUtils::RemoveDevConsoleCommand("GenerateFlavorCombinations", &RunState::GenerateFlavorCombinations);
	DevConsoleUtils::RemoveDevConsoleCommand("SaveAbilityDefs", &RunState::SaveAbilityDefsToXML);

	g_renderer->ReleaseVertexBuffer(m_untexturedVerts);
	g_renderer->ReleaseVertexBuffer(m_textVerts);

	m_runData.Shutdown();

	GameState::Exit(props);
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Update(float)
{
}



//----------------------------------------------------------------------------------------------------------------------
void RunState::Render() const
{
	// No longer renders right now, we skip straight to the next mission

	VertexBuffer& textVBO = *g_renderer->GetVertexBuffer(m_textVerts);
	textVBO.ClearVerts();

	Font const* font = g_assetManager->Get<Font>(m_fontID);
	if (font)
	{
		AABB2 cameraBounds = m_camera.GetOrthoBounds2D();
		font->AddVertsForAlignedText2D(textVBO, cameraBounds.GetCenter(), Vec2::ZeroVector, 100.f, "Run State", Rgba8::AliceBlue);
		std::string instructionText = StringUtils::StringF("Press Space to Start Mission %i\nPress ESC to return to Main Menu", m_runData.m_missionIndex + 1);
		font->AddVertsForAlignedText2D(textVBO, cameraBounds.GetBottomCenter() + Vec2(0.f, 50.f), Vec2(0.f, 1.f), 50.f, instructionText, Rgba8::Yellow);
	}

	g_renderer->BeginCameraAndWindow(&m_camera, g_window);

	g_renderer->BindTexture();
	g_renderer->BindShader();
	g_renderer->DrawVertexBuffer(m_untexturedVerts);

	if (font)
	{
		font->SetRendererState(*g_renderer);
		g_renderer->DrawVertexBuffer(m_textVerts);
	}
}



//----------------------------------------------------------------------------------------------------------------------
bool RunState::MissionOver(NamedProperties&)
{
	if (m_runData.m_health <= 0.f)
	{
		NamedProperties popStateProps;
		popStateProps.Set<Name>("state", "MainMenu");
		popStateProps.Set<bool>("wipe", true);
		g_eventSystem->FireEvent("ChangeState", popStateProps);
	}
	else
	{
		m_runData.m_missionIndex++;
		if (m_runData.m_missionIndex == StaticGameSettings::s_numMissionsForVictory || m_runData.m_missionGenData[m_runData.m_missionIndex].m_mapName == Name::Invalid)
		{
			NamedProperties changeStateProps;
			changeStateProps.Set<Name>("state", "GameOver");
			g_eventSystem->FireEvent("ChangeState", changeStateProps);
		}
		else
		{
			NamedProperties popStateProps;
			popStateProps.Set<Name>("state", "RunState");
			g_eventSystem->FireEvent("PopState", popStateProps);

			NamedProperties transitionProps;
			transitionProps.Set<Name>("state", "TowerDefense");
			transitionProps.Set<RunData*>("runData", &m_runData);
			g_eventSystem->FireEvent("PushState", transitionProps);
		}
	}

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool RunState::SaveAbilityDefsToXML(NamedProperties& props)
{
	Name filename = props.Get<Name>("filename", Name("AbilitiesSaved"));

	std::string filepath = "Data/Definitions/" + filename.ToString() + ".xml";
	AbilityDef::SaveToXML(filepath);

	return false;
}



//----------------------------------------------------------------------------------------------------------------------
bool RunState::GenerateFlavorCombinations(NamedProperties& props)
{
	Name filename = props.Get<Name>("filename", Name("FlavorCombinationAbilityDefs"));

	std::string filepath = "Data/Definitions/" + filename.ToString() + ".xml";

	XmlDocument doc;

	XmlElement* rootElem = doc.NewElement("AbilityDefs");
	doc.InsertFirstChild(rootElem);

	std::vector<FlavorDef> const& allFlavorDefs = FlavorDef::GetAllFlavorDefs();

	// Double combinations
	for (size_t base = 0; base < allFlavorDefs.size(); ++base)
	{
		for (size_t swirl = 0; swirl < allFlavorDefs.size(); ++swirl)
		{
			if (swirl == base)
			{
				continue;
			}

			FlavorDef const& baseFlavor = allFlavorDefs[base];
			FlavorDef const& swirlFlavor = allFlavorDefs[swirl];

			AbilityDef const* mainAbility = AbilityDef::GetAbilityDef(baseFlavor.m_abilities[0]);
			ASSERT_OR_DIE(mainAbility != nullptr, StringUtils::StringF("Failed to get ability \"%s\" for flavor combination generation.", baseFlavor.m_abilities[0].ToCStr()).c_str());

			// Combined ability uses the main ability of the base flavor
			AbilityDef* combinedAbility = mainAbility->Copy();
			ASSERT_OR_DIE(combinedAbility != nullptr, StringUtils::StringF("Failed to copy ability \"%s\" for flavor combination generation.", mainAbility->m_name.ToCStr()).c_str());

			combinedAbility->m_name = StringUtils::StringF("%s_%s_g", baseFlavor.m_name.ToCStr(), swirlFlavor.m_name.ToCStr());

			AbilityDef const* swirlAbility = AbilityDef::GetAbilityDef(swirlFlavor.m_abilities[0]);
			ASSERT_OR_DIE(swirlAbility != nullptr, StringUtils::StringF("Failed to get ability \"%s\" for flavor combination generation.", swirlFlavor.m_abilities[0].ToCStr()).c_str());

			combinedAbility->WriteToXmlDoc(&doc, rootElem); 

			delete combinedAbility;
		}
	}

	// Triple combinations
	for (size_t base = 0; base < allFlavorDefs.size(); ++base)
	{
		for (size_t swirlA = 0; swirlA < allFlavorDefs.size(); ++swirlA)
		{
			if (swirlA == base)
			{
				continue;
			}
	
			for (size_t swirlB = swirlA + 1; swirlB < allFlavorDefs.size(); ++swirlB)
			{
				if (swirlB == base)
				{
					continue;
				}
	
				FlavorDef const& baseFlavor = allFlavorDefs[base];
				FlavorDef const* swirl1 = &allFlavorDefs[swirlA];
				FlavorDef const* swirl2 = &allFlavorDefs[swirlB];

				// Alphebetize swirl names for consistent ability naming
				if (swirl1->m_name.ToString() > swirl2->m_name.ToString())
				{
					swirl1 = &allFlavorDefs[swirlB];
					swirl2 = &allFlavorDefs[swirlA];
				}

				AbilityDef const* mainAbility = AbilityDef::GetAbilityDef(baseFlavor.m_abilities[0]);
				AbilityDef* copy = mainAbility->Copy();
				copy->m_name = StringUtils::StringF("%s_%s_%s_g", baseFlavor.m_name.ToCStr(), swirl1->m_name.ToCStr(), swirl2->m_name.ToCStr());
				copy->WriteToXmlDoc(&doc, rootElem);

				delete copy;
			}
		}
	}

	doc.SaveFile(filepath.c_str());

	return false;
}
