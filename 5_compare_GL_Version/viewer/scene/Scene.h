#ifndef KATA_SCENE_SCENE_H_
#define KATA_SCENE_SCENE_H_

#include <malloc.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <linmath.h>

#include "../../GL/GLWindow.h"

#include "../../component/InputComponent.h"
//#include "../../component/PhysicsComponent.h"
//#include "../../component/GraphicsComponent.h"


namespace kata
{
	namespace scene
	{
		class Scene
		{
		private:
			kata::GL::GLWindow *m_GLWindow;

			//kata::component::InputComponent *m_input;
			//kata::component::PhysicsComponent *m_physics;
			//kata::component::GraphicsComponent *m_graphics;

			struct
			{
				float x, y;
				float r, g, b;
			} vertices[3] =
			{
				{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
				{ 0.6f, -0.4f, 0.f, 1.f, 0.f },
				{ 0.f,   0.6f, 0.f, 0.f, 1.f }
			};

			const char* vertex_shader_text =
				"uniform mat4 MVP;\n"
				"attribute vec3 vCol;\n"
				"attribute vec2 vPos;\n"
				"varying vec3 color;\n"
				"void main()\n"
				"{\n"
				"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
				"    color = vCol;\n"
				"}\n";

			const char* fragment_shader_text =
				"varying vec3 color;\n"
				"void main()\n"
				"{\n"
				"    gl_FragColor = vec4(color, 1.0);\n"
				"}\n";

			GLuint vertex_buffer, vertex_shader, fragment_shader, program;
			GLint mvp_location, vpos_location, vcol_location;

			int spin = -1;

			float param_speed_spin = 1.0f;
			float param_resize = 1.0f;
			ImVec4 param_color_clear = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImVec4 param_color_vertex_r = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			ImVec4 param_color_vertex_g = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
			ImVec4 param_color_vertex_b = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);


		public:
			Scene()
			{
				m_GLWindow = new kata::GL::GLWindow();
			}

			//Scene(kata::component::InputComponent *input)
			//	: m_input(input)
			//{
			//	m_GLWindow = new kata::GL::GLWindow();
			//}

			//Scene(kata::component::InputComponent *input,
			//	  kata::component::PhysicsComponent *phyiscs,
			//	  kata::component::GraphicsComponent *graphics)
			//	  : m_input(input),
			//	    m_physics(phyiscs),
			//	    m_graphics(graphics)
			//{
			//	m_GLWindow = new kata::GL::GLWindow();
			//}

			void reset()
			{

			}

			void setup()
			{
				glfwMakeContextCurrent(m_GLWindow->m_window);
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

			void render()
			{
				float ratio;
				int width, height;
				mat4x4 m, p, mvp;

				glfwMakeContextCurrent(m_GLWindow->m_window);
				glfwGetFramebufferSize(m_GLWindow->m_window, &width, &height);
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

				glfwSwapBuffers(m_GLWindow->m_window);
			}

			void getPixels()
			{

			}
		};
	}
}

#endif // KATA_SCENE_SCENE_H_