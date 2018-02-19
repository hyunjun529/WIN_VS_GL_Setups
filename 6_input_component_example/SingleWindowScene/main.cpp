#include "util/Log.h"

#include "render/World.h"

#include "component/InputComponent/CameraInputComponent.h"

#include "component/RenderComponenet/OBJRenderComponent.h"


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

	kata::render::World *world = new kata::render::World(800, 800);
	world->updateWindowTitle("Single WIndow Mode");

	GLFWwindow *window = world->m_window;

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

	kata::component::OBJRenderComponent *objRender
		= new kata::component::OBJRenderComponent();

	objRender->onSingleWindowMode();
	objRender->setGLWindow(world);
	objRender->setImguiInput(imguiComponent);

	objRender->setup();

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

		objRender->render(glCameraInputComponent->getWVP());

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