#include "documentmanager.h"

DocumentManager::DocumentManager() :
    QObject(0)
{
}


DocumentManager* DocumentManager::manager()
{
    static DocumentManager manager;
    return &manager;
}

ScriptDocument* DocumentManager::open(const QString& path, ScriptDocument::DocumentType type)
{
    ScriptDocument* doc = new ScriptDocument(path, type);
    documentList.append(doc);
    return doc;
}

QList<ScriptDocument*> DocumentManager::list()
{
    return documentList;
}
