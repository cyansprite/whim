#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QDockWidget>
#include "Highlighter.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class LineNr;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

    public:
        CodeEditor(QWidget* parent = 0);

        void lineNrPaintEvent(QPaintEvent* event);
        int lineNrAreaWidth();

        void setFilename(QString filename){
            _filename = filename;
        }

        QString getFilename(){
            return _filename;
        }

        int getActive(){
            return _active;
        }


        static int ActiveCount;

    protected:
        void resizeEvent(QResizeEvent* event);
        void focusInEvent(QFocusEvent* event);

    private slots:
        void padForLineNr(int newBlockCount);
        void highlightCurrentLine();
        void updateLineNr(const QRect &, int);

    private:
        int _active;
        int _paddingFromLineNr;

        LineNr* _lineNr;
        Highlighter* _highlighter;
        QString _filename;
};

class LineNr : public QWidget
{
    public:
        LineNr(CodeEditor* editor) : QWidget(editor) {
            codeEditor = editor;
        }

        QSize sizeHint() const {
            return QSize(codeEditor->lineNrAreaWidth(), 0);
        }

    protected:
        // QT
        void paintEvent(QPaintEvent* event) {
            codeEditor->lineNrPaintEvent(event);
        }

    private:
        CodeEditor* codeEditor;
};

#endif
