#ifndef RENDERERGL44_H
#define RENDERERGL44_H

#include "abstractrenderer.h"

#if !defined(__APPLE__) && !defined(__ANDROID__)
class RendererGL44 : public AbstractRenderer
{
public:
    RendererGL44();
    virtual ~RendererGL44();
    virtual void Initialize();
    virtual void Render(RENDERER::RENDER_SCOPE const p_scope, std::vector<AbstractRenderCommand> const & p_object_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> const & p_scoped_light_containers);
    virtual void PreRender(std::unordered_map<size_t, std::vector<AbstractRenderCommand> > & p_scoped_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> & p_scoped_light_containers);
    virtual void PostRender(std::unordered_map<size_t, std::vector<AbstractRenderCommand> > & p_scoped_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> & p_scoped_light_containers);
    virtual void UpgradeShaderSource(QByteArray & p_shader_source, bool p_is_vertex_shader);

protected:
    void UpdatePerObjectSSBO(std::unordered_map<size_t, std::vector<AbstractRenderCommand>> & p_scoped_render_commands, const bool p_using_instancing = false);
    void GL44ShaderSourceUpgrade(QByteArray& p_shader_source, bool p_is_vertex_shader);
    PersistentGLBuffer<static_cast<uint32_t>(BUFFER_CHUNK_COUNT), 2> m_perObjectBuffer;
    PersistentGLBuffer<static_cast<uint32_t>(BUFFER_CHUNK_COUNT), 3> m_perMaterialBuffer;
    PersistentGLBuffer<static_cast<uint32_t>(BUFFER_CHUNK_COUNT), 4> m_perInstanceBuffer;
    PersistentGLBuffer<static_cast<uint32_t>(BUFFER_CHUNK_COUNT), 5> m_perViewportBuffer;
};
#endif

#endif // RENDERERGL44_H
