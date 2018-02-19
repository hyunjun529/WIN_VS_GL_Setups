#include "util/Log.h"

#include <assert.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include "GL/GLWindow.h"

#include "component/ImguiComponent/ImguiComponent.h"

#include "component/SceneComponenet/SceneGL40Component.h"
#include "component/SceneComponenet/SceneOBJComponent.h"

#include "camera.h"

Camera *camera = new Camera();

enum class CameraMode
{
	None,
	Orbit,
	Dolly,
	Pan,
};
CameraMode m_cameraMode;

enum class MouseLockMode
{
	None,
	RequestLock,
	Lock,
};
MouseLockMode m_mouseLockMode;

double m_dx, m_dy;
double m_prevX, m_prevY;
double m_scrollX, m_scrollY;

void OnScroll(double offsetx, double offsety)
{
	m_scrollX = offsetx;
	m_scrollY = offsety;
}

void OnScrollStub(GLFWwindow * window, double offsetx, double offsety)
{
	ImGui_ImplGlfwGL3_ScrollCallback(window, offsetx, offsety);

	glfwGetWindowUserPointer(window);
	
	OnScroll(offsetx, offsety);
}

void OnMouseButton(GLFWwindow * m_window, int button, int action, int mods)
{
	auto prevCameraMode = m_cameraMode;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		m_cameraMode = CameraMode::Orbit;
		if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			m_cameraMode = CameraMode::Orbit;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS)
		{
			m_cameraMode = CameraMode::Pan;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS)
		{
			m_cameraMode = CameraMode::Dolly;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		m_cameraMode = CameraMode::Dolly;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		m_cameraMode = CameraMode::Pan;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT ||
		button == GLFW_MOUSE_BUTTON_RIGHT ||
		button == GLFW_MOUSE_BUTTON_MIDDLE
		)
	{
		if (action == GLFW_RELEASE)
		{
			m_cameraMode = CameraMode::None;
			m_mouseLockMode = MouseLockMode::None;
		}
	}
	if (prevCameraMode == CameraMode::None && m_cameraMode != CameraMode::None)
	{
		m_mouseLockMode = MouseLockMode::RequestLock;
	}
}

void OnMouseButtonStub(GLFWwindow * window, int button, int action, int mods)
{
	ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);

	glfwGetWindowUserPointer(window);

	OnMouseButton(window, button, action, mods);
}

void updateMouse(GLFWwindow * window)
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);

	double curX, curY;
	glfwGetCursorPos(window, &curX, &curY);

	m_dx = (curX - m_prevX) / (double)w;
	m_dy = (curY - m_prevY) / (double)h;
	m_prevX = curX;
	m_prevY = curY;
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

	camera->Initialize(glm::vec3(0.f, 0.f, 1.f), 0.2f);

	glfwSetMouseButtonCallback(window, OnMouseButtonStub);
	glfwSetScrollCallback(window, OnScrollStub);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// camera
		updateMouse(window);
		if (m_mouseLockMode == MouseLockMode::RequestLock)
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				m_mouseLockMode = MouseLockMode::None;
				m_cameraMode = CameraMode::None;
			}
			else
			{
				m_mouseLockMode = MouseLockMode::Lock;
			}
		}
		if (m_mouseLockMode == MouseLockMode::Lock && m_cameraMode != CameraMode::None)
		{
			if (m_cameraMode == CameraMode::Orbit)
			{
				camera->Orbit((float)m_dx, (float)m_dy);
			}
			if (m_cameraMode == CameraMode::Dolly)
			{
				camera->Dolly((float)m_dx + (float)m_dy);
			}
			if (m_cameraMode == CameraMode::Pan)
			{
				camera->Pan((float)m_dx, (float)m_dy);
			}
		}
		if (m_scrollY != 0 && (!ImGui::GetIO().WantCaptureMouse))
		{
			camera->Dolly((float)m_scrollY * 0.1f);
		}
		m_scrollX = 0;
		m_scrollY = 0;
		camera->UpdateMatrix();

		auto world = glm::mat4(1.0);
		auto view = camera->GetViewMatrix();
		auto proj = camera->GetProjectionMatrix();
		glm::mat4x4 WVP = proj * view * world;
		
		//scene->render();
		scene->render(WVP);

		glfwMakeContextCurrent(window);
		ImGui_ImplGlfwGL3_NewFrame();
		imguiComponent->render();

		ImGui::Begin("WVP");
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ImGui::Text("%d %d : %lf", i, j, WVP[i][j]);
			}
		}
		ImGui::End();

		ImGui::Begin("Mouse");
		ImGui::Text("m_dx = %lf", m_dx);
		ImGui::Text("m_dy = %lf", m_dy);
		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
		ImGui::Text("m_prevX = %lf", m_prevX);
		ImGui::Text("m_prevY = %lf", m_prevY);
		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
		ImGui::Text("m_cameraMode = %d", m_cameraMode);
		ImGui::End();

		ImGui::Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}