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
#include "CTransform.h"
#include "CRender.h"
#include "CCamera.h"
#include "CPhysics.h"
#include "CMovement.h"
#include "CPlayerController.h"
#include "CCollision.h"



//----------------------------------------------------------------------------------------------------------------------
// Singletons
#include "SCEntityFactory.h"
#include "SCRenderer.h"
#include "SCUniverse.h"
#include "SCDebug.h"
