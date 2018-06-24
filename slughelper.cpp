#include "slughelper.h"

SlugHelper::SlugHelper()
{

}

Font::Font(QString fileName)
{
    // Open .slug font file
    QFile font_file(fileName);
    font_file.open(QIODevice::ReadOnly);
    m_file_data = font_file.readAll();
    font_file.close();

    // Extract font header from font file file
    m_font_header = Slug::GetFontHeader(m_file_data.constData());

    // Allocate data to hold the texture format and data from font header
    int32_t curveTextureWidth = m_font_header->curveTextureSize[0];
    int32_t curveTextureHeight = m_font_header->curveTextureSize[1];
    Slug::FontTexel* curveTextureData = new Slug::FontTexel[curveTextureWidth * curveTextureHeight];

    int32_t bandTextureWidth = m_font_header->bandTextureSize[0];
    int32_t bandTextureHeight = m_font_header->bandTextureSize[1];
    Slug::FontTexel* bandTextureData = new Slug::FontTexel[bandTextureWidth * bandTextureHeight];

    // Extract texture data from font file
    Slug::ExtractFontTextures(m_font_header, curveTextureData, bandTextureData);

    std::vector<std::shared_ptr<TextureHandle>> handles = RendererInterface::m_pimpl->CreateSlugTextureHandles(curveTextureWidth, curveTextureHeight, curveTextureData,
                                                                                          bandTextureWidth, bandTextureHeight, bandTextureData);
    m_curve_texture_handle = handles[0];
    m_band_texture_handle = handles[1];

    delete[] bandTextureData;
    delete[] curveTextureData;
}
