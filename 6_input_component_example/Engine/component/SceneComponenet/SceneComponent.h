#ifndef KATA_COMPONENT_SCENECOMPONENT_H_
#define KATA_COMPONENT_SCENECOMPONENT_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "../../GL/GLWindow.h"

#include "../Component.h"
#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

#include "../ImguiComponent/ImguiComponent.h"


namespace kata
{
	namespace component
	{
		class ScenePhysicsComponent : public PhysicsComponent
		{
		public:
			ScenePhysicsComponent() {}
			void update() {}
		};

		class SceneGraphicsComponent : public GraphicsComponent
		{
		protected:
			ImguiInputComponent * m_imguiInputComponenet = nullptr;
			
			GL::GLWindow *m_GLWindow = nullptr;
			
			GLuint *m_pixel = nullptr;
			
			bool isSingleWindow = false;

		public:
			SceneGraphicsComponent() {}

			void setGLWindow(GL::GLWindow *GLWindow)
			{
				m_GLWindow = GLWindow;
			}

			void setImguiInput(ImguiInputComponent *imguiInputComponent)
			{
				m_imguiInputComponenet = imguiInputComponent;
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
