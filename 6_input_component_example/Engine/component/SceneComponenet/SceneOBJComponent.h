#ifndef KATA_COMPONENT_SCENEOBJCOMPONENT_H_
#define KATA_COMPONENT_SCENEOBJCOMPONENT_H_

#include "../../util/Log.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
		class SceneOBJGraphicsComponent : public SceneGraphicsComponent
		{
		private:
			// http://openglbook.com/chapter-2-vertices-and-shapes.html

			GLuint
				VertexShaderId,
				FragmentShaderId,
				ProgramId,
				VaoId,
				VboId,
				ColorBufferId,
				WvpId;

			const GLchar* VertexShader =
			{
				"#version 400\n"\

				"layout(location=0) in vec4 in_Position;\n"\
				"layout(location=1) in vec4 in_Color;\n"\
				"out vec4 ex_Color;\n"\

				"uniform mat4 WVP;\n"\

				"void main(void)\n"\
				"{\n"\
				"  gl_Position = WVP * in_Position;\n"\
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

			typedef struct {
				GLuint vb_id;  // vertex buffer id
				int numTriangles;
				size_t material_id;
			} DrawObject;

			std::vector<DrawObject> gDrawObjects;

			std::string base_dir = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//";
			std::string inputfile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//cube.obj";
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::map<std::string, GLuint> textures;
			std::string err;

			std::vector<glm::vec4> bufferV;
			std::vector<glm::vec4> bufferC;


			void CreateVBO(void)
			{
				GLenum ErrorCheckValue = glGetError();
			
				for (size_t s = 0; s < shapes.size(); s++)
				{
					for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
					{
						tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
						tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
						tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

						glm::vec4 v[3];
						glm::vec4 c[3];
						for (int k = 0; k < 3; k++)
						{
							int f0 = idx0.vertex_index;
							int f1 = idx1.vertex_index;
							int f2 = idx2.vertex_index;

							v[0][k] = attrib.vertices[3 * f0 + k];
							v[1][k] = attrib.vertices[3 * f1 + k];
							v[2][k] = attrib.vertices[3 * f2 + k];
							v[k][3] = 1.0f;
							
							c[0][k] = attrib.vertices[3 * f0 + k];
							c[1][k] = attrib.vertices[3 * f1 + k];
							c[2][k] = attrib.vertices[3 * f2 + k];
							c[k][3] = 1.0f;
						}

						for (int k = 0; k < 3; k++)
						{
							bufferV.push_back(v[k]);
							bufferC.push_back(c[k]);
						}
					}
				}

				glGenBuffers(1, &VboId);
				glBindBuffer(GL_ARRAY_BUFFER, VboId);
				glBufferData(GL_ARRAY_BUFFER, bufferV.size() * sizeof(glm::vec4), &bufferV[0], GL_STATIC_DRAW);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glGenBuffers(1, &ColorBufferId);
				glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
				glBufferData(GL_ARRAY_BUFFER, bufferC.size() * sizeof(glm::vec4), &bufferC[0], GL_STATIC_DRAW);
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

				WvpId = glGetUniformLocation(ProgramId, "WVP");

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

			void Cleanup(void)
			{
				DestroyShaders();
				DestroyVBO();
			}

		public:
			SceneOBJGraphicsComponent() {}

			void setup()
			{
				if (!m_GLWindow)
				{
					assert("check init order");
				}
				
				// load OBJ
				bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str(), base_dir.c_str());
				if (!err.empty())
				{
					KATA_CONSOLE_ERROR(err.c_str());
				}
				if (!ret) assert(true);
				KATA_CONSOLE_INFO("# of vertices  = {}\n", (int)(attrib.vertices.size()) / 3);
				KATA_CONSOLE_INFO("# of normals   = {}\n", (int)(attrib.normals.size()) / 3);
				KATA_CONSOLE_INFO("# of texcoords = {}\n", (int)(attrib.texcoords.size()) / 2);
				KATA_CONSOLE_INFO("# of materials = {}\n", (int)materials.size());
				KATA_CONSOLE_INFO("# of shapes    = {}\n", (int)shapes.size());

				glfwMakeContextCurrent(m_GLWindow->m_window);
				if (!isSingleWindow) glfwHideWindow(m_GLWindow->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
				glEnable(GL_DEPTH_TEST);

				CreateShaders();
				CreateVBO();
			}

			void render(const glm::mat4 &WVP)
			{
				if (!m_imguiInputComponenet)
				{
					assert("check init order");
				}
				glfwMakeContextCurrent(m_GLWindow->m_window);

				// clear buffer
				glClearColor(
					m_imguiInputComponenet->getVec4Clear()->x,
					m_imguiInputComponenet->getVec4Clear()->y,
					m_imguiInputComponenet->getVec4Clear()->z,
					m_imguiInputComponenet->getVec4Clear()->w
				);
				if (!isSingleWindow) glClear(GL_COLOR_BUFFER_BIT);

				// WVP
				glUniformMatrix4fv(WvpId, 1, GL_FALSE, &WVP[0][0]);

				// draw
				glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 0, bufferV.size());

				if (!isSingleWindow) glfwSwapBuffers(m_GLWindow->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEOBJCOMPONENT_H_
