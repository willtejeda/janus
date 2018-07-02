#ifndef ASSETWINDOW_H
#define ASSETWINDOW_H

#include <QtGui>
#include <QtWidgets>

#include "game.h"

class AssetWindow : public QWidget
{
    Q_OBJECT

public:

    AssetWindow(Game *g);

    void Update();
    void keyReleaseEvent(QKeyEvent * e);

public slots:

    void AddAsset();
    void AddAssetGivenPath(QString path);
    void RemoveAsset();

private:

    QPushButton add_asset_pushbutton;
    QPushButton remove_asset_pushbutton;
    QTableWidget table_widget;
    Game * game;
    QPointer <Room> cur_room;
    bool update;
};

#endif // ASSETWINDOW_H
