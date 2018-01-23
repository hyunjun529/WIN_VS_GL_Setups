#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include <stdio.h>

#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>

#include "linmath.h"

static struct
{
	float x, y;
	float r, g, b;
} vertices[3] =
{
	{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
	{ 0.6f, -0.4f, 0.f, 1.f, 0.f },
	{ 0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

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

	// Setup Triangle
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	
	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 2));
	
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

		// render Triangle
		vertices[0].r = param_color_vertex_r.x;
		vertices[0].g = param_color_vertex_r.y;
		vertices[0].b = param_color_vertex_r.z;
		vertices[1].r = param_color_vertex_g.x;
		vertices[1].g = param_color_vertex_g.y;
		vertices[1].b = param_color_vertex_g.z;
		vertices[2].r = param_color_vertex_b.x;
		vertices[2].g = param_color_vertex_b.y;
		vertices[2].b = param_color_vertex_b.z;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		mat4x4_identity(m);
		mat4x4_rotate_Z(m, m, (float)glfwGetTime() * -param_speed_spin);
		mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_scale_aniso(p, p, param_resize, param_resize, param_resize);
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
		glDrawArrays(GL_TRIANGLES, 0, 3);

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