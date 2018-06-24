#include "SlugDemo.h"


#if SLUG_WINDOWS

	// Declaring the following externally visible global variables is how we're supposed to tell
	// the system to use the high-performance GPU when there's also an integrated GPU available.

	extern "C"
	{
		__declspec(dllexport) DWORD NvOptimusEnablement = 1;
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}

#endif


const float nearDistance = 0.1F;		// View frustum parameters.
const float focalLength = 2.0F;
float moveFactor = 1.0F;

float viewDistance;						// View state.
float viewAzimuth;
float viewAltitude;
float viewOffsetX;
float viewOffsetY;
Demo::Transform4D viewTransform;

bool leftButtonFlag = false;			// Input state.
bool rightButtonFlag = false;
float lastMouseX = 0.0F;
float lastMouseY = 0.0F;

int32 screenWidth;
int32 screenHeight;
unsigned_int32 frameIndex = 0;

bool quitFlag = false;					// Program quits when this flag is set.
bool weightFlag = false;				// This flag is set when Tab is pressed.
int32 currentScene = 0;					// The index of the scene to render.

unsigned_int32 extraLayoutFlags = Slug::kLayoutOpticalWeight;	// Tab toggles the kLayoutOpticalWeight flag.

Demo::Matrix4D			sceneProjection;
Demo::Matrix4D			overlayProjection;
bool					sceneOrthoFlag[Demo::kSceneCount];
Array<Demo::Text *>		sceneTextArray[Demo::kSceneCount];
Array<Demo::Text *>		overlayTextArray;
Demo::Text				*timeText;

#if SLUG_OPENGL

	GLuint			vertexArrayObject;
	GLuint			timerQueryName[4];

	const char		*rendererDescription;

#elif SLUG_D3D11

	ID3D11Query		*disjointQuery[4];
	ID3D11Query		*beginTimeQuery[4];
	ID3D11Query		*endTimeQuery[4];

	ID3DBlob *vertexBlob[Slug::kMaxVertexShaderCount] = {nullptr};
	ID3DBlob *pixelBlob[Slug::kMaxPixelShaderCount] = {nullptr};

	char			rendererDescription[128];

#endif

#if SLUG_WINDOWS

	HWND			demoWindow;
	LARGE_INTEGER	counterFrequency;

	#if SLUG_OPENGL

		HDC			deviceContext;
		HGLRC		openglContext;

	#elif SLUG_D3D11

		ID3D11Device			*d3dDevice;
		ID3D11DeviceContext		*d3dContext;
		ID3D11RenderTargetView	*d3dTargetView;
		IDXGISwapChain			*d3dSwapChain;

	#endif

#elif SLUG_MACOS

	NSWindow					*demoWindow;
	NSOpenGLView				*openglView;
	NSOpenGLContext				*openglContext;
	CFBundleRef					openglBundle;

	void *GetBundleFunctionAddress(const char *name)
	{
		CFStringRef string = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, name, kCFStringEncodingASCII, kCFAllocatorNull);
		void *address = CFBundleGetFunctionPointerForName(openglBundle, string);
		CFRelease(string);
		return (address);
	}

#elif SLUG_LINUX

	Display			*mainDisplay;
	Colormap		openglColormap;
	Window			openglWindow;
	GLXContext		openglContext;

#endif


void CalculateView(void)
{
	float t = viewAzimuth * 6.2831853F;
	float p = viewAltitude * 6.2831853F;
	float ct = cosf(t);
	float st = sinf(t);
	float cp = cosf(p);
	float sp = sinf(p);

	Demo::Vector3D view(ct * cp, sp, -st * cp);

	float x = view.x;
	float z = view.z;
	float f = 1.0F / sqrtf(x * x + z * z);
	Demo::Vector3D right(z * f, 0.0F, -x * f);
	Demo::Vector3D down = Cross(view, right);

	viewTransform(0,0) = right.x;
	viewTransform(1,0) = right.y;
	viewTransform(2,0) = right.z;

	viewTransform(0,1) = down.x;
	viewTransform(1,1) = down.y;
	viewTransform(2,1) = down.z;

	viewTransform(0,2) = view.x;
	viewTransform(1,2) = view.y;
	viewTransform(2,2) = view.z;

	viewTransform(0,3) = viewOffsetX * right.x + viewOffsetY * down.x - viewDistance * view.x;
	viewTransform(1,3) = viewOffsetX * right.y + viewOffsetY * down.y - viewDistance * view.y;
	viewTransform(2,3) = viewOffsetX * right.z + viewOffsetY * down.z - viewDistance * view.z;
}

void ResetView(void)
{
	viewDistance = 600.0F;
	viewAzimuth = 0.75F;
	viewAltitude = 0.0F;
	viewOffsetX = 0.0F;
	viewOffsetY = 0.0F;

	CalculateView();
}

void HandleLeftMouseDrag(float dx, float dy)
{
	float z = viewDistance * moveFactor;
	viewOffsetX -= dx * z;
	viewOffsetY -= dy * z;
	CalculateView();
}

void HandleRightMouseDrag(float dx, float dy)
{
	viewAzimuth = fmodf(viewAzimuth + dx * 0.001F, 1.0F);
	float p = viewAltitude + dy * 0.001F;
	p = (p < 0.23F) ? p : 0.23F;
	p = (p > -0.23F) ? p : -0.23F;
	viewAltitude = p;
	CalculateView();
}

void HandleMouseWheel(float delta)
{
	float d = viewDistance * powf(2.0F, delta);
	d = (d > nearDistance * 1.125F) ? d : nearDistance * 1.125F;
	viewDistance = d;
	CalculateView();
}


#if SLUG_OPENGL

	#if SLUG_WINDOWS

		LRESULT WglWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
		{
			if (message == WM_CREATE)
			{
				const CREATESTRUCT *createStruct = (CREATESTRUCT *) lparam;
				const PIXELFORMATDESCRIPTOR *formatDescriptor = (PIXELFORMATDESCRIPTOR *) createStruct->lpCreateParams;

				HDC deviceContext = GetDC(window);
				int pixelFormat = ChoosePixelFormat(deviceContext, formatDescriptor);
				if (pixelFormat != 0)
				{
					if (SetPixelFormat(deviceContext, pixelFormat, formatDescriptor))
					{
						HGLRC openglContext = wglCreateContext(deviceContext);
						if (wglMakeCurrent(deviceContext, openglContext))
						{
							GETGLPROC(wglCreateContextAttribsARB);
							GETGLPROC(wglChoosePixelFormatARB);
							GETGLPROC(wglSwapIntervalEXT);

							wglMakeCurrent(nullptr, nullptr);
						}

						wglDeleteContext(openglContext);
					}
				}

				ReleaseDC(window, deviceContext);
				return (0);
			}
	
			return (DefWindowProc(window, message, wparam, lparam));
		}

		bool InitWglExtensions(HINSTANCE instance, PIXELFORMATDESCRIPTOR *formatDescriptor)
		{
			// We have to create a legacy OpenGL context in order to get pointers to
			// the functions that we need to create a core OpenGL context. Yay, OpenGL!

			static const wchar_t name[] = L"wgl";

			WNDCLASSEXW		windowClass;

			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_OWNDC;
			windowClass.lpfnWndProc = &WglWindowProc;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = instance;
			windowClass.hIcon = nullptr;
			windowClass.hCursor = nullptr;
			windowClass.hbrBackground = nullptr;
			windowClass.lpszMenuName = nullptr;
			windowClass.lpszClassName = name;
			windowClass.hIconSm = nullptr;

			RegisterClassEx(&windowClass);
			DestroyWindow(CreateWindowExW(0, name, name, WS_POPUP, 0, 0, 32, 32, nullptr, nullptr, instance, formatDescriptor));
			UnregisterClass(name, instance);

			if ((!wglCreateContextAttribsARB) || (!wglChoosePixelFormatARB) || (!wglSwapIntervalEXT))
			{
				return (false);
			}

			return (true);
		}

	#endif

	#if SLUG_WINDOWS

		bool InitGraphics(HINSTANCE instance, HWND window)

	#elif SLUG_MACOS

		bool InitGraphics(void)

	#elif SLUG_LINUX

		bool InitGraphics(int32 screenWidth, int32 screenHeight, Display *display)

	#endif

	{
		#if SLUG_WINDOWS

			static const int formatAttributes[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, true,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_SUPPORT_OPENGL_ARB, true,
				WGL_DOUBLE_BUFFER_ARB, true,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, true,
				WGL_COLOR_BITS_ARB, 24,
				WGL_ALPHA_BITS_ARB, 8,
				0, 0
			};

			static const int contextAttributes[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0, 0
			};

			PIXELFORMATDESCRIPTOR	formatDescriptor;
			UINT					formatCount;
			int						pixelFormat;

			memset(&formatDescriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));
			formatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			formatDescriptor.nVersion = 1;
			formatDescriptor.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED;
			formatDescriptor.cColorBits = 24;
			formatDescriptor.cAlphaBits = 8;

			if (!InitWglExtensions(instance, &formatDescriptor))
			{
				return (false);
			}

			deviceContext = GetDC(window);

			if ((!wglChoosePixelFormatARB(deviceContext, formatAttributes, nullptr, 1, &pixelFormat, &formatCount)) || (formatCount == 0))
			{
				ReleaseDC(window, deviceContext);
				return (false);
			}

			if (!SetPixelFormat(deviceContext, pixelFormat, &formatDescriptor))
			{
				ReleaseDC(window, deviceContext);
				return (false);
			}

			openglContext = wglCreateContextAttribsARB(deviceContext, nullptr, contextAttributes);
			if (!openglContext)
			{
				ReleaseDC(window, deviceContext);
				return (false);
			}

			if (!wglMakeCurrent(deviceContext, openglContext))
			{
				wglDeleteContext(openglContext);
				ReleaseDC(window, deviceContext);
				return (false);
			}

		#elif SLUG_MACOS

			static NSOpenGLPixelFormatAttribute formatAttributes[] =
			{
				NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
				NSOpenGLPFAMinimumPolicy,
				NSOpenGLPFAAccelerated,
				NSOpenGLPFADoubleBuffer,
				NSOpenGLPFAColorSize, 24,
				NSOpenGLPFAAlphaSize, 8,
				0
			};

			NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: formatAttributes];
			if (!pixelFormat)
			{
				return (false);
			}

			openglContext = [[NSOpenGLContext alloc] initWithFormat: pixelFormat shareContext: nil];
			[pixelFormat release];

			if (!openglContext)
			{
				return (false);
			}

			CGLContextObj contextObject = (CGLContextObj) [openglContext CGLContextObj];
			GLint resolution[2] = {screenWidth, screenHeight};
			CGLSetParameter(contextObject, kCGLCPSurfaceBackingSize, resolution);
			CGLEnable(contextObject, kCGLCESurfaceBackingSize);

			[openglContext makeCurrentContext];
			[openglView setOpenGLContext: openglContext];
			[openglContext setView: openglView];

			NSFileManager *fileManager = [NSFileManager defaultManager];
			NSArray *urlArray = [fileManager URLsForDirectory: NSLibraryDirectory inDomains: NSSystemDomainMask];

			NSUInteger urlCount = [urlArray count];
			for (unsigned_machine a = 0; a < urlCount; a++)
			{
				NSError		*error;

				NSURL *folderURL = [urlArray objectAtIndex: a];
				NSURL *frameworkURL = [[NSURL alloc] initWithString: @"Frameworks/OpenGL.framework" relativeToURL: folderURL];
				if ([frameworkURL checkResourceIsReachableAndReturnError: &error] == YES)
				{
					openglBundle = CFBundleCreate(kCFAllocatorDefault, (CFURLRef) frameworkURL);
					[frameworkURL release];
					break;
				}

				[frameworkURL release];
			}

		#elif SLUG_LINUX

			static const int formatAttributes[] =
			{
				GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
				GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
				GLX_DOUBLEBUFFER, True,
				GLX_RENDER_TYPE, GLX_RGBA_BIT,
				GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, True,
				GLX_RED_SIZE, 8,
				GLX_GREEN_SIZE, 8,
				GLX_BLUE_SIZE, 8,
				GLX_ALPHA_SIZE, 8,
				None
			};

			static const int contextAttributes[] =
			{
				GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
				GLX_CONTEXT_MINOR_VERSION_ARB, 3,
				GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
				0, 0
			};

			int						configCount;
			XSetWindowAttributes	windowAttributes;

			GETGLPROC(glXCreateContextAttribsARB);
			GETGLPROC(glXSwapIntervalEXT);

			if (!glXCreateContextAttribsARB)
			{
				return (false);
			}

			GLXFBConfig *configList = glXChooseFBConfig(display, DefaultScreen(display), formatAttributes, &configCount);
			if ((!configList) || (configCount < 1))
			{
				return (false);
			}

			XVisualInfo *visualInfo = glXGetVisualFromFBConfig(display, configList[0]);
			if (!visualInfo)
			{
				return (false);
			}

			openglColormap = XCreateColormap(display, DefaultRootWindow(display), visualInfo->visual, AllocNone);

			windowAttributes.event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
			windowAttributes.override_redirect = true;
			windowAttributes.colormap = openglColormap;

			openglWindow = XCreateWindow(display, DefaultRootWindow(display), 0, 0, screenWidth, screenHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWEventMask | CWOverrideRedirect | CWColormap, &windowAttributes);
			openglContext = glXCreateContextAttribsARB(display, configList[0], nullptr, true, contextAttributes);

			XFree(visualInfo);
			XFree(configList);

			if (!openglContext)
			{
				XDestroyWindow(display, openglWindow);
				XFreeColormap(display, openglColormap);
				return (false);
			}

			if (!glXMakeCurrent(display, openglWindow, openglContext))
			{
				XDestroyWindow(display, openglWindow);
				XFreeColormap(display, openglColormap);
				return (false);
			}

			XMapWindow(display, openglWindow);
			XSetInputFocus(display, openglWindow, RevertToPointerRoot, CurrentTime);

		#endif

		// Retrieve pointers to all the functions we need that aren't in opengl32.lib.

		#if SLUG_WINDOWS

			GETGLPROC(glActiveTexture);

		#endif

		#if SLUG_WINDOWS || SLUG_LINUX

			GETGLPROC(glMultiDrawElements);
			GETGLPROC(glEnableVertexAttribArray);
			GETGLPROC(glDisableVertexAttribArray);
			GETGLPROC(glVertexAttribPointer);
			GETGLPROC(glGenBuffers);
			GETGLPROC(glDeleteBuffers);
			GETGLPROC(glBindBuffer);
			GETGLPROC(glBufferData);
			GETGLPROC(glMapBuffer);
			GETGLPROC(glUnmapBuffer);
			GETGLPROC(glCreateShader);
			GETGLPROC(glDeleteShader);
			GETGLPROC(glShaderSource);
			GETGLPROC(glCompileShader);
			GETGLPROC(glAttachShader);
			GETGLPROC(glCreateProgram);
			GETGLPROC(glDeleteProgram);
			GETGLPROC(glLinkProgram);
			GETGLPROC(glUseProgram);
			GETGLPROC(glGetUniformLocation);
			GETGLPROC(glUniform1i);
			GETGLPROC(glUniform4f);
			GETGLPROC(glUniform4fv);
			GETGLPROC(glGenQueries);
			GETGLPROC(glDeleteQueries);
			GETGLPROC(glBeginQuery);
			GETGLPROC(glEndQuery);

		#endif

		GETGLPROC(glGenVertexArrays);
		GETGLPROC(glDeleteVertexArrays);
		GETGLPROC(glBindVertexArray);
		GETGLPROC(glGetQueryObjectui64v);

		// Turn off V-sync, or else GPUs can throttle down to save power because we're going way faster than 60 fps,
		// resulting in us not getting accurate timings. This may not work if a control panel setting overrides it.

		#if SLUG_WINDOWS

			wglSwapIntervalEXT(0);

		#elif SLUG_MACOS

			GLint swapInterval = 0;
			[openglContext setValues: &swapInterval forParameter: NSOpenGLCPSwapInterval];

		#elif SLUG_LINX

			if (glXSwapIntervalEXT)
			{
				glXSwapIntervalEXT(display, openglWindow, 0);
			}

		#endif

		rendererDescription = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

		// Create a global VAO, bind it, and forget about it.

		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		// Generate query names for our timers.

		glGenQueries(4, timerQueryName);

		// The slug shaders produce colors with premultiplied alpha.

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		// Slug outputs linear color, so have the hardware convert to sRGB.

		glEnable(GL_FRAMEBUFFER_SRGB);

		// We're not using a depth buffer in this demo, but note that depth operations should be disabled while rendering text.

		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);

		return (true);
	}

	#if SLUG_WINDOWS

		void TermGraphics(HWND window)

	#elif SLUG_MACOS

		void TermGraphics(void)

	#elif SLUG_LINUX

		void TermGraphics(Display *display)

	#endif

	{
		glDeleteQueries(4, timerQueryName);
		glDeleteVertexArrays(1, &vertexArrayObject);

		#if SLUG_WINDOWS

			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(openglContext);

			ReleaseDC(window, deviceContext);

		#elif SLUG_MACOS

			[NSOpenGLContext clearCurrentContext];
			[openglContext release];

			CFRelease(openglBundle);

		#elif SLUG_LINUX

			glXMakeCurrent(display, None, nullptr);
			glXDestroyContext(display, openglContext);

			XDestroyWindow(display, openglWindow);
			XFreeColormap(display, openglColormap);

		#endif
	}

#elif SLUG_D3D11

	bool InitGraphics(HINSTANCE instance, HWND window)
	{
		static const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;

		DXGI_ADAPTER_DESC			adapterDesc;
		DXGI_SWAP_CHAIN_DESC		swapChainDesc;
		D3D11_VIEWPORT				viewport;
		D3D11_QUERY_DESC			queryDesc[2];
		D3D11_RASTERIZER_DESC		rasterizerDesc;
		D3D11_BLEND_DESC			blendDesc;
		D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
		IDXGIDevice					*dxgiDevice;
		IDXGIAdapter				*adapter;
		ID3D11Texture2D				*backBuffer;
		ID3D11RasterizerState		*rasterizerState;
		ID3D11BlendState			*blendState;
		ID3D11DepthStencilState		*depthStencilState;

		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;		// Slug outputs linear color, so have the hardware convert to sRGB.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = window;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &d3dSwapChain, &d3dDevice, nullptr, &d3dContext);
		if (FAILED(result))
		{
			return (false);
		}

		result = d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **) &backBuffer);
		if (FAILED(result))
		{
			d3dSwapChain->Release();
			d3dDevice->Release();
			return (false);
		}

		result = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &d3dTargetView);
		if (FAILED(result))
		{
			d3dSwapChain->Release();
			d3dDevice->Release();
			return (false);
		}

		d3dContext->OMSetRenderTargets(1, &d3dTargetView, nullptr);

		d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **) &dxgiDevice);
		dxgiDevice->GetAdapter(&adapter);

		adapter->GetDesc(&adapterDesc);
		for (machine a = 0;; a++)
		{
			WCHAR w = adapterDesc.Description[a];
			rendererDescription[a] = char(w);		// Just assume ASCII.
			if (w == 0) break;
		}

		adapter->Release();
		dxgiDevice->Release();

		viewport.Width = float(screenWidth);
		viewport.Height = float(screenHeight);
		viewport.MinDepth = 0.0F;
		viewport.MaxDepth = 1.0F;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		d3dContext->RSSetViewports(1, &viewport);

		queryDesc[0].Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		queryDesc[0].MiscFlags = 0;

		queryDesc[1].Query = D3D11_QUERY_TIMESTAMP;
		queryDesc[1].MiscFlags = 0;

		for (machine a = 0; a < 4; a++)
		{
			d3dDevice->CreateQuery(&queryDesc[0], &disjointQuery[a]);
			d3dDevice->CreateQuery(&queryDesc[1], &beginTimeQuery[a]);
			d3dDevice->CreateQuery(&queryDesc[1], &endTimeQuery[a]);
		}

		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0F;
		rasterizerDesc.SlopeScaledDepthBias = 0.0F;
		rasterizerDesc.DepthClipEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = false;

		d3dDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
		d3dContext->RSSetState(rasterizerState);
		rasterizerState->Release();

		// The slug shaders produce colors with premultiplied alpha.

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		d3dDevice->CreateBlendState(&blendDesc, &blendState);
		d3dContext->OMSetBlendState(blendState, nullptr, ~0U);
		blendState->Release();

		// We're not using a depth buffer in this demo, but note that depth operations should be disabled while rendering text.

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
		d3dContext->OMSetDepthStencilState(depthStencilState, 0);
		depthStencilState->Release();

		return (true);
	}

	void TermGraphics(HWND window)
	{
		for (machine a = 0; a < Slug::kMaxVertexShaderCount; a++)
		{
			if (vertexBlob[a]) vertexBlob[a]->Release();
		}

		for (machine a = 0; a < Slug::kMaxPixelShaderCount; a++)
		{
			if (pixelBlob[a]) pixelBlob[a]->Release();
		}

		for (machine a = 3; a >= 0; a--)
		{
			endTimeQuery[a]->Release();
			beginTimeQuery[a]->Release();
			disjointQuery[a]->Release();
		}

		d3dTargetView->Release();
		d3dSwapChain->Release();
		d3dContext->Release();
		d3dDevice->Release();
	}

#endif

#if SLUG_WINDOWS

	bool HandleKeyboardEvent(UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (wparam == VK_ESCAPE)
		{
			quitFlag = true;
		}
		else if (wparam == VK_TAB)
		{
			weightFlag = true;
		}
		else if (wparam == VK_SPACE)
		{
			currentScene = (currentScene + 1) % Demo::kSceneCount;
		}
		else if (wparam == VK_F1)
		{
			ResetView();
		}

		return (true);
	}

	bool HandleMouseEvent(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (message == WM_LBUTTONDOWN)
		{
			leftButtonFlag = true;
		}
		else if ((message == WM_LBUTTONUP) || (message == WM_NCLBUTTONUP))
		{
			leftButtonFlag = false;
		}
		else if (message == WM_RBUTTONDOWN)
		{
			rightButtonFlag = true;
		}
		else if ((message == WM_RBUTTONUP) || (message == WM_NCRBUTTONUP))
		{
			rightButtonFlag = false;
		}
		else if (message == WM_MOUSEWHEEL)
		{
			int32 delta = GET_WHEEL_DELTA_WPARAM(wparam);
			HandleMouseWheel(float(delta) * -0.001F);
		}
		else if ((message == WM_MOUSEMOVE) || (WM_NCMOUSEMOVE))
		{
			float x = float(LOWORD(lparam));
			float y = float(HIWORD(lparam));
			float dx = x - lastMouseX;
			float dy = y - lastMouseY;

			if (leftButtonFlag)
			{
				HandleLeftMouseDrag(dx, dy);
			}
			else if (rightButtonFlag)
			{
				HandleRightMouseDrag(dx, dy);
			}

			lastMouseX = x;
			lastMouseY = y;
		}

		return (true);
	}

	LRESULT WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		switch (message)
		{
			case WM_CLOSE:

				quitFlag = true;
				return (0);

			case WM_ACTIVATE:

				#if !SLUG_DEBUG

					if (wparam != 0)
					{
						SetWindowPos(demoWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE);
					}
					else
					{
						SetWindowPos(demoWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE);
					}

				#endif

				return (0);

			case WM_KEYDOWN:

				if (HandleKeyboardEvent(message, wparam, lparam))
				{
					return (0);
				}

				break;

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_NCLBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_NCRBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_MOUSEMOVE:
			case WM_NCMOUSEMOVE:
			
				if (HandleMouseEvent(window, message, wparam, lparam))
				{
					return (0);
				}

				break;
		}

		return (DefWindowProc(window, message, wparam, lparam));
	}

#elif SLUG_MACOS

	void HandleMouseEvent(NSEvent *event)
	{
		NSPoint position = [event locationInWindow];

		NSEventType type = [event type];
		if (type == NSEventTypeLeftMouseDown)
		{
			leftButtonFlag = true;
		}
		else if (type == NSEventTypeLeftMouseUp)
		{
			leftButtonFlag = false;
		}
		else if (type == NSEventTypeRightMouseDown)
		{
			rightButtonFlag = true;
		}
		else if (type == NSEventTypeRightMouseUp)
		{
			rightButtonFlag = false;
		}
		else if ((type == NSEventTypeLeftMouseDragged) || (type == NSEventTypeRightMouseDragged))
		{
			float dx = position.x - lastMouseX;
			float dy = position.y - lastMouseY;

			if (leftButtonFlag)
			{
				HandleLeftMouseDrag(dx, -dy);
			}
			else if (rightButtonFlag)
			{
				HandleRightMouseDrag(dx, -dy);
			}
		}
		else if (type == NSEventTypeScrollWheel)
		{
			float delta = ([event hasPreciseScrollingDeltas]) ? [event scrollingDeltaY] * 0.1F : [event deltaY];
			HandleMouseWheel(delta * -0.1F);
		}

		lastMouseX = position.x;
		lastMouseY = position.y;
	}

	void HandleKeyboardEvent(NSEvent *event)
	{
		if ([event type] == NSEventTypeKeyDown)
		{
			NSString *string = [event characters];
			if ([string length] > 0)
			{
				unichar code = [string characterAtIndex: 0];

				if (code == 27)
				{
					quitFlag = true;
				}
				else if (code == 9)
				{
					weightFlag = true;
				}
				else if (code == ' ')
				{
					currentScene = (currentScene + 1) % Demo::kSceneCount;
				}
				else if (code == NSF1FunctionKey)
				{
					ResetView();
				}
			}
		}
	}

#elif SLUG_LINUX

	void HandleKeyboardEvent(const XKeyEvent *event)
	{
		char		string[4];
		KeySym		symbol;

		XLookupString(const_cast<XKeyEvent *>(event), string, 4, &symbol, nullptr);

		if (symbol == XK_Escape)
		{
			quitFlag = true;
		}
		else if (symbol == XK_Tab)
		{
			weightFlag = true;
		}
		else if (symbol == XK_space)
		{
			currentScene = (currentScene + 1) % Demo::kSceneCount;
		}
		else if (symbol == XK_F1)
		{
			ResetView();
		}
	}

	void HandleMouseButtonEvent(const XButtonEvent *event)
	{
		if (event->type == ButtonPress)
		{
			if (event->button == Button1)
			{
				leftButtonFlag = true;
			}
			if (event->button == Button3)
			{
				rightButtonFlag = true;
			}
			else if ((event->button == Button4) || (event->button == Button5))
			{
				int32 delta = (event->button == Button4) ? 1 : -1;
				HandleMouseWheel(float(delta) * -0.2F);
			}
		}
		else
		{
			if (event->button == Button1)
			{
				leftButtonFlag = false;
			}
			else if (event->button == Button3)
			{
				rightButtonFlag = false;
			}
		}
	}

	void HandleMouseMotionEvent(const XMotionEvent *event)
	{
		int32 x = float(event->x);
		int32 y = float(event->y);
		float dx = x - lastMouseX;
		float dy = y - lastMouseY;

		if (leftButtonFlag)
		{
			HandleLeftMouseDrag(dx, dy);
		}
		else if (rightButtonFlag)
		{
			HandleRightMouseDrag(dx, dy);
		}

		lastMouseX = x;
		lastMouseY = y;
	}

#endif

void InitDemo(void)
{
	const float aspectRatio = float(screenWidth) / float(screenHeight);
	moveFactor = 2.0F / (focalLength * float(screenWidth));

	// Create a perspective projection with an infinite far plane for the main scene.
	// Our camera space has x pointing right, y pointing down, and z pointing into the screen.

	sceneProjection(0,1) = sceneProjection(0,2) = sceneProjection(0,3) = sceneProjection(1,0) = sceneProjection(1,2) = sceneProjection(1,3) = sceneProjection(2,0) = sceneProjection(2,1) = sceneProjection(3,0) = sceneProjection(3,1) = sceneProjection(3,3) = 0.0F;
	sceneProjection(2,2) = sceneProjection(3,2) = 1.0F;
	sceneProjection(0,0) = focalLength;
	sceneProjection(1,1) = -focalLength * aspectRatio;

	#if SLUG_OPENGL

		sceneProjection(2,3) = nearDistance * -2.0F;

	#elif SLUG_D3D11

		sceneProjection(2,3) = nearDistance;

	#endif

	// Create an orthographic projection for the overlay.

	overlayProjection(0,1) = overlayProjection(0,2) = overlayProjection(1,0) = overlayProjection(1,2) = overlayProjection(2,0) = overlayProjection(2,1) = overlayProjection(2,3) = overlayProjection(3,0) = overlayProjection(3,1) = overlayProjection(3,2) = 0.0F;
	overlayProjection(2,2) = overlayProjection(3,3) = 1.0F;
	overlayProjection(0,0) = 2.0F / float(screenWidth);
	overlayProjection(0,3) = -1.0F;
	overlayProjection(1,1) = -2.0F / float(screenHeight);
	overlayProjection(1,3) = 1.0F;

	// Create all of the Text objects.

	InitSceneText(sceneTextArray, sceneOrthoFlag, extraLayoutFlags);
	InitOverlayText(screenWidth, screenHeight, overlayTextArray, &timeText, extraLayoutFlags);

	ResetView();
}

void TermDemo(void)
{
	TermOverlayText(overlayTextArray);
	TermSceneText(sceneTextArray);
}

bool RunDemo(void)
{
	for (;;)
	{
		#if SLUG_WINDOWS

			MSG		message;

			while (PeekMessageW(&message, demoWindow, 0, 0, PM_REMOVE))
			{
				DispatchMessageW(&message);
			}

		#elif SLUG_LINUX

			while (XPending(mainDisplay))
			{
				XEvent		event;

				XNextEvent(mainDisplay, &event);
				switch (event.type)
				{
					case ButtonPress:
					case ButtonRelease:

						HandleMouseButtonEvent(&event.xbutton);
						break;

					case MotionNotify:

						HandleMouseMotionEvent(&event.xmotion);
						break;

					case KeyPress:

						HandleKeyboardEvent(&event.xkey);
						break;
				}
			}

		#endif

		if (quitFlag)
		{
			break;
		}

		if (weightFlag)
		{
			weightFlag = false;
			extraLayoutFlags ^= Slug::kLayoutOpticalWeight;

			TermOverlayText(overlayTextArray);
			TermSceneText(sceneTextArray);

			InitSceneText(sceneTextArray, sceneOrthoFlag, extraLayoutFlags);
			InitOverlayText(screenWidth, screenHeight, overlayTextArray, &timeText, extraLayoutFlags);
		}

		#if SLUG_OPENGL

			glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
			glClear(GL_COLOR_BUFFER_BIT);

		#elif SLUG_D3D11

			float color[4] = {1.0F, 1.0F, 1.0F, 1.0F};
			d3dContext->ClearRenderTargetView(d3dTargetView, color);

		#endif

		// Draw main scene with GPU timestamps before and after.

		unsigned_int32 index = frameIndex & 3;
		
		#if SLUG_OPENGL

			glBeginQuery(GL_TIME_ELAPSED, timerQueryName[index]);

		#elif SLUG_D3D11

			d3dContext->Begin(disjointQuery[index]);
			d3dContext->End(beginTimeQuery[index]);

		#endif

		Demo::Render(sceneTextArray[currentScene], sceneOrthoFlag[currentScene] ? overlayProjection : sceneProjection * Inverse(viewTransform));

		#if SLUG_OPENGL

			glEndQuery(GL_TIME_ELAPSED);

		#elif SLUG_D3D11

			d3dContext->End(endTimeQuery[index]);
			d3dContext->End(disjointQuery[index]);

		#endif

		if (frameIndex >= 3)
		{
			static char timeString[32] = "GPU Time: ";

			// Display the time for three frames in the past so that we
			// don't stall waiting for the query results to be available.

			index = (index - 3) & 3;

			#if SLUG_OPENGL

				GLuint64	queryTime;

				glGetQueryObjectui64v(timerQueryName[index], GL_QUERY_RESULT, &queryTime);
				int32 time = int32(queryTime / 10000);		// In tens of microseconds.

			#elif SLUG_D3D11

				D3D11_QUERY_DATA_TIMESTAMP_DISJOINT		disjointData;
				UINT64									beginTime, endTime;

				while (d3dContext->GetData(disjointQuery[index], &disjointData, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0) != S_OK)
				{
					Sleep(1);
				}

				d3dContext->GetData(beginTimeQuery[index], &beginTime, 8, 0);
				d3dContext->GetData(endTimeQuery[index], &endTime, 8, 0);

				int32 time = (!disjointData.Disjoint) ? int32((endTime - beginTime) * 100000 / disjointData.Frequency) : 0;

			#endif

			int32 len = 10;
			if (time > 0)
			{
				time = (time < 99999) ? time : 99999;

				len += Demo::UintToString(time / 100, &timeString[len], 31 - len);
				timeString[len++] = '.';
				timeString[len++] = char((time / 10) % 10 + 48);
				timeString[len++] = char(time % 10 + 48);
				timeString[len++] = ' ';
				timeString[len++] = 'm';
				timeString[len++] = 's';
			}

			timeString[len] = 0;
			timeText->Build(timeString);
			Demo::Render(overlayTextArray, overlayProjection);
		}

		frameIndex++;

		#if SLUG_WINDOWS

			#if SLUG_OPENGL

				SwapBuffers(deviceContext);

			#elif SLUG_D3D11

				d3dSwapChain->Present(0, 0);

			#endif

		#elif SLUG_MACOS

			[openglContext flushBuffer];

		#elif SLUG_LINUX

			glXSwapBuffers(mainDisplay, openglWindow);

		#endif

		#if SLUG_MACOS

			return (true);

		#endif
	}

	return (false);
}

#if SLUG_WINDOWS

	int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int cmdShow)
	{
		WNDCLASSEXW		windowClass;

		static const wchar_t applicationName[] = L"SlugDemo";

		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_OWNDC;
		windowClass.lpfnWndProc = &WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = instance;
		windowClass.hIcon = nullptr;
		windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = applicationName;
		windowClass.hIconSm = nullptr;
		RegisterClassExW(&windowClass);
	
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);
		RECT rect = {0, 0, screenWidth, screenHeight};

		#if SLUG_DEBUG

			DWORD exStyle = 0;

		#else

			DWORD exStyle = WS_EX_TOPMOST;

		#endif

		DWORD style = WS_VISIBLE | WS_POPUPWINDOW;
		AdjustWindowRectEx(&rect, style, false, exStyle);
		demoWindow = CreateWindowExW(exStyle, applicationName, applicationName, style, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, instance, nullptr);
	
		SetFocus(demoWindow);
		SetCursor(windowClass.hCursor);

		if (InitGraphics(instance, demoWindow))
		{
			InitDemo();
			RunDemo();
			TermDemo();
			TermGraphics(demoWindow);
		}

		DestroyWindow(demoWindow);
		UnregisterClassW(applicationName, instance);
		return (0);
	}

#elif SLUG_MACOS

	@interface WindowDelegate : NSObject<NSWindowDelegate>
	{
	}
	@end

	@implementation WindowDelegate

	- (BOOL) windowShouldClose: (id) sender
	{
		quitFlag = true;
		return (NO);
	}

	- (NSApplicationPresentationOptions) window: (NSWindow *) window willUseFullScreenPresentationOptions: (NSApplicationPresentationOptions) proposedOptions
	{
		return (NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar | NSApplicationPresentationFullScreen);
	}

	@end

	@interface DemoWindow : NSWindow
	{
		@public

			WindowDelegate		*windowDelegate;
	}
	@end

	@implementation DemoWindow

	- (void) mouseDown: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) mouseUp: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) mouseDragged: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) rightMouseDown: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) rightMouseUp: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) rightMouseDragged: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) mouseMoved: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) scrollWheel: (NSEvent *) event
	{
		HandleMouseEvent(event);
	}

	- (void) keyDown: (NSEvent *) event
	{
		HandleKeyboardEvent(event);
	}

	- (BOOL) canBecomeKeyWindow
	{
		return (YES);
	}

	- (BOOL) canBecomeMainWindow
	{
		return (YES);
	}

	@end

	@interface DemoDelegate : NSObject<NSApplicationDelegate>
	{
		@private

			NSTimer		*demoTimer;
	}
	@end

	@implementation DemoDelegate

	- (void) applicationWillFinishLaunching: (NSNotification *) notification
	{
		NSRect rect = [[NSScreen mainScreen] frame];
		screenWidth = NSWidth(rect);
		screenHeight = NSHeight(rect);

		DemoWindow *window = [[DemoWindow alloc] initWithContentRect: rect styleMask: NSBorderlessWindowMask backing: NSBackingStoreBuffered defer: NO];
		demoWindow = window;

		WindowDelegate *delegate = [WindowDelegate alloc];
		[window setDelegate: delegate];
		window->windowDelegate = delegate;

		[window setOpaque: YES];
		[window setReleasedWhenClosed: NO];
		[window setAcceptsMouseMovedEvents: YES];
		[window setAnimationBehavior: NSWindowAnimationBehaviorNone];
		[window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];

		NSOpenGLView *view = [[NSOpenGLView alloc] initWithFrame: rect];
		openglView = view;

		[view addTrackingRect: rect owner: window userData: nil assumeInside: NO];
		[window setContentView: view];
		[view release];

		[window makeMainWindow];
		[window makeKeyAndOrderFront: nil];
		[window toggleFullScreen: nil];

		demoTimer = [NSTimer scheduledTimerWithTimeInterval: 0.0 target: self selector: @selector(runLoop:) userInfo: nil repeats: YES];

		if (InitGraphics())
		{
			NSString *string = [[NSBundle mainBundle] resourcePath];
			string = [string stringByAppendingString: @"/Font"];
			chdir([string UTF8String]);

			InitDemo();
		}
		else
		{
			quitFlag = true;
		}
	}

	- (void) applicationWillTerminate: (NSNotification *) notification
	{
		TermDemo();
		TermGraphics();

		[demoTimer invalidate];
		[demoWindow close];
		[demoWindow release];
	}

	- (void) runLoop: (NSTimer *) timer
	{
		if (!RunDemo())
		{
			[NSApp terminate: nil];
		}
	}

	@end

	int main(int argc, const char **argv)
	{
		return (NSApplicationMain(argc, argv));
	}

#elif SLUG_LINUX

	int main(int argc, const char **argv)
	{
		Display *display = XOpenDisplay(nullptr);
		Screen *screen = ScreenOfDisplay(display, DefaultScreen(display));
		mainDisplay = display;

		screenWidth = WidthOfScreen(screen);
		screenHeight = HeightOfScreen(screen);

		if (InitGraphics(screenWidth, screenHeight, display))
		{
			InitDemo();
			RunDemo();
			TermDemo();
			TermGraphics(display);
		}

		XCloseDisplay(display);
		return (0);
	}

#endif
