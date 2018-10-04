#include "domnode.h"

int DOMNode::next_uuid = 0;
QPointer <DOMNode> DOMNode::default_object;

DOMNode::DOMNode(QObject *parent): QObject(parent)
{
    SetI("_uuid", NextUUID());
    SetS("js_id", GetS("_uuid"));
}

DOMNode::~DOMNode()
{
//    qDebug() << "DOMNode::~DOMNode()" << this;
    for (QList <QPointer <DOMNode> >::iterator it = children_nodes.begin(); it!=children_nodes.end(); ++it) {
        if (!it->isNull()) {
            delete *it;
        }
    }
    children_nodes.clear();
}

void DOMNode::SetDefaultAttributes()
{
    if (default_object.isNull()) {
        default_object = new DOMNode();
        default_object->SetDefaults();
    }
}

void DOMNode::SetDefaults()
{
    //janus internals
    SetS("_type", "object");
    SetI("_uuid", NextUUID());
    SetB("_text_changed", false);
    SetB("_url_changed", false);
    SetB("_dirty", false);
    SetB("_circular", false);
    SetF("_progress", 0.0f);
    SetI("_cur_mount", 0);
    SetB("_ready", false);
    SetB("_ready_for_screenshot", false);
    SetB("_started_auto_play", false);
    SetB("_triggered", false);
    SetS("_original_url", "");
    SetI("_room_object_uuid", 0);
    SetB("_auto_load_triggered", false);
    SetB("_highlighted", false);
    SetB("_draw_back", false);
    SetB("_translator_busy", false);
    SetB("_teleport_override", false);
    SetB("_custom_element_processed", false);
    SetB("_reloaded", false);

    //public facing attributes
    SetS("use_local_asset", "");
    SetS("js_id", GetS("_uuid"));
    SetS("id", "");
    SetS("collision_id", "");
    SetS("shader_id", "");
    SetS("rect", "0 0 0 0");
    SetF("blend0",  0.0f);
    SetF("blend1",  0.0f);
    SetF("blend2",  0.0f);
    SetF("blend3",  0.0f);
    SetS("cull_face", "back");
    SetB("lighting", true);
    SetB("visible", true);
    SetB("collision_static", true);
    SetB("collision_trigger", false);
    SetF("collision_ccdmotionthreshold", 1.0f);
    SetF("collision_ccdsweptsphereradius", 0.0f);
    SetF("collision_radius", 0.0f);
    SetB("collision_response", true);
    SetB("emit_local", false);
    SetB("loop", false);
    SetB("play_once", false);
    SetF("pitch", 1.0f);
    SetF("gain", 1.0f);
    SetF("outer_gain", 0.0f);
    SetF("inner_angle", 360.0f);
    SetF("outer_angle", 360.0f);
    SetI("font_size", 32);
    SetB("sync", false);
    SetF("anim_speed", 1.0f);
    SetB("auto_load", false);
    SetB("draw_text", true);
    SetB("open", false);
    SetB("mirror", false);
    SetB("active", true);
    SetI("count", 0);
    SetI("rate", 1);
    SetF("duration", 1.0f);
    SetF("fade_in", 1.0f);
    SetF("fade_out", 1.0f);
    SetF("current_time", 0.0f);
    SetF("total_time", 0.0f);
    SetF("rotate_deg_per_sec", 0.0f);
    SetF("light_intensity", 1.0f);
    SetF("light_cone_angle", 0.0f);
    SetF("light_cone_exponent", 1.0f);
    SetF("light_range", 0.5f);
    SetI("draw_layer", 0);
    SetB("locked", false);
    SetV("pos", QVector3D(0,0,0));
    SetV("xdir", QVector3D(1,0,0));
    SetV("ydir", QVector3D(0,1,0));
    SetV("zdir", QVector3D(0,0,1));
    SetV("scale", QVector3D(1,1,1));
    SetV("rotation", QVector3D(0,0,0));
    SetS("rotation_order", "xyz");
    SetV("vel", QVector3D(0,0,0));
    SetV("accel", QVector3D(0,0,0));
    SetC("col", QColor(255,255,255,255));
    SetC("chromakey_col", QColor(0,0,0,0));
    SetV4("tile", QVector4D(1,1,0,0));
    SetV("rand_pos", QVector3D(0,0,0));
    SetV("rand_vel", QVector3D(0,0,0));
    SetV("rand_accel", QVector3D(0,0,0));
    SetV("rand_col", QVector3D(0,0,0));
    SetV("rand_scale", QVector3D(0,0,0));
    SetV4("lmap_scale", QVector4D(1,1,0,0));
    SetV("collision_pos", QVector3D(0,0,0));
    SetV("collision_scale", QVector3D(1,1,1));
    SetS("blend_src", "src_alpha");
    SetS("blend_dest", "one_minus_src_alpha");
    SetV("rotate_axis", QVector3D(0,1,0));
    SetB("auto_play", false);
    SetC("back_color", QColor(255,255,255));
    SetF("back_alpha", 1.0f);
    SetC("text_color", QColor(0,0,0));
    SetS("url", "");
    SetS("text", "");

    //room specific
    SetS("server", SettingsManager::GetServer());
    SetI("port", SettingsManager::GetPort());
    SetF("teleport_min_dist", 0.0f);
    SetF("teleport_max_dist", 100.0f);
    SetB("party_mode", true);
    SetB("cursor_visible", true);
    SetF("near_dist", 0.01f);
    SetF("far_dist", 1000.0f);
    SetF("grab_dist", 0.5f);
    SetB("fog", false);
    SetS("fog_mode", "exp");
    SetF("fog_density", 1.0f);
    SetF("fog_start", 0.0f);
    SetF("fog_end", 1.0f);
    SetC("fog_col", QColor(0,0,0,255));
    SetF("gravity", -9.8f);
    SetF("jump_velocity", 5.0f);
    SetF("walk_speed", 1.8f);
    SetF("run_speed", 5.4f);
    SetS("reset_volume", QString("-1000 -1000 -1000 1000 -100 1000"));

    //player specific
    SetV("dir", QVector3D(0, 0, -1));
    SetV("view_dir", QVector3D(0, 0, 0));
    SetV("up_dir", QVector3D(0, 1, 0));
    SetB("cursor_active", false);
    SetV("cursor_pos", QVector3D(0, 0, 0));
    SetV("cursor_xdir", QVector3D(1, 0, 0));
    SetV("cursor_ydir", QVector3D(0, 1, 0));
    SetV("cursor_zdir", QVector3D(0, 0, -1));
    SetF("cursor_scale", 1.0f);
    SetB("cursor0_active", false);
    SetV("cursor0_pos", QVector3D(0, 0, 0));
    SetV("cursor0_xdir", QVector3D(1, 0, 0));
    SetV("cursor0_ydir", QVector3D(0, 1, 0));
    SetV("cursor0_zdir", QVector3D(0, 0, -1));
    SetF("cursor0_scale", 1.0f);
    SetB("cursor1_active", false);
    SetV("cursor1_pos", QVector3D(0, 0, 0));
    SetV("cursor1_xdir", QVector3D(1, 0, 0));
    SetV("cursor1_ydir", QVector3D(0, 1, 0));
    SetV("cursor1_zdir", QVector3D(0, 0, -1));
    SetF("cursor1_scale", 1.0f);
    SetB("seated", true);
    SetS("oncollision", "");
    SetF("player_collision_radius", 0.2f);
    SetS("userid", "");
    SetS("hmd_type", "2d");
    SetB("hmd_enabled", false);
    SetB("hmd_calibrated", false);
    SetB("follow_mode", false);
    SetS("follow_mode_userid", "");
    SetV("local_head_pos", QVector3D(0, 0, 0));
    SetV("head_pos", QVector3D(0, 0, 0));
    SetV("eye_pos", QVector3D(0, 1.6f, 0));
    SetB("hand0_active", false);
    SetV("hand0_trackpad", QVector3D(0,0,0));
    SetV("hand0_pos", QVector3D(0,0,0));
    SetV("hand0_pos_old", QVector3D(0,0,0));
    SetV("hand0_vel", QVector3D(0,0,0));
    SetV("hand0_xdir", QVector3D(1,0,0));
    SetV("hand0_ydir", QVector3D(0,1,0));
    SetV("hand0_zdir", QVector3D(0,0,1));
    SetV("hand0_p0", QVector3D(0,0,0));
    SetV("hand0_p1", QVector3D(0,0,0));
    SetV("hand0_p2", QVector3D(0,0,0));
    SetV("hand0_p3", QVector3D(0,0,0));
    SetV("hand0_p4", QVector3D(0,0,0));
    SetB("hand1_active", false);
    SetV("hand1_trackpad", QVector3D(0,0,0));
    SetV("hand1_pos", QVector3D(0,0,0));
    SetV("hand1_pos_old", QVector3D(0,0,0));
    SetV("hand1_vel", QVector3D(0,0,0));
    SetV("hand1_xdir", QVector3D(1,0,0));
    SetV("hand1_ydir", QVector3D(0,1,0));
    SetV("hand1_zdir", QVector3D(0,0,1));
    SetV("hand1_p0", QVector3D(0,0,0));
    SetV("hand1_p1", QVector3D(0,0,0));
    SetV("hand1_p2", QVector3D(0,0,0));
    SetV("hand1_p3", QVector3D(0,0,0));
    SetV("hand1_p4", QVector3D(0,0,0));
    SetB("running", false);
    SetB("flying", false);
    SetB("walking", false);
    SetB("speaking", false);
    SetB("typing", false);
    SetB("entering_text", false);
    SetB("jump", false);
    SetB("recording", false);
    SetS("anim_id", "");
    SetV("impulse_vel", QVector3D(0,0,0));
    SetF("delta_time", 0.0f);
    SetF("scale_velx", 1.0f);
    SetF("scale_velz", 1.0f);
    SetF("hand_update_time", 0.0f);
    SetF("theta", 0.0f);
    SetF("correction_theta", 0.0f);
    SetF("phi", 90.0f);

    SetB("walk_forward", false);
    SetB("walk_back", false);
    SetB("walk_left", false);
    SetB("walk_right", false);
    SetB("spin_left", false);
    SetB("spin_right", false);
    SetB("fly_down", false);
    SetB("fly_up", false);

    SetI("sample_rate", 44100);

    SetB("sbs3d", false);
    SetB("ou3d", false);
    SetB("reverse3d", false);
    SetB("tex_clamp", false);
    SetB("tex_linear", true);
    SetB("tex_premultiply", true);
    SetB("tex_compress", false);
    SetB("tex_mipmap", true);
    SetS("tex_alpha", "undefined");
    SetS("tex_colorspace",  "sRGB");
    SetB("_save_to_markup", true);
    SetB("_primitive", false);
}
void DOMNode::SetFwd(const ScriptableVector * v)
{
    if (v) {
        QVector3D d(v->GetX(), v->GetY(), v->GetZ());

        const QVector3D z = d.normalized();
        const QVector3D x = QVector3D::crossProduct(QVector3D(0, 1, 0), z).normalized();
        const QVector3D y = QVector3D::crossProduct(z, x).normalized();

        SetV("xdir", x);
        SetV("ydir", y);
        SetV("zdir", z);
    }
}

ScriptableVector * DOMNode::GetFwd() const
{
    return qvariant_cast<ScriptableVector *>(property("zdir"));
}

void DOMNode::SetV(const char * name, const QVector3D p)
{
    if (!property(name).isValid() || property(name).isNull()) {
        setProperty(name, QVariant::fromValue<ScriptableVector *>(new ScriptableVector(p.x(), p.y(), p.z(), this)));
    }
    else {
        ScriptableVector *v=qvariant_cast<ScriptableVector *>(property(name));
        if (v) {
            v->SetX(p.x());
            v->SetY(p.y());
            v->SetZ(p.z());
        }
    }
}

QVector3D DOMNode::GetV(const char * name) const
{
    if (property(name).isValid() && !property(name).isNull()) {
        ScriptableVector *v=qvariant_cast<ScriptableVector *>(property(name));
        if (v) {
            return QVector3D(v->GetX(), v->GetY(), v->GetZ());
        }
    }
    if (this != default_object) {
        return default_object->GetV(name);
    }
    return QVector3D();
}

void DOMNode::SetV4(const char * name, const QVector4D p)
{
    if (!property(name).isValid() || property(name).isNull()) {
        setProperty(name, QVariant::fromValue<ScriptableVector *>(new ScriptableVector(p.x(), p.y(), p.z(), p.w(), this)));
    }
    else {
        ScriptableVector *v=qvariant_cast<ScriptableVector *>(property(name));
        if (v) {
            v->SetX(p.x());
            v->SetY(p.y());
            v->SetZ(p.z());
            v->SetW(p.w());
        }
    }
}

QVector4D DOMNode::GetV4(const char * name) const
{
    if (property(name).isValid() && !property(name).isNull()) {
        ScriptableVector *v=qvariant_cast<ScriptableVector *>(property(name));
        if (v) {
            return QVector4D(v->GetX(), v->GetY(), v->GetZ(), v->GetW());
        }
    }
    if (this != default_object) {
        return default_object->GetV4(name);
    }
    return QVector4D();
}

void DOMNode::SetF(const char * name, const float f)
{
    setProperty(name, QString::number(f));
}

float DOMNode::GetF(const char * name) const
{
    if (property(name).isValid()) {
        return property(name).toFloat();
    }
    if (this != default_object) {
        return default_object->GetF(name);
    }
    return 0.0f;
}

void DOMNode::SetI(const char * name, const int i)
{
    setProperty(name, QString::number(i));
}

int DOMNode::GetI(const char * name) const
{
    if (property(name).isValid()) {
        return property(name).toInt();
    }
    if (this != default_object) {
        return default_object->GetI(name);
    }
    return 0;
}

void DOMNode::SetB(const char * name, const bool b)
{
    setProperty(name, b ? "true" : "false");
}

bool DOMNode::GetB(const char * name) const
{
    if (property(name).isValid()) {
        return property(name).toString().toLower() == "true";
    }
    if (this != default_object) {
        return default_object->GetB(name);
    }
    return false;
}

void DOMNode::SetS(const char * name, const QString s)
{
    ScriptableVector *v0 = qvariant_cast<ScriptableVector *>(property(name));
    if (v0) {
        SetV(name, MathUtil::GetStringAsVector(s));
    }
    else {
        if (this != default_object && default_object && default_object->property(name).isValid()) {
            ScriptableVector *v1 = qvariant_cast<ScriptableVector *>(default_object->property(name));
            if (v1) {
                SetV(name, MathUtil::GetStringAsVector(s));
            }
            else {
                setProperty(name, s);
            }
        }
        else {
            setProperty(name, s);
        }
    }
}

QString DOMNode::GetS(const char * name) const
{
    ScriptableVector *v=qvariant_cast<ScriptableVector *>(property(name));
    if (v) {
        return QString::number(v->GetX()) + " " +
                QString::number(v->GetY()) + " " +
                QString::number(v->GetZ());
    }
    if (property(name).isValid()) {
        return property(name).toString();
    }
    if (this != default_object) {
        return default_object->GetS(name);
    }
    return QString();
}

void DOMNode::SetC(const char * name, const QColor c)
{
    setProperty(name, MathUtil::GetColourAsString(c, false));
}

QColor DOMNode::GetC(const char * name) const
{
    ScriptableVector *v=qvariant_cast<ScriptableVector *>(property(name));
    //release 60.0 - col can be assigned via a vector type (e.g. in JS)
    if (v) {
        QString s = QString::number(v->GetX()) + " " +
                QString::number(v->GetY()) + " " +
                QString::number(v->GetZ());
        if (v->GetW() != 1.0f) {
            s += " " + QString::number(v->GetW());
        }
        return MathUtil::GetStringAsColour(s);
    }
    if (property(name).isValid()) {
        return MathUtil::GetStringAsColour(property(name).toString());
    }
    if (this != default_object) {
        return default_object->GetC(name);
    }
    return QColor(255,255,255,255);
}

void DOMNode::Copy(QPointer<DOMNode> o)
{
    if (o) {
        QList <QByteArray> b = o->dynamicPropertyNames();
        for (int i=0; i<b.size(); ++i) {
            if (!o->property(b[i]).isNull()) {
                ScriptableVector *v = qvariant_cast<ScriptableVector *>(o->property(b[i]));
                if (v) {
                    SetV(b[i], QVector3D(v->GetX(), v->GetY(), v->GetZ()));
                }
                else {
                    setProperty(b[i], o->property(b[i]));
                }
            }
        }
    }
}

int DOMNode::NextUUID()
{
    return next_uuid++;
}

bool DOMNode::GetIsCustomElement() const
{
    const QString t = GetS("_type");
    return (t != "link" &&
            t != "sound" &&
            t != "light" &&
            t != "image" &&
            t != "image3d" &&
            t != "object" &&
            t != "video" &&
            t != "sound" &&
            t != "particle" &&
            t != "text" &&
            t != "paragraph");
}

void DOMNode::AppendChild(QPointer<DOMNode> child)
{
    if (child) {
        if (child->GetParent()) {
            child->GetParent()->RemoveChildByJSID(child->GetS("js_id"));
        }

        if (!children_nodes.empty() && children_nodes.last()) {
            children_nodes.last()->SetRightSibling(child);
            child->SetLeftSibling(children_nodes.last());
        }
        children_nodes.append(child);
        child->SetParent(this);
    }
}

void DOMNode::appendChild(DOMNode *child)
{
    QString oldParentJSID = "__null";
    if (child->GetParent())
        oldParentJSID = child->GetParent()->GetS("js_id");
    AppendChild(QPointer <DOMNode>(child));
    if (engine() && oldParentJSID!=QString("__null"))
    {
        QScriptValue parentChanged = engine()->globalObject().property("__parent_changed");
        if (!parentChanged.isValid()) {
            parentChanged = engine()->newObject();
            engine()->globalObject().setProperty("__parent_changed", parentChanged);
        }
        QScriptValue map = engine()->newObject();
        QScriptValue existingMap = parentChanged.property(child->GetS("js_id"));
        if (existingMap.isValid()) {
            map.setProperty("old_parent_id", existingMap.property("old_parent_id"));
        }
        else {
            map.setProperty("old_parent_id", oldParentJSID);
        }
        //map.setProperty("new_parent_id", QScriptValue(QString("__null")));
        map.setProperty("new_parent_id", GetS("js_id"));

        parentChanged.setProperty(child->GetS("js_id"), map);
    }
}

void DOMNode::RemoveChild(QPointer <DOMNode> n)
{
    RemoveChildAt(children_nodes.indexOf(n));
}

void DOMNode::PrependChild(QPointer<DOMNode> child)
{
    if (!children_nodes.empty())
    {
        children_nodes.first()->SetLeftSibling(child);
        child->SetRightSibling(children_nodes.first());
    }
    children_nodes.prepend(child);
    child->SetParent(this);
}

void DOMNode::SetChildren(QList<DOMNode* > children)
{
    if (engine())
    {
        context()->throwError("Unable to set the children property. Use appendChild() instead.");
        return;
    }
    children_nodes.clear();

    for (int i=0; i<children.size(); ++i)
        children_nodes.append(QPointer <DOMNode>(children[i]));
}

void DOMNode::SetParent(DOMNode *parent)
{
    if (engine())
    {
        context()->throwError("Unable to set the parent property. Use appendChild() on the intended parent instead.");
        return;
    }
    parent_node = QPointer <DOMNode>(parent);
}

void DOMNode::SetLeftSibling(DOMNode *node)
{
//    if (engine())
//    {
//        context()->throwError("The leftSibling property is read-only.");
//        return;
//    }

    left_sibling = QPointer <DOMNode>(node);
}

void DOMNode::SetRightSibling(DOMNode *node)
{
//    if (engine())
//    {
//        context()->throwError("The rightSibling property is read-only.");
//        return;
//    }
    right_sibling = QPointer <DOMNode>(node);
}

DOMNode *DOMNode::GetParent() const
{
    if (parent_node && parent_node->IsDirty())
        return NULL;
    return parent_node.data();
}

DOMNode * DOMNode::GetLeftSibling() const
{
    if (left_sibling && left_sibling->IsDirty())
        return left_sibling->GetLeftSibling();
    return left_sibling.data();
}

DOMNode* DOMNode::GetRightSibling() const
{
    if (right_sibling && right_sibling->IsDirty())
        return right_sibling->GetRightSibling();
    return right_sibling.data();
}

QList <DOMNode *> DOMNode::GetChildren() const
{
    QList <DOMNode *> unguarded_children;
    for (int i=0; i<children_nodes.size(); ++i)
    {
        //trick to ensure that deleted objects currently in "dirty" state are not returned.
//        qDebug()<<children_nodes[i]->GetJSID()<<children_nodes[i]->IsDirty();
        if (children_nodes[i] && !children_nodes[i]->IsDirty()) {
            unguarded_children.append(children_nodes[i].data());
        }
    }
//    qDebug() << "DOMNode::GetChildren()" << children_nodes.size() << unguarded_children.size();
    return unguarded_children;
}

void DOMNode::ClearChildren()
{
    children_nodes.clear();
}

void DOMNode::RemoveChildAt(int pos)
{
    if (pos>=0 && pos<children_nodes.size())
    {
        if (pos-1>=0 && children_nodes[pos-1])
            children_nodes[pos-1]->SetRightSibling(children_nodes[pos]->GetRightSibling());
        if (pos+1<children_nodes.size() && children_nodes[pos+1])
            children_nodes[pos+1]->SetLeftSibling(children_nodes[pos]->GetLeftSibling());

        children_nodes[pos]->SetParent(NULL);
        children_nodes.removeAt(pos);
    }
}

QPointer <DOMNode> DOMNode::RemoveChildByJSID(QString jsid)
{
    QPointer <DOMNode> child;
    for (int i=0; i<children_nodes.size(); ++i)
    {
        if (children_nodes[i]->GetS("js_id")==jsid)
        {
            child = children_nodes[i];
            RemoveChildAt(i);
            return child;
        }
    }
    return child;
}

//removeChild() is the user faceing JS function

DOMNode * DOMNode::removeChild(QString jsid)
{
    QPointer <DOMNode> child = RemoveChildByJSID(jsid);
    if (child)
    {
        if (engine())
        {
            QScriptValue parentChanged = engine()->globalObject().property("__parent_changed");
            if (!parentChanged.isValid()) {
                parentChanged = engine()->newObject();
                engine()->globalObject().setProperty("__parent_changed", parentChanged);
            }
            QScriptValue map = engine()->newObject();
            QScriptValue existingMap = parentChanged.property(jsid);
            if (existingMap.isValid())
                map.setProperty("old_parent_id", existingMap.property("old_parent_id"));
            else
                map.setProperty("old_parent_id", GetS("js_id"));
            map.setProperty("new_parent_id", QScriptValue(QString("__null")));
            parentChanged.setProperty(child->GetS("js_id"), map);

            QScriptValue roomObject = engine()->globalObject().property("__room");
            roomObject.property("appendChild").call(roomObject, engine()->toScriptValue <DOMNode *>(child.data()));
        }

//        qDebug()<<jsid<<"detached from parent"<<GetJSID();
        return child.data();
    }
    else
        return NULL;
}

DOMNode * DOMNode::removeChild(DOMNode *node)
{
    QPointer <DOMNode> child = RemoveChildByJSID(node->GetS("js_id"));
    if (child) {
        if (engine()) {
            QScriptValue parentChanged = engine()->globalObject().property("__parent_changed");
            if (!parentChanged.isValid()) {
                parentChanged = engine()->newObject();
                engine()->globalObject().setProperty("__parent_changed", parentChanged);
            }
            QScriptValue map = engine()->newObject();
            QScriptValue existingMap = parentChanged.property(child->GetS("js_id"));
            if (existingMap.isValid()) {
                map.setProperty("old_parent_id", existingMap.property("old_parent_id"));
            }
            else {
                map.setProperty("old_parent_id", GetS("js_id"));
            }
            map.setProperty("new_parent_id", QScriptValue(QString("__null")));
            parentChanged.setProperty(child->GetS("js_id"), map);

            QScriptValue roomObject = engine()->globalObject().property("__room");
            roomObject.property("appendChild").call(roomObject, engine()->toScriptValue <DOMNode *>(child.data()));
        }
        return child.data();
    }
    else {
        return NULL;
    }
}

bool DOMNode::GetSaveAttribute(const char * name, const bool even_if_default)
{
    //attribs with leading underscore are for internal use
    QString n(name);
    if (n.left(1) == "_" || (n == "js_id" && GetS("_type") == "room")) {
        return false;
    }

    if (even_if_default || !default_object->property(name).isValid() || GetS(name) != default_object->GetS(name)) { //attribute value test
        //save it out if we either:
        // accept defaults,
        // have no default attrib for comparison (custom attribute),
        // or the attribute is not default
        return true;
    }
    return false;
}
