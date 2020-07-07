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

    void setZoom(const float &z);
    void setPos(const float &z);

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
    QImage data;    // The image we are working on
    //QImage image;   // The image we are drawing on
    QSize frameSize;    // size of each thumbnail must stay the same across gif

    int selectIndex, hoverIndex;
    QRect selectRect, hoverRect;
    bool hoverChanged, selectChanged;  // Must update() if either is true

    bool mouseLeftPressed, mouseRightPressed;
    QPoint mouseLeftDownPos, mouseRightDownPos;
    QPoint mouseLastPosition;

    bool rubberBandShown;
    QRect rubberBandRect;
    QPixmap pixmap;

    // TODO: encapsulate zoom/scroll settings in struct
    float scrollPos, sp_f;
    float zoomFactor, zf_f, zf_i;
    bool halfFrames;

    // TODO: store delays
    std::vector<QImage> thumbnails; // Store thumbnails in the timeline struct

    bool checkSize(const QImage &image);


    QPoint toImageSpace(const int &x_w, const int &y_w);
    QPoint toWidgetSpace(const int &x_i, const int &y_i);

    void refreshPixmap();
    void drawImage(QPainter *painter);

    void updateOverlay();
};

#endif
