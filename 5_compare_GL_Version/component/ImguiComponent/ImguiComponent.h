#ifndef KATA_COMPONENT_IMGUICOMPONENT_H_
#define KATA_COMPONENT_IMGUICOMPONENT_H_

#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

namespace kata
{
	namespace component
	{
		class ImguiInputComponent : public InputComponent
		{
		public:
			ImguiInputComponent() {}
			void update() {}
		};

		class ImguiPhysicsComponent : public PhysicsComponent
		{
		public:
			ImguiPhysicsComponent() {}
			void update() {}
		};

		class ImguiGraphicsComponent : public GraphicsComponent
		{
		public:
			ImguiGraphicsComponent() {}
			void update() {}
		};
	}
}

#endif // KATA_COMPONENT_IMGUICOMPONENT_H_
