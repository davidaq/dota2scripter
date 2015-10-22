#ifndef TOKENFINDER_H
#define TOKENFINDER_H

#include "global.h"
#include <QStringList>
#include <QVariant>

class TokenFinder
{
public:
    TokenFinder();

    void addTokens(const QStringList& tokens, const QVariant& tag=QVariant());
    void addToken(const QString& token, const QVariant& tag=QVariant());
    void setText(const QString& text, int pos=0);
    void skip(int len);
    void rewind();

    bool next();
    int position() const;
    const QString& token() const;
    const QVariant& tag() const;
private:
    typedef QPair<QString, QVariant > Candidate;
    QMap<QChar, QList<Candidate> > tokens;
    QString text;
    QString matchedText;
    QVariant matchedTag;
    int cursor, matchedPos;
};

#endif // TOKENFINDER_H
