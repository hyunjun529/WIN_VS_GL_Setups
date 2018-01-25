#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include <stdio.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(1280, 720, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	if (gl3wInit()) {
		printf("failed to initialize OpenGL\n");
		return -1;
	}
	glfwSwapInterval(1);
	
	// Setup ImGui
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsClassic();

	// params
	bool show_demo_window = false;
	float param_speed_spin = 1.0f;
	float param_resize = 1.0f;
	ImVec4 param_color_clear = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 param_color_vertex_r = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	ImVec4 param_color_vertex_g = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
	ImVec4 param_color_vertex_b = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		mat4x4 m, p, mvp;
		
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClearColor(param_color_clear.x, param_color_clear.y, param_color_clear.z, param_color_clear.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// render ImGui
		ImGui_ImplGlfwGL3_NewFrame();

		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::Begin("example control window");
			ImGui::Text("Hell world!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if (ImGui::Button("Demo Window")) show_demo_window ^= 1;
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::SliderFloat("spin", &param_speed_spin, -2.0f, 2.0f);
			ImGui::SliderFloat("size", &param_resize, 0.0f, 2.0f);
			ImGui::ColorEdit3("clear color", (float*)&param_color_clear);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::ColorEdit3("vertex r", (float*)&param_color_vertex_r);
			ImGui::ColorEdit3("vertex g", (float*)&param_color_vertex_g);
			ImGui::ColorEdit3("vertex b", (float*)&param_color_vertex_b);
			ImGui::End();
		}
		if (show_demo_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		ImGui::Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplGlfwGL3_Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}