#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

namespace GLEngine {
namespace Renderer {
	class I_RenderableComponent : public Entity::I_Component {
	public:
		virtual ~I_RenderableComponent() = default;
		virtual Entity::E_ComponentType GetType() const override
		{
			return Entity::E_ComponentType::Graphical;
		}

	};
}
}
