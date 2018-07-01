#include "performancelogger.h"

QVector <PerformanceLoggerSample> PerformanceLogger::frame_samples;
QOpenGLTexture * PerformanceLogger::tex = NULL;
int PerformanceLogger::max_frame_samples = 180;
clock_t PerformanceLogger::last_clock = 0;

PerformanceLogger::PerformanceLogger() :
      m_total_main_CPU_time(0),
      samples_num(0),
      logging(false),
      m_average_render_GPU_time(0),
      m_average_render_CPU_time(0),
      m_average_main_CPU_time(0)
{    
    frame_samples.resize(max_frame_samples);
    main_thread_cpu_time_samples.resize(max_frame_samples);    
    m_frame_time_timer.start();
    Clear();
}

unsigned int PerformanceLogger::GetNumSamples() const
{
    return samples_num;
}

void PerformanceLogger::SetURL(const QString & s)
{
    data["url"] = s;
    data["hash"] = MathUtil::MD5Hash(s);
}

void PerformanceLogger::Clear()
{
    logging = false;

    data["minftTOTAL"] = FLT_MAX;
    data["maxftTOTAL"] = -FLT_MAX;
    data["minftGPU"] = FLT_MAX;
    data["maxftGPU"] = -FLT_MAX;
    data["medianftTOTAL"] = 0;
    data["medianftGPU"] = 0;

    data["sysmem"] = "0";
    data["processorvendor"] = "unknown";
    data["gpudriverversion"] = "unknown";
    data["version"] = __JANUS_VERSION;       
}

QVariantMap & PerformanceLogger::GetData()
{
    return data;
}

void PerformanceLogger::SubmitData()
{
//    qDebug() << "PerformanceLogger::SubmitData()" << data;
#ifndef QT_DEBUG
    //Doing it
//    qDebug() << "PerformanceLogger::SubmitData() min" << data["minftTOTAL"].toFloat() << "max" << data["maxftTOTAL"].toFloat() << "median" << data["medianftTOTAL"].toFloat();
    //do http post
    QUrl u("http://content.janusvr.com/api/perflog.php");
//    const QByteArray b = "data={\"version\":\"54.0\",\"res\":[1280,1024],\"msaa\":4,\"fov\":\"30\",\"url\":\"http://vesta.janusvr.com/alainchesnais/my-test-room\",\"hash\":\"54bca559366d2b61addd134d6ef70650\",\"minftCPU\":\"0.01\",\"medianftCPU\":\"0.01\",\"maxftCPU\":\"0.01\",\"minftGPU\":\"0.01\",\"medianftGPU\":\"0.01\",\"maxftGPU\":\"0.01\",\"OS\":\"Mac OS 10.12.1\",\"sysmem\":\"8\",\"processorvendor\":\"intel\",\"processordevice\":\"2.9 GHz Intel Core i5\",\"gpuvendor\":\"intel\",\"gpudevice\":\"Intel Iris Graphics 6100 1536 MB\",\"gpudriverversion\":\"10.12.1\",\"rendermode\":\"direct\"}"; //for testing
    webasset.DoHTTPPost(u, QByteArray("data=")+QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact));
#endif
}

void PerformanceLogger::StartFrameSample()
{
    last_clock = clock();
}

void PerformanceLogger::SetLogging(const bool b)
{
    logging = b;
}

void PerformanceLogger::EndFrameSample()
{
    uint32_t const results_index = samples_num % max_frame_samples;
    samples_num++;

    main_thread_cpu_time_samples[results_index] = double(m_frame_time_timer.nsecsElapsed() / 1000000);
    m_frame_time_timer.start();

    m_average_render_GPU_time = 0.0;
    m_average_render_CPU_time = 0.0;
    m_average_main_CPU_time = 0.0;

    for (int sample_index = 0; sample_index < max_frame_samples; ++sample_index)
    {
        m_average_render_GPU_time += static_cast<double>(frame_samples[sample_index].render_thread_gpu_time);
        m_average_render_CPU_time += static_cast<double>(frame_samples[sample_index].render_thread_cpu_time);
        m_average_main_CPU_time += static_cast<double>(main_thread_cpu_time_samples[sample_index]);
    }

    m_average_render_GPU_time /= max_frame_samples;
    m_average_render_CPU_time /= max_frame_samples;
    m_average_main_CPU_time /= max_frame_samples;
}

void PerformanceLogger::SetGPUTimeQueryResults(std::vector<GLuint64> & v)
{
    for (unsigned int i=0; i<(unsigned int)(frame_samples.size()); ++i) {
        if (i < v.size()) {
            frame_samples[i].render_thread_gpu_time = double(v[i] / 1000000.0);
        }
        else {
            frame_samples[i].render_thread_gpu_time = 0.0;
        }
    }
}

void PerformanceLogger::SetCPUTimeQueryResults(std::vector<uint64_t>& v)
{
    for (unsigned int i=0; i<(unsigned int)(frame_samples.size()); ++i) {
        if (i < v.size()) {
            frame_samples[i].render_thread_cpu_time = double(v[i] / 1000000.0);
        }
        else {
            frame_samples[i].render_thread_cpu_time = 0.0;
        }
    }
}

void PerformanceLogger::SetNumFrameSamples(const int i)
{
    max_frame_samples = i;
}

int PerformanceLogger::GetNumFrameSamples() const
{
    return max_frame_samples;
}

TextureHandle* PerformanceLogger::GetFrameSamplesTextureHandle()
{
    const unsigned int perf_height = 44;
    QPixmap pixmap(max_frame_samples, perf_height);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter p(&pixmap);
    std::vector<QRect> render_thread_gpu_time_rects;
    render_thread_gpu_time_rects.reserve(max_frame_samples);
    std::vector<QRect> render_thread_cpu_time_rects;
    render_thread_cpu_time_rects.reserve(max_frame_samples);
    std::vector<QRect> main_thread_cpu_time_rects;
    main_thread_cpu_time_rects.reserve(max_frame_samples);

//    qDebug() << frame_samples;
    for (int i=0; i<frame_samples.size(); ++i) {
        QRect gpu_r(i, 0, 1, frame_samples[i].render_thread_gpu_time);
        QRect cpu_r(i, 0, 1, frame_samples[i].render_thread_cpu_time);
        QRect cpu_m(i, 0, 1, main_thread_cpu_time_samples[i]);

        render_thread_gpu_time_rects.emplace_back(gpu_r);
        render_thread_cpu_time_rects.emplace_back(cpu_r);
        main_thread_cpu_time_rects.emplace_back(cpu_m);
    }

    p.setPen(QPen(QColor(0, 0, 255, 255)));
    p.drawRects(main_thread_cpu_time_rects.data(), max_frame_samples);

    p.setPen(QPen(QColor(255, 255, 255, 255)));
    p.drawRects(render_thread_cpu_time_rects.data(), max_frame_samples);

    p.setPen(QPen(QColor(0, 255, 0, 255)));
    p.drawRects(render_thread_gpu_time_rects.data(), max_frame_samples);

    p.setPen(QPen(QColor(255, 255 ,255, 255)));
    p.drawLine(0, 11, max_frame_samples, 11);

    p.setPen(QPen(QColor(255, 255, 255, 255)));
    p.drawLine(0, 22, max_frame_samples, 22);

    p.end();

    QImage perfTex = pixmap.toImage();
    perfTex = perfTex.convertToFormat(QImage::Format_RGBA8888_Premultiplied);

    if (!m_texture_handle)
    {
        m_texture_handle = RendererInterface::m_pimpl->CreateTextureQImage(perfTex, true, false, true, TextureHandle::ALPHA_TYPE::CUTOUT, TextureHandle::COLOR_SPACE::SRGB);
    }

    RendererInterface::m_pimpl->UpdateTextureHandleData(m_texture_handle.get(), 0, 0, 0, perfTex.width(), perfTex.height(), GL_RGBA, GL_UNSIGNED_BYTE, (void *)perfTex.constBits(), perfTex.width() * perfTex.height() * 4);
    RendererInterface::m_pimpl->GenerateTextureHandleMipMap(m_texture_handle.get());
    return m_texture_handle.get();
}

double PerformanceLogger::GetAverageMainThreadCPUTime() const
{
    return m_average_main_CPU_time;
}

double PerformanceLogger::GetAverageRenderThreadCPUTime() const
{
    return m_average_render_CPU_time;
}

double PerformanceLogger::GetAverageRenderThreadGPUTime() const
{
    return m_average_render_GPU_time;
}

