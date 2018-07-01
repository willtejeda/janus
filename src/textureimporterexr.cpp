#include "textureimporterexr.h"

TextureImporterEXR::TextureImporterEXR()
{
}

TextureImporterEXR::~TextureImporterEXR()
{
}

bool TextureImporterEXR::CanImport(const QByteArray& , QString extension, QPointer <DOMNode> )
{
	return QString::compare(extension, "exr", Qt::CaseInsensitive) == 0;
}

QPointer<BaseAssetData> TextureImporterEXR::Import(const QByteArray & buffer, QPointer <DOMNode> )
{
	unsigned char* da = (unsigned char*)buffer.data();
	int size = buffer.length();
	Mem_IStream stream(da, size);
	RgbaInputFile file(stream);

	Header header = file.header();
	IMATH_NAMESPACE::Box2i dw = header.dataWindow();
	int width = dw.max.x - dw.min.x + 1;
	int height = dw.max.y - dw.min.y + 1;

	int count = width * height;
	Rgba* pixels = new Rgba[count];

	file.setFrameBuffer(pixels - dw.min.x - dw.min.y * width, 1, width);
	file.readPixels(dw.min.y, dw.max.y);

    QByteArray pixelsfdata;
    pixelsfdata.resize(count * 3);
    float* pixelsf = (float *)(pixelsfdata.data());
	for (int j = 0; j < count; j++)
	{
		Rgba pixel = pixels[j];
		int index = j * 3;
		pixelsf[index] = pixel.r;
		pixelsf[index + 1] = pixel.g;
		pixelsf[index + 2] = pixel.b;
	}

    QPointer<AssetImageData> data = new AssetImageData();

	data->SetWidth(width);
	data->SetHeight(height);
	data->SetPixelSize(12);
	data->SetTotalTextures(1);
	data->SetHDR(true);
    data->format = QImage::Format::Format_Invalid;

    data->SetFrameData(0, pixelsfdata, true);

    return static_cast<BaseAssetData *>(data.data());
}
