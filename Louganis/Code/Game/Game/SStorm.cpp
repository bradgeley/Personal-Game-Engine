// Bradley Christensen - 2022-2025
#include "SStorm.h"
#include "SCStorm.h"
#include "SCWorld.h"
#include "Engine/Math/MathUtils.h"



//----------------------------------------------------------------------------------------------------------------------
void SStorm::Startup()
{
	AddWriteDependencies<SCStorm>();
	AddReadDependencies<SCWorld>();

	SCStorm& scStorm = g_ecs->GetSingleton<SCStorm>();
	scStorm.m_stormLocation = scStorm.m_initialStormLocation;
	scStorm.m_stormMovementSpeed = scStorm.m_initialStormMovementSpeed;
	scStorm.m_stormRadius = scStorm.m_initialStormRadius;
	scStorm.m_stormRadiusGrowthRate = scStorm.m_initialStormRadiusGrowthRate;
	scStorm.m_stormRadiusAccelerationRate = scStorm.m_initialStormRadiusAccelerationRate;
}



//----------------------------------------------------------------------------------------------------------------------
void SStorm::Run(SystemContext const& context)
{
	SCStorm& scStorm = g_ecs->GetSingleton<SCStorm>();
	SCWorld const& scWorld = g_ecs->GetSingleton<SCWorld>();

	Vec2 playerLocation = scWorld.GetTileBounds(scWorld.m_lastKnownPlayerWorldCoords).GetCenter();
	
	Vec2 toPlayerVector = playerLocation - scStorm.m_stormLocation;
	float distanceToTravel = toPlayerVector.GetLength();
	Vec2 toPlayerNormal = toPlayerVector / distanceToTravel;

	if (distanceToTravel < scStorm.m_stormMovementSpeed * context.m_deltaSeconds)
	{
		toPlayerNormal = toPlayerVector; // Move directly to player if within this frame's movement distance
	}
	else
	{
		scStorm.m_stormLocation += toPlayerNormal * scStorm.m_stormMovementSpeed * context.m_deltaSeconds; // Move storm toward player at constant speed
	}

	scStorm.m_stormRadiusGrowthRate += scStorm.m_stormRadiusAccelerationRate * context.m_deltaSeconds;
	scStorm.m_stormRadius += scStorm.m_stormRadiusGrowthRate * context.m_deltaSeconds;
}