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

			ImGui_ImplGlfwGL3_Init(m_window, true);
			ImGui::StyleColorsClassic();
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
			
			bool show_demo_window = false;
			ImVec4 param_color_clear = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			while (!glfwWindowShouldClose(m_window))
			{
				int width, height;
				glfwMakeContextCurrent(m_window);
				glfwGetFramebufferSize(m_window, &width, &height);
				glViewport(0, 0, width, height);
				glClearColor(param_color_clear.x, param_color_clear.y, param_color_clear.z, param_color_clear.w);
				glClear(GL_COLOR_BUFFER_BIT);
				glfwPollEvents();

				for (std::shared_ptr<kata::scene::Scene> s : m_scenes) {
					s->render();
				}

				glfwMakeContextCurrent(m_window);
				ImGui_ImplGlfwGL3_NewFrame();
				{
					ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
					ImGui::Begin("example control window");
					ImGui::Text("Hell world!");
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
						1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					if (ImGui::Button("Demo Window")) show_demo_window ^= 1;
					ImGui::ColorEdit3("clear color", (float*)&param_color_clear);
					if (show_demo_window)
					{
						ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
						ImGui::ShowDemoWindow(&show_demo_window);
					}
					ImGui::End();
				}
				ImGui::Render();
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