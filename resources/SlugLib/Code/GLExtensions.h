#if SLUG_WINDOWS

	GLEXTFUNC(void, glActiveTexture, (GLenum))

#endif

#if SLUG_WINDOWS || SLUG_LINUX

	GLEXTFUNC(void, glMultiDrawElements, (GLenum, const GLsizei *, GLenum, const void *const *, GLsizei))

	GLEXTFUNC(void, glEnableVertexAttribArray, (GLuint))
	GLEXTFUNC(void, glDisableVertexAttribArray, (GLuint))
	GLEXTFUNC(void, glVertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))

	GLEXTFUNC(void, glGenBuffers, (GLsizei, GLuint *))
	GLEXTFUNC(void, glDeleteBuffers, (GLsizei, const GLuint *))
	GLEXTFUNC(void, glBindBuffer, (GLenum, GLuint))
	GLEXTFUNC(void, glBufferData, (GLenum, GLsizeiptr, const void *, GLenum))
	GLEXTFUNC(void *, glMapBuffer, (GLenum, GLenum))
	GLEXTFUNC(GLboolean, glUnmapBuffer, (GLenum))

	GLEXTFUNC(GLuint, glCreateShader, (GLenum))
	GLEXTFUNC(void, glDeleteShader, (GLuint))
	GLEXTFUNC(void, glShaderSource, (GLuint, GLsizei, const GLchar **, const GLint *))
	GLEXTFUNC(void, glCompileShader, (GLuint))
	GLEXTFUNC(void, glAttachShader, (GLuint, GLuint))
	GLEXTFUNC(GLuint, glCreateProgram, (void))
	GLEXTFUNC(void, glDeleteProgram, (GLuint))
	GLEXTFUNC(void, glLinkProgram, (GLuint))
	GLEXTFUNC(void, glUseProgram, (GLuint))
	GLEXTFUNC(GLint, glGetUniformLocation, (GLuint, const GLchar *))
	GLEXTFUNC(void, glUniform1i, (GLint, GLint))
	GLEXTFUNC(void, glUniform4f, (GLint, GLfloat, GLfloat, GLfloat, GLfloat))
	GLEXTFUNC(void, glUniform4fv, (GLint, GLsizei, const GLfloat *))

	GLEXTFUNC(void, glGenQueries, (GLsizei, GLuint *))
	GLEXTFUNC(void, glDeleteQueries, (GLsizei, const GLuint *))
	GLEXTFUNC(void, glBeginQuery, (GLenum, GLuint))
	GLEXTFUNC(void, glEndQuery, (GLenum))

#endif

GLEXTFUNC(void, glGenVertexArrays, (GLsizei, GLuint *))
GLEXTFUNC(void, glDeleteVertexArrays, (GLsizei, const GLuint *))
GLEXTFUNC(void, glBindVertexArray, (GLuint))

GLEXTFUNC(void, glGetQueryObjectui64v, (GLuint, GLenum, GLuint64 *))

#if SLUG_WINDOWS

	GLEXTFUNC(BOOL, wglChoosePixelFormatARB, (HDC, const int *, const FLOAT *, UINT, int *, UINT *))
	GLEXTFUNC(HGLRC, wglCreateContextAttribsARB, (HDC, HGLRC, const int *))
	GLEXTFUNC(BOOL, wglSwapIntervalEXT, (int))

#elif SLUG_LINUX

	GLEXTFUNC(GLXContext, glXCreateContextAttribsARB, (::Display *, GLXFBConfig, GLXContext, Bool, const int *))
	GLEXTFUNC(void, glXSwapIntervalEXT, (::Display *, GLXDrawable, int))

#endif
