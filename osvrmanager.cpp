#include "osvrmanager.h"

OSVRManager::OSVRManager() :
    context("com.osvr.janusvr"),
    render(NULL),
    flip_vertical(false)
{
}

bool OSVRManager::Initialize()
{
    // Open OpenGL and set up the context for rendering to
    // an HMD.  Do this using the OSVR RenderManager interface,
    // which maps to the nVidia or other vendor direct mode
    // to reduce the latency.
    render = osvr::renderkit::createRenderManager(context.get(), "OpenGL");
    if ((render == nullptr) || (!render->doingOkay())) {
        qDebug() << "OSVRManager::Initialize() - Could not create RenderManager";
        return false;
    }
    else {
        qDebug() << "OSVRManager::Initialize() - createRenderManager OpenGL succeeded";
    }

    return true;
}

void OSVRManager::PostPresent()
{

}

void OSVRManager::InitializeGL()
{
    // Open the display and make sure this worked.
    osvr::renderkit::RenderManager::OpenResults ret = render->OpenDisplay();
    if (ret.status == osvr::renderkit::RenderManager::OpenStatus::FAILURE) {
        qDebug() << "OSVRManager::InitializeGL() - Could not open display";
        delete render;
        render = NULL;
        return;
    }

    //SetupRendering
    if (ret.library.OpenGL == nullptr) {
        qDebug() << "OSVRManager::InitializeGL() - No OpenGL GraphicsLibrary, this should not happen";
        return;
    }

    // Do a call to get the information we need to construct our
    // color and depth render-to-texture buffers.
    Update();   
}

bool OSVRManager::GetEnabled() const
{
    return context.checkStatus();
}

QSize OSVRManager::GetTextureSize() const
{    
    if (renderInfo.size() > 0) {
        int width = static_cast<int>(renderInfo[0].viewport.width);
        int height = static_cast<int>(renderInfo[0].viewport.height);
        return QSize(width, height);
    }
    return QSize(2560/2, 1440);
}

float OSVRManager::GetIPD() const
{
    return 0.0f;
}

QString OSVRManager::GetHMDString() const
{
    return "";
}

QString OSVRManager::GetHMDType() const
{
    return "osvr";
}

void OSVRManager::Update()
{
    context.update();
    if (render) {
        renderInfo = render->GetRenderInfo();
    }
}

QMatrix4x4 OSVRManager::GetHMDTransform() const
{
//    qDebug() << "OSVRManager::GetHMDTransform() proj" << projM << "view" << viewM;
    return viewM;
}

QMatrix4x4 OSVRManager::GetControllerTransform(const int i) const
{
    return QMatrix4x4();
}

QMatrix4x4 OSVRManager::GetLastControllerTransform(const int i) const
{
    return QMatrix4x4();
}

int OSVRManager::GetNumControllers() const
{
    return 0; //placeholder
}

bool OSVRManager::GetControllerTracked(const int i)
{
    return false; //placeholder
}

QVector2D OSVRManager::GetControllerThumb(const int i) const
{
    return QVector2D(0,0);
}

bool OSVRManager::GetControllerThumbTouched(const int i) const
{
    return false;
}

bool OSVRManager::GetControllerThumbPressed(const int i) const
{
    return false;
}

float OSVRManager::GetControllerTrigger(const int i) const
{
    return 0.0f;
}

float OSVRManager::GetControllerGrip(const int i) const
{
    return 0.0f;
}

bool OSVRManager::GetControllerMenuPressed(const int i) const
{
    return false;
}

bool OSVRManager::GetControllerButtonAPressed() const
{
    return false;
}

bool OSVRManager::GetControllerButtonBPressed() const
{
    return false;
}

bool OSVRManager::GetControllerButtonXPressed() const
{
    return false;
}

bool OSVRManager::GetControllerButtonYPressed() const
{
    return false;
}

bool OSVRManager::GetControllerButtonATouched() const
{
    return false;
}

bool OSVRManager::GetControllerButtonBTouched() const
{
    return false;
}

bool OSVRManager::GetControllerButtonXTouched() const
{
    return false;
}

bool OSVRManager::GetControllerButtonYTouched() const
{
    return false;
}

void OSVRManager::BeginRenderEye(const int eye)
{
    if (renderInfo.size() <= eye) {
        qDebug() << "OSVRManager::BeginRenderEye() - Problem!  renderInfo size too small: " << renderInfo.size();
        return;
    }

    // Set the OpenGL projection/modeview matrices
    GLdouble projection[16];
    GLfloat projectionf[16];
    GLdouble modelView[16];
    GLfloat modelViewf[16];
    osvr::renderkit::OSVR_Projection_to_OpenGL(projection, renderInfo[eye].projection);
    osvr::renderkit::OSVR_PoseState_to_OpenGL(modelView, renderInfo[eye].pose);
    for (int i=0; i<16; ++i) {
        projectionf[i] = projection[i];
        modelViewf[i] = modelView[i];
    }

    projM = QMatrix4x4(projectionf).transposed();
    if (flip_vertical) {
        projM.setColumn(1, -projM.column(1));
    }
    viewM = QMatrix4x4(modelViewf).transposed().inverted();

    MathUtil::LoadProjectionMatrix(projM);
    MathUtil::LoadModelIdentity();
}

void OSVRManager::EndRenderEye(const int eye)
{

}

void OSVRManager::BeginRendering()
{
    if (render == NULL || !render->doingOkay()) {
        qDebug() << "OSVRManager::BeginRendering() - render null or not OK";
        return;
    }

    //Register framebuffers if we have not already done so
    // Register our constructed buffers so that we can use them for
    // presentation.
    GLuint colorBufferName = RendererInterface::m_pimpl->GetTextureID(FBO_TEXTURE::COLOR, false);
    if (colorBuffers.empty() || (colorBuffers[0].OpenGL->colorBufferName != colorBufferName)) {
        osvr::renderkit::RenderBuffer rb;
        rb.OpenGL = new osvr::renderkit::RenderBufferOpenGL;
        rb.OpenGL->colorBufferName = colorBufferName;

        colorBuffers.push_back(rb);
        colorBuffers.push_back(rb);

        if (!render->RegisterRenderBuffers(colorBuffers)) {
            qDebug() << "OSVRManager::BeginRendering() - RegisterRenderBuffers() returned false, cannot continue";
            return;
        }
        else {
            qDebug() << "OSVRManager::BeginRendering() - Registered render buffer";
        }
    }
}

void OSVRManager::EndRendering()
{
    if (render == NULL || !render->doingOkay()) {
        qDebug() << "OSVRManager::EndRendering() - render null or not OK";
        return;
    }

    const int w = RendererInterface::m_pimpl->GetWindowWidth();
    const int h = RendererInterface::m_pimpl->GetWindowHeight();

    //This blits the multisampled framebuffer to the framebuffer which contains a texture for the OpenVR SDK
    RendererInterface::m_pimpl->BlitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD::COLOR, 0, 0, w, h, 0, 0, w, h);
//    RendererInterface::m_pimpl->BlitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD::COLOR, 0, 0, w, h, 0, h, w, -h); //cannot flip vertically
    // Bind nothing to the framebuffer so that we can access the textures of our FBO
    MathUtil::glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Send the rendered results to the screen
    std::vector<osvr::renderkit::OSVR_ViewportDescription> viewports;
    osvr::renderkit::OSVR_ViewportDescription v0;
    osvr::renderkit::OSVR_ViewportDescription v1;

    //Note - this inverts vertically!
    const QSize s = GetTextureSize();
    v0.width = 0.5f;
    v0.height = -1.0f;
    v0.left = 0.0f;
    v0.lower = 1.0f;
    v1.width = 0.5f;
    v1.height = -1.0f;
    v1.left = 0.5f;
    v1.lower = 1.0f;

    viewports.push_back(v0);
    viewports.push_back(v1);

    if (!render->PresentRenderBuffers(colorBuffers,
                                      renderInfo,
                                      osvr::renderkit::RenderManager::RenderParams(),
                                      viewports)) {
        qDebug() << "OSVRManager::EndRendering() - PresentRenderBuffers() returned false, maybe because it was asked to quit";
    }
}

void OSVRManager::ReCentre()
{

}

void OSVRManager::TriggerHapticPulse(const int i, const int val)
{
    Q_UNUSED(i)
    Q_UNUSED(val)
    //no-op
}

void OSVRManager::Platform_ProcessMessages()
{

}

bool OSVRManager::Platform_GetEntitled() const
{
    return true;
}

bool OSVRManager::Platform_GetShouldQuit() const
{
    return false;
}
