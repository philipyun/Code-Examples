#include "LeftCtrl.hpp"
#include "RightCtrl.hpp"
#include "EditorException.hpp"


void RightCtrl::execute(EditorModel& model)
{
    int max = model.line(model.cursorLine()).size();

    if (model.cursorColumn() <= max)
    {
        model.moveCursorRight();
    }
    else if (model.cursorColumn() == max + 1)
    {
        if (model.cursorLine() != model.lineCount())
        {    
            model.moveCursorDown();
            model.setCursorHome(); 
        }
        else
        {
            throw EditorException("Already at end");
        }
    }
}


void RightCtrl::undo(EditorModel& model)
{
    LeftCtrl left;
    left.execute(model);
}