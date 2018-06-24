#include "debugrenderer.h"

DebugRenderer::DebugRenderer()
    : m_debugMode(btIDebugDraw::DBG_NoDebug)
{
    m_shader = QPointer<AssetShader>(new AssetShader());
    m_shader->SetSrc(MathUtil::GetApplicationURL(), "assets/shaders/trans_frag.txt", "");
    m_shader->SetS("id", "BulletPhysicsDebugShader");
    m_shader->Load();
}

DebugRenderer::~DebugRenderer()
{
    delete m_shader;
}

void DebugRenderer::drawLine(const btVector3& , const btVector3& , const btVector3& )
{
    /*// These verts are in the physics engine's world-space, since we use the same units
    // this is also world-space for our renderer which means we want an identity modelmatrix
    // to avoid double applying the model to world space transform.

    // Room Uniforms
    MathUtil::RoomMatrix().setToIdentity();

    // Object Uniforms
    MathUtil::PushModelMatrix();
    MathUtil::LoadModelIdentity();


    m_shader->SetConstColour(QVector4D(color.getX(), color.getY(), color.getZ(), 1.0f));
    m_shader->UpdateObjectUniforms();
    MathUtil::PopModelMatrix();

    // Material Uniforms
    m_shader->SetUseClipPlane(false);
    m_shader->SetUseLighting(false);
    m_shader->SetUseTextureAll(false);

    // Create VAO and VBO
    float verts[6] = {from.getX(),  from.getY(),    from.getZ(),
                      to.getX(),    to.getY(),      to.getZ()};
    GLuint line_vao;
    MathUtil::glFuncs->glGenVertexArrays(1, &line_vao);
    MathUtil::glFuncs->glBindVertexArray(line_vao);
    GLuint line_vbo;
    MathUtil::glFuncs->glGenBuffers(1, &line_vbo);
    MathUtil::glFuncs->glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
    MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);
    MathUtil::glFuncs->glEnableVertexAttribArray((GLuint)VAO_ATTRIB::POSITION);
    MathUtil::glFuncs->glVertexAttribPointer((GLuint)VAO_ATTRIB::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Render line
    RendererInterface * renderer = RendererInterface::m_pimpl;
    renderer->SetPolyMode(PolyMode::LINE);
    renderer->PushAbstractRenderCommand(AbstractRenderCommand(PrimitiveType::LINES,
                                                              2,
                                                              1,
                                                              0,
                                                              0,
                                                              0,
                                                              line_vao,
                                                              m_shader,
                                                              m_shader->GetFrameUniforms(),
                                                              m_shader->GetRoomUniforms(),
                                                              m_shader->GetObjectUniforms(),
                                                              m_shader->GetMaterialUniforms(),
                                                              renderer->GetCurrentlyBoundTextures(),
                                                              FaceCullMode::DISABLED,
                                                              DepthFunc::ALWAYS,
                                                              DepthMask::DEPTH_WRITES_DISABLED,
                                                              StencilFunc(StencilTestFuncion::ALWAYS, StencilReferenceValue(0), StencilMask(0xffffffff)),
                                                              StencilOp(StencilOpAction::KEEP, StencilOpAction::KEEP, StencilOpAction::KEEP),
                                                              renderer->GetPolyMode(),
                                                              ColorMask::COLOR_WRITES_ENABLED,
                                                              BlendFunction::SRC_ALPHA,
                                                              BlendFunction::ONE_MINUS_SRC_ALPHA));
    renderer->SetPolyMode(PolyMode::FILL);

    // Clean up VAO and VBO
    MathUtil::glFuncs->glDeleteVertexArrays(1, &line_vao);
    MathUtil::glFuncs->glDeleteBuffers(1, &line_vbo);*/
}

void DebugRenderer::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

int DebugRenderer::getDebugMode() const
{
    return m_debugMode;
}

void DebugRenderer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
    (void) PointOnB;
    (void) normalOnB;
    (void) distance;
    (void) lifeTime;
    (void) color;
    // TODO
}

void DebugRenderer::reportErrorWarning(const char* warningString)
{
    (void) warningString;
    // TODO
}

void DebugRenderer::draw3dText(const btVector3& location, const char* textString)
{
    (void) location;
    (void) textString;
    // TODO
}
