#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui>

#include "domnode.h"
#include "settingsmanager.h"
#include "assetshader.h"
#include "leaphands.h"

class Player : public QObject
{
    Q_OBJECT

public:

    Player();   

    void SetProperties(QPointer <DOMNode> props);
    QPointer <DOMNode> GetProperties();

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

    void DrawLeapHandsGL(QPointer <AssetShader> shader);

    void SetViewGL(const bool render_left_eye, const float eye_ipd, const QMatrix4x4 xform, const bool no_pitch = false );

    void UpdateDir();
    void UpdateEyePoint();

    QVector3D GetRightDir() const;   

    QMatrix4x4 GetTransform() const;

    void SetCursorActive(const bool b, const int cursor_index);
    bool GetCursorActive(const int cursor_index) const;
    void SetCursorObject(const QString & s, const int cursor_index);
    QString GetCursorObject(const int cursor_index) const;
    void SetCursorPos(const QVector3D & p, const int cursor_index);
    QVector3D GetCursorPos(const int cursor_index) const;
    void SetCursorXDir(const QVector3D & p, const int cursor_index);
    QVector3D GetCursorXDir(const int cursor_index) const;
    void SetCursorYDir(const QVector3D & p, const int cursor_index);
    QVector3D GetCursorYDir(const int cursor_index) const;
    void SetCursorZDir(const QVector3D & p, const int cursor_index);
    QVector3D GetCursorZDir(const int cursor_index) const;
    void SetCursorScale(const float f, const int cursor_index);
    float GetCursorScale(const int cursor_index) const;

    float ComputeCursorScale(const int cursor_index) const;   

    QTime & GetComfortTimer();      

    void SpinView(const float f, const bool scale_rotation_speed);
    void TiltView(const float f);    

    bool GetWalking() const;

    void DoSpinLeft(const bool b);
    void DoSpinRight(const bool b);

    QVector3D GetViewToWorldPoint(const QVector3D & p) const;
    QVector3D GetViewToWorldDir(const QVector3D & d) const;

    void SetFlying(const bool b);

    void ResetCentre();

    void Update(const float move_speed);

    QPair <LeapHand, LeapHand> GetHands() const;
    LeapHand & GetHand(const int i);
    const LeapHand & GetHand(const int i) const;

private:

    QPointer <QObject> CreateControllerQObject();

    QPointer <DOMNode> props;   

    bool comfort_spinleft;
    bool comfort_spinright;
    QTime comfort_timer;      
    QTime time;
    bool flytransition;    
    float flyduration;       

    QPair <LeapHand, LeapHand> hands;  
};

#endif // PLAYER_H
