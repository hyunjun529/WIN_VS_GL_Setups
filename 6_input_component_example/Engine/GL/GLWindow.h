#ifndef KATA_GL_WINDOW_H_
#define KATA_GL_WINDOW_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <linmath.h>


namespace kata
{
	namespace GL
	{
		class GLWindow
		{
		public:
			GLFWwindow * m_window;

			GLWindow()
			{
				m_window = glfwCreateWindow(800, 800, "???", NULL, NULL);
			}

		};
	}
}


#endif // KATA_GL_WINDOW_H_