#include "scriptassistant.h"
#include "scripteditor.h"
#include <QTextCursor>
#include <QMetaMethod>

ScriptAssistant::ScriptAssistant(QTextDocument *parent) :
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
