#include "framebuffermanager.h"

FramebufferManager * FramebufferManager::m_framebufferManager = nullptr;

GLsizei m_windowWidth;
GLsizei m_windowHeight;
GLsizei m_sampleCount;
size_t m_frameCounter;

FramebufferManager::FramebufferManager() :
    m_windowWidth(1280),
    m_windowHeight(720),
    m_sampleCount(4),
    m_frameCounter(0),
    m_isConfigured(false)
{
}

FramebufferManager::~FramebufferManager()
{
    deleteGLObjects();
}

GLuint FramebufferManager::getTextureID(FBO_TEXTURE_ENUM const p_texture_index, const bool p_multisampled) const
{
    const size_t offset = (p_multisampled) ? FBO_TEXTURE::COUNT : 0;
    GLuint texture_id = m_textures[p_texture_index + offset];
    return texture_id;
}

TextureHandle* FramebufferManager::getTextureHandle(const FBO_TEXTURE_ENUM p_texture_index, const bool p_multisampled) const
{
    return m_texture_handles[p_texture_index + (p_multisampled) ? FBO_TEXTURE::COUNT : 0].get();
}

void FramebufferManager::InitializeGL()
{    

}

void FramebufferManager::deleteGLObjects()
{
    if (m_isConfigured)
    {
        MathUtil::glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
        size_t const fbo_count = m_FBOs.size();
        MathUtil::glFuncs->glDeleteFramebuffers(static_cast<GLsizei>(fbo_count), m_FBOs.data());
        m_FBOs.clear();

        size_t const texture_count = m_textures.size();
        MathUtil::glFuncs->glDeleteTextures(static_cast<GLsizei>(texture_count), m_textures.data());
        m_textures.clear();
    }
    m_isConfigured = false;
}

void FramebufferManager::configureFramebuffer(int p_windowWidth, int p_windowHeight, int p_sampleCount)
{
    if (p_windowWidth == 0 || p_windowHeight == 0)
    {
        // This can happen during window initialisation on certain systems
        return;
    }

    bool const requiresReset = (m_windowWidth != p_windowWidth
            || m_windowHeight != p_windowHeight
            || m_sampleCount != p_sampleCount) ? true : false;

    m_windowWidth = p_windowWidth;
    m_windowHeight = p_windowHeight;
    m_sampleCount = p_sampleCount;

    if(requiresReset)
    {
        deleteGLObjects();
        initDefaultFBO();
    }
}

void FramebufferManager::configureSamples(GLsizei p_sampleCount)
{
    if (m_sampleCount != p_sampleCount)
    {
        m_sampleCount = p_sampleCount;
        deleteGLObjects();
        initDefaultFBO();
    }
}

void FramebufferManager::bindFBOAndTextures(std::vector<GLenum>& p_bound_buffers, GLenum const p_texture_type, GLenum const p_framebuffer_target, GLuint const p_fbo, size_t const p_texture_offset, FBO_TEXTURE_BITFIELD_ENUM const p_textures) const
{
    MathUtil::glFuncs->glBindFramebuffer((GLenum)p_framebuffer_target, (GLuint)p_fbo);

    if ((p_textures & FBO_TEXTURE_BITFIELD::COLOR) != 0)
    {
        MathUtil::glFuncs->glFramebufferTexture2D(p_framebuffer_target,
                                                  GL_COLOR_ATTACHMENT0,
                                                  p_texture_type,
                                                  m_textures[FBO_TEXTURE::COLOR + p_texture_offset],
                                                  0);
        p_bound_buffers.push_back(GL_COLOR_ATTACHMENT0);
    }

    if ((p_textures & FBO_TEXTURE_BITFIELD::TRANSMISSION) != 0)
    {
        MathUtil::glFuncs->glFramebufferTexture2D(p_framebuffer_target,
                                                  GL_COLOR_ATTACHMENT1,
                                                  p_texture_type,
                                                  m_textures[FBO_TEXTURE::TRANSMISSION + p_texture_offset],
                                                  0);
        p_bound_buffers.push_back(GL_COLOR_ATTACHMENT1);
    }

    if ((p_textures & FBO_TEXTURE_BITFIELD::MODULATION_DIFFUSION) != 0)
    {
        MathUtil::glFuncs->glFramebufferTexture2D(p_framebuffer_target,
                                                  GL_COLOR_ATTACHMENT2,
                                                  p_texture_type,
                                                  m_textures[FBO_TEXTURE::MODULATION_DIFFUSION + p_texture_offset],
                                                  0);
        p_bound_buffers.push_back(GL_COLOR_ATTACHMENT2);
    }

    if ((p_textures & FBO_TEXTURE_BITFIELD::DELTA) != 0)
    {
        MathUtil::glFuncs->glFramebufferTexture2D(p_framebuffer_target,
                                                  GL_COLOR_ATTACHMENT3,
                                                  p_texture_type,
                                                  m_textures[FBO_TEXTURE::DELTA + p_texture_offset],
                                                  0);
        p_bound_buffers.push_back(GL_COLOR_ATTACHMENT3);
    }

    if ((p_textures & FBO_TEXTURE_BITFIELD::COMPOSITED) != 0)
    {
        MathUtil::glFuncs->glFramebufferTexture2D(p_framebuffer_target,
                                                  GL_COLOR_ATTACHMENT4,
                                                  p_texture_type,
                                                  m_textures[FBO_TEXTURE::COMPOSITED + p_texture_offset],
                                                  0);
        p_bound_buffers.push_back(GL_COLOR_ATTACHMENT4);
    }

    if ((p_textures & FBO_TEXTURE_BITFIELD::DEPTH_STENCIL) != 0)
    {
        MathUtil::glFuncs->glFramebufferTexture2D(p_framebuffer_target,
                                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                                  p_texture_type,
                                                  m_textures[FBO_TEXTURE::DEPTH_STENCIL + p_texture_offset],
                                                  0);
    }
}

std::vector<GLenum> FramebufferManager::bindFBOToRead(FBO_TEXTURE_BITFIELD_ENUM const p_textures, bool const p_bind_multisampled /*= true*/) const
{
    std::vector<GLenum> read_buffers;

	if (m_isConfigured)
	{
		bool is_multisampled = (p_bind_multisampled) ? ((m_sampleCount > 0) ? true : false) : p_bind_multisampled;
		if ((is_multisampled && m_FBOs.size() >= 2) || (!is_multisampled && m_FBOs.size() >= 1))
		{
			GLuint const fbo = m_FBOs[(is_multisampled) ? 1 : 0];
			MathUtil::glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

			read_buffers.reserve(FBO_TEXTURE::COUNT);
			GLenum const texture_type = (is_multisampled) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			GLenum const framebuffer_target = GL_READ_FRAMEBUFFER;
			size_t const texture_offset = (is_multisampled) ? FBO_TEXTURE::COUNT : 0;

			bindFBOAndTextures(read_buffers, texture_type, framebuffer_target, fbo, texture_offset, p_textures);

			MathUtil::glFuncs->glReadBuffer((read_buffers.size() != 0) ? read_buffers[0] : GL_COLOR_ATTACHMENT0);
		}
    }
    else
    {
        qDebug("ERROR: FramebufferManager::bindFBOToRead() called while not configured.");
    }

    return read_buffers;
}

std::vector<GLenum> FramebufferManager::bindFBOToDraw(FBO_TEXTURE_BITFIELD_ENUM const p_textures, bool const p_bind_multisampled /*= true*/) const
{
    std::vector<GLenum> draw_buffers;

	if (m_isConfigured)
	{
		bool is_multisampled = (p_bind_multisampled) ? ((m_sampleCount > 0) ? true : false) : p_bind_multisampled;
		if ((is_multisampled && m_FBOs.size() >= 2) || (!is_multisampled && m_FBOs.size() >= 1))
		{
			GLuint const fbo = m_FBOs[(is_multisampled) ? 1 : 0];
			draw_buffers.reserve(FBO_TEXTURE::COUNT);
			GLenum const texture_type = (is_multisampled) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			GLenum const framebuffer_target = GL_DRAW_FRAMEBUFFER;
			size_t const texture_offset = (is_multisampled) ? FBO_TEXTURE::COUNT : 0;

			bindFBOAndTextures(draw_buffers, texture_type, framebuffer_target, fbo, texture_offset, p_textures);

			size_t const draw_buffers_size = draw_buffers.size();
			if (draw_buffers_size == 0)
			{
                GLenum buf = GL_NONE;
                MathUtil::glFuncs->glDrawBuffers(1, &buf);
			}
			else
			{
				MathUtil::glFuncs->glDrawBuffers(static_cast<GLsizei>(draw_buffers_size), draw_buffers.data());
			}
		}
    }
    else
    {
        qDebug("ERROR: FramebufferManager::bindFBOToDraw() called while not configured.");
    }

    return draw_buffers;
}


void FramebufferManager::resolveMultisampleFramebuffer()
{
    /*if(m_isConfigured)
    {
        if (p_eyeIndex >= m_FBOs.size())
        {
            return;
        }

        if (m_sampleCount > 0)
        {
            // Bind nothing to GL_READ_FRAMEBUFFER to make sure our MSAA fbo textures are available for binding
            MathUtil::glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            // Bind non-MSAA fbo to GL_DRAW_FRAMEBUFFER
            bindFBOToDraw(p_eyeIndex, false);
            glDepthFunc(GL_ALWAYS);
            glDisable(GL_STENCIL_TEST);

            GLuint texture = m_textures[p_eyeIndex][0][m_frameCounter%2][0];

            // Bind the textures that make up the MSAA fbo as input textures
            MathUtil::glFuncs->glActiveTexture(GL_TEXTURE0);
            MathUtil::glFuncs->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);

            // Draw our full-screen sized quad as an alternative to blitting that can do all layers simultaniously
			RendererInterface::m_pimpl->BindVAO(MathUtil::m_fullScreenQuadVAO);
            GLuint shaderProgram = MathUtil::m_fullScreenQuadShaderProgram;
            MathUtil::glFuncs->glUseProgram(shaderProgram);
            MathUtil::glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);           

            // Unbind our textures
            MathUtil::glFuncs->glActiveTexture(GL_TEXTURE0);
            MathUtil::glFuncs->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            bindFBOToRead(p_eyeIndex, false);
            bindFBOToDraw(p_eyeIndex, true);

            glDepthFunc(GL_LEQUAL);
            glEnable(GL_STENCIL_TEST);
        }
    }
    else
    {
        qDebug("ERROR: FramebufferManager::bindFBOToRead() called while not configured.");
    }*/
}

void FramebufferManager::blitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD_ENUM const p_textures) const
{
    blitMultisampledFramebuffer(p_textures, 0, 0, m_windowWidth, m_windowHeight, 0, 0, m_windowWidth, m_windowHeight);
}
void FramebufferManager::blitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD_ENUM const p_textures,
                                                     GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                                     GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1) const
{
    if(m_isConfigured)
    {
        if (m_sampleCount > 0)
        {
            std::vector<GLenum> read_buffers = bindFBOToRead(p_textures, true);
            bindFBOToDraw(p_textures, false);

            const size_t read_buffers_size = read_buffers.size();
            for (size_t read_buffer_index = 0; read_buffer_index < read_buffers_size; ++read_buffer_index)
            {
                MathUtil::glFuncs->glReadBuffer(read_buffers[read_buffer_index]);
                MathUtil::glFuncs->glDrawBuffers(1, &(read_buffers[read_buffer_index]));
                MathUtil::glFuncs->glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
                                                     dstX0, dstY0, dstX1, dstY1,
                                                     GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }

            bindFBOToRead(p_textures, false);
            bindFBOToDraw(p_textures, true);
        }
    }
    else
    {
        qDebug("ERROR: FramebufferManager::bindFBOToRead() called while not configured.");
    }
}

void FramebufferManager::swapBuffers()
{
    if(m_isConfigured)
    {        
        ++m_frameCounter;
    }
    else
    {
        qDebug("ERROR: FramebufferManager::swapBuffers() called while not configured.");
    }
}

void FramebufferManager::initDefaultFBO()
{
    if (m_isConfigured == false)
    {                        
        MathUtil::glFuncs->glActiveTexture(GL_TEXTURE0);

        bool const do_multi_fbos = (m_sampleCount > 0);
        size_t const fbo_count = (do_multi_fbos ? 2 : 1);
        m_FBOs.resize(fbo_count);
        m_textures.resize(fbo_count * FBO_TEXTURE::COUNT);
        m_texture_handles.resize(fbo_count * FBO_TEXTURE::COUNT, nullptr);

        // For each fbo
        for (uint32_t fbo_index = 0; fbo_index < fbo_count; ++fbo_index)
        {
            GLenum const texture_type = (fbo_index == 0) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

            // GL_RGBA8 colour
            GLuint colour_texture = 0;
            MathUtil::glFuncs->glGenTextures(1, &colour_texture);
            m_textures[fbo_index * FBO_TEXTURE::COUNT + FBO_TEXTURE::COLOR] = colour_texture;
            //m_texture_handles[fbo_index * FBO_TEXTURE::COUNT + FBO_TEXTURE::COLOR] = RendererInterface::m_pimpl->creat;
            MathUtil::glFuncs->glBindTexture(texture_type, colour_texture);
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
            if (texture_type == GL_TEXTURE_2D_MULTISAMPLE)
            {
#ifndef __ANDROID__
                MathUtil::glFuncs->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_RGBA8, m_windowWidth, m_windowHeight, GL_TRUE);
#else
                MathUtil::glFuncs->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_RGBA8, m_windowWidth, m_windowHeight, GL_TRUE);
#endif
            }
            else
            {
                MathUtil::glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA8), m_windowWidth,  m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            }
            MathUtil::glFuncs->glBindTexture(texture_type, 0);

            // GL_DEPTH24_STENCIL8 (On most modern cards this is emulated by using 24-bits of a 32-bit int texture for depth and an 8-bit texture for stencil)
            GLuint depth_stencil_texture = 0;
            MathUtil::glFuncs->glGenTextures(1, &depth_stencil_texture);
            m_textures[fbo_index * FBO_TEXTURE::COUNT + FBO_TEXTURE::DEPTH_STENCIL] = depth_stencil_texture;
            MathUtil::glFuncs->glBindTexture(texture_type, depth_stencil_texture);
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
            MathUtil::glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

            if (texture_type == GL_TEXTURE_2D_MULTISAMPLE)
            {
#ifndef __ANDROID__
                MathUtil::glFuncs->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_DEPTH24_STENCIL8, m_windowWidth, m_windowHeight, GL_TRUE);
#else
                MathUtil::glFuncs->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_sampleCount, GL_DEPTH24_STENCIL8, m_windowWidth, m_windowHeight, GL_TRUE);
#endif
            }
            else
            {

                MathUtil::glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_DEPTH24_STENCIL8), m_windowWidth,  m_windowHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
            }
            MathUtil::glFuncs->glBindTexture(texture_type, 0);

            // Attach layers and check completeness
            GLuint FBO;
            MathUtil::glFuncs->glGenFramebuffers(1, &FBO);
            m_FBOs[fbo_index] = FBO;
            MathUtil::glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
            MathUtil::glFuncs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_type, colour_texture, 0);
            MathUtil::glFuncs->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture_type, depth_stencil_texture, 0);
            checkFrameBufferCompleteness(GL_DRAW_FRAMEBUFFER);
            GLenum drawBuffers[1];
            drawBuffers[0] = GL_COLOR_ATTACHMENT0;
            GLsizei drawBufferCount = 1;
            MathUtil::glFuncs->glDrawBuffers(drawBufferCount, &drawBuffers[0]);
        }

        m_isConfigured = true;
    }
    else
    {
        qDebug("ERROR: FramebufferManager::initDefaultFBO() called while already configured.");
    }
}

void FramebufferManager::checkFrameBufferCompleteness(GLenum const p_target)
{
    GLenum error = MathUtil::glFuncs->glCheckFramebufferStatus((GLenum)p_target);
    if (error != GL_FRAMEBUFFER_COMPLETE)
    {
        switch(error)
        {
            case GL_FRAMEBUFFER_UNDEFINED : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_UNDEFINED."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT."); break;
            //case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER."); break;
            //case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER."); break;
            case GL_FRAMEBUFFER_UNSUPPORTED : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_UNSUPPORTED."); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE."); break;
            //case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS."); break;
            case GL_INVALID_ENUM : qDebug("ERROR: Framebuffer incomplete, GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS."); break;
            default : qDebug("ERROR: Framebuffer incomplete, error not recognised: "); qDebug() << error; break;
        }
    }
}
