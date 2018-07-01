#ifndef ABSTRACTTEXTURE_H
#define ABSTRACTTEXTURE_H

#include <QtCore>

class AbstractTexture
{
protected:
	bool isHdr;
	int textures;

	int frameLoop;
	QVector<int> delays;

public:
	AbstractTexture();

	int GetFrameLoop() { return frameLoop; }
	void SetFrameLoop(int value) { frameLoop = value; }

	QVector<int> GetImageDelays() { return delays; }
	void SetImageDelays(QVector<int> value) { delays = value; }

	bool IsAnimated() { return delays.size() > 1; }

	bool IsHDR() { return isHdr; }
	void IsHDR(bool value) { isHdr = value; }

	virtual void Release();
	int GetTextureCount();

	// This method is deprecated from the start, it's supposed to exist
	// for quick re-implementation of the original OpenGL stuff
	// but marks where we need to fix abstraction
	virtual int* GetIdentifier();

};

#endif // ABSTRACTTEXTURE_H