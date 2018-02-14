#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <linmath.h>

#include "GL/GLWindow.h"

#include "component/ImguiComponent/ImguiComponent.h"

#include "component/SceneComponenet/SceneGL40Component.h"


int main(int argc, char** argv)
{
	if (!glfwInit()) assert("failed glfwinit");

	kata::GL::GLWindow *glWindow = new kata::GL::GLWindow();

	GLFWwindow *window = glWindow->m_window;

	if (!window)
	{
		glfwTerminate();
		assert("failed create glfw");
	}

	glfwMakeContextCurrent(window);

	if (gl3wInit())
	{
		assert("failed gl3winit");
	}

	kata::component::ImguiInputComponent *imguiComponent
		= new kata::component::ImguiInputComponent;

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsClassic();

	kata::component::SceneGL40GraphicsComponent *scene 
		= new kata::component::SceneGL40GraphicsComponent();

	scene->onSingleWindowMode();
	scene->setGLWindow(glWindow);
	scene->setImguiInput(imguiComponent);

	scene->setup();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		scene->render();

		glfwMakeContextCurrent(window);
		ImGui_ImplGlfwGL3_NewFrame();
		imguiComponent->render();
		ImGui::Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}