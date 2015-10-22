#ifndef SCRIPTASSISTANTLUA_H
#define SCRIPTASSISTANTLUA_H

#include "scriptassistant.h"

class ScriptAssistantLua : public ScriptAssistant
{
    Q_OBJECT
public:
    explicit ScriptAssistantLua(QTextDocument *document = 0);
    bool autoIndent(QTextCursor &cursor);
    void update(int key, ScriptEditor& editor);
protected:
    void highlightBlock(const QString &text);
signals:

public slots:
private:
};

#endif // SCRIPTASSISTANTLUA_H
