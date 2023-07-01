// Bradley Christensen - 2023
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// ADD NEW COMPONENT:
//
// Update:
//  1. Write constructor that takes an xml element
//  2. EntityDef header and constructor
//	3. SEntityFactory::CreateEntityFromDef
//  4. Register component in Game.cpp



//----------------------------------------------------------------------------------------------------------------------
// Components
#include "Components/CTransform.h"
#include "Components/CRender.h"
#include "Components/CCamera.h"
#include "Components/CPhysics.h"
#include "Components/CMovement.h"
#include "Components/CPlayerController.h"



//----------------------------------------------------------------------------------------------------------------------
// Singletons
#include "Singletons/SCEntityFactory.h"
#include "Singletons/SCRenderer.h"
#include "Singletons/SCUniverse.h"
