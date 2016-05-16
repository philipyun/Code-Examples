#include "HomeCtrl.hpp"


HomeCtrl::HomeCtrl()
    : col_{0}
{
}


void HomeCtrl::execute(EditorModel& model)
{
    col_ = model.cursorColumn();

    model.setCursorHome();
}


void HomeCtrl::undo(EditorModel& model)
{
    model.setCol(col_);
}