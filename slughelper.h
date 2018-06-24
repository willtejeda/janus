#ifndef SLUGHELPER_H
#define SLUGHELPER_H

#include <stdint.h>
#include <QFile>
#include "Slug.h"
#include "rendererinterface.h"
#include "assetimage.h"

class Font
{
private:

    QByteArray m_file_data;
    Slug::FontHeader const * m_font_header;
    int32_t referenceCount;
    std::shared_ptr<TextureHandle> m_curve_texture_handle;
    std::shared_ptr<TextureHandle> m_band_texture_handle;
    ~Font();

public:

    Font(QString fileName);

    const Slug::FontHeader* GetFontHeader() const
    {
        return (Slug::GetFontHeader(m_file_data.constData()));
    }
};

class Text
{
    private:

        Font *              textFont;				// The font used by this text.
        Slug::LayoutData	textLayoutData;			// A copy of the layout data passed into the constructor.

        int32				elementCount[2];		// The number of vertex indices passed to the draw function.
        unsigned_machine	effectOffset;			// The offset at which indices for the effect layer start.

    public:

        Text(Font* font, const Slug::LayoutData* layoutData, const char* text, float x, float y);
        ~Text();

        void Build(QString text);
};

class SlugHelper
{
public:
    SlugHelper();
};

#endif // SLUGHELPER_H
