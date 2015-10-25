#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "global.h"
#include <QPlainTextEdit>

class QListWidget;
class ScriptEditorLineNumbers;
class ScriptAssistant;

class ScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget *parent = 0);
    void setDocument(QTextDocument *document);
    void onReceiveInputTip(ScriptAssistant* assistant);
protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void insertFromMimeData(const QMimeData *source);
signals:
    void onFocus(ScriptEditor*);
public slots:
    void show();
    void commentSelection();

private slots:
    void updateLineNumbers(const QRect&, int dy);
    void linesNumberChanged(int);
    void highlightCurrentLine();
    void adjustInputTipWidget();

private:
    friend class ScriptEditorLineNumbers;
    ScriptEditorLineNumbers *lineNumbers;
    QListWidget* inputTipWidget;
};

#endif // SCRIPTEDITOR_H
