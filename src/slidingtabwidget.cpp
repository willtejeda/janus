#include "slidingtabwidget.h"

SlidingTabWidget::SlidingTabWidget()
{
    sliding = false;
    dx = 0;
    dy = 0;
    max_dx = INT_MAX;
    min_dx = INT_MIN;
    max_dy = INT_MAX;
    min_dy = INT_MIN;
}

SlidingTabWidget::~SlidingTabWidget()
{

}

void SlidingTabWidget::SetDeltaX(int x)
{
    dx = x;
}

void SlidingTabWidget::SetDeltaY(int y)
{
    dy = y;
}

int SlidingTabWidget::GetDeltaX()
{
    return dx;
}

int SlidingTabWidget::GetDeltaY()
{
    return dy;
}

bool SlidingTabWidget::GetSliding()
{
    return sliding;
}

void SlidingTabWidget::SetMaxDeltaX(int x)
{
    max_dx = x;
}

void SlidingTabWidget::SetMinDeltaX(int x)
{
    min_dx = x;
}

void SlidingTabWidget::SetMaxDeltaY(int y)
{
    max_dy = y;
}

void SlidingTabWidget::SetMinDeltaY(int y)
{
    min_dy = y;
}

void SlidingTabWidget::mousePressEvent(QMouseEvent * event)
{
    //qDebug() << "MOUSEPRESSEDHERE";
    if (this->tabBar()->tabAt(event->pos()) >= 0)
    {
        sliding = true;
        initial_mouse_pos = event->pos();
        last_mouse_pos = event->pos();

        slide_time.restart();
    }
}

void SlidingTabWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (sliding)
    {
        float elapsed = (float) slide_time.elapsed() / 1000.0f;
        if (elapsed < 0.25f && sqrt(pow(event->pos().x() - initial_mouse_pos.x(), 2) + pow(event->pos().y() - initial_mouse_pos.y(), 2)) < 50) {
            if (dx == max_dx) dx = min_dx;
            else if (dx == min_dx) dx = max_dx;
            if (dy == max_dy) dy = min_dy;
            else if (dy == min_dy) dy = max_dy;
        }
        else{
            float slide_speed_x = (float)(event->pos().x() - initial_mouse_pos.x())/elapsed;
            float slide_speed_y = (float)(event->pos().y() - initial_mouse_pos.y())/elapsed;

            if (abs(slide_speed_x) > 100)
            {
                if (slide_speed_x > 0) dx = max_dx;
                else dx = min_dx;
            }

            if (abs(slide_speed_y) > 100)
            {
                if (slide_speed_y > 0) dy = max_dy;
                else dy = min_dy;
            }
        }
    }
    sliding = false;
}
