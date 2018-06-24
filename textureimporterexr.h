#ifndef TEXTUREIMPORTEREXR_H
#define TEXTUREIMPORTEREXR_H

#include "contentimporter.h"

#include <QPointer>

#include "mem_istream.h"
#include "half.h"
#include "ImfInputFile.h"
#include "ImfChannelList.h"
#include "ImfArray.h"
#include "ImfRgbaFile.h"

#include "assetimagedata.h"

using namespace OPENEXR_IMF_INTERNAL_NAMESPACE; // for OpenEXR support

class TextureImporterEXR : public ContentImporter
{
public:
    TextureImporterEXR();
    ~TextureImporterEXR();

    bool CanImport(const QByteArray& buffer, QString extension, QPointer <DOMNode> props);
    QPointer<BaseAssetData> Import(const QByteArray& buffer, QPointer <DOMNode> props);
};

#endif
