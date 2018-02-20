#ifndef KATA_RENDER_DRAWOBJECT_H_
#define KATA_RENDER_DRAWOBJECT_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


namespace kata
{
	namespace render
	{
		class DrawObject
		{
		private:
			int m_idx = -1;

		public:
			int numTriangles = 0; // not safe 
			GLuint vb_id;
			GLuint texture_id;

			std::vector<glm::vec4> bufferPosition;
			std::vector<glm::vec2> bufferUV;
			std::vector<glm::vec3> bufferNormal;

			int iw, ih, icomp;
			unsigned char* image = nullptr;
		};
	}
}


#endif // KATA_RENDER_DRAWOBJECT_H_