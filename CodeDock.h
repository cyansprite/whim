#ifndef CODEDOCK_H
#define CODEDOCK_H

#include <QPlainTextEdit>
#include <QObject>
#include <QDockWidget>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include "CodeEditor.h"

class CodeDock : public QDockWidget
{
    Q_OBJECT

    public:
        CodeDock(QWidget* parent = 0);
        bool openNew(QString fileText, QString path);
        bool save();

        CodeEditor* getEditor() { return _editor; }
        int getActive(){ return _active; }

        static int ActiveCount;

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        bool areYouSure();
        CodeEditor* _editor;
        int _active;
};

#endif
