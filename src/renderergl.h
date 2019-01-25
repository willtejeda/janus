#ifndef RENDERERGL
#define RENDERERGL

#include <QObject>
#include <QImage>
#include <QWindow>

#include <QThread>
#include <memory>

#include "abstractrenderer.h"
#include "jniutil.h"

class RendererGL : public QObject, public AbstractRenderer
{
    Q_OBJECT
public:
    RendererGL();
    virtual ~RendererGL();
    virtual void Initialize();
    virtual void Render(QHash<int, QVector<AbstractRenderCommand>> * p_scoped_render_commands,
                        QHash<StencilReferenceValue, LightContainer> * p_scoped_light_containers);
    virtual void PreRender(QHash<int, QVector<AbstractRenderCommand> > * p_scoped_render_commands,
                           QHash<StencilReferenceValue, LightContainer> * p_scoped_light_containers);
    virtual void PostRender(QHash<int, QVector<AbstractRenderCommand> > * p_scoped_render_commands,
                            QHash<StencilReferenceValue, LightContainer> * p_scoped_light_containers);
    virtual void UpgradeShaderSource(QByteArray & p_shader_source, bool p_is_vertex_shader);
    virtual std::shared_ptr<ProgramHandle> CompileAndLinkShaderProgram(QByteArray * p_vertex_shader, QString p_vertex_shader_path,
                                                                              QByteArray * p_fragment_shader, QString p_fragment_shader_path);
    virtual void CreateMeshHandleForGeomVBOData(GeomVBOData * p_VBO_data);
    virtual std::shared_ptr<MeshHandle> CreateMeshHandle(VertexAttributeLayout p_layout);
    virtual void InitializeGLObjects();
    void CompileAndLinkShaderProgram2(std::shared_ptr<ProgramHandle> *p_abstract_program, QByteArray *p_vertex_shader, QString p_vertex_shader_path, QByteArray *p_fragment_shader, QString p_fragment_shader_path, QVector<QVector<GLint> > *p_map);
    //virtual void RemoveMeshHandleFromMap(MeshHandle *p_handle);

    //from renderthread
    void SaveScreenshot();

    void InitializeGLContext(QOpenGLContext* p_gl_context);    
    void CreateMeshHandle(std::shared_ptr<MeshHandle> * p_handle, VertexAttributeLayout p_layout);
    void DecoupledRender();       

private:

    //renderthread
    void UpdatePerObjectData(QHash<int, QVector<AbstractRenderCommand> > * p_scoped_render_commands);

    void RenderEqui();

    QOffscreenSurface *  m_gl_surface;
    QOpenGLContext * m_gl_context;
    QOpenGLExtraFunctions * m_gl_funcs;    

    GLuint m_main_fbo;
    std::shared_ptr<TextureHandle> m_equi_cubemap_handle;
    uint32_t m_equi_cubemap_face_size;    
    bool m_is_initialized;
    bool m_hmd_initialized;

    bool m_screenshot_pbo_pending;
    GLuint m_screenshot_pbo;
};

#endif // RENDERERGL
