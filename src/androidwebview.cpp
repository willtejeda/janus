#include "androidwebview.h"

int AndroidWebView::sm_tag = 0;

//TODO: keyboard events, Janus window object (for translators and menu/pocketspace dashboard)

AndroidWebView::AndroidWebView()
    : m_androidID(generateNewTag())
{
    repaint_requested = false;
    m_tex_handle = nullptr;

    JNIUtil::CreateNewWebView(androidID());
    JNIUtil::AttachWebViewToMainLayout(androidID());

    move(10000, 10000);
}

AndroidWebView::~AndroidWebView()
{
    JNIUtil::RemoveWebView(androidID());
}

void AndroidWebView::move(int x, int y)
{
    //QWidget::move(x, y);
    JNIUtil::MoveWebView(androidID(), x, y);
}

void AndroidWebView::move(QPoint const& p)
{
    AndroidWebView::move(p.x(), p.y());
}

int AndroidWebView::generateNewTag()
{
    sm_tag++;
    return sm_tag;
}

void AndroidWebView::initialize()
{

}

void AndroidWebView::initializeMenu()
{

}

void AndroidWebView::initializeNonMenu()
{

}

void AndroidWebView::terminate()
{

}

void AndroidWebView::update()
{
    GetScrollRequested();
    GetURLChanged();
    GetRepaintRequested();
}

void AndroidWebView::mousePressEvent(QMouseEvent * e)
{
    JNIUtil::MousePressWebView(androidID(), e->x(), e->y());
}

void AndroidWebView::mouseMoveEvent(QMouseEvent * e)
{
    JNIUtil::MouseMoveWebView(androidID(), e->x(), e->y());
}

void AndroidWebView::mouseReleaseEvent(QMouseEvent * e)
{
    JNIUtil::MouseReleaseWebView(androidID(), e->x(), e->y());
}

void AndroidWebView::keyPressEvent(QKeyEvent * e)
{

}

void AndroidWebView::keyReleaseEvent(QKeyEvent * e)
{

}

void AndroidWebView::wheelEvent(QWheelEvent * e)
{

}

void AndroidWebView::dragEnterEvent(QDragEnterEvent *)
{

}

void AndroidWebView::dragMoveEvent(QDragMoveEvent *)
{

}

void AndroidWebView::dragLeaveEvent(QDragLeaveEvent *)
{

}

void AndroidWebView::dropEvent(QDropEvent *)
{

}

void AndroidWebView::setCookieJar(QPointer <CookieJar> j)
{
    //AndroidWebView->page()->networkAccessManager()->setCookieJar(j);
}

WebHitTestResult AndroidWebView::getHitTestContent(QPoint p)
{
    return JNIUtil::GetHitTestContentWebView(androidID());
}

bool AndroidWebView::getTextEditing()
{
    return false;
}

QPoint AndroidWebView::getTextCursorPosition()
{
    return QPoint();
}

void AndroidWebView::addToJavaScriptWindowObject(QString n, QObject* o)
{

}

void AndroidWebView::addToChildFramesJavaScriptWindowObject(QString n, QObject* o)
{

}

void AndroidWebView::evaluateJavaScript(QString s)
{
    JNIUtil::EvaluateJavaScriptAtWebView(androidID(), s);
}

void AndroidWebView::render(QPainter* p, QRect r)
{
    /*QImage image;
    if (repaint_requested)
    {
        image = JNIUtil::GetBitmap(androidID());
        repaint_requested = false;
    }

    if (!image.isNull())
    {
        //qDebug() << "RENDERINGIMAGE";
        QRect s(0,0,image.width(),image.height());
        //qDebug() << s;
        p->drawImage(r, image, s);

        old_image = image;
    }
    else if (!old_image.isNull()){
        //qDebug() << "RENDERINGOLDIMAGE";
        QRect s(0,0,old_image.width(),old_image.height());
        p->drawImage(r, old_image, s);
    }*/
}
QPalette AndroidWebView::palette()
{
    return QWidget::palette();
}

void AndroidWebView::setPalette(QPalette p)
{
    //QWidget::setPalette(p);
}

void AndroidWebView::setUpdatesEnabled(bool b)
{
    //QWidget::setUpdatesEnabled(b);
    //JNIUtil::SetUpdatesEnabledWebView(androidID(),b);
}

void AndroidWebView::setViewportSize(QSize s)
{
    //QWidget::resize(s.width(), s.height());
    JNIUtil::ResizeWebView(androidID(), s.width(), s.height());

    QImage img(s.width(), s.height(), QImage::Format_RGBA8888);
    img.fill(Qt::darkGray);
    m_tex_handle = RendererInterface::m_pimpl->CreateTextureQImage(img, true, true, true, TextureHandle::ALPHA_TYPE::BLENDED, TextureHandle::COLOR_SPACE::SRGB);
}

QSize AndroidWebView::viewportSize()
{
    return QSize(JNIUtil::GetWidthWebView(androidID()), JNIUtil::GetHeightWebView(androidID()));
}

void AndroidWebView::scroll(int dx, int dy)
{
    //qDebug() << "SCROLLINGWEBVIEW" << dx << dy;
    JNIUtil::ScrollByWebView(androidID(), dx, dy);
}

void AndroidWebView::setScrollPosition(QPoint p)
{
    int x_range = JNIUtil::GetHorizontalScrollRange(androidID());
    int y_range = JNIUtil::GetVerticalScrollRange(androidID());

    p.setX(qMin((qMax(0, p.x())), x_range));
    p.setY(qMin((qMax(0, p.y())), y_range));

    JNIUtil::ScrollToWebView(androidID(), p.x(), p.y());
}

QPoint AndroidWebView::scrollPosition()
{
    //TODO: implement in Java; get x/y
    return QPoint();
}

QUrl AndroidWebView::url()
{
    return JNIUtil::GetURLWebView(androidID());
}

void AndroidWebView::setUrl(QUrl url)
{
    JNIUtil::LoadUrlAtWebView(androidID(), url);
}

void AndroidWebView::setHtml(QString s, QUrl url)
{
    JNIUtil::LoadHtmlAtWebView(androidID(), s);
}

void AndroidWebView::load(QUrl url)
{
    JNIUtil::LoadUrlAtWebView(androidID(), url);
}

void AndroidWebView::reload()
{
    JNIUtil::ReloadWebView(androidID());
}

void AndroidWebView::stop()
{
    JNIUtil::StopWebView(androidID());
}

void AndroidWebView::back()
{
    JNIUtil::GoBackOrForwardAtWebView(androidID(), -1);
}

void AndroidWebView::forward()
{
    JNIUtil::GoBackOrForwardAtWebView(androidID(), 1);
}

bool AndroidWebView::GetRepaintRequested()
{
    //repaint_requested = JNIUtil::GetRepaintRequestedAtWebView(androidID());
    //if (repaint_requested){
        //RepaintRequested(QRect(0,0,JNIUtil::GetWidthWebView(androidID()), JNIUtil::GetHeightWebView(androidID())));

        if (m_tex_handle != nullptr) {
            //JNIUtil::SetUpdatesEnabledWebView(androidID(), false);
            JNIUtil::UpdateTextureWebView(androidID(), m_tex_handle.get());
            //JNIUtil::SetUpdatesEnabledWebView(androidID(), true);
        }
    //}
    return repaint_requested;
}

bool AndroidWebView::GetScrollRequested()
{
    if (JNIUtil::GetScrollRequestedAtWebView(androidID()))
    {
        //ScrollRequested(JNIUtil::GetScrollXWebView(androidID()),JNIUtil::GetScrollYWebView(androidID()),QRect());
        return true;
    }
    return false;
}

bool AndroidWebView::GetURLChanged()
{
    if (JNIUtil::GetURLChangedAtWebView(androidID()))
    {
        URLChanged(QUrl(JNIUtil::GetURLWebView(androidID())));
        return true;
    }
    return false;
}

TextureHandle * AndroidWebView::GetTextureHandle()
{
    return m_tex_handle.get();
}

void AndroidWebView::SetFocus(const bool b)
{

}

bool AndroidWebView::GetFocus() const
{
    return false;
}
