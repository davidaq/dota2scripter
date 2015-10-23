#include "scriptdocument.h"
#include "scriptassistantlua.h"
#include <QPlainTextDocumentLayout>

ScriptDocument::ScriptDocument(const QString& path, DocumentType type) :
    QTextDocument(0),
    docPath(path),
    docType(type)
{
    setDocumentLayout(new QPlainTextDocumentLayout(this));
    switch (type) {
    case lua:
        docAssistant = new ScriptAssistantLua(this);
        break;
    default:
        break;
    }
}

const QString& ScriptDocument::path() const
{
    return docPath;
}

ScriptDocument::DocumentType ScriptDocument::type() const
{
    return docType;
}

ScriptAssistant* ScriptDocument::assistant() const
{
    return docAssistant;
}
