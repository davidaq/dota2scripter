#ifndef SCRIPTASSISTANT_H
#define SCRIPTASSISTANT_H

#include <QSyntaxHighlighter>

class ScriptAssistant : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit ScriptAssistant(QTextDocument *parent = 0);

protected:
    virtual bool isWordBreak(const QChar& c) const;
    virtual bool isWordBreak(const QString& str, int pos) const;
signals:

public slots:

};

#endif // SCRIPTASSISTANT_H
