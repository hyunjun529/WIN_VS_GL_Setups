#ifndef KATA_GL_WINDOW_H_
#define KATA_GL_WINDOW_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <linmath.h>

#include "GLVertexUtil.h"
#include "GLShaderUtil.h"

namespace kata
{
	namespace GL
	{
		class GLWindow
		{
		private:
			GLVertexUtil m_vertexUtil;
			GLShaderUtil m_shaderUtil;

		public:
			GLFWwindow * m_window;

			GLWindow()
			{
				m_window = glfwCreateWindow(400, 400, "???", NULL, NULL);
			}

		};
	}
}


#endif // KATA_GL_WINDOW_H_