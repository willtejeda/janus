#include "portal.h"

float Portal::spacing = 0.1f;

Portal::Portal() :
    room(NULL),
    object(new RoomObject()),
    portal(NULL),            
    text(new RoomObject()),
    thumb_image(0),
    auto_load_triggered(false),
    highlighted(false),
    draw_back(false),
    circular(false),
    cull(false),
    swallow_time(0),
    swallow_state(0)
{    
//    qDebug() << "Portal::Portal()" << this;
    SetScale(QVector3D(1.8f, 2.5f, 1.0f));

    object->SetType(TYPE_LINK);
    object->SetInterpolate(false);

	text->SetType(TYPE_PARAGRAPH);
	text->SetBackAlpha(0.0f);
    text->SetFontSize(32);
	text->SetPos(QVector3D(0.0f, 0.0f, 0.0f));
	text->SetDir(QVector3D(0.0f, 0.0f, 1.0f));
	text->SetLighting(false);
}

Portal::~Portal()
{
    //This portal owns it's own object and text
    if (object) {
        delete object;
    }
    if (text) {
        delete text;
    }
}

void Portal::Reset()
{
    if (object) {
        object->SetOpen(false);
    }
    room.clear();
}

void Portal::SetProperties(QPointer <Portal> p)
{
    if (p.isNull()) {
        return;
    }

    circular = p->circular;
    thumb_image = p->thumb_image;

    object->CopyProperties(p->object);
}

QPointer <Portal> Portal::Copy()
{
    QPointer <Portal> p = new Portal();

    p->circular = circular;
    p->thumb_image = thumb_image;        
    p->room = room;    

    p->SetScale(GetScale());
    p->SetURL("", GetURL());
    p->SetTitle(GetTitle());
    p->SetPos(GetPos());
    p->SetXDirs(GetXDir(), GetYDir(), GetZDir());
    p->SetColour(GetColour());
    p->SetOpen(GetOpen());
    p->SetVisible(GetVisible());
    p->SetThumbID(GetThumbID());
    p->SetAutoLoad(GetAutoLoad());
    p->SetDrawGlow(GetDrawGlow());
    p->SetDrawText(GetDrawText());

    return p;
}

QString Portal::ShortenString(const QString & s)
{
    const int maxlen = 40;
    if (s.length() < maxlen) {
       return s;
    }
    else {
        return s.left(maxlen/2) + QString("...") + s.right(maxlen/2);
    }
}

void Portal::SetURL(const QString & base, const QString & url_str)
{    
    if (object) {
        if (object->GetOriginalURL().isEmpty()) {
            object->SetOriginalURL(url_str);
        }

//        const QString s = object->GetURL();
        object->SetURL(base, url_str);
//        qDebug() << "Portal::SetURL()" << base << url_str << s << object->GetURL();
    }
}

QString Portal::GetURL() const
{
    if (object) {
        return object->GetURL();
    }
    else {
        return QString();
    }
}

void Portal::SetTitle(const QString & s) {
    if (object) {
        object->SetTitle(s);
    }
}

QString Portal::GetTitle() const
{
    if (object) {
        return object->GetTitle();
    }
    else {
        return QString();
    }
}

void Portal::SetDrawText(const bool b)
{
    if (object) {
        object->SetDrawText(b);
    }
}

bool Portal::GetDrawText() const
{
    return (object ? object->GetDrawText() : false);
}

void Portal::SetDrawGlow(const bool b)
{
    if (object) {
        object->SetDrawGlow(b);
    }
}

bool Portal::GetDrawGlow() const
{
    return (object ? object->GetDrawGlow() : false);
}

void Portal::SetMirror(const bool b)
{
    if (object) {
        object->SetMirror(b);
    }
}

bool Portal::GetMirror() const
{
    return (object ? object->GetMirror() : false);
}

void Portal::SetActive(const bool b)
{
    if (object) {
        object->SetActive(b);
    }
}

bool Portal::GetActive() const
{
    return (object ? object->GetActive() : false);
}

void Portal::SetAutoLoad(const bool b)
{
    if (object) {
        object->SetAutoLoad(b);
    }
}

bool Portal::GetAutoLoad() const
{
    return (object ? object->GetAutoLoad() : false);
}

void Portal::SetAutoLoadTriggered(const bool b)
{
    auto_load_triggered = b;
}

bool Portal::GetAutoLoadTriggered() const
{
    return auto_load_triggered;
}

void Portal::SetLocked(const bool b)
{
    if (object) {
        object->SetLocked(b);
    }
}

bool Portal::GetLocked() const
{
    return (object ? object->GetLocked() : false);
}

void Portal::SetRoom(const QPointer <Room> r)
{
    room = r;
}

QPointer <Room> Portal::GetRoom() const
{
    return room;
}

void Portal::SetObject(const QPointer <RoomObject> o)
{        
    object = o;
    if (object) {
        object->SetInterpolate(false);
    }
}

QPointer <RoomObject> Portal::GetObject()
{
    return object;
}

void Portal::SetPortal(const QPointer <Portal> p)
{
    portal = p;
}

QPointer <Portal> Portal::GetPortal()
{
    return portal;
}

void Portal::PlayCreatePortal()
{
    if (object) {
        object->PlayCreatePortal();
    }
}

void Portal::SetPos(const QVector3D & p)
{    
    if (object) {
        object->SetPos(p);
    }
}

QVector3D Portal::GetPos() const
{
    return (object ? object->GetPos() : QVector3D());
}

void Portal::SetDir(const QVector3D & d)
{
    QVector3D t = MathUtil::GetRotatedAxis(MathUtil::_PI_OVER_2, d, QVector3D(0, 1, 0));
    QVector3D b = QVector3D(0, 1, 0);
    QVector3D n = d;
    if (object) {
        object->SetXDirs(t, b, n);
    }
}

QVector3D Portal::GetDir() const
{
    return (object ? object->GetZDir() : QVector3D());
}

void Portal::SetXDir(const QVector3D & d)
{
    if (object) {
        object->SetXDir(d);
    }
}

QVector3D Portal::GetXDir() const
{
    return (object ? object->GetXDir() : QVector3D());
}

void Portal::SetYDir(const QVector3D & d)
{
    if (object) {
        object->SetYDir(d);
    }
}

QVector3D Portal::GetYDir() const
{
    return (object ? object->GetYDir() : QVector3D());
}

void Portal::SetZDir(const QVector3D & d)
{
    if (object) {
        object->SetZDir(d);
    }
}

QVector3D Portal::GetZDir() const
{
    return (object ? object->GetZDir() : QVector3D());
}

void Portal::SetXDirs(const QString & x, const QString & y, const QString & z)
{
    if (object) {
        object->SetXDirs(x, y, z);
    }
}

void Portal::SetXDirs(const QVector3D & x, const QVector3D & y, const QVector3D & z)
{
    if (object) {
        object->SetXDirs(x, y, z);
    }
}

void Portal::SetColour(const QColor & c)
{
    if (object) {
        object->SetColour(c);
    }
}

QColor Portal::GetColour() const
{
    return (object ? object->GetColour() : QColor());
}

void Portal::DrawGL(QPointer <AssetShader> shader)
{
    if (object.isNull() || !object->GetVisible()) {
        return;
    }              

//    const bool processed = (room && room->GetProcessed());
    const bool draw_inside = !object->GetOpen();// || !processed;

//    qDebug() << "  Portal::DrawGL" << this << draw_inside << this->GetPos();
//    if (portal && portal->GetObject() && portal->portal) {
//        qDebug() << "    Portal::DrawGL" << portal->GetOpen() << portal->portal->GetOpen();
//    }
//    qDebug() << "Portal::DrawGL - Drawing portal" << object->GetVisible() << object->GetMirror() << fill_inside << draw_back << thumb_image;
    //object->DrawPortalGL(shader, draw_inside, draw_back, thumb_image, circular, highlighted);
    object->DrawPortalGL(shader, draw_inside, draw_back, thumb_image, circular, highlighted);
}

void Portal::DrawDecorationsGL(QPointer <AssetShader> shader)
{
//    qDebug() << "Portal::DrawGL - Drawing portal" << object->GetVisible();
    if (object.isNull() || !object->GetVisible()) {
        return;
    }

    if (object->GetDrawText() && (object->GetURL() != last_url || object->GetTitle() != last_title)) {
//        qDebug() << "Portal::DrawDecorationsGL() updating text" << object->GetURL() << last_url;
        text->SetText(QString("<p align=\"center\">") + object->GetTitle() + QString("<br><font color=\"#0000ff\">") + object->GetURL() + QString("</font></p>"), false);
        last_url = object->GetURL();
        last_title = object->GetTitle();
    }

    const bool startedurlrequest = (room && room->GetStartedURLRequest());
    const bool processed = (room && room->GetProcessed());
    const bool draw_loading = startedurlrequest && !processed;

    object->DrawPortalDecorationsGL(shader, startedurlrequest, text, circular, draw_loading);
}

void Portal::DrawStencilGL(QPointer <AssetShader> shader, const QVector3D & player_pos) const
{
    if (object.isNull() || !object->GetVisible()) {
        return;
    }

    object->DrawPortalStencilGL(shader, circular, GetPlayerAtSigned(player_pos));
}

float Portal::GetWidth() const
{
    return (object ? object->GetScale().x() : 1.0f);
}

bool Portal::GetPlayerFacing(const QVector3D & player_dir) const
{
    if (object) {
        return MathUtil::GetAngleBetweenRadians(player_dir, -object->GetZDir()) < MathUtil::_PI_OVER_4; //45 degree angle
    }
    else {
        return false;
    }
}

bool Portal::GetPlayerAt(const QVector3D & player_pos) const
{
    if (object) {
        const float width = object->GetScale().x() * 0.5f;
        const float height = object->GetScale().y();
        const QVector3D local = GetLocal(player_pos);
#ifdef __ANDROID__
        return (fabsf(local.x()) <= (width+spacing) && local.y() >= (0.0f-spacing) && local.y() <= (height+spacing) && fabsf(local.z()) <= 1.0f);
#else
        return (fabsf(local.x()) <= width && local.y() >= 0.0f && local.y() <= height && fabsf(local.z()) <= 1.0f);
#endif
    }
    else {
        return false;
    }
}

bool Portal::GetPlayerAtSigned(const QVector3D & player_pos) const
{
    if (object) {
        const float width = object->GetScale().x() * 0.5f;
        const float height = object->GetScale().y();
        const QVector3D local = GetLocal(player_pos);
        return (fabsf(local.x()) <= width && local.y() >= 0.0f && local.y() <= height && local.z() >= 0.0f && local.z() <= 1.0f);
    }
    else {
        return false;
    }
}

QVector3D Portal::GetLocal(const QVector3D & p) const
{    
    if (object) {
        QVector3D local = p - object->GetPos();
        return QVector3D(QVector3D::dotProduct(local, object->GetXDir()),
                         QVector3D::dotProduct(local, object->GetYDir()),
                         QVector3D::dotProduct(local, object->GetZDir()));
    }
    else {
        return QVector3D();
    }
}

QVector3D Portal::GetGlobal(const QVector3D & p) const
{   
    if (object) {
        return p.x() * object->GetXDir() + p.y() * object->GetYDir() + (p.z() + spacing) * object->GetZDir() + object->GetPos();
    }
    else {
        return QVector3D();
    }
}

void Portal::SetCircular(const bool b)
{
    circular = b;
}

bool Portal::GetCircular() const
{
    return circular;
}

void Portal::SetHighlighted(const bool b)
{
    highlighted = b;
}

bool Portal::GetHighlighted() const
{
    return highlighted;
}

QVector3D Portal::GetPointThroughPortal(const QVector3D & p)
{
    if (portal) {
        QVector3D new_pos = GetLocal(p);
        new_pos.setX(-new_pos.x());
        new_pos.setZ(-new_pos.z());
        new_pos = GetPortal()->GetGlobal(new_pos + QVector3D(0,0,spacing));
        return new_pos;
    }
    else {
        return p;
    }
}

bool Portal::GetPlayerCrossed(const QVector3D & player_pos, const QVector3D & player_last_pos) const
{
    QVector3D local1 = GetLocal(player_pos);
    QVector3D local2 = GetLocal(player_last_pos);

    return (local1.z() - spacing) < 0.0f && (local2.z() - spacing) >= 0.0f && GetPlayerAt(player_last_pos);
}

float Portal::GetSpacing()
{
    return spacing;
}

void Portal::SetScale(const QVector3D & s)
{    
    if (object) {
        object->SetScale(s);
    }
}

QVector3D Portal::GetScale() const
{
    return (object ? object->GetScale() : QVector3D());
}

QString Portal::GetJSID() const
{
    if (object) {
        return object->GetJSID();
    }
}

void Portal::SetThumbID(const QString & s)
{    
    if (object) {
        object->SetThumbID(s);
    }
}

QString Portal::GetThumbID() const
{
    return (object ? object->GetThumbID() : QString());
}

void Portal::SetThumbAssetImage(const unsigned int uuid)
{
    thumb_image = uuid;
}

unsigned int Portal::GetThumbAssetImage()
{
    return thumb_image;
}

void Portal::SetOpen(const bool b)
{
    if (object) {
        object->SetOpen(b);
    }
    swallow_time = QTime::currentTime().msecsSinceStartOfDay();
}

bool Portal::GetOpen() const
{
    return (object ? object->GetOpen() : false);
}

void Portal::SetVisible(const bool b)
{
    if (object) {
        object->SetVisible(b);
    }
}

bool Portal::GetVisible() const
{
    return (object ? object->GetVisible() : false);
}

void Portal::SetCull(const bool b)
{
    cull = b;
}

bool Portal::GetCull() const
{
    return cull;
}

void Portal::SetDrawBack(const bool b)
{
    draw_back = b;
}

bool Portal::GetDrawBack() const
{
    return draw_back;
}

QVariantMap Portal::GetJSONCode(const bool show_defaults) const
{
    return (object ? object->GetJSONCode(show_defaults) : QVariantMap());
}

QString Portal::GetXMLCode(const bool show_defaults) const
{
    return (object ? object->GetXMLCode(show_defaults) : QString());
}

void Portal::SetSaveToMarkup(const bool b)
{
    object->SetSaveToMarkup(b);
}

bool Portal::GetSaveToMarkup() const
{
    return (object ? object->GetSaveToMarkup() : true);
}
