#ifndef ABSTRACTWEBVIEW_H
#define ABSTRACTWEBVIEW_H

#include <QtCore>
#include <QtGui>
#include <QWidget>

#include "cookiejar.h"

struct WebHitTestResult
{
    QUrl link_url;
    QUrl media_url;
    QUrl image_url;

    QRect bounding_rect;

    bool editable;
    bool selected;
    bool is_null;

    QUrl linkUrl() const
    {
        return link_url;
    }

    QUrl mediaUrl() const
    {
        return media_url;
    }

    QUrl imageUrl() const
    {
        return image_url;
    }

    bool isContentEditable() const
    {
        return editable;
    }

    bool isContentSelected() const
    {
        return selected;
    }

    QRect boundingRect() const
    {
        return bounding_rect;
    }

    bool isNull() const
    {
        return is_null;
    }
};

class AbstractWebView : public QWidget
{
    Q_OBJECT

public:
    AbstractWebView();
    virtual ~AbstractWebView() = 0;

    virtual void initialize() = 0;
    virtual void initializeMenu() = 0;
    virtual void initializeNonMenu() = 0;
    virtual void terminate() = 0;
    virtual void update() = 0;

    virtual void mousePressEvent(QMouseEvent * e) = 0;
    virtual void mouseMoveEvent(QMouseEvent * e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent * e) = 0;

    virtual void keyPressEvent(QKeyEvent * e) = 0;
    virtual void keyReleaseEvent(QKeyEvent * e) = 0;

    virtual void wheelEvent(QWheelEvent * e) = 0;

    virtual void dragEnterEvent(QDragEnterEvent *) = 0;
    virtual void dragMoveEvent(QDragMoveEvent *) = 0;
    virtual void dragLeaveEvent(QDragLeaveEvent *) = 0;
    virtual void dropEvent(QDropEvent *) = 0;

    virtual WebHitTestResult getHitTestContent(QPoint p = QPoint()) = 0;
    virtual bool getTextEditing() = 0;
    virtual QPoint getTextCursorPosition() = 0;
    virtual void addToJavaScriptWindowObject(QString n, QObject* o) = 0;
    virtual void addToChildFramesJavaScriptWindowObject(QString n, QObject* o) = 0;
    virtual void evaluateJavaScript(QString s) = 0;

    virtual void setCookieJar(QPointer <CookieJar> j) = 0;

    virtual void render(QPainter* p, QRect r) = 0;
    virtual QPalette palette() = 0;
    virtual void setPalette(QPalette p) = 0;
    virtual void setUpdatesEnabled(bool b) = 0;

    virtual void setViewportSize(QSize s) = 0;
    virtual QSize viewportSize() = 0;
    virtual void scroll(int dx, int dy) = 0;
    virtual void setScrollPosition(QPoint p) = 0;
    virtual QPoint scrollPosition() = 0;

    virtual QUrl url() = 0;
    virtual void setUrl(QUrl url) = 0;
    virtual void setHtml(QString s, QUrl url) = 0;
    virtual void load(QUrl url) = 0;
    virtual void reload() = 0;
    virtual void stop() = 0;
    virtual void back() = 0;
    virtual void forward() = 0;

    virtual TextureHandle* GetTextureHandle() = 0;
    virtual void SetFocus(const bool b) = 0;
    virtual bool GetFocus() const = 0;

signals:
    void repaintRequested(QRect r);
    void scrollRequested(int dx,int dy, QRect r);
    void loadStarted();
    void loadProgress(int i);
    void loadFinished(bool b);
    void urlChanged(QUrl u);
    void javaScriptWindowObjectCleared();
    void loadFinishedFrame(bool b);

public slots:
    void RepaintRequested(QRect r);
    void ScrollRequested(int dx,int dy, QRect r);
    void LoadStarted();
    void LoadProgress(int i);
    void LoadFinished(bool b);
    void URLChanged(QUrl u);
    void JavaScriptWindowObjectCleared();
    void LoadFrameFinished(bool b);
};

#endif // ABSTRACTWEBVIEW_H
