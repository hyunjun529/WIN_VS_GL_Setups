#ifndef KATA_COMPONENT_SCENEOBJCOMPONENT_H_
#define KATA_COMPONENT_SCENEOBJCOMPONENT_H_

#include "../../util/Log.h"

#include "tiny_obj_loader.h"

#include "stb_image.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../render/World.h"

#include "../Component.h"
#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

#include "../InputComponent/ImguiViewerInputComponent.h"

#include "RenderComponent.h"


namespace kata
{
	namespace component
	{
		class OBJRenderComponent : public RenderComponent
		{
		private:
			// http://www.opengl-tutorial.org/kr/beginners-tutorials/tutorial-7-model-loading/

			component::ImguiInputComponent *m_inputImgui = nullptr;

			GLuint
				VboVId,
				VboUVId;

			GLuint
				TextureId,
				WvpId;

			GLuint
				VertexShaderId,
				FragmentShaderId,
				ProgramId;

			const GLchar *VertexShader =
			{
				"#version 400 core\n"\

				"layout(location = 0) in vec4 vertexPosition;\n"\

				"layout(location = 1) in vec2 vertexUV;\n"\

				"out vec2 UV;\n"\

				"uniform mat4 WVP;\n"\

				"void main(){\n"\

				"	gl_Position =  WVP * vertexPosition;\n"\

				"	UV = vertexUV;\n"\

				"}\n"
			};

			const GLchar *FragmentShader =
			{
				"#version 400 core\n"\

				"in vec2 UV;\n"\

				"out vec4 color;\n"\

				"uniform sampler2D myTextureSampler;\n"\

				"void main(){\n"\

				"	color = vec4(texture( myTextureSampler, UV ).rgb, 1.0f);\n"\
				"	//color = vec4(0.6f, 0.0f, 0.8f, 1.0f);\n"\

				"}\n"
			};

			typedef struct {
				GLuint vb_id;
				int numTriangles;
				size_t material_id;
			} DrawObject;

			std::vector<DrawObject> gDrawObjects;

			std::string base_dir = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//";
			std::string inputfile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//cube.obj";
			std::string texturefile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//default.png";
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::map<std::string, GLuint> textures;
			std::string err;

			std::vector<glm::vec4> bufferV;
			std::vector<glm::vec2> bufferUV;

			void CreateVBO(void)
			{
				glfwMakeContextCurrent(m_world->m_window);

				GLenum ErrorCheckValue = glGetError();
			
				// load obj info
				for (size_t s = 0; s < shapes.size(); s++)
				{
					for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
					{
						tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
						tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
						tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

						glm::vec4 v[3];
						for (int k = 0; k < 3; k++)
						{
							int f0 = idx0.vertex_index;
							int f1 = idx1.vertex_index;
							int f2 = idx2.vertex_index;

							v[0][k] = attrib.vertices[3 * f0 + k];
							v[1][k] = attrib.vertices[3 * f1 + k];
							v[2][k] = attrib.vertices[3 * f2 + k];
							v[k][3] = 1.0f;
						}

						glm::vec2 tc[3];
						if (attrib.texcoords.size() > 0) {
							if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
								(idx2.texcoord_index < 0)) {
								// face does not contain valid uv index.
								tc[0][0] = 0.0f;
								tc[0][1] = 0.0f;
								tc[1][0] = 0.0f;
								tc[1][1] = 0.0f;
								tc[2][0] = 0.0f;
								tc[2][1] = 0.0f;
							}
							else
							{
								assert(attrib.texcoords.size() >
									size_t(2 * idx0.texcoord_index + 1));
								assert(attrib.texcoords.size() >
									size_t(2 * idx1.texcoord_index + 1));
								assert(attrib.texcoords.size() >
									size_t(2 * idx2.texcoord_index + 1));

								// Flip Y coord.
								tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
								tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
								tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
								tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
								tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
								tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
							}
						}
						else {
							tc[0][0] = 0.0f;
							tc[0][1] = 0.0f;
							tc[1][0] = 0.0f;
							tc[1][1] = 0.0f;
							tc[2][0] = 0.0f;
							tc[2][1] = 0.0f;
						}

						// push buffer
						for (int k = 0; k < 3; k++)
						{
							bufferV.push_back(v[k]);
							bufferUV.push_back(tc[k]);
						}
					}
				}

				int w, h, comp;
				unsigned char* image = stbi_load(texturefile.c_str(), &w, &h, &comp, STBI_default);

				glActiveTexture(GL_TEXTURE0);
				glGenTextures(1, &TextureId);
				glBindTexture(GL_TEXTURE_2D, TextureId);
				if (comp == 3) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
						GL_UNSIGNED_BYTE, image);
				}
				else if (comp == 4) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
						GL_UNSIGNED_BYTE, image);
				}
				else {
					assert(0);  // TODO
				}
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glUniform1i(TextureId, 0);

				// origin function
				glGenBuffers(1, &VboVId);
				glBindBuffer(GL_ARRAY_BUFFER, VboVId);
				glBufferData(GL_ARRAY_BUFFER, bufferV.size() * sizeof(glm::vec4), &bufferV[0], GL_STATIC_DRAW);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glGenBuffers(1, &VboUVId);
				glBindBuffer(GL_ARRAY_BUFFER, VboUVId);
				glBufferData(GL_ARRAY_BUFFER, bufferUV.size() * sizeof(glm::vec2), &bufferUV[0], GL_STATIC_DRAW);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
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
				glDeleteBuffers(1, &VboVId);
				glDeleteBuffers(1, &VboUVId);

				glBindVertexArray(0);

				ErrorCheckValue = glGetError();
				if (ErrorCheckValue != GL_NO_ERROR)
				{
					assert("errer");
				}
			}

			void CreateShaders(void)
			{
				glfwMakeContextCurrent(m_world->m_window);

				GLenum ErrorCheckValue = glGetError();

				//const GLchar *VertexShader = m_world->loadShader("vert");
				//const GLchar *FragmentShader = m_world->loadShader("frag");

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
				TextureId = glGetUniformLocation(ProgramId, "myTextureSampler");

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
			OBJRenderComponent() {}

			~OBJRenderComponent()
			{
				DestroyShaders();
				DestroyVBO();
			}

			void setImguiInputComponent(component::ImguiInputComponent *_inputImgui)
			{
				m_inputImgui = _inputImgui;
			}

			void setup()
			{
				if (!m_world)
				{
					assert("check init order");
				}
				
				// load obj file
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

				if (!isSingleWindow) glfwHideWindow(m_world->m_window);
				
				glfwMakeContextCurrent(m_world->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
				
				glEnable(GL_DEPTH_TEST);

				CreateShaders();
				CreateVBO();
			}

			void render()
			{
				render(glm::mat4x4(1.0));
			}

			void render(const glm::mat4 &WVP)
			{
				if (!m_inputImgui)
				{
					assert("check init order");
				}
				glfwMakeContextCurrent(m_world->m_window);

				// clear buffer
				glClearColor(
					m_inputImgui->getVec4Clear()->x,
					m_inputImgui->getVec4Clear()->y,
					m_inputImgui->getVec4Clear()->z,
					m_inputImgui->getVec4Clear()->w
				);
				if (!isSingleWindow) glClear(GL_COLOR_BUFFER_BIT);

				// WVP
				glUniformMatrix4fv(WvpId, 1, GL_FALSE, &WVP[0][0]);

				// draw
				glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)bufferV.size());

				if (!isSingleWindow) glfwSwapBuffers(m_world->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEOBJCOMPONENT_H_
