#include <QtGui>
#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _id = -1;
    // Window handles TODO CONFIGCONFIG
    // TODO Make editor inside of QDockWidget? sounds like a nice feat.
    setupFileMenu();
    setupHelpMenu();
    setupEditor("MainWindow.h", Qt::LeftDockWidgetArea);
    setupEditor("MainWindow.cpp", Qt::RightDockWidgetArea);
    // menuBar()->setVisible(false);
    _modeWidget = new QLabel("Normal");
    _modeWidget->setObjectName("mode");
    createStatusBar();
    setWindowTitle(tr("Whim"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Whim"),
                tr("I want to make something great." \
                   "I love text editors." \
                   "And so, here it is." \
                   "My lovely editor, inspired from vim and unix."));
}

void MainWindow::newFile()
{
    _editors.front()->clear();
}

CodeEditor* MainWindow::getActiveEditor()
{
    int active = 0;
    CodeEditor* editor = nullptr;
    for (CodeEditor* ed : _editors) {
        if (ed->getActive() > active) {
            active = ed->getActive();
            editor = ed;
        }
    }

    return editor;
}

void MainWindow::openFileDialog()
{
    // FIXME fix type
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "",
        "C++ Files (*.cpp *.h)"
    );

    if (!fileName.isEmpty()) {
        CodeEditor* ed = getActiveEditor();
        if (ed != nullptr)
            openFileInEditor(fileName, ed);
    }
}

void MainWindow::openFileInEditor(const QString &path, CodeEditor* editor)
{
    QFile file(path);
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        editor->setPlainText(file.readAll());
        editor->setFilename(path);
    }
}

// TODO ADD open type (split, dir, size, etc)
CodeEditor* MainWindow::setupEditor(QString filename, Qt::DockWidgetArea area)
{
    QDockWidget *dock;
    dock = new QDockWidget(this);
    CodeEditor* ed = new CodeEditor(dock);
    dock->setWidget(ed);

    _editors.push_back(ed);

    openFileInEditor(filename, ed);
    addDockWidget(area, dock);
    return ed;
}

// TODO figure out why the fuck it's on the right side
// TODO multiple different message types.
// TODO public function to handle these.
// TODO export QStatusBar to it's own inherited class?
void MainWindow::createStatusBar()
{
    statusBar()->addPermanentWidget(_modeWidget);
    statusBar()->showMessage(tr("Welcome to whim!"), 3 * 1000);
}

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
                        QKeySequence::New);

    fileMenu->addAction(tr("&Open..."), this, SLOT(openFileDialog()),
                        QKeySequence::Open);

    fileMenu->addAction(tr("&Save..."), this, SLOT(save()),
                        QKeySequence::Save);

    fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()),
                        QKeySequence::Quit);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, SLOT(about()));
    helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::save()
{
    QString path = _editors.front()->getFilename();
    statusBar()->showMessage(tr("Saving File : %1").arg(path), 3 * 1000);

    if (_editors.isEmpty())
        return;

    QFile file(path);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Dock Widgets"),
                tr("Cannot write file %1:\n%2.")
                .arg(path)
                .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << _editors.front()->toPlainText();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("Saved '%1'").arg(path), 2000);
}

void MainWindow::saveas()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                        tr("Choose a file name"), ".",
                        tr("HTML (*.html *.htm)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Dock Widgets"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << _editors.front()->toPlainText();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}
