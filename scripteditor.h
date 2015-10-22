#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include <QPlainTextEdit>

class ScriptEditorLineNumbers;

class ScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *);
signals:

public slots:
    void show();

private slots:
    void updateLineNumbers(const QRect&, int dy);
    void linesNumberChanged(int);
    void highlightCurrentLine();

private:
    friend class ScriptEditorLineNumbers;
    ScriptEditorLineNumbers *lineNumbers;
};

#endif // SCRIPTEDITOR_H