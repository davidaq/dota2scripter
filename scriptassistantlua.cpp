#include "scriptassistantlua.h"
#include "tokenfinder.h"

static bool tokensSetup = true;
static TokenFinder luaTokens;

const static int KEYWORD    = 1;
const static int COMMENT    = 2;
const static int COMMENT_M  = 3;
const static int STRING     = 4;
const static int STRING_M   = 5;
const static int OPERATOR   = 6;
const static int GLOBALS    = 7;

const static int COMMENT_MASK = 1 << 28;
const static int STRING_MASK  = 1 << 29;

ScriptAssistantLua::ScriptAssistantLua(QTextDocument *document) :
    ScriptAssistant(document)
{
    if (tokensSetup) {
        tokensSetup = false;
        luaTokens.addTokens(QStringList()
                            << "and"       << "break"     << "do"        << "elseif"    << "else"
                            << "end"       << "false"     << "for"       << "function"  << "if"
                            << "in"        << "local"     << "nil"       << "not"       << "or"
                            << "repeat"    << "return"    << "then"      << "true"      << "until"
                            << "while"
                            , KEYWORD);
        luaTokens.addTokens(QStringList()
                            << "--[[" << "--[="
                            , COMMENT_M);
        luaTokens.addTokens(QStringList()
                            << "--"
                            , COMMENT);
        luaTokens.addTokens(QStringList()
                            << "\"" << "'"
                            , STRING);
        luaTokens.addTokens(QStringList()
                            << "[[" << "[="
                            , STRING_M);
        luaTokens.addTokens(QStringList()
                            << "+" << "-" << "*" << "/" << "%" << "^"
                            << "|" << "&" << "?" << "~" << ">" << "<"
                            << "=" << "(" << ")" << "{" << "}" << "."
                            << ":" << "#"
                            , OPERATOR);
        luaTokens.addTokens(QStringList()
                            << "getfenv"    << "setfenv"    << "pcall"          << "rawequal"       << "gcinfo"
                            << "os"         << "debug"      << "collectgarbage" << "dofile"         << "_G"
                            << "print"      << "require"    << "unpack"         << "io"             << "file"
                            << "ipairs"     << "coroutine"  << "assert"         << "error"          << "next"
                            << "pairs"      << "type"       << "tostring"       << "tonumber"       << "getmetatable"
                            << "string"     << "table"      << "math"           << "setmetatable"   << "rawset"
                            << "self"       << "class"
                            , GLOBALS);
    }
}

void ScriptAssistantLua::highlightBlock(const QString &text)
{
    int find, len, N, t;
    QString sub;

    setCurrentBlockState(-1);

    luaTokens.setText(text);
    int prevState = previousBlockState();
    if (prevState != -1) {
        if (prevState & COMMENT_MASK) {
            N = prevState - COMMENT_MASK;
            sub = "--]";
            for (int i = 0; i < N; i++)
                sub += "=";
            sub += "]";
            find = text.indexOf(sub);
            if (find > -1) {
                len = find + sub.length();
            } else {
                len = text.length();
                setCurrentBlockState(COMMENT_MASK | N);
            }
            setFormat(luaTokens.position(), len, 0x999999);
            luaTokens.skip(len);
        } else if (prevState & STRING_MASK) {
            N = prevState - STRING_MASK;
            sub = "]";
            for (int i = 0; i < N; i++)
                sub += "=";
            sub += "]";
            find = text.indexOf(sub);
            if (find > -1) {
                len = find + sub.length();
            } else {
                len = text.length();
                setCurrentBlockState(STRING_MASK | N);
            }
            setFormat(luaTokens.position(), len, 0x5caa15);
            luaTokens.skip(len);
        }
    }
    while (luaTokens.next()) {
        switch (luaTokens.tag().toInt()) {
        case KEYWORD:
            if (isWordBreak(text, luaTokens.position() - 1)
                    && isWordBreak(text, luaTokens.position() + luaTokens.token().length())) {
                setFormat(luaTokens.position(), luaTokens.token().length(), QColor(0x46a2da));
            }
            break;
        case GLOBALS:
            if (isWordBreak(text, luaTokens.position() - 1)
                    && isWordBreak(text, luaTokens.position() + luaTokens.token().length())) {
                setFormat(luaTokens.position(), luaTokens.token().length(), QColor(0x436199));
            }
            break;
        case OPERATOR:
            setFormat(luaTokens.position(), luaTokens.token().length(), QColor(0x3783d8));
            break;
        case COMMENT:
            setFormat(luaTokens.position(), text.length(), QColor(0x999999));
            luaTokens.skip(text.length());
            break;
        case COMMENT_M:
            if (luaTokens.token().at(3) == '=') {
                N = 1;
                for (t = luaTokens.position() + 4; t < text.length() && text.at(t) == '='; t++) {
                    N++;
                }
                if (t >= text.length() || text.at(t) != '[') {
                    setFormat(luaTokens.position(), text.length(), QColor(0x999999));
                    luaTokens.skip(text.length());
                    break;
                }
            } else {
                N = 0;
            }
            find = luaTokens.position() + 4 + N;
            sub = "--]";
            for (int i = 0; i < N; i++)
                sub += "=";
            sub += "]";
            find = text.indexOf(sub, find);
            if (find > -1) {
                len = find + sub.length() - luaTokens.position();
            } else {
                len = text.length() - luaTokens.position();
                setCurrentBlockState(COMMENT_MASK | N);
            }
            setFormat(luaTokens.position(), len, 0x999999);
            luaTokens.skip(len - luaTokens.token().length());
            break;
        case STRING:
            find = luaTokens.position();
            while(true) {
                find = text.indexOf(luaTokens.token().at(0), find + 1);
                if (find > -1) {
                    int i = 1;
                    bool escape = false;
                    for (; i < text.length() && text.at(find - i) == '\\'; i++) {
                        escape = !escape;
                    }
                    if (escape) {
                        continue;
                    }
                }
                break;
            }
            if (find > -1) {
                len = find - luaTokens.position() + 1;
            } else {
                len = text.length() - luaTokens.position();
            }
            setFormat(luaTokens.position(), len, QColor(0x5caa15));
            luaTokens.skip(len - luaTokens.token().length());
            break;
        case STRING_M:
            if (luaTokens.token().at(1) == '=') {
                N = 1;
                for (t = luaTokens.position() + 2; t < text.length() && text.at(t) == '='; t++) {
                    N++;
                }
                if (t >= text.length() || text.at(t) != '[') {
                    break;
                }
            } else {
                N = 0;
            }
            find = luaTokens.position() + 2 + N;
            sub = "]";
            for (int i = 0; i < N; i++)
                sub += "=";
            sub += "]";
            find = text.indexOf(sub, find);
            if (find > -1) {
                len = find + sub.length() - luaTokens.position();
            } else {
                len = text.length() - luaTokens.position();
                setCurrentBlockState(STRING_MASK | N);
            }
            setFormat(luaTokens.position(), len, 0x5caa15);
            luaTokens.skip(len - luaTokens.token().length());
            break;
        default:
            break;
        }
    }
}
