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
    if (e->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        if (cursor.anchor() == cursor.position()) {
            insertPlainText("    ");
        } else {
            QTextBlock b = cursor.block();
            cursor.beginEditBlock();
            while (b.position() > cursor.selectionStart()) {
                b = b.previous();
            }
            int end = cursor.selectionEnd();
            while (b.isValid() && b.position() <= end) {
                cursor.setPosition(b.position());
                cursor.insertText("    ");
                b = b.next();
            }
            cursor.endEditBlock();
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
