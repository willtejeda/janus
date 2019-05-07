#include "glwidget.h"

bool GLWidget::novsync = false;
DisplayMode GLWidget::disp_mode = MODE_AUTO;

GLWidget::GLWidget()
{
    snap_mouse = true;
    grabbed = false;
    counted_frames = 0;
    fps = 0;
    framerate_time.start();

    take_screenshot = false;
    take_screenshot_cubemap = false;
    take_screenthumb = false;
    take_bookmark = false;

#ifndef __ANDROID__
    QSurfaceFormat format;   
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);

#if defined(QT_DEBUG)
    format.setOption(QSurfaceFormat::DebugContext);
#endif
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);

    //set vsync appropriately
    if (novsync || (disp_mode == MODE_RIFT || disp_mode == MODE_VIVE)) {
        format.setSwapInterval(0); //this disables vsync
    }

    QSurfaceFormat::setDefaultFormat(format);
    setFormat(format);
#else
    setFormat(QSurfaceFormat::defaultFormat());

    last_mouse_pos = GetLocalWinCentre();
    mouse_pos = GetLocalWinCentre();
#endif
}

GLWidget::~GLWidget()
{
//    qDebug() << "GLWidget::~GLWidget()";
    //delete RendererInterface::m_pimpl;
}

void GLWidget::SetGame(Game * g)
{
    game = g;
#ifdef __ANDROID__
    grabGesture(Qt::PinchGesture);
#endif
}

void GLWidget::SetHMDManager(AbstractHMDManager* h)
{
    hmd_manager = h;
    RendererInterface::m_pimpl->InitializeHMDManager(hmd_manager);
}

void GLWidget::SetDisplayMode(const DisplayMode d)
{
    disp_mode = d;
}

DisplayMode GLWidget::GetDisplayMode()
{
    return disp_mode;
}

void GLWidget::SetNoVSync(const bool b)
{
    novsync = b;

    MathUtil::m_frame_limiter_render_thread = (b == true) ? 0 : 1;
}

bool GLWidget::GetNoVSync()
{
    return novsync;
}

void GLWidget::SetDefaultProjectionPersp(const float fov, const float aspect, const float near_dist, const float far_dist)
{
    MathUtil::LoadProjectionIdentity();
    MathUtil::ProjectionMatrix().perspective(fov, aspect, near_dist, far_dist);
}

void GLWidget::SetDefaultProjectionOrtho()
{
    MathUtil::LoadProjectionIdentity();
    MathUtil::ProjectionMatrix().ortho(0,1,0,1,-1,1);
}

void GLWidget::SetGrab(const bool b)
{
//    qDebug() << "GLWidget::SetGrab" << b;
    grabbed = b;

    if (b) {
        QCursor::setPos(GetWinCentre());
#ifdef WIN32
        if (QApplication::overrideCursor() == 0) {
            QApplication::setOverrideCursor(Qt::BlankCursor);
        }
        setCursor(QCursor(Qt::BlankCursor));
        grabMouse();
        grabKeyboard();
        setMouseTracking(true);
        setFocus();
#else
//#ifndef QT_DEBUG
        setCursor(QCursor(Qt::BlankCursor));
        grabMouse();
        grabKeyboard();
        setMouseTracking(true);
        setFocus();
//#endif
#endif
    }
    else {
#ifdef __ANDROID__
        last_mouse_pos = GetWinCentre();
        mouse_pos = GetWinCentre();
#endif
        QCursor::setPos(GetWinCentre());
        QApplication::restoreOverrideCursor();
        setCursor(QCursor(Qt::ArrowCursor));
        releaseKeyboard();
        releaseMouse();
        setMouseTracking(false);
        clearFocus();
        game->GetPlayer()->SetWalkBack(false);
        game->GetPlayer()->SetWalkForward(false);
        game->GetPlayer()->SetWalkLeft(false);
        game->GetPlayer()->SetWalkRight(false);
    }
}

bool GLWidget::GetGrab() const
{
    return grabbed;
}

QPoint GLWidget::GetWinCentre()
{
    return mapToGlobal(GetLocalWinCentre());
}

QPoint GLWidget::GetLocalWinCentre()
{
    return QPoint(width()/2,height()/2);
}

void GLWidget::DoSaveThumb(const QString path)
{
    take_screenthumb = true;
    take_screenshot_path = path;
    MathUtil::m_last_screenshot_path = path;
}

void GLWidget::DoSaveScreenshot(const QString path)
{
    take_screenshot = true;
    take_screenshot_path = path;
    MathUtil::m_last_screenshot_path = path;
}

void GLWidget::DoSaveEqui(const QString path)
{
    take_screenshot_cubemap = true;
    take_screenshot_path = path;
    MathUtil::m_last_screenshot_path = path;
}

#ifdef __ANDROID__
bool GLWidget::event(QEvent *e)
{
    if (e->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(e));
    return QWidget::event(e);
}

bool GLWidget::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        PinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

void GLWidget::PinchTriggered(QPinchGesture *gesture)
{
    float scale = gesture->totalScaleFactor();
    //qDebug() << "pinching" << gesture->totalScaleFactor() << gesture->state();
    if (gesture->state() == Qt::GestureFinished || gesture->state() == Qt::GestureCanceled)
    {
        game->GetPlayer()->SetWalkForward(false);
        game->GetPlayer()->SetWalkBack(false);
        game->GetPlayer()->SetRunning(false);

        QMouseEvent * e = new QMouseEvent(QEvent::MouseButtonRelease, mouse_pos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        game->mouseReleaseEvent(e, 0);

        QCursor::setPos(GetWinCentre()); //qcursor wants global position
        QPoint centre = GetLocalWinCentre(); //interally we use local position
        last_mouse_pos = centre;
        mouse_pos = centre;
        game->ResetCursor(centre);
    }
    else
    {
        game->SetPinching(true);
        if (scale < 0.8)
        {
            if (scale < 0.5) game->GetPlayer()->SetRunning(true);
            game->GetPlayer()->SetWalkForward(false);
            game->GetPlayer()->SetWalkBack(true);
        }
        else if (scale > 1.2)
        {
            if (scale > 2) game->GetPlayer()->SetRunning(true);
            game->GetPlayer()->SetWalkForward(true);
            game->GetPlayer()->SetWalkBack(false);
        }
        else if (scale <= 1.2 && scale >= 0.8)
        {
            //game->GetPlayer()->SetRunning(false);
            game->GetPlayer()->SetWalkForward(false);
            game->GetPlayer()->SetWalkBack(false);
        }

        if (gesture->state() == Qt::GestureStarted)
        {
            last_mouse_pos = gesture->centerPoint().toPoint();
            mouse_pos = gesture->centerPoint().toPoint();

            QMouseEvent * e = new QMouseEvent(QEvent::MouseButtonPress, mouse_pos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
            game->mousePressEvent(e, 0, QSize(width(),height()), QPointF(mouse_pos.x(), height()-mouse_pos.y()));
        }
        else
        {
            last_mouse_pos = mouse_pos;
            mouse_pos = gesture->centerPoint().toPoint();

            QMouseEvent * e = new QMouseEvent(QEvent::MouseMove, mouse_pos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            const float x = float(mouse_pos.x() - last_mouse_pos.x());
            const float y = -float(mouse_pos.y() - last_mouse_pos.y());
            game->mouseMoveEvent(e, x, y, 0);
        }
    }
}
#endif

void GLWidget::mousePressEvent(QMouseEvent * e)
{
#ifdef __ANDROID__
    if (JNIUtil::GetShowingVR()) return;
#endif

    if (!grabbed)
    {
        return;
    }

#ifdef __ANDROID__
    if (game->GetPinching()){
        return;
    }

    last_mouse_pos = e->pos();
    mouse_pos = e->pos();
    game->ResetCursor(e->pos());
#endif

    game->mousePressEvent(e, 0, QSize(width(),height()), QPointF(mouse_pos.x(), height()-mouse_pos.y()));
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{
#ifdef __ANDROID__
    if (JNIUtil::GetShowingVR()) return;
#endif

    if (!grabbed)
    {
        return;
    }

#ifndef __ANDROID__
    // Every frame set the cursor back to the center of the window
    QCursor::setPos(GetWinCentre()); //qcursor wants global position
    QPoint centre = GetLocalWinCentre(); //interally we use local position
    game->ResetCursor(centre);
    last_mouse_pos = centre;

    // Create mouse move event with the delta from the center to the cursor position
    // provided in the current QMouseEvent
    mouse_pos = e->pos();
    const float x = float(mouse_pos.x() - last_mouse_pos.x());
    const float y = -float(mouse_pos.y() - last_mouse_pos.y());
    game->mouseMoveEvent(e, x, y, 0);
#else
    if (game->GetPinching()) {
        return;
    }
    if (last_mouse_pos == GetLocalWinCentre()) {
        last_mouse_pos = e->pos();
        mouse_pos = e->pos();

        return;
    }

    last_mouse_pos = mouse_pos;
    mouse_pos = e->pos();

    const float x = float(mouse_pos.x() - last_mouse_pos.x());
    const float y = -float(mouse_pos.y() - last_mouse_pos.y());
    game->mouseMoveEvent(e, x, y, 0, QSize(width(),height()), QPointF(mouse_pos.x(), height()-mouse_pos.y()));

    //with mouse hidden, always be resetting it to the centre of the screen
    /*if (abs(mouse_pos.x() - width()/2) > width()/4 || abs(mouse_pos.y() - height()/2) > height()/4) {
        snap_mouse = true;
    }*/
#endif
}

void GLWidget::mouseReleaseEvent(QMouseEvent * e)
{
#ifdef __ANDROID__
    if (JNIUtil::GetShowingVR()) return;
#endif

    if (!GetGrab())
    {
        SetGrab(true);
        // Reset cursor to window center to avoid an unwanted mouse move event as the cursor
        // recenters itself
        QCursor::setPos(GetWinCentre()); //qcursor wants global position
        QPoint centre = GetLocalWinCentre(); //interally we use local position
        game->ResetCursor(centre);
    }
    else {
#ifdef __ANDROID__
        if (game->GetPinching()){
            return;
        }
#endif

        game->mouseReleaseEvent(e, 0, QSize(width(),height()), QPointF(mouse_pos.x(), height()-mouse_pos.y()));

#ifdef __ANDROID__
        //Reset cursor on mouse release
        QCursor::setPos(GetWinCentre()); //qcursor wants global position
        QPoint centre = GetLocalWinCentre(); //interally we use local position
        last_mouse_pos = centre;
        mouse_pos = centre;
        game->ResetCursor(centre);
#endif
    }
}

void GLWidget::wheelEvent(QWheelEvent * e)
{
    game->wheelEvent(e);
}

void GLWidget::DoBookmark()
{
    take_screenshot_path = game->GetRoomScreenshotPath(game->GetEnvironment()->GetCurRoom());
    take_screenthumb = true;
    take_bookmark = true;
}

void GLWidget::keyPressEvent(QKeyEvent * e)
{
    //m_context->makeCurrent(this);
    makeCurrent();

    //qDebug() << "GLWidget::keyPressEvent";
    switch (e->key()) {
    case Qt::Key_F4:
        if ((e->modifiers() & Qt::ControlModifier) > 0) {
            game->SetDoExit(true);
        }
        break;
    case Qt::Key_B:
        if ((e->modifiers() & Qt::ControlModifier) > 0) {
            DoBookmark();
        }
        break;
    case Qt::Key_F7:
        take_screenshot_path = game->GetRoomScreenshotPath(game->GetEnvironment()->GetCurRoom());
        MathUtil::SetLastScreenshotPath(take_screenshot_path);
        take_screenthumb = true;
        break;
    case Qt::Key_F8:
        if ((e->modifiers() & Qt::ControlModifier) > 0) {
            take_screenshot_path = MathUtil::GetScreenshotPath() + "out-" + MathUtil::GetCurrentDateTimeAsString() + ".jpg";
            MathUtil::SetLastScreenshotPath(take_screenshot_path);
            take_screenshot_cubemap = true;
        }
        else {
            take_screenshot_path = MathUtil::GetScreenshotPath() + "out-" + MathUtil::GetCurrentDateTimeAsString() + ".jpg";
            MathUtil::SetLastScreenshotPath(take_screenshot_path);
            take_screenshot = true;
        }
        break;
//    case Qt::Key_Escape:
//        if (game->GetState() == JVR_STATE_DEFAULT) {
//            last_mouse_pos = GetLocalWinCentre();
//            mouse_pos = last_mouse_pos;
//            QCursor::setPos(GetWinCentre());
//        }
//        break;
//    case Qt::Key_F11:
//        if (windowState() & Qt::WindowFullScreen ||
//                windowState() & Qt::WindowMaximized) {
//            showNormal();
//            QRect screenSize = QApplication::desktop()->screenGeometry();
//            setGeometry(100, 100, screenSize.width()-200, screenSize.height()-200);
//        }
//        else {
//            showFullScreen();
//        }
//        break;
//    case Qt::Key_F12:
//    {
//        const bool is_fullscreen = windowState() & Qt::WindowFullScreen;
//        if (is_fullscreen) {
//            showNormal();
//        }
//        const int scrn_count = QApplication::desktop()->screenCount();
//        if (scrn_count > 1) {
//            cur_screen = (cur_screen+1) % scrn_count;
//        }
//        QRect screenres = QApplication::desktop()->screenGeometry(cur_screen);
//        setGeometry(screenres);
//        if (is_fullscreen) {
//            showFullScreen();
//        }
//    }
//        break;
//    case Qt::Key_P:
//        if ((e->modifiers() & Qt::ControlModifier) > 0
//            && game->GetMenu().GetPortalHotkeys()
//            && !game->GetMenu().GetFocus()
//            && game->GetWebSurfaceSelected().isNull()
//            && game->GetState() == JVR_STATE_DEFAULT) {
//            take_screenshot_cubemap = true;
//        }
//        break;
    case Qt::Key_R:
        if (hmd_manager && game->GetState() == JVR_STATE_DEFAULT && game->GetWebSurfaceSelected().isNull()) {
            hmd_manager->ReCentre();
        }
        break;

    case Qt::Key_T:
        if (!game->GetPlayerEnteringText()) {
            game->EndKeyboardFocus();
            SetGrab(false);
        }
        break;

    case Qt::Key_F11:
        SetGrab(false);
        break;

    case Qt::Key_F6:
        SetGrab(false);
        break;

    case Qt::Key_L:
        if ((e->modifiers() & Qt::ControlModifier) > 0) {
            SetGrab(false);
        }
        break;

    case Qt::Key_Escape:
        if (game->GetPrivateWebsurfacesVisible()) {
            game->SetPrivateWebsurfacesVisible(false);
        }
        else {
            SetGrab(false);
        }
        break;

//    case Qt::Key_H:
//        if ((e->modifiers() & Qt::ControlModifier) > 0) {

//            if (windowState() & Qt::WindowFullScreen) {
//                showNormal();
//                QRect screenSize = QApplication::desktop()->screenGeometry();
//                setGeometry(100, 100, screenSize.width()-200, screenSize.height()-200);
//            }

//            if (hierarchy_window->isVisible()) {
//                hierarchy_window->hide();
//            }
//            else {
//                hierarchy_window->show();
//            }

//            if (properties_window->isVisible()) {
//                properties_window->hide();
//            }
//            else {
//                properties_window->show();
//            }
//        }
//        break;

//    case Qt::Key_PageDown:
//        EndAdvertisement();
//        break;

    default:
        break;
    }

    game->keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent * e)
{
    //m_context->makeCurrent(this);
    makeCurrent();
    game->keyReleaseEvent(e);
}

void GLWidget::leaveEvent(QEvent * )
{
    if (grabbed) {
        SetGrab(true);
        QCursor::setPos(GetWinCentre()); //qcursor wants global position
        QPoint centre = GetLocalWinCentre(); //interally we use local position
        qDebug() << "GLWidget::leaveEvent snap_mouse=true";
        snap_mouse = true;
        last_mouse_pos = centre;
        mouse_pos = centre;
        game->ResetCursor(centre);
    }
}

void GLWidget::SetupFramebuffer()
{
#ifdef __ANDROID__
    uint32_t samples = 0;
#else
    uint32_t samples = (SettingsManager::GetAntialiasingEnabled()) ? 4 : 0;
#endif
    const float dpr = this->devicePixelRatio();
    RendererInterface::m_pimpl->SetIsUsingEnhancedDepthPrecision(SettingsManager::GetEnhancedDepthPrecisionEnabled());
    switch(disp_mode)
    {
    case MODE_RIFT:
    case MODE_VIVE:
    case MODE_GVR:
    case MODE_GEAR:
        if (hmd_manager) {
            const QSize s = hmd_manager->GetTextureSize();
            RendererInterface::m_pimpl->ConfigureFramebuffer(s.width() * 2, s.height(), samples);
        }
        break;
    case MODE_SBS:
    case MODE_SBS_REVERSE:
    case MODE_OU3D:
    case MODE_CUBE:
    case MODE_EQUI:
        RendererInterface::m_pimpl->ConfigureFramebuffer(width() * dpr, height() * dpr, samples);
        break;
    default:
#ifndef __ANDROID__
        RendererInterface::m_pimpl->ConfigureFramebuffer(width() * dpr, height() * dpr, samples);
#else
        RendererInterface::m_pimpl->ConfigureFramebuffer(1280, 720, samples);
#endif
        break;
    }
}

void GLWidget::initializeGL()
{
//    qDebug() << "GLWidget::initializeGL()";
//    glwidget->makeCurrent();

    if (MathUtil::InitializeGLContext())
    {
        RendererInterface::m_pimpl = Renderer::GetSingleton();
        RendererInterface::m_pimpl->Initialize();
        SetupFramebuffer();
    }
}

void GLWidget::resizeGL(int w, int h)
{
//    qDebug() << "GLWidget::resizeGL" << w << h;
    //m_context->makeCurrent(this);
//    glwidget->makeCurrent();

//    if (game) {
//        if (disp_mode == MODE_SBS || disp_mode == MODE_SBS_REVERSE) {
//            game->GetMenu().SetResolution(width()/2, height());
//        }
//        else if (disp_mode == MODE_OU3D) {
//            game->GetMenu().SetResolution(width(), height()/2);
//        }
//        else {
//            game->GetMenu().SetResolution(width(), height());
//        }
//    }
    m_equi_cubemap_handle = nullptr;
    SetupFramebuffer();
}

void GLWidget::paintGL()
{
//    qDebug() << "GLWidget::paintGL()" << disp_mode;
    if (game == nullptr || game->GetEnvironment() == nullptr || game->GetEnvironment()->GetCurRoom() == nullptr) {
        return;
    }

    if (game->GetVirtualMenu()->GetDoBookmarkAdd() || game->GetVirtualMenu()->GetDoBookmarkRemove()) {
        DoBookmark();
        game->GetVirtualMenu()->SetTakingScreenshot(true);
    }

    //int64_t t1 = JNIUtil::GetTimestampNsec();
    game->SetDrawCursor(this->hasFocus());   
    game->Update();

    //60.0 - note, get curroom AFTER call to update (crossportals can change it!)
    QPointer <Room> r = game->GetEnvironment()->GetCurRoom();

#ifndef __ANDROID__
    r->GetPerformanceLogger().StartFrameSample();
#endif

    const int w = width();
    const int h = height();
    const int w2 = RendererInterface::m_pimpl->GetWindowWidth();
    const int h2 = RendererInterface::m_pimpl->GetWindowHeight();
    const float near_dist = game->GetCurrentNearDist();
    const float far_dist = game->GetCurrentFarDist();
#ifndef __ANDROID__
    QPointF mouse_pos(w2*0.5f, h2*0.5f);
#else
    QPointF mouse_pos(this->mouse_pos.x(), h2 - this->mouse_pos.y());
#endif

    QSize s(w2, h2);

    QMatrix4x4 base_xform;
    const qreal dpr = this->devicePixelRatio();

    //calibrate player for first time (if needed)
    if (hmd_manager && hmd_manager->GetEnabled() &&
            !game->GetPlayer()->GetHMDCalibrated() &&
            (game->GetPlayer()->GetWalking())) {
        hmd_manager->ReCentre();
        game->GetPlayer()->SetHMDCalibrated(true);
    }

    //for screenshots, we do
    uint32_t overrideWidth = RendererInterface::m_pimpl->GetWindowWidth();
    uint32_t overrideHeight = RendererInterface::m_pimpl->GetWindowHeight();
    if (take_screenshot) {
        //take_screenshot_path
#ifdef __ANDROID__
        overrideWidth = width();
        overrideHeight = height();
        RendererInterface::m_pimpl->RequestScreenShot(width(), height(), RendererInterface::m_pimpl->GetMSAACount(), false, (RendererInterface::m_pimpl->GetLastSubmittedFrameID() + 1));
#else
        RendererInterface::m_pimpl->RequestScreenShot(width(), height(), RendererInterface::m_pimpl->GetMSAACount(), false, (RendererInterface::m_pimpl->GetLastSubmittedFrameID() + 1));
#endif
        take_screenshot = false;
    }

    DisplayMode disp_mode_override = disp_mode;
    if (take_screenshot_cubemap)
    {
        disp_mode_override = MODE_EQUI;
#ifdef __ANDROID__
        overrideWidth = width();
        overrideHeight = height();
        RendererInterface::m_pimpl->RequestScreenShot(width(), height(), RendererInterface::m_pimpl->GetMSAACount(), false, (RendererInterface::m_pimpl->GetLastSubmittedFrameID() + 1));
#else
        //take_screenshot_path
        RendererInterface::m_pimpl->RequestScreenShot(width(), height(), RendererInterface::m_pimpl->GetMSAACount() * 2, true, (RendererInterface::m_pimpl->GetLastSubmittedFrameID() + 1));
#endif
        take_screenshot_cubemap = false;
    }

    //draw
    switch (disp_mode_override)
    {
    case MODE_OU3D:
    {
        RendererInterface::m_pimpl->BindFBOToDraw(FBO_TEXTURE_BITFIELD::ALL);
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::NONE);

        const int w = RendererInterface::m_pimpl->GetWindowWidth();
        const int h = RendererInterface::m_pimpl->GetWindowHeight();

        RendererInterface::m_pimpl->SetDepthMask(DepthMask::DEPTH_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetColorMask(ColorMask::COLOR_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetDepthFunc(DepthFunc::LEQUAL);

        QVector<GLfloat> m_viewPortArray;
        m_viewPortArray.reserve(8);

        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(overrideWidth);
        m_viewPortArray.push_back(overrideHeight/2);

        m_viewPortArray.push_back(0);
        m_viewPortArray.push_back(overrideHeight/2);
        m_viewPortArray.push_back(overrideWidth);
        m_viewPortArray.push_back(overrideHeight/2);

        const float ipd = SettingsManager::GetIPD(); //we use the custom ipd as specified by the player (0 by default)
        const float fov = SettingsManager::GetFOV();

        game->GetPlayer()->SetViewGL(false, ipd, base_xform);
        QMatrix4x4 qt_viewMatrixR = MathUtil::ViewMatrix();

        game->GetPlayer()->SetViewGL(true, ipd, base_xform);
        QMatrix4x4 qt_viewMatrixL = MathUtil::ViewMatrix();

        cameras.clear();
        cameras.reserve(4);

        // Left offset half-screen camera
        cameras.push_back(VirtualCamera(qt_viewMatrixL,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w)/float(h/2), fov, near_dist, far_dist));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        // Right offset half-screen camera
        cameras.push_back(VirtualCamera(qt_viewMatrixR,
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                float(w)/float(h/2), fov, near_dist, far_dist));
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);
        cameras[1].SetLeftEye(false);

        // Full screen menu cameras
        cameras.push_back(VirtualCamera(QVector3D(0.0, 0.0, 0.0), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w)/float(h/2), fov, near_dist, far_dist));
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);

        cameras.push_back(VirtualCamera(QVector3D(0.0, 0.0, 0.0), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                float(w)/float(h/2), fov, near_dist, far_dist));
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);
        cameras[3].SetLeftEye(false);

        RendererInterface::m_pimpl->SetCameras(&cameras);

        // Set matrices and draw the game
        SetDefaultProjectionPersp(SettingsManager::GetFOV(), float(w)/float(h/2), near_dist, far_dist);
        MathUtil::LoadModelIdentity();
        if (mouse_pos.y() > h/2) {
            game->DrawGL(ipd, base_xform, true, QSize(w, h/2), QPoint((mouse_pos.x()), mouse_pos.y() - h/2));
        }
        else {
            game->DrawGL(ipd, base_xform, true, QSize(w, h/2), QPoint(mouse_pos.x(), mouse_pos.y()));
        }

        // Tell Renderer we are done submitting render commands
        RendererInterface::m_pimpl->SubmitFrame();

        // Draw the frame
        RendererInterface::m_pimpl->Render(); // TODO: This call will no longer be needed once it's running on it's own thread

        // Blit Attachment 0 of the MSAA FBO to the contex default FBO for presenting to the user
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
        MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        GLenum buf = GL_COLOR_ATTACHMENT0;
        MathUtil::glFuncs->glDrawBuffers(1, &buf);
        MathUtil::glFuncs->glBlitFramebuffer(0,    0, w * dpr * 1.0f, h * dpr,
                                             0,    0, w * dpr * 1.0f, h * dpr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        break;
    }

    case MODE_SBS:
    case MODE_SBS_REVERSE:
    {
        RendererInterface::m_pimpl->BindFBOToDraw(FBO_TEXTURE_BITFIELD::ALL);
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::NONE);

        const int w = RendererInterface::m_pimpl->GetWindowWidth();
        const int h = RendererInterface::m_pimpl->GetWindowHeight();

        RendererInterface::m_pimpl->SetDepthMask(DepthMask::DEPTH_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetColorMask(ColorMask::COLOR_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetDepthFunc(DepthFunc::LEQUAL);

        QVector<GLfloat> m_viewPortArray;
        m_viewPortArray.reserve(8);

        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(overrideWidth/2);
        m_viewPortArray.push_back(overrideHeight);

        m_viewPortArray.push_back(overrideWidth/2);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(overrideWidth/2);
        m_viewPortArray.push_back(overrideHeight);

        const float ipd = SettingsManager::GetIPD(); //we use the custom ipd as specified by the player (0 by default)
        const float fov = SettingsManager::GetFOV();

        game->GetPlayer()->SetViewGL(false, ipd, base_xform);
        QMatrix4x4 qt_viewMatrixR = MathUtil::ViewMatrix();

        game->GetPlayer()->SetViewGL(true, ipd, base_xform);
        QMatrix4x4 qt_viewMatrixL = MathUtil::ViewMatrix();

        if (disp_mode == MODE_SBS_REVERSE)
        {
            QMatrix4x4 temp_matrix = qt_viewMatrixR;
            qt_viewMatrixR = qt_viewMatrixL;
            qt_viewMatrixL = temp_matrix;
        }

        cameras.clear();
        cameras.reserve(4);

        // Left offset half-screen camera
        cameras.push_back(VirtualCamera(qt_viewMatrixL,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w/2)/float(h), fov, near_dist, far_dist));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        // Right offset half-screen camera
        cameras.push_back(VirtualCamera(qt_viewMatrixR,
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                float(w/2)/float(h), fov, near_dist, far_dist));
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);
        cameras[1].SetLeftEye(false);

        // Full screen menu cameras
        cameras.push_back(VirtualCamera(QVector3D(0.0, 0.0, 0.0), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w/2)/float(h), fov, near_dist, far_dist));
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);

        cameras.push_back(VirtualCamera(QVector3D(0.0, 0.0, 0.0), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                float(w/2)/float(h), fov, near_dist, far_dist));
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);
        cameras[3].SetLeftEye(false);

        RendererInterface::m_pimpl->SetCameras(&cameras);

        // Set matrices and draw the game
        SetDefaultProjectionPersp(SettingsManager::GetFOV(), float(w/2)/float(h), near_dist, far_dist);
        MathUtil::LoadModelIdentity();
        if (mouse_pos.x() > w/2)
        {
            game->DrawGL(ipd, base_xform, true, QSize(w/2, h), QPoint((mouse_pos.x() - w/2), mouse_pos.y()));
        }
        else
        {
            game->DrawGL(ipd, base_xform, true, QSize(w/2, h), QPoint(mouse_pos.x()/2, mouse_pos.y()));
        }

        // Tell Renderer we are done submitting render commands
        RendererInterface::m_pimpl->SubmitFrame();

        // Draw the frame
        RendererInterface::m_pimpl->Render(); // TODO: This call will no longer be needed once it's running on it's own thread

        // Blit Attachment 0 of the MSAA FBO to the contex default FBO for presenting to the user
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
        MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        GLenum buf = GL_COLOR_ATTACHMENT0;
        MathUtil::glFuncs->glDrawBuffers(1, &buf);
        MathUtil::glFuncs->glBlitFramebuffer(0,    0, w * dpr * 1.0f, h * dpr,
                                             0,    0, w * dpr * 1.0f, h * dpr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        break;
    }

    case MODE_CUBE:
    {
        RendererInterface::m_pimpl->BindFBOToDraw(FBO_TEXTURE_BITFIELD::ALL);
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::NONE);

        const int w = RendererInterface::m_pimpl->GetWindowWidth();
        const int h = RendererInterface::m_pimpl->GetWindowHeight();

        RendererInterface::m_pimpl->SetDepthMask(DepthMask::DEPTH_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetColorMask(ColorMask::COLOR_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetDepthFunc(DepthFunc::LEQUAL);

        game->GetPlayer()->SetViewGL(true, 0.0f, base_xform);
        QMatrix4x4 qt_viewMatrix = MathUtil::ViewMatrix();

        QVector<GLfloat> m_viewPortArray;
        m_viewPortArray.reserve(28);
        // Left
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(overrideHeight/3);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        // Forward
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        // Forward menu
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        // Right
        m_viewPortArray.push_back(overrideWidth/2);
        m_viewPortArray.push_back(overrideHeight/3);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        // Back
        m_viewPortArray.push_back(overrideWidth * 3/4);
        m_viewPortArray.push_back(overrideHeight/3);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        // Up
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);
        // Down
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight * 2/3);
        m_viewPortArray.push_back(overrideWidth/4);
        m_viewPortArray.push_back(overrideHeight/3);


        cameras.clear();
        cameras.reserve(7);

        //left
        QMatrix4x4 mat;
        mat.rotate(-90.0f, 0.0f, 1.0f, 0.0f);
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //forward
        mat = qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                                        1.0f, 90.0f, near_dist, far_dist));

        //forward menu
        cameras.push_back(VirtualCamera(QVector3D(0.0f, 0.0f, 0.0f), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(m_viewPortArray[8], m_viewPortArray[9], m_viewPortArray[10], m_viewPortArray[11]),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);

        //right
        mat.setToIdentity();
        mat.rotate(90.0f, 0.0f, 1.0f, 0.0f);
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        QVector4D(m_viewPortArray[12], m_viewPortArray[13], m_viewPortArray[14], m_viewPortArray[15]),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //back
        mat.setToIdentity();
        mat.rotate(180.0f, 0.0f, 1.0f, 0.0f);
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        QVector4D(m_viewPortArray[16], m_viewPortArray[17], m_viewPortArray[18], m_viewPortArray[19]),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[4].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //up
        mat.setToIdentity();
        mat.rotate(90.0f, 1.0f, 0.0f, 0.0f);
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        QVector4D(m_viewPortArray[20], m_viewPortArray[21], m_viewPortArray[22], m_viewPortArray[23]),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[5].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //down
        mat.setToIdentity();
        mat.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        QVector4D(m_viewPortArray[24], m_viewPortArray[25], m_viewPortArray[26], m_viewPortArray[27]),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[6].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        RendererInterface::m_pimpl->SetCameras(&cameras);

        SetDefaultProjectionPersp(90.0f, 1.0f, near_dist, far_dist);
        MathUtil::LoadModelIdentity();
        game->DrawGL(0.0f, base_xform, false, s, mouse_pos);

        // Tell Renderer we are done submitting render commands
        RendererInterface::m_pimpl->SubmitFrame();

        // Draw the frame
        RendererInterface::m_pimpl->Render(); // TODO: This call will no longer be needed once it's running on it's own thread

        // Blit Attachment 0 of the MSAA FBO to the contex default FBO for presenting to the user
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
        MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        GLenum buf = GL_COLOR_ATTACHMENT0;
        MathUtil::glFuncs->glDrawBuffers(1, &buf);
        MathUtil::glFuncs->glBlitFramebuffer(0,    0, w * dpr * 1.0f, h * dpr,
                                             0,    0, w * dpr * 1.0f, h * dpr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        break;
    }

    case MODE_RIFT:
    case MODE_VIVE:
    case MODE_GVR:
    case MODE_GEAR:
    {
        RendererInterface::m_pimpl->BindFBOToDraw(FBO_TEXTURE_BITFIELD::ALL);
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::NONE);
        RendererInterface::m_pimpl->SetDepthMask(DepthMask::DEPTH_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetColorMask(ColorMask::COLOR_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetDepthFunc(DepthFunc::LEQUAL);

        const int fbo_w = RendererInterface::m_pimpl->GetWindowWidth();
        const int fbo_h = RendererInterface::m_pimpl->GetWindowHeight();

        QVector<GLfloat> m_viewPortArray;
#ifdef __ANDROID__
        m_viewPortArray.reserve(4);
#else
        m_viewPortArray.reserve(8);
#endif

        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(fbo_w/2);
        m_viewPortArray.push_back(fbo_h);

#ifndef __ANDROID__
        m_viewPortArray.push_back(fbo_w/2);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(fbo_w/2);
        m_viewPortArray.push_back(fbo_h);
#endif

        // Regular clip planes
        hmd_manager->SetNearDist(near_dist, false);
        hmd_manager->SetFarDist(far_dist, false);

        // Avatar Clip Planes
        hmd_manager->SetNearDist(0.25f, true);
        hmd_manager->SetFarDist(far_dist, true);

        // We use the base_xform (player position and orientation) as the camera view matrix
        // then in the render loop we apply the player-to-HMD and HMD-to-eye transforms to this
        // to get the final viewMatrix for rendering.
        game->GetPlayer()->SetViewGL(true, 0, base_xform);
        QMatrix4x4 qt_viewMatrixL = MathUtil::ViewMatrix();
#ifndef __ANDROID__
        QMatrix4x4 qt_viewMatrixR = MathUtil::ViewMatrix();
#endif

        // Projection matrixes for each eye, botth normal and avatar(further near clip plane to avoid blocking view)
        QMatrix4x4 qt_projMatrixL = hmd_manager->GetEyeProjectionMatrix(0, false);
        QMatrix4x4 qt_projMatrixL_avatar = hmd_manager->GetEyeProjectionMatrix(0, true);
#ifndef __ANDROID__
        QMatrix4x4 qt_projMatrixR = hmd_manager->GetEyeProjectionMatrix(1, false);
        QMatrix4x4 qt_projMatrixR_avatar = hmd_manager->GetEyeProjectionMatrix(1, true);
#endif

        cameras.clear();
#ifndef __ANDROID__
        cameras.reserve(4);

        // Left offset half-screen camera (all but avatar)
        cameras.push_back(VirtualCamera(qt_viewMatrixL,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                qt_projMatrixL));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, false);

        // Right offset half-sreen camera (all but avatar)
        cameras.push_back(VirtualCamera(qt_viewMatrixR,
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                qt_projMatrixR));
        cameras[1].SetLeftEye(false);
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, false);

        // Left offset half-screen camera (just avatar)
        cameras.push_back(VirtualCamera(qt_viewMatrixL,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                qt_projMatrixL_avatar));
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, true);

        // Right offset half-sreen camera (just avatar)
        cameras.push_back(VirtualCamera(qt_viewMatrixR,
                                        QVector4D(m_viewPortArray[4], m_viewPortArray[5], m_viewPortArray[6], m_viewPortArray[7]),
                qt_projMatrixR_avatar));
        cameras[3].SetLeftEye(false);
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, true);
#else
        cameras.reserve(2);

        // Left offset half-screen camera (all but avatar)
        cameras.push_back(VirtualCamera(qt_viewMatrixL,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                qt_projMatrixL));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, false);

        // Left offset half-screen camera (just avatar)
        cameras.push_back(VirtualCamera(qt_viewMatrixL,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                qt_projMatrixL_avatar));
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, true);
#endif

        RendererInterface::m_pimpl->SetCameras(&cameras);

        // Set matrices and draw the game
        // hmd_xform here is set as the base viewMatrix for things like ray-casting and selection
        MathUtil::LoadModelIdentity();
#ifndef __ANDROID__
        game->DrawGL(0, base_xform, true, s, mouse_pos);
#else
        QMatrix4x4 hmd_xform = hmd_manager->GetHMDTransform();
        hmd_xform.setColumn(3, QVector4D(0,0,0,1));
        game->DrawGL(0, hmd_xform, true, s, mouse_pos);
#endif

        // Tell Renderer we are done submitting render commands
        RendererInterface::m_pimpl->SubmitFrame();

        // Draw the frame
        RendererInterface::m_pimpl->Render(); // TODO: This call will no longer be needed once it's running on it's own thread

        // Blit Attachment 0 of the MSAA FBO to the contex default FBO for presenting to the user
        // because we already blit the MSAA into the non MSAA in EndRendering() we can blit the already resolved
        // image to the window.

        //Don't need to blit if submitting to GVR/Gear
#ifndef __ANDROID__
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
        MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        GLenum buf = GL_COLOR_ATTACHMENT0;
        MathUtil::glFuncs->glDrawBuffers(1, &buf);
        MathUtil::glFuncs->glBlitFramebuffer(0, 0, fbo_w, fbo_h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif

        break;
    }

    case MODE_EQUI:
    {
        RendererInterface::m_pimpl->BindFBOToDraw(FBO_TEXTURE_BITFIELD::ALL);
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::NONE);
        RendererInterface::m_pimpl->SetDepthMask(DepthMask::DEPTH_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetColorMask(ColorMask::COLOR_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetDepthFunc(DepthFunc::LEQUAL);

        const int cube_cross_width = overrideWidth;
        const int cube_cross_height = overrideHeight;

        int32_t const cube_face_dim = qMin(cube_cross_width / 3, cube_cross_height / 2);

        // No Pitch in transform for equi to keep horizon level
        game->GetPlayer()->SetViewGL(true, 0.0f, base_xform, true);
        QMatrix4x4 qt_viewMatrix = MathUtil::ViewMatrix();

        cameras.clear();
        cameras.reserve(7);

        QVector<QVector4D> viewports;
        viewports.reserve(6);
        // This is a 3x2 grid layout to use all of the available framebuffer space
        viewports.push_back(QVector4D(cube_face_dim * 0.0f, cube_face_dim * 0.0f, cube_face_dim, cube_face_dim)); // X+
        viewports.push_back(QVector4D(cube_face_dim * 1.0f, cube_face_dim * 0.0f, cube_face_dim, cube_face_dim)); // X-
        viewports.push_back(QVector4D(cube_face_dim * 2.0f, cube_face_dim * 0.0f, cube_face_dim, cube_face_dim)); // Y+
        viewports.push_back(QVector4D(cube_face_dim * 0.0f, cube_face_dim * 1.0f, cube_face_dim, cube_face_dim)); // Y-
        viewports.push_back(QVector4D(cube_face_dim * 1.0f, cube_face_dim * 1.0f, cube_face_dim, cube_face_dim)); // Z+
        viewports.push_back(QVector4D(cube_face_dim * 2.0f, cube_face_dim * 1.0f, cube_face_dim, cube_face_dim)); // Z-

        //right
        QMatrix4x4 mat;
        mat.setToIdentity(); // Now facing -z
        mat.rotate(-90.0f, 0.0f, 1.0f, 0.0f); // Now facing +x
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        viewports[0],
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //left
        mat.setToIdentity(); // Now facing -z
        mat.rotate(90.0f, 0.0f, 1.0f, 0.0f);  // Now facing -x
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        viewports[1],
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //up
        mat.setToIdentity(); // Now facing -z
        mat.rotate(180.0f, 0.0f, 1.0f, 0.0f); // Now facing +z
        mat.rotate(-90.0f, 1.0f, 0.0f, 0.0f); // Now facing +y
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        viewports[2],
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //down
        mat.setToIdentity(); // Now facing -z
        mat.rotate(180.0f, 0.0f, 1.0f, 0.0f); // Now facing +z
        mat.rotate(90.0f, 1.0f, 0.0f, 0.0f); // Now facing -y
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        viewports[3],
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[3].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //back
        mat.setToIdentity(); // Now facing -z
        mat.rotate(180.0f, 0.0f, 1.0f, 0.0f); // Now facing +z
        mat = mat * qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        viewports[4],
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[4].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);

        //forward
        mat.setToIdentity(); // Now facing -z
        mat = qt_viewMatrix;
        cameras.push_back(VirtualCamera(mat,
                                        viewports[5],
                                        1.0f, 90.0f, near_dist, far_dist));

        //forward menu
        cameras.push_back(VirtualCamera(QVector3D(0.0f, 0.0f, 0.0f), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(cube_face_dim, cube_face_dim, cube_face_dim, cube_face_dim),
                                        1.0f, 90.0f, near_dist, far_dist));
        cameras[6].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[6].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);

        RendererInterface::m_pimpl->SetCameras(&cameras);

        SetDefaultProjectionPersp(90.0f, 1.0f, near_dist, far_dist);
        game->DrawGL(0.0f, base_xform, false, s, mouse_pos);

        // Tell Renderer we are done submitting render commands
        RendererInterface::m_pimpl->SubmitFrame();

        // Draw the frame
        RendererInterface::m_pimpl->Render(); // TODO: This call will no longer be needed once it's running on it's own thread

        // Blit Attachment 0 of the MSAA FBO to the contex default FBO for presenting to the user
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
        MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        GLenum buf = GL_COLOR_ATTACHMENT0;
        MathUtil::glFuncs->glDrawBuffers(1, &buf);
        MathUtil::glFuncs->glBlitFramebuffer(0,    0, width() * dpr, height() * dpr,
                                             0,    0, width() * dpr, height() * dpr,
                                             GL_COLOR_BUFFER_BIT, GL_NEAREST);
        break;
    }

    case MODE_AUTO:
    case MODE_2D:
    default:
    {        
#if !(defined(OCULUS_SUBMISSION_BUILD) && defined(ANDROID))
        // Make sure we have no FBO bound here, so that the render-thread won't have texture binding issues
        // when it tries to copy it's resulting texture to our FBO color layer.
        RendererInterface::m_pimpl->BindFBOToDraw(FBO_TEXTURE_BITFIELD::NONE);
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::NONE);

        RendererInterface::m_pimpl->SetDepthMask(DepthMask::DEPTH_WRITES_ENABLED);
        RendererInterface::m_pimpl->SetDepthFunc(DepthFunc::LEQUAL);
        RendererInterface::m_pimpl->SetColorMask(ColorMask::COLOR_WRITES_ENABLED);

        // Setup matrices as uniforms
        SetDefaultProjectionPersp(SettingsManager::GetFOV(), float(w)/float(h), near_dist, far_dist);
        MathUtil::LoadModelIdentity();

        game->GetPlayer()->SetViewGL(true, 0.0f, base_xform);
        QMatrix4x4 qt_viewMatrix = MathUtil::ViewMatrix();

        QVector<GLfloat> m_viewPortArray;
        m_viewPortArray.reserve(4);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(0.0f);
        m_viewPortArray.push_back(overrideWidth * dpr);
        m_viewPortArray.push_back(overrideHeight * dpr);

        cameras.clear();
        cameras.reserve(3);

        cameras.push_back(VirtualCamera(qt_viewMatrix,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w)/float(h), SettingsManager::GetFOV(), near_dist, far_dist));
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, true);
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, false);
        cameras[0].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, false);

        cameras.push_back(VirtualCamera(qt_viewMatrix,
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w)/float(h), SettingsManager::GetFOV(), 0.25f, far_dist));
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[1].SetScopeMask(RENDERER::RENDER_SCOPE::AVATARS, true);

        cameras.push_back(VirtualCamera(QVector3D(0.0, 0.0, 0.0), QQuaternion(), QVector3D(1.0f, 1.0f, 1.0f),
                                        QVector4D(m_viewPortArray[0], m_viewPortArray[1], m_viewPortArray[2], m_viewPortArray[3]),
                float(w)/float(h), SettingsManager::GetFOV(), near_dist, far_dist));
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::ALL, false);
        cameras[2].SetScopeMask(RENDERER::RENDER_SCOPE::MENU, true);

        RendererInterface::m_pimpl->SetCameras(&cameras);

        // Render the scene
        game->DrawGL(0.0f, base_xform, true, s, mouse_pos);

        // Tell Renderer we are done submitting render commands
        RendererInterface::m_pimpl->SubmitFrame();

        // Draw the frame
        RendererInterface::m_pimpl->Render(); // TODO: This call will no longer be needed once it's running on it's own thread

        // Blit Attachment 0 of the MSAA FBO to the contex default FBO for presenting to the user
        RendererInterface::m_pimpl->BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
        MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
        GLenum buf = GL_COLOR_ATTACHMENT0;
        MathUtil::glFuncs->glDrawBuffers(1, &buf);
#ifndef __ANDROID__
        MathUtil::glFuncs->glBlitFramebuffer(0,    0, w * dpr * 1.0f, h * dpr,
                                             0,    0, w * dpr * 1.0f, h * dpr, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#else
        MathUtil::glFuncs->glBlitFramebuffer(0,    0, 1280, 720,
                                             0,    0, width(), height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif
#endif

        break;
    }
    } // switch (disp_mode)

    //49.65 - we set the READ_FRAMEBUFFER to default for saving screenshots
    MathUtil::glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER, defaultFramebufferObject());

    game->SetWindowSize(size());

#ifndef __ANDROID__
    r->GetPerformanceLogger().SetGPUTimeQueryResults(RendererInterface::m_pimpl->GetGPUTimeQueryResults());
    r->GetPerformanceLogger().SetCPUTimeQueryResults(RendererInterface::m_pimpl->GetCPUTimeQueryResults());
    r->GetPerformanceLogger().EndFrameSample();
#endif

    //for thumbs we don't want instructions
    if (take_screenthumb) {
        MathUtil::glFuncs->glFlush();
        game->SaveScreenThumb(take_screenshot_path);
        take_screenthumb = false;
    }

    //save bookmark (with screenthumb)
    if (take_bookmark) {
        MathUtil::glFuncs->glFlush();
        game->SaveBookmark();
        take_bookmark = false;
        if (game->GetVirtualMenu()->GetVisible()) {
            game->GetVirtualMenu()->SetTakingScreenshot(false);
            game->GetVirtualMenu()->ConstructSubmenus();
        }
    }   

    /*int64_t t10 = JNIUtil::GetTimestampNsec();
    qDebug() << "  GLWidget::paintGL time" << t10 - t1;
    qDebug() << "AVERAGE MAIN THREAD CPU TIME" << game->GetEnvironment()->GetCurNodeRoom()->GetPerformanceLogger().GetAverageMainThreadCPUTime();
    qDebug() << "AVERAGE RENDER THREAD CPU TIME" << game->GetEnvironment()->GetCurNodeRoom()->GetPerformanceLogger().GetAverageRenderThreadCPUTime();
    qDebug() << "AVERAGE RENDER THREAD GPU TIME" << game->GetEnvironment()->GetCurNodeRoom()->GetPerformanceLogger().GetAverageRenderThreadGPUTime();*/
}
