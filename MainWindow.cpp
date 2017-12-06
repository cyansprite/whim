#include <QtGui>
#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _id = -1;
    // Window handles TODO CONFIGCONFIG
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
    _docks.front()->openNew("","");
}

CodeDock* MainWindow::getActiveDock()
{
    int active = 0;
    CodeDock* rtndock = nullptr;
    for (CodeDock* dock : _docks) {
        if (dock->getEditor()->getActive() > active) {
            active = dock->getEditor()->getActive();
            rtndock = dock;
        }
    }

    return rtndock;
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
        CodeDock* dock = getActiveDock();
        if (dock != nullptr)
            openFileInEditor(fileName, dock);
        else
            setupEditor(fileName, Qt::LeftDockWidgetArea);
    }
}

void MainWindow::openFileInEditor(const QString &path, CodeDock* dock)
{
    QFile file(path);
    if (file.open(QFile::ReadWrite | QFile::Text)) {
        dock->openNew(file.readAll(), path);
    }
}

// TODO ADD open type (split, dir, size, etc)
void MainWindow::setupEditor(QString filename, Qt::DockWidgetArea area)
{
    CodeDock* dock = new CodeDock(this);
    _docks.push_back(dock);
    openFileInEditor(filename, dock);
    addDockWidget(area, dock);
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
    if (_docks.isEmpty())
        return;

    CodeDock* dock = getActiveDock();
    if ( dock == nullptr )
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool suc = dock->save();
    if ( suc )
        statusBar()->showMessage(tr("Saved '%1'").arg(dock->windowTitle()), 2000);
    else
        statusBar()->showMessage(tr("Failed Saving '%1'").arg(dock->windowTitle()), 2000);
    QApplication::restoreOverrideCursor();
}
