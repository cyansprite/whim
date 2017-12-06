#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QList>

#include "CodeEditor.h"

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() {
        while (!_editors.isEmpty())
            delete _editors.takeFirst(); 
    }

public slots:
    void about();
    void save();
    void saveas();
    void newFile();
    void openFileDialog();

private:
    CodeEditor* setupEditor(QString filename, Qt::DockWidgetArea area);
    CodeEditor* getActiveEditor();
    void setupFileMenu();
    void setupHelpMenu();
    void createStatusBar();
    void openFileInEditor(const QString &path, CodeEditor* editor);
    int _id;

    QList<CodeEditor*> _editors;
    CodeEditor* _activeEditor;

    QLabel* _modeWidget;
};

#endif
