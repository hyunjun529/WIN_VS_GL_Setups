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

			const GLfloat g_vertex_buffer_data[3][3] = {
				-1.0f, -1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,
				 0.0f,  1.0f, 0.0f,
			};

			const char *vert = "#version 330 core\n"
				"\n"
				"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
				"\n"
				"void main(){\n"
				"    gl_Position.xyz = vertexPosition_modelspace;\n"
				"    gl_Position.w = 1.0;\n"
				"}\n";

			const char *frag = "#version 330 core\n"
				"out vec3 color;\n"
				"void main(){\n"
				"  color = vec3(1,0,0);\n"
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
				// glfwHideWindow(m_GLWindow->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

				glGenVertexArrays(1, &VertexArrayID);
				glBindVertexArray(VertexArrayID);

				// �̰��� �츮�� ���ؽ� ���۸� ����ŵ�ϴ�.
				// GLuint vertexbuffer;
				// ���۸� �ϳ� �����մϴ�. vertexbuffer �� ��� �ĺ��ڸ� �ֽ��ϴ�
				glGenBuffers(1, &vertexbuffer);
				// �Ʒ��� ��ɾ���� �츮�� "vertexbuffer" ���ۿ� ���ؼ� �ٷ�̴ϴ�
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				// �츮�� ���ؽ����� OpenGL�� �Ѱ��ݴϴ�
				glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

				// ������ ù��° �Ӽ���(attribute) : ���ؽ���
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				glVertexAttribPointer(
					0,                  // 0��° �Ӽ�(attribute). 0 �� �� Ư���� ������ ������, ���̴��� ���̾ƿ�(layout)�� �ݵ�� ���߾�� �մϴ�.
					3,                  // ũ��(size)
					GL_FLOAT,           // Ÿ��(type)
					GL_FALSE,           // ����ȭ(normalized)?
					0,                  // ���� ��� ���� ����(stride)
					(void*)0            // �迭 ������ ������(offset; �ű�� ��)
				);
				glEnableVertexAttribArray(0);

				// ���̴��� ����
				GLuint VertexShaderID, FragmentShaderID;
				VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
				FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

				// ���ؽ� ���̴� �ڵ带 ���Ͽ��� �б�
				// �����׸�Ʈ ���̴� �ڵ带 ���Ͽ��� �б�
				// GL32�� ���������� �������� �׳� const char*�� �����ص���
				GLint Result = GL_FALSE;
				int InfoLogLength;

				// ���ؽ� ���̴��� ������
				char const * VertexSourcePointer = vert;
				glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
				glCompileShader(VertexShaderID);

				// ���ؽ� ���̴��� �˻�
				glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
				glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
					glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
					printf("%s\n", &VertexShaderErrorMessage[0]);
				}

				// �����׸�Ʈ ���̴��� ������
				char const * FragmentSourcePointer = frag;
				glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
				glCompileShader(FragmentShaderID);

				// �����׸�Ʈ ���̴��� �˻�
				glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
				glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
					glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
					printf("%s\n", &FragmentShaderErrorMessage[0]);
				}

				// ���α׷��� ��ũ
				printf("Linking program\n");
				GLuint ProgramID = glCreateProgram();
				glAttachShader(ProgramID, VertexShaderID);
				glAttachShader(ProgramID, FragmentShaderID);
				glLinkProgram(ProgramID);

				// ���α׷� �˻�
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
			}

			void render()
			{
				if (!m_imguiInputComponenet)
				{
					assert("check init order");
				}
				glfwMakeContextCurrent(m_GLWindow->m_window);

				// �츮�� ���̴��� ����ϱ�
				glUseProgram(programID);

				glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// �ﰢ�� �׸���!
				glDrawArrays(GL_TRIANGLES, 0, 3); // ���ؽ� 0���� �����ؼ�; �� 3���� ���ؽ��� -> �ϳ��� �ﰢ��

				glfwSwapBuffers(m_GLWindow->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEGL33COMPONENT_H_
