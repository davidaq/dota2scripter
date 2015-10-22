#include "scripteditor.h"
#include "scripteditorlinenumbers.h"
#include "scriptdocument.h"
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
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

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

static void inline indent(QTextCursor& cursor, bool isBackTab) {
    QList<QTextBlock> selectedBlocks;
    if (cursor.anchor() != cursor.position()) {
        QTextBlock b = cursor.block();
        while (b.position() > cursor.selectionStart()) {
            b = b.previous();
        }
        int end = cursor.selectionEnd();
        if (b.next().isValid() && b.next().position() <= end) {
            while (b.isValid() && b.position() <= end) {
                selectedBlocks << b;
                b = b.next();
            }
        }
    }
    if (selectedBlocks.isEmpty()) {
        int width = 4 - (cursor.positionInBlock() & 3);
        if (width < 2)
            width += 4;
        if (isBackTab) {
            cursor.removeSelectedText();
            const QString& text = cursor.block().text();
            if (cursor.atEnd() && text.at(cursor.positionInBlock() - 1) != ' ') {
                cursor.movePosition(QTextCursor::StartOfBlock);
                for (int i = 0; i < text.length() && text.at(i) == ' '; i++) {
                    cursor.movePosition(QTextCursor::Right);
                }
            }
            int e = cursor.positionInBlock() - width - 1;
            if (e < 0)
                e = 0;
            for (int i = cursor.positionInBlock() - 1; i >= e && text.at(i) == ' '; i--) {
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            }
            cursor.removeSelectedText();
        } else {
            if (cursor.atBlockStart() && cursor.block().text().trimmed().isEmpty()) {
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                ScriptDocument* doc = dynamic_cast<ScriptDocument*>(cursor.document());
                if (doc && doc->assistant()->autoIndent(cursor)) {
                    return;
                }
            }
            cursor.insertText(QString(" ").repeated(width));
        }
    } else {
        foreach (const QTextBlock& b, selectedBlocks) {
            cursor.setPosition(b.position());
            if (isBackTab) {
                for (int i = 0; i < 4 && i < b.text().length() && b.text().at(i) == ' '; i++) {
                    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                }
                cursor.removeSelectedText();
            } else {
                cursor.insertText("    ");
            }
        }
        cursor.setPosition(selectedBlocks.first().position());
        cursor.setPosition(selectedBlocks.last().position(), QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }
}

void ScriptEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        indent(cursor, e->key() == Qt::Key_Backtab || e->modifiers() & Qt::ShiftModifier);
        cursor.endEditBlock();
        setTextCursor(cursor);
    } else if(e->key() == Qt::Key_Return) {
        QPlainTextEdit::keyPressEvent(e);
        ScriptDocument* doc = dynamic_cast<ScriptDocument*>(document());
        if (doc) {
            QTextCursor cursor = textCursor();
            cursor.beginEditBlock();
            doc->assistant()->autoIndent(cursor);
            cursor.endEditBlock();
            setTextCursor(cursor);
        }
    } else {
        QPlainTextEdit::keyPressEvent(e);
        ScriptDocument* doc = dynamic_cast<ScriptDocument*>(document());
        if (doc) {
            doc->assistant()->update(e->key(), *this);
        }
    }
}

void ScriptEditor::onTextChanged()
{
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
