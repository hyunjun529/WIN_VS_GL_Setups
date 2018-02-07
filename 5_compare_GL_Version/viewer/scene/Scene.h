#ifndef KATA_SCENE_SCENE_H_
#define KATA_SCENE_SCENE_H_

#include "../../GL/GLWindow.h"

#include "../../component/ImguiComponent/ImguiComponent.h"
#include "../../component/SceneComponenet/SceneComponent.h"


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
			}

			void reset()
			{

			}

			void setup()
			{
				m_graphics->setup();
			}
		};
	}
}

#endif // KATA_SCENE_SCENE_H_