#include <CodeEditor.h>
#include <QtGui>
#include <iostream>
#include <cmath>

int CodeEditor::ActiveCount = 0;

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent),
    _active(CodeEditor::ActiveCount),
    _paddingFromLineNr(3)
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

void CodeEditor::focusInEvent(QFocusEvent* e)
{
    QPlainTextEdit::focusInEvent(e);
    _active = CodeEditor::ActiveCount++;
}
