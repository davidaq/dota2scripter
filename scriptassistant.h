#ifndef SCRIPTASSISTANT_H
#define SCRIPTASSISTANT_H

#include <QSyntaxHighlighter>
#include <QMetaMethod>
#include <QIcon>
#include <QTextEdit>

class ScriptEditor;
class ScriptDocument;

class ScriptAssistant : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit ScriptAssistant(ScriptDocument *parent = 0);
    virtual bool autoIndent(QTextCursor& cursor) = 0;
    virtual void onExtraHighLight(QList<QTextEdit::ExtraSelection>& selection, QTextCursor cursor);

    void informInputFromEditor(int keyPressed, QTextCursor& cursor, ScriptEditor* editor);
    virtual QString lineCommentMark() = 0;
    virtual QString blockCommentStart() = 0;
    virtual QString blockCommentEnd() = 0;

    virtual QString title();
    virtual QIcon icon();
    virtual QStringList inputTip(QTextCursor cursor);

    bool commentSelection(QTextCursor& cursor);
    bool commentLines(QTextCursor& cursor, const QList<QTextBlock>& lines);
protected:
    void onInput(const QString& token, QObject *obj, const char* slot, bool wordBreak=true);
    virtual bool isWordBreak(const QChar& c) const;
    virtual bool isWordBreak(const QString& str, int pos) const;
signals:
    void exampleListenerSignal(QString token, QTextCursor* cursor, ScriptEditor* editor);
public slots:

private:
    struct InputListener {
        QString token;
        QObject* obj;
        QMetaMethod slot;
        bool wordBreak;
        InputListener(const QString& token_, QObject* obj_, const QMetaMethod& slot_, bool wordBreak_) :
            token(token_), obj(obj_), slot(slot_), wordBreak(wordBreak_) {
        }
    };
    QMap<QChar, QList<InputListener> > inputListeners;

};

#endif // SCRIPTASSISTANT_H
