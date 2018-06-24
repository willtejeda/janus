#ifndef FRAMEBUFFERMANAGER_H
#define FRAMEBUFFERMANAGER_H

#ifdef __ANDROID__
//#define GLdouble float
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#endif

#include <cstdint>
#include <vector>
#include <QtDebug>
#include <QtOpenGL>

#include "mathutil.h"
#include "performancelogger.h"
/*
namespace FBO_TEXTURE
{
    enum
    {
        COLOR                   = 0,
        TRANSMISSION            = 1,
        MODULATION_DIFFUSION    = 2,
        DELTA                   = 3,
        COMPOSITED              = 4,
        DEPTH_STENCIL           = 5,
        COUNT                   = 6,
        SIZE_FORCE              = 0xffffffff
    };
}
typedef uint32_t FBO_TEXTURE_ENUM;

namespace FBO_TEXTURE_BITFIELD
{
    enum
    {
        NONE                    = 0,
        COLOR                   = 1 << 0,
        TRANSMISSION            = 1 << 1,
        MODULATION_DIFFUSION    = 1 << 2,
        DELTA                   = 1 << 3,
        COMPOSITED              = 1 << 4,
        DEPTH_STENCIL           = 1 << 5,
        ALL                     = COLOR | DEPTH_STENCIL // For now I don't want to bind the other layers
    };
}
typedef uint32_t FBO_TEXTURE_BITFIELD_ENUM;
*/
class FramebufferManager
{
public:
    FramebufferManager();
    ~FramebufferManager();
    void InitializeGL();
    void initDefaultFBO();
    void configureFramebuffer(int const p_windowWidth, int const p_windowHeight, int const p_sampleCount);
    void configureSamples(int const p_sampleCount);
    std::vector<GLenum> bindFBOToRead(FBO_TEXTURE_BITFIELD_ENUM const p_textures_bitmask, bool const p_bind_multisampled = true) const;
	std::vector<GLenum> bindFBOToDraw(FBO_TEXTURE_BITFIELD_ENUM const p_textures_bitmask, bool const p_bind_multisampled = true) const;
    void bindFramebuffer(FBO_TEXTURE_BITFIELD_ENUM const p_textures_bitmask, bool const p_multisampled) const;
    void blitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD_ENUM const p_textures,
                                                         GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                                         GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1) const;
    void blitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD_ENUM const p_textures) const;
    void resolveMultisampleFramebuffer();
    void swapBuffers();
    GLuint getTextureID(FBO_TEXTURE_ENUM const p_texture_index, bool const p_multisampled) const;
    TextureHandle* getTextureHandle(FBO_TEXTURE_ENUM const p_texture_index, bool const p_multisampled) const;

    GLsizei m_windowWidth;
    GLsizei m_windowHeight;
    GLsizei m_sampleCount;
    size_t m_frameCounter;
    static FramebufferManager * m_framebufferManager;    

private:
    void deleteGLObjects();
    static void checkFrameBufferCompleteness(GLenum const p_target);
    void bindFBOAndTextures(std::vector<GLenum>& p_bound_buffers, GLenum const p_texture_type,
                            GLenum const p_framebuffer_target, GLuint const p_fbo,
                            size_t const p_texture_offset, FBO_TEXTURE_BITFIELD_ENUM const p_textures) const;


    std::vector<GLuint> m_FBOs;
    std::vector<GLuint> m_textures;
    std::vector<std::shared_ptr<TextureHandle>> m_texture_handles;
    bool m_isConfigured;
};

#endif // FRAMEBUFFERMANAGER_H
