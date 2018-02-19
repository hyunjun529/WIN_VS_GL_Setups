#ifndef KATA_SCENE_SCENE_H_
#define KATA_SCENE_SCENE_H_

#include <malloc.h>

#include "GL/GLWindow.h"

#include "component/ImguiComponent/ImguiComponent.h"
#include "component/SceneComponenet/SceneComponent.h"


namespace kata
{
	namespace scene
	{
		class Scene
		{
		private:
			GL::GLWindow *m_GLWindow = nullptr;
			component::ImguiInputComponent *m_input;
			component::ScenePhysicsComponent *m_physics;
			component::SceneGraphicsComponent *m_graphics;

			GLuint *m_pixel = (GLuint*)malloc(400 * 400 * 4 * sizeof(GLuint));

		public:
			Scene(component::ImguiInputComponent *input,
				  component::ScenePhysicsComponent *phyiscs,
				  component::SceneGraphicsComponent *graphics)
				: m_input(input),
				  m_physics(phyiscs),
				  m_graphics(graphics)
			{
				m_GLWindow = new GL::GLWindow();

				m_graphics->setImguiInput(m_input);
				m_graphics->setGLWindow(m_GLWindow);
				m_graphics->setPixel(m_pixel);
			}

			void reset()
			{

			}

			void setup()
			{
				m_graphics->setup();
			}

			GLuint* getPixel()
			{
				return m_pixel;
			}
		};
	}
}

#endif // KATA_SCENE_SCENE_H_