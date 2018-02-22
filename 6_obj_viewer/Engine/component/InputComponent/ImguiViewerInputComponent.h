#ifndef KATA_COMPONENT_IMGUICOMPONENT_H_
#define KATA_COMPONENT_IMGUICOMPONENT_H_

#include "../Component.h"
#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>


namespace kata
{
	namespace component
	{
		class ImguiInputComponent : public InputComponent
		{
		private:
			

		public:
			int spin = -1;

			bool show_demo_window = false;
			bool funcReset = false;
			bool funcLoad = false;

			float param_speed_spin = 1.0f;
			float param_resize = 1.0f;
			ImVec4 param_color_clear = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImVec4 param_color_vertex_r = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			ImVec4 param_color_vertex_g = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
			ImVec4 param_color_vertex_b = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);

			ImguiInputComponent()
			{

			}

			void render()
			{
				// need check ImGui context is exist

				ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
				ImGui::Begin("contorller");
				ImGui::Text("Hell world!");
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				if (ImGui::Button("Demo Window")) show_demo_window ^= 1;
				
				ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

				ImGui::Text("reset or load .obj file");
				if (ImGui::Button("reset")) funcReset = true;
				if (ImGui::Button("load")) funcLoad = true;

				ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

				ImGui::SliderFloat("spin", &param_speed_spin, -2.0f, 2.0f);
				ImGui::SliderFloat("size", &param_resize, 0.0f, 2.0f);
				ImGui::ColorEdit3("clear color", (float*)&param_color_clear);

				ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

				ImGui::ColorEdit3("vertex r", (float*)&param_color_vertex_r);
				ImGui::ColorEdit3("vertex g", (float*)&param_color_vertex_g);
				ImGui::ColorEdit3("vertex b", (float*)&param_color_vertex_b);

				if (show_demo_window)
				{
					ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
					ImGui::ShowDemoWindow(&show_demo_window);
				}

				ImGui::End();
			}

			const ImVec4* getVec4Clear() {
				return &param_color_clear;
			}
		};
	}
}

#endif // KATA_COMPONENT_IMGUICOMPONENT_H_
