#include "util/Log.h"

#include "OBJViewerScene.h"


kata::component::CameraInputComponent *g_camera;

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
	const int singleWindowSizeW = 800;
	const int singleWindowSizeH = 800;

	if (!glfwInit()) assert("failed glfwinit");

	kata::render::World *world
		= new kata::render::World(singleWindowSizeW, singleWindowSizeH);

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

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsClassic();


	kata::component::CameraInputComponent *inputCamera
		= new kata::component::CameraInputComponent();

	kata::component::ImguiInputComponent *inputImgui
		= new kata::component::ImguiInputComponent;

	kata::component::PhysicsComponent *physics
		 = new kata::component::PhysicsComponent();

	kata::component::OBJRenderComponent *renderOBJ
		= new kata::component::OBJRenderComponent();

	kata::scene::OBJViewerScene *scene = new kata::scene::OBJViewerScene(inputCamera, inputImgui, physics, renderOBJ);

	scene->setWorld(world);
	scene->setSceneSize(800, 800);
	scene->setSingleMode(true);
	scene->setup();

	// set Camera on Single Window
	g_camera = inputCamera;
	glfwSetMouseButtonCallback(window, OnMouseButtonStub);
	glfwSetScrollCallback(window, OnScrollStub);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// input
		inputCamera->update();

		// physics
		physics->update();

		// render
		renderOBJ->render(inputCamera->getWVP());

		glfwMakeContextCurrent(window);
		ImGui_ImplGlfwGL3_NewFrame();
		inputImgui->render();
		inputCamera->renderImGui();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplGlfwGL3_Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}