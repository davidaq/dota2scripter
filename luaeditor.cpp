#include "luaeditor.h"
#include "ui_luaeditor.h"

LuaEditor::LuaEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LuaEditor)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

LuaEditor::~LuaEditor()
{
    delete ui;
}
