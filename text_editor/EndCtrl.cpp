#include "EndCtrl.hpp"

EndCtrl::EndCtrl()
    : col_{0}
{
}


void EndCtrl::execute(EditorModel& model)
{
    col_ = model.cursorColumn();
    
    model.setCursorEnd();
}


void EndCtrl::undo(EditorModel& model)
{
    model.setCol(col_);
}