#include "Whim.h"

Whim::Whim(int & argc, char ** argv, bool GUIenabled) : QApplication(argc,argv, GUIenabled),
    _mode(Normal),
    _oldmode(Normal)
{
    setCursorFlashTime(0);
}

void Whim::changeMode(Mode oldmode, Mode mode)
{
    _oldmode = oldmode;
    _mode = mode;
    emit modeChanged(_oldmode, _mode);
}

QString Whim::ModeToText(Mode mode)
{
    switch(mode) {
        case Normal:
            return "Normal";
            break;
        case Insert:
            return "Insert";
            break;
        default:
            return "Da hell you do?";
            break;
    }
}
