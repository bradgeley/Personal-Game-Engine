// Bradley Christensen - 2022-2025
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
//
#include "CAIController.h"
#include "CAnimation.h"
#include "CCollision.h"
#include "CDeath.h"
#include "CEntityDebug.h"
#include "CHealth.h"
#include "CLifetime.h"
#include "CMovement.h"
#include "CProjectile.h"
#include "CRender.h"
#include "CTime.h"
#include "CTransform.h"
#include "CWeapon.h"



//----------------------------------------------------------------------------------------------------------------------
// Singletons
//
#include "AllSingletonComponents.h"
