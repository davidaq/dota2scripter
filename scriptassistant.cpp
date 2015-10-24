#include "scriptassistant.h"
#include "scripteditor.h"
#include "scriptdocument.h"
#include <QTextCursor>
#include <QMetaMethod>

ScriptAssistant::ScriptAssistant(ScriptDocument *parent) :
    QSyntaxHighlighter(parent)
{
}

bool ScriptAssistant::isWordBreak(const QChar& c) const
{
    char cc = c.toLatin1();
    if ((cc >= '0' && cc <= '9') || (cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z')) {
        return false;
    }
    if (cc == '_') {
        return true;
    }
    return true;
}

bool ScriptAssistant::isWordBreak(const QString& str, int pos) const
{
    if (pos < 0 || pos >= str.length())
        return true;
    return isWordBreak(str.at(pos));
}

void ScriptAssistant::informInputFromEditor(int keyPressed, QTextCursor &cursor, ScriptEditor* editor)
{
    const QString& text = cursor.block().text().trimmed();
    QChar key;
    if (keyPressed == Qt::Key_Return) {
        key = QChar('\r');
    } else if (text.isEmpty()) {
        key = QChar('\n');
    } else {
        key = text.at(text.length() - 1);
    }
    foreach (const InputListener& listener, inputListeners[QChar(0)]) {
        listener.slot.invoke(listener.obj, Qt::DirectConnection, Q_ARG(QString, QString(key)), Q_ARG(QTextCursor*, &cursor), Q_ARG(ScriptEditor*, editor));
    }
    if (inputListeners.contains(key)) {
        foreach (const InputListener& listener, inputListeners[key]) {
            if ((listener.token.length() <= 1 || text.endsWith(listener.token))
                    && (!listener.wordBreak || isWordBreak(text, text.length() - listener.token.length() - 1))) {
                listener.slot.invoke(listener.obj, Qt::DirectConnection, Q_ARG(QString, listener.token), Q_ARG(QTextCursor*, &cursor), Q_ARG(ScriptEditor*, editor));
            }
        }
    }
}

void ScriptAssistant::onInput(const QString& token, QObject* obj, const char* slot, bool wordBreak)
{
    QChar key;
    if (token.isEmpty()) {
        key = QChar(0);
    } else {
        key = token.at(token.length() - 1);
    }
    const QMetaObject* metaObject = obj->metaObject();
    int index = metaObject->indexOfSlot(&slot[1]);
    const QMetaMethod& method = metaObject->method(index);
    inputListeners[key].append(InputListener(token, obj, method, wordBreak));
}

QString ScriptAssistant::title()
{
    return ((ScriptDocument*)document())->path();
}

QIcon ScriptAssistant::icon()
{
    return QIcon(":/icons/toolbar/question.png");
}

bool ScriptAssistant::commentLines(QTextCursor& cursor, const QList<QTextBlock> &lines)
{
    QString mark = lineCommentMark();
    if (mark.isEmpty()) {
        return commentSelection(cursor);
    }
    bool uncomment = true;
    foreach (const QTextBlock& block, lines) {
        if (!block.text().trimmed().startsWith(mark)) {
            uncomment = false;
            break;
        }
    }
    foreach (const QTextBlock& block, lines) {
        cursor.setPosition(block.position());
        if (uncomment) {
            if (block.text().at(0) == ' ') {
                cursor.movePosition(QTextCursor::NextWord);
            }
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, mark.length());
            cursor.removeSelectedText();
        } else {
            cursor.insertText(mark);
        }
    }
    cursor.setPosition(lines.first().position());
    cursor.setPosition(lines.last().position(), QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    return true;
}

bool ScriptAssistant::commentSelection(QTextCursor &cursor)
{
    QString prefix = blockCommentStart();
    QString suffix = blockCommentEnd();
    if (prefix.isEmpty() || suffix.isEmpty()) {
        return false;
    }
    const QString& selText = cursor.selectedText();
    const QString& blockText = cursor.block().text();
    const QString& before = blockText.mid(cursor.selectionStart() - cursor.block().position() - prefix.length(), prefix.length());
    const QString& after = blockText.mid(cursor.selectionEnd() - cursor.block().position(), suffix.length());
    if ((selText.startsWith(prefix) || before == prefix) && (selText.endsWith(suffix) || after == suffix)) {
        int start = cursor.selectionStart();
        if (before == prefix) {
            start -= prefix.length();
        }
        int end = cursor.selectionEnd();
        if (after == suffix) {
            end += suffix.length();
        }
        cursor.setPosition(end);
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, suffix.length());
        cursor.removeSelectedText();
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, prefix.length());
        cursor.removeSelectedText();
    } else {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        cursor.setPosition(end);
        cursor.insertText(suffix);
        cursor.setPosition(start);
        cursor.insertText(prefix);
        cursor.setPosition(end);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, prefix.length() + suffix.length());
        cursor.setPosition(start, QTextCursor::KeepAnchor);
        return true;
    }
    return false;
}
