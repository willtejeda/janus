#include "asset.h"

Asset::Asset()
{        
    props = new DOMNode();
    SetS("_type", "asset");
    SetS("_tagname", "Asset");
//    qDebug() << "Asset::Asset()" << props;

    //60.0 - make js_id property invalid for Assets
    props->setProperty("js_id", QVariant());
}

void Asset::SetV(const char * name, const QVector3D p)
{
    if (props) {
        props->SetV(name, p);
    }
}

QVector3D Asset::GetV(const char * name) const
{
    if (props) {
        return props->GetV(name);
    }
    return QVector3D();
}

void Asset::SetV4(const char * name, const QVector4D p)
{
    if (props) {
        props->SetV4(name, p);
    }

}

QVector4D Asset::GetV4(const char * name) const
{
    if (props) {
        return props->GetV4(name);
    }
    return QVector4D();
}

void Asset::SetF(const char * name, const float f)
{
    if (props) {
        props->SetF(name, f);
    }
}

float Asset::GetF(const char * name) const
{
    if (props) {
        return props->GetF(name);
    }
    return 0.0f;
}

void Asset::SetI(const char * name, const int i)
{
    if (props) {
        props->SetI(name, i);
    }
}

int Asset::GetI(const char * name) const
{
    if (props) {
        return props->GetI(name);
    }
    return 0;
}

void Asset::SetB(const char * name, const bool b)
{
    if (props) {
        props->SetB(name, b);
    }
}

bool Asset::GetB(const char * name) const
{
    if (props) {
        return props->GetB(name);
    }
    return false;
}

void Asset::SetS(const char * name, const QString s)
{
    if (props) {
        props->SetS(name, s);
    }
}

QString Asset::GetS(const char * name) const
{
    if (props) {
        return props->GetS(name);
    }
    return QString();
}

void Asset::SetC(const char * name, const QColor c)
{
    if (props) {
        props->SetC(name, c);
    }
}

QColor Asset::GetC(const char * name) const
{
    if (props) {
        return props->GetC(name);
    }
    return QColor(255,255,255);
}

void Asset::SetSrc(const QString & base, const QString & src_str)
{
//    qDebug() << "Asset::SetSrc" << base << src_str;
    SetS("_base_url", base);
    if (src_str.right(5) == "data:") {
        SetS("src", src_str);
        SetS("_src_url", "");
        SetURL(src_str);
    }
    else {
        SetS("src", src_str.trimmed());
        SetS("_src_url", QUrl(base).resolved(src_str).toString());
        SetURL(GetS("_src_url"));
    }
}

void Asset::SetProperties(const QVariantMap & d)
{
    QVariantMap::const_iterator it;
    for (it=d.begin(); it!=d.end(); ++it) {
//        qDebug() << " " << it.key() << it.value().toString();
        if (!it.key().isEmpty()) {
            props->SetS(it.key().trimmed().toLower().toLatin1().data(), it.value().toString());
        }
    }
}

QString Asset::GetXMLCode() const
{
    QString s = QString("<") + GetS("_tagname");
    QList <QByteArray> p = props->dynamicPropertyNames();
    for (int i=0; i<p.size(); ++i) {
        if (props->GetSaveAttribute(p[i].data(), false)) {
            s += QString(" ") + QString(p[i]) + "=\"" + props->property(p[i]).toString() + "\"";
        }
    }
    s += " />";
    return s;
}

QVariantMap Asset::GetJSONCode() const
{
    QVariantMap m;
    QList <QByteArray> p = props->dynamicPropertyNames();
    for (int i=0; i<p.size(); ++i) {
        if (props->GetSaveAttribute(p[i].data(), false)) {
            m[p[i]] = props->property(p[i]);
        }
    }
    return m;
}
