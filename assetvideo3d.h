#ifndef ASSETVIDEO3D_H
#define ASSETVIDEO3D_H

#include <QtConcurrent/QtConcurrent>

#include "asset.h"
#include "assetshader.h"

class AssetVideo3D : public Asset
{
    Q_OBJECT

public:

    AssetVideo3D();

    void Load();

    void Update();
    void DrawGL(QPointer <AssetShader> shader, const bool left_eye);

private:

    void ProcessMVXData();

};

#endif // ASSETVIDEO3D_H
