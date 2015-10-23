#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include "scriptdocument.h"

class DocumentManager : public QObject
{
    Q_OBJECT
public:
    static DocumentManager* manager();

    ScriptDocument* open(const QString& path, ScriptDocument::DocumentType type);

    QList<ScriptDocument*> list();
signals:
    void listUpdated();
public slots:

private:
    explicit DocumentManager();

    QList<ScriptDocument*> documentList;
};

#endif // DOCUMENTMANAGER_H
