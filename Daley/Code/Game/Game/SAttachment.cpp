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
void SAttachment::Run(SystemContext const& context) const
{
	// Read dependencies
	auto& attachStorage = context.GetArrayStorageConst<CAttachment>();

	// Write dependencies
	auto& transStorage = context.GetArrayStorage<CTransform>();
	auto& factory = context.GetSingleton<SCEntityFactory>();

	for (auto it = context.Iterate<CAttachment, CTransform>(); it.IsValid(); ++it)
	{
		CAttachment const& attach = attachStorage[it];

		bool hasValidAttachment = context.IsValid(attach.m_attachedTo);
		if (hasValidAttachment)
		{
			CTransform& transform = transStorage[it];
			CTransform const& attachedToTransform = transStorage[attach.m_attachedTo];
			transform.m_pos = attachedToTransform.m_pos;
			transform.m_orientation = attachedToTransform.m_orientation;
		}
		else if (attach.m_destroyIfAttachedToEntityDestroyed)
		{
			factory.m_entitiesToDestroy.push_back(it.GetEntityID());
		}
	}
}
