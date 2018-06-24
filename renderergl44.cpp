#include "renderergl44.h"

#if !defined(__APPLE__) && !defined(__ANDROID__)
RendererGL44::RendererGL44()
{

}

RendererGL44::~RendererGL44()
{

}

void RendererGL44::Initialize()
{    
    m_name = QString("OpenGL 4.4");
    /*m_perObjectBuffer.Initialize((void*)m_per_object_data.data(), m_per_object_data.size() * sizeof(PerObjectSSBOData), GL_SHADER_STORAGE_BUFFER);
    m_perMaterialBuffer.Initialize((void*)m_per_material_data.data(), m_per_material_data.size() * sizeof(PerMaterialSSBOData), GL_SHADER_STORAGE_BUFFER);
    m_perInstanceBuffer.Initialize((void*)m_per_instance_data.data(), m_per_instance_data.size() * sizeof(PerInstanceSSBOData), GL_SHADER_STORAGE_BUFFER);
    m_perViewportBuffer.Initialize((void*)m_per_viewport_data.data(), m_per_viewport_data.size() * sizeof(PerCameraSSBOData), GL_SHADER_STORAGE_BUFFER);*/
    // Create Default GL 4.4 (GLSL 440) Shaders

    // Object Shader
    QString default_object_vertex_shader_path = MathUtil::GetApplicationPath() + "assets/shaders/vertex.txt";
    QFile default_object_vertex_shader_file(default_object_vertex_shader_path);
    default_object_vertex_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray * default_object_vertex_shader_bytes = new QByteArray(default_object_vertex_shader_file.readAll());
    default_object_vertex_shader_file.close();

    QString default_object_fragment_shader_path = MathUtil::GetApplicationPath() + "assets/shaders/trans_frag.txt";
    QFile default_object_fragment_shader_file(default_object_fragment_shader_path);
    default_object_fragment_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray * default_object_fragment_shader_bytes = new QByteArray(default_object_fragment_shader_file.readAll());
    default_object_fragment_shader_file.close();

    m_default_object_shader = CompileAndLinkShaderProgram(default_object_vertex_shader_bytes, default_object_vertex_shader_path,
                                default_object_fragment_shader_bytes, default_object_fragment_shader_path);
    // Skybox Shader
    QString default_cubemap_vertex_shader_path = MathUtil::GetApplicationPath() + "assets/shaders/cubemap_vert.txt";
    QFile default_cubemap_vertex_shader_file(default_cubemap_vertex_shader_path);
    default_cubemap_vertex_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray * default_cubemap_vertex_shader_bytes = new QByteArray(default_cubemap_vertex_shader_file.readAll());
    default_cubemap_vertex_shader_file.close();

    QString default_cubemap_fragment_shader_path = MathUtil::GetApplicationPath() + "assets/shaders/cubemap_frag.txt";
    QFile default_cubemap_fragment_shader_file(default_cubemap_fragment_shader_path);
    default_cubemap_fragment_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray * default_cubemap_fragment_shader_bytes = new QByteArray(default_cubemap_fragment_shader_file.readAll());
    default_cubemap_fragment_shader_file.close();

    m_default_skybox_shader = CompileAndLinkShaderProgram(default_cubemap_vertex_shader_bytes, default_cubemap_vertex_shader_path,
                                default_cubemap_fragment_shader_bytes, default_cubemap_fragment_shader_path);
    // Portal Shader
    QString default_portal_vertex_shader_path = MathUtil::GetApplicationPath() + "assets/shaders/vertex.txt";
    QFile default_portal_vertex_shader_file(default_portal_vertex_shader_path);
    default_portal_vertex_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray * default_portal_vertex_shader_bytes = new QByteArray(default_portal_vertex_shader_file.readAll());
    default_portal_vertex_shader_file.close();

    QString default_portal_fragment_shader_path = MathUtil::GetApplicationPath() + "assets/shaders/portal_frag.txt";
    QFile default_portal_fragment_shader_file(default_portal_fragment_shader_path);
    default_portal_fragment_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray * default_portal_fragment_shader_bytes = new QByteArray(default_portal_fragment_shader_file.readAll());
    default_portal_fragment_shader_file.close();

    m_default_portal_shader = CompileAndLinkShaderProgram(default_portal_vertex_shader_bytes, default_portal_vertex_shader_path,
                                default_portal_fragment_shader_bytes, default_portal_fragment_shader_path);
}

void RendererGL44::Render(RENDERER::RENDER_SCOPE const p_scope, std::vector<AbstractRenderCommand> const & p_object_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> const & p_scoped_light_containers)
{
    AbstractRenderer::RenderObjectsNaive(p_scope, p_object_render_commands, p_scoped_light_containers);
}

void RendererGL44::PreRender(std::unordered_map<size_t, std::vector<AbstractRenderCommand> > & p_scoped_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> & p_scoped_light_containers)
{
    Q_UNUSED(p_scoped_light_containers);
    UpdatePerObjectSSBO(p_scoped_render_commands, true);
}

void RendererGL44::PostRender(std::unordered_map<size_t, std::vector<AbstractRenderCommand> > & p_scoped_render_commands, std::unordered_map<StencilReferenceValue, LightContainer> & p_scoped_light_containers)
{
    Q_UNUSED(p_scoped_render_commands)
    Q_UNUSED(p_scoped_light_containers)
}

void RendererGL44::UpgradeShaderSource(QByteArray & p_shader_source, bool p_is_vertex_shader)
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
                                "void main"
                                );

        // Add in new vertex shader viewport and instance ID code
        p_shader_source.replace("gl_Position",
                                "iInstanceID = gl_InstanceID;\n"
                                "gl_Position"
                                );

        p_shader_source.replace("iObjectPickID",                        "iInstanceID"); //56.0 - if iObjectPickID was used in code, just replace with iInstanceID

        // Replace object uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iConstColour",                        "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iConstColour");
        p_shader_source.replace("iChromaKeyColour",                    "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iChromaKeyColour");
        p_shader_source.replace("iUseLighting",                        "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iUseLighting");
        p_shader_source.replace("iModelMatrix",                        "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iModelMatrix");
        p_shader_source.replace("iViewMatrix",                         "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iViewMatrix");
        p_shader_source.replace("iProjectionMatrix",                   "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iProjectionMatrix");
        p_shader_source.replace("iInverseViewMatrix",                  "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iInverseViewMatrix");
        p_shader_source.replace("iModelViewMatrix",                    "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iModelViewMatrix");
        p_shader_source.replace("iModelViewProjectionMatrix",          "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iModelViewProjectionMatrix");
        p_shader_source.replace("iTransposeInverseModelMatrix",        "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iTransposeInverseModelMatrix");
        p_shader_source.replace("iTransposeInverseModelViewMatrix",    "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iTransposeInverseModelViewMatrix");
        p_shader_source.replace("iUseSkelAnim",                        "per_object_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iUseSkelAnim");

        // Replace material uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iAmbient",        "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iAmbient");
        p_shader_source.replace("iDiffuse",        "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iDiffuse");
        p_shader_source.replace("iSpecular",       "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iSpecular");
        p_shader_source.replace("iShininess",      "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iShininess");
        p_shader_source.replace("iEmission",       "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iEmission");
        p_shader_source.replace("iLightmapScale",  "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iLightmapScale");
        p_shader_source.replace("iTiling",         "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iTiling");
        p_shader_source.replace("iUseTexture",     "per_instance_material_data_array[int(gl_InstanceID * iViewportCount.y + iMiscObjectData[0][0])].iUseTexture");
    }
    else
    {
        // Add in new fragment shader inputs
        p_shader_source.replace("void main",
                                "flat in int iInstanceID;\n"
                                "void main"
                                );

        p_shader_source.replace("iObjectPickID",                        "iInstanceID"); //56.0 - if iObjectPickID was used in code, just replace with iInstanceID

		// Add gamma correction step as we output to a linear texture
        prependDataInShaderMainFunction(p_shader_source, g_gamma_correction_GLSL);

        // Replace object uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iConstColour",                        "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iConstColour");
        p_shader_source.replace("iChromaKeyColour",                    "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iChromaKeyColour");
        p_shader_source.replace("iUseLighting",                        "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iUseLighting");
        p_shader_source.replace("iModelMatrix",                        "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iModelMatrix");
        p_shader_source.replace("iViewMatrix",                         "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iViewMatrix");
        p_shader_source.replace("iProjectionMatrix",                   "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iProjectionMatrix");
        p_shader_source.replace("iInverseViewMatrix",                  "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iInverseViewMatrix");
        p_shader_source.replace("iModelViewMatrix",                    "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iModelViewMatrix");
        p_shader_source.replace("iModelViewProjectionMatrix",          "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iModelViewProjectionMatrix");
        p_shader_source.replace("iTransposeInverseModelMatrix",        "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iTransposeInverseModelMatrix");
        p_shader_source.replace("iTransposeInverseModelViewMatrix",    "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iTransposeInverseModelViewMatrix");
        p_shader_source.replace("iUseSkelAnim",                        "per_object_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iUseSkelAnim");


        // Replace material uniform reads with reads from the Object Struct packed into the SSBO
        p_shader_source.replace("iAmbient",        "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iAmbient");
        p_shader_source.replace("iDiffuse",        "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iDiffuse");
        p_shader_source.replace("iSpecular",       "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iSpecular");
        p_shader_source.replace("iShininess",      "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iShininess");
        p_shader_source.replace("iEmission",       "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iEmission");
        p_shader_source.replace("iLightmapScale",  "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iLightmapScale");
        p_shader_source.replace("iTiling",         "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iTiling");
        p_shader_source.replace("iUseTexture",     "per_instance_material_data_array[int(iInstanceID * iViewportCount.y + iMiscObjectData[0][0])].iUseTexture");
    }

    // Replace GLSL 330 define with GLSL 440 define, extensions, and SSBO info
    p_shader_source.replace("#version 330 core",
                            "#version 440 core\n"
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

void RendererGL44::UpdatePerObjectSSBO(std::unordered_map<size_t, std::vector<AbstractRenderCommand>> & , bool const  /* = false */)
{
    /*QMatrix4x4 temp_matrix;
    float base_SSBO_offset = 0.0f;
    float previous_base_SSBO_offset = FLT_MAX;
    QVector<float> misc_object_data;
    misc_object_data.resize(16);
    AbstractRenderCommand * previous_unique_command = nullptr;
    size_t previous_unique_command_index = 0;
    QMatrix4x4 model_matrix;
    QMatrix4x4 model_view_matrix;
    uint32_t camera_index = 0;

    for (const RENDERER::RENDER_SCOPE scope : m_scopes)
    {
        std::vector<AbstractRenderCommand>& render_command_vector = p_scoped_render_commands[(size_t)scope];

        // Objects are sorted by front-to-back distance from the player, we want the opposite of this for
        // any transparent scopes so they blend correctly.
        auto const size = render_command_vector.size();
        if (size != 0)
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

        uint32_t const camera_count = GetCamerasPerScope(scope);
        size_t const command_count = render_command_vector.size();

        previous_unique_command = nullptr;

		for (size_t command_index = 0; command_index < command_count; command_index += camera_count)
        {
            camera_index = 0;
            AbstractRenderCommand & base_render_command = render_command_vector[command_index];
            AssetShader_Material const * new_material_uniforms = base_render_command.GetMaterialUniformsPointer();

            // Update misc data, currently using this for objectID instanceID is added to this to fetch right eye matrices
            if (p_using_instancing
                && previous_unique_command != nullptr
                && previous_unique_command->IsInstancableWith(base_render_command)                    
               )
            {
                // We found a batchable command so use the same draw ID as the previous
                // and increment the instance count of the first member of the batch
                misc_object_data[0] = previous_base_SSBO_offset;

                for (uint32_t camera_index = 0; camera_index < camera_count; camera_index++)
                {
                    render_command_vector[previous_unique_command_index + camera_index].IncrementInstanceCount();
                }
            }
            else
            {
                // We didn't find a batchable command or we're the first command so store this command as the unique_command
                misc_object_data[0] = base_SSBO_offset;
                previous_base_SSBO_offset = base_SSBO_offset;
                previous_unique_command = &base_render_command;
                previous_unique_command_index = command_index;
            }

            auto cache = misc_object_data[0];
            // Recompute matrices for each camera
            for (const VirtualCamera& camera : m_cameras)
            {
                // If this camera is active for the current scope compute the new matrices for this frame
                if (camera.getScopeMask(scope) == true)
                {
                    AbstractRenderCommand & render_command = render_command_vector[command_index + camera_index];

                    AssetShader_Object & new_object_uniforms = render_command.GetObjectUniformsReference();

                    // This contains the base offset which the instanceID is added to to get this instance's
                    // TODO: Ideally iMiscObjectData could also store the offset into the material SSBO to avoid needing to
                    // give each object it's own copy to match the object uniform offset.
                    memcpy(new_object_uniforms.iMiscObjectData, (char*)misc_object_data.constData(), 16 * sizeof(float));
                    misc_object_data[0] += 1.0;

                    // This is already valid from when the command was pushed
                    memcpy((char*)model_matrix.constData(), new_object_uniforms.iModelMatrix, 16 * sizeof(float));
                    model_matrix.optimize(); //56.0 - call optimize so internal type is not identity and inverse does nothing

                    QMatrix4x4 const & view_matrix = camera.getViewMatrix();
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

                    m_new_per_object_data[base_SSBO_offset] = AssetShader_Object_Compact(new_object_uniforms);
                    m_new_per_material_data[base_SSBO_offset] = *new_material_uniforms;
                    ++base_SSBO_offset;
                    ++camera_index;
                }
            }
            misc_object_data[0] = cache;
        }
    }

    // Push the vectors with both eye's data into the SSBOs for use this frame
    m_perObjectBuffer.updateData((float*)m_new_per_object_data.data(), base_SSBO_offset * sizeof(AssetShader_Object_Compact));
    m_perMaterialBuffer.updateData((float*)m_new_per_material_data.data(), base_SSBO_offset * sizeof(AssetShader_Material));*/
}
#endif
