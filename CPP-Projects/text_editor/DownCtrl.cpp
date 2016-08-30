#include "DownCtrl.hpp"
#include "EditorException.hpp"

DownCtrl::DownCtrl()
    : col_{0}
{
}


void DownCtrl::execute(EditorModel& model)
{
    col_ = model.cursorColumn();

    int currentCol = model.cursorColumn();
    int nextMaxCol = model.line(model.cursorLine() + 1).size();

    if (model.cursorLine() < model.lineCount())
    {
        model.moveCursorDown();

        if (currentCol > nextMaxCol)
        {
            model.setCursorEnd();
        }
    }
    else
    {
        throw EditorException("Already at bottom");
    }
}


void DownCtrl::undo(EditorModel& model)
{
    model.moveCursorUp();
    model.setCol(col_);
}