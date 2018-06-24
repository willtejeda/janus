#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtCore>

#include "abstractwebview.h"
#include "cookiejar.h"

class QWebView_public : public QObject //QWebEngineView
{
    Q_OBJECT

public:
    QWebView_public(){}
    ~QWebView_public(){}

    //Make events public
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent * e);
    void keyReleaseEvent(QKeyEvent * e);
    void wheelEvent(QWheelEvent * e);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);

};

class WebView : public AbstractWebView
{
    Q_OBJECT

public:

    WebView();
    virtual ~WebView();

    virtual void initialize();
    virtual void initializeMenu();
    virtual void initializeNonMenu();
    virtual void terminate();

    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);

    virtual void keyPressEvent(QKeyEvent * e);
    virtual void keyReleaseEvent(QKeyEvent * e);

    virtual void wheelEvent(QWheelEvent * e);

    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *);

    virtual WebHitTestResult getHitTestContent(QPoint p = QPoint());
    virtual bool getTextEditing();
    virtual QPoint getTextCursorPosition();
    virtual void addToJavaScriptWindowObject(QString n, QObject* o);
    virtual void addToChildFramesJavaScriptWindowObject(QString n, QObject* o);
    virtual void evaluateJavaScript(QString s);

    virtual void setCookieJar(QPointer <CookieJar> j);

    virtual void render(QPainter* p, QRect r);
    virtual QPalette palette();
    virtual void setPalette(QPalette p);
    virtual void setUpdatesEnabled(bool b);

    virtual void setViewportSize(QSize s);
    virtual QSize viewportSize();
    virtual void scroll(int dx, int dy);
    virtual void setScrollPosition(QPoint p);
    virtual QPoint scrollPosition();

    virtual QUrl url();
    virtual void setUrl(QUrl url);
    virtual void setHtml(QString s, QUrl url);
    virtual void load(QUrl url);
    virtual void reload();
    virtual void stop();
    virtual void back();
    virtual void forward();

private:
    QWebView_public* webview;
};

#endif // WEBVIEW_H
