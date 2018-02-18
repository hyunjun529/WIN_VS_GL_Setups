#include "util/Log.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assert.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <linmath.h>

#include "GL/GLWindow.h"

#include "component/ImguiComponent/ImguiComponent.h"

#include "component/SceneComponenet/SceneGL40Component.h"


static bool FileExists(const std::string& abs_filename) {
	bool ret;
	FILE* fp;
	errno_t err = fopen_s(&fp, abs_filename.c_str(), "rb");
	if (fp) {
		ret = true;
		fclose(fp);
	}
	else {
		ret = false;
	}
	return ret;
}

static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
	float v10[3];
	v10[0] = v1[0] - v0[0];
	v10[1] = v1[1] - v0[1];
	v10[2] = v1[2] - v0[2];

	float v20[3];
	v20[0] = v2[0] - v0[0];
	v20[1] = v2[1] - v0[1];
	v20[2] = v2[2] - v0[2];

	N[0] = v20[1] * v10[2] - v20[2] * v10[1];
	N[1] = v20[2] * v10[0] - v20[0] * v10[2];
	N[2] = v20[0] * v10[1] - v20[1] * v10[0];

	float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
	if (len2 > 0.0f) {
		float len = sqrtf(len2);

		N[0] /= len;
		N[1] /= len;
		N[2] /= len;
	}
}

// Check if `mesh_t` contains smoothing group id.
bool hasSmoothingGroup(const tinyobj::shape_t& shape)
{
	for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) {
		if (shape.mesh.smoothing_group_ids[i] > 0) {
			return true;
		}
	}
	return false;
}

typedef struct {
	GLuint vb_id;  // vertex buffer id
	int numTriangles;
	size_t material_id;
} DrawObject;

std::vector<DrawObject> gDrawObjects;


// glfw
int xpos, ypos;


int main(int argc, char** argv)
{
	std::string base_dir = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//capsule//";
	std::string inputfile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//capsule//capsule.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	
	std::string err;
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

	std::map<std::string, GLuint> textures;

	// -------

	if (!glfwInit()) assert("failed glfwinit");

	kata::GL::GLWindow *glWindow = new kata::GL::GLWindow(800, 800);
	glWindow->updateWindowTitle("Single WIndow Mode");

	GLFWwindow *window = glWindow->m_window;

	if (!window)
	{
		glfwTerminate();
		assert("failed create glfw");
	}

	glfwMakeContextCurrent(window);

	if (gl3wInit())
	{
		assert("failed gl3winit");
	}

	kata::component::ImguiInputComponent *imguiComponent
		= new kata::component::ImguiInputComponent;

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsClassic();

	kata::component::SceneGL40GraphicsComponent *scene 
		= new kata::component::SceneGL40GraphicsComponent();

	scene->onSingleWindowMode();
	scene->setGLWindow(glWindow);
	scene->setImguiInput(imguiComponent);

	// scene->setup();

	// Load diffuse textures
	{
		for (size_t m = 0; m < materials.size(); m++) {
			tinyobj::material_t* mp = &materials[m];

			if (mp->diffuse_texname.length() > 0) {
				// Only load the texture if it is not already loaded
				if (textures.find(mp->diffuse_texname) == textures.end()) {
					GLuint texture_id;
					int w, h;
					int comp;

					std::string texture_filename = mp->diffuse_texname;
					if (!FileExists(texture_filename)) {
						// Append base dir.
						texture_filename = base_dir + mp->diffuse_texname;
						if (!FileExists(texture_filename)) {
							KATA_CONSOLE_ERROR("Unable to find file: {}", mp->diffuse_texname);
							exit(1);
						}
					}

					unsigned char* image =
						stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
					if (!image) {
						KATA_CONSOLE_ERROR("Unable to load texture: {}", texture_filename);
						exit(1);
					}
					KATA_CONSOLE_INFO("Loaded texture: {}, w = {}, h = {}, comp = {}", texture_filename, w, h, comp);

					glGenTextures(1, &texture_id);
					glBindTexture(GL_TEXTURE_2D, texture_id);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
					glBindTexture(GL_TEXTURE_2D, 0);
					stbi_image_free(image);
					textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
				}
			}
		}
	}

	float bmin[3], bmax[3];
	bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
	bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();
	
	{
		for (size_t s = 0; s < shapes.size(); s++) {
			DrawObject o;
			std::vector<float> buffer;  // pos(3float), normal(3float), color(3float)

										// Check for smoothing group and compute smoothing normals
			std::map<int, vec3> smoothVertexNormals;
			//if (hasSmoothingGroup(shapes[s]) > 0) {
			//	std::cout << "Compute smoothingNormal for shape [" << s << "]" << std::endl;
			//	computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
			//}

			for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
				tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

				int current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) ||
					(current_material_id >= static_cast<int>(materials.size()))) {
					// Invaid material ID. Use default material.
					current_material_id =
						materials.size() -
						1;  // Default material is added to the last item in `materials`.
				}
				// if (current_material_id >= materials.size()) {
				//    std::cerr << "Invalid material index: " << current_material_id <<
				//    std::endl;
				//}
				//
				float diffuse[3];
				for (size_t i = 0; i < 3; i++) {
					diffuse[i] = materials[current_material_id].diffuse[i];
				}
				float tc[3][2];
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
					else {
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

				float v[3][3];
				for (int k = 0; k < 3; k++) {
					int f0 = idx0.vertex_index;
					int f1 = idx1.vertex_index;
					int f2 = idx2.vertex_index;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);

					v[0][k] = attrib.vertices[3 * f0 + k];
					v[1][k] = attrib.vertices[3 * f1 + k];
					v[2][k] = attrib.vertices[3 * f2 + k];
					bmin[k] = std::min(v[0][k], bmin[k]);
					bmin[k] = std::min(v[1][k], bmin[k]);
					bmin[k] = std::min(v[2][k], bmin[k]);
					bmax[k] = std::max(v[0][k], bmax[k]);
					bmax[k] = std::max(v[1][k], bmax[k]);
					bmax[k] = std::max(v[2][k], bmax[k]);
				}

				float n[3][3];
				{
					bool invalid_normal_index = false;
					if (attrib.normals.size() > 0) {
						int nf0 = idx0.normal_index;
						int nf1 = idx1.normal_index;
						int nf2 = idx2.normal_index;

						if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
							// normal index is missing from this face.
							invalid_normal_index = true;
						}
						else {
							for (int k = 0; k < 3; k++) {
								assert(size_t(3 * nf0 + k) < attrib.normals.size());
								assert(size_t(3 * nf1 + k) < attrib.normals.size());
								assert(size_t(3 * nf2 + k) < attrib.normals.size());
								n[0][k] = attrib.normals[3 * nf0 + k];
								n[1][k] = attrib.normals[3 * nf1 + k];
								n[2][k] = attrib.normals[3 * nf2 + k];
							}
						}
					}
					else {
						invalid_normal_index = true;
					}

					if (invalid_normal_index && !smoothVertexNormals.empty()) {
						// Use smoothing normals
						int f0 = idx0.vertex_index;
						int f1 = idx1.vertex_index;
						int f2 = idx2.vertex_index;

						if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
							//n[0][0] = smoothVertexNormals[f0].v[0];
							//n[0][1] = smoothVertexNormals[f0].v[1];
							//n[0][2] = smoothVertexNormals[f0].v[2];

							//n[1][0] = smoothVertexNormals[f1].v[0];
							//n[1][1] = smoothVertexNormals[f1].v[1];
							//n[1][2] = smoothVertexNormals[f1].v[2];

							//n[2][0] = smoothVertexNormals[f2].v[0];
							//n[2][1] = smoothVertexNormals[f2].v[1];
							//n[2][2] = smoothVertexNormals[f2].v[2];

							invalid_normal_index = false;
						}
					}

					if (invalid_normal_index) {
						// compute geometric normal
						CalcNormal(n[0], v[0], v[1], v[2]);
						n[1][0] = n[0][0];
						n[1][1] = n[0][1];
						n[1][2] = n[0][2];
						n[2][0] = n[0][0];
						n[2][1] = n[0][1];
						n[2][2] = n[0][2];
					}
				}

				for (int k = 0; k < 3; k++) {
					buffer.push_back(v[k][0]);
					buffer.push_back(v[k][1]);
					buffer.push_back(v[k][2]);
					buffer.push_back(n[k][0]);
					buffer.push_back(n[k][1]);
					buffer.push_back(n[k][2]);
					// Combine normal and diffuse to get color.
					float normal_factor = 0.2;
					float diffuse_factor = 1 - normal_factor;
					float c[3] = { n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
						n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
						n[k][2] * normal_factor + diffuse[2] * diffuse_factor };
					float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
					if (len2 > 0.0f) {
						float len = sqrtf(len2);

						c[0] /= len;
						c[1] /= len;
						c[2] /= len;
					}
					buffer.push_back(c[0] * 0.5 + 0.5);
					buffer.push_back(c[1] * 0.5 + 0.5);
					buffer.push_back(c[2] * 0.5 + 0.5);

					buffer.push_back(tc[k][0]);
					buffer.push_back(tc[k][1]);
				}
			}

			o.vb_id = 0;
			o.numTriangles = 0;

			// OpenGL viewer does not support texturing with per-face material.
			if (shapes[s].mesh.material_ids.size() > 0 &&
				shapes[s].mesh.material_ids.size() > s) {
				o.material_id = shapes[s].mesh.material_ids[0];  // use the material ID
																 // of the first face.
			}
			else {
				o.material_id = materials.size() - 1;  // = ID for default material.
			}
			printf("shape[%d] material_id %d\n", int(s), int(o.material_id));

			if (buffer.size() > 0) {
				glGenBuffers(1, &o.vb_id);
				glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);
				glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);
				o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) / 3;  // 3:vtx, 3:normal, 3:col, 2:texcoord
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				printf("shape[%d] # of triangles = %d\n", static_cast<int>(s), o.numTriangles);
			}
			gDrawObjects.push_back(o);
		}
	}

	std::vector<DrawObject> drawObjects = gDrawObjects;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		// scene->render();

		glClearColor(
			imguiComponent->param_color_clear.x,
			imguiComponent->param_color_clear.y,
			imguiComponent->param_color_clear.z,
			imguiComponent->param_color_clear.w
		);

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		for (size_t i = 0; i < drawObjects.size(); i++) {
			DrawObject o = drawObjects[i];

			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
		}

		glfwMakeContextCurrent(window);
		ImGui_ImplGlfwGL3_NewFrame();
		imguiComponent->render();
		ImGui::Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}