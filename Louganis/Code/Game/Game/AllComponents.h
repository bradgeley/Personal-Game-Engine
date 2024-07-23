// Bradley Christensen - 2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// ADD NEW COMPONENT:
//
// Update:
//  1. Write constructor that takes an xml element
//  2. EntityDef header and constructor (EntityDef::EntityDef)
//	3. SEntityFactory::CreateEntityFromDef
//  4. Register component in Game.cpp



//----------------------------------------------------------------------------------------------------------------------
// Components
#include "CCamera.h"
#include "CCollision.h"
#include "CMovement.h"
#include "CPlayerController.h"
#include "CRender.h"
#include "CTransform.h"



//----------------------------------------------------------------------------------------------------------------------
// Singletons
#include "SCEntityFactory.h"
#include "SCFlowField.h"
#include "SCWorld.h"
