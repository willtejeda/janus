#include "renderergl44_viewportinstancing_decoupled.h"

RendererGL44_ViewportInstancing_decoupled::RendererGL44_ViewportInstancing_decoupled()
    :
      m_main_thread_renderer(nullptr),
      m_is_rendering(false),
      m_is_initialised(false)
{

}

RendererGL44_ViewportInstancing_decoupled::~RendererGL44_ViewportInstancing_decoupled()
{

}

void RendererGL44_ViewportInstancing_decoupled::Initialise()
{

}

void RendererGL44_ViewportInstancing_decoupled::InitialiseDefaultShaders(std::shared_ptr<AbstractShaderProgram>* default_object_shader,
    std::shared_ptr<AbstractShaderProgram>* default_skybox_shader,
    std::shared_ptr<AbstractShaderProgram>* default_portal_shader,
    std::unordered_map<GLuint, std::unordered_map<QString, GLint> >* p_map)
{
#ifdef WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
    QThread::currentThread()->setPriority(QThread::TimeCriticalPriority);

    m_gl_context->makeCurrent(m_gl_surface);

#if defined(QT_DEBUG) && !defined(__APPLE__)
    if (m_gl_context->hasExtension(QByteArrayLiteral("GL_ARB_debug_output")))
    {
        PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB = NULL;
        glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)m_gl_context->getProcAddress(QByteArrayLiteral("glDebugMessageCallbackARB"));

        if (glDebugMessageCallbackARB != NULL)
        {
            qDebug() << "DEBUG OUTPUT SUPPORTED";

            glDebugMessageCallbackARB((GLDEBUGPROCARB)&MathUtil::DebugCallbackARB, NULL);
            m_gl_funcs->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        }
        else
        {
            qDebug() << "DEBUG OUTPUT NOT SUPPORTED!";
        }
    }
#endif

    InitialiseGLObjects();
    m_is_initialised = true;
}

void RendererGL44_ViewportInstancing_decoupled::GetProgramID(GLuint * p_id, ProgramHandle * p_handle)
{
    *p_id = 0;
    auto const program_count = m_program_handle_to_GL_ID.size();
    for (auto itr = 0; itr < program_count; ++itr)
    {
        if (m_program_handle_to_GL_ID[itr].first == p_handle && m_program_handle_to_GL_ID[itr].first->m_UUID.m_UUID == p_handle->m_UUID.m_UUID)
        {
            {
                *p_id = m_program_handle_to_GL_ID[itr].second;
                return;
            }
        }
    }
#ifdef QT_DEBUG
    qDebug() << QString("ERROR: called GetProgramHandleID with invalid ProgramHandle") + QString::number((uint64_t)p_handle);
#endif
}

void RendererGL44_ViewportInstancing_decoupled::CompileAndLinkShaderProgram(std::shared_ptr<AbstractShaderProgram> * p_abstract_program, QByteArray * p_vertex_shader,
                                                            QString * p_vertex_shader_path, QByteArray * p_fragment_shader, QString * p_fragment_shader_path,
                                                            std::unordered_map<GLuint, std::unordered_map<QString, GLint> > * p_map)
{
    *p_abstract_program = std::make_shared<AbstractShaderProgram>();
    GLuint program_id;
    std::shared_ptr<ProgramHandle> program_handle = CreateProgramHandle(&program_id);
    (*p_abstract_program)->SetProgram(program_handle);

    GLuint vertex_shader_id = MathUtil::glFuncs->glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = MathUtil::glFuncs->glCreateShader(GL_FRAGMENT_SHADER);
    GLint vertex_compile_result = GL_FALSE;
    GLint fragment_compile_result = GL_FALSE;
    GLint program_link_result = GL_FALSE;

    bool shader_failed = false;
    if (p_vertex_shader->contains("#version 330 core"))
    {
        UpgradeShaderSource(*p_vertex_shader, true);
        GLchar * shader_data = p_vertex_shader->data();
        GLint shader_data_size = p_vertex_shader->size();
        MathUtil::glFuncs->glShaderSource(vertex_shader_id, 1, &shader_data, &shader_data_size);
        MathUtil::glFuncs->glCompileShader(vertex_shader_id);
        MathUtil::glFuncs->glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &vertex_compile_result);

        if (vertex_compile_result == GL_FALSE)
        {
            shader_failed = true;
            int log_length;
            MathUtil::glFuncs->glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> vertex_shader_log((log_length > 1) ? log_length : 1);
            MathUtil::glFuncs->glGetShaderInfoLog(vertex_shader_id, log_length, NULL, &vertex_shader_log[0]);
            //MathUtil::ErrorLog(QString("Compilation of vertex shader \"") + p_vertex_shader_path + QString("\" failed:") + QString("\n") + vertex_shader_log.data());
        }
    }
    else
    {
        QString default_object_vertex_shader_path("assets/shaders/vertex.txt");
        QFile default_object_vertex_shader_file(default_object_vertex_shader_path);
        default_object_vertex_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray default_object_vertex_shader_bytes = default_object_vertex_shader_file.readAll();
        default_object_vertex_shader_file.close();

        UpgradeShaderSource(default_object_vertex_shader_bytes, true);
        GLchar * shader_data = default_object_vertex_shader_bytes.data();
        GLint shader_data_size = default_object_vertex_shader_bytes.size();
        MathUtil::glFuncs->glShaderSource(vertex_shader_id, 1, &shader_data, &shader_data_size);
        MathUtil::glFuncs->glCompileShader(vertex_shader_id);
        MathUtil::glFuncs->glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &vertex_compile_result);

        if (vertex_compile_result == GL_FALSE)
        {
            shader_failed = true;
            int log_length;
            MathUtil::glFuncs->glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> vertex_shader_log((log_length > 1) ? log_length : 1);
            MathUtil::glFuncs->glGetShaderInfoLog(vertex_shader_id, log_length, NULL, &vertex_shader_log[0]);
            //MathUtil::ErrorLog(QString("Compilation of vertex shader \"") + default_object_vertex_shader_path + QString("\" failed:") + QString("\n") + vertex_shader_log.data());
        }
    }

    if (!shader_failed && p_fragment_shader->contains("#version 330 core"))
    {
        UpgradeShaderSource(*p_fragment_shader, false);
        GLchar * shader_data = p_fragment_shader->data();
        GLint shader_data_size = p_fragment_shader->size();
        MathUtil::glFuncs->glShaderSource(fragment_shader_id, 1, &shader_data, &shader_data_size);
        MathUtil::glFuncs->glCompileShader(fragment_shader_id);
        MathUtil::glFuncs->glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &fragment_compile_result);

        if (fragment_compile_result == GL_FALSE)
        {
            shader_failed = true;
            int log_length;
            MathUtil::glFuncs->glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> fragment_shader_lod((log_length > 1) ? log_length : 1);
            MathUtil::glFuncs->glGetShaderInfoLog(fragment_shader_id, log_length, NULL, &fragment_shader_lod[0]);
            //MathUtil::ErrorLog(QString("Compilation of fragment shader \"") + p_fragment_shader_path + QString("\" failed:") + QString("\n") + fragment_shader_lod.data());
        }
    }
    else
    {
        QString default_object_fragment_shader_path("assets/shaders/trans_frag.txt");
        QFile default_object_fragment_shader_file(default_object_fragment_shader_path);
        default_object_fragment_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray default_object_fragment_shader_bytes = default_object_fragment_shader_file.readAll();
        default_object_fragment_shader_file.close();

        UpgradeShaderSource(default_object_fragment_shader_bytes, false);
        GLchar * shader_data = default_object_fragment_shader_bytes.data();
        GLint shader_data_size = default_object_fragment_shader_bytes.size();
        MathUtil::glFuncs->glShaderSource(fragment_shader_id, 1, &shader_data, &shader_data_size);
        MathUtil::glFuncs->glCompileShader(fragment_shader_id);
        MathUtil::glFuncs->glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &fragment_compile_result);

        if (fragment_compile_result == GL_FALSE)
        {
            shader_failed = true;
            int log_length;
            MathUtil::glFuncs->glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> fragment_shader_lod((log_length > 1) ? log_length : 1);
            MathUtil::glFuncs->glGetShaderInfoLog(fragment_shader_id, log_length, NULL, &fragment_shader_lod[0]);
            //MathUtil::ErrorLog(QString("Compilation of fragment shader \"") + default_object_fragment_shader_path + QString("\" failed:") + QString("\n") + fragment_shader_lod.data());
        }
    }

    if (!shader_failed)
    {
        MathUtil::glFuncs->glAttachShader(program_id, vertex_shader_id);
        MathUtil::glFuncs->glAttachShader(program_id, fragment_shader_id);
        MathUtil::glFuncs->glLinkProgram(program_id);
        MathUtil::glFuncs->glGetProgramiv(program_id, GL_LINK_STATUS, &program_link_result);

        if (program_link_result == GL_FALSE)
        {
            int log_length;
            MathUtil::glFuncs->glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> program_log( (log_length > 1) ? log_length : 1 );
            MathUtil::glFuncs->glGetProgramInfoLog(program_id, log_length, NULL, &program_log[0]);

            shader_failed = true;
            //MathUtil::ErrorLog(QString("Linking of shaders \"") + p_vertex_shader_path + QString("\" & \"") + p_fragment_shader_path + QString("\" failed:") + QString("\n") + program_log.data());
        }
    }

    // If we failed just return the default object shader
    if (shader_failed)
    {
        (*p_abstract_program) = m_default_object_shader;
    }
    else
    {
        int log_length;
        MathUtil::glFuncs->glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<char> program_log( (log_length > 1) ? log_length : 1 );
        MathUtil::glFuncs->glGetProgramInfoLog(program_id, log_length, NULL, &program_log[0]);;
        //MathUtil::ErrorLog(QString("Linking of shaders \"") + p_vertex_shader_path + QString("\" & \"") + p_fragment_shader_path + QString("\" successful:") + QString("\n") + program_log.data());

        MathUtil::glFuncs->glUseProgram(program_id);
        CacheUniformLocations(program_id, &m_uniform_locs);
        CacheUniformLocations(program_id, p_map);

        MathUtil::glFuncs->glDeleteShader(vertex_shader_id);
        MathUtil::glFuncs->glDeleteShader(fragment_shader_id);
    }
}

void RendererGL44_ViewportInstancing_decoupled::CreateMeshHandleForGeomVBODataMIRRORCOPY(AbstractRenderer * p_main_thread_renderer, GeomVBOData * p_VBO_data)
{
    // Calling CreateMeshHandleForGeomVBOData here will cause the VAO to be created on the render-thread's GL Context, any attempt to bind a
    // VAO obtained from the MeshHandles stored in GeomVBOData on the main-thread will cause wierd behaviour
    // as those VAO ID are not guaranteed to even exist in the main-thread GL context
    //CreateMeshHandleForGeomVBOData(p_VBO_data);

    VertexAttributeLayout layout;
    layout.attributes[(uint32_t)VAO_ATTRIB::POSITION].in_use = true;
    layout.attributes[(uint32_t)VAO_ATTRIB::POSITION].element_count = 3;
    layout.attributes[(uint32_t)VAO_ATTRIB::POSITION].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::POSITION].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::POSITION].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::NORMAL].in_use = true;
    layout.attributes[(uint32_t)VAO_ATTRIB::NORMAL].element_count = 3;
    layout.attributes[(uint32_t)VAO_ATTRIB::NORMAL].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::NORMAL].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::NORMAL].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD0].in_use = true;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD0].element_count = 2;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD0].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD0].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD0].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD1].in_use = true;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD1].element_count = 2;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD1].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD1].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::TEXCOORD1].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::COLOR].in_use = true;
    layout.attributes[(uint32_t)VAO_ATTRIB::COLOR].element_count = 4;
    layout.attributes[(uint32_t)VAO_ATTRIB::COLOR].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::COLOR].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::COLOR].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMINDICES].in_use = p_VBO_data->use_skelanim ? true : false;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMINDICES].element_count = 4;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMINDICES].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMINDICES].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMINDICES].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMWEIGHTS].in_use = p_VBO_data->use_skelanim ? true : false;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMWEIGHTS].element_count = 4;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMWEIGHTS].element_type = GL_FLOAT;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMWEIGHTS].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::SKELANIMWEIGHTS].offset_in_bytes = 0;

    layout.attributes[(uint32_t)VAO_ATTRIB::INDICES].in_use = true;
    layout.attributes[(uint32_t)VAO_ATTRIB::INDICES].element_count = 1;
    layout.attributes[(uint32_t)VAO_ATTRIB::INDICES].element_type = GL_UNSIGNED_INT;
    layout.attributes[(uint32_t)VAO_ATTRIB::INDICES].stride_in_bytes = 0;
    layout.attributes[(uint32_t)VAO_ATTRIB::INDICES].offset_in_bytes = 0;

    CreateMeshHandle(p_main_thread_renderer, &p_VBO_data->m_mesh_handle,
                     layout);
    p_main_thread_renderer->BindMeshHandle(p_VBO_data->m_mesh_handle.get());

    auto buffer_handles = p_main_thread_renderer->GetBufferHandlesForMeshHandle(p_VBO_data->m_mesh_handle.get());

    p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::INDICES].get());
    MathUtil::glFuncs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_VBO_data->m_indices.size() * sizeof(uint32_t), p_VBO_data->m_indices.data(), GL_STATIC_DRAW);

    p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::POSITION].get());
    MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_positions.size() * sizeof(GLfloat), p_VBO_data->m_positions.data(), GL_STATIC_DRAW);

    p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::NORMAL].get());
    MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_normals.size() * sizeof(GLfloat), p_VBO_data->m_normals.data(), GL_STATIC_DRAW);

    p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::TEXCOORD0].get());
    MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_tex_coords0.size() * sizeof(GLfloat), p_VBO_data->m_tex_coords0.data(), GL_STATIC_DRAW);

    p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::TEXCOORD1].get());
    MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_tex_coords1.size() * sizeof(GLfloat), p_VBO_data->m_tex_coords1.data(), GL_STATIC_DRAW);

    p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::COLOR].get());
    MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_colors.size() * sizeof(GLfloat), p_VBO_data->m_colors.data(), GL_STATIC_DRAW);

    if (p_VBO_data->use_skelanim)
    {
        p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::SKELANIMINDICES].get());
        MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_skel_anim_indices.size() * sizeof(GLfloat), p_VBO_data->m_skel_anim_indices.data(), GL_STATIC_DRAW);

        p_main_thread_renderer->BindBufferHandle((*buffer_handles)[(uint32_t)VAO_ATTRIB::SKELANIMWEIGHTS].get());
        MathUtil::glFuncs->glBufferData(GL_ARRAY_BUFFER, p_VBO_data->m_skel_anim_weights.size() * sizeof(GLfloat), p_VBO_data->m_skel_anim_weights.data(), GL_STATIC_DRAW);
    }
}

void RendererGL44_ViewportInstancing_decoupled::CreateMeshHandle(AbstractRenderer *p_main_thread_renderer, std::shared_ptr<MeshHandle> *p_handle, VertexAttributeLayout p_layout)
{
    // Calling CreateMeshHandle here will cause the VAO to be created on the render-thread's GL Context, any attempt to bind a
    // VAO obtained from the MeshHandles stored in GeomVBOData on the main-thread will cause wierd behaviour
    // as those VAO ID are not guaranteed to even exist in the main-thread GL context
    uint32_t VAO_id = 0;
    MathUtil::glFuncs->glGenVertexArrays(1, &VAO_id);

    *p_handle = p_main_thread_renderer->CreateMeshHandle(p_layout, VAO_id);

}

void RendererGL44_ViewportInstancing_decoupled::Render(std::unordered_map<size_t, std::vector<AbstractRenderCommand>> * p_scoped_render_commands,
                                 std::unordered_map<StencilReferenceValue, LightContainer> * p_scoped_light_containers)
{
    Q_UNUSED(p_scoped_render_commands)
    Q_UNUSED(p_scoped_light_containers)
    //AbstractRenderer::RenderObjectsNaive(p_scope, p_object_render_commands, p_scoped_light_containers);
}

void RendererGL44_ViewportInstancing_decoupled::PreRender(std::unordered_map<size_t, std::vector<AbstractRenderCommand> > * p_scoped_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> * p_scoped_light_containers)
{
    Q_UNUSED(p_scoped_light_containers)
    UpdatePerObjectData(p_scoped_render_commands);
}

void RendererGL44_ViewportInstancing_decoupled::PostRender(std::unordered_map<size_t, std::vector<AbstractRenderCommand> > * p_scoped_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> * p_scoped_light_containers)
{
    Q_UNUSED(p_scoped_render_commands)
    Q_UNUSED(p_scoped_light_containers)
}

void RendererGL44_ViewportInstancing_decoupled::UpgradeShaderSource(QByteArray & p_shader_source, bool p_is_vertex_shader)
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
    p_shader_source.replace("uniform vec4 iBlend;",      "");
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
        p_shader_source.replace("iBlend",                              "per_object_data_array[int(gl_InstanceID + iMiscObjectData[0][0])].iBlend");
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
        p_shader_source.replace("iBlend",                              "per_object_data_array[int(iInstanceID + iMiscObjectData[0][0])].iBlend");
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
                            "    vec4 iBlend;\n"
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

void RendererGL44_ViewportInstancing_decoupled::UpdatePerObjectData(std::unordered_map<size_t, std::vector<AbstractRenderCommand>> * p_scoped_render_commands)
{
    QMatrix4x4 temp_matrix;
    QVector<float> misc_object_data;
    misc_object_data.resize(16);
    QMatrix4x4 model_matrix;
    QMatrix4x4 model_view_matrix;
    uint32_t camera_index = 0;

    m_per_frame_cameras.resize(m_cameras.size());
    m_per_frame_cameras_is_left_eye.resize(m_cameras.size());
    if (m_main_thread_renderer->m_hmd_manager != nullptr && m_main_thread_renderer->m_hmd_manager->GetEnabled() == true)
    {
        QMatrix4x4 const eye_view_matrix_L = m_main_thread_renderer->m_hmd_manager->GetEyeViewMatrix(0);
        QMatrix4x4 const eye_view_matrix_R = m_main_thread_renderer->m_hmd_manager->GetEyeViewMatrix(1);
        for (size_t camera_index = 0; camera_index < m_cameras.size(); ++camera_index)
        {
            QMatrix4x4 composited_view_matrix = m_cameras[camera_index].getViewMatrix();
            bool const isLeftEye = m_cameras[camera_index].getIsLeftEye();
            composited_view_matrix = ((isLeftEye) ? eye_view_matrix_L : eye_view_matrix_R) * composited_view_matrix;
            m_per_frame_cameras[camera_index] = composited_view_matrix;
            m_per_frame_cameras_is_left_eye[camera_index] = isLeftEye;
        }
    }
    else
    {
        for (size_t camera_index = 0; camera_index < m_cameras.size(); ++camera_index)
        {
            m_per_frame_cameras[camera_index] = m_cameras[camera_index].getViewMatrix();
            m_per_frame_cameras_is_left_eye[camera_index] = m_cameras[camera_index].getIsLeftEye();
        }
    }

    for (const RENDERER::RENDER_SCOPE scope : m_scopes)
    {
        std::vector<AbstractRenderCommand> & render_command_vector = (*p_scoped_render_commands)[static_cast<size_t>(scope)];

        auto const command_count(render_command_vector.size());
        auto const camera_count(GetCamerasPerScope(scope));

        if (command_count != 0)
        {
            if (scope == RENDERER::RENDER_SCOPE::CURRENT_ROOM_OBJECTS_TRANSPARENT || scope == RENDERER::RENDER_SCOPE::CHILD_ROOM_OBJECTS_TRANSPARENT)
            {
                std::sort(render_command_vector.begin(), render_command_vector.end(),
                            []
                            (AbstractRenderCommand& a, AbstractRenderCommand& b)
                            {
                              AssetShader_Object& a_object_ref = a.GetObjectUniformsReference();
                              AssetShader_Object& b_object_ref = b.GetObjectUniformsReference();
                              // Sort first by room
                              if (a.GetStencilFunc().GetStencilReferenceValue() != b.GetStencilFunc().GetStencilReferenceValue())
                              {
                                  return (a.GetStencilFunc().GetStencilReferenceValue() < b.GetStencilFunc().GetStencilReferenceValue());
                              }
                              // The by draw layer
                              else if (a_object_ref.m_draw_layer != b_object_ref.m_draw_layer)
                              {
                                  return (a_object_ref.m_draw_layer < b_object_ref.m_draw_layer);
                              }
                              // Then by distance in back-to-front ordering
                              else if (a_object_ref.m_room_space_position_and_distance.w() != b_object_ref.m_room_space_position_and_distance.w())
                              {
                                  return (a_object_ref.m_room_space_position_and_distance.w() > b_object_ref.m_room_space_position_and_distance.w());
                              }
                              // Then by draw_id to group draws of the same object together
                              else  if (a.m_draw_id != b.m_draw_id)
                              {
                                  return (a.m_draw_id < b.m_draw_id);
                              }
                              // Finally by camera_id so that sequential commands match expected camera indices
                              else
                              {
                                  return (a.m_camera_id < b.m_camera_id);
                              }
                            });
            }
        }

        for (uint32_t command_index = 0; command_index < command_count; command_index += camera_count)
        {
            camera_index = 0;
            size_t camera_offset = 0;
            // Recompute matrices for each camera
            for (; camera_index < m_cameras.size(); ++camera_index)
            {
                const VirtualCamera& camera = m_cameras[camera_index];
                // If this camera is active for the current scope compute the new matrices for this frame
                if (camera.getScopeMask(scope) == true)
                {
                    //AbstractRenderCommand new_command(render_command);
                    AbstractRenderCommand & render_command = render_command_vector[command_index + camera_offset];
                    AssetShader_Object & new_object_uniforms = render_command.GetObjectUniformsReference();

                    memcpy((char*)model_matrix.constData(), new_object_uniforms.iModelMatrix, 16 * sizeof(float));
                    model_matrix.optimize(); //56.0 - call optimize so internal type is not identity and inverse does nothing

                    QMatrix4x4 const & view_matrix = m_per_frame_cameras[camera_index];
                    memcpy(new_object_uniforms.iViewMatrix, view_matrix.constData(), 16 * sizeof(float));

                    memcpy(new_object_uniforms.iProjectionMatrix, camera.getProjectionMatrix().constData(), 16 * sizeof(float));

                    temp_matrix = view_matrix.inverted();
                    memcpy(new_object_uniforms.iInverseViewMatrix, temp_matrix.constData(), 16 * sizeof(float));

                    model_view_matrix = view_matrix * model_matrix;
                    memcpy(new_object_uniforms.iModelViewMatrix, model_view_matrix.constData(), 16 * sizeof(float));

                    temp_matrix = camera.getProjectionMatrix() * model_view_matrix;
                    memcpy(new_object_uniforms.iModelViewProjectionMatrix, temp_matrix.constData(), 16 * sizeof(float));

                    temp_matrix = model_matrix.inverted().transposed();
                    memcpy(new_object_uniforms.iTransposeInverseModelMatrix, temp_matrix.constData(), 16 * sizeof(float));

                    temp_matrix = model_view_matrix.inverted().transposed();
                    memcpy(new_object_uniforms.iTransposeInverseModelViewMatrix, temp_matrix.constData(), 16 * sizeof(float));
                    ++camera_offset;
                }
            }
        }
    }
}

void RendererGL44_ViewportInstancing_decoupled::PrintFPS()
{
    if (m_main_thread_renderer != nullptr && m_main_thread_renderer->m_GPUTimeQueryResults.size() != 0)
    {
        double sum_frame_time = 0.0;
        for (GLuint64 frame_time : m_main_thread_renderer->m_GPUTimeQueryResults)
        {
            sum_frame_time += static_cast<double>(frame_time);
        }

        sum_frame_time /= m_main_thread_renderer->m_GPUTimeQueryResults.size();

//        qDebug() << "GPUFPS: " << 1000000000.0 / sum_frame_time;
//        qDebug() << "CPUFPS: " << (1000000000.0 / static_cast<double>(m_frame_time));
    }
}

void RendererGL44_ViewportInstancing_decoupled::InitialiseGLContext(QOpenGLContext * p_gl_context)
{
    qDebug("RendererGL44_ViewportInstancing_decoupled::initialise");

    m_thread = QThread::currentThread();
    m_timer = new QTimer();
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(DecoupledRender()));
    m_timer->start(0);

    m_fps_timer = new QTimer();
    QObject::connect(m_fps_timer, SIGNAL(timeout()), this, SLOT(PrintFPS()));
    m_fps_timer->start(500);

    m_gl_context = p_gl_context;

    m_gl_surface = new QOffscreenSurface();
    auto format = m_gl_context->format();
    m_gl_surface->setFormat(format);
    m_gl_surface->create();

    bool make_current_result = m_gl_context->makeCurrent(m_gl_surface);

    m_gl_funcs = m_gl_context->versionFunctions<QOpenGLFunctions_3_3_Core>();

    // Create FBO to use for attaching main-thread FBO textures to for blitting
    m_main_thread_fbo = 0;
    MathUtil::glFuncs->glGenFramebuffers(1, &m_main_thread_fbo);

    AbstractRenderer::InitialiseState();
}

void RendererGL44_ViewportInstancing_decoupled::Process(AbstractRenderer * p_main_thread_renderer,
                                        std::unordered_map<size_t, std::vector<AbstractRenderCommand> > *p_scoped_object_render_commands,
                                        std::unordered_map<StencilReferenceValue, LightContainer> *p_scoped_light_containers)
{
    m_cameras = p_main_thread_renderer->m_cameras;
    m_cameras_per_scope = p_main_thread_renderer->m_cameras_per_scope;

    m_main_thread_renderer = p_main_thread_renderer;
    m_scoped_object_render_commands = p_scoped_object_render_commands;
    m_scoped_light_containers = *p_scoped_light_containers;

    if (m_is_rendering == false && m_is_initialised == true)
    {
        if (m_main_thread_renderer->m_hmd_manager != nullptr)
        {
            m_main_thread_renderer->m_hmd_manager->InitializeGL();
            m_main_thread_renderer->m_hmd_manager->ReCentre();
        }

        m_is_rendering = true;
    }

    const bool do_VR = (m_main_thread_renderer->m_hmd_manager != nullptr && m_main_thread_renderer->m_hmd_manager->GetEnabled() == true);

    if (do_VR == false && MathUtil::m_frame_limiter_render_thread != 0)
    {
        m_frame_rate_limiter.release(1);
    }
}

void RendererGL44_ViewportInstancing_decoupled::DecoupledRender()
{
    if (m_is_rendering == true && m_is_initialised == true)
    {
#ifdef WIN32
        // RenderDoc Capture code
        RENDERDOC_API_1_0_0* rdoc_api = nullptr;
        if (MathUtil::m_capture_frame == true)
        {
            // RenderDoc Capture code
            if(HMODULE mod = GetModuleHandleA("renderdoc.dll"))
            {
                pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");

                RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_0_0, (void **)&rdoc_api);
            }

            if (rdoc_api != nullptr)
            {
                rdoc_api->StartFrameCapture(NULL, NULL);
            }
        }
#endif
        const bool do_VR = (m_main_thread_renderer->m_hmd_manager != nullptr && m_main_thread_renderer->m_hmd_manager->GetEnabled() == true);

        bool render_limit = m_frame_rate_limiter.tryAcquire((do_VR == false && MathUtil::m_frame_limiter_render_thread != 0) ? 1 : 0);

        if (render_limit == true)
        {

            m_main_thread_renderer->StartFrame();
            auto texture_size = (m_main_thread_renderer->m_hmd_manager != nullptr)
                    ? m_main_thread_renderer->m_hmd_manager->GetTextureSize()
                    : QSize(m_main_thread_renderer->m_window_width / 2, m_main_thread_renderer->m_window_height);


            ConfigureFramebuffer(texture_size.width()*2, texture_size.height(), m_main_thread_renderer->GetMSAACount());
            BindFBOToDraw(FBO_TEXTURE_BITFIELD::COLOR | FBO_TEXTURE_BITFIELD::DEPTH_STENCIL, true);
            UpdateFramebuffer();
            m_main_thread_renderer->WaitforFrameSyncObject();

            // This call allows the PreRender function to get recent pose data for the HMD for this frames render
            // this is important for reducing motion-to-photon latency in VR rendering and to ensure that the timers
            // from the various HMD perf overlays show correct latency values and return us accurate predicted pose values
            if (do_VR)
            {
                m_main_thread_renderer->m_hmd_manager->Update();
            }

            PreRender(m_scoped_object_render_commands, &m_scoped_light_containers);

            if (do_VR)
            {
                m_main_thread_renderer->m_hmd_manager->BeginRendering();
                m_main_thread_renderer->m_hmd_manager->BeginRenderEye(0);
                m_main_thread_renderer->m_hmd_manager->BeginRenderEye(1);
            }

            for (size_t scope = 0; scope < (size_t)RENDERER::RENDER_SCOPE::SCOPE_COUNT; ++scope)
            {
                auto current_scope  = static_cast<RENDERER::RENDER_SCOPE>(scope);
                RenderObjectsNaiveDecoupled(m_main_thread_renderer, current_scope, (*m_scoped_object_render_commands)[scope], (m_scoped_light_containers));
            }

            BlitMultisampledFramebuffer(FBO_TEXTURE_BITFIELD::COLOR);
            MathUtil::glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // TODO:: Make this function create a mip-chain for the attached fbo color texture
            // this combined with a layer for the OVRSDK allows for better filtering
            // it is also useful if we want to create a mip-chain for the depth to use for frame n+1 reprojection or object culling techniques.
            m_main_thread_renderer->PostRender(m_scoped_object_render_commands, &m_scoped_light_containers);

            // Bind our current FBO as read, and the main-thread textures as our draw-FBO
            // This may have issues if those same main-thread textures are bound to a FBO on the main-thread context.

            BindFBOToRead(FBO_TEXTURE_BITFIELD::COLOR, false);
            std::vector<uint32_t> draw_buffers;
            draw_buffers.reserve(FBO_TEXTURE::COUNT);
            m_main_thread_renderer->BindFBOAndTextures(draw_buffers, GL_TEXTURE_2D, GL_DRAW_FRAMEBUFFER, m_main_thread_fbo, 0, FBO_TEXTURE_BITFIELD::COLOR);

            MathUtil::glFuncs->glBlitFramebuffer(0,0, m_window_width, m_window_height,
                                                 0, 0, m_main_thread_renderer->m_window_width, m_main_thread_renderer->m_window_height,
                                                 GL_COLOR_BUFFER_BIT, GL_LINEAR);

            if (do_VR)
            {
                // OpenVR binds textures inside of its submission step.
                // I change the active slot here to one we do not use for normal
                // rendering to avoid the call invalidating our GL state copy.
                //
                // OpenVR needs the OpenGL handle for the texture, so we update it here.
                if (m_main_thread_renderer->m_hmd_manager->m_using_openVR)
                {
                    MathUtil::glFuncs->glActiveTexture(GL_TEXTURE15);
                    m_active_texture_slot = 15;
                    m_main_thread_renderer->m_hmd_manager->m_color_texture_id =  GetTextureID(FBO_TEXTURE::COLOR, false);
                }

                m_main_thread_renderer->m_hmd_manager->EndRenderEye(0);
                m_main_thread_renderer->m_hmd_manager->EndRenderEye(1);
                m_main_thread_renderer->m_hmd_manager->EndRendering();
                MathUtil::glFuncs->glFlush();
            }

            m_main_thread_renderer->LockFrameSyncObject();
            m_main_thread_renderer->EndFrame();

    #ifdef WIN32
            if (MathUtil::m_capture_frame == true)
            {
                if (rdoc_api != nullptr)
                {
                    rdoc_api->EndFrameCapture(NULL, NULL);
                }
                MathUtil::m_capture_frame = false;
            }
    #endif
        }
    }
}

void RendererGL44_ViewportInstancing_decoupled::FinishThread()
{
    emit finished();
}

void RendererGL44_ViewportInstancing_decoupled::InitialiseGLObjectsMIRROR(AbstractRenderer * p_renderer)
{
    p_renderer->InitialiseGLObjects2();
}

void RendererGL44_ViewportInstancing_decoupled::Render(AbstractRenderer * p_main_thread_renderer, const RENDERER::RENDER_SCOPE p_scope, std::vector<AbstractRenderCommand> const * const p_object_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> const * const p_scoped_light_containers)
{

}
