#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

#include "LinearMath/btIDebugDraw.h"
#include "rendererinterface.h"
#include "assetshader.h"
#include "mathutil.h"

class DebugRenderer : public btIDebugDraw
{
public:
    DebugRenderer();
    ~DebugRenderer();

    virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
    virtual void setDebugMode(int debugMode);
    virtual int getDebugMode() const;
    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    virtual void reportErrorWarning(const char* warningString);
    virtual void draw3dText(const btVector3& location,const char* textString);

    QPointer<AssetShader> m_shader;
    int m_debugMode;
};

#endif // DEBUGRENDERER_H
