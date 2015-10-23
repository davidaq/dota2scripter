#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "global.h"
#include <QPlainTextEdit>

class ScriptEditorLineNumbers;

class ScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget *parent = 0);
protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
signals:
    void onFocus(ScriptEditor*);
public slots:
    void show();
    void commentSelection();

private slots:
    void updateLineNumbers(const QRect&, int dy);
    void linesNumberChanged(int);
    void highlightCurrentLine();

private:
    friend class ScriptEditorLineNumbers;
    ScriptEditorLineNumbers *lineNumbers;
};

#endif // SCRIPTEDITOR_H
