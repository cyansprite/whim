#include <QtGui>

#include "MainWindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // TODO CONFIGCONFIG
    // setTabPosition(Qt::DockWidgetAreas areas, QTabWidget::TabPosition tabPosition)
    // connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));

    //... what does this do?
    setDocumentMode(true);
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

    connect(qobject_cast<Whim*>(qApp), SIGNAL(modeChanged(Mode, Mode)), this, SLOT(modeChanged(Mode, Mode)));

    // TODO make the command line an actual overloaded class...
    // need custom key commands on pmenu for it, and blah blah.
    QStringList wordList;
    wordList << "alpha" << "omega" << "omicron" << "zeta";
    QLineEdit *lineEdit = new QLineEdit(this);
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    QToolBar* toolbar = new QToolBar("Command");
    toolbar->addWidget(lineEdit);
    addToolBar(Qt::BottomToolBarArea, toolbar);
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

void MainWindow::openFileDialog()
{
    CodeEditor *ed = qobject_cast<CodeEditor*>(QApplication::focusWidget());

    // FIXME fix type
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "",
        "C++ Files (*.cpp *.h)"
    );

    if (!fileName.isEmpty()) {
        if ( ed )
            openFileInEditor(fileName, qobject_cast<CodeDock*>(ed->parentWidget()));
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

    CodeEditor *ed = qobject_cast<CodeEditor*>(QApplication::focusWidget());
    if ( !ed )
        return;
    CodeDock* dock = qobject_cast<CodeDock*>(ed->parentWidget());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool suc = dock->save();
    if ( suc )
        statusBar()->showMessage(tr("Saved '%1'").arg(dock->windowTitle()), 2000);
    else
        statusBar()->showMessage(tr("Failed Saving '%1'").arg(dock->windowTitle()), 2000);
    QApplication::restoreOverrideCursor();
}

void MainWindow::modeChanged(Mode /*oldmode*/, Mode mode)
{
    _modeWidget->setText(Whim::ModeToText(mode));
}
