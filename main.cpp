#include <QtGui>
#include "MainWindow.h"
// #include <iostream>

int main(int argv, char **args)
{
    QApplication app(argv, args);
    QFile file("/home/joj/.whim.qss");
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        // std::cout << "File has been read" << std::endl;
        app.setStyleSheet(file.readAll());
    } else {
        // std::cout << "File error" << std::endl;
    }

    // FIXME placeholder :: window size
    MainWindow window;
    window.resize(640, 512);
    window.show();

    return app.exec();
}
