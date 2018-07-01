#ifndef GVRRENDERER_H
#define GVRRENDERER_H

//#define GLdouble float
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>
#include <QtPlatformHeaders/QEGLNativeContext>

#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QtGui>

#include "vr/gvr/capi/include/gvr.h"
#include "vr/gvr/capi/include/gvr_controller.h"
#include "vr/gvr/capi/include/gvr_gesture.h"

#include "mathutil.h"
#include "rendererinterface.h"
#include "abstracthmdmanager.h"
#include "jniutil.h"
#include "renderer.h"

class GVRManager : public AbstractHMDManager
{

public:

    GVRManager();

    bool Initialize();
	void PostPresent();
	void InitializeGL();

    bool GetEnabled() const;    
    QSize GetTextureSize() const;
    float GetIPD() const;
    QString GetHMDString() const;
    QString GetHMDType() const;

    void EnterVR();
    void ExitVR();
    void Update();

    QMatrix4x4 GetHMDTransform() const;
    QMatrix4x4 GetControllerTransform(const int i) const;
    QMatrix4x4 GetLastControllerTransform(const int i) const;

    int GetNumControllers() const;    
    bool GetControllerTracked(const int i);

    QVector2D GetControllerThumbpad(const int i) const;
    bool GetControllerThumbpadTouched(const int i) const;
    bool GetControllerThumbpadPressed(const int i) const;

    QVector2D GetControllerStick(const int i) const;
    bool GetControllerStickTouched(const int i) const;
    bool GetControllerStickPressed(const int i) const;

    float GetControllerTrigger(const int i) const;
    float GetControllerGrip(const int i) const;
    bool GetControllerMenuPressed(const int i);

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

    void TriggerHapticPulse(const int i, const int val);

    void Platform_ProcessMessages();
    bool Platform_GetEntitled() const;
    bool Platform_GetShouldQuit() const;

private:

    QMatrix4x4 GVRMatrixToMatrix(gvr::Mat4f m);
    gvr::Mat4f MatrixToGVRMatrix(QMatrix4x4 m);
    QMatrix4x4 PerspectiveMatrixFromView(const gvr::Rectf& fov, float near_clip, float far_clip);
    QMatrix4x4 SymmetricPerspectiveMatrixFromView(const gvr::Rectf& lfov, const gvr::Rectf& rfov, float near_clip, float far_clip);

    gvr_context* context;
    gvr::Frame frame;
    std::unique_ptr<gvr::GvrApi> gvr_api;
    std::unique_ptr<gvr::ControllerApi> controller_api;
    gvr::ControllerState controller_state;
    std::unique_ptr<gvr::SwapChain> swap_chain;
    gvr::BufferViewportList buffer_viewports;
    gvr::BufferViewport left_viewport;
    gvr::BufferViewport right_viewport;

    bool gl_initialized;

    gvr::Sizei idealTextureSize;

    //gvr::ClockTimePoint pred_time;
    QMatrix4x4 hmd_xform;
    gvr::Mat4f gvr_hmd_xform;
    //QMatrix4x4 left_eye_view;
    //QMatrix4x4 right_eye_view;

    QMatrix4x4 controller_xform;
    QMatrix4x4 last_controller_xform;

    bool initialized;
};

#endif // GVRRENDERER_H
