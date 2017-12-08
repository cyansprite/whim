#include <QtGui>
#include "MainWindow.h"
#include "Whim.h"

int main(int argv, char **args)
{
    Whim app(argv, args, true);

    QFile file("/home/joj/.whim.qss");
    if (file.open(QFile::ReadWrite | QFile::Text))
        app.setStyleSheet(file.readAll());

    // FIXME placeholder :: window size
    MainWindow window;
    window.resize(640, 512);
    window.show();

    return app.exec();
}
