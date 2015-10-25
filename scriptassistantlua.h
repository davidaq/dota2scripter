#ifndef SCRIPTASSISTANTLUA_H
#define SCRIPTASSISTANTLUA_H

#include "scriptassistant.h"

class ScriptAssistantLua : public ScriptAssistant
{
    Q_OBJECT
public:
    explicit ScriptAssistantLua(ScriptDocument *document = 0);
    bool autoIndent(QTextCursor& cursor);
    void onExtraHighLight(QList<QTextEdit::ExtraSelection>& selection, QTextCursor cursor);
    QString lineCommentMark();
    QString blockCommentStart();
    QString blockCommentEnd();
    QIcon icon();
    QStringList inputTip(QTextCursor cursor);
protected:
    void highlightBlock(const QString &text);

public slots:
    void checkInputTip(QString token, QTextCursor* cursorPtr, ScriptEditor* editor);
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
