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
    return new ScriptDocument(path, type);
}
