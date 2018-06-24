#ifndef SlugDemo_h
#define SlugDemo_h


#if defined(SLUG_WINDOWS)

	#undef SLUG_WINDOWS
	#define SLUG_WINDOWS	1
	#define SLUG_MACOS		0
	#define SLUG_LINUX		0

#elif defined(SLUG_MACOS)

	#undef SLUG_MACOS
	#define SLUG_MACOS		1
	#define SLUG_WINDOWS	0
	#define SLUG_LINUX		0

#elif defined(SLUG_LINUX)

	#undef SLUG_LINUX
	#define SLUG_LINUX		1
	#define SLUG_WINDOWS	0
	#define SLUG_MACOS		0

#endif


#if defined(SLUG_OPENGL)

	#undef SLUG_OPENGL
	#define SLUG_OPENGL		1
	#define SLUG_D3D11		0

#elif defined(SLUG_D3D11)

	#undef SLUG_D3D11
	#define SLUG_D3D11		1
	#define SLUG_OPENGL		0

#endif


#if defined(SLUG_DEBUG)

	#undef SLUG_DEBUG
	#define SLUG_DEBUG		1

#else

	#define SLUG_DEBUG		0

#endif


#if defined(_MSC_VER)

	#pragma warning(disable: 4577)		// Silence warning about exceptions being turned off.

#endif


#if SLUG_WINDOWS

	#include <windows.h>

	#if SLUG_OPENGL

		#include <gl/gl.h>

	#elif SLUG_D3D11

		#include <d3d11.h>
		#include <d3dcompiler.h>

	#endif

#elif SLUG_MACOS

	#include <sys/stat.h>
	#include <unistd.h>
	#include <AppKit/NSScreen.h>
	#include <AppKit/NSWindow.h>
	#include <AppKit/NSOpenGL.h>
	#include <AppKit/NSOpenGLView.h>
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>

#elif SLUG_LINUX

	#define GL_GLEXT_LEGACY

	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <GL/glx.h>
	#include <X11/keysymdef.h>

#endif


#include <math.h>
#include "Slug.h"
#include "TSArray.h"


#if SLUG_OPENGL

	// Define the OpenGL enums and types we need that aren't in gl.h.

	#define GL_TEXTURE0								0x84C0
	#define GL_TEXTURE1								0x84C1
	#define GL_TEXTURE_RECTANGLE					0x84F5
	#define GL_RGBA_INTEGER							0x8D99
	#define GL_HALF_FLOAT							0x140B
	#define GL_RGBA16UI								0x8D76
	#define GL_RGBA16F								0x881A
	#define GL_VERTEX_SHADER						0x8B31
	#define GL_FRAGMENT_SHADER						0x8B30
	#define GL_ARRAY_BUFFER							0x8892
	#define GL_ELEMENT_ARRAY_BUFFER					0x8893
	#define GL_STATIC_DRAW							0x88E4
	#define GL_WRITE_ONLY							0x88B9
	#define GL_QUERY_RESULT							0x8866
	#define GL_TIME_ELAPSED							0x88BF
	#define GL_TIMESTAMP							0x8E28
	#define GL_FRAMEBUFFER_SRGB						0x8DB9

	#if SLUG_WINDOWS

		#define WGL_CONTEXT_MAJOR_VERSION_ARB			0x2091
		#define WGL_CONTEXT_MINOR_VERSION_ARB			0x2092
		#define WGL_CONTEXT_PROFILE_MASK_ARB			0x9126
		#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB		0x00000001
		#define WGL_DRAW_TO_WINDOW_ARB					0x2001
		#define WGL_ACCELERATION_ARB					0x2003
		#define WGL_SUPPORT_OPENGL_ARB					0x2010
		#define WGL_DOUBLE_BUFFER_ARB					0x2011
		#define WGL_PIXEL_TYPE_ARB						0x2013
		#define WGL_COLOR_BITS_ARB						0x2014
		#define WGL_ALPHA_BITS_ARB						0x201B
		#define WGL_FULL_ACCELERATION_ARB				0x2027
		#define WGL_TYPE_RGBA_ARB						0x202B
		#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB		0x20A9

	#elif SLUG_LINUX

		#define GLX_CONTEXT_MAJOR_VERSION_ARB			0x2091
		#define GLX_CONTEXT_MINOR_VERSION_ARB			0x2092
		#define GLX_CONTEXT_FLAGS_ARB					0x2094
		#define GLX_CONTEXT_PROFILE_MASK_ARB			0x9126
		#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB		0x00000001
		#define GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB		0x20B2

	#endif


	typedef char			GLchar;
	typedef ptrdiff_t		GLsizeiptr;
	typedef unsigned_int64	GLuint64;


	// This macro is used to retrieve function pointers from the GL driver.

	#if SLUG_WINDOWS

		#define GETGLPROC(name) *reinterpret_cast<PROC *>(&name) = wglGetProcAddress(#name)

	#elif SLUG_MACOS

		#define GETGLPROC(name) *reinterpret_cast<void **>(&name) = GetBundleFunctionAddress(#name)

	#elif SLUG_LINUX

		#define GETGLPROC(name) *reinterpret_cast<void (**)()>(&name) = glXGetProcAddress(reinterpret_cast<const GLubyte *>(#name))

	#endif

	// Declare the GL functions we need that aren't in opengl32.lib.
	// The actual storage is defined at the top of SlugDemo.cpp.

	#define GLEXTFUNC(type, name, params) extern type (*name)params;
	#include "GLExtensions.h"

#endif


using namespace Terathon;	// The namespace containing the Array class.


namespace Demo
{
	// Define some minimal linear algebra stuff.

	class Vector3D
	{
		public:

			float	x;
			float	y;
			float	z;

			Vector3D() = default;

			Vector3D(float a, float b, float c)
			{
				x = a;
				y = b;
				z = c;
			}

			Vector3D& operator =(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return (*this);
			}

			Vector3D& operator *=(float s)
			{
				x *= s;
				y *= s;
				z *= s;
				return (*this);
			}
	};


	inline Vector3D operator *(const Vector3D& v, float s)
	{
		return (Vector3D(v.x * s, v.y * s, v.z * s));
	}

	inline float Dot(const Vector3D& a, const Vector3D& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	inline Vector3D Cross(const Vector3D& a, const Vector3D& b)
	{
		return (Vector3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x));
	}


	class Matrix4D
	{
		protected:

			alignas(16) float	n[4][4];

		public:

			Matrix4D() = default;

			Matrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);

			float& operator ()(machine i, machine j)
			{
				return (n[j][i]);
			}

			const float& operator ()(machine i, machine j) const
			{
				return (n[j][i]);
			}
	};


	class Transform4D : public Matrix4D
	{
		public:

			Transform4D()
			{
				n[0][3] = n[1][3] = n[2][3] = 0.0F;
				n[3][3] = 1.0F;
			}

			Transform4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23) : Matrix4D(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23, 0.0F, 0.0F, 0.0F, 1.0F)
			{
			}

			Transform4D(const Vector3D& r0, float n03, const Vector3D& r1, float n13, const Vector3D& r2, float n23);

			const Vector3D& operator [](machine j) const
			{
				return (*reinterpret_cast<const Vector3D *>(n[j]));
			}
	};


	Matrix4D operator *(const Matrix4D& m1, const Matrix4D& m2);
	Transform4D Inverse(const Transform4D& m);


	// A Font object manages the data from a .slug file and the textures associated with a font.
	// Each Text object holds a reference to the Font object that is uses.

	class Font
	{
		private:

			char							*dataStorage;		// The storage allocated for the .slug file.
			char							*slugFile;			// The 64-byte aligned location where the .slug data begins.

			int32							referenceCount;		// The font's current reference count.

			#if SLUG_OPENGL

				GLuint						textureName[2];		// The OpenGL names for the curve and band textures for this font.

			#elif SLUG_D3D11

				ID3D11Texture2D				*textureObject[2];
				ID3D11ShaderResourceView	*textureView[2];

			#endif

			~Font();

		public:

			Font(const char *fileName);

			const Slug::FontHeader *GetFontHeader(void) const
			{
				return (Slug::GetFontHeader(slugFile));
			}

			void Retain(void)
			{
				referenceCount++;
			}

			void Release(void)
			{
				if (--referenceCount == 0)
				{
					delete this;
				}
			}

			#if SLUG_OPENGL

				GLuint GetTextureName(int32 index) const
				{
					return (textureName[index]);
				}

			#elif SLUG_D3D11

				ID3D11ShaderResourceView *const *GetTextureViews(void) const
				{
					return (textureView);
				}

			#endif
	};


	// A Text object manages a single line of text drawn with a specific font and layout options.

	class Text
	{
		private:

			Font				*textFont;				// The font used by this text.
			Slug::LayoutData	textLayoutData;			// A copy of the layout data passed into the constructor.
			Matrix4D			textTransform;			// The 4x4 transform for this text object.

			int32				elementCount[2];		// The number of vertex indices passed to the draw function.
			unsigned_machine	effectOffset;			// The offset at which indices for the effect layer start.

			#if SLUG_OPENGL

				GLuint			vertexShaderName;		// The OpenGL name for the vertex shader.
				GLuint			pixelShaderName;		// The OpenGL name for the pixel shader.
				GLuint			programName;			// The OpenGL name for the shader program.
				GLint			vparamLocation;			// The location of the vertex shader uniform parameter.
				GLuint			bufferName[2];			// The OpenGL names of the vertex and index buffers.

			#elif SLUG_D3D11

				ID3D11VertexShader	*vertexShaderObject;
				ID3D11PixelShader	*pixelShaderObject;
				ID3D11InputLayout	*inputLayoutObject;
				ID3D11Buffer		*vertexBufferObject;
				ID3D11Buffer		*indexBufferObject;
				ID3D11Buffer		*constantBufferObject;

			#endif

		public:

			Text(Font *font, const Slug::LayoutData *layoutData, const char *text, float x, float y);
			~Text();

			void Build(const char *text);
			void Render(const Matrix4D& viewproj);
	};


	enum
	{
		kSceneCount = 5
	};


	extern int32 UintToString(unsigned_int32 num, char *text, int32 max);

	extern void InitSceneText(Array<Text *> (& textArray)[kSceneCount], bool (& orthoFlag)[kSceneCount], unsigned_int32 extraLayoutFlags);
	extern void TermSceneText(Array<Text *> (& textArray)[kSceneCount]);

	extern void InitOverlayText(int32 width, int32 height, Array<Text *>& textArray, Text **timeText, unsigned_int32 extraLayoutFlags);
	extern void TermOverlayText(Array<Text *>& textArray);

	extern void Render(const ImmutableArray<Text *>& textArray, const Matrix4D& viewproj);
}


#endif
