#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QList>

#include "CodeDock.h"
#include "Whim.h"

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

private slots:
    void modeChanged(Mode oldmode, Mode mode);

private:
    void setupEditor(QString filename, Qt::DockWidgetArea area);
    void setupFileMenu();
    void setupHelpMenu();
    void createStatusBar();
    void openFileInEditor(const QString &path, CodeDock* editor);
    int _id;

    QList<CodeDock*> _docks;
    CodeDock* _activeDock;
    QLabel* _modeWidget;
};

#endif
