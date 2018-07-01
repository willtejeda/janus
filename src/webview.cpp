#include "webview.h"

void QWebView_public::mousePressEvent(QMouseEvent * e)
{
//        QWebEngineView::mousePressEvent(e);
}
\
void QWebView_public::mouseMoveEvent(QMouseEvent * e)
{
//        QWebEngineView::mouseMoveEvent(e);
}

void QWebView_public::mouseReleaseEvent(QMouseEvent * e)
{
//        QWebEngineView::mouseReleaseEvent(e);
}

void QWebView_public::keyPressEvent(QKeyEvent * e)
{
//        QWebEngineView::keyPressEvent(e);
}

void QWebView_public::keyReleaseEvent(QKeyEvent * e)
{
//        QWebEngineView::keyReleaseEvent(e);
}

void QWebView_public::wheelEvent(QWheelEvent * e)
{
//        QWebEngineView::wheelEvent(e);
}

void QWebView_public::dragEnterEvent(QDragEnterEvent *)
{

}

void QWebView_public::dragMoveEvent(QDragMoveEvent *)
{

}

void QWebView_public::dragLeaveEvent(QDragLeaveEvent *)
{

}

void QWebView_public::dropEvent(QDropEvent *)
{

}

WebView::WebView()
{
    webview = new QWebView_public();

//    connect(webview->page(), SIGNAL(repaintRequested(QRect)), this, SLOT(RepaintRequested(QRect)));
//    connect(webview->page(), SIGNAL(scrollRequested(int,int,QRect)), this, SLOT(ScrollRequested(int,int,QRect)));
//    connect(webview->page(), SIGNAL(loadStarted()), this, SLOT(LoadStarted()));
//    connect(webview->page(), SIGNAL(loadProgress(int)), this, SLOT(LoadProgress(int)));
//    connect(webview->page(), SIGNAL(loadFinished(bool)), this, SLOT(LoadFinished(bool)));
//    connect(webview, SIGNAL(urlChanged(QUrl)), this, SLOT(URLChanged(QUrl)));
//    connect(webview->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(JavaScriptWindowObjectCleared()));
//    connect(webview->page()->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(LoadFrameFinished(bool)));
}

WebView::~WebView()
{
    if (webview)
    {
        delete webview;
        webview = NULL;
    }
}

void WebView::initialize()
{
//    webview->setParent(NULL);
//    webview->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
//    webview->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
//    webview->settings()->setAttribute(QWebSettings::WebGLEnabled, false);
//    webview->settings()->setAttribute(QWebSettings::PrintElementBackgrounds, false);
//    webview->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
//    webview->setAcceptDrops(false);

//    webview->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
//    webview->setCursor(Qt::BlankCursor);
}

void WebView::initializeMenu()
{
//    webview->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

void WebView::initializeNonMenu()
{
//    webview->setRenderHints(QPainter::Antialiasing |
//        QPainter::HighQualityAntialiasing |
//        QPainter::SmoothPixmapTransform |
//        QPainter::TextAntialiasing);
//    webview->setAttribute(Qt::WA_OpaquePaintEvent);
//    webview->setAttribute(Qt::WA_NoSystemBackground, true);
}

void WebView::terminate()
{
//    webview->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
//    webview->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
}

void WebView::mousePressEvent(QMouseEvent * e)
{
    webview->mousePressEvent(e);
}
\
void WebView::mouseMoveEvent(QMouseEvent * e)
{        
    webview->mouseMoveEvent(e);
}

void WebView::mouseReleaseEvent(QMouseEvent * e)
{
    webview->mouseReleaseEvent(e);
}

void WebView::keyPressEvent(QKeyEvent * e)
{
    webview->keyPressEvent(e);
}

void WebView::keyReleaseEvent(QKeyEvent * e)
{
    webview->keyReleaseEvent(e);
}

void WebView::wheelEvent(QWheelEvent * e)
{
    webview->wheelEvent(e);
}

void WebView::dragEnterEvent(QDragEnterEvent *)
{

}

void WebView::dragMoveEvent(QDragMoveEvent *)
{

}

void WebView::dragLeaveEvent(QDragLeaveEvent *)
{

}

void WebView::dropEvent(QDropEvent *)
{

}

void WebView::setCookieJar(QPointer <CookieJar> j)
{
//    webview->page()->networkAccessManager()->setCookieJar(j);
}

WebHitTestResult WebView::getHitTestContent(QPoint p)
{
    WebHitTestResult h;

//    QWebHitTestResult qh = webview->page()->mainFrame()->hitTestContent(p);
//    if (qh.isNull())
//    {
//        h.is_null = true;
//    }
//    else
//    {
//        if (!qh.linkUrl().isEmpty()) {
//            h.link_url = qh.linkUrl();
//        }
//        if (!qh.mediaUrl().isEmpty()) {
//            h.media_url = qh.mediaUrl();
//        }
//        if (!qh.imageUrl().isEmpty()) {
//            h.image_url = qh.imageUrl();
//        }
//        h.editable = qh.isContentEditable();
//        h.selected = qh.isContentSelected();
//        h.bounding_rect = qh.boundingRect();
//        h.is_null = false;
//    }

    return h;
}

bool WebView::getTextEditing()
{
//    return webview->page()->mainFrame()->hitTestContent(webview->page()->inputMethodQuery(Qt::ImMicroFocus).toRect().center()).isContentEditable() ||
//        webview->page()->mainFrame()->hitTestContent(webview->page()->inputMethodQuery(Qt::ImMicroFocus).toRect().center()).isContentSelected();
    return false;
}

QPoint WebView::getTextCursorPosition()
{
//    return QPoint(webview->page()->inputMethodQuery(Qt::ImMicroFocus).toRect().center());
    return QPoint(0,0);
}

void WebView::addToJavaScriptWindowObject(QString n, QObject* o)
{
//    webview->page()->mainFrame()->addToJavaScriptWindowObject(n, o);
}

void WebView::addToChildFramesJavaScriptWindowObject(QString n, QObject* o)
{
//    QList <QWebFrame*> child_frames = webview->page()->mainFrame()->childFrames();
//    for (int j = 0; j < child_frames.size(); ++j) {
//        child_frames[j]->addToJavaScriptWindowObject(n, o);
//    }
}

void WebView::evaluateJavaScript(QString s)
{
//    webview->page()->mainFrame()->evaluateJavaScript(s);
}

void WebView::render(QPainter* p, QRect r)
{
//    webview->page()->mainFrame()->render(p, r);
}

QPalette WebView::palette()
{
//    return webview->palette();
    return QPalette();
}

void WebView::setPalette(QPalette p)
{
//    webview->setPalette(p);
}

void WebView::setUpdatesEnabled(bool b)
{
//    webview->setUpdatesEnabled(b);
}

void WebView::setViewportSize(QSize s)
{
//    webview->page()->setViewportSize(s);
}

QSize WebView::viewportSize()
{
//    return webview->page()->viewportSize();
    return QSize();
}

void WebView::scroll(int dx, int dy)
{
//    webview->page()->mainFrame()->scroll(dx, dy);
}

void WebView::setScrollPosition(QPoint p)
{
//    webview->page()->mainFrame()->setScrollPosition(p);
}

QPoint WebView::scrollPosition()
{
//    return webview->page()->mainFrame()->scrollPosition();
    return QPoint(0,0);
}

QUrl WebView::url()
{
//    return webview->url();
    return QUrl();
}

void WebView::setUrl(QUrl url)
{
//    webview->setUrl(url);
}

void WebView::setHtml(QString s, QUrl url)
{
//    webview->setHtml(s, url);
}

void WebView::load(QUrl url)
{
//    webview->load(url);
}

void WebView::reload()
{
//    webview->reload();
}

void WebView::stop()
{
//    webview->stop();
}

void WebView::back()
{
//    webview->back();
}

void WebView::forward()
{
//    webview->forward();
}
