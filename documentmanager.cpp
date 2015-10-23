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
    return addDocument(new ScriptDocument(path, type));
}

ScriptDocument* DocumentManager::addDocument(ScriptDocument* document)
{
    if (!document)
        return 0;
    documentList.append(document);
    emit listUpdated();
    return document;
}

int DocumentManager::count() const
{
    return documentList.size();
}

ScriptDocument* DocumentManager::at(int index) const
{
    if (index < 0 || index >= documentList.size())
        return 0;
    return documentList[index];
}
