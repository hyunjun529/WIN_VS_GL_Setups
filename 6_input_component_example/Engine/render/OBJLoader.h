#ifndef KATA_RENDER_OBJLOADER_H_
#define KATA_RENDER_OBJLOADER_H_

#include "tiny_obj_loader.h"

#include "stb_image.h"

#include "DrawObject.h"


namespace kata
{
	namespace render
	{
		// https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
		class OBJLoader
		{
		private:
			std::string base_dir = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//";
			std::string inputfile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//cube.obj";
			//std::string base_dir = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//kizunaai//";
			//std::string inputfile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//kizunaai//kizunaai.obj";
			std::string texturefile = "C://Users//hyunjun529//Documents//WIN_VS_GL_Setups//6_input_component_example//resource//cube//default.png";
			
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			
			std::string err;


		public:
			// vector�� �ƴ϶� map�̳� �̸� �پ��ִ� �ٸ� collection���� �ٲ�� �� ���� ����
			std::vector<DrawObject> loadOBJ(const char *_file, const char *_path)
			{
				std::vector<DrawObject> o;

				DrawObject tmp_o;

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

				// load texture
				tmp_o.image = stbi_load(texturefile.c_str(), &tmp_o.iw, &tmp_o.ih, &tmp_o.icomp, STBI_default);

				// load obj position, uv, normal
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
							tmp_o.bufferPosition.push_back(v[k]);
							tmp_o.bufferUV.push_back(tc[k]);
							tmp_o.numTriangles++;
						}
					}
				}

				o.push_back(tmp_o);
				
				return o;
			}
		};
	}
}


#endif // KATA_RENDER_OBJLOADER_H_