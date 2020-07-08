#include <QApplication>

#include "editor.h"

#include <QDebug>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Editor editor;

    QImage img(":/images/earth.png");

    editor.setImage(img);

    editor.show();

    return app.exec();
}
