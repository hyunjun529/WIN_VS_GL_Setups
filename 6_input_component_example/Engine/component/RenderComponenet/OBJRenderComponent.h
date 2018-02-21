#ifndef KATA_COMPONENT_SCENEOBJCOMPONENT_H_
#define KATA_COMPONENT_SCENEOBJCOMPONENT_H_

#include "../../util/Log.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderComponent.h"

#include "../InputComponent/ImguiViewerInputComponent.h"

#include "../../render/World.h"
#include "../../render/DrawObject.h"
#include "../../render/OBJLoader.h"

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
				MvpId;

			GLuint
				VertexShaderId,
				FragmentShaderId,
				ProgramId;

			GLuint
				cntTirangle = 0;

			std::vector<render::DrawObject> m_drawObjects;


			void CreateVBO(void)
			{
				glfwMakeContextCurrent(m_world->m_window);

				GLenum ErrorCheckValue = glGetError();

				for (int k = 0; k < m_drawObjects.size(); k++)
				{
					render::DrawObject *o = &m_drawObjects[k];
					// texture
					for (int i = 0; i < o->subMeshs.size(); i++)
					{
						render::DrawObject::SubMesh *sm = &o->subMeshs[i];
						if (sm->textureId == 0)
						{
							glActiveTexture(GL_TEXTURE0);
							glGenTextures(1, &sm->textureId);
							glBindTexture(GL_TEXTURE_2D, sm->textureId);
							if (o->textures[sm->texname].comp == 3) {
								glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, o->textures[sm->texname].w, o->textures[sm->texname].h, 0, GL_RGB,
									GL_UNSIGNED_BYTE, o->textures[sm->texname].image);
							}
							else if (o->textures[sm->texname].comp == 4) {
								glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, o->textures[sm->texname].w, o->textures[sm->texname].h, 0, GL_RGBA,
									GL_UNSIGNED_BYTE, o->textures[sm->texname].image);
							}
							else {
								assert(0);  // TODO
							}
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						}
					}
					// stbi_image_free(o->textures[sm.texname].image);

					// Position
					glGenBuffers(1, &VboVId);
					glBindBuffer(GL_ARRAY_BUFFER, VboVId);
					glBufferData(GL_ARRAY_BUFFER, o->bufferPosition.size() * sizeof(glm::vec4), &o->bufferPosition[0], GL_STATIC_DRAW);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(0);

					// UV
					glGenBuffers(1, &VboUVId);
					glBindBuffer(GL_ARRAY_BUFFER, VboUVId);
					glBufferData(GL_ARRAY_BUFFER, o->bufferUV.size() * sizeof(glm::vec2), &o->bufferUV[0], GL_STATIC_DRAW);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(1);

					// Normal

					// count Tirangle
					cntTirangle += o->numTriangles;
				}

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

				const GLchar *VertexShader = m_world->loadShader("UVonlyV");
				const GLchar *FragmentShader = m_world->loadShader("UVonlyF");

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

				MvpId = glGetUniformLocation(ProgramId, "MVP");
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

				if (!isSingleWindow) glfwHideWindow(m_world->m_window);

				glfwMakeContextCurrent(m_world->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
				
				glEnable(GL_DEPTH_TEST);

				render::OBJLoader objLoader;
				m_drawObjects.push_back(objLoader.loadOBJ("file", "path"));

				CreateShaders();
				CreateVBO();
			}

			void render()
			{
				render(glm::mat4x4(1.0));
			}

			void render(const glm::mat4 &MVP)
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
				glUniformMatrix4fv(MvpId, 1, GL_FALSE, &MVP[0][0]);

				// draw
				glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);

				for (render::DrawObject o : m_drawObjects)
				{
					for (render::DrawObject::SubMesh sm : o.subMeshs)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, sm.textureId);
						glUniform1i(TextureId, 0);

						glDrawArrays(GL_TRIANGLES, sm.idxBegin, sm.cntVertex);
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}

				if (!isSingleWindow) glfwSwapBuffers(m_world->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEOBJCOMPONENT_H_
