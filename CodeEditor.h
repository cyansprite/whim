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
class QCompleter;
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
        void focusInEvent(QFocusEvent* event);
        void keyPressEvent(QKeyEvent* e);
        void keyReleaseEvent(QKeyEvent* e);
        void copy();
        void paste();
        void cut();

    private slots:
        void editorMargins(int newBlockCount);
        void highlightCurrentLine();
        void updateLineNr(const QRect &, int);
        void modeChanged(Mode oldmode, Mode mode);
        void insertCompletion(const QString& completion);

    private:
        QString textUnderCursor() const;
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
        QCompleter* _completer;
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
        void paintEvent(QPaintEvent* event) {
            codeEditor->lineNrPaintEvent(event);
        }

    private:
        CodeEditor* codeEditor;
};

// TODO see if lineedit will need this
class PMenu : public QMenu
{
    public:
        PMenu() : QMenu() {
            setObjectName("pmenu");
            QGraphicsDropShadowEffect *q = new QGraphicsDropShadowEffect();
            q->setBlurRadius(3);
            setGraphicsEffect(q);
        }

    void keyPressEvent(QKeyEvent* e)
    {
        // NOTE: yes I know of e->modifiers() but it can't be trusted.
        switch( e->key() ) {
            /*************************************************
            ***************** REGION: MODS ******************
            *************************************************/
            case Qt::Key_Shift:
                _shiftHeld = true;
                break;
            case Qt::Key_Control:
                _ctrlHeld = true;
                break;
            case Qt::Key_Alt:
                _altHeld = true;
                break;

            /*************************************************
            ***************** REGION: MOVE ******************
            *************************************************/
            case Qt::Key_C:
                if ( _ctrlHeld )
                    close();
            case Qt::Key_Space:
                QMenu::keyPressEvent(e);
            case Qt::Key_N: case Qt::Key_J:
                if ( _ctrlHeld )
                    focusNextPrevChild(true);
                break;
            case Qt::Key_P: case Qt::Key_K:
                if ( _ctrlHeld )
                    focusNextPrevChild(false);
                break;
            default:
                QMenu::keyPressEvent(e);
                break;
        }
    }
    private:
        bool _ctrlHeld;
        bool _shiftHeld;
        bool _altHeld;
};


#endif
