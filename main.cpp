#include <QApplication>

#include "editor.h"

#include <QDebug>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Editor editor;

    editor.show();


    QImage img(":images/earth.png");

    editor.setImage(img);


    return app.exec();
}
