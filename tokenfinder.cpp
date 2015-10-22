#include "tokenfinder.h"

TokenFinder::TokenFinder()
{
    rewind();
}

void TokenFinder::addTokens(const QStringList& tokens, const QVariant& tag)
{
    foreach (const QString& token, tokens) {
        addToken(token, tag);
    }
}

void TokenFinder::addToken(const QString& token, const QVariant& tag)
{
    tokens[token.at(0)].append(Candidate(token.mid(1), tag));
}

void TokenFinder::setText(const QString& text, int pos)
{
    rewind();
    this->text = text;
    cursor = pos;
}

void TokenFinder::skip(int len)
{
    cursor += len;
}

void TokenFinder::rewind()
{
    cursor = 0;
    matchedPos = 0;
    matchedText = "";
}

bool TokenFinder::next()
{
    while (cursor < text.length()) {
        const QChar& c = text.at(cursor);
        if (tokens.contains(c)) {
            foreach (const Candidate& candidate, tokens[c]) {
                if (candidate.first.length() == 0 ||
                        text.mid(cursor + 1, candidate.first.length()) == candidate.first) {
                    matchedPos = cursor;
                    matchedText = c + candidate.first;
                    matchedTag = candidate.second;
                    cursor += candidate.first.length() + 1;
                    return true;
                }
            }
        }
        cursor++;
    }
    return false;
}

int TokenFinder::position() const
{
    return matchedPos;
}

const QString& TokenFinder::token() const
{
    return matchedText;
}

const QVariant& TokenFinder::tag() const
{
    return matchedTag;
}
