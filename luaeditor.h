#ifndef LUAEDITOR_H
#define LUAEDITOR_H

#include <QWidget>

namespace Ui {
class LuaEditor;
}

class LuaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit LuaEditor(QWidget *parent = 0);
    ~LuaEditor();

private:
    Ui::LuaEditor *ui;
};

#endif // LUAEDITOR_H
