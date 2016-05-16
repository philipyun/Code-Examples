#include "UpCtrl.hpp"
#include "EditorException.hpp"


UpCtrl::UpCtrl()
    : col_{0}
{
}


void UpCtrl::execute(EditorModel& model)
{
    col_ = model.cursorColumn();

    if (model.cursorLine() > 1)
    {
        int currentCol = model.cursorColumn();
        int prevMaxCol = model.line(model.cursorLine() - 1).size();

        model.moveCursorUp();

        if (currentCol > prevMaxCol)
        {
            model.setCursorEnd();
        }
    }
    else if (model.cursorLine() == 1)
    {
        throw EditorException("Already at top");
    }
}


void UpCtrl::undo(EditorModel& model)
{
    model.moveCursorDown();
    model.setCol(col_);
}