// Bradley Christensen - 2023
#include "AllComponentsAndSystems.h"
#include "Engine/ECS/AdminSystem.h"
#include "Engine/Multithreading/JobSystem.h"
#include "gtest/gtest.h"



int TEST_ECS_NUM_ITERATIONS = 1;



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, CreateDestroy)
{
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);
    ecs->Startup();
    ecs->Shutdown();
    delete ecs;
    ecs = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, RegisterComponentBits)
{
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);
    ecs->Startup();

    ecs->RegisterComponent<CTransform2D>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CPhysics>(ComponentStorageType::MAP);
    ecs->RegisterComponent<CMovement>(ComponentStorageType::TAG);
    ecs->RegisterComponent<SCWorld>(ComponentStorageType::SINGLETON);

    EXPECT_EQ(ecs->GetComponentBitMask<CTransform2D>(), 1);
    EXPECT_EQ(ecs->GetComponentBitMask<CPhysics>(), 2);
    EXPECT_EQ(ecs->GetComponentBitMask<CMovement>(), 4);
    EXPECT_EQ(ecs->GetComponentBitMask<SCWorld>(), 8);
    
    ecs->Shutdown();
    delete ecs;
    ecs = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, RegisterSystems)
{
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);

    ecs->RegisterComponent<CTransform2D>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CPhysics>(ComponentStorageType::MAP);
    ecs->RegisterComponent<CMovement>(ComponentStorageType::TAG);

    ecs->RegisterSystem<SMovement>();
    ecs->RegisterSystem<SPhysics>(); 
    
    ecs->Startup();
    ecs->RunFrame(0.0166f);
    ecs->Shutdown();
    
    delete ecs;
    ecs = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, SystemSubgraphs)
{
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);

    ecs->RegisterComponent<CTransform2D>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CPhysics>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CMovement>(ComponentStorageType::ARRAY);

    ecs->RegisterSystem<SMovement>(0);
    ecs->RegisterSystem<SPhysics>(0);
    
    ecs->RegisterSystem<SMovement>(1);
    ecs->RegisterSystem<SPhysics>(1);
    
    auto& subgraph0 = ecs->GetSystemSubgraph(0);
    auto& subgraph1 = ecs->GetSystemSubgraph(1);
    subgraph1.m_timeStep = 0.01f;

    EXPECT_EQ(subgraph0.m_systems.size(), 2);
    EXPECT_EQ(subgraph1.m_systems.size(), 2);
    
    ecs->Startup();

    for (int i = 0; i < (int) MAX_ENTITIES; ++i)
    {
        EntityID entity = ecs->CreateEntity(i);
        EXPECT_EQ(entity, (EntityID) i);

        CTransform2D* transform = ecs->AddComponent<CTransform2D>(entity);
        EXPECT_NE(transform, nullptr);
    
        CPhysics* physics = ecs->AddComponent<CPhysics>(entity);
        EXPECT_NE(physics, nullptr);
    
        CMovement* movement = ecs->AddComponent<CMovement>(entity);
        EXPECT_NE(movement, nullptr);
        
        transform->m_screenPosition = Vec2(100.f, 100.f);
        movement->m_frameMoveDirection = Vec2(1.f, 0.f);
        movement->m_frameMoveStrength = 1.f;
        movement->m_movementSpeed = 100.f;
    }
    
    ecs->RunFrame(1.f); // Should run SMovement + SPhysics for a total of 2s of time. (since they are registered twice, once with a timestep and once without).
    
    CTransform2D* transform = ecs->GetComponent<CTransform2D>(0);
    EXPECT_FLOAT_EQ(transform->m_screenPosition.x, 100.0 + 2.0 * 100.0);

    // Run both sets of systems yet another time, but through the direct run subgraph functions
    ecs->RunSystemSubgraph(0, 1.f);
    ecs->RunSystemSubgraph(1, 1.f);
    
    EXPECT_FLOAT_EQ(transform->m_screenPosition.x, 100.0 + 4.0 * 100.0);
    
    ecs->Shutdown();
    
    delete ecs;
    ecs = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, ECSFrameSingleThreaded)
{
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);

    ecs->RegisterComponent<CTransform2D>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CPhysics>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CMovement>(ComponentStorageType::ARRAY);

    ecs->RegisterSystem<SMovement>();
    ecs->RegisterSystem<SPhysics>();
    
    ecs->Startup();

    for (int i = 0; i < (int) MAX_ENTITIES; ++i)
    {
        EntityID entity = ecs->CreateEntity(i);
        EXPECT_EQ(entity, (EntityID) i);

        CTransform2D* transform = ecs->AddComponent<CTransform2D>(entity);
        EXPECT_NE(transform, nullptr);
    
        CPhysics* physics = ecs->AddComponent<CPhysics>(entity);
        EXPECT_NE(physics, nullptr);
    
        CMovement* movement = ecs->AddComponent<CMovement>(entity);
        EXPECT_NE(movement, nullptr);
        
        transform->m_screenPosition = Vec2(100.f, 100.f);
        movement->m_frameMoveDirection = Vec2(1.f, 0.f);
        movement->m_frameMoveStrength = 1.f;
        movement->m_movementSpeed = 100.f;
    }

    for (int i = 0; i < TEST_ECS_NUM_ITERATIONS; ++i)
    {
        ecs->RunFrame(1.f);
    }
    
    CTransform2D* transform = ecs->GetComponent<CTransform2D>(0);

    EXPECT_NE(transform->m_screenPosition, Vec2(100.f, 100.f));
    EXPECT_FLOAT_EQ(transform->m_screenPosition.x, 100.f + (float) TEST_ECS_NUM_ITERATIONS * 100.f); // move over to the right by 100 units bc we moving at 100 units/sec to the right and t == 1 second
    
    ecs->Shutdown();
    delete ecs;
    ecs = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, ECSFrameAutoMultithreading)
{
    g_jobSystem = new JobSystem();
    g_jobSystem->Startup();
    
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);
    ecs->SetAllMultithreadingSettings(true, true, false);

    ecs->RegisterComponent<CTransform2D>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CPhysics>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CMovement>(ComponentStorageType::ARRAY);

    ecs->RegisterSystem<SMovement>();
    ecs->RegisterSystem<SPhysics>();
    
    ecs->Startup();

    for (int i = 0; i < (int) MAX_ENTITIES; ++i)
    {
        EntityID entity = ecs->CreateEntity(i);
        EXPECT_EQ(entity, (EntityID) i);

        CTransform2D* transform = ecs->AddComponent<CTransform2D>(entity);
        EXPECT_NE(transform, nullptr);
    
        CPhysics* physics = ecs->AddComponent<CPhysics>(entity);
        EXPECT_NE(physics, nullptr);
    
        CMovement* movement = ecs->AddComponent<CMovement>(entity);
        EXPECT_NE(movement, nullptr);
        
        transform->m_screenPosition = Vec2(100.f, 100.f);
        movement->m_frameMoveDirection = Vec2(1.f, 0.f);
        movement->m_frameMoveStrength = 1.f;
        movement->m_movementSpeed = 100.f;
    }

    for (int i = 0; i < TEST_ECS_NUM_ITERATIONS; ++i)
    {
        ecs->RunFrame(1.f);
    }
    
    CTransform2D* transform = ecs->GetComponent<CTransform2D>(0);

    EXPECT_NE(transform->m_screenPosition, Vec2(100.f, 100.f));
    EXPECT_FLOAT_EQ(transform->m_screenPosition.x, 100.f + (float) TEST_ECS_NUM_ITERATIONS * 100.f); // move over to the right by 100 units bc we moving at 100 units/sec to the right and t == 1 second
    
    ecs->Shutdown();
    delete ecs;
    ecs = nullptr;
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
TEST(ECS, ECSFrameSystemSplitting)
{
    g_jobSystem = new JobSystem();
    g_jobSystem->Startup();
    
    AdminSystemConfig config;
    AdminSystem* ecs = new AdminSystem(config);
    ecs->SetAllMultithreadingSettings(true, false, true);

    ecs->RegisterComponent<CTransform2D>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CPhysics>(ComponentStorageType::ARRAY);
    ecs->RegisterComponent<CMovement>(ComponentStorageType::ARRAY);

    ecs->RegisterSystem<SMovement>();
    ecs->RegisterSystem<SPhysics>();
    
    ecs->Startup();

    for (int i = 0; i < (int) MAX_ENTITIES; ++i)
    {
        EntityID entity = ecs->CreateEntity(i);
        EXPECT_EQ(entity, (EntityID) i);

        CTransform2D* transform = ecs->AddComponent<CTransform2D>(entity);
        EXPECT_NE(transform, nullptr);
    
        CPhysics* physics = ecs->AddComponent<CPhysics>(entity);
        EXPECT_NE(physics, nullptr);
    
        CMovement* movement = ecs->AddComponent<CMovement>(entity);
        EXPECT_NE(movement, nullptr);
        
        transform->m_screenPosition = Vec2(100.f, 100.f);
        movement->m_frameMoveDirection = Vec2(1.f, 0.f);
        movement->m_frameMoveStrength = 1.f;
        movement->m_movementSpeed = 100.f;
    }

    for (int i = 0; i < TEST_ECS_NUM_ITERATIONS; ++i)
    {
        ecs->RunFrame(1.f);
    }
    
    CTransform2D* transform = ecs->GetComponent<CTransform2D>(0);

    EXPECT_NE(transform->m_screenPosition, Vec2(100.f, 100.f));
    EXPECT_FLOAT_EQ(transform->m_screenPosition.x, 100.f + (float) TEST_ECS_NUM_ITERATIONS * 100.f); // move over to the right by 100 units bc we moving at 100 units/sec to the right and t == 1 second
    
    ecs->Shutdown();
    delete ecs;
    ecs = nullptr;
    
    g_jobSystem->Shutdown();
    delete g_jobSystem;
    g_jobSystem = nullptr;
}