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
		private:
			int spin = -1;

			float param_speed_spin = 1.0f;
			float param_resize = 1.0f;
			ImVec4 param_color_clear = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImVec4 param_color_vertex_r = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			ImVec4 param_color_vertex_g = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
			ImVec4 param_color_vertex_b = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);

		public:
			ImguiInputComponent() {}
			void update()
			{

			}
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
