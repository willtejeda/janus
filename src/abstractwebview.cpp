#include "abstractwebview.h"

AbstractWebView::AbstractWebView()
{

}

AbstractWebView::~AbstractWebView()
{

}

void AbstractWebView::RepaintRequested(QRect r)
{
    emit repaintRequested(r);
}

void AbstractWebView::ScrollRequested(int dx,int dy, QRect r)
{
    emit scrollRequested(dx, dy, r);
}

void AbstractWebView::LoadStarted()
{
    emit loadStarted();
}

void AbstractWebView::LoadProgress(int i)
{
    emit loadProgress(i);
}

void AbstractWebView::LoadFinished(bool b)
{
    emit loadFinished(b);
}

void AbstractWebView::URLChanged(QUrl u)
{
    emit urlChanged(u);
}

void AbstractWebView::JavaScriptWindowObjectCleared()
{
    emit javaScriptWindowObjectCleared();
}

void AbstractWebView::LoadFrameFinished(bool b)
{
    emit loadFinishedFrame(b);
}
