#ifndef WHIM_H
#define WHIM_H

#include <QtGui>
#include <QApplication>

enum Mode { Normal, Insert };

class Whim : public QApplication
{
    Q_OBJECT

public:
    Whim(int & argc, char ** argv, bool GUIenabled);
    Mode getMode() { return _mode; }
    void changeMode(Mode oldmode, Mode mode);

    static QString ModeToText(Mode mode);

    signals:
        void modeChanged(Mode oldmode, Mode mode);

private:
    int _argc;
    char** argv;
    Mode _mode;
    Mode _oldmode;
};

#endif
