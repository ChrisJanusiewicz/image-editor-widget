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

    zoomLevel = 1.0f;
    //data = QImage(size(), QImage::Format_ARGB32);
    //data.fill(backgroundColor);

}

void Editor::setOptimalView()
{
    // attempt to show the whole image and center it
    displayPos = QPoint(data.size().width() * 0.5f,
                        data.size().height() * 0.5f);
}

void Editor::setImage(const QImage &image)
{
    if (image.size().width() < 1 || image.size().height() < 1) {
        return;
    }
    defined = true;
    data = image;

    setOptimalView();

    refreshPixmap();
}


QSize Editor::sizeHint() const
{
    if (data.size().width() < 1 || data.size().height() < 1)
    {
        return QSize(32, 32);
    }
    return QSize(data.width(),  data.height());
}


QSize Editor::minimumSizeHint() const
{
    if (data.size().width() < 1 || data.size().height() < 1)
    {
        return QSize(32,32);
    }
    return data.size();
}


void Editor::resizeEvent(QResizeEvent * /* event */)
{
    update();
}


void Editor::keyPressEvent(QKeyEvent *event)
{

}


void Editor::wheelEvent(QWheelEvent *event)
{
    int dY = event->pixelDelta().y();
    int dX = event->pixelDelta().x();

    displayPos += QPointF(dX / zoomLevel, dY / zoomLevel);
    update();


    event->ignore();
}


void Editor::setPos(float pos)
{
    refreshPixmap();
    update();
}


void Editor::setZoom(float zoom)
{
    zoom = fmaxf(fminf(zoom, 8.0f), 0.25f);
    if (zoomLevel != zoom) {

        refreshPixmap();
        update();
    }
}



void Editor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouseLeftPressed = true;
        mouseLeftDownPos = event->pos();

        setCursor(QCursor(Qt::ClosedHandCursor));

    } else if (event->button() == Qt::RightButton) {
        mouseRightPressed = true;
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
        //updateRubberBandRegion();
    }
    mouseLastPosition = event->pos();
}


void Editor::mouseMoveEvent(QMouseEvent *event)
{

    if (mouseLeftPressed) {
        // Perform scrolling
        int dX = event->pos().x() - mouseLastPosition.x();
        int dY = event->pos().y() - mouseLastPosition.y();
        displayPos += QPointF(dX / zoomLevel, dY / zoomLevel);

        update();
    }

    if (mouseRightPressed) {
        // Perform rubberband operations
    }

    mouseLastPosition = event->pos();
}


void Editor::refreshPixmap()
{
    if (!defined) {
        pixmap = QPixmap(QSize(32,32));
        pixmap.fill(backgroundColor);
        return;
    }

    pixmap = QPixmap(data.size() * zoomLevel);
    pixmap.fill(Qt::black);

    QPainter painter(&pixmap);

    drawImage(&painter);

    update();
}


// Conversion from widget space to frame space
QPoint Editor::toImageSpace(const int &x_w, const int &y_w)
{
    return QPoint((x_w - width() / 2) * zoomLevel + displayPos.x(),
                  (y_w - height() / 2) * zoomLevel + displayPos.y());
}


QPoint Editor::toWidgetSpace(const int &x_i, const int &y_i)
{
    return QPoint((x_i - displayPos.x()) * zoomLevel + width() / 2,
                  (y_i - displayPos.y()) * zoomLevel + height() / 2);
}


void Editor::drawImage(QPainter *painter)
{
    int srcIncrement = 1;
    int dstPixelSize = fmax(zoomLevel, 1);
    if (zoomLevel <= 0) {
        qDebug() << "Rendering error: Zoom level = " << zoomLevel;
        return;
        srcIncrement = 1 / zoomLevel;
    }

    int x_p = 0;
    int y_p = 0;

    for (int y_src = 0; y_src < data.height(); y_src+=srcIncrement) {
        x_p = 0;
        for (int x_src = 0; x_src < data.width(); x_src+=srcIncrement) {

            painter->setPen(data.pixelColor(x_src, y_src));

            painter->drawRect(x_p, y_p, dstPixelSize, dstPixelSize);

            x_p += dstPixelSize;
        }
        y_p += dstPixelSize;
    }
}


void Editor::updateOverlay()
{

}


void Editor::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    displayPos;


    painter.drawPixmap(displayPos.x(), displayPos.y(), pixmap);
}
