// Bradley Christensen - 2022-2026
#include "SAttachment.h"
#include "CAttachment.h"
#include "CRender.h"
#include "CTransform.h"
#include "SCEntityFactory.h"



//----------------------------------------------------------------------------------------------------------------------
void SAttachment::Startup()
{
	AddReadDependencies<CAttachment>();
	AddWriteDependencies<CTransform, CRender, SCEntityFactory>();
}



//----------------------------------------------------------------------------------------------------------------------
void SAttachment::Run(SystemContext const& context)
{
	// Read dependencies
	auto const& attachStorage = g_ecs->GetArrayStorage<CAttachment>();

	// Write dependencies
	auto& transStorage = g_ecs->GetArrayStorage<CTransform>();
	auto& renderStorage = g_ecs->GetArrayStorage<CRender>();
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
		}
		else if (attach.m_destroyIfAttachedToEntityDestroyed)
		{
			CRender& render = *renderStorage.Get(it);
			render.SetIsHidden(true); // Hide while pending kill, prevents a bug where attachments pop up at 0,0
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}



//----------------------------------------------------------------------------------------------------------------------
void SAttachment::Shutdown()
{

}
