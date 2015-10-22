#ifndef SCRIPTEDITORLINENUMBERS_H
#define SCRIPTEDITORLINENUMBERS_H

#include <QWidget>

class ScriptEditor;

class ScriptEditorLineNumbers : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptEditorLineNumbers(ScriptEditor *parent = 0);

    int getPreferedWidth() const;
    void paintEvent(QPaintEvent *);
signals:

public slots:

private:
    ScriptEditor *editor;
};

#endif // SCRIPTEDITORLINENUMBERS_H
