#ifndef KATA_VIEWER_VIEWER_H_
#define KATA_VIEWER_VIEWER_H_

#include <memory>
#include <vector>
#include <assert.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <linmath.h>

#include "../util/Log.h"

#include "scene/Scene.h"

#include "../component/InputComponent.h"
//#include "../component/PhysicsComponent.h"
//#include "../component/GraphicsComponent.h"

#include "../component/ImguiComponent/ImguiComponent.h"

namespace kata
{
	class Viewer
	{
	private:
		GLFWwindow *m_window;
		std::vector<std::shared_ptr<scene::Scene>> m_scenes;
		std::list<kata::component::InputComponent*> m_inputComponents;
		//std::list<kata::component::PhysicsComponent*> m_physicsComponents;
		//std::list<kata::component::GraphicsComponent*> m_graphicsComponents;

		int m_windowW = 0;
		int m_windowH = 0;

		
	public:
		Viewer(int _w = 1280, int _h = 720)
			: m_windowW(_w), m_windowH(_h)
		{
			if (!glfwInit()) assert("failed glfwinit"); 
			
			m_window = glfwCreateWindow(m_windowW, m_windowH, "Viewer", NULL, NULL);
			
			if (!m_window)
			{
				glfwTerminate();
				assert("failed create glfw");
			}
			
			glfwMakeContextCurrent(m_window);
			
			if (gl3wInit()) 
			{
				assert("failed gl3winit");
			}
		}

		void reset()
		{
			for (std::shared_ptr<kata::scene::Scene> s : m_scenes) {
				s->reset();
			}
		}

		void setup()
		{
			reset();
			for (std::shared_ptr<kata::scene::Scene> s : m_scenes) {
				s->setup();
			}
		}

		void run()
		{
			setup();

			while (!glfwWindowShouldClose(m_window))
			{
				glfwMakeContextCurrent(m_window);
				glfwPollEvents();

				for (std::shared_ptr<kata::scene::Scene> s : m_scenes) {
					s->render();
				}

				glfwMakeContextCurrent(m_window);
				glfwSwapBuffers(m_window);
			}
		}

		void addScene()
		{
			kata::component::InputComponent *t_input 
				= new kata::component::ImguiInputComponent();
			//kata::component::PhysicsComponent *t_physics
			//	= new kata::component::ImguiPhysicsComponent();
			//kata::component::GraphicsComponent *t_graphics
			//	= new kata::component::ImguiGraphicsComponent();

			std::shared_ptr<kata::scene::Scene> tmp_scene
				= std::make_shared<kata::scene::Scene>();
					//t_input, t_physics, t_graphics);

			m_inputComponents.push_back(t_input);
			//m_physicsComponents.push_back(t_physics);
			//m_graphicsComponents.push_back(t_graphics);

			m_scenes.push_back(tmp_scene);

			CONSOLE_INFO("add new scene");
		}
	};
}

#endif // KATA_VIEWER_VIEWER_H_