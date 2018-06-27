#include "assetwebsurface_pdf.h"

AssetWebSurface_PDF::AssetWebSurface_PDF() :
    document(NULL),
    buffer(NULL),
    update_viewport(true),
    cur_page(0),
    cursor_active(false)
{
    SetWidth(300);
    SetHeight(300);
}

QPdfDocument * AssetWebSurface_PDF::LoadPDFDocument(const QString filename)
{
    QPdfDocument* document = new QPdfDocument;
    document->load(filename);
    if (!document || document->status() != QPdfDocument::Ready) {
        qDebug() << "AssetWebSurface_PDF::LoadPDFDocument - Unable to load" << filename;
        delete document;
        return NULL;
    }
    return document;
}

bool AssetWebSurface_PDF::RenderPDFDocument(QPdfDocument * document, const unsigned int page, QImage & image, int width, int height)
{
    // Paranoid safety check
    if (document == 0 || document->status() != QPdfDocument::Ready) {
        qDebug() << "AssetWebSurface_PDF::RenderPDFDocument - Document is null or not ready" << page;
        return false;
    }

    // Generate a QImage of the rendered page    
    image = document->render(page, QSize(width, height));
    if (image.isNull()) {
        qDebug() << "AssetWebSurface_PDF::RenderPDFDocument - Could not render image of page" << page;
        return false;
    }

    return true;
}

void AssetWebSurface_PDF::SetProperties(const QVariantMap & d)
{
    Asset::SetProperties(d);

    if (d.contains("width")) {
        SetWidth(d["width"].toInt());
    }
    if (d.contains("height")) {
        SetHeight(d["height"].toInt());
    }
}

void AssetWebSurface_PDF::SetTextureAlpha(const bool )
{

}

void AssetWebSurface_PDF::SetSrc(const QString & base, const QString & u)
{
//    qDebug() << "AssetWebSurface::SetURL()" << base << u;
    Asset::SetSrc(base, u);
}

void AssetWebSurface_PDF::Load()
{
    //qDebug() << "AssetWebSurface_PDF::Load()" << src_url;
    if (!GetStarted()) {
        WebAsset::Load(QUrl(GetS("_src_url")));
    }
}

void AssetWebSurface_PDF::Reload()
{
//    if (webview) {
//        webview->reload();
//    }
}

void AssetWebSurface_PDF::Unload()
{
    if (document) {
        delete document;
        document = NULL;
    }
    if (buffer) {
        buffer->close();
        delete buffer;
        buffer = NULL;
    }
}

bool AssetWebSurface_PDF::GetLoadStarted()
{
    return GetStarted();
}

bool AssetWebSurface_PDF::GetLoaded() const
{
    return GetLoaded();
}

float AssetWebSurface_PDF::GetProgress() const
{
    return GetProgress();
}

void AssetWebSurface_PDF::SetURL(const QString & )
{
}

QString AssetWebSurface_PDF::GetURL() const
{
    return GetS("_src_url");
}

void AssetWebSurface_PDF::SetOriginalURL(const QString & s)
{
    SetS("_src_url", s);
}

QString AssetWebSurface_PDF::GetOriginalURL() const
{
    return GetS("_src_url");
}

QUrl AssetWebSurface_PDF::GetLinkClicked(const int )
{
    return QUrl("");
}

void AssetWebSurface_PDF::mousePressEvent(QMouseEvent * , const int )
{

}

void AssetWebSurface_PDF::mouseMoveEvent(QMouseEvent * , const int )
{

}

void AssetWebSurface_PDF::mouseReleaseEvent(QMouseEvent * e, const int )
{
    if (document && document->pageCount() > 1) {
//        qDebug() << e->pos().x() << temp_image.width();
        if (e->pos().x() > temp_image.width()/2) {
            if (cur_page < document->pageCount()-1) {
                cur_page = (cur_page + 1) % document->pageCount();
            }
        }
        else {
            if (cur_page > 0) {
                cur_page = (cur_page + document->pageCount() - 1) % document->pageCount();
            }
        }
        update_viewport = true;
    }
}

void AssetWebSurface_PDF::keyPressEvent(QKeyEvent * )
{
}

void AssetWebSurface_PDF::keyReleaseEvent(QKeyEvent * )
{
}

void AssetWebSurface_PDF::wheelEvent(QWheelEvent * )
{
}

void AssetWebSurface_PDF::GoForward()
{
}

void AssetWebSurface_PDF::GoBack()
{
}

void AssetWebSurface_PDF::SetCursorPosition(const QPoint & )
{
}

QPoint AssetWebSurface_PDF::GetCursorPosition() const
{
    return QPoint(0,0);
}

QPoint AssetWebSurface_PDF::GetTextCursorPosition() const
{
    return QPoint(0,0);
}

void AssetWebSurface_PDF::SetWidth(const int w)
{
    const int width = GetI("width");

    if (w != width) {
        SetI("width", w);
    }
}

void AssetWebSurface_PDF::SetHeight(const int h)
{
    const int height = GetI("height");

    if (h != height) {
        SetI("height", h);
    }
}

TextureHandle* AssetWebSurface_PDF::GetTextureHandle() const
{
    return m_texture_handle.get();
}

void AssetWebSurface_PDF::UpdateGL()
{
    const int width = GetI("width");
    const int height = GetI("height");

//    qDebug() << "AssetWebSurface_PDF::UpdateGL()" << webasset.GetProgress() << webasset.GetError() << webasset.GetFinished() << document << webasset.GetDataSize() << tex;

    //do everything here
    if (!GetLoaded()) {
        return;
    }

    if (document == NULL && GetData().size() > 0) {
        buffer = new QBuffer(new QByteArray(GetData()));
        if (!buffer->open(QIODevice::ReadWrite)){
            delete document;
            document = NULL;
            delete buffer;
            buffer = NULL;
            return;
        }
        document = new QPdfDocument();
        document->load(buffer);
        buffer->close();
        ClearData();
        SetFinished(true);
    }

    if (document && document->status() == QPdfDocument::Ready && update_viewport)  {
        update_viewport = false;

        buffer->open(QIODevice::ReadWrite);
        RenderPDFDocument(document, cur_page, temp_image, width, height);
        buffer->close();

        if (!temp_image.isNull()) {
            //tex = MathUtil::CreateTextureGL(QSize(width, height), true, true, true); //100+ms
            if (!m_texture_handle) {
                m_texture_handle = RendererInterface::m_pimpl->CreateTextureQImage(temp_image, true, true, false, TextureHandle::ALPHA_TYPE::BLENDED, TextureHandle::COLOR_SPACE::SRGB);
            }
            else {
                RendererInterface::m_pimpl->UpdateTextureHandleData(m_texture_handle.get(), 0 ,0, 0,
                        temp_image.width(), temp_image.height(), GL_RGBA, GL_UNSIGNED_BYTE, (void *)temp_image.constBits(), temp_image.width() * temp_image.height() * 4);
                RendererInterface::m_pimpl->GenerateTextureHandleMipMap(m_texture_handle.get());
            }
        }
    }
}

QString AssetWebSurface_PDF::GetFireBoxCodePacket() const
{
    return QString();
}

void AssetWebSurface_PDF::ReadFireBoxCode(const QString & , const QString & )
{
}

WebHitTestResult AssetWebSurface_PDF::GetHitTestResult(const int ) const
{
    return WebHitTestResult();
}

QRect AssetWebSurface_PDF::GetHitTestResultRect(const int ) const
{
    return QRect();
}

QPointer <AbstractWebView> AssetWebSurface_PDF::GetWebView()
{
    QPointer <AbstractWebView> null_webview = nullptr;
    return null_webview;
}

bool AssetWebSurface_PDF::GetTextEditing()
{
    return false;
}

void AssetWebSurface_PDF::SetFocus(const bool b)
{
    cursor_active = b;
}

bool AssetWebSurface_PDF::GetFocus() const
{
    return cursor_active;
}

