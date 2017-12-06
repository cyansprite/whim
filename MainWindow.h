#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QList>

// #include "CodeEditor.h"
#include "CodeDock.h"

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {
        while (!_docks.isEmpty())
            delete _docks.takeFirst();
    }

public slots:
    void about();
    void save();
    void newFile();
    void openFileDialog();

private:
    void setupEditor(QString filename, Qt::DockWidgetArea area);
    CodeDock* getActiveDock();
    void setupFileMenu();
    void setupHelpMenu();
    void createStatusBar();
    void openFileInEditor(const QString &path, CodeDock* editor);
    int _id;

    QList<CodeDock*> _docks;
    // CodeEditor* _activeEditor;

    QLabel* _modeWidget;
};

#endif
