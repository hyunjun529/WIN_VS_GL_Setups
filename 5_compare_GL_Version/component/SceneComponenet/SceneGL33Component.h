#ifndef KATA_COMPONENT_SCENEGL33COMPONENT_H_
#define KATA_COMPONENT_SCENEGL33COMPONENT_H_

#include <string>
#include <vector>

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
		class SceneGL33GraphicsComponent : public SceneGraphicsComponent
		{
		private:
			// http://www.opengl-tutorial.org/kr/beginners-tutorials/tutorial-2-the-first-triangle/

			GLuint VertexArrayID;
			GLuint vertexbuffer;
			GLuint programID;

			GLfloat g_vertex_buffer_data[3][3] = {
				-0.6f, -0.4f, 1.0f,
				 0.6f, -0.4f, 0.0f,
				 0.0f,  0.6f, 0.0f
			};

			const char *vert = "#version 330 core\n"
				"\n"
				"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
				"\n"
				"void main(){\n"
				"    gl_Position.xyz = vertexPosition_modelspace;\n"
				"    gl_Position.w = 1.0;\n"
				"}\n";

			//const char *frag = "#version 330 core\n"
			//	"out vec3 color;\n"
			//	"void main(){\n"
			//	"  color = vec3(1,0,0);\n"
			//	"}\n";
			const char *frag = "#version 330 core\n"
				"out vec4 color;\n"
				"void main(){\n"
				"  color = vec4(1,0,0,1);\n"
				"}\n";

		public:
			SceneGL33GraphicsComponent() {}

			void setup()
			{
				if (!m_GLWindow)
				{
					assert("check init order");
				}

				glfwMakeContextCurrent(m_GLWindow->m_window);
				glfwHideWindow(m_GLWindow->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

				glGenVertexArrays(1, &VertexArrayID);
				glBindVertexArray(VertexArrayID);

				glGenBuffers(1, &vertexbuffer);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(0);

				GLuint VertexShaderID, FragmentShaderID;
				VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
				FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

				GLint Result = GL_FALSE;
				int InfoLogLength;

				// 버텍스 쉐이더를 컴파일
				char const * VertexSourcePointer = vert;
				glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
				glCompileShader(VertexShaderID);

				// 버텍스 쉐이더를 검사
				glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
				glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
					glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
					printf("%s\n", &VertexShaderErrorMessage[0]);
				}

				// 프래그먼트 쉐이더를 컴파일
				char const * FragmentSourcePointer = frag;
				glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
				glCompileShader(FragmentShaderID);

				// 프래그먼트 쉐이더를 검사
				glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
				glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
					glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
					printf("%s\n", &FragmentShaderErrorMessage[0]);
				}

				// 프로그램에 링크
				printf("Linking program\n");
				GLuint ProgramID = glCreateProgram();
				glAttachShader(ProgramID, VertexShaderID);
				glAttachShader(ProgramID, FragmentShaderID);
				glLinkProgram(ProgramID);

				// 프로그램 검사
				glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
				glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
					glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
					printf("%s\n", &ProgramErrorMessage[0]);
				}

				glDetachShader(ProgramID, VertexShaderID);
				glDetachShader(ProgramID, FragmentShaderID);

				glDeleteShader(VertexShaderID);
				glDeleteShader(FragmentShaderID);

				programID = ProgramID;

				glUseProgram(programID);
			}

			void render()
			{
				if (!m_imguiInputComponenet)
				{
					assert("check init order");
				}
				glfwMakeContextCurrent(m_GLWindow->m_window);
				
				glClear(GL_COLOR_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

				glDrawArrays(GL_TRIANGLES, 0, 3);

				glfwSwapBuffers(m_GLWindow->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEGL33COMPONENT_H_
