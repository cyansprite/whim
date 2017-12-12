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
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(editorMargins(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNr(QRect, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(qobject_cast<Whim*>(qApp), SIGNAL(modeChanged(Mode,Mode)), this, SLOT(modeChanged(Mode,Mode)));

    editorMargins(0);

    // TODO CONFIGCONFIG
    highlightCurrentLine();

    // TODO CONFIGCONFIG
    setCursorWidth(fontMetrics().width(QLatin1Char('a')));

    // TODO CONFIGCONFIG
    // show end of buffer
    setBackgroundVisible(true);

    // TODO CONFIGCONFIG
    setLineWrapMode(QPlainTextEdit::NoWrap);

    _completer = new QCompleter(this);
    _completer->setModelSorting(QCompleter::UnsortedModel);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    _completer->setWrapAround(false);
    _completer->setWidget(this);
    // TODO CONFIGCONFIG
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    connect(_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

    // TODO CONFIGCONFIG 
    // setTabStopWidth(int width);

    // TODO COMMANDLINE!! :

    // TODO Buffer swapping
    // QPlainTextEdit::setDocument

    // TODO Keyboard commands
    // TODO NORMAL MODE
    // c-y/e QPlainTextEdit::scrollContentsBy(int dx, int dy)
    // d  -> QPlainTextEdit::cut()
    // p  -> QPlainTextEdit::paste()
    // y  -> QPlainTextEdit::copy(), need registers as well
    // as well as a list kind of like my plugin extract
    // will probably need -> QPlainTextEdit::copyAvailable

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

int CodeEditor::lineNrAreaWidth()
{
    int digs = std::log10(blockCount()) + 1;
    int space = fontMetrics().width(QLatin1Char('9')) * digs;
    return space * 1.25f;
}

void CodeEditor::editorMargins(int /*newBlockCount*/)
{
    setViewportMargins(lineNrAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNr(const QRect &rect, int dy)
{
    if (dy)
        _lineNr->scroll(0, dy);
    else
        _lineNr->update(0, rect.y(), _lineNr->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        editorMargins(0);
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    // TODO Read only and non-readonly CONFIGCONFIG
    QColor lineColor;
    if (!isReadOnly()) {
        if ( qobject_cast<Whim*>(qApp)->getMode() == Insert )
            lineColor = QColor(Qt::cyan).lighter(190);
        else if ( qobject_cast<Whim*>(qApp)->getMode() == Normal )
            lineColor = QColor(Qt::blue).lighter(190);
    }
    else
        lineColor = QColor(Qt::red ).lighter(190);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNrPaintEvent(QPaintEvent *event)
{
    QPainter painter(_lineNr);
    QStyleOptionButton option;
    option.initFrom(this);
    painter.fillRect(event->rect(), option.palette.color(QPalette::Background).darker(125));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        // TODO line nr color , and linenr padding CONFIGCONFIG
        QString number = QString::number(blockNumber + 1);
        if (blockNumber ==  textCursor().block().blockNumber()) {
            painter.fillRect(0, top, _lineNr->width() / 1.25f, fontMetrics().height(), option.palette.color(QPalette::Background));
            painter.setPen(QColor(Qt::black));
        }
        else
            painter.setPen(option.palette.color(QPalette::Foreground));
        painter.drawText(0, top, _lineNr->width() / 1.25f, fontMetrics().height(), Qt::AlignRight, number);

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

    painter.fillRect(_lineNr->width() - _paddingFromLineNr, event->rect().y(), _paddingFromLineNr, event->rect().height(),option.palette.color(QPalette::Background).darker(110));
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
            _ctrlHeld = false;
            break;
        case Qt::Key_Alt:
            _altHeld = false;
            break;

        // ENDREGION: Mods
        /**************************************************/
        default:
            break;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent* e)
{
    bool cont = false;
    // TODO TOO much to list :)
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
        default:
            cont = true;
    }

    if ( !cont )
        return;

    if ( qobject_cast<Whim*>(qApp)->getMode() == Normal)
        normalKeyMode(e);
    else if ( qobject_cast<Whim*>(qApp)->getMode() == Insert)
        insertKeyMode(e);

}

// add to list
void CodeEditor::copy()
{
    QPlainTextEdit::copy();
}

void CodeEditor::cut()
{
    QPlainTextEdit::cut();
}

void CodeEditor::paste()
{
    QPlainTextEdit::paste();
}

// TODO move to a dictionary or something, with std::functionals or something.
void CodeEditor::insertKeyMode(QKeyEvent* e)
{
    QTextCursor cursor(textCursor());
    bool sendUp = true;
    switch( e->key() )
    {
        case Qt::Key_Escape:
            qobject_cast<Whim*>(qApp)->changeMode(Insert, Normal);
            sendUp = false;
            break;
        case Qt::Key_N:
            if ( _ctrlHeld ) {
            }
            break;
        case Qt::Key_C:
            if ( _ctrlHeld ) {
                qobject_cast<Whim*>(qApp)->changeMode(Insert, Normal);
                sendUp = false;
            }
            break;
        case Qt::Key_V:
            if ( _ctrlHeld ) {
            }
            break;
        case Qt::Key_W:
            if ( _ctrlHeld ) {
                cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                sendUp = false;
            }

            break;
        case Qt::Key_U:
            if ( _ctrlHeld ) {
                cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                sendUp = false;
            }
            break;
        default:
            break;
    }

    if ( sendUp ) {
        // Completer
        if (_completer && _completer->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
            switch (e->key()) {
                case Qt::Key_Enter:
                case Qt::Key_Return:
                case Qt::Key_Escape:
                case Qt::Key_Tab:
                case Qt::Key_Backtab:
                    e->ignore();
                    return; // let the completer do default behavior
                default:
                    break;
            }
        }

        QString buffer = toPlainText();
        QStringList list(buffer.split(QRegExp("\\s+|\\W"), QString::SkipEmptyParts));
        list.removeDuplicates();
        _completer->setModel(new QStringListModel(list));

        QPlainTextEdit::keyPressEvent(e);

        static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
        QString completionPrefix = textUnderCursor();

        std::cout << completionPrefix.toStdString() << std::endl;

        // TODO < amt
        if (completionPrefix.length() < 2 || eow.contains(e->text().right(1))) {
            _completer->popup()->hide();
            return;
        }

        if (completionPrefix != _completer->completionPrefix()) {
            _completer->setCompletionPrefix(completionPrefix);
            _completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
        }

        QRect cr = cursorRect();
        cr.setWidth(_completer->popup()->sizeHintForColumn(0) + _completer->popup()->verticalScrollBar()->sizeHint().width());
        _completer->complete(cr); // popup it up!
    }
}

void CodeEditor::normalKeyMode(QKeyEvent* e)
{
    QTextCursor cursor(textCursor());
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
        case Qt::Key_S:
            if ( !_shiftHeld ) {
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

     // QTextCursor::StartOfLine       3   Move to the start of the current line.
     // QTextCursor::EndOfLine         13  Move to the end of the current line.
        /*************************************************
         ***************** REGION: MODE ******************
         *************************************************/
        case Qt::Key_A:
            if ( !_shiftHeld ) {
            } else {
                cursor.movePosition(QTextCursor::EndOfLine);
                qobject_cast<Whim*>(qApp)->changeMode(Normal, Insert);
            }
            break;
        case Qt::Key_O:
            while ( _count-- )
                if ( !_shiftHeld ) {
                    cursor.movePosition(QTextCursor::Down);
                    qobject_cast<Whim*>(qApp)->changeMode(Normal, Insert);
                    setTextCursor(cursor);
                    insertPlainText("\n");
                    cursor.movePosition(QTextCursor::Up);
                } else {
                    qobject_cast<Whim*>(qApp)->changeMode(Normal, Insert);
                    insertPlainText("\n");
                    cursor.movePosition(QTextCursor::Up);
                }
            break;
        case Qt::Key_I:
            if ( !_shiftHeld ) {
                qobject_cast<Whim*>(qApp)->changeMode(Normal, Insert);
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                qobject_cast<Whim*>(qApp)->changeMode(Normal, Insert);
            }
            break;

        /*************************************************
         ***************** REGION: EDIT ******************
         *************************************************/
        case Qt::Key_D:
            if ( !_shiftHeld ) {
                if ( _ctrlHeld ) {
                    // cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, _lastBlock - firstVisibleBlock().blockNumber() );
                } else {
                    // undo();
                }
            }
            break;
        case Qt::Key_U:
            if ( !_shiftHeld ) {
                if ( _ctrlHeld ) {
                    // cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, _lastBlock - firstVisibleBlock().blockNumber() );
                } else {
                    undo();
                }
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
}

void CodeEditor::modeChanged(Mode oldmode, Mode mode)
{
    highlightCurrentLine();
    if (oldmode == Insert) {
        _completer->popup()->hide();
    }

    switch(mode) {
        case Normal:
            setCursorWidth(fontMetrics().width(QLatin1Char('a')) - 1);
            break;
        case Insert:
            setCursorWidth(2);
            break;
        default:
            break;
    }
}

void CodeEditor::insertCompletion(const QString& completion)
{
    if (_completer->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - _completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (_completer)
        _completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}
