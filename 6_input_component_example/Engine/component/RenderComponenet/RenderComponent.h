#ifndef KATA_COMPONENT_SCENECOMPONENT_H_
#define KATA_COMPONENT_SCENECOMPONENT_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "../../render/World.h"

#include "../Component.h"
#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

#include "../InputComponent/ImguiViewerInputComponent.h"


namespace kata
{
	namespace component
	{
		class RenderComponent : public GraphicsComponent
		{
		protected:
			render::World *m_world = nullptr;
			
			GLuint *m_pixel = nullptr;
			
			bool isSingleWindow = false;

		public:
			RenderComponent() {}

			void setGLWindow(render::World *_world)
			{
				m_world = _world;
			}

			void setPixel(GLuint *pixel)
			{
				m_pixel = pixel;
			}

			void onSingleWindowMode()
			{
				isSingleWindow = true;
			}

			virtual void setup() {}

			virtual void render() {}
		};
	}
}

#endif // KATA_COMPONENT_SCENECOMPONENT_H_
