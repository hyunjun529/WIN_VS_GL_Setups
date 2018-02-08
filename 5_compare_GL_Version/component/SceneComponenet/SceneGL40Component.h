#ifndef KATA_COMPONENT_SCENEGL40COMPONENT_H_
#define KATA_COMPONENT_SCENEGL40COMPONENT_H_

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
		class SceneGL40GraphicsComponent : public SceneGraphicsComponent
		{
		private:
			// http://openglbook.com/chapter-2-vertices-and-shapes.html

			GLuint
				VertexShaderId,
				FragmentShaderId,
				ProgramId,
				VaoId,
				VboId,
				ColorBufferId;
			
			const GLchar* VertexShader =
			{
				"#version 400\n"\

				"layout(location=0) in vec4 in_Position;\n"\
				"layout(location=1) in vec4 in_Color;\n"\
				"out vec4 ex_Color;\n"\

				"void main(void)\n"\
				"{\n"\
				"  gl_Position = in_Position;\n"\
				"  ex_Color = in_Color;\n"\
				"}\n"
			};

			const GLchar* FragmentShader =
			{
				"#version 400\n"\

				"in vec4 ex_Color;\n"\
				"out vec4 out_Color;\n"\

				"void main(void)\n"\
				"{\n"\
				"  out_Color = ex_Color;\n"\
				"}\n"
			};

			void Cleanup(void)
			{
				DestroyShaders();
				DestroyVBO();
			}

			void CreateVBO(void)
			{
				GLfloat Vertices[] = {
					-0.8f, -0.8f, 0.0f, 1.0f,
					0.0f,  0.8f, 0.0f, 1.0f,
					0.8f, -0.8f, 0.0f, 1.0f
				};

				GLfloat Colors[] = {
					1.0f, 0.0f, 0.0f, 1.0f,
					0.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f
				};

				GLenum ErrorCheckValue = glGetError();

				glGenVertexArrays(1, &VaoId);
				glBindVertexArray(VaoId);

				glGenBuffers(1, &VboId);
				glBindBuffer(GL_ARRAY_BUFFER, VboId);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glGenBuffers(1, &ColorBufferId);
				glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				ErrorCheckValue = glGetError();
				if (ErrorCheckValue != GL_NO_ERROR)
				{
					assert("errer");
				}
			}

			void DestroyVBO(void)
			{
				GLenum ErrorCheckValue = glGetError();

				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDeleteBuffers(1, &ColorBufferId);
				glDeleteBuffers(1, &VboId);

				glBindVertexArray(0);
				glDeleteVertexArrays(1, &VaoId);

				ErrorCheckValue = glGetError();
				if (ErrorCheckValue != GL_NO_ERROR)
				{
					assert("errer");
				}
			}

			void CreateShaders(void)
			{
				GLenum ErrorCheckValue = glGetError();

				VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
				glCompileShader(VertexShaderId);

				FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
				glCompileShader(FragmentShaderId);

				ProgramId = glCreateProgram();
				glAttachShader(ProgramId, VertexShaderId);
				glAttachShader(ProgramId, FragmentShaderId);
				glLinkProgram(ProgramId);
				glUseProgram(ProgramId);

				ErrorCheckValue = glGetError();
				if (ErrorCheckValue != GL_NO_ERROR)
				{
					assert("errer");
				}
			}

			void DestroyShaders(void)
			{
				GLenum ErrorCheckValue = glGetError();

				glUseProgram(0);

				glDetachShader(ProgramId, VertexShaderId);
				glDetachShader(ProgramId, FragmentShaderId);

				glDeleteShader(FragmentShaderId);
				glDeleteShader(VertexShaderId);

				glDeleteProgram(ProgramId);

				ErrorCheckValue = glGetError();
				if (ErrorCheckValue != GL_NO_ERROR)
				{
					assert("errer");
				}
			}

		public:
			SceneGL40GraphicsComponent() {}

			void setup()
			{
				if (!m_GLWindow)
				{
					assert("check init order");
				}

				glfwMakeContextCurrent(m_GLWindow->m_window);
				// glfwHideWindow(m_GLWindow->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

				CreateShaders();
				CreateVBO();
			}

			void render()
			{
				if (!m_imguiInputComponenet)
				{
					assert("check init order");
				}
				glfwMakeContextCurrent(m_GLWindow->m_window);

				glDrawArrays(GL_TRIANGLES, 0, 3);

				glfwSwapBuffers(m_GLWindow->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEGL40COMPONENT_H_
