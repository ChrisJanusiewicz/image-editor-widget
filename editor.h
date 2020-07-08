#ifndef EDITOR_H
#define EDITOR_H

#include <QColor>
#include <QImage>
#include <QWidget>
#include <vector>
#include <stack>


/**
 * Custom Widget that displays a timeline composed of thumbnails.
 *
 * @author Chris Janusiewicz
 *
 */
class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(QWidget *parent = 0);

    void setImage(const QImage &image);

    void setZoom(float z);
    void setPos(float p);
    void setOptimalView();

    bool isDefined() { return defined; };

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:

signals:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent* event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:

    QColor backgroundColor;
    QColor foregroundColor;
    QImage data;

    bool mouseLeftPressed, mouseRightPressed;
    QPoint mouseLeftDownPos, mouseRightDownPos;
    QPoint mouseLastPosition;

    QPixmap pixmap;

    QPointF displayPos;  // The pixel on which we are centered
    float zoomLevel;

    bool defined = false;

    QPoint toImageSpace(const int &x_w, const int &y_w);
    QPoint toWidgetSpace(const int &x_i, const int &y_i);

    void refreshPixmap();
    void drawImage(QPainter *painter);

    void updateOverlay();
};

#endif
