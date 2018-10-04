#ifndef DOMNODE_H
#define DOMNODE_H

#include <QObject>
#include <QtScript>

#include "settingsmanager.h"
#include "scriptablevector.h"
#include "mathutil.h"

class DOMNode : public QObject, protected QScriptable
{
    Q_OBJECT

    Q_PROPERTY(ScriptableVector * fwd READ GetFwd WRITE SetFwd)
public:

    explicit DOMNode(QObject *parent = 0);
    ~DOMNode();

    void SetFwd(const ScriptableVector * v);
    ScriptableVector * GetFwd() const;

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

    void Copy(QPointer <DOMNode> o);

    bool GetIsCustomElement() const;

    void SetParent(DOMNode *);
    DOMNode * GetParent() const;

    void SetChildren(QList <DOMNode *>);
    QList <DOMNode *> GetChildren() const;
    void ClearChildren();

    void SetLeftSibling (DOMNode *);
    DOMNode * GetLeftSibling() const;

    void SetRightSibling (DOMNode*);
    DOMNode * GetRightSibling() const;

    void AppendChild (QPointer <DOMNode>);
    void PrependChild (QPointer <DOMNode>);
    void RemoveChild (QPointer <DOMNode>);
    void RemoveChildAt (int pos);
    QPointer <DOMNode> RemoveChildByJSID(QString);

    inline bool IsRoom()
    {
        return property("_type").toString() == "room";
    }

    inline bool IsObject()
    {
        return property("_type").toString() != "room";
    }

    inline bool IsDirty() const
    {
        return property("_dirty").toString() == "true";
    }

    inline void SetDirty(const bool val)
    {
        setProperty("_dirty", val ? "true" : "false");
    }

    bool GetSaveAttribute(const char * name, const bool even_if_default);

    static int NextUUID();
    static void SetDefaultAttributes();

public slots:

    void appendChild(DOMNode *);
    DOMNode * removeChild(QString);
    DOMNode * removeChild(DOMNode *);

private:

    void SetDefaults();

    //Connections
    QPointer <DOMNode> parent_node;
    QList <QPointer <DOMNode> > children_nodes;
    QPointer <DOMNode> left_sibling, right_sibling;

    static int next_uuid;
    static QPointer <DOMNode> default_object;
};

Q_DECLARE_METATYPE(QList <DOMNode *>)
#endif // DOMNODE_H
