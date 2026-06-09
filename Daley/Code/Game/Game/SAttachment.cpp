// Bradley Christensen - 2022-2026
#include "SAttachment.h"
#include "CAttachment.h"
#include "CTransform.h"
#include "SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SAttachment::Startup()
{
	AddReadDependencies<CAttachment>();
	AddWriteDependencies<CTransform, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAttachment::Run(SystemContext const& context)
{
	// Read dependencies
	auto const& attachStorage = g_ecs->GetArrayStorage<CAttachment>();

	// Write dependencies
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& factory = g_ecs->GetSingleton<SCEntityFactory>();

	for (auto it = g_ecs->Iterate<CAttachment, CTransform>(context); it.IsValid(); ++it)
	{
		CAttachment const& attach = *attachStorage.Get(it);

		bool hasValidAttachment = g_ecs->IsValid(attach.m_attachedTo);
		if (hasValidAttachment)
		{
			CTransform& transform = *transStorage.Get(it);
			CTransform const& attachedToTransform = *transStorage.Get(attach.m_attachedTo.GetIndex());
			transform.m_pos = attachedToTransform.m_pos;
			transform.m_orientation = attachedToTransform.m_orientation;
		}
		else if (attach.m_destroyIfAttachedToEntityDestroyed)
		{
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SAttachment::Shutdown()
{

}
