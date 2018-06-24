#ifndef ABSTRACTWEBSURFACE_H
#define ABSTRACTWEBSURFACE_H

#include "asset.h"
#include "abstractwebview.h"
#include "texturemanager.h"

class AbstractWebSurface : public Asset
{
public:
    AbstractWebSurface();
    virtual ~AbstractWebSurface() = 0;

    virtual void SetProperties(const QVariantMap & d) = 0;

    virtual void SetTextureAlpha(const bool b) = 0;

    virtual void SetSrc(const QString & base, const QString & url_str) = 0;

    virtual void SetURL(const QString & u) = 0;
    virtual QString GetURL() const = 0;

    virtual void SetOriginalURL(const QString & u) = 0;
    virtual QString GetOriginalURL() const = 0;

    virtual void Load() = 0;
    virtual void Reload() = 0;
    virtual void Unload() = 0;

    virtual bool GetLoadStarted() = 0;
    virtual bool GetLoaded() const = 0;

    virtual void mousePressEvent(QMouseEvent * e, const int cursor_index) = 0;
    virtual void mouseMoveEvent(QMouseEvent * e, const int cursor_index) = 0;
    virtual void mouseReleaseEvent(QMouseEvent * e, const int cursor_index) = 0;
    virtual void wheelEvent(QWheelEvent * e) = 0;

    virtual void keyPressEvent(QKeyEvent * e) = 0;
    virtual void keyReleaseEvent(QKeyEvent * e) = 0;

    virtual void GoForward() = 0;
    virtual void GoBack() = 0;

    virtual void SetCursorPosition(const QPoint & p) = 0;
    virtual QPoint GetCursorPosition() const = 0;

    virtual QPoint GetTextCursorPosition() const = 0;

    virtual void SetWidth(const int w) = 0;
    virtual void SetHeight(const int h) = 0;

    virtual TextureHandle* GetTextureHandle() const = 0;
    virtual void UpdateGL() = 0;

    virtual QUrl GetLinkClicked(const int cursor_index) = 0;
    virtual WebHitTestResult GetHitTestResult(const int cursor_index) const = 0;

    virtual QString GetFireBoxCodePacket() const = 0;
    virtual void ReadFireBoxCode(const QString & userid, const QString & s) = 0;

    virtual QPointer <AbstractWebView> GetWebView() = 0;

    virtual QRect GetHitTestResultRect(const int cursor_index) const = 0;
    virtual bool GetTextEditing() = 0;

    virtual void SetFocus(const bool b) = 0;
    virtual bool GetFocus() const = 0;
};

#endif // ABSTRACTWEBSURFACE_H
