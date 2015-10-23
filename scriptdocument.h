#ifndef SCRIPTDOCUMENT_H
#define SCRIPTDOCUMENT_H

#include <QTextDocument>
#include "scriptassistant.h"

class ScriptDocument : public QTextDocument
{
    Q_OBJECT
public:
    enum DocumentType {
        lua, hero
    };


    const QString& path() const;
    DocumentType type() const;
    ScriptAssistant* assistant() const;

protected:
    friend class DocumentManager;
    explicit ScriptDocument(const QString& path="", DocumentType type = lua);

signals:

public slots:

private:
    QString docPath;
    DocumentType docType;
    ScriptAssistant* docAssistant;
};

#endif // SCRIPTDOCUMENT_H
