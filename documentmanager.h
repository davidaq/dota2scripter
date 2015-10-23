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

    int count() const;
    ScriptDocument* at(int index) const;
signals:
    void listUpdated();
public slots:

protected:
    explicit DocumentManager();

    ScriptDocument* addDocument(ScriptDocument* document);

private:

    QVector<ScriptDocument*> documentList;
};

#endif // DOCUMENTMANAGER_H
