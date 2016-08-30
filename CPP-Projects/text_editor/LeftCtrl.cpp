#include "LeftCtrl.hpp"
#include "RightCtrl.hpp"
#include "EditorException.hpp"


void LeftCtrl::execute(EditorModel& model)
{
    if (model.cursorColumn() > 1)
    {
        model.moveCursorLeft();
    }
    else if (model.cursorColumn() == 1)
    {
        if (model.cursorLine() != 1)
        {
            model.moveCursorUp();
            model.setCursorEnd();
        }
        else
        {
            throw EditorException("Already at beginning!");
        }
    }
}


void LeftCtrl::undo(EditorModel& model)
{
    RightCtrl right;
    right.execute(model);
}