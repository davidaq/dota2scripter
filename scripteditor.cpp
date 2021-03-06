#include "scripteditor.h"
#include "scripteditorlinenumbers.h"
#include "scriptdocument.h"
#include <QTextDocumentFragment>
#include <QTextBlock>
#include <QMimeData>
#include <QListWidget>

ScriptEditor::ScriptEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    lineNumbers = new ScriptEditorLineNumbers(this);

    inputTipWidget = new QListWidget(this->viewport());
    inputTipWidget->hide();
    inputTipWidget->setSortingEnabled(true);

    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumbers(QRect,int)));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(linesNumberChanged(int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(adjustInputTipWidget()));

    linesNumberChanged(blockCount());
    highlightCurrentLine();
}

void ScriptEditor::setDocument(QTextDocument *document)
{
    QPlainTextEdit::setDocument(document);
    setStyleSheet("font-family: Monaco, Monospace, Courier New, Courier !important; font-size: 13px");
}

void ScriptEditor::onReceiveInputTip(ScriptAssistant* assistant)
{
    if (!inputTipWidget->isHidden())
        return;
    QList<ScriptAssistant::Tip> candidate = assistant->inputTip(textCursor());
    inputTipWidget->clear();
    foreach (const ScriptAssistant::Tip& tip, candidate) {
        QListWidgetItem* item = new QListWidgetItem();
        if (tip.second.isEmpty()) {
            item->setText(tip.first);
        } else {
            item->setText(tip.second);
        }
        item->setData(Qt::UserRole, tip.first);
        inputTipWidget->addItem(item);
    }
    inputTipWidget->setCurrentRow(0);
    inputTipWidget->show();
    filterInputTip();
    adjustInputTipWidget();
    inputTipWidget->scrollToTop();
}

void ScriptEditor::adjustInputTipWidget()
{
    if (inputTipWidget->isHidden())
        return;
    int showLines = inputTipWidget->count();
    if (showLines <= 0) {
        inputTipWidget->hide();
        return;
    }
    if (showLines > 7) {
        showLines = 7;
    }
    int h = inputTipWidget->sizeHintForRow(0) * showLines;
    QMargins margin = inputTipWidget->contentsMargins();
    h += margin.top() + margin.bottom();
    int w = 0;
    for (int i = 0; i < inputTipWidget->count(); i++) {
        int cw = inputTipWidget->sizeHintForColumn(i);
        if (cw > w) {
            w = cw;
        }
    }
    w += margin.left() + margin.right() + 40;
    int mw = viewport()->width() * 2 / 3;
    if (w > mw) {
        w = mw;
        h += 20;
    }
    inputTipWidget->resize(w, h);
    int x = cursorRect().left();
    int y = cursorRect().bottom();
    int wMargin = viewport()->contentsMargins().left() + viewport()->contentsMargins().right();
    if (x + w + wMargin > width()) {
        x = width() - w - wMargin;
    }
    if (y + h > height()) {
        y = cursorRect().top() - h;
    }
    inputTipWidget->move(x, y);
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

static inline QList<QTextBlock> getSelectedBlocks(QTextCursor& cursor)
{
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
    return selectedBlocks;
}

static inline void indent(QTextCursor& cursor, bool isBackTab) {
    QList<QTextBlock> selectedBlocks = getSelectedBlocks(cursor);
    if (selectedBlocks.isEmpty()) {
        if (isBackTab) {
            const QString& text = cursor.block().text();
            if (cursor.selectionStart() != cursor.selectionEnd()) {
                cursor.removeSelectedText();
            } else if (cursor.atBlockEnd() && text.at(cursor.positionInBlock() - 1) != ' ') {
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition(QTextCursor::NextWord);
            }
            int width = cursor.positionInBlock() & 3;
            if (width == 0)
                width = 4;
            int e = cursor.positionInBlock() - width;
            if (e < 0)
                e = 0;
            for (int i = cursor.positionInBlock() - 1; i >= e && text.at(i) == ' '; i--) {
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            }
            cursor.removeSelectedText();
        } else {
            int width = 4 - (cursor.positionInBlock() & 3);
            if (width == 0)
                width = 4;
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
    ScriptDocument* doc = dynamic_cast<ScriptDocument*>(document());
    if (!doc) {
        QPlainTextEdit::keyPressEvent(e);
        return;
    }
    if (!inputTipWidget->isHidden()) {
        switch(e->key()) {
        case Qt::Key_Left:
        case Qt::Key_Up: {
            int row = inputTipWidget->currentRow();
            row--;
            if (row < 0)
                row = 0;
            inputTipWidget->setCurrentRow(row);
            return;
        }
        case Qt::Key_Right:
        case Qt::Key_Down:{
            int row = inputTipWidget->currentRow();
            row++;
            if (row >= inputTipWidget->count())
                row = inputTipWidget->count() - 1;
            inputTipWidget->setCurrentRow(row);
            return;
        }
        case Qt::Key_Tab:
        case Qt::Key_Return: {
            QTextCursor cursor = textCursor();
            QString line = cursor.block().text();
            while (!doc->assistant()->isWordBreak(line, cursor.positionInBlock() - 1)) {
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            }
            cursor.insertText(inputTipWidget->currentItem()->data(Qt::UserRole).toString());
            inputTipWidget->clear();
            inputTipWidget->hide();
            return;
        }
        }
        if (e->key() != Qt::Key_Shift && doc->assistant()->isWordBreak(e->text(), 0)) {
            inputTipWidget->clear();
            inputTipWidget->hide();
        }
    }
    if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        indent(cursor, e->key() == Qt::Key_Backtab || e->modifiers() & Qt::ShiftModifier);
        cursor.endEditBlock();
        setTextCursor(cursor);
    } else {
        bool autoIndent = false;
        if(e->key() == Qt::Key_Return) {
            autoIndent = true;
            if (e->modifiers() & Qt::ShiftModifier) {
                e->setModifiers(Qt::NoModifier);
                autoIndent = false;
            }
        }
        bool modified = doc->isModified();
        doc->setModified(false);
        QPlainTextEdit::keyPressEvent(e);
        if (doc->isModified()) {
            ScriptAssistant* assistant = doc->assistant();
            QTextCursor cursor = textCursor();
            cursor.beginEditBlock();
            if (autoIndent) {
                assistant->autoIndent(cursor);
            }
            assistant->informInputFromEditor(e->key(), cursor, this);
            cursor.endEditBlock();
        }
        if (modified) {
            doc->setModified(true);
        }
        filterInputTip();
    }
}

void ScriptEditor::filterInputTip()
{
    ScriptDocument* doc = dynamic_cast<ScriptDocument*>(document());
    if (!doc) {
        return;
    }
    if (!inputTipWidget->isHidden()) {
        QTextCursor cursor = textCursor();
        QString prefix = doc->assistant()->wordBeforeCursor(cursor);
        if (!prefix.isEmpty()) {
            for (int i = 0; i < inputTipWidget->count(); i++) {
                if (!inputTipWidget->item(i)->data(Qt::UserRole).toString().startsWith(prefix)) {
                    delete inputTipWidget->takeItem(i);
                    i--;
                }
            }
        }
        if (inputTipWidget->count() == 0) {
            inputTipWidget->clear();
            inputTipWidget->hide();
        } else {
            inputTipWidget->setCurrentRow(0);
            adjustInputTipWidget();
        }
    }
}

void ScriptEditor::mousePressEvent(QMouseEvent *e)
{
    QPlainTextEdit::mousePressEvent(e);
    inputTipWidget->clear();
    inputTipWidget->hide();
}

void ScriptEditor::focusInEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusInEvent(e);
    emit onFocus(this);
    inputTipWidget->clear();
    inputTipWidget->hide();
}

void ScriptEditor::focusOutEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusOutEvent(e);
//    inputTipWidget->clear();
//    inputTipWidget->hide();
}

void ScriptEditor::insertFromMimeData(const QMimeData *source)
{
    insertPlainText(source->text());
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
        QTextEdit::ExtraSelection wholeLine;
        wholeLine.format.setBackground(QColor(247, 247, 255));
        wholeLine.format.setProperty(QTextFormat::FullWidthSelection, true);
        wholeLine.cursor = textCursor();
        wholeLine.cursor.clearSelection();
        extraSelections.append(wholeLine);
        QTextEdit::ExtraSelection texted;
        texted.format.setBackground(QColor(240, 240, 255));
        texted.cursor = textCursor();
        texted.cursor.movePosition(QTextCursor::StartOfBlock);
        texted.cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        extraSelections.append(texted);
        ScriptDocument* doc = dynamic_cast<ScriptDocument*>(document());
        if (doc) {
            doc->assistant()->onExtraHighLight(extraSelections, textCursor());
        }
    }

    setExtraSelections(extraSelections);
}

void ScriptEditor::commentSelection()
{
    ScriptDocument* doc = dynamic_cast<ScriptDocument*>(document());
    if (doc) {
        QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        QList<QTextBlock> selectedBlocks = getSelectedBlocks(cursor);
        bool haveSelection = cursor.anchor() != cursor.position();
        if (!haveSelection) {
            selectedBlocks << cursor.block();
        }
        bool changeSelection = false;
        if (selectedBlocks.isEmpty()) {
            changeSelection = doc->assistant()->commentSelection(cursor);
        } else {
            changeSelection = doc->assistant()->commentLines(cursor, selectedBlocks);
        }
        if (haveSelection && changeSelection) {
            setTextCursor(cursor);
        }
        cursor.endEditBlock();
    }
}
