#include <EntityStdafx.h>

#include <Entity/IComponent.h>

#include <Entity/IEntity.h>

namespace GLEngine::Entity
{

//=================================================================================
I_Component::I_Component(std::shared_ptr<I_Entity> owner)
	: m_Owner(owner)
	, m_ModelMatrix(glm::mat4(1.0f))
{

}

//=================================================================================
std::shared_ptr<I_Entity> I_Component::GetOwner() const
{
	// component without entity is invalid state
	if (auto owner = m_Owner.lock())
	{
		return owner;
	}
	GLE_ASSERT(m_Owner.expired(), "Component has no owner.");
	return nullptr;
}

}