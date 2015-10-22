#include "scripteditorlinenumbers.h"
#include "scripteditor.h"
#include <QPainter>
#include <QTextBlock>

ScriptEditorLineNumbers::ScriptEditorLineNumbers(ScriptEditor *parent) :
    QWidget(parent),
    editor(parent)
{
}

int ScriptEditorLineNumbers::getPreferedWidth() const
{
    int digits = 0;
    int lines = editor->blockCount();
    while (lines > 0) {
        digits++;
        lines /= 10;
    }
    return editor->fontMetrics().width(QLatin1Char('9')) * digits + 10;
}

void ScriptEditorLineNumbers::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) editor->blockBoundingGeometry(block).translated(editor->contentOffset()).top();
    int bottom = top + (int) editor->blockBoundingRect(block).height();

    painter.setPen(QColor(180, 180, 180));
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, width() - 5, editor->fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) editor->blockBoundingRect(block).height();
        blockNumber++;
    }
}
