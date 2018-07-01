#ifndef FRUSTUMCULLINGMANAGER_H
#define FRUSTUMCULLINGMANAGER_H

#include "debugrenderer.h"
#ifdef WIN32
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "LinearMath/btVector3.h"
#elif defined(__ANDROID__)
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#else
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#endif
#include "QVector3D"
#include "QQuaternion"
#include "memory"

class FrustumCullingProxyObject;
class FrustumCullingPhysicsWorld;
class FrustumCullingManager;

class FrustumCullingProxyObject
{
    friend class FrustumCullingPhysicsWorld;
public:
    ~FrustumCullingProxyObject();

    FrustumCullingPhysicsWorld* GetOwnerWorld();

private:
    FrustumCullingProxyObject(FrustumCullingPhysicsWorld * p_owner_world, QVector3D p_half_extents, QVector3D p_scale, QVector3D p_position, QQuaternion p_rotation);
    FrustumCullingPhysicsWorld * m_owner_world;
    std::unique_ptr<btCollisionShape> m_collision_shape;
    std::unique_ptr<btMotionState> m_motion_state;
    std::unique_ptr<btRigidBody> m_rigid_body;
};

class FrustumCullingPhysicsWorld
{
    friend class FrustumCullingManager;
public:

    // p_extents is the un-transformed extents of the AABB, this
    // p_model_matrix is the transform applied to the AABB(turning it into an OBB)
    std::unique_ptr<FrustumCullingProxyObject> CreateNewBoxProxy(QVector3D p_half_extents, QVector3D p_scale, QVector3D p_position, QQuaternion p_rotation);
    void RemoveProxyObject(FrustumCullingProxyObject * p_proxy_object);

    void DebugDrawWorld();
    ~FrustumCullingPhysicsWorld();
private:
    FrustumCullingPhysicsWorld(DebugRenderer * p_debug_renderer);

    std::unique_ptr<btDefaultCollisionConfiguration> m_collision_configuration;
    std::unique_ptr<btCollisionDispatcher> m_collision_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase_interface;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_constraint_solver;
    std::unique_ptr<btDynamicsWorld> m_dynamics_world;
};

class FrustumCullingManager
{
public:
    static FrustumCullingManager* GetSingleton()
    {
        static FrustumCullingManager * singleton = new FrustumCullingManager();
        return singleton;
    }

    std::unique_ptr<FrustumCullingPhysicsWorld> CreateNewPhysicsWorld();

private:
    FrustumCullingManager();
    DebugRenderer m_debug_renderer;
};

#endif // FRUSTUMCULLINGMANAGER_H
