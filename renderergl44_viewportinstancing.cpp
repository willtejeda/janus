#include "renderergl44_viewportinstancing.h"

#if !defined(__APPLE__) && !defined(__ANDROID__)
RendererGL44_ViewportInstancing::RendererGL44_ViewportInstancing()
    : m_transparency_FBO(0)
{
    ConfigureTransparencyFramebuffer();
}

RendererGL44_ViewportInstancing::~RendererGL44_ViewportInstancing()
{
    for (size_t i = 0; i < m_transparency_FBO_textures.size(); ++i)
    {
       MathUtil::glFuncs->glDeleteTextures(static_cast<GLsizei>(m_transparency_FBO_textures.size()), &m_transparency_FBO_textures[i]);
    }

    MathUtil::glFuncs->glDeleteFramebuffers(1, &m_transparency_FBO);
}

void RendererGL44_ViewportInstancing::Initialize()
{
    RendererGL44::Initialize();
    m_name = QString("OpenGL 4.4 with viewport instancing");
}

void RendererGL44_ViewportInstancing::Render(RENDERER::RENDER_SCOPE const , std::vector<AbstractRenderCommand> const & , std::unordered_map<StencilReferenceValue, LightContainer> const & )
{
    //AbstractRenderer::RenderObjectsStereoViewportInstanced(p_scope, p_object_render_commands, p_scoped_light_containers);
}

void RendererGL44_ViewportInstancing::UpgradeShaderSource(QByteArray & p_shader_source, bool p_is_vertex_shader)
{
    p_shader_source.replace("uniform vec4 iConstColour;",                      "");
    p_shader_source.replace("uniform vec4 iChromaKeyColour;",                  "");
    p_shader_source.replace("uniform vec4 iUseLighting;",                      "");
    p_shader_source.replace("uniform mat4 iModelMatrix;",                      "");
    p_shader_source.replace("uniform mat4 iViewMatrix;",                       "");
    p_shader_source.replace("uniform mat4 iProjectionMatrix;",                 "");
    p_shader_source.replace("uniform mat4 iInverseViewMatrix;",                "");
    p_shader_source.replace("uniform mat4 iModelViewMatrix;",                  "");
    p_shader_source.replace("uniform mat4 iModelViewProjectionMatrix;",        "");
    p_shader_source.replace("uniform mat4 iTransposeInverseModelMatrix;",      "");
    p_shader_source.replace("uniform mat4 iTransposeInverseModelViewMatrix;",  "");
    p_shader_source.replace("uniform vec4 iUseSkelAnim;",  "");
    p_shader_source.replace("uniform vec4 iObjectPickID;",  "");

    // Remove unneeded material uniform declarations
    p_shader_source.replace("uniform vec4 iAmbient;",           "");
    p_shader_source.replace("uniform vec4 iDiffuse;",           "");
    p_shader_source.replace("uniform vec4 iSpecular;",          "");
    p_shader_source.replace("uniform vec4 iShininess;",         "");
    p_shader_source.replace("uniform vec4 iEmission;",          "");
    p_shader_source.replace("uniform vec4 iLightmapScale;",     "");
    p_shader_source.replace("uniform vec4 iTiling;",            "");
    p_shader_source.replace("uniform vec4 iUseTexture[4];",     "");


    if (p_is_vertex_shader)
    {
        // Add in new vertex shader outputs
        p_shader_source.replace("void main",
                                "flat out int iInstanceID;\n"
                                "flat out int iViewportID;\n"
                                "void main"
                                );

        // Add in new vertex shader viewport and instance ID code
        p_shader_source.replace("gl_Position",
                                "iInstanceID = gl_InstanceID;\n"
                                "iViewportID = gl_InstanceID % int(iViewportCount.x);\n"
                                "gl_ViewportIndex = iViewportID;\n"
                                "gl_Position"
                                );

        p_shader_source.replace("iObjectPickID",                        "iInstanceID"); //56.0 - if iObjectPickID was used in code, just replace with iInstanceID

        // Replace object uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iConstColour",                        "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iConstColour");
        p_shader_source.replace("iChromaKeyColour",                    "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iChromaKeyColour");
        p_shader_source.replace("iUseLighting",                        "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iUseLighting");
        p_shader_source.replace("iModelMatrix",                        "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iModelMatrix");
        p_shader_source.replace("iViewMatrix",                         "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iViewMatrix");
        p_shader_source.replace("iProjectionMatrix",                   "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iProjectionMatrix");
        p_shader_source.replace("iInverseViewMatrix",                  "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iInverseViewMatrix");
        p_shader_source.replace("iModelViewMatrix",                    "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iModelViewMatrix");
        p_shader_source.replace("iModelViewProjectionMatrix",          "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iModelViewProjectionMatrix");
        p_shader_source.replace("iTransposeInverseModelMatrix",        "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iTransposeInverseModelMatrix");
        p_shader_source.replace("iTransposeInverseModelViewMatrix",    "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iTransposeInverseModelViewMatrix");
        p_shader_source.replace("iUseSkelAnim",                        "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iUseSkelAnim");

        // Replace material uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iAmbient",        "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iAmbient");
        p_shader_source.replace("iDiffuse",        "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iDiffuse");
        p_shader_source.replace("iSpecular",       "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iSpecular");
        p_shader_source.replace("iShininess",      "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iShininess");
        p_shader_source.replace("iEmission",       "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iEmission");
        p_shader_source.replace("iLightmapScale",  "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iLightmapScale");
        p_shader_source.replace("iTiling",         "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iTiling");
        p_shader_source.replace("iUseTexture",     "per_instance_material_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iUseTexture");
    }
    else
    {
        // Add in new fragment shader inputs
        p_shader_source.replace("void main",
                                "flat in int iInstanceID;\n"
                                "flat in int iViewportID;\n"
                                "void main"
                                );

        p_shader_source.replace("iObjectPickID",                        "iInstanceID"); //56.0 - if iObjectPickID was used in code, just replace with iInstanceID

		// Add gamma correction step as we output to a linear texture
        prependDataInShaderMainFunction(p_shader_source, g_gamma_correction_GLSL);

        // Replace object uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iConstColour",                        "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iConstColour");
        p_shader_source.replace("iChromaKeyColour",                    "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iChromaKeyColour");
        p_shader_source.replace("iUseLighting",                        "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iUseLighting");
        p_shader_source.replace("iModelMatrix",                        "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iModelMatrix");
        p_shader_source.replace("iViewMatrix",                         "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iViewMatrix");
        p_shader_source.replace("iProjectionMatrix",                   "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iProjectionMatrix");
        p_shader_source.replace("iInverseViewMatrix",                  "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iInverseViewMatrix");
        p_shader_source.replace("iModelViewMatrix",                    "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iModelViewMatrix");
        p_shader_source.replace("iModelViewProjectionMatrix",          "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iModelViewProjectionMatrix");
        p_shader_source.replace("iTransposeInverseModelMatrix",        "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iTransposeInverseModelMatrix");
        p_shader_source.replace("iTransposeInverseModelViewMatrix",    "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iTransposeInverseModelViewMatrix");
        p_shader_source.replace("iUseSkelAnim",                        "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iUseSkelAnim");


        // Replace material uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iAmbient",        "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iAmbient");
        p_shader_source.replace("iDiffuse",        "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iDiffuse");
        p_shader_source.replace("iSpecular",       "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iSpecular");
        p_shader_source.replace("iShininess",      "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iShininess");
        p_shader_source.replace("iEmission",       "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iEmission");
        p_shader_source.replace("iLightmapScale",  "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iLightmapScale");
        p_shader_source.replace("iTiling",         "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iTiling");
        p_shader_source.replace("iUseTexture",     "per_instance_material_data_array[int(iInstanceID + iMiscObjectData[0][0])].iUseTexture");
    }

    // Replace GLSL 330 define with GLSL 440 define, extensions, and SSBO info
    p_shader_source.replace("#version 330 core",
                            "#version 440 core\n"
                            "#extension GL_AMD_vertex_shader_viewport_index : enable\n"
                            "#extension GL_NV_viewport_array2 : enable\n"
                            "uniform vec4 iViewportCount;\n"
                            "struct Per_object_data\n"
                            "{\n"
                            "    mat4 iMiscObjectData;\n"
                            "    vec4 iConstColour;\n"
                            "    vec4 iChromaKeyColour;\n"
                            "    vec4 iUseLighting;\n"
                            "    mat4 iModelMatrix;\n"
                            "    mat4 iViewMatrix;\n"
                            "    mat4 iProjectionMatrix;\n"
                            "    mat4 iInverseViewMatrix;\n"
                            "    mat4 iModelViewMatrix;\n"
                            "    mat4 iModelViewProjectionMatrix;\n"
                            "    mat4 iTransposeInverseModelMatrix;\n"
                            "    mat4 iTransposeInverseModelViewMatrix;\n"
                            "    vec4 iUseSkelAnim;\n"
                            "};\n"
                            "\n"
                            "layout(std430, binding = 2) buffer per_instance_data\n"
                            "{\n"
                            "    Per_object_data per_object_data_array[];\n"
                            "};\n"
                            "\n"
                            "struct Per_material_data\n"
                            "{\n"
                            "    vec4 iAmbient;\n"
                            "    vec4 iDiffuse;\n"
                            "    vec4 iSpecular;\n"
                            "    vec4 iShininess;\n"
                            "    vec4 iEmission;\n"
                            "    vec4 iLightmapScale;\n"
                            "    vec4 iTiling;\n"
                            "    vec4 iUseTexture[4];\n"
                            "};\n"
                            "\n"
                            "layout(std430, binding = 3) buffer per_instance_material_data\n"
                            "{\n"
                            "    Per_material_data per_instance_material_data_array[];\n"
                            "};\n"
                            "\n"
                            );
}

void RendererGL44_ViewportInstancing::ConfigureTransparencyFramebuffer()
{
    /*GLuint rt0 = 0;
    MathUtil::glFuncs->glGenTextures(1, &rt0);
    m_transparency_FBO_textures.emplace_back(rt0);
    BindTexture(0, GL_TEXTURE_2D, rt0);

    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    MathUtil::glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA16F), 1280,  720, 0, GL_RGBA, GL_FLOAT, NULL);

    GLuint rt1 = 0;
    MathUtil::glFuncs->glGenTextures(1, &rt1);
    m_transparency_FBO_textures.emplace_back(rt1);
    BindTexture(0, GL_TEXTURE_2D, rt1);
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    MathUtil::glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA8), 1280,  720, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    GLuint rt2 = 0;
    MathUtil::glFuncs->glGenTextures(1, &rt2);
    m_transparency_FBO_textures.emplace_back(rt2);
    BindTexture(0, GL_TEXTURE_2D, rt2);
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    MathUtil::glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RG8_SNORM), 1280,  720, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);

    BindTexture(0, GL_TEXTURE_2D, 0);
    MathUtil::glFuncs->glGenFramebuffers(1, &m_transparency_FBO);
    MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_transparency_FBO);
    MathUtil::glFuncs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt0, 0);
    MathUtil::glFuncs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt1, 0);
    MathUtil::glFuncs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, rt2, 0);

    GLenum drawBuffers[3];
    drawBuffers[0] = GL_COLOR_ATTACHMENT0;
    drawBuffers[1] = GL_COLOR_ATTACHMENT1;
    drawBuffers[2] = GL_COLOR_ATTACHMENT2;
    GLsizei drawBufferCount = 3;
    MathUtil::glFuncs->glDrawBuffers(drawBufferCount, &drawBuffers[0]);

    GLenum error = MathUtil::glFuncs->glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (error != GL_FRAMEBUFFER_COMPLETE)
    {
        switch(error)
        {
            case GL_FRAMEBUFFER_UNDEFINED : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_UNDEFINED."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER."); break;
            case GL_FRAMEBUFFER_UNSUPPORTED : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_UNSUPPORTED."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS."); break;
            case GL_INVALID_ENUM : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS."); break;
            default : qDebug("ERROR: Framebuffer incomplete, error not recognised: "); break;
        }
    }*/
}
#endif
