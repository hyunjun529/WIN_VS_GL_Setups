#ifndef KATA_RENDER_SHADERUTIL_H_
#define KATA_RENDER_SHADERUTIL_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


namespace kata
{
	namespace render
	{
		class ShaderUtil
		{
		public:
			const GLchar *VertexShader =
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

			const GLchar *FragmentShader =
			{
				"#version 400\n"\

				"in vec4 ex_Color;\n"\
				"out vec4 out_Color;\n"\

				"void main(void)\n"\
				"{\n"\
				"  out_Color = ex_Color;\n"\
				"}\n"
			};
		};
	}
}


#endif // KATA_RENDER_SHADERUTIL_H_