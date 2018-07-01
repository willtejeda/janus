#ifndef ASSET_H
#define ASSET_H

#include <QtCore>
#include <memory>

#include "domnode.h"
#include "webasset.h"

class Asset : public WebAsset
{
    Q_OBJECT

public:
    Asset();   

    void SetV(const char * name, QVector3D v);
    QVector3D GetV(const char * name) const;

    void SetV4(const char * name, QVector4D v);
    QVector4D GetV4(const char * name) const;

    void SetF(const char * name, float v);
    float GetF(const char * name) const;

    void SetI(const char * name, int v);
    int GetI(const char * name) const;

    void SetB(const char * name, bool v);
    bool GetB(const char * name) const;

    void SetS(const char * name, QString v);
    QString GetS(const char * name) const;

    void SetC(const char * name, QColor v);
    QColor GetC(const char * name) const;

    void SetProperties(const QVariantMap & d);

    void SetSrc(const QString & base, const QString & src_str);

    QString GetXMLCode() const;
    QVariantMap GetJSONCode() const;

protected:

    QPointer <DOMNode> props;       

};

#endif // ASSET_H
