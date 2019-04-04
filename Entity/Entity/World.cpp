#include <EntityStdafx.h>

#include <Entity/World.h>

#include <Entity/IEntity.h>

#include <Physics/Primitives/Frustum.h>


namespace GLEngine {
namespace Entity {

//=================================================================================
C_World::C_World()
	: m_Entities(new std::remove_pointer<decltype(m_Entities)>::type)
{

}

//=================================================================================
C_World::~C_World()
{
	delete m_Entities;
}

//=================================================================================
std::shared_ptr<I_Entity> C_World::GetEntity(GUID id) const
{
	return *std::find_if(m_Entities->begin(), m_Entities->end(), [id](const std::shared_ptr<I_Entity>&entity) {
		return entity->GetID() == id;
	});
}

//=================================================================================
std::vector<std::shared_ptr<I_Entity>> C_World::GetEntities(Physics::Primitives::C_Frustum frust)
{
	return *m_Entities;
}

//=================================================================================
void C_World::AddEntity(std::shared_ptr<I_Entity> entity)
{
	m_Entities->push_back(entity);
}

}
}