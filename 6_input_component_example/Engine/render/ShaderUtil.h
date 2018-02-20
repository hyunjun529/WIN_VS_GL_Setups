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

			const GLchar *vertUVonly =
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

			const GLchar *fragUVonly =
			{
				"#version 400 core\n"\

				"in vec2 UV;\n"\

				"out vec4 color;\n"\

				"uniform sampler2D myTextureSampler;\n"\

				"void main(){\n"\

				"	color = vec4(texture( myTextureSampler, UV ).rgb, 1.0f);\n"\

				"}\n"
			};
		};
	}
}


#endif // KATA_RENDER_SHADERUTIL_H_