#include "frustumcullingmanager.h"


FrustumCullingPhysicsWorld::FrustumCullingPhysicsWorld(DebugRenderer* p_debug_renderer)
{
    m_collision_configuration = std::unique_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
    m_collision_dispatcher = std::unique_ptr<btCollisionDispatcher>(new btCollisionDispatcher(m_collision_configuration.get()));
    m_broadphase_interface = std::unique_ptr<btBroadphaseInterface>(new btDbvtBroadphase());
    m_constraint_solver = std::unique_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
    m_dynamics_world = std::unique_ptr<btDynamicsWorld>(new btDiscreteDynamicsWorld(m_collision_dispatcher.get(),
                                                   m_broadphase_interface.get(),
                                                   m_constraint_solver.get(),
                                                   m_collision_configuration.get()));
    m_dynamics_world.get()->setDebugDrawer(p_debug_renderer);
}

FrustumCullingPhysicsWorld::~FrustumCullingPhysicsWorld()
{
    for (int i = m_dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_dynamics_world->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamics_world->removeCollisionObject(obj);
        delete obj;
    }
}

std::unique_ptr<FrustumCullingProxyObject> FrustumCullingPhysicsWorld::CreateNewBoxProxy(QVector3D p_half_extents, QVector3D p_scale, QVector3D p_position, QQuaternion p_rotation)
{
    std::unique_ptr<FrustumCullingProxyObject> proxy(new FrustumCullingProxyObject(this, p_half_extents, p_scale, p_position, p_rotation));

    // Add rigid body to physics world
    m_dynamics_world->addRigidBody(proxy.get()->m_rigid_body.get());

    return proxy;
}

void FrustumCullingPhysicsWorld::RemoveProxyObject(FrustumCullingProxyObject * p_proxy_object)
{
    // Remove rigid body from physics world
    m_dynamics_world->removeCollisionObject(p_proxy_object->m_rigid_body.get());
}

void FrustumCullingPhysicsWorld::DebugDrawWorld()
{
    m_dynamics_world->debugDrawWorld();
}

FrustumCullingProxyObject::FrustumCullingProxyObject(FrustumCullingPhysicsWorld * p_owner_world, QVector3D p_half_extents, QVector3D p_scale, QVector3D p_position, QQuaternion p_orientation)
    : m_owner_world(p_owner_world)
{
    // Collision Shape
    m_collision_shape = std::unique_ptr<btCollisionShape>(new btBoxShape(btVector3(btScalar(p_half_extents.x() * p_scale.x()), btScalar(p_half_extents.y() * p_scale.y()), btScalar(p_half_extents.z() * p_scale.z()))));

    // Tranform
    QVector4D quat_scalers(p_orientation.toVector4D());
    btQuaternion quat(quat_scalers.x(), quat_scalers.y(), quat_scalers.z(), quat_scalers.w());
    btTransform transform;
    transform.setIdentity();
    transform.setRotation(quat);
    transform.setOrigin(btVector3(btScalar(p_position.x()), btScalar(p_position.y()), btScalar(p_position.z())));

    // Mass
    btScalar mass(0.0);

    // Intertia
    btVector3 local_inertia(0,0,0);

    // Motion State
    m_motion_state = std::unique_ptr<btMotionState>(new btDefaultMotionState(transform));

    // Rigid Body
    btRigidBody::btRigidBodyConstructionInfo info(mass, m_motion_state.get(), m_collision_shape.get(), local_inertia);
    m_rigid_body = std::unique_ptr<btRigidBody>(new btRigidBody(info));
}

FrustumCullingProxyObject::~FrustumCullingProxyObject()
{
    m_owner_world->RemoveProxyObject(this);
}

FrustumCullingPhysicsWorld* FrustumCullingProxyObject::GetOwnerWorld()
{
    return m_owner_world;
}


FrustumCullingManager::FrustumCullingManager()
    : m_debug_renderer()
{
    m_debug_renderer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

std::unique_ptr<FrustumCullingPhysicsWorld> FrustumCullingManager::CreateNewPhysicsWorld()
{
    return std::unique_ptr<FrustumCullingPhysicsWorld>(new FrustumCullingPhysicsWorld(&m_debug_renderer));
}
