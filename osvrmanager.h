#ifndef OSVRMANAGER_H
#define OSVRMANAGER_H

#include <QtCore>
#include <QtGui>

//#include <osvr/ClientKit/ClientKit.h>
//#include <osvr/ClientKit/DisplayC.h>

#include "abstracthmdmanager.h"
#include "mathutil.h"

#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/RenderKit/RenderManager.h>
#include <osvr/RenderKit/GraphicsLibraryOpenGL.h>

class OSVRManager : public AbstractHMDManager
{
public:
    OSVRManager();

    bool Initialize();
    void PostPresent();
    void InitializeGL();

    bool GetEnabled() const;
    QSize GetTextureSize() const;
    float GetIPD() const;
    QString GetHMDString() const;
    QString GetHMDType() const;

    void Update();

    QMatrix4x4 GetHMDTransform() const;
    QMatrix4x4 GetControllerTransform(const int i) const;
    QMatrix4x4 GetLastControllerTransform(const int i) const;

    int GetNumControllers() const;
    bool GetControllerTracked(const int i);
    QVector2D GetControllerThumb(const int i) const;
    bool GetControllerThumbTouched(const int i) const;
    bool GetControllerThumbPressed(const int i) const;

    float GetControllerTrigger(const int i) const;
    float GetControllerGrip(const int i) const;
    bool GetControllerMenuPressed(const int i) const;

    bool GetControllerButtonAPressed() const;
    bool GetControllerButtonBPressed() const;
    bool GetControllerButtonXPressed() const;
    bool GetControllerButtonYPressed() const;

    bool GetControllerButtonATouched() const;
    bool GetControllerButtonBTouched() const;
    bool GetControllerButtonXTouched() const;
    bool GetControllerButtonYTouched() const;

    void BeginRenderEye(const int eye);
    void EndRenderEye(const int eye);

    void BeginRendering();
    void EndRendering();

    void ReCentre();

    void SetNearDist(const float f);
    float GetNearDist() const;
    void SetFarDist(const float f);
    float GetFarDist() const;

    void TriggerHapticPulse(const int i, const int val);

    //OVR Platform
    void Platform_ProcessMessages();
    bool Platform_GetEntitled() const;
    bool Platform_GetShouldQuit() const;

private:

    osvr::clientkit::ClientContext context;
    osvr::renderkit::RenderManager * render;
    std::vector<osvr::renderkit::RenderInfo> renderInfo;
    std::vector<osvr::renderkit::RenderBuffer> colorBuffers;

    QMatrix4x4 projM;
    QMatrix4x4 viewM;

    bool flip_vertical; //needed for Meta it seems
};

#endif // OSVRMANAGER_H
