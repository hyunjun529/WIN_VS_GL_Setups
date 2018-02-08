#ifndef KATA_COMPONENT_SCENEGL20COMPONENT_H_
#define KATA_COMPONENT_SCENEGL20COMPONENT_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "../../GL/GLWindow.h"

#include "../Component.h"
#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

#include "../ImguiComponent/ImguiComponent.h"

#include "SceneComponent.h"


namespace kata
{
	namespace component
	{
		class SceneGL20GraphicsComponent : public SceneGraphicsComponent
		{
		private:
			GLuint vertex_buffer, vertex_shader, fragment_shader, program;
			GLint mvp_location, vpos_location, vcol_location;

			struct
			{
				float x, y;
				float r, g, b;
			} vertices[3] =
			{
				{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
				{  0.6f, -0.4f, 0.f, 1.f, 0.f },
				{  0.f,   0.6f, 0.f, 0.f, 1.f }
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

		public:
			SceneGL20GraphicsComponent() {}

			void setup()
			{
				if (!m_GLWindow)
				{
					assert("check init order");
				}

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

				glfwHideWindow(m_GLWindow->m_window);
			}

			void render()
			{
				if (!m_imguiInputComponenet)
				{
					assert("check init order");
				}

				float ratio;
				int width, height;
				mat4x4 m, p, mvp;

				glfwMakeContextCurrent(m_GLWindow->m_window);
				glfwGetFramebufferSize(m_GLWindow->m_window, &width, &height);
				ratio = width / (float)height;
				glViewport(0, 0, width, height);
				glClearColor(
					m_imguiInputComponenet->getVec4Clear()->x,
					m_imguiInputComponenet->getVec4Clear()->y,
					m_imguiInputComponenet->getVec4Clear()->z,
					m_imguiInputComponenet->getVec4Clear()->w
				);
				glClear(GL_COLOR_BUFFER_BIT);

				vertices[0].r = m_imguiInputComponenet->param_color_vertex_r.x;
				vertices[0].g = m_imguiInputComponenet->param_color_vertex_r.y;
				vertices[0].b = m_imguiInputComponenet->param_color_vertex_r.z;
				vertices[1].r = m_imguiInputComponenet->param_color_vertex_g.x;
				vertices[1].g = m_imguiInputComponenet->param_color_vertex_g.y;
				vertices[1].b = m_imguiInputComponenet->param_color_vertex_g.z;
				vertices[2].r = m_imguiInputComponenet->param_color_vertex_b.x;
				vertices[2].g = m_imguiInputComponenet->param_color_vertex_b.y;
				vertices[2].b = m_imguiInputComponenet->param_color_vertex_b.z;
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

				mat4x4_identity(m);
				mat4x4_rotate_Z(m, m, (float)glfwGetTime() * m_imguiInputComponenet->param_speed_spin * m_imguiInputComponenet->spin);
				mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
				mat4x4_scale_aniso(p, p, m_imguiInputComponenet->param_resize, m_imguiInputComponenet->param_resize, m_imguiInputComponenet->param_resize);
				mat4x4_mul(mvp, p, m);

				glUseProgram(program);
				glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glUseProgram(0);

				glfwSwapBuffers(m_GLWindow->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEGL20COMPONENT_H_
