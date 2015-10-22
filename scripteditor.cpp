#include "scripteditor.h"
#include "scripteditorlinenumbers.h"
#include <QTextDocumentFragment>
#include <QTextBlock>

ScriptEditor::ScriptEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    lineNumbers = new ScriptEditorLineNumbers(this);

    setStyleSheet("font-family: Monaco, Monospace, Courier New, Courier; font-size: 13px");

    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumbers(QRect,int)));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(linesNumberChanged(int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    linesNumberChanged(blockCount());
    highlightCurrentLine();
}

void ScriptEditor::updateLineNumbers(const QRect&, int dy)
{
    if (dy) {
        lineNumbers->scroll(0, dy);
    } else {
        lineNumbers->update();
    }
}

void ScriptEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    cr.setWidth(lineNumbers->getPreferedWidth());
    lineNumbers->setGeometry(cr);
}

void ScriptEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        if (e->key() == Qt::Key_Backtab || e->modifiers() & Qt::ShiftModifier) {
            if (cursor.anchor() != cursor.position()) {
                QTextBlock b = cursor.block();
                while (b.position() > cursor.selectionStart()) {
                    b = b.previous();
                }
                if (b.next().isValid() && b.next().position() <= cursor.selectionEnd()) {
                    int selStart = b.position();
                    int selEnd = b.position() + b.length() - 1;
                    int end = cursor.selectionEnd();
                    cursor.beginEditBlock();
                    while (b.isValid() && b.position() <= end) {
                        cursor.setPosition(b.position());
                        for (int i = 0; i < 4 && i < b.text().length() && b.text().at(i) == ' '; i++) {
                            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                        }
                        cursor.removeSelectedText();
                        selEnd = b.position() + b.length() - 1;
                        b = b.next();
                    }
                    cursor.endEditBlock();
                    cursor.clearSelection();
                    cursor.setPosition(selStart);
                    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
                    setTextCursor(cursor);
                    return;
                }
            }
            int width = cursor.positionInBlock() & 3;
            if (width < 2)
                width += 4;
            cursor.beginEditBlock();
            cursor.removeSelectedText();
            int e = cursor.positionInBlock() - width - 1;
            if (e < 0)
                e = 0;
            for (int i = cursor.positionInBlock() - 1; i >= e && cursor.block().text().at(i) == ' '; i--) {
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            }
            cursor.removeSelectedText();
            cursor.endEditBlock();
        } else {
            if (cursor.anchor() != cursor.position()) {
                QTextBlock b = cursor.block();
                while (b.position() > cursor.selectionStart()) {
                    b = b.previous();
                }
                if (b.next().isValid() && b.next().position() <= cursor.selectionEnd()) {
                    int selStart = b.position();
                    int selEnd = b.position() + b.length() - 1;
                    int end = cursor.selectionEnd();
                    cursor.beginEditBlock();
                    while (b.isValid() && b.position() <= end) {
                        cursor.setPosition(b.position());
                        cursor.insertText("    ");
                        selEnd = b.position() + b.length() - 1;
                        b = b.next();
                    }
                    cursor.endEditBlock();
                    cursor.clearSelection();
                    cursor.setPosition(selStart);
                    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
                    setTextCursor(cursor);
                    return;
                }
            }
            int width = 4 - (cursor.positionInBlock() & 3);
            if (width < 2)
                width += 4;
            cursor.insertText(QString(" ").repeated(width));
        }
    } else {
        QPlainTextEdit::keyPressEvent(e);
    }
}

void ScriptEditor::show()
{
    QPlainTextEdit::show();
    QRect cr = contentsRect();
    cr.setWidth(lineNumbers->getPreferedWidth());
    lineNumbers->setGeometry(cr);
    setViewportMargins(lineNumbers->getPreferedWidth(), 0, 0, 0);
}

void ScriptEditor::linesNumberChanged(int)
{
    setViewportMargins(lineNumbers->getPreferedWidth(), 0, 0, 0);
}

void ScriptEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(QColor(240, 240, 255));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
