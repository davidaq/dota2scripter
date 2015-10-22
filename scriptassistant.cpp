#include "scriptassistant.h"

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
