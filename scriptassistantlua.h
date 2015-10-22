#ifndef SCRIPTASSISTANTLUA_H
#define SCRIPTASSISTANTLUA_H

#include "scriptassistant.h"

class ScriptAssistantLua : public ScriptAssistant
{
    Q_OBJECT
public:
    explicit ScriptAssistantLua(QTextDocument *document = 0);
protected:
    void highlightBlock(const QString &text);
signals:

public slots:
private:
};

#endif // SCRIPTASSISTANTLUA_H
