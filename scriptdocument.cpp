#include "scriptdocument.h"
#include "scriptassistantlua.h"
#include <QPlainTextDocumentLayout>

ScriptDocument::ScriptDocument(const QString& path, DocumentType type) :
    QTextDocument(0),
    docPath(path),
    docType(type)
{
    setDocumentLayout(new QPlainTextDocumentLayout(this));

    setDefaultStyleSheet("font-family: Monaco, Monospace, Courier New, Courier !important; font-size: 13px");

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

QString ScriptDocument::title()
{
    if (docTitle.isEmpty()) {
        docTitle = docAssistant->title();
        if (docTitle.isEmpty()) {
            static int untitledCounter = 0;
            untitledCounter++;
            docTitle = tr("Untitled") + QString().sprintf(" %d", untitledCounter);
        }
    }
    return docTitle;
}

QIcon ScriptDocument::icon()
{
    return docAssistant->icon();
}

ScriptDocument::DocumentType ScriptDocument::type() const
{
    return docType;
}

ScriptAssistant* ScriptDocument::assistant() const
{
    return docAssistant;
}
