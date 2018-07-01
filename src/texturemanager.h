#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#ifdef __ANDROID__
//#define GLdouble float
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#endif

#include <QtOpenGL>
#include <QtCore>
#include <unordered_map>

#include "rendererinterface.h"
#include "domnode.h"

#include "assetimagedata.h"
#include "assetimagedataq.h"

struct TextureElement
{
	TextureElement() :
		tex(nullptr),
		pixelSize(0)
	{
	}

	int index = -1;
    std::shared_ptr<TextureHandle> tex;
	uchar pixelSize;
};

class TextureManager
{
public:
	TextureManager();
    static void CreateTexture(QPointer<AssetImageData> data, QPointer <DOMNode> props);
    static void Clear();

private:
	static std::map<unsigned int, TextureElement> textures;
	static uint uuid;
};

#endif // TEXTUREMANAGER_H
