#include <QtWidgets>

#include "editor.h"
#include <QDebug>
#include <QPoint>
#include <QRect>

#define SCROLL_MULTIPLIER -1.0f

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





void Editor::setPos(QPointF pos)
{
    float x = fminf(fmaxf(pos.x(), 0), data.size().width());
    float y = fminf(fmaxf(pos.y(), 0), data.size().height());

    displayPos = QPointF(x, y);

    update();
}

void Editor::adjustPos(QPointF pos)
{
    setPos(displayPos + pos);
}


void Editor::zoomIn()
{
    // TODO: decide whether to allow user finer control of zooming
    setZoom(zoomLevel * 2);
    /*if (zoomLevel <= 1.0f) {
        setZoom(zoomLevel * 2);
    } else {
        setZoom(zoomLevel + 1);
    }*/
}
void Editor::zoomOut()
{
    // TODO: decide whether to allow user finer control of zooming
    setZoom(zoomLevel / 2);
    /*if (zoomLevel <= 1.0f) {
        setZoom(zoomLevel / 2);
    } else {
        setZoom(zoomLevel - 1);
    }*/
}

void Editor::setZoom(float zoom)
{
    // TODO: consider allowing other zoom levels
    if (zoom < 1.0f) {
        // must convert to a fraction 1/(2^n)
        zoom = 1 / floor(1 / zoom);
    } else {
        zoom = floor(zoom);
    }

    zoom = fmaxf(fminf(zoom, 32.0f), 0.125f);

    if (zoomLevel != zoom) {
        zoomLevel = zoom;


        refreshPixmap();
        update();
    }

}

void Editor::wheelEvent(QWheelEvent *event)
{
    int dY = event->pixelDelta().y();
    int dX = event->pixelDelta().x();

    adjustPos(QPointF(dX / zoomLevel, dY / zoomLevel));

    event->ignore();
}


void Editor::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Minus:
        zoomOut();
        break;

    case Qt::Key_Equal:
        zoomIn();
        break;
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
    return QPoint(((x_w - width() / 2) / zoomLevel + displayPos.x()) * zoomLevel,
                  ((y_w - height() / 2) / zoomLevel + displayPos.y()) * zoomLevel);
}


QPoint Editor::toImageSpace(const QPoint &p)
{
    return toImageSpace(p.x(), p.y());

}


QPoint Editor::toWidgetSpace(const int &x_i, const int &y_i)
{
    return QPoint(((x_i - displayPos.x()) / zoomLevel + width() / 2) / zoomLevel,
                  ((y_i - displayPos.y()) / zoomLevel + height() / 2) / zoomLevel);
}


QPoint Editor::toWidgetSpace(const QPoint &p)
{
    return toWidgetSpace(p.x(), p.y());
}


/**
 * @brief Editor::drawImage Draws the image at the current zoom level onto a
 * pixmap. This double buffering technique allows tool action previews to be
 * shown and the pixmap can be moved around the widget without redrawing
 * @param painter
 */
void Editor::drawImage(QPainter *painter)
{
    int srcIncrement = 1;
    int dstPixelSize = fmax(zoomLevel, 1);
    if (zoomLevel <= 0) {
        qDebug() << "Rendering error: Zoom level = " << zoomLevel;
        return;
    }
    if (zoomLevel < 1) {
        srcIncrement = 1 / zoomLevel;
    }

    int x_p = 0;
    int y_p = 0;

    // TODO: use built in functions to draw upscaled image onto pixmap.
    // TODO: manually draw pixel borders when zoom levels are high
    painter->drawImage(QRect(QPoint(0, 0), zoomLevel * data.size()),
                       data, data.rect());
    /*
    for (int y_src = 0; y_src < data.height(); y_src+=srcIncrement) {
        x_p = 0;
        for (int x_src = 0; x_src < data.width(); x_src+=srcIncrement) {

            //painter->setPen(data.pixelColor(x_src, y_src));

            painter->fillRect(x_p, y_p, dstPixelSize, dstPixelSize,
                              QBrush(data.pixelColor(x_src, y_src)));

            x_p += dstPixelSize;
        }
        y_p += dstPixelSize;
    }*/
}


void Editor::updateOverlay()
{

}


void Editor::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    QPoint p1 = toImageSpace(0, 0);
    QPoint p2 = toImageSpace(width(), height());

    QRect dst(0, 0, width(), height());
    QRect src(QPoint(p1.x(), p1.y()), QSize(p2.x() - p1.x(), p2.y() - p1.y()));

    painter.drawPixmap(dst, pixmap, src);


    painter.setPen(Qt::magenta);
    painter.drawText(16, 16, QString("%1:%2").arg(displayPos.x()).arg(displayPos.y()));
    painter.drawText(16, 32, QString("%1").arg(zoomLevel));
    painter.drawText(16, 64, QString("%1:%2").arg(p1.x()).arg(p1.y()));
    painter.drawText(16, 80, QString("%1:%2").arg(p2.x()).arg(p2.y()));
    painter.drawText(16, 128, QString("%1:%2").arg(pixmap.size().width()).arg(pixmap.size().height()));
}
