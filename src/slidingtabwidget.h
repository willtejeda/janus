#ifndef SLIDINGTABWIDGET_H
#define SLIDINGTABWIDGET_H

#include <QtCore>
#include <QtWidgets>

class SlidingTabWidget : public QTabWidget
{
public:
    SlidingTabWidget();
    ~SlidingTabWidget();

    void SetDeltaX(int x);
    void SetDeltaY(int y);
    int GetDeltaX();
    int GetDeltaY();
    bool GetSliding();

    void SetMaxDeltaX(int x);
    void SetMinDeltaX(int x);
    void SetMaxDeltaY(int y);
    void SetMinDeltaY(int y);

    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

private:
    int dx;
    int dy;
    int max_dx;
    int min_dx;
    int max_dy;
    int min_dy;

    bool sliding;

    QTime slide_time;

    QPoint initial_mouse_pos;
    QPoint last_mouse_pos;
};

#endif // SLIDINGTABWIDGET_H
