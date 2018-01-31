#include <stdio.h>
#include <malloc.h>

#include <GL/gl3w.h> // 순서에 주의
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h> // <>와 ""의 차이점

#include <linmath.h> // 그래픽스 하면서 수학 라이브러리가 따로 있는 이유


static struct
{
	float x, y;
	float r, g, b;
} vertices[3] =
{
	{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
	{  0.6f, -0.4f, 0.f, 1.f, 0.f },
	{  0.f,   0.6f, 0.f, 0.f, 1.f }
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


// 원래 이러면 안됨, 편의상 전역에 올려둠
bool show_demo_window = false;
bool show_hidden = false;
float param_speed_spin = 1.0f;
float param_resize = 1.0f;
ImVec4 param_color_clear = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
ImVec4 param_color_vertex_r = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
ImVec4 param_color_vertex_g = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
ImVec4 param_color_vertex_b = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);

static GLuint texture_id = 0;
GLuint *data = (GLuint*)malloc(400 * 400 * 4 * sizeof(GLuint));

// gl3w는 한 번만 초기화해야함
// 이게 하는 역할에 대해서 다시 정리해야함
bool is_Init_gl3w = false;


class GLWindow {
private:
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	int windowW = 0;
	int windowH = 0;

public:
	GLFWwindow *window; // 원래는 이러면 안됨, 원리 설명하려고 해뒀다
	int spin = -1;

	GLWindow(int _w, int _h)
	: windowW(_w), windowH(_h){
		window = glfwCreateWindow(windowW, windowH, "OpenGL Version Test with imgui", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			// assert?
		}
		glfwMakeContextCurrent(window);
		if (gl3wInit() && !is_Init_gl3w) { // 나중에 한 번만 처리할 수 있도록 따로 묶어야함
			fprintf(stderr, "failed to initialize OpenGL\n");
			// assert?
			is_Init_gl3w = true;
		}
	}
	const int getWindowWidth() {
		return windowW;
	}
	const int getWindowHeight() {
		return windowH;
	}

	void setup() {
		glfwMakeContextCurrent(window);
		// 버택스 바인딩은 따로 때야함
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		// 여기서부터 쉐이더 프로그램
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
	}

	void render() {
		float ratio;
		int width, height;
		mat4x4 m, p, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClearColor(param_color_clear.x, param_color_clear.y, param_color_clear.z, param_color_clear.w);
		glClear(GL_COLOR_BUFFER_BIT);

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
		mat4x4_rotate_Z(m, m, (float)glfwGetTime() * param_speed_spin * spin);
		mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_scale_aniso(p, p, param_resize, param_resize, param_resize);
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(0);
	}
};

int main(int, char**)
{
	if (!glfwInit()) return 1;

	GLWindow *window_main = new GLWindow(1280, 720);
	GLWindow *window_hidden = new GLWindow(400, 400);

	window_main->setup();
	glfwMakeContextCurrent(window_main->window);
	ImGui_ImplGlfwGL3_Init(window_main->window, true);
	ImGui::StyleColorsClassic();
	glGenTextures(1, &texture_id);

	window_hidden->setup();
	glfwMakeContextCurrent(window_hidden->window);
	window_hidden->spin = 1;
	glfwHideWindow(window_hidden->window);

	while (!glfwWindowShouldClose(window_main->window))
	{
		glfwMakeContextCurrent(window_hidden->window);
		glfwPollEvents();
		window_hidden->render();
		glReadPixels(0, 0,
			window_hidden->getWindowWidth(), window_hidden->getWindowHeight(),
			GL_RGBA, GL_UNSIGNED_INT, data);
		glfwSwapBuffers(window_hidden->window);

		glfwMakeContextCurrent(window_main->window);
		glfwPollEvents();
		window_main->render();
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::Begin("example control window");
			ImGui::Text("Hell world!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if (ImGui::Button("Demo Window")) show_demo_window ^= 1;

			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			ImGui::SliderFloat("spin", &param_speed_spin, -2.0f, 2.0f);
			ImGui::SliderFloat("size", &param_resize, 0.0f, 2.0f);
			ImGui::ColorEdit3("clear color", (float*)&param_color_clear);

			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			ImGui::ColorEdit3("vertex r", (float*)&param_color_vertex_r);
			ImGui::ColorEdit3("vertex g", (float*)&param_color_vertex_g);
			ImGui::ColorEdit3("vertex b", (float*)&param_color_vertex_b);

			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			if (ImGui::Button("Show Hidden")) show_hidden ^= 1;

			ImGui::Spacing();

			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 400, 400, 0, GL_RGBA, GL_UNSIGNED_INT, data);
			ImGui::Image((ImTextureID)texture_id,
				ImVec2(400, 400),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			ImGui::End();
		}
		if (show_demo_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		if (show_hidden) {
			if (!glfwGetWindowAttrib(window_hidden->window, GLFW_VISIBLE)) {
				glfwShowWindow(window_hidden->window);
			}
			else {
				glfwHideWindow(window_hidden->window);
			}
			show_hidden = false;
		}
		ImGui::Render();
		glfwSwapBuffers(window_main->window);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	glfwDestroyWindow(window_main->window);
	glfwDestroyWindow(window_hidden->window);
	glfwTerminate();
	return 0;
}