#include <QtWidgets>

#include "editor.h"
#include <QDebug>
#include <QPoint>
#include <QRect>


Editor::Editor(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    backgroundColor = QWidget::palette().color(QWidget::backgroundRole());
    foregroundColor = QWidget::palette().color(QWidget::foregroundRole());

    //data = QImage(size(), QImage::Format_ARGB32);
    //data.fill(backgroundColor);

}


void Editor::setImage(const QImage &image)
{
    data = image;
    refreshPixmap();
}


QSize Editor::sizeHint() const
{
    if (data.size().width() < 1 || data.size().height() < 1)
    {
        return QSize(32, 32);
    }
    return QSize(frameSize.width() * fmin(thumbnails.size(), 8),
                 frameSize.height());
}


QSize Editor::minimumSizeHint() const
{
    if (data.size().width() < 1 || data.size().height() < 1)
    {
        return QSize(32,32);
    }
    return frameSize;
}


void Editor::resizeEvent(QResizeEvent * /* event */)
{
    refreshPixmap();
}


void Editor::keyPressEvent(QKeyEvent *event)
{

}


void Editor::wheelEvent(QWheelEvent *event)
{
    int dY = event->pixelDelta().y();   // Scrolling up and down zooms
    int dX = event->pixelDelta().x();   // Scrolling left and right scrolls


    event->ignore();
}


void Editor::setPos(const float &pos)
{
    refreshPixmap();
    update();
}


void Editor::setZoom(const float &zoom)
{
    refreshPixmap();
    update();
}



void Editor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouseLeftPressed = true;
        mouseLeftDownPos = event->pos();

        setCursor(QCursor(Qt::ClosedHandCursor));

    } else if (event->button() == Qt::RightButton) {
        mouseRightPressed = true;
        rubberBandShown = true;
        mouseRightDownPos = event->pos();
    }
    mouseLastPosition = event->pos();
}


void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouseLeftPressed = false;

        setCursor(QCursor(Qt::ArrowCursor));

    } else if (event->button() == Qt::RightButton) {
        mouseRightPressed = false;
        rubberBandShown = false;
        //updateRubberBandRegion();
    }
    mouseLastPosition = event->pos();
}


void Editor::mouseMoveEvent(QMouseEvent *event)
{

    if (mouseLeftPressed) {
        // Perform scrolling/scrubbing
        int dX = event->pos().x() - mouseLastPosition.x();
        //adjustPos(dX);
    }

    if (mouseRightPressed) {
        // Perform rubberband operations
        rubberBandRect = QRect(QPoint(mouseRightDownPos.x(), 0),
                               QPoint(event->pos().x(), frameSize.height()));
        //updateRubberBandRegion();
    }

    mouseLastPosition = event->pos();
}


void Editor::refreshPixmap()
{
    pixmap = QPixmap(size());
    pixmap.fill(backgroundColor);

    QPainter painter(&pixmap);

    drawImage(&painter);

    update();
}


// Conversion from widget space to frame space
QPoint Editor::toImageSpace(const int &x_w, const int &y_w)
{

}


QPoint Editor::toWidgetSpace(const int &x_i, const int &y_i)
{

}


void Editor::drawImage(QPainter *painter)
{

}


void Editor::updateOverlay()
{

}


void Editor::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, pixmap);
}
