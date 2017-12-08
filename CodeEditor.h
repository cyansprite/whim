#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

#include "Whim.h"
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

    protected:
        void resizeEvent(QResizeEvent* event);
        void keyPressEvent(QKeyEvent* e);
        void keyReleaseEvent(QKeyEvent* e);

    private slots:
        void padForLineNr(int newBlockCount);
        void highlightCurrentLine();
        void updateLineNr(const QRect &, int);

    private:
        void normalKeyMode(QKeyEvent* e);
        void insertKeyMode(QKeyEvent* e);

        LineNr* _lineNr;
        Highlighter* _highlighter;
        QString _filename;

        int _paddingFromLineNr;

        bool _shiftHeld;
        bool _altHeld;
        bool _ctrlHeld;
        bool _modHeld; // TODO

        int _count;
        int _countLimiter;
        bool _isCounting;
        QString _keySeq;
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
