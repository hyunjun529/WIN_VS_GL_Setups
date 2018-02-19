#include "util/Log.h"

#include <assert.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include "GL/GLWindow.h"

#include "component/GLComponent/GLCameraInputComponent.h"

#include "component/ImguiComponent/ImguiComponent.h"

#include "component/SceneComponenet/SceneGL40Component.h"
#include "component/SceneComponenet/SceneOBJComponent.h"


kata::component::GLCameraInputComponent *g_camera;

void OnScrollStub(GLFWwindow * window, double offsetx, double offsety)
{
	ImGui_ImplGlfwGL3_ScrollCallback(window, offsetx, offsety);

	glfwGetWindowUserPointer(window);

	g_camera->OnScroll(offsetx, offsety);
}

void OnMouseButtonStub(GLFWwindow * window, int button, int action, int mods)
{
	ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);

	glfwGetWindowUserPointer(window);

	g_camera->OnMouseButton(window, button, action, mods);
}

int main(int argc, char** argv)
{
	if (!glfwInit()) assert("failed glfwinit");

	kata::GL::GLWindow *glWindow = new kata::GL::GLWindow(800, 800);
	glWindow->updateWindowTitle("Single WIndow Mode");

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

	kata::component::SceneOBJGraphicsComponent *scene
		= new kata::component::SceneOBJGraphicsComponent();

	scene->onSingleWindowMode();
	scene->setGLWindow(glWindow);
	scene->setImguiInput(imguiComponent);

	scene->setup();

	kata::component::GLCameraInputComponent *glCameraInputComponent
		= new kata::component::GLCameraInputComponent();
	g_camera = glCameraInputComponent;
	glCameraInputComponent->setWindow(window);

	glfwSetMouseButtonCallback(window, OnMouseButtonStub);
	glfwSetScrollCallback(window, OnScrollStub);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glCameraInputComponent->update();

		scene->render(glCameraInputComponent->getWVP());

		glfwMakeContextCurrent(window);
		ImGui_ImplGlfwGL3_NewFrame();
		imguiComponent->render();
		glCameraInputComponent->renderImGui();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}