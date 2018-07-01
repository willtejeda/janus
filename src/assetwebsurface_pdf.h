#ifndef ASSETWEBSURFACE_PDF_H
#define ASSETWEBSURFACE_PDF_H

#ifdef __ANDROID__
//#define GLdouble float
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#endif

#include <QPdfDocument>

#include "abstractwebsurface.h"
#include "renderer.h"
#ifndef __ANDROID__
    #include "webview.h"
#else
    #include "androidwebview.h"
#endif
#include "rendererinterface.h"

class AssetWebSurface_PDF : public AbstractWebSurface
{
    Q_OBJECT

public:
    AssetWebSurface_PDF();

    virtual void SetProperties(const QVariantMap & d);

    virtual void SetTextureAlpha(const bool b);

    virtual void SetSrc(const QString & base, const QString & url_str);

    virtual void SetURL(const QString & u);
    virtual QString GetURL() const;

    virtual void SetOriginalURL(const QString & u);
    virtual QString GetOriginalURL() const;

    virtual void Load();
    virtual void Reload();
    virtual void Unload();

    virtual bool GetLoadStarted();
    virtual bool GetLoaded() const;
    virtual float GetProgress() const;

    virtual void mousePressEvent(QMouseEvent * e, const int cursor_index);
    virtual void mouseMoveEvent(QMouseEvent * e, const int cursor_index);
    virtual void mouseReleaseEvent(QMouseEvent * e, const int cursor_index);
    virtual void wheelEvent(QWheelEvent * e);

    virtual void keyPressEvent(QKeyEvent * e);
    virtual void keyReleaseEvent(QKeyEvent * e);

    virtual void GoForward();
    virtual void GoBack();

    virtual void SetCursorPosition(const QPoint & p);
    virtual QPoint GetCursorPosition() const;

    virtual QPoint GetTextCursorPosition() const;

    virtual void SetWidth(const int w);
    virtual void SetHeight(const int h);

    virtual TextureHandle* GetTextureHandle() const;
    virtual void UpdateGL();

    virtual QUrl GetLinkClicked(const int cursor_index);
    virtual WebHitTestResult GetHitTestResult(const int cursor_index) const;

    virtual QString GetFireBoxCodePacket() const;
    virtual void ReadFireBoxCode(const QString & userid, const QString & s);

    virtual QPointer <AbstractWebView> GetWebView();

    virtual QRect GetHitTestResultRect(const int cursor_index) const;
    virtual bool GetTextEditing();

    virtual void SetFocus(const bool b);
    virtual bool GetFocus() const;

private:

    QPdfDocument * document;
    QBuffer * buffer;
	std::shared_ptr<TextureHandle> m_texture_handle;
    QImage temp_image;

    bool update_viewport;
    int cur_page;

    static QPdfDocument * LoadPDFDocument(const QString filename);
    static bool RenderPDFDocument(QPdfDocument * document, const unsigned int page, QImage & image, int width = 300, int height = 300);

    bool cursor_active;

};

#endif // ASSETWEBSURFACE_PDF_H
