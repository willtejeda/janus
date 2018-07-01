#ifndef RENDERERGL44_VIEWPORTINSTANCING_H
#define RENDERERGL44_VIEWPORTINSTANCING_H

#include "renderergl44.h"

#if !defined(__APPLE__) && !defined(__ANDROID__)

class RendererGL44_ViewportInstancing : public RendererGL44
{
public:
    RendererGL44_ViewportInstancing();
    virtual ~RendererGL44_ViewportInstancing();
    virtual void Initialize();
    virtual void Render(RENDERER::RENDER_SCOPE const p_scope, std::vector<AbstractRenderCommand> const & p_object_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> const & p_scoped_light_containers);
    virtual void UpgradeShaderSource(QByteArray& p_shader_source, bool p_is_vertex_shader);

private:
    void ConfigureTransparencyFramebuffer();
    GLuint m_transparency_FBO;
    std::vector<GLuint> m_transparency_FBO_textures;
};
#endif

#endif // RENDERERGL44_VIEWPORTINSTANCING_H
