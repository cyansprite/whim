#include "CodeDock.h"
#include <iostream>

CodeDock::CodeDock(QWidget* parent) : QDockWidget(parent)
{
    _editor = new CodeEditor();
    setWidget(_editor);
}

bool CodeDock::openNew(QString fileText, QString path)
{
    if ( !areYouSure() )
        return false;

    _editor->setPlainText(fileText);
    _editor->setFilename(path);
    setWindowTitle(path);
    return true;
}

bool CodeDock::save()
{
    // FIXME filter types
    QString fileName = windowTitle();
    if (fileName == "") {
        fileName = QFileDialog::getSaveFileName(this,
                            tr("Choose a file name"), ".",
                            tr("HTML (*.html *.htm)"));
    }

    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Dock Widgets"),
                tr("Cannot write file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << _editor->toPlainText();
    _editor->document()->setModified(false);
    return true;
}

bool CodeDock::areYouSure()
{
    if ( !_editor->document()->isModified() )
        return true;

    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret) {
        case QMessageBox::Save:
            save();
        case QMessageBox::Discard:
            return true;
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
    }

    return false;

}

void CodeDock::closeEvent(QCloseEvent * /*event*/ )
{
    areYouSure();
    /* if (maybeSave()) {
    //     writeSettings();
    event->accept();
    // } else {
    //     event->ignore();
    // }
    // 
    */
}
