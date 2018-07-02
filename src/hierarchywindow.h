#ifndef HIERARCHYWINDOW_H
#define HIERARCHYWINDOW_H

#include <QtGui>
#include <QtWidgets>

#include "game.h"

class HierarchyWindow : public QWidget
{
    Q_OBJECT

public:

    HierarchyWindow(Game * g);

    void Update();
    void keyReleaseEvent(QKeyEvent * e);

public slots:

    void ItemSelectionChanged();
    void RowsInserted(const QModelIndex & parent, int start, int end);

    void CreateObject();
    void DeleteSelected();

private:

    void Update_Helper(QTreeWidgetItem * parent_item, DOMNode * d);    

    QPushButton create_object_pushbutton;
    QPushButton delete_object_pushbutton;
    QTreeWidget tree_widget;
    Game * game;
    QPointer <Room> cur_room;
    int cur_room_objects;
    int cur_room_envobjects;
};

#endif // HIERARCHYWINDOW_H
