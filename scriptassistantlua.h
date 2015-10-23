#ifndef SCRIPTASSISTANTLUA_H
#define SCRIPTASSISTANTLUA_H

#include "scriptassistant.h"

class ScriptAssistantLua : public ScriptAssistant
{
    Q_OBJECT
public:
    explicit ScriptAssistantLua(QTextDocument *document = 0);
    bool autoIndent(QTextCursor& cursor);
protected:
    void highlightBlock(const QString &text);

public slots:
    void newlineAutoEndBlock(QString token, QTextCursor* cursorPtr, ScriptEditor* editor);
    void undoAutoUnindent(QString token, QTextCursor* cursorPtr, ScriptEditor* editor);
    void autoUnindent(QString token, QTextCursor* cursorPtr, ScriptEditor* editor);
    void autoUnindentString(QString token, QTextCursor* cursorPtr, ScriptEditor* editor);
private:
    QString indentBeforeAutoIndent, autoIndentToken;
    int autoIndentLen;
    int autoIndentBlockIndex;
};

#endif // SCRIPTASSISTANTLUA_H
