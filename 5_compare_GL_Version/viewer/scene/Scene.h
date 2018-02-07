#ifndef KATA_SCENE_SCENE_H_
#define KATA_SCENE_SCENE_H_

#include <malloc.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <linmath.h>

#include "../../GL/GLWindow.h"

#include "../../component/InputComponent.h"
//#include "../../component/PhysicsComponent.h"
//#include "../../component/GraphicsComponent.h"


namespace kata
{
	namespace scene
	{
		class Scene
		{
		private:
			kata::GL::GLWindow *m_GLWindow;

			//kata::component::InputComponent *m_input;
			//kata::component::PhysicsComponent *m_physics;
			//kata::component::GraphicsComponent *m_graphics;


		public:
			Scene()
			{
				m_GLWindow = new kata::GL::GLWindow();
			}

			//Scene(kata::component::InputComponent *input)
			//	: m_input(input)
			//{
			//	m_GLWindow = new kata::GL::GLWindow();
			//}

			//Scene(kata::component::InputComponent *input,
			//	  kata::component::PhysicsComponent *phyiscs,
			//	  kata::component::GraphicsComponent *graphics)
			//	  : m_input(input),
			//	    m_physics(phyiscs),
			//	    m_graphics(graphics)
			//{
			//	m_GLWindow = new kata::GL::GLWindow();
			//}

			void reset()
			{

			}

			void setup()
			{

			}

			void render()
			{

			}

			void getPixels()
			{

			}
		};
	}
}

#endif // KATA_SCENE_SCENE_H_