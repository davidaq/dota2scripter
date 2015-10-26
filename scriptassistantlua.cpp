#include "scriptassistantlua.h"
#include "tokenfinder.h"
#include "scripteditor.h"
#include "scriptdocument.h"
#include <QResource>

static bool tokensSetup = true;
static TokenFinder luaTokens;
static QSet<QString> indentKeywords;
static QSet<QString> unindentKeywords;
static QList<ScriptAssistant::Tip> gapi;
static QList<ScriptAssistant::Tip> mapi;

const static int KEYWORD    = 1;
const static int COMMENT    = 2;
const static int COMMENT_M  = 3;
const static int STRING     = 4;
const static int STRING_M   = 5;
const static int OPERATOR   = 6;
const static int GLOBALS    = 7;
const static int LITERAL    = 8;
const static int BRACKET_O  = 9;
const static int BRACKET_C  = 10;

const static int COMMENT_MASK = 1 << 28;
const static int STRING_MASK  = 1 << 29;

class LuaBlockData : public QTextBlockUserData
{
public:
    bool indentNextLine;
    QString closer;
    QList<QPair<int,bool> > brackets;
    QString prepared;

    LuaBlockData() :
        indentNextLine(false),
        closer("")
    {
    }
};

ScriptAssistantLua::ScriptAssistantLua(ScriptDocument *document) :
    ScriptAssistant(document)
{
    if (tokensSetup) {
        tokensSetup = false;
        luaTokens.addTokens(QStringList()
                            << "and"       << "break"     << "do"        << "elseif"    << "else"
                            << "end"       << "for"       << "function"  << "if"
                            << "in"        << "local"     << "not"       << "or"
                            << "repeat"    << "return"    << "then"      << "until"
                            << "while"
                            , KEYWORD);
        luaTokens.addTokens(QStringList()
                            << "true"      << "false"     << "nil"
                            , LITERAL);
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
                            << "=" << "." << ":" << "#"
                            , OPERATOR);
        luaTokens.addTokens(QStringList()
                            << "(" << "{"
                            , BRACKET_O);
        luaTokens.addTokens(QStringList()
                            << ")" << "}"
                            , BRACKET_C);
        luaTokens.addTokens(QStringList()
                            << "getfenv"    << "setfenv"    << "pcall"          << "rawequal"       << "gcinfo"
                            << "os"         << "debug"      << "collectgarbage" << "dofile"         << "_G"
                            << "print"      << "require"    << "unpack"         << "io"             << "file"
                            << "ipairs"     << "coroutine"  << "assert"         << "error"          << "next"
                            << "pairs"      << "type"       << "tostring"       << "tonumber"       << "getmetatable"
                            << "string"     << "table"      << "math"           << "setmetatable"   << "rawset"
                            << "self"       << "class"      << "select"
                            , GLOBALS);
        indentKeywords      << "do"     << "elseif"    << "else"    << "then"
                            << "for"    << "function"  << "if"      << "repeat"     << "while";
        unindentKeywords    << "do"     << "elseif"    << "else"    << "then"
                            << "end"    << "until";
        QResource res(":/data/lua_api.txt");
        QByteArray resBytes((const char*)res.data(), res.size());
        if (res.isCompressed()) {
            resBytes = qUncompress(resBytes);
        }
        QString luaApi = resBytes;
        qDebug() << luaApi;
        QStringList part = luaApi.split("!!!");
        QString input;
        foreach(const QString& line, part[0].split("\n")) {
            if (line.isEmpty())
                continue;
            if (input.isEmpty()) {
                input = line;
            } else {
                gapi.append(Tip(input, line));
                input = "";
            }
        }
        foreach(const QString& line, part[1].split("\n")) {
            if (line.isEmpty())
                continue;
            if (input.isEmpty()) {
                input = line;
            } else {
                mapi.append(Tip(input, line));
                input = "";
            }
        }
    }
    onInput("end", this, SLOT(autoUnindent(QString,QTextCursor*,ScriptEditor*)));
    onInput("until", this, SLOT(autoUnindent(QString,QTextCursor*,ScriptEditor*)));
    onInput("else", this, SLOT(autoUnindent(QString,QTextCursor*,ScriptEditor*)));
    onInput("elseif", this, SLOT(autoUnindent(QString,QTextCursor*,ScriptEditor*)));
    onInput("then", this, SLOT(autoUnindent(QString,QTextCursor*,ScriptEditor*)));
    onInput("]", this, SLOT(autoUnindentString(QString,QTextCursor*,ScriptEditor*)));
    onInput("", this, SLOT(undoAutoUnindent(QString,QTextCursor*,ScriptEditor*)));
    onInput("", this, SLOT(checkInputTip(QString,QTextCursor*,ScriptEditor*)));
    onInput("\r", this, SLOT(newlineAutoEndBlock(QString,QTextCursor*,ScriptEditor*)));

    autoIndentBlockIndex = -1;
}

QIcon ScriptAssistantLua::icon()
{
    return QIcon(":/icons/toolbar/script.png");
}

QList<ScriptAssistant::Tip> ScriptAssistantLua::inputTip(QTextCursor cursor)
{
    QString word = wordBeforeCursor(cursor);
    if (!cursor.atBlockStart()) {
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        if (cursor.selectedText() == ":") {
            return mapi;
        }
    }
    return gapi;
}

QString ScriptAssistantLua::lineCommentMark()
{
    return "--";
}

QString ScriptAssistantLua::blockCommentStart()
{
    return "--[[";
}

QString ScriptAssistantLua::blockCommentEnd()
{
    return "--]]";
}

void ScriptAssistantLua::highlightBlock(const QString &text)
{
    int find, len, N, t;
    QString sub;

    setCurrentBlockState(-1);
    LuaBlockData* blockData = dynamic_cast<LuaBlockData*>(currentBlockUserData());
    if (!blockData) {
        blockData = new LuaBlockData();
        setCurrentBlockUserData(blockData);
    }

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
    int needIndent = 0;
    blockData->closer = "";
    blockData->brackets.clear();
    blockData->brackets.append(QPair<int,bool>(-1, false));
    while (luaTokens.next()) {
        switch (luaTokens.tag().toInt()) {
        case BRACKET_O:
            setFormat(luaTokens.position(), luaTokens.token().length(), 0x7941ba);
            blockData->brackets.append(QPair<int,bool>(luaTokens.position(), true));
            if (luaTokens.token() == "(") {
                blockData->closer = ")";
            } else if (luaTokens.token() == "{") {
                blockData->closer = "}";
            }
            break;
        case BRACKET_C:
            setFormat(luaTokens.position(), luaTokens.token().length(), 0x7941ba);
            blockData->brackets.append(QPair<int,bool>(luaTokens.position(), false));
            break;
        case LITERAL:
            if (isWordBreak(text, luaTokens.position() - 1)
                    && isWordBreak(text, luaTokens.position() + luaTokens.token().length())) {
                setFormat(luaTokens.position(), luaTokens.token().length(), QColor(0xa99951));
            }
            break;
        case KEYWORD:
            if (isWordBreak(text, luaTokens.position() - 1)
                    && isWordBreak(text, luaTokens.position() + luaTokens.token().length())) {
                const QString& keyword = luaTokens.token();
                setFormat(luaTokens.position(), keyword.length(), QColor(0x46a2da));
                if (needIndent > 0 && unindentKeywords.contains(keyword)) {
                    needIndent--;
                }
                if (indentKeywords.contains(keyword)) {
                    needIndent++;
                }
                if (keyword == "then" || keyword == "function" || keyword == "do") {
                    blockData->closer = "end";
                } else if (keyword == "repeat") {
                    blockData->closer = "until";
                } else if (keyword == "end" || keyword == "until") {
                    blockData->closer = "";
                }
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
                needIndent++;
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
                needIndent++;
            }
            setFormat(luaTokens.position(), len, 0x5caa15);
            luaTokens.skip(len - luaTokens.token().length());
            break;
        default:
            break;
        }
    }
    blockData->indentNextLine = needIndent > 0;
}

static int preferedIndent(QTextCursor &cursor, bool autoIndent=true) {
    const QTextBlock& prev = cursor.block().previous();
    if (!prev.isValid()) {
        return 0;
    }
    QString prevText = prev.text();
    int sz = prevText.length();
    while (sz > 0 && prevText.at(sz - 1) == ' ') {
        sz--;
    }
    prevText = prevText.left(sz);
    int space = 0;
    for (; space < prevText.length() && prevText.at(space) == ' '; space++) {
    }
    if (autoIndent) {
        LuaBlockData* blockData = dynamic_cast<LuaBlockData*>(prev.userData());
        if (blockData) {
            if (blockData->indentNextLine) {
                space += 4;
            }
        }
    }
    return space;
}

bool ScriptAssistantLua::autoIndent(QTextCursor &cursor)
{
    int space = preferedIndent(cursor);
    if (!space)
        return false;
    cursor.insertText(QString(" ").repeated(space));
    return true;
}

void ScriptAssistantLua::onExtraHighLight(QList<QTextEdit::ExtraSelection>& selection, QTextCursor cursor)
{
    if (cursor.atBlockStart())
        return;
    QChar c = cursor.block().text().at(cursor.positionInBlock() - 1);
    if (c == '}' || c == ')' || c == '{' || c == '(') {
        QTextEdit::ExtraSelection beforeCursor;
        beforeCursor.format.setBackground(QColor(150, 220, 150));
        beforeCursor.cursor = cursor;
        beforeCursor.cursor.clearSelection();
        beforeCursor.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);

        QTextBlock matchBlock = cursor.block();
        LuaBlockData* blockData = dynamic_cast<LuaBlockData*>(matchBlock.userData());
        int matchPos = -1;
        if (blockData) {
            int startPos = cursor.positionInBlock() - 1;
            QList<QPair<int,bool> >::iterator iter = blockData->brackets.begin() + 1;
            while (iter != blockData->brackets.end()) {
                if (iter->first == startPos)
                    break;
                iter++;
            }
            if (iter == blockData->brackets.end()) {
                return;
            } else {
                bool isClose = c == '}' || c == ')';
                int level = isClose ? -1 : 1;
                QList<QPair<int,bool> >::iterator end = isClose ? blockData->brackets.begin() : blockData->brackets.end();
                while (level != 0) {
                    if (isClose) {
                        iter--;
                    } else {
                        iter++;
                    }
                    if (iter == end) {
                        if (isClose) {
                            matchBlock = matchBlock.previous();
                        } else {
                            matchBlock = matchBlock.next();
                        }
                        if (!matchBlock.isValid()) {
                            break;
                        }
                        blockData = dynamic_cast<LuaBlockData*>(matchBlock.userData());
                        if (!blockData) {
                            break;
                        }
                        if (isClose) {
                            iter = blockData->brackets.end();
                            end = blockData->brackets.begin();
                        } else {
                            iter = blockData->brackets.begin();
                            end = blockData->brackets.end();
                        }
                    } else {
                        if (iter->second) {
                            level++;
                        } else {
                            level--;
                        }
                    }
                }
                if (level == 0) {
                    matchPos = iter->first + matchBlock.position();
                }
            }
        }
        if (matchPos > -1) {
            if (matchBlock.isVisible()) {
                QChar mc = matchBlock.text().at(matchPos - matchBlock.position());
                QTextEdit::ExtraSelection match;
                bool badMatch = false;
                if (c == '(') {
                    if (mc != ')') {
                        badMatch = true;
                    }
                } else if (c == ')') {
                    if (mc != '(') {
                        badMatch = true;
                    }
                } else if (c == '}') {
                    if (mc != '{') {
                        badMatch = true;
                    }
                } else if (c == '{') {
                    if (mc != '}') {
                        badMatch = true;
                    }
                }
                if (badMatch) {
                    beforeCursor.format.setBackground(QColor(250, 150, 150));
                    match.format.setBackground(QColor(250, 150, 150));
                } else {
                    match.format.setBackground(QColor(150, 220, 150));
                }
                match.cursor = cursor;
                match.cursor.clearSelection();
                match.cursor.setPosition(matchPos);
                match.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                selection.append(match);
            }
        } else if (c == '{' || c == '(') {
            beforeCursor.format.setBackground(QColor(200, 220, 200));
        } else {
            beforeCursor.format.setBackground(QColor(250, 150, 150));
        }
        selection.append(beforeCursor);
    }
}

void ScriptAssistantLua::checkInputTip(QString token, QTextCursor* cursorPtr, ScriptEditor* editor)
{
    if (token == ":" || wordBeforeCursor(*cursorPtr).length() > 2) {
        editor->onReceiveInputTip(this);
    }
}

void ScriptAssistantLua::newlineAutoEndBlock(QString /*token*/, QTextCursor* cursorPtr, ScriptEditor* editor)
{
    QTextCursor &cursor = *cursorPtr;
    const QTextBlock& prev = cursor.block().previous();
    if (!prev.isValid()) {
        return;
    }
    LuaBlockData* blockData = dynamic_cast<LuaBlockData*>(prev.userData());
    if (blockData && !blockData->closer.isEmpty()) {
        int pos = cursor.position();
        int space = preferedIndent(cursor, false);
        cursor.insertBlock();
        cursor.insertText(QString(" ").repeated(space) + blockData->closer);
        cursor.setPosition(pos);
        editor->setTextCursor(cursor);
    }
}

void ScriptAssistantLua::undoAutoUnindent(QString /*token*/, QTextCursor* cursorPtr, ScriptEditor* /*editor*/)
{
    QTextCursor &cursor = *cursorPtr;
    if (autoIndentBlockIndex > -1 && cursor.atBlockEnd()) {
        if (cursor.block().blockNumber() == autoIndentBlockIndex) {
            QString text = cursor.block().text().trimmed();
            text = text.replace(autoIndentToken, "");
            bool undo = false;
            for (int i = 0; i < text.length(); i++) {
                if (!isWordBreak(text, i)) {
                    undo = true;
                    break;
                }
            }
            if (undo) {
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, autoIndentLen);
                cursor.insertText(indentBeforeAutoIndent);
            }
        }
        autoIndentBlockIndex = -1;
    }
}

void ScriptAssistantLua::autoUnindent(QString token, QTextCursor* cursorPtr, ScriptEditor* /*editor*/)
{
    QTextCursor &cursor = *cursorPtr;
    const QTextBlock& block = cursor.block();
    const QString& blockText = block.text();
    if (!isWordBreak(blockText, cursor.positionInBlock() - 1) && blockText.trimmed() == token) {
        int space = preferedIndent(cursor) - 4;
        cursor.movePosition(QTextCursor::StartOfBlock);
        for (int i = 0; i < blockText.length() && blockText.at(i) == ' '; i++) {
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
        if (!token.endsWith("]")) {
            indentBeforeAutoIndent = cursor.selectedText();
            autoIndentBlockIndex = block.blockNumber();
            autoIndentLen = space;
            autoIndentToken = token;
        }
        cursor.insertText(QString(" ").repeated(space));
    }
}

void ScriptAssistantLua::autoUnindentString(QString /*token*/, QTextCursor* cursorPtr, ScriptEditor* editor)
{
    QTextCursor &cursor = *cursorPtr;
    const QTextBlock& block = cursor.block();
    QString blockText = block.text().trimmed();
    QString checkText = blockText;
    checkText = checkText.replace('=', "");
    if (checkText == "--]]" || checkText == "]]") {
        autoUnindent(blockText, cursorPtr, editor);
    }
}
