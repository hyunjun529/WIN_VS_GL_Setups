#ifndef KATA_COMPONENT_SCENEGL32COMPONENT_H_
#define KATA_COMPONENT_SCENEGL32COMPONENT_H_

#include <memory>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "../../GL/GLWindow.h"

#include "../Component.h"
#include "../InputComponent.h"
#include "../PhysicsComponent.h"
#include "../GraphicsComponent.h"

#include "../ImguiComponent/ImguiComponent.h"

#include "SceneComponent.h"


namespace kata
{
	namespace component
	{
		class SceneGL32GraphicsComponent : public SceneGraphicsComponent
		{
		private:
			// https://www.khronos.org/opengl/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
			// tutorial2.c > drawscene function

			GLuint vao, vbo[2]; /* Create handles for our Vertex Array Object and two Vertex Buffer Objects */
			int IsCompiled_VS, IsCompiled_FS;
			int IsLinked;
			int maxLength;
			char *vertexInfoLog;
			char *fragmentInfoLog;
			char *shaderProgramInfoLog;

			/* We're going to create a simple diamond made from lines */
			const GLfloat diamond[4][2] = {
				{ 0.0,  1.0 }, /* Top point */
				{ 1.0,  0.0 }, /* Right point */
				{ 0.0, -1.0 }, /* Bottom point */
				{ -1.0,  0.0 } }; /* Left point */

			const GLfloat colors[4][3] = {
				{ 1.0,  0.0,  0.0 }, /* Red */
				{ 0.0,  1.0,  0.0 }, /* Green */
				{ 0.0,  0.0,  1.0 }, /* Blue */
				{ 1.0,  1.0,  1.0 } }; /* White */

			/* These pointers will receive the contents of our shader source code files */
			GLchar *vertexsource, *fragmentsource;

			const char *vert = "#version 150/w"
				"// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1/w"
				"in  vec2 in_Position;/w"
				"in  vec3 in_Color;/w"
				"/w"
				"// We output the ex_Color variable to the next shader in the chain/w"
				"out vec3 ex_Color;/w"
				"void main(void) {/w"
				"    // Since we are using flat lines, our input only had two points: x and y./w"
				"    // Set the Z coordinate to 0 and W coordinate to 1/w"
				"/w"
				"    gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);/w"
				"/w"
				"    // GLSL allows shorthand use of vectors too, the following is also valid:/w"
				"    // gl_Position = vec4(in_Position, 0.0, 1.0);/w"
				"    // We're simply passing the color through unmodified/w"
				"/w"
				"    ex_Color = in_Color;/w"
				"}/w";

			const char *frag = "#version 150/w"
				"// It was expressed that some drivers required this next line to function properly/w"
				"precision highp float;/w"
				"/w"
				"in  vec3 ex_Color;/w"
				"out vec4 gl_FragColor;/w"
				"/w"
				"void main(void) {/w"
				"    // Pass through our original color with full opacity./w"
				"    gl_FragColor = vec4(ex_Color,1.0);/w"
				"}/w";

			/* These are handles used to reference the shaders */
			GLuint vertexshader, fragmentshader;

			/* This is a handle to the shader program */
			GLuint shaderprogram;

		public:
			SceneGL32GraphicsComponent() {}

			void setup()
			{
				if (!m_GLWindow)
				{
					assert("check init order");
				}

				glfwMakeContextCurrent(m_GLWindow->m_window);
				glfwHideWindow(m_GLWindow->m_window);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

				/* Allocate and assign a Vertex Array Object to our handle */
				glGenVertexArrays(1, &vao);

				/* Bind our Vertex Array Object as the current used object */
				glBindVertexArray(vao);

				/* Allocate and assign two Vertex Buffer Objects to our handle */
				glGenBuffers(2, vbo);

				/* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

				/* Copy the vertex data from diamond to our buffer */
				/* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
				glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), diamond, GL_STATIC_DRAW);
				
				/* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

				/* Enable attribute index 0 as being used */
				glEnableVertexAttribArray(0);

				/* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
				glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

				/* Copy the color data from colors to our buffer */
				/* 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values */
				glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

				/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
				
				/* Enable attribute index 1 as being used */
				glEnableVertexAttribArray(1);

				/* Read our shaders into the appropriate buffers */
				/* 파일 읽기가 귀찮으니 그냥 const char*에서 복사해서 쓰자 */
				char _vert[9999];
				char _frag[9999];
				strcpy_s(_vert, vert);
				strcpy_s(_frag, frag);
				
				vertexsource = _vert;
				fragmentsource = _frag;

				/* Create an empty vertex shader handle */
				vertexshader = glCreateShader(GL_VERTEX_SHADER);

				/* Send the vertex shader source code to GL */
				/* Note that the source code is NULL character terminated. */
				/* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
				glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);

				/* Compile the vertex shader */
				glCompileShader(vertexshader);

				glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
				if (IsCompiled_VS == FALSE)
				{
					glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);

					/* The maxLength includes the NULL character */
					vertexInfoLog = (char *)malloc(maxLength);

					glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);

					/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
					/* In this simple program, we'll just leave */
					free(vertexInfoLog);
					return;
				}

				/* Create an empty fragment shader handle */
				fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

				/* Send the fragment shader source code to GL */
				/* Note that the source code is NULL character terminated. */
				/* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
				glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
				
				/* Compile the fragment shader */
				glCompileShader(fragmentshader);

				glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
				if (IsCompiled_FS == FALSE)
				{
					glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);

					/* The maxLength includes the NULL character */
					fragmentInfoLog = (char *)malloc(maxLength);

					glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);

					/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
					/* In this simple program, we'll just leave */
					free(fragmentInfoLog);
					return;
				}

				/* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
				/* We must link them together to make a GL shader program */
				/* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
				/* Assign our program handle a "name" */
				shaderprogram = glCreateProgram();

				/* Attach our shaders to our program */
				glAttachShader(shaderprogram, vertexshader);
				glAttachShader(shaderprogram, fragmentshader);

				/* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
				/* Attribute locations must be setup before calling glLinkProgram. */
				glBindAttribLocation(shaderprogram, 0, "in_Position");
				glBindAttribLocation(shaderprogram, 1, "in_Color");

				/* Link our program */
				/* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
				/* The binary code is uploaded to the GPU, if there is no error. */
				glLinkProgram(shaderprogram);

				/* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
				/* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
				/* too many texel fetch instructions or too many interpolators or dynamic loops. */
				glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int *)&IsLinked);
				if (IsLinked == FALSE)
				{
					/* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
					glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);

					/* The maxLength includes the NULL character */
					shaderProgramInfoLog = (char *)malloc(maxLength);

					/* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
					glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);

					/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
					/* In this simple program, we'll just leave */
					free(shaderProgramInfoLog);
					return;
				}
			}

			void render()
			{
				if (!m_imguiInputComponenet)
				{
					assert("check init order");
				}
				glfwMakeContextCurrent(m_GLWindow->m_window);

				/* Load the shader into the rendering pipeline */
				glUseProgram(shaderprogram);

				/* Loop our display increasing the number of shown vertexes each time.
				* Start with 2 vertexes (a line) and increase to 3 (a triangle) and 4 (a diamond) */
				for (int i = 2; i <= 4; i++)
				{
					/* Make our background black */
					glClearColor(0.0, 0.0, 0.0, 1.0);
					glClear(GL_COLOR_BUFFER_BIT);

					/* Invoke glDrawArrays telling that our data is a line loop and we want to draw 2-4 vertexes */
					glDrawArrays(GL_LINE_LOOP, 0, i);
				}

				glfwSwapBuffers(m_GLWindow->m_window);

				glReadPixels(0, 0, 400, 400, GL_RGBA, GL_UNSIGNED_INT, m_pixel);
			}
		};
	}
}

#endif // KATA_COMPONENT_SCENEGL32COMPONENT_H_
