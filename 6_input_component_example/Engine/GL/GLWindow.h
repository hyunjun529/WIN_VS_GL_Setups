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
		private:
			int m_windowWidth = 400;
			int m_windowHeight = 400;

			char windowTitle[64] = "??";

		public:
			GLFWwindow * m_window;

			GLWindow(int _w = 400, int _h = 400)
				: m_windowWidth(_w), m_windowHeight(_h)
			{
				m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, windowTitle, NULL, NULL);
			}

			void updateWindowTitle(const char *_title)
			{
				strncpy_s(windowTitle, _title, sizeof(windowTitle));
				glfwSetWindowTitle(m_window, windowTitle);
			}
		};
	}
}


#endif // KATA_GL_WINDOW_H_