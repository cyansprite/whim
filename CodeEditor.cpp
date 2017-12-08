#include <CodeEditor.h>

#include <QtGui>
#include <cmath>
#include <climits>
#include <iostream>

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent),
    _paddingFromLineNr(3),// TODO CONFIGCONFIG
    _shiftHeld(false), _altHeld(false), _ctrlHeld(false), _modHeld(false),
    _count(1),
    _countLimiter(pow(10,3)),
    _isCounting(false),
    _keySeq("")
{
    // TODO decide on what kind of config, json?
    // TODO work on Highlighter stuff
    _highlighter = new Highlighter(document());

    // TODO CONFIGCONFIG
    _lineNr = new LineNr(this);

    // TODO CONFIGCONFIG
    QFont font;
    font.setFamily("PT Mono");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);

    // SIGNALS and SLOTS
    // TODO SIGNALS
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(padForLineNr(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNr(QRect, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    padForLineNr(0);

    // TODO CONFIGCONFIG
    highlightCurrentLine();

    // TODO CONFIGCONFIG
    // will have to do what mode it is in
    setCursorWidth(fontMetrics().width(QLatin1Char('a')));

    // TODO CONFIGCONFIG
    // show end of buffer
    setBackgroundVisible(true);

    // TODO CONFIGCONFIG
    setLineWrapMode(QPlainTextEdit::NoWrap);

    // TODO CONFIGCONFIG 
    // setTabStopWidth(int width);

    // TODO COMMANDLINE!! :

    // TODO Buffer swapping
    // QPlainTextEdit::setDocument

    // TODO Keyboard commands
    // TODO NORMAL MODE
    // c-y/e QPlainTextEdit::scrollContentsBy(int dx, int dy)
    // zz -> QPlainTextEdit::centerCursor()
    // r  -> QPlainTextEdit::redo() -> might need to query QPlainTextEdit::redoAvailable(bool available)
    // u  -> QPlainTextEdit::undo() -> might need to query QPlainTextEdit::undoAvailable(bool available)
    // d  -> QPlainTextEdit::cut()
    // p  -> QPlainTextEdit::paste()
    // y  -> QPlainTextEdit::copy(), need registers as well
    // as well as a list kind of like my plugin extract
    // will probably need -> QPlainTextEdit::copyAvailable

    // TODO Insert mode
    // c-w   Delete word backwards in insert mode
    // c-u   Delete line in insert mode

    // TODO Multiple cursors anyone ? Need to use with regex too.
    // QList<QTextEdit::ExtraSelection> QPlainTextEdit::extraSelections() const

    // TODO QPlainTextEdit::find(QString)
    // Will need to extend with regex

    // TODO Cursor movements
    // QPlainTextEdit::setTextCursor(const QTextCursor & cursor)
    // H -> QPlainTextEdit::firstVisibleBlock()
    // QPlainTextEdit::moveCursor(QTextCursor::MoveOperation operation,
    //  QTextCursor::MoveMode mode = QTextCursor::MoveAnchor)

    // TODO Interface
    // QPlainTextEdit::modificationChanged(bool changed)

}

// TODO fix this lame ass function
int CodeEditor::lineNrAreaWidth()
{
    int digs = std::log10(blockCount()) + 1;
    int space = fontMetrics().width(QLatin1Char('9')) * digs;
    return space;
}

void CodeEditor::padForLineNr(int /*newBlockCount*/)
{
    setViewportMargins(lineNrAreaWidth() + _paddingFromLineNr, 0, 0, 0);
}

void CodeEditor::updateLineNr(const QRect &rect, int dy)
{
    if (dy)
        _lineNr->scroll(0, dy);
    else
        _lineNr->update(0, rect.y(), _lineNr->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        padForLineNr(0);
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(190);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNrPaintEvent(QPaintEvent *event)
{
    QPainter painter(_lineNr);
    painter.fillRect(event->rect(), QColor(Qt::lightGray).lighter(149));
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, _lineNr->width(), fontMetrics().height(),
                    Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

// QT Events
void CodeEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    _lineNr->setGeometry(QRect(cr.left(), cr.top(), lineNrAreaWidth(), cr.height()));
}

void CodeEditor::keyReleaseEvent(QKeyEvent* e)
{

    switch( e->key() )
    {
        // REGION: Mods
        case Qt::Key_Shift:
            _shiftHeld = false;
            break;
        case Qt::Key_Control:
            _shiftHeld = false;
            break;
        case Qt::Key_Alt:
            _shiftHeld = false;
            break;

        // ENDREGION: Mods
        /**************************************************/
        default:
            break;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent* e)
{
    // TODO TOO much to list :)
    if ( qobject_cast<Whim*>(qApp)->getMode() == Normal)
        normalKeyMode(e);
    else if ( qobject_cast<Whim*>(qApp)->getMode() == Insert)
        insertKeyMode(e);
}


void CodeEditor::insertKeyMode(QKeyEvent* e)
{
    switch( e->key() )
    {
        case Qt::Key_Escape:
            qobject_cast<Whim*>(qApp)->changeMode(Normal);
        default:
            QPlainTextEdit::keyPressEvent(e);
            break;
    }
}

void CodeEditor::normalKeyMode(QKeyEvent* e)
{
    QTextCursor cursor(textCursor());
    cursor.beginEditBlock();
    /* QT text cursor movements reference
     -- lol why
     QTextCursor::NoMove            0   Keep the cursor where it is

     -- Home/End
     QTextCursor::StartOfLine       3   Move to the start of the current line.
     QTextCursor::EndOfLine         13  Move to the end of the current line.

     -- Blocks
     QTextCursor::NextBlock         16  Move to the beginning of the next block.
     QTextCursor::PreviousBlock     6   Move to the start of the previous block.
     QTextCursor::StartOfBlock      4   Move to the start of the current block.
     QTextCursor::EndOfBlock        15  Move to the end of the current block.

     --Table Movement-- I don't use tables but it looks awesome
     QTextCursor::NextCell          21  Move to the beginning of the next table cell inside the current table. If the current cell is the last cell in the row, the cursor will move to the first cell in the next row.
     QTextCursor::PreviousCell      22  Move to the beginning of the previous table cell inside the current table. If the current cell is the first cell in the row, the cursor will move to the last cell in the previous row.
     QTextCursor::NextRow           23  Move to the first new cell of the next row in the current table.
     QTextCursor::PreviousRow       24  Move to the last cell of the previous row in the current table.
     */

    int count = _count;
    switch( e->key() )
    {
        /**************************************************
         **************** REGION: MOVEMENT ****************
         **************************************************/
        /* Chars
            QTextCursor::Up                2   Move up one line.
            QTextCursor::Left              9   Move left one character.
            QTextCursor::Down              12  Move down one line.
            QTextCursor::Right             19  Move right one character.

            QTextCursor::NextCharacter     17  Move to the next character.
            QTextCursor::PreviousCharacter 7   Move to the previous character.
        */
        case Qt::Key_H:
            if ( !_shiftHeld ) {
                while (_count--)
                    cursor.movePosition(QTextCursor::Left);
            }
            break;
        case Qt::Key_J:
            if ( !_shiftHeld ) {
                while (_count--) {
                    cursor.movePosition(QTextCursor::Down);
                }
            }
            break;
        case Qt::Key_K:
            if ( !_shiftHeld ) {
                while (_count--)
                    cursor.movePosition(QTextCursor::Up);
            }
            break;
        case Qt::Key_L:
            if ( !_shiftHeld ) {
                while (_count--)
                    cursor.movePosition(QTextCursor::Right);
            }
            break;


        /* Words
            QTextCursor::StartOfWord       5   Move to the start of the current word.
            QTextCursor::EndOfWord         14  Move to the end of the current word.

            QTextCursor::NextWord          18  Move to the next word.
            QTextCursor::PreviousWord      8   Move to the beginning of the previous word.

            QTextCursor::WordRight         20  Move right one word.
            QTextCursor::WordLeft          10  Move left one word.
        */

        case Qt::Key_W:
            if ( !_shiftHeld ) {
                while (_count--)
                    cursor.movePosition(QTextCursor::NextWord);
            }
            break;
        case Qt::Key_B:
            if ( !_shiftHeld ) {
                while (_count--)
                    cursor.movePosition(QTextCursor::PreviousWord);
            }
            break;
        case Qt::Key_E:
            if ( !_shiftHeld ) {
                while (_count--) {
                    cursor.movePosition(QTextCursor::EndOfWord);
                    cursor.movePosition(QTextCursor::Left);
                    if(cursor.position() == textCursor().position()) {
                        cursor.movePosition(QTextCursor::NextWord);
                        cursor.movePosition(QTextCursor::EndOfWord);
                        cursor.movePosition(QTextCursor::Left);
                    }
                }
            }
            break;
        // TODO subject to change
        case Qt::Key_S:
            if ( !_shiftHeld ) {
                while (_count--)
                    cursor.movePosition(QTextCursor::StartOfWord);
            }
            break;

        /* gg/G
            QTextCursor::Start             1   Move to the start of the document.
            QTextCursor::End               11  Move to the end of the document.
        */
        case Qt::Key_G:
            if ( !_shiftHeld ) {
                cursor.movePosition(QTextCursor::Start);
            } else {
                cursor.movePosition(QTextCursor::End);
            }
            break;

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
         ***************** REGION: NUMS ******************
         *************************************************/
        case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4:
        case Qt::Key_5: case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9:
            if ( Qt::Key_0 == e->key() && !_isCounting) {
                break;
            }

            if ( _countLimiter >= _count ) {
                unsigned num = e->key() - Qt::Key_0;
                if ( _isCounting )
                    _count = _count * 10 + num;
                else {
                    _count = num;
                    _isCounting = true;
                }
            }
            std::cout << _count << " - " << _countLimiter << std::endl;
            break;

        /*************************************************
         ***************** REGION: MODE ******************
         *************************************************/
        case Qt::Key_I:
            if ( !_shiftHeld ) {
                qobject_cast<Whim*>(qApp)->changeMode(Insert);
            }
            break;

        /*************************************************
         ***************** REGION: EDIT ******************
         *************************************************/
        case Qt::Key_U:
            if ( !_shiftHeld ) {
                undo();
            }
            break;
        case Qt::Key_R:
            if ( _ctrlHeld ) {
                redo();
            }
            break;

        // FIXME _keySeq needs to have clear on invalid...
        /*************************************************
         **************** REGION: SCROLL *****************
         *************************************************/
        case Qt::Key_Z:
            if ( !_shiftHeld ) {
                if ( _keySeq == "z" ) {
                    centerCursor();
                    _keySeq = "";
                }
                else {
                    _keySeq = "z";
                }
            }
            break;

        default:
            std::cout << e->text().toStdString() << std::endl;
    }

    if (count > _count) {
        _isCounting = false;
        _count = 1;
    }

    setTextCursor(cursor);
    cursor.endEditBlock();
}
