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
			GLuint vb_id;
			int numTriangles;
			size_t material_id;
		};
	}
}


#endif // KATA_RENDER_DRAWOBJECT_H_