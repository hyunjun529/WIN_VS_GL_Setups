#ifndef KATA_SCENE_SCENE_H_
#define KATA_SCENE_SCENE_H_

#include <malloc.h>

#include "render/World.h"

#include "component/InputComponent/ImguiViewerInputComponent.h"
#include "component/RenderComponenet/RenderComponent.h"


namespace kata
{
	namespace scene
	{
		class Scene
		{
		private:
			render::World *m_world = nullptr;
			component::ImguiInputComponent *m_input;
			component::ScenePhysicsComponent *m_physics;
			component::RenderComponent *m_graphics;

			GLuint *m_pixel = (GLuint*)malloc(400 * 400 * 4 * sizeof(GLuint));

		public:
			Scene(component::ImguiInputComponent *input,
				  component::ScenePhysicsComponent *phyiscs,
				  component::RenderComponent *graphics)
				: m_input(input),
				  m_physics(phyiscs),
				  m_graphics(graphics)
			{
				m_world = new render::World();

				m_graphics->setImguiInput(m_input);
				m_graphics->setGLWindow(m_world);
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