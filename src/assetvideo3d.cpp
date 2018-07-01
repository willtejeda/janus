#include "assetvideo3d.h"

AssetVideo3D::AssetVideo3D()
{
    SetS("_type", "assetvideo3d");
}

void AssetVideo3D::Load()
{
    WebAsset::Load(QUrl(GetS("_src_url")));
}

void AssetVideo3D::Update()
{
//    qDebug() << "AssetVideo3D::Update()" << this->GetProperty("src");
    if (GetLoaded() && !GetProcessing()) {
        SetProcessing(true);
        if (!GetError()) {
            QtConcurrent::run(this, &AssetVideo3D::ProcessMVXData); //49.25 - do image loading from a non-UI thread
        }
        else {
            SetProcessed(true);
        }
    }
}

void AssetVideo3D::DrawGL(QPointer <AssetShader> , const bool )
{

}

void AssetVideo3D::ProcessMVXData()
{
//    qDebug() << "AssetVideo3D::ProcessMVXData()" << webasset.GetError() << webasset.GetErrorString() << webasset.GetStatusCode();
    //first 11 bytes "magic": .MVX_Mantis
    QByteArray ba = GetData();
    QDataStream data(&ba, QIODevice::ReadOnly);

//    char s[12];
//    data.readRawData(s, 1);
//    data.readRawData(s, 11);
//    s[11] = '\0';
//    qDebug() << "AssetVideo3D::ProcessMVXData() - magic string:" << s;

//    unsigned int num_val = 0;
//    data.readRawData(s, 4);
//    memcpy(&num_val, s, 4);
//    qDebug() << "AssetVideo3D::ProcessMVXData() - uint32:" << num_val;

//    data.readRawData(s, 4);
//    memcpy(&num, s, 4);
//    qDebug() << "AssetVideo3D::ProcessMVXData() - uint32:" << num;


//    unsigned int num_size;
//    data.readRawData(s, 4);
//    memcpy(&num_size, s, 4);
//    qDebug() << "AssetVideo3D::ProcessMVXData() - num_size:" << num_size;

    //next 4 bytes are 0'ed out
//    data.readRawData(s, 4);

//    unsigned int num[4];
//    for (unsigned int i=0; i<100; ++i) {
//        for (int j=0; j<4; ++j) {
//            data.readRawData(s, 4);
//            memcpy(&num[j], s, 4);
//        }
//        qDebug() << "AssetVideo3D::ProcessMVXData() - uint32s" << num[0] << num[1] << num[2] << num[3];
//    }


    //offset 16777380 some new format starts

    SetProcessed(true);
}
